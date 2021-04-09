/*!
	@file RuEntity_Widget_Tetrahedron.h

	Copyright (c) 2004-2007 All rights reserved
*/

#ifndef _RUENTITY_WIDGET_TETRAHEDRON_H_
#define _RUENTITY_WIDGET_TETRAHEDRON_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	Tetrahedron widget.

	@version 2007.04.15
*/
class CRuEntity_Widget_Tetrahedron : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
											CRuEntity_Widget_Tetrahedron();
	virtual									~CRuEntity_Widget_Tetrahedron();
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
