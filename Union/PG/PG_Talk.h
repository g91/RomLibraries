#ifndef __PG_TALK_H__
#define __PG_TALK_H__
#pragma warning (disable:4949)
#include "roledata/RoleBaseDef.h"
#include "NetWakerPGEnum.h"
#pragma unmanaged

//#define MAX_TALK_CONTENT				512

//----------------------------------------------------------------------------------------
// �ƥ�T�����O
enum	EM_EVENT_MSG
{
	EM_EVENT_MSG_GIVEQUESTITEM		= 0,
};

// �o�e�W�D�̸�T
struct TalkSenderInfo
{
	union {
		int __data[2];
		struct {
			char	vocation[2];				// �D¾[0] ��¾[1] Voc_ENUM
			char	level[2];					// �D��¾����
		};
	};

	TalkSenderInfo() {
		memset(__data, 0, sizeof(__data));
	};
};

//----------------------------------------------------------------------------------------
//�P�Y���[��@�͸�
struct PG_Talk_TalkTo
{
    GamePGCommandEnum	Command;

	StaticString <_MAX_NAMESTRING_LEN_> SenderName;
	StaticString <_MAX_NAMESTRING_LEN_> TargetName;
	TalkSenderInfo						SenderInfo;			//�o�e�W�D�̸�T
	bool								IsSystem;			//�t�εo�X���T��	
	Voc_ENUM							Job;
    int 	                            ContentSize;	
    StaticString <MAX_TALK_CONTENT>     Content;
    //char	Content[512];

    PG_Talk_TalkTo()
    {	
		Command = EM_PG_Talk_TalkTo;	
	};

    int	Size()
    {
        return sizeof(*this)-sizeof(Content)+ContentSize;
    }

    static int SizeBase()
    {
        PG_Talk_TalkTo Temp;
        return sizeof( Temp ) - sizeof( Temp.Content ) + 1;
    }
    bool CheckPG( )
    {
        if( ContentSize >= sizeof( Content ) )
            return false;
        return true;
    }
};
//----------------------------------------------------------------------------------------
//�d����
struct PG_Talk_Range
{
    GamePGCommandEnum	Command;

    int			GItemID;
	Voc_ENUM	Job;
    StaticString <_MAX_NAMESTRING_LEN_> Name;
	TalkSenderInfo						SenderInfo;			//�o�e�W�D�̸�T
	bool								IsSystem;			//�t�εo�X���T��
	bool								IsNPCGossip;		// �O�_ NPC �b����

    int 	                            ContentSize;
	StaticString <MAX_TALK_CONTENT>     Content;

    PG_Talk_Range()
    {	
		Command		= EM_PG_Talk_Range;	
		IsNPCGossip	= false;
	};

    int	Size()
    {
        return sizeof(*this)-sizeof(Content)+ContentSize;
    }

    static int SizeBase()
    {
        PG_Talk_Range Temp;
        return sizeof( Temp ) - sizeof( Temp.Content ) + 1;
    }
    bool CheckPG( )
    {

        if( ContentSize > sizeof( Content ) )
            return false;
        return true;
    }
};
//-----------------------------------------------------------------------------------------------------
enum  TalkChannelTypeENUM
{
	EM_TalkChannelType_Normal	,
	EM_TalkChannelType_Sys		,
	EM_TalkChannelType_GM		,
	
};
//�W�D����
struct PG_Talk_Channel
{
	GamePGCommandEnum	Command;

	GroupObjectTypeEnum Type;
	int					ChannelID;
	TalkChannelTypeENUM	MsgType;
	Voc_ENUM			Job;

	StaticString <_MAX_NAMESTRING_LEN_> Name;
	TalkSenderInfo						SenderInfo;			//�o�e�W�D�̸�T
	int 	                            ContentSize;
	StaticString <MAX_TALK_CONTENT>     Content;


	PG_Talk_Channel()
	{	Command = EM_PG_Talk_Channel;	};

	int	Size()
	{
		return sizeof(*this)-sizeof(Content)+ContentSize;
	}

	static int SizeBase()
	{
		PG_Talk_Channel Temp;
		return sizeof( Temp ) - sizeof( Temp.Content ) + 1;
	}
	bool CheckPG( )
	{

		if( ContentSize >= sizeof( Content ) )
			return false;
		return true;
	}
};

struct PG_Talk_LtoChat_SysChannel
{
	GamePGCommandEnum	Command;

	GroupObjectTypeEnum Type;
	int					ChannelID;
	int					PlayerDBID;
	TalkChannelTypeENUM	MsgType;

	StaticString <_MAX_NAMESTRING_LEN_> Name;
	TalkSenderInfo						SenderInfo;			//�o�e�W�D�̸�T
	int 	                            ContentSize;
	StaticString <MAX_TALK_CONTENT>     Content;


