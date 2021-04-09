#ifndef LOG_CENTER_BASE_STATIC_BUFFER_H
#define LOG_CENTER_BASE_STATIC_BUFFER_H

//////////////////////////////////////////////////////////////////////////
// Last Update - by slime 2011/01/07
//////////////////////////////////////////////////////////////////////////

#include "../../../ShareLib/sm_debug.h"

// 預設靜態封包暫存區配置大小.
//#define PG_LCBASE_STATIC_PACKAGE_BUFFER_SIZE 0x0080 //   128
//#define PG_LCBASE_STATIC_PACKAGE_BUFFER_SIZE 0x0100 //   256
#define PG_LCBASE_STATIC_PACKAGE_BUFFER_SIZE 0x0200 //   512
//#define PG_LCBASE_STATIC_PACKAGE_BUFFER_SIZE 0x0400 //  1024
//#define PG_LCBASE_STATIC_PACKAGE_BUFFER_SIZE 0x0800 //  2048
//#define PG_LCBASE_STATIC_PACKAGE_BUFFER_SIZE 0x1000 //  4096
//#define PG_LCBASE_STATIC_PACKAGE_BUFFER_SIZE 0x2000 //  8192
//#define PG_LCBASE_STATIC_PACKAGE_BUFFER_SIZE 0x4000 // 16384
// note : 設定過大會影響發送端效能, 完全不影響接收端效能.
// note : 假設暫存區不夠大, 則 push 會失敗, 而 pop 有可能會出錯.

#if _DEBUG
#define LC_BASE_STATIC_BUFFER_CHECK_PUSH_POP 1  // 是否在 push / pop 檢查輸入資訊.
#define LC_BASE_STATIC_BUFFER_PRINT_SIZE 1      // 是否印出 size (for debug)
#else
#define LC_BASE_STATIC_BUFFER_CHECK_PUSH_POP 1
#define LC_BASE_STATIC_BUFFER_PRINT_SIZE 1
#endif

// 靜態範本型封包標頭定義.
struct s_PG_LCBase_StaticBuffer_Header
{
    GamePGCommandEnum       m_eCommand;     // 封包命令種類.
    e_LogCenterCommandType  m_eCmdType;     // 日誌紀錄指令種類.
    int                     m_iBufferSize;  // 動態暫存區大小.
    int                     m_sizeBuffer;   // 剩餘暫存區大小.
    int                     m_sizeTotal;    // 已用暫存區大小.
    int                     m_itemCount;    // 內含元素數量.
    byte*                   m_ptr;          // 目前資料指標.
};

// 靜態範本型封包 (Static Buffer)
// 優點 : 可節省接收端的額外記憶體配置(接收端只使用指標取用封包中的資料)
// 缺點 : 收發兩端的配置必須一致(否則會有資料遺失的風險)

template <int N1>
class PG_LCBase_AtoLC_StaticBuffer
{
public:
    PG_LCBase_AtoLC_StaticBuffer()
        : m_eCommand(EM_PG_Log_AtoLC_ExecuteSqlCommand_Static)
        , m_eCmdType(LC_CMD_NULL)
        , m_iBufferSize(0)
        , m_sizeBuffer(0)
        , m_sizeTotal(0)
        , m_itemCount(0)
        , m_ptr(NULL)
    {
        m_iBufferSize = N1;     // base size (do not change it)
        m_sizeBuffer = N1;
        ResetPointer();
    }

