/*!
	@project Rune
	@file RuHelper_IRuEntity_Renderable.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/04/13
*/

#pragma once

#include "../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

// Utility functions
extern BOOL						RuEntity_EnumerateMeshes(CRuEntity *entity, CRuArrayList<IRuMesh *> &meshListOut, CRuArrayList<CRuMatrix4x4> *worldTransformListOut = NULL);
extern BOOL						RuEntity_EnumerateSkinnableMeshes(CRuEntity *entity, CRuArrayList<IRuMesh *> &meshListOut);
extern BOOL						RuEntity_EnumerateMaterials(CRuEntity *entity, CRuArrayList<IRuMaterial *> &materialListOut);
extern BOOL						RuEntity_EnumerateMaterials(CRuEntity *entity, BOOL honorNoEnumerateFlag, CRuArrayList<IRuMaterial *> &materialListOut);

extern IRuEntity_Renderable*	RuEntity_FindFirstRenderableEntity(CRuEntity *entity);
extern IRuMesh*					RuEntity_FindFirstMesh(CRuEntity *entity);
extern IRuMesh*					RuEntity_FindFirstSkinnableMesh(CRuEntity *entity);

// Material functions
extern BOOL						RuEntity_UpdateSkinShaders(CRuEntity *entity);

// ************************************************************************************************************************************************************

#pragma managed(pop)