	PG_Talk_LtoChat_SysChannel()
	{	Command = EM_PG_Talk_LtoChat_SysChannel;	};

	int	Size()
	{
		return sizeof(*this)-sizeof(Content)+ContentSize;
	}

	static int SizeBase()
	{
		PG_Talk_Channel Temp;
		return sizeof( Temp ) - sizeof( Temp.Content ) + 1;
	}
	bool CheckPG( )
	{

		if( ContentSize >= sizeof( Content ) )
			return false;
		return true;
	}
};
//-----------------------------------------------------------------------------------------------------
//�T��
struct PG_Talk_LtoC_SystemMsg
{
    GamePGCommandEnum	Command;

    int 	            ContentSize;
    StaticString <MAX_TALK_CONTENT>  Content;


    PG_Talk_LtoC_SystemMsg()
    {	Command = EM_PG_Talk_LtoC_SystemMsg;	};

    int	Size()
    {
        return sizeof(*this)-sizeof(Content)+ContentSize;
    }
    static int SizeBase()
    {
        PG_Talk_LtoC_SystemMsg Temp;
        return sizeof( Temp ) - sizeof( Temp.Content ) + 1;
    }
    bool CheckPG( )
    {

        if( ContentSize >= sizeof( Content ) )
            return false;
        return true;
    }
};
//-----------------------------------------------------------------------------------------------------
//GM�R�O�n�D
struct PG_Talk_CtoL_GMCommand
{
    GamePGCommandEnum	Command;

    int		            GItemID;
    int 	            ContentSize;
    StaticString <MAX_TALK_CONTENT>  Content;


    PG_Talk_CtoL_GMCommand()
    {	Command = EM_PG_Talk_CtoL_GMCommand;	};

    int	Size()
    {
        return sizeof(*this)-sizeof(Content)+ContentSize;
    }
    static int SizeBase()
    {
        PG_Talk_CtoL_GMCommand Temp;
        return sizeof( Temp ) - sizeof( Temp.Content ) + 1;
    }
    bool CheckPG( )
    {

        if( ContentSize >= sizeof( Content ) )
            return false;
        return true;
    }
};
//-----------------------------------------------------------------------------------------------------
//���a�e�T���� GMTools
// Client -> GMTools
struct PG_GM_CtoL_SendMsg
{
    GamePGCommandEnum	Command;
    int 	            ContentSize;
    StaticString <MAX_TALK_CONTENT>  Content;

    PG_GM_CtoL_SendMsg( )
    {	Command = EM_PG_GM_CtoL_SendMsg;	}

    bool CheckPG( )
    {
        if( ContentSize >= sizeof( Content ) )
            return false;
        return true;
    }
};
//-----------------------------------------------------------------------------------------
//GMTools�e�T���a
// GMTools -> Client
struct PG_GM_LtoC_SendMsg
{
    GamePGCommandEnum	Command;
    int 	            ContentSize;
    StaticString <MAX_TALK_CONTENT>  Content;

    PG_GM_LtoC_SendMsg( )
    {	Command = EM_PG_GM_LtoC_SendMsg;	}
};
//-----------------------------------------------------------------------------------------
//�t�ΰT���q��
// Local -> Client
struct PG_GM_LtoC_SendGameMessageEvent
{
	GamePGCommandEnum		Command;
	GameMessageEventENUM	Event;
	PG_GM_LtoC_SendGameMessageEvent( )
	{	Command = EM_PG_GM_LtoC_SendGameMessageEvent;	}
};
//-----------------------------------------------------------------------------------------

enum GameMessageDataTypeENUM
{
	EM_GameMessageDataType_None			,
	EM_GameMessageDataType_Int			,
	EM_GameMessageDataType_ObjID_Name	,
};

struct GameMessageDataArgStruct
{	
	GameMessageDataTypeENUM Type;
	int						Value;
};


//�t�ΰT���q��
// Local -> Client
struct PG_GM_LtoC_GameMessageDataEvent
{
	GamePGCommandEnum			Command;
	GameMessageEventDataENUM	Event;
	
	GameMessageDataArgStruct	Data[5];
	//StaticString <64>			StringData;

	int							iNumArg;

	
	PG_GM_LtoC_GameMessageDataEvent( )
	{	Command = EM_PG_GM_LtoC_GameMessageDataEvent;	}
};
//-----------------------------------------------------------------------------------------

enum EM_SCRIPTMESSAGE
{
	EM_SCRIPTMESSAGE_NOTICE		= 0,
	EM_SCRIPTMESSAGE_WARNING	= 1,
	EM_SCRIPTMESSAGE_SYSTEM		= 2,
	EM_SCRIPTMESSAGE_BOTH		= 3
};
//-----------------------------------------------------------------------------------------
//�q���]���O
struct PG_Talk_LtoC_RunningMsg
{
	GamePGCommandEnum	Command;

	int 	            ContentSize;
	StaticString <512>  Content;


