#ifndef _RUWORLD_WATERBODYDESCRIPTOR_H_
#define _RUWORLD_WATERBODYDESCRIPTOR_H_

#include "../../Engine/Base/RuCore.h"
#include "../../Engine/Base/RuMaterialBase.h"
#include "../../Scene/Controller/RuController_MaterialTexture.h"
#include "../../Scene/Terrain/RuWorld_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	CRuWorld_WaterBodyDescriptor
	@author John Tang
	@version 2008.09.10

	Stores information about water bodies.

	The object's properties array contains the following additional properties:

	Property Name                                                Semantics                      Description
	-------------------------------------------------------------------------------------------------------------------------------------
	waterDesc:damage factor                                      REAL                           Damage factor (0.0 disables)

	waterDesc:common:bump factor                                 REAL                           Bump map factor [0.0, 1.0]
	waterDesc:common:wave color                                  RUCOLOR                        Wave color

	waterDesc:full:bump frame rate                               INT32                          Bump map frame rate
	waterDesc:full:bump frame count                              INT32                          Bump map total frame count
	waterDesc:full:bump name                                     const char *                   Bump map base name (filename formation: [base name]_[frame index].[extension])

	waterDesc:full:bump x scroll 0                               REAL                           Bump map scroll speed along x for layer 0
	waterDesc:full:bump y scroll 0                               REAL                           Bump map scroll speed along y for layer 0
	waterDesc:full:bump scale 0                                  REAL                           Bump map scale for layer 0
	waterDesc:full:bump weight 0                                 REAL                           Bump map weight for layer 0

	waterDesc:full:bump x scroll 1                               REAL                           Bump map scroll speed along x for layer 1
	waterDesc:full:bump y scroll 1                               REAL                           Bump map scroll speed along y for layer 1
	waterDesc:full:bump scale 1                                  REAL                           Bump map scale for layer 1
	waterDesc:full:bump weight 1                                 REAL                           Bump map weight for layer 1

	waterDesc:full:bump x scroll 2                               REAL                           Bump map scroll speed along x for layer 2
	waterDesc:full:bump y scroll 2                               REAL                           Bump map scroll speed along y for layer 2
	waterDesc:full:bump scale 2                                  REAL                           Bump map scale for layer 2
	waterDesc:full:bump weight 2                                 REAL                           Bump map weight for layer 2

	waterDesc:full:bump x scroll 3                               REAL                           Bump map scroll speed along x for layer 3
	waterDesc:full:bump y scroll 3                               REAL                           Bump map scroll speed along y for layer 3
	waterDesc:full:bump scale 3                                  REAL                           Bump map scale for layer 3
	waterDesc:full:bump weight 3                                 REAL                           Bump map weight for layer 3

	waterDesc:submerged:bump frame rate                          INT32                          Submerged bump map frame rate
	waterDesc:submerged:bump frame count                         INT32                          Submerged bump map total frame count
	waterDesc:submerged:bump name                                const char *                   Submerged bump map base name (filename formation: [base name]_[frame index].[extension])
	waterDesc:submerged:bump strength                            REAL                           Submerged bump strength [0, 1]

*/

class CRuWorld_WaterBodyDescriptor : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	// Identifying data
	CRuGUID									m_guid;											//!< Water body GUID

	// Water settings
	RuWorld::WaterBodyType					m_bodyType;										//!< Water body type

	RuWaterEffectType						m_effectType;									//!< Water effect type

	RUCOLOR									m_submergedColor;								//!< Color to use while submerged
	REAL									m_submergedConstantFog;
	REAL									m_submergedFogStart;
	REAL									m_submergedFogEnd;

	// Runtime
	BOOL									m_isDirty;										//!< The record is dirty
	CRuMaterial*							m_waterMaterial;								//!< Instanced material
	CRuController_MaterialTexture*			m_baseController;								//!< Controller that alters water base texture
	CRuController_MaterialTexture*			m_causticsController;							//!< Controller that alters water caustics texture

	REAL									m_bumpParams_0[4];
	REAL									m_bumpParams_1[4];
	REAL									m_bumpParams_2[4];
	REAL									m_bumpParams_3[4];

	CRuController_MaterialTexture*			m_bumpController;

	CRuAnimTrack<char *>					m_track_SubmergedBumpName;

public:
											CRuWorld_WaterBodyDescriptor();
	virtual									~CRuWorld_WaterBodyDescriptor();

	// IRuObject serialization
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	void									Update(REAL elapsedTime);

	REAL									GetSubmergedBumpStrength();
	CRuMaterial*							GetWaterBodyMaterial();

	void									BuildWaterBodyMaterial();
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
