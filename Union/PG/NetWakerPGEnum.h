#ifndef __NETWAKERPGEM_H__
#define __NETWAKERPGEM_H__

//-----------------------------------------------------------------------------------
#include "NetWaker/NetWakerDefine.h"
#include "SmallObj/StaticString.h"
//-----------------------------------------------------------------------------------
#pragma		warning (disable:4200)
#define _MAX_NAMESTRING_LEN_		    64          //角色名稱長度
//#define _Def_Max_ZoneID_                255         //最多給個 區域
#define _Def_Max_ItemID_                10000       //區域最多幾個物件
#define _Def_WorldItemID_Base_          100000      //基本區域配發id的區塊大小
//-----------------------------------------------------------------------------------
#define Def_Item_Pos_Body		0
#define Def_Item_Pos_Bank		1
#define Def_Item_Pos_EQ     	2
#define Def_Item_Pos_BodyMoney	3
#define Def_Item_Pos_BankMoney	4
//-----------------------------------------------------------------------------------
enum	GamePGCommandEnum
{
    EM_PG__NULL											= -1,
    // 測試資料
    EM_PG_Test_CliToGSrv								= 0,
    EM_PG_Test_GSrvToCli								,
    EM_PG_Test_GSrvToAllCli								,
    EM_PG_Test_GSrvToGSrv								,

	// 角色連結 GSrv 的封包
	EM_PG_CliConnect_LtoC_SysVersionInfo				= 8,
	EM_PG_CliConnect_LtoC_LogoutResult					= 9,
    EM_PG_CliConnect_LtoC_PlayerItemID					= 10,
    EM_PG_CliConnect_LtoC_ZoneInfo						,
    EM_PG_CliConnect_LtoC_PlayerData					,    
    EM_PG_CliConnect_LtoC_PrepareChangeZone				,
    EM_PG_CliConnect_LtoC_ChangeZoneOK					,
    EM_PG_CliConnect_CtoL_RequestLogout					,
    EM_PG_CliConnect_LtoL_ChangeZoneNodify				,
    EM_PG_CliConnect_LtoC_ModifyNPCInfo					,
    EM_PG_CliConnect_CtoL_ModifyNPCInfo					,
	EM_PG_CliConnect_LtoL_ChangeZoneNodifyResult		,

    // 動做位置 的封包
    EM_PG_Move_LtoC_AddToPartition						= 20,
    EM_PG_Move_LtoC_DelFromPartition					,
    EM_PG_Move_LtoC_SetPostion							,
    EM_PG_Move_LtoC_ObjectMove							,
    EM_PG_Move_CtoL_PlayerMoveObject					,
    EM_PG_Move_LtoC_DoAction							,
    EM_PG_Move_LtoC_MoveSpeed							,
    EM_PG_Move_LtoC_ModelRate							,
    EM_PG_Move_LtoC_EQInfo								,
    EM_PG_Move_LtoC_FixEQInfo							,
    EM_PG_Move_LtoC_LookFace							,
    EM_PG_Move_CtoL_DoAction							,
    EM_PG_Move_LtoC_ClientAutoPlot						,
    EM_PG_Move_LtoC_ActionType							,
    EM_PG_Move_CtoL_JumpBegin							,
    EM_PG_Move_LtoC_JumpBegin							,
    EM_PG_Move_CtoL_JumpEnd								,
    EM_PG_Move_LtoC_JumpEnd								,
    EM_PG_Move_LtoC_Relation							,
	EM_PG_Move_LtoC_PartyMemberMove						,
	EM_PG_Move_LtoC_PlayMotion							,
	EM_PG_Move_LtoC_SpecialMove							,
	EM_PG_Move_CtoL_Ping								,
	EM_PG_Move_LtoC_Ping								,
	EM_PG_Move_LtoC_ClientLoading						,
	EM_PG_Move_CtoL_ClientLoadingOK						,
	EM_PG_Move_LtoC_SetMinimapIcon						,
	EM_PG_Move_LtoC_SpecialEfficacy						,
	EM_PG_Move_LtoC_SetDir								,
	EM_PG_Move_LtoC_SetIdleMotion						,

	EM_PG_Move_LtoC_SetGuildID							,
	EM_PG_Move_LtoC_SetTitleID							,
	EM_PG_Move_LtoC_PlayerDropDownDamage				,
	EM_PG_Move_LtoC_ObjGuildBuildingInfo				,
	EM_PG_Move_LtoC_HorseColor							,
	EM_PG_Move_LtoC_ObjGuildFurnitureInfo				,
	EM_PG_Move_LtoC_MoveInfo							,
	EM_PG_Move_LtoC_RoomID								,

	EM_PG_Move_LtoC_PingLog								,
	EM_PG_Move_CtoL_PingLog								,

	EM_PG_Move_LtoC_DeadZone							,

	EM_PG_Move_LtoC_AttachObj							,
	EM_PG_Move_LtoC_DetachObj							,

	EM_PG_Move_LtoC_BattleGround_AddPlayer				,
	EM_PG_Move_LtoC_BattleGround_DelPlayer				,
	EM_PG_Move_LtoC_BattleGround_PlayerPos				,
	EM_PG_Move_LtoC_BattleGround_PlayerHPMP				,

	EM_PG_Move_CtoL_UnlockRolePosRequest				,
	EM_PG_Move_LtoC_UnlockRolePosOK						,

	EM_PG_Move_CtoL_AttachObjRequest					,
	EM_PG_Move_LtoC_AttachObjResult						,

	EM_PG_Move_LtoC_FlowerPotInfo						,
	EM_PG_Move_LtoC_UnlockRolePosTime					,
	EM_PG_Move_LtoC_ColdownInfo							,
	EM_PG_Move_LtoC_PlayerPos							,
	EM_PG_Move_CtoL_SetPartition						,
	EM_PG_Move_LtoC_PlayerMove							,

	EM_PG_Move_LtoC_BattleGround_PlayerInfo_Zip			,

	EM_PG_Move_LtoC_AllPlayerPos						,

	// SERVER LIST
	EM_PG_SL_SL2SL_RequestServerData					= 80,
	EM_PG_SL_SL2SL_RegsiterServerData					,
	EM_PG_SL_SL2SL_TransmitServerData					,
	EM_PG_SL_SL2SL_RegSrvDataFailed						,
	EM_PG_SL_SL2SL_LocalID								,
	EM_PG_SL_SL2SL_ServerReady							,
	EM_PG_SL_SL2SL_Shutdown								,
	EM_PG_SL_S2M_RegisterAccountEvent					,
	EM_PG_SL_M2S_AccountEnterWorld						,
	EM_PG_SL_M2S_AccountLeaveWorld						,

    EM_PG_ChannelBase_toChat_DataTransmitToGSrv_Group	= 100,
    EM_PG_ChannelBase_toChat_DataTransmitToCli_Group    ,
    EM_PG_ChannelBase_toChat_DataTransmitToSrv          ,
    EM_PG_ChannelBase_toChat_DataTransmitToCli          ,

    EM_PG_ChannelBase_LtoChat_CreateChannel             ,
    EM_PG_ChannelBase_ChattoL_CreateChannelResult       ,
    EM_PG_ChannelBase_LtoChat_AddUser                   ,
    EM_PG_ChannelBase_ChattoL_AddUserResult             ,
    EM_PG_ChannelBase_LtoChat_DelUser                   ,
    EM_PG_ChannelBase_ChattoL_DelUserResult             ,
	
	EM_PG_ChannelBase_CtoL_CreateChannel				,
	EM_PG_ChannelBase_LtoC_CreateChannelResult			,
	EM_PG_ChannelBase_CtoL_JoinChannel					,
	EM_PG_ChannelBase_LtoC_JoinChannelResult			,
	EM_PG_ChannelBase_CtoL_LeaveChannel					,
	EM_PG_ChannelBase_LtoC_LeaveChannelResult			,
	EM_PG_ChannelBase_CtoChat_DataTransmitToCli_Group	,
	EM_PG_ChannelBase_ChattoC_DataTransmitCliToCli		,

    EM_PG_ChannelBase_LtoChat_ChangeRoleInfo            ,
	EM_PG_ChannelBase_ChattoC_ClientConnectToChat		,

	EM_PG_ChannelBase_ChattoL_AddorSetOnlinePlayer		,
	EM_PG_ChannelBase_ChattoL_DelOnlinePlayer			,
	EM_PG_ChannelBase_CtoChat_DataTransmitToCli_Name	,

	EM_PG_ChannelBase_CtoChat_ChannelPlayerList			,
	EM_PG_ChannelBase_ChattoC_ChannelPlayerListResult	,
	EM_PG_ChannelBase_CtoChat_JoinPlayerDisabled		,
	EM_PG_ChannelBase_ChattoC_JoinPlayerDisabledResult	,
	EM_PG_ChannelBase_CtoChat_KickPlayer				,
	EM_PG_ChannelBase_CtoChat_SetManager				,
	EM_PG_ChannelBase_ChattoC_SysChannel				,

	EM_PG_ChannelBase_CtoChat_ZoneChannelID				,
	EM_PG_ChannelBase_ChattoAll_SystemTime				,
	EM_PG_ChannelBase_LtoChat_SetDSysTime				,

	EM_PG_ChannelBase_ChattoC_AddUserNotify				,
	EM_PG_ChannelBase_ChattoC_DelUserNotify				,

	EM_PG_ChannelBase_ChattoC_SetManagerResult			,
	EM_PG_ChannelBase_ChattoC_ChannelOwner				,
	EM_PG_ChannelBase_ChattoC_PlayerNotFind				,

	EM_PG_ChannelBase_CtoL_DataTransmitToSrv			,
	EM_PG_ChannelBase_LtoSrv_DataTransmitToSrv			,

	EM_PG_ChannelBase_ChattoL_SetChannelOwner			,

    EM_PG_Talk_TalkTo									= 150,
    EM_PG_Talk_Range                                    ,
    EM_PG_Talk_Zone                                     ,
    EM_PG_Talk_LtoC_SystemMsg                           ,
    EM_PG_Talk_CtoL_GMCommand                           ,
	EM_PG_Talk_LtoC_EventMsg							,
	EM_PG_Talk_Channel									,

	EM_PG_Talk_CtoL_Party								,
	EM_PG_Talk_LtoC_Party								,
	EM_PG_Talk_CtoL_Guild								,
	EM_PG_Talk_LtoC_Guild								,

	EM_PG_Talk_LtoC_ScriptMessage						,
	EM_PG_Talk_LtoChat_SysChannel						,

    EM_PG_DCBase_XtoRD_SavePlayer						= 170,
    EM_PG_DBBase_RDtoX_SavePlayerError                  ,
    EM_PG_DCBase_LtoD_SaveNPCRequest					,
    EM_PG_DCBase_DtoL_SaveNPCResult						,
    EM_PG_DBBase_LtoD_DropNPCRequest                    ,
    EM_PG_DBBase_DtoL_DropNPCResult                     ,
    EM_PG_DBBase_LtoD_LoadAllNPC                        ,
    EM_PG_DBBase_DtoL_LoadAllNPCResult                  ,
    EM_PG_DBBase_XtoRD_SetPlayerLiveTime                ,
    EM_PG_DBBase_RDtoX_SetPlayerLiveTimeError           ,
    EM_PG_DBBase_XtoRD_SetPlayerLiveTime_ByAccount      ,
    EM_PG_DBBase_RDtoX_SetPlayerLiveTimeError_ByAccount ,
	EM_PG_DBBase_LtoD_LoadAllNPCMoveInfo				,
	EM_PG_DBBase_DtoL_NPCMoveInfoResult					,
	EM_PG_DBBase_LtoD_SaveNPCMoveInfo					,
	EM_PG_DBBase_LtoD_SqlCommand						,

	EM_PG_DBBase_XtoRD_CheckRoleDataSize				,
	EM_PG_DBBase_RDtoX_CheckRoleDataSizeResult			,
	EM_PG_DBBase_LtoD_ZoneInfoRequest					,
	EM_PG_DBBase_DtoL_ZoneInfoResult					,
	EM_PG_DBBase_DtoL_GlobalZoneInfoResult				,

	EM_PG_DBBase_LtoD_LogSqlCommand						,

	EM_PG_DBBase_DtoL_GmCommandRequest					,
	EM_PG_DBBase_LtoD_GmCommandResult					,
	EM_PG_DBBase_DtoL_CheckRoleLiveTime					,
	EM_PG_DBBase_RDtoX_CheckRoleStruct					,
	EM_PG_DBBase_DtoL_GlobalZoneInfoList				,
	EM_PG_DBBase_LtoD_LogData							,

	// Master Server
	// Login , Client 剛連入時, 取出資料用
	EM_PG_Login_M2C_IdentifyMaster						= 200,
	EM_PG_Login_M2D_LoadAccountData						,
	EM_PG_Login_D2M_RoleData							,
	EM_PG_Login_D2M_RoleDataCount						,
	EM_PG_Login_D2M_LoadAccountFailed					,

	EM_PG_Login_M2C_SampleRoleData						,
	EM_PG_Login_M2C_SampleRoleDataEnd					,

	EM_PG_Login_C2M_CreateRole							,
	EM_PG_Login_M2C_CreateRoleResult					,
	EM_PG_Login_M2D_CreateRole							,
	EM_PG_Login_D2M_CreateRoleResult					,

	EM_PG_Login_C2M_DeleteRole							,
	EM_PG_Login_M2C_DeleteRoleResult					,
	EM_PG_Login_M2D_DeleteRole							,
	EM_PG_Login_D2M_DeleteRoleResult					,

	EM_PG_Login_C2M_SelectRole							,	
	EM_PG_Login_M2L_SetRoleToWorld						,
	EM_PG_Login_L2M_SetRoleToWorldResult				,
	EM_PG_Login_M2C_SelectRoleResult					,

	EM_PG_Login_M2C_EnterWorld							,
	EM_PG_Login_M2C_LeaveWorld							,
	
	EM_PG_Login_M2D_ClientLogout						,

	EM_PG_Login_M2C_LoginResult							,
	EM_PG_Login_C2M_LoadRoleData						,

	EM_PG_Login_M2C_LoadDataEvent						,

