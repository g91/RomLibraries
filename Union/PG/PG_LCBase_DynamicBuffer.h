#ifndef LOG_CENTER_BASE_DYNAMIC_BUFFER_H
#define LOG_CENTER_BASE_DYNAMIC_BUFFER_H

//////////////////////////////////////////////////////////////////////////
// Last Update - by slime 2011/01/07
//////////////////////////////////////////////////////////////////////////

#include "../../../ShareLib/sm_debug.h"

#define LC_BASE_DYNAMIC_BUFFER_ZERO_INIT 1          // 初始化時清乾淨記憶體區塊(額外效能耗損)
#define LC_BASE_DYNAMIC_BUFFER_USE_BOOST_POOL 0     // 是否使用 boost::pool 取得記憶體區塊(減少連續的 new - delete 造成記憶體破碎) // todo:

#if _DEBUG
#define LC_BASE_DYNAMIC_BUFFER_CHECK_PUSH_POP 1     // 是否在 push / pop 檢查輸入資訊.
#define LC_BASE_DYNAMIC_BUFFER_PRINT_SIZE 1         // 是否印出 size (for debug)
#else
#define LC_BASE_DYNAMIC_BUFFER_CHECK_PUSH_POP 0
#define LC_BASE_DYNAMIC_BUFFER_PRINT_SIZE 1
#endif

#if LC_BASE_DYNAMIC_BUFFER_USE_BOOST_POOL
//typedef boost::pool<byte> bytePool;
#endif

// 全動態型封包標頭定義(對齊位置用)
struct s_PG_LCBase_DynamicBuffer_Header
{
    int     commandID;      // 封包指令編號.
    int     commandType;    // 封包指令種類.
    int     sizeBuffer;     // 暫存區大小.
    int     sizeCanPop;     // 可用大小.
    int     sizePushed;     // 已用大小.
    int     itemCount;      // 內含元素數量.
    byte*   dataPtrNow;     // 目前資料指標位置.
    byte*   dataPtrTop;     // 最初資料指標位置.(相當於 array top)
};

// 全動態型封包(會增加些微額外的計算效能負擔，請謹慎使用)
// 目前已確認動作正常 typename : byte
// (理論上, 占用記憶體相同大小的 char 也能正常運作)

template <typename T1>
class PG_LCBase_DynamicBuffer
{
public:
    PG_LCBase_DynamicBuffer(int nDataAllocSize)
        :   m_buf(NULL)
    {
#if LC_BASE_DYNAMIC_BUFFER_USE_BOOST_POOL
#else
        m_buf = new T1[sizeof(s_PG_LCBase_DynamicBuffer_Header)+nDataAllocSize];
#endif
#if LC_BASE_DYNAMIC_BUFFER_ZERO_INIT
        memset(m_buf, 0, sizeof(s_PG_LCBase_DynamicBuffer_Header)+nDataAllocSize);
#endif
        s_PG_LCBase_DynamicBuffer_Header* pHeader = reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf);
        pHeader->commandID = EM_PG_Log_AtoLC_ExecuteSqlCommand_Dynamic;
        pHeader->commandType = LC_CMD_NULL;
        pHeader->sizeBuffer = nDataAllocSize;
        pHeader->sizeCanPop = nDataAllocSize;
        pHeader->sizePushed = 0;
        pHeader->itemCount = 0;
        pHeader->dataPtrTop = m_buf + sizeof(s_PG_LCBase_DynamicBuffer_Header) - sizeof(T1*);
        pHeader->dataPtrNow = pHeader->dataPtrTop;
    }

    virtual ~PG_LCBase_DynamicBuffer()
    {
        if (m_buf)
        {
#if LC_BASE_DYNAMIC_BUFFER_USE_BOOST_POOL
#else
            delete [] m_buf;
#endif
            m_buf = NULL;
        }
    }

    int GetTypeSize(int nType)
    {
        switch (nType)
        {
        case LC_SQLCDT_int:
            return static_cast<int>(sizeof(int));
        case LC_SQLCDT_smallint:
            return static_cast<int>(sizeof(short));
        case LC_SQLCDT_bigint:
            return static_cast<int>(sizeof(__int64));
        case LC_SQLCDT_tinyint:
            return static_cast<int>(sizeof(char));
        case LC_SQLCDT_bit:
            return static_cast<int>(sizeof(boolean));
        case LC_SQLCDT_float:
            return static_cast<int>(sizeof(float));
        case LC_SQLCDT_real:
            return static_cast<int>(sizeof(double));
        case LC_SQLCDT_char:
            return 0;
        case LC_SQLCDT_varchar:
            return 0;
        case LC_SQLCDT_nchar:
            return 0;
        case LC_SQLCDT_nvarchar:
            return 0;
        case LC_SQLCDT_binary:
            return 0;
        case LC_SQLCDT_varbinary:
            return 0;
        //case LC_SQLCDT_datetime:
        //    return 0;
        //case LC_SQLCDT_smalldatetime:
        //    return 0;
        default:
            SM_ASSERT(true, "Unknown Command Type");
            return 0;
        }
    }

