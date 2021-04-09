#include "../../Scene/Controller/RuController_Bone.h"

#include "../../Scene/Base/RuEntityContainer.h"
#include "../../Scene/Base/RuHierarchy_GR2.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_Bone, IRuEntity_Controller)
ruHEAP_DEFINE_SUBCLASS(CRuController_Bone, "CRuController_Bone", "IRuEntity_Controller")
ruCLASSGUID_DEFINE(CRuController_Bone, 0xED0A1539, 0x66B14cd9, 0xAB2A77C8, 0x1AC05748)

// ************************************************************************************************************************************************************

CRuController_Bone::CRuController_Bone()
:	m_blendGradient(1.0f),
	m_targetEntity(NULL)
{
	// Override base parameters
	m_duration = FLT_MAX;

	m_basisTransform.SetToBasis(CRuVector3(0.0f, -1.0f, 0.0f), CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f));
}

CRuController_Bone::~CRuController_Bone()
{
	if(m_targetEntity)
	{
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity->Event_Update().UnregisterHandler(this->GetGUID());
	}

	for(INT32 i = 0; i < m_transformDescriptors.Count(); ++i)
	{
		if(m_transformDescriptors[i].m_subNodeName)
		{
			delete[] m_transformDescriptors[i].m_subNodeName;
		}
	}
}

BOOL CRuController_Bone::Update(REAL elapsedTime)
{
	// Chain update call to base controller class
	return IRuEntity_Controller::Update(elapsedTime);
}

IRuObject *CRuController_Bone::GetTarget()
{
	return m_targetEntity;
}

BOOL CRuController_Bone::SetTarget(IRuObject *targetEntity)
{
	if(targetEntity->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		m_targetEntity = static_cast<CRuEntity *>(targetEntity);

		// Setup event handlers
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Bone::HandleEvent_Dispose, this, _1));
		m_targetEntity->Event_Update().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Bone::HandleEvent_Update, this, _1));
	}

	return TRUE;
}

BOOL CRuController_Bone::SetBasis(CRuVector3 xBasis, CRuVector3 yBasis, CRuVector3 zBasis)
{
	m_basisTransform.SetToBasis(xBasis, yBasis, zBasis);

	return TRUE;
}

BOOL CRuController_Bone::SetTransformTarget(const char *subNodeName, const CRuVector3 &scale, const CRuVector3 &originVector, const CRuVector3 &targetVector, const CRuVector3 &translation, REAL weight)
{
	if(m_targetEntity == NULL)
		return FALSE;

	// Find appropriate rotation descriptor
	INT32 descIndex = -1;
	for(INT32 i = 0; i < m_transformDescriptors.Count(); ++i)
	{
		if(stricmp(m_transformDescriptors[i].m_subNodeName, subNodeName) == 0)
		{
			descIndex = i;
			break;
		}
	}

	// If no appropriate rotation descriptor is available, create one
	if(descIndex < 0)
	{
		descIndex = m_transformDescriptors.Count();
		m_transformDescriptors.Add(TransformDesc());
		m_transformDescriptors[descIndex].m_subNodeName = ruNEW char [strlen(subNodeName) + 1];
		strcpy(m_transformDescriptors[descIndex].m_subNodeName, subNodeName);
		m_transformDescriptors[descIndex].m_subNodeID = 0;

		if(m_targetEntity->GetType() == CRuFrameHierarchy::Type())
		{
			m_transformDescriptors[descIndex].m_subNodeIndex = -1;
		}
		else if(m_targetEntity->GetType() == CRuHierarchy_GR2::Type())
		{
			m_transformDescriptors[descIndex].m_subNodeIndex = static_cast<CRuHierarchy_GR2 *>(m_targetEntity)->GetSubNodeIndexByName(m_transformDescriptors[descIndex].m_subNodeName);
		}
		else
		{
			m_transformDescriptors[descIndex].m_subNodeIndex = -1;
		}

		m_transformDescriptors[descIndex].m_scale_Current.Set(1.0f, 1.0f, 1.0f);
		m_transformDescriptors[descIndex].m_scale_0.Set(1.0f, 1.0f, 1.0f);
		m_transformDescriptors[descIndex].m_scale_1.Set(1.0f, 1.0f, 1.0f);

		m_transformDescriptors[descIndex].m_rotation_Current.Set(0.0f, 0.0f, 0.0f, -1.0f);
		m_transformDescriptors[descIndex].m_rotation_0.Set(0.0f, 0.0f, 0.0f, -1.0f);
		m_transformDescriptors[descIndex].m_rotation_1.Set(0.0f, 0.0f, 0.0f, -1.0f);

		m_transformDescriptors[descIndex].m_translation_Current.Set(0.0f, 0.0f, 0.0f);
		m_transformDescriptors[descIndex].m_translation_0.Set(0.0f, 0.0f, 0.0f);
		m_transformDescriptors[descIndex].m_translation_1.Set(0.0f, 0.0f, 0.0f);

		BlendDeltaKey(m_transformDescriptors[descIndex].m_subNodeIndex, m_transformDescriptors[descIndex].m_scale_Current, CRuVector3(0.0f, 1.0f, 0.0f), 0.0f, m_transformDescriptors[descIndex].m_translation_Current);
	}

	return SetTransformTarget_Internal(descIndex, scale, originVector, targetVector, translation, weight);
}

