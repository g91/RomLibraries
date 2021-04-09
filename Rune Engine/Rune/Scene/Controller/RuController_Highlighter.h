#ifndef _RUCONTROLLER_HIGHLIGHTER_H_
#define _RUCONTROLLER_HIGHLIGHTER_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Highlighter controller. Used to produce a strobing highlight effect
	on entities.

	@author John Tang
	@version 2005/11/10
*/
class CRuController_Highlighter : public IRuEntity_Controller
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct MatDesc
	{
		CRuMatrix4x4*						m_transform;
		CRuMaterial*						m_material;
	};

	CRuHashMap<CRuGUID, MatDesc>*			m_materialHash;

	CRuEntity*								m_targetEntity;									//!< Entity that the controller affects

	BOOL									m_useCelHighlight;
	REAL									m_pulsePeriod;
	CRuColor4								m_highlightColor_0;
	CRuColor4								m_highlightColor_1;

	CRuStack<CRuEntity *>					m_workStack;

public:
											CRuController_Highlighter();
	virtual									~CRuController_Highlighter();

	void									UseCelHighlight(BOOL useCelHighlight);
	void									SetPulsePeriod(REAL pulsePeriod);
	void									SetHighlightColor0(const CRuColor4 &highlightColor0);
	void									SetHighlightColor1(const CRuColor4 &highlightColor1);

	REAL									GetPulsePeriod() const;
	const CRuColor4&						GetHighlightColor0() const;
	const CRuColor4&						GetHighlightColor1() const;

	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *targetEntity);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Submitted(RuEventArgs *eventArgs);

protected:
	void									ReleaseTarget();

	BOOL									ClearMaterialHashCallback(const void *key, void *data);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
