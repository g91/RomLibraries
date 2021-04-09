#pragma once
#include "NetWakerPGEnum.h"
#include <memory.h>
#pragma unmanaged

#define PE_KEY_STRING_LENGTH 64

//---------------------------------------------------------------------
// AreaEvent status
//---------------------------------------------------------------------
enum PublicEncounterStatus
{
	EM_PE_STATUS_SLEEPING			= 0,	//PE is deactivated
	EM_PE_STATUS_PAUSE				= 1,	//PE is paused
	EM_PE_STATUS_PROCESSING			= 2,	//PE is processing
	EM_PE_STATUS_WAIT_FOR_RESET		= 3,	//PE is waiting for restart for a certain time
	EM_PE_STATUS_SUCCESS			= 4,	//PE is successful.
	EM_PE_STATUS_FAILED						//PE is failed.
};

//---------------------------------------------------------------------
// phase status
//---------------------------------------------------------------------
enum PublicEncounterPhaseStatus
{
	EM_PHASE_STATUS_PROCESSING		= 0,	//Phase is processing
	EM_PHASE_STATUS_SUCCESS			= 1,	//PE is successful
	EM_PHASE_STATUS_SUCCESS_WAIT	= 2,	//PE is successful, but waiting for entering next phase.
	EM_PHASE_STATUS_FAILED			= 3,	//PE is failed.
	EM_PHASE_STATUS_FAILED_WAIT		= 4		//PE is failed, but waiting for entering next phase.
};

//---------------------------------------------------------------------
// objective status
//---------------------------------------------------------------------
enum PublicEncounterObjectiveStatus
{
	EM_OBJ_STATUS_PROCESSING		= 0,	//Objective is processing
	EM_OBJ_STATUS_ACHIEVED			= 1		//Objective is achieved
};

//---------------------------------------------------------------------
// objective status
//---------------------------------------------------------------------
enum PublicEncounterObjectiveKind
{
	EM_OBJ_KIND_SUCCESSFUL			= 0,	//Objective is a successful term.
	EM_OBJ_KIND_FAIL				= 1		//Objective is a failed	term.
};

//---------------------------------------------------------------------
// objective status
//---------------------------------------------------------------------
enum PublicEncounterObjectiveType
{
	EM_OBJ_TYPE_NONE				= 0,	//
	EM_OBJ_TYPE_GREATER				= 1,	// Objective condition: >
	EM_OBJ_TYPE_SMALLER				= 2,	// Objective condition: <
	EM_OBJ_TYPE_EQUAL				= 3,	// Objective condition: =
	EM_OBJ_TYPE_NOT_EQUAL			= 4,	// Objective condition: !=
	EM_OBJ_TYPE_EQUAL_AND_GREATER	= 5,	// Objective condition: >=
	EM_OBJ_TYPE_EQUAL_AND_SMALLER	= 6,	// Objective condition: <=
	EM_OBJ_TYPE_TIME_LIMIT			= 7		// Objective condition: time limit
};

//---------------------------------------------------------------------
// objective status
//---------------------------------------------------------------------
enum PublicEncounterObjectiveLogic
{
	EM_OBJ_LOGIC_AND				= 0,
	EM_OBJ_LOGIC_OR					= 1
};

//---------------------------------------------------------------------
// player status
//---------------------------------------------------------------------
enum PublicEncounterPlayerStatus
{
	PE_PLAYER_DEACTIVATED				= 0,
	PE_PLAYER_ACTIVATED					= 1,
	PE_PLAYER_DISCONNECTED				= 2,
	PE_PLAYER_DISCONNECTED_WAIT_REMOVE	= 3,
};

//---------------------------------------------------------------------
// objective flags
//---------------------------------------------------------------------
enum PublicEncounterObjectiveFlag
{
	EM_OBJ_FLAG_VISIBLE	= 0x00000001,
	EM_OBJ_FLAG_MINOR	= 0x00000002
};

//---------------------------------------------------------------------
// PE flags
//---------------------------------------------------------------------
enum PublicEncounterFlag
{
	EM_PE_FLAG_SCORE_VISIBLE = 0x00000001,
	EM_PE_FLAG_UI_VISIBLE	 = 0x00000002
};

