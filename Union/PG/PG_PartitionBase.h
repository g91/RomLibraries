#pragma once

#include <windows.h>
#include "NetWakerPGEnum.h"
//----------------------------------------------------------------------------------------------------------
//Local -> Part 
//設定 GameSrv 的處理範圍
struct PG_PartBase_LtoP_SetLocal
{
    GamePGCommandEnum	Command;
    int		ZoneID;
	int		MapID;
	char	MapFileName[64];
    
    PG_PartBase_LtoP_SetLocal()
    {
        Command = EM_PG_PartBase_LtoP_SetLocal;
    }   
};
//----------------------------------------------------------------------------------------------------------
//Local -> Part 
//加入一個物件
struct PG_PartBase_LtoP_AddObj
{
    GamePGCommandEnum	Command;
    int                 GItemID;
    float               PosX , PosY , PosZ;    //所在位置
    float               Dir;
	char				RoleName[32];
	int					ParallelID;
    PG_PartBase_LtoP_AddObj()
    {
        Command = EM_PG_PartBase_LtoP_AddObj;
    }   
};
//----------------------------------------------------------------------------------------------------------
//Local -> Part 
//物件移動
struct PG_PartBase_LtoP_MoveObj
{
    GamePGCommandEnum	Command;
    int                 GItemID;
    float               PosX , PosY , PosZ;    //所在位置
    float               Dir;

    PG_PartBase_LtoP_MoveObj()
    {
        Command = EM_PG_PartBase_LtoP_MoveObj;
    }   
};
//----------------------------------------------------------------------------------------------------------
//Part -> Local
//主控物件移動 要求 Mirror 物件 Srv 設定 Mirror 物件
struct PG_PartBase_PtoL_MoveObj
{
    GamePGCommandEnum	Command;
    int                 GItemID;
    float               PosX , PosY , PosZ;    //所在位置
    float               Dir;

    PG_PartBase_PtoL_MoveObj()
    {
        Command = EM_PG_PartBase_PtoL_MoveObj;
    }   
};
//----------------------------------------------------------------------------------------------------------
//Part -> Local
//主控物件移動 要求 Mirror 物件 Srv 設定 Mirror 物件
struct PG_PartBase_PtoL_ChangeZone
{
    GamePGCommandEnum	Command;
    int                 GItemID;
    int                 ToZoneID;

    PG_PartBase_PtoL_ChangeZone()
    {
        Command = EM_PG_PartBase_PtoL_ChangeZone;
    }   
};
//----------------------------------------------------------------------------------------------------------
//Local -> Part 
//物件刪除	
struct PG_PartBase_LtoP_DelObj
{
    GamePGCommandEnum	Command;
    int                 GItemID;

    PG_PartBase_LtoP_DelObj()
    {
        Command = EM_PG_PartBase_LtoP_DelObj;
    }   
};
//----------------------------------------------------------------------------------------------------------
//Part -> Local
//物件所在的GameSrv : 物件加入某一個Srv 
struct PG_PartBase_PtoL_OnAddtoOtherSrv
{
    GamePGCommandEnum	Command;
    int                 OtherZoneID;
    int                 GItemID;

    PG_PartBase_PtoL_OnAddtoOtherSrv()
    {
        Command = EM_PG_PartBase_PtoL_OnAddtoOtherSrv;
    }   
};
//----------------------------------------------------------------------------------------------------------
//Part -> Local 
//Mirror的物件的GameSrv: 某物件加入
struct PG_PartBase_PtoL_OnAdd
{
    GamePGCommandEnum	Command;
    int                 GItemID;

    PG_PartBase_PtoL_OnAdd()
    {
        Command = EM_PG_PartBase_PtoL_OnAdd;
    }   
};
//----------------------------------------------------------------------------------------------------------
//Part -> Local
//物件所在的GameSrv : 物件在某Srv已被移除
struct PG_PartBase_PtoL_OnDelFromOtherSrv
{
    GamePGCommandEnum	Command;
    int                 OtherZoneID;
    int                 GItemID;

    PG_PartBase_PtoL_OnDelFromOtherSrv()
    {
        Command = EM_PG_PartBase_PtoL_OnDelFromOtherSrv;
    }   
};
//----------------------------------------------------------------------------------------------------------
//Part -> Local 
//Mirror的物件的GameSrv : 物件移除
struct PG_PartBase_PtoL_OnDel
{
    GamePGCommandEnum	Command;
    int                 GItemID;

    PG_PartBase_PtoL_OnDel()
    {
        Command = EM_PG_PartBase_PtoL_OnDel;
    }   
};
//----------------------------------------------------------------------------------------------------------
//Local -> Part 
//轉送資料到某一個區塊
struct PG_PartBase_LtoP_DataTransfer
{
    GamePGCommandEnum	Command;
    int                 GItemID;        //以那一個物件為中心
    int                 DataSize;
    char                Data[0xffff];                 

    PG_PartBase_LtoP_DataTransfer()
    {
        Command = EM_PG_PartBase_LtoP_DataTransfer;
    }   
    int Size()
    {
        return sizeof(*this) - sizeof(Data) + DataSize;
    }
};
//----------------------------------------------------------------------------------------------------------
//Part -> Local
//轉送資料到某一個區塊
struct PG_PartBase_PtoL_NotInControlArea
{
	GamePGCommandEnum	Command;
	int                 GItemID;        //以那一個物件為中心

	PG_PartBase_PtoL_NotInControlArea()
	{
		Command = EM_PG_PartBase_PtoL_NotInControlArea;
	}   
};
//----------------------------------------------------------------------------------------------------------