// Push
#if 1

    bool Push(int nType, int nSize, void* vpData)
    {
#if LC_BASE_DYNAMIC_BUFFER_CHECK_PUSH_POP
        if (nType <= 0)
        {
            SM_ASSERT((nType <= 0), "invalid type");
            return false;
        }
        if (nSize < 1)
        {
            SM_ASSERT((nSize < 1), "invalid size");
            return false;
        }
        if (vpData == NULL)
        {
            SM_ASSERT((vpData == NULL), "invalid data");
            return false;
        }
#endif

        s_PG_LCBase_DynamicBuffer_Header* pHeader = reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf);

        if (pHeader->dataPtrNow == NULL)
        {
            SM_ASSERT((pHeader->dataPtrNow == NULL), "invalid pointer");
            return false;
        }
        
        pHeader->sizePushed += sizeof(nType);
        if (pHeader->sizePushed > pHeader->sizeBuffer)
        {
            SM_ASSERT((pHeader->sizePushed > pHeader->sizeBuffer), "buffer too small");
            return false;
        }

        memcpy(pHeader->dataPtrNow, &nType, sizeof(nType));
        pHeader->dataPtrNow += sizeof(nType);

        if (GetTypeSize(nType) == 0)
        {
            pHeader->sizePushed += sizeof(nSize);
            if (pHeader->sizePushed > pHeader->sizeBuffer)
            {
                SM_ASSERT((pHeader->sizePushed > pHeader->sizeBuffer), "buffer too small");
                return false;
            }

            memcpy(pHeader->dataPtrNow, &nSize, sizeof(nSize));
            pHeader->dataPtrNow += sizeof(nSize);
        }

        pHeader->sizePushed += nSize;
        if (pHeader->sizePushed > pHeader->sizeBuffer)
        {
            SM_ASSERT((pHeader->sizePushed > pHeader->sizeBuffer), "buffer too small");
            return false;
        }

        memcpy(pHeader->dataPtrNow, vpData, nSize);
        pHeader->dataPtrNow += nSize;

        ++pHeader->itemCount;

        return true;
    }

    bool PushInt32(int InputData)
    {
        return Push(LC_SQLCDT_int, static_cast<int>(sizeof(int)), &InputData);
    }

    bool PushInt16(short InputData)
    {
        return Push(LC_SQLCDT_smallint, static_cast<int>(sizeof(short)), &InputData);
    }

    bool PushInt64(__int64 InputData)
    {
        return Push(LC_SQLCDT_bigint, static_cast<int>(sizeof(__int64)), &InputData);
    }

    bool PushInt8(char InputData)
    {
        return Push(LC_SQLCDT_tinyint, static_cast<int>(sizeof(char)), &InputData);
    }

    bool PushBit(bool InputData)
    {
        return Push(LC_SQLCDT_bit, static_cast<int>(sizeof(bool)), &InputData);
    }

    bool PushFloat(float InputData)
    {
        return Push(LC_SQLCDT_float, static_cast<int>(sizeof(float)), &InputData);
    }

    bool PushDouble(double InputData)
    {
        return Push(LC_SQLCDT_real, static_cast<int>(sizeof(double)), &InputData);
    }

    bool PushChar(char* InputData)
    {
        return Push(LC_SQLCDT_char, static_cast<int>(sizeof(char) * (strlen(InputData) + 1)), InputData);
    }

    bool PushChar(char* InputData, size_t nSize)
    {
        return Push(LC_SQLCDT_char, static_cast<int>(nSize), InputData);
    }

    bool PushVarChar(char* InputData)
    {
        return Push(LC_SQLCDT_varchar, static_cast<int>(sizeof(char) * (strlen(InputData) +1 )), InputData);
    }

    bool PushVarChar(char* InputData, size_t nSize)
    {
        return Push(LC_SQLCDT_varchar, static_cast<int>(nSize), InputData);
    }

    bool PushNChar(wchar_t* InputData)
    {
        return Push(LC_SQLCDT_nchar, static_cast<int>(sizeof(wchar_t) * (wcslen(InputData) + 1)), InputData);
    }

    bool PushNChar(wchar_t* InputData, size_t nSize)
    {
        return Push(LC_SQLCDT_nchar, static_cast<int>(nSize), InputData);
    }

    bool PushNVarChar(wchar_t* InputData)
    {
        return Push(LC_SQLCDT_nvarchar, static_cast<int>(sizeof(wchar_t) * (wcslen(InputData) + 1)), InputData);
    }

    bool PushNVarChar(wchar_t* InputData, size_t nSize)
    {
        return Push(LC_SQLCDT_nvarchar, static_cast<int>(nSize), InputData);
    }

    bool PushBinary(char* InputData, size_t nSize)
    {
        return Push(LC_SQLCDT_binary, static_cast<int>(nSize), InputData);
    }

    bool PushVarBinary(char* InputData, size_t nSize)
    {
        return Push(LC_SQLCDT_varbinary, static_cast<int>(nSize), InputData);
    }

    bool PushBinary(byte* InputData, size_t nSize)
    {
        return Push(LC_SQLCDT_binary, static_cast<int>(nSize), InputData);
    }

    bool PushVarBinary(byte* InputData, size_t nSize)
    {
        return Push(LC_SQLCDT_varbinary, static_cast<int>(nSize), InputData);
    }

    //void PushDateTime() {};

    //void PushSmallDateTime() {};

