#ifndef _RUMOJO_BASE_
#define _RUMOJO_BASE_

// Base types
#include "../Core/Type/RuType_Object.h"
#include "../Core/Math/RuMathVector.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

template<class AttributeType>
class CRuMojo_AttributeParcel : public IRuObject
{
	ruRTTI_DECLARE

protected:
	// Height field positional setting
	CRuVector3									m_apOrigin;					//!< Height field origin
	REAL										m_apSpacing;				//!< Spacing between each height field row/column

	// Relational position setting
	INT32										m_apOverlapGap;				//!< Size of overlap with neighboring height fields, in number of rows

	// Height field dimensions
	INT32										m_apWidth;					//!< Height field width
	INT32										m_apHeight;					//!< Height field height

	// Height field data
	AttributeType*								m_attributeField;			//!< Height field data

public:
												CRuMojo_AttributeParcel();
												CRuMojo_AttributeParcel(INT32 width, INT32 height);
	virtual										~CRuMojo_AttributeParcel();

	// IRuObject interface
	RUAPI virtual IRuObject*					Clone(IRuObject *clonedObject = NULL);			//!< Clones the object
	RUAPI virtual BOOL							SerializeFrom(IRuStream *stream);
	RUAPI virtual BOOL							SerializeTo(IRuStream *stream);

	// Height field parameters
	RUAPI virtual INT32							GetParcelWidth() const;
	RUAPI virtual INT32							GetParcelHeight() const;
	RUAPI virtual AttributeType*				GetParcel();

	// Other stuff
	RUAPI virtual const CRuVector3&				GetParcelOrigin() const;
	RUAPI virtual void							SetParcelOrigin(const CRuVector3 &hfOrigin);
	RUAPI virtual REAL							GetParcelSpacing() const;
	RUAPI virtual void							SetParcelSpacing(REAL hfSpacing);
	RUAPI virtual INT32							GetParcelOverlapGap() const;
	RUAPI virtual void							SetParcelOverlapGap(INT32 hfOverlapGap);

	RUAPI BOOL									Stitch(INT32 neighborID, CRuMojo_AttributeParcel<AttributeType> *neighborAP);
};

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS_TEMPLATE(CRuMojo_AttributeParcel, AttributeType, IRuObject)

// ************************************************************************************************************************************************************

template<class AttributeType>
CRuMojo_AttributeParcel<AttributeType>::CRuMojo_AttributeParcel()
{
	m_apSpacing = 1.0f;
	m_apOverlapGap = 0;

	m_apWidth = 0;
	m_apHeight = 0;

	m_attributeField = NULL;
}

template<class AttributeType>
CRuMojo_AttributeParcel<AttributeType>::CRuMojo_AttributeParcel(INT32 width, INT32 height)
{
	m_apSpacing = 1.0f;
	m_apOverlapGap = 0;

	m_apWidth = width;
	m_apHeight = height;

	m_attributeField = ruNEW AttributeType [m_apWidth * m_apHeight];
}

template<class AttributeType>
CRuMojo_AttributeParcel<AttributeType>::~CRuMojo_AttributeParcel()
{
	delete[] m_attributeField;
}

template<class AttributeType>
IRuObject *CRuMojo_AttributeParcel<AttributeType>::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuMojo_AttributeParcel(m_apWidth, m_apHeight);
	CRuMojo_AttributeParcel *clonedHF = static_cast<CRuMojo_AttributeParcel *>(clonedObject);

	// Chain cloning call to parent class
	IRuObject::Clone(clonedObject);

	clonedHF->m_apOrigin = m_apOrigin;
	clonedHF->m_apSpacing = m_apSpacing;
	clonedHF->m_apOverlapGap = m_apOverlapGap;

	// Copy height field
	memcpy(clonedHF->m_attributeField, m_attributeField, sizeof(AttributeType) * m_apWidth * m_apHeight);

	return clonedObject;
}

