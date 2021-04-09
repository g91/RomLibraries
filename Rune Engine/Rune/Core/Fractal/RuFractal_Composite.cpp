#include "../../Core/Fractal/RuFractal_Composite.h"

#include <math.h>

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFractal_Composite, CRuFractal_Base)
ruHEAP_DEFINE_SUBCLASS(CRuFractal_Composite, "CRuFractal_Composite", "CRuFractal_Base")
ruCLASSGUID_DEFINE(CRuFractal_Composite, 0xDBA8B086, 0xB4F34b12, 0xA58EA815, 0xE15AD077)

// ************************************************************************************************************************************************************

CRuFractal_Composite::CRuFractal_Composite()
{
}

CRuFractal_Composite::~CRuFractal_Composite()
{
	for(INT32 i = 0; i < m_componentFractals.Count(); ++i)
	{
		ruSAFE_RELEASE(m_componentFractals[i].m_fractal);
	}
}

void CRuFractal_Composite::AppendFractal(CRuFractal_Base *fractal, RuFractalCompositeOp compositeOp)
{
	m_componentFractals.Add(FractalDesc(fractal, compositeOp));
}

void CRuFractal_Composite::DeleteFractal(INT32 index)
{
	ruSAFE_RELEASE(m_componentFractals[index].m_fractal);
	m_componentFractals.RemoveAt(index);
}

INT32 CRuFractal_Composite::GetNumFractals() const
{
	return m_componentFractals.Count();
}

INT32 CRuFractal_Composite::GetFractalIndex(CRuFractal_Base *fractal)
{
	for(INT32 i = 0; i < m_componentFractals.Count(); ++i)
	{
		if(m_componentFractals[i].m_fractal == fractal)
		{
			return i;
		}
	}

	return -1;
}

CRuFractal_Base *CRuFractal_Composite::GetFractal(INT32 index)
{
	return m_componentFractals[index].m_fractal;
}

RuFractalCompositeOp CRuFractal_Composite::GetFractalCompositeOp(INT32 index)
{
	return m_componentFractals[index].m_compositeOp;
}

void CRuFractal_Composite::SetFractalCompositeOp(INT32 index, RuFractalCompositeOp compositeOp)
{
	m_componentFractals[index].m_compositeOp = compositeOp;
}

BOOL CRuFractal_Composite::SerializeFrom(IRuStream *stream)
{
	// Read class GUID
	CRuGUID classGUID;
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuFractal_Base::SerializeFrom(stream);

	// Read number of fractal components
	INT32 numFractals = 0;
	stream->Read(&numFractals, sizeof(INT32));

	// Read fractal components
	for(INT32 i = 0; i < numFractals; ++i)
	{
		// Read composite op
		RuFractalCompositeOp compositeOp;
		stream->Read(&compositeOp, sizeof(RuFractalCompositeOp));

		// Construct fractal
		IRuObject *fractalObject = RuObjectFactory().CreateObject(stream);

		if(fractalObject)
		{
			if(fractalObject->GetType().IsSubClassOf(CRuFractal_Base::Type()))
			{
				m_componentFractals.Add(FractalDesc(static_cast<CRuFractal_Base *>(fractalObject), compositeOp));
			}
			else
			{
				ruSAFE_RELEASE(fractalObject);
			}
		}
	}

	return TRUE;
}

BOOL CRuFractal_Composite::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuFractal_Base::SerializeTo(stream);

	// Write number of fractal components
	INT32 numFractals = m_componentFractals.Count();
	stream->Write(&numFractals, sizeof(INT32));

	// Write fractal components
	for(INT32 i = 0; i < numFractals; ++i)
	{
		// Write composite op
		stream->Write(&m_componentFractals[i].m_compositeOp, sizeof(RuFractalCompositeOp));

		// Serialize fractal
		m_componentFractals[i].m_fractal->SerializeTo(stream);
	}

	return TRUE;
}

REAL CRuFractal_Composite::Evaluate(REAL x, REAL y, REAL z)
{
	REAL value = 0.0f;

	// Adjust input coordinates
	x += m_xOffset;
	y += m_yOffset;
	x = x * m_scale;
	y = y * m_scale;
	z = z * m_scale;

	for(INT32 i = 0; i < m_componentFractals.Count(); ++i)
	{
		switch(m_componentFractals[i].m_compositeOp)
		{
			default:
				{
				}

				break;

			case ruFRACTALCOMPOSITEOP_ADD:
				{
					value += m_componentFractals[i].m_fractal->Evaluate(x, y, z);
				}

				break;

			case ruFRACTALCOMPOSITEOP_SUBTRACT:
				{
					value -= m_componentFractals[i].m_fractal->Evaluate(x, y, z);
				}

				break;

			case ruFRACTALCOMPOSITEOP_MULTIPLY:
				{
					value *= m_componentFractals[i].m_fractal->Evaluate(x, y, z);
				}

				break;

			case ruFRACTALCOMPOSITEOP_DIVIDE:
				{
					value /= m_componentFractals[i].m_fractal->Evaluate(x, y, z);
				}

				break;

			case ruFRACTALCOMPOSITEOP_SET:
				{
					value = m_componentFractals[i].m_fractal->Evaluate(x, y, z);
				}

				break;

			case ruFRACTALCOMPOSITEOP_POSTFX:
				{
					value = m_componentFractals[i].m_fractal->Evaluate(value, 0.0f, 0.0f);
				}

				break;

			case ruFRACTALCOMPOSITEOP_PARAM_X:
				{
					if(i + 1 < m_componentFractals.Count())
					{
						x += m_componentFractals[i].m_fractal->Evaluate(x, y, z) / m_componentFractals[i + 1].m_fractal->GetScale();
					}
				}

				break;

			case ruFRACTALCOMPOSITEOP_PARAM_Y:
				{
					if(i + 1 < m_componentFractals.Count())
					{
						y += m_componentFractals[i].m_fractal->Evaluate(x, y, z) / m_componentFractals[i + 1].m_fractal->GetScale();
					}
				}

				break;
		}
	}

	return (value + m_bias) * m_factor;
//	return (((value + 1.0f) / 2.0f) + m_bias) * m_factor;
//	return ((value + m_bias) * m_factor + 1.0f) / 2.0f;
//	return max(-1.0f, min(1.0f, (value + m_bias) * m_factor));
}

// ************************************************************************************************************************************************************

#pragma managed