    virtual ~PG_LCBase_AtoLC_StaticBuffer()
    {
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
#if LC_BASE_STATIC_BUFFER_CHECK_PUSH_POP
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

        if (m_ptr == NULL)
        {
            SM_ASSERT((m_ptr == NULL), "invalid pointer");
            return false;
        }

        m_sizeTotal += sizeof(nType);
        if (m_sizeTotal > m_sizeBuffer)
        {
            SM_ASSERT((m_sizeTotal > m_sizeBuffer), "buffer too small");
            return false;
        }

        memcpy(m_ptr, &nType, sizeof(nType));       // set type
        m_ptr += sizeof(nType);                     // shift ptr

        if (GetTypeSize(nType) == 0)
        {
            m_sizeTotal += sizeof(nSize);
            if (m_sizeTotal > m_sizeBuffer)
            {
                SM_ASSERT((m_sizeTotal > m_sizeBuffer), "buffer too small");
                return false;
            }

            memcpy(m_ptr, &nSize, sizeof(nSize));   // set size
            m_ptr += sizeof(nSize);                 // shift ptr
        }

        m_sizeTotal += nSize;
        if (m_sizeTotal > m_sizeBuffer)
        {
            SM_ASSERT((m_sizeTotal > m_sizeBuffer), "buffer too small");
            return false;
        }

        memcpy(m_ptr, vpData, nSize);               // set data
        m_ptr += nSize;                             // shift ptr

        ++m_itemCount;

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
        if (m_ptr == NULL)
        {
            SM_ASSERT((m_ptr == NULL), "invalid pointer");
            return LC_SQLCDT_NULL;
        }
        if (m_itemCount <= 0)
        {
            SM_ASSERT((m_itemCount <= 0), "no more items");
            return LC_SQLCDT_NULL;
        }
        if (m_sizeTotal <= 0)
        {
            SM_ASSERT((m_sizeTotal <= 0), "out of buffer");
            return LC_SQLCDT_NULL;
        }

        int iType = *m_ptr;
        return iType;
    }

    int PeekSize()
    {
        if (m_ptr == NULL)
        {
            SM_ASSERT((m_ptr == NULL), "invalid pointer");
            return 0;
        }
        if (m_itemCount <= 0)
        {
            SM_ASSERT((m_itemCount <= 0), "no more items");
            return 0;
        }
        if (m_sizeTotal <= 0)
        {
            SM_ASSERT((m_sizeTotal <= 0), "out of buffer");
            return 0;
        }

        byte* temp_ptr = m_ptr;

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
#if LC_BASE_STATIC_BUFFER_CHECK_PUSH_POP
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

        if (m_ptr == NULL)
        {
            SM_ASSERT((m_ptr == NULL), "invalid pointer");
            return false;
        }
        if (m_itemCount <= 0)
        {
            SM_ASSERT((m_itemCount <= 0), "no more items");
            return false;
        }
        if (m_sizeTotal <= 0)
        {
            SM_ASSERT((m_sizeTotal <= 0), "out of buffer");
            return false;
        }

        int iType = *m_ptr;                 // get type

        m_sizeTotal -= sizeof(iType);       // size used
        m_ptr += sizeof(iType);             // shift ptr

        int iSize = GetTypeSize(iType);
        if (iSize == 0)
        {
            if (m_sizeTotal <= 0)
            {
                SM_ASSERT((m_sizeTotal <= 0), "out of buffer");
                return false;
            }

            iSize = *m_ptr;                 // get size

            if (iSize > nTargetSize)
            {
                SM_ASSERT((iSize > nTargetSize), "target size too small");
                return false;
            }

            m_sizeTotal -= sizeof(iSize);   // size used
            m_ptr += sizeof(iSize);         // shift ptr
        }

        if (m_sizeTotal <= 0)
        {
            SM_ASSERT((m_sizeTotal <= 0), "out of buffer");
            return false;
        }

        memcpy(vpOutput, m_ptr, iSize);     // get data

        m_sizeTotal -= iSize;               // size used
        m_ptr += iSize;                     // shift ptr

        --m_itemCount;

        return true;
    }

#endif

    //////////////////////////////////////////////////////////////////////////

    // remember do this after getting package pointer, before pop data.
    inline void ResetPointer() { m_ptr = reinterpret_cast<byte*>(m_buf); }

    inline int Count() { return m_itemCount; }

