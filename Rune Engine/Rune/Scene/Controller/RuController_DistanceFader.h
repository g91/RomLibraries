#ifndef _RUCONTROLLER_DISTANCEFADER_H_
#define _RUCONTROLLER_DISTANCEFADER_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!

	@author John Tang
	@version 2005/11/04
*/
class CRuController_DistanceFader : public IRuEntity_Controller
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct TargetMaterialDesc
	{
		IRuMaterial*						m_targetMaterial;
		IRuShader*							m_oldShader;
	};

	CRuEntity*								m_targetEntity;									//!< Entity that the controller affects

	CRuArrayList<TargetMaterialDesc>		m_targetMaterials;								//!< List of affected materials

	CRuParameterBlock						m_parameterBlock;								//!< Parameter block

	// Settings
	IRuShader*								m_fadeShader;
	BOOL									m_isFadeOut;									//!< Is this a fade out controller?

public:
											CRuController_DistanceFader();
	virtual									~CRuController_DistanceFader();

	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *targetEntity);

	void									SetFadeType(BOOL isFadeOut);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