#endif

    int PeekType()
    {
        s_PG_LCBase_DynamicBuffer_Header* pHeader = reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf);

        if (pHeader->dataPtrNow == NULL)
        {
            SM_ASSERT((pHeader->dataPtrNow == NULL), "invalid pointer");
            return LC_SQLCDT_NULL;
        }
        if (pHeader->itemCount <= 0)
        {
            SM_ASSERT((pHeader->itemCount <= 0), "no more items");
            return LC_SQLCDT_NULL;
        }
        if (pHeader->sizePushed <= 0)
        {
            SM_ASSERT((pHeader->sizePushed <= 0), "out of buffer");
            return LC_SQLCDT_NULL;
        }

        int iType = *(pHeader->dataPtrNow);
        return iType;
    }

    int PeekSize()
    {
        s_PG_LCBase_DynamicBuffer_Header* pHeader = reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf);

        if (pHeader->dataPtrNow == NULL)
        {
            SM_ASSERT((pHeader->dataPtrNow == NULL), "invalid pointer");
            return 0;
        }
        if (pHeader->itemCount <= 0)
        {
            SM_ASSERT((pHeader->itemCount <= 0), "no more items");
            return 0;
        }
        if (pHeader->sizePushed <= 0)
        {
            SM_ASSERT((pHeader->sizePushed <= 0), "out of buffer");
            return 0;
        }

        T1* temp_ptr = pHeader->dataPtrNow;

        int iType = *temp_ptr;
        temp_ptr += sizeof(iType);

        int iSize = GetTypeSize(iType);
        if (iSize == 0)
        {
            iSize = *temp_ptr;
        }

        return iSize;
    }