    inline int Size()
    {
#if LC_BASE_STATIC_BUFFER_PRINT_SIZE
        char szDebug[64];
        sprintf(szDebug, "PG/PG_LCBase_AtoLC_StaticBuffer::Size() = %d\n", m_sizeTotal + sizeof(s_PG_LCBase_StaticBuffer_Header));
        OutputDebugString(szDebug);
#endif
        return m_sizeTotal + sizeof(s_PG_LCBase_StaticBuffer_Header);
    }

    inline int BuffSize()
    { return m_sizeBuffer; }

    inline int UsedSize()
    { return m_sizeTotal; }

    inline e_LogCenterCommandType GetType()
    { return m_eCmdType; }

    inline void SetType(e_LogCenterCommandType eType)
    { m_eCmdType = eType; }

protected:

private:
    GamePGCommandEnum       m_eCommand;     // 封包命令種類.
    e_LogCenterCommandType  m_eCmdType;     // 日誌紀錄指令種類.
    int                     m_iBufferSize;  // 動態暫存區大小.
    int                     m_sizeBuffer;   // for pop
    int                     m_sizeTotal;    // for push
    int                     m_itemCount;    // for double check
    byte*                   m_ptr;          // data pointer
    byte*                   m_buf[N1];      // data buffer

};

//////////////////////////////////////////////////////////////////////////

// 舊寫法...(捨棄繼承，直接改用標頭mapping的方式取用資訊)
#if 0

// 日誌伺服器命令封包(動態暫存)基礎類別.

class PG_LCBase_AtoLC_LogCommandDynamicBufferBase
{
public:

    PG_LCBase_AtoLC_LogCommandDynamicBufferBase()
        : m_eCommand(EM_PG_Log_AtoLC_ExecuteSqlCommand_Dynamic)
        , m_eCmdType(LC_CMD_NULL)
        , m_iBufferSize(0)
    {
    }

    //////////////////////////////////////////////////////////////////////////

    inline int BaseSize() { return m_iBufferSize + sizeof(PG_LCBase_AtoLC_LogCommandDynamicBufferBase); }

    inline e_LogCenterCommandType CommandType() { return m_eCmdType; }
    inline void CommandType(e_LogCenterCommandType eType) { m_eCmdType = eType; }

    inline int BufferSize() { return m_iBufferSize; }
    inline void BufferSize(int nSize) { m_iBufferSize = nSize; }

protected:

private:
    GamePGCommandEnum       m_eCommand;     // 封包命令種類.
    e_LogCenterCommandType  m_eCmdType;     // 日誌紀錄指令種類.
    int                     m_iBufferSize;  // 動態暫存區大小.

};

// 日誌伺服器命令封包(動態暫存)類別模板.

template <int N1>
class PG_LCBase_AtoLC_LogCommandDynamicBuffer : public PG_LCBase_AtoLC_LogCommandDynamicBufferBase
{
public:
    PG_LCBase_AtoLC_LogCommandDynamicBuffer()
        : m_sizeBuffer(0)
        , m_sizeTotal(0)
        , m_itemCount(0)
        , m_ptr(NULL)
    {
        BufferSize(N1);     // base size (do not change it)
        m_sizeBuffer = N1;
        ResetPointer();
    }

    bool Push(int nType, int nSize, void* vpData)
    {
        if (!m_ptr)
            return false;

        m_sizeTotal += sizeof(nType);
        if (m_sizeTotal > m_sizeBuffer)
        {
            assert(!(m_sizeTotal > m_sizeBuffer) && "buffer too small");
            return false;
        }

        memcpy(m_ptr, &nType, sizeof(nType));       // set type
        m_ptr += sizeof(nType);                     // shift ptr

        if (GetTypeSize(nType) == 0)
        {
            m_sizeTotal += sizeof(nSize);
            if (m_sizeTotal > m_sizeBuffer)
            {
                assert(!(m_sizeTotal > m_sizeBuffer) && "buffer too small");
                return false;
            }

            memcpy(m_ptr, &nSize, sizeof(nSize));   // set size
            m_ptr += sizeof(nSize);                 // shift ptr
        }

        m_sizeTotal += nSize;
        if (m_sizeTotal > m_sizeBuffer)
        {
            assert(!(m_sizeTotal > m_sizeBuffer) && "buffer too small");
            return false;
        }

        memcpy(m_ptr, vpData, nSize);               // set data
        m_ptr += nSize;                             // shift ptr

        ++m_itemCount;

        return true;
    }

