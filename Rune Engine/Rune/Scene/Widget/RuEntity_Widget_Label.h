/*!
	@file RuEntity_Widget_Label.h

	Copyright (c) 2004-2008 All rights reserved
*/

#pragma once

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Label widget. Used for billboarded text in 3D space.

	@version 2008.08.05
*/
class CRuEntity_Widget_Label : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	enum RuLabelEffect
	{
		ruLABELEFFECT_NONE,
		ruLABELEFFECT_DROPSHADOW,
		ruLABELEFFECT_BORDER
	};

	enum RuGlyphJustify
	{
		ruGLYPHJUSTIFY_LEFT,
		ruGLYPHJUSTIFY_CENTER,
		ruGLYPHJUSTIFY_RIGHT
	};

protected:
	struct GlyphDescriptor
	{
		// Glyph properties
		REAL								m_glyphSize;									//!< Glyph size
		RUCOLOR								m_glyphColor;									//!< Glyph color modifier
		IRuTexture*							m_glyphTexture;									//!< Glyph texture
		CRuRectangle						m_glyphUV;										//!< Glyph UV
		RuGlyphJustify						m_glyphJustify;									//!< Glyph justify setting

		// Run-time
		REAL								m_glyphAR;										//!< Glyph aspect ratio (width-to-height)
		REAL								m_glyphWidth;									//!< Glyph width
		REAL								m_glyphHeight;									//!< Glyph height
	};

	struct RowDescriptor
	{
		CRuArrayList<GlyphDescriptor>		m_glyphs;										//!< Glyphs belonging to the row
	};

	struct MeshDescriptor
	{
		CRuDynamicMesh*						m_mesh;
		CRuMaterial*						m_material;

		INT32								m_iteration;
		BOOL								m_geometryDirty;
	};

	RuLabelEffect							m_labelEffect;									//!< Label special effect
	REAL									m_labelEffectSize;
	REAL									m_globalScale;
	REAL									m_refScaleDistance;								//!< Reference scaling distance

	BOOL									m_geometryDirty;
	CRuArrayList<RowDescriptor *>			m_rowDescriptors;
	CRuArrayList<MeshDescriptor>			m_meshes;

public:
											CRuEntity_Widget_Label();
	virtual									~CRuEntity_Widget_Label();

	// Label setup
	BOOL									SetLabelEffect(RuLabelEffect labelEffect);
	BOOL									SetLabelEffectSize(REAL labelEffectSize);
	BOOL									SetLabelScale(REAL labelScale);
	BOOL									SetScaleDistance(REAL scaleDistance);
	BOOL									InsertGlyph(INT32 row, INT32 numGlyphs, const CRuRectangle *glyphRects, REAL glyphScale, RUCOLOR glyphColor, IRuTexture *texture);
	BOOL									InsertGlyph(INT32 row, INT32 numGlyphs, const CRuRectangle *glyphRects, REAL glyphScale, RUCOLOR glyphColor, IRuTexture *texture, RuGlyphJustify justify);
	BOOL									ClearAllGlyphs();
	BOOL									ClearAllMeshes();
	INT32									GetRowCount();

	// Geometry building
	BOOL									BuildGeometry();

	// Base entity interface
	virtual CRuEntity*						Clone(CRuEntity *clonedEntity = NULL);

	// Base interface
	virtual BOOL							Update(float elapsedTime);

	// IRuEntity_Renderable interface
	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	virtual void							CalculateEntityBounds();

	// Event handlers
	BOOL									HandleEvent_Camera(RuEventArgs *eventArgs);

protected:
	INT32									GetBatchableGlyphCount(INT32 rowIndex, INT32 glyphIndex);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