BOOL CRuController_Bone::SetTransformTarget(INT32 subNodeID, const CRuVector3 &scale, const CRuVector3 &originVector, const CRuVector3 &targetVector, const CRuVector3 &translation, REAL weight)
{
	if(m_targetEntity == NULL)
		return FALSE;

	// Find appropriate rotation descriptor
	INT32 descIndex = -1;
	for(INT32 i = 0; i < m_transformDescriptors.Count(); ++i)
	{
		if(m_transformDescriptors[i].m_subNodeID == subNodeID)
		{
			descIndex = i;
			break;
		}
	}

	// If no appropriate rotation descriptor is available, create one
	if(descIndex < 0)
	{
		descIndex = m_transformDescriptors.Count();
		m_transformDescriptors.Add(TransformDesc());
		m_transformDescriptors[descIndex].m_subNodeName = NULL;
		m_transformDescriptors[descIndex].m_subNodeID = subNodeID;

		if(m_targetEntity->GetType() == CRuFrameHierarchy::Type())
		{
			m_transformDescriptors[descIndex].m_subNodeIndex = static_cast<CRuFrameHierarchy *>(m_targetEntity)->GetSubNodeIndex(m_transformDescriptors[descIndex].m_subNodeID);
		}
		else if(m_targetEntity->GetType() == CRuHierarchy_GR2::Type())
		{
			m_transformDescriptors[descIndex].m_subNodeIndex = static_cast<CRuHierarchy_GR2 *>(m_targetEntity)->GetSubNodeIndex(m_transformDescriptors[descIndex].m_subNodeID);
		}
		else
		{
			m_transformDescriptors[descIndex].m_subNodeIndex = -1;
		}

		m_transformDescriptors[descIndex].m_scale_Current.Set(1.0f, 1.0f, 1.0f);
		m_transformDescriptors[descIndex].m_scale_0.Set(1.0f, 1.0f, 1.0f);
		m_transformDescriptors[descIndex].m_scale_1.Set(1.0f, 1.0f, 1.0f);

		m_transformDescriptors[descIndex].m_rotation_Current.Set(0.0f, 0.0f, 0.0f, -1.0f);
		m_transformDescriptors[descIndex].m_rotation_0.Set(0.0f, 0.0f, 0.0f, -1.0f);
		m_transformDescriptors[descIndex].m_rotation_1.Set(0.0f, 0.0f, 0.0f, -1.0f);

		m_transformDescriptors[descIndex].m_translation_Current.Set(0.0f, 0.0f, 0.0f);
		m_transformDescriptors[descIndex].m_translation_0.Set(0.0f, 0.0f, 0.0f);
		m_transformDescriptors[descIndex].m_translation_1.Set(0.0f, 0.0f, 0.0f);

		BlendDeltaKey(m_transformDescriptors[descIndex].m_subNodeIndex, m_transformDescriptors[descIndex].m_scale_Current, CRuVector3(0.0f, 1.0f, 0.0f), 0.0f, m_transformDescriptors[descIndex].m_translation_Current);
	}

	return SetTransformTarget_Internal(descIndex, scale, originVector, targetVector, translation, weight);
}

