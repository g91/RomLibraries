#include "../Mojo/RuMojo_Helper.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

BOOL Mojo_ResetHeight(CRuMojo_AttributeParcel<REAL> *hfParcel, REAL height)
{
	for(INT32 z = 0; z < hfParcel->GetParcelHeight(); ++z)
	{
		for(INT32 x = 0; x < hfParcel->GetParcelWidth(); ++x)
		{
			hfParcel->GetParcel()[x + z * hfParcel->GetParcelWidth()] = height;
		}
	}

	return TRUE;
}

BOOL Mojo_PaintHeight(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &center, REAL radius, REAL delta, REAL refHeight, BOOL adaptivePainting, RuFalloffFunction falloffFunction)
{
	// Get parcel data
	const CRuVector3 &apOrigin = hfParcel->GetParcelOrigin();
	REAL apSpacing = hfParcel->GetParcelSpacing();
	INT32 apWidth = hfParcel->GetParcelWidth();
	INT32 apHeight = hfParcel->GetParcelHeight();
	REAL *attributeField = hfParcel->GetParcel();

	// Figure out the area to alter
	CRuVector3 relativeCenter = center - apOrigin;

	INT32 sX = max(0, (INT32) ((relativeCenter.x - radius) / apSpacing));
	INT32 sZ = max(0, (INT32) ((relativeCenter.z - radius) / apSpacing));
	INT32 eX = min(apWidth - 1, (INT32) ((relativeCenter.x + radius) / apSpacing + 1));
	INT32 eZ = min(apHeight - 1, (INT32) ((relativeCenter.z + radius) / apSpacing + 1));

	REAL brushStrength = delta;

	refHeight += brushStrength * falloffFunction(0.0f, radius);

	// Check brush against height field
	for(INT32 z = sZ; z < eZ; ++z)
	{
		for(INT32 x = sX; x < eX; ++x)
		{
			CRuVector3 pointPos(apOrigin.x + x * apSpacing, center.y, apOrigin.z + z * apSpacing);

			REAL distance = (center - pointPos).Magnitude();

			if(distance <= radius)
			{
				REAL heightDelta = brushStrength * falloffFunction(distance, radius);

				if(heightDelta >= 0.0f)
				{
					if(!adaptivePainting || attributeField[x + z * apWidth] <= refHeight)
					{
						// Alter height field value
						attributeField[x + z * apWidth] += heightDelta;

					}
				}
				else
				{
					if(!adaptivePainting || attributeField[x + z * apWidth] >= refHeight)
					{
						// Alter height field value
						attributeField[x + z * apWidth] += heightDelta;
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL Mojo_NoiseHeight(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &center, REAL radius, REAL variation)
{
	// Get parcel data
	const CRuVector3 &apOrigin = hfParcel->GetParcelOrigin();
	REAL apSpacing = hfParcel->GetParcelSpacing();
	INT32 apWidth = hfParcel->GetParcelWidth();
	INT32 apHeight = hfParcel->GetParcelHeight();
	REAL *attributeField = hfParcel->GetParcel();

	// Figure out the area to alter
	CRuVector3 relativeCenter = center - apOrigin;

	INT32 sX = max(0, (INT32) ((relativeCenter.x - radius) / apSpacing));
	INT32 sZ = max(0, (INT32) ((relativeCenter.z - radius) / apSpacing));
	INT32 eX = min(apWidth - 1, (INT32) ((relativeCenter.x + radius) / apSpacing + 1));
	INT32 eZ = min(apHeight - 1, (INT32) ((relativeCenter.z + radius) / apSpacing + 1));

	// Check brush against height field
	for(INT32 z = sZ; z < eZ; ++z)
	{
		for(INT32 x = sX; x < eX; ++x)
		{
			CRuVector3 pointPos(apOrigin.x + x * apSpacing, center.y, apOrigin.z + z * apSpacing);

			REAL distance = (center - pointPos).Magnitude();

			if(distance <= radius)
			{
				// Alter height field value
				attributeField[x + z * apWidth] += (RuGlobalMTRand().GenerateFloat() - 0.5f) * variation;
			}
		}
	}

	return TRUE;
}

BOOL Mojo_SetHeight(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &center, float radius, float height, float brushStrength, RuFalloffFunction falloffFunction)
{
	// Get parcel data
	const CRuVector3 &apOrigin = hfParcel->GetParcelOrigin();
	REAL apSpacing = hfParcel->GetParcelSpacing();
	INT32 apWidth = hfParcel->GetParcelWidth();
	INT32 apHeight = hfParcel->GetParcelHeight();
	REAL *attributeField = hfParcel->GetParcel();

	// Figure out the area to alter
	CRuVector3 relativeCenter = center - apOrigin;

	INT32 sX = max(0, (INT32) ((relativeCenter.x - radius) / apSpacing));
	INT32 sZ = max(0, (INT32) ((relativeCenter.z - radius) / apSpacing));
	INT32 eX = min(apWidth - 1, (INT32) ((relativeCenter.x + radius) / apSpacing + 1));
	INT32 eZ = min(apHeight - 1, (INT32) ((relativeCenter.z + radius) / apSpacing + 1));

	if(falloffFunction)
	{
		if(brushStrength < 0.0f)
		{
			brushStrength = -brushStrength;

			// Check brush against height field
			for(INT32 z = sZ; z < eZ; ++z)
			{
				for(INT32 x = sX; x < eX; ++x)
				{
					CRuVector3 pointPos(apOrigin.x + x * apSpacing, center.y, apOrigin.z + z * apSpacing);

					float distance = (center - pointPos).Magnitude();

					if(distance <= radius)
					{
						// Alter height field value
						REAL diff = height - attributeField[x + z * apWidth];

						if(diff > 0.0f)
						{
							attributeField[x + z * apWidth] += diff * brushStrength * 0.05f;
							attributeField[x + z * apWidth] = min(attributeField[x + z * apWidth], height);
						}
					}
				}
			}
		}
		else
		{
			// Check brush against height field
			for(INT32 z = sZ; z < eZ; ++z)
			{
				for(INT32 x = sX; x < eX; ++x)
				{
					CRuVector3 pointPos(apOrigin.x + x * apSpacing, center.y, apOrigin.z + z * apSpacing);

					float distance = (center - pointPos).Magnitude();

					if(distance <= radius)
					{
						// Alter height field value
						REAL diff = height - attributeField[x + z * apWidth];

						if(diff > 0.0f)
						{
							attributeField[x + z * apWidth] += brushStrength * falloffFunction(distance, radius);
							attributeField[x + z * apWidth] = min(attributeField[x + z * apWidth], height);
						}
					}
				}
			}
		}
	}
	else
	{
		// Check brush against height field
		for(INT32 z = sZ; z < eZ; ++z)
		{
			for(INT32 x = sX; x < eX; ++x)
			{
				CRuVector3 pointPos(apOrigin.x + x * apSpacing, center.y, apOrigin.z + z * apSpacing);

				float distance = (center - pointPos).Magnitude();

				if(distance <= radius)
				{
					// Alter height field value
					attributeField[x + z * apWidth] = height;
				}
			}
		}
	}

	return TRUE;
}

BOOL Mojo_SmoothHeight(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &center, float radius, float delta)
{
	// Get parcel data
	const CRuVector3 &apOrigin = hfParcel->GetParcelOrigin();
	REAL apSpacing = hfParcel->GetParcelSpacing();
	INT32 apWidth = hfParcel->GetParcelWidth();
	INT32 apHeight = hfParcel->GetParcelHeight();
	REAL *attributeField = hfParcel->GetParcel();

	// Figure out the area to alter
	CRuVector3 relativeCenter = center - apOrigin;

	INT32 sX = max(1, (INT32) ((relativeCenter.x - radius) / apSpacing));
	INT32 sZ = max(1, (INT32) ((relativeCenter.z - radius) / apSpacing));
	INT32 eX = min(apWidth - 1, (INT32) ((relativeCenter.x + radius) / apSpacing + 1));
	INT32 eZ = min(apHeight - 1, (INT32) ((relativeCenter.z + radius) / apSpacing + 1));

	float erosionRate = 0.0001f * delta;
	float weights[3] = { erosionRate, 1.0f - (2.0f * erosionRate), erosionRate };
	float *smoothedValues = ruNEW float [max(apWidth, apHeight)];

	// Smooth along x axis
	for(INT32 z = sZ; z < eZ; ++z)
	{
		// Calculate smoothed values and update into the smoothedValues array
		for(INT32 x = sX; x < eX; ++x)
		{
			CRuVector3 pointPos(apOrigin.x + x * apSpacing, center.y, apOrigin.z + z * apSpacing);

			float distance = (center - pointPos).Magnitude();

			if(distance <= radius)
			{
				float height = 0.0f;

				for(INT32 a = -1; a < 2; ++a)
				{
					height += attributeField[(x + a) + z * apWidth] * weights[a + 1];
				}

				smoothedValues[x] = height;
			}
			else
			{
				smoothedValues[x] = attributeField[x + z * apWidth];
			}
		}

		// Update smoothed values
		for(INT32 i = sX; i < eX; ++i)
		{
			attributeField[i + z * apWidth] = smoothedValues[i];
		}
	}

	// Smooth along z axis
	for(INT32 x = sX; x < eX; ++x)
	{
		// Calculate smoothed values and update into the smoothedValues array
		for(INT32 z = sZ; z < eZ; ++z)
		{
			CRuVector3 pointPos(apOrigin.x + x * apSpacing, center.y, apOrigin.z + z * apSpacing);

			float distance = (center - pointPos).Magnitude();

			if(distance <= radius)
			{
				float height = 0.0f;

				for(INT32 a = -1; a < 2; ++a)
				{
					height += attributeField[x + (z + a) * apWidth] * weights[a + 1];
				}

				smoothedValues[z] = height;
			}
			else
			{
				smoothedValues[z] = attributeField[x + z * apWidth];
			}
		}

		// Update smoothed values
		for(INT32 i = sZ; i < eZ; ++i)
		{
			attributeField[x + i * apWidth] = smoothedValues[i];
		}
	}

	delete[] smoothedValues;

	return TRUE;
}

BOOL Mojo_OffsetHeight(CRuMojo_AttributeParcel<REAL> *hfParcel, REAL offset)
{
	// Get parcel data
	const CRuVector3 &apOrigin = hfParcel->GetParcelOrigin();
	REAL apSpacing = hfParcel->GetParcelSpacing();
	INT32 apWidth = hfParcel->GetParcelWidth();
	INT32 apHeight = hfParcel->GetParcelHeight();
	REAL *attributeField = hfParcel->GetParcel();

	INT32 sX = 0;
	INT32 sZ = 0;
	INT32 eX = apWidth;
	INT32 eZ = apHeight;

	for(INT32 z = sZ; z < eZ; ++z)
	{
		for(INT32 x = sX; x < eX; ++x)
		{
			// Alter height field value
			attributeField[x + z * apWidth] = attributeField[x + z * apWidth] + offset;
		}
	}

	return TRUE;
}

CRuMesh *Mojo_BuildMeshFromHeightField(CRuMojo_AttributeParcel<REAL> *hfParcel, CRuMojo_AttributeParcel<RUCOLOR> *cfParcel, CRuMojo_AttributeParcel<BYTE> *flagParcel, BYTE flagValue)
{
	INT32 vertexGap = 1;

	// Get height field parcel data
	const CRuVector3 &hfOrigin = hfParcel->GetParcelOrigin();
	REAL hfSpacing = hfParcel->GetParcelSpacing();
	INT32 hfWidth = hfParcel->GetParcelWidth();
	INT32 hfHeight = hfParcel->GetParcelHeight();
	INT32 hfOverlapGap = hfParcel->GetParcelOverlapGap();
	REAL *heightField = hfParcel->GetParcel();

	// Get height field parcel data
	const CRuVector3 &cfOrigin = cfParcel->GetParcelOrigin();
	REAL cfSpacing = cfParcel->GetParcelSpacing();
	INT32 cfWidth = cfParcel->GetParcelWidth();
	INT32 cfHeight = cfParcel->GetParcelHeight();
	INT32 cfOverlapGap = cfParcel->GetParcelOverlapGap();
	RUCOLOR *colorField = cfParcel->GetParcel();

	// Get flag parcel data
	const CRuVector3 &fpOrigin = flagParcel->GetParcelOrigin();
	REAL fpSpacing = flagParcel->GetParcelSpacing();
	INT32 fpWidth = flagParcel->GetParcelWidth();
	INT32 fpHeight = flagParcel->GetParcelHeight();
	INT32 fpOverlapGap = flagParcel->GetParcelOverlapGap();
	BYTE *attributeField = flagParcel->GetParcel();

	// Calculate dimensions of mesh grid
	INT32 fieldGridWidth = (hfWidth - hfOverlapGap * 2) - 1;
	INT32 fieldGridHeight = (hfHeight - hfOverlapGap * 2) - 1;

	// Create temporary holding arrays for mesh data
	CRuVector3 *vertexArray = ruNEW CRuVector3 [(hfWidth - hfOverlapGap * 2) * (hfHeight - hfOverlapGap * 2)];
	RUCOLOR *diffuseArray = ruNEW RUCOLOR [(hfWidth - hfOverlapGap * 2) * (hfHeight - hfOverlapGap * 2)];
	UINT16 *indexArray = ruNEW UINT16 [fieldGridWidth * fieldGridHeight * 6];

	// Copy mesh vertices
	for(INT32 z = 0, i = 0; z <= fieldGridHeight; ++z)
	{
		for(INT32 x = 0; x <= fieldGridWidth; ++x, ++i)
		{
			vertexArray[i].Set	(
									(x * hfSpacing * vertexGap) + (hfOrigin.m_x + hfSpacing * hfOverlapGap),
									heightField[(x * vertexGap) + hfOverlapGap + ((z * vertexGap) + hfOverlapGap) * hfWidth],
									(z * hfSpacing * vertexGap) + (hfOrigin.m_z + hfSpacing * hfOverlapGap)
								);
		}
	}

	// Set up diffuse color
	if(attributeField)
	{
		for(INT32 z = 0, i = 0; z <= fieldGridHeight; ++z)
		{
			for(INT32 x = 0; x <= fieldGridWidth; ++x, ++i)
			{
				REAL alpha = (REAL) (attributeField[(x * vertexGap) + fpOverlapGap + ((z * vertexGap) + fpOverlapGap) * fpWidth] & 0x3F) / 0x3F;
				diffuseArray[i] = RUCOLOR_ARGB((INT32) (alpha * 255.0f), 0x00, 0x00, 0x00) | (colorField[(x * vertexGap) + cfOverlapGap + ((z * vertexGap) + cfOverlapGap) * cfWidth] & 0x00FFFFFF);
			}
		}
	}

	// Build indices for the mesh body
	INT32 evenFaceConstruction[]	= { 0, 3, 4, 0, 4, 1, 1, 4, 2, 2, 4, 5 };
	INT32 oddFaceConstruction[]		= { 0, 3, 1, 1, 3, 4, 1, 5, 2, 1, 4, 5 };

	INT32 indexCount = 0;
	UINT16 indexSet[6];

	INT32 vaZStride = (fieldGridWidth + 1);

	for(INT32 z = 0; z < fieldGridHeight; ++z)
	{
		// Build indices for four faces at a time
		for(INT32 x = 0; x < fieldGridWidth; x += 2)
		{
			// Calculate the indices used in these four faces
			indexSet[0] = x + z * vaZStride;
			indexSet[1] = indexSet[0] + vertexGap;
			indexSet[2] = indexSet[1] + vertexGap;
			indexSet[3] = indexSet[0] + vaZStride;
			indexSet[4] = indexSet[1] + vaZStride;
			indexSet[5] = indexSet[2] + vaZStride;

			// Alternate between even and odd rows
			INT32 *faceConstruction = (z % 2) ? oddFaceConstruction : evenFaceConstruction;

			BOOL visible = FALSE;
			for(INT32 j = 0; j < 12; ++j)
			{
				if(attributeField[indexSet[faceConstruction[j]]] & flagValue)
				{
					visible = TRUE;
				}
			}

			if(visible)
			{
				for(INT32 j = 0; j < 12; ++j)
				{
					indexArray[indexCount++] = indexSet[faceConstruction[j]];
				}
			}
		}
	}

	CRuMesh *mesh = ruNEW CRuMesh();
	mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, (fieldGridWidth + 1) * (fieldGridHeight + 1), indexCount / 3, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);

	// Copy vertices
	memcpy(static_cast<CRuMorphTarget *>(mesh->GetMorphTarget(0))->GetPosition(), vertexArray, sizeof(CRuVector3) * (fieldGridWidth + 1) * (fieldGridHeight + 1));

	// Copy diffuse
	memcpy(static_cast<CRuMorphTarget *>(mesh->GetMorphTarget(0))->GetDiffuse(), diffuseArray, sizeof(RUCOLOR) * (fieldGridWidth + 1) * (fieldGridHeight + 1));

	// Copy indices
	memcpy(mesh->GetIndices(), indexArray, sizeof(UINT16) * indexCount);

	// Recalculate bounds
	mesh->CalculateBounds();

	// Free resources
	delete[] vertexArray;
	delete[] diffuseArray;
	delete[] indexArray;

	return mesh;
}

BOOL Mojo_SerializeFrom(CRuMojo_AttributeParcel<REAL> **hfParcel, IRuStream *stream)
{
	// Read field origin
	CRuVector3 parcelOrigin;
	stream->Read(&parcelOrigin.m_x, sizeof(REAL));
	stream->Read(&parcelOrigin.m_y, sizeof(REAL));
	stream->Read(&parcelOrigin.m_z, sizeof(REAL));

	// Read parcel spacing
	REAL parcelSpacing;
	stream->Read(&parcelSpacing, sizeof(REAL));

	// Read overlap gap
	INT32 parcelOverlapGap;
	stream->Read(&parcelOverlapGap, sizeof(INT32));

	// Read dimensions
	INT32 parcelWidth;
	stream->Read(&parcelWidth, sizeof(INT32));

	INT32 parcelHeight;
	stream->Read(&parcelHeight, sizeof(INT32));

	// Initialize parcel
	*hfParcel = ruNEW CRuMojo_AttributeParcel<REAL>(parcelWidth, parcelHeight);
	(*hfParcel)->SetParcelOrigin(parcelOrigin);
	(*hfParcel)->SetParcelSpacing(parcelSpacing);
	(*hfParcel)->SetParcelOverlapGap(parcelOverlapGap);

	// Read minima
	REAL minima;
	stream->Read(&minima, sizeof(REAL));

	// Read maxima
	REAL maxima;
	stream->Read(&maxima, sizeof(REAL));

	// Read quantized height values
	INT32 numHeightValues = parcelWidth * parcelHeight;
	UINT16 *quantizedHFData = ruNEW UINT16 [numHeightValues];
	stream->Read(quantizedHFData, sizeof(UINT16) * numHeightValues);

	// De-quantize height values
	REAL heightRange = maxima - minima;
	REAL *hfData = (*hfParcel)->GetParcel();
	for(INT32 i = 0; i < numHeightValues; ++i)
	{
		hfData[i] = (quantizedHFData[i] / 60000.0f) * heightRange + minima;
	}

	// Release temporaries
	delete[] quantizedHFData;

	return TRUE;
}

BOOL Mojo_SerializeTo(CRuMojo_AttributeParcel<REAL> *hfParcel, IRuStream *stream)
{
	REAL *hfData = hfParcel->GetParcel();
	INT32 numHeightValues = hfParcel->GetParcelWidth() * hfParcel->GetParcelHeight();

	// Scan through height field parcel to find minima and maxima
	REAL minima = FLT_MAX;
	REAL maxima = -FLT_MAX;

	for(INT32 i = 0; i < numHeightValues; ++i)
	{
		minima = min(minima, hfData[i]);
		maxima = max(maxima, hfData[i]);
	}

	REAL heightRange = maxima - minima;

	// Quantize height values into 16-bit integers
	UINT16 *quantizedHFData = ruNEW UINT16 [numHeightValues];
	for(INT32 i = 0; i < numHeightValues; ++i)
	{
		quantizedHFData[i] = (UINT16) (((hfData[i] - minima) / heightRange) * 60000.0f);
	}

	// Write field origin
	stream->Write(&hfParcel->GetParcelOrigin().m_x, sizeof(REAL));
	stream->Write(&hfParcel->GetParcelOrigin().m_y, sizeof(REAL));
	stream->Write(&hfParcel->GetParcelOrigin().m_z, sizeof(REAL));

	// Write parcel spacing
	REAL parcelSpacing = hfParcel->GetParcelSpacing();
	stream->Write(&parcelSpacing, sizeof(REAL));

	// Write overlap gap
	INT32 parcelOverlapGap = hfParcel->GetParcelOverlapGap();
	stream->Write(&parcelOverlapGap, sizeof(INT32));

	// Write dimensions
	INT32 parcelWidth = hfParcel->GetParcelWidth();
	stream->Write(&parcelWidth, sizeof(INT32));

	INT32 parcelHeight = hfParcel->GetParcelHeight();
	stream->Write(&parcelHeight, sizeof(INT32));

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

REAL Mojo_GetHeightAtPoint(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &point)
{
	// Get parcel data
	const CRuVector3 &apOrigin = hfParcel->GetParcelOrigin();
	REAL apSpacing = hfParcel->GetParcelSpacing();
	INT32 apWidth = hfParcel->GetParcelWidth();
	INT32 apHeight = hfParcel->GetParcelHeight();
	REAL *attributeField = hfParcel->GetParcel();

	// Find point coordinates relative to height field
	CRuVector3 hfPoint = (point - apOrigin) / apSpacing;

	// Find four height field points enclosing the specified point
	INT32 iX0 = max(0, min(apWidth - 1, static_cast<INT32>(hfPoint.x)));
	INT32 iX1 = max(0, min(apWidth - 1, iX0 + 1));
	INT32 iZ0 = max(0, min(apHeight - 1, static_cast<INT32>(hfPoint.z)));
	INT32 iZ1 = max(0, min(apHeight - 1, iZ0 + 1));

	// Find weights along X and Z directions
	REAL xW1 = (hfPoint.x - iX0);
	REAL xW0 = 1.0f - xW1;
	REAL zW1 = (hfPoint.z - iZ0);
	REAL zW0 = 1.0f - zW1;

	// Interpolate along X
	REAL xH0 = attributeField[iX0 + iZ0 * apWidth] * xW0 + attributeField[iX1 + iZ0 * apWidth] * xW1;
	REAL xH1 = attributeField[iX0 + iZ1 * apWidth] * xW0 + attributeField[iX1 + iZ1 * apWidth] * xW1;

	// Interpolate along Z
	REAL interpolatedHeight = xH0 * zW0 + xH1 * zW1;

	return interpolatedHeight;
}

REAL Mojo_GetHeightAtPoint_Multisampled(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &point)
{
	// Get parcel data
	const CRuVector3 &apOrigin = hfParcel->GetParcelOrigin();
	REAL apSpacing = hfParcel->GetParcelSpacing();
	INT32 apWidth = hfParcel->GetParcelWidth();
	INT32 apHeight = hfParcel->GetParcelHeight();
	REAL *attributeField = hfParcel->GetParcel();

	// Find point coordinates relative to height field
	CRuVector3 hfPoint = (point - apOrigin) / apSpacing;

	// Find four height field points immediately enclosing the specified point
	INT32 iX[4], iZ[4];

	iX[1] = max(0, min(apWidth - 1, static_cast<INT32>(hfPoint.x)));
	iX[2] = max(0, min(apWidth - 1, iX[1] + 1));
	iX[0] = max(0, iX[1] - 1);
	iX[3] = min(apWidth - 1, iX[2] + 1);

	iZ[1] = max(0, min(apHeight - 1, static_cast<INT32>(hfPoint.z)));
	iZ[2] = max(0, min(apHeight - 1, iZ[1] + 1));
	iZ[0] = max(0, iZ[1] - 1);
	iZ[3] = min(apHeight - 1, iZ[2] + 1);

	// Find weights along X and Z directions
	REAL xW = (hfPoint.x - iX[1]);
	REAL zW = (hfPoint.z - iZ[1]);

	// Setup support matrix
	REAL hgPt[4][4];
	for(INT32 x = 0; x < 4; ++x)
	{
		for(INT32 z = 0; z < 4; ++z)
		{
			hgPt[x][z] = attributeField[iX[x] + iZ[z] * apWidth];
		}
	}

	// Interpolate along x direction to get control points along x
	REAL xcPt[4];
	for(INT32 x = 0; x < 4; ++x)
	{
		xcPt[x] = CatmullRomSpline(hgPt[x][0], hgPt[x][1], hgPt[x][2], hgPt[x][3], zW);
	}

	// Interpolate along z direction to get control points along z
	REAL zcPt[4];
	for(INT32 z = 0; z < 4; ++z)
	{
		zcPt[z] = CatmullRomSpline(hgPt[0][z], hgPt[1][z], hgPt[2][z], hgPt[3][z], xW);
	}

	// Interpolate along x control points to get solution wrt x
	REAL xPt = CatmullRomSpline(xcPt[0], xcPt[1], xcPt[2], xcPt[3], xW);

	// Interpolate along z control points to get solution wrt z
	REAL zPt = CatmullRomSpline(zcPt[0], zcPt[1], zcPt[2], zcPt[3], zW);

	// Return average of the results
	return (xPt + zPt) / 2.0f;
}

REAL Mojo_GetSlopeAtPoint(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &point)
{
	// Get parcel data
	const CRuVector3 &apOrigin = hfParcel->GetParcelOrigin();
	REAL apSpacing = hfParcel->GetParcelSpacing();
	INT32 apWidth = hfParcel->GetParcelWidth();
	INT32 apHeight = hfParcel->GetParcelHeight();
	REAL *attributeField = hfParcel->GetParcel();

	// Find point coordinates relative to height field
	CRuVector3 hfPoint = (point - apOrigin) / apSpacing;

	// Find four height field points enclosing the specified point
	INT32 iX0 = max(0, min(apWidth - 1, static_cast<INT32>(hfPoint.x)));
	INT32 iX1 = max(0, min(apWidth - 1, iX0 + 1));
	INT32 iZ0 = max(0, min(apHeight - 1, static_cast<INT32>(hfPoint.z)));
	INT32 iZ1 = max(0, min(apHeight - 1, iZ0 + 1));

	// Find weights along X and Z directions
	REAL xW1 = (hfPoint.x - iX0);
	REAL xW0 = 1.0f - xW1;
	REAL zW1 = (hfPoint.z - iZ0);
	REAL zW0 = 1.0f - zW1;

	// Interpolate along X
	REAL xH0 = attributeField[iX0 + iZ0 * apWidth] * xW0 + attributeField[iX1 + iZ0 * apWidth] * xW1;
	REAL xH1 = attributeField[iX0 + iZ1 * apWidth] * xW0 + attributeField[iX1 + iZ1 * apWidth] * xW1;

	// Interpolate along Z
	REAL zH0 = attributeField[iX0 + iZ0 * apWidth] * zW0 + attributeField[iX0 + iZ1 * apWidth] * zW1;
	REAL zH1 = attributeField[iX1 + iZ0 * apWidth] * zW0 + attributeField[iX1 + iZ1 * apWidth] * zW1;

	// Get quick estimate of normal at point
	CRuVector3 normal(zH0 - zH1, apSpacing * 2.0f, xH1 - xH0);
	normal.Normalize();

	return max(0.0f, 1.0f - DotProduct(CRuVector3(0.0f, 1.0f, 0.0f), normal));
}

REAL Mojo_GetSlopeAtPoint_Multisampled(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &point)
{
	// Get parcel data
	const CRuVector3 &apOrigin = hfParcel->GetParcelOrigin();
	REAL apSpacing = hfParcel->GetParcelSpacing();
	INT32 apWidth = hfParcel->GetParcelWidth();
	INT32 apHeight = hfParcel->GetParcelHeight();
	REAL *attributeField = hfParcel->GetParcel();

	REAL multisampleRange = 1.875f;

	REAL xH0 = Mojo_GetHeightAtPoint_Multisampled(hfParcel, point - CRuVector3(multisampleRange, 0.0f, 0.0f));
	REAL xH1 = Mojo_GetHeightAtPoint_Multisampled(hfParcel, point + CRuVector3(multisampleRange, 0.0f, 0.0f));

	REAL zH0 = Mojo_GetHeightAtPoint_Multisampled(hfParcel, point - CRuVector3(0.0f, 0.0f, multisampleRange));
	REAL zH1 = Mojo_GetHeightAtPoint_Multisampled(hfParcel, point + CRuVector3(0.0f, 0.0f, multisampleRange));

	// Get quick estimate of normal at point
//	CRuVector3 normal(zH0 - zH1, apSpacing * 2.0f, xH1 - xH0);
	CRuVector3 normal(zH0 - zH1, multisampleRange * 2.0f, xH1 - xH0);
	normal.Normalize();

	REAL slope = 0.0f;

	if(DotProduct(CRuVector3(0.0f, 1.0f, 0.0f), normal) < (1.0f - ruEPSILON))
	{
		CRuVector3 rotAxis = CrossProduct(normal, CRuVector3(0.0f, 1.0f, 0.0));
		rotAxis.Normalize();

		CalculateAngleDifferential(CRuVector3(0.0f, 1.0f, 0.0), normal, rotAxis, slope);

		slope = fabs(slope);
	}

	return slope;
}

REAL Mojo_GetValleyDepth_Multisampled(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &point)
{
	REAL depth = 0.0f;

	// Get parcel data
	const CRuVector3 &apOrigin = hfParcel->GetParcelOrigin();
	REAL apSpacing = hfParcel->GetParcelSpacing();
	INT32 apWidth = hfParcel->GetParcelWidth();
	INT32 apHeight = hfParcel->GetParcelHeight();
	REAL *attributeField = hfParcel->GetParcel();

	REAL multisampleRange = 1.875f;

	REAL xH0 = Mojo_GetHeightAtPoint_Multisampled(hfParcel, point - CRuVector3(multisampleRange, 0.0f, 0.0f));
	REAL xH1 = Mojo_GetHeightAtPoint_Multisampled(hfParcel, point + CRuVector3(multisampleRange, 0.0f, 0.0f));

	REAL zH0 = Mojo_GetHeightAtPoint_Multisampled(hfParcel, point - CRuVector3(0.0f, 0.0f, multisampleRange));
	REAL zH1 = Mojo_GetHeightAtPoint_Multisampled(hfParcel, point + CRuVector3(0.0f, 0.0f, multisampleRange));

	REAL h0 = Mojo_GetHeightAtPoint(hfParcel, point);

	depth = max(0.0f, (xH0 + xH1 + zH0 + zH1) * 0.25f - h0);

	return depth;
}

BOOL Mojo_ResetFlags(CRuMojo_AttributeParcel<BYTE> *parcel, BYTE flagValue, BYTE flagMask)
{
	for(INT32 z = 0; z < parcel->GetParcelHeight(); ++z)
	{
		for(INT32 x = 0; x < parcel->GetParcelWidth(); ++x)
		{
			parcel->GetParcel()[x + z * parcel->GetParcelWidth()] = (parcel->GetParcel()[x + z * parcel->GetParcelWidth()] & ~flagMask) | (flagValue & flagMask);;
		}
	}

	return TRUE;
}

BOOL Mojo_PaintFlags(CRuMojo_AttributeParcel<BYTE> *parcel, const CRuVector3 &center, REAL radius, BYTE flagValue, BYTE flagMask)
{
	// Get parcel data
	const CRuVector3 &apOrigin = parcel->GetParcelOrigin();
	REAL apSpacing = parcel->GetParcelSpacing();
	INT32 apWidth = parcel->GetParcelWidth();
	INT32 apHeight = parcel->GetParcelHeight();
	BYTE *attributeField = parcel->GetParcel();

	// Figure out the area to alter
	CRuVector3 relativeCenter = center - apOrigin;

	INT32 sX = max(0, (INT32) ((relativeCenter.x - radius) / apSpacing));
	INT32 sZ = max(0, (INT32) ((relativeCenter.z - radius) / apSpacing));
	INT32 eX = min(apWidth - 1, (INT32) ((relativeCenter.x + radius) / apSpacing + 1));
	INT32 eZ = min(apHeight - 1, (INT32) ((relativeCenter.z + radius) / apSpacing + 1));

	// Check brush against height field
	for(INT32 z = sZ; z <= eZ; ++z)
	{
		for(INT32 x = sX; x <= eX; ++x)
		{
			CRuVector3 pointPos(apOrigin.x + x * apSpacing, center.y, apOrigin.z + z * apSpacing);

			REAL distance = (center - pointPos).Magnitude();

			if(distance <= radius)
			{
				// Alter flag value
				attributeField[x + z * apWidth] = (attributeField[x + z * apWidth] & ~flagMask) | (flagValue & flagMask);
			}
		}
	}

	return TRUE;
}

BOOL Mojo_PaintValue(CRuMojo_AttributeParcel<BYTE> *parcel, const CRuVector3 &center, REAL radius, REAL value, REAL power, RuFalloffFunction falloffFunction, BYTE flagMask)
{
	// Get parcel data
	const CRuVector3 &apOrigin = parcel->GetParcelOrigin();
	REAL apSpacing = parcel->GetParcelSpacing();
	INT32 apWidth = parcel->GetParcelWidth();
	INT32 apHeight = parcel->GetParcelHeight();
	BYTE *attributeField = parcel->GetParcel();

	// Figure out the area to alter
	CRuVector3 relativeCenter = center - apOrigin;

	INT32 sX = max(0, (INT32) ((relativeCenter.x - radius) / apSpacing));
	INT32 sZ = max(0, (INT32) ((relativeCenter.z - radius) / apSpacing));
	INT32 eX = min(apWidth - 1, (INT32) ((relativeCenter.x + radius) / apSpacing + 1));
	INT32 eZ = min(apHeight - 1, (INT32) ((relativeCenter.z + radius) / apSpacing + 1));

	// Check brush against height field
	for(INT32 z = sZ; z <= eZ; ++z)
	{
		for(INT32 x = sX; x <= eX; ++x)
		{
			CRuVector3 pointPos(apOrigin.x + x * apSpacing, center.y, apOrigin.z + z * apSpacing);

			REAL distance = (center - pointPos).Magnitude();

			if(distance <= radius)
			{
				// there are obvious issues with generality of this following statement
				REAL v1 = (REAL) (attributeField[x + z * apWidth] & flagMask) / flagMask;

				// Calculate t0 and t1 interpolation terms based on falloff and power
				REAL t0 = min(1.0f, falloffFunction(distance, radius) * power);
				REAL t1 = 1.0f - t0;

				// Calculate new value
				REAL vFinal = value * t0 + v1 * t1;
				BYTE vFinalByte = static_cast<BYTE>(vFinal * flagMask);

				// Alter flag value
				attributeField[x + z * apWidth] = (attributeField[x + z * apWidth] & ~flagMask) | (vFinalByte & flagMask);
			}
		}
	}

	return TRUE;
}

BOOL Mojo_PaintColor(CRuMojo_AttributeParcel<RUCOLOR> *parcel, const CRuVector3 &center, REAL radius, RUCOLOR color, REAL delta, RuFalloffFunction falloffFunction)
{
	// Get parcel data
	const CRuVector3 &cfOrigin = parcel->GetParcelOrigin();
	REAL cfSpacing = parcel->GetParcelSpacing();
	INT32 cfWidth = parcel->GetParcelWidth();
	INT32 cfHeight = parcel->GetParcelHeight();
	RUCOLOR *colorField = parcel->GetParcel();

	// Figure out the area to alter
	CRuVector3 relativeCenter = center - cfOrigin;

	INT32 sX = max(0, (INT32) ((relativeCenter.x - radius) / cfSpacing));
	INT32 sZ = max(0, (INT32) ((relativeCenter.z - radius) / cfSpacing));
	INT32 eX = min(cfWidth - 1, (INT32) ((relativeCenter.x + radius) / cfSpacing + 1));
	INT32 eZ = min(cfHeight - 1, (INT32) ((relativeCenter.z + radius) / cfSpacing + 1));

	CRuColor4 c0(color);

	// Check brush against height field
	for(INT32 z = sZ; z <= eZ; ++z)
	{
		for(INT32 x = sX; x <= eX; ++x)
		{
			CRuVector3 pointPos(cfOrigin.x + x * cfSpacing, center.y, cfOrigin.z + z * cfSpacing);

			REAL distance = (center - pointPos).Magnitude();

			if(distance <= radius)
			{
				REAL t0 = falloffFunction(distance, radius);
				REAL t1 = 1.0f - t0;

				CRuColor4 c1(colorField[x + z * cfWidth]);

				colorField[x + z * cfWidth] = (c0 * t0 + c1 * t1).GetARGB();
			}
		}
	}

	return TRUE;
}

RUCOLOR Mojo_PickColor(CRuMojo_AttributeParcel<RUCOLOR> *parcel, const CRuVector3 &center)
{
	// Get parcel data
	const CRuVector3 &cfOrigin = parcel->GetParcelOrigin();
	REAL cfSpacing = parcel->GetParcelSpacing();
	INT32 cfWidth = parcel->GetParcelWidth();
	INT32 cfHeight = parcel->GetParcelHeight();
	RUCOLOR *colorField = parcel->GetParcel();

	// Figure out the relative center
	CRuVector3 relativeCenter = center - cfOrigin;

	// Find nearest coordinates
	INT32 x = min(cfWidth - 1, max(0, (INT32) ((relativeCenter.x) / cfSpacing)));
	INT32 z = min(cfHeight - 1, max(0, (INT32) ((relativeCenter.z) / cfSpacing)));

	return colorField[x + z * cfWidth];
}

// ************************************************************************************************************************************************************

#pragma managed
