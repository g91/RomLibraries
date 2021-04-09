/*!
	@file RuEntity_Widget_ClipRect.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../../Scene/Widget/RuEntity_Widget_ClipRect.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Widget_ClipRect, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Widget_ClipRect, "CRuEntity_Widget_ClipRect", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuEntity_Widget_ClipRect, 0xB601017B, 0xE4BD4bb0, 0xBC9C0645, 0x39A5B82F)

// ************************************************************************************************************************************************************

void FindIntersectingPoints(const CRuVector3 &v0, const CRuVector3 &v1, const CRuVector3 &v2, const CRuPlane4 &plane, CRuArrayList<CRuVector3> &pointsOut)
{
	// Clear result
	pointsOut.Clear();

	// Calculate distance to all vertices
	REAL dv0 = plane.GetDistance(v0);
	REAL dv1 = plane.GetDistance(v1);
	REAL dv2 = plane.GetDistance(v2);

	// Triangle coplanar with plane?
	if(fabs(dv0) < ruEPSILON && fabs(dv1) < ruEPSILON && fabs(dv2) < ruEPSILON)
	{
		pointsOut.Add(v0);
		pointsOut.Add(v1);
		pointsOut.Add(v2);
		return;
	}

	// Triangle does not intersect plane?
	if((dv0 > 0.0f && dv1 > 0.0f && dv2 > 0.0f) || (dv0 < 0.0f && dv1 < 0.0f && dv2 < 0.0f))
	{
		return;
	}

	// Segment (v0, v1) intersects plane?
	if(dv0 * dv1 < 0.0f)
	{
		REAL wv0 = fabs(dv1) / (fabs(dv0) + fabs(dv1));
		pointsOut.Add(v0 * wv0 + v1 * (1.0f - wv0));
	}

	// Segment (v0, v2) intersects plane?
	if(dv0 * dv2 < 0.0f)
	{
		REAL wv0 = fabs(dv2) / (fabs(dv0) + fabs(dv2));
		pointsOut.Add(v0 * wv0 + v2 * (1.0f - wv0));
	}

	// Segment (v1, v2) intersects plane?
	if(dv1 * dv2 < 0.0f)
	{
		REAL wv1 = fabs(dv2) / (fabs(dv1) + fabs(dv2));
		pointsOut.Add(v1 * wv1 + v2 * (1.0f - wv1));
	}
}

CRuEntity_Widget_ClipRect::CRuEntity_Widget_ClipRect()
:	m_targetObject(NULL),
	m_gridInterval(0.0f),
	m_meshDisplayMode(ruMESHDISPLAYMODE_LINE),
	m_polyLineWidth(0.25f),
	m_lineColor(0xFFFFFFFF),
	m_dirty(TRUE),
	m_clipRectMesh(NULL),
	m_clipRectMaterial(NULL)
{
	m_clipRectMesh = ruNEW CRuDynamicMesh();
	m_clipRectMesh->Initialize(ruPRIMTYPE_LINELIST, 8, 8, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);

	m_clipRectMaterial = ruNEW CRuMaterial();
	m_clipRectMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_EDITOR_TRANSLUCENT"));
	m_clipRectMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, FALSE);
}

CRuEntity_Widget_ClipRect::~CRuEntity_Widget_ClipRect()
{
	ruSAFE_RELEASE(m_targetObject);

	ruSAFE_RELEASE(m_clipRectMesh);
	ruSAFE_RELEASE(m_clipRectMaterial);
}

BOOL CRuEntity_Widget_ClipRect::SetTargetObject(IRuObject *targetObject)
{
	if(targetObject)
	{
		targetObject->AddRef();
		ruSAFE_RELEASE(m_targetObject);
		m_targetObject = targetObject;
	}
	else
	{
		ruSAFE_RELEASE(m_targetObject);
	}

	return TRUE;
}

BOOL CRuEntity_Widget_ClipRect::SetClipBox(const CRuOBB &clipBox)
{
	// Copy clip box settings
	m_clipBox = clipBox;

	// Re-center so that the clip box is at origin and translation is on the widget entity
	this->Translate(m_clipBox.Center(), ruTRANSFORMOP_SET);
	m_clipBox.Center().Set(0.0f, 0.0f, 0.0f);

	// Mark mesh as dirty
	m_dirty = TRUE;

	return TRUE;
}

BOOL CRuEntity_Widget_ClipRect::SetGridInterval(REAL interval)
{
	// Store new grid interval
	m_gridInterval = interval;

	// Mark mesh as dirty
	m_dirty = TRUE;

	return TRUE;
}

BOOL CRuEntity_Widget_ClipRect::SetMeshDisplayMode(MeshDisplayMode meshDisplayMode)
{
	// Store new mesh display mode
	m_meshDisplayMode = meshDisplayMode;

	// Mark mesh as dirty
	m_dirty = TRUE;

	return TRUE;
}

BOOL CRuEntity_Widget_ClipRect::SetLineColor(RUCOLOR color)
{
	// Store new line color
	m_lineColor = color;

	return TRUE;
}

BOOL CRuEntity_Widget_ClipRect::MarkMeshAsDirty()
{
	// Mark mesh as dirty
	m_dirty = TRUE;

	return TRUE;
}

BOOL CRuEntity_Widget_ClipRect::Update(float elapsedTime)
{
	// Chain update call to base class
	IRuEntity_Renderable::Update(elapsedTime);

	if(m_dirty)
	{
		if(m_targetObject)
		{
			// Build mesh segments
			BuildMeshSegments();

			// Build line mesh
			switch(m_meshDisplayMode)
			{
				case ruMESHDISPLAYMODE_LINE:
					{
						BuildLineMesh();
					}
					break;

				case ruMESHDISPLAYMODE_POLYLINE:
					{
						BuildPolyLineMesh();
					}
					break;
			}

			// Mark as not dirty
			m_dirty = FALSE;
		}
	}

	return TRUE;
}

INT32 CRuEntity_Widget_ClipRect::GetNumMeshes() const
{
	if(m_clipRectMesh)
		return 1;

	return 0;
}

BOOL CRuEntity_Widget_ClipRect::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex == 0)
	{
		*meshOut = m_clipRectMesh;
		*materialOut = m_clipRectMaterial;

		return TRUE;
	}

	return FALSE;
}

void CRuEntity_Widget_ClipRect::BuildMeshSegments()
{
	// Clear existing mesh vertices
	m_meshVertices.Clear();

	if(m_targetObject)
	{
		// Build a OBB triangle enumeration query
		CRuCollisionQuery_OBB_TriEnum colQuery;
		colQuery.m_queryOnOriginalGeometry = TRUE;
		colQuery.m_colOBB = m_clipBox;
		colQuery.m_worldTransform = const_cast<CRuMatrix4x4 *>(&this->GetWorldTransform());
		colQuery.m_invWorldTransform = const_cast<CRuMatrix4x4 *>(&this->GetInverseWorldTransform());

		// Perform collision query
		if(m_targetObject->GetType().IsSubClassOf(CRuEntity::Type()))
		{
			static_cast<CRuEntity *>(m_targetObject)->QueryCollision(&colQuery);
		}
		else if(m_targetObject->GetType().IsSubClassOf(IRuCollisionObject::Type()))
		{
			static_cast<IRuCollisionObject *>(m_targetObject)->QueryCollision(&colQuery);
		}

		// ...
		CRuOBB box(m_clipBox);
		box.Transform(this->GetWorldTransform());

		// Build four planes to clip against
		CRuArrayList<CRuPlane4> clipPlanes;

		// +X
		clipPlanes.Add(CRuPlane4(box.Axis()[0], box.Center() + box.Axis()[0] * box.Extents()[0]));

		// -X
		clipPlanes.Add(CRuPlane4(box.Axis()[0] * -1.0f, box.Center() + box.Axis()[0] * box.Extents()[0] * -1.0f));

		// +Z
		clipPlanes.Add(CRuPlane4(box.Axis()[2], box.Center() + box.Axis()[2] * box.Extents()[2]));

		// -Z
		clipPlanes.Add(CRuPlane4(box.Axis()[2] * -1.0f, box.Center() + box.Axis()[2] * box.Extents()[2] * -1.0f));

		// Build grid planes
		if(m_gridInterval > 0.0f)
		{
			REAL curExtents;

			curExtents = 0.0f;
			while(curExtents < box.Extents()[0])
			{
				clipPlanes.Add(CRuPlane4(box.Axis()[0], box.Center() + box.Axis()[0] * curExtents));
				clipPlanes.Add(CRuPlane4(box.Axis()[0] * -1.0f, box.Center() + box.Axis()[0] * curExtents * - 1.0f));

				curExtents += m_gridInterval;
			}

			curExtents = 0.0f;
			while(curExtents < box.Extents()[2])
			{
				clipPlanes.Add(CRuPlane4(box.Axis()[2], box.Center() + box.Axis()[2] * curExtents));
				clipPlanes.Add(CRuPlane4(box.Axis()[2] * -1.0f, box.Center() + box.Axis()[2] * curExtents * - 1.0f));

				curExtents += m_gridInterval;
			}

			// Center grid line along Z
			clipPlanes.Add(CRuPlane4(box.Axis()[0], box.Center()));

			// Center grid line along X
			clipPlanes.Add(CRuPlane4(box.Axis()[2], box.Center()));
		}

		// Perform clipping against all enumerated triangles
		CRuArrayList<CRuVector3> vertices;
		for(INT32 i = 0, numTris = colQuery.m_colTriVerts->Count() / 3; i < numTris; ++i)
		{
			CRuVector3 &v0 = (*colQuery.m_colTriVerts)[i * 3];
			CRuVector3 &v1 = (*colQuery.m_colTriVerts)[i * 3 + 1];
			CRuVector3 &v2 = (*colQuery.m_colTriVerts)[i * 3 + 2];

			for(INT32 j = 0; j < clipPlanes.Count(); ++j)
			{
				FindIntersectingPoints(v0, v1, v2, clipPlanes[j], vertices);

				if(vertices.Count() == 2)
				{
					m_meshVertices.Add(vertices[0]);
					m_meshVertices.Add(vertices[1]);
				}
			}
		}

		// Clip all edges against box
		for(INT32 i = 0, numEdges = m_meshVertices.Count() / 2; i < numEdges; ++i)
		{
			CRuVector3 &v0 = m_meshVertices[i * 2];
			CRuVector3 &v1 = m_meshVertices[i * 2 + 1];

			for(INT32 j = 0; j < 4; ++j)
			{
				REAL dv0 = clipPlanes[j].GetDistance(v0);
				REAL dv1 = clipPlanes[j].GetDistance(v1);

				if(dv0 > ruEPSILON)
				{
					if(dv1 > ruEPSILON)
					{
						// Both vertices outside valid bounds
						m_meshVertices.RemoveAt(i * 2);
						m_meshVertices.RemoveAt(i * 2);
						--i;
						--numEdges;
						break;
					}
					else
					{
						// v0 is outside, v1 is inside
						v0 = v1 + (v0 - v1) * (fabs(dv1) / (fabs(dv0) + fabs(dv1)));
					}
				}
				else
				{
					if(dv1 > ruEPSILON)
					{
						// v0 is inside, v1 is outside
						v1 = v0 + (v1 - v0) * (fabs(dv0) / (fabs(dv0) + fabs(dv1)));
					}
					else
					{
						// Both vertices inside (do nothing)
					}
				}

				// Is the segment of length zero?
				if((v0 - v1).Magnitude() < ruEPSILON)
				{
					m_meshVertices.RemoveAt(i * 2);
					m_meshVertices.RemoveAt(i * 2);
					--i;
					--numEdges;
				}
			}
		}
	}
}

void CRuEntity_Widget_ClipRect::BuildLineMesh()
{
	// Calculate required mesh space
	INT32 vertexCount = m_meshVertices.Count();
	INT32 primitiveCount = m_meshVertices.Count() / 2;

	// If current mesh does not have enough space, release it
	if(m_clipRectMesh && (m_clipRectMesh->GetPrimitiveType() != ruPRIMTYPE_LINELIST || (m_clipRectMesh->GetMaxVertices() < vertexCount || m_clipRectMesh->GetMaxPrimitives() < primitiveCount)))
	{
		ruSAFE_RELEASE(m_clipRectMesh);
	}

	// If there is no existing mesh, create it
	if(m_clipRectMesh == NULL)
	{
		m_clipRectMesh = ruNEW CRuDynamicMesh();
		m_clipRectMesh->Initialize(ruPRIMTYPE_LINELIST, m_meshVertices.Count(), m_meshVertices.Count() / 2, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);
	}

	// Set vertex and primitive count
	m_clipRectMesh->SetNumVertices(m_meshVertices.Count());
	m_clipRectMesh->SetNumPrimitives(m_meshVertices.Count() / 2);
	m_clipRectMesh->IncrementMeshIteration();

	const CRuMatrix4x4 &invWorldTransform = this->GetInverseWorldTransform();

	IRuMorphTarget *morphTarget = m_clipRectMesh->GetMorphTarget(0);
	CRuVector3 *position = morphTarget->GetPosition();
	DWORD *diffuse = morphTarget->GetDiffuse();
	UINT16 *indices = m_clipRectMesh->GetIndices();

	for(INT32 i = 0; i < m_meshVertices.Count(); ++i)
	{
		// Transform mesh vertices back into object-local space
		invWorldTransform.TransformPoint(m_meshVertices[i]);

		// Store vertex data
		position[i] = m_meshVertices[i];
		diffuse[i] = m_lineColor;
		indices[i] = i;
	}

	m_clipRectMesh->CalculateBounds();

	// Set local entity bounds
	this->SetLocalEntityBounds(m_clipRectMesh->GetBounds());
}

void CRuEntity_Widget_ClipRect::BuildPolyLineMesh()
{
	// Calculate required mesh space
	INT32 vertexCount = m_meshVertices.Count() > 1 ? m_meshVertices.Count() * 2 : 0;
	INT32 primitiveCount = m_meshVertices.Count() > 1 ? m_meshVertices.Count() : 0;

	if(primitiveCount > 0)
	{
		// If current mesh does not have enough space, release it
		if(m_clipRectMesh && (m_clipRectMesh->GetPrimitiveType() != ruPRIMTYPE_TRIANGLELIST || (m_clipRectMesh->GetMaxVertices() < vertexCount || m_clipRectMesh->GetMaxPrimitives() < primitiveCount)))
		{
			ruSAFE_RELEASE(m_clipRectMesh);
		}

		// If there is no existing mesh, create it
		if(m_clipRectMesh == NULL)
		{
			m_clipRectMesh = ruNEW CRuDynamicMesh();
			m_clipRectMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, vertexCount, primitiveCount, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);
		}

		// Set vertex and primitive count
		m_clipRectMesh->SetNumVertices(vertexCount);
		m_clipRectMesh->SetNumPrimitives(primitiveCount);
		m_clipRectMesh->IncrementMeshIteration();

		const CRuMatrix4x4 &worldTransform = this->GetWorldTransform();
		const CRuMatrix4x4 &invWorldTransform = this->GetInverseWorldTransform();

		IRuMorphTarget *morphTarget = m_clipRectMesh->GetMorphTarget(0);
		CRuVector3 *position = morphTarget->GetPosition();
		DWORD *diffuse = morphTarget->GetDiffuse();
		UINT16 *indices = m_clipRectMesh->GetIndices();

		for(INT32 i = 0, j = 0, k = 0; i < m_meshVertices.Count(); ++i)
		{
			CRuVector3 P;

			worldTransform.TransformPoint(m_meshVertices[i], P);

			CRuVector3 Z(0.0f, 1.0f, 0.0f);
			CRuVector3 T;

			if(i == 0)
			{
				T = (m_meshVertices[1] - m_meshVertices[0]).Normalize();
			}
			else if(i + 1 == m_meshVertices.Count())
			{
				T = (m_meshVertices[i] - m_meshVertices[i - 1]).Normalize();
			}
			else if(i % 2)
			{
				T = (m_meshVertices[i] - m_meshVertices[i - 1]).Normalize();
			}
			else
			{
				T = (m_meshVertices[i + 1] - m_meshVertices[i]).Normalize();
			}

			CRuVector3 TxZ = CrossProduct(T, Z);
			if(TxZ.Magnitude() < ruEPSILON)
			{
				TxZ = CRuVector3(1.0f, 0.0f, 0.0f);
			}
			else
			{
				TxZ.Normalize();
			}

			position[j] = m_meshVertices[i] + TxZ * m_polyLineWidth * 0.5f;
			invWorldTransform.TransformPoint(position[j]);
			diffuse[j++] = m_lineColor;

			position[j] = m_meshVertices[i] - TxZ * m_polyLineWidth * 0.5f;
			invWorldTransform.TransformPoint(position[j]);
			diffuse[j++] = m_lineColor;
		}

		for(INT32 i = 0, j = 0; i < primitiveCount / 2; ++i)
		{
			INT32 k = i * 4;

			indices[j++] = k;
			indices[j++] = k + 2;
			indices[j++] = k + 1;

			indices[j++] = k + 1;
			indices[j++] = k + 2;
			indices[j++] = k + 3;
		}

		m_clipRectMesh->CalculateBounds();

		// Set local entity bounds
		this->SetLocalEntityBounds(m_clipRectMesh->GetBounds());
	}
	else
	{
		if(m_clipRectMesh)
		{
			m_clipRectMesh->SetNumVertices(0);
			m_clipRectMesh->SetNumPrimitives(0);
		}
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
