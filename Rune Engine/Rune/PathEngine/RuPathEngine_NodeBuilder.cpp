/*!
	@project Rune
	@file RuPathEngine_NodeBuilder.cpp

	Copyright (c) 2004-2009 All rights reserved

	@author John Tang
	@date 2009-02-24
*/

#include "../PathEngine/RuPathEngine_NodeBuilder.h"

#include "../Collision/RuCollision_Base.h"
#include "../Engine/Geometry/RuMeshCollision.h"
#include "../Scene/Base/RuEntityBase.h"
#include "../Scene/Base/RuEntityModel.h"

#include "../Scene/Gizmo/RuGizmo_Box.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuPathEngine_NodeBuilder, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuPathEngine_NodeBuilder, "CRuPathEngine_NodeBuilder", "IRuObject")
ruCLASSGUID_DEFINE(CRuPathEngine_NodeBuilder, 0x06347A1B, 0xB2114c5d, 0xAEE82E9E, 0xE8D78E2B)

// ************************************************************************************************************************************************************

CRuPathEngine_NodeBuilder::CRuPathEngine_NodeBuilder()
:	m_unitSize(1.0f),
	m_stepHeight(4.0f),
	m_previewCentroids(FALSE),
	m_previewModel(NULL)
{
}

CRuPathEngine_NodeBuilder::~CRuPathEngine_NodeBuilder()
{
	// Release reference to all geometries
	for(INT32 i = 0; i < m_geometries.Count(); ++i)
	{
		ruSAFE_RELEASE(m_geometries[i]);
	}
	m_geometries.Clear();

	// Destroy model
	ruSAFE_RELEASE(m_previewModel);
}

BOOL CRuPathEngine_NodeBuilder::Initialize(const CRuAABB &bounds, REAL unitSize)
{
	// Store node builder parameters
	m_bounds = bounds;
	m_unitSize = unitSize;

	// Construct candidate point grid
	INT32 gridWidth = static_cast<INT32>((m_bounds.Maxima().m_x - m_bounds.Minima().m_x) / unitSize + 0.5f);
	INT32 gridHeight = static_cast<INT32>((m_bounds.Maxima().m_z - m_bounds.Minima().m_z) / unitSize + 0.5f);
	m_candidateGrid.Initialize(gridWidth, gridHeight);

	return TRUE;
}

BOOL CRuPathEngine_NodeBuilder::SetTriangleExclusionMask(DWORD triExclusionMask)
{
	m_triExclusionMask = triExclusionMask;
	return TRUE;
}

BOOL CRuPathEngine_NodeBuilder::AppendGeometry(IRuObject *object)
{
	// Store reference to object
	object->AddRef();
	m_geometries.Add(object);

	// Setup triangle enumeration query
	CRuCollisionQuery_OBB_TriEnum query;
	query.m_colOBB.Set(m_bounds);
	query.m_exclusionMask = m_triExclusionMask;
	query.m_queryOnOriginalGeometry = FALSE;

	// Perform collision query
	if(object->GetType().IsSubClassOf(IRuCollisionObject::Type()))
	{
		static_cast<IRuCollisionObject *>(object)->QueryCollision(&query);
	}
	else if(object->GetType().IsSubClassOf(IRuCollisionStructure::Type()))
	{
		static_cast<IRuCollisionStructure *>(object)->QueryCollision(&query);
	}
	else if(object->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		static_cast<CRuEntity *>(object)->QueryCollision(&query);
	}

	// Process resulting triangles
	for(INT32 i = 0, numTris = query.m_colTriVerts->Count(); i < numTris; i += 3)
	{
		// Insert into triangle list
		Triangle tri(query.m_colTriVerts->Get(i), query.m_colTriVerts->Get(i + 1), query.m_colTriVerts->Get(i + 2));

		// NOTE: make sure we do not have degenerate triangles

		m_triangles.Add(tri);
	}

	// Release existing preview model
	ruSAFE_RELEASE(m_previewModel);

	return TRUE;
}

