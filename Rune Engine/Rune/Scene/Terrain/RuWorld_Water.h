#ifndef _RUWORLD_WATER_H_
#define _RUWORLD_WATER_H_

#include "../../Mojo/RuMojo_Base.h"
#include "../../Mojo/RuMojo_Helper.h"
#include "../../Scene/Terrain/RuWorld_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuWorld_Water : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Global identifier
	CRuGUID									m_waterBodyGUID;					//!< GUID identifying the water body this layer belongs to

	// Water layer attributes
	CRuMojo_AttributeParcel<REAL>*			m_waterHF;							//!< Water's height field
	CRuMojo_AttributeParcel<RUCOLOR>*		m_waterCF;							//!< Water's color field
	CRuMojo_AttributeParcel<BYTE>*			m_waterFlags;						//!< Water's flag field

	// Rendering data
	CRuMesh*								m_waterMesh;						//!< Water mesh
	IRuMaterial*							m_waterMaterial;					//!< Water material
	CRuPlane4								m_splitPlane;

	// Runtime stuff
	RuWorld::WaterBodyType					m_runtime_WaterBodyType;

public:
											CRuWorld_Water();
	virtual									~CRuWorld_Water();

	// IRuObject serialization
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// IRuEntity_Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// Initialization
	BOOL									Allocate(const CRuGUID &waterBodyGUID, const CRuVector3 &layerOrigin, INT32 dimension, REAL gridSpacing, INT32 overlapGap);

	// Editing
	BOOL									PaintHeight(const CRuVector3 &center, REAL radius, REAL delta, RuFalloffFunction falloffFunction);
	BOOL									SetHeight(const CRuVector3 &center, REAL radius, REAL height, REAL brushStrength, RuFalloffFunction falloffFunction);
	BOOL									SmoothHeight(const CRuVector3 &center, REAL radius, REAL delta);
	BOOL									PaintSurface(const CRuVector3 &center, REAL radius, BOOL waterVisible);
	BOOL									PaintAlpha(const CRuVector3 &center, REAL radius, REAL alpha, REAL delta, RuFalloffFunction falloffFunction);
	BOOL									PaintColor(const CRuVector3 &center, REAL radius, RUCOLOR color, REAL delta, RuFalloffFunction falloffFunction);
	RUCOLOR									PickColor(const CRuVector3 &center);

	// Layer stitching
	BOOL									StitchWaterLayer(INT32 neighborID, CRuWorld_Water *neighborLayer);

	// Material override
	BOOL									SetWaterLayerMaterial(IRuMaterial *material);

	// Other shit
	BOOL									BuildCollisionStructure();
	void									UpdateCollisionFlags(RuWorld::WaterBodyType waterBodyType);

	// Property access
	const CRuGUID&							GetWaterBodyGUID() const;
	BOOL									ValidateMesh() const;

protected:
	void									BuildWaterLayerMesh();
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
