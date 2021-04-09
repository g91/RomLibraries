#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

//-----------------------------------------------------------------------------------------
//		寄件
//-----------------------------------------------------------------------------------------
//要求寄件		
//Client -> Local
struct PG_Mail_CtoL_SendMail
{
	GamePGCommandEnum	Command;

	MailBaseInfoStruct		MailBaseInfo;
	int						ItemPos[10];	
	MailBackGroundTypeENUM	BackGroundType;
	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

	int 					ContentSize;
	StaticString <2048>		Content;

	PG_Mail_CtoL_SendMail( )
	{	Command = EM_PG_Mail_CtoL_SendMail;	}

	int	PGSize()
	{
		return sizeof(*this)-sizeof(Content)+ContentSize;
	}
	static int BasePGSize()
	{
		return sizeof( PG_Mail_CtoL_SendMail) - 2048;
	}
};
//-----------------------------------------------------------------------------------------
//要求寄件結果		
//Local -> Client
enum	SendMailResultEnum
{
	EM_SendMailResult_OK	= 0			,
	EM_SendMailResult_Unknow			,
	EM_SendMailResult_MoneyError		,
	EM_SendMailResult_ItemError			,
	EM_SendMailResult_TargetNotFind		,
	EM_SendMailResult_TargetMailFull	,
	EM_SendMailResult_OwnerNameError	,
	EM_SendMailResult_AccountMoneyError	,
	EM_SendMailResult_ItemLock			,	//物品密碼鎖，不能用
	EM_SendMailResult_PasswordError		,
	EM_SendMailResult_BonusMoneyError	,
	EM_SendMailResult_SendTooOften		,


};
struct PG_Mail_LtoC_SendMailResult
{
	GamePGCommandEnum	Command;
	SendMailResultEnum	Result;
	StaticString <_MAX_NAMERECORD_SIZE_>	TargetName;

	PG_Mail_LtoC_SendMailResult( )
	{	Command = EM_PG_Mail_LtoC_SendMailResult;	}

};
//-----------------------------------------------------------------------------------------
//撿查是否可以寄信送給此玩家
//Local -> DataCenter
struct PG_Mail_LtoD_CheckSendMail
{
	GamePGCommandEnum	Command;

	int										PlayerDBID;	//要求者的DBID
	StaticString <_MAX_NAMERECORD_SIZE_>	TargetName;
	int										MapKey;


	PG_Mail_LtoD_CheckSendMail( )
	{	Command = EM_PG_Mail_LtoD_CheckSendMail;	}

};
//-----------------------------------------------------------------------------------------
//撿查是否可以寄信送給此玩家
// DataCenter -> Local
struct PG_Mail_DtoL_CheckSendMailResult
{
	GamePGCommandEnum	Command;

	int										PlayerDBID;	//要求者的DBID
	StaticString <_MAX_NAMERECORD_SIZE_>	TargetName;
	int										TargetDBID;
	int										MapKey;
	bool									Result;
	int										ErrorCode;

	PG_Mail_DtoL_CheckSendMailResult( )
	{	Command = EM_PG_Mail_DtoL_CheckSendMailResult;	}

};

//-----------------------------------------------------------------------------------------
//要求寄件		
//Local -> DataCenter
struct PG_Mail_LtoD_SendMail
{
	GamePGCommandEnum	Command;

	int					DBID;		//要求者的DBID
	int					TargetDBID;	//要求送給的目標DBID ( MailBaseInfo.OrgTargetName 沒資料時，則以TargetDBID找出目標名字)

	MailBaseInfoStruct  MailBaseInfo;

	int 					ContentSize;
	StaticString <2048>		Content;

	PG_Mail_LtoD_SendMail( )
	{	Command = EM_PG_Mail_LtoD_SendMail;	}

	int	PGSize()
	{
		return sizeof(*this)-sizeof(Content)+ContentSize;
	}
};
//-----------------------------------------------------------------------------------------
//要求寄件結果
//DC -> Local
struct PG_Mail_DtoL_SendMailResult
{
	GamePGCommandEnum	Command;

	int					DBID;				//要求者的DBID

