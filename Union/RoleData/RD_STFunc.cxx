#include "RoleDataEx.h"
//-----------------------------------------------------------------------------------------
boost::function< int(int) >             					RoleDataEx::St_GetKeyMagicID        = Empty_GetKeyMagicID;		
boost::function< GameObjDbStructEx*(int) >             	RoleDataEx::St_GetObjDB             = Empty_GetObjDB;		
boost::function< RoleDataEx*(int) >                    	RoleDataEx::St_GetRoleData          = Empty_GetRoleData;		
boost::function< int() >	                            	RoleDataEx::St_GenerateItemVersion  = Empty_GenerateItemVersionFunction;
boost::function< bool(ItemFieldStruct&,int,int) >			RoleDataEx::St_NewItemInit          = Empty_NewitemInit;
boost::function< void(RoleDataEx* , RoleUpdateEvent) > 	RoleDataEx::St_UpDateEvent          = Empty_UpDateEvent;
boost::function< bool(RoleDataEx*, RoleDataEx*) >      	RoleDataEx::St_ProcessDead          = Empty_ProcessDead;
boost::function< bool(RoleDataEx*,MagicProcInfo*,int) >	RoleDataEx::St_SpellMagic           = Empty_SpellMagic;
boost::function< int(int,float,float,float,float,int,int) >RoleDataEx::St_CreateObj			= Empty_CreateObj;
boost::function< bool(int,int) >							RoleDataEx::St_AddToPartition		= Empty_AddToPartition;
boost::function< bool(int) >								RoleDataEx::St_DelFromPartition		= Empty_DelFromPartition;
boost::function< void(int) >				             	RoleDataEx::St_ClearPath			= Empty_ClearPath;
boost::function< void(int) >				             	RoleDataEx::St_SetPos				= Empty_ClearPath;
boost::function< const char*(RoleDataEx*) >			   	RoleDataEx::St_GetRoleName			= Empty_GetRoleName;
boost::function< SysKeyValueStruct*() >	             	RoleDataEx::St_SysKeyValue			= Empty_SysKeyValueStruct;
boost::function< GameObjDbStructEx*(GameObjDbStructEx*,GameObjDbStructEx*) >	RoleDataEx::St_RuneExchange = Empty_RuneExchange;
boost::function< GameObjDbStructEx*(GameObjDbStructEx*) >	RoleDataEx::St_NextRuneLv			= Empty_NextRuneLv;
boost::function< int(const char* ) >		             	RoleDataEx::St_GetSysKeyValue		= Empty_GetSysKeyValue;
boost::function< int(const char*) >						RoleDataEx::St_GetPCenterID			= Empty_GetPCenterID;
//-----------------------------------------------------------------------------------------
#define Def_Disabled_Update_Flag 0x01
#define Def_Disabled_Insert_Flag 0x02
#define Def_Disabled_Select_Flag 0x04
//-----------------------------------------------------------------------------------------
void RoleDataEx::OnTimeProcAll( )
{
    G_SysTime = timeGetTime() - G_SysTime_Base;
    G_Clock++;

}
//----------------------------------------------------------------------------------------------------------------
ReaderClass<PlayerRoleData>* RoleDataEx::ReaderBase_Account( )
{
	static  ReaderClass<PlayerRoleData> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		char    Buf[256];
		
		PlayerRoleData* Pt = NULL;
		Reader.SetData(0, "AccountMoney"  , &Pt->PlayerBaseData.Body.Money_Account	, ENUM_Type_IntPoint       );
		Reader.SetData(0, "BonusMoney"	, &Pt->PlayerBaseData.Money_Bonus			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "ManageLv"		, &Pt->PlayerBaseData.ManageLV				, ENUM_Type_IntPoint      , 0			, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );
		Reader.SetData(0, "Password"		,  Pt->PlayerBaseData.Password.Begin()		, ENUM_Type_CharString    , sizeof(Pt->PlayerBaseData.Password)	, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );
		Reader.SetData(0, "PlayTimeQuota" , &Pt->PlayerBaseData.PlayTimeQuota			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "LogoutTime"	, &Pt->Base.LogoutTime						, ENUM_Type_IntPoint      , 0			, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );

		Reader.SetData(0, "LockAccountTime"			, &Pt->PlayerBaseData.LockAccountTime			, ENUM_Type_IntPoint		);
		for( int i = 0; i < Def_AccountMoneyLockTime_Max_Count_ ; i++ )
		{
			sprintf_s( Buf , sizeof(Buf) , "LockAccountMoney%d" , i+1  );
			Reader.SetData(0, Buf						, &Pt->PlayerBaseData.LockAccountMoney[i]		, ENUM_Type_IntPoint		);
		}
		Reader.SetData(0, "LockAccountMoney_Forever"	, &Pt->PlayerBaseData.LockAccountMoney_Forever	, ENUM_Type_IntPoint		);
		Reader.SetData(0, "AccountFlag"	, &Pt->Base.AccountFlag						, ENUM_Type_BinaryData	, sizeof(Pt->Base.AccountFlag) );

		Reader.SetData(0, "VipMemberType"		, &Pt->PlayerBaseData.VipMember.Type		, ENUM_Type_IntPoint      , 0			, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );
		Reader.SetData(0, "VipMemberTerm"		, &Pt->PlayerBaseData.VipMember.Term		, ENUM_Type_SmallDateTime , 0			, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );
		Reader.SetData(0, "VipMemberFlag"		, &Pt->PlayerBaseData.VipMember._Flag		, ENUM_Type_IntPoint      , 0			, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );
		Reader.SetData(0, "UntrustFlag"		, &Pt->PlayerBaseData.VipMember.UntrustFlag._Flag		, ENUM_Type_IntPoint      , 0			, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );
		Reader.SetData(0, "CoolClothList"		, &Pt->PlayerBaseData.CoolClothList			, ENUM_Type_BinaryData		, sizeof(Pt->PlayerBaseData.CoolClothList) );
		IsReady = true;
	}      
	return &Reader;
}

