/*!
	@file RuEntity_Widget_Flag.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Scene/Widget/RuEntity_Widget_Flag.h"

#include "../../Engine/Geometry/RuPrimitiveToolkit.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Widget_Flag, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Widget_Flag, "CRuEntity_Widget_Flag", "CRuEntity")
ruCLASSGUID_DEFINE(CRuEntity_Widget_Flag, 0xBA7CE18E, 0x6712422d, 0x80317337, 0x04DDB14A)

// ************************************************************************************************************************************************************

CRuEntity_Widget_Flag::CRuEntity_Widget_Flag()
:	m_widgetScale(1.0f),
	m_textWidget(NULL),
	m_textMesh(NULL),
	m_textMaterial(NULL),
	m_flagModel(NULL)
{
	INT32 vertexCount = CRuPrimitiveToolkit::FlagWidget_VertexCount();
	INT32 primitiveCount = CRuPrimitiveToolkit::FlagWidget_PrimitiveCount();

	// Create new sphere mesh
	m_flagMesh = ruNEW CRuMesh();
	m_flagMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, vertexCount, primitiveCount, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);

	// Create new sphere material
	m_flagMaterial = ruNEW CRuMaterial();
	m_flagMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_EDITOR_TRANSLUCENT"));
	m_flagMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, FALSE);
	// Set sphere material color
	m_flagMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, 0xFF00FF00);

	// Set sphere scale and location
	CRuVector3 *position = static_cast<CRuMorphTarget *>(m_flagMesh->GetMorphTarget(0))->GetPosition();
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_flagMesh->GetMorphTarget(0))->GetDiffuse();
	UINT16 *indices = m_flagMesh->GetIndices();

	REAL scale = 3.0f;
	INT32 vertexOffset = 0;
	INT32 indexOffset = 0;
	CRuPrimitiveToolkit::FlagWidget_Build(0.4f * scale, 0.1f * scale, 0.9f * scale, 1.0f * scale, 0.1f * scale, vertexOffset, indexOffset, position, indices);

	for(INT32 i = 0; i < vertexCount; ++i)
	{
		diffuse[i] = 0x6FFFFFFF;
	}

	m_textWidgetOffset.Set(0.4f * scale, 0.9f * scale, 0.0f);

	// Update mesh iteration and bounds
	m_flagMesh->CalculateBounds();

	// Create flag model
	m_flagModel = ruNEW CRuModel();
	m_flagModel->AddSubModel(m_flagMesh, m_flagMaterial);
	this->AddChild(m_flagModel);

	m_flagModel->SetOption(ruENTITYOPTION_BILLBOARD, ruBILLBOARDTYPE_UNCONSTRAINED);
}

CRuEntity_Widget_Flag::~CRuEntity_Widget_Flag()
{
	ruSAFE_RELEASE(m_textWidget);

	ruSAFE_RELEASE(m_flagMesh);
	ruSAFE_RELEASE(m_flagMaterial);

	ruSAFE_RELEASE(m_flagModel);
}

void CRuEntity_Widget_Flag::SetHighlight(BOOL enable)
{
	if(enable)
	{
		m_flagMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, 0xFFFFFFFF);
	}
	else
	{
		m_flagMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, 0xFF00FF00);
	}
}

void CRuEntity_Widget_Flag::SetText(CRuFontType *fontType, REAL textScale, RUCOLOR textColor, const char *text)
{
	if(fontType)
	{
		// Create text widget if it does not exist
		if(m_textWidget == NULL)
		{
			m_textWidget = ruNEW CRuEntity_Widget_Text();
		}

		// Set text and retrieve its mesh/material pair
		m_textWidget->SetText(fontType, textScale, ruTEXTALIGNMENT_BOTTOM_LEFT, textColor, text);
		m_textWidget->GetMesh(0, &m_textMesh, &m_textMaterial);

		// Offset the text mesh
		IRuMorphTarget *textMeshMT = m_textMesh->GetMorphTarget(0);
		if(textMeshMT->GetType() == CRuMorphTarget::Type())
		{
			CRuVector3 *position = static_cast<CRuMorphTarget *>(textMeshMT)->GetPosition();

			for(INT32 i = 0, numVertices = m_textMesh->GetNumVertices(); i < numVertices; ++i)
			{
				position[i] = position[i] + m_textWidgetOffset;
			}
		}

		// Setup flag model
		m_flagModel->RemoveAllSubModels();
		m_flagModel->AddSubModel(m_flagMesh, m_flagMaterial);
		m_flagModel->AddSubModel(m_textMesh, m_textMaterial);
	}
}

BOOL CRuEntity_Widget_Flag::Update(float elapsedTime)
{
	// Chain update call to base class
	CRuEntity::Update(elapsedTime);

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
