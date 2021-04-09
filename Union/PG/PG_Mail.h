#pragma once
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

//-----------------------------------------------------------------------------------------
//		�H��
//-----------------------------------------------------------------------------------------
//�n�D�H��		
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
//�n�D�H�󵲪G		
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
	EM_SendMailResult_ItemLock			,	//���~�K�X��A�����
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
//�߬d�O�_�i�H�H�H�e�������a
//Local -> DataCenter
struct PG_Mail_LtoD_CheckSendMail
{
	GamePGCommandEnum	Command;

	int										PlayerDBID;	//�n�D�̪�DBID
	StaticString <_MAX_NAMERECORD_SIZE_>	TargetName;
	int										MapKey;


	PG_Mail_LtoD_CheckSendMail( )
	{	Command = EM_PG_Mail_LtoD_CheckSendMail;	}

};
//-----------------------------------------------------------------------------------------
//�߬d�O�_�i�H�H�H�e�������a
// DataCenter -> Local
struct PG_Mail_DtoL_CheckSendMailResult
{
	GamePGCommandEnum	Command;

	int										PlayerDBID;	//�n�D�̪�DBID
	StaticString <_MAX_NAMERECORD_SIZE_>	TargetName;
	int										TargetDBID;
	int										MapKey;
	bool									Result;
	int										ErrorCode;

	PG_Mail_DtoL_CheckSendMailResult( )
	{	Command = EM_PG_Mail_DtoL_CheckSendMailResult;	}

};

//-----------------------------------------------------------------------------------------
//�n�D�H��		
//Local -> DataCenter
struct PG_Mail_LtoD_SendMail
{
	GamePGCommandEnum	Command;

	int					DBID;		//�n�D�̪�DBID
	int					TargetDBID;	//�n�D�e�����ؼ�DBID ( MailBaseInfo.OrgTargetName �S��ƮɡA�h�HTargetDBID��X�ؼЦW�r)

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
//�n�D�H�󵲪G
//DC -> Local
struct PG_Mail_DtoL_SendMailResult
{
	GamePGCommandEnum	Command;

	int					DBID;				//�n�D�̪�DBID

	//	ItemFieldStruct		Item;				//�H�e���~

	int					Money;				
	int					Money_Account;		//(�b����)
	int					Money_Bonus;
	bool				IsSendMoney;		//true �H�e���� false ���I��
	bool				IsSystem;			//�O�_���t�ΫH��
	StaticString <_MAX_NAMERECORD_SIZE_>	TargetName;

	SendMailResultEnum	Result;

	PG_Mail_DtoL_SendMailResult( )
	{	Command = EM_PG_Mail_DtoL_SendMailResult;	}

};
//-----------------------------------------------------------------------------------------
//		���H
//-----------------------------------------------------------------------------------------
//�n�D�l��C�� 
//Client -> Local
struct PG_Mail_CtoL_MailList
{
	GamePGCommandEnum	Command;

	PG_Mail_CtoL_MailList( )
	{	Command = EM_PG_Mail_CtoL_MailList;	}
};
//-----------------------------------------------------------------------------------------
//�n�D�l��C��
//Local -> DataCenter
struct PG_Mail_LtoD_MailList
{
	GamePGCommandEnum	Command;
	int					DBID;		//�n�D�̪�DBID
	int					CliSockID;	//�ΨӪ����Ǹ�Ƶ�Client
	int					CliProxyID;

	PG_Mail_LtoD_MailList( )
	{	Command = EM_PG_Mail_LtoD_MailList;	}
};
//-----------------------------------------------------------------------------------------
//�q���H���`��
//DataCenter -> Client
struct PG_Mail_DtoC_MailListCountResult
{
	GamePGCommandEnum	Command;

	int					MaxCount;	//�`�q
	int					Count;		//�n�e�X���q



	PG_Mail_DtoC_MailListCountResult( )
	{	Command = EM_PG_Mail_DtoC_MailListCountResult;	}
};
//-----------------------------------------------------------------------------------------
//�n�D�H��C���G
//DataCenter -> Client
struct PG_Mail_DtoC_MailListResult
{
	GamePGCommandEnum	Command;

	int					MailDBID;	//�H��DBID
	int					Index;		//�a�X��

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
//�h�H
//Client -> Local
struct PG_Mail_CtoL_SendBack
{
	GamePGCommandEnum	Command;