ReaderClass<PlayerRoleData>* RoleDataEx::ReaderBase( )
{
    static  ReaderClass<PlayerRoleData> Reader;
    static  bool    IsReady = false;
    if( IsReady == false )
    {   
        char    Buf[256];
        PlayerRoleData* Pt = NULL;
        Reader.SetData(0, "CharField"     , &Pt->Base.CharField               , ENUM_Type_IntPoint       );
        Reader.SetData(0, "DBID"          , &Pt->Base.DBID                    , ENUM_Type_IntPoint       , 0								, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag);
		Reader.SetData(0, "Account_ID"    ,  Pt->Base.Account_ID.Begin()      , ENUM_Type_CharString     , sizeof(Pt->Base.Account_ID)	, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );
        Reader.SetData(0, "CreateTime"    ,  Pt->Base.CreateTime.Begin()      , ENUM_Type_CharString     , sizeof(Pt->Base.CreateTime)	, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag);
        Reader.SetData(0, "LogoutTime"    , &Pt->Base.LogoutTime              , ENUM_Type_IntPoint       );
        Reader.SetData(0, "Type"          , &Pt->Base.Type                    , ENUM_Type_IntPoint       , 0								, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag);

        Reader.SetData(0, "Mode"          , &Pt->BaseData.Mode                , ENUM_Type_IntPoint       );
		Reader.SetData(0, "RoleName"      ,  Pt->BaseData.RoleName.Begin()    , ENUM_Type_WCharString    , sizeof(Pt->BaseData.RoleName)  , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );
        Reader.SetData(0, "ZoneID"        , &Pt->BaseData.ZoneID              , ENUM_Type_IntPoint       );
		Reader.SetData(0, "X"             , &Pt->BaseData.Pos.X               , ENUM_Type_FloatPoint     );
		Reader.SetData(0, "Y"             , &Pt->BaseData.Pos.Y               , ENUM_Type_FloatPoint     );
        Reader.SetData(0, "Z"             , &Pt->BaseData.Pos.Z               , ENUM_Type_FloatPoint     );
        Reader.SetData(0, "Dir"           , &Pt->BaseData.Pos.Dir             , ENUM_Type_FloatPoint     );
        Reader.SetData(0, "RoomID"        , &Pt->BaseData.RoomID              , ENUM_Type_IntPoint       );
		Reader.SetData(0, "vX"            , &Pt->BaseData.vX               	, ENUM_Type_FloatPoint     );
		Reader.SetData(0, "vY"            , &Pt->BaseData.vY               	, ENUM_Type_FloatPoint     );
		Reader.SetData(0, "vZ"            , &Pt->BaseData.vZ               	, ENUM_Type_FloatPoint     );


		Reader.SetData(0, "PlayTime"      , &Pt->BaseData.PlayTime            , ENUM_Type_IntPoint		);

		Reader.SetData(0, "IsShowTitle"	, &Pt->BaseData.IsShowTitle			, ENUM_Type_BitPoint       );
		Reader.SetData(0, "TitleStr"      ,  Pt->PlayerBaseData.TitleStr.Begin()    , ENUM_Type_WCharString     , sizeof(Pt->PlayerBaseData.TitleStr)  , ENUM_Indirect_Address   );
        Reader.SetData(0, "TitleID"       , &Pt->BaseData.TitleID             , ENUM_Type_IntPoint       );        
        Reader.SetData(0, "Race"          , &Pt->BaseData.Race                , ENUM_Type_IntPoint       );        
        Reader.SetData(0, "Voc"           , &Pt->BaseData.Voc                 , ENUM_Type_IntPoint       );        
        Reader.SetData(0, "Voc_Sub"       , &Pt->BaseData.Voc_Sub             , ENUM_Type_IntPoint       );        
		Reader.SetData(0, "LV"            , &Pt->TempData.Attr.Level			, ENUM_Type_IntPoint       );        
		Reader.SetData(0, "LV_Sub"        , &Pt->TempData.Attr.Level_Sub      , ENUM_Type_IntPoint       );        

        //Reader.SetData(0, "Famous"        , &Pt->PlayerBaseData.Famous              , ENUM_Type_IntPoint       );        
        Reader.SetData(0, "Sex"           , &Pt->BaseData.Sex                 , ENUM_Type_IntPoint       );        
        Reader.SetData(0, "HP"            , &Pt->BaseData.HP                  , ENUM_Type_FloatPoint     );        
        Reader.SetData(0, "MP"            , &Pt->BaseData.MP                  , ENUM_Type_FloatPoint     );        
		Reader.SetData(0, "StomachPoint"  , &Pt->BaseData.StomachPoint        , ENUM_Type_IntPoint       );        
//		Reader.SetData(0, "AddExp"        , &Pt->PlayerBaseData.AddExp              , ENUM_Type_IntPoint       );        
//		Reader.SetData(0, "AddTp"         , &Pt->PlayerBaseData.AddTp               , ENUM_Type_IntPoint       );        

		Reader.SetData(0, "MaxLv"         , &Pt->PlayerBaseData.MaxLv         , ENUM_Type_IntPoint       );        
 
		Reader.SetData(0, "BodyCount"     , &Pt->PlayerBaseData.Body.Count    , ENUM_Type_IntPoint       );        
		Reader.SetData(0, "BankCount"     , &Pt->PlayerBaseData.Bank.Count	, ENUM_Type_IntPoint       );      
		Reader.SetData(0, "BodyMoney"     , &Pt->PlayerBaseData.Body.Money    , ENUM_Type_IntPoint       );        
		Reader.SetData(0, "BankMoney"     , &Pt->PlayerBaseData.Bank.Money	, ENUM_Type_IntPoint       );      

        Reader.SetData(0, "RevZoneID"     , &Pt->SelfData.RevZoneID     		, ENUM_Type_IntPoint       );
        Reader.SetData(0, "RevX"          , &Pt->SelfData.RevPos.X      		, ENUM_Type_FloatPoint     );
        Reader.SetData(0, "RevY"          , &Pt->SelfData.RevPos.Y      		, ENUM_Type_FloatPoint     );
        Reader.SetData(0, "RevZ"          , &Pt->SelfData.RevPos.Z      		, ENUM_Type_FloatPoint     );
        Reader.SetData(0, "RevDir"        , &Pt->SelfData.RevPos.Dir    		, ENUM_Type_FloatPoint     );

		Reader.SetData(0, "ReturnZoneID"  , &Pt->SelfData.ReturnZoneID  		, ENUM_Type_IntPoint       );
		Reader.SetData(0, "ReturnX"       , &Pt->SelfData.ReturnPos.X   		, ENUM_Type_FloatPoint     );
		Reader.SetData(0, "ReturnY"       , &Pt->SelfData.ReturnPos.Y   		, ENUM_Type_FloatPoint     );
		Reader.SetData(0, "ReturnZ"       , &Pt->SelfData.ReturnPos.Z   		, ENUM_Type_FloatPoint     );
		Reader.SetData(0, "ReturnDir"     , &Pt->SelfData.ReturnPos.Dir 		, ENUM_Type_FloatPoint     );


        Reader.SetData(0, "KeyItem"		, &Pt->BaseData.KeyItem				, ENUM_Type_BinaryData  , sizeof(Pt->BaseData.KeyItem)  );        
		Reader.SetData(0, "Card"			, &Pt->PlayerBaseData.Card			, ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.Card)  );        
        Reader.SetData(0, "Title"			, &Pt->PlayerBaseData.Title         , ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.Title)                 );        
        Reader.SetData(0, "QuestHistory"	, &Pt->PlayerBaseData.Quest.QuestHistory  , ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.Quest.QuestHistory)    );        
		Reader.SetData(0, "RentFunction"	, &Pt->PlayerBaseData.RentFun		, ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.RentFun) );        

		Reader.SetData(0, "QuestLog"			, &Pt->PlayerBaseData.Quest.QuestLog		, ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.Quest.QuestLog)    );  

		Reader.SetData(0, "QuestDaily"		, &Pt->PlayerBaseData.Quest.QuestDaily		, ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.Quest.QuestDaily)    );        
		Reader.SetData(0, "QuestGroup"		, &Pt->PlayerBaseData.Quest.iQuestGroup		, ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.Quest.iQuestGroup)    );
		Reader.SetData(0, "LastCompleteTime"	, &Pt->PlayerBaseData.Quest.LastCompleteTime	, ENUM_Type_IntPoint   );


        for( int i = 0 ; i < _MAX_SKILLVALUE_TYPE ; i++ )
        {
            sprintf_s( Buf , sizeof(Buf) , "SkillValue%d" , i + 1 );
            Reader.SetData(0, Buf        , &Pt->BaseData.SkillValue.Skill[i]  , ENUM_Type_FloatPoint     );
        }

        Reader.SetData(0, "Buff"          , &Pt->BaseData.Buff                , ENUM_Type_BinaryData  , sizeof(Pt->BaseData.Buff)  );        
        Reader.SetData(0, "DeadCountDown" , &Pt->PlayerBaseData.DeadCountDown       , ENUM_Type_IntPoint     );
        
        //®e»ª
        Reader.SetData(0, "FaceID"          , &Pt->BaseData.Look.FaceID       , ENUM_Type_IntPoint       );
        Reader.SetData(0, "HairID"          , &Pt->BaseData.Look.HairID       , ENUM_Type_IntPoint       );
        Reader.SetData(0, "HairColor"       , &Pt->BaseData.Look.HairColor    , ENUM_Type_IntPoint       );
        Reader.SetData(0, "BodyColor"       , &Pt->BaseData.Look.BodyColor    , ENUM_Type_IntPoint       );

		Reader.SetData(0, "BoneScale"		  , &Pt->BaseData.Look.BoneScale  , ENUM_Type_BinaryData  , sizeof(Pt->BaseData.Look.BoneScale)  );        

        Reader.SetData(0, "OrgObjID"        , &Pt->BaseData.ItemInfo.OrgObjID , ENUM_Type_IntPoint       );
		Reader.SetData(0, "PartyID"         , &Pt->BaseData.PartyID			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "GuildID"         , &Pt->BaseData.GuildID			, ENUM_Type_IntPoint       , 0			, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag);
		Reader.SetData(0, "GuildLeaveTime"  , &Pt->BaseData.GuildLeaveTime	, ENUM_Type_SmallDateTime  , 0			, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag);

		Reader.SetData(0, "FriendNote"		,  Pt->PlayerSelfData.FriendList.Note.Begin()			, ENUM_Type_WCharString , sizeof(Pt->PlayerSelfData.FriendList.Note)   );

        Reader.SetData(0, "Coldown"          , &Pt->BaseData.Coldown           , ENUM_Type_BinaryData  , sizeof(Pt->BaseData.Coldown)  );
		Reader.SetData(0, "RefineColdown"    , &Pt->PlayerBaseData.RefineColdown     , ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.RefineColdown)  );

		Reader.SetData(0, "IsBillboardAnonymous"	, &Pt->PlayerBaseData.IsBillboardAnonymous	, ENUM_Type_BitPoint       );
		Reader.SetData(0, "PKScore"				, &Pt->PlayerBaseData.PKScore					, ENUM_Type_IntPoint       );	
		Reader.SetData(0, "TitleCount"			, &Pt->PlayerBaseData.TitleCount				, ENUM_Type_IntPoint       );	
		Reader.SetData(0, "HouseDBID"				, &Pt->PlayerBaseData.HouseDBID					, ENUM_Type_IntPoint       , 0			, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );	
		Reader.SetData(0, "ClientFlag"			, &Pt->PlayerBaseData.ClientFlag				, ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.ClientFlag)  );        
		Reader.SetData(0, "ClientData"			, &Pt->PlayerBaseData.ClientData				, ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.ClientData)  );        

		Reader.SetData(0, "FriendClientData"		, &Pt->PlayerSelfData.FriendList.ClientData		, ENUM_Type_BinaryData  , sizeof(Pt->PlayerSelfData.FriendList.ClientData)  );        
		Reader.SetData(0, "FriendGroupSortID"		, &Pt->PlayerSelfData.FriendList.GroupSortID	, ENUM_Type_BinaryData  , sizeof(Pt->PlayerSelfData.FriendList.GroupSortID)  );        
		Reader.SetData(0, "MonsterHunter"			, &Pt->PlayerSelfData.MonsterHunter				, ENUM_Type_BinaryData  , sizeof(Pt->PlayerSelfData.MonsterHunter)  );        

		Reader.SetData(0, "SysFlag"				, &Pt->BaseData.SysFlag._Value					, ENUM_Type_IntPoint		);        

		for( int i = 0 ; i < _MAX_FAMOUS_COUNT_ ; i++ )
		{
			sprintf_s( Buf , sizeof(Buf) , "Famous%d" , i+1  );
			Reader.SetData(0, Buf					,  &Pt->PlayerBaseData.Famous[i]				, ENUM_Type_IntPoint		);
		}
		Reader.SetData(0, "DestroyTime"			, &Pt->PlayerBaseData.DestroyTime				, ENUM_Type_IntPoint       , 0								, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag); 
//		Reader.SetData(0, "ShowEQ"				, &Pt->PlayerBaseData.ShowEQ._Value				, ENUM_Type_IntPoint		);        
        Reader.SetData(0, "GoodEvil"				, &Pt->PlayerBaseData.GoodEvil       			, ENUM_Type_FloatPoint		);
        Reader.SetData(0, "GoodEvilBonus"			, &Pt->PlayerBaseData.GoodEvilBonus  			, ENUM_Type_FloatPoint		);
        Reader.SetData(0, "VocCount"				, &Pt->PlayerBaseData.VocCount       			, ENUM_Type_IntPoint		);
		Reader.SetData(0, "BoxEnergy"				, &Pt->PlayerBaseData.BoxEnergy       			, ENUM_Type_IntPoint		);

//		Reader.SetData(0, "BoxEnergy"				, &Pt->PlayerBaseData.BoxEnergy       		, ENUM_Type_IntPoint		);

		Reader.SetData(0, "RealTimeFlag"			, &Pt->PlayerBaseData.RealTimeFlag				, ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.RealTimeFlag)  );        
		Reader.SetData(0, "PlayTimeFlag"			, &Pt->PlayerBaseData.PlayTimeFlag				, ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.PlayTimeFlag)  );        
		Reader.SetData(0, "LuaValueFlag"			, &Pt->PlayerBaseData.LuaValueFlag				, ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.LuaValueFlag)  );        
		Reader.SetData(0, "MedalCount"			, &Pt->PlayerBaseData.MedalCount       			, ENUM_Type_IntPoint		);
//		Reader.SetData(0, "Money_Bonus"			, &Pt->PlayerBaseData.Money_Bonus       			, ENUM_Type_IntPoint		);
		Reader.SetData(0, "LocationSave"			, &Pt->PlayerBaseData.LocationSave				, ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.LocationSave)  );        
		Reader.SetData(0, "HorseColor"			, &Pt->BaseData.HorseColor						, ENUM_Type_BinaryData  , sizeof(Pt->BaseData.HorseColor)  );

		Reader.SetData(0, "RaiseMode"				, &Pt->PlayerBaseData.RaiseInfo.Mode.Mode		, ENUM_Type_IntPoint		);
		Reader.SetData(0, "RaiseExpRate"			, &Pt->PlayerBaseData.RaiseInfo.ExpRate  		, ENUM_Type_IntPoint		);
		Reader.SetData(0, "RaiseZoneID"			, &Pt->PlayerBaseData.RaiseInfo.ZoneID   		, ENUM_Type_IntPoint		);
		Reader.SetData(0, "RaiseRevX"				, &Pt->PlayerBaseData.RaiseInfo.RevX     		, ENUM_Type_FloatPoint		);
		Reader.SetData(0, "RaiseRevY"				, &Pt->PlayerBaseData.RaiseInfo.RevY     		, ENUM_Type_FloatPoint		);
		Reader.SetData(0, "RaiseRevZ"				, &Pt->PlayerBaseData.RaiseInfo.RevZ     		, ENUM_Type_FloatPoint		);

//		Reader.SetData(0, "TombItemGUID"			, &Pt->PlayerSelfData.Tomb.ItemGUID			, ENUM_Type_IntPoint		);
		Reader.SetData(0, "TombCreateTime"		, &Pt->PlayerSelfData.Tomb.CreateTime			, ENUM_Type_IntPoint		);
		Reader.SetData(0, "TombZoneID"			, &Pt->PlayerSelfData.Tomb.ZoneID				, ENUM_Type_IntPoint		);
		Reader.SetData(0, "TombX"					, &Pt->PlayerSelfData.Tomb.X					, ENUM_Type_FloatPoint		);
		Reader.SetData(0, "TombY"					, &Pt->PlayerSelfData.Tomb.Y					, ENUM_Type_FloatPoint		);
		Reader.SetData(0, "TombZ"					, &Pt->PlayerSelfData.Tomb.Z					, ENUM_Type_FloatPoint		);
		Reader.SetData(0, "TombDebtExp"			, &Pt->PlayerSelfData.Tomb.DebtExp				, ENUM_Type_IntPoint		);
		Reader.SetData(0, "TombDebtTp"			, &Pt->PlayerSelfData.Tomb.DebtTp				, ENUM_Type_IntPoint		);
		Reader.SetData(0, "TombExp"				, &Pt->PlayerSelfData.Tomb.Exp					, ENUM_Type_IntPoint		);

		Reader.SetData(0, "Honor"					, &Pt->PlayerBaseData.Honor						, ENUM_Type_FloatPoint		);
		Reader.SetData(0, "HonorWorth"			, &Pt->PlayerBaseData.HonorWorth				, ENUM_Type_FloatPoint		);
		Reader.SetData(0, "LastBeHonorKilledTime"	, &Pt->PlayerBaseData.LastBeHonorKilledTime		, ENUM_Type_IntPoint		);
		Reader.SetData(0, "MaxPlantCount"			, &Pt->PlayerBaseData.MaxPlantCount				, ENUM_Type_IntPoint		);

		Reader.SetData(0, "MoneyTraderCount"		, &Pt->PlayerBaseData.Defendant.MoneyTraderCount, ENUM_Type_FloatPoint		);
		Reader.SetData(0, "AutoBotCount"			, &Pt->PlayerBaseData.Defendant.AutoBotCount	, ENUM_Type_FloatPoint		);

		Reader.SetData(0, "BotReportDBID"			, &Pt->PlayerBaseData.Defendant.BotReportDBID	, ENUM_Type_IntPoint		);
		Reader.SetData(0, "BotReportTime"			, &Pt->PlayerBaseData.Defendant.BotReportTime	, ENUM_Type_IntPoint		);
		Reader.SetData(0, "BotReportProtectTime"	, &Pt->PlayerBaseData.Defendant.BotReportProtectTime	, ENUM_Type_IntPoint		);

