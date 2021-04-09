/*!
	@file RuPrimitiveToolkit.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Engine/Geometry/RuPrimitiveToolkit.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

INT32 CRuPrimitiveToolkit::Cone_VertexCount(INT32 numSegments)
{
	return numSegments + 2;
}

INT32 CRuPrimitiveToolkit::Cone_PrimitiveCount(INT32 numSegments)
{
	return numSegments * 2;
}

BOOL CRuPrimitiveToolkit::Cone_Build(INT32 numSegments, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut)
{
	INT32 vertIndex = vertexOffset;
	float arcLength = (ruPI * 2.0f) / numSegments;

	// Build vertex list
	verticesOut[vertIndex++].Set(0.0f, 0.0f, 0.0f);
	verticesOut[vertIndex++].Set(0.0f, 1.0f, 0.0f);

	for(INT32 i = 0; i < numSegments; ++i, ++vertIndex)
	{
		float angle = arcLength * i;
		verticesOut[vertIndex].Set(cos(angle) * 0.5f, 0.0f, sin(angle) * 0.5f);
	}

	// Build index list
	INT32 j = indexOffset;
	for(INT32 i = 0; i < numSegments; ++i)
	{
		if(i + 1 == numSegments)
		{
			indicesOut[j++] = 0 + vertexOffset;
			indicesOut[j++] = i + 2 + vertexOffset;
			indicesOut[j++] = 2 + vertexOffset;
		}
		else
		{
			indicesOut[j++] = 0 + vertexOffset;
			indicesOut[j++] = i + 2 + vertexOffset;
			indicesOut[j++] = i + 3 + vertexOffset;
		}
	}

	for(INT32 i = 0; i < numSegments; ++i)
	{
		if(i + 1 == numSegments)
		{
			indicesOut[j++] = 1 + vertexOffset;
			indicesOut[j++] = 2 + vertexOffset;
			indicesOut[j++] = i + 2 + vertexOffset;
		}
		else
		{
			indicesOut[j++] = 1 + vertexOffset;
			indicesOut[j++] = i + 3 + vertexOffset;
			indicesOut[j++] = i + 2 + vertexOffset;
		}
	}

	vertexOffset = vertIndex;
	indexOffset = j;

	return TRUE;
}

INT32 CRuPrimitiveToolkit::Cylinder_VertexCount(INT32 numSegments)
{
	return numSegments * 2 + 2;
}

INT32 CRuPrimitiveToolkit::Cylinder_PrimitiveCount(INT32 numSegments)
{
	return numSegments * 4;
}

BOOL CRuPrimitiveToolkit::Cylinder_Build(INT32 numSegments, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut)
{
	INT32 vertIndex = vertexOffset;
	float arcLength = (ruPI * 2.0f) / numSegments;

	// Build vertex list
	verticesOut[vertIndex++].Set(0.0f, 0.0f, 0.0f);
	verticesOut[vertIndex++].Set(0.0f, 1.0f, 0.0f);

	for(INT32 i = 0; i < numSegments; ++i, ++vertIndex)
	{
		float angle = arcLength * i;
		verticesOut[vertIndex].Set(cos(angle) * 0.5f, 0.0f, sin(angle) * 0.5f);
	}

	for(INT32 i = 0; i < numSegments; ++i, ++vertIndex)
	{
		float angle = arcLength * i;
		verticesOut[vertIndex].Set(cos(angle) * 0.5f, 1.0f, sin(angle) * 0.5f);
	}

	// Build index list
	INT32 j = indexOffset;
	for(INT32 i = 0; i < numSegments; ++i)
	{
		indicesOut[j++] = 0 + vertexOffset;

		indicesOut[j++] = i + 2 + vertexOffset;

		if(i + 1 == numSegments)
			indicesOut[j++] = 2 + vertexOffset;
		else
			indicesOut[j++] = i + 3 + vertexOffset;
	}

	for(INT32 i = 0; i < numSegments; ++i)
	{
		indicesOut[j++] = 1 + vertexOffset;

		if(i + 1 == numSegments)
			indicesOut[j++] = 2 + numSegments + vertexOffset;
		else
			indicesOut[j++] = i + 3 + numSegments + vertexOffset;

		indicesOut[j++] = i + 2 + numSegments + vertexOffset;
	}

	for(INT32 i = 0; i < numSegments; ++i)
	{
		if(i + 1 == numSegments)
		{
			indicesOut[j++] = 2 + i + vertexOffset;
			indicesOut[j++] = 2 + i + numSegments + vertexOffset;
			indicesOut[j++] = 2 + vertexOffset;

			indicesOut[j++] = 2 + vertexOffset;
			indicesOut[j++] = 2 + i + numSegments + vertexOffset;
			indicesOut[j++] = 2 + numSegments + vertexOffset;
		}
		else
		{
			indicesOut[j++] = 2 + i + vertexOffset;
			indicesOut[j++] = 2 + i + numSegments + vertexOffset;
			indicesOut[j++] = 2 + i + 1 + vertexOffset;

			indicesOut[j++] = 2 + i + 1 + vertexOffset;
			indicesOut[j++] = 2 + i + numSegments + vertexOffset;
			indicesOut[j++] = 2 + i + numSegments + 1 + vertexOffset;
		}
	}

	vertexOffset = vertIndex;
	indexOffset = j;

	return TRUE;
}

INT32 CRuPrimitiveToolkit::Quad_VertexCount()
{
	return 4;
}

INT32 CRuPrimitiveToolkit::Quad_PrimitiveCount()
{
	return 4;
}

BOOL CRuPrimitiveToolkit::Quad_Build(INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut)
{
	INT32 vertIndex = vertexOffset;

	// Build vertex list
	verticesOut[vertIndex++].Set(0.5f, 0.0f, 0.5f);
	verticesOut[vertIndex++].Set(-0.5f, 0.0f, 0.5f);
	verticesOut[vertIndex++].Set(-0.5f, 0.0f, -0.5f);
	verticesOut[vertIndex++].Set(0.5f, 0.0f, -0.5f);

	INT32 j = indexOffset;
	indicesOut[j++] = 0 + vertexOffset;
	indicesOut[j++] = 2 + vertexOffset;
	indicesOut[j++] = 1 + vertexOffset;

	indicesOut[j++] = 0 + vertexOffset;
	indicesOut[j++] = 3 + vertexOffset;
	indicesOut[j++] = 2 + vertexOffset;

	indicesOut[j++] = 0 + vertexOffset;
	indicesOut[j++] = 1 + vertexOffset;
	indicesOut[j++] = 2 + vertexOffset;

	indicesOut[j++] = 0 + vertexOffset;
	indicesOut[j++] = 2 + vertexOffset;
	indicesOut[j++] = 3 + vertexOffset;

	vertexOffset = vertIndex;
	indexOffset = j;

	return TRUE;
}

INT32 CRuPrimitiveToolkit::Torus_VertexCount(INT32 numSegments, INT32 numSides)
{
	return numSegments * numSides;
}

INT32 CRuPrimitiveToolkit::Torus_PrimitiveCount(INT32 numSegments, INT32 numSides)
{
	return numSegments * numSides * 2;
}

BOOL CRuPrimitiveToolkit::Torus_Build(INT32 numSegments, INT32 numSides, float tubeRadius, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut)
{
	float ringRadius = 1.0f - tubeRadius;
	float arcLength1 = (ruPI * 2.0f) / numSegments;
	float arcLength2 = (ruPI * 2.0f) / numSides;
	INT32 torusVertexCount = Torus_VertexCount(numSegments, numSides);

	CRuVector3 ringOffset(ringRadius, 0.0f, 0.0f);
	CRuMatrix4x4 ringTransform;

	for(INT32 i = 0, vertIndex = vertexOffset; i < numSegments; ++i)
	{
		// Generate the angle for the ring on the torus
		float angle1 = i * arcLength1;

		for(INT32 j = 0; j < numSides; ++j, ++vertIndex)
		{
			// Generate the angle for the vertex on the segment ring
			float angle2 = j * arcLength2;

			// Generate a vertex on the segment ring centered at (0, 0, 0)
			verticesOut[vertIndex].Set(cos(angle2) * tubeRadius, sin(angle2) * tubeRadius, 0.0f);

			// Translate the vertex to the correct ring radius
			verticesOut[vertIndex] += ringOffset;

			// Rotate about y-axis to bring the vertex to the correct angle on the ring
			ringTransform.SetToRotation(CRuVector3(0.0f, 1.0f, 0.0f), angle1);
			ringTransform.TransformPoint(verticesOut[vertIndex]);
		}
	}

	for(INT32 i = 0, vertIndex = vertexOffset, index = indexOffset; i < numSegments; ++i)
	{
		for(INT32 j = 0; j < numSides; ++j, ++vertIndex)
		{
			if(j + 1 == numSides)
			{
				if(i + 1 == numSegments)
				{
					indicesOut[index++] = vertIndex;
					indicesOut[index++] = vertIndex - numSides + 1;
					indicesOut[index++] = (vertIndex + 1) % torusVertexCount + vertexOffset;

					indicesOut[index++] = (vertIndex + 1) % torusVertexCount + vertexOffset;
					indicesOut[index++] = (vertIndex + numSides) % torusVertexCount + vertexOffset;
					indicesOut[index++] = vertIndex;
				}
				else
				{
					indicesOut[index++] = vertIndex;
					indicesOut[index++] = vertIndex - numSides + 1;
					indicesOut[index++] = vertIndex + 1;

					indicesOut[index++] = vertIndex + 1;
					indicesOut[index++] = vertIndex + numSides;
					indicesOut[index++] = vertIndex;
				}
			}
			else
			{
				if(i + 1 == numSegments)
				{
					indicesOut[index++] = vertIndex;
					indicesOut[index++] = vertIndex + 1;
					indicesOut[index++] = (vertIndex + 1 + numSides) % torusVertexCount + vertexOffset;

					indicesOut[index++] = (vertIndex + 1 + numSides) % torusVertexCount + vertexOffset;
					indicesOut[index++] = (vertIndex + numSides) % torusVertexCount + vertexOffset;
					indicesOut[index++] = vertIndex;
				}
				else
				{
					indicesOut[index++] = vertIndex;
					indicesOut[index++] = vertIndex + 1;
					indicesOut[index++] = vertIndex + 1 + numSides;

					indicesOut[index++] = vertIndex + 1 + numSides;
					indicesOut[index++] = vertIndex + numSides;
					indicesOut[index++] = vertIndex;
				}
			}
		}
	}

	vertexOffset += torusVertexCount;
	indexOffset += (Torus_PrimitiveCount(numSegments, numSides) * 3);

	return TRUE;
}

INT32 CRuPrimitiveToolkit::Grid_VertexCount(INT32 numSegments)
{
	return (numSegments * 2 + 1) * 2 * 2;
}

INT32 CRuPrimitiveToolkit::Grid_PrimitiveCount(INT32 numSegments)
{
	return (numSegments * 2 + 1) * 2;
}

INT32 CRuPrimitiveToolkit::Grid_Build(INT32 numSegments, float spacing, const CRuVector3 &up, const CRuVector3 &right, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut)
{
	INT32 vertIndex = vertexOffset;
	INT32 index = indexOffset;

	INT32 numSteps = numSegments * 2 + 1;

	float gridStart = -(numSegments * spacing);
	float gridEnd = -gridStart;

	float upCoord = gridStart;
	for(INT32 upStep = 0; upStep < numSteps; ++upStep, upCoord += spacing)
	{
		verticesOut[vertIndex++] = (right * gridStart) + (up * upCoord);
		verticesOut[vertIndex++] = (right * gridEnd) + (up * upCoord);
		indicesOut[index++] = vertIndex - 2;
		indicesOut[index++] = vertIndex - 1;
	}

	float rightCoord = gridStart;
	for(INT32 rightStep = 0; rightStep < numSteps; ++rightStep, rightCoord += spacing)
	{
		verticesOut[vertIndex++] = (up * gridStart) + (right * rightCoord);
		verticesOut[vertIndex++] = (up * gridEnd) + (right * rightCoord);
		indicesOut[index++] = vertIndex - 2;
		indicesOut[index++] = vertIndex - 1;
	}

	return TRUE;
}

INT32 CRuPrimitiveToolkit::PlaneXZ_VertexCount()
{
	return 4;
}

INT32 CRuPrimitiveToolkit::PlaneXZ_PrimitiveCount()
{
	return 2;
}

INT32 CRuPrimitiveToolkit::PlaneXZ_Build(float size, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut)
{
	INT32 vertIndex = vertexOffset;
	INT32 index = indexOffset;

	float halfSize = size / 2.0f;

	verticesOut[vertIndex++].Set(-halfSize, 0.0f, halfSize);
	verticesOut[vertIndex++].Set(halfSize, 0.0f, halfSize);
	verticesOut[vertIndex++].Set(-halfSize, 0.0f, -halfSize);
	verticesOut[vertIndex++].Set(halfSize, 0.0f, -halfSize);

	indicesOut[index++] = 0;
	indicesOut[index++] = 1;
	indicesOut[index++] = 2;

	indicesOut[index++] = 1;
	indicesOut[index++] = 3;
	indicesOut[index++] = 2;

	return TRUE;
}

INT32 CRuPrimitiveToolkit::Shell_VertexCount(INT32 horzSegments, INT32 vertSegments)
{
	INT32 numRingPoints = horzSegments + 1;
	INT32 numVertices = 1 + numRingPoints * vertSegments;

	return numVertices;
}

INT32 CRuPrimitiveToolkit::Shell_PrimitiveCount(INT32 horzSegments, INT32 vertSegments)
{
	INT32 numPrimitives = (horzSegments * 2) * (vertSegments - 1) + horzSegments;

	return numPrimitives;
}

INT32 CRuPrimitiveToolkit::Shell_Build(INT32 horzSegments, INT32 vertSegments, REAL vertSweep, REAL horzSweep, REAL radius, REAL vertScale, REAL horzCompression, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut, DWORD *diffuseOut)
{
	INT32 numRingPoints = horzSegments + 1;

	// Compute the angular sweep
	REAL tIncrement = 1.0f / vertSegments;

	// Starting from top, build the vertex rings
	CRuVector3 yAxis(0.0f, 1.0f, 0.0f), zAxis(0.0f, 0.0f, 1.0f);
	CRuMatrix4x4 rotMat;

	// Start with the hemisphere apex
	verticesOut[0].Set(0.0f, radius * vertScale, 0.0f);
	diffuseOut[0] = 0x00000000;

	INT32 k = 1;
	for(INT32 i = 0; i < vertSegments; ++i)
	{
		// Figure out how much to rotate along the z axis
		REAL t = static_cast<float>(pow((vertSegments - i - 1) * tIncrement, horzCompression));

		// Rotate point from apex into proper ring height
		CRuVector3 point(0.0f, radius, 0.0f);
		rotMat.SetToRotation(zAxis, vertSweep - vertSweep * t);
		rotMat.TransformPoint(point);
		point.y = point.y * vertScale;

		// Sweep through the horizontal ring
		rotMat.SetToRotation(yAxis, horzSweep / horzSegments);
		for(INT32 j = 0; j < numRingPoints; ++j, ++k)
		{
			rotMat.TransformPoint(point);
			verticesOut[k] = point;

			// Store texture coordinate in diffuse color
			REAL u = min(max((float) j / (numRingPoints - 1.0f), 0.0f), 1.0f);
			REAL v = min(max((radius - point.y) / radius, 0.0f), 1.0f);
			BYTE iU = static_cast<BYTE>(u * 255.0f);
			BYTE iV = static_cast<BYTE>(v * 255.0f);
			diffuseOut[k] = RUCOLOR_ARGB(0x00, iU, iV, 0x00);
		}
	}

	// Build the face index array, starting from the hemisphere cap
	k = 0;
	for(INT32 j = 1; j <= horzSegments; j++)
	{
		indicesOut[k++] = 0;
		indicesOut[k++] = j + 1;
		indicesOut[k++] = j;
	}

	for(INT32 i = 1; i < vertSegments; i++)
	{
		int ring1Offset = numRingPoints * (i - 1) + 1;
		int ring2Offset = ring1Offset + numRingPoints;
		for (INT32 j = 0; j < horzSegments; j++)
		{
			indicesOut[k++] = ring1Offset + j;
			indicesOut[k++] = ring2Offset + j + 1;
			indicesOut[k++] = ring2Offset + j;
			indicesOut[k++] = ring1Offset + j;
			indicesOut[k++] = ring1Offset + j + 1;
			indicesOut[k++] = ring2Offset + j + 1;
		}
	}

	return TRUE;
}

INT32 CRuPrimitiveToolkit::HalfCapsule_VertexCount(INT32 horzSegments, INT32 vertSegments)
{
	INT32 numRingPoints = horzSegments + 1;
	INT32 numVertices = 1 + numRingPoints * (vertSegments + 1);

	return numVertices;
}

INT32 CRuPrimitiveToolkit::HalfCapsule_PrimitiveCount(INT32 horzSegments, INT32 vertSegments)
{
	INT32 numPrimitives = (horzSegments * 2) * (vertSegments) + horzSegments;

	return numPrimitives;
}

INT32 CRuPrimitiveToolkit::HalfCapsule_Build(INT32 horzSegments, INT32 vertSegments, REAL vertSweep, REAL horzSweep, REAL radius, REAL vertScale, REAL horzCompression, REAL skirtExtension, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut, DWORD *diffuseOut)
{
	INT32 numRingPoints = horzSegments + 1;

	// Compute the angular sweep
	REAL tIncrement = 1.0f / vertSegments;

	// Starting from top, build the vertex rings
	CRuVector3 yAxis(0.0f, 1.0f, 0.0f), zAxis(0.0f, 0.0f, 1.0f);
	CRuMatrix4x4 rotMat;

	// Start with the hemisphere apex
	verticesOut[0].Set(0.0f, radius * vertScale, 0.0f);
	diffuseOut[0] = 0x00000000;

	INT32 k = 1;
	for(INT32 i = 0; i < vertSegments; ++i)
	{
		// Figure out how much to rotate along the z axis
		REAL t = static_cast<float>(pow((vertSegments - i - 1) * tIncrement, horzCompression));

		// Rotate point from apex into proper ring height
		CRuVector3 point(0.0f, radius, 0.0f);
		rotMat.SetToRotation(zAxis, vertSweep - vertSweep * t);
		rotMat.TransformPoint(point);
		point.y = point.y * vertScale;

		// Sweep through the horizontal ring
		rotMat.SetToRotation(yAxis, horzSweep / horzSegments);
		for(INT32 j = 0; j < numRingPoints; ++j, ++k)
		{
			rotMat.TransformPoint(point);
			verticesOut[k] = point;

			// Store texture coordinate in diffuse color
			REAL u = min(max((float) j / (numRingPoints - 1.0f), 0.0f), 1.0f);
			REAL v = min(max((radius - point.y) / radius, 0.0f), 1.0f);
			BYTE iU = static_cast<BYTE>(u * 255.0f);
			BYTE iV = static_cast<BYTE>(v * 255.0f);
			diffuseOut[k] = RUCOLOR_ARGB(0x00, iU, iV, 0x00);
		}
	}

	// Repeat the last horizontal ring vertices
	for(INT32 j = 0; j < numRingPoints; ++j, ++k)
	{
		verticesOut[k].Set(verticesOut[k - numRingPoints].m_x, verticesOut[k - numRingPoints].m_y - skirtExtension, verticesOut[k - numRingPoints].m_z);
		diffuseOut[k] = diffuseOut[k - numRingPoints];
	}

	// Build the face index array, starting from the hemisphere cap
	k = 0;
	for(INT32 j = 1; j <= horzSegments; j++)
	{
		indicesOut[k++] = 0;
		indicesOut[k++] = j + 1;
		indicesOut[k++] = j;
	}

	INT32 i;
	for(i = 1; i < vertSegments; i++)
	{
		int ring1Offset = numRingPoints * (i - 1) + 1;
		int ring2Offset = ring1Offset + numRingPoints;
		for (INT32 j = 0; j < horzSegments; j++)
		{
			indicesOut[k++] = ring1Offset + j;
			indicesOut[k++] = ring2Offset + j + 1;
			indicesOut[k++] = ring2Offset + j;
			indicesOut[k++] = ring1Offset + j;
			indicesOut[k++] = ring1Offset + j + 1;
			indicesOut[k++] = ring2Offset + j + 1;
		}
	}

	// Repeat the last horizontal ring primitives
	int ring1Offset = numRingPoints * (i - 1) + 1;
	int ring2Offset = ring1Offset + numRingPoints;
	for (INT32 j = 0; j < horzSegments; j++)
	{
		indicesOut[k++] = ring1Offset + j;
		indicesOut[k++] = ring2Offset + j + 1;
		indicesOut[k++] = ring2Offset + j;
		indicesOut[k++] = ring1Offset + j;
		indicesOut[k++] = ring1Offset + j + 1;
		indicesOut[k++] = ring2Offset + j + 1;
	}

	return TRUE;
}

INT32 CRuPrimitiveToolkit::Sphere_VertexCount(INT32 segments)
{
	// Make sure segments is even
	segments += (segments % 2) ? 1 : 0;

	INT32 hemisphereSegments = segments / 2;
	INT32 numRingPoints = segments + 1;
	INT32 numVertices = numRingPoints * (hemisphereSegments + 1);

	return numVertices;
}

INT32 CRuPrimitiveToolkit::Sphere_PrimitiveCount(INT32 segments)
{
	// Make sure segments is even
	segments += (segments % 2) ? 1 : 0;

	INT32 hemisphereSegments = segments / 2;
	INT32 numPrimitives = (segments * 2) * hemisphereSegments;

	return numPrimitives;
}

INT32 CRuPrimitiveToolkit::Sphere_Build(INT32 segments, const CRuSphere &sphere, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut, DWORD *diffuseOut)
{
	// Make sure segments is even
	segments += (segments % 2) ? 1 : 0;

	INT32 hemisphereSegments = segments / 2;

	// Compute the angular sweep
	REAL tIncrement = 1.0f / hemisphereSegments;

	// Starting from top, build the vertex rings
	CRuVector3 yAxis(0.0f, 1.0f, 0.0f), zAxis(0.0f, 0.0f, 1.0f);
	CRuMatrix4x4 rotMat;

	// Start with the north hemisphere apex and move our way down
	INT32 k = 0;
	for(INT32 i = 0; i <= hemisphereSegments; ++i)
	{
		// Figure out how much to rotate along the z axis
		REAL t = i * tIncrement;

		// Rotate point from apex into proper ring height
		CRuVector3 point(0.0f, sphere.Radius(), 0.0f);
		rotMat.SetToRotation(zAxis, t * ruPI);
		rotMat.TransformPoint(point);

		// Sweep through the horizontal ring
		rotMat.SetToRotation(yAxis, ruPI * 2.0f / segments);
		for(INT32 j = 0; j <= segments; ++j, ++k)
		{
			rotMat.TransformPoint(point);
			verticesOut[k].Set(point.x + sphere.Center().x, point.y + sphere.Center().y, point.z + sphere.Center().z);
			diffuseOut[k] = 0xFFFFFFFF;
		}
	}

	// Build the face index array, starting from the hemisphere cap
	k = 0;
	for(INT32 i = 0; i < hemisphereSegments; i++)
	{
		int ring1Offset = (segments + 1) * i;
		int ring2Offset = ring1Offset + (segments + 1);
		for(INT32 j = 0; j < segments; j++)
		{
			indicesOut[k++] = ring1Offset + j;
			indicesOut[k++] = ring2Offset + j;
			indicesOut[k++] = ring2Offset + j + 1;
			indicesOut[k++] = ring1Offset + j;
			indicesOut[k++] = ring2Offset + j + 1;
			indicesOut[k++] = ring1Offset + j + 1;
		}
	}

	return TRUE;
}

INT32 CRuPrimitiveToolkit::FlagWidget_VertexCount()
{
	return 7;
}

INT32 CRuPrimitiveToolkit::FlagWidget_PrimitiveCount()
{
	return 3;
}

INT32 CRuPrimitiveToolkit::FlagWidget_Build(REAL tiltDist, REAL pointerWidth, REAL pointerHeight, REAL linerWidth, REAL linerHeight, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut)
{
	INT32 vIdx = vertexOffset;
	INT32 iIdx = indexOffset;

	// Pointer fin
	verticesOut[vIdx++].Set(0.0f, 0.0f, 0.0f);
	verticesOut[vIdx++].Set(tiltDist - pointerWidth, pointerHeight, 0.0f);
	verticesOut[vIdx++].Set(tiltDist, pointerHeight, 0.0f);

	indicesOut[iIdx++] = 0 + vertexOffset;
	indicesOut[iIdx++] = 1 + vertexOffset;
	indicesOut[iIdx++] = 2 + vertexOffset;

	// Liner quad
	verticesOut[vIdx++].Set(0.0f, pointerHeight + linerHeight, 0.0f);
	verticesOut[vIdx++].Set(linerWidth, pointerHeight + linerHeight, 0.0f);
	verticesOut[vIdx++].Set(0.0f, pointerHeight, 0.0f);
	verticesOut[vIdx++].Set(linerWidth, pointerHeight, 0.0f);

	indicesOut[iIdx++] = 3 + vertexOffset;
	indicesOut[iIdx++] = 4 + vertexOffset;
	indicesOut[iIdx++] = 5 + vertexOffset;

	indicesOut[iIdx++] = 4 + vertexOffset;
	indicesOut[iIdx++] = 6 + vertexOffset;
	indicesOut[iIdx++] = 5 + vertexOffset;

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed
