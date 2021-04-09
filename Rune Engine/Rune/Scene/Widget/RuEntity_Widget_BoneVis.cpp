/*!
	@file RuEntity_Widget_BoneVis.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Scene/Widget/RuEntity_Widget_BoneVis.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Widget_BoneVis, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Widget_BoneVis, "CRuEntity_Widget_BoneVis", "CRuEntity")
ruCLASSGUID_DEFINE(CRuEntity_Widget_BoneVis, 0xE7AD9F71, 0x793F42c3, 0x93746311, 0x0FACA8D8)

// ************************************************************************************************************************************************************

CRuEntity_Widget_BoneVis::CRuEntity_Widget_BoneVis()
{
	m_boneMesh = NULL;
	m_boneMaterial = NULL;
	m_boneModel = NULL;
}

CRuEntity_Widget_BoneVis::~CRuEntity_Widget_BoneVis()
{
	ruSAFE_RELEASE(m_boneMesh);
	ruSAFE_RELEASE(m_boneMaterial);
	ruSAFE_RELEASE(m_boneModel);
}

void CRuEntity_Widget_BoneVis::SetTarget(CRuEntity *target)
{
	if(target->GetType().IsSubClassOf(CRuFrameHierarchy::Type()))
	{
		ruSAFE_RELEASE(m_boneMesh);
		ruSAFE_RELEASE(m_boneMaterial);
		ruSAFE_RELEASE(m_boneModel);

		m_targetHierarchy = static_cast<CRuFrameHierarchy *>(target);

		m_boneMesh = ruNEW CRuDynamicMesh();
		m_boneMesh->Initialize(ruPRIMTYPE_LINELIST, m_targetHierarchy->GetNumSubNodes() * 2, m_targetHierarchy->GetNumSubNodes(), ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);
		m_boneMesh->SetNumPrimitives(m_targetHierarchy->GetNumSubNodes());
		m_boneMesh->SetNumVertices(m_targetHierarchy->GetNumSubNodes() * 2);

		m_boneMaterial = ruNEW CRuMaterial();
		m_boneMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_STATIC_PRELIT"));

		m_boneModel = ruNEW CRuModel();
		m_boneModel->AddSubModel(m_boneMesh, m_boneMaterial);

		// Add bone model
		this->AddChild(m_boneModel);
	}
}

CRuEntity *CRuEntity_Widget_BoneVis::Clone(CRuEntity *clonedEntity)
{
	return NULL;
}

BOOL CRuEntity_Widget_BoneVis::ResetStates()
{
	return TRUE;
}

BOOL CRuEntity_Widget_BoneVis::Update(float elapsedTime)
{
	// Chain update call to base class
	CRuEntity::Update(elapsedTime);

	if(m_targetHierarchy)
	{
		CRuStack<INT32> parentStack;

		INT32 numSubNodes = m_targetHierarchy->GetNumSubNodes();

		CRuVector3 *position = static_cast<CRuMorphTarget *>(m_boneMesh->GetMorphTarget(0))->GetPosition();
		DWORD *diffuse = static_cast<CRuMorphTarget *>(m_boneMesh->GetMorphTarget(0))->GetDiffuse();
		UINT16 *indices = m_boneMesh->GetIndices();

		DWORD colors[3] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000 };

		for(INT32 i = 0, currentParent = 0, j = 0, k = 0; i < numSubNodes; ++i)
		{
			UINT32 flags = m_targetHierarchy->GetSubNodeFlags(i);

			CRuVector3 p0(0.0f, 0.0f, 0.0f);
			CRuVector3 p1(0.0f, 0.0f, 0.0f);

			if(currentParent == i)
			{
				// Current node has no parent, do nothing
			}
			else
			{
				CRuMatrix4x4 p0Transform = m_targetHierarchy->GetSubNodeSkinToBoneTransform(currentParent).GetInverse() * m_targetHierarchy->GetSubNodeTransform(currentParent);
				CRuMatrix4x4 p1Transform = m_targetHierarchy->GetSubNodeSkinToBoneTransform(i).GetInverse() * m_targetHierarchy->GetSubNodeTransform(i);

				p0Transform.TransformPoint(p0);
				p1Transform.TransformPoint(p1);
			}

			position[j] = p0;
			diffuse[j++] = colors[i % 3];

			position[j] = p1;
			diffuse[j++] = colors[i % 3];

			indices[k++] = j - 2;
			indices[k++] = j - 1;

			// Handle traversal of the hierarchy
			if(flags & ruFRAMEHIERARCHY_PUSH)
			{
				parentStack.Push(currentParent);
			}

			if(flags & ruFRAMEHIERARCHY_POP)
			{
				// If the stack is empty when we hit a pop flag, that means we are starting a new tree
				if(parentStack.IsEmpty())
				{
					// Set current parent to the next node
					currentParent = i + 1;
				}
				else
				{
					currentParent = parentStack.Pop();
				}
			}
			else
			{
				currentParent = i;
			}
		}

		m_boneMesh->IncrementMeshIteration();
		m_boneMesh->CalculateBounds();
	}

	return TRUE;
}

BOOL CRuEntity_Widget_BoneVis::QueryCollision(CRuCollisionQuery *colQuery)
{
	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
