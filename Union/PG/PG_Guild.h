#pragma once
#include "RoleData\RoleDataEx.h"
#include "NetWakerPGEnum.h"
//////////////////////////////////////////////////////////////////////////
//		�إ߳s�p���|
//////////////////////////////////////////////////////////////////////////
//�n�D�}�ҫإ߳s�p���|������
//Local -> Client
struct PG_Guild_LtoC_OpenCreateGuild
{
	GamePGCommandEnum	Command;
	int					TargetNPCID;

	PG_Guild_LtoC_OpenCreateGuild( )
	{	Command = EM_PG_Guild_LtoC_OpenCreateGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//Server�q�������إ߳s�p���|����
//Client -> Server
struct PG_Guild_LtoC_CloseCreateGuild
{
	GamePGCommandEnum	Command;

	PG_Guild_LtoC_CloseCreateGuild( )
	{	Command = EM_PG_Guild_LtoC_CloseCreateGuild;	}
};

//////////////////////////////////////////////////////////////////////////
//�q�������إ߳s�p���|����
//Client -> Server
struct PG_Guild_CtoL_CloseCreateGuild
{
	GamePGCommandEnum	Command;

	PG_Guild_CtoL_CloseCreateGuild( )
	{	Command = EM_PG_Guild_CtoL_CloseCreateGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�إߤ��|
//Client -> Local
struct PG_Guild_CtoL_CreateGuild
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>	GuildName;	//���|�W��

	PG_Guild_CtoL_CreateGuild( )
	{	Command = EM_PG_Guild_CtoL_CreateGuild;	}

};
//////////////////////////////////////////////////////////////////////////
//�n�D�إߤ��|
//Local -> DataCenter
struct PG_Guild_LtoD_CreateGuild
{
	GamePGCommandEnum	Command;
	
	int					PlayerDBID;	//�n�D��DBID
	StaticString <_MAX_NAMESTRING_LEN_>	GuildName;	//���|�W��

	PG_Guild_LtoD_CreateGuild( )
	{	Command = EM_PG_Guild_LtoD_CreateGuild;	}

};
//////////////////////////////////////////////////////////////////////////
enum CreateGuildResultENUM
{
	EM_CreateGuildResult_OK				,	//��?
	EM_CreateGuildResult_Failed_Exist	,	//�w�����|-
	EM_CreateGuildResult_Failed_Name	,	//���|�W�٭��_or �r�꦳���D
	EM_CreateGuildResult_Failed_Money	,	//��������
	EM_CreateGuildResult_NameToShort	,		// �W�٤ӵu,���X�k
	EM_CreateGuildResult_NameToLong		,		// �W�٤Ӫ�,���X�k
	EM_CreateGuildResult_NameIsWrongful	,		// �����~���Ÿ��Φr��
	EM_CreateGuildResult_HasSpecialString,		// �S��W�٤����
	EM_CreateGuildResult_OnlyBasicLetters,		// �u��ϥΰ򥻭^��r��
	EM_CreateGuildResult_EndLetterError	,		// �����r�����~(',")
};

//�^���إߵ��G
//Local -> DataCenter
struct PG_Guild_DtoL_CreateGuildResult
{
	GamePGCommandEnum	Command;

	int						PlayerDBID;	//�n�D��DBID
	int						GuildID;	//���|ID
	StaticString <_MAX_NAMESTRING_LEN_>		GuildName;	//���|�W��
	CreateGuildResultENUM	Result;

	PG_Guild_DtoL_CreateGuildResult( )
	{	Command = EM_PG_Guild_DtoL_CreateGuildResult;	}

};
//////////////////////////////////////////////////////////////////////////
//�^���إߵ��G
//Local -> Client
struct PG_Guild_LtoC_CreateGuildResult
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>		GuildName;	//���|�W��
	CreateGuildResultENUM	Result;
	int						GuildID;

	PG_Guild_LtoC_CreateGuildResult( )
	{	Command = EM_PG_Guild_LtoC_CreateGuildResult;	}

};
//////////////////////////////////////////////////////////////////////////
//		�ܽгs�p����
//////////////////////////////////////////////////////////////////////////
//�ܽгs�p����
//Client -> Local
struct PG_Guild_CtoL_NewGuildInvite
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>		PlayerName;		//�Q�ܽФH�W


	PG_Guild_CtoL_NewGuildInvite( )
	{	Command = EM_PG_Guild_CtoL_NewGuildInvite;	}
};
//////////////////////////////////////////////////////////////////////////
//�q���Q�n��(�s�p����)
//Local -> Client
struct PG_Guild_LtoC_NewGuildInvite
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>	LeaderName;			//�ܽЪ̤H�W
	int					LeaderDBID;
	int					GuildID;			//���|ID

	PG_Guild_LtoC_NewGuildInvite( )
	{	Command = EM_PG_Guild_LtoC_NewGuildInvite;	}
};
//////////////////////////////////////////////////////////////////////////
//�^���ܽ�
//Client -> Local
struct PG_Guild_CtoL_NewGuildInviteResult
{
	GamePGCommandEnum	Command;

	int					LeaderDBID;
	int					GuildID;			//���|ID
	bool				Result;

	PG_Guild_CtoL_NewGuildInviteResult( )
	{	Command = EM_PG_Guild_CtoL_NewGuildInviteResult;	}
};
//////////////////////////////////////////////////////////////////////////
enum	NewGuildInviteResultENUM
{
	EM_NewGuildInviteResult_OK			,	//�n�Ц�?
	EM_NewGuildInviteResult_Failed		,	//�n�Х���- 
	EM_NewGuildInviteResult_NotFind		,	//�䤣�� �Q�ܽЪ�
	EM_NewGuildInviteResult_HasGuild	,	//�Q�ܽЪ� �w�g�����|
	EM_NewGuildInviteResult_Disagree	,	//�Q�ܽЪ� ���P�N

};
//�p�G���P�N�ܽгq��
//Local -> Client
struct PG_Guild_LtoC_NewGuildInviteResult
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>			PlayerName;			//�Q�ܽЪ̤H�W
	NewGuildInviteResultENUM	Result;				//���G

	PG_Guild_LtoC_NewGuildInviteResult( )
	{	Command = EM_PG_Guild_LtoC_NewGuildInviteResult;	}
};

//////////////////////////////////////////////////////////////////////////
//�q���[�J�s����
//Local -> DataCenter
struct PG_Guild_LtoD_AddNewGuildMember
{
	GamePGCommandEnum	Command;

	int					LeaderDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	LeaderName;			//�ܽФH�W
	int					GuildID;			//���|ID
	int					NewMemberDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	NewMember;			//�s�[�J�̤H�W


	PG_Guild_LtoD_AddNewGuildMember( )
	{	Command = EM_PG_Guild_LtoD_AddNewGuildMember;	}
};
//////////////////////////////////////////////////////////////////////////
//�^���[�J���G
//DataCenter -> Local
struct PG_Guild_DtoL_AddNewGuildMemberResult
{
	GamePGCommandEnum	Command;

	int					LeaderDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	LeaderName;			//�ܽФH�W
	int					GuildID;			//���|ID
	StaticString <_MAX_NAMESTRING_LEN_>	NewMember;			//�s�[�J�̤H�W
	bool				Result;

	PG_Guild_DtoL_AddNewGuildMemberResult( )
	{	Command = EM_PG_Guild_DtoL_AddNewGuildMemberResult;	}
};
//////////////////////////////////////////////////////////////////////////
//�q���s�[�J�s�p��  �[�J��?
//Local -> Client
struct PG_Guild_LtoC_JoindNewGuild
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>	LeaderName;			//�ܽФH�W
	int					GuildID;			//���|ID

	PG_Guild_LtoC_JoindNewGuild( )
	{	Command = EM_PG_Guild_LtoC_JoindNewGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D���}�s�p���|
//Client -> Local
struct PG_Guild_CtoL_LeaveNewGuild
{
	GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_>	PlayerName;		

	PG_Guild_CtoL_LeaveNewGuild( )
	{	Command = EM_PG_Guild_CtoL_LeaveNewGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D���}�s�p���|
//Local -> DataCenter
struct PG_Guild_LtoD_LeaveNewGuild
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	PlayerName;		