	//	ItemFieldStruct		Item;				//寄送物品

	int					Money;				
	int					Money_Account;		//(帳號幣)
	int					Money_Bonus;
	bool				IsSendMoney;		//true 寄送金錢 false 獲到付款
	bool				IsSystem;			//是否為系統信件
	StaticString <_MAX_NAMERECORD_SIZE_>	TargetName;

	SendMailResultEnum	Result;

	PG_Mail_DtoL_SendMailResult( )
	{	Command = EM_PG_Mail_DtoL_SendMailResult;	}

};
//-----------------------------------------------------------------------------------------
//		收信
//-----------------------------------------------------------------------------------------
//要求郵件列表 
//Client -> Local
struct PG_Mail_CtoL_MailList
{
	GamePGCommandEnum	Command;

	PG_Mail_CtoL_MailList( )
	{	Command = EM_PG_Mail_CtoL_MailList;	}
};
//-----------------------------------------------------------------------------------------
//要求郵件列表
//Local -> DataCenter
struct PG_Mail_LtoD_MailList
{
	GamePGCommandEnum	Command;
	int					DBID;		//要求者的DBID
	int					CliSockID;	//用來直接傳資料給Client
	int					CliProxyID;

	PG_Mail_LtoD_MailList( )
	{	Command = EM_PG_Mail_LtoD_MailList;	}
};
//-----------------------------------------------------------------------------------------
//通知信件總數
//DataCenter -> Client
struct PG_Mail_DtoC_MailListCountResult
{
	GamePGCommandEnum	Command;

	int					MaxCount;	//總量
	int					Count;		//要送出的量



	PG_Mail_DtoC_MailListCountResult( )
	{	Command = EM_PG_Mail_DtoC_MailListCountResult;	}
};
//-----------------------------------------------------------------------------------------
//要求信件列表結果
//DataCenter -> Client
struct PG_Mail_DtoC_MailListResult
{
	GamePGCommandEnum	Command;

	int					MailDBID;	//信件的DBID
	int					Index;		//地幾筆

	MailBaseInfoStruct	MailBaseInfo;
	int					ContentSize;
	StaticString <2048>	Content;

	PG_Mail_DtoC_MailListResult( )
	{	Command = EM_PG_Mail_DtoC_MailListResult;	}

	int	PGSize()
	{
		return sizeof(*this)-sizeof(Content)+ContentSize;
	}
};
//-----------------------------------------------------------------------------------------
//退信
//Client -> Local
struct PG_Mail_CtoL_SendBack
{
	GamePGCommandEnum	Command;

	int		MailDBID;	//信件的DBID

	PG_Mail_CtoL_SendBack( )
	{	Command = EM_PG_Mail_CtoL_SendBack;	}
};
//-----------------------------------------------------------------------------------------
//退信
//Local -> DataCenter
struct PG_Mail_LtoD_SendBack
{
	GamePGCommandEnum	Command;

	int		MailDBID;	//信件的DBID
	int		DBID;		//擁有者

	PG_Mail_LtoD_SendBack( )
	{	Command = EM_PG_Mail_LtoD_SendBack;	}
};
//-----------------------------------------------------------------------------------------
// 刪信
//-----------------------------------------------------------------------------------------
//刪信
//Client -> Local
struct PG_Mail_CtoL_DelMail
{
	GamePGCommandEnum	Command;

	int		MailDBID;	//信件的DBID

	PG_Mail_CtoL_DelMail( )
	{	Command = EM_PG_Mail_CtoL_DelMail;	}
};
//-----------------------------------------------------------------------------------------
//刪信
//Local -> DataCenter
struct PG_Mail_LtoD_DelMail
{
	GamePGCommandEnum	Command;

	int		MailDBID;	//信件的DBID
	int		DBID;		//擁有者

	PG_Mail_LtoD_DelMail( )
	{	Command = EM_PG_Mail_LtoD_DelMail;	}
};
//-----------------------------------------------------------------------------------------
//	閱讀
//-----------------------------------------------------------------------------------------
//閱讀要求
//Client -> Local
struct PG_Mail_CtoL_ReadMail
{
	GamePGCommandEnum	Command;

