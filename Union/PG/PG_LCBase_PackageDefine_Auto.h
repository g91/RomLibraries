#ifndef LOG_CENTER_BASE_PACKAGE_DEFINE_AUTO_H
#define LOG_CENTER_BASE_PACKAGE_DEFINE_AUTO_H

//////////////////////////////////////////////////////////////////////////
// Last Update - by slime 2011/02/22
//////////////////////////////////////////////////////////////////////////

#define LOG_CENTER_BASE_PACKAGE_USE_BUFFER 1        // 是否使用暫存區(預存程序綁定用)
#define LOG_CENTER_BASE_PACKAGE_USE_TEMPLATE 1      // 是否使用樣板建置暫存區.

#if LOG_CENTER_BASE_PACKAGE_USE_TEMPLATE
#else
#include "../../DataClass/DataClassDefine.h"
#endif

#include "NetWakerPGEnum.h"

//////////////////////////////////////////////////////////////////////////

// 傳入的指令種類(表格名稱)
enum e_LogCenterCommandType
{
    LC_CMD_NULL = 0,

    LC_CMD_LocalServerProcTimeLog,
    LC_CMD_DesignLog,
    LC_CMD_RoleCenterStatus,
    LC_CMD_ClientEventLog,
    LC_CMD_ServerItemLog,
    LC_CMD_ProxyPingLog,
    LC_CMD_ExpLog,
    LC_CMD_GuildContributionLog,
    LC_CMD_SkillLvLog,
    LC_CMD_GameLoginLog,
    LC_CMD_TalkLog,
    LC_CMD_Warning_MoneyVendor,
    LC_CMD_BonusMoneyLog,
    LC_CMD_Warning_Bot,
    LC_CMD_GuildLog,
    LC_CMD_PlayerPlayHistory,
    LC_CMD_DepartmentStoreLog,
    LC_CMD_LevelLog,
    LC_CMD_GMToolsCommandLog,
    LC_CMD_DebugMessage,
    LC_CMD_Version,
    LC_CMD_LocalServerStatus,
    LC_CMD_MasterServerStatus,
    LC_CMD_MoneyLog,
    LC_CMD_MonsterDead,
    LC_CMD_GMCommandLog,
    LC_CMD_PlayerActionLog,
    LC_CMD_ItemTradeLog,
    LC_CMD_ReadWriteTestTable,
    LC_CMD_AcLog,
    LC_CMD_PlayerDead,
    LC_CMD_AccountMoneyLog,
    LC_CMD_ClientStatus,
    LC_CMD_HouseLog,
    LC_CMD_GameGuardReport,
    LC_CMD_DataCenterServerStatus,
    LC_CMD_ClientPingLog,
    LC_CMD_QuestLog,
    LC_CMD_AgencyCCU,

    LC_CMD_TotalCount,
    LC_CMD_MAX
};

//////////////////////////////////////////////////////////////////////////

// 日誌紀錄伺服器封包基礎類別.
struct PG_LCBase_AtoLC_PackageBase
{
    GamePGCommandEnum       eCommand;   // 封包命令種類.
    e_LogCenterCommandType  eCmdType;   // 日誌紀錄指令種類.

    PG_LCBase_AtoLC_PackageBase()
        : eCommand(EM_PG_Log_AtoLC_ExecuteSqlCommand)
        , eCmdType(LC_CMD_NULL)
    {
    }
};

//////////////////////////////////////////////////////////////////////////

#if LOG_CENTER_BASE_PACKAGE_USE_TEMPLATE

template <e_LogCenterCommandType N1, typename T1>
struct PG_LCBase_AtoLC_Package : public PG_LCBase_AtoLC_PackageBase
{
    T1 table;           // 對應資料類別(or 結構)
    PG_LCBase_AtoLC_Package()
    {
        eCmdType = N1;  // 命令編號.
    }
};

#else

struct PG_LCBase_AtoLC_LocalServerProcTimeLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::LocalServerProcTimeLog table;
    PG_LCBase_AtoLC_LocalServerProcTimeLog()
    {
        eCmdType = LC_CMD_LocalServerProcTimeLog;
    }
};

struct PG_LCBase_AtoLC_DesignLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::DesignLog table;
    PG_LCBase_AtoLC_DesignLog()
    {
        eCmdType = LC_CMD_DesignLog;
    }
};