	int		MailDBID;	//�H��DBID

	PG_Mail_CtoL_SendBack( )
	{	Command = EM_PG_Mail_CtoL_SendBack;	}
};
//-----------------------------------------------------------------------------------------
//�h�H
//Local -> DataCenter
struct PG_Mail_LtoD_SendBack
{
	GamePGCommandEnum	Command;

	int		MailDBID;	//�H��DBID
	int		DBID;		//�֦���

	PG_Mail_LtoD_SendBack( )
	{	Command = EM_PG_Mail_LtoD_SendBack;	}
};
//-----------------------------------------------------------------------------------------
// �R�H
//-----------------------------------------------------------------------------------------
//�R�H
//Client -> Local
struct PG_Mail_CtoL_DelMail
{
	GamePGCommandEnum	Command;

	int		MailDBID;	//�H��DBID

	PG_Mail_CtoL_DelMail( )
	{	Command = EM_PG_Mail_CtoL_DelMail;	}
};
//-----------------------------------------------------------------------------------------
//�R�H
//Local -> DataCenter
struct PG_Mail_LtoD_DelMail
{
	GamePGCommandEnum	Command;

	int		MailDBID;	//�H��DBID
	int		DBID;		//�֦���

	PG_Mail_LtoD_DelMail( )
	{	Command = EM_PG_Mail_LtoD_DelMail;	}
};
//-----------------------------------------------------------------------------------------
//	�\Ū
//-----------------------------------------------------------------------------------------
//�\Ū�n�D
//Client -> Local
struct PG_Mail_CtoL_ReadMail
{
	GamePGCommandEnum	Command;

	int		MailDBID;	//�H��DBID

	PG_Mail_CtoL_ReadMail( )
	{	Command = EM_PG_Mail_CtoL_ReadMail;	}
};
//-----------------------------------------------------------------------------------------
//�\Ū�n�D
//Local -> DataCenter
struct PG_Mail_LtoD_ReadMail
{
	GamePGCommandEnum	Command;

	int		MailDBID;	//�H��DBID
	int		DBID;		//�֦���
	//	int		CliSockID;	//�ΨӪ����Ǹ�Ƶ�Client
	//	int		CliProxyID;

