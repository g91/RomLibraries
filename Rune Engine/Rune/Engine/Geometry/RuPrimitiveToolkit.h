/*!
	@file RuPrimitiveToolkit.h

	Copyright (c) 2004-2005 All rights reserved
*/

#ifndef _RUPRIMITIVETOOLKIT_H_
#define _RUPRIMITIVETOOLKIT_H_

#include "../../Engine/Base/RuCore.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuPrimitiveToolkit
{
public:
	static INT32							Cone_VertexCount(INT32 numSegments);
	static INT32							Cone_PrimitiveCount(INT32 numSegments);
	static BOOL								Cone_Build(INT32 numSegments, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut);

	static INT32							Cylinder_VertexCount(INT32 numSegments);
	static INT32							Cylinder_PrimitiveCount(INT32 numSegments);
	static BOOL								Cylinder_Build(INT32 numSegments, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut);

	static INT32							Quad_VertexCount();
	static INT32							Quad_PrimitiveCount();
	static BOOL								Quad_Build(INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut);

	static INT32							Torus_VertexCount(INT32 numSegments, INT32 numSides);
	static INT32							Torus_PrimitiveCount(INT32 numSegments, INT32 numSides);
	static BOOL								Torus_Build(INT32 numSegments, INT32 numSides, float tubeRadius, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut);

	static INT32							Grid_VertexCount(INT32 numSegments);
	static INT32							Grid_PrimitiveCount(INT32 numSegments);
	static INT32							Grid_Build(INT32 numSegments, float spacing, const CRuVector3 &up, const CRuVector3 &right, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut);

	static INT32							PlaneXZ_VertexCount();
	static INT32							PlaneXZ_PrimitiveCount();
	static INT32							PlaneXZ_Build(float size, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut);

	static INT32							Shell_VertexCount(INT32 horzSegments, INT32 vertSegments);
	static INT32							Shell_PrimitiveCount(INT32 horzSegments, INT32 vertSegments);
	static INT32							Shell_Build(INT32 horzSegments, INT32 vertSegments, REAL vertSweep, REAL horzSweep, REAL radius, REAL vertScale, REAL horzCompression, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut, DWORD *diffuseOut);

	static INT32							HalfCapsule_VertexCount(INT32 horzSegments, INT32 vertSegments);
	static INT32							HalfCapsule_PrimitiveCount(INT32 horzSegments, INT32 vertSegments);
	static INT32							HalfCapsule_Build(INT32 horzSegments, INT32 vertSegments, REAL vertSweep, REAL horzSweep, REAL radius, REAL vertScale, REAL horzCompression, REAL skirtExtension, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut, DWORD *diffuseOut);

	static INT32							Sphere_VertexCount(INT32 segments);
	static INT32							Sphere_PrimitiveCount(INT32 segments);
	static INT32							Sphere_Build(INT32 segments, const CRuSphere &sphere, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut, DWORD *diffuseOut);

	static INT32							FlagWidget_VertexCount();
	static INT32							FlagWidget_PrimitiveCount();
	static INT32							FlagWidget_Build(REAL tiltDist, REAL pointerWidth, REAL pointerHeight, REAL linerWidth, REAL linerHeight, INT32 &vertexOffset, INT32 &indexOffset, CRuVector3 *verticesOut, UINT16 *indicesOut);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