struct PG_LCBase_AtoLC_RoleCenterStatus : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::RoleCenterStatus table;
    PG_LCBase_AtoLC_RoleCenterStatus()
    {
        eCmdType = LC_CMD_RoleCenterStatus;
    }
};

struct PG_LCBase_AtoLC_ClientEventLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::ClientEventLog table;
    PG_LCBase_AtoLC_ClientEventLog()
    {
        eCmdType = LC_CMD_ClientEventLog;
    }
};

struct PG_LCBase_AtoLC_ServerItemLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::ServerItemLog table;
    PG_LCBase_AtoLC_ServerItemLog()
    {
        eCmdType = LC_CMD_ServerItemLog;
    }
};

struct PG_LCBase_AtoLC_ProxyPingLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::ProxyPingLog table;
    PG_LCBase_AtoLC_ProxyPingLog()
    {
        eCmdType = LC_CMD_ProxyPingLog;
    }
};

struct PG_LCBase_AtoLC_ExpLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::ExpLog table;
    PG_LCBase_AtoLC_ExpLog()
    {
        eCmdType = LC_CMD_ExpLog;
    }
};

struct PG_LCBase_AtoLC_GuildContributionLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::GuildContributionLog table;
    PG_LCBase_AtoLC_GuildContributionLog()
    {
        eCmdType = LC_CMD_GuildContributionLog;
    }
};

struct PG_LCBase_AtoLC_SkillLvLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::SkillLvLog table;
    PG_LCBase_AtoLC_SkillLvLog()
    {
        eCmdType = LC_CMD_SkillLvLog;
    }
};

struct PG_LCBase_AtoLC_GameLoginLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::GameLoginLog table;
    PG_LCBase_AtoLC_GameLoginLog()
    {
        eCmdType = LC_CMD_GameLoginLog;
    }
};

struct PG_LCBase_AtoLC_TalkLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::TalkLog table;
    PG_LCBase_AtoLC_TalkLog()
    {
        eCmdType = LC_CMD_TalkLog;
    }
};

struct PG_LCBase_AtoLC_Warning_MoneyVendor : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::Warning_MoneyVendor table;
    PG_LCBase_AtoLC_Warning_MoneyVendor()
    {
        eCmdType = LC_CMD_Warning_MoneyVendor;
    }
};

struct PG_LCBase_AtoLC_BonusMoneyLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::BonusMoneyLog table;
    PG_LCBase_AtoLC_BonusMoneyLog()
    {
        eCmdType = LC_CMD_BonusMoneyLog;
    }
};

struct PG_LCBase_AtoLC_Warning_Bot : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::Warning_Bot table;
    PG_LCBase_AtoLC_Warning_Bot()
    {
        eCmdType = LC_CMD_Warning_Bot;
    }
};

struct PG_LCBase_AtoLC_GuildLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::GuildLog table;
    PG_LCBase_AtoLC_GuildLog()
    {
        eCmdType = LC_CMD_GuildLog;
    }
};

struct PG_LCBase_AtoLC_PlayerPlayHistory : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::PlayerPlayHistory table;
    PG_LCBase_AtoLC_PlayerPlayHistory()
    {
        eCmdType = LC_CMD_PlayerPlayHistory;
    }
};

struct PG_LCBase_AtoLC_DepartmentStoreLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::DepartmentStoreLog table;
    PG_LCBase_AtoLC_DepartmentStoreLog()
    {
        eCmdType = LC_CMD_DepartmentStoreLog;
    }
};

struct PG_LCBase_AtoLC_LevelLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::LevelLog table;
    PG_LCBase_AtoLC_LevelLog()
    {
        eCmdType = LC_CMD_LevelLog;
    }
};

struct PG_LCBase_AtoLC_GMToolsCommandLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::GMToolsCommandLog table;
    PG_LCBase_AtoLC_GMToolsCommandLog()
    {
        eCmdType = LC_CMD_GMToolsCommandLog;
    }
};

struct PG_LCBase_AtoLC_DebugMessage : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::DebugMessage table;
    PG_LCBase_AtoLC_DebugMessage()
    {
        eCmdType = LC_CMD_DebugMessage;
    }
};

struct PG_LCBase_AtoLC_Version : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::Version table;
    PG_LCBase_AtoLC_Version()
    {
        eCmdType = LC_CMD_Version;
    }
};

