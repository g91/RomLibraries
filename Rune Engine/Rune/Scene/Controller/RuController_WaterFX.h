#ifndef _RUCONTROLLER_WATERFX_H_
#define _RUCONTROLLER_WATERFX_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Controller/RuController_MaterialTexture.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!

	@author John Tang
	@version 2007.11.08
*/
class CRuController_WaterFX : public IRuEntity_KeyableController
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEntity*								m_targetEntity;									//!< Entity that the controller affects

	// Properties
	RuWaterEffectType						m_effectType;

	REAL									m_alphaFactor;
	REAL									m_baseFactor;
	REAL									m_reflectionFactor;
	REAL									m_refractionFactor;
	REAL									m_specularStrength;

	RUCOLOR									m_baseColor;

	REAL									m_uvScale;

	INT32									m_baseMapFrames;
	char*									m_baseMapName;

	INT32									m_normalMapFrames;
	char*									m_normalMapName;

	INT32									m_cubeMapFrames;
	char*									m_cubeMapName;

	INT32									m_bumpMapFrames;
	char*									m_bumpMapName;

	REAL									m_bumpScale[4];
	REAL									m_bumpWeight[4];
	CRuVector2								m_bumpScroll[4];

	// Controller tracks
	CRuAnimTrack<char *>					m_track_Motion;

	// Runtime
	BOOL									m_isApplied;
	BOOL									m_isMaterialDirty;
	CRuMaterial*							m_material;
	CRuController_MaterialTexture*			m_bumpController;

public:
											CRuController_WaterFX();
	virtual									~CRuController_WaterFX();

	// Properties
	RuWaterEffectType						GetEffectType();
	void									SetEffectType(RuWaterEffectType effectType);

	REAL									GetAlphaFactor();
	void									SetAlphaFactor(REAL factor);

	REAL									GetBaseFactor();
	void									SetBaseFactor(REAL factor);

	REAL									GetReflectionFactor();
	void									SetReflectionFactor(REAL factor);

	REAL									GetRefractionFactor();
	void									SetRefractionFactor(REAL factor);

	REAL									GetSpecularStrength();
	void									SetSpecularStrength(REAL strength);

	RUCOLOR									GetBaseColor();
	void									SetBaseColor(RUCOLOR baseColor);

	REAL									GetUVScale();
	void									SetUVScale(REAL uvScale);

	INT32									GetBaseMapFrames();
	void									SetBaseMapFrames(INT32 frames);

	const char*								GetBaseMapName();
	void									SetBaseMapName(const char *name);

	INT32									GetNormalMapFrames();
	void									SetNormalMapFrames(INT32 frames);

	const char*								GetNormalMapName();
	void									SetNormalMapName(const char *name);

	INT32									GetCubeMapFrames();
	void									SetCubeMapFrames(INT32 frames);

	const char*								GetCubeMapName();
	void									SetCubeMapName(const char *name);

	INT32									GetBumpMapFrames();
	void									SetBumpMapFrames(INT32 frames);

	const char*								GetBumpMapName();
	void									SetBumpMapName(const char *name);

	REAL									GetBumpScale(INT32 layerIdx);
	void									SetBumpScale(INT32 layerIdx, REAL scale);

	REAL									GetBumpWeight(INT32 layerIdx);
	void									SetBumpWeight(INT32 layerIdx, REAL weight);

	const CRuVector2&						GetBumpScroll(INT32 layerIdx);
	void									SetBumpScroll(INT32 layerIdx, const CRuVector2 &scroll);

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *targetEntity);

	// IRuEntity_KeyableController implementation
	virtual INT32							GetNumTracks() const;
	virtual BOOL							GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);

protected:
	void									BuildMaterial();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_WaterFX, CRuController_WaterFX)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