//		Reader.SetData(0, "InstanceSetting"		, &Pt->PlayerBaseData.InstanceSetting			, ENUM_Type_BinaryData		, sizeof(Pt->PlayerBaseData.InstanceSetting) );
		Reader.SetData(0, "InstanceSetting_KeyID"		, &Pt->PlayerBaseData.InstanceSetting.KeyID		, ENUM_Type_BinaryData		, sizeof(Pt->PlayerBaseData.InstanceSetting.KeyID) );
		Reader.SetData(0, "InstanceSetting_LiveTime"	, &Pt->PlayerBaseData.InstanceSetting.LiveTime	, ENUM_Type_BinaryData		, sizeof(Pt->PlayerBaseData.InstanceSetting.LiveTime) );
		Reader.SetData(0, "InstanceSetting_ExtendTime", &Pt->PlayerBaseData.InstanceSetting.ExtendTime, ENUM_Type_BinaryData		, sizeof(Pt->PlayerBaseData.InstanceSetting.ExtendTime) );


		Reader.SetData(0, "ExpDownRate"			, &Pt->PlayerBaseData.RaiseInfo.ExpDownRate		, ENUM_Type_IntPoint		);
		Reader.SetData(0, "DebtExpRate"			, &Pt->PlayerBaseData.RaiseInfo.DebtExpRate		, ENUM_Type_IntPoint		);
		Reader.SetData(0, "DebtTpRate"			, &Pt->PlayerBaseData.RaiseInfo.DebtTpRate		, ENUM_Type_IntPoint		);

		Reader.SetData(0, "RoomKey"				, &Pt->PlayerBaseData.RoomKey					, ENUM_Type_IntPoint		);

		Reader.SetData(0, "SuitSkill_LearnCount"	, &Pt->PlayerBaseData.SuitSkill.LeanSkillCount	, ENUM_Type_IntPoint		);
		Reader.SetData(0, "SuitSkill_MaxEqCount"	, &Pt->PlayerBaseData.SuitSkill.MaxEqSkillCount	, ENUM_Type_IntPoint		);
		Reader.SetData(0, "SuitSkill_Learn"		, &Pt->PlayerBaseData.SuitSkill.LeanSkillList	, ENUM_Type_BinaryData		, sizeof(Pt->PlayerBaseData.SuitSkill.LeanSkillList));
		Reader.SetData(0, "SuitSkill_EQ"			, &Pt->PlayerBaseData.SuitSkill.EQ				, ENUM_Type_BinaryData		, sizeof(Pt->PlayerBaseData.SuitSkill.EQ));

		Reader.SetData(0, "Pet_NextMergeResetTime", &Pt->PlayerBaseData.Pet.NextMergeResetTime	, ENUM_Type_IntPoint		);
		Reader.SetData(0, "Pet_Money"				, &Pt->PlayerBaseData.Pet.Money					, ENUM_Type_IntPoint		);

		Reader.SetData(0, "CoolSuitList"			, &Pt->PlayerBaseData.CoolSuitList				, ENUM_Type_BinaryData		, sizeof(Pt->PlayerBaseData.CoolSuitList) );
//		Reader.SetData(0, "CoolClothList"			, &Pt->PlayerBaseData.CoolClothList				, ENUM_Type_BinaryData		, sizeof(Pt->PlayerBaseData.CoolClothList) );
		Reader.SetData(0, "CoolSuitID"			, &Pt->PlayerBaseData.CoolSuitID				, ENUM_Type_IntPoint		);

		Reader.SetData(0, "PetExBag"				, &Pt->PlayerBaseData.PetExBag					, ENUM_Type_BinaryData		, sizeof(Pt->PlayerBaseData.PetExBag) );
		Reader.SetData(0, "HorseExBag"			, &Pt->PlayerBaseData.HorseExBag				, ENUM_Type_BinaryData		, sizeof(Pt->PlayerBaseData.HorseExBag) );
		Reader.SetData(0, "IncursivePoint"		, &Pt->PlayerBaseData.IncursivePoint			, ENUM_Type_IntPoint		);
		Reader.SetData(0, "DuelistReward"			, &Pt->PlayerBaseData.DuelistReward				, ENUM_Type_IntPoint		);
		Reader.SetData(0, "MirrorCoin"			, &Pt->PlayerBaseData.MirrorCoin				, ENUM_Type_IntPoint		);

		Reader.SetData(0, "TitleSys_Famous"		, &Pt->PlayerBaseData.TitleSys.Famous			, ENUM_Type_FloatPoint		);
		Reader.SetData(0, "TitleSys_Lv"			, &Pt->PlayerBaseData.TitleSys.Lv				, ENUM_Type_IntPoint		);
		Reader.SetData(0, "TitleSys_Exp"			, &Pt->PlayerBaseData.TitleSys.Exp				, ENUM_Type_IntPoint		);
		for( int i = 0 ; i < _MAX_TITLESYS_TITLE_COUNT_ ; i++ )
		{
			sprintf_s( Buf , sizeof(Buf) , "TitleSys_TitleID%d" , i+1  );
			Reader.SetData(0, Buf					,  &Pt->PlayerBaseData.TitleSys.TitleID[i]		, ENUM_Type_IntPoint		);
		}
		Reader.SetData(0, "SlotMachine_History"	, &Pt->PlayerBaseData.SlotMachine.History		, ENUM_Type_BinaryData		, sizeof(Pt->PlayerBaseData.SlotMachine.History) );
		Reader.SetData(0, "SlotMachine_RandSeed"	, &Pt->PlayerBaseData.SlotMachine.RandSeed		, ENUM_Type_BinaryData		, sizeof(Pt->PlayerBaseData.SlotMachine.RandSeed) );
		Reader.SetData(0, "SlotMachine_FreeCount"	, &Pt->PlayerBaseData.SlotMachine.FreeCount		, ENUM_Type_IntPoint		);
		Reader.SetData(0, "PetCard"				, &Pt->PlayerBaseData.PetCard					, ENUM_Type_BinaryData		, sizeof(Pt->PlayerBaseData.PetCard) );
		Reader.SetData(0, "DreamlandCoin"			, &Pt->PlayerBaseData.DreamlandCoin				, ENUM_Type_IntPoint		);
		Reader.SetData(0, "PhiriusShellCoin"		, &Pt->PlayerBaseData.PhiriusShellCoin			, ENUM_Type_IntPoint		);
		Reader.SetData(0, "EnergyJusticeCoin"		, &Pt->PlayerBaseData.EnergyJusticeCoin			, ENUM_Type_IntPoint		);
		Reader.SetData(0, "ProofLegendCoin"		, &Pt->PlayerBaseData.ProofLegendCoin			, ENUM_Type_IntPoint		);
		Reader.SetData(0, "RelicsCoin"			, &Pt->PlayerBaseData.RelicsCoin				, ENUM_Type_IntPoint		);
		Reader.SetData(0, "TrialBadgeCoin"		, &Pt->PlayerBaseData.TrialBadgeCoin			, ENUM_Type_IntPoint		);

		Reader.SetData(0, "WeekInstance"			, &Pt->PlayerBaseData.WeekInstance				, ENUM_Type_BinaryData		, sizeof(Pt->PlayerBaseData.WeekInstance) );
		Reader.SetData(0, "WeekInstDay"			, &Pt->PlayerBaseData.WeekInstDay				, ENUM_Type_IntPoint		);
		Reader.SetData(0, "WeekResetCount"		, &Pt->PlayerBaseData.WeekResetCount				, ENUM_Type_IntPoint		);
		Reader.SetData(0, "Phantom"				, &Pt->PlayerBaseData.Phantom					, ENUM_Type_BinaryData		, sizeof(Pt->PlayerBaseData.Phantom) );

        IsReady = true;
    }      
    return &Reader;
}

//----------------------------------------------------------------------------------------------------------------
/*
ReaderClass<RoleDataEx>* RoleDataEx::ReaderBaseEx( )
{
	static  ReaderClass<RoleDataEx> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		char    Buf[256];
		RoleData* Pt = NULL;
		Reader.SetData(0, "DBID"					, &Pt->Base.DBID						, ENUM_Type_IntPoint    , 0								, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag);

		Reader.SetData(0, "RealTimeFlag"			, &Pt->PlayerBaseData.RealTimeFlag			, ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.RealTimeFlag)  );        
		Reader.SetData(0, "PlayTimeFlag"			, &Pt->PlayerBaseData.PlayTimeFlag			, ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.PlayTimeFlag)  );        
		Reader.SetData(0, "LuaValueFlag"			, &Pt->PlayerBaseData.LuaValueFlag			, ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.LuaValueFlag)  );        
		Reader.SetData(0, "MedalCount"			, &Pt->PlayerBaseData.MedalCount       		, ENUM_Type_IntPoint		);
		Reader.SetData(0, "Money_Bonus"			, &Pt->PlayerBaseData.Money_Bonus       		, ENUM_Type_IntPoint		);
		Reader.SetData(0, "LocationSave"			, &Pt->PlayerBaseData.LocationSave			, ENUM_Type_BinaryData  , sizeof(Pt->PlayerBaseData.LocationSave)  );        
		IsReady = true;
	}      
	return &Reader;
}
*/