	EM_PG_Login_C2M_DelRoleRecoverRequest				,
	EM_PG_Login_M2D_DelRoleRecoverRequest				,
	EM_PG_Login_D2C_DelRoleRecoverResult				,

	EM_PG_Login_M2C_SetSecondPasswordRequest			,
	EM_PG_Login_C2M_SetSecondPassword					,
	EM_PG_Login_M2D_SetSecondPassword					,
	EM_PG_Login_C2M_ZoneLoadingRequest					,
	EM_PG_Login_M2C_ZoneLoading							,

	EM_PG_Nonage_Nonage2M_SetPlayerHours				,

	EM_PG_Login_M2C_APEXTOCLIENT						,
	EM_PG_Login_C2M_APEXTOSERVER						,

	EM_PG_BTI_BTI2L_BTIPlayTime							,
	EM_PG_BTI_BTI2M_SetPlayerHours						,

	EM_PG_Login_APEX2C_APEXSERVERID						,
	EM_PG_Login_C2APEX_APEXSTARTVALUE					,

	EM_PG_Login_C2M_ChangeNameRequest					,
	EM_PG_Login_M2D_ChangeNameRequest					,
	EM_PG_Login_D2M_ChangeNameResult					,
	EM_PG_Login_M2C_ChangeNameResult					,
	EM_PG_Login_C2M_SelectRole_EnterZone1				,
	EM_PG_Login_LtoM_CloseMaster						,

	EM_PG_Nonage_M2L_HealthTime							,
	EM_PG_Login_MtoC_MaxRoleCount						,

	EM_PG_Login_MtoD_ReserveRoleRequest					,
	EM_PG_Login_DtoC_ReserveRole						,

	EM_PG_Login_MtoC_SecondPasswordRequest				,
	EM_PG_Login_CtoM_SecondPassword						,
	EM_PG_Login_MtoC_SecondPasswordResult				,
	EM_PG_Login_MtoP_GetObjCount						,
	EM_PG_Login_PtoM_GetObjCountResult					,
	EM_PG_Login_MtoC_SetGameGuard						,
	EM_PG_Login_CtoM_GameGuardReport					,

	EM_PG_Login_MtoC_VivoxAccount						,
	EM_PG_Login_CtoM_CreateVivoxAccount					,
	EM_PG_Login_MtoC_VivoxInfo							,

	EM_PG_Login_M2D_SetCheckSecondPasswordResult		,

	// LocalServer 和 Master 間狀態溝通
	EM_PG_ZoneStatus_L2M_RoleEnterZone					= 270,
	EM_PG_ZoneStatus_L2M_RoleLeaveZone					,
	EM_PG_ZoneStatus_M2L_RequestZoneSrvReg				,
    EM_PG_ZoneStatus_L2M_ZoneSrvReg						,
    EM_PG_ZoneStatus_M2L_ZoneSrvRegResult				,
	EM_PG_ZoneStatus_L2M_ZoneSrvOkay					,
	EM_PG_ZoneStatus_L2M_RoleLeaveWorld					,
	EM_PG_ZoneStatus_M2L_KickZonePlayer					,
	EM_PG_ZoneStatus_L2M_KickZonePlayerResult			,

	// LocalServer 送 Zone status 給 DBCenter
	EM_PG_ZoneStatus_L2D_RoleLeaveWorld					,
	EM_PG_ZoneStatus_L2D_RoleEnterZone					,

    //Partition 使用的封包
    EM_PG_PartBase_LtoP_SetLocal						= 300,
    EM_PG_PartBase_LtoP_AddObj                  		,
    EM_PG_PartBase_LtoP_MoveObj                 		,
    EM_PG_PartBase_LtoP_DelObj                  		,
    EM_PG_PartBase_PtoL_OnAddtoOtherSrv         		,
    EM_PG_PartBase_PtoL_OnAdd                   		,
    EM_PG_PartBase_PtoL_OnDelFromOtherSrv       		,
    EM_PG_PartBase_PtoL_OnDel                   		,
    EM_PG_PartBase_LtoP_DataTransfer            		,
    EM_PG_PartBase_PtoL_MoveObj                 		,
    EM_PG_PartBase_PtoL_ChangeZone              		,
	EM_PG_PartBase_PtoL_NotInControlArea				,

    //區域重疊資料傳輸
    EM_PG_Part_LtoL_NewMirrorRole						= 350,
    EM_PG_Part_LtoL_ModifyMirrorRole					,
    EM_PG_Part_LtoL_SettingStr							,
    EM_PG_Part_LtoL_SettingNumber						,

    //物品
    EM_PG_Item_CtoL_GetGroundItem						,
    EM_PG_Item_LtoC_GetItemResult						,
    EM_PG_Item_LtoC_ClsItemInBuffer						,
    EM_PG_Item_LtoC_NewItemInBuffer						,
    EM_PG_Item_CtoL_GetBufferItem						,
    EM_PG_Item_CtoL_DeleteItem							,
    EM_PG_Item_LtoC_DeleteItem							,
    EM_PG_Item_CtoL_ExchangeField						,
    EM_PG_Item_LtoC_ExchangeField						,
    EM_PG_Item_CtoL_RequestBodyBankMoneyExchange		,
    EM_PG_Item_LtoC_BodyBankMoneyExchange				,
    EM_PG_Item_CtoL_RequestBufferInfo					,
	EM_PG_Item_CtoL_ApartBodyItemRequest				,
    EM_PG_Item_LtoC_FixItemInfo							,
    EM_PG_Item_CtoL_RequestFixAllItemInfo				,
    EM_PG_Item_LtoC_FixAllItemInfo						,
    EM_PG_Item_LtoC_FixMoney							,
    EM_PG_Item_LtoC_MaxItemCount						,
    EM_PG_Item_LtoC_OpenBank							,
    EM_PG_Item_CtoL_CloseBank							,
	EM_PG_Item_LtoC_CloseBank							,
    EM_PG_Item_CtoL_UseItem								,
	EM_PG_Item_LtoC_UseItemResult						,
	EM_PG_Item_LtoC_SetKeyItemFlag						,
	EM_PG_Item_LtoC_ApartBodyItemResult					,

    //裝備
    EM_PG_EQ_CtoL_SetEQ									,
    EM_PG_EQ_LtoC_SetEQResult							,

	EM_PG_Item_CtoL_UseItemToItem_Notify				,
	EM_PG_Item_LtoC_UseItemToItem_Result				,
	EM_PG_Item_LtoC_UsePowerLight						,
	EM_PG_Item_LtoC_EqRefineResult						,
	EM_PG_Item_LtoC_SetTitleFlag						,

	EM_PG_Item_CtoL_ItemDissolutionRequest				,
	EM_PG_Item_LtoC_ItemDissolutionResult				,
	EM_PG_Item_LtoC_SetCardFlag							,
	EM_PG_Item_CtoL_TakeOutCardRequest					,
	EM_PG_Item_LtoC_TakeOutCardResult					,

	EM_PG_EQ_CtoL_SetShowEQ								,
	EM_PG_EQ_LtoC_EQBroken								,
	EM_PG_Item_LtoC_DigHoleResult						,
	EM_PG_Item_LtoC_EQCombinOpen						,
	EM_PG_Item_CtoL_EQCombinRequest						,
	EM_PG_Item_LtoC_EQCombinResult						,
	EM_PG_Item_LtoC_EQCombinClose						,
	EM_PG_Item_CtoL_EQCombinClose						,

	EM_PG_Item_LtoD_AccountBagInfoRequest				,
	EM_PG_Item_DtoL_AccountBagInfoResult				,
	EM_PG_Item_LtoC_OpenAccountBag						,
	EM_PG_Item_CtoL_AccountBagGetItemRequest			,
	EM_PG_Item_LtoC_AccountBagGetItemResult				,
	EM_PG_Item_CtoL_AccountBagClose						,

	EM_PG_EQ_LtoC_SetShowEQ								,

	EM_PG_Item_LtoC_ItemTimeout							,

	EM_PG_Item_CtoL_MagicBoxRequest						,
	EM_PG_Item_LtoC_MagicBoxResult						,
	EM_PG_Item_LtoC_UseEq								,

	EM_PG_EQ_CtoL_SwapBackupEQ							,
	EM_PG_EQ_LtoC_SwapBackupEQ							,

	EM_PG_Item_CtoL_GamebleItemState					,
	EM_PG_Item_LtoC_GamebleItemStateResult				,
	EM_PG_Item_CtoL_GetGamebleItemState					,
	EM_PG_Item_LtoC_GetGamebleItemStateResult			,
	EM_PG_Item_LtoC_DelItemMsg							,

	EM_PG_Item_LtoC_OpenGamebleItemState				,
	EM_PG_Item_CtoL_CloseGamebleItemState				,
	EM_PG_Item_LtoC_CloseGamebleItemState				,

	EM_PG_Item_LtoC_OpenMagicStoneExchange				,

    EM_PG_Attack_CtoL_AttackSignal						= 420,
    EM_PG_Attack_LtoC_AttackResult						,
    EM_PG_Attack_LtoC_Dead								,
    EM_PG_Attack_LtoC_Resurrection						,
    EM_PG_Attack_CtoL_RequestResurrection				,
    EM_PG_Attack_LtoC_HPMPSP							,
    EM_PG_Attack_LtoC_MaxHPMaxMPMaxSP					,
    EM_PG_Attack_CtoL_EndAttackSignal					,
    EM_PG_Attack_LtoC_AttackSignalResult				,
    
	EM_PG_Attack_LtoC_PartyMemberHPMPSP					,
	EM_PG_Attack_LtoC_PartyMemberMaxHPMPSP				,
	EM_PG_Attack_LtoC_NPCAttackTarget					,
	EM_PG_Attack_LtoC_SetAttackMode						,
	EM_PG_Attack_LtoC_ZonePKFlag						,

	EM_PG_Item_CtoL_MagicStoneExchangeResult			= 440,
	EM_PG_Item_LtoC_MagicStoneExchangeResult			,
	EM_PG_Item_CtoL_DrawOutRuneRequest					,
	EM_PG_Item_LtoC_DrawOutRuneResult					,

    //數值修正
    EM_PG_RoleValue_CtoL_RequestFixRoleValue			= 450,
    EM_PG_RoleValue_LtoC_FixAllRoleValue				,
    EM_PG_RoleValue_LtoC_FixRoleValue					,
    EM_PG_RoleValue_CtoL_RequestSetPoint				,
    EM_PG_RoleValue_LtoC_ResultSetPoint					,
    EM_PG_RoleValue_LtoC_NotifyLvUp						,
    EM_PG_RoleValue_LtoC_SetTitleFlag					,
    EM_PG_RoleValue_LtoC_SetFlag						,
    EM_PG_RoleValue_CtoL_SetTitlRequest					,
    EM_PG_RoleValue_LtoC_SetTitleResult					,
    EM_PG_RoleValue_LtoC_RoleInfoChange					,

	EM_PG_RoleValue_LtoC_OpenChangeJob					,
	EM_PG_RoleValue_CtoL_CloseChangeJobRequest			,	
	EM_PG_RoleValue_CtoL_ChangeJobRequest				,
	EM_PG_RoleValue_LtoC_ChangeJobResult				,
	EM_PG_RoleValue_LtoC_CloseChangeJob					,
	EM_PG_RoleValue_CtoL_ClientLogout					,

	EM_PG_RoleValue_CtoL_SetPlayerAllZoneState			,
	EM_PG_RoleValue_LtoC_SetPlayerAllZoneState			,
	EM_PG_RoleValue_CtoL_SetClientFlag					,
	EM_PG_RoleValue_LtoC_DeltaRoleValue					,
	EM_PG_RoleValue_LtoC_ClientLogoutResult				,
	EM_PG_RoleValue_LtoC_CancelClientLogout				,
	EM_PG_RoleValue_CtoL_CancelClientLogout				,


	EM_PG_RoleValue_CtoL_ChangeParallel					,
	EM_PG_RoleValue_CtoL_CancelChangeParallel			,
	EM_PG_RoleValue_LtoC_WaitChangeParallel				,

	EM_PG_RoleValue_LtoC_ClientComputerInfoRequest		,
	EM_PG_RoleValue_CtoL_ClientComputerInfo				,
	EM_PG_RoleValue_CtoL_SetClientData					,
	EM_PG_RoleValue_LtoC_ObjMode						,
	EM_PG_RoleValue_CtoL_ClientEnvironmentState			,
	EM_PG_RoleValue_LtoC_ObjRoleInfo					,
	EM_PG_RoleValue_LtoC_GetTargetRoleValue				,

    //玩家交易
    EM_PG_Trade_CtoL_RequestTrade						= 500,
    EM_PG_Trade_LtoC_RequestTrade						,
    EM_PG_Trade_CtoL_AgreeTrade							,
    EM_PG_Trade_LtoC_OpenTrade							,
    EM_PG_Trade_CtoL_PutItem							,
    EM_PG_Trade_LtoC_TargetPutItem						,
    EM_PG_Trade_CtoL_PutMoney							,
    EM_PG_Trade_LtoC_TargetPutMoney						,
    EM_PG_Trade_LtoC_OwnerPutMoney						,
    EM_PG_Trade_CtoL_ItemOK								,
    EM_PG_Trade_LtoC_TargetItemOK						,
    EM_PG_Trade_LtoC_CancelItemOK						,
    EM_PG_Trade_LtoC_FinalOKEnable						,
    EM_PG_Trade_CtoL_FinalOK							,
    EM_PG_Trade_LtoC_TargetFinalOK						,
    EM_PG_Trade_LtoC_ClearTradeItem						,
    EM_PG_Trade_LtoC_TradeItemResult					,
    EM_PG_Trade_CtoL_StopTrade							,
    EM_PG_Trade_LtoC_PutItemResult						,
	EM_PG_Trade_LtoC_RequestTradeError					,
	EM_PG_Trade_LtoC_DisagreeTrade						,
	EM_PG_Trade_LtoC_StopTrade							,
	EM_PG_Trade_LtoC_TradeState							,
	EM_PG_Trade_LtoC_CancelTradeRequest					,
	EM_PG_Trade_CtoL_CancelTradeRequest					,
	EM_PG_Trade_LtoC_FinalOKFailed						,

