#pragma once
#ifndef	__AURORA_AGENT_PACKET_DEF_H__
#define __AURORA_AGENT_PACKET_DEF_H__

#include <memory.h>

#define MAX_AA_ACCOUNT_NAME_SIZE			64
#define MAX_AA_PASSWORD_SIZE				256
#define MAX_AA_MAC_ADDRESS_SIZE				256
#define MAX_AA_ITEM_NAME_SIZE				64
#define MAX_AA_NAMERECORD_SIZE				32
#define MAX_LUA_FUNTION_NAME_SIZE			32
// ************************************************************************************************************************************************************
enum AuroraAngetPacketType
{
	EM_PG_AURORA_AGENT_BASE							= 0,			//default packet
	EM_PG_AURORA_AGENT_ACtoAA_LOGIN					= 100,			//account -> aurora agent login packet	
	EM_PG_AURORA_AGENT_ACtoAA_LOGOUT				= 101,			//account -> aurora agent logout packet
	EM_PG_AURORA_AGENT_SLtoAA_CHECK_ACCOUNT			= 102,			//serverlist -> aurora agent check account(the same as login)

	EM_PG_AURORA_AGENT_DCtoAA_LOG_DATA				= 200,			//datacenter -> aurora agent log data

	EM_PG_AURORA_AGENT_AAtoAC_LOGIN_RESULT			= 300,			//aurora agent -> account login result
	EM_PG_AURORA_AGENT_AAtoAC_LOGOUT_RESULT			= 301,			//aurora agent -> account logout result
	EM_PG_AURORA_AGENT_AAtoSL_CHECK_ACCOUNT_RESULT	= 302,			//aurora agent -> serverlist check account result

	EM_PG_AURORA_AGENT_DCtoAA_ROLE_LEAVE_WORLD		= 400,			//datacenter -> aurora agent part of role data -> KR middle DB

	EM_PG_AURORA_AGENT_DCtoAA_QUERY_PC_BENEFIT		= 500,
	EM_PG_AURORA_AGENT_AAtoDC_PC_BENEFIT_RESULT		= 501,

	EM_PG_AURORA_AGENT_FORCE_DWORD					= 0x7FFFFFFF
};

//登入結果
enum AuroraAngetLoginResult
{
	EM_AA_LOGIN_RESULT_UNKNOW								= -1,
	EM_AA_LOGIN_RESULT_SUCCESS								= 100,				//Login Successful
	EM_AA_LOGIN_RESULT_FREEZE_OTHER4						= 900,				//Freeze other 4
	EM_AA_LOGIN_RESULT_ID_OR_PASSWORD_MISMATCH				= 911,				//Your ID or password does not match
	EM_AA_LOGIN_RESULT_AGE_RESTRICTION						= 913,				//Your age is restricted from the service.
	EM_AA_LOGIN_RESULT_TEAM_AND_POLICY_AGREEMENT_NOT_MET	= 914,				//Please click on “I have read and agree to the Terms & Policy“
	EM_AA_LOGIN_RESULT_ACCESS_DENIED_IP						= 921,				//This IP’s access is denied
	EM_AA_LOGIN_RESULT_ACCESS_DENIED_ID						= 922,				//Your ID is restricted due to [reason]	
	EM_AA_LOGIN_RESULT_FORCE_DWORD							= 0x7FFFFFFF
};

//登出結果
enum AuroraAngetLogoutResult
{
	EM_AA_LOGOUT_RESULT_OK     = 0,
	EM_AA_LOGOUT_RESULT_FORCE_DWORD = 0x7FFFFFFF
};

//
enum AuroraAngetShopType
{
	EM_SHOP_TYPE_NONE			,
	EM_SHOP_TYPE_DIAOND_MALL	,
	EM_SHOP_TYPE_RUBY_MALL		,
	EM_SHOP_TYPE_PHIRIUS_MALL
};

//貨幣種類
enum AuroraAngetCurrencyType
{
	EM_AA_CURRENCY_ACTUAL	= 0,
	EM_AA_CURRENCY_VIRTUAL	= 1
};

enum AuroraAngetCardType
{
	EM_CARD_TYPE_NONE	= 0,
	EM_CARD_TYPE_R		= 1,
	EM_CARD_TYPE_O		= 2,
	EM_CARD_TYPE_M		= 3
};


// ************************************************************************************************************************************************************

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
struct PG_AURORA_AGENT_BASE
{
	int iType;
	PG_AURORA_AGENT_BASE()
	{
		iType = EM_PG_AURORA_AGENT_BASE;
	}
};

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
struct PG_AURORA_AGENT_ACtoAA_LOGIN : public PG_AURORA_AGENT_BASE
{
	char			szAccount[MAX_AA_ACCOUNT_NAME_SIZE];
	char			szPassword[MAX_AA_PASSWORD_SIZE];
	unsigned long	dwIP;