//----------------------------------------------------------------------------------------------------------------
ReaderClass<NPCData>* RoleDataEx::ReaderNPC( )
{
    static  ReaderClass<NPCData> Reader;
    static  bool    IsReady = false;
    if( IsReady == false )
    {   
        NPCData* Pt = NULL;

        Reader.SetData(0, "DBID"          , &Pt->DBID                 	, ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );
        Reader.SetData(0, "CreateTime"    ,  Pt->CreateTime.Begin()   	, ENUM_Type_CharString     , sizeof(Pt->CreateTime) , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );
        Reader.SetData(0, "OrgObjID"      ,  &Pt->OrgObjID            	, ENUM_Type_IntPoint       );
        
        Reader.SetData(0, "Mode"          , &Pt->Mode                 	, ENUM_Type_IntPoint       );
        Reader.SetData(0, "RoleName"      ,  Pt->RoleName.Begin()     	, ENUM_Type_WCharString     , sizeof(Pt->RoleName) );
        Reader.SetData(0, "ZoneID"        , &Pt->ZoneID               	, ENUM_Type_IntPoint       );
		Reader.SetData(0, "RoomID"        , &Pt->RoomID               	, ENUM_Type_IntPoint       );
        Reader.SetData(0, "X"             , &Pt->Pos.X                	, ENUM_Type_FloatPoint     );
        Reader.SetData(0, "Y"             , &Pt->Pos.Y                	, ENUM_Type_FloatPoint     );
        Reader.SetData(0, "Z"             , &Pt->Pos.Z                	, ENUM_Type_FloatPoint     );
        Reader.SetData(0, "Dir"           , &Pt->Pos.Dir              	, ENUM_Type_FloatPoint     );

		Reader.SetData(0, "vX"            , &Pt->vX						, ENUM_Type_FloatPoint     );
		Reader.SetData(0, "vY"            , &Pt->vY						, ENUM_Type_FloatPoint     );
		Reader.SetData(0, "vZ"            , &Pt->vZ			            , ENUM_Type_FloatPoint     );


        Reader.SetData(0, "LiveTime"      , &Pt->LiveTime             	, ENUM_Type_IntPoint       );        
        Reader.SetData(0, "OwnerDBID"     , &Pt->OwnerDBID            	, ENUM_Type_IntPoint       );        
        Reader.SetData(0, "PID"           , &Pt->PID                  	, ENUM_Type_IntPoint       );        
        Reader.SetData(0, "PlotClassName" ,  Pt->PlotClassName.Begin()	, ENUM_Type_CharString     , sizeof(Pt->PlotClassName) );        
        Reader.SetData(0, "AutoPlot"      ,  Pt->AutoPlot.Begin()     	, ENUM_Type_CharString     , sizeof(Pt->AutoPlot) );        
        Reader.SetData(0, "QuestID"       , &Pt->QuestID			    	, ENUM_Type_IntPoint	   );        
        Reader.SetData(0, "CreateAccount" ,  Pt->CreateAccount.Begin()    , ENUM_Type_CharString	   , sizeof(Pt->CreateAccount) );        
        IsReady = true;
    }      
    return &Reader;
}
//----------------------------------------------------------------------------------------------------------------
ReaderClass<DB_ItemFieldStruct>*  RoleDataEx::ReaderRD_Field( )
{
	static  ReaderClass<DB_ItemFieldStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		DB_ItemFieldStruct* Pt = NULL;
        Reader.SetData(0, "DBID"          , &Pt->DBID                 , ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );
        Reader.SetData(0, "FieldID"       , &Pt->FieldID              , ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );
        Reader.SetData(0, "FieldType"     , &Pt->FieldType            , ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );

        Reader.SetData(0, "OrgObjID"      , &Pt->Info.OrgObjID        , ENUM_Type_IntPoint        );
        Reader.SetData(0, "Serial"        , &Pt->Info.Serial          , ENUM_Type_IntPoint        );
        Reader.SetData(0, "CreateTime"    , &Pt->Info.CreateTime      , ENUM_Type_IntPoint        );
		Reader.SetData(0, "ExValue"		, &Pt->Info.ExValue			, ENUM_Type_IntPoint        );
        Reader.SetData(0, "Count"         , &Pt->Info.Count           , ENUM_Type_IntPoint        );

//		Reader.SetData(0, "MainColor"		, &Pt->Info.MainColor		, ENUM_Type_IntPoint        );
//		Reader.SetData(0, "OffColor"      , &Pt->Info.OffColor        , ENUM_Type_IntPoint        );
        Reader.SetData(0, "Durable"       , &Pt->Info.Durable         , ENUM_Type_IntPoint      );
        Reader.SetData(0, "ImageObjectID" , &Pt->Info.ImageObjectID   , ENUM_Type_IntPoint      );
        Reader.SetData(0, "Ability"		, Pt->Info.Ability			, ENUM_Type_BinaryData  , sizeof(Pt->Info.Ability)  );        
		Reader.SetData(0, "Mode"			, &Pt->Info.Mode			, ENUM_Type_IntPoint      );

		IsReady = true;
	}

	return &Reader;
}
//----------------------------------------------------------------------------------------------------------------
ReaderClass<DB_AbilityStruct>*  RoleDataEx::ReaderRD_Ability( )
{
	static  ReaderClass<DB_AbilityStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		char    Buf[256];
		DB_AbilityStruct* Pt = NULL;
        Reader.SetData(0, "DBID"          , &Pt->DBID                 , ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );
        Reader.SetData(0, "Job"           , &Pt->Job                  , ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );

        Reader.SetData(0, "EXP"           , &Pt->Info.EXP             , ENUM_Type_IntPoint        );
        Reader.SetData(0, "TotalTpEXP"    , &Pt->Info.TotalTpExp      , ENUM_Type_IntPoint        );
		Reader.SetData(0, "TpEXP"         , &Pt->Info.TpExp           , ENUM_Type_IntPoint        );
		Reader.SetData(0, "DebtTP"        , &Pt->Info.DebtTP          , ENUM_Type_IntPoint        );
		Reader.SetData(0, "DebtExp"       , &Pt->Info.DebtExp         , ENUM_Type_IntPoint        );
		Reader.SetData(0, "AddTP"         , &Pt->Info.AddTp	        , ENUM_Type_IntPoint        );
		Reader.SetData(0, "AddExp"        , &Pt->Info.AddExp         , ENUM_Type_IntPoint        );

        Reader.SetData(0, "Level"         , &Pt->Info.Level           , ENUM_Type_IntPoint        );
        Reader.SetData(0, "Point"         , &Pt->Info.Point           , ENUM_Type_IntPoint        );
        Reader.SetData(0, "PlayTime"		, &Pt->Info.PlayTime		, ENUM_Type_IntPoint        );
        Reader.SetData(0, "STR"           , &Pt->Info.STR             , ENUM_Type_IntPoint        );
        Reader.SetData(0, "STA"           , &Pt->Info.STA             , ENUM_Type_IntPoint        );
        Reader.SetData(0, "MND"           , &Pt->Info.MND             , ENUM_Type_IntPoint        );
        Reader.SetData(0, "INT"           , &Pt->Info.INT             , ENUM_Type_IntPoint        );
        Reader.SetData(0, "AGI"           , &Pt->Info.AGI             , ENUM_Type_IntPoint        );

        for( int i = 0 ; i < _MAX_NormalSkill_COUNT_ ; i++ )
        {
            sprintf_s( Buf , sizeof(Buf) , "NormalSkill%d" , i+1 );
            Reader.SetData(0, Buf           , &Pt->Info.NormalSkill[i]             , ENUM_Type_FloatPoint        );
        }
		for( int i = 0 ; i < _MAX_SPSkill_COUNT_ ; i++ )
		{
			sprintf_s( Buf , sizeof(Buf) , "SPSkill%d" , i+1 );
			Reader.SetData(0, Buf           , &Pt->Info.SPSkill[i]             , ENUM_Type_FloatPoint        );
		}

		IsReady = true;
	}      
	return &Reader;
}
//----------------------------------------------------------------------------------------------------------------

ReaderClass<DB_BaseFriendStruct>*  RoleDataEx::ReaderRD_BaseFriendStruct( )
{
	static  ReaderClass<DB_BaseFriendStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   		
		DB_BaseFriendStruct* Pt = NULL;
		Reader.SetData(0, "OwnerDBID"     , &Pt->OwnerDBID            	, ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );
		Reader.SetData(0, "FieldID"       , &Pt->FieldID              	, ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );
		Reader.SetData(0, "Type"			, &Pt->Type	                	, ENUM_Type_IntPoint       );

		Reader.SetData(0, "DBID"			, &Pt->Info.DBID                , ENUM_Type_IntPoint       );
		Reader.SetData(0, "Name"			,  Pt->Info.Name				, ENUM_Type_WCharString     , sizeof(Pt->Info.Name	));
		Reader.SetData(0, "Note"			,  Pt->Info.Note				, ENUM_Type_BinaryData		, sizeof(Pt->Info.Note	));
		Reader.SetData(0, "Relation"		, &Pt->Info.Relation            , ENUM_Type_IntPoint       );
		Reader.SetData(0, "Lv"			, &Pt->Info.Lv					, ENUM_Type_FloatPoint       );

		IsReady = true;
	}      
	return &Reader;
}

