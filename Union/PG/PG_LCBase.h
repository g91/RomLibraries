#ifndef LOG_CENTER_BASE_PACKAGE_DEFINE_H
#define LOG_CENTER_BASE_PACKAGE_DEFINE_H

//////////////////////////////////////////////////////////////////////////
// Last Update - by slime 2011/01/03
//////////////////////////////////////////////////////////////////////////

#include "PG/PG_LCBase_PackageDefine_Auto.h"

//#include "D:\PROJECT\Server\NetGameServer\NetGameServer\RoleData\RoleBaseDef.h" // TODO:

#if 0
enum e_LogCenterOutputMessageLevel
{
    LC_OUTMSG_LV_NULL = 0,

    LC_OUTMSG_LV_1_INFO,        // information
    LC_OUTMSG_LV_2,
    LC_OUTMSG_LV_3_WARNING,     // warning
    LC_OUTMSG_LV_4,
    LC_OUTMSG_LV_5_ERROR,       // error
    LC_OUTMSG_LV_6,
    LC_OUTMSG_LV_7_CRITICAL,    // critical
    LC_OUTMSG_LV_8,
    LC_OUTMSG_LV_9_ASSERT,      // assert

    LC_OUTMSG_LV_MAX
};
#endif

// 傳入指令的 SQL 分類.
enum e_LogCenterSqlCommandType
{
    LC_SQLCMD_NULL = 0,

    LC_SQLCMD_SELECT,   // 查詢.
    LC_SQLCMD_INSERT,   // 新增.
    LC_SQLCMD_UPDATE,   // 修改.
    LC_SQLCMD_DELETE,   // 刪除.

    LC_SQLCMD_MAX
};

// 傳入欄位的資料型態.
enum e_LogCenterSqlColumnDataType
{
    LC_SQLCDT_NULL = 0,

    LC_SQLCDT_int,
    LC_SQLCDT_smallint,
    LC_SQLCDT_bigint,
    LC_SQLCDT_tinyint,
    LC_SQLCDT_bit,
    LC_SQLCDT_float,
    LC_SQLCDT_real,
    LC_SQLCDT_char,
    LC_SQLCDT_varchar,
    LC_SQLCDT_nchar,
    LC_SQLCDT_nvarchar,
    LC_SQLCDT_binary,
    LC_SQLCDT_varbinary,
    //LC_SQLCDT_datetime,
    //LC_SQLCDT_smalldatetime,
    LC_SQLCDT_unknown,

    LC_SQLCDT_MAX
};

#if 0
enum SqlCommandTypeENUM
{
    EM_SqlCommandType_GlobalDB	,
    EM_SqlCommandType_ImportDB	,
    EM_SqlCommandType_WorldDB	,
    EM_SqlCommandType_WorldLogDB,	

};

struct PG_DBBase_LtoD_SqlCommand
{
    GamePGCommandEnum	Command;
    SqlCommandTypeENUM Type;
    int		ThreadID;
    int		Size;
    StaticString< 0x10000 >   Data;

    PG_DBBase_LtoD_SqlCommand()
    {
        Command	= EM_PG_DBBase_LtoD_SqlCommand;
    }

    int PGSize()
    {
        return Size + sizeof(*this) - sizeof( Data );
    }
};

#endif

//////////////////////////////////////////////////////////////////////////

#if 0

#define LOG_CENTER_PACKAGE_DATA_BUFFER_SIZE_MAX 4096

struct s_LogCenter_PackageDataBufferDiscription
{
    // 綁定參數(狀態編號, 參數編號, 資料種類, 參數種類,
    //           欄位大小, 小數點位數, 參數資料指標, 緩衝區大小,
    //           輸入/輸出(SQL_PARAM_INPUT(1)/SQL_PARAM_OUTPUT(4)), 回傳大小)
    //int             BindParameterByID(  int StmtID, int ParamID, int ValueType, int ParamType,
    //                                    int ColumnSize, int DecimalDigits, void* ParamValue, int BufferLength,
    //                                    int InOut = 1, long* ReturnSize = NULL); // SQL_PARAM_INPUT = 1

    int iColumnNameSize;
    int iColumnDataSize;
};

// 日誌紀錄伺服器封包延伸類別.
struct PG_LCBase_AtoLC_SqlCommandEx : PG_LCBase_AtoLC_PackageBase
{
    e_LogCenterSqlCommandType   eSqlType;   // SQL 命令種類.
    int iTotalSize;
    int iTableNameSize;
    char bufferData[LOG_CENTER_PACKAGE_DATA_BUFFER_SIZE_MAX];   // 資料暫存區(沒用到的就浪費掉了)
};

#endif

//////////////////////////////////////////////////////////////////////////

struct PG_LCBase_AtoLC_SqlCommand
{
    GamePGCommandEnum       eCommand;
    //SqlCommandTypeENUM      eSqlType;
    e_LogCenterCommandType  eCmdType;
    int                     iSize;
    StaticString<0x2000>    Data;   // size = 8192

    PG_LCBase_AtoLC_SqlCommand()
        : eCommand(EM_PG_Log_AtoLC_ExecuteSqlCommand)
        //, eType(EM_SqlCommandType_WorldLogDB)
        , eCmdType(LC_CMD_NULL)
        , iSize(0)
    {
    }
};

#endif // LOG_CENTER_BASE_PACKAGE_DEFINE_H