BOOL CRuPathEngine_NodeBuilder::ClassifyGeometry(const CRuVector3 &upVector, REAL slopeLimit)
{
	for(INT32 i = 0; i < m_triangles.Count(); ++i)
	{
		Triangle &tri = m_triangles[i];

		// Calculate two triangle edge vectors, assume clockwise winding
		CRuVector3 e0 = tri.m_v[1] - tri.m_v[0];
		CRuVector3 e1 = tri.m_v[2] - tri.m_v[1];

		e0.Normalize();
		e1.Normalize();

		// Calculate triangle normal
		CRuVector3 triNormal = CrossProduct(e0, e1).Normalize();

		// Calculate slope
		REAL slope = 1.0f - DotProduct(upVector, triNormal);

		if(slope < slopeLimit)
		{
			tri.m_walkable = TRUE;
		}
		else
		{
			tri.m_walkable = FALSE;
		}
	}

	// Release existing preview model
	ruSAFE_RELEASE(m_previewModel);

	return TRUE;
}

BOOL CRuPathEngine_NodeBuilder::TessellateGeometry()
{
	for(INT32 i = 0; i < m_triangles.Count(); ++i)
	{
		Triangle &tri = m_triangles[i];

		if(tri.m_walkable)
		{
			// Find longest triangle edge
			INT32 longestEdgeIdx = 0;
			REAL longestEdgeLen = 0.0f;
			for(INT32 j = 0; j < 3; ++j)
			{
				CRuVector3 edge = tri.m_v[(j + 1) % 3] - tri.m_v[j];

				if(edge.Magnitude() > longestEdgeLen)
				{
					longestEdgeIdx = j;
					longestEdgeLen = edge.Magnitude();
				}
			}

			// If longest edge exceeds size threshold, tessellate the triangle
			if(longestEdgeLen > m_unitSize)
			{
				// Calculate edge midpoint
				CRuVector3 midPt = (tri.m_v[(longestEdgeIdx + 1) % 3] - tri.m_v[longestEdgeIdx]) * 0.5f + tri.m_v[longestEdgeIdx];

				// Build triangles
				Triangle t0(tri.m_v[longestEdgeIdx], midPt, tri.m_v[(longestEdgeIdx + 2) % 3]);
				t0.m_walkable = TRUE;

				Triangle t1(midPt, tri.m_v[(longestEdgeIdx + 1) % 3], tri.m_v[(longestEdgeIdx + 2) % 3]);
				t1.m_walkable = TRUE;

				// Replace original triangle with t0
				tri = t0;

				// Insert t1 into triangle list only if it is within builder bounds
				if(IntersectsTriangle(m_bounds, t1.m_v[0], t1.m_v[1], t1.m_v[2]))
				{
					m_triangles.Add(t1);
				}

				// Restart process at the triangle t0
				--i;
			}
		}
	}

	// Release existing preview model
	ruSAFE_RELEASE(m_previewModel);

	return TRUE;
}

BOOL CRuPathEngine_NodeBuilder::GenerateCandidatePoints()
{
	// Process all triangles
	INT32 trisTested = 0;
	INT32 trisAccepted = 0;
	INT32 trisRejected = 0;

	CRuCollisionQuery_OBB_HitTest hitQuery;

	for(INT32 i = 0; i < m_triangles.Count(); ++i)
	{
		Triangle &tri = m_triangles[i];

		if(tri.m_walkable)
		{
			// Convert centroid (X, Z) into integer grid coordinates
			INT32 iX = static_cast<INT32>((tri.m_centroid.m_x - m_bounds.Minima().m_x) / m_unitSize + 0.5f);
			INT32 iZ = static_cast<INT32>((tri.m_centroid.m_z - m_bounds.Minima().m_z) / m_unitSize + 0.5f);

			// Calculate candidate position by snapping it to grid centroid
			CRuVector3 candidatePos(iX * m_unitSize + m_unitSize * 0.5f + m_bounds.Minima().m_x, tri.m_centroid.m_y, iZ * m_unitSize + m_unitSize * 0.5f  + m_bounds.Minima().m_z);

			// Look for merge-able candidate points (first-pass, for quick-rejection)
			BOOL skipCandidate = FALSE;
			for(INT32 j = 0, numCandidates = m_candidateGrid.Count(iX, iZ); j < numCandidates; ++j)
			{
				// Reference to candidate point
				CandidatePoint *candidate = m_candidateGrid.Get(iX, iZ, j);

				// If candidate points are within step height, merge them
				if(fabs(candidate->m_position.m_y - candidatePos.m_y) < m_stepHeight)
				{
					// Note: for the moment we simply discard the new candidate point
					skipCandidate = TRUE;
					break;
				}
			}

			if(skipCandidate == FALSE)
			{
				++trisTested;

				// Re-anchor candidate position to correct height
				if(!AnchorPoint(candidatePos))
				{
					continue;
				}

				// Look for merge-able candidate points (again... after we re-anchor)
				BOOL skipCandidate = FALSE;
				for(INT32 j = 0, numCandidates = m_candidateGrid.Count(iX, iZ); j < numCandidates; ++j)
				{
					// Reference to candidate point
					CandidatePoint *candidate = m_candidateGrid.Get(iX, iZ, j);

					// If candidate points are within step height, merge them
					if(fabs(candidate->m_position.m_y - candidatePos.m_y) < m_stepHeight)
					{
						// Note: for the moment we simply discard the new candidate point
						skipCandidate = TRUE;
						break;
					}
				}

				if(skipCandidate == FALSE)
				{
					// Determine validity of candidate point (make sure it does not intersect any triangles)
					if(!IsValidCandidatePosition(candidatePos))
					{
						continue;
					}

					// Generate new candidate point
					CandidatePoint *newCandidate = ruNEW CandidatePoint(candidatePos, iX, iZ);
					m_candidateGrid.Add(iX, iZ, newCandidate);

					// Increment accepted triangle count
					++trisAccepted;
				}
			}
		}
	}

	trisRejected = m_triangles.Count() - trisAccepted;

	// Verification code
	INT32 totalCandidatePoints = m_candidateGrid.Count();
	m_previewCentroids = TRUE;
	ruSAFE_RELEASE(m_previewModel);

	return TRUE;
}

