#include "../../Scene/Base/RuEntityController.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_Light, IRuEntity_Controller)
ruHEAP_DEFINE_SUBCLASS(CRuController_Light, "CRuController_Light", "IRuEntity_Controller")
ruCLASSGUID_DEFINE(CRuController_Light, 0x670D84AC, 0xD09E4729, 0xA2736B71, 0xE4476D2F)

// ************************************************************************************************************************************************************

CRuController_Light::CRuController_Light()
:	m_targetLight(NULL),
	m_intervalIndex(0),
	m_intervalT(0.0f),
	m_trackInterval(10)
{
}

CRuController_Light::~CRuController_Light()
{
}

BOOL CRuController_Light::Update(REAL elapsedTime)
{
	if(m_targetLight)
	{
		m_intervalT += elapsedTime;
		
		while(m_intervalT > m_trackInterval)
		{
			++m_intervalIndex;
			m_intervalT -= m_trackInterval;
		}

		if(m_intervalIndex >= m_positionTrack.Count())
		{
			m_intervalIndex = m_intervalIndex % m_positionTrack.Count();
		}

		REAL t = m_intervalT / m_trackInterval;

		// Interpolate using hermite spline
/*
		INT32 i0 = (m_intervalIndex - 1 + m_positionTrack.Count()) % m_positionTrack.Count();
		INT32 i1 = (m_intervalIndex + 0) % m_positionTrack.Count();
		INT32 i2 = (m_intervalIndex + 1) % m_positionTrack.Count();
		INT32 i3 = (m_intervalIndex + 2) % m_positionTrack.Count();
*/
		INT32 i0 = (m_intervalIndex + 0) % m_positionTrack.Count();
		INT32 i1 = (m_intervalIndex + 1) % m_positionTrack.Count();
		INT32 i2 = (m_intervalIndex + 2) % m_positionTrack.Count();

		CRuVector3 lightPosition;
		HermiteSpline(m_positionTrack[i0], m_positionTrack[i1], m_positionTrack[i2], t, lightPosition);
//		CatmullRomSpline(m_positionTrack[i0], m_positionTrack[i1], m_positionTrack[i2], m_positionTrack[i3], t, lightPosition);
		m_targetLight->SetPosition(lightPosition);
	}

	// Chain update call to base controller class
	return IRuEntity_Controller::Update(elapsedTime);
}

BOOL CRuController_Light::SetTargetLight(CRuEntity_Light *targetLight)
{
	m_targetLight = targetLight;

	return TRUE;
}

BOOL CRuController_Light::AppendPositionTrack(const CRuVector3 *positions, INT32 numPositions)
{
	for(INT32 i = 0; i < numPositions; ++i)
	{
		m_positionTrack.Add(positions[i]);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
