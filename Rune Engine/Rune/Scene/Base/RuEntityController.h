#ifndef _RUENTITYCONTROLLER_H_
#define _RUENTITYCONTROLLER_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityContainer.h"
#include "../../Scene/Base/RuEntityModel.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuController_Light : public IRuEntity_Controller
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuEntity_Light*						m_targetLight;									// Target light

	INT32									m_intervalIndex;								// Currently playing interval
	REAL									m_intervalT;									// Current interval time [0, 1]

	REAL									m_trackInterval;								// Time interval between track samples, in seconds
	CRuArrayList<CRuVector3>				m_positionTrack;								// Position track samples

public:
											CRuController_Light();
	virtual									~CRuController_Light();

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	BOOL									SetTargetLight(CRuEntity_Light *targetLight);

	BOOL									AppendPositionTrack(const CRuVector3 *positions, INT32 numPositions);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