	// Quest 及 Script 系統
	EM_PG_Script_CtoL_QuestAccept						= 550,
	EM_PG_Script_CtoL_QuestComplete						,
	EM_PG_Script_CtoL_QuestDelete						,
	EM_PG_Script_CtoL_TouchEvent						,
	EM_PG_Script_CtoL_RangeEvent						,
	EM_PG_Script_CtoL_TouchObj							,
	EM_PG_Script_LtoC_QuestAccept_Result				,
	EM_PG_Script_LtoC_QuestComplete_Result				,
	EM_PG_Script_LtoC_QuestDelete_Result				,
	EM_PG_Script_LtoC_QuestFlag							,			// GM Command
	EM_PG_Script_CtoL_ClickQuestObj						,
	EM_PG_Script_CtoL_ClickQuestObjFailed				,
	EM_PG_Script_LtoC_ClickQuestObjResult				,
	EM_PG_Script_LtoC_KillQuestObjNotify				,
	EM_PG_Script_LtoC_PlayMotion						,
	EM_PG_Script_CtoL_LoadQuestSpeakOption				,
	EM_PG_Script_LtoC_SpeakOption						,
	EM_PG_Script_CtoL_ChoiceOption						,
	EM_PG_Script_LtoC_ChoiceOptionResult				,
	EM_PG_Script_LtoC_CloseSpeak						,
	EM_PG_Script_LtoC_PlaySound							,
	EM_PG_Script_CtoL_DelayPatrol						,
	EM_PG_Script_LtoC_SetQuestState						,

	EM_PG_Script_LtoC_ShowBorder						,
	EM_PG_Script_LtoC_AddBorderPage						,
	EM_PG_Script_LtoC_ClearBorder						,
	
	EM_PG_Script_CtoL_AcceptBorderQuest					,
	EM_PG_Script_CtoL_CloseBorder						,
	EM_PG_Script_LtoC_GetQuestObjNotify					,

	EM_PG_Script_CtoL_CollisionEvent					,
	EM_PG_Script_LtoC_ScriptCast						,
	EM_PG_Script_LtoC_ScriptCastResult					,
	EM_PG_Script_LtoC_ResetDailyQuest					,

	EM_PG_Script_LtoC_AttachCastMotionToolID			,
	EM_PG_Script_LtoC_StartClientCountDown				,

	EM_PG_Script_LtoC_AttachTool						,
	EM_PG_Script_LtoC_StopSound							,
	EM_PG_Script_LtoC_SetCameraFollowUnit				,

	EM_PG_Script_LtoC_Play3DSound						,
	EM_PG_Script_LtoC_PlayMusic							,

    //魔法封包
    EM_PG_Magic_CtoL_BeginSpell							= 600,
    EM_PG_Magic_CtoL_BeginSpellSP						,
    EM_PG_Magic_LtoC_BeginSpell							,
    EM_PG_Magic_LtoC_EndSpell							,
    EM_PG_Magic_LtoC_BeginSpellResult					,
    EM_PG_Magic_LtoC_MagicAttack						,
    EM_PG_Magic_LtoC_MagicAssistResult					,
    EM_PG_Magic_LtoC_FixAssistMagicTime					,
    EM_PG_Magic_LtoC_CancelSpellMagic					,
    EM_PG_Magic_CtoL_SetMagicPoint						,
    EM_PG_Magic_LtoC_SetMagicPointResult				,
	EM_PG_Magic_CtoL_BeginSpell_Pos						,
	EM_PG_Magic_LtoC_AddBuffInfo						,
	EM_PG_Magic_LtoC_DelBuffInfo						,
	EM_PG_Magic_LtoC_ResetColdown						,
	EM_PG_Magic_LtoC_CancelAssistMagic					,
	EM_PG_Magic_LtoC_BeginShoot							,
	EM_PG_Magic_LtoC_SysBeginShoot						,
	EM_PG_Magic_LtoC_SysEndShoot						,
	EM_PG_Magic_LtoC_MagicShieldInfo					,
	EM_PG_Magic_LtoC_MagicEnd							,
	EM_PG_Magic_LtoC_CreatePet							,
	EM_PG_Magic_CtoL_PetCommand							,
	EM_PG_Magic_CtoL_PetMagicSwitchRequest				,
	EM_PG_Magic_LtoC_PetMagicSwitchResult				,
	EM_PG_Magic_LtoC_DeletePet							,

	EM_PG_Magic_LtoC_RaiseMagic							,
	EM_PG_Magic_CtoL_CancelMagicRequest					,
	EM_PG_Magic_LtoC_EndShoot							,
	EM_PG_Magic_CtoL_SetMagicPointEx					,

	EM_PG_Magic_CtoL_BuffSkillRequest					,
	EM_PG_Magic_LtoC_BuffSkillResult					,
	EM_PG_Magic_LtoC_ModifyBuffInfo						,
	EM_PG_Magic_LtoC_PartyMemberBuff					,
	EM_PG_Magic_LtoC_CreatePet_Range					,

    //GMTools
    EM_PG_GM_LToGM_ZoneInfo								= 650,
    EM_PG_GM_LToGM_PlayerInfo							,
    EM_PG_GM_LToGM_NPCInfo								,
    EM_PG_GM_LToGM_DelPlayer							,
    EM_PG_GM_LToGM_DelNPC								,
    EM_PG_GM_GMtoL_ObjectDataRequest					,
	EM_PG_GM_LtoGM_ObjectData							,
    EM_PG_GM_CtoL_SendMsg								,
    EM_PG_GM_LtoGM_SendMsg								,
    EM_PG_GM_GMtoL_SendMsg								,
    EM_PG_GM_LtoC_SendMsg								,
    EM_PG_GM_LtoGM_SrvSysMsg							,
	EM_PG_GM_LtoC_SendGameMessageEvent					,
	EM_PG_GM_GMtoL_SendGMCommand						,
	EM_PG_GM_LtoC_GameMessageDataEvent					,
	EM_PG_Talk_LtoC_RunningMsg							,
	EM_PG_GM_GMtoX_ServerStateRequest					,
	EM_PG_GM_XtoGM_ServerStateResult					,

	EM_PG_Talk_LtoD_OfflineMessageRequest				,
	EM_PG_Talk_DtoC_OfflineMessage						,
	EM_PG_Talk_DtoL_OfflineGMCommand					,

	EM_PG_Talk_CtoChat_RegisterBillBoardInfo			,
	EM_PG_Talk_ChattoC_BillBoardInfo					,	
	EM_PG_Talk_CtoChat_BillBoardLiveTime				,
	EM_PG_Talk_ChattoC_BillBoardDelete					,
	EM_PG_Talk_LtoC_RunningMsgEx						,

    //商店
    EM_PG_Shop_LtoC_ShopOpen							= 700, 
    EM_PG_Shop_LtoC_ShopClose							,
    EM_PG_Shop_CtoL_ShopCloseNotify						,
    EM_PG_Shop_CtoL_BuyItemRequest						,
    EM_PG_Shop_LtoC_BuyItemResult						,
    EM_PG_Shop_CtoL_SelectSellItemRequest				,
    EM_PG_Shop_LtoC_SelectSellItemResult				,
    EM_PG_Shop_CtoL_RepairWeaponRequest					,
    EM_PG_Shop_LtoC_RepairWeaponResult					,

	EM_PG_Shop_LtoC_AddSellItemRecord					,
	EM_PG_Shop_LtoC_DelSellItemRecord					,
	EM_PG_Shop_CtoL_BuySellItemRecord					,
	EM_PG_Shop_LtoC_FixShopItemCount					,

	EM_PG_Shop_CtoL_AccountMoneyShop					,	
	EM_PG_Shop_LtoC_AccountMoneyShopResult				,
	EM_PG_Shop_CtoL_AccountMoneyShopBuyList				,
	EM_PG_Shop_LtoC_AccountMoneyShopBuyListResult		,
	EM_PG_Shop_LtoC_FixSellItemBackup					,

	EM_PG_Item_CtoL_PutCoolClothRequest					= 750,
	EM_PG_Item_LtoC_PutCoolClothResult					,
	EM_PG_Item_CtoL_DelCoolClothRequest					,
	EM_PG_Item_LtoC_DelCoolClothResult					,
	EM_PG_Item_CtoL_SetCoolSuitRequest					,
	EM_PG_Item_LtoC_SetCoolSuitResult					,
	EM_PG_Item_CtoL_ShowCoolSuitIndexRequest			,
	EM_PG_Item_LtoC_ShowCoolSuitIndexResult				,

	EM_PG_Item_CtoL_PutInExBagRequest					,
	EM_PG_Item_LtoC_PutInExBagResult					,
	EM_PG_Item_CtoL_UseExBagRequest						,
	EM_PG_Item_LtoC_UseExBagResult						,

	EM_PG_Item_CtoL_SwapCoolClothRequest				,
	EM_PG_Item_LtoC_SwapCoolClothResult					,

	EM_PG_Item_LtoC_OpenRare3EqExchangeItem				,
	EM_PG_Item_CtoL_Rare3EqExchangeItemRequest			,
	EM_PG_Item_LtoC_Rare3EqExchangeItemResult			,
	EM_PG_Item_CtoL_CloseRare3EqExchangeItem			,

	EM_PG_Item_CtoL_PopExBagRequest						,
	EM_PG_Item_LtoC_PopExBagResult						,

	EM_PG_Item_CtoL_ClsAttribute						,
	EM_PG_Item_LtoC_ClsAttributeResult					,
	EM_PG_Item_CtoL_MagicBoxAttrDrawOff					,
	EM_PG_Item_LtoC_MagicBoxAttrDrawOffResult			,

	//Party 
	EM_PG_Party_CtoChat_Invite							= 800,
	EM_PG_Party_ChattoC_Invite							,
	EM_PG_Party_CtoChat_Join							,
	EM_PG_Party_ChattoL_PartyBaseInfo					,
	EM_PG_Party_ChattoL_AddMember						,
	EM_PG_Party_ChattoL_DelMember						,
	EM_PG_Party_ChattoC_PartyBaseInfo					,
	EM_PG_Party_ChattoC_AddMember						,
	EM_PG_Party_ChattoC_DelMember						,
	EM_PG_Party_ChattoC_FixMember						,
	EM_PG_Party_CtoChat_KickMember						,
	EM_PG_Party_CtoChat_PartyRule						,
	EM_PG_Party_LtoC_PartyMemberInfo					,
	EM_PG_Party_ChattoL_FixMember						,
	//Party 物品分配
	EM_PG_Party_LtoChat_PartyTreasure					,
	EM_PG_Party_ChattoC_PartyTreasure					,
	EM_PG_Party_ChattoL_GiveItem						,
	EM_PG_Party_ChattoC_PartyTreasureLoot				,
	EM_PG_Party_CtoChat_PartyTreasureLoot				,
	EM_PG_Party_ChattoC_GiveItemInfo					,

	EM_PG_Party_LtoC_PartyMemberHPMPSP					,
	EM_PG_Party_LtoC_PartyMemberMaxHPMPSP				,
	EM_PG_Party_LtoChat_PartyRelogin					,
	EM_PG_Party_ChattoL_PartyReloginResult				,

	EM_PG_Party_LtoC_MemberLogout						,

	EM_PG_Party_CtoChat_SetMemberPosRequest				,
	EM_PG_Party_ChattoC_SetMemberPosResult				,
	EM_PG_Party_CtoChat_SetMemberMode					,

	EM_PG_ZoneParty_CtoL_Invite							,
	EM_PG_ZoneParty_LtoC_Invite							,
	EM_PG_ZoneParty_CtoL_Join							,
	EM_PG_ZoneParty_LtoC_AddMember						,
	EM_PG_ZoneParty_LtoC_DelMember						,
	EM_PG_ZoneParty_LtoC_FixMember						,
	EM_PG_ZoneParty_CtoL_KickMember						,
	EM_PG_ZoneParty_CtoL_SetMemberPosRequest			,
	EM_PG_ZoneParty_LtoC_SetMemberPosResult				,
	EM_PG_ZoneParty_LtoC_PartyBase						,

	EM_PG_ZoneParty_CtoL_ChangeLeader					,
	EM_PG_ZoneParty_LtoC_ChangeLeaderResult				,

	EM_PG_ZoneParty_CtoL_Talk							,
	EM_PG_ZoneParty_LtoC_Talk							,


	EM_PG_Party_CtoL_GetMemberInfo						,
	EM_PG_Party_LtoL_GetMemberInfo						,
	EM_PG_Party_LtoC_GetMemberInfoResult				,

	//其它
	EM_PG_Other_LtoL_RequestPlayerPos					= 850,
	EM_PG_Other_LtoL_PlayerPos							,
	EM_PG_Other_LtoL_CallPlayer							,

	EM_PG_Other_LtoC_OpenDialog							,
	EM_PG_Other_LtoC_DialogSelectStr					,
	EM_PG_Other_LtoC_CloseDialog						,
	EM_PG_Other_LtoC_DialogSelectStrEnd					,
	EM_PG_Other_CtoL_DialogSelectID						,
	EM_PG_Other_LtoC_DialogSetContent					,

	EM_PG_Other_CtoL_BeginCastingRequest				,
	EM_PG_Other_LtoC_BeginCastingResult					,
	EM_PG_Other_CtoL_InterruptCastingRequest			,
	EM_PG_Other_LtoC_InterruptCasting					,
	EM_PG_Other_LtoC_CastingOK							,
	EM_PG_Other_CtoL_RangeDataTransferRequest			,
	EM_PG_Other_LtoC_RangeDataTransfer					,
	EM_PG_Other_CtoL_ColoringShopRequest				,
	EM_PG_Other_LtoC_ColoringShopResult					,
	EM_PG_Other_CtoL_PlayerInfoRequest					,
	EM_PG_Other_LtoC_PlayerInfoResult					,
	EM_PG_Other_CtoL_SysKeyFunctionRequest				,
	EM_PG_Other_LtoC_SysKeyFunctionResult				,
	EM_PG_Other_CtoL_AllObjectPosRequest				,
	EM_PG_Other_LtoC_ObjectPosInfo						,
	EM_PG_Other_LtoC_OpenClientMenu						,

	EM_PG_Other_LtoC_DialogSetTitle						,
	EM_PG_Other_CtoL_RunGlobalPlotRequest				,
	EM_PG_Other_LtoC_RunGlobalPlotResult				,