//---------------------------------------------------------------------
// objective base data, for both server and client
//---------------------------------------------------------------------
struct StructPEObjectiveBaseData
{
	char szNameKeyStr[PE_KEY_STRING_LENGTH]; // Name display key string
	char szDescKeyStr[PE_KEY_STRING_LENGTH]; // Description display key string
	int  iKind;							     // 0 = successful objective, 1 = fail objective , see PublicEncounterObjectiveTermKind
	int  iType;								 // see PublicEncounterObjectiveTermType
	int	 iInitValue;						 // the init value of this term
	int	 iNowValue;							 // the now value of this term
	int	 iDoorsill;							 // the door sill value of this term
	int  iPEIndex;							 // the index value for identifying which PE
	int  iPHIndex;							 // the index value for identifying which phase
	int  iOBIndex;							 // the index value for identifying which objective
	int  iFlag;								 // bit1 = visible bit2 = minor
	int  iStatus;							 // current status value of this objective

	StructPEObjectiveBaseData()
	{
		memset(this, 0, sizeof(*this));
	}
};

//---------------------------------------------------------------------
// phase base data, for both server and client
//---------------------------------------------------------------------
struct StructPEPhaseBaseData
{
	char  szNameKeyStr[PE_KEY_STRING_LENGTH]; // Name display key string
	char  szDescKeyStr[PE_KEY_STRING_LENGTH]; // Description key string
	int   iPEIndex;							 // the index value for identifying which PE
	int   iPHIndex;							 // the index value for identifying which phase
	int   iSuccLogic;						 // 0 = and, 1 = or, see PublicEncounterObjectiveLogic
	int   iFailLogic;						 // 0 = and, 1 = or, see PublicEncounterObjectiveLogic
	float fPHScore;							 // the total score for the phase
	int   iStatus;							 // current status value of this phase

	StructPEPhaseBaseData()
	{
		memset(this, 0, sizeof(*this));
	}
};

//---------------------------------------------------------------------
// public encounter base data, for both server and client
//---------------------------------------------------------------------
struct StructPEBaseData
{
	char szNameKeyStr[PE_KEY_STRING_LENGTH]; // Name key string
	char szDescKeyStr[PE_KEY_STRING_LENGTH]; // Description display key string
	int  iPEIndex;							 // the index value for identifying which PE
	int  iActivePhaseIndex;					 // the index value of active phase
	int  iStatus;							 // current status value of this PE
	int	 iFlag;								 // bit1 = Score visible

	StructPEBaseData()
	{
		memset(this, 0, sizeof(*this));
	}
};

//---------------------------------------------------------------------
// public encounter base data, for both server and client
//---------------------------------------------------------------------
struct StructPERegionData
{
	int  iPEIndex;
	int  iRegionID;							 // the id of the region

	StructPERegionData()
	{
		memset(this, 0, sizeof(*this));
	}
};

//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_LtoC_PEConfigData
{	
	GamePGCommandEnum	Command;
	int					iTotalPENum;				// the index value for identifying which PE
	int					iTotalRegionNum;
	int					iTotalPhaseNum;
	int					iTotalObjectiveNum;

	PG_PE_LtoC_PEConfigData()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_LtoC_PEConfigData;
	}
};

//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_LtoC_PEStatus
{	
	GamePGCommandEnum	Command;
	int					iPEIndex;	// the index value for identifying which PE
	int					iStatus;    // the status of the PE, see PublicEncounterStatus

	PG_PE_LtoC_PEStatus()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_LtoC_PEStatus;
	}
};

//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_LtoC_PhaseStatus
{	
	GamePGCommandEnum	Command;
	int					iPEIndex;	// the index value for identifying which PE
	int					iPHIndex;	// the index value for identifying which phase
	int					iStatus;    // the status of the phase, see PublicEncounterPhaseStatus

	PG_PE_LtoC_PhaseStatus()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_LtoC_PhaseStatus;
	}
};

//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_LtoC_ObjectiveStatus
{	
	GamePGCommandEnum	Command;
	int					iPEIndex;	// the index value for identifying which PE
	int					iPHIndex;	// the index value for identifying which phase
	int					iOBIndex;	// the index value for identifying which objective
	int					iStatus;    // the status of the objective, see PublicEncounterObjectiveStatus

	PG_PE_LtoC_ObjectiveStatus()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_LtoC_ObjectiveStatus;
	}
};


