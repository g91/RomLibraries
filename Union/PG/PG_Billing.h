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
//����b�������G���
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
//Local -> Billing�d�߬Y�b�����b�����l�B
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
//Local -> Billing�d�ߪ��y�t�Ϊ��A
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
//Local -> Billing�W��Y�b�����b�����l�B
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
//Billing -> Local�d�߱b�����l�B���G
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
//Billing -> Local�W��Y�b�����b�������G
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
//Billing -> Local���y�t�Ϊ��A
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
//Local -> Billing��s���Q���Υ����žl�B
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