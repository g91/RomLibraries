#ifndef _RUWORLD_DOODADSYSTEM_H_
#define _RUWORLD_DOODADSYSTEM_H_

#include "../../Core/DataStore/RuDataStore_Base.h"
#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Particle/RuEntity_Particle_Doodad.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuWorld_DoodadCluster
{
public:
	CRuGUID								m_guid;								//!< Doodad GUID
	CRuArrayList<CRuDoodad_Descriptor>	m_doodads;							//!< Doodad descriptors
};

// ************************************************************************************************************************************************************

namespace RuWorld
{
	typedef boost::function<void (CRuArrayList<INT32> **, CRuArrayList<CRuWorld_DoodadCluster *> **)> DoodadEnumerator;		// Parameters: (enumeratedKeys, enumeratedClusters)
}

// ************************************************************************************************************************************************************

class CRuWorld_DoodadPaintSetting
{
public:
	CRuGUID									m_guid;								//!< Doodad GUID
	INT32									m_textureIndex;						//!< Texture index to use
	REAL									m_minScale;							//!< Minimum scale
	REAL									m_maxScale;							//!< Maximum scale
	REAL									m_swayMagnitude;					//!< Sway magnitude
	BOOL									m_randomRotation;					//!< Enable random doodad rotation
	BOOL									m_alignUpVector;					//!< Enable aligning of up vector with paint surface
	DWORD									m_colMask;							//!< Collision mask to use when detecting spray position
};

// ************************************************************************************************************************************************************

class CRuWorld_DoodadContainer : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Data store
	CRuDataStore_Base*										m_dataStore;							//!< Data store
	INT32													m_tableHandle_DoodadTemplate;			//!< Handle to doodad template table

	// Doodad template cache
	CRuLRUQueue<CRuGUID, CRuDoodad_Template *>*				m_templateQueue;						//!< LRU queue of doodad templates

	// Doodad template list (editor mode only)
	CRuArrayList<CRuDoodad_Template *>*						m_templateList;							//!< List of doodad templates used in by-index mode

	// Update list
	CRuArrayList<CRuDoodad_Template *>						m_changedTemplateList;					//!< List of changed templates
	CRuArrayList<CRuGUID>									m_deletedTemplateList;					//!< List of deleted GUIDs

	// Worker thread
	BOOL													m_enableDeferredLoading;
	CRuThread_WorkQueue<CRuGUID, CRuDoodad_Template*>*		m_workQueue;

public:
											CRuWorld_DoodadContainer();
	virtual									~CRuWorld_DoodadContainer();

	// Data source management
	BOOL									Attach(CRuDataStore_Base *dataStore, BOOL readOnly = TRUE);
	BOOL									SaveChanges();
	BOOL									CopyTo(CRuDataStore_Base *targetDataStore);
	BOOL									EnableDeferredLoading(BOOL enableDeferredLoading);

	// Doodad template database
	INT32									GetNumDoodadTemplates() const;
	BOOL									InsertDoodadTemplate(CRuDoodad_Template *doodadTemplate);
	BOOL									DeleteDoodadTemplate(const CRuGUID &guid);
	BOOL									MarkDoodadTemplateForUpdate(const CRuGUID &guid);
	CRuDoodad_Template*						GetDoodadTemplate(const CRuGUID &guid, BOOL &asyncPendingOut);
	CRuDoodad_Template*						GetDoodadTemplateByIndex(INT32 index);

	BOOL									ClearTemplateQueue();

protected:
	CRuDoodad_Template*						GetDoodadTemplateFromChangeList(const CRuGUID &guid);
	CRuDoodad_Template*						GetDoodadTemplateFromTemplateList(const CRuGUID &guid);

	BOOL									ClearTemplateQueueCallback(const void *key, void *data);

	BOOL									WorkerFunction();

#ifdef KALYDO
	static void								DownloadCallback(const BYTE* key, void* userData);
	void									KalydoQueue(const CRuGUID &guid);
#endif // KALYDO
};

// ************************************************************************************************************************************************************

class CRuWorld_DoodadManager : public CRuEntity_Container_Impl
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct ClusterTuple
	{
		INT32										m_key;
		CRuWorld_DoodadCluster*						m_cluster;

													ClusterTuple()
													{
													}

													ClusterTuple(INT32 key, CRuWorld_DoodadCluster *cluster)
													:	m_key(key),
														m_cluster(cluster)
													{
													}
	};

	CRuWorld_DoodadContainer*						m_doodadContainer;					//!< Doodad container
	RuWorld::DoodadEnumerator						m_enumerator;						//!< Function used for doodad GUID enumeration

	REAL											m_enumerationMarginSize;			//!< Size of margin to add to enumeration bounds
	CRuAABB											m_enumerationBounds;				//!< Current enumeration bounds including margin

	CRuQueue<ClusterTuple>							m_clusterWorkQueue;

	CRuLRUQueue<CRuGUID, CRuDoodad_Cluster *>*		m_clusterQueue;						//!< LRU queue of clusters

public:
											CRuWorld_DoodadManager();
	virtual									~CRuWorld_DoodadManager();

	// Container management
	BOOL									Attach(CRuWorld_DoodadContainer *container);
	BOOL									SetEnumerator(RuWorld::DoodadEnumerator enumerator);
	BOOL									ClearEnumerationCache();

	BOOL									UpdateEnumeration(const CRuAABB &enumationBounds);
	virtual BOOL							Update(REAL elapsedTime);

	BOOL									ClearClusterQueue();

protected:
	CRuDoodad_Cluster*						GetDoodadCluster(const CRuGUID &guid, BOOL &asyncPendingOut);

	BOOL									ClearClusterQueueCallback(const void *key, void *data);
	BOOL									ClearClusterCallback(const void *key, void *data);
	BOOL									EnumerateActiveClusterCallback(const void *key, void *data);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