	PG_Mail_LtoD_ReadMail( )
	{	Command = EM_PG_Mail_LtoD_ReadMail;	}
};
//-----------------------------------------------------------------------------------------
//�\Ū���G
//DataCenter -> Client
/*
struct PG_Mail_DtoC_ReadMailResult
{
GamePGCommandEnum	Command;

int					MailDBID;	//�H��DBID
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
//	�����~�P����
//-----------------------------------------------------------------------------------------
//�����~�P����
//Client -> Local	
struct PG_Mail_CtoL_GetItem
{
	GamePGCommandEnum	Command;

	int		MailDBID;		//�H��DBID (�@�w�n�̫�@��Ū�����H��)
	int		Money;			//(�C����)	+����o -���I�X	
	int		Money_Account;	//(�b����)
	int		Money_Bonus;
	StaticString< _MAX_NAMERECORD_SIZE_ >		Password;

	PG_Mail_CtoL_GetItem( )
	{	Command = EM_PG_Mail_CtoL_GetItem;	}
};
//-----------------------------------------------------------------------------------------
//�ˬd�O�_�i�H�����~
//Local	-> DataCenter
struct PG_Mail_LtoD_CheckGetItemRequest
{
	GamePGCommandEnum	Command;

	int		DBID;			//�֦��̪�DBID
	int		MailDBID;		//�H��DBID (�@�w�n�̫�@��Ū�����H��)
	int		Money;			//+����o -���I�X
	int		Money_Account;	//(�b����)
	int		Moneu_Bonus;

	PG_Mail_LtoD_CheckGetItemRequest( )
	{	Command = EM_PG_Mail_LtoD_CheckGetItemRequest;	}
};
//-----------------------------------------------------------------------------------------
//�ˬd�O�_�i�H�����~
//DataCenter -> Local	
struct PG_Mail_DtoL_CheckGetItemResult
{
	GamePGCommandEnum	Command;

	int		DBID;			//�֦��̪�DBID
	int		TargetDBID;		//��l�H�H��
	int		MailDBID;		//�H��DBID (�@�w�n�̫�@��Ū�����H��)
	int		Money;			//+����o -���I�X
	int		Money_Account;	//(�b����)
	int		Money_Bonus;
	bool	Result;

	PG_Mail_DtoL_CheckGetItemResult( )
	{	Command = EM_PG_Mail_DtoL_CheckGetItemResult;	}
};
//-----------------------------------------------------------------------------------------
//�����~�P����
//�n�D DBCenter ��s��Ʈw����T
//Local	-> DataCenter
struct PG_Mail_LtoD_GetItem
{
	GamePGCommandEnum	Command;

	int		DBID;		//�֦��̪�DBID
	int		MailDBID;	//�H��DBID (�@�w�n�̫�@��Ū�����H��)
	int		Money;		//+����o -���I�X
	int		Money_Account;	//(�b����)
	int		Money_Bonus;

	PG_Mail_LtoD_GetItem( )
	{	Command = EM_PG_Mail_LtoD_GetItem;	}
};
//-----------------------------------------------------------------------------------------
//�����~�P���� ���G
//DataCenter -> Local
struct PG_Mail_DtoL_GetItemResult
{
	GamePGCommandEnum	Command;
	int		DBID;
	int		OrgSenderDBID;
	int		MailDBID;	//�H��DBID (�@�w�n�̫�@��Ū�����H��)
	bool	Result;
	int		Money;		//���� ( + ��o  - �I�X )
	int		Money_Account;	//(�b����)
	int		Money_Bonus;
	ItemFieldStruct		Item[10];		//�H�e���~


	PG_Mail_DtoL_GetItemResult( )
	{	Command = EM_PG_Mail_DtoL_GetItemResult;	}
};
//-----------------------------------------------------------------------------------------
//�����~�P���� ���G
//Local -> Client
struct PG_Mail_LtoC_GetItemResult
{
	GamePGCommandEnum	Command;

	int		MailDBID;	//�H��DBID (�@�w�n�̫�@��Ū�����H��)
	bool	Result;


	PG_Mail_LtoC_GetItemResult( )
	{	Command = EM_PG_Mail_LtoC_GetItemResult;	}
};
//-----------------------------------------------------------------------------------------
//�}�� Mail
//Local -> Client
struct PG_Mail_LtoC_OpenMail
{
	GamePGCommandEnum	Command;
	int		NpcGUID;				//�l��ID

	PG_Mail_LtoC_OpenMail( )
	{	Command = EM_PG_Mail_LtoC_OpenMail;	}
};
//-----------------------------------------------------------------------------------------
//�n�D���� Mail
//Local -> Client
struct PG_Mail_LtoC_CloseMail
{
	GamePGCommandEnum	Command;

	PG_Mail_LtoC_CloseMail( )
	{	Command = EM_PG_Mail_LtoC_CloseMail;	}
};
//-----------------------------------------------------------------------------------------
//Client�q���w���� Mail
//Local -> Client
struct PG_Mail_CtoL_CloseMail
{
	GamePGCommandEnum	Command;

	PG_Mail_CtoL_CloseMail( )
	{	Command = EM_PG_Mail_CtoL_CloseMail;	}
};
//-----------------------------------------------------------------------------------------
//�n�D�q���|��Ū�����l��q
//Local -> DataCenter
struct PG_Mail_LtoD_UnReadMailCountRequest
{
	GamePGCommandEnum	Command;
	int					DBID;		//�n�D�̪�DBID
	int					CliSockID;	//�ΨӪ����Ǹ�Ƶ�Client
	int					CliProxyID;

	PG_Mail_LtoD_UnReadMailCountRequest( )
	{	Command = EM_PG_Mail_LtoD_UnReadMailCountRequest;	}
};
//-----------------------------------------------------------------------------------------
//�|��Ū�����l��ƶq
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
//�q�����s�H��
//Local -> Client
struct PG_Mail_DtoC_NewMail
{
	GamePGCommandEnum	Command;
	bool IsGift;

	PG_Mail_DtoC_NewMail( )
	{	Command = EM_PG_Mail_DtoC_NewMail;	}
};
//-----------------------------------------------------------------------------------------
//�n�D�H��(�h�H)		
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