//----------------------------------------------------------------------------------------------------------------
ReaderClass<DB_MailItemStruct>*  RoleDataEx::ReaderRD_MailItem( )
{
	static  ReaderClass<DB_MailItemStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   

		DB_MailItemStruct* Pt = NULL;
		Reader.SetData(0, "MailBaseGUID"	, &Pt->MailBaseGUID				, ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );
		Reader.SetData(0, "OrgObjID"      , &Pt->Item.OrgObjID			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Serial"		, &Pt->Item.Serial				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "CreateTime"    , &Pt->Item.CreateTime			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "[Count]"		, &Pt->Item.Count				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Durable"		, &Pt->Item.Durable				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Ability"		,  Pt->Item.Ability				, ENUM_Type_BinaryData  , sizeof(Pt->Item.Ability)  );        
		Reader.SetData(0, "ExValue"		, &Pt->Item.ExValue				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Mode"			, &Pt->Item.Mode				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "ImageObjectID"	, &Pt->Item.ImageObjectID		, ENUM_Type_IntPoint		);

		IsReady = true;
	}      
	return &Reader;
}

ReaderClass<DB_MailStruct>*  RoleDataEx::ReaderRD_MailBase( )
{
	static  ReaderClass<DB_MailStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{    
		DB_MailStruct* Pt = NULL;
		Reader.SetData(0, "GUID"          , &Pt->GUID								, ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );
//		Reader.SetData(0, "ItemGUID"      , &Pt->ItemGUID							, ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );

		Reader.SetData(0, "BKMoney"		, &Pt->MailBase.Money					, ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Select_Flag );
		Reader.SetData(0, "BKMoney_Account", &Pt->MailBase.Money_Account			, ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Select_Flag );
		Reader.SetData(0, "BKMoney_Bonus"	, &Pt->MailBase.Money_Bonus				, ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Select_Flag );

		Reader.SetData(0, "Money"			, &Pt->MailBase.Money					, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Money_Account"	, &Pt->MailBase.Money_Account			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Money_Bonus"	, &Pt->MailBase.Money_Bonus				, ENUM_Type_IntPoint       );

		Reader.SetData(0, "IsSendMoney"	, &Pt->MailBase.IsSendMoney				, ENUM_Type_BitPoint       );
		Reader.SetData(0, "IsSystem"		, &Pt->MailBase.IsSystem				, ENUM_Type_BitPoint       );
		Reader.SetData(0, "IsRead"		, &Pt->MailBase.IsRead					, ENUM_Type_BitPoint       );
		Reader.SetData(0, "IsGift"		, &Pt->MailBase.IsGift					, ENUM_Type_BitPoint       );

		Reader.SetData(0, "AcDBID"		, &Pt->MailBase.AcDBID					, ENUM_Type_IntPoint       );
		Reader.SetData(0, "OwnerDBID"		, &Pt->OwnerDBID						, ENUM_Type_IntPoint       );
		Reader.SetData(0, "OrgSendDBID"	, &Pt->OrgSendDBID						, ENUM_Type_IntPoint       );
		Reader.SetData(0, "OrgSendName"	,  Pt->MailBase.OrgSendName.Begin()		, ENUM_Type_WCharString     , sizeof(Pt->MailBase.OrgSendName)	);
		Reader.SetData(0, "OrgTargetName"	,  Pt->MailBase.OrgTargetName.Begin()	, ENUM_Type_WCharString     , sizeof(Pt->MailBase.OrgTargetName) );
		Reader.SetData(0, "Title"			,  Pt->MailBase.Title.Begin()			, ENUM_Type_WCharString     , sizeof(Pt->MailBase.Title)			);
		Reader.SetData(0, "LiveTime"		, &Pt->MailBase.LiveTime				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Content"		,  Pt->Content.Begin()					, ENUM_Type_WCharString	   , sizeof(Pt->Content)  );
		Reader.SetData(0, "PaperType"		, &Pt->MailBase.PaperType				, ENUM_Type_IntPoint       );
		IsReady = true;
	}      
	return &Reader;
}
//----------------------------------------------------------------------------------------------------------------
ReaderClass<DB_BillBoardRoleInfoStruct>*	RoleDataEx::ReaderRD_BillBoard( )
{
	static  ReaderClass<DB_BillBoardRoleInfoStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{    
		DB_BillBoardRoleInfoStruct* Pt = NULL;

		Reader.SetData(0, "RoleName"			,  Pt->RoleName.Begin()					, ENUM_Type_WCharString	   , sizeof(Pt->RoleName)  );		
		Reader.SetData(0, "Rank_PKScore"		, &Pt->Rank_PKScore						, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Rank_TitleCount"	, &Pt->Rank_Title						, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Rank_Money"		, &Pt->Rank_Money						, ENUM_Type_IntPoint       );
		
		for( int i = 0 ; i < 8 ; i++ )
		{
			char Buf[256];
			sprintf_s( Buf , sizeof(Buf) , "Rank_Job%d" ,  i+ 1 );
			Reader.SetData(0, Buf				, &Pt->Rank_Job[i]						, ENUM_Type_IntPoint       );
		}

		for( int i = 0 ; i < 8 ; i++ )
		{
			char Buf[256];
			sprintf_s( Buf , sizeof(Buf) , "Job%dLv" ,  i+ 1 );
			Reader.SetData(0, Buf				, &Pt->JobLv[i]						, ENUM_Type_IntPoint       );
		}

		Reader.SetData(0, "IsAnonymous"		, &Pt->IsAnonymous						, ENUM_Type_BitPoint       );
		Reader.SetData(0, "DBID"				, &Pt->DBID								, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Voc"				, &Pt->Voc								, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Voc_Sub"			, &Pt->Voc_Sub							, ENUM_Type_IntPoint       );
//		Reader.SetData(0, "Lv"				, &Pt->Lv								, ENUM_Type_IntPoint       );
//		Reader.SetData(0, "Lv_Sub"			, &Pt->Lv_Sub							, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Money"				, &Pt->Money							, ENUM_Type_IntPoint       );
		Reader.SetData(0, "TitleCount"		, &Pt->TitleCount						, ENUM_Type_IntPoint       );
		Reader.SetData(0, "PKScore"			, &Pt->PKScore							, ENUM_Type_IntPoint       );

		IsReady = true;
	}      
	return &Reader;
}

//----------------------------------------------------------------------------------------------------------------
/*
ReaderClass<DB_ACItemStruct>*  RoleDataEx::ReaderRD_ACItem( )
{
	static  ReaderClass<DB_ACItemStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		DB_ACItemStruct* Pt = NULL;
		Reader.SetData(0, "GUID"          , &Pt->ACDIBD			, ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );
		Reader.SetData(0, "OrgObjID"      , &Pt->Item.OrgObjID	, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Serial"		, &Pt->Item.Serial		, ENUM_Type_IntPoint       );
		Reader.SetData(0, "CreateTime"    , &Pt->Item.CreateTime	, ENUM_Type_IntPoint       );
		Reader.SetData(0, "[Count]"		, &Pt->Item.Count		, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Coldown"		, &Pt->Item.Coldown		, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Durable"		, &Pt->Item.Durable		, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Inherent"      , &Pt->Item.Inherent	, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Rune1"			, &Pt->Item.Rune[0]		, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Rune2"			, &Pt->Item.Rune[1]		, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Rune3"		    , &Pt->Item.Rune[2]		, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Rune4"			, &Pt->Item.Rune[3]		, ENUM_Type_IntPoint       );
		Reader.SetData(0, "RuneVolume"    , &Pt->Item.RuneVolume	, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Mode"			, &Pt->Item.Mode		, ENUM_Type_IntPoint       );
		IsReady = true;
	}      
	return &Reader;
}
*/
ReaderClass<DB_ACItemStruct>*  RoleDataEx::ReaderRD_ACBase( )
{
	static  ReaderClass<DB_ACItemStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		DB_ACItemStruct* Pt = NULL;

		Reader.SetData(0, "GUID"          , &Pt->GUID					, ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );
		Reader.SetData(0, "LiveTime"      , &Pt->LiveTime				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "NowPrices"     , &Pt->NowPrices			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "BuyOutPrices"  , &Pt->BuyOutPrices			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "PricesType"	, &Pt->PricesType			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "SellerDBID"	, &Pt->SellerDBID			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "BuyerDBID"		, &Pt->BuyerDBID			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "SellerName"	,  Pt->SellerName.Begin()	, ENUM_Type_WCharString     , sizeof(Pt->SellerName)	);
		Reader.SetData(0, "BuyerName"		,  Pt->BuyerName.Begin()	, ENUM_Type_WCharString     , sizeof(Pt->BuyerName)	);

		Reader.SetData(0, "ItemName"		,  Pt->ItemName.Begin()		, ENUM_Type_WCharString     , sizeof(Pt->ItemName)	);
		Reader.SetData(0, "Type1"     	, &Pt->ItemType[0]			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "Type2"     	, &Pt->ItemType[1]			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "Type3"     	, &Pt->ItemType[2]			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "ItemLevel"		, &Pt->ItemLevel			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "EqWearType"	, &Pt->EQType._Skill		, ENUM_Type_IntPoint		);
		Reader.SetData(0, "Rare"			, &Pt->Rare					, ENUM_Type_IntPoint		);
		Reader.SetData(0, "SexType"		, &Pt->SexType._Sex			, ENUM_Type_IntPoint		);
		Reader.SetData(0, "RaceType"		, &Pt->RaceType._Race		, ENUM_Type_IntPoint		);
		Reader.SetData(0, "RuneVolume"	, &Pt->RuneVolume			, ENUM_Type_IntPoint		);        

		Reader.SetData(0, "OrgObjID"      , &Pt->Item.OrgObjID		, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "Serial"		, &Pt->Item.Serial			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "CreateTime"    , &Pt->Item.CreateTime		, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "[Count]"		, &Pt->Item.Count			, ENUM_Type_IntPoint       	);
//		Reader.SetData(0, "Coldown"		, &Pt->Item.Coldown			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "Durable"		, &Pt->Item.Durable			, ENUM_Type_IntPoint       	);

		Reader.SetData(0, "Ability"		,  Pt->Item.Ability			, ENUM_Type_BinaryData  , sizeof(Pt->Item.Ability)  );        
		/*
		Reader.SetData(0, "Inherent"      , &Pt->Item.Inherent		, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "Rune1"			, &Pt->Item.Rune[0]			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "Rune2"			, &Pt->Item.Rune[1]			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "Rune3"		    , &Pt->Item.Rune[2]			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "Rune4"			, &Pt->Item.Rune[3]			, ENUM_Type_IntPoint       	);
		*/
		Reader.SetData(0, "ExValue"		, &Pt->Item.ExValue			, ENUM_Type_IntPoint       	);
//		Reader.SetData(0, "MainColor"		, &Pt->Item.MainColor		, ENUM_Type_IntPoint        );
//		Reader.SetData(0, "OffColor"      , &Pt->Item.OffColor		, ENUM_Type_IntPoint        );
		Reader.SetData(0, "ImageObjectID"	, &Pt->Item.ImageObjectID	, ENUM_Type_IntPoint		);        

		Reader.SetData(0, "Mode"			, &Pt->Item.Mode			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "ImportAcItemGUID"	, &Pt->ImportAcItemGUID	, ENUM_Type_IntPoint       	);

		IsReady = true;
	}      
	return &Reader;
}

ReaderClass<DB_ACItemHistoryStruct>*	RoleDataEx::ReaderRD_ACHistory( )
{
	static  ReaderClass<DB_ACItemHistoryStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		DB_ACItemHistoryStruct* Pt = NULL;
		Reader.SetData(0, "GUID"          , &Pt->GUID					, ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );		
		Reader.SetData(0, "SellerName"	,  Pt->SellerName.Begin()	, ENUM_Type_WCharString     , sizeof(Pt->SellerName)	);
		Reader.SetData(0, "BuyerName"		,  Pt->BuyerName.Begin()	, ENUM_Type_WCharString     , sizeof(Pt->BuyerName)	);
		Reader.SetData(0, "DateTime"		, &Pt->Date					, ENUM_Type_IntPoint		);
		Reader.SetData(0, "Prices"		, &Pt->Prices				, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "PricesType"	, &Pt->PricesType			, ENUM_Type_IntPoint       	);
		
		Reader.SetData(0, "OrgObjID"      , &Pt->Item.OrgObjID		, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "Serial"		, &Pt->Item.Serial			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "CreateTime"    , &Pt->Item.CreateTime		, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "[Count]"		, &Pt->Item.Count			, ENUM_Type_IntPoint       	);
//		Reader.SetData(0, "Coldown"		, &Pt->Item.Coldown			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "Durable"		, &Pt->Item.Durable			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "Ability"		,  Pt->Item.Ability			, ENUM_Type_BinaryData		, sizeof(Pt->Item.Ability)  );        
		/*
		Reader.SetData(0, "Inherent"      , &Pt->Item.Inherent		, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "Rune1"			, &Pt->Item.Rune[0]			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "Rune2"			, &Pt->Item.Rune[1]			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "Rune3"		    , &Pt->Item.Rune[2]			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "Rune4"			, &Pt->Item.Rune[3]			, ENUM_Type_IntPoint       	);
		*/
		Reader.SetData(0, "ExValue"		, &Pt->Item.ExValue			, ENUM_Type_IntPoint       	);
//		Reader.SetData(0, "MainColor"		, &Pt->Item.MainColor		, ENUM_Type_IntPoint        );
//		Reader.SetData(0, "OffColor"      , &Pt->Item.OffColor		, ENUM_Type_IntPoint        );
		Reader.SetData(0, "ImageObjectID"	, &Pt->Item.ImageObjectID	, ENUM_Type_IntPoint		);
		Reader.SetData(0, "Mode"			, &Pt->Item.Mode			, ENUM_Type_IntPoint       	);

		IsReady = true;
	}
	return &Reader;
}

ReaderClass<DB_NPC_MoveBaseStruct>*	RoleDataEx::ReaderRD_NPC_MoveBase( )
{
	static  ReaderClass<DB_NPC_MoveBaseStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		DB_NPC_MoveBaseStruct* Pt = NULL;
		Reader.SetData(0, "NPCDBID"   		, &Pt->NPCDBID				, ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );		
		Reader.SetData(0, "IndexID"   		, &Pt->IndexID				, ENUM_Type_IntPoint       , 0 , ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );		

		Reader.SetData(0, "ZoneID"			, &Pt->ZoneID				, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "X"					, &Pt->Base.X				, ENUM_Type_FloatPoint      );
		Reader.SetData(0, "Y"					, &Pt->Base.Y				, ENUM_Type_FloatPoint      );
		Reader.SetData(0, "Z"					, &Pt->Base.Z				, ENUM_Type_FloatPoint      );
		Reader.SetData(0, "Dir"				, &Pt->Base.Dir				, ENUM_Type_FloatPoint      );
		Reader.SetData(0, "Range"				, &Pt->Base.Range			, ENUM_Type_FloatPoint      );
		Reader.SetData(0, "MoveType"			, &Pt->Base.MoveType		, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "WaitTime_Base"		, &Pt->Base.WaitTime_Base	, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "WaitTime_Rand"		, &Pt->Base.WaitTime_Rand	, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "ActionType"		, &Pt->Base.ActionType		, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "DirMode"			, &Pt->Base.DirMode			, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "LuaFunc"			, Pt->Base.sLuaFunc.Begin()	, ENUM_Type_CharString     , sizeof(Pt->Base.sLuaFunc )	 );		

		IsReady = true;
	}
	return &Reader;
}