	int		MailDBID;	//信件的DBID

	PG_Mail_CtoL_ReadMail( )
	{	Command = EM_PG_Mail_CtoL_ReadMail;	}
};
//-----------------------------------------------------------------------------------------
//閱讀要求
//Local -> DataCenter
struct PG_Mail_LtoD_ReadMail
{
	GamePGCommandEnum	Command;

	int		MailDBID;	//信件的DBID
	int		DBID;		//擁有者
	//	int		CliSockID;	//用來直接傳資料給Client
	//	int		CliProxyID;

	PG_Mail_LtoD_ReadMail( )
	{	Command = EM_PG_Mail_LtoD_ReadMail;	}
};
//-----------------------------------------------------------------------------------------
//閱讀結果
//DataCenter -> Client
/*
struct PG_Mail_DtoC_ReadMailResult
{
GamePGCommandEnum	Command;

int					MailDBID;	//信件的DBID
bool				Result;

MailBaseInfoStruct  MailBaseInfo;

int 					ContentSize;
StaticString <2048>		Content;


PG_Mail_DtoC_ReadMailResult( )
{	Command = EM_PG_Mail_DtoC_ReadMailResult;	}

int	PGSize()
{
return sizeof(*this)-sizeof(Content)+ContentSize;
}
};
*/
//-----------------------------------------------------------------------------------------
//	取物品與金錢
//-----------------------------------------------------------------------------------------
//取物品與金錢
//Client -> Local	
struct PG_Mail_CtoL_GetItem
{
	GamePGCommandEnum	Command;

	int		MailDBID;		//信件的DBID (一定要最後一次讀取的信件)
	int		Money;			//(遊戲幣)	+為獲得 -為付出	
	int		Money_Account;	//(帳號幣)
	int		Money_Bonus;
	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

	PG_Mail_CtoL_GetItem( )
	{	Command = EM_PG_Mail_CtoL_GetItem;	}
};
//-----------------------------------------------------------------------------------------
//檢查是否可以取物品
//Local	-> DataCenter
struct PG_Mail_LtoD_CheckGetItemRequest
{
	GamePGCommandEnum	Command;

	int		DBID;			//擁有者的DBID
	int		MailDBID;		//信件的DBID (一定要最後一次讀取的信件)
	int		Money;			//+為獲得 -為付出
	int		Money_Account;	//(帳號幣)
	int		Moneu_Bonus;

	PG_Mail_LtoD_CheckGetItemRequest( )
	{	Command = EM_PG_Mail_LtoD_CheckGetItemRequest;	}
};
//-----------------------------------------------------------------------------------------
//檢查是否可以取物品
//DataCenter -> Local	
struct PG_Mail_DtoL_CheckGetItemResult
{
	GamePGCommandEnum	Command;

	int		DBID;			//擁有者的DBID
	int		TargetDBID;		//原始寄信者
	int		MailDBID;		//信件的DBID (一定要最後一次讀取的信件)
	int		Money;			//+為獲得 -為付出
	int		Money_Account;	//(帳號幣)
	int		Money_Bonus;
	bool	Result;

	PG_Mail_DtoL_CheckGetItemResult( )
	{	Command = EM_PG_Mail_DtoL_CheckGetItemResult;	}
};
//-----------------------------------------------------------------------------------------
//取物品與金錢
//要求 DBCenter 更新資料庫的資訊
//Local	-> DataCenter
struct PG_Mail_LtoD_GetItem
{
	GamePGCommandEnum	Command;

	int		DBID;		//擁有者的DBID
	int		MailDBID;	//信件的DBID (一定要最後一次讀取的信件)
	int		Money;		//+為獲得 -為付出
	int		Money_Account;	//(帳號幣)
	int		Money_Bonus;

	PG_Mail_LtoD_GetItem( )
	{	Command = EM_PG_Mail_LtoD_GetItem;	}
};
//-----------------------------------------------------------------------------------------
//取物品與金錢 結果
//DataCenter -> Local
struct PG_Mail_DtoL_GetItemResult
{
	GamePGCommandEnum	Command;
	int		DBID;
	int		OrgSenderDBID;
	int		MailDBID;	//信件的DBID (一定要最後一次讀取的信件)
	bool	Result;
	int		Money;		//金錢 ( + 獲得  - 付出 )
	int		Money_Account;	//(帳號幣)
	int		Money_Bonus;
	ItemFieldStruct		Item[10];		//寄送物品