	PG_AURORA_AGENT_ACtoAA_LOGIN()
	{
		memset(this, 0, sizeof(*this));
		iType = EM_PG_AURORA_AGENT_ACtoAA_LOGIN;
	}
};

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
struct PG_AURORA_AGENT_ACtoAA_LOGOUT : public PG_AURORA_AGENT_BASE
{
	char szAccount[MAX_AA_ACCOUNT_NAME_SIZE];

	PG_AURORA_AGENT_ACtoAA_LOGOUT()
	{
		memset(this, 0, sizeof(*this));
		iType = EM_PG_AURORA_AGENT_ACtoAA_LOGOUT;
	}
};

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
struct PG_AURORA_AGENT_SLtoAA_CHECK_ACCOUNT : public PG_AURORA_AGENT_BASE
{
	char			szAccount[MAX_AA_ACCOUNT_NAME_SIZE];
	char			szPassword[MAX_AA_PASSWORD_SIZE];
	unsigned long	dwIP;

	PG_AURORA_AGENT_SLtoAA_CHECK_ACCOUNT()
	{
		memset(this, 0, sizeof(*this));
		iType = EM_PG_AURORA_AGENT_SLtoAA_CHECK_ACCOUNT;
	}
};

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
struct PG_AURORA_AGENT_AAtoAC_LOGIN_RESULT : public PG_AURORA_AGENT_BASE
{	
	char szAccount[MAX_AA_ACCOUNT_NAME_SIZE];
	int	 iResult;

	PG_AURORA_AGENT_AAtoAC_LOGIN_RESULT()
	{
		memset(this, 0, sizeof(*this));
		iType = EM_PG_AURORA_AGENT_AAtoAC_LOGIN_RESULT;
	}
};

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
struct PG_AURORA_AGENT_AAtoAC_LOGOUT_RESULT : public PG_AURORA_AGENT_BASE
{		
	char szAccount[MAX_AA_ACCOUNT_NAME_SIZE];
	int	 iResult;

	PG_AURORA_AGENT_AAtoAC_LOGOUT_RESULT()
	{
		memset(this, 0, sizeof(*this));
		iType = EM_PG_AURORA_AGENT_AAtoAC_LOGOUT_RESULT;
	}
};

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
struct PG_AURORA_AGENT_AAtoSL_CHECK_ACCOUNT_RESULT : public PG_AURORA_AGENT_BASE
{	
	char szAccount[MAX_AA_ACCOUNT_NAME_SIZE];
	char szPassword[MAX_AA_PASSWORD_SIZE];
	int	 iAge;
	int	 iResult;	

	PG_AURORA_AGENT_AAtoSL_CHECK_ACCOUNT_RESULT()
	{
		memset(this, 0, sizeof(*this));
		iType = EM_PG_AURORA_AGENT_AAtoSL_CHECK_ACCOUNT_RESULT;
	}
};

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
struct PG_AURORA_AGENT_DCtoAA_LOG_DATA : public PG_AURORA_AGENT_BASE
{
	int			  iLogType;
	unsigned long dwDataSize;

	PG_AURORA_AGENT_DCtoAA_LOG_DATA()
	{
		memset(this, 0, sizeof(*this));
		iType = EM_PG_AURORA_AGENT_DCtoAA_LOG_DATA;
	}
};

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
struct PG_AURORA_AGENT_DCtoAA_ROLE_LEAVE_WORLD : public PG_AURORA_AGENT_BASE
{
	char szAccountName[MAX_AA_ACCOUNT_NAME_SIZE];
	char szRoleName[MAX_AA_NAMERECORD_SIZE];
	int iRoleDBID;
	int	iWorldID;
	int	iZoneID;
	int	iRace;
	int	iVoc;
	int	iVoc_Sub;
	int	iSex;
	int	iLV;
	int	iLV_Sub;	

	PG_AURORA_AGENT_DCtoAA_ROLE_LEAVE_WORLD()
	{
		memset(this, 0, sizeof(*this));
		iType = EM_PG_AURORA_AGENT_DCtoAA_ROLE_LEAVE_WORLD;
	}
};

//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
struct PG_AURORA_AGENT_DCtoAA_QUERY_PC_BENEFIT : public PG_AURORA_AGENT_BASE
{
	char szAccount[MAX_AA_ACCOUNT_NAME_SIZE];
	int iDBID;
	int iZoneID;

	PG_AURORA_AGENT_DCtoAA_QUERY_PC_BENEFIT()
	{
		memset(this, 0, sizeof(*this));
		iType = EM_PG_AURORA_AGENT_DCtoAA_QUERY_PC_BENEFIT;
	}
};
//---------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------
struct PG_AURORA_AGENT_AAtoDC_PC_BENEFIT_RESULT : public PG_AURORA_AGENT_BASE
{
	char szLuaPlot[MAX_LUA_FUNTION_NAME_SIZE];
	int iPCBenefit;
	int iDBID;
	int iZoneID;

	PG_AURORA_AGENT_AAtoDC_PC_BENEFIT_RESULT()
	{
		memset(this, 0, sizeof(*this));
		iType = EM_PG_AURORA_AGENT_AAtoDC_PC_BENEFIT_RESULT;
	}
};

#endif //__AURORA_AGENT_PACKET_DEF_H__