BOOL CRuPathEngine_NodeBuilder::GeneratePathNode(IRuPathGraph_Base *pathGraph)
{
	INT32 nodesGenerated = 0;

	// Enumerate all candidate points
	CRuArrayList<CandidatePoint *> candidatePoints(1024);

	for(INT32 x = 0; x < m_candidateGrid.GetWidth(); ++x)
	{
		for(INT32 z = 0; z < m_candidateGrid.GetHeight(); ++z)
		{
			for(INT32 i = 0, candidateCount = m_candidateGrid.Count(x, z); i < candidateCount; ++i)
			{
				candidatePoints.Add(m_candidateGrid.Get(x, z, i));
			}
		}
	}

	// Generate permutation array and other associated arrays
	CRuArrayList<INT32> permArray(candidatePoints.Count());
	permArray.SetNumEntries(candidatePoints.Count());

	for(INT32 i = 0; i < candidatePoints.Count(); ++i)
	{
		permArray[i] = i;
		candidatePoints[i]->m_state = 0;
	}

	// Randomize the permutation array
	for(INT32 i = 0; i < candidatePoints.Count(); ++i)
	{
		INT32 swapIdx = RuGlobalMTRand().GenerateUINT32() % candidatePoints.Count();
		INT32 a = permArray[i];
		permArray[i] = permArray[swapIdx];
		permArray[swapIdx] = a;
	}

	for(INT32 i = 0; i < candidatePoints.Count(); ++i)
	{
		CandidatePoint &candidatePt = *candidatePoints[permArray[i]];

		// If candidate point has not been processed
		if(candidatePt.m_state == 0)
		{
			INT32 offsets[4][4] = { { 0, 0, 0, 1 }, { 0, 0, 1, 0 }, { 0, -1, 0, 0 }, { -1, 0, 0, 0 } };
			INT32 nodeGridBounds[4] = { candidatePt.m_iX, candidatePt.m_iZ, candidatePt.m_iX, candidatePt.m_iZ };

			CRuArrayList<CandidatePoint *> nodeCandidates;

			BOOL expandable = FALSE;

			do
			{
				expandable = FALSE;

				for(INT32 j = 0; j < 4; ++j)
				{
					// Expand in current cardinal direction
					INT32 newNodeGridBounds[4];

					newNodeGridBounds[0] = min(max(0, nodeGridBounds[0] + offsets[j][0]), m_candidateGrid.GetWidth() - 1);
					newNodeGridBounds[1] = min(max(0, nodeGridBounds[1] + offsets[j][1]), m_candidateGrid.GetHeight() - 1);
					newNodeGridBounds[2] = min(max(0, nodeGridBounds[2] + offsets[j][2]), m_candidateGrid.GetWidth() - 1);
					newNodeGridBounds[3] = min(max(0, nodeGridBounds[3] + offsets[j][3]), m_candidateGrid.GetHeight() - 1);

					if(nodeGridBounds[0] == newNodeGridBounds[0] && nodeGridBounds[1] == newNodeGridBounds[1] && nodeGridBounds[2] == newNodeGridBounds[2] && nodeGridBounds[3] == newNodeGridBounds[3])
					{
						continue;
					}

					REAL aspectRatio = static_cast<REAL>(newNodeGridBounds[2] - newNodeGridBounds[0] + 1) / static_cast<REAL>(newNodeGridBounds[3] - newNodeGridBounds[1] + 1);

					if(aspectRatio < 0.5f || aspectRatio > 2.0f)
					{
						continue;
					}

					// Test validity of current grid bounds
					CRuArrayList<CandidatePoint *> mergedCandidates;
					BOOL expansionValid = TRUE;
					for(INT32 x = newNodeGridBounds[0]; x <= newNodeGridBounds[2] && expansionValid; ++x)
					{
						for(INT32 z = newNodeGridBounds[1]; z <= newNodeGridBounds[3]; ++z)
						{
							// Find a compatible candidate
							BOOL compatibleCandidateFound = FALSE;
							for(INT32 k = 0, numCandidates = m_candidateGrid.Count(x, z); k < numCandidates; ++k)
							{
								// If a compatible candidate is valid, stop the search
								CandidatePoint &curCandidate = *m_candidateGrid.Get(x, z, k);
								if(curCandidate.m_state != 2 && fabs(candidatePt.m_position.m_y - curCandidate.m_position.m_y) < m_stepHeight)
								{
									mergedCandidates.Add(m_candidateGrid.Get(x, z, k));
									compatibleCandidateFound = TRUE;
									break;
								}
							}

							// If no compatible candidates exist, abort expansion of this cardinal direction
							if(!compatibleCandidateFound)
							{
								expansionValid = FALSE;
								break;
							}
						}
					}

					if(expansionValid)
					{
						// Copy new node grid bounds
						for(INT32 k = 0; k < 4; ++k)
						{
							nodeGridBounds[k] = newNodeGridBounds[k];
						}

						// Mark merged candidates as merged
						for(INT32 k = 0; k < mergedCandidates.Count(); ++k)
						{
							if(mergedCandidates[k]->m_state == 0)
							{
								nodeCandidates.Add(mergedCandidates[k]);
								mergedCandidates[k]->m_state = 1;
							}
						}

						expandable = TRUE;
					}
				}
			} while (expandable);

			// Mark all merged candidates as consumed
			for(INT32 j = 0; j < nodeCandidates.Count(); ++j)
			{
				nodeCandidates[j]->m_state = 2;
			}

			// Make an AABB
			CRuAABB nodeBounds;
			nodeBounds.Minima().Set(nodeGridBounds[0] * m_unitSize + m_bounds.Minima().m_x, candidatePt.m_position.m_y - 10.0f, nodeGridBounds[1] * m_unitSize + m_bounds.Minima().m_z);
			nodeBounds.Maxima().Set((nodeGridBounds[2] + 1) * m_unitSize + m_bounds.Minima().m_x, candidatePt.m_position.m_y + 10.0f, (nodeGridBounds[3] + 1) * m_unitSize + m_bounds.Minima().m_z);

			pathGraph->InsertPathNode(nodeBounds, FALSE);
/*
			CRuMatrix4x4 identityTransform;
			identityTransform.SetToIdentity();

			CRuOBB obb(nodeBounds, identityTransform);

			CRuGizmo_Box *boxGizmo = ruNEW CRuGizmo_Box();
			boxGizmo->SetBox(obb);

			CRuEntity *entity = static_cast<CRuEntity *>(this->GetPreviewModel());
			entity->AddChild(boxGizmo);

			ruSAFE_RELEASE(boxGizmo);
*/
			// Count number of nodes generated
			++nodesGenerated;
		}
	}

	return TRUE;
}

