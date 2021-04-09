/*!
	@file RuEntity_Widget_Label.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../../Scene/Widget/RuEntity_Widget_Label.h"

#include "../../Engine/Geometry/RuPrimitiveToolkit.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Widget_Label, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Widget_Label, "CRuEntity_Widget_Label", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuEntity_Widget_Label, 0xA022AD46, 0x6BFE444e, 0xA0648907, 0xBD007589)

// ************************************************************************************************************************************************************

CRuEntity_Widget_Label::CRuEntity_Widget_Label()
:	m_labelEffect(ruLABELEFFECT_NONE),
	m_labelEffectSize(2.0f),
	m_globalScale(1.0f / 1024.0f),
	m_refScaleDistance(2000.0f)
{
	// Setup event handlers
	Event_Camera().RegisterHandler(this->GetGUID(), boost::bind(&CRuEntity_Widget_Label::HandleEvent_Camera, this, _1));
}

CRuEntity_Widget_Label::~CRuEntity_Widget_Label()
{
	// Clear all glyphs
	ClearAllGlyphs();

	// Delete all instantiated geometry and material
	for(INT32 i = 0; i < m_meshes.Count(); ++i)
	{
		ruSAFE_RELEASE(m_meshes[i].m_mesh);
		ruSAFE_RELEASE(m_meshes[i].m_material);
	}

	m_meshes.Clear();
}

BOOL CRuEntity_Widget_Label::SetLabelEffect(RuLabelEffect labelEffect)
{
	// Store label effect
	m_labelEffect = labelEffect;

	// Mark geometry as dirty
	m_geometryDirty = TRUE;

	return TRUE;
}

BOOL CRuEntity_Widget_Label::SetLabelEffectSize(REAL labelEffectSize)
{
	// Store label effect size
	m_labelEffectSize = labelEffectSize;

	// Mark geometry as dirty
	m_geometryDirty = TRUE;

	return TRUE;
}

BOOL CRuEntity_Widget_Label::SetLabelScale(REAL labelScale)
{
	// Store label scale
	m_globalScale = labelScale;

	// Mark geometry as dirty
	m_geometryDirty = TRUE;

	return TRUE;
}

BOOL CRuEntity_Widget_Label::SetScaleDistance(REAL scaleDistance)
{
	m_refScaleDistance = scaleDistance;
	return TRUE;
}

BOOL CRuEntity_Widget_Label::InsertGlyph(INT32 row, INT32 numGlyphs, const CRuRectangle *glyphRects, REAL glyphScale, RUCOLOR glyphColor, IRuTexture *texture)
{
	return InsertGlyph(row, numGlyphs, glyphRects, glyphScale, glyphColor, texture, ruGLYPHJUSTIFY_CENTER);
}

BOOL CRuEntity_Widget_Label::InsertGlyph(INT32 row, INT32 numGlyphs, const CRuRectangle *glyphRects, REAL glyphScale, RUCOLOR glyphColor, IRuTexture *texture, RuGlyphJustify justify)
{
	// Abort if no valid texture was specified
	if(texture == NULL)
	{
		return FALSE;
	}

	// Make sure number of row descriptors is at least the same as the row specified
	while(m_rowDescriptors.Count() < (row + 1))
	{
		m_rowDescriptors.Add(ruNEW RowDescriptor());
	}

	// Short-hand to the specified row
	RowDescriptor *rowDesc = m_rowDescriptors[row];

	// Insert glyphs to the specified row
	for(INT32 i = 0; i < numGlyphs; ++i)
	{
		GlyphDescriptor glyphDesc;

		// Set glyph properties
		glyphDesc.m_glyphSize = glyphScale;
		glyphDesc.m_glyphColor = glyphColor;
		glyphDesc.m_glyphTexture = texture;
		glyphDesc.m_glyphUV = glyphRects[i];
		glyphDesc.m_glyphJustify = justify;

		// Calculate glyph's width-to-height aspect ratio
		glyphDesc.m_glyphAR = (glyphDesc.m_glyphUV.m_right - glyphDesc.m_glyphUV.m_left) / (glyphDesc.m_glyphUV.m_bottom - glyphDesc.m_glyphUV.m_top);

		// Glyph height is specified by glyph size
		glyphDesc.m_glyphHeight = glyphDesc.m_glyphSize;

		// Glyph width is calculated from the glyph height and the aspect ratio
		glyphDesc.m_glyphWidth = glyphDesc.m_glyphAR * glyphDesc.m_glyphSize;

		// Increment reference count on the texture
		texture->AddRef();

		// Insert glyph into glyph array
		rowDesc->m_glyphs.Add(glyphDesc);
	}

	// Mark geometry as dirty
	m_geometryDirty = TRUE;

	return TRUE;
}

BOOL CRuEntity_Widget_Label::ClearAllGlyphs()
{
	// Iterate through all rows
	for(INT32 i = 0; i < m_rowDescriptors.Count(); ++i)
	{
		// Short-hand to the current row
		RowDescriptor *rowDesc = m_rowDescriptors[i];

		// Release resources held by each glyph
		for(INT32 j = 0; j < rowDesc->m_glyphs.Count(); ++j)
		{
			ruSAFE_RELEASE(rowDesc->m_glyphs[j].m_glyphTexture);
		}

		// Clear glyphs
		rowDesc->m_glyphs.Clear();

		// Delete the row descriptor
		delete rowDesc;
	}

	// CLear row descriptors array
	m_rowDescriptors.Clear();

	// Mark geometry as dirty
	m_geometryDirty = TRUE;

	return TRUE;
}

BOOL CRuEntity_Widget_Label::ClearAllMeshes()
{
	// Delete all instantiated geometry and material
	for(INT32 i = 0; i < m_meshes.Count(); ++i)
	{
		ruSAFE_RELEASE(m_meshes[i].m_mesh);
		ruSAFE_RELEASE(m_meshes[i].m_material);
	}

	m_meshes.Clear();

	// Mark geometry as dirty
	m_geometryDirty = TRUE;

	return TRUE;
}

INT32 CRuEntity_Widget_Label::GetRowCount()
{
	return m_rowDescriptors.Count();
}

BOOL CRuEntity_Widget_Label::BuildGeometry()
{
	if(m_geometryDirty == FALSE)
	{
		return TRUE;
	}

	// Clear previously instantiated meshes
	ClearAllMeshes();

	REAL yOffset = 0.0f;

	// Iterate through each row of text to find the total height of the label
	for(INT32 i = 0; i < m_rowDescriptors.Count(); ++i)
	{
		// Short-hand to the current glyph array
		CRuArrayList<GlyphDescriptor> &glyphs = m_rowDescriptors[i]->m_glyphs;

		// Determine row width and height in pixel space
		REAL rowWidth = 0.0f;
		REAL rowHeight = 0.0f;

		for(INT32 j = 0; j < glyphs.Count(); ++j)
		{
			REAL glyphWidth = glyphs[j].m_glyphWidth * m_globalScale;
			REAL glyphHeight = glyphs[j].m_glyphHeight * m_globalScale;

			// Accumulate row width and take maxima for row height
			rowWidth += glyphWidth;
			rowHeight = max(rowHeight, glyphHeight);
		}

		yOffset += rowHeight;
	}

	IRuTexture *curTexture = NULL;
	CRuDynamicMesh *curMesh = NULL;
	CRuMaterial *curMaterial = NULL;

	INT32 vertexCount = 0;
	INT32 uvCount = 0;
	INT32 indexCount = 0;
	CRuVector3 *position = NULL;
	DWORD *diffuse = NULL;
	REAL *uv = NULL;
	UINT16 *indices = NULL;

	UINT32 texWidth = 0;
	UINT32 texHeight = 0;

	for(INT32 i = 0; i < m_rowDescriptors.Count(); ++i)
	{
		// Short-hand to the current glyph array
		CRuArrayList<GlyphDescriptor> &glyphs = m_rowDescriptors[i]->m_glyphs;

		// Re-arrange glyph rendering order based on justification
		for(INT32 j = 0, k = 0, maxGlyphs = glyphs.Count(); k < maxGlyphs; ++j, ++k)
		{
			switch(glyphs[j].m_glyphJustify)
			{
				case ruGLYPHJUSTIFY_LEFT:
					{
						// Copy glyph to beginning
						GlyphDescriptor glyph = glyphs[j];
						glyphs.Insert(0, glyph);

						// Remove old glyph
						++j;
						glyphs.RemoveAt(j);
						--j;
					}
					break;

				case ruGLYPHJUSTIFY_CENTER:
					// Do nothing
					break;

				case ruGLYPHJUSTIFY_RIGHT:
					{
						// Copy glyph to end
						GlyphDescriptor glyph = glyphs[j];
						glyphs.Add(glyph);

						// Remove old glyph
						glyphs.RemoveAt(j);
						--j;
					}
					break;
			}
		}
	}

	// Generate geometry for each row
	for(INT32 i = 0; i < m_rowDescriptors.Count(); ++i)
	{
		// Short-hand to the current glyph array
		CRuArrayList<GlyphDescriptor> &glyphs = m_rowDescriptors[i]->m_glyphs;

		// Determine row width and height in pixel space
		REAL rowWidth = 0.0f;
		REAL rowHeight = 0.0f;

		for(INT32 j = 0; j < glyphs.Count(); ++j)
		{
			REAL glyphWidth = glyphs[j].m_glyphWidth * m_globalScale;
			REAL glyphHeight = glyphs[j].m_glyphHeight * m_globalScale;

			// Accumulate row width and take maxima for row height
			rowWidth += glyphWidth;
			rowHeight = max(rowHeight, glyphHeight);
		}

		// Calculate row origin
		CRuVector3 rowOrigin(-rowWidth / 2.0f, yOffset, 0.0f);
		yOffset -= rowHeight;

		for(INT32 j = 0; j < glyphs.Count(); ++j)
		{
			// If the glyph is using a different texture, begin a new mesh
			if(glyphs[j].m_glyphTexture != curTexture)
			{
				// Terminate previous batch
				if(curMesh && curMaterial)
				{
					MeshDescriptor meshDesc = { curMesh, curMaterial, 0, FALSE };
					meshDesc.m_mesh->CalculateBounds();
					m_meshes.Add(meshDesc);
				}

				// Store current texture
				curTexture = glyphs[j].m_glyphTexture;

				// Get number of glyphs in this batch
				INT32 batchableGlyphs = GetBatchableGlyphCount(i, j);

				// Allocate new dynamic mesh
				curMesh = ruNEW CRuDynamicMesh();
				curMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, batchableGlyphs * 4, batchableGlyphs * 2, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE);
				curMesh->AddTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
				curMesh->SetNumVertices(batchableGlyphs * 4);
				curMesh->SetNumPrimitives(batchableGlyphs * 2);

				// Retrieve morph target from the dynamic mesh and get its interior pointers
				IRuMorphTarget *morphTarget = static_cast<IRuMorphTarget *>(curMesh->GetMorphTarget(0));

				vertexCount = 0;
				uvCount = 0;
				indexCount = 0;
				position = morphTarget->GetPosition();
				diffuse = morphTarget->GetDiffuse();
				uv = morphTarget->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
				indices = curMesh->GetIndices();

				// Allocate new material
				curMaterial = ruNEW CRuMaterial();
				curMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_WIDGET_LABEL"));
				curMaterial->SetTexture(ruTEXCHANNEL_DIFFUSEMAP, curTexture);
				curMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 10);

				// If the texture has no alpha channel, mark it
				RuSurfaceFormat texFormat = curTexture->GetSurfaceFormat();
				if(texFormat == ruSURFACEFMT_L8)
				{
					curMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_CUSTOM_1, 0);
				}
				else
				{
					curMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_CUSTOM_1, 1);
				}

				// Retrieve texture dimensions
				texWidth = curTexture->GetLevelWidth(0);
				texHeight = curTexture->GetLevelHeight(0);
			}

			// Generate geometry for the current glyph
			INT32 vertexOffset = vertexCount;

			// Standard colored glyph
			REAL glyphWidth = glyphs[j].m_glyphWidth * m_globalScale;
			REAL glyphHeight = glyphs[j].m_glyphHeight * m_globalScale;

			position[vertexCount].Set(rowOrigin.x, rowOrigin.y + glyphHeight, 0.0f);
			diffuse[vertexCount++] = glyphs[j].m_glyphColor;
			uv[uvCount++] = glyphs[j].m_glyphUV.m_left / texWidth;
			uv[uvCount++] = glyphs[j].m_glyphUV.m_top / texHeight;

			position[vertexCount].Set(rowOrigin.x + glyphWidth, rowOrigin.y + glyphHeight, 0.0f);
			diffuse[vertexCount++] = glyphs[j].m_glyphColor;
			uv[uvCount++] = glyphs[j].m_glyphUV.m_right / texWidth;
			uv[uvCount++] = glyphs[j].m_glyphUV.m_top / texHeight;

			position[vertexCount].Set(rowOrigin.x, rowOrigin.y, 0.0f);
			diffuse[vertexCount++] = glyphs[j].m_glyphColor;
			uv[uvCount++] = glyphs[j].m_glyphUV.m_left / texWidth;
			uv[uvCount++] = glyphs[j].m_glyphUV.m_bottom / texHeight;

			position[vertexCount].Set(rowOrigin.x + glyphWidth,	rowOrigin.y, 0.0f);
			diffuse[vertexCount++] = glyphs[j].m_glyphColor;
			uv[uvCount++] = glyphs[j].m_glyphUV.m_right / texWidth;
			uv[uvCount++] = glyphs[j].m_glyphUV.m_bottom / texHeight;

			indices[indexCount++] = vertexOffset;
			indices[indexCount++] = vertexOffset + 1;
			indices[indexCount++] = vertexOffset + 2;

			indices[indexCount++] = vertexOffset + 1;
			indices[indexCount++] = vertexOffset + 3;
			indices[indexCount++] = vertexOffset + 2;

			rowOrigin.x += glyphWidth;
		}
	}

	if(curMesh && curMaterial)
	{
		MeshDescriptor meshDesc = { curMesh, curMaterial, 0, FALSE };
		meshDesc.m_mesh->CalculateBounds();
		m_meshes.Add(meshDesc);
	}

	// Glyph shadow processing
	CRuArrayList<MeshDescriptor> shadowMeshes;

	for(INT32 i = 0; i < m_meshes.Count(); ++i)
	{
		switch(m_labelEffect)
		{
			case ruLABELEFFECT_DROPSHADOW:
				{
					CRuDynamicMesh *dropShadowMesh = static_cast<CRuDynamicMesh *>(m_meshes[i].m_mesh->Clone());
					CRuMaterial *dropShadowMaterial = static_cast<CRuMaterial *>(m_meshes[i].m_material->Clone());

					dropShadowMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ZWRITEMODE, ruZWRITEMODE_DISABLE);

					CRuVector3 *position = dropShadowMesh->GetMorphTarget(0)->GetPosition();
					CRuVector3 dropOffset(m_labelEffectSize, -m_labelEffectSize, 0.0f);
					dropOffset = dropOffset * m_globalScale;

					for(INT32 j = 0, vertexCount = dropShadowMesh->GetNumVertices(); j < vertexCount; ++j)
					{
						position[j] = position[j] + dropOffset;
					}

					DWORD *diffuse = dropShadowMesh->GetMorphTarget(0)->GetDiffuse();

					for(INT32 j = 0, vertexCount = dropShadowMesh->GetNumVertices(); j < vertexCount; ++j)
					{
						diffuse[j] = 0xFF000000;
					}

					// Add to the list of shadow meshes
					MeshDescriptor meshDesc = { dropShadowMesh, dropShadowMaterial, 0, FALSE };
					shadowMeshes.Add(meshDesc);
				}

				break;

			case ruLABELEFFECT_BORDER:
				{
					CRuVector3 borderOffset[4] = { CRuVector3(m_labelEffectSize, 0.0f, 0.0f), CRuVector3(-m_labelEffectSize, 0.0f, 0.0f), CRuVector3(0.0f, m_labelEffectSize, 0.0f), CRuVector3(0.0f, -m_labelEffectSize, 0.0f) };

					for(INT32 k = 0; k < 4; ++k)
					{
						borderOffset[k] = borderOffset[k] * m_globalScale;
					}

					INT32 shadowMeshVertices = m_meshes[i].m_mesh->GetNumVertices() * 4;
					INT32 shadowMeshPrimitives = m_meshes[i].m_mesh->GetNumPrimitives() * 4;

					CRuDynamicMesh *borderShadowMesh = ruNEW CRuDynamicMesh();
					borderShadowMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, shadowMeshVertices, shadowMeshPrimitives, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE);
					borderShadowMesh->AddTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
					borderShadowMesh->SetNumVertices(shadowMeshVertices);
					borderShadowMesh->SetNumPrimitives(shadowMeshPrimitives);

					CRuMaterial *borderShadowMaterial = static_cast<CRuMaterial *>(m_meshes[i].m_material->Clone());
					borderShadowMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ZWRITEMODE, ruZWRITEMODE_DISABLE);

					IRuMorphTarget *srcMorphTarget = m_meshes[i].m_mesh->GetMorphTarget(0);
					CRuVector3 *srcPosition = srcMorphTarget->GetPosition();
					DWORD *srcDiffuse = srcMorphTarget->GetDiffuse();
					REAL *srcUV = srcMorphTarget->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);

					IRuMorphTarget *dstMorphTarget = borderShadowMesh->GetMorphTarget(0);
					CRuVector3 *dstPosition = dstMorphTarget->GetPosition();
					DWORD *dstDiffuse = dstMorphTarget->GetDiffuse();
					REAL *dstUV = dstMorphTarget->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
					UINT16 *dstIndices = borderShadowMesh->GetIndices();

					INT32 dstVertexCount = 0;
					INT32 dstUVCount = 0;
					INT32 dstIndexCount = 0;

					for(INT32 k = 0, numPrimitives = m_meshes[i].m_mesh->GetNumPrimitives() / 2; k < numPrimitives; ++k)
					{
						for(INT32 m = 0; m < 4; ++m)
						{
							INT32 vertexOffset = dstVertexCount;

							// Modify vertices
							for(INT32 n = 0; n < 4; ++n)
							{
								dstPosition[dstVertexCount] = srcPosition[k * 4 + n] + borderOffset[m];
								dstDiffuse[dstVertexCount++] = 0xFF000000;
								dstUV[dstUVCount++] = srcUV[(k * 4 + n) * 2];
								dstUV[dstUVCount++] = srcUV[(k * 4 + n) * 2 + 1];
							}

							// Setup primitive for quad
							dstIndices[dstIndexCount++] = vertexOffset;
							dstIndices[dstIndexCount++] = vertexOffset + 1;
							dstIndices[dstIndexCount++] = vertexOffset + 2;

							dstIndices[dstIndexCount++] = vertexOffset + 1;
							dstIndices[dstIndexCount++] = vertexOffset + 3;
							dstIndices[dstIndexCount++] = vertexOffset + 2;
						}
					}

					// Add to the list of shadow meshes
					MeshDescriptor meshDesc = { borderShadowMesh, borderShadowMaterial, 0, FALSE };
					shadowMeshes.Add(meshDesc);
				}

				break;
		}
	}

	for(INT32 i = 0; i < shadowMeshes.Count(); ++i)
	{
		m_meshes.Insert(i, shadowMeshes[i]);
	}

	// Mark geometry as not dirty
	m_geometryDirty = FALSE;

	return TRUE;
}

CRuEntity *CRuEntity_Widget_Label::Clone(CRuEntity *clonedEntity)
{
	// No cloning is allowed
	return NULL;
}

BOOL CRuEntity_Widget_Label::Update(float elapsedTime)
{
	// Build widget geometry
	BuildGeometry();

	// Chain update call to base class
	IRuEntity_Renderable::Update(elapsedTime);

	return TRUE;
}

INT32 CRuEntity_Widget_Label::GetNumMeshes() const
{
	// Return number of meshes in the array
	return m_meshes.Count();
}

BOOL CRuEntity_Widget_Label::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	// Return the mesh & material specified by the mesh index
	*meshOut = m_meshes[meshIndex].m_mesh;
	*materialOut  = m_meshes[meshIndex].m_material;

	return TRUE;
}

void CRuEntity_Widget_Label::CalculateEntityBounds()
{
	CRuSphere localBounds;
	const CRuMatrix4x4 &worldTransform = this->GetWorldTransform();

	if(m_meshes.Count() > 0)
	{
		localBounds = m_meshes[0].m_mesh->GetBounds();
		m_meshes[0].m_iteration = m_meshes[0].m_mesh->GetMeshIteration();

		for(INT32 i = 1; i < m_meshes.Count(); ++i)
		{
			localBounds.Merge(m_meshes[i].m_mesh->GetBounds());
			m_meshes[i].m_iteration = m_meshes[0].m_mesh->GetMeshIteration();
		}
	}
	else
	{
		// No meshes, so set the mesh's bounds to be at the origin of local space
		localBounds.Set(CRuVector3(0.0f, 0.0f, 0.0f), 0.0f);
	}

	// Set local entity bounds
	this->SetLocalEntityBounds(localBounds);
}

BOOL CRuEntity_Widget_Label::HandleEvent_Camera(RuEventArgs *eventArgs)
{
	RuEntityEvent_Camera_Args *cameraArgs = static_cast<RuEntityEvent_Camera_Args *>(eventArgs);

	const CRuMatrix4x4 &viewTransform = cameraArgs->m_camera->GetCameraTransform();
	const CRuMatrix4x4 &projTransform = cameraArgs->m_camera->GetProjectionTransform();

	CRuVector3 pt0 = this->GetWorldBounds().Center();
	viewTransform.TransformPoint(pt0);

	REAL z = pt0.m_z;
	REAL distT = 1.0f - max(0.0f, min(1.0f, (z / (m_refScaleDistance * 0.25f))));

	CRuVector3 pt0a = pt0 + CRuVector3(1.0f, 0.0f, 0.0);
	
	projTransform.TransformPointProjected(pt0);
	projTransform.TransformPointProjected(pt0a);

	REAL len0 = (pt0a - pt0).Magnitude();
	REAL invScale = 1.0f / len0;

	invScale *= distT;

	this->GetParameterBlock()->SetParameter(ruPARAMTYPE_CUSTOM_0, RuCastFloatToINT64(invScale));

	return TRUE;
}

INT32 CRuEntity_Widget_Label::GetBatchableGlyphCount(INT32 rowIndex, INT32 glyphIndex)
{
	GlyphDescriptor baseGlyph = m_rowDescriptors[rowIndex]->m_glyphs[glyphIndex];

	INT32 glyphCount = 0;

	for(INT32 i = rowIndex, startGlyphIdx = glyphIndex; i < m_rowDescriptors.Count(); ++i)
	{
		// Short-hand to the current glyph array
		CRuArrayList<GlyphDescriptor> &glyphs = m_rowDescriptors[i]->m_glyphs;

		// Iterate through glyphs in the glyph array
		for(INT32 j = startGlyphIdx; j < glyphs.Count(); ++j, ++glyphCount)
		{
			// If the glyph is using a different texture than the base glyph, terminate and return current glyph count
			if(glyphs[j].m_glyphTexture != baseGlyph.m_glyphTexture)
			{
				return glyphCount;
			}
		}

		// Set starting glyph index to zero
		startGlyphIdx = 0;
	}

	return glyphCount;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
