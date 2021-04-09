#ifndef __PG_BILLING_H__
#define __PG_BILLING_H__
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

#define BillingResultCB	boost::function<void (void* userdata)>

//----------------------------------------------------------------
enum BillingResultENUM
{
	EM_BillingResult_Unknow	= -1,
	EM_BillingResult_OK		,
	EM_BillingResult_Failed	,
};

//----------------------------------------------------------------
enum BillingSystemStatusENUM
{
	EM_BillingSystemStatus_Offline,
	EM_BillingSystemStatus_Online ,
};

//----------------------------------------------------------------
//提領帳號幣結果資料
struct BillingResultData
{
	StaticString <_MAX_ACCOUNT_SIZE_> Account;
	int								  PlayerDBID;
	int								  Balance;
	int								  Result;
	int								  CustomID;

	BillingResultData()
	{
		memset(this, 0 ,sizeof(*this));
	}
};

//----------------------------------------------------------------
//Local -> Billing查詢某帳號的帳號幣餘額
struct PG_Billing_LtoB_BalanceCheck
{
	GamePGCommandEnum Command;

	StaticString <_MAX_ACCOUNT_SIZE_> Account;
	int								  PlayerDBID;
	int								  CustomID;
	int								  CBID;

	PG_Billing_LtoB_BalanceCheck()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_Billing_LtoB_BalanceCheck;
	}
};

//----------------------------------------------------------------
//Local -> Billing查詢金流系統狀態
struct PG_Billing_LtoB_RequestBillingSystemStatus
{
	GamePGCommandEnum Command;

	PG_Billing_LtoB_RequestBillingSystemStatus()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_Billing_LtoB_RequestBillingSystemStatus;
	}
};

//----------------------------------------------------------------
//Local -> Billing增減某帳號的帳號幣餘額
struct PG_Billing_LtoB_AddAccountMoney
{
	GamePGCommandEnum Command;

	StaticString <_MAX_ACCOUNT_SIZE_> Account;
	int								  PlayerDBID;
	int								  Amount;
	int								  CustomID;
	int								  CBID;

	PG_Billing_LtoB_AddAccountMoney()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_Billing_LtoB_AddAccountMoney;
	}
};

//----------------------------------------------------------------
//Billing -> Local查詢帳號幣餘額結果
struct PG_Billing_BtoL_BalanceCheckResult
{
	GamePGCommandEnum Command;

	StaticString <_MAX_ACCOUNT_SIZE_> Account;
	int								  PlayerDBID;
	int								  Balance;
	int								  Result;
	int								  CustomID;
	int								  CBID;

	PG_Billing_BtoL_BalanceCheckResult()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_Billing_BtoL_BalanceCheckResult;
	}
};

//----------------------------------------------------------------
//Billing -> Local增減某帳號的帳號幣結果
struct PG_Billing_BtoL_AddAccountMoneyResult
{
	GamePGCommandEnum Command;

	StaticString <_MAX_ACCOUNT_SIZE_> Account;
	int								  PlayerDBID;
	int								  Balance;
	int								  Result;
	int								  CustomID;
	int								  CBID;

	PG_Billing_BtoL_AddAccountMoneyResult()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_Billing_BtoL_AddAccountMoneyResult;
	}
};

//----------------------------------------------------------------
//Billing -> Local金流系統狀態
struct PG_Billing_BtoL_BillingSystemStatus
{
	GamePGCommandEnum Command;

	int Status;

	PG_Billing_BtoL_BillingSystemStatus()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_Billing_BtoL_BillingSystemStatus;
	}
};

//----------------------------------------------------------------
//Local -> Billing更新紅利幣及必爾汀餘額
struct PG_Billing_LtoB_MoneyInfo
{
	GamePGCommandEnum Command;

	StaticString <_MAX_ACCOUNT_SIZE_> Account;	
	int								  PlayerDBID;
	int								  WorldID;
	int								  BonusMoney;
	int								  PhiriusCoin;

	PG_Billing_LtoB_MoneyInfo()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_Billing_LtoB_MoneyInfo;
	}
};

#pragma managed
#endif //__PG_BILLING_H__