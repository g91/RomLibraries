#ifndef _RUENTITYSKYDOME_H_
#define _RUENTITYSKYDOME_H_

// Rune entity base
#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityContainer.h"
#include "../../Scene/Base/RuEntityModel.h"

#include "../../Scene/Terrain/RuWorld_CloudPlane.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

template<class DataType>
class CRuValueInterpolator
{
protected:
	REAL									m_t;						//!< Interpolation t value, in [0, 1]
	REAL									m_tGradient;				//!< t value transition gradient, in units per second
	DataType								m_v0;						//!< Initial value
	DataType								m_v1;						//!< Final value

public:
											CRuValueInterpolator();
	virtual									~CRuValueInterpolator();

	void									Transition(DataType value, REAL tGradient);
	void									Update(REAL elapsedTime);

	// Assignment
	CRuValueInterpolator<DataType>&			operator=(const DataType value);

	// Casting operators
											operator DataType() const;
};

// ************************************************************************************************************************************************************

class CRuSkyDome : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	enum CloudTransitionMode
	{
		ctNONE,
		ctPENDING,
		ctEXECUTING
	};

	struct HorizonCloudDesc
	{
		REAL								m_age;
		REAL								m_fadeStartTime;
		REAL								m_theta;								//!< Position on the cloud ring
		REAL								m_thetaGradient;						//!< Theta gradient, in radians per second

		CRuMaterial*						m_material;
		CRuModel*							m_model;

		REAL								m_sizeT;
		REAL								m_sizeTGradient;

		INT32								m_typeIndex;							//!< Cloud type 0 (vertical axis on the texture map)
		INT32								m_sizeIndex0;							//!< Cloud "size" 0 (horizontal axis on the texture map)
		INT32								m_sizeIndex1;							//!< Cloud "size" 1 (horizontal axis on the texture map)
		REAL								m_vsConstants[8];
	};

	struct OverheadCloudDesc
	{
		REAL								m_age;
		REAL								m_fadeStartTime;
		REAL								m_omega;
		REAL								m_omegaGradient;
		REAL								m_rho;
		REAL								m_rhoGradient;
		REAL								m_theta;								//!< Position on the cloud ring
		REAL								m_thetaGradient;						//!< Theta gradient, in radians per second

		CRuMaterial*						m_material;
		CRuModel*							m_model;

		REAL								m_sizeT;
		REAL								m_sizeTGradient;

		INT32								m_typeIndex;							//!< Cloud type 0 (vertical axis on the texture map)
		INT32								m_sizeIndex0;							//!< Cloud "size" 0 (horizontal axis on the texture map)
		INT32								m_sizeIndex1;							//!< Cloud "size" 1 (horizontal axis on the texture map)
		REAL								m_vsConstants[8];
	};

	// Dome geometry settings
	INT32									m_horizontalSegments;
	INT32									m_verticalSegments;
	float									m_domeCenterY;

	float									m_domeGradientT;
	float									m_domeGradientTDelta;
	IRuTexture*								m_domeGradientTexture0;
	IRuTexture*								m_domeGradientTexture1;					//!< 

	CRuModel*								m_domeModel;							//!< Skydome model
	CRuMesh*								m_domeMesh;								//!< Dome mesh
	CRuMaterial*							m_domeMaterial;							//!< Dome material

	// Cloud plane and settings
	CRuWorld_CloudPlane*					m_cloudPlane;
	CRuValueInterpolator<REAL>				m_cloudCover;
	CRuValueInterpolator<REAL>				m_cloudSharpness;
	CRuValueInterpolator<CRuColor4>			m_cloudColor0;
	CRuValueInterpolator<CRuColor4>			m_cloudColor1;
	CRuValueInterpolator<REAL>				m_octaveWeight[8];
	CRuValueInterpolator<REAL>				m_octaveScrollSpeed[8];

	// Bounds
	CRuSphere								m_localBounds;							//!< Local space bounding sphere

public:
											CRuSkyDome();
	virtual									~CRuSkyDome();

	// IRuObject serialization
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Entity interface
	virtual BOOL							ResetStates();
	virtual BOOL							Update(float elapsedTime);

	// Event handlers
	BOOL									HandleEvent_Camera(RuEventArgs *eventArgs);

/*
	// Required subclass interface
	virtual void							UpdateEntityTransform();
	virtual void							UpdateEntityBounds();
*/
	BOOL									Initialize(INT32 horzSegments = 4, INT32 vertSegments = 10, float vertSweep = 90.0f, float radius = 3000.0f, float vertScale = 1.0f, float horzCompression = 2.0f, INT32 cloudPlaneSegments = 16);

	// stuff...
	void									SetCamera(CRuCamera *viewCamera);

	CRuWorld_CloudPlane*					GetCloudPlane();

	BOOL									IsInTransition() const;

	void									SetSkyDomeGradientTexture(IRuTexture *gradientTexture);
	void									TransitionSkyDomeGradientTexture(IRuTexture *gradientTexture, float transitionTime);

	void									SetCloudCover(REAL cloudCover);
	void									TransitionCloudCover(REAL cloudCover, float transitionTime);

	void									SetCloudSharpness(REAL cloudSharpness);
	void									TransitionCloudSharpness(REAL cloudSharpness, float transitionTime);

	void									SetCloudColor0(CRuColor4 cloudColor);
	void									TransitionCloudColor0(CRuColor4 cloudColor, float transitionTime);

	void									SetCloudColor1(CRuColor4 cloudColor);
	void									TransitionCloudColor1(CRuColor4 cloudColor, float transitionTime);

	void									SetOctaveWeight(REAL octaveWeight[8]);
	void									TransitionOctaveWeight(REAL octaveWeight[8], REAL transitionTime);

	void									SetOctaveScrollSpeed(REAL octaveScrollSpeed[8]);
	void									TransitionOctaveScrollSpeed(REAL octaveScrollSpeed[8], REAL transitionTime);
};

// ************************************************************************************************************************************************************

template<class DataType>
inline CRuValueInterpolator<DataType>::CRuValueInterpolator()
:	m_t(0.0f),
	m_tGradient(0.0f)
{
}

template<class DataType>
inline CRuValueInterpolator<DataType>::~CRuValueInterpolator()
{
}

template<class DataType>
inline void CRuValueInterpolator<DataType>::Transition(DataType value, REAL tGradient)
{
	if(m_tGradient > 0.0f)
	{
		m_v0 = m_v0 * (1.0f - m_t) + m_v1 * m_t;
	}

	// Setup new transition
	m_t = 0.0f;
	m_tGradient = tGradient;
	m_v1 = value;
}

template<class DataType>
inline void CRuValueInterpolator<DataType>::Update(REAL elapsedTime)
{
	if(m_tGradient > 0.0f)
	{
		m_t += m_tGradient * elapsedTime;

		if(m_t > 1.0f)
		{
			m_t = 0.0f;
			m_tGradient = 0.0f;
			m_v0 = m_v1;
		}
	}
}

template<class DataType>
inline CRuValueInterpolator<DataType> &CRuValueInterpolator<DataType>::operator=(const DataType value)
{
	m_t = 0.0f;
	m_tGradient = 0.0f;
	m_v0 = value;
	return *this;
}

template<class DataType>
inline CRuValueInterpolator<DataType>::operator DataType() const
{
	if(m_t == 0.0f)
	{
		return m_v0;
	}

	return m_v0 * (1.0f - m_t) + m_v1 * m_t;
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