	PG_Mail_DtoL_GetItemResult( )
	{	Command = EM_PG_Mail_DtoL_GetItemResult;	}
};
//-----------------------------------------------------------------------------------------
//取物品與金錢 結果
//Local -> Client
struct PG_Mail_LtoC_GetItemResult
{
	GamePGCommandEnum	Command;

	int		MailDBID;	//信件的DBID (一定要最後一次讀取的信件)
	bool	Result;


	PG_Mail_LtoC_GetItemResult( )
	{	Command = EM_PG_Mail_LtoC_GetItemResult;	}
};
//-----------------------------------------------------------------------------------------
//開啟 Mail
//Local -> Client
struct PG_Mail_LtoC_OpenMail
{
	GamePGCommandEnum	Command;
	int		NpcGUID;				//郵筒ID

	PG_Mail_LtoC_OpenMail( )
	{	Command = EM_PG_Mail_LtoC_OpenMail;	}
};
//-----------------------------------------------------------------------------------------
//要求關閉 Mail
//Local -> Client
struct PG_Mail_LtoC_CloseMail
{
	GamePGCommandEnum	Command;

	PG_Mail_LtoC_CloseMail( )
	{	Command = EM_PG_Mail_LtoC_CloseMail;	}
};
//-----------------------------------------------------------------------------------------
//Client通知已關閉 Mail
//Local -> Client
struct PG_Mail_CtoL_CloseMail
{
	GamePGCommandEnum	Command;

	PG_Mail_CtoL_CloseMail( )
	{	Command = EM_PG_Mail_CtoL_CloseMail;	}
};
//-----------------------------------------------------------------------------------------
//要求通知尚未讀取的郵件量
//Local -> DataCenter
struct PG_Mail_LtoD_UnReadMailCountRequest
{
	GamePGCommandEnum	Command;
	int					DBID;		//要求者的DBID
	int					CliSockID;	//用來直接傳資料給Client
	int					CliProxyID;

	PG_Mail_LtoD_UnReadMailCountRequest( )
	{	Command = EM_PG_Mail_LtoD_UnReadMailCountRequest;	}
};
//-----------------------------------------------------------------------------------------
//尚未讀取的郵件數量
//DataCenter -> Client 
struct PG_Mail_DtoC_UnReadMailCount
{
	GamePGCommandEnum	Command;
	int		Count;
	int		GiftCount;

	PG_Mail_DtoC_UnReadMailCount( )
	{	Command = EM_PG_Mail_DtoC_UnReadMailCount;	}
};
//-----------------------------------------------------------------------------------------
//通知有新信件
//Local -> Client
struct PG_Mail_DtoC_NewMail
{
	GamePGCommandEnum	Command;
	bool IsGift;

	PG_Mail_DtoC_NewMail( )
	{	Command = EM_PG_Mail_DtoC_NewMail;	}
};
//-----------------------------------------------------------------------------------------
//要求寄件(多人)		
//Client -> Local
struct PG_Mail_CtoL_SendMailList
{
	GamePGCommandEnum	Command;

	int										TargetCount;
	StaticString <_MAX_NAMERECORD_SIZE_>	OrgTargetName[20];
	StaticString <_MAX_NAMERECORD_SIZE_>	Title;

	MailBackGroundTypeENUM					PaperType;
	int 									ContentSize;
	StaticString <2048>						Content;

	PG_Mail_CtoL_SendMailList( )
	{	Command = EM_PG_Mail_CtoL_SendMailList;	}

	int	PGSize()
	{
		return sizeof(*this)-sizeof(Content)+ContentSize;
	}
	static int BasePGSize()
	{
		return sizeof( PG_Mail_CtoL_SendMailList ) - 2048;
	}
};
//-----------------------------------------------------------------------------------------
#pragma managed