IRuObject *CRuPathEngine_NodeBuilder::GetPreviewModel()
{
	// Build the preview model if it does not exist
	if(m_previewModel == NULL)
	{
		CRuModel *outputModel = ruNEW CRuModel();

		if(m_previewCentroids)
		{
			// Enumerate all candidate points
			CRuArrayList<CandidatePoint *> candidatePoints(1024);

			for(INT32 x = 0; x < m_candidateGrid.GetWidth(); ++x)
			{
				for(INT32 z = 0; z < m_candidateGrid.GetHeight(); ++z)
				{
					for(INT32 i = 0, candidateCount = m_candidateGrid.Count(x, z); i < candidateCount; ++i)
					{
						candidatePoints.Add(m_candidateGrid.Get(x, z, i));
					}
				}
			}

			INT32 curPtIdx = 0;
			INT32 ptCount = candidatePoints.Count();

			while(curPtIdx < ptCount)
			{
				INT32 ptThisBatch = min(10000, ptCount - curPtIdx);

				CRuMesh *mesh = ruNEW CRuMesh();
				mesh->Initialize(ruPRIMTYPE_LINELIST, ptThisBatch * 4, ptThisBatch * 2, ruVERTEXFORMAT_POSITION);

				CRuMaterial *material = ruNEW CRuMaterial();
				material->SetProperty("material:materialType", "Standard Material");

				material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_NONE);
				material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 0);

				material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATIONCOLOR, RUCOLOR_ARGB(0, 128, 128, 64));
				material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(1.0f));

				material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_WIRE, FALSE);

				IRuMorphTarget *morphTarget = mesh->GetMorphTarget(0);
				IRuMeshTopology *meshTopology = mesh->GetMeshTopology();

				CRuVector3 *pos = morphTarget->GetPosition();
				UINT16 *indices = meshTopology->GetIndices();

				for(INT32 i = 0, j = 0; i < ptThisBatch; ++i, ++curPtIdx, j += 4)
				{
					pos[j + 0] = candidatePoints[curPtIdx]->m_position - CRuVector3(1.0f, 0.0f, 0.0f);
					pos[j + 1] = candidatePoints[curPtIdx]->m_position + CRuVector3(1.0f, 0.0f, 0.0f);
					pos[j + 2] = candidatePoints[curPtIdx]->m_position - CRuVector3(0.0f, 0.0f, 1.0f);
					pos[j + 3] = candidatePoints[curPtIdx]->m_position + CRuVector3(0.0f, 0.0f, 1.0f);

					indices[j + 0] = j + 0;
					indices[j + 1] = j + 1;
					indices[j + 2] = j + 2;
					indices[j + 3] = j + 3;
				}

				mesh->CalculateBounds();

				outputModel->AddSubModel(mesh, material);

				ruSAFE_RELEASE(mesh);
				ruSAFE_RELEASE(material);
			}
		}
		else
		{
			INT32 curTriIdx = 0;
			INT32 triCount = m_triangles.Count();

			while(curTriIdx < triCount)
			{
				INT32 trisThisBatch = min(20000, triCount - curTriIdx);

				CRuMesh *mesh = ruNEW CRuMesh();
				mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, trisThisBatch * 3, trisThisBatch, ruVERTEXFORMAT_POSITION);

				CRuMaterial *material = ruNEW CRuMaterial();
				material->SetProperty("material:materialType", "Standard Material");

				material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_NONE);
				material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 0);

				material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATIONCOLOR, RUCOLOR_ARGB(0, 128, 128, 64));
				material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(1.0f));

				material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_WIRE, TRUE);

				IRuMorphTarget *morphTarget = mesh->GetMorphTarget(0);
				IRuMeshTopology *meshTopology = mesh->GetMeshTopology();

				CRuVector3 *pos = morphTarget->GetPosition();
				UINT16 *indices = meshTopology->GetIndices();

				for(INT32 i = 0, j = 0; i < trisThisBatch; ++i, ++curTriIdx, j += 3)
				{
					if(m_triangles[curTriIdx].m_walkable)
					{
						for(INT32 k = 0; k < 3; ++k)
						{
							pos[j + k] = m_triangles[curTriIdx].m_v[k];
							indices[j + k] = j + k;
						}
					}
					else
					{
						for(INT32 k = 0; k < 3; ++k)
						{
							pos[j + k] = m_triangles[curTriIdx].m_v[0];
							indices[j + k] = j + k;
						}
					}
				}

				mesh->CalculateBounds();

				outputModel->AddSubModel(mesh, material);

				ruSAFE_RELEASE(mesh);
				ruSAFE_RELEASE(material);
			}
		}

		m_previewModel = outputModel;
	}

	return m_previewModel;
}

