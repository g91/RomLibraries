/*!
	@file RuEntity_Widget_Flag.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITY_WIDGET_FLAG_H_
#define _RUENTITY_WIDGET_FLAG_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#include "../../Scene/Base/RuEntityContainer.h"

#include "../../Scene/Widget/RuEntity_Widget_Text.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	Flag visualizer widget.

	@version 2006.01.04
*/
class CRuEntity_Widget_Flag : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	REAL									m_widgetScale;
	CRuVector3								m_textWidgetOffset;
	CRuEntity_Widget_Text*					m_textWidget;

	CRuMesh*								m_flagMesh;
	CRuMaterial*							m_flagMaterial;
//	CRuModel*								m_flagModel;

	IRuMesh*								m_textMesh;
	IRuMaterial*							m_textMaterial;

	CRuModel*								m_flagModel;

public:
											CRuEntity_Widget_Flag();
	virtual									~CRuEntity_Widget_Flag();

	void									SetHighlight(BOOL enable);
	void									SetText(CRuFontType *fontType, REAL textScale, RUCOLOR textColor, const char *text);

	// Base interface
	virtual BOOL							Update(float elapsedTime);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