template<class AttributeType>
BOOL CRuMojo_AttributeParcel<AttributeType>::SerializeFrom(IRuStream *stream)
{
	// Read field origin
	stream->Read(&m_apOrigin.m_x, sizeof(REAL));
	stream->Read(&m_apOrigin.m_y, sizeof(REAL));
	stream->Read(&m_apOrigin.m_z, sizeof(REAL));

	// Read parcel spacing
	stream->Read(&m_apSpacing, sizeof(REAL));

	// Read overlap gap
	stream->Read(&m_apOverlapGap, sizeof(INT32));

	// Read dimensions
	stream->Read(&m_apWidth, sizeof(INT32));
	stream->Read(&m_apHeight, sizeof(INT32));

	// Initialize parcel data
	delete[] m_attributeField;
	m_attributeField = ruNEW AttributeType [m_apWidth * m_apHeight];

	// Read attribute values
	stream->Read(m_attributeField, sizeof(AttributeType) * m_apWidth * m_apHeight);

	return TRUE;
}
/*
template<>
BOOL CRuMojo_AttributeParcel<REAL>::SerializeFrom(IRuStream *stream)
{
	// Read field origin
	stream->Read(&m_apOrigin.m_x, sizeof(REAL));
	stream->Read(&m_apOrigin.m_y, sizeof(REAL));
	stream->Read(&m_apOrigin.m_z, sizeof(REAL));

	// Read parcel spacing
	stream->Read(&m_apSpacing, sizeof(REAL));

	// Read overlap gap
	stream->Read(&m_apOverlapGap, sizeof(INT32));

	// Read dimensions
	stream->Read(&m_apWidth, sizeof(INT32));
	stream->Read(&m_apHeight, sizeof(INT32));

	// Read minima
	REAL minima;
	stream->Read(&minima, sizeof(REAL));

	// Read maxima
	REAL maxima;
	stream->Read(&maxima, sizeof(REAL));

	// Read quantized height values
	INT32 numHeightValues = m_apWidth * m_apHeight;
	UINT16 *quantizedHFData = ruNEW UINT16 [numHeightValues];
	stream->Read(quantizedHFData, sizeof(UINT16) * numHeightValues);

	// De-quantize height values
	REAL heightRange = maxima - minima;
	for(INT32 i = 0; i < numHeightValues; ++i)
	{
		m_attributeField[i] = (quantizedHFData[i] / 60000.0f) * heightRange + minima;
	}

	// Release temporaries
	delete[] quantizedHFData;

	return TRUE;
}
*/
template<class AttributeType>
BOOL CRuMojo_AttributeParcel<AttributeType>::SerializeTo(IRuStream *stream)
{
	// Write field origin
	stream->Write(&m_apOrigin.m_x, sizeof(REAL));
	stream->Write(&m_apOrigin.m_y, sizeof(REAL));
	stream->Write(&m_apOrigin.m_z, sizeof(REAL));

	// Write parcel spacing
	stream->Write(&m_apSpacing, sizeof(REAL));

	// Write overlap gap
	stream->Write(&m_apOverlapGap, sizeof(INT32));

	// Write dimensions
	stream->Write(&m_apWidth, sizeof(INT32));
	stream->Write(&m_apHeight, sizeof(INT32));

	// Write attribute values
	stream->Write(m_attributeField, sizeof(AttributeType) * m_apWidth * m_apHeight);

	return TRUE;
}
/*
template<>
BOOL CRuMojo_AttributeParcel<REAL>::SerializeTo(IRuStream *stream)
{
	INT32 numHeightValues = m_apWidth * m_apHeight;

	// Write field origin
	stream->Write(&m_apOrigin.m_x, sizeof(REAL));
	stream->Write(&m_apOrigin.m_y, sizeof(REAL));
	stream->Write(&m_apOrigin.m_z, sizeof(REAL));

	// Write parcel spacing
	stream->Write(&m_apSpacing, sizeof(REAL));

	// Write overlap gap
	stream->Write(&m_apOverlapGap, sizeof(INT32));

	// Write dimensions
	stream->Write(&m_apWidth, sizeof(INT32));
	stream->Write(&m_apHeight, sizeof(INT32));

	// Scan through height field parcel to find minima and maxima
	REAL minima = FLT_MAX;
	REAL maxima = -FLT_MAX;

	for(INT32 i = 0; i < numHeightValues; ++i)
	{
		minima = min(minima, m_attributeField[i]);
		maxima = max(maxima, m_attributeField[i]);
	}

	// Quantize height values into 16-bit integers
	REAL heightRange = maxima - minima;
	UINT16 *quantizedHFData = ruNEW UINT16 [numHeightValues];
	for(INT32 i = 0; i < numHeightValues; ++i)
	{
		quantizedHFData[i] = (UINT16) (((m_attributeField[i] - minima) / heightRange) * 60000.0f);
	}

	// Write minima
	stream->Write(&minima, sizeof(REAL));

	// Write maxima
	stream->Write(&maxima, sizeof(REAL));

	// Write quantized height values
	stream->Write(quantizedHFData, sizeof(UINT16) * numHeightValues);

	// Release temporaries
	delete[] quantizedHFData;

	return TRUE;
}
*/
template<class AttributeType>
INT32 CRuMojo_AttributeParcel<AttributeType>::GetParcelWidth() const
{
	return m_apWidth;
}