	PG_Talk_LtoC_RunningMsg()
	{	Command = EM_PG_Talk_LtoC_RunningMsg;	};

	int	Size()
	{
		return sizeof(*this)-sizeof(Content)+ContentSize;
	}
	static int SizeBase()
	{
		PG_Talk_LtoC_RunningMsg Temp;
		return sizeof( Temp ) - sizeof( Temp.Content ) + 1;
	}
	bool CheckPG( )
	{
		if( ContentSize >= sizeof( Content ) )
			return false;
		return true;
	}
};
//-----------------------------------------------------------------------------------------
enum RunningMsgExTypeENUM
{
	EM_RunningMsgExType_GM			,
	EM_RunningMsgExType_Monitor		,
	EM_RunningMsgExType_Shop		,
	EM_RunningMsgExType_Other		,
};
//�q���]���O
struct PG_Talk_LtoC_RunningMsgEx
{
	GamePGCommandEnum		Command;
	RunningMsgExTypeENUM	Type;
	int 					ContentSize;
	StaticString <512>		Content;


	PG_Talk_LtoC_RunningMsgEx()
	{	Command = EM_PG_Talk_LtoC_RunningMsgEx;	};

	int	Size()
	{
		return sizeof(*this)-sizeof(Content)+ContentSize;
	}
	static int SizeBase()
	{
		PG_Talk_LtoC_RunningMsgEx Temp;
		return sizeof( Temp ) - sizeof( Temp.Content ) + 1;
	}
	bool CheckPG( )
	{
		if( ContentSize >= sizeof( Content ) )
			return false;
		return true;
	}
};
//-----------------------------------------------------------------------------------------
//���X�Y�H�����u�T��
//Local -> DataCenter
struct PG_Talk_LtoD_OfflineMessageRequest
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;

	PG_Talk_LtoD_OfflineMessageRequest()
	{	Command = EM_PG_Talk_LtoD_OfflineMessageRequest;	};
};
//-----------------------------------------------------------------------------------------
//���X�Y�H�����u�T��
//DataCenter -> Client
struct PG_Talk_DtoC_OfflineMessage
{
	GamePGCommandEnum	Command;
	int					FromDBID;
	float				Time;
	StaticString <_MAX_NAMESTRING_LEN_> FromName;
	StaticString <_MAX_NAMESTRING_LEN_> ToName;

	StaticString <512>  Content;

	PG_Talk_DtoC_OfflineMessage()
	{	Command = EM_PG_Talk_DtoC_OfflineMessage;	};
};
//-----------------------------------------------------------------------------------------
//���X�Y�H�����u�T��( GM �R�O)
//DataCenter -> Local
struct PG_Talk_DtoL_OfflineGMCommand
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	ManagementENUM		ManageLv;
	StaticString <512>  Content;

	PG_Talk_DtoL_OfflineGMCommand()
	{	Command = EM_PG_Talk_DtoL_OfflineGMCommand;	};
};
//////////////////////////////////////////////////////////////////////////
//Client ��T������
//////////////////////////////////////////////////////////////////////////
//���U���G����
//Client -> Chat
struct PG_Talk_CtoChat_RegisterBillBoardInfo
{
	GamePGCommandEnum	Command;
	int					DataSize;
	char				Data[ 1024 ];

	PG_Talk_CtoChat_RegisterBillBoardInfo()
	{	Command = EM_PG_Talk_CtoChat_RegisterBillBoardInfo;	};
};
//////////////////////////////////////////////////////////////////////////
//�q���Ҧ��H���U��T
//Chat -> Client
struct PG_Talk_ChattoC_BillBoardInfo
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;
	int					DataSize;
	char				Data[ 1024 ];

	PG_Talk_ChattoC_BillBoardInfo()
	{	Command = EM_PG_Talk_ChattoC_BillBoardInfo;	};

	int Size() 
	{
		return sizeof( *this ) - sizeof( Data ) + DataSize;
	}
};
//////////////////////////////////////////////////////////////////////////
//�]�w���U��ƥͦs��
//Client -> Chat
struct PG_Talk_CtoChat_BillBoardLiveTime
{
	GamePGCommandEnum	Command;
	int					LiveTime;	//��

	PG_Talk_CtoChat_BillBoardLiveTime()
	{	Command = EM_PG_Talk_CtoChat_BillBoardLiveTime;	};
};
//////////////////////////////////////////////////////////////////////////
//�R�����
//Chat -> Client
struct PG_Talk_ChattoC_BillBoardDelete
{
	GamePGCommandEnum	Command;
	int					PlayerDBID;

	PG_Talk_ChattoC_BillBoardDelete()
	{	Command = EM_PG_Talk_ChattoC_BillBoardDelete;	};
};

//////////////////////////////////////////////////////////////////////////
#pragma managed
#endif //__PG_TALK_H__