BOOL CRuPathEngine_NodeBuilder::AnchorPoint(CRuVector3 &point)
{
	BOOL retVal = FALSE;

	REAL errorThreshold = 0.0f;
	REAL anchorThreshold = 10.0f;

	// Formulate query for upwards test
	CRuCollisionQuery_Segment_Nearest upSegQuery;
	upSegQuery.m_colSegment.SetFromPoints(point + CRuVector3(0.0f, anchorThreshold, 0.0f), point + CRuVector3(0.0f, errorThreshold, 0.0f));
	upSegQuery.m_exclusionMask = m_triExclusionMask;
	upSegQuery.m_ignoreBackface = FALSE;

	// Perform query against all geometry
	for(INT32 j = 0; j < m_geometries.Count(); ++j)
	{
		if(m_geometries[j]->GetType().IsSubClassOf(IRuCollisionObject::Type()))
		{
			static_cast<IRuCollisionObject *>(m_geometries[j])->QueryCollision(&upSegQuery);
		}
		else if(m_geometries[j]->GetType().IsSubClassOf(IRuCollisionStructure::Type()))
		{
			static_cast<IRuCollisionStructure *>(m_geometries[j])->QueryCollision(&upSegQuery);
		}
		else if(m_geometries[j]->GetType().IsSubClassOf(CRuEntity::Type()))
		{
			static_cast<CRuEntity *>(m_geometries[j])->QueryCollision(&upSegQuery);
		}
	}

	if(upSegQuery.m_positiveHit)
	{
		point = upSegQuery.m_collisionPoint;
		retVal = TRUE;
	}
	else
	{
		// Formulate query for downwards test
		CRuCollisionQuery_Segment_Nearest downSegQuery;
		downSegQuery.m_colSegment.SetFromPoints(point - CRuVector3(0.0f, errorThreshold, 0.0f), point - CRuVector3(0.0f, anchorThreshold, 0.0f));
		downSegQuery.m_exclusionMask = m_triExclusionMask;
		downSegQuery.m_ignoreBackface = FALSE;

		// Perform query against all geometry
		for(INT32 j = 0; j < m_geometries.Count(); ++j)
		{
			if(m_geometries[j]->GetType().IsSubClassOf(IRuCollisionObject::Type()))
			{
				static_cast<IRuCollisionObject *>(m_geometries[j])->QueryCollision(&downSegQuery);
			}
			else if(m_geometries[j]->GetType().IsSubClassOf(IRuCollisionStructure::Type()))
			{
				static_cast<IRuCollisionStructure *>(m_geometries[j])->QueryCollision(&downSegQuery);
			}
			else if(m_geometries[j]->GetType().IsSubClassOf(CRuEntity::Type()))
			{
				static_cast<CRuEntity *>(m_geometries[j])->QueryCollision(&downSegQuery);
			}
		}

		if(downSegQuery.m_positiveHit)
		{
			point = downSegQuery.m_collisionPoint;
			retVal = TRUE;
		}
	}

	return retVal;
}

