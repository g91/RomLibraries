#include "../../Scene/PathEngineVis/RuEntity_PathGraphVis.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_PathGraphVis, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_PathGraphVis, "CRuEntity_PathGraphVis", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuEntity_PathGraphVis, 0xF2D79EA5, 0x135242e1, 0x8AC4BD5D, 0x13F345FF)

// ************************************************************************************************************************************************************

CRuEntity_PathGraphVis::CRuEntity_PathGraphVis()
:	m_maxNodesPerBatch(100),
	m_boxMesh(NULL),
	m_pathGraph(NULL)
{
	IRuObject *meshObj = g_ruResourceManager->GetGenericObject("RUSYSTEM_PATHGRAPHVIS_BOXMESH");

	if(meshObj && meshObj->GetType().IsSubClassOf(CRuMesh::Type()))
	{
		m_boxMesh = static_cast<CRuMesh *>(meshObj);	
	}
	else
	{
		m_boxMesh = ruNEW CRuMesh();
		m_boxMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, 24 * m_maxNodesPerBatch, 12 * m_maxNodesPerBatch, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);
		m_boxMesh->AddTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);

		REAL extents[3] = { 0.5f, 0.5f, 0.5f };

		// Initialize box mesh
		CRuVector3 *position = static_cast<CRuMorphTarget *>(m_boxMesh->GetMorphTarget(0))->GetPosition();
		DWORD *diffuse = static_cast<CRuMorphTarget *>(m_boxMesh->GetMorphTarget(0))->GetDiffuse();
		REAL *uv = m_boxMesh->GetMorphTarget(0)->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
		UINT16 *indices = m_boxMesh->GetIndices();

		for(INT32 i = 0, j = 0, k = 0, l = 0, m = 0; i < m_maxNodesPerBatch; ++i)
		{
			INT32 v0Idx = 0;

			// Create box centered at (0, 0, 0) with the current box extents

			// Top face
			v0Idx = j;

			position[j++].Set( extents[0],  extents[1],  extents[2]);
			position[j++].Set( extents[0],  extents[1], -extents[2]);
			position[j++].Set(-extents[0],  extents[1], -extents[2]);
			position[j++].Set(-extents[0],  extents[1],  extents[2]);

			for(INT32 n = 0; n < 4; ++n)
			{
				diffuse[k++] = RUCOLOR_ARGB(i, 0xFF, 0xFF, 0xFF);
			}

			uv[m++] = 1.0f;
			uv[m++] = 1.0f;
			uv[m++] = 1.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 1.0f;

			indices[l++] = 0 + v0Idx;
			indices[l++] = 1 + v0Idx;
			indices[l++] = 2 + v0Idx;

			indices[l++] = 0 + v0Idx;
			indices[l++] = 2 + v0Idx;
			indices[l++] = 3 + v0Idx;

			// Bottom face
			v0Idx = j;

			position[j++].Set( extents[0], -extents[1],  extents[2]);
			position[j++].Set( extents[0], -extents[1], -extents[2]);
			position[j++].Set(-extents[0], -extents[1], -extents[2]);
			position[j++].Set(-extents[0], -extents[1],  extents[2]);

			for(INT32 n = 0; n < 4; ++n)
			{
				diffuse[k++] = RUCOLOR_ARGB(i, 0xFF, 0xFF, 0xFF);
			}

			uv[m++] = 1.0f;
			uv[m++] = 1.0f;
			uv[m++] = 1.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 1.0f;

			indices[l++] = 0 + v0Idx;
			indices[l++] = 2 + v0Idx;
			indices[l++] = 1 + v0Idx;

			indices[l++] = 0 + v0Idx;
			indices[l++] = 3 + v0Idx;
			indices[l++] = 2 + v0Idx;

			// Right face
			v0Idx = j;

			position[j++].Set( extents[0],  extents[1],  extents[2]);
			position[j++].Set( extents[0], -extents[1],  extents[2]);
			position[j++].Set( extents[0], -extents[1], -extents[2]);
			position[j++].Set( extents[0],  extents[1], -extents[2]);

			for(INT32 n = 0; n < 4; ++n)
			{
				diffuse[k++] = RUCOLOR_ARGB(i, 0xFF, 0xFF, 0xFF);
			}

			uv[m++] = 1.0f;
			uv[m++] = 1.0f;
			uv[m++] = 1.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 1.0f;

			indices[l++] = 0 + v0Idx;
			indices[l++] = 1 + v0Idx;
			indices[l++] = 2 + v0Idx;

			indices[l++] = 0 + v0Idx;
			indices[l++] = 2 + v0Idx;
			indices[l++] = 3 + v0Idx;

			// Left face
			v0Idx = j;

			position[j++].Set(-extents[0],  extents[1],  extents[2]);
			position[j++].Set(-extents[0], -extents[1],  extents[2]);
			position[j++].Set(-extents[0], -extents[1], -extents[2]);
			position[j++].Set(-extents[0],  extents[1], -extents[2]);

			for(INT32 n = 0; n < 4; ++n)
			{
				diffuse[k++] = RUCOLOR_ARGB(i, 0xFF, 0xFF, 0xFF);
			}

			uv[m++] = 1.0f;
			uv[m++] = 1.0f;
			uv[m++] = 1.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 1.0f;

			indices[l++] = 0 + v0Idx;
			indices[l++] = 2 + v0Idx;
			indices[l++] = 1 + v0Idx;

			indices[l++] = 0 + v0Idx;
			indices[l++] = 3 + v0Idx;
			indices[l++] = 2 + v0Idx;

			// Forward face
			v0Idx = j;

			position[j++].Set( extents[0],  extents[1],  extents[2]);
			position[j++].Set(-extents[0],  extents[1],  extents[2]);
			position[j++].Set(-extents[0], -extents[1],  extents[2]);
			position[j++].Set( extents[0], -extents[1],  extents[2]);

			for(INT32 n = 0; n < 4; ++n)
			{
				diffuse[k++] = RUCOLOR_ARGB(i, 0xFF, 0xFF, 0xFF);
			}

			uv[m++] = 1.0f;
			uv[m++] = 1.0f;
			uv[m++] = 1.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 1.0f;

			indices[l++] = 0 + v0Idx;
			indices[l++] = 1 + v0Idx;
			indices[l++] = 2 + v0Idx;

			indices[l++] = 0 + v0Idx;
			indices[l++] = 2 + v0Idx;
			indices[l++] = 3 + v0Idx;

			// Forward face
			v0Idx = j;

			position[j++].Set( extents[0],  extents[1], -extents[2]);
			position[j++].Set(-extents[0],  extents[1], -extents[2]);
			position[j++].Set(-extents[0], -extents[1], -extents[2]);
			position[j++].Set( extents[0], -extents[1], -extents[2]);

			for(INT32 n = 0; n < 4; ++n)
			{
				diffuse[k++] = RUCOLOR_ARGB(i, 0xFF, 0xFF, 0xFF);
			}

			uv[m++] = 1.0f;
			uv[m++] = 1.0f;
			uv[m++] = 1.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 0.0f;
			uv[m++] = 1.0f;

			indices[l++] = 0 + v0Idx;
			indices[l++] = 2 + v0Idx;
			indices[l++] = 1 + v0Idx;

			indices[l++] = 0 + v0Idx;
			indices[l++] = 3 + v0Idx;
			indices[l++] = 2 + v0Idx;
		}

		// Update mesh iteration and bounds
		m_boxMesh->IncrementMeshIteration();
		m_boxMesh->CalculateBounds();

		// Insert mesh into generic object dictionary
		g_ruResourceManager->AddGenericObject("RUSYSTEM_PATHGRAPHVIS_BOXMESH", m_boxMesh);
	}

	// Update local entity bounds
	this->SetLocalEntityBounds(m_boxMesh->GetBounds());
}

