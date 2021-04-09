#pragma once

//#include <windows.h>
#include "NetWakerPGEnum.h"
#include "roledata/RoleDataEx.h"
//----------------------------------------------------------------------------------------------------------
//�q������Mirror�Ҧb��Server���ͪ���
//Local -> Mirror Local
struct PG_Part_LtoL_NewMirrorRole
{
    GamePGCommandEnum	Command;

    int         GItemID;     
    RoleData    Role;

    PG_Part_LtoL_NewMirrorRole()
    {
        Command = EM_PG_Part_LtoL_NewMirrorRole;
    }

};
//----------------------------------------------------------------------------------------------------------
//�ץ�Mirror���󪺸��
//Local -> Mirror Local
struct PG_Part_LtoL_ModifyMirrorRole
{
    GamePGCommandEnum	Command;

    int         GItemID;     
    int         Offset;
    int         DataSize;
    RoleData    Role;

    PG_Part_LtoL_ModifyMirrorRole()
    {
        Command = EM_PG_Part_LtoL_ModifyMirrorRole;
    }

    int Size( )
    {
        return DataSize + 12;
    }

};
//----------------------------------------------------------------------------------------------------------
//�]�w�r��
//Local -> Mirror Local

struct PG_Part_LtoL_SettingStr
{
    GamePGCommandEnum	Command;
    
    int         GItemID;     
    int         ValueID;
    char        Str[64];


    PG_Part_LtoL_SettingStr()
    {
        Command = EM_PG_Part_LtoL_SettingStr;
    }

};
//----------------------------------------------------------------------------------------------------------
//�]�w�Ʀr
//Local -> Mirror Local

struct PG_Part_LtoL_SettingNumber
{
    GamePGCommandEnum	Command;

    int         GItemID;
    int         ValueID;
    float       Value;


    PG_Part_LtoL_SettingNumber()
    {
        Command = EM_PG_Part_LtoL_SettingNumber;
    }

};
//----------------------------------------------------------------------------------------------------------