//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_LtoC_PlayerScore
{	
	GamePGCommandEnum	Command;
	int					iPEIndex;				// the index value for identifying which PE
	int					iScore;					// the player's score

	PG_PE_LtoC_PlayerScore()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_LtoC_PlayerScore;
	}
};

//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_LtoC_PEActivated
{
	GamePGCommandEnum	Command;
	int					iPEIndex;			// the index value for identifying which PE
	int					iActivePHIndex;		// the index value of active phase

	PG_PE_LtoC_PEActivated()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_LtoC_PEActivated;
	}
};


//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_LtoC_PEDeactivated
{
	GamePGCommandEnum	Command;
	int					iPEIndex;				// the index value for identifying which PE

	PG_PE_LtoC_PEDeactivated()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_LtoC_PEDeactivated;
	}
};

//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_LtoC_PEActivePhaseChange
{
	GamePGCommandEnum	Command;
	int					iPEIndex;			// the index value for identifying which PE
	int					iActivePHIndex;		// the index value of active phase

	PG_PE_LtoC_PEActivePhaseChange()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_LtoC_PEActivePhaseChange;
	}
};


//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_LtoC_ObjectiveValueChage
{	
	GamePGCommandEnum	Command;
	int					iPEIndex;	// the index value for identifying which PE
	int					iPHIndex;	// the index value for identifying which phase
	int					iOBIndex;	// the index value for identifying which objective
	int					iNowValue;  // the now value of the objective

	PG_PE_LtoC_ObjectiveValueChage()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_LtoC_ObjectiveValueChage;
	}
};

//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_LtoC_PESuccess
{	
	GamePGCommandEnum	Command;
	int					iPEIndex;		// the index value for identifying which PE
	int					iResetTimeGap;	// how long will the PE restart again

	PG_PE_LtoC_PESuccess()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_LtoC_PESuccess;
	}
};

//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_LtoC_PEFailed
{	
	GamePGCommandEnum	Command;
	int					iPEIndex;		// the index value for identifying which PE
	int					iResetTimeGap;	// how long will the PE restart again

	PG_PE_LtoC_PEFailed()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_LtoC_PEFailed;
	}
};

//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_LtoC_PhaseSuccess
{	
	GamePGCommandEnum	Command;
	int					iPEIndex;			// the index value for identifying which PE
	int					iPHIndex;		    // the index value for identifying which phase
	int					iNextPhaseTimeGap;	// how long will the next phase begin

	PG_PE_LtoC_PhaseSuccess()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_LtoC_PhaseSuccess;
	}
};

//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_LtoC_PhaseFailed
{	
	GamePGCommandEnum	Command;
	int					iPEIndex;			// the index value for identifying which PE
	int					iPHIndex;		    // the index value for identifying which phase
	int					iNextPhaseTimeGap;	// how long will the next phase begin

	PG_PE_LtoC_PhaseFailed()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_LtoC_PhaseFailed;
	}
};

//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_LtoC_ObjectiveAchieved
{	
	GamePGCommandEnum	Command;
	int					iPEIndex;	// the index value for identifying which PE
	int					iPHIndex;	// the index value for identifying which phase
	int					iOBIndex;	// the index value for identifying which objective

	PG_PE_LtoC_ObjectiveAchieved()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_LtoC_ObjectiveAchieved;
	}
};

//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_CtoL_RegionChanged
{	
	GamePGCommandEnum	Command;
	int					iRegionID;			// public encounter region id

	PG_PE_CtoL_RegionChanged()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_CtoL_RegionChanged;
	}
};

//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_LtoC_ObjectiveFlag
{	
	GamePGCommandEnum	Command;
	int					iPEIndex;	// the index value for identifying which PE
	int					iPHIndex;	// the index value for identifying which phase
	int					iOBIndex;	// the index value for identifying which objective
	int					iFlag;		// flag value

	PG_PE_LtoC_ObjectiveFlag()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_LtoC_ObjectiveFlag;
	}
};

//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------
struct PG_PE_LtoC_PEFlag
{	
	GamePGCommandEnum	Command;
	int					iPEIndex;	// the index value for identifying which PE
	int					iFlag;		// flag value

	PG_PE_LtoC_PEFlag()
	{
		memset(this, 0, sizeof(*this));
		Command = EM_PG_PE_LtoC_PEFlag;
	}
};

#pragma managed