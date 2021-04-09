/*!
	@file RuGizmo_Base.cpp

	Copyright (c) 2004-2007 All rights reserved
*/

#include "../../Scene/Gizmo/RuGizmo_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuGizmo_Base, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuGizmo_Base, "CRuGizmo_Base", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuGizmo_Base, 0x1022BF11, 0x23DB46e2, 0xBCB3E3F5, 0xDCAA70E3)

// ************************************************************************************************************************************************************

CRuGizmo_Base::~CRuGizmo_Base()
{
}

// ************************************************************************************************************************************************************

#pragma managed(push, off)
