#pragma once

//#include "RoleData\RoleData.h"
#include "NetWakerPGEnum.h"

// Client �o�e�� Local Server �I��Ķ�����
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
	EM_GATHER_FAILED	= -1,		// ���v�S�n��
	EM_GATHER_GONE		= -2,
	EM_GATHER_LOWSKILL	= -3,
	EM_GATHER_NULLOBJ	= -4,		// �Q�I�����󤣦s�b
	EM_GATHER_OBJBUSY	= -5,		// ���󥿳Q�I����
	EM_GATHER_BEGIN		= 3			// 

};

// Local Server �^�� Client ���G
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

	int						iGatherID;		// �ֶ}�l�Ķ�
	int						iMotionID;		// ���񪺰ʧ@
	int						iMineType;		// �q������
	//int						iObjID;			// �Ķ����󪺥N�X
};


struct PG_Gather_CtoL_CancelGather
{
	PACKET_CONSTRUCT( PG_Gather_CtoL_CancelGather );
	int						iObjID;			// �Ķ�����
};