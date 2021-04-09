#include "../../Core/Fractal/RuFractal_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFractal_Base, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuFractal_Base, "CRuFractal_Base", "IRuObject")
ruCLASSGUID_DEFINE(CRuFractal_Base, 0xD53B2D58, 0x2C3B4fde, 0xAEF37F03, 0xB22BE1D5)

// ************************************************************************************************************************************************************

CRuFractal_Base::CRuFractal_Base()
:	m_scale(1.0f),
	m_bias(0.0f),
	m_factor(1.0f),
	m_xOffset(0.0f),
	m_yOffset(0.0f),
	m_noiseFunction(NULL)
{
}

CRuFractal_Base::~CRuFractal_Base()
{
	ruSAFE_RELEASE(m_noiseFunction);
}

BOOL CRuFractal_Base::SerializeFrom(IRuStream *stream)
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
	IRuObject::SerializeFrom(stream);

	// Read scale
	stream->Read(&m_scale, sizeof(REAL));

	// Read bias
	stream->Read(&m_bias, sizeof(REAL));

	// Read factor
	stream->Read(&m_factor, sizeof(REAL));

	if(version >= 2)
	{
		// Read x offset
		stream->Read(&m_xOffset, sizeof(REAL));

		// Read y offset
		stream->Read(&m_yOffset, sizeof(REAL));
	}

	// Read noise function available flag
	BOOL noiseFunctionAvailable = FALSE;
	stream->Read(&noiseFunctionAvailable, sizeof(BOOL));

	// Serialize noise function
	if(noiseFunctionAvailable)
	{
		IRuObject *noiseObject = RuObjectFactory().CreateObject(stream);

		if(noiseObject)
		{
			if(noiseObject->GetType().IsSubClassOf(CRuNoise_Base::Type()))
			{
				m_noiseFunction = static_cast<CRuNoise_Base *>(noiseObject);
			}
			else
			{
				ruSAFE_RELEASE(noiseObject);
			}
		}
	}

	return TRUE;
}

BOOL CRuFractal_Base::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 2;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeTo(stream);

	// Write scale
	stream->Write(&m_scale, sizeof(REAL));

	// Write bias
	stream->Write(&m_bias, sizeof(REAL));

	// Write factor
	stream->Write(&m_factor, sizeof(REAL));

	// Write x offset
	stream->Write(&m_xOffset, sizeof(REAL));

	// Write y offset
	stream->Write(&m_yOffset, sizeof(REAL));

	if(m_noiseFunction)
	{
		// Mark noise function available
		BOOL noiseFunctionAvailable = TRUE;
		stream->Write(&noiseFunctionAvailable, sizeof(BOOL));

		// Serialize noise function
		m_noiseFunction->SerializeTo(stream);
	}
	else
	{
		// Mark noise function available
		BOOL noiseFunctionAvailable = FALSE;
		stream->Write(&noiseFunctionAvailable, sizeof(BOOL));
	}

	return TRUE;
}

REAL CRuFractal_Base::GetScale()
{
	return m_scale;
}

void CRuFractal_Base::SetScale(REAL scale)
{
	m_scale = scale;
}

REAL CRuFractal_Base::GetBias()
{
	return m_bias;
}

void CRuFractal_Base::SetBias(REAL bias)
{
	m_bias = bias;
}

REAL CRuFractal_Base::GetFactor()
{
	return m_factor;
}

void CRuFractal_Base::SetFactor(REAL factor)
{
	m_factor = factor;
}

REAL CRuFractal_Base::GetXOffset()
{
	return m_xOffset;
}

void CRuFractal_Base::SetXOffset(REAL offset)
{
	m_xOffset = offset;
}

REAL CRuFractal_Base::GetYOffset()
{
	return m_yOffset;
}

void CRuFractal_Base::SetYOffset(REAL offset)
{
	m_yOffset = offset;
}

CRuNoise_Base *CRuFractal_Base::GetNoiseFunction()
{
	return m_noiseFunction;
}

BOOL CRuFractal_Base::SetNoiseFunction(CRuNoise_Base *noiseFunction)
{
	// Set new noise function
	noiseFunction->AddRef();
	ruSAFE_RELEASE(m_noiseFunction);
	m_noiseFunction = noiseFunction;

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