	EM_PG_Other_CtoL_HateListRequest					,
	EM_PG_Other_LtoC_HateListResult						,

	EM_PG_Other_CtoL_ManagePasswordKey					,
	EM_PG_Other_CtoL_ManagePassword						,

	EM_PG_Other_LtoC_OpenGiveItemPlot					,
	EM_PG_Other_CtoL_GiveItemPlot						,

	EM_PG_Other_CtoL_CheckPassword						,
	EM_PG_Other_LtoC_CheckPasswordResult				,

	EM_PG_Other_CtoL_Find_DBID_RoleName					,
	EM_PG_Other_LtoD_Find_DBID_RoleName					,
	EM_PG_Other_DtoC_Find_DBID_RoleName					,
	EM_PG_Other_LtoC_GoodEvilValue						,
	EM_PG_Other_LtoC_CampID								,
	EM_PG_Other_CtoL_DialogInput						,
	EM_PG_Other_DtoL_ExchangeMoneyTable					,
	EM_PG_Other_LtoC_ExchangeMoneyTable					,
	EM_PG_Other_CtoL_GmNotification						,

	EM_PG_Other_CtoL_ColoringHorse						,
	EM_PG_Other_LtoC_ColoringHorseResult				,

	EM_PG_Other_DtoL_DBStringTable						,
	EM_PG_Other_LtoC_DBStringTable						,
	EM_PG_Other_CtoL_RoleNameDuplicateErr				,
	EM_PG_Other_LtoC_PKDeadDropItem						,
	EM_PG_Other_LtoC_PlayerEvent						,
	EM_PG_Other_LtoC_ClientClock						,
	EM_PG_Other_LtoC_AccountMoneyLockInfo				,
	EM_PG_Other_LtoC_AccountMoneyTranError				,

	EM_PG_Other_CtoL_LookTarget							,
	EM_PG_Other_CtoL_SetCycleMagic						,
	EM_PG_Other_LtoC_SetCycleMagic						,

	EM_PG_Other_CtoL_CloseMasterKeyRequest				,
	EM_PG_Other_CtoL_CloseMaster						,
	EM_PG_Other_LtoD_SelectDB							,
	EM_PG_Other_LtoC_DelJob								,
	EM_PG_Other_LtoC_SetSmallGameMenuType				,
	EM_PG_Other_LtoC_SetSmallGameMenuValue				,
	EM_PG_Other_LtoC_SetSmallGameMenuStr				,
	EM_PG_Other_CtoL_SetSendPacketMode					,

	EM_PG_Other_CtoL_ResetInstanceRequest				,
	EM_PG_Other_LtoC_ResetInstanceResult				,
	EM_PG_Other_LtoL_ResetInstance						,
	EM_PG_Other_LtoC_ResetInstanceInfo					,
	EM_PG_Other_CtoL_GmResetRole						,

	EM_PG_Other_CtoL_PartyDice							,
	EM_PG_Other_LtoC_PartyDice							,

	EM_PG_Other_CtoL_BodyShop							,
	EM_PG_Other_LtoC_BodyShopResult						,

	EM_PG_Other_CtoL_MoneyVendorReport					,
	EM_PG_Other_LtoC_MoneyVendorReport					,
	EM_PG_Other_CtoL_BotReport							,
	EM_PG_Other_LtoC_BotReport							,

	EM_PG_Other_LtoC_UnlockBotReport					,
	EM_PG_Other_CtoL_UnlockBotReport					,

	EM_PG_Other_CtoL_SortBankRequest					,
	EM_PG_Other_LtoC_SortBankEnd						,

	EM_PG_Other_CtoL_LoadClientData						,
	EM_PG_Other_LtoD_LoadClientData						,
	EM_PG_Other_DtoC_LoadClientDataResult				,
	EM_PG_Other_CtoL_SaveClientData						,
	EM_PG_Other_LtoC_SaveClientDataResult				,

	EM_PG_Other_LtoC_SetMapMark							,
	EM_PG_Other_LtoC_ClsMapMark							,

	EM_PG_Other_CtoL_Suicide							,

	EM_PG_Other_LtoC_SetRoomValue						,
	EM_PG_Other_LtoC_ClsRoomValue						,

	EM_PG_Other_CtoM_LoadClientData_Account				,
	EM_PG_Other_MtoD_LoadClientData_Account				,
	EM_PG_Other_DtoC_LoadClientDataResult_Account		,
	EM_PG_Other_CtoM_SaveClientData_Account				,
	EM_PG_Other_MtoC_SaveClientDataResult_Account		,

	EM_PG_Other_DtoL_SysKeyValue						,
	EM_PG_Other_CtoL_Performance						,
	EM_PG_Other_LtoD_Performance						,
	EM_PG_Other_CtoL_TracertList						,

	//好友名單
	EM_PG_FriendList_CtoL_InsertFriend					= 950,
	EM_PG_FriendList_LtoC_InsertFriendResult			,
	EM_PG_FriendList_LtoChat_FriendListInfo				,
	EM_PG_FriendList_ChattoC_PlayerOnlineInfo			,
	EM_PG_FriendList_ChattoC_PlayerOfflineInfo			,
	EM_PG_FriendList_CtoL_EraseFriend					,
	EM_PG_FriendList_LtoC_EraseFriendResult				,
	EM_PG_FriendList_CtoL_ModifyFriend					,
	EM_PG_FriendList_LtoC_ModifyFriendResult			,
	EM_PG_FriendList_LtoC_FriendListInfo_Zip			,
	EM_PG_FriendList_CtoL_ModifyNote					,
	EM_PG_FriendList_CtoL_FixClientData					,
	EM_PG_FriendList_CtoL_FixGroupSortID				,
	EM_PG_FriendList_LtoC_FixFriendInfo					,

	EM_PG_Friend_CtoL_SearchFindPartyPlayerRequest		,
	EM_PG_Friend_LtoC_SearchFindPartyPlayerResult		,

	EM_PG_Other_LtoC_AddExSkill							= 980,
	EM_PG_Other_LtoC_DelExSkill							,
	EM_PG_Other_CtoL_SpellExSkill						,
	EM_PG_Other_LtoC_SpellExSkillResult					,
	EM_PG_Other_CtoL_ClientEventLog						,
	EM_PG_Other_LtoC_BaseObjValueChange					,
	EM_PG_Other_CtoL_TransferData_Range					,
	EM_PG_Other_LtoC_TransferData_Range					,
	EM_PG_Other_LtoC_LuaClientEvent						,
	EM_PG_Other_CtoL_SetRoleValue						,
	EM_PG_Other_LtoC_SetRoleValueResult					,
	EM_PG_Other_LtoC_ZoneOpen							,
	EM_PG_Other_CtoL_Client_Language					,
	EM_PG_Other_DtoL_RoleRunPlot						,
	EM_PG_Other_CtoL_SetNewTitleSys						,
	EM_PG_Other_LtoC_SetNewTitleSysResult				,
	EM_PG_Other_CtoL_NewTitleSys_UseItem				,
	EM_PG_Other_LtoC_NewTitleSys_UseItemResult			,
	EM_PG_Other_LtoL_PCall								,

	//郵件系統
	EM_PG_Mail_CtoL_SendMail							= 1000,
	EM_PG_Mail_CtoL_MailList							,
	EM_PG_Mail_CtoL_SendBack							,
	EM_PG_Mail_CtoL_DelMail								,
	EM_PG_Mail_CtoL_ReadMail							,
	EM_PG_Mail_CtoL_GetItem								,

	EM_PG_Mail_LtoC_SendMailResult						,
	EM_PG_Mail_LtoC_GetItemResult						,

	EM_PG_Mail_LtoD_SendMail							,
	EM_PG_Mail_LtoD_MailList							,
	EM_PG_Mail_LtoD_SendBack							,
	EM_PG_Mail_LtoD_DelMail								,
	EM_PG_Mail_LtoD_ReadMail							,
	EM_PG_Mail_LtoD_GetItem								,

	EM_PG_Mail_DtoL_SendMailResult						,
	EM_PG_Mail_DtoL_GetItemResult						,
	
	EM_PG_Mail_DtoC_MailListResult						,
	EM_PG_Mail_DtoC_MailListCountResult					,

	EM_PG_Mail_LtoC_OpenMail							,
	EM_PG_Mail_LtoC_CloseMail							,
	EM_PG_Mail_CtoL_CloseMail							,

	EM_PG_Mail_LtoD_UnReadMailCountRequest				,
	EM_PG_Mail_DtoC_UnReadMailCount						,
	EM_PG_Mail_DtoC_NewMail								,

	EM_PG_Mail_LtoD_CheckSendMail						,
	EM_PG_Mail_DtoL_CheckSendMailResult					,

	EM_PG_Mail_LtoD_CheckGetItemRequest					,
	EM_PG_Mail_DtoL_CheckGetItemResult					,
	EM_PG_Mail_CtoL_SendMailList						,

	//AC 交易
	EM_PG_AC_CtoL_SearchItem							= 1100,
	EM_PG_AC_CtoL_GetNextSearchItem						,
	EM_PG_AC_CtoL_GetMyACItemInfo						,
	EM_PG_AC_CtoL_BuyItem								,
	EM_PG_AC_CtoL_SellItem								,
	EM_PG_AC_CtoL_CancelSellItem						,
	EM_PG_AC_CtoL_CloseAC								,
	EM_PG_AC_CtoL_ItemHistoryRequest					,

	EM_PG_AC_LtoDC_SearchItem							,
	EM_PG_AC_LtoDC_GetNextSearchItem					,
	EM_PG_AC_LtoDC_GetMyACItemInfo						,
	EM_PG_AC_LtoDC_BuyItem								,
	EM_PG_AC_LtoDC_SellItem								,
	EM_PG_AC_LtoDC_CancelSellItem						,
	EM_PG_AC_LtoDC_CloseAC								,
	EM_PG_AC_LtoDC_OpenAC								,
	EM_PG_AC_LtoDC_ItemHistoryRequest					,

	EM_PG_AC_DCtoC_SearchItemCountResult				,
	EM_PG_AC_DCtoC_SearchItemResult						,
	EM_PG_AC_DCtoC_FixSearchItemResult					,
	EM_PG_AC_DCtoC_SelfCompetitive_BuyItem				,
	EM_PG_AC_DCtoC_SelfCompetitive_SellItem				,
	EM_PG_AC_DCtoC_CancelSellItemResult					,
	EM_PG_AC_DCtoC_CloseAC								,
	EM_PG_AC_DCtoC_OpenAC								,	
	EM_PG_AC_DCtoC_LostCompetitiveItem					,
	EM_PG_AC_DCtoC_FixBuyOrderItem						,
	EM_PG_AC_DCtoC_ItemHistoryListResult				,

	EM_PG_AC_DCtoL_BuyItemResult						,
	EM_PG_AC_DCtoL_SellItemResult						,

	EM_PG_AC_LtoC_BuyItemResult							,
	EM_PG_AC_LtoC_SellItemResult						,

	EM_PG_AC_DCtoC_SelfCompetitive_ItemDataEnd			,
	EM_PG_AC_DCtoC_FixSellOrderItem						,

	EM_PG_AC_LtoC_AcAlreadyOpen							,
	EM_PG_AC_CtoL_SellMoney								,
	EM_PG_AC_LtoC_SellMoneyResult						,

	EM_PG_AC_DtoC_AcTradeOk								,
	EM_PG_AC_D_C_AcBuyFailed							,

	//全Server擁有的腳色資料
	EM_PG_PlayerInfo_LtoAllSrv_SetPlayerData			= 1200,
	EM_PG_PlayerInfo_LtoAllSrv_PlayerLogout				,

	//NPC路徑處理
	EM_PG_CliConnect_CtoL_GetNPCMoveInfoRequest			= 1250,
	EM_PG_CliConnect_LtoC_GetNPCMoveInfoResult			,
	EM_PG_CliConnect_CtoL_SaveNPCMoveInfo				,

	EM_PG_CliConnect_CtoL_GetFlagList					,
	EM_PG_CliConnect_LtoC_GetFlagListResult				,
	EM_PG_CliConnect_CtoL_EditFlag						,
	EM_PG_CliConnect_LtoC_EditFlagResult				,
	EM_PG_CliConnect_CtoL_DelFlag						,
	EM_PG_CliConnect_LtoC_DelFlagResult					,
	EM_PG_CliConnect_CtoL_DelFlagGroup					,
	EM_PG_CliConnect_LtoC_DelFlagGroupResult			,

	EM_PG_CliConnect_LtoC_ParalledID					,

	EM_PG_CliConnect_LtoC_PlayerData_Zip				,

	EM_PG_CliConnect_LtoBL_ChangeZoneToOtherWorld		,
	EM_PG_CliConnect_BLtoL_ChangeZoneToOtherWorldResult	,
	EM_PG_CliConnect_LtoC_ConnectToBattleWorld			,
	EM_PG_CliConnect_CtoL_ConnectToBattleWorldOK		,
	EM_PG_CliConnect_LtoBL_ConnectCliAndGSrv			,
	EM_PG_CliConnect_BLtoL_RetNomorlWorld				,

	EM_PG_CliConnect_LtoL_DelTomb						,
	EM_PG_CliConnect_LtoC_SkyProcType					,
	EM_PG_CliConnect_LtoC_TouchTomb						,
	EM_PG_CliConnect_LtoC_TombInfo						,
	EM_PG_CliConnect_LtoC_WorldReturnNotify				,
	EM_PG_CliConnect_CtoL_WorldReturnOK					,
	EM_PG_CliConnect_BLtoBL_SendToPlayerByDBID			,

	EM_PG_CliConnect_BLtoL_BattleSavePlayer				,
	EM_PG_CliConnect_LtoC_ZoneOpenState					,

	EM_PG_CliConnect_LtoBL_ChangeZoneToOtherWorld_Zip	,
	EM_PG_CliConnect_BLtoL_BattleSavePlayer_Zip			,
	EM_PG_CliConnect_BLtoL_RetNomorlWorld_Zip			,

	EM_PG_CliConnect_BLtoL_BattleZone					,
	EM_PG_CliConnect_LtoBL_ReturnWorldRequest			,
	EM_PG_CliConnect_BLtoL_ReturnWorldResult			,


