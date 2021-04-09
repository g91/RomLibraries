#ifndef _RUCONTROLLER_BONE_H_
#define _RUCONTROLLER_BONE_H_

#include "../../Scene/Base/RuEntityController.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuController_Bone : public IRuEntity_Controller
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct TransformDesc
	{
		char*								m_subNodeName;
		INT32								m_subNodeID;									//!< ID of the sub node to control
		INT32								m_subNodeIndex;									//!< Index of the sub node to control

		REAL								m_scaleWeight;
		REAL								m_rotationWeight;

		REAL								m_scale_BlendWeight;
		CRuVector3							m_scale_Current;								//!< Current scale
		CRuVector3							m_scale_0;										//!< Origin scale
		CRuVector3							m_scale_1;										//!< Target scale

		REAL								m_rotation_BlendWeight;
		CRuQuaternion						m_rotation_Current;								//!< Current rotation
		CRuQuaternion						m_rotation_0;									//!< Origin rotation
		CRuQuaternion						m_rotation_1;									//!< Target rotation

		REAL								m_translation_BlendWeight;
		CRuVector3							m_translation_Current;							//!< Current translation
		CRuVector3							m_translation_0;								//!< Origin translation
		CRuVector3							m_translation_1;								//!< Target translation
	};

	// Controller settings
	CRuMatrix4x4							m_basisTransform;								//!< Basis transform to be applied onto all transformations
	REAL									m_blendGradient;								//!< 

	// Target entity
	CRuEntity*								m_targetEntity;									//!< Entity that the controller affects

	// Rotation data
	CRuArrayList<TransformDesc>				m_transformDescriptors;

public:
											CRuController_Bone();
	virtual									~CRuController_Bone();

	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *targetEntity);

	virtual BOOL							SetBasis(CRuVector3 xBasis, CRuVector3 yBasis, CRuVector3 zBasis);
	virtual BOOL							SetTransformTarget(const char *subNodeName, const CRuVector3 &scale, const CRuVector3 &originVector, const CRuVector3 &targetVector, const CRuVector3 &translation, REAL weight);
	virtual BOOL							SetTransformTarget(INT32 subNodeID, const CRuVector3 &scale, const CRuVector3 &originVector, const CRuVector3 &targetVector, const CRuVector3 &translation, REAL weight);
	virtual BOOL							AddTransformStopTarget(const char *subNodeName, const char *stopTargetName);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);
	BOOL									HandleEvent_Update(RuEventArgs *eventArgs);

protected:
	void									BlendDeltaKey(INT32 boneIndex, const CRuVector3 &scale, const CRuVector3 &rotAxis, REAL rotAngle, const CRuVector3 &position);

	BOOL									SetTransformTarget_Internal(INT32 descIndex, const CRuVector3 &scale, const CRuVector3 &originVector, const CRuVector3 &targetVector, const CRuVector3 &translation, REAL weight);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
