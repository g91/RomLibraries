#ifndef _RUENTITY_ENVIRONMENT_H_
#define _RUENTITY_ENVIRONMENT_H_

// Rune entity base
//#include "../../Gaia/RuGaia_Base.h"
#include "../../Scene/Container/RuEntityAABBTree.h"
#include "../../Scene/Terrain/RuWorld_Base.h"
#include "../../Scene/Terrain/RuEntitySkyDome.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuEntity_Environment : public CRuEntity_Container_Impl
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32									m_iteration;										//!< Current light update iteration
	INT32									m_lastApplyIteration;								//!< Iteration during which light data application was done

	CRuCamera*								m_camera;											//!< Active camera

	CRuWorld_ZoneDescriptor*				m_activeZoneDesc;									//!< Currently active zone descriptor

	float									m_zoneSettingsDefaultTransitionDuration;
	float									m_zoneSettingsTransitionT;							//!< Current transition time
	float									m_zoneSettingsTransitionDuration;					//!< Transition duration, 0 if none
	CRuWorld_ZoneSettings*					m_activeZoneSettings;								//!< Currently active zone settings
	CRuWorld_ZoneSettings*					m_queuedZoneSettings;								//!< Zone setting queued for transition at the completion of the current transition
	CRuWorld_ZoneSettings*					m_zoneSettings0;									//!< Currently active zone descriptor
	CRuWorld_ZoneSettings*					m_zoneSettings1;									//!< Zone descriptor to transition to, NULL if none

	BOOL									m_fogOverride_Enable;
	RUCOLOR									m_fogOverride_Color;
	REAL									m_fogOverride_Start;
	REAL									m_fogOverride_End;
	REAL									m_fogOverride_Constant;

	// Environment lighting
	CRuFrame*								m_lightFrame;
	CRuEntity_Light*						m_ambientLight;										//!< Ambient light
	CRuEntity_Light*						m_envLight0;										//!< Primary directional light
	CRuEntity_Light*						m_envLight1;										//!< Secondary directional light 1
	CRuEntity_Light*						m_envLight2;										//!< Secondary directional light 2

	// Sky dome
	CRuSkyDome*								m_skyDome;											//!< Sky dome object
	IRuTexture*								m_sky_Gradient;										//!< Current sky gradient texture
	IRuTexture*								m_sky_Horizon;										//!< Current horizon cloud texture
	IRuTexture*								m_sky_Overhead;										//!< Current overhead cloud texture

	CRuEntity*								m_customSkyBox0;									//!< Custom sky box entity
	CRuEntity*								m_customSkyBox1;									//!< Custom sky box entity

public:
											CRuEntity_Environment();
	virtual									~CRuEntity_Environment();

	// IRuObject serialization
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Entity interface
	virtual BOOL							ResetStates();
	virtual BOOL							Update(float elapsedTime);

	BOOL									Initialize(INT32 horzSegments = 4, INT32 vertSegments = 10, float vertSweep = 90.0f, float radius = 3000.0f, float vertScale = 1.0f, float horzCompression = 2.0f, INT32 cloudPlaneSegments = 16);

	void									SetCamera(CRuCamera *viewCamera);

	// Environmental controls
	BOOL									SetZoneSettings(CRuWorld_ZoneSettings *zoneSettings);
	BOOL									TransitionZoneSettings(CRuWorld_ZoneSettings *zoneSettings, float transitionDuration);

	void									SelectZoneDescriptor(CRuWorld_ZoneDescriptor *zoneDesc, INT32 timeGroup, INT32 weatherGroup);
	void									SelectZoneDescriptor(CRuWorld_ZoneDescriptor *zoneDesc, CRuWorld_ZoneSettings *zoneSettings);

	void									EnableFogOverride(BOOL enable);
	void									SetFogOverrideParameters(RUCOLOR fogColor, REAL fogStart, REAL fogEnd, REAL fogConstant);

	INT32									IncrementIteration();

	// Queries
	CRuWorld_ZoneDescriptor*				GetCurrentZoneDescriptor();
	CRuWorld_ZoneSettings*					GetCurrentZoneSettings();
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