	//合成製造
	EM_PG_Recipe_CtoL_LockResourceAndRefineRequest		= 1300,
	EM_PG_Recipe_LtoC_LockResourceAndRefineResult		,
	EM_PG_Recipe_LtoC_ProduceItemResult					,
	EM_PG_Recipe_LtoC_RecipeList						,
	EM_PG_Recipe_CtoL_LearnRecipe						,
	EM_PG_Recipe_LtoC_LearnRecipeResult					,
	EM_PG_Recipe_LtoC_RefineCooldown					,

	//公會
	EM_PG_Guild_CtoL_CreateGuild						= 1400,
	EM_PG_Guild_LtoD_CreateGuild						,
	EM_PG_Guild_DtoL_CreateGuildResult					,
	EM_PG_Guild_LtoC_CreateGuildResult					,
	EM_PG_Guild_CtoL_NewGuildInvite						,
	EM_PG_Guild_LtoC_NewGuildInvite						,
	EM_PG_Guild_CtoL_NewGuildInviteResult				,
	EM_PG_Guild_LtoC_NewGuildInviteResult				,
	EM_PG_Guild_LtoD_AddNewGuildMember					,
	EM_PG_Guild_DtoL_AddNewGuildMemberResult			,
	EM_PG_Guild_LtoC_JoindNewGuild						,

	EM_PG_Guild_DtoL_AddGuild							,
	EM_PG_Guild_DtoL_DelGuild							,
	EM_PG_Guild_DtoL_AddGuildMember						,
	EM_PG_Guild_DtoL_DelGuildMember						,

	EM_PG_Guild_CtoL_LeaveNewGuild						,
	EM_PG_Guild_LtoD_LeaveNewGuild						,
	EM_PG_Guild_DtoL_LeaveNewGuildResult				,
	EM_PG_Guild_LtoC_LeaveNewGuildResult				,
	EM_PG_Guild_CtoL_DelNewGuild						,
	EM_PG_Guild_LtoD_DelNewGuild						,
	EM_PG_Guild_DtoL_DelNewGuildResult					,
	EM_PG_Guild_LtoC_DelNewGuildResult					,

	EM_PG_Guild_DtoL_ModifyGuild						,
	EM_PG_Guild_DtoL_ModifyGuildMember					,
	EM_PG_Guild_DtoC_GuildCreate						,
	EM_PG_Guild_CtoL_GuildInfoRequet					,
	EM_PG_Guild_LtoC_GuildInfo							,
	EM_PG_Guild_CtoL_GuildMemberListRequet				,
	EM_PG_Guild_LtoC_GuildMemberListCount				,
	EM_PG_Guild_LtoC_GuildMemberBaseInfo				,
	EM_PG_Guild_CtoL_GuildMemberInfoRequet				,
	EM_PG_Guild_LtoC_GuildMemberInfo					,
	EM_PG_Guild_LtoD_ModifyGuildInfoRequest				,
	EM_PG_Guild_CtoL_ModifyGuildInfoRequest				,
	EM_PG_Guild_CtoL_ModifyGuildMemberInfoRequest		,
	EM_PG_Guild_LtoD_ModifyGuildMemberInfoRequest		,

	EM_PG_Guild_CtoL_GuildInvite						,
	EM_PG_Guild_LtoC_GuildInvite						,
	EM_PG_Guild_CtoL_GuildInviteResult					,
	EM_PG_Guild_LtoC_GuildInviteResult					,
	EM_PG_Guild_LtoD_AddGuildMember						,
	EM_PG_Guild_DtoL_AddGuildMemberResult				,
	EM_PG_Guild_LtoC_JoindGuild							,
	EM_PG_Guild_CtoL_LeaveGuild							,
	EM_PG_Guild_LtoD_LeaveGuild							,
	EM_PG_Guild_DtoL_LeaveGuildResult					,
	EM_PG_Guild_LtoC_LeaveGuildResult					,
	EM_PG_Guild_CtoL_DelGuild							,
	EM_PG_Guild_LtoD_DelGuild							,
	EM_PG_Guild_DtoL_DelGuildResult						,
	EM_PG_Guild_LtoC_DelGuildResult						,

	EM_PG_Guild_LtoD_SetGuildReady						,

	EM_PG_Guild_CtoL_BoardPostRequest					,
	EM_PG_Guild_LtoD_BoardPostRequest					,
	EM_PG_Guild_DtoC_BoardPostResult					,
	EM_PG_Guild_CtoL_BoardListRequest					,
	EM_PG_Guild_LtoD_BoardListRequest					,
	EM_PG_Guild_DtoC_BoardListCount						,
	EM_PG_Guild_DtoC_BoardListData						,
	EM_PG_Guild_CtoL_BoardMessageRequest				,
	EM_PG_Guild_LtoD_BoardMessageRequest				,
	EM_PG_Guild_DtoC_BoardMessageResult					,
	EM_PG_Guild_CtoL_BoardModifyMessageRequest			,
	EM_PG_Guild_LtoD_BoardModifyMessageRequest			,
	EM_PG_Guild_DtoC_BoardModifyMessageResult			,
	EM_PG_Guild_CtoL_BoardModifyModeRequest				,
	EM_PG_Guild_LtoD_BoardModifyModeRequest				,
	EM_PG_Guild_DtoC_BoardModifyModeResult				,

	EM_PG_Guild_LtoC_GuildBaseInfo						,
	EM_PG_Guild_LtoC_GuildDestroy						,
	EM_PG_Guild_DtoC_JoinGuildMember					,
	EM_PG_Guild_DtoC_LeaveGuildMember					,

	EM_PG_Guild_CtoL_CloseCreateGuild					,
	EM_PG_Guild_CtoL_GuildContributionItem				,
	EM_PG_Guild_CtoL_GuildContributionOK				,
	EM_PG_Guild_CtoL_GuildContributionClose				,

	EM_PG_Guild_LtoC_OpenCreateGuild					,
	EM_PG_Guild_LtoC_CloseCreateGuild					,
	EM_PG_Guild_LtoC_OpenGuildContribution				,
	EM_PG_Guild_LtoC_GuildContributionItemResult		,
	EM_PG_Guild_LtoC_GuildContributionClose				,

	EM_PG_Guild_LtoD_AddGuildResource					,

	EM_PG_Guild_CtoL_AllGuildMemberCountRequest			,
	EM_PG_Guild_LtoC_AllGuildMemberCountResult			,

	//公會戰競標
	EM_PG_Guild_LtoD_GuildFightInfoRequest				,
	EM_PG_Guild_DtoC_GuildFightInfo						,

	EM_PG_Guild_LtoC_GuildFightAuctionOpen				,
	EM_PG_Guild_CtoL_GuildFightAuctionClose				,
	EM_PG_Guild_LtoC_GuildFightAuctionClose				,
	EM_PG_Guild_CtoL_GuildFightAuctionBid				,
	EM_PG_Guild_LtoD_GuildFightAuctionBid				,
	EM_PG_Guild_DtoC_GuildFightAuctionBidResult			,
	EM_PG_Guild_DtoC_GuildFightAuctionBidNewOne			,
	EM_PG_Guild_DtoL_GuildFightAuctionInfo				,
	EM_PG_Guild_DtoL_GuildPlayerOnline					,
	EM_PG_Guild_DtoC_GuildLeaderChange					,
	EM_PG_Guild_DtoC_GuildMemberRankChange				,

	EM_PG_Guild_LtoC_SetGuildReady						,
	EM_PG_Guild_DtoC_GuildFightAuctionEnd				,

	EM_PG_Guild_DtoC_GuildFightBeginNotify				,
	EM_PG_Guild_DtoL_CreateGuildFightRoom				,
	EM_PG_Guild_LtoC_GuildFightLeaderInvite				,
	EM_PG_Guild_CtoL_GuildFightRequestInvite			,
	EM_PG_Guild_LtoC_GuildFightInviteNotify				,
	EM_PG_Guild_CtoL_GuildFightInviteRespond			,
	EM_PG_Guild_LtoC_GuildIDChange						,

	//公會自由宣戰
	EM_PG_Guild_CtoL_DeclareWar							,
	EM_PG_Guild_LtoD_DeclareWar							,
	EM_PG_Guild_DtoL_DeclareWarResult					,
	EM_PG_Guild_LtoC_DeclareWarResult					,
	EM_PG_Guild_DtoL_WarInfo							,
	EM_PG_Guild_LtoC_WarInfo							,
	EM_PG_Guild_LtoC_LoginWarInfo						,

	EM_PG_Guild_LtoD_DeclareWarScore					,
	EM_PG_Guild_DtoC_DeclareWarFinalResult				,	

	EM_PG_Guild_CtoL_SetGuildFlagInfoRequest			,
	EM_PG_Guild_LtoD_SetGuildFlagInfoRequest			,
	EM_PG_Guild_XtoC_SetGuildFlagInfoResult				,

	//公會改名
	EM_PG_Guild_CtoL_ChangeGuildNameRequest				,
	EM_PG_Guild_LtoD_ChangeGuildNameRequest				,
	EM_PG_Guild_DtoC_ChangeGuildNameResult				,
	EM_PG_Guild_DtoC_ChangeGuildName					,

	EM_PG_Guild_DtoC_PrepareWar							,

	//公會升級
	EM_PG_Guild_CtoL_GuildUpgradeRequest				,
	EM_PG_Guild_LtoD_GuildUpgradeRequest				,
	EM_PG_Guild_DtoC_GuildUpgradeResult					,

	EM_PG_Guild_DtoL_AddGuildResourceResult				,

	EM_PG_Guild_CtoL_SelectContributionLog				,
	EM_PG_Guild_LtoD_SelectContributionLog				,
	EM_PG_Guild_DtoC_SelectContributionLogResult		,

	EM_PG_Guild_DtoX_ModifySimpleGuild					,
	EM_PG_Guild_DtoC_GuildBaseInfoUpdate				,
	EM_PG_Guild_DtoC_Guild_LevelUp						,
	EM_PG_Guild_LtoD_SetNeedChangeName					,
	EM_PG_Guild_DtoC_GuildInfoChange					,

	EM_PG_Guild_LtoD_GuildWarEndResult					,

	EM_PG_Guild_CtoL_GuildWarHistroy					,
	EM_PG_Guild_LtoD_GuildWarHistroy					,
	EM_PG_Guild_DtoC_GuildWarHistroyResult				,

	// 採集
	EM_PG_Gather_CtoL_ClickGatherObj					= 1600,
	EM_PG_Gather_LtoC_ClickGatherObj_Result				,
	EM_PG_Gather_LtoC_GatherMotion						,
	EM_PG_Gather_CtoL_CancelGather						,

	//
	EM_PG_Bot_CtoL_RequestMovePath						= 1700,
	EM_PG_Bot_CtoL_MovePoint							,

	EM_PG_Billboard_LtoC_OpenBillboard					= 1800,
	EM_PG_Billboard_LtoC_CloseBillboard					,
	EM_PG_Billboard_CtoL_CloseBillboardRequest			,
	EM_PG_Billboard_CtoL_RankInfoRequest				,
	EM_PG_Billboard_LtoD_RankInfoRequest				,
	EM_PG_Billboard_DtoC_RankInfoResult					,
	EM_PG_Billboard_DtoC_RankInfoList					,
	EM_PG_Billboard_CtoL_SetAnonymous					,
	EM_PG_Billboard_LtoD_SetAnonymous					,

	//新版的排行榜
	EM_PG_NewBillboard_CtoL_SearchRank					,
	EM_PG_NewBillboard_LtoD_SearchRank					,
	EM_PG_NewBillboard_DtoC_SearchRankResult			,

	EM_PG_NewBillboard_CtoL_SearchMyRank				,
	EM_PG_NewBillboard_LtoD_SearchMyRank				,
	EM_PG_NewBillboard_DtoC_SearchMyRankResult			,

	EM_PG_NewBillboard_LtoD_Register					,

	EM_PG_NewBillboard_CtoL_RegisterNote				,
	EM_PG_NewBillboard_LtoD_RegisterNote				,
	
	EM_PG_PK_CtoL_PKInviteRequest						= 1900,
	EM_PG_PK_LtoC_PKInviteNotify						,
	EM_PG_PK_CtoL_PKInviteResult						,
	EM_PG_PK_LtoC_PKInviteResult						,
	EM_PG_PK_LtoC_PKPrepare								,
	EM_PG_PK_LtoC_PKBegin								,
	EM_PG_PK_LtoC_PKEnd									,
	EM_PG_PK_CtoL_PKCancel								,
	EM_PG_PK_LtoC_PKCancel								,

	EM_PG_Lottery_LtoC_OpenExchangePrize				= 2000,
	EM_PG_Lottery_CtoL_ExchangePrize					,
	EM_PG_Lottery_LtoD_ExchangePrize					,
	EM_PG_Lottery_DtoL_ExchangePrizeResult				,
	EM_PG_Lottery_LtoC_ExchangePrizeResult				,
	EM_PG_Lottery_CtoL_CloseExchangePrize				,
	EM_PG_Lottery_LtoC_CloseExchangePrize				,
	EM_PG_Lottery_LtoC_OpenBuyLottery					,
	EM_PG_Lottery_CtoL_BuyLottery						,
	EM_PG_Lottery_LtoD_BuyLottery						,
	EM_PG_Lottery_DtoL_BuyLotteryResult					,
	EM_PG_Lottery_LtoC_BuyLotteryResult					,
	EM_PG_Lottery_CtoL_CloseBuyLottery					,
	EM_PG_Lottery_LtoC_CloseBuyLottery					,
	EM_PG_Lottery_CtoL_LotteryInfoRequest				,
	EM_PG_Lottery_DtoC_LotteryInfo						,
	EM_PG_Lottery_LtoD_LotteryInfoRequest				,
	EM_PG_Lottery_DtoC_RunLottery						,
	EM_PG_Lottery_CtoL_LotteryHistoryInfoRequest		,
	EM_PG_Lottery_LtoD_LotteryHistoryInfoRequest		,
	EM_PG_Lottery_DtoC_LotteryHistoryInfo				,