ReaderClass<GuildMemberStruct>*	RoleDataEx::ReaderRD_GuildMember( )
{
	static  ReaderClass<GuildMemberStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		GuildMemberStruct* Pt = NULL;
		Reader.SetData(0, "GuildID"			, &Pt->GuildID				, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "GuildRank"			, &Pt->Rank					, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "GuildGroupID"		, &Pt->GroupID				, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "GuildRankTime"		, &Pt->RankTime				, ENUM_Type_IntPoint       	);
		Reader.SetData(0, "DBID"   			, &Pt->MemberDBID			, ENUM_Type_IntPoint       , 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );		
		Reader.SetData(0, "RoleName"			,  Pt->MemberName.Begin()	, ENUM_Type_WCharString     , sizeof(Pt->MemberName)	, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );		

		Reader.SetData(0, "GuildSelfNote"		,  Pt->SelfNote.Begin()		, ENUM_Type_WCharString     , sizeof(Pt->SelfNote) );		
		Reader.SetData(0, "GuildLeaderNote"	,  Pt->LeaderNote.Begin()	, ENUM_Type_WCharString     , sizeof(Pt->LeaderNote) );		

		Reader.SetData(0, "Voc"           	, &Pt->Voc                  , ENUM_Type_IntPoint       , 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );        
		Reader.SetData(0, "Voc_Sub"       	, &Pt->Voc_Sub              , ENUM_Type_IntPoint       , 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );        
		Reader.SetData(0, "Sex"           	, &Pt->Sex                  , ENUM_Type_IntPoint       , 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );        
		Reader.SetData(0, "LV"           		, &Pt->LV                   , ENUM_Type_IntPoint       , 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );        
		Reader.SetData(0, "LV_Sub"       		, &Pt->LV_Sub               , ENUM_Type_IntPoint       , 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );        
		Reader.SetData(0, "LogoutTime"       	, &Pt->LogoutTime           , ENUM_Type_IntPoint       , 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag | Def_Disabled_Insert_Flag );        

		IsReady = true;
	}
	return &Reader;
}
ReaderClass<GuildBaseStruct>*	RoleDataEx::ReaderRD_GuildBase( )
{
	static  ReaderClass<GuildBaseStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		GuildBaseStruct* Pt = NULL;
		Reader.SetData(0, "GUID"				, &Pt->GuildID				, ENUM_Type_IntPoint       	, 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag  );		
		Reader.SetData(0, "LeaderDBID"		, &Pt->LeaderDBID			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Medal"				, &Pt->Medal				, ENUM_Type_IntPoint       );

		Reader.SetData(0, "WarGuildID"		, &Pt->WarGuildID			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "WarMyScore"		, &Pt->WarMyScore			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "WarEnemyScore"		, &Pt->WarEnemyScore		, ENUM_Type_IntPoint       );
		Reader.SetData(0, "WarTime"			, &Pt->WarTime				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "WarEnemyKillCount"	, &Pt->WarEnemyKillCount	, ENUM_Type_IntPoint       );
		Reader.SetData(0, "WarMyKillCount"	, &Pt->WarMyKillCount		, ENUM_Type_IntPoint       );
		

		Reader.SetData(0, "WarWinCount"		, &Pt->WarWinCount			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "WarEvenCount"		, &Pt->WarEvenCount			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "WarLostCount"		, &Pt->WarLostCount			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "WarDeclareTime"	, &Pt->WarDeclareTime		, ENUM_Type_IntPoint       );


		Reader.SetData(0, "GuildType"			, &Pt->GuildType			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "MaxMemberCount"	, &Pt->MaxMemberCount		, ENUM_Type_IntPoint       );
//		Reader.SetData(0, "Score"				, &Pt->Score				, ENUM_Type_IntPoint       );

		for( int i = 0 ; i < 7 ; i++ )
		{
			char Buf[256];
			sprintf_s( Buf , sizeof(Buf) ,"GuildResource%d" , i+1 );
			Reader.SetData(0, Buf				, &Pt->Resource._Value[i]	, ENUM_Type_IntPoint       );
		}

		Reader.SetData(0, "GuildName"			,  Pt->GuildName.Begin()	, ENUM_Type_WCharString    , sizeof(Pt->GuildName)		);		
		Reader.SetData(0, "LeaderName"		,  Pt->LeaderName.Begin()	, ENUM_Type_WCharString    , sizeof(Pt->LeaderName)		);		
		Reader.SetData(0, "Note"				,  Pt->Note.Begin()			, ENUM_Type_WCharString    , sizeof(Pt->Note)			);		
		Reader.SetData(0, "Introduce"			,  Pt->Introduce.Begin()	, ENUM_Type_WCharString    , sizeof(Pt->Introduce)		);		
		Reader.SetData(0, "Flag"				, &Pt->Flag					, ENUM_Type_BinaryData	   , sizeof(Pt->Flag)  );        
		Reader.SetData(0, "GuildFlag"			, &Pt->GuildFlag			, ENUM_Type_BinaryData	   , sizeof(Pt->GuildFlag)  );        
		Reader.SetData(0, "RankCount"			, &Pt->RankCount			, ENUM_Type_IntPoint       );
		for( int i = 0 ; i < EM_GuildRank_Count ; i++ )
		{
			char Buf[256];
			sprintf_s( Buf , sizeof(Buf) ,"RankAbility%d" , i+1 );
			Reader.SetData(0, Buf				, &Pt->Rank[i].Setting.Type		, ENUM_Type_IntPoint       );
			sprintf_s( Buf , sizeof(Buf) ,"RankName%d" , i+1 );
			Reader.SetData(0, Buf				,  Pt->Rank[i].RankName.Begin() , ENUM_Type_WCharString     , sizeof( Pt->Rank[i].RankName )	);		
		}

		for( int i = 0 ; i < _MAX_GUILD_GROUP_COUNT_ ; i++ )
		{
			char Buf[256];
			sprintf_s( Buf , sizeof(Buf) ,"GroupName%d" , i+1 );
			Reader.SetData(0, Buf		, Pt->Group[i].Begin()			, ENUM_Type_WCharString     , sizeof(  Pt->Group[i] )	);		 
		}

		Reader.SetData(0, "Lv"				, &Pt->Level				, ENUM_Type_IntPoint       );


		Reader.SetData(0, "HouesWar_WinCount"		, &Pt->HousesWar.WinCount		, ENUM_Type_IntPoint       );
		Reader.SetData(0, "HouesWar_EvenCount"	, &Pt->HousesWar.EvenCount		, ENUM_Type_IntPoint       );
		Reader.SetData(0, "HouesWar_LostCount"	, &Pt->HousesWar.LostCount		, ENUM_Type_IntPoint       );
		Reader.SetData(0, "HouesWar_Score"		, &Pt->HousesWar.Score			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "HouesWar_NextWarTime"	, &Pt->HousesWar.NextWarTime	, ENUM_Type_IntPoint       );

		IsReady = true;
	}
	return &Reader;

}

ReaderClass<GuildBoardStruct>*	RoleDataEx::ReaderRD_GuildBoard( )
{
	static  ReaderClass<GuildBoardStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		GuildBoardStruct* Pt = NULL;
		Reader.SetData(0, "GUID"				, &Pt->GUID					, ENUM_Type_IntPoint       	, 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag  );		
		Reader.SetData(0, "GuildID"			, &Pt->GuildID				, ENUM_Type_IntPoint       );		

		Reader.SetData(0, "Type"				, &Pt->Type._Type			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "PostTime"			, &Pt->Time					, ENUM_Type_IntPoint       );

		Reader.SetData(0, "Title"				,  Pt->Title.Begin()		, ENUM_Type_WCharString      , sizeof(Pt->Title)		);		
		Reader.SetData(0, "Message"			,  Pt->Message.Begin()		, ENUM_Type_WCharString      , sizeof(Pt->Message)		);		
		Reader.SetData(0, "PostName"			,  Pt->PlayerName.Begin()	, ENUM_Type_WCharString		, sizeof(Pt->PlayerName)		);		

		IsReady = true;
	}
	return &Reader;
}


ReaderClass<HouseBaseDBStruct>*	RoleDataEx::ReaderRD_HouseBase( )
{
	static  ReaderClass<HouseBaseDBStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		HouseBaseDBStruct* Pt = NULL;

		Reader.SetData(0, "HouseDBID"				, &Pt->HouseDBID			, ENUM_Type_IntPoint       	, 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag  );		

		Reader.SetData(0, "MaxItemCount"			, &Pt->MaxItemCount			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "EnergyPoint"			, &Pt->EnergyPoint			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "HouseType"				, &Pt->HouseType			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "LastRantelTime"		, &Pt->LastRantelTime		, ENUM_Type_IntPoint       );

		Reader.SetData(0, "Password"				,  Pt->Password.Begin()		, ENUM_Type_CharString      , sizeof(Pt->Password)		);		
//		Reader.SetData(0, "Account_ID"			,  Pt->AccountID.Begin()	, ENUM_Type_CharString      , sizeof(Pt->AccountID)	, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag  );		
		Reader.SetData(0, "OwnerDBID"				, &Pt->OwnerDBID			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "ShopFunction"			, &Pt->ShopFunction			, ENUM_Type_BinaryData		, sizeof(Pt->ShopFunction)  );      
		Reader.SetData(0, "HouseName"				,  Pt->HouseName.Begin()	, ENUM_Type_WCharString     , sizeof(Pt->HouseName)		);		
		Reader.SetData(0, "ClientData"			,  Pt->ClientData			, ENUM_Type_BinaryData		, sizeof(Pt->ClientData)	);		
		Reader.SetData(0, "ItemLog"				,  Pt->ItemLog				, ENUM_Type_BinaryData		, sizeof(Pt->ItemLog)		);		
		Reader.SetData(0, "UnCheckItemLogCount"	, &Pt->UnCheckItemLogCount	, ENUM_Type_IntPoint		);
		Reader.SetData(0, "ItemLogPos"			, &Pt->ItemLogPos			, ENUM_Type_IntPoint		);
//		Reader.SetData(0, "Servant"				, &Pt->Servant				, ENUM_Type_BinaryData		, sizeof( Pt->Servant )		);
		Reader.SetData(0, "LastServantProcTime"	, &Pt->LastServantProcTime	, ENUM_Type_IntPoint       );

		for( int i = 0 ; i < MAX_SERVANT_COUNT ; i++ )
		{
			char Buf[512];
			sprintf( Buf , "Servant%d" , i+1 );
			Reader.SetData(0, Buf	, &Pt->Servant[i] , ENUM_Type_BinaryData , sizeof(HouseServantStruct)       );
		}

		IsReady = true;
	}
	return &Reader;
}