BOOL CRuPathEngine_NodeBuilder::IsValidCandidatePosition(const CRuVector3 &point)
{
	CRuCollisionQuery_OBB_HitTest hitQuery;
	CRuVector3 hitOBBExtents;
	CRuVector3 hitOBBOffset;

	hitOBBExtents.Set(m_unitSize * 0.5f, 9.0f, m_unitSize * 0.5f);
	hitOBBOffset.Set(0.0f, hitOBBExtents.m_y + m_stepHeight, 0.0f);

	// Determine validity of candidate point (make sure it does not intersect any triangles)
	hitQuery.m_colOBB.Set(point + hitOBBOffset, hitOBBExtents);
	hitQuery.m_exclusionMask = m_triExclusionMask;
	hitQuery.m_positiveHit = FALSE;

	for(INT32 j = 0; j < m_geometries.Count(); ++j)
	{
		if(m_geometries[j]->GetType().IsSubClassOf(IRuCollisionObject::Type()))
		{
			static_cast<IRuCollisionObject *>(m_geometries[j])->QueryCollision(&hitQuery);
		}
		else if(m_geometries[j]->GetType().IsSubClassOf(IRuCollisionStructure::Type()))
		{
			static_cast<IRuCollisionStructure *>(m_geometries[j])->QueryCollision(&hitQuery);
		}
		else if(m_geometries[j]->GetType().IsSubClassOf(CRuEntity::Type()))
		{
			static_cast<CRuEntity *>(m_geometries[j])->QueryCollision(&hitQuery);
		}
	}

	return hitQuery.m_positiveHit == FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
