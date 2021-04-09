#include "../../Scene/Controller/RuController_Morph.h"

#include "../../Helper/RuHelper_Entity.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuController_Morph, IRuEntity_Controller)
ruHEAP_DEFINE_SUBCLASS(CRuController_Morph, "CRuController_Morph", "IRuEntity_Controller")
ruCLASSGUID_DEFINE(CRuController_Morph, 0x9ACD4594, 0xA3AD416e, 0x8B8BF07D, 0xEE233974)

// ************************************************************************************************************************************************************

CRuController_Morph::CRuController_Morph()
:	m_target(NULL),
	m_enableCurveMode(FALSE),
	m_curve_t0(0.0f),
	m_curve_t1(0.0f),
	m_frameRate(30.0f),
	m_startFrame(0),
	m_endFrame(0)
{
	// Override default parameter to enable looping
	m_loopTime = 0.0f;

	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Morph::HandleEvent_Dispose, this, _1), NULL);
}

CRuController_Morph::~CRuController_Morph()
{
	// NOTE: Destruction of resources is done in the dispose event handler.
}

IRuObject *CRuController_Morph::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuController_Morph();
	CRuController_Morph *controller = static_cast<CRuController_Morph *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_Controller::Clone(clonedObject);

	controller->m_enableCurveMode = m_enableCurveMode;

	controller->m_curve_t0 = m_curve_t0;
	controller->m_curve_t1 = m_curve_t1;

	controller->m_frameRate = m_frameRate;
	controller->m_startFrame = m_startFrame;
	controller->m_endFrame = m_endFrame;

	return clonedObject;
}

BOOL CRuController_Morph::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Controller::SerializeFrom(stream);

	switch(version)
	{
		case 1:
			{
				stream->Read(&m_frameRate, sizeof(REAL));
				stream->Read(&m_startFrame, sizeof(INT32));
				stream->Read(&m_endFrame, sizeof(INT32));
			}

			break;

		case 2:
			{
				stream->Read(&m_enableCurveMode, sizeof(BOOL));
				stream->Read(&m_curve_t0, sizeof(REAL));
				stream->Read(&m_curve_t1, sizeof(REAL));
				stream->Read(&m_frameRate, sizeof(REAL));
				stream->Read(&m_startFrame, sizeof(INT32));
				stream->Read(&m_endFrame, sizeof(INT32));
			}

			break;
	}

	return TRUE;
}

BOOL CRuController_Morph::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 2;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Controller::SerializeTo(stream);

	stream->Write(&m_enableCurveMode, sizeof(BOOL));
	stream->Write(&m_curve_t0, sizeof(REAL));
	stream->Write(&m_curve_t1, sizeof(REAL));
	stream->Write(&m_frameRate, sizeof(REAL));
	stream->Write(&m_startFrame, sizeof(INT32));
	stream->Write(&m_endFrame, sizeof(INT32));

	return TRUE;
}

BOOL CRuController_Morph::Update(REAL elapsedTime)
{
	// Increment current time
	if(m_target && m_enabled)
	{
		CRuParameterBlock *parameterBlock = m_target->GetParameterBlock();

		if(m_enableCurveMode)
		{
			REAL t = m_age + m_curve_t0;

			// If t has passed the t1 threshold, either loop or clamp
			if(t > m_curve_t1)
			{
				REAL meshAnimLength = m_curve_t1 - m_curve_t0;

				if(m_loopTime >= 0.0f)
				{
					t = fmod(t - m_curve_t0, meshAnimLength) + m_curve_t0;
				}
				else
				{
					t = m_curve_t1;
				}
			}

			parameterBlock->SetParameter(ruPARAMTYPE_TWEEN_FACTOR, RuCastFloatToINT64(t));
		}
		else
		{
			INT32 frameCount = m_endFrame - m_startFrame;
			REAL t = m_age;
			REAL frameLength = 1.0f / m_frameRate;
			REAL meshAnimLength = frameCount * frameLength;

			if(meshAnimLength == 0.0f)
			{
				parameterBlock->SetParameter(ruPARAMTYPE_TWEEN_FACTOR, RuCastFloatToINT64(0.0f));
				parameterBlock->SetParameter(ruPARAMTYPE_TWEEN_MORPHTARGET_0, 0);
				parameterBlock->SetParameter(ruPARAMTYPE_TWEEN_MORPHTARGET_1, 0);
			}
			else
			{
				// Adjust time to fit into proper range
				if(t > meshAnimLength)
				{
					if(m_loopTime >= 0.0f)
					{
						t = fmod(t, meshAnimLength);
					}
					else
					{
						t = meshAnimLength;
					}
				}

				INT32 f0 = (INT32) ((t / meshAnimLength) * frameCount) + m_startFrame;

				if(f0 > m_endFrame)
				{
					if(m_loopTime >= 0.0f)
					{
						f0 = m_startFrame;
					}
					else
					{
						f0 = m_endFrame;
					}
				}

				INT32 f1 = f0 + 1;

				if(f1 > m_endFrame)
				{
					if(m_loopTime >= 0.0f)
					{
						f1 = m_startFrame;
					}
					else
					{
						f1 = f0;
					}
				}

				REAL tweenTime = (t - f0 * frameLength) / frameLength;

				parameterBlock->SetParameter(ruPARAMTYPE_TWEEN_FACTOR, RuCastFloatToINT64(tweenTime));
				parameterBlock->SetParameter(ruPARAMTYPE_TWEEN_MORPHTARGET_0, f0);
				parameterBlock->SetParameter(ruPARAMTYPE_TWEEN_MORPHTARGET_1, f1);
			}
		}
	}

	// Chain update call to base controller class
	return IRuEntity_Controller::Update(elapsedTime);
}

IRuObject *CRuController_Morph::GetTarget()
{
	return m_target;
}

BOOL CRuController_Morph::SetTarget(IRuObject *target)
{
	if(target->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		// If we had a previous target, unregister it
		if(m_target)
		{
			m_target->Event_Dispose().UnregisterHandler(this->GetGUID());
		}

		// Store target
		m_target = static_cast<CRuEntity *>(target);
		m_target->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuController_Morph::HandleEvent_Dispose, this, _1), NULL);

		// Get highest morph target count
		m_morphTargetCount = RuEntity_GetMaxMorphTargetCount(m_target);

		return TRUE;
	}

	return FALSE;
}

void CRuController_Morph::EnableCurveMode(BOOL enableCurveMode)
{
	m_enableCurveMode = enableCurveMode;
}

void CRuController_Morph::SetCurveTimeRange(REAL t0, REAL t1)
{
	m_curve_t0 = t0;
	m_curve_t1 = t1;

	// Recalculate morph duration
	CalculateDuration();
}

void CRuController_Morph::SetFrameRate(REAL frameRate)
{
	m_frameRate = frameRate;

	// Recalculate morph duration
	CalculateDuration();
}

void CRuController_Morph::SetFrameRange(INT32 startFrame, INT32 endFrame)
{
	m_startFrame = startFrame;
	m_endFrame = endFrame;

	// Recalculate morph duration
	CalculateDuration();
}

BOOL CRuController_Morph::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_target)
	{
		m_target->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_target = NULL;
	}

	return TRUE;
}

void CRuController_Morph::CalculateDuration()
{
	if(m_enableCurveMode)
	{
		m_duration = m_curve_t1 - m_curve_t0;
	}
	else
	{
		m_duration = (m_endFrame - m_startFrame) / m_frameRate;
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
