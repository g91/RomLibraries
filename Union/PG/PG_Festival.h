#pragma once
#include "NetWakerPGEnum.h"

#define _FESTIVAL_NAME_LENGTH 128
#define _FESTIVAL_SCENE_CONTROL_STRING_LENGTH 32

//////////////////////////////////////////////////////////////////////////
//the festival time
//////////////////////////////////////////////////////////////////////////
struct StructFestivalTime
{
	DWORD dwStartTime;
	DWORD dwEndTime;

	StructFestivalTime()
	{
		dwStartTime = 0;
		dwEndTime   = 0;
	}
};


enum FestivalStatus
{
	EM_FESTIVAL_STATUS_NONE  = 0,
	EM_FESTIVAL_STATUS_BEGIN = 1,
	EM_FESTIVAL_STATUS_END   = 2
};

//////////////////////////////////////////////////////////////////////////
//Send request to DataCenter for festival info
//////////////////////////////////////////////////////////////////////////
struct PG_Festival_LtoD_RequestFestivalInfo
{
	GamePGCommandEnum	Command;
	int					iZoneID;

	PG_Festival_LtoD_RequestFestivalInfo( )
	{	Command = EM_PG_Festival_LtoD_RequestFestivalInfo;	}
};

//////////////////////////////////////////////////////////////////////////
//Send a single festival status to client
//////////////////////////////////////////////////////////////////////////
struct PG_Festival_LtoC_FestivalStatus
{
	GamePGCommandEnum	Command;
	char				strSceneControl[_FESTIVAL_SCENE_CONTROL_STRING_LENGTH];
	int					iFestivalID;
	int					iStatus;

	PG_Festival_LtoC_FestivalStatus( )
	{	Command = EM_PG_Festival_LtoC_FestivalStatus;	}	
};