#pragma once

#include "RoleData\RoleData.h"
#include "NetWakerPGEnum.h"

struct PG_BTI_BTI2L_BTIPlayTime
{
	PACKET_CONSTRUCT( PG_BTI_BTI2L_BTIPlayTime );
	
	int		iRoleDBID;		// �n�J������
	int		iHours;			// �����ɶ�
};



struct PG_Nonage_Nonage2M_SetPlayerHours
{
	PACKET_CONSTRUCT( PG_Nonage_Nonage2M_SetPlayerHours );

	StaticString<_MAX_ACCOUNT_SIZE_>		Account;
	int										iHours;
};