	//房屋系統
	EM_PG_Houses_LtoD_BuildHouseRequest					= 2100,
	EM_PG_Houses_DtoL_BuildHouseResult					,
	EM_PG_Houses_LtoC_BuildHouseResult					,
	EM_PG_Houses_LtoC_OpenVisitHouse					,
	EM_PG_Houses_CtoL_VisitHouseRequest					,
	EM_PG_Houses_LtoD_FindHouseRequest					,
	EM_PG_Houses_DtoL_FindHouseResult					,
	EM_PG_Houses_LtoC_FindHouseResult					,
	EM_PG_Houses_LtoC_CloseVisitHouse					,
	EM_PG_Houses_CtoL_CloseVisitHouse					,
	EM_PG_Houses_LtoC_HouseInfoLoading					,
	EM_PG_Houses_LtoD_HouseInfoLoading					,
	EM_PG_Houses_DtoL_HouseBaseInfo						,
	EM_PG_Houses_DtoL_ItemInfo							,
	EM_PG_Houses_LtoC_HouseBaseInfo						,
	EM_PG_Houses_LtoC_ItemInfo							,
	EM_PG_Houses_DtoL_HouseInfoLoadOK					,
	EM_PG_Houses_LtoC_HouseInfoLoadOK					,
	EM_PG_Houses_CtoL_SwapBodyHouseItem					,
	EM_PG_Houses_LtoC_SwapBodyHouseItemResult			,
	EM_PG_Houses_LtoC_FixHouseItem						,
	EM_PG_Houses_CtoL_SwapHouseItemRequest				,
	EM_PG_Houses_LtoC_SwapHouseItemResult				,
	EM_PG_Houses_CtoL_HouseItemLayoutRequest			,
	EM_PG_Houses_LtoC_HouseItemLayoutResult				,

	EM_PG_Houses_CtoL_ChangeHouseRequest				,
	EM_PG_Houses_LtoC_ChangeHouseResult					,
	EM_PG_Houses_CtoL_BuyFunctionRequest				,
	EM_PG_Houses_LtoC_BuyFunctionResult					,
	EM_PG_Houses_CtoL_FunctionSettingRequest			,
	EM_PG_Houses_LtoC_FunctionSettingResult				,
	EM_PG_Houses_CtoL_BuyEnergyRequest					,
	EM_PG_Houses_LtoC_BuyEnergyResult					,
	EM_PG_Houses_LtoD_SaveHouseBase						,
	EM_PG_Houses_LtoD_SaveItem							,
	EM_PG_Houses_CtoL_LeaveHouse						,
	EM_PG_Houses_CtoL_BoxInfoRequset					,
	EM_PG_Houses_LtoC_BoxInfoResult						,
	EM_PG_Houses_CtoL_ExchangeEq						,
	EM_PG_Houses_LtoC_ExchangeEqResult					,
	EM_PG_Houses_CtoL_SetPassword						,
	EM_PG_Houses_LtoC_SetPasswordResult					,

	EM_PG_Houses_CtoL_DestroyItemRequest				,
	EM_PG_Houses_LtoC_DestroyItemResult					,

	EM_PG_Houses_CtoL_SetHouseName						,
	EM_PG_Houses_LtoC_SetHouseNameResult				,

	EM_PG_Houses_CtoL_PlantClearRequest					,
	EM_PG_Houses_LtoC_PlantClearResult					,
	EM_PG_Houses_CtoL_PlantItemRequest					,
	EM_PG_Houses_LtoC_PlantItemResult					,
	EM_PG_Houses_CtoL_PlantLockRequest					,
	EM_PG_Houses_LtoC_PlantLockResult					,
	EM_PG_Houses_CtoL_PlantGetProductRequest			,
	EM_PG_Houses_LtoC_PlantGetProductResult				,

	EM_PG_Houses_DtoL_FriendDBID						,
	EM_PG_Houses_LtoC_FriendDBID						,
	EM_PG_Houses_CtoL_ModifyFriendDBID					,
	EM_PG_Houses_LtoC_ModifyFriendDBID					,

	EM_PG_Houses_CtoL_SetClientData						,
	EM_PG_Houses_LtoC_FriendItemLog						,

	EM_PG_Houses_CtoL_VisitHouseRequest_ByName			,
	EM_PG_Houses_LtoD_FindHouseRequest_ByName			,

	EM_PG_Houses_CtoL_ClearItemLogRequest				,
	EM_PG_Houses_LtoC_ClearItemLogResult				,

	EM_PG_Houses_CtoL_ServantHireListRequest			,
	EM_PG_Houses_LtoC_ServantHireList					,
	EM_PG_Houses_CtoL_ServantHireRequest				,
	EM_PG_Houses_LtoC_ServantHireResult					,
	EM_PG_Houses_CtoL_ServantDelRequest					,
	EM_PG_Houses_LtoC_ServantDelResult					,
	EM_PG_Houses_CtoL_ServantEventRequest				,
	EM_PG_Houses_LtoC_ServantEvent						,
	EM_PG_Houses_CtoL_ServantLayout						,
	EM_PG_Houses_CtoL_ServantRenameRequest				,
	EM_PG_Houses_LtoC_ServantRenameResult				,
	EM_PG_Houses_LtoC_FixServantValue					,

	EM_PG_DepartmentStore_CtoL_ShopFunctionRequest		= 2200,
	EM_PG_DepartmentStore_LtoC_ShopFunctionResult		,
	EM_PG_DepartmentStore_CtoL_SellTypeRequest			,
	EM_PG_DepartmentStore_LtoD_SellTypeRequest			,
	EM_PG_DepartmentStore_DtoC_SellType					,


	EM_PG_DepartmentStore_CtoL_ShopInfoRequest			,
	EM_PG_DepartmentStore_LtoD_ShopInfoRequest			,
	EM_PG_DepartmentStore_DtoC_ShopInfo					,
	EM_PG_DepartmentStore_CtoL_BuyItem					,
	EM_PG_DepartmentStore_LtoD_BuyItem					,
	EM_PG_DepartmentStore_DtoL_BuyItemResult			,
	EM_PG_DepartmentStore_LtoD_BuyItemResult			,
	EM_PG_DepartmentStore_LtoC_BuyItemResult			,

	//帳號幣提領
	EM_PG_DepartmentStore_CtoL_ImportAccountMoney		,
	EM_PG_DepartmentStore_LtoD_ImportAccountMoney		,
	EM_PG_DepartmentStore_DtoL_ImportAccountMoneyResult , 
	EM_PG_DepartmentStore_LtoD_ImportAccountMoneyFailed	,
	EM_PG_DepartmentStore_LtoC_ImportAccountMoneyResult	,

	EM_PG_DepartmentStore_CtoL_ExchangeItemRequest		,
	EM_PG_DepartmentStore_LtoD_ExchangeItemRequest		,
	EM_PG_DepartmentStore_DtoL_ExchangeItemResult		,
	EM_PG_DepartmentStore_LtoD_ExchangeItemResultOK		,
	EM_PG_DepartmentStore_LtoC_ExchangeItemResult		,

	EM_PG_DepartmentStore_CtoL_RunningMessageRequest	,
	EM_PG_DepartmentStore_LtoD_RunningMessageRequest	,
	EM_PG_DepartmentStore_DtoC_RunningMessage_Zip		,

	EM_PG_DepartmentStore_CtoL_MailGift					,
	EM_PG_DepartmentStore_LtoD_CheckMailGift			,
	EM_PG_DepartmentStore_DtoL_CheckMailGiftResult		,
	EM_PG_DepartmentStore_LtoC_MailGiftResult			,

	EM_PG_DepartmentStore_DtoC_ShopInfo_Zip				,

	EM_PG_DepartmentStore_CtoL_WebBagRequest			,
	EM_PG_DepartmentStore_LtoD_WebBagRequest			,
	EM_PG_DepartmentStore_DtoL_WebBagResult				,

	EM_PG_DepartmentStore_DtoL_SlotMachineInfo			,
	EM_PG_DepartmentStore_LtoC_SlotMachineInfoChange	,
	EM_PG_DepartmentStore_CtoL_SlotMachineInfoRequest	,
	EM_PG_DepartmentStore_LtoC_SlotMachineInfo			,
	EM_PG_DepartmentStore_CtoL_SlotMachinePlay			,
	EM_PG_DepartmentStore_LtoC_SlotMachinePlayResult	,
	EM_PG_DepartmentStore_CtoL_GetSlotMachineItem		,

	EM_PG_DepartmentStore_LtoC_WebBagResult				,

	EM_PG_ImportBoard_CtoL_ImportBoardRequest			= 2250,
	EM_PG_ImportBoard_LtoD_ImportBoardRequest			,
	EM_PG_ImportBoard_DtoC_ImportBoardInfo				,
	EM_PG_ImportBoard_DtoC_ImportBoardNewVersion		,
	EM_PG_ImportBoard_LtoD_NewVersionRequest			,

	EM_PG_Teleport_CtoL_AddTeleportRequest				= 2300,
	EM_PG_Teleport_LtoC_AddTeleportResult				,
	EM_PG_Teleport_CtoL_DelTeleportRequest				,
	EM_PG_Teleport_LtoC_DelTeleportResult				,
	EM_PG_Teleport_CtoL_SwapTeleportRequest				,
	EM_PG_Teleport_LtoC_SwapTeleportResult				,
	EM_PG_Teleport_CtoL_ModifyTeleportRequest			,
	EM_PG_Teleport_LtoC_ModifyTeleportResult			,
	EM_PG_Teleport_CtoL_UseTeleportRequest				,
	EM_PG_Teleport_LtoC_UseTeleportResult				,
	EM_PG_Teleport_LtoL_CreateTeleport					,

	EM_PG_Treasure_LtoC_BaseInfo						= 2350,
	EM_PG_Treasure_CtoL_OpenRequest						,
	EM_PG_Treasure_LtoC_OpenFailed						,
	EM_PG_Treasure_LtoC_Open							,
	EM_PG_Treasure_CtoL_GetItem							,
	EM_PG_Treasure_LtoC_GetItemResult					,
	EM_PG_Treasure_CtoL_Close							,

	EM_PG_BattleGround_CtoL_JoinBattleGround			= 2400,
	EM_PG_BattleGround_CtoL_GetBattleGround				,
	EM_PG_BattleGround_LtoL_JoinBattleGround			,
	EM_PG_BattleGround_LtoL_GetBattleGround				,
	EM_PG_BattleGround_LtoC_GetBattleGroundResult		,
	EM_PG_BattleGround_LtoC_EnterBattleGroundRight		,
	EM_PG_BattleGround_LtoC_NotifyEnterBattleGroundRight,
	EM_PG_BattleGround_LtoC_LostEnterBattleGroundRight	,
	EM_PG_BattleGround_LtoC_EmptyBattleGround			,
	EM_PG_BattleGround_CtoL_EnterBattleGround			,
	EM_PG_BattleGround_LtoL_EnterBattleGround			,
	EM_PG_BattleGround_LtoL_ChangeZoneToBattleGround	,
	EM_PG_BattleGround_LtoC_JoinQueueResult				,
	EM_PG_BattleGround_CtoL_LeaveQueueRequest			,
	EM_PG_BattleGround_CtoL_LeaveBattleGround			,
	EM_PG_BattleGround_LtoC_BattleGroundClose			,
	EM_PG_BattleGround_LtoC_BattleGroundStatus			,
	EM_PG_BattleGround_LtoC_BattleGroundLandMark		,
	EM_PG_BattleGround_LtoChat_AskGroupListToJoin		,
	EM_PG_BattleGround_ChattoL_AskGroupListToJoinResult	,
	EM_PG_BattleGround_LtoC_SetBattleGroundData			,
	EM_PG_BattleGround_CtoL_GetBattleGroundScoreInfo	,
	EM_PG_BattleGround_LtoC_BattleGroundScoreInfo		,
	EM_PG_BattleGround_CtoL_LeaveBattleGroundWaitQueue	,
	EM_PG_BattleGround_LtoL_LeaveBattleGroundWaitQueue	,
	EM_PG_BattleGround_LtoC_BattleGroundWaitQueueStatus	,
	EM_PG_BattleGround_LtoC_AllLandMarkInfo				,
	EM_PG_BattleGround_LtoC_AllBattleGroundPlayerPos	,
	EM_PG_BattleGround_LtoD_GetArenaData				,
	EM_PG_BattleGround_DtoL_UpdateArenaData				,
	EM_PG_BattleGround_LtoC_UpdateArenaData				,
	EM_PG_BattleGround_LtoD_SetArenaData				,
	EM_PG_BattleGround_CtoL_BattleGroundWaitQueueStatusRespond,
	EM_PG_BattleGround_LtoL_BattleGroundWaitQueueStatusRespond,
	EM_PG_BattleGround_CtoL_GetBattleGroundList			,
	EM_PG_BattleGround_LtoL_GetBattleGroundList			,
	EM_PG_BattleGround_WLtoWL_EnterBattleGround			,
	EM_PG_BattleGround_WLtoWL_ChangeZoneToBattleGround	,
	EM_PG_BattleGround_WLtoWL_JoinBattleGround			,
	EM_PG_BattleGround_WLtoWL_GetBattleGroundList		,
	EM_PG_BattleGround_WLtoWL_LeaveBattleGroundWaitQueue,

	EM_PG_BattleGround_WLtoWD_AddRankPoint				,
	EM_PG_BattleGround_WDtoWL_AddRankPointResult		,
	EM_PG_BattleGround_WDtoWL_GetRankPointResult		,
	EM_PG_BattleGround_WLtoWD_GetRankPersonalInfo		,
	EM_PG_BattleGround_WDtoWL_GetRankPersonalInfoResult	,
	EM_PG_BattleGround_CtoL_GetRankPersonalInfo			,
	EM_PG_BattleGround_LtoC_GetRankPersonalInfoResult	,
	EM_PG_BattleGround_LtoC_GetRankListCount			,
	EM_PG_BattleGround_WDtoWL_GetRankList				,
	EM_PG_BattleGround_LtoC_GetRankList					,

	EM_PG_BattleGround_CtoL_LeaveBattleGroundRemote		,
	EM_PG_BattleGround_LtoL_LeaveBattleGroundRemote		,