ReaderClass<HouseItemDBStruct>*	RoleDataEx::ReaderRD_HouseItem( )
{
	static  ReaderClass<HouseItemDBStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		HouseItemDBStruct* Pt = NULL;

		Reader.SetData(0, "ItemDBID"			, &Pt->ItemDBID					, ENUM_Type_IntPoint       , 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );		
		Reader.SetData(0, "HouseDBID"			, &Pt->HouseDBID				, ENUM_Type_IntPoint       , 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );		
		Reader.SetData(0, "ParentItemDBID"	, &Pt->ParentItemDBID			, ENUM_Type_IntPoint       );		
		Reader.SetData(0, "HousePos"			, &Pt->Pos						, ENUM_Type_IntPoint       );		

//		Reader.SetData(0, "ItemPos"	      	, &Pt->Item.Pos					, ENUM_Type_IntPoint       );
		Reader.SetData(0, "OrgObjID"      	, &Pt->Item.OrgObjID			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Serial"			, &Pt->Item.Serial				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "CreateTime"    	, &Pt->Item.CreateTime			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "[Count]"			, &Pt->Item.Count				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Durable"			, &Pt->Item.Durable				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Ability"			,  Pt->Item.Ability				, ENUM_Type_BinaryData		, sizeof(Pt->Item.Ability)  );        
		/*
		Reader.SetData(0, "Inherent"      	, &Pt->Item.Inherent			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Rune1"				, &Pt->Item.Rune[0]				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Rune2"				, &Pt->Item.Rune[1]				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Rune3"		    	, &Pt->Item.Rune[2]				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Rune4"				, &Pt->Item.Rune[3]				, ENUM_Type_IntPoint       );
		*/
		Reader.SetData(0, "ExValue"			, &Pt->Item.ExValue				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Mode"				, &Pt->Item.Mode				, ENUM_Type_IntPoint       );
//		Reader.SetData(0, "MainColor"			, &Pt->Item.MainColor			, ENUM_Type_IntPoint       );
//		Reader.SetData(0, "OffColor"      	, &Pt->Item.OffColor			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "ImageObjectID"		, &Pt->Item.ImageObjectID		, ENUM_Type_IntPoint		);        
		Reader.SetData(0, "Layout"			, &Pt->Layout					, ENUM_Type_BinaryData  , sizeof(Pt->Layout)  ); 
		IsReady = true;
	}
	return &Reader;
}

ReaderClass<AccountBagItemFieldDBStruct>*	RoleDataEx::ReaderRD_AccountBag( )
{
	static  ReaderClass<AccountBagItemFieldDBStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		AccountBagItemFieldDBStruct* Pt = NULL;

		Reader.SetData(0, "GUID"				, &Pt->ItemDBID					, ENUM_Type_IntPoint       , 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );		
		Reader.SetData(0, "OrgObjID"      	, &Pt->Item.OrgObjID			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Serial"			, &Pt->Item.Serial				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "CreateTime"    	, &Pt->Item.CreateTime			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "[Count]"			, &Pt->Item.Count				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Durable"			, &Pt->Item.Durable				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Ability"			,  Pt->Item.Ability				, ENUM_Type_BinaryData		, sizeof(Pt->Item.Ability)  );        
		Reader.SetData(0, "ExValue"			, &Pt->Item.ExValue				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Mode"				, &Pt->Item.Mode				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "ImageObjectID"		, &Pt->Item.ImageObjectID		, ENUM_Type_IntPoint		);        
		IsReady = true;
	}
	return &Reader;
}

ReaderClass<WebShopBagItemFieldDBStruct>*	RoleDataEx::ReaderRD_WebShopBag( )
{
	static  ReaderClass<WebShopBagItemFieldDBStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		WebShopBagItemFieldDBStruct* Pt = NULL;

		Reader.SetData(0, "GUID"				, &Pt->ItemDBID					, ENUM_Type_IntPoint       , 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag );		
		Reader.SetData(0, "OrgObjID"      	, &Pt->Item.OrgObjID			, ENUM_Type_IntPoint		);
		Reader.SetData(0, "Serial"			, &Pt->Item.Serial				, ENUM_Type_IntPoint		);
		Reader.SetData(0, "CreateTime"    	, &Pt->Item.CreateTime			, ENUM_Type_IntPoint		);
		Reader.SetData(0, "[Count]"			, &Pt->Item.Count				, ENUM_Type_IntPoint		);
		Reader.SetData(0, "Durable"			, &Pt->Item.Durable				, ENUM_Type_IntPoint		);
		Reader.SetData(0, "Ability"			,  Pt->Item.Ability				, ENUM_Type_BinaryData		, sizeof(Pt->Item.Ability)  );
		Reader.SetData(0, "ExValue"			, &Pt->Item.ExValue				, ENUM_Type_IntPoint		);
		Reader.SetData(0, "Mode"				, &Pt->Item.Mode				, ENUM_Type_IntPoint		);
		Reader.SetData(0, "ImageObjectID"		, &Pt->Item.ImageObjectID		, ENUM_Type_IntPoint		);
		Reader.SetData(0, "Message"			,  Pt->Message					, ENUM_Type_WCharString		, sizeof( Pt->Message )		);
		Reader.SetData(0, "PriceType"			, &Pt->PriceType				, ENUM_Type_IntPoint		);
		Reader.SetData(0, "Price"				, &Pt->Price					, ENUM_Type_IntPoint		);
		Reader.SetData(0, "AddBonusMoney"		, &Pt->AddBonusMoney			, ENUM_Type_IntPoint		);
		IsReady = true;
	}
	return &Reader;
}

ReaderClass<DB_DepartmentStoreBaseInfoStruct>*	RoleDataEx::ReaderRD_DepartmentStoreItem( )
{
	static  ReaderClass<DB_DepartmentStoreBaseInfoStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		char	Buf[256];
		DB_DepartmentStoreBaseInfoStruct* Pt = NULL;

		Reader.SetData(0, "GUID"					, &Pt->Item.GUID						, ENUM_Type_IntPoint       	, 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag  );		
		Reader.SetData(0, "NeedMemberID"			, &Pt->Item.NeedMemberID				, ENUM_Type_IntPoint    );		
		Reader.SetData(0, "SortNumber"			, &Pt->Item.SortNumber					, ENUM_Type_IntPoint    );		
		Reader.SetData(0, "SellType"				, &Pt->Item.SellType[0]					, ENUM_Type_IntPoint    );		
		Reader.SetData(0, "SellType1"				, &Pt->Item.SellType[1]					, ENUM_Type_IntPoint    );		
		Reader.SetData(0, "SellType2"				, &Pt->Item.SellType[2]					, ENUM_Type_IntPoint    );		
		Reader.SetData(0, "Item_DisplayerObjID"	, &Pt->Item.Item.OrgObjID				, ENUM_Type_IntPoint    );		
       Reader.SetData(0, "Item_Name"				,  Pt->Item.ItemName				    , ENUM_Type_WCharStringBase     , sizeof(Pt->Item.ItemName)	, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag);
		Reader.SetData(0, "Item_Note"				,  Pt->Item.Note					    , ENUM_Type_WCharStringBase     , sizeof(Pt->Item.Note)		, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag);


		for( int i = 0 ; i < 8 ; i++ )
		{
			sprintf_s( Buf , sizeof(Buf) , "Item_OrgObjID%d" , i+1 );
			Reader.SetData(0, Buf		, &Pt->ItemIDList[i]		, ENUM_Type_IntPoint    );
			sprintf_s( Buf , sizeof(Buf) , "Item_Count%d" , i+1 );
			Reader.SetData(0, Buf		, &Pt->ItemCountList[i]		, ENUM_Type_IntPoint    );
		}

		Reader.SetData(0, "Sell_BeginTime"		, &Pt->Item.Sell.BeginTime			, ENUM_Type_SmallDateTime	);
		Reader.SetData(0, "Sell_EndTime"			, &Pt->Item.Sell.EndTime				, ENUM_Type_SmallDateTime	);
		Reader.SetData(0, "Sell_Count"			, &Pt->Item.Sell.OrgCount			, ENUM_Type_IntPoint    );
		Reader.SetData(0, "Sell_MaxCount"			, &Pt->Item.Sell.MaxCount			, ENUM_Type_IntPoint    );
		Reader.SetData(0, "Sell_Cost"      		, &Pt->Item.Sell.Cost				, ENUM_Type_IntPoint    );
		Reader.SetData(0, "Sell_Cost_Bonus"  		, &Pt->Item.Sell.Cost_Bonus			, ENUM_Type_IntPoint    );
		Reader.SetData(0, "Sell_Cost_Free"  		, &Pt->Item.Sell.Cost_Free			, ENUM_Type_IntPoint    );
		Reader.SetData(0, "Sell_Get_Bonus"  		, &Pt->Item.Sell.Get_Bonus			, ENUM_Type_IntPoint    );
		Reader.SetData(0, "Icon"  				, &Pt->Item.Sell.Icon				, ENUM_Type_IntPoint    );

		Reader.SetData(0, "Gamble_Count"  		, &Pt->Item.Gamble.Count			, ENUM_Type_ShortPoint    );

		for( int i = 0 ; i < 8 ; i++ )
		{
			sprintf_s( Buf , sizeof(Buf) , "Gamble_Rate%d" , i+1 );
			Reader.SetData(0, Buf		, &Pt->Item.Gamble.Rate[i]		, ENUM_Type_ShortPoint    );		
		}

		IsReady = true;
	}
	return &Reader;
}

ReaderClass<ImportBoardBaseStruct>*	RoleDataEx::ReaderRD_ImportBoardBase( )
{
	static  ReaderClass<ImportBoardBaseStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		ImportBoardBaseStruct* Pt = NULL;
		Reader.SetData(0, "UpdateTime"   		, &Pt->UpdateTime			, ENUM_Type_SmallDateTime   );		
		Reader.SetData(0, "SortNumber"   		, &Pt->SortNumber			, ENUM_Type_IntPoint        );		

		Reader.SetData(0, "Title"				, &Pt->Title				, ENUM_Type_WCharString		, sizeof(Pt->Title)       	);
		Reader.SetData(0, "Content"			, &Pt->Content				, ENUM_Type_WCharString		, sizeof(Pt->Content)      	);
		
		Reader.SetData(0, "IsHot"				, &Pt->IsHot				, ENUM_Type_BitPoint        );
		Reader.SetData(0, "IsRed"				, &Pt->IsRed				, ENUM_Type_BitPoint        );
		Reader.SetData(0, "BeginTime"   		, &Pt->BeginTime			, ENUM_Type_SmallDateTime   );		
		Reader.SetData(0, "EndTime"   		, &Pt->EndTime				, ENUM_Type_SmallDateTime   );		

		for( int i = 0 ; i < 5 ; i++ )
		{
			char Buf[ 256 ];
			sprintf_s( Buf , sizeof(Buf) , "Item%d" , i + 1);
			Reader.SetData(0, Buf   		, &Pt->Item[i]					, ENUM_Type_IntPoint   );		
			sprintf_s( Buf , sizeof(Buf) , "ItemCount%d" , i + 1);
			Reader.SetData(0, Buf   		, &Pt->ItemCount[i]				, ENUM_Type_IntPoint   );		
		}
		Reader.SetData(0, "Money"   			, &Pt->Money				, ENUM_Type_IntPoint        );		
		Reader.SetData(0, "Money_Account"   	, &Pt->Money_Account		, ENUM_Type_IntPoint        );		
		IsReady = true;
	}
	return &Reader;
}

ReaderClass<GuildHousesInfoStruct>*	RoleDataEx::ReaderRD_GuildHouseInfo( )
{
	static  ReaderClass<GuildHousesInfoStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		GuildHousesInfoStruct* Pt = NULL;
		Reader.SetData(0, "GuildID"			, &Pt->GuildID				, ENUM_Type_IntPoint       	, 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag  );		
		Reader.SetData(0, "ItemMaxPageCount"	, &Pt->ItemMaxPageCount		, ENUM_Type_IntPoint    );
		Reader.SetData(0, "FurnitureMaxCount"	, &Pt->FurnitureMaxCount	, ENUM_Type_IntPoint    );
		Reader.SetData(0, "LastCalRentTime"	, &Pt->LastCalRentTime		, ENUM_Type_IntPoint		, 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag);
		Reader.SetData(0, "GuildStyle"		, &Pt->GuildStyle			, ENUM_Type_IntPoint    );
		Reader.SetData(0, "GuildStyleRight"	, &Pt->GuildStyleRight		, ENUM_Type_IntPoint    );
		IsReady = true;
	}
	return &Reader;
}

ReaderClass<GuildHouseBuildingInfoStruct>*	RoleDataEx::ReaderRD_GuildHouseBuildingInfo( )
{
	static  ReaderClass<GuildHouseBuildingInfoStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		GuildHouseBuildingInfoStruct* Pt = NULL;
		Reader.SetData(0, "GuildID"			, &Pt->GuildID			, ENUM_Type_IntPoint       	, 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag  );		
		Reader.SetData(0, "BuildingDBID"		, &Pt->BuildingDBID		, ENUM_Type_IntPoint    );
		Reader.SetData(0, "ParentDBID"		, &Pt->ParentDBID		, ENUM_Type_IntPoint    );
		Reader.SetData(0, "BuildingID"		, &Pt->BuildingID		, ENUM_Type_IntPoint    );
		Reader.SetData(0, "Dir"				, &Pt->Dir				, ENUM_Type_FloatPoint    );
		Reader.SetData(0, "PointStr"			, &Pt->PointStr			, ENUM_Type_CharString		, sizeof(Pt->PointStr)  );      
		Reader.SetData(0, "Value"				, &Pt->Value			, ENUM_Type_BinaryData		, sizeof(Pt->Value)  );      
		IsReady = true;
	}
	return &Reader;
}

