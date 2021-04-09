#include "../Mojo/RuMojo_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuMojo_Operator_Base, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuMojo_Operator_Base, "CRuMojo_Operator_Base", "IRuObject")
ruCLASSGUID_DEFINE(CRuMojo_Operator_Base, 0x2F5D434E, 0xC4FB487d, 0x8CDDF748, 0x942EEA97)

ruRTTI_DEFINE_SUBCLASS(CRuMojo_Operator_Erosion, CRuMojo_Operator_Base)
ruHEAP_DEFINE_SUBCLASS(CRuMojo_Operator_Erosion, "CRuMojo_Operator_Erosion", "CRuMojo_Operator_Base")
ruCLASSGUID_DEFINE(CRuMojo_Operator_Erosion, 0x62BF8A5C, 0x20294deb, 0xA888C03A, 0x65AD7C14)

// ************************************************************************************************************************************************************

CRuMojo_Operator_Base::~CRuMojo_Operator_Base()
{
}

// ************************************************************************************************************************************************************

CRuMojo_Operator_Erosion::CRuMojo_Operator_Erosion()
:	m_attributeField(NULL),
	m_attributeField0(NULL),
	m_attributeField1(NULL),
	m_waterDensityField0(NULL),
	m_waterDensityField1(NULL),
	m_sedimentDensityField0(NULL),
	m_sedimentDensityField1(NULL)
{
	m_currentIteration = 0;
}

CRuMojo_Operator_Erosion::~CRuMojo_Operator_Erosion()
{
	ruSAFE_RELEASE(m_attributeField);

	delete[] m_attributeField0;
	delete[] m_attributeField1;
	delete[] m_waterDensityField0;
	delete[] m_waterDensityField1;
	delete[] m_sedimentDensityField0;
	delete[] m_sedimentDensityField1;
}

BOOL CRuMojo_Operator_Erosion::SetProperty(const char *propertyName, IRuObject *propertyValue)
{
	if(strcmp(propertyName, "parcel") == 0 && propertyValue->GetType() == CRuMojo_AttributeParcel<REAL>::Type())
	{
		// Store reference to height field
		ruSAFE_RELEASE(m_attributeField);
		m_attributeField = static_cast<CRuMojo_AttributeParcel<REAL> *>(propertyValue);
		m_attributeField->AddRef();

		// Initialize workspaces
		delete[] m_attributeField0;
		m_attributeField0 = ruNEW REAL [m_attributeField->GetParcelWidth() * m_attributeField->GetParcelHeight()];

		delete[] m_attributeField1;
		m_attributeField1 = ruNEW REAL [m_attributeField->GetParcelWidth() * m_attributeField->GetParcelHeight()];

		delete[] m_waterDensityField0;
		m_waterDensityField0 = ruNEW REAL [m_attributeField->GetParcelWidth() * m_attributeField->GetParcelHeight()];

		delete[] m_waterDensityField1;
		m_waterDensityField1 = ruNEW REAL [m_attributeField->GetParcelWidth() * m_attributeField->GetParcelHeight()];

		delete[] m_sedimentDensityField0;
		m_sedimentDensityField0 = ruNEW REAL [m_attributeField->GetParcelWidth() * m_attributeField->GetParcelHeight()];

		delete[] m_sedimentDensityField1;
		m_sedimentDensityField1 = ruNEW REAL [m_attributeField->GetParcelWidth() * m_attributeField->GetParcelHeight()];

		return TRUE;
	}

	return FALSE;
}

