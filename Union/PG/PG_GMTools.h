#pragma once
#pragma warning (disable:4949)
#include <windows.h>
#include "NetWakerPGEnum.h"
#include "PG/PG_GMToolsStruct.h"
#include "roledata/RoleDataEx.h"

#pragma unmanaged

//-----------------------------------------------------------------------------------------
// Local -> GM
// �e�ϰ��T
struct PG_GM_LToGM_ZoneInfo
{
    GamePGCommandEnum	Command;
    ZoneInfoStruct      Info;

    PG_GM_LToGM_ZoneInfo( )
    {	Command = EM_PG_GM_LToGM_ZoneInfo;	}
};
//-----------------------------------------------------------------------------------------
// Local -> GM
// �e�ϰ쪱�a���
struct PG_GM_LToGM_PlayerInfo
{
    GamePGCommandEnum	Command;
    PlayerInfoStruct    Info;

    PG_GM_LToGM_PlayerInfo( )
    {	Command = EM_PG_GM_LToGM_PlayerInfo;	}
};
//-----------------------------------------------------------------------------------------
// Local -> GM
// �e�ϰ�NPC���
struct PG_GM_LToGM_NPCInfo
{
    GamePGCommandEnum	Command;
    NPCInfoStruct       Info;

    PG_GM_LToGM_NPCInfo( )
    {	Command = EM_PG_GM_LToGM_NPCInfo;	}
};
//-----------------------------------------------------------------------------------------
// Local -> GM
// �e�ϰ쪱�a���
struct PG_GM_LToGM_DelPlayer
{
    GamePGCommandEnum	Command;
    int             GUID;
    int             ZoneID;
	int				DBID;

    PG_GM_LToGM_DelPlayer( )
    {	Command = EM_PG_GM_LToGM_DelPlayer;	}
};
//-----------------------------------------------------------------------------------------
// Local -> GM
// �e�ϰ�NPC���
struct PG_GM_LToGM_DelNPC
{
    GamePGCommandEnum	Command;
    int             GUID;
    int             ZoneID;

    PG_GM_LToGM_DelNPC( )
    {	Command = EM_PG_GM_LToGM_DelNPC;	}
};
//-----------------------------------------------------------------------------------------
//�n�D�Y�@���㹳���󪺸��
struct PG_GM_GMtoL_ObjectDataRequest
{
    GamePGCommandEnum	Command;
    int                 GUID;       //���󸹽X

    PG_GM_GMtoL_ObjectDataRequest( )
    {	Command = EM_PG_GM_GMtoL_ObjectDataRequest;	}
};
//-----------------------------------------------------------------------------------------
//�^�ǬY�@���㹳���󪺸��
struct PG_GM_LtoGM_ObjectData
{
    GamePGCommandEnum	Command;
    RoleDataEx          Role;   

    PG_GM_LtoGM_ObjectData( )
    {	Command = EM_PG_GM_LtoGM_ObjectData;	}
};
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
//���a�e�T���� GMTools
struct PG_GM_LtoGM_SendMsg
{
    GamePGCommandEnum	Command;

    StaticString <_MAX_NAMESTRING_LEN_> Name;
//    StaticString <_MAX_NAMESTRING_LEN_> Account;
    int 	                            ContentSize;
    StaticString <256>                  Content;

    PG_GM_LtoGM_SendMsg( )
    {	Command = EM_PG_GM_LtoGM_SendMsg;	}
};
//-----------------------------------------------------------------------------------------
//GMTools�e�T���a
struct PG_GM_GMtoL_SendMsg
{
    GamePGCommandEnum	Command;
    int                                 GUID;
    StaticString <_MAX_NAMESTRING_LEN_> Name;
    int 	                            ContentSize;
    StaticString <256>                  Content;

    PG_GM_GMtoL_SendMsg( )
    {	Command = EM_PG_GM_GMtoL_SendMsg;	}
};
//-----------------------------------------------------------------------------------------
//�t�ΰT��
struct PG_GM_LtoGM_SrvSysMsg
{
    GamePGCommandEnum	Command;
    int                                 LV;
//    StaticString <_MAX_NAMESTRING_LEN_> SrvName;
    StaticString <64>                   Title;
    StaticString <256>                  Msg;

    PG_GM_LtoGM_SrvSysMsg( )
    {	Command = EM_PG_GM_LtoGM_SrvSysMsg;	}
};
//-----------------------------------------------------------------------------------------
//�n�D���� GM ���O
struct PG_GM_GMtoL_SendGMCommand
{
	GamePGCommandEnum	Command;
	int					GUID;
	int					ManageLv;
	StaticString <64>   Account;
	int 	            ContentSize;
	StaticString <256>  Content;


	PG_GM_GMtoL_SendGMCommand( )
	{	Command = EM_PG_GM_GMtoL_SendGMCommand;	}
};
//-----------------------------------------------------------------------------------------
enum ServerStateRequestTypeENUM
{
	EM_ServerStateRequestType_Normal	,
	EM_ServerStateRequestType_Unknow	,
};
//�q��Server �n�D�ثe��Server��T
struct PG_GM_GMtoX_ServerStateRequest
{
	GamePGCommandEnum			Command;
	ServerStateRequestTypeENUM	RequestType;
	PG_GM_GMtoX_ServerStateRequest( )
	{	Command = EM_PG_GM_GMtoX_ServerStateRequest;	}
};
//-----------------------------------------------------------------------------------------
//�^��GMTools  �UServer���򥻪��A
struct ServerStateArgStruct
{
//	StaticString <32>	Type;
//	StaticString <64>	Value;
};
struct PG_GM_XtoGM_ServerStateResult
{
	GamePGCommandEnum	Command;
	ServerStateRequestTypeENUM	RequestType;

	StaticString <32>	Name;
	StaticString <32>	Type;
	StaticString <32>	State;

	//ServerStateArgStruct Info[4];
	StaticString <64>	Content[8];

	PG_GM_XtoGM_ServerStateResult( )
	{	Command = EM_PG_GM_XtoGM_ServerStateResult;	}
};
//-----------------------------------------------------------------------------------------
#pragma managed