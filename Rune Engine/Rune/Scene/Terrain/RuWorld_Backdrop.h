/*!
	@file RuEntity_Backdrop.h

	Copyright (c) 2004-2007 All rights reserved
*/

#ifndef _RUWORLD_BACKDROP_H_
#define _RUWORLD_BACKDROP_H_

#include "../../Scene/Terrain/RuWorld_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuWorld_Backdrop : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32									m_backdropKey;

	CRuVector3								m_origin;							//!< Origin of the backdrop
	REAL									m_gridSpacing;
	INT32									m_dimension;
	INT32									m_lodLevels;

	// Runtime
	CRuMesh*								m_mesh;
	CRuMaterial*							m_material;

	CRuArrayList<CRuProxyMesh *>			m_lodMeshes;
	INT32									m_currentLOD;

	CRuAABB									m_boundingBox;

public:
											CRuWorld_Backdrop();
	virtual									~CRuWorld_Backdrop();

	INT32									GetBackdropKey();
	void									SetBackdropKey(INT32 backdropKey);

	BOOL									Initialize(const CRuVector3 &origin, REAL gridSpacing, INT32 dimension, INT32 lodLevels, REAL *heightField, IRuMeshTopology *meshTopology);

	INT32									GetLOD();
	void									SetLOD(INT32 lod);

	const CRuAABB&							GetBoundingBox() const;

	// IRuObject serialization
	BOOL									SerializeFrom(IRuStream *stream, IRuMeshTopology *meshTopology);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Memory reporting
	virtual INT32							GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	// IRuEntity_Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