BOOL CRuMojo_Operator_Erosion::Execute()
{
	if(m_attributeField)
	{
		INT32 hfWidth = m_attributeField->GetParcelWidth();
		INT32 hfHeight = m_attributeField->GetParcelHeight();

		REAL depositDensity = 0.002f;
		REAL dropChance = 1.50f;
		INT32 numIterations = 5;

		REAL Kc = 1.0f;			// Sediment capacity constant
		REAL Kd = 0.2f;			// Deposition constant
		REAL Ks = 0.2f;			// Soil softness constant

		if(m_currentIteration == 0)
		{
			memset(m_attributeField0, 0, sizeof(REAL) * hfWidth * hfHeight);
			memcpy(m_attributeField0, m_attributeField->GetParcel(), sizeof(REAL) * hfWidth * hfHeight);
			memcpy(m_attributeField1, m_attributeField0, sizeof(REAL) * hfWidth * hfHeight);
			memset(m_waterDensityField0, 0, sizeof(REAL) * hfWidth * hfHeight);
			memset(m_waterDensityField1, 0, sizeof(REAL) * hfWidth * hfHeight);
			memset(m_sedimentDensityField0, 0, sizeof(REAL) * hfWidth * hfHeight);
			memset(m_sedimentDensityField1, 0, sizeof(REAL) * hfWidth * hfHeight);

		}

			// Deposit water at random
			for(INT32 i = 0; i < hfWidth * hfHeight; ++i)
			{
				m_waterDensityField0[i] += (RuGlobalMTRand().GenerateFloat() < dropChance) ? 1.5f : 0.0f;
//				m_waterDensityField0[i] += (RuGlobalMTRand().GenerateFloat() < dropChance) ? 1.5f * m_attributeField0[i] : 0.0f;
			}

		for(INT32 iteration = 0; iteration < numIterations; ++iteration, ++m_currentIteration)
		{
			// Run simulation once
			for(INT32 y = 0; y < hfHeight; ++y)
			{
				for(INT32 x = 0; x < hfWidth; ++x)
				{
/*					if(iteration + 1 == numIterations)
					{
						INT32 centerIndex = x + y * hfWidth;
						m_attributeField0[centerIndex] += m_sedimentDensityField0[centerIndex];
					}
					else*/
					{
						INT32 centerIndex = x + y * hfWidth;
						REAL centerOverallLevel = m_attributeField0[centerIndex] + m_waterDensityField0[centerIndex];

						CRuArrayList<NeighborDesc> validNeighbors;

						// First pass
						REAL totalHeightDiff = 0.0f;
						for(INT32 j = -1; j <= 1; ++j)
						{
							for(INT32 i = -1; i <= 1; ++i)
							{
								INT32 neighborIndex = min(max(0, x + i), hfWidth - 1) + min(max(0, y + j), hfHeight - 1) * hfWidth;

								// Skip if we are looking at ourselves
								if(neighborIndex == centerIndex)
									continue;

								// Calculate overall altitude and height difference from center
								REAL neighborOverallLevel = m_attributeField0[neighborIndex] + m_waterDensityField0[neighborIndex];
								REAL heightDiff = centerOverallLevel - neighborOverallLevel;

								// Neighbor is valid for processing only its height is lower than the center
								if(heightDiff > 0.0f)
								{
									// Add neighbor to list of valid neighbors
									validNeighbors.Add(NeighborDesc(neighborIndex));
									validNeighbors[validNeighbors.Count() - 1].m_apHeightDiff = heightDiff;

									// Accumulate into total height delta
									totalHeightDiff += heightDiff;
								}
							}
						}

						// Second pass - calculate dw and sediment density
						for(INT32 i = 0; i < validNeighbors.Count(); ++i)
						{
							validNeighbors[i].m_apHeightRatio = validNeighbors[i].m_apHeightDiff / totalHeightDiff;
							validNeighbors[i].m_dw = min(m_waterDensityField0[centerIndex], validNeighbors[i].m_apHeightDiff) * validNeighbors[i].m_apHeightRatio;
							validNeighbors[i].m_sedimentDensity = m_sedimentDensityField0[centerIndex] * validNeighbors[i].m_apHeightRatio;
						}

						// Third pass - make changes to data fields
						for(INT32 i = 0; i < validNeighbors.Count(); ++i)
						{
							// Move water from center to neighbor
							m_waterDensityField0[centerIndex] -= validNeighbors[i].m_dw;
							m_waterDensityField0[validNeighbors[i].m_index] += validNeighbors[i].m_dw;

							// Calculate sediment capacity in dw
							REAL dwSedimentCapacity = Kc * validNeighbors[i].m_dw;

							if(validNeighbors[i].m_sedimentDensity >= dwSedimentCapacity)
							{
								// Sediment density at center is larger than what the water transfer can carry, so transfer the maximum sediment density.
								m_sedimentDensityField0[validNeighbors[i].m_index] += dwSedimentCapacity;

								m_sedimentDensityField0[centerIndex] -= dwSedimentCapacity;
							}
							else
							{
								REAL newlyDissolvedSedimentDensity = Ks * (dwSedimentCapacity - validNeighbors[i].m_sedimentDensity);

								// Transfer entire sediment load into neighbor plus newly dissolved sediment from center.
								m_sedimentDensityField0[validNeighbors[i].m_index] += validNeighbors[i].m_sedimentDensity + newlyDissolvedSedimentDensity;

								// Adjust height field at center
								m_attributeField0[centerIndex] -= newlyDissolvedSedimentDensity;

								// Adjust remaining sediment at center
								m_sedimentDensityField0[centerIndex] -= validNeighbors[i].m_sedimentDensity;
							}
						}

						// If there has been no water movement...
						if(validNeighbors.Count() == 0)
						{
							REAL depositQuantity = Kd * m_sedimentDensityField0[centerIndex];
							m_attributeField0[centerIndex] += depositQuantity;
							m_sedimentDensityField0[centerIndex] -= depositQuantity;
						}
/*
						// Evaporation
						m_waterDensityField0[centerIndex] -= 0.1f;
						if(m_waterDensityField0[centerIndex] < 0.0f)
						{
							m_waterDensityField0[centerIndex] = 0.0f;
						}
*/
						REAL maxSedimentDensity = m_waterDensityField0[centerIndex] * Kc;

						if(m_sedimentDensityField0[centerIndex] > maxSedimentDensity)
						{
							REAL overflow = m_sedimentDensityField0[centerIndex] - maxSedimentDensity;
							m_attributeField0[centerIndex] += overflow;
							m_sedimentDensityField0[centerIndex] -= overflow;
						}
					}
				}
			}
		}

		memcpy(m_attributeField->GetParcel(), m_attributeField0, sizeof(REAL) * hfWidth * hfHeight);
	}

	return TRUE;
}

REAL *CRuMojo_Operator_Erosion::GetRepresentation() { return m_waterDensityField0; }
//REAL *CRuMojo_Operator_Erosion::GetRepresentation() { return m_sedimentDensityField0; }

// ************************************************************************************************************************************************************

#pragma managed