    bool PushInt32(int InputData)
    {
        return Push(LC_SQLCDT_int, sizeof(int), &InputData);
    }

    bool PushInt16(short InputData)
    {
        return Push(LC_SQLCDT_smallint, sizeof(short), &InputData);
    }

    bool PushInt64(__int64 InputData)
    {
        return Push(LC_SQLCDT_bigint, sizeof(__int64), &InputData);
    }

    bool PushInt8(char InputData)
    {
        return Push(LC_SQLCDT_tinyint, sizeof(char), &InputData);
    }

    bool PushBit(bool InputData)
    {
        return Push(LC_SQLCDT_bit, sizeof(bool), &InputData);
    }

    bool PushFloat(float InputData)
    {
        return Push(LC_SQLCDT_float, sizeof(float), &InputData);
    }

    bool PushDouble(double InputData)
    {
        return Push(LC_SQLCDT_real, sizeof(double), &InputData);
    }

    bool PushChar(char* InputData)
    {
        return Push(LC_SQLCDT_char, sizeof(char) * (strlen(InputData)+1), InputData);
    }

    bool PushChar(char* InputData, size_t nSize)
    {
        return Push(LC_SQLCDT_char, nSize, InputData);
    }

    bool PushVarChar(char* InputData)
    {
        return Push(LC_SQLCDT_varchar, sizeof(char) * (strlen(InputData)+1), InputData);
    }

    bool PushVarChar(char* InputData, size_t nSize)
    {
        return Push(LC_SQLCDT_varchar, nSize, InputData);
    }

    bool PushNChar(wchar_t* InputData)
    {
        return Push(LC_SQLCDT_nchar, sizeof(wchar_t) * (wcslen(InputData)+1), InputData);
    }

    bool PushNChar(wchar_t* InputData, size_t nSize)
    {
        return Push(LC_SQLCDT_nchar, nSize, InputData);
    }

    bool PushNVarChar(wchar_t* InputData)
    {
        return Push(LC_SQLCDT_nvarchar, sizeof(wchar_t) * (wcslen(InputData)+1), InputData);
    }

    bool PushNVarChar(wchar_t* InputData, size_t nSize)
    {
        return Push(LC_SQLCDT_nvarchar, nSize, InputData);
    }

    bool PushBinary(char* InputData, size_t nSize)
    {
        return Push(LC_SQLCDT_binary, nSize, InputData);
    }

    bool PushVarBinary(char* InputData, size_t nSize)
    {
        return Push(LC_SQLCDT_varbinary, nSize, InputData);
    }

    bool PushBinary(byte* InputData, size_t nSize)
    {
        return Push(LC_SQLCDT_binary, nSize, InputData);
    }

    bool PushVarBinary(byte* InputData, size_t nSize)
    {
        return Push(LC_SQLCDT_varbinary, nSize, InputData);
    }

    //void PushDateTime() {};

    //void PushSmallDateTime() {};

    int PeekType()
    {
        if (!m_ptr)
            return LC_SQLCDT_NULL;

        if (m_itemCount <= 0)
        {
            assert(!(m_itemCount <= 0) && "no more items");
            return LC_SQLCDT_NULL;
        }

        if (m_sizeTotal <= 0)
        {
            assert(!(m_sizeTotal <= 0) && "out of buffer");
            return LC_SQLCDT_NULL;
        }

#if _DEBUG
        int iType = *m_ptr;
        return iType;
#else
        return *m_ptr;
#endif
    }