CRuEntity_PathGraphVis::~CRuEntity_PathGraphVis()
{
	ClearBatches();

	ruSAFE_RELEASE(m_boxMesh);

	ruSAFE_RELEASE(m_pathGraph);

	while(m_freeBatchDescStack.IsEmpty() == FALSE)
	{
		BatchDesc *batchDesc = m_freeBatchDescStack.Pop();
		delete batchDesc;
	}
}

IRuObject *CRuEntity_PathGraphVis::Clone(IRuObject *clonedObject)
{
	// This object cannot be cloned
	return NULL;
}

BOOL CRuEntity_PathGraphVis::Update(REAL elapsedTime)
{
	UpdateBatches();

	return IRuEntity_Renderable::Update(elapsedTime);
}

INT32 CRuEntity_PathGraphVis::GetNumMeshes() const
{
	return m_batches.Count();
}

BOOL CRuEntity_PathGraphVis::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex < m_batches.Count())
	{
		*meshOut = m_batches[meshIndex]->m_boxMesh;
		*materialOut = m_batches[meshIndex]->m_boxMaterial;
		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_PathGraphVis::SetPathGraph(IRuPathGraph_Base *pathGraph)
{
	if(pathGraph)
	{
		pathGraph->AddRef();
		ruSAFE_RELEASE(m_pathGraph);
		m_pathGraph = pathGraph;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_PathGraphVis::UpdateBatches()
{
	if(m_pathGraph)
	{
		// Clear previous batches
		ClearBatches();

		RUCOLOR colorMarkers[3] = { 0xFFFF9900, 0xFFFFFF00, 0x00000000 };

		for(INT32 pass = 0; ; ++pass)
		{
			// Build batches for all path nodes
			BatchDesc *batch = NULL;
			INT32 nodesThisBatch = 0;

			RUCOLOR colorThisPass = colorMarkers[pass] ? colorMarkers[pass] : 0xFFFFFFFF;

			for(INT32 i = 0, j = 4; i < m_pathGraph->GetNumPathNodes(); ++i)
			{
				// If the path node is marked, skip it
				if(!m_pathGraph->IsPathNodeMarked(i, colorMarkers[pass]))
				{
					continue;
				}

				// Detect if there is a need for a new batch
				if(nodesThisBatch >= m_maxNodesPerBatch || batch == NULL)
				{
					// Set number of instances for the previous batch if applicable
					if(batch)
					{
						batch->m_boxMesh->SetProxiedMesh(m_boxMesh, 0, nodesThisBatch * 12);
						batch->m_boxMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, nodesThisBatch);
						batch->m_boxMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, colorThisPass);
					}

					// Get a free batch descriptor
					if(m_freeBatchDescStack.IsEmpty())
					{
						batch = ruNEW BatchDesc(m_maxNodesPerBatch);
					}
					else
					{
						batch = m_freeBatchDescStack.Pop();
					}

					batch->m_vsConstants[0] = 256.0f;						// diffuse color scale factor - so we can get a proper integer index
					batch->m_vsConstants[1] = 2.0f;							// 2 constant rows per instance dataset
					batch->m_vsConstants[2] = 32.0f;						// Instance dataset begins at c32
					batch->m_vsConstants[3] = 0.0f;							// Unused

					// Reset constant index
					nodesThisBatch = 0;
					j = 4;

					// Add batch to batch list
					m_batches.Add(batch);
				}

				// Get current path node
				const CRuPathNode *pathNode = m_pathGraph->GetPathNode(i);

				// Initialize AABB representing the path node
				CRuVector3 pathNodePos = pathNode->Centroid();
				CRuVector3 pathNodeExtents = (pathNode->NodeBounds().Maxima() - pathNode->NodeBounds().Minima());

				// Setup constant data for this particle
				batch->m_vsConstants[j++] = pathNodeExtents.m_x;												// scale x
				batch->m_vsConstants[j++] = pathNodeExtents.m_y;												// scale y
				batch->m_vsConstants[j++] = pathNodeExtents.m_z;												// scale z
				batch->m_vsConstants[j++] = 0.0f;

				batch->m_vsConstants[j++] = pathNodePos.m_x;													// position x
				batch->m_vsConstants[j++] = pathNodePos.m_y;													// position y
				batch->m_vsConstants[j++] = pathNodePos.m_z;													// position z
				batch->m_vsConstants[j++] = 0.0f;

				++nodesThisBatch;
			}

			// Set number of instances for the last batch if applicable
			if(batch)
			{
				batch->m_boxMesh->SetProxiedMesh(m_boxMesh, 0, nodesThisBatch * 12);
				batch->m_boxMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, nodesThisBatch);
				batch->m_boxMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, colorThisPass);
			}

			if(colorMarkers[pass] == 0x00000000)
			{
				break;
			}
		}

		CRuSphere pathGraphBounds((m_pathGraph->Bounds().Minima() + m_pathGraph->Bounds().Maxima()) / 2.0f, (m_pathGraph->Bounds().Maxima() - m_pathGraph->Bounds().Minima()).Magnitude() / 2.0f);

		// Set local entity bounds
		this->SetLocalEntityBounds(pathGraphBounds);
	}

	return TRUE;
}

BOOL CRuEntity_PathGraphVis::ClearBatches()
{
	// Push all used batches onto stack
	for(INT32 i = 0; i < m_batches.Count(); ++i)
	{
		m_freeBatchDescStack.Push(m_batches[i]);
	}

	// Clear batches
	m_batches.Clear();

	m_boxMesh->ClearBatches();

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