template<class AttributeType>
INT32 CRuMojo_AttributeParcel<AttributeType>::GetParcelHeight() const
{
	return m_apHeight;
}

template<class AttributeType>
AttributeType *CRuMojo_AttributeParcel<AttributeType>::GetParcel()
{
	return m_attributeField;
}

template<class AttributeType>
const CRuVector3 &CRuMojo_AttributeParcel<AttributeType>::GetParcelOrigin() const
{
	return m_apOrigin;
}

template<class AttributeType>
void CRuMojo_AttributeParcel<AttributeType>::SetParcelOrigin(const CRuVector3 &hfOrigin)
{
	m_apOrigin = hfOrigin;
}

template<class AttributeType>
REAL CRuMojo_AttributeParcel<AttributeType>::GetParcelSpacing() const
{
	return m_apSpacing;
}

template<class AttributeType>
void CRuMojo_AttributeParcel<AttributeType>::SetParcelSpacing(REAL hfSpacing)
{
	m_apSpacing = hfSpacing;
}

template<class AttributeType>
INT32 CRuMojo_AttributeParcel<AttributeType>::GetParcelOverlapGap() const
{
	return m_apOverlapGap;
}

template<class AttributeType>
void CRuMojo_AttributeParcel<AttributeType>::SetParcelOverlapGap(INT32 hfOverlapGap)
{
	m_apOverlapGap = hfOverlapGap;
}

