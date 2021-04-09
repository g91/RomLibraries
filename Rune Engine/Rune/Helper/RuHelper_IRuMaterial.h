/*!
	@project Rune
	@file RuHelper_IRuMaterial.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/04/13
*/

#pragma once

#include "../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

extern BOOL							RuMaterial_BuildDyeFXMask(IRuMaterial *material);
extern BOOL							RuMaterial_GetDyeFXColors(IRuMaterial *material, BOOL *layerActivatedOut, RUCOLOR *layerColorOut);
extern BOOL							RuMaterial_SetDyeFXColors(IRuMaterial *material, const BOOL *layerActivated, const RUCOLOR *layerColor);
extern BOOL							RuMaterial_GetDyeFXExists(IRuMaterial *material, INT32 layerIdx);

// ************************************************************************************************************************************************************

#pragma managed(pop)
