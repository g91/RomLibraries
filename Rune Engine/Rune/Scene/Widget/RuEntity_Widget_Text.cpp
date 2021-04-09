/*!
	@file RuEntity_Widget_Text.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Scene/Widget/RuEntity_Widget_Text.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Widget_Text, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Widget_Text, "CRuEntity_Widget_Text", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuEntity_Widget_Text, 0x2B8F9C58, 0x2F714a65, 0x9C58A55F, 0x97B1F4DB)

// ************************************************************************************************************************************************************

CRuEntity_Widget_Text::CRuEntity_Widget_Text()
:	m_fontType(NULL)
{
	m_textMesh = NULL;
	m_textMaterial = NULL;
}

CRuEntity_Widget_Text::~CRuEntity_Widget_Text()
{
	ruSAFE_RELEASE(m_textMesh);
	ruSAFE_RELEASE(m_textMaterial);
}

void CRuEntity_Widget_Text::SetText(CRuFontType *fontType, REAL textScale, RuTextAlignment textAlignment, RUCOLOR textColor, const char *text)
{
	m_fontType = fontType;
	m_textScale = textScale;

	if(m_fontType)
	{
		// Convert string to unicode string
		INT32 textLength = (INT32)strlen(text);
		WCHAR *unicodeText = ruNEW WCHAR [textLength + 1];
		MultiByteToWideChar(CP_ACP, 0, text, textLength + 1, unicodeText, textLength + 1);

		// Make sure dynamic mesh has enough space for the text string
		if(m_textMesh && m_textMesh->GetMaxPrimitives() < textLength)
		{
			ruSAFE_RELEASE(m_textMesh);
		}

		// If the dynamic mesh has not been created, create it now
		if(!m_textMesh)
		{
			m_textMesh = ruNEW CRuDynamicMesh();
			m_textMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, textLength * 4, textLength * 2, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);
			m_textMesh->AddTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
		}

		// Create new material
		if(m_textMaterial == NULL)
		{
			m_textMaterial = ruNEW CRuMaterial();
			m_textMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_WIDGET_LABEL"));
			m_textMaterial->SetTexture(ruTEXCHANNEL_DIFFUSEMAP, CRuFontTexture2::GetRuTextureD3D());
			m_textMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, FALSE);
		}

		CRuVector3 *position = static_cast<CRuMorphTarget *>(m_textMesh->GetMorphTarget(0))->GetPosition();
		DWORD *diffuse = static_cast<CRuMorphTarget *>(m_textMesh->GetMorphTarget(0))->GetDiffuse();
		REAL *uv = static_cast<CRuMorphTarget *>(m_textMesh->GetMorphTarget(0))->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
		UINT16 *indices = m_textMesh->GetIndices();

		// Iterate through each character
		REAL curRowHeight = 0.0f;
		REAL curX = 0.0f, curY = 0.0f;		

		vector<wstring> strList;
		CRuFontString::SplitString(unicodeText, textLength, strList);
		INT32 vertIndex = 0, iIndex = 0;
		for ( vector<wstring>::iterator iter = strList.begin(); iter != strList.end(); iter++ )
		{
			CRuFontString* fontString = CRuFontString::GetFontString(m_fontType, iter->c_str(), (int)iter->length(), TRUE);
			if ( fontString )
			{
				CRuFontTexture2* fontTexture = fontString->GetFontTexture();
				if ( fontTexture )
				{
					CRuRectangle rect =  fontTexture->GetRectangle();

					// Get character dimensions and multiply by scale
					REAL quadWidth = m_textScale * fontString->GetXSize();
					REAL quadHeight = m_textScale * fontString->GetYSize();

					curRowHeight = max(curRowHeight, quadHeight);

					indices[iIndex++] = vertIndex;
					indices[iIndex++] = vertIndex + 1;
					indices[iIndex++] = vertIndex + 2;

					indices[iIndex++] = vertIndex + 1;
					indices[iIndex++] = vertIndex + 3;
					indices[iIndex++] = vertIndex + 2;

					// Create quad for the character
					uv[vertIndex * 2] = rect.m_left / CRuFontTexture2::GetRuTextureWidth();
					uv[vertIndex * 2 + 1] = rect.m_top / CRuFontTexture2::GetRuTextureHeight();
					diffuse[vertIndex] = textColor;
					position[vertIndex++].Set(curX, curY, 0.0f);

					uv[vertIndex * 2] = (rect.m_left + fontString->GetXSize()) / CRuFontTexture2::GetRuTextureWidth();
					uv[vertIndex * 2 + 1] = rect.m_top / CRuFontTexture2::GetRuTextureHeight();
					diffuse[vertIndex] = textColor;
					position[vertIndex++].Set(curX + quadWidth, curY, 0.0f);

					uv[vertIndex * 2] = rect.m_left / CRuFontTexture2::GetRuTextureWidth();
					uv[vertIndex * 2 + 1] = (rect.m_top + fontString->GetYSize()) / CRuFontTexture2::GetRuTextureHeight();
					diffuse[vertIndex] = textColor;
					position[vertIndex++].Set(curX, curY - quadHeight, 0.0f);

					uv[vertIndex * 2] = (rect.m_left + fontString->GetXSize()) / CRuFontTexture2::GetRuTextureWidth();
					uv[vertIndex * 2 + 1] = (rect.m_top + fontString->GetYSize()) / CRuFontTexture2::GetRuTextureHeight();
					diffuse[vertIndex] = textColor;
					position[vertIndex++].Set(curX + quadWidth, curY - quadHeight, 0.0f);

					m_textMesh->SetNumPrimitives(iIndex / 3);
					m_textMesh->SetNumVertices(vertIndex);

					curX += quadWidth;
				}
			}
		}

		/*
		for(INT32 i = 0, vertIndex = 0, iIndex = 0; i < textLength; ++i)
		{
			CRuFontChar *fontChar = CRuFontChar::GetFontChar(m_fontType, unicodeText[i]);
			CRuFontTexture *fontTexture = fontChar->GetFontTexture();
			CRuRectangle rect =  fontTexture->GetRectangle();

			// Get character dimensions and multiply by scale
			REAL quadWidth = m_textScale * fontChar->GetXSize();
			REAL quadHeight = m_textScale * fontChar->GetYSize();

			curRowHeight = max(curRowHeight, quadHeight);

			indices[iIndex++] = vertIndex;
			indices[iIndex++] = vertIndex + 1;
			indices[iIndex++] = vertIndex + 2;

			indices[iIndex++] = vertIndex + 1;
			indices[iIndex++] = vertIndex + 3;
			indices[iIndex++] = vertIndex + 2;

			// Create quad for the character
			uv[vertIndex * 2] = rect.m_left / CRuFontTexture::GetRuTextureWidth();
			uv[vertIndex * 2 + 1] = rect.m_top / CRuFontTexture::GetRuTextureHeight();
			diffuse[vertIndex] = textColor;
			position[vertIndex++].Set(curX, curY, 0.0f);

			uv[vertIndex * 2] = (rect.m_left + fontChar->GetXSize()) / CRuFontTexture::GetRuTextureWidth();
			uv[vertIndex * 2 + 1] = rect.m_top / CRuFontTexture::GetRuTextureHeight();
			diffuse[vertIndex] = textColor;
			position[vertIndex++].Set(curX + quadWidth, curY, 0.0f);

			uv[vertIndex * 2] = rect.m_left / CRuFontTexture::GetRuTextureWidth();
			uv[vertIndex * 2 + 1] = (rect.m_top + fontChar->GetYSize()) / CRuFontTexture::GetRuTextureHeight();
			diffuse[vertIndex] = textColor;
			position[vertIndex++].Set(curX, curY - quadHeight, 0.0f);

			uv[vertIndex * 2] = (rect.m_left + fontChar->GetXSize()) / CRuFontTexture::GetRuTextureWidth();
			uv[vertIndex * 2 + 1] = (rect.m_top + fontChar->GetYSize()) / CRuFontTexture::GetRuTextureHeight();
			diffuse[vertIndex] = textColor;
			position[vertIndex++].Set(curX + quadWidth, curY - quadHeight, 0.0f);

			m_textMesh->SetNumPrimitives(iIndex / 3);
			m_textMesh->SetNumVertices(vertIndex);

			curX += quadWidth;
		}
		*/

		curY += curRowHeight;

		delete[] unicodeText;

		REAL xOffset = 0.0f, yOffset = 0.0f;

		switch(textAlignment)
		{
			case ruTEXTALIGNMENT_TOP_LEFT:
				break;

			case ruTEXTALIGNMENT_TOP_CENTER:
				xOffset = -curX / 2.0f;
				break;

			case ruTEXTALIGNMENT_TOP_RIGHT:
				xOffset = -curX;
				break;

			case ruTEXTALIGNMENT_MIDDLE_LEFT:
				yOffset = curY / 2.0f;
				break;

			case ruTEXTALIGNMENT_MIDDLE_CENTER:
				xOffset = -curX / 2.0f;
				yOffset = curY / 2.0f;
				break;

			case ruTEXTALIGNMENT_MIDDLE_RIGHT:
				xOffset = -curX;
				yOffset = curY / 2.0f;
				break;

			case ruTEXTALIGNMENT_BOTTOM_LEFT:
				yOffset = curY;
				break;

			case ruTEXTALIGNMENT_BOTTOM_CENTER:
				xOffset = -curX / 2.0f;
				yOffset = curY;
				break;

			case ruTEXTALIGNMENT_BOTTOM_RIGHT:
				xOffset = -curX;
				yOffset = curY;
				break;
		}

		for(INT32 i = 0; i < m_textMesh->GetNumVertices(); ++i)
		{
			position[i].x += xOffset;
			position[i].y += yOffset;
		}

		// Update mesh iteration and bounds
		m_textMesh->IncrementMeshIteration();
		m_textMesh->CalculateBounds();

		// Set local entity bounds
		this->SetLocalEntityBounds(m_textMesh->GetBounds());

		// Set billboard option to unconstrained
		SetOption(ruENTITYOPTION_BILLBOARD, ruBILLBOARDTYPE_UNCONSTRAINED);
	}
}

BOOL CRuEntity_Widget_Text::Update(float elapsedTime)
{
	// Chain update call to base class
	IRuEntity_Renderable::Update(elapsedTime);

	return TRUE;
}

INT32 CRuEntity_Widget_Text::GetNumMeshes() const
{
	if(m_textMesh)
		return 1;

	return 0;
}

BOOL CRuEntity_Widget_Text::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex == 0)
	{
		*meshOut = m_textMesh;
		*materialOut = m_textMaterial;

		return TRUE;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed
