/*!
	@project Rune
	@file RuEntity_PathGraphVis.h

	Copyright (c) 2005-2007 All rights reserved

	@author John Tang
	@date 2007/08/14
*/

#pragma once

#include "../../PathEngine/RuPathGraph_Base.h"
#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Path graph visualizer

	@author John Tang
	@date 2007/08/14
*/

class CRuEntity_PathGraphVis : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	class BatchDesc
	{
	public:
		INT32								m_numParticles;						//!< Number of particles in this batch
		REAL*								m_vsConstants;						//!< Vertex shader constants array
		CRuProxyMesh*						m_boxMesh;							//!< Proxy mesh
		CRuMaterial*						m_boxMaterial;						//!< Material to use for the batch

	public:
		BatchDesc(INT32 maxInstancesPerBatch)
		{
			// Create constants array
			m_vsConstants = ruNEW REAL [8 * maxInstancesPerBatch + 4];

			// Create proxy mesh
			m_boxMesh = ruNEW CRuProxyMesh();

			// Create material and assign constants array to it
			m_boxMaterial = ruNEW CRuMaterial();
			m_boxMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_EDITOR_GRAPHVIS"));
			m_boxMaterial->SetTexture(ruTEXCHANNEL_DIFFUSEMAP, g_ruResourceManager->LoadTexture("RUSYSTEM_GRID1X1"));
			m_boxMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, 0);
			m_boxMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_GEOINSTANCE_S_DATA, (UINT64) m_vsConstants);
			m_boxMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_TRANSLUCENT, TRUE);
			m_boxMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, ruALPHAMODE_NORMAL);
			m_boxMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_NONE);
			m_boxMaterial->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(0.5f));
		}

		~BatchDesc()
		{
			// Delete constants array
			delete[] m_vsConstants;

			// Release proxy mesh
			ruSAFE_RELEASE(m_boxMesh);

			// Release material
			ruSAFE_RELEASE(m_boxMaterial);
		}
	};

	// Constant settings
	INT32										m_maxNodesPerBatch;

	// Meshes
	CRuMesh*									m_boxMesh;

	IRuPathGraph_Base*							m_pathGraph;

	// Batch data
	CRuArrayList<BatchDesc *>					m_batches;							//!< Currently active batch list
	CRuStack<BatchDesc *>						m_freeBatchDescStack;				//!< Stack of unused batch descriptors

public:
												CRuEntity_PathGraphVis();
	virtual 									~CRuEntity_PathGraphVis();

	// IRuObject interface
	virtual IRuObject*							Clone(IRuObject *clonedObject = NULL);

	// Entity interface
	virtual BOOL								Update(REAL elapsedTime);

	// IRuEntity_Renderable interface
	virtual INT32								GetNumMeshes() const;
	virtual BOOL								GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	BOOL										SetPathGraph(IRuPathGraph_Base *pathGraph);

	BOOL										UpdateBatches();
	BOOL										ClearBatches();
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
