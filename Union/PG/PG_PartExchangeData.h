#pragma once

//#include <windows.h>
#include "NetWakerPGEnum.h"
#include "roledata/RoleDataEx.h"
//----------------------------------------------------------------------------------------------------------
//通知物件Mirror所在的Server產生物件
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
//修正Mirror物件的資料
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
//設定字串
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
//設定數字
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