    int PeekSize()
    {
        if (!m_ptr)
            return 0;

        if (m_itemCount <= 0)
        {
            assert(!(m_itemCount <= 0) && "no more items");
            return 0;
        }

        if (m_sizeTotal <= 0)
        {
            assert(!(m_sizeTotal <= 0) && "out of buffer");
            return 0;
        }

        byte* temp_ptr = m_ptr;

        int iType = *temp_ptr;
        temp_ptr += sizeof(iType);

        int iSize = GetTypeSize(iType);
        if (iSize == 0)
        {
            iSize = *temp_ptr;
        }

        return iSize;
    }

    bool Pop(int nTargetSize, void* vpOutput)
    {
        if (!m_ptr)
            return false;

        if (m_itemCount <= 0)
        {
            assert(!(m_itemCount <= 0) && "no more items");
            return false;
        }

        if (m_sizeTotal <= 0)
        {
            assert(!(m_sizeTotal <= 0) && "out of buffer");
            return false;
        }

        int iType = *m_ptr;                 // get type

        m_sizeTotal -= sizeof(iType);       // size used
        m_ptr += sizeof(iType);             // shift ptr

        int iSize = GetTypeSize(iType);
        if (iSize == 0)
        {
            if (m_sizeTotal <= 0)
            {
                assert(!(m_sizeTotal <= 0) && "out of buffer");
                return false;
            }

            iSize = *m_ptr;                 // get size

            if (iSize > nTargetSize)
            {
                assert(!(iSize > nTargetSize) && "target size too small");
                return false;
            }

            m_sizeTotal -= sizeof(iSize);   // size used
            m_ptr += sizeof(iSize);         // shift ptr
        }

        if (m_sizeTotal <= 0)
        {
            assert(!(m_sizeTotal <= 0) && "out of buffer");
            return false;
        }

        memcpy(vpOutput, m_ptr, iSize); // get data

        m_sizeTotal -= iSize;           // size used
        m_ptr += iSize;                 // shift ptr

        --m_itemCount;

        return true;
    }

    int GetTypeSize(int nType)
    {
        switch (nType)
        {
        case LC_SQLCDT_int:
            return sizeof(int);
        case LC_SQLCDT_smallint:
            return sizeof(short);
        case LC_SQLCDT_bigint:
            return sizeof(__int64);
        case LC_SQLCDT_tinyint:
            return sizeof(char);
        case LC_SQLCDT_bit:
            return sizeof(boolean);
        case LC_SQLCDT_float:
            return sizeof(float);
        case LC_SQLCDT_real:
            return sizeof(double);
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
            assert(0 && "Unknown Command Type");
            return 0;
        }
    }

    // remember do this after getting package pointer, before pop data.
    inline void ResetPointer() { m_ptr = reinterpret_cast<byte*>(m_buf); }

    inline int Count() { return m_itemCount; }

    inline int Size() { return m_sizeTotal + c_iHeaderSize + sizeof(PG_LCBase_AtoLC_LogCommandDynamicBufferBase); }

    inline int BuffSize() { return m_sizeBuffer; }
    inline void BuffSize(int nSize) { m_sizeBuffer = nSize; }

    inline int UsedSize() { return m_sizeTotal; }

protected:

private:
    // headerSize = sizeof(m_sizeBuffer + m_sizeTotal + m_itemCount + m_ptr)
    static const int c_iHeaderSize = sizeof(int) + sizeof(int) + sizeof(int) + sizeof(byte*);

    // note: size use integer for the comparison, do not use unsigned integer.
    int             m_sizeBuffer;   // for pop
    int             m_sizeTotal;    // for push
    int             m_itemCount;    // for double check
    byte*           m_ptr;          // data pointer
    byte*           m_buf[N1];      // data buffer

};

#endif

//////////////////////////////////////////////////////////////////////////

#endif // LOG_CENTER_BASE_STATIC_BUFFER_H