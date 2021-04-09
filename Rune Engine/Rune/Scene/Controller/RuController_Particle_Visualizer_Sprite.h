#ifndef _RUCONTROLLER_PARTICLE_VISUALIZER_SPRITE_H_
#define _RUCONTROLLER_PARTICLE_VISUALIZER_SPRITE_H_

#include "../../Scene/Base/RuEntityBase.h"

#include "../../Scene/Particle/RuEntity_Particle_Visualizer_Sprite.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Sprite visualizer controller.

	@author John Tang
	@version 2005/11/30
*/
class CRuController_Particle_Visualizer_Sprite : public IRuEntity_KeyableController
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Particle visualizer setting
	CRuEntity_Particle_Visualizer_Sprite*	m_particleVisualizer;				//!< Target visualizer

	// Particle visualization settings
	BOOL									m_allowWorldRotation;
	RuOrthogonalAxis						m_orientation;						//!< Orientation of the particle polygon
	RuBillboardType							m_billboardType;					//!< Billboard type
	RuOrthogonalAxis						m_billboardAxis;
	RuAlphaMode								m_blendType;						//!< Blend mode

	char*									m_spriteTextureName;				//!< Sprite texture name
	INT32									m_spriteSubTextureWidth;
	INT32									m_spriteSubTextureHeight;
	REAL									m_frameRate;
	INT32									m_numFrames;

	BOOL									m_orientToDirectionOfTravel;
	BOOL									m_randomRotation;

	CRuVector3								m_quadOffset;
	CRuVector3								m_quadScale;						//!< Sprite quad scale

	CRuAnimTrack<CRuColor4>					m_track_Color;						//!< Color track
	CRuAnimTrack<REAL>						m_track_Alpha;						//!< Alpha track
	CRuAnimTrack<REAL>						m_track_Scale;						//!< Scale track
	CRuAnimTrack<REAL>						m_track_RotationSpeed;				//!< Rotation speed

public:
											CRuController_Particle_Visualizer_Sprite();
	virtual									~CRuController_Particle_Visualizer_Sprite();

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);
	virtual BOOL							ResetStates();

	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *targetEntity);

	// IRuEntity_KeyableController implementation
	virtual INT32							GetNumTracks() const;
	virtual BOOL							GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);

	// Property access
	BOOL									GetAllowWorldRotation() const;
	void									SetAllowWorldRotation(BOOL allowWorldRotation);

	RuOrthogonalAxis						GetOrientation() const;
	void									SetOrientation(RuOrthogonalAxis orientation);

	RuBillboardType							GetBillboardType() const;
	void									SetBillboardType(RuBillboardType billboardType);

	RuOrthogonalAxis						GetBillboardAxis() const;
	void									SetBillboardAxis(RuOrthogonalAxis orientation);

	RuAlphaMode								GetBlendType() const;
	void									SetBlendType(RuAlphaMode blendType);

	const char*								GetTextureName();
	void									SetTextureName(const char *textureName);

	INT32									GetSubTextureWidth() const;
	void									SetSubTextureWidth(INT32 width);

	INT32									GetSubTextureHeight() const;
	void									SetSubTextureHeight(INT32 height);

	REAL									GetFrameRate() const;
	void									SetFrameRate(REAL frameRate);

	INT32									GetNumFrames() const;
	void									SetNumFrames(INT32 numFrames);

	BOOL									GetOrientToDirectionOfTravel() const;
	void									SetOrientToDirectionOfTravel(BOOL orientToDirectionOfTravel);

	BOOL									GetRandomRotation() const;
	void									SetRandomRotation(BOOL randomRotation);

	const CRuVector3&						GetQuadOffset() const;
	void									SetQuadOffset(const CRuVector3 &quadOffset);

	const CRuVector3&						GetQuadScale() const;
	void									SetQuadScale(const CRuVector3 &quadScale);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_Particle_Visualizer_Sprite, CRuController_Particle_Visualizer_Sprite)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