template<class AttributeType>
BOOL CRuMojo_AttributeParcel<AttributeType>::Stitch(INT32 neighborID, CRuMojo_AttributeParcel *neighborAP)
{
	/*
		Neighbor ID Defined
		-------------------

		| 0 | 1 | 2 |
		-------------
		| 3 | X | 4 |
		-------------
		| 5 | 6 | 7 |

		Neighbor ID is defined as an integer, specifying the spatial location
		relative to the current chunk. For instance, 0 specifies the neighboring
		chunk at (-1, -1). 
	*/

	INT32 overlapRegionSize = m_apOverlapGap + 1;
//	INT32 overlapRegionSize = m_apOverlapGap * 2 + 1;

	INT32 xdMin = m_apOverlapGap;
	INT32 xdMax = m_apWidth - 1;
	INT32 zdMin = m_apOverlapGap;
	INT32 zdMax = m_apHeight - 1;

	INT32 xsMin = m_apOverlapGap * 2;
	INT32 xsMax = m_apWidth - 1 - m_apOverlapGap;
	INT32 zsMin = m_apOverlapGap * 2;
	INT32 zsMax = m_apHeight - 1 - m_apOverlapGap;
/*
	INT32 xMin = overlapRegionSize - 1;
	INT32 xMax = m_apWidth - 1 - m_apOverlapGap;
	INT32 zMin = overlapRegionSize - 1;
	INT32 zMax = m_apHeight - 1 - m_apOverlapGap;
*/
	switch(neighborID)
	{
		case 0:
			{
				for(INT32 zS = zsMax, zD = zdMin, i = 0; i < overlapRegionSize; --zS, --zD, ++i)
				{
					for(INT32 xS = xsMax, xD = xdMin, j = 0; j < overlapRegionSize; --xS, --xD, ++j)
					{
						m_attributeField[zD * m_apWidth + xD] = neighborAP->m_attributeField[zS * m_apWidth + xS];
					}
				}
			}

			break;
		case 1:
			{
				for(INT32 zS = zsMax, zD = zdMin, i = 0; i < overlapRegionSize; --zS, --zD, ++i)
				{
					for(INT32 xS = xsMin, xD = xsMin, j = 0; j < m_apWidth - (m_apOverlapGap * 2); ++xS, ++xD, ++j)
					{
						m_attributeField[zD * m_apWidth + xD] = neighborAP->m_attributeField[zS * m_apWidth + xS];
					}
				}
			}

			break;
		case 2:
			{
				for(INT32 zS = zsMax, zD = zdMin, i = 0; i < overlapRegionSize; --zS, --zD, ++i)
				{
					for(INT32 xS = xsMin, xD = xdMax, j = 0; j < overlapRegionSize; --xS, --xD, ++j)
					{
						m_attributeField[zD * m_apWidth + xD] = neighborAP->m_attributeField[zS * m_apWidth + xS];
					}
				}
			}

			break;
		case 3:
			{
				for(INT32 xS = xsMax, xD = xdMin, i = 0; i < overlapRegionSize; --xS, --xD, ++i)
				{
					for(INT32 zS = zdMin, zD = zdMin, j = 0; j < m_apHeight - (m_apOverlapGap * 2); ++zS, ++zD, ++j)
					{
						m_attributeField[zD * m_apWidth + xD] = neighborAP->m_attributeField[zS * m_apWidth + xS];
					}
				}
			}

			break;
		case 4:
			{
				for(INT32 xS = xsMin, xD = xdMax, i = 0; i < overlapRegionSize; --xS, --xD, ++i)
				{
					for(INT32 zS = zdMin, zD = zdMin, j = 0; j < m_apHeight - (m_apOverlapGap * 2); ++zS, ++zD, ++j)
					{
						m_attributeField[zD * m_apWidth + xD] = neighborAP->m_attributeField[zS * m_apWidth + xS];
					}
				}
			}

			break;
		case 5:
			{
				for(INT32 zS = zsMin, zD = zdMax, i = 0; i < overlapRegionSize; --zS, --zD, ++i)
				{
					for(INT32 xS = xsMax, xD = xdMin, j = 0; j < overlapRegionSize; --xS, --xD, ++j)
					{
						m_attributeField[zD * m_apWidth + xD] = neighborAP->m_attributeField[zS * m_apWidth + xS];
					}
				}
			}

			break;
		case 6:
			{
				for(INT32 zS = zsMin, zD = zdMax, i = 0; i < overlapRegionSize; --zS, --zD, ++i)
				{
					for(INT32 xS = xsMin, xD = xsMin, j = 0; j < m_apWidth - (m_apOverlapGap * 2); ++xS, ++xD, ++j)
					{
						m_attributeField[zD * m_apWidth + xD] = neighborAP->m_attributeField[zS * m_apWidth + xS];
					}
				}
			}

			break;
		case 7:
			{
				for(INT32 zS = zsMin, zD = zdMax, i = 0; i < overlapRegionSize; --zS, --zD, ++i)
				{
					for(INT32 xS = xsMin, xD = xdMax, j = 0; j < overlapRegionSize; --xS, --xD, ++j)
					{
						m_attributeField[zD * m_apWidth + xD] = neighborAP->m_attributeField[zS * m_apWidth + xS];
					}
				}
			}

			break;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

class CRuMojo_Operator_Base : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual										~CRuMojo_Operator_Base();
};

// ************************************************************************************************************************************************************


class CRuMojo_Operator_Erosion : public CRuMojo_Operator_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct NeighborDesc
	{
		INT32									m_index;						//!< Index to neighbor
		REAL									m_apHeightDiff;
		REAL									m_apHeightRatio;
		REAL									m_dw;							//!< Density of water distributed to this neighbor
		REAL									m_sedimentDensity;				//!< Density of sediment distributed to this neighbor

												NeighborDesc(INT32 index = 0) : m_index(index) { }
	};

	CRuMojo_AttributeParcel<REAL>*				m_attributeField;

	INT32										m_currentIteration;

	// Scratch space
	REAL*										m_attributeField0;
	REAL*										m_attributeField1;
	REAL*										m_waterDensityField0;
	REAL*										m_waterDensityField1;
	REAL*										m_sedimentDensityField0;
	REAL*										m_sedimentDensityField1;

public:
												CRuMojo_Operator_Erosion();
	virtual										~CRuMojo_Operator_Erosion();

	virtual BOOL								SetProperty(const char *propertyName, IRuObject *propertyValue);
	virtual BOOL								Execute();

	REAL*										GetRepresentation();
};


// ************************************************************************************************************************************************************

#pragma managed

#endif