struct PG_LCBase_AtoLC_LocalServerStatus : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::LocalServerStatus table;
    PG_LCBase_AtoLC_LocalServerStatus()
    {
        eCmdType = LC_CMD_LocalServerStatus;
    }
};

struct PG_LCBase_AtoLC_MasterServerStatus : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::MasterServerStatus table;
    PG_LCBase_AtoLC_MasterServerStatus()
    {
        eCmdType = LC_CMD_MasterServerStatus;
    }
};

struct PG_LCBase_AtoLC_MoneyLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::MoneyLog table;
    PG_LCBase_AtoLC_MoneyLog()
    {
        eCmdType = LC_CMD_MoneyLog;
    }
};

struct PG_LCBase_AtoLC_MonsterDead : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::MonsterDead table;
    PG_LCBase_AtoLC_MonsterDead()
    {
        eCmdType = LC_CMD_MonsterDead;
    }
};

struct PG_LCBase_AtoLC_GMCommandLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::GMCommandLog table;
    PG_LCBase_AtoLC_GMCommandLog()
    {
        eCmdType = LC_CMD_GMCommandLog;
    }
};

struct PG_LCBase_AtoLC_PlayerActionLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::PlayerActionLog table;
    PG_LCBase_AtoLC_PlayerActionLog()
    {
        eCmdType = LC_CMD_PlayerActionLog;
    }
};

struct PG_LCBase_AtoLC_ItemTradeLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::ItemTradeLog table;
    PG_LCBase_AtoLC_ItemTradeLog()
    {
        eCmdType = LC_CMD_ItemTradeLog;
    }
};

struct PG_LCBase_AtoLC_ReadWriteTestTable : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::ReadWriteTestTable table;
    PG_LCBase_AtoLC_ReadWriteTestTable()
    {
        eCmdType = LC_CMD_ReadWriteTestTable;
    }
};

struct PG_LCBase_AtoLC_AcLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::AcLog table;
    PG_LCBase_AtoLC_AcLog()
    {
        eCmdType = LC_CMD_AcLog;
    }
};

struct PG_LCBase_AtoLC_PlayerDead : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::PlayerDead table;
    PG_LCBase_AtoLC_PlayerDead()
    {
        eCmdType = LC_CMD_PlayerDead;
    }
};

struct PG_LCBase_AtoLC_AccountMoneyLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::AccountMoneyLog table;
    PG_LCBase_AtoLC_AccountMoneyLog()
    {
        eCmdType = LC_CMD_AccountMoneyLog;
    }
};

struct PG_LCBase_AtoLC_ClientStatus : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::ClientStatus table;
    PG_LCBase_AtoLC_ClientStatus()
    {
        eCmdType = LC_CMD_ClientStatus;
    }
};

struct PG_LCBase_AtoLC_HouseLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::HouseLog table;
    PG_LCBase_AtoLC_HouseLog()
    {
        eCmdType = LC_CMD_HouseLog;
    }
};

struct PG_LCBase_AtoLC_GameGuardReport : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::GameGuardReport table;
    PG_LCBase_AtoLC_GameGuardReport()
    {
        eCmdType = LC_CMD_GameGuardReport;
    }
};

struct PG_LCBase_AtoLC_DataCenterServerStatus : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::DataCenterServerStatus table;
    PG_LCBase_AtoLC_DataCenterServerStatus()
    {
        eCmdType = LC_CMD_DataCenterServerStatus;
    }
};

struct PG_LCBase_AtoLC_ClientPingLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::ClientPingLog table;
    PG_LCBase_AtoLC_ClientPingLog()
    {
        eCmdType = LC_CMD_ClientPingLog;
    }
};

struct PG_LCBase_AtoLC_QuestLog : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::QuestLog table;
    PG_LCBase_AtoLC_QuestLog()
    {
        eCmdType = LC_CMD_QuestLog;
    }
};

struct PG_LCBase_AtoLC_AgencyCCU : public PG_LCBase_AtoLC_PackageBase
{
    DataClassLayer::AgencyCCU table;
    PG_LCBase_AtoLC_AgencyCCU()
    {
        eCmdType = LC_CMD_AgencyCCU;
    }
};

#endif

#endif // LOG_CENTER_BASE_PACKAGE_DEFINE_AUTO_H
