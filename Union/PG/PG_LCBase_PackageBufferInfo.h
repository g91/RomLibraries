#ifndef LOG_CENTER_BASE_PACKAGE_BUFFER_INFO_H
#define LOG_CENTER_BASE_PACKAGE_BUFFER_INFO_H

//////////////////////////////////////////////////////////////////////////
// Last Update - by slime 2011/06/08
//////////////////////////////////////////////////////////////////////////

#if _DEBUG
    #ifndef LOG_CENTER_BASE_PACKAGE_BUFFER_USE_TEMPLATE
    #define LOG_CENTER_BASE_PACKAGE_BUFFER_USE_TEMPLATE 0   // don't use template now ... it's only byte array from network packages.
    #endif
    #ifndef LOG_CENTER_PRINT_PACKAGE_BUFFER_CREATE
    #define LOG_CENTER_PRINT_PACKAGE_BUFFER_CREATE 0        // �O�_��ܫإ߻P���񤺳��Ȧs�Ϫ��T��. (������)
    #endif
#else
    #ifndef LOG_CENTER_BASE_PACKAGE_BUFFER_USE_TEMPLATE
    #define LOG_CENTER_BASE_PACKAGE_BUFFER_USE_TEMPLATE 0
    #endif
    #ifndef LOG_CENTER_PRINT_PACKAGE_BUFFER_CREATE
    #define LOG_CENTER_PRINT_PACKAGE_BUFFER_CREATE 0
    #endif
#endif

#include "../../../ShareLib/cDirectSQLBase.h"

// �ʥ]�Ȧs�θ�T.
#if LOG_CENTER_BASE_PACKAGE_BUFFER_USE_TEMPLATE
template <typename T1>
#endif
class PackageBufferInfo
{
public:
    char            szVersion[10+1];    // ����(x.x.x.xxxx) // �e�T�X�u���0~9
    tm              timeAppend;         // �[�J�ɶ�(�����ɶ�)
    tm              timeSave;           // �x�s�ɶ�(�ɦW�̾�)
    int             iSID;               // �ǦC�s��(�۰ʻ��W, �C�����ҫ᭫�m)
    int             iExecCount;         // ����L�X��.
    int             iNetID;             // �ШD�ݦ��A���s��(�u���b runtime ����)
    int             iCmdID;             // ���O�s��.
    int             iCmdType;           // ���O����.
    int             iSize;              // ��Ƥj�p.
#if LOG_CENTER_BASE_PACKAGE_BUFFER_USE_TEMPLATE
    T1*             pData;              // ��ƹ�ڦ�m.
#else
    byte*           pData;              // ��ƹ�ڦ�m.
#endif
    DataAccessLayer::SqlStmtAsynExecCallback_Static cbDone_S;    // static callback function
    DataAccessLayer::SqlStmtAsynExecCallback_Static cbError_S;   // static callback function
    DataAccessLayer::SqlStmtAsynExecCallback_Boost  cbDone_B;    // boost::bind callback function
    DataAccessLayer::SqlStmtAsynExecCallback_Boost  cbError_B;   // boost::bind callback function
    PackageBufferInfo*          pThis;  // self pointer.
    DataAccessLayer::s_StmtObj* pStmt;  // statement object for callback
    int             iRID;               // return ID for callback

    PackageBufferInfo()
        : iSID(0)
        , iExecCount(0)
        , iNetID(0)
        , iCmdID(0)
        , iCmdType(0)
        , iSize(0)
        , pData(NULL)
        , cbDone_S(NULL)
        , cbError_S(NULL)
        , cbDone_B(NULL)
        , cbError_B(NULL)
        , pThis(NULL)
        , pStmt(NULL)
        , iRID(0)
    {
        memset(szVersion, 0, sizeof(szVersion));
    }

    virtual ~PackageBufferInfo()
    {
        ReleaseInnerBuffer();
    }

    void CreateInnerBuffer(int nSourceSize, void* vpSourceData)
    {
        // make sure that buffer memory, allocated before, will be deleted.
        ReleaseInnerBuffer();

        // get size
        SM_ASSERT((nSourceSize > 0x2800), "nSourceSize is too big!");
        iSize = nSourceSize;
        if (iSize < 1)
        {
            iSize = 1;
        }
        // create buffer
#if LOG_CENTER_BASE_PACKAGE_BUFFER_USE_TEMPLATE
        pData = new T1[iSize]; // please remember to delete it.
#else
        pData = new byte[iSize];
#endif
        // copy data
        if (vpSourceData != NULL)
        {
            memcpy(pData, vpSourceData, iSize); // copy package data and keep it.
        }
#if LOG_CENTER_PRINT_PACKAGE_BUFFER_CREATE
        SM_TRACE(_T("CreateInnerBuffer(%d, 0x%p -> 0x%p)\n"), iSize, vpSourceData, pData);
#endif
    }

    void ReleaseInnerBuffer()
    {
#if LOG_CENTER_PRINT_PACKAGE_BUFFER_CREATE
        SM_TRACE(_T("ReleaseInnerBuffer(0x%p)\n"), pData);
#endif
        if (pData != NULL)
        {
            delete [] pData;
            pData = NULL;
        }
    }
};

#endif // LOG_CENTER_BASE_PACKAGE_BUFFER_INFO_H