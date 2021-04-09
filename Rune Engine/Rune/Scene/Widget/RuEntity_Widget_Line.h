/*!
	@file RuEntity_Widget_LineArray.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUENTITY_WIDGET_LINE_H_
#define _RUENTITY_WIDGET_LINE_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#include "../../Scene/Base/RuEntityContainer.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	Flag visualizer widget.

	@version 2006.01.04
*/
class CRuEntity_Widget_Line: public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuMesh*								m_mesh;
	CRuMaterial*							m_material;

public:
											CRuEntity_Widget_Line();
	virtual									~CRuEntity_Widget_Line();

	void									SetLine(const CRuVector3 &v0, const CRuVector3 &v1, BOOL v0Arrow, BOOL v1Arrow, RUCOLOR color);

	// Base interface
	virtual BOOL							Update(float elapsedTime);

	// Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
