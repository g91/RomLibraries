#ifndef _RUWORLD_CLOUDPLANE_H_
#define _RUWORLD_CLOUDPLANE_H_

#include "../../Scene/Terrain/RuWorld_Base.h"
#include "../../Core/Noise/RuNoise_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuWorld_CloudPlane : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32									m_numOctaves;
	INT32									m_baseOctaveSize;
	INT32*									m_octaveSize;
	REAL*									m_octaveScale;
	REAL*									m_octaveWeight;
	REAL*									m_octaveScrollSpeed;

	// Run-time stuff
	REAL*									m_octaveScrollMultiplier;

	CRuVector3								m_sunDirection;				//!< Sun direction, defined as the unit vector pointing towards the sun

	CRuMesh*								m_cloudPlaneMesh;
	CRuMaterial*							m_cloudPlaneMaterial;
	IRuTexture*								m_cloudTexture;

	CRuMesh*								m_cloudGenMesh;
	CRuMaterial*							m_cloudGenMaterial;
	CRuModel*								m_cloudGenModel;

	INT32									m_noiseTextureCount;
	CRuTexture_Proxy**						m_noiseTextures;
	REAL*									m_noiseTextureWeights;
	INT32*									m_noiseTextureSizes;

	REAL									m_vsConstants[32];
	REAL									m_psConstants[12];

public:
											CRuWorld_CloudPlane();
	virtual									~CRuWorld_CloudPlane();

	void									SetNumOctaves(INT32 numOctaves);
	void									SetBaseOctaveSize(INT32 baseOctaveSize);
	void									SetOctaveSize(INT32 *octaveSize);
	void									SetOctaveScale(REAL *octaveScale);
	void									SetOctaveWeight(REAL *octaveWeight);
	void									SetOctaveScrollSpeed(REAL *octaveScrollSpeed);

	void									SetCloudCover(REAL cloudCover);
	void									SetCloudSharpness(REAL cloudSharpness);
	void									SetCloudColor0(RUCOLOR cloudColor);
	void									SetCloudColor1(RUCOLOR cloudColor);

	BOOL									SetSunDirection(const CRuVector3 &sunDirection);

	// IRuObject serialization
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							Update(float elapsedTime);

	// IRuEntity_Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	// Event handlers
	BOOL									HandleEvent_Submitted(RuEventArgs *eventArgs);

protected:
	BOOL									CreateNoiseTextures(INT32 numOctaves, INT32 *octaveSize, REAL *octaveScale, REAL *octaveWeights, CRuNoise_Base *noiseGenerator);
	BOOL									ReleaseNoiseTextures();
	BOOL									ShiftNoiseTextures();

	BOOL									InitializeCloudPlaneMesh(REAL size, REAL apexHeight, REAL baseHeight, INT32 numSegments, RuFalloffFunction falloff);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
