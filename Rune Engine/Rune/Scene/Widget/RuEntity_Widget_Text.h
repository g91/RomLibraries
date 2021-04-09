/*!
	@file RuEntity_Widget_Text.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITY_WIDGET_TEXT_H_
#define _RUENTITY_WIDGET_TEXT_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#include "../../Scene/Base/RuEntityContainer.h"

#include "../../UI/RuUI_Font.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

enum RuTextAlignment // blah.. .what the hell
{
	ruTEXTALIGNMENT_TOP_LEFT			= 1,
	ruTEXTALIGNMENT_TOP_CENTER			= 2,
	ruTEXTALIGNMENT_TOP_RIGHT			= 3,
	ruTEXTALIGNMENT_MIDDLE_LEFT			= 4,
	ruTEXTALIGNMENT_MIDDLE_CENTER		= 5,
	ruTEXTALIGNMENT_MIDDLE_RIGHT		= 6,
	ruTEXTALIGNMENT_BOTTOM_LEFT			= 7,
	ruTEXTALIGNMENT_BOTTOM_CENTER		= 8,
	ruTEXTALIGNMENT_BOTTOM_RIGHT		= 9
};

// ************************************************************************************************************************************************************

/*!
	Text widget.

	@version 2006.03.08
*/
class CRuEntity_Widget_Text : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32									m_numSegments;
	
	CRuFontType*							m_fontType;
	REAL									m_textScale;

	CRuDynamicMesh*							m_textMesh;
	CRuMaterial*							m_textMaterial;

public:
											CRuEntity_Widget_Text();
	virtual									~CRuEntity_Widget_Text();

	void									SetText(CRuFontType *fontType, REAL textScale, RuTextAlignment textAlignment, RUCOLOR textColor, const char *text);

	// Base interface
	virtual BOOL							Update(float elapsedTime);

	// Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