	PG_Guild_LtoD_LeaveNewGuild( )
	{	Command = EM_PG_Guild_LtoD_LeaveNewGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//�^�����}�s�p���|���G
//DataCenter -> Local 
struct PG_Guild_DtoL_LeaveNewGuildResult
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	PlayerName;		

	bool				Result;

	PG_Guild_DtoL_LeaveNewGuildResult( )
	{	Command = EM_PG_Guild_DtoL_LeaveNewGuildResult;	}
};
//////////////////////////////////////////////////////////////////////////
//�^�����}�s�p���|���G
//Local -> Client
struct PG_Guild_LtoC_LeaveNewGuildResult
{
	GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_>	KickPlayerName;		
	bool				Result;

	PG_Guild_LtoC_LeaveNewGuildResult( )
	{	Command = EM_PG_Guild_LtoC_LeaveNewGuildResult;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�Ѵ��s�p���|
//Client -> Local
struct PG_Guild_CtoL_DelNewGuild
{
	GamePGCommandEnum	Command;

	PG_Guild_CtoL_DelNewGuild( )
	{	Command = EM_PG_Guild_CtoL_DelNewGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�Ѵ��s�p���|
//Local -> DataCenter
struct PG_Guild_LtoD_DelNewGuild
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;

	PG_Guild_LtoD_DelNewGuild( )
	{	Command = EM_PG_Guild_LtoD_DelNewGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//�^���Ѵ��s�p���G
//DataCenter -> Local 
struct PG_Guild_DtoL_DelNewGuildResult
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;
	bool				Result;

	PG_Guild_DtoL_DelNewGuildResult( )
	{	Command = EM_PG_Guild_DtoL_DelNewGuildResult;	}
};
//////////////////////////////////////////////////////////////////////////
//�^���Ѵ��s�p���G
//Local -> Client
struct PG_Guild_LtoC_DelNewGuildResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Guild_LtoC_DelNewGuildResult( )
	{	Command = EM_PG_Guild_LtoC_DelNewGuildResult;	}
};


//////////////////////////////////////////////////////////////////////////
//	���|��� �q�� Local�إ�
//////////////////////////////////////////////////////////////////////////
//�W�[���|
// DataCenter -> Local
struct PG_Guild_DtoL_AddGuild
{
	GamePGCommandEnum	Command;

	GuildBaseStruct	Guild;

	PG_Guild_DtoL_AddGuild( )
	{	Command = EM_PG_Guild_DtoL_AddGuild;	}
};
//////////////////////////////////////////////////////////////////////////
// �������|
// DataCenter -> Local
struct PG_Guild_DtoL_DelGuild
{
	GamePGCommandEnum	Command;

	int		GuildID;

	PG_Guild_DtoL_DelGuild( )
	{	Command = EM_PG_Guild_DtoL_DelGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//�W�[���|����
// DataCenter -> Local
struct PG_Guild_DtoL_AddGuildMember	
{
	GamePGCommandEnum	Command;

	GuildMemberStruct	Member;

	PG_Guild_DtoL_AddGuildMember( )
	{	Command = EM_PG_Guild_DtoL_AddGuildMember;	}
};
//////////////////////////////////////////////////////////////////////////
// �������|����
// DataCenter -> Local
struct PG_Guild_DtoL_DelGuildMember
{
	GamePGCommandEnum	Command;

	int					GuildID;
	StaticString <_MAX_NAMESTRING_LEN_>	MemberName;			//�����̦W��

	PG_Guild_DtoL_DelGuildMember( )
	{	Command = EM_PG_Guild_DtoL_DelGuildMember;	}
};
//////////////////////////////////////////////////////////////////////////
//�ק綠�|���
struct PG_Guild_DtoL_ModifyGuild
{
	GamePGCommandEnum	Command;

	GuildBaseStruct	Guild;

	PG_Guild_DtoL_ModifyGuild( )
	{	Command = EM_PG_Guild_DtoL_ModifyGuild;	}
};

//�ק�`�ܪ����|���
//Datacenter -> Local
//DataCenter -> Client
struct PG_Guild_DtoX_ModifySimpleGuild
{
	GamePGCommandEnum	Command;

	SimpleGuildInfoStruct	SimpleGuildInfo;

	PG_Guild_DtoX_ModifySimpleGuild( )
	{	Command = EM_PG_Guild_DtoX_ModifySimpleGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//�ק綠�|�������
struct PG_Guild_DtoL_ModifyGuildMember
{
	GamePGCommandEnum	Command;

	GuildMemberStruct	Member;

	PG_Guild_DtoL_ModifyGuildMember( )
	{	Command = EM_PG_Guild_DtoL_ModifyGuildMember;	}
};

//////////////////////////////////////////////////////////////////////////
//�s�p�q�L
//////////////////////////////////////////////////////////////////////////
//�q���Ҧ����|�����s������
//DataCenter -> Client
struct PG_Guild_DtoC_GuildCreate
{
	GamePGCommandEnum	Command;
	int		GuildID;
	PG_Guild_DtoC_GuildCreate( )
	{	Command = EM_PG_Guild_DtoC_GuildCreate;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D���|�򥻸��
//Client -> Local
struct PG_Guild_CtoL_GuildInfoRequet
{
	GamePGCommandEnum	Command;
	int					GuildID;
	PG_Guild_CtoL_GuildInfoRequet( )
	{	Command = EM_PG_Guild_CtoL_GuildInfoRequet;	}
};
//////////////////////////////////////////////////////////////////////////
//�q�����|�򥻸��
//Local -> Client
struct PG_Guild_LtoC_GuildInfo
{
	GamePGCommandEnum	Command;

	GuildBaseStruct		Guild;
	int					MemberCount;

	PG_Guild_LtoC_GuildInfo( )
	{	Command = EM_PG_Guild_LtoC_GuildInfo;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D���|�����C����
//Client -> Local
struct PG_Guild_CtoL_GuildMemberListRequet
{
	GamePGCommandEnum	Command;
	int					GuildID;
	PG_Guild_CtoL_GuildMemberListRequet( )
	{	Command = EM_PG_Guild_CtoL_GuildMemberListRequet;	}
};
//////////////////////////////////////////////////////////////////////////
//�q�����|�����C����
//Local -> Client
struct PG_Guild_LtoC_GuildMemberListCount
{
	GamePGCommandEnum	Command;
	int		MemberCount;
	int		GuildID;

	PG_Guild_LtoC_GuildMemberListCount( )
	{	Command = EM_PG_Guild_LtoC_GuildMemberListCount;	}
};

struct GuildMemberBaseInfoStruct
{
	StaticString< _MAX_NAMERECORD_SIZE_ >   MemberName;
	int						MemberDBID;
	GuildRankENUM			Rank;				//���⤽�|����
	Voc_ENUM				Voc;
	Voc_ENUM				Voc_Sub;
	Sex_ENUM				Sex;
	int						LV;
	int						LV_Sub;
	int						GroupID;
	bool					IsOnline;
	int						GUID;				//�ϰ�ID

};

//���|���� �C���n���
//Local -> Client
struct PG_Guild_LtoC_GuildMemberBaseInfo
{
	GamePGCommandEnum	Command;

	GuildMemberBaseInfoStruct Info;

	PG_Guild_LtoC_GuildMemberBaseInfo( )
	{	Command = EM_PG_Guild_LtoC_GuildMemberBaseInfo;	}
};

//////////////////////////////////////////////////////////////////////////
//�n�D���|�����򥻸��
//Client -> Local
struct PG_Guild_CtoL_GuildMemberInfoRequet
{
	GamePGCommandEnum	Command;
	int					MemberDBID;
	PG_Guild_CtoL_GuildMemberInfoRequet( )
	{	Command = EM_PG_Guild_CtoL_GuildMemberInfoRequet;	}
};
//////////////////////////////////////////////////////////////////////////
//�q�����|�򥻸��
//Local -> Client
struct PG_Guild_LtoC_GuildMemberInfo
{
	GamePGCommandEnum	Command;

	GuildMemberStruct	Member;

	PG_Guild_LtoC_GuildMemberInfo( )
	{	Command = EM_PG_Guild_LtoC_GuildMemberInfo;	}
};
//////////////////////////////////////////////////////////////////////////
//�ק綠�|���
//Client -> Local
struct PG_Guild_CtoL_ModifyGuildInfoRequest
{
	GamePGCommandEnum	Command;
	GuildBaseStruct		Guild;
	PG_Guild_CtoL_ModifyGuildInfoRequest( )
	{	Command = EM_PG_Guild_CtoL_ModifyGuildInfoRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//�ק綠�|���
//Local -> DataCenter
struct PG_Guild_LtoD_ModifyGuildInfoRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;			//�n�D�ק��
	GuildBaseStruct		Guild;
	PG_Guild_LtoD_ModifyGuildInfoRequest( )
	{	Command = EM_PG_Guild_LtoD_ModifyGuildInfoRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�ק綠�|����
//Client -> Local
struct PG_Guild_CtoL_ModifyGuildMemberInfoRequest
{
	GamePGCommandEnum	Command;
	GuildMemberStruct	Member;

	PG_Guild_CtoL_ModifyGuildMemberInfoRequest( )
	{	Command = EM_PG_Guild_CtoL_ModifyGuildMemberInfoRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�ק綠�|����
//Local -> DataCenter
struct PG_Guild_LtoD_ModifyGuildMemberInfoRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;			//�n�D�ק��
	GuildMemberStruct	Member;
	PG_Guild_LtoD_ModifyGuildMemberInfoRequest( )
	{	Command = EM_PG_Guild_LtoD_ModifyGuildMemberInfoRequest;	}
};

//////////////////////////////////////////////////////////////////////////
// �������|�n�Х[�J
//////////////////////////////////////////////////////////////////////////
//�ܽХ[�J���|
//Client -> Local
struct PG_Guild_CtoL_GuildInvite
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>	PlayerName;		//�Q�ܽФH�W


	PG_Guild_CtoL_GuildInvite( )
	{	Command = EM_PG_Guild_CtoL_GuildInvite;	}
};
//////////////////////////////////////////////////////////////////////////
//�q���Q�n�Х[�J���|
//Local -> Client
struct PG_Guild_LtoC_GuildInvite
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>	LeaderName;			//�ܽЪ̤H�W
	int					LeaderDBID;
	int					GuildID;			//���|ID

	PG_Guild_LtoC_GuildInvite( )
	{	Command = EM_PG_Guild_LtoC_GuildInvite;	}
};
//////////////////////////////////////////////////////////////////////////
//�^���ܽХ[�J���|
//Client -> Local
struct PG_Guild_CtoL_GuildInviteResult
{
	GamePGCommandEnum	Command;

	int					LeaderDBID;
	int					GuildID;			//���|ID
	bool				Result;

	PG_Guild_CtoL_GuildInviteResult( )
	{	Command = EM_PG_Guild_CtoL_GuildInviteResult;	}
};
//////////////////////////////////////////////////////////////////////////
enum	GuildInviteResultENUM
{
	EM_GuildInviteResult_OK			,	//�n�Ц�?
	EM_GuildInviteResult_Failed		,	//�n�Х���-
	EM_GuildInviteResult_NotFind	,	//�䤣�� �Q�ܽЪ�
	EM_GuildInviteResult_HasGuild	,	//�Q�ܽЪ� �w�g�����|
	EM_GuildInviteResult_Disagree	,	//�Q�ܽЪ� ���P�N

};
//�p�G���P�N�[�J���|�ܽгq��
//Local -> Client
struct PG_Guild_LtoC_GuildInviteResult
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>		PlayerName;			//�Q�ܽЪ̤H�W
	GuildInviteResultENUM	Result;				//���G

	PG_Guild_LtoC_GuildInviteResult( )
	{	Command = EM_PG_Guild_LtoC_GuildInviteResult;	}
};

//////////////////////////////////////////////////////////////////////////
//�q���[�J�s����
//Local -> DataCenter
struct PG_Guild_LtoD_AddGuildMember
{
	GamePGCommandEnum	Command;

	int					MemberDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	MemberName;			//�ܽФH�W
	int					GuildID;			//���|ID
	int					NewMemberDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	NewMember;			//�s�[�J�̤H�W


	PG_Guild_LtoD_AddGuildMember( )
	{	Command = EM_PG_Guild_LtoD_AddGuildMember;	}
};
//////////////////////////////////////////////////////////////////////////
//�^���[�J���G
//DataCenter -> Local
struct PG_Guild_DtoL_AddGuildMemberResult
{
	GamePGCommandEnum	Command;

	int					LeaderDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	LeaderName;			//�ܽФH�W
	int					GuildID;			//���|ID
	StaticString <_MAX_NAMESTRING_LEN_>	NewMember;			//�s�[�J�̤H�W
	bool				Result;

	PG_Guild_DtoL_AddGuildMemberResult( )
	{	Command = EM_PG_Guild_DtoL_AddGuildMemberResult;	}
};
//////////////////////////////////////////////////////////////////////////
//�q���s�[�J���|��  �[�J��
//Local -> Client
struct PG_Guild_LtoC_JoindGuild
{
	GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_>	LeaderName;			//�ܽФH�W
	int					GuildID;			//���|ID

	PG_Guild_LtoC_JoindGuild( )
	{	Command = EM_PG_Guild_LtoC_JoindGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D���}���|
//Client -> Local
struct PG_Guild_CtoL_LeaveGuild
{
	GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_>	PlayerName;		

	PG_Guild_CtoL_LeaveGuild( )
	{	Command = EM_PG_Guild_CtoL_LeaveGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D���}���|
//Local -> DataCenter
struct PG_Guild_LtoD_LeaveGuild
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	PlayerName;		


	PG_Guild_LtoD_LeaveGuild( )
	{	Command = EM_PG_Guild_LtoD_LeaveGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//�^�����}���|���G
//DataCenter -> Local 
struct PG_Guild_DtoL_LeaveGuildResult
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	PlayerName;		

	bool				Result;

	PG_Guild_DtoL_LeaveGuildResult( )
	{	Command = EM_PG_Guild_DtoL_LeaveGuildResult;	}
};
//////////////////////////////////////////////////////////////////////////
//�^�����}���|���G
//Local -> Client
struct PG_Guild_LtoC_LeaveGuildResult
{
	GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_>	KickPlayerName;		
	bool				Result;

	PG_Guild_LtoC_LeaveGuildResult( )
	{	Command = EM_PG_Guild_LtoC_LeaveGuildResult;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�Ѵ����|
//Client -> Local
struct PG_Guild_CtoL_DelGuild
{
	GamePGCommandEnum	Command;

	PG_Guild_CtoL_DelGuild( )
	{	Command = EM_PG_Guild_CtoL_DelGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�Ѵ����|
//Local -> DataCenter
struct PG_Guild_LtoD_DelGuild
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;

	PG_Guild_LtoD_DelGuild( )
	{	Command = EM_PG_Guild_LtoD_DelGuild;	}
};
//////////////////////////////////////////////////////////////////////////
//�^���Ѵ����|���G
//DataCenter -> Local 
struct PG_Guild_DtoL_DelGuildResult
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;
	bool				Result;

	PG_Guild_DtoL_DelGuildResult( )
	{	Command = EM_PG_Guild_DtoL_DelGuildResult;	}
};
//////////////////////////////////////////////////////////////////////////
//�^���Ѵ����|���G
//Local -> Client
struct PG_Guild_LtoC_DelGuildResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Guild_LtoC_DelGuildResult( )
	{	Command = EM_PG_Guild_LtoC_DelGuildResult;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D��s�p���|�ର�������|
//Local -> DataCenter
struct PG_Guild_LtoD_SetGuildReady
{
	GamePGCommandEnum	Command;
	int					GuildID;
	PG_Guild_LtoD_SetGuildReady( )
	{	Command = EM_PG_Guild_LtoD_SetGuildReady;	}
};
//////////////////////////////////////////////////////////////////////////
//�q��Client �B�z���G
//Local -> DataCenter
struct PG_Guild_LtoC_SetGuildReady
{
	GamePGCommandEnum	Command;
	bool				Result;
	PG_Guild_LtoC_SetGuildReady( )
	{	Command = EM_PG_Guild_LtoC_SetGuildReady;	}
};

//////////////////////////////////////////////////////////////////////////
//	���|�d���O
//////////////////////////////////////////////////////////////////////////
//�n�D�d��
//Client -> Local
struct PG_Guild_CtoL_BoardPostRequest
{
	GamePGCommandEnum	Command;
	GuildBoardStruct	Message;

	PG_Guild_CtoL_BoardPostRequest( )
	{	Command = EM_PG_Guild_CtoL_BoardPostRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�d��
//Local -> DataCenter
struct PG_Guild_LtoD_BoardPostRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	GuildBoardStruct	Message;

	PG_Guild_LtoD_BoardPostRequest( )
	{	Command = EM_PG_Guild_LtoD_BoardPostRequest;	}
};
//////////////////////////////////////////////////////////////////////////

//DataCenter -> Client
struct PG_Guild_DtoC_BoardPostResult
{
	GamePGCommandEnum	Command;
	bool	Result;

	PG_Guild_DtoC_BoardPostResult( )
	{	Command = EM_PG_Guild_DtoC_BoardPostResult;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�d���C��
//Client -> Local
struct PG_Guild_CtoL_BoardListRequest
{
	GamePGCommandEnum	Command;
	int		PageID;					//���@��

	PG_Guild_CtoL_BoardListRequest( )
	{	Command = EM_PG_Guild_CtoL_BoardListRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�d���C��
//Local -> DataCenter
struct PG_Guild_LtoD_BoardListRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					PageID;

	PG_Guild_LtoD_BoardListRequest( )
	{	Command = EM_PG_Guild_LtoD_BoardListRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//�^���d���C�� �ƶq
//DataCenter -> Client
struct PG_Guild_DtoC_BoardListCount
{
	GamePGCommandEnum	Command;
	int					MaxCount;		//�̤j����
	int					Count;			//�e�X�X�� (0��ܧ䤣��)
	int					PageID;

	PG_Guild_DtoC_BoardListCount( )
	{	Command = EM_PG_Guild_DtoC_BoardListCount;	}
};

//�^���d���C�� �浧���
struct PG_Guild_DtoC_BoardListData
{
	GamePGCommandEnum		Command;
	int						Index;		//�ĴX�����
	int						GUID;		//�Ǹ�
	StaticString< _MAX_NAMERECORD_SIZE_ >   PlayerName;	    //�o��H
	GuildBoardModeStruct	Type;		//���A
	int						Time;		//�o��ɶ�
	StaticString< 128 >		Title;		//���D


	PG_Guild_DtoC_BoardListData( )
	{	Command = EM_PG_Guild_DtoC_BoardListData;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�d�����e
//Client -> Local
struct PG_Guild_CtoL_BoardMessageRequest
{
	GamePGCommandEnum	Command;
	int					GUID;		//�T�����s��

	PG_Guild_CtoL_BoardMessageRequest( )
	{	Command = EM_PG_Guild_CtoL_BoardMessageRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�d�����e
//Local -> DataCenter
struct PG_Guild_LtoD_BoardMessageRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					GUID;


	PG_Guild_LtoD_BoardMessageRequest( )
	{	Command = EM_PG_Guild_LtoD_BoardMessageRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//�d�����e
//DataCenter -> Client
struct PG_Guild_DtoC_BoardMessageResult
{
	GamePGCommandEnum	Command;
	GuildBoardStruct	Message;

	PG_Guild_DtoC_BoardMessageResult( )
	{	Command = EM_PG_Guild_DtoC_BoardMessageResult;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�ק�d��
//Client -> Local
struct PG_Guild_CtoL_BoardModifyMessageRequest
{
	GamePGCommandEnum		Command;
	int						MessageGUID;
	StaticString< 512 >		Message;		//�s���d��

	PG_Guild_CtoL_BoardModifyMessageRequest( )
	{	Command = EM_PG_Guild_CtoL_BoardModifyMessageRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�ק�d��
//Local -> DataCenter
struct PG_Guild_LtoD_BoardModifyMessageRequest
{
	GamePGCommandEnum		Command;
	int						PlayerDBID;
	int						MessageGUID;
	StaticString< 512 >		Message;		//�s���d��

	PG_Guild_LtoD_BoardModifyMessageRequest( )
	{	Command = EM_PG_Guild_LtoD_BoardModifyMessageRequest;	}
};
//////////////////////////////////////////////////////////////////////////

//DataCenter -> Client
struct PG_Guild_DtoC_BoardModifyMessageResult
{
	GamePGCommandEnum	Command;
	bool	Result;

	PG_Guild_DtoC_BoardModifyMessageResult( )
	{	Command = EM_PG_Guild_DtoC_BoardModifyMessageResult;	}
};
//////////////////////////////////////////////////////////////////////////
//�]�w�d���ݩ�
//Client -> Local
struct PG_Guild_CtoL_BoardModifyModeRequest
{
	GamePGCommandEnum		Command;
	int						MessageGUID;
	GuildBoardModeStruct	Mode;

	PG_Guild_CtoL_BoardModifyModeRequest( )
	{	Command = EM_PG_Guild_CtoL_BoardModifyModeRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�ק�d��
//Local -> DataCenter
struct PG_Guild_LtoD_BoardModifyModeRequest
{
	GamePGCommandEnum		Command;
	int						PlayerDBID;
	int						MessageGUID;
	GuildBoardModeStruct	Mode;

	PG_Guild_LtoD_BoardModifyModeRequest( )
	{	Command = EM_PG_Guild_LtoD_BoardModifyModeRequest;	}
};
//////////////////////////////////////////////////////////////////////////

//DataCenter -> Client
struct PG_Guild_DtoC_BoardModifyModeResult
{
	GamePGCommandEnum	Command;
	bool	Result;

	PG_Guild_DtoC_BoardModifyModeResult( )
	{	Command = EM_PG_Guild_DtoC_BoardModifyModeResult;	}
};
//////////////////////////////////////////////////////////////////////////
//���|�W�ٻPid���
//Local -> Client
struct PG_Guild_LtoC_GuildBaseInfo
{
	GamePGCommandEnum	Command;
	
	int			GuildID;
	StaticString <_MAX_NAMESTRING_LEN_>	GuildName;	//���|�W��
	GuildFlagStruct		Flag;
	bool		IsReady;							//�O�_��������

	PG_Guild_LtoC_GuildBaseInfo( )
	{	Command = EM_PG_Guild_LtoC_GuildBaseInfo;	}

};
//////////////////////////////////////////////////////////////////////////
//���|�R��
//Local -> Client
struct PG_Guild_LtoC_GuildDestroy
{
	GamePGCommandEnum	Command;

	int			GuildID;

	PG_Guild_LtoC_GuildDestroy( )
	{	Command = EM_PG_Guild_LtoC_GuildDestroy;	}

};
//////////////////////////////////////////////////////////////////////////
//�ۤv���|���[�J�s����
//DataCenter -> Client
struct PG_Guild_DtoC_JoinGuildMember
{
	GamePGCommandEnum	Command;
	GuildMemberStruct	Member;

	PG_Guild_DtoC_JoinGuildMember( )
	{	Command = EM_PG_Guild_DtoC_JoinGuildMember;	}

};
//////////////////////////////////////////////////////////////////////////
//�ۤv���|���H���}
//DataCenter -> Client
struct PG_Guild_DtoC_LeaveGuildMember
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;
	int					MemberDBID;

	PG_Guild_DtoC_LeaveGuildMember( )
	{	Command = EM_PG_Guild_DtoC_LeaveGuildMember;	}

};
//////////////////////////////////////////////////////////////////////////
//���|�^�m
//////////////////////////////////////////////////////////////////////////
//�}�Ҥ��|�^�m���
//Local -> Client
struct PG_Guild_LtoC_OpenGuildContribution
{
	GamePGCommandEnum	Command;
	int					TargetNPCID;

	PG_Guild_LtoC_OpenGuildContribution( )
	{	Command = EM_PG_Guild_LtoC_OpenGuildContribution;	}
};
//////////////////////////////////////////////////////////////////////////
//�]�w�n��m�����~
//Client -> Server
struct PG_Guild_CtoL_GuildContributionItem
{
	GamePGCommandEnum	Command;
	
	int					Pos;			//������
	ItemFieldStruct		Item;
	bool				IsPut;			//true ��  false ���^

	PG_Guild_CtoL_GuildContributionItem()
	{
		Command = EM_PG_Guild_CtoL_GuildContributionItem;
	}
};
//////////////////////////////////////////////////////////////////////////
//�]�w��m�^�m���~�����G
//Server -> Client
struct PG_Guild_LtoC_GuildContributionItemResult
{
	GamePGCommandEnum	Command;
	bool				Result;

	PG_Guild_LtoC_GuildContributionItemResult()
	{
		Command = EM_PG_Guild_LtoC_GuildContributionItemResult;
	}
};

//////////////////////////////////////////////////////////////////////////
//�T�w�^�m
//Client -> Server
struct PG_Guild_CtoL_GuildContributionOK
{
	GamePGCommandEnum	Command;
	int					Money;	//�n�@�m����
	int					Bonus_Money;

	PG_Guild_CtoL_GuildContributionOK( )
	{	Command = EM_PG_Guild_CtoL_GuildContributionOK;	}
};
//////////////////////////////////////////////////////////////////////////
//Server�q�� �����^�m���~������
//Server -> Client
struct PG_Guild_LtoC_GuildContributionClose
{
	GamePGCommandEnum	Command;

	PG_Guild_LtoC_GuildContributionClose()
	{
		Command = EM_PG_Guild_LtoC_GuildContributionClose;
	}
};
//////////////////////////////////////////////////////////////////////////
//�����^�m���~������
//Client -> Server
struct PG_Guild_CtoL_GuildContributionClose
{
	GamePGCommandEnum	Command;

	PG_Guild_CtoL_GuildContributionClose()
	{
		Command = EM_PG_Guild_CtoL_GuildContributionClose;
	}
};
//////////////////////////////////////////////////////////////////////////
/*
 
//Local -> Datacenter
struct PG_Guild_LtoD_BuyFunction
{
	GamePGCommandEnum	Command;
	int					GuildID;
	int					PlayerDBID;
	int					Score;
	int					Type;			 
	int					Value;

	PG_Guild_LtoD_BuyFunction( )
	{	Command = EM_PG_Guild_LtoD_BuyFunction;	}
};
*/

//////////////////////////////////////////////////////////////////////////
//���|�ө�
//////////////////////////////////////////////////////////////////////////
/*
//Local -> Client 
//�}�Ҥ��|�ө�
struct PG_Guild_LtoC_GuildShopOpen
{
	GamePGCommandEnum	Command;
	int				TargetNPCID;
	int				Score;
	StaticFlag<256>	Flag;

	PG_Guild_LtoC_GuildShopOpen( )
	{	Command = EM_PG_Guild_LtoC_GuildShopOpen;	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 
 
struct PG_Guild_CtoL_GuildShopBuy
{
	GamePGCommandEnum	Command;
	int					FlagID;

	PG_Guild_CtoL_GuildShopBuy( )
	{	Command = EM_PG_Guild_CtoL_GuildShopBuy;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DC 
 
struct PG_Guild_LtoD_GuildShopBuy
{
	GamePGCommandEnum	Command;

	int				PlayerDBID; 
	int				FlagID;

	PG_Guild_LtoD_GuildShopBuy( )
	{	Command = EM_PG_Guild_LtoD_GuildShopBuy;	}
};
//////////////////////////////////////////////////////////////////////////
//DC -> Client 
 
struct PG_Guild_DtoC_GuildShopBuyResult
{
	GamePGCommandEnum	Command;
	int					FlagID;
	bool				Result;

	PG_Guild_DtoC_GuildShopBuyResult( )
	{	Command = EM_PG_Guild_DtoC_GuildShopBuyResult;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client 
//��������
struct PG_Guild_LtoC_GuildShopClose
{
	GamePGCommandEnum	Command;

	PG_Guild_LtoC_GuildShopClose( )
	{	Command = EM_PG_Guild_LtoC_GuildShopClose;	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local 
//��������		
struct PG_Guild_CtoL_GuildShopClose
{
	GamePGCommandEnum	Command;

	PG_Guild_CtoL_GuildShopClose( )
	{	Command = EM_PG_Guild_CtoL_GuildShopClose;	}
};
*/
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//�n�D�Ҧ����|�H�Ƹ��
struct PG_Guild_CtoL_AllGuildMemberCountRequest
{
	GamePGCommandEnum	Command;

	PG_Guild_CtoL_AllGuildMemberCountRequest( )
	{	Command = EM_PG_Guild_CtoL_AllGuildMemberCountRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Client
//�q���C�Ӥ��|���H�Ƹ��

#define DEF_MAX_PG_GUILD_MEMBER_COUNT_ 1000
struct GuildMemberCountStruct
{
	int GuildID;
	int Count;
};
struct PG_Guild_LtoC_AllGuildMemberCountResult
{
	GamePGCommandEnum	Command;
	int	Count;
	GuildMemberCountStruct MemberList[ DEF_MAX_PG_GUILD_MEMBER_COUNT_ ];

	PG_Guild_LtoC_AllGuildMemberCountResult( )
	{	Command = EM_PG_Guild_LtoC_AllGuildMemberCountResult;	}

	int Size()
	{
		return sizeof( *this ) - sizeof( int ) * (DEF_MAX_PG_GUILD_MEMBER_COUNT_ - Count );
	}
};

//////////////////////////////////////////////////////////////////////////
//���|��
//////////////////////////////////////////////////////////////////////////
//Local -> Client 
//�}���v�Ф���
struct PG_Guild_LtoC_GuildFightAuctionOpen
{
	GamePGCommandEnum	Command;
	int					TargetNPCID;
	int					CrystalID;			//�������X
	int 				OwnerGuildID;		//�ثe�֦������|
	int 				BidGuildID;			//�ثe�o�Ъ����|
	int 				BidScore;			//�ثe�v�Ъ����B
	PG_Guild_LtoC_GuildFightAuctionOpen( )
	{	Command = EM_PG_Guild_LtoC_GuildFightAuctionOpen;	}
};
//////////////////////////////////////////////////////////////////////////
//Client - > Local
//�����v�Ф���
struct PG_Guild_CtoL_GuildFightAuctionClose
{
	GamePGCommandEnum	Command;

	PG_Guild_CtoL_GuildFightAuctionClose( )
	{	Command = EM_PG_Guild_CtoL_GuildFightAuctionClose;	}
};
//////////////////////////////////////////////////////////////////////////
//Local - > Client
//�����v�Ф���
struct PG_Guild_LtoC_GuildFightAuctionClose
{
	GamePGCommandEnum	Command;

	PG_Guild_LtoC_GuildFightAuctionClose( )
	{	Command = EM_PG_Guild_LtoC_GuildFightAuctionClose;	}
};
//////////////////////////////////////////////////////////////////////////
//Client - > Local
//�n�v��
struct PG_Guild_CtoL_GuildFightAuctionBid
{
	GamePGCommandEnum	Command;
	//int					iNPCID;
	int					CrystalID;
	int					BidScore;

	PG_Guild_CtoL_GuildFightAuctionBid( )
	{	Command = EM_PG_Guild_CtoL_GuildFightAuctionBid;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter 
//�n�v��
struct PG_Guild_LtoD_GuildFightAuctionBid
{
	GamePGCommandEnum	Command;
	int					LeaderDBID;
	int					CrystalID;
	int					BidScore;

	PG_Guild_LtoD_GuildFightAuctionBid( )
	{	Command = EM_PG_Guild_LtoD_GuildFightAuctionBid;	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Client 
//�v�е��G
enum EM_GuildFightAuctionBidResult
{
	EM_GuildFightAuctionBidResult_Success		= 0,	 
	EM_GuildFightAuctionBidResult_Toolow		,		// �X����ثe�����٧C
	EM_GuildFightAuctionBidResult_Timeout		,		// �w�g�W�L�i�H�X�����ɶ�
	EM_GuildFightAuctionBidResult_LowGuildPoint	,		// ���|�n�������Ф���
	EM_GuildFightAuctionBidResult_Bidding		,		// �n�D�̰ѻP�F�䥦�������v��
	EM_GuildFightAuctionBidResult_Exist			,		// �n�D�֦̾��䥦�������v�O
};


struct PG_Guild_DtoC_GuildFightAuctionBidResult
{
	GamePGCommandEnum	Command;
	int					CrystalID;			//�������X
	int 				OwnerGuildID;		//�ثe�֦������|
	int 				BidGuildID;			//�ثe�o�Ъ����|
	int					iResult;
	int					iHighestAmount;		//�ثe�̰����B

	PG_Guild_DtoC_GuildFightAuctionBidResult( )
	{	Command = EM_PG_Guild_DtoC_GuildFightAuctionBidResult;	}
};

//////////////////////////////////////////////////////////////////////////
//DataCenter -> Client 
//�q�����H�v��
struct PG_Guild_DtoC_GuildFightAuctionBidNewOne
{
	GamePGCommandEnum	Command;
	int					CrystalID;			//�������X
	int 				OwnerGuildID;		//�ثe�֦������|
	int 				BidGuildID;			//�ثe�o�Ъ����|
	int					iHighestAmount;		//�ثe�̰����B
	int					iRemainTime;		//�ѩ󪺼л��ɶ�

	PG_Guild_DtoC_GuildFightAuctionBidNewOne( )
	{	Command = EM_PG_Guild_DtoC_GuildFightAuctionBidNewOne;	}
};

//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local
//�q���Ҧ��ϰ��v�Эק諸��T
struct PG_Guild_DtoL_GuildFightAuctionInfo
{
	GamePGCommandEnum	Command;
	int					CrystalID;			//�������X
	int 				OwnerGuildID;		//�ثe�֦������|
	int 				BitGuildID;			//�ثe�o�Ъ����|

	PG_Guild_DtoL_GuildFightAuctionInfo( )
	{	Command = EM_PG_Guild_DtoL_GuildFightAuctionInfo;	}
};
//////////////////////////////////////////////////////////////////////////

struct PG_Guild_LtoD_GuildFightInfoRequest
{
	GamePGCommandEnum	Command;
	int					CrystalID;			//�������X
	int 				RequestPlayerGUID;	//�n�D�����a���X

	PG_Guild_LtoD_GuildFightInfoRequest( )
	{	Command = EM_PG_Guild_LtoD_GuildFightInfoRequest;	}
};

enum EM_GuildFightInfo_Result
{
	EM_GuildFightInfo_Result_Okay		= 0,
	EM_GuildFightInfo_Result_Bidding	= 1,	// �n�D�̰ѻP�F�䥦�������v��
	EM_GuildFightInfo_Result_Exist		= 2,	// �n�D�֦̾��䥦�������v�O
	EM_GuildFightInfo_Result_Timeout	= 3,	// �L�F�v�Юɶ�
};


struct PG_Guild_DtoC_GuildFightInfo
{
	GamePGCommandEnum			Command;
	EM_GuildFightInfo_Result	emResult;			//  �d�ߵ��G 0 ���`, 1 �d�ߪ̥����w�g�U�Ш䥦�����F, 2 �d�ߪ̥����w�g�֦��䥦�����F
		
	int							iCrystalID;			//�������X
	//int					iHighestGUILDID;
	int							iHighestAmount;
	int							iDefenderGuildID;
	int							iAttackerGuildID;

	int							iOtherCrystalID;	// �n�D�̰ѻP�F�䥦�������v��, �n�D�֦̾��䥦�������v�O ������N��

	int							iGuildPoint;		// ���|�Z��
	int							iRemainTime;		//�Ѿl���ɶ�
	int							iFightTime;

	PG_Guild_DtoC_GuildFightInfo( )
	{	Command = EM_PG_Guild_DtoC_GuildFightInfo;	}
};


//////////////////////////////////////////////////////////////////////////
//DataCenter -> Client 
//�q�����H�W�U�u
struct PG_Guild_DtoL_GuildPlayerOnline
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	bool				IsOnline;

	PG_Guild_DtoL_GuildPlayerOnline( )
	{	Command = EM_PG_Guild_DtoL_GuildPlayerOnline;	}
};
//////////////////////////////////////////////////////////////////////////
// DataCenter -> Client
struct PG_Guild_DtoC_GuildLeaderChange
{
	GamePGCommandEnum	Command;
	int					NewLeaderDBID;
	int					OldLeaderDBID;

	PG_Guild_DtoC_GuildLeaderChange( )
	{	Command = EM_PG_Guild_DtoC_GuildLeaderChange;	}
};
//////////////////////////////////////////////////////////////////////////
// DataCenter -> Client
struct PG_Guild_DtoC_GuildMemberRankChange
{
	GamePGCommandEnum	Command;
	int				MemberDBID;
	int				Rank;

	PG_Guild_DtoC_GuildMemberRankChange( )
	{	Command = EM_PG_Guild_DtoC_GuildMemberRankChange;	}
};
//////////////////////////////////////////////////////////////////////////
struct PG_Guild_DtoC_GuildFightAuctionEnd
{
	GamePGCommandEnum	Command;

	int					iCrystalID;
	int					iDefenderGuildID;
	int					iAttackerGuildID;
	int					iFightTime;
	int					iHighestAmount;

	PG_Guild_DtoC_GuildFightAuctionEnd( )
	{	Command = EM_PG_Guild_DtoC_GuildFightAuctionEnd;	}

};

struct PG_Guild_DtoC_GuildFightBeginNotify
{
	GamePGCommandEnum	Command;

	int					iCrystalID;
	int					iRemainTime;

	PG_Guild_DtoC_GuildFightBeginNotify( )
	{	Command = EM_PG_Guild_DtoC_GuildFightBeginNotify;	}

};

struct PG_Guild_DtoL_CreateGuildFightRoom
{
	GamePGCommandEnum	Command;

	int					iCrystalID;
	int					iDefenderGuildID;
	int					iAttackerGuildID;

	PG_Guild_DtoL_CreateGuildFightRoom( )
	{	Command = EM_PG_Guild_DtoL_CreateGuildFightRoom;	}

};

struct PG_Guild_LtoC_GuildFightLeaderInvite
{
	GamePGCommandEnum	Command;

	int					iCrystalID;
	int					iDefenderGuildID;
	int					iAttackerGuildID;
	int					iSrvID;					// SrvID
	int					iRoomID;				// ���U���ҥ�

	PG_Guild_LtoC_GuildFightLeaderInvite( )
	{	Command = EM_PG_Guild_LtoC_GuildFightLeaderInvite;	}

};

struct PG_Guild_CtoL_GuildFightRequestInvite
{
	GamePGCommandEnum	Command;

	int					iRoomID;
	int					iCrystalID;
	int					iMemberDBID;
	int					iGuildID;

	PG_Guild_CtoL_GuildFightRequestInvite( )
	{	Command = EM_PG_Guild_CtoL_GuildFightRequestInvite;	}
};

struct PG_Guild_LtoC_GuildFightInviteNotify
{
	GamePGCommandEnum	Command;

	int					iSrvID;
	int					iRoomID;
	int					iCrystalID;
	

	PG_Guild_LtoC_GuildFightInviteNotify( )
	{	Command = EM_PG_Guild_LtoC_GuildFightInviteNotify;	}
};

struct PG_Guild_CtoL_GuildFightInviteRespond
{
	GamePGCommandEnum	Command;

	int					iRoomID;
	int					iCrystalID;
	bool				bJoin;
	int					iMemberDBID;
	int					iGuildID;

	PG_Guild_CtoL_GuildFightInviteRespond( )
	{	Command = EM_PG_Guild_CtoL_GuildFightInviteRespond;	}
};

//�q���P�򪱮a�Y�H���|Id����
//Local -> Client
struct PG_Guild_LtoC_GuildIDChange
{
	GamePGCommandEnum	Command;
	int		GItemID;
	int		GuildID;

	PG_Guild_LtoC_GuildIDChange( )
	{	Command = EM_PG_Guild_LtoC_GuildIDChange;	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local	
//���|�|���n�D�P�Y ���|���Ĺ窱�p
struct PG_Guild_CtoL_DeclareWar
{
	GamePGCommandEnum	Command;
	int		Type;				//�ž�����
	int		TargetGuildID;

	PG_Guild_CtoL_DeclareWar( )
	{	Command = EM_PG_Guild_CtoL_DeclareWar;	}
};

//Local	-> DataCenter
//���|�|���n�D�P�Y ���|���Ĺ窱�p
struct PG_Guild_LtoD_DeclareWar
{
	GamePGCommandEnum	Command;
	int		Type;				//�ž�����
	int		PlayerDBID;
	int		TargetGuildID;

	PG_Guild_LtoD_DeclareWar( )
	{	Command = EM_PG_Guild_LtoD_DeclareWar;	}
};

enum DeclareWarResultENUM
{
	EM_DeclareWarResult_OK				,
	EM_DeclareWarResult_DataErr			,	//��ƿ��~
	EM_DeclareWarResult_TimeErr			,	//�žԹL��
	EM_DeclareWarResult_OnWarErr		,	//�žԤ�
	EM_DeclareWarResult_OnWarPrepareErr	,	//�žԷǳƤ�
	EM_DeclareWarResult_ScoreErr		,	//���|��������
	EM_DeclareWarResult_MonyeErr		,	//��������
	EM_DeclareWarResult_LeaderErr		,	//���O�|��
	EM_DeclareWarResult_GuildLvErr		,	//���|���Ť���
	EM_DeclareWarResult_TargetGuildErr	,	//�ؼФ��|���s�b
	EM_DeclareWarResult_TargetGuildLvErr,	//�ؼФ��|���Ť���
};

//DataCenter -> Local
//�žԵ��G
struct PG_Guild_DtoL_DeclareWarResult
{
	GamePGCommandEnum	Command;
	int		Type;				//�ž�����
	int		PlayerDBID;
	int		GuildID;
	int		TargetGuildID;
	DeclareWarResultENUM	Result;

	PG_Guild_DtoL_DeclareWarResult( )
	{	Command = EM_PG_Guild_DtoL_DeclareWarResult;	}
};

//Local -> DataCenter
//�žԵ��G
struct PG_Guild_LtoC_DeclareWarResult
{
	GamePGCommandEnum	Command;
	int		Type;				//�ž�����
	int		TargetGuildID;
	DeclareWarResultENUM	Result;

	PG_Guild_LtoC_DeclareWarResult( )
	{	Command = EM_PG_Guild_LtoC_DeclareWarResult;	}
};

//DataCenter -> Client
//�žԸ�T 
struct PG_Guild_DtoC_PrepareWar
{
	GamePGCommandEnum	Command;
	int			GuildID;
	int			Time;			//�žԮɶ�(��)

	PG_Guild_DtoC_PrepareWar( )
	{	Command = EM_PG_Guild_DtoC_PrepareWar;	}
};


//DataCenter -> Local
//�žԸ�T 
struct PG_Guild_DtoL_WarInfo
{
	GamePGCommandEnum	Command;
	bool		IsDeclareWar;
	int			GuildID[2];
	int			Time;			//�žԮɶ�
	
	PG_Guild_DtoL_WarInfo( )
	{	Command = EM_PG_Guild_DtoL_WarInfo;	}
};

//Local -> Client
//�ήɫžԸ�T 
struct PG_Guild_LtoC_WarInfo
{
	GamePGCommandEnum	Command;
	bool		IsDeclareWar;
	int			GuildID[2];
	int			Time;
	int			BeginTime;

	PG_Guild_LtoC_WarInfo( )
	{	Command = EM_PG_Guild_LtoC_WarInfo;	}
};

//Local -> Client
//�žԸ�T 
struct PG_Guild_LtoC_LoginWarInfo
{
	GamePGCommandEnum	Command;
	int			GuildID[2];
	int			Time;
	int			BeginTime;

	PG_Guild_LtoC_LoginWarInfo( )
	{	Command = EM_PG_Guild_LtoC_LoginWarInfo;	}
};

//////////////////////////////////////////////////////////////////////////
//Local -> DataCenter
//���|�@��ž� �n������
struct PG_Guild_LtoD_DeclareWarScore
{
	GamePGCommandEnum	Command;
	int			GuildID[2];
	int			Score;
	int			KillCount;

	PG_Guild_LtoD_DeclareWarScore( )
	{	Command = EM_PG_Guild_LtoD_DeclareWarScore;	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Local
//���|�@��ž� �԰����G
struct PG_Guild_DtoC_DeclareWarFinalResult
{
	GamePGCommandEnum	Command;
	int			GuildID[2];
	int			Score[2];
	int			KillCount[2];

	PG_Guild_DtoC_DeclareWarFinalResult( )
	{	Command = EM_PG_Guild_DtoC_DeclareWarFinalResult;	}
};
//////////////////////////////////////////////////////////////////////////
/*
//Local -> Client 
//�}�Ҥ��|�X�]�w
struct PG_Guild_LtoC_OpenSetGuildFlagInfo
{
	GamePGCommandEnum	Command;
	//GuildFlagStruct		Flag;
	PG_Guild_LtoC_OpenSetGuildFlagInfo( )
	{	Command = EM_PG_Guild_LtoC_OpenSetGuildFlagInfo;	}
};
*/
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//���|�X�]�w
struct PG_Guild_CtoL_SetGuildFlagInfoRequest
{
	GamePGCommandEnum	Command;
	GuildFlagStruct		Flag;
	PG_Guild_CtoL_SetGuildFlagInfoRequest( )
	{	Command = EM_PG_Guild_CtoL_SetGuildFlagInfoRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Datacenter
//���|�X�]�w
struct PG_Guild_LtoD_SetGuildFlagInfoRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	GuildFlagStruct		Flag;
	PG_Guild_LtoD_SetGuildFlagInfoRequest( )
	{	Command = EM_PG_Guild_LtoD_SetGuildFlagInfoRequest;	}
};
//////////////////////////////////////////////////////////////////////////
//Local -> Datacenter
//���|�X�]�w
enum	SetGuildFlagInfoResultENUM
{
	EM_SetGuildFlagInfoResult_OK			,	
	EM_SetGuildFlagInfoResult_GuildError	,	//���|���~
	EM_SetGuildFlagInfoResult_LeaderError	,	//���O�|��
	EM_SetGuildFlagInfoResult_ScoreError	,	//�n������
};

struct PG_Guild_XtoC_SetGuildFlagInfoResult
{
	GamePGCommandEnum	Command;
	SetGuildFlagInfoResultENUM		Result;

	PG_Guild_XtoC_SetGuildFlagInfoResult( )
	{	Command = EM_PG_Guild_XtoC_SetGuildFlagInfoResult;	}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//��綠�|�W�r
struct PG_Guild_CtoL_ChangeGuildNameRequest
{
	GamePGCommandEnum	Command;
	StaticString <_MAX_NAMESTRING_LEN_>	GuildName;	//���|�W��

	PG_Guild_CtoL_ChangeGuildNameRequest( )
	{	Command = EM_PG_Guild_CtoL_ChangeGuildNameRequest;	}
};
//////////////////////////////////////////////////////////////////////////
// Local -> DataCenter
//��綠�|�W�r
struct PG_Guild_LtoD_ChangeGuildNameRequest
{
	GamePGCommandEnum	Command;
	int					GuildID;
	int					PlayerDBID;
	StaticString <_MAX_NAMESTRING_LEN_>	GuildName;	//���|�W��

	PG_Guild_LtoD_ChangeGuildNameRequest( )
	{	Command = EM_PG_Guild_LtoD_ChangeGuildNameRequest;	}
};
//////////////////////////////////////////////////////////////////////////
enum ChangeGuildNameResultENUM
{
	EM_ChangeGuildNameResult_OK				,
	EM_ChangeGuildNameResult_NameUsed		,		//�W�r����
	EM_ChangeGuildNameResult_FlagError		,		//�S���}�ҧ�W
	EM_ChangeGuildNameResult_NotLeader		,		//���O�|��	
	EM_ChangeGuildNameResult_NameToShort	,		// �W�٤ӵu,���X�k
	EM_ChangeGuildNameResult_NameToLong		,		// �W�٤Ӫ�,���X�k
	EM_ChangeGuildNameResult_NameIsWrongful	,		// �����~���Ÿ��Φr��
	EM_ChangeGuildNameResult_HasSpecialString,		// �S��W�٤����
	EM_ChangeGuildNameResult_OnlyBasicLetters,		// �u��ϥΰ򥻭^��r��
	EM_ChangeGuildNameResult_EndLetterError	,		// �����r�����~(',")
	EM_ChangeGuildNameResult_UnknowErr		,		//�������~
};
// DataCenter -> Client
//��綠�|�W�r
struct PG_Guild_DtoC_ChangeGuildNameResult
{
	GamePGCommandEnum				Command;
	ChangeGuildNameResultENUM		Result;

	PG_Guild_DtoC_ChangeGuildNameResult( )
	{	Command = EM_PG_Guild_DtoC_ChangeGuildNameResult;	}
};
//////////////////////////////////////////////////////////////////////////
//DataCenter -> Client
//�q���Ҧ����a�Y���|���
struct PG_Guild_DtoC_ChangeGuildName
{
	GamePGCommandEnum	Command;
	int					GuildID;
	StaticString <_MAX_NAMESTRING_LEN_>	GuildName;	//���|�W��
	

	PG_Guild_DtoC_ChangeGuildName( )
	{	Command = EM_PG_Guild_DtoC_ChangeGuildName;	}
};

//////////////////////////////////////////////////////////////////////////
//���|�ɯ�
//////////////////////////////////////////////////////////////////////////
//Client -> Local 
//�n�D���|�ɯ�
struct PG_Guild_CtoL_GuildUpgradeRequest
{
	GamePGCommandEnum	Command;
	int			GuildLv;

	PG_Guild_CtoL_GuildUpgradeRequest( )
	{	Command = EM_PG_Guild_CtoL_GuildUpgradeRequest;	}
};

//Local -> DataCenter
//�n�D���|�ɯ�
struct PG_Guild_LtoD_GuildUpgradeRequest
{
	GamePGCommandEnum	Command;
	int			GuildID;
	int			GuildLv;
	int			PlayerDBID;		//�n�D��

	PG_Guild_LtoD_GuildUpgradeRequest( )
	{	Command = EM_PG_Guild_LtoD_GuildUpgradeRequest;	}
};

//DataCenter->Client
//�^�����|�ɯŵ��G
struct PG_Guild_DtoC_GuildUpgradeResult
{
	GamePGCommandEnum	Command;
	int			GuildLv;
	bool		Result;

	PG_Guild_DtoC_GuildUpgradeResult( )
	{	Command = EM_PG_Guild_DtoC_GuildUpgradeResult;	}
};

//////////////////////////////////////////////////////////////////////////
enum AddGuildResourceTypeENUM
{
	EM_AddGuildResourceType_Contribution	,	//���m
	EM_AddGuildResourceType_Build			,	//���|�ؿv
	EM_AddGuildResourceType_Upgrade			,	//���|�ؿv�ɯ�
	EM_AddGuildResourceType_Plot			,	//�@��
	EM_AddGuildResourceType_Tax				,	//�ؿv�������O
	EM_AddGuildResourceType_Active			,	//�ؿv���P��
	EM_AddGuildResourceType_Buy_ItemPage	,	//�ʶR���|�񪫫~������
	EM_AddGuildResourceType_BuildDestroy	,	//�R���ؿv��
	EM_AddGuildResourceType_Buy_GuildStyle	,	//�ʶR���|�˦�
};
//�W�[���|�n��
//Local -> Datacenter
struct PG_Guild_LtoD_AddGuildResource
{
	GamePGCommandEnum	Command;
	int					GuildID;
	int					PlayerDBID;
	GuildResourceStruct	Resource;

	AddGuildResourceTypeENUM LogType;
	int					LogType_ObjID;
	

	StaticString <_MAX_NAMESTRING_LEN_>	Ret_Runplot;	//�^�ǫ�Ȧ檺�@��   bool Runplot( Result )

	PG_Guild_LtoD_AddGuildResource( )
	{	Command = EM_PG_Guild_LtoD_AddGuildResource;	}
};

//�W�[���|�n��
//DataCenter -> Local
struct PG_Guild_DtoL_AddGuildResourceResult
{
	GamePGCommandEnum	Command;
	int					GuildID;
	int					PlayerDBID;
	GuildResourceStruct	Resource;

	StaticString <_MAX_NAMESTRING_LEN_>	Ret_Runplot;	//�^�ǫ�Ȧ檺�@��   bool Runplot( Result )
	bool				Result;

	PG_Guild_DtoL_AddGuildResourceResult( )
	{	Command = EM_PG_Guild_DtoL_AddGuildResourceResult;	}
};
//////////////////////////////////////////////////////////////////////////
//�n�D�Y�Ѫ����|���m���
//Client -> Local
struct PG_Guild_CtoL_SelectContributionLog
{
	GamePGCommandEnum	Command;
	int					DayBefore;			//�X�ѫe

	PG_Guild_CtoL_SelectContributionLog( )
	{	Command = EM_PG_Guild_CtoL_SelectContributionLog;	}
};

//�n�D�Y�Ѫ����|���m���
//Local -> DataCenter
struct PG_Guild_LtoD_SelectContributionLog
{
	GamePGCommandEnum	Command;
	int					DayBefore;			//�X�ѫe
	int					PlayerDBID;

	PG_Guild_LtoD_SelectContributionLog( )
	{	Command = EM_PG_Guild_LtoD_SelectContributionLog;	}
};

//�n�D�Y�Ѫ����|���m���
//DataCenter -> Client
struct ContributionInfoLogStruct
{
	int		PlayerDBID;
	int		Time;
	GuildResourceStruct Resource;
};
struct PG_Guild_DtoC_SelectContributionLogResult
{
	GamePGCommandEnum	Command;
	int		Count;
	ContributionInfoLogStruct Log[500];

	int Size()
	{
		return sizeof(int)*2 + Count* sizeof(ContributionInfoLogStruct);
	}

	PG_Guild_DtoC_SelectContributionLogResult( )
	{	Command = EM_PG_Guild_DtoC_SelectContributionLogResult;	}
};

//���|��Ʀ��Q��s
//DataCenter -> Client
struct PG_Guild_DtoC_GuildBaseInfoUpdate
{
	GamePGCommandEnum	Command;
	PG_Guild_DtoC_GuildBaseInfoUpdate( )
	{	Command = EM_PG_Guild_DtoC_GuildBaseInfoUpdate;	}
};

//////////////////////////////////////////////////////////////////////////
//�q�����|�ɯ�
//DataCenter -> Client
struct PG_Guild_DtoC_Guild_LevelUp
{
	GamePGCommandEnum	Command;
	int					GuildID;
	int					Level;
	PG_Guild_DtoC_Guild_LevelUp( )
	{	Command = EM_PG_Guild_DtoC_Guild_LevelUp;	}
};

//////////////////////////////////////////////////////////////////////////
//�]�w���|�ݧ�W
//Local -> DataCenter
struct PG_Guild_LtoD_SetNeedChangeName
{
	GamePGCommandEnum	Command;
	int					GuildID;
	PG_Guild_LtoD_SetNeedChangeName( )
	{	Command = EM_PG_Guild_LtoD_SetNeedChangeName;	}
};
//////////////////////////////////////////////////////////////////////////

//�q�����|���������|��Ƨ�s
//DataCenter -> Client
struct PG_Guild_DtoC_GuildInfoChange
{
	GamePGCommandEnum		Command;
	GuildInfoChangeStruct	ChangeMode;

	PG_Guild_DtoC_GuildInfoChange( )
	{	Command = EM_PG_Guild_DtoC_GuildInfoChange;	}
};
//////////////////////////////////////////////////////////////////////////
//�q��DataCenter ���|�Կn������
struct PG_Guild_LtoD_GuildWarEndResult
{
	GamePGCommandEnum	Command;
	int						GuildID;
	GameResultENUM			GameResult;
	int						GuildScore;		//���|�n���ץ�
	GuildWarBaseInfoStruct	TargetGuild;
	PG_Guild_LtoD_GuildWarEndResult()
	{Command = EM_PG_Guild_LtoD_GuildWarEndResult;}
};
//////////////////////////////////////////////////////////////////////////
//Client -> Local
//�n�D���|�Ծ��v���
struct PG_Guild_CtoL_GuildWarHistroy
{
	GamePGCommandEnum	Command;

	PG_Guild_CtoL_GuildWarHistroy( )
	{	Command = EM_PG_Guild_CtoL_GuildWarHistroy;	}
};

//Local -> DataCenter
//�n�D���|�Ծ��v���
struct PG_Guild_LtoD_GuildWarHistroy
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;

	PG_Guild_LtoD_GuildWarHistroy( )
	{	Command = EM_PG_Guild_LtoD_GuildWarHistroy;	}
};

//DataCenter -> Client
//�n�D���|�Ծ��v��Ƶ��G
struct PG_Guild_DtoC_GuildWarHistroyResult
{
	GamePGCommandEnum	Command;
	int							Count;
	GuildHouseWarHistoryStruct	Log[50];

	PG_Guild_DtoC_GuildWarHistroyResult( )
	{	Command = EM_PG_Guild_DtoC_GuildWarHistroyResult;	}

	int PGSize()
	{
		return sizeof(int)*2 + Count* sizeof(GuildHouseWarHistoryStruct);
	}

};