#pragma once

#include <windows.h>
#include "NetWakerPGEnum.h"
//----------------------------------------------------------------------------------------------------------
//Local -> Part 
//�]�w GameSrv ���B�z�d��
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
//�[�J�@�Ӫ���
struct PG_PartBase_LtoP_AddObj
{
    GamePGCommandEnum	Command;
    int                 GItemID;
    float               PosX , PosY , PosZ;    //�Ҧb��m
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
//���󲾰�
struct PG_PartBase_LtoP_MoveObj
{
    GamePGCommandEnum	Command;
    int                 GItemID;
    float               PosX , PosY , PosZ;    //�Ҧb��m
    float               Dir;

    PG_PartBase_LtoP_MoveObj()
    {
        Command = EM_PG_PartBase_LtoP_MoveObj;
    }   
};
//----------------------------------------------------------------------------------------------------------
//Part -> Local
//�D�����󲾰� �n�D Mirror ���� Srv �]�w Mirror ����
struct PG_PartBase_PtoL_MoveObj
{
    GamePGCommandEnum	Command;
    int                 GItemID;
    float               PosX , PosY , PosZ;    //�Ҧb��m
    float               Dir;

    PG_PartBase_PtoL_MoveObj()
    {
        Command = EM_PG_PartBase_PtoL_MoveObj;
    }   
};
//----------------------------------------------------------------------------------------------------------
//Part -> Local
//�D�����󲾰� �n�D Mirror ���� Srv �]�w Mirror ����
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
//����R��	
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
//����Ҧb��GameSrv : ����[�J�Y�@��Srv 
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
//Mirror������GameSrv: �Y����[�J
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
//����Ҧb��GameSrv : ����b�YSrv�w�Q����
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
//Mirror������GameSrv : ���󲾰�
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
//��e��ƨ�Y�@�Ӱ϶�
struct PG_PartBase_LtoP_DataTransfer
{
    GamePGCommandEnum	Command;
    int                 GItemID;        //�H���@�Ӫ��󬰤���
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
//��e��ƨ�Y�@�Ӱ϶�
struct PG_PartBase_PtoL_NotInControlArea
{
	GamePGCommandEnum	Command;
	int                 GItemID;        //�H���@�Ӫ��󬰤���

	PG_PartBase_PtoL_NotInControlArea()
	{
		Command = EM_PG_PartBase_PtoL_NotInControlArea;
	}   
};
//----------------------------------------------------------------------------------------------------------