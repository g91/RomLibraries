/*!
	@file RuEntity_Particle_Doodad.h

	Copyright (c) 2004-2006 All rights reserved
*/

#ifndef _RUENTITY_PARTICLE_DOODAD_H_
#define _RUENTITY_PARTICLE_DOODAD_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuDoodad_Descriptor
{
public:
	UINT16									m_textureIndex;						//!< Texture index
	REAL									m_scale;							//!< Object scale
	CRuQuaternion							m_rotation;							//!< Object rotation
	CRuVector3								m_translation;						//!< Object translation
	REAL									m_swayMagnitude;					//!< Object swaying magnitude
};

// ************************************************************************************************************************************************************

class CRuDoodad_Template : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Template settings
	char*									m_doodadName;							//!< Doodad name
	char*									m_modelName;							//!< Model to use as basis for doodads
	char*									m_textureName;							//!< Texture to use as basis for doodads
	INT32									m_uSubdivision;							//!< Number of subdivisions along u
	INT32									m_vSubdivision;							//!< Number of subdivisions along v

	// Runtime
	IRuMesh*								m_mesh;									//!< Source mesh
	IRuMaterial*							m_material;								//!< Source material
	IRuTexture*								m_texture;								//!< Source texture

public:
											CRuDoodad_Template();
	virtual									~CRuDoodad_Template();

	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);

	// IRuObject serialization
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	const char*								GetDoodadName() const;
	void									SetDoodadName(const char *doodadName);

	const char*								GetModelName() const;
	void									SetModelName(const char *modelName);

	const char*								GetTextureName();
	void									SetTextureName(const char *textureName);

	INT32									GetUSubdivision() const;
	void									SetUSubdivision(INT32 uSubdivision);

	INT32									GetVSubdivision() const;
	void									SetVSubdivision(INT32 vSubdivision);

	IRuMesh*								GetMesh();
	IRuMaterial*							GetMaterial();
	IRuTexture*								GetTexture();

protected:
	void									ReloadModel();
	void									ReloadTexture();
};

// ************************************************************************************************************************************************************

class CRuDoodad_Cluster : public IRuEntity_Container
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	class BatchDesc
	{
	public:
		INT32								m_numDoodads;						//!< Number of doodads in this batch
		REAL*								m_vsConstants;						//!< Vertex shader constants array
		CRuProxyMesh*						m_mesh;								//!< Proxy mesh
		CRuMaterial*						m_material;							//!< Material to use for the batch

	public:
		BatchDesc(INT32 maxDoodadsPerBatch)
		{
			// Create constants array
			m_vsConstants = ruNEW REAL [12 * maxDoodadsPerBatch + 24];

			// Create proxy mesh
			m_mesh = ruNEW CRuProxyMesh();

			// Create material and assign constants array to it
			m_material = ruNEW CRuMaterial();
			m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_PARTICLE_DOODAD"));
			m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_GEOINSTANCE_S_COUNT, 0);
			m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_GEOINSTANCE_S_DATA, (UINT64) m_vsConstants);
		}

		~BatchDesc()
		{
			// Delete constants array
			delete[] m_vsConstants;

			// Release proxy mesh
			ruSAFE_RELEASE(m_mesh);

			// Release material
			ruSAFE_RELEASE(m_material);
		}
	};

	struct DoodadList
	{
		INT32								m_key;								//!< Key to which this doodad list is associated with
		CRuArrayList<CRuDoodad_Descriptor>*	m_doodadList;						//!< Doodad list
	};

	// Settings
	INT32									m_maxDoodadsPerBatch;				//!< Maximum number of particles to draw per batch

	REAL									m_periodInterval;					//!< Length of time, in seconds, of one period (PI)
	REAL									m_t;

	// Doodad template and data
	CRuDoodad_Template*						m_doodadTemplate;					//!< Doodad template
	CRuArrayList<DoodadList>				m_doodadLists;						//!<
	CRuStack<DoodadList>					m_freeDoodadList;					//!< Stack of unused doodad list descriptors

	// Rendering data
	BOOL									m_batchesDirty;
	CRuMesh*								m_mesh;								//!< Mesh
	CRuArrayList<BatchDesc *>				m_batches;							//!< Currently active batch list
	CRuStack<BatchDesc *>					m_freeBatchDescStack;				//!< Stack of unused batch descriptors

	CRuArrayList<CRuModel *>				m_models;
	CRuStack<CRuModel *>					m_freeModels;						//!< Stack of unused models

public:
											CRuDoodad_Cluster();
	virtual									~CRuDoodad_Cluster();

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	// IRuEntity_Renderable interface
//	virtual INT32							GetNumMeshes() const;
//	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	CRuDoodad_Template*						GetDoodadTemplate();
	void									SetDoodadTemplate(CRuDoodad_Template *doodadTemplate);

	void									InsertDoodadList(INT32 key, const CRuArrayList<CRuDoodad_Descriptor> &doodadList);
	void									RemoveDoodadLists(INT32 key);
	void									RemoveAllDoodadLists();

protected:
	void									CreateBatches();
	void									ClearBatches();
};

// ************************************************************************************************************************************************************

class CRuDoodad_ClusterManager : public CRuEntity_Container_Impl
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct ClusterDesc
	{
		CRuGUID								m_clusterGUID;						//!< GUID identifier for the cluster
		CRuDoodad_Cluster*					m_cluster;							//!< Cluster object
	};

	 CRuArrayList<ClusterDesc>				m_clusters;							//!< Cluster list

public:
											CRuDoodad_ClusterManager();
	virtual									~CRuDoodad_ClusterManager();

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	void									SetDoodadTemplate(const CRuGUID &clusterGUID, CRuDoodad_Template *doodadTemplate);

	void									InsertDoodadList(INT32 key, const CRuGUID &clusterGUID, const CRuArrayList<CRuDoodad_Descriptor> &doodadList);
	void									RemoveDoodadLists(INT32 key);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