ReaderClass<GuildHouseItemStruct>*	RoleDataEx::ReaderRD_GuildHouseItem( )
{
	static  ReaderClass<GuildHouseItemStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		GuildHouseItemStruct* Pt = NULL;
		Reader.SetData(0, "GuildID"			, &Pt->GuildID			, ENUM_Type_IntPoint       	, 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag  );		
		Reader.SetData(0, "ItemDBID"			, &Pt->ItemDBID			, ENUM_Type_IntPoint       	, 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag  );		
		Reader.SetData(0, "PageID"			, &Pt->PageID			, ENUM_Type_IntPoint    );
		Reader.SetData(0, "PosID"				, &Pt->PosID			, ENUM_Type_IntPoint    );

		Reader.SetData(0, "OrgObjID"      	, &Pt->Item.OrgObjID			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Serial"			, &Pt->Item.Serial				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "CreateTime"    	, &Pt->Item.CreateTime			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "[Count]"			, &Pt->Item.Count				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Durable"			, &Pt->Item.Durable				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Ability"			,  Pt->Item.Ability				, ENUM_Type_BinaryData		, sizeof(Pt->Item.Ability)  );        
		Reader.SetData(0, "ExValue"			, &Pt->Item.ExValue				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Mode"				, &Pt->Item.Mode				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "ImageObjectID"		, &Pt->Item.ImageObjectID		, ENUM_Type_IntPoint		);        

		IsReady = true;
	}
	return &Reader;
}

ReaderClass<GuildHouseFurnitureItemStruct>*	RoleDataEx::ReaderRD_GuildHouseFurnitureItem( )
{
	static  ReaderClass<GuildHouseFurnitureItemStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		GuildHouseFurnitureItemStruct* Pt = NULL;
		Reader.SetData(0, "GuildID"			, &Pt->GuildID			, ENUM_Type_IntPoint       	, 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag  );		
		Reader.SetData(0, "ItemDBID"			, &Pt->ItemDBID			, ENUM_Type_IntPoint       	, 0						, ENUM_Indirect_Address ,  Def_Disabled_Update_Flag  );		
		Reader.SetData(0, "Pos"				, &Pt->Pos				, ENUM_Type_IntPoint    );

		Reader.SetData(0, "OrgObjID"      	, &Pt->Item.OrgObjID			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Serial"			, &Pt->Item.Serial				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "CreateTime"    	, &Pt->Item.CreateTime			, ENUM_Type_IntPoint       );
		Reader.SetData(0, "[Count]"			, &Pt->Item.Count				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Durable"			, &Pt->Item.Durable				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Ability"			,  Pt->Item.Ability				, ENUM_Type_BinaryData		, sizeof(Pt->Item.Ability)  );        
		Reader.SetData(0, "ExValue"			, &Pt->Item.ExValue				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "Mode"				, &Pt->Item.Mode				, ENUM_Type_IntPoint       );
		Reader.SetData(0, "ImageObjectID"		, &Pt->Item.ImageObjectID		, ENUM_Type_IntPoint		);        
		Reader.SetData(0, "Layout"			, &Pt->Layout					, ENUM_Type_BinaryData  , sizeof(Pt->Layout)  ); 
		Reader.SetData(0, "OwnerDBID"      	, &Pt->OwnerDBID				, ENUM_Type_IntPoint       );

		IsReady = true;
	}
	return &Reader;
}

ReaderClass<OfflineMessageDBStruct>*			RoleDataEx::ReaderRD_OfflineMessage()
{
	static  ReaderClass<OfflineMessageDBStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		OfflineMessageDBStruct* Pt = NULL;

		Reader.SetData(0, "Time"				, &Pt->Time						, ENUM_Type_SmallDateTime    );		
		Reader.SetData(0, "Type"				, &Pt->Type						, ENUM_Type_IntPoint         );		
		Reader.SetData(0, "ToPlayerDBID"		, &Pt->ToPlayerDBID				, ENUM_Type_IntPoint         );		
		Reader.SetData(0, "FromPlayerDBID"	, &Pt->FromPlayerDBID			, ENUM_Type_IntPoint         );		
		Reader.SetData(0, "ManageLv"			, &Pt->ManageLv					, ENUM_Type_IntPoint	    );		
		
		Reader.SetData(0, "ToPlayerName"		,  Pt->ToName.Begin()			, ENUM_Type_WCharString      , sizeof(Pt->ToName)		);		
		Reader.SetData(0, "FromPlayerName"	,  Pt->FromName.Begin()			, ENUM_Type_WCharString      , sizeof(Pt->FromName)		);		
		Reader.SetData(0, "Message"			,  Pt->Content.Begin()			, ENUM_Type_WCharString      , sizeof(Pt->Content)		);		

		IsReady = true;
	}
	return &Reader;
}


ReaderClass<GuildHouseWarInfoStruct>*			RoleDataEx::ReaderRD_GuildHouseWarInfo()
{
	static  ReaderClass<GuildHouseWarInfoStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		GuildHouseWarInfoStruct* Pt = NULL;

//		Reader.SetData(0, "FightTime"			, &Pt->FightTime			, ENUM_Type_SmallDateTime    );		
		Reader.SetData(0, "GuildID"			, &Pt->GuildID				, ENUM_Type_IntPoint         );		
		Reader.SetData(0, "Score"				, &Pt->Score				, ENUM_Type_IntPoint         );		
		Reader.SetData(0, "EnemyGuildID"		, &Pt->EnemyGuildID			, ENUM_Type_IntPoint         );		
		Reader.SetData(0, "State"				, &Pt->State				, ENUM_Type_IntPoint		 );				
		Reader.SetData(0, "GuildName"			,  Pt->GuildName.Begin()	, ENUM_Type_WCharString		 , sizeof( Pt->GuildName ) );		
		Reader.SetData(0, "ZoneGroupID"		, &Pt->ZoneGroupID			, ENUM_Type_IntPoint		 );		
		IsReady = true;
	}
	return &Reader;
}

ReaderClass<GuildHouseWarHistoryStruct>*			RoleDataEx::ReaderRD_GuildHouseWarHistory()
{
	static  ReaderClass<GuildHouseWarHistoryStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		GuildHouseWarHistoryStruct* Pt = NULL;

		Reader.SetData(0, "GuildID"			, &Pt->GuildID					, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "TargetGuildID"		, &Pt->TargetGuildID			, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "EndFightTime"		, &Pt->EndFightTime				, ENUM_Type_SmallDateTime	 );		
		Reader.SetData(0, "Result"			, &Pt->Result					, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "GuildScore"		, &Pt->GuildScore				, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "TargetGuildScore"	, &Pt->TargetGuildScore			, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "TargetGuildName"	, Pt->TargetGuildName.Begin()	, ENUM_Type_WCharString		 , sizeof( Pt->TargetGuildName )		 );		

		IsReady = true;
	}
	return &Reader;
}

int	RoleDataEx::RareAddLv( int Rare )
{
	if( (unsigned)Rare >= 10 )
		return 0;
	return AddLv_byRare[ Rare ];
}

ReaderClass<ReserverRoleStruct>*			RoleDataEx::ReaderRD_ReserveRole()
{
	static  ReaderClass<ReserverRoleStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		ReserverRoleStruct* Pt = NULL;

		Reader.SetData(0, "RoleName"			, Pt->RoleName.Begin()		, ENUM_Type_WCharString		, sizeof( Pt->RoleName )		 );		

		Reader.SetData(0, "IsAutoCreate"		, &Pt->IsAutoCreate			, ENUM_Type_BitPoint		 );		

		Reader.SetData(0, "Sex"				, &Pt->Sex					, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "Race"				, &Pt->Race					, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "Job"				, &Pt->Job					, ENUM_Type_IntPoint		 );		
//		Reader.SetData(0, "GiftItemID"		, &Pt->GiftItemID			, ENUM_Type_IntPoint		 );		
		
		Reader.SetData(0, "BoneScale"			, &Pt->Look.BoneScale		, ENUM_Type_BinaryData		, sizeof( Pt->Look.BoneScale )	 );		
		Reader.SetData(0, "FaceID"			, &Pt->Look.FaceID			, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "HairID"			, &Pt->Look.HairID			, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "HairColor"			, &Pt->Look.HairColor		, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "BodyColor"			, &Pt->Look.BodyColor		, ENUM_Type_IntPoint		 );		

		IsReady = true;
	}
	return &Reader;
}


ReaderClass<DB_CultivatePetStruct>*			RoleDataEx::ReaderRD_CultivatePetStruct()
{
	static  ReaderClass<DB_CultivatePetStruct> Reader;
	static  bool    IsReady = false;
	if( IsReady == false )
	{   
		DB_CultivatePetStruct* Pt = NULL;

		Reader.SetData(0, "PlayerDBID"		, &Pt->PlayerDBID				, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "Pos"				, &Pt->Pos						, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "Name"				, Pt->Info.Name.Begin()			, ENUM_Type_WCharString		, sizeof( Pt->Info.Name )		 );		
		Reader.SetData(0, "ItemOrgID"			, &Pt->Info.ItemOrgID			, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "CreateTime"		, &Pt->Info.CreateTime			, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "PetOrgID"			, &Pt->Info.PetOrgID			, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "Lv"				, &Pt->Info.Lv					, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "PetType"			, &Pt->Info.PetType				, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "EventColdown"		, &Pt->Info.EventColdown		, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "EventType"			, &Pt->Info.EventType			, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "LifeSkillTablePos"	, &Pt->Info.LifeSkillTablePos	, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "Exp"				, &Pt->Info.Exp					, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "SkillPoint"		, &Pt->Info.SkillPoint			, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "TotalSkillPoint"	, &Pt->Info.TotalSkillPoint		, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "Loyal"				, &Pt->Info.Loyal				, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "Hunger"			, &Pt->Info.Hunger				, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "Talent"			, &Pt->Info.Talent				, ENUM_Type_FloatPoint		 );		
		Reader.SetData(0, "Spirit"			, &Pt->Info.Spirit				, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "Mode"				, &Pt->Info.Mode				, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "STR"				, &Pt->Info.STR					, ENUM_Type_FloatPoint		 );		
		Reader.SetData(0, "STA"				, &Pt->Info.STA					, ENUM_Type_FloatPoint		 );		
		Reader.SetData(0, "AGI"				, &Pt->Info.AGI					, ENUM_Type_FloatPoint		 );		
		Reader.SetData(0, "INT"				, &Pt->Info.INT					, ENUM_Type_FloatPoint		 );		
		Reader.SetData(0, "MND"				, &Pt->Info.MND					, ENUM_Type_FloatPoint		 );		
		Reader.SetData(0, "Property"			, &Pt->Info.Property			, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "TrainCount"		, &Pt->Info.TrainCount			, ENUM_Type_IntPoint		 );		

		Reader.SetData(0, "DayMergeCount"		, &Pt->Info.DayMergeCount		, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "LifeSkill1"		, &Pt->Info.LifeSkill[0]		, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "LifeSkill2"		, &Pt->Info.LifeSkill[1]		, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "LifeSkill3"		, &Pt->Info.LifeSkill[2]		, ENUM_Type_IntPoint		 );		
		Reader.SetData(0, "LifeSkill4"		, &Pt->Info.LifeSkill[3]		, ENUM_Type_IntPoint		 );		

		Reader.SetData(0, "Magic"				, &Pt->Info.Magic				, ENUM_Type_BinaryData		, sizeof( Pt->Info.Magic )	 );		
		IsReady = true;
	}
	return &Reader;
}