	EM_PG_GuildHouses_LtoD_BuildHouseRequest			= 2500,
	EM_PG_GuildHouses_DtoL_BuildHouseResult				,
	EM_PG_GuildHouses_LtoC_BuildHouseResult				,
	EM_PG_GuildHouses_LtoC_OpenVisitHouse				,
	EM_PG_GuildHouses_CtoL_VisitHouseRequest			,
	EM_PG_GuildHouses_LtoD_FindHouseRequest				,
	EM_PG_GuildHouses_DtoL_FindHouseResult				,
	EM_PG_GuildHouses_LtoC_FindHouseResult				,
	EM_PG_GuildHouses_CtoL_CloseVisitHouse				,
	EM_PG_GuildHouses_LtoC_CloseVisitHouse				,
	EM_PG_GuildHouses_LtoC_HouseInfoLoading				,
	EM_PG_GuildHouses_LtoD_HouseInfoLoading				,
	EM_PG_GuildHouses_DtoL_HouseBaseInfo				,
	EM_PG_GuildHouses_DtoL_ItemInfo						,
	EM_PG_GuildHouses_DtoL_BuildingInfo					,
	EM_PG_GuildHouses_DtoL_HouseInfoLoadOK				,
	EM_PG_GuildHouses_LtoC_HouseInfoLoadOK				,
	EM_PG_GuildHouses_CtoL_LeaveHouse					,

	EM_PG_GuildHouses_CtoL_CreateBuildingRequest		,
	EM_PG_GuildHouses_LtoC_CreateBuildingResult			,
	EM_PG_GuildHouses_CtoL_BuildingUpgradeRequest		,
	EM_PG_GuildHouses_LtoC_BuildingUpgradeResult		,
	EM_PG_GuildHouses_CtoL_DeleteBuildingRequest		,
	EM_PG_GuildHouses_LtoC_DeleteBuildingResult			,
	
	//公會倉庫
	EM_PG_GuildHouses_CtoL_HouseItemRequest				,
	EM_PG_GuildHouses_LtoC_HouseItemInfo				,
	EM_PG_GuildHouses_CtoL_SwapBodyHouseItem			,
	EM_PG_GuildHouses_LtoC_SwapBodyHouseItemResult		,
	EM_PG_GuildHouses_LtoC_FixHouseItem					,
	EM_PG_GuildHouses_CtoL_SwapHouseItemRequest			,
	EM_PG_GuildHouses_LtoC_SwapHouseItemResult			,

	EM_PG_GuildHouses_CtoL_AllBuildingInfoRequest		,
	EM_PG_GuildHouses_LtoC_AllBuildingInfo				,

	EM_PG_GuildHouses_LtoC_NewBuildingInfo				,
	EM_PG_GuildHouses_LtoC_DelBuildingInfo				,

	EM_PG_GuildHouses_DtoL_FurnitureItemInfo			,
	EM_PG_GuildHouses_LtoC_FurnitureItemInfo			,
	EM_PG_GuildHouses_CtoL_SwapBodyHouseFurniture		,
	EM_PG_GuildHouses_LtoC_SwapBodyHouseFurnitureResult	,
	EM_PG_GuildHouses_LtoC_FixHouseFurniture			,
	EM_PG_GuildHouses_CtoL_SwapHouseFurnitureRequest	,
	EM_PG_GuildHouses_LtoC_SwapHouseFurnitureResult		,
	EM_PG_GuildHouses_CtoL_HouseItemLayoutRequest		,
	EM_PG_GuildHouses_LtoC_HouseItemLayoutResult		,

	EM_PG_GuildHouses_DtoL_StoreConfig					,
	EM_PG_GuildHouses_LtoC_StoreConfig					,
	EM_PG_GuildHouses_CtoL_SetStoreConfig				,
	EM_PG_GuildHouses_LtoC_SetStoreConfigResult			,

	EM_PG_GuildHouses_DtoL_ItemLog						,
	EM_PG_GuildHouses_CtoL_ItemLogRequest				,
	EM_PG_GuildHouses_LtoC_ItemLogList					,

	EM_PG_GuildHouses_CtoL_ActiveBuildingRequest		,
	EM_PG_GuildHouses_LtoC_ActiveBuildingResult			,

	EM_PG_GuildHouses_CtoL_BuyItemPageRequest			,
	EM_PG_GuildHouses_LtoC_BuyItemPageResult			,

	EM_PG_GuildHouses_LtoC_ItemLog						,
	EM_PG_GuildHouses_LtoC_BuildingActiveType			,

	EM_PG_GuildHouses_CtoL_FurnitureSendBack			,
	EM_PG_GuildHouses_LtoC_FurnitureSendBackResult		,

	EM_PG_GuildHouses_CtoL_BuyHouseStyle				,
	EM_PG_GuildHouses_LtoC_BuyHouseStyleResult			,
	EM_PG_GuildHouses_CtoL_SetHouseStyle				,
	EM_PG_GuildHouses_LtoC_SetHouseStyleResult			,
	EM_PG_GuildHouses_CtoL_HouseStyleRequest			,
	EM_PG_GuildHouses_LtoC_HouseStyle					,

	EM_PG_CultivatePet_CtoL_PushPet						=2600,
	EM_PG_CultivatePet_LtoC_PushPetResult				,
	EM_PG_CultivatePet_CtoL_PopPet						,
	EM_PG_CultivatePet_LtoC_PopPetResult				,
	EM_PG_CultivatePet_CtoL_PetName						,
	EM_PG_CultivatePet_LtoC_PetNameResult				,
	EM_PG_CultivatePet_CtoL_PetEvent					,
	EM_PG_CultivatePet_LtoC_PetEventResult				,
	EM_PG_CultivatePet_CtoL_LearnSkill					,
	EM_PG_CultivatePet_LtoC_LearnSkillResult			,
	EM_PG_CultivatePet_CtoL_PetLifeSkill				,
	EM_PG_CultivatePet_LtoC_PetLifeResult				,
	EM_PG_CultivatePet_CtoL_SwapItem					,
	EM_PG_CultivatePet_LtoC_SwapItemResult				,
	EM_PG_CultivatePet_LtoC_FixItem						,
	EM_PG_CultivatePet_LtoC_FixPetBase					,
	EM_PG_CultivatePet_CtoL_GetProductRequest			,
	EM_PG_CultivatePet_LtoC_GetProductResult			,
	EM_PG_CultivatePet_LtoC_PetEventProcess				,
	EM_PG_CultivatePet_CtoL_PetMarge					,
	EM_PG_CultivatePet_LtoC_PetMargeResult				,
	EM_PG_CultivatePet_CtoL_RegPetCard					,
	EM_PG_CultivatePet_LtoC_RegPetCardResult			,

	EM_PG_GuildHousesWar_CtoL_OpenMenu					= 2700,
	EM_PG_GuildHousesWar_LtoD_OpenMenu					,
	EM_PG_GuildHousesWar_DtoC_OpenMenu					,
	EM_PG_GuildHousesWar_CtoL_WarRegister				,
	EM_PG_GuildHousesWar_LtoD_WarRegister				,
	EM_PG_GuildHousesWar_DtoC_WarRegisterResult			,
	EM_PG_GuildHousesWar_DtoL_WarBegin					,
	EM_PG_GuildHousesWar_LtoC_WarBegin					,
	EM_PG_GuildHousesWar_CtoL_EnterWar					,
	EM_PG_GuildHousesWar_LtoC_EnterWarResult			,
	EM_PG_GuildHousesWar_CtoL_LeaveWar					,
	EM_PG_GuildHousesWar_LtoC_LeaveWarResult			,
	EM_PG_GuildHousesWar_LtoC_EndWar					,
	EM_PG_GuildHousesWar_DtoL_WarEnd					,

	EM_PG_GuildHousesWar_LtoD_LoadHouseBaseInfo			,
	EM_PG_GuildHousesWar_DtoL_HouseBaseInfo				,
	EM_PG_GuildHousesWar_DtoL_BuildingInfo				,
	EM_PG_GuildHousesWar_DtoL_HouseInfoLoadOK			,
	EM_PG_GuildHousesWar_DtoL_FurnitureItemInfo			,
	EM_PG_GuildHousesWar_LtoD_WarEndOK					,
	EM_PG_GuildHousesWar_LtoD_DebugTime					,
	EM_PG_GuildHousesWar_LtoC_EndWarPlayerInfo			,

	EM_PG_GuildHousesWar_CtoL_PricesRequst				,
	EM_PG_GuildHousesWar_LtoC_PricesResult				,

	EM_PG_GuildHousesWar_CtoL_HistoryRequest			,
	EM_PG_GuildHousesWar_LtoD_HistoryRequest			,
	EM_PG_GuildHousesWar_DtoC_HistoryResult				,

	EM_PG_GuildHousesWar_LtoC_GuildScore				,
	EM_PG_GuildHousesWar_CtoL_PlayerScoreRequest		,
	EM_PG_GuildHousesWar_LtoC_PlayerScore				,

	EM_PG_Festival_DtoL_FestivalInfo					= 2800 ,
	EM_PG_Festival_LtoD_RequestFestivalInfo			    ,
	EM_PG_Festival_LtoC_FestivalStatus					,

	EM_PG_BG_GuildWar_CtoBL_GuildNameRequest			= 2900,
	EM_PG_BG_GuildWar_BLtoL_GuildNameRequest			,
	EM_PG_BG_GuildWar_LtoBL_GuildNameResult				,
	EM_PG_BG_GuildWar_BLtoC_GuildNameResult				,
	EM_PG_BG_GuildWar_CtoL_OpenMenuRequest				,
	EM_PG_BG_GuildWar_LtoBL_OpenMenuRequest				,
	EM_PG_BG_GuildWar_BLtoL_OpenMenuResult				,
	EM_PG_BG_GuildWar_LtoC_OpenMenuResult				,
	EM_PG_BG_GuildWar_CtoL_RegisterRequest				,
	EM_PG_BG_GuildWar_LtoBL_RegisterRequest				,
	EM_PG_BG_GuildWar_BLtoL_RegisterResult				,
	EM_PG_BG_GuildWar_LtoC_RegisterResult				,
	EM_PG_BG_GuildWar_BLtoL_Status						,
	EM_PG_BG_GuildWar_LtoC_Status						,
	EM_PG_BG_GuildWar_BLtoL_HouseLoadRequest			,
	EM_PG_BG_GuildWar_LtoBL_HouseBaseInfo				,
	EM_PG_BG_GuildWar_LtoBL_HouseBuildingInfo			,
	EM_PG_BG_GuildWar_LtoBL_HouseFurnitureInfo			,
	EM_PG_BG_GuildWar_LtoBL_HouseLoadOK					,
	EM_PG_BG_GuildWar_CtoL_EnterRequest					,
	EM_PG_BG_GuildWar_LtoBL_EnterRequest				,
	EM_PG_BG_GuildWar_BLtoL_EnterResult					,
	EM_PG_BG_GuildWar_LtoC_EnterResult					,
	EM_PG_BG_GuildWar_CtoBL_LeaveRequest				,
	EM_PG_BG_GuildWar_BLtoC_LeaveResult					,
	EM_PG_BG_GuildWar_BLtoC_GuildScore					,
	EM_PG_BG_GuildWar_BLtoC_BeginWar					,
	EM_PG_BG_GuildWar_BLtoC_EndWar						,
	EM_PG_BG_GuildWar_LtoBL_DebugTime					,

	EM_PG_BG_GuildWar_CtoBL_PlayerInfoRequest			,
	EM_PG_BG_GuildWar_BLtoC_PlayerInfoResult			,
	EM_PG_BG_GuildWar_BLtoC_PlayerFinalInfo				,
	EM_PG_BG_GuildWar_BLtoL_EndWarResult				,
	EM_PG_BG_GuildWar_LtoD_EndWarResult					,

	EM_PG_BG_GuildWar_BLtoD_LoadInfo					,
	EM_PG_BG_GuildWar_DtoBL_OrderInfo					,

	EM_PG_BG_GuildWar_BLtoC_PlayerListInfo_Zip			,

	EM_PG_BG_GuildWar_DtoBL_RegGuildWarRankInfo			,
	EM_PG_BG_GuildWar_CtoL_GuildWarRankInfoRequest		,
	EM_PG_BG_GuildWar_LtoBL_GuildWarRankInfoRequest		,
	EM_PG_BG_GuildWar_BLtoL_GuildWarHisotry				,
	EM_PG_BG_GuildWar_LtoC_GuildWarHisotry				,
	EM_PG_BG_GuildWar_BLtoL_GuildWarRankInfo			,
	EM_PG_BG_GuildWar_LtoC_GuildWarRankInfo				,

	EM_PG_Instance_DtoL_ResetTime						= 3000,
	EM_PG_Instance_DtoL_PlayInfo						,
	EM_PG_Instance_LtoC_ResetTime						,
	EM_PG_Instance_LtoC_JoinInstanceRequest				,
	EM_PG_Instance_CtoL_JoinInstanceResult				,
	EM_PG_Instance_LtoC_InstanceReset					,
	EM_PG_Instance_LtoC_InstanceCreate					,

	EM_PG_Instance_CtoL_InstanceExtend					,
	EM_PG_Instance_LtoL_InstanceExtend					,
	EM_PG_Instance_LtoL_InstanceExtendResult			,
	EM_PG_Instance_LtoC_FixInstanceExtend				,

	EM_PG_Instance_CtoL_InstanceRestTimeRequest			,
	EM_PG_Instance_LtoL_InstanceRestTimeRequest			,
	EM_PG_Instance_LtoC_InstanceRestTime				,

	EM_PG_Instance_LtoD_SetWorldVar						,
	EM_PG_Instance_DtoL_SetWorldVar						,
	EM_PG_Instance_CtoL_GetWorldVar						,
	EM_PG_Instance_LtoC_GetWorldVarResult				,

	EM_PG_PE_LtoC_PEConfigData							= 3100,
	EM_PG_PE_LtoC_PEStatus								,
	EM_PG_PE_LtoC_PhaseStatus							,
	EM_PG_PE_LtoC_ObjectiveStatus						,
	EM_PG_PE_LtoC_PlayerScore							,
	EM_PG_PE_LtoC_PEActivated							,
	EM_PG_PE_LtoC_PEDeactivated							,
	EM_PG_PE_LtoC_PEActivePhaseChange					,
	EM_PG_PE_LtoC_ObjectiveValueChage					,
	EM_PG_PE_LtoC_PESuccess								,
	EM_PG_PE_LtoC_PEFailed								,
	EM_PG_PE_LtoC_PhaseSuccess							,
	EM_PG_PE_LtoC_PhaseFailed							,
	EM_PG_PE_LtoC_ObjectiveAchieved						,
	EM_PG_PE_CtoL_RegionChanged							,
	EM_PG_PE_LtoC_ObjectiveFlag							,
	EM_PG_PE_LtoC_PEFlag								,