// Pop
#if 1

    bool Pop(int nTargetSize, void* vpOutput)
    {
#if LC_BASE_DYNAMIC_BUFFER_CHECK_PUSH_POP
        if (nTargetSize < 1)
        {
            SM_ASSERT((nTargetSize < 1), "invalid size");
            return false;
        }
        if (vpOutput == NULL)
        {
            SM_ASSERT((vpOutput == NULL), "invalid data");
            return false;
        }
#endif

        s_PG_LCBase_DynamicBuffer_Header* pHeader = reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf);

        if (pHeader->dataPtrNow == NULL)
        {
            SM_ASSERT((pHeader->dataPtrNow == NULL), "invalid pointer");
            return false;
        }
        if (pHeader->itemCount <= 0)
        {
            SM_ASSERT((pHeader->itemCount <= 0), "no more items");
            return false;
        }
        if (pHeader->sizePushed <= 0)
        {
            SM_ASSERT((pHeader->sizePushed <= 0), "out of buffer");
            return false;
        }

        int iType = *pHeader->dataPtrNow;

        pHeader->sizePushed -= sizeof(iType);
        pHeader->dataPtrNow += sizeof(iType);

        int iSize = GetTypeSize(iType);
        if (iSize == 0)
        {
            if (pHeader->sizePushed <= 0)
            {
                SM_ASSERT((pHeader->sizePushed <= 0), "out of buffer");
                return false;
            }

            iSize = *pHeader->dataPtrNow;

            if (iSize > nTargetSize)
            {
                SM_ASSERT((iSize > nTargetSize), "target size too small");
                return false;
            }

            pHeader->sizePushed -= sizeof(iSize);
            pHeader->dataPtrNow += sizeof(iSize);
        }

        if (pHeader->sizePushed <= 0)
        {
            SM_ASSERT((pHeader->sizePushed <= 0), "out of buffer");
            return false;
        }

        memcpy(vpOutput, pHeader->dataPtrNow, iSize);
        
        pHeader->sizePushed -= iSize;
        pHeader->dataPtrNow += iSize;

        --pHeader->itemCount;

        return true;
    }

#endif

    //////////////////////////////////////////////////////////////////////////

    inline void ResetPointer()
    { reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->dataPtrNow = m_buf + sizeof(s_PG_LCBase_DynamicBuffer_Header) - sizeof(byte*); }

    inline int Count()
    { return reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->itemCount; }

    inline int Size()
    {
#if LC_BASE_DYNAMIC_BUFFER_PRINT_SIZE
        char szDebug[64];
        sprintf(szDebug, "PG/PG_LCBase_DynamicBuffer::Size() = %d\n", reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->sizePushed + sizeof(s_PG_LCBase_DynamicBuffer_Header));
        OutputDebugString(szDebug);
#endif
        return reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->sizePushed + sizeof(s_PG_LCBase_DynamicBuffer_Header);
    }

    inline T1* Buffer()
    { return m_buf; }

// 不打算公開的函式.
#if 0
    inline s_PG_LCBase_DynamicBuffer_Header* Header()
    { return reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf); }

    inline void SetCommandID(int nValue)
    { reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->commandID = nValue; }

    inline int GetCommandID()
    { return reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->commandID; }

    inline void SetCommandType(int nValue)
    { reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->commandType = nValue; }

    inline int GetCommandType()
    { return reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->commandType; }

    inline void SetBufferSize(int nValue)
    { reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->sizeBuffer = nValue; }

    inline int GetBufferSize()
    { return reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->sizeBuffer; }

    inline void SetPopSize(int nValue)
    { reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->sizeCanPop = nValue; }

    inline int GetPopSize()
    { return reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->sizeCanPop; }

    inline void SetPushSize(int nValue)
    { reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->sizePushed = nValue; }

    inline int GetPushSize()
    { return reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->sizePushed; }

    inline void SetItemCount(int nValue)
    { reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->itemCount = nValue; }

    inline int GetItemCount()
    { return reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->itemCount; }

    inline void MoveInnerPtr(int nShift)
    { reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->dataPtrNow += nShift; }

    inline byte* GetInnerPtr()
    { return reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->dataPtrNow; }

    inline byte* GetInnerData()
    { reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->dataPtrTop; }

#endif

// Get And Set
#if 1

    inline void SetCommandType(int nValue)
    { reinterpret_cast<s_PG_LCBase_DynamicBuffer_Header*>(m_buf)->commandType = nValue; }

#endif

protected:

private:
    T1*               m_buf;

};

#endif // LOG_CENTER_BASE_DYNAMIC_BUFFER_H