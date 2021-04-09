#pragma once

//#include "RoleData\RoleData.h"
#include "NetWakerPGEnum.h"

// Client 發送給 Local Server 點選採集物件
struct PG_Gather_CtoL_ClickGatherObj
{
	PACKET_CONSTRUCT( PG_Gather_CtoL_ClickGatherObj );

	int						iObjID;
};

enum ENUM_GATHER_RESULT
{
	EM_GATHER_NONE		= 0,
	EM_GATHER_ENDCAST	= 1,		
	EM_GATHER_SUCCESSED	= 2,
	EM_GATHER_FAILED	= -1,		// 機率沒搖到
	EM_GATHER_GONE		= -2,
	EM_GATHER_LOWSKILL	= -3,
	EM_GATHER_NULLOBJ	= -4,		// 被點擊物件不存在
	EM_GATHER_OBJBUSY	= -5,		// 物件正被點擊中
	EM_GATHER_BEGIN		= 3			// 

};

// Local Server 回應 Client 結果
struct PG_Gather_LtoC_ClickGatherObj_Result
{
	PACKET_CONSTRUCT( PG_Gather_LtoC_ClickGatherObj_Result );

	int						iResult;
	int						iItemID;
	int						iItemVal;
};

struct PG_Gather_LtoC_GatherMotion
{
	PACKET_CONSTRUCT( PG_Gather_LtoC_GatherMotion );

	int						iGatherID;		// 誰開始採集
	int						iMotionID;		// 播放的動作
	int						iMineType;		// 礦的類型
	//int						iObjID;			// 採集物件的代碼
};


struct PG_Gather_CtoL_CancelGather
{
	PACKET_CONSTRUCT( PG_Gather_CtoL_CancelGather );
	int						iObjID;			// 採集物件
};