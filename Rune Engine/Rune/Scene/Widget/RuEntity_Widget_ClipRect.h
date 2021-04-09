/*!
	@file RuEntity_Widget_ClipRect.h

	Copyright (c) 2004-2008 All rights reserved
*/

#pragma once

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Clip rect widget. Used to clip a box against a mesh, with the resulting
	intersections displayed as a line mesh.

	@version 2008.09.16
*/
class CRuEntity_Widget_ClipRect : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	enum MeshDisplayMode
	{
		ruMESHDISPLAYMODE_LINE				= 0,
		ruMESHDISPLAYMODE_POLYLINE			= 1
	};

protected:
	IRuObject*								m_targetObject;
	CRuOBB									m_clipBox;
	REAL									m_gridInterval;
	MeshDisplayMode							m_meshDisplayMode;
	REAL									m_polyLineWidth;
	RUCOLOR									m_lineColor;

	BOOL									m_dirty;
	CRuArrayList<CRuVector3>				m_meshVertices;
	CRuDynamicMesh*							m_clipRectMesh;
	CRuMaterial*							m_clipRectMaterial;

public:
											CRuEntity_Widget_ClipRect();
	virtual									~CRuEntity_Widget_ClipRect();

	BOOL									SetTargetObject(IRuObject *targetObject);
	BOOL									SetClipBox(const CRuOBB &clipBox);
	BOOL									SetGridInterval(REAL interval);
	BOOL									SetMeshDisplayMode(MeshDisplayMode meshDisplayMode);
	BOOL									SetLineColor(RUCOLOR color);
	BOOL									MarkMeshAsDirty();

	// Base interface
	virtual BOOL							Update(float elapsedTime);

	// Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

protected:
	void									BuildMeshSegments();
	void									BuildLineMesh();
	void									BuildPolyLineMesh();
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