	EM_PG_EQ_LtoC_LearnSuitSkill_Open					= 3200,
	EM_PG_EQ_CtoL_LearnSuitSkill_Close					,
	EM_PG_EQ_CtoL_LearnSuitSkillRequest					,
	EM_PG_EQ_LtoC_LearnSuitSkillResult					,
	EM_PG_EQ_CtoL_SetSuitSkillRequest					,
	EM_PG_EQ_LtoC_SetSuitSkillResult					,
	EM_PG_EQ_CtoL_UseSuitSkillRequest					,
	EM_PG_EQ_LtoC_UseSuitSkillResult					,

	EM_PG_Login_MtoC_SecondPasswordRequest2				= 3300,
	EM_PG_Login_CtoM_SecondPassword2					,
	EM_PG_Login_MtoC_SecondPasswordResult2				,
	EM_PG_Login_MtoC_CaptchaCheck						,
	EM_PG_Login_CtoM_CaptchaRefresh						,
	EM_PG_Login_CtoM_CaptchaReply						,
	EM_PG_Login_MtoC_CaptchaCheckResult					,
	
    // Log Center 接收用封包列舉.
    // EM_PG_Log_AtoLC = Enum / Package / for Log / All Server send to LogCenter Server
    EM_PG_Log_AtoLC_ExecuteSqlCommand					= 4100,   // 直接執行用(相當於原 EM_PG_DBBase_LtoD_LogSqlCommand)
    EM_PG_Log_AtoLC_ExecuteSqlCommand_Result			,   // 直接執行用回應.
    EM_PG_Log_AtoLC_ExecuteSqlCommand_Dynamic			,   // 動態封包(動態暫存).
    EM_PG_Log_AtoLC_ExecuteSqlCommand_Dynamic_Result	,   // 動態封包(動態暫存)回應.

	EM_PG_Other_CtoL_OpenSession						= 4200,
	EM_PG_Other_CtoL_CloseSession						,	
	EM_PG_Other_LtoC_OpenSessionResult					,
	EM_PG_Other_LtoD_OpenSession						,
	EM_PG_Other_LtoD_CloseSession						,	
	EM_PG_Other_LtoC_FixAccountFlag						,
	EM_PG_Other_CtoL_AutoReportPossibleCheater			,

	EM_PG_Other_DtoL_WeekInstances						,
	EM_PG_Other_CtoL_WeekInstancesRequest				,
	EM_PG_Other_LtoC_WeekInstances						,
	EM_PG_Other_CtoL_WeekInstancesTeleport				,
	EM_PG_Other_LtoC_WeekInstancesTeleportResult		,
	EM_PG_Other_CtoL_WeekInstancesZoneReset				,
	EM_PG_Other_LtoL_WeekInstancesZoneReset				,
	EM_PG_Other_CtoL_WeekInstancesReset					,
	EM_PG_Other_LtoC_WeekInstancesReset					,
	EM_PG_Other_LtoC_WeekInstancesLiveTime				,

	EM_PG_Other_CtoL_PhantomRankUp						,
	EM_PG_Other_LtoC_PhantomRankUpResult				,
	EM_PG_Other_CtoL_PhantomLevelUp						,
	EM_PG_Other_LtoC_PhantomLevelUpResult				,
	EM_PG_Other_CtoL_PhantomEQ							,
	EM_PG_Other_LtoC_PhantomEQResult					,
	EM_PG_Other_LtoC_FixPhantomEQ						,
	EM_PG_Other_LtoC_FixPhantom							,
	EM_PG_Other_LtoC_OpenPhantomFrame					,
	EM_PG_Other_LtoC_OpenWeekInstancesFrame				,
	EM_PG_Other_DtoL_BuffSchedule						,

	EM_PG_Other_LtoC_ItemCombinGroupID					,
	EM_PG_Other_DtoL_ItemCombinInfo						,
	EM_PG_Other_LtoC_ItemCombinInfo						,

	EM_PG_Billing_LtoB_BalanceCheck						= 4300,
	EM_PG_Billing_LtoB_AddAccountMoney					,
	EM_PG_Billing_LtoB_RequestBillingSystemStatus		,
	EM_PG_Billing_BtoL_BalanceCheckResult				,
	EM_PG_Billing_BtoL_AddAccountMoneyResult			,
	EM_PG_Billing_BtoL_BillingSystemStatus				,
	EM_PG_Billing_LtoB_MoneyInfo						,

	EM_PG_Login_CtoL_RequestBonusList					= 4400,
	EM_PG_Login_CtoL_RequestBonus						,

	EM_PG_Login_LtoD_RequestBonusList					= 4120,
	EM_PG_Login_LtoD_RequestBonus						,
	EM_PG_Login_LtoD_RequestBonusComplete				,
	EM_PG_Login_LtoD_CliConnect							,
	EM_PG_Login_LtoD_CliDisconnect						,

	EM_PG_Login_DtoL_BonusList							= 4140,
	EM_PG_Login_DtoL_RequestBonusResult					,
	EM_PG_Login_DtoL_BonusPeriod						,
	EM_PG_Login_LtoC_BonusList							= 4160,
	EM_PG_Login_LtoC_RequestBonusResult					,
	EM_PG_Login_LtoC_BonusPeriod						,

    EM_GamePGCommandEnum_MAX							,
};
enum CliToCliPGCommandEnum
{
	
	EM_PG_CtoC_Test								,	//測試封包

	EM_PG_CtoC_MapPing						    = 100 , //轉送點擊地圖的訊息
	
	EM_PG_CtoC_Emote                            = 200 , //傳送情緒指令	

	EM_PG_CtoC_GetModleInfoRequest				= 250 , //要求取得ModleInfo		 	
	EM_PG_CtoC_GetModleInfoResult				= 251 , //傳回取得ModleInfo結果	 

	EM_PG_CtoC_GiveMeYourFriendInfo			    = 260 , //要求取得好友資訊
	EM_PG_CtoC_GiveYouMyFriendInfo			    = 261 , //回報好友資訊
	EM_PG_CtoC_AmIYourFriend					= 262 , //詢問是否是對方的朋友
	EM_PG_CtoC_AmIYourFriendResult				= 263 , //詢問是否是對方的朋友結果

	EM_PG_CtoC_AskPlayerInfo					= 264 , //查詢玩家資訊
	EM_PG_CtoC_AskPlayerInfoResult				= 265 , //查詢玩家資訊結果
 
	EM_PG_CtoC_Message							= 300,	// Cli To Cli 傳送訊息
	//EM_PG_CtoC_DelFriend						= 350,  // 通知刪除好友 //2008-07-10 好友名單從雙向改成單向 不用再通知對方刪除
	EM_PG_CtoC_PlayerTarget						= 351,	// 範圍廣播玩家目標
  	EM_PG_CtoC_SetRaidTarget					= 352,	// 設定團隊目標
	EM_PG_CtoC_TrackPlayer						= 400,	// 追蹤玩家
	EM_PG_CtoC_TrackPlayerResult				= 401,	// 追蹤玩家 回報玩家座標

	EM_PG_CtoC_InviteChannel					=500,	//要求加入頻道 msn
	EM_PG_CtoC_KickChannel						=501,	//踢出頻道     msn

 	EM_PG_CtoC_ChannelInvite					= 510,	//邀請加入頻道
	

	EM_PG_CtoC_AskHasItem						= 600,	// 訊問玩家是否有此物品
	EM_PG_CtoC_HasItemResult					= 601,	// 回應查詢結果
	EM_PG_CtoC_AskRideMount						= 602,	// 訊問玩家是否要塔乘坐騎
	EM_PG_CtoC_RideMountResult					= 603,	// 回應查詢結果


	EM_PG_CtoC_LeaderChange 					= 700,	// 訊問玩家是否要當會長
	EM_PG_CtoC_LeaderChangeResult				= 701,	// 回應結果

	EM_PG_Party_CtoC_AskJoinToParty				= 800,	// 申請加入隊伍
	EM_PG_Party_CtoC_AskJoinToPartyResult		= 801,	// 申請加入隊伍的回應
	EM_PG_Party_CtoC_DeletePBPost				= 802,	// 要求刪除在組隊招募留言板上的留言

	EM_CliToCliPGCommandEnum_MAX				,

};

//-----------------------------------------------------------------------------------
//Client to Client 封包機本型定義
//---------------------------------------------------------------------------
//封包基本型
struct ClitoCliPacketBaseStruct
{
	CliToCliPGCommandEnum	Command;
	char    Data[0];
};

//-----------------------------------------------------------------------------------
//封包機本型定義
//---------------------------------------------------------------------------
//封包基本型
struct PacketBaseStruct
{
    GamePGCommandEnum	Command;
    char    Data[0];
};
//-----------------------------------------------------------------------------------
//封包基本型
struct PacketBaseMaxStruct
{
    GamePGCommandEnum	Command;
    char    Data[0x1000];
};
//-----------------------------------------------------------------------------------
//封包基本型
struct PacketBaseDefine
{
	GamePGCommandEnum	Command;
	PacketBaseDefine( GamePGCommandEnum tCommand ){ Command = tCommand; }
};
//-----------------------------------------------------------------------------------
//全域的ID
union GItemData
{
    int GUID;
    struct
    {
        short ID;
        short Zone;
    };
};

//-----------------------------------------------------------------------------------
//修改物件資訊
struct ModifyNPCInfo
{
    int     GItemID;
    int		OrgObjID;	
    float 	X;			
    float 	Y;			
    float 	Z;			
    float 	Dir;			
    int     QuestID;
    int     Mode;        
    int     PID;         
    StaticString <_MAX_NAMESTRING_LEN_> Name;
    StaticString <_MAX_NAMESTRING_LEN_> AutoPlot;
    StaticString <_MAX_NAMESTRING_LEN_> ClassName;
	float	vX , vY , vZ;
};
//單次Tracert記錄分析結果
struct Traceoute
{
	unsigned long round_time;	//往返時間
	unsigned long ip_address;	//對端IP位址
	Traceoute()
	{
		round_time = 0;
		ip_address = 0;
	}
} ;
//Tracert總結果
struct TracertList
{
	unsigned long count;
	Traceoute Tracert[30];
} ;
//-----------------------------------------------------------------------------------
//遊戲物件兩者互相的關係
struct RoleRelationStruct
{
	union
	{
		int	Value;
		struct  
		{
			bool	CanAttack		: 1;
			bool	TouckPlot		: 1;	//有處碰劇情
			bool	Enemy			: 1;	//會攻擊玩家(本地玩家)
			bool	BGTouch			: 1;	//戰場中是否可點擊
		};
	};
	RoleRelationStruct()
	{
		Value = 0;
	}
};
//-----------------------------------------------------------------------------------
// 要加 Server 類型, EM_SERVER_TYPE_END 一定要調整

enum EM_SERVER_TYPE
{
	EM_SERVER_TYPE_MASTER		=	0,
	EM_SERVER_TYPE_LOCAL		,   // 1
	EM_SERVER_TYPE_CHAT			,   // 2
	EM_SERVER_TYPE_DATACENTER	,   // 3
    EM_SERVER_TYPE_PARTITION	,   // 4
	EM_SERVER_TYPE_GMTOOLS  	,   // 5
	EM_SERVER_TYPE_ROLECENTER  	,   // 6
	EM_SERVER_TYPE_NONAGE		,   // 7
	EM_SERVER_TYPE_APEX			,   // 8
    EM_SERVER_TYPE_LOGCENTER    ,   // 9
	EM_SERVER_TYPE_BILLING		,   // 10

//    EM_SERVER_TYPE_SMALLGAME	, //測試
	EM_SERVER_TYPE_END			= 16,

    EM_SERVER_TYPE_MAX         = 0xffffffff,
};



#define DF_NO_LOCALID			-1
#define DF_REQUEST_LOCALID			-2

//-----------------------------------------------------------------------------------
// 小輝泛用 Define

#define		DF_OK				0

// 小輝自定 Define
//------------------------------------------------------
#define M_PACKET( type ) type* pPacket = (type*)pData; type* p = (type*)pData
#define M_CLIENT_OBJ( id ) BaseItemObject*	pObj =	Global::GetObjBySockID( id ); if( pObj == NULL ) return
//------------------------------------------------------
#define PACKET_CONSTRUCT( type ) GamePGCommandEnum Command; type(){ Command = EM_##type; }

#define  Srv_NetCliReg( PGName )	_Net->RegOnCliPacketFunction( EM_##PGName , _##PGName , sizeof(PGName) , sizeof(PGName) )
#define  Cli_NetReg( PGName )		{ _Net.RegOnSrvPacketFunction( EM_##PGName , _##PGName );_NetBattle.RegOnSrvPacketFunction( EM_##PGName , _##PGName ); }

#define  Server_Reg_Client_Packet( PGName )  _Net->RegOnCliPacketFunction( EM_##PGName , _##PGName , sizeof(PGName) , sizeof(PGName) )
#define  Server_Reg_Server_Packet( PGName )  _Net->RegOnSrvPacketFunction( EM_##PGName , _##PGName )
#define  Server_Reg_World_Packet( PGName )  _Net->RegOnOtherWorldPacketFunction( EM_##PGName , _##PGName )

#define  RegServerPacket( PGName )  _Net.RegOnSrvPacketFunction( EM_##PGName , _##PGName )

#define  Cli_NetReg_NonStatic( ClassName , PGName )   {	OnSrvPacketFunctionBase pf;		\
														pf = boost::bind<void>( &##ClassName::_##PGName , this , _1 , _2 , _3 );\
														_Global->Net()->RegOnSrvPacketFunction( EM_##PGName  , pf );\
														_Global->NetBattle()->RegOnSrvPacketFunction( EM_##PGName , pf ); }

//#define  
//-----------------------------------------------------------------------------------
#endif //__NETWAKERPGEM_H__