BOOL CRuController_Bone::AddTransformStopTarget(const char *subNodeName, const char *stopTargetName)
{
	if(m_targetEntity == NULL)
		return FALSE;

	// Find appropriate rotation descriptor
	INT32 descIndex = -1;
	for(INT32 i = 0; i < m_transformDescriptors.Count(); ++i)
	{
		if(stricmp(m_transformDescriptors[i].m_subNodeName, subNodeName) == 0)
		{
			descIndex = i;
			break;
		}
	}

	if(descIndex < 0)
	{
		descIndex = m_transformDescriptors.Count();
		m_transformDescriptors.Add(TransformDesc());
		m_transformDescriptors[descIndex].m_subNodeName = ruNEW char [strlen(subNodeName) + 1];
		strcpy(m_transformDescriptors[descIndex].m_subNodeName, subNodeName);
		m_transformDescriptors[descIndex].m_subNodeID = 0;

		if(m_targetEntity->GetType() == CRuFrameHierarchy::Type())
		{
			m_transformDescriptors[descIndex].m_subNodeIndex = -1;
		}
		else if(m_targetEntity->GetType() == CRuHierarchy_GR2::Type())
		{
			m_transformDescriptors[descIndex].m_subNodeIndex = static_cast<CRuHierarchy_GR2 *>(m_targetEntity)->GetSubNodeIndexByName(m_transformDescriptors[descIndex].m_subNodeName);
		}
		else
		{
			m_transformDescriptors[descIndex].m_subNodeIndex = -1;
		}

		m_transformDescriptors[descIndex].m_scale_Current.Set(1.0f, 1.0f, 1.0f);
		m_transformDescriptors[descIndex].m_scale_0.Set(1.0f, 1.0f, 1.0f);
		m_transformDescriptors[descIndex].m_scale_1.Set(1.0f, 1.0f, 1.0f);

		m_transformDescriptors[descIndex].m_rotation_Current.Set(0.0f, 0.0f, 0.0f, -1.0f);
		m_transformDescriptors[descIndex].m_rotation_0.Set(0.0f, 0.0f, 0.0f, -1.0f);
		m_transformDescriptors[descIndex].m_rotation_1.Set(0.0f, 0.0f, 0.0f, -1.0f);

		m_transformDescriptors[descIndex].m_translation_Current.Set(0.0f, 0.0f, 0.0f);
		m_transformDescriptors[descIndex].m_translation_0.Set(0.0f, 0.0f, 0.0f);
		m_transformDescriptors[descIndex].m_translation_1.Set(0.0f, 0.0f, 0.0f);

		SetTransformTarget_Internal(descIndex, CRuVector3(1.0f, 1.0f, 1.0f), CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 0.0f), 1.0f);

		BlendDeltaKey(m_transformDescriptors[descIndex].m_subNodeIndex, m_transformDescriptors[descIndex].m_scale_Current, CRuVector3(0.0f, 1.0f, 0.0f), 0.0f, m_transformDescriptors[descIndex].m_translation_Current);
	}

	if(m_targetEntity->GetType() == CRuHierarchy_GR2::Type())
	{
		INT32 terminatorIndex = static_cast<CRuHierarchy_GR2 *>(m_targetEntity)->GetSubNodeIndexByName(stopTargetName);

		if(terminatorIndex >= 0)
		{
			if(m_targetEntity->GetType() == CRuHierarchy_GR2::Type())
			{
				static_cast<CRuHierarchy_GR2 *>(m_targetEntity)->AddSubNodeDeltaKeyStopIndex(reinterpret_cast<PTRVALUE>(this), m_transformDescriptors[descIndex].m_subNodeIndex, terminatorIndex);
			}

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuController_Bone::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_targetEntity)
	{
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity->Event_Update().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	return TRUE;
}

BOOL CRuController_Bone::HandleEvent_Update(RuEventArgs *eventArgs)
{
	if(m_targetEntity)
	{
		for(INT32 i = 0; i < m_transformDescriptors.Count(); ++i)
		{
			// Update blend weights
			m_transformDescriptors[i].m_scale_BlendWeight += m_blendGradient;
			m_transformDescriptors[i].m_scale_BlendWeight = min(m_transformDescriptors[i].m_scale_BlendWeight, 1.0f);

			m_transformDescriptors[i].m_rotation_BlendWeight += m_blendGradient;
			m_transformDescriptors[i].m_rotation_BlendWeight = min(m_transformDescriptors[i].m_rotation_BlendWeight, 1.0f);

			m_transformDescriptors[i].m_translation_BlendWeight += m_blendGradient;
			m_transformDescriptors[i].m_translation_BlendWeight = min(m_transformDescriptors[i].m_translation_BlendWeight, 1.0f);

			// Calculate current scale
			m_transformDescriptors[i].m_scale_Current = m_transformDescriptors[i].m_scale_0 * (1.0f - m_transformDescriptors[i].m_scale_BlendWeight) + m_transformDescriptors[i].m_scale_1 * m_transformDescriptors[i].m_scale_BlendWeight;

			// Calculate current rotational quaternion
			m_transformDescriptors[i].m_rotation_Current = QuaternionSlerp(m_transformDescriptors[i].m_rotation_0, m_transformDescriptors[i].m_rotation_1, m_transformDescriptors[i].m_rotation_BlendWeight);

			// Break down rotational quaternion into angle-axis
			CRuVector3 rotAxis;
			REAL rotAngle;
			m_transformDescriptors[i].m_rotation_Current.GetAngleAxis(rotAxis, rotAngle);

			// Calculate current translation
			m_transformDescriptors[i].m_translation_Current = m_transformDescriptors[i].m_translation_0 * (1.0f - m_transformDescriptors[i].m_translation_BlendWeight) + m_transformDescriptors[i].m_translation_1 * m_transformDescriptors[i].m_translation_BlendWeight;

			// Blend into key frame
			BlendDeltaKey(m_transformDescriptors[i].m_subNodeIndex, m_transformDescriptors[i].m_scale_Current, rotAxis, rotAngle, m_transformDescriptors[i].m_translation_Current);
		}
	}

	return TRUE;
}

void CRuController_Bone::BlendDeltaKey(INT32 boneIndex, const CRuVector3 &scale, const CRuVector3 &rotAxis, REAL rotAngle, const CRuVector3 &position)
{
	if(boneIndex < 0)
		return;

	// Transform the delta key by the basis transform such that the key is in local space of the target hierarchy
	CRuVector3 rotationAxis;
	CRuVector3 localScale;
	CRuVector3 localPosition;

	m_basisTransform.TransformVector(rotAxis, rotationAxis);
	m_basisTransform.TransformVector(scale, localScale);
	m_basisTransform.TransformVector(position, localPosition);

	localScale.m_x = fabs(localScale.m_x);
	localScale.m_y = fabs(localScale.m_y);
	localScale.m_z = fabs(localScale.m_z);

	CRuQuaternion rotation;
	rotation.FromAngleAxis(rotationAxis, rotAngle);

	if(m_targetEntity->GetType() == CRuFrameHierarchy::Type())
	{
		static_cast<CRuFrameHierarchy *>(m_targetEntity)->GetInterpolator()->BlendDeltaKey(boneIndex, localScale, rotation, localPosition);
	}
	else if(m_targetEntity->GetType() == CRuHierarchy_GR2::Type())
	{
		static_cast<CRuHierarchy_GR2 *>(m_targetEntity)->SetSubNodeDeltaKey(reinterpret_cast<PTRVALUE>(this), boneIndex, localScale, rotation, localPosition);
	}
}

BOOL CRuController_Bone::SetTransformTarget_Internal(INT32 descIndex, const CRuVector3 &scale, const CRuVector3 &originVector, const CRuVector3 &targetVector, const CRuVector3 &translation, REAL weight)
{
	// Save scaling information
	m_transformDescriptors[descIndex].m_scale_BlendWeight = 0.0f;
	m_transformDescriptors[descIndex].m_scale_0 = m_transformDescriptors[descIndex].m_scale_Current;
	m_transformDescriptors[descIndex].m_scale_1 = scale;

	// Calculate rotational quaternion
	REAL dot = fabs(DotProduct(targetVector, originVector));

	if(dot < 1.0f - ruEPSILON)
	{
		// Calculate rotational vector from origin to target
		CRuVector3 rotationAxis = CrossProduct(targetVector, originVector);
		rotationAxis.Normalize();

		REAL turnAngle = acos(DotProduct(originVector, targetVector));
		REAL tripleProduct = TripleProduct_Scalar(rotationAxis, targetVector, originVector);

		if(tripleProduct < 0.0f)
		{
			turnAngle = ruPI * 2.0f - turnAngle;
		}

		turnAngle = turnAngle * weight;

		// Save rotation information
		m_transformDescriptors[descIndex].m_rotation_BlendWeight = 0.0f;
		m_transformDescriptors[descIndex].m_rotationWeight = weight;
		m_transformDescriptors[descIndex].m_rotation_0 = m_transformDescriptors[descIndex].m_rotation_Current;
		m_transformDescriptors[descIndex].m_rotation_1.FromAngleAxis(rotationAxis, turnAngle);
	}
	else
	{
		// Save rotation information
		m_transformDescriptors[descIndex].m_rotation_BlendWeight = 0.0f;
		m_transformDescriptors[descIndex].m_rotationWeight = weight;
		m_transformDescriptors[descIndex].m_rotation_0 = m_transformDescriptors[descIndex].m_rotation_Current;
		m_transformDescriptors[descIndex].m_rotation_1.Set(0.0f, 0.0f, 0.0f, 1.0f);
	}

	// Save scaling information
	m_transformDescriptors[descIndex].m_translation_BlendWeight = 0.0f;
	m_transformDescriptors[descIndex].m_translation_0 = m_transformDescriptors[descIndex].m_translation_Current;
	m_transformDescriptors[descIndex].m_translation_1 = translation;

/*
	// Calculate rotational quaternion
	CRuVector3 deltaVector = targetVector - originVector;
	CRuVector3 effectiveTargetVector = originVector + (deltaVector * weight);

	// Save rotation information
	m_transformDescriptors[descIndex].m_rotation_BlendWeight = 0.0f;
	m_transformDescriptors[descIndex].m_rotationWeight = weight;
	m_transformDescriptors[descIndex].m_rotation_0 = m_transformDescriptors[descIndex].m_rotation_Current;
	m_transformDescriptors[descIndex].m_rotation_1.RotateIntoVector(effectiveTargetVector, originVector);

	// Save scaling information
	m_transformDescriptors[descIndex].m_translation_BlendWeight = 0.0f;
	m_transformDescriptors[descIndex].m_translation_0 = m_transformDescriptors[descIndex].m_translation_Current;
	m_transformDescriptors[descIndex].m_translation_1 = translation;
*/
	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
