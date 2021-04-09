/*!
	@file RuEntity_Decal.h

	Copyright (c) 2004-2006 All rights reserved
*/

#ifndef _RUENTITY_DECAL_H_
#define _RUENTITY_DECAL_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Decal entity.

	@version 2006.03.16
*/
class CRuEntity_Decal : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32									m_maximumDecalRadius;
	INT32									m_maximumDecalFaces;
	CRuMatrix4x4							m_decalTransform;
	RuBillboardType							m_uvBillboardType;
	REAL									m_decalRadius;
	REAL									m_decalVerticalLimit;
	CRuVector3								m_decalUVBasis[3];
	REAL									m_decalUOffset;
	REAL									m_decalVOffset;
	CRuSphere								m_decalBounds;

	REAL									m_enumerationPadding;
	CRuSphere								m_enumerationBounds;

	// Runtime stuff
	CRuMatrix4x4							m_uvTransform;
	CRuMatrix4x4							m_aggregateMatrix;

	CRuDynamicMesh*							m_mesh;
	CRuMaterial*							m_material;

public:
											CRuEntity_Decal();
	virtual									~CRuEntity_Decal();

	void									SetDecalRadius(REAL decalRadius);
	void									SetDecalTexture(IRuTexture *texture);
	void									SetDecalTransform(const CRuMatrix4x4 &decalTransform);
	void									SetDecalUVBasis(const CRuVector3 basisVectors[3], REAL uOffset, REAL vOffset);

	void									BuildDecalMesh(const CRuArrayList<CRuEntity *> &decalTargets);

	// Properties
	RuBillboardType							GetBillboardType();
	void									SetBillboardType(RuBillboardType billboardType);

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	// IRuEntity_Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	// Event handlers
	BOOL									HandleEvent_Camera(RuEventArgs *eventArgs);
	BOOL									HandleEvent_DecalTarget(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_Decal, CRuEntity_Decal)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
