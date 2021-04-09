#pragma once

#include "NetWakerPGEnum.h"
//-----------------------------------------------------------------------------------------------------------------------
//L -> G1 �[�J�p�C��
struct PG_SmallGame_LtoG1_JoinGame
{
    GamePGCommandEnum	Command;
    char    Account_ID[32];
    int     Money;

    PG_SmallGame_LtoG1_JoinGame()
    {
        Command = EM_PG_SmallGame_LtoG1_JoinGame;
    }
};
//-----------------------------------------------------------------------------------------------------------------------
//G1 -> L �[�J�p�C�� ���G
struct PG_SmallGame_G1toL_JoinGameResult
{
    GamePGCommandEnum	Command;
    char    Account_ID[32];
    int     Money;
    bool    Result;

    PG_SmallGame_G1toL_JoinGameResult()
    {
        Command = EM_PG_SmallGame_G1toL_JoinGameResult;
    }
};
//-----------------------------------------------------------------------------------------------------------------------
//L -> G1 �n�D���}�C��
struct PG_SmallGame_LtoG1_LeaveGame
{
    GamePGCommandEnum	Command;
    char    Account_ID[32];

    PG_SmallGame_LtoG1_LeaveGame()
    {
        Command = EM_PG_SmallGame_LtoG1_LeaveGame;
    }    
};
//-----------------------------------------------------------------------------------------------------------------------
//G1-> L �h�X�C��
struct PG_SmallGame_G1toL_GameOver
{
    GamePGCommandEnum	Command;
    char    Account_ID[32];
    int     Money;

    PG_SmallGame_G1toL_GameOver()
    {
        Command = EM_PG_SmallGame_G1toL_GameOver;
    }
};
//-----------------------------------------------------------------------------------------------------------------------
//G1 -> L �n�D�ಾ����
struct PG_SmallGame_G1toL_TrainMoney
{
    GamePGCommandEnum	Command;
    char    Account_ID[32];
    int     Money;

    PG_SmallGame_G1toL_TrainMoney()
    {
        Command = EM_PG_SmallGame_G1toL_TrainMoney;
    }    
};
//-----------------------------------------------------------------------------------------------------------------------
// L -> G1 �ಾ�������G
struct PG_SmallGame_LtoG1_TrainMoneyResult
{
    GamePGCommandEnum	Command;
    char    Account_ID[32];
    int     Money;
    bool    Result;

    PG_SmallGame_LtoG1_TrainMoneyResult()
    {
        Command = EM_PG_SmallGame_LtoG1_TrainMoneyResult;
    }    
};
//-----------------------------------------------------------------------------------------------------------------------
/*
//L -> G1 �إߤp�ж�
struct PG_SmallGame_LtoG1_CreateRoom
{
    GamePGCommandEnum	Command;
    char    Account_ID[32];
    int     SockID;
    int     DBID;
    int     Money;
    
    PG_SmallGame_LtoG1_CreateRoom()
    {
        Command = EM_PG_SmallGame_LtoG1_CreateRoom;
    }
};
//-----------------------------------------------------------------------------------------------------------------------
//G1 -> L �إߵ��G
struct PG_SmallGame_G1toL_CreateRoomResult
{
    GamePGCommandEnum	Command;
    char    Account_ID[32];
    int     RomeID;
    int     Money;
    bool    Result;

    PG_SmallGame_G1toL_CreateRoomResult()
    {
        Command = EM_PG_SmallGame_G1toL_CreateRoomResult;
    }
};
//-----------------------------------------------------------------------------------------------------------------------
//L -> G1  �[�J�p�ж�
struct PG_SmallGame_LtoG1_AddPlayer
{
    GamePGCommandEnum	Command;
    char    Account_ID[32];
    int	    RoomID;	
    int     SockID;
    int     DBID;
    int     Money;

    PG_SmallGame_LtoG1_AddPlayer()
    {
        Command = EM_PG_SmallGame_LtoG1_AddPlayer;
    }
};
//-----------------------------------------------------------------------------------------------------------------------
//G1 -> L �إߵ��G
struct PG_SmallGame_G1toL_AddPlayerResult
{
    GamePGCommandEnum	Command;
    char    Account_ID[32];
    int     RomeID;
    int     Money;
    bool    Result;

    PG_SmallGame_G1toL_AddPlayerResult()
    {
        Command = EM_PG_SmallGame_G1toL_AddPlayerResult;
    }
};
//-----------------------------------------------------------------------------------------------------------------------
//L -> G1 ���}�C��
struct PG_SmallGame_LtoG1_LeaveGame
{
    GamePGCommandEnum	Command;
    char    Account_ID[32];

    PG_SmallGame_LtoG1_LeaveGame()
    {
        Command = EM_PG_SmallGame_LtoG1_LeaveGame;
    }    
};
//-----------------------------------------------------------------------------------------------------------------------
//G1-> L �h�X�C��
struct PG_SmallGame_G1toL_GameOver
{
    GamePGCommandEnum	Command;
    char    Account_ID[32];
    int     DBID;
    int     Money;

    PG_SmallGame_G1toL_GameOver()
    {
        Command = EM_PG_SmallGame_G1toL_GameOver;
    }
};
*/
//-----------------------------------------------------------------------------------------------------------------------
