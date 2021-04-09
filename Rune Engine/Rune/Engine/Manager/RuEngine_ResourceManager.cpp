#include "../../Engine/Manager/RuEngine_ResourceManager.h"

// Rune base renderer core
#include "../../Engine/Base/RuMaterialBase.h"
#include "../../Engine/Geometry/RuMeshBase.h"

#include "../../Engine/Texture/RuTexture_Proxy.h"
#include "../../Engine/Texture/RuTextureUtility.h"

#include "../../Engine/Animation/RuAnimBase.h"

// Rune base entity system core
#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityACT.h"

#include "../../Helper/RuHelper_Entity.h"
#include "../../Helper/RuHelper_IRuEntity_Renderable.h"

#include <tchar.h>
#include "../../../Tools/FilePackage/cFilePackage.h"

#ifdef KALYDO
#include "SocialIntegration/Exception.h"

// a struct for downloading files.
struct CDownloadStruct
{
	CRuEngine_ResourceManager* resourceManager;
	CRuString workName;
	BOOL queueWork;
	int tries;
};

struct CDownloadDependenciesStruct
{
	CRuString workName;
	int dependencies;
	TKCPRequestResultCallback* callback;
	void* userData;
	int tries;
};

void CRuEngine_ResourceManager::textureDownloadCallback(const char* fileName, TKResult result, void* id)
{
	CDownloadStruct* ds = reinterpret_cast<CDownloadStruct*>(id);

	if (KRF_FAILURE(result))
	{
		if (--ds->tries > 0)
		{
			// try again
			krfRequestKCPFile(fileName, &textureDownloadCallback, id);
			return;
		}
	}
	g_ruResourceManager->RemovePending();


	if (ds->queueWork)
	{
		ds->resourceManager->m_streamingTextures.Add(ds->workName, ds->workEntry);

		ds->resourceManager->m_textureWorkQueue->QueueWork(ds->workName);
	}

	ds->resourceManager->m_filesDownloaded[ds->workName] = KRF_SUCCESS(result)? YES : NO;

	delete ds;
}

void CRuEngine_ResourceManager::KalydoQueueTexture(CRuString& canonicalTextureName, BOOL queueWork, TextureWorkEntry* workEntry)
{
	switch (GetDownloadState(canonicalTextureName))
	{
	case NO:
		break;
	case YES:
		if (queueWork)
		{
			m_streamingTextures.Add(canonicalTextureName, *workEntry);

			// Failed using KRF, queue for loading so it can set the fail flag.
			m_textureWorkQueue->QueueWork(canonicalTextureName);
		}
		return;
	case PENDING:
		if (queueWork)
			break;
	case PENDING_QUEUE:
	case FAILED:
		return;
	}
	// check whether we need to download this texture
	// Iterate through all texture file extensions
	const INT32 numTexExtensions = 4;
	const char *texExtensions[] = { ".dds", ".tga", ".png", ".bmp" };

	CDownloadStruct* ds = new CDownloadStruct;
	ds->workName = canonicalTextureName;
	ds->resourceManager = this;
	ds->queueWork = queueWork;
	ds->tries = 10;
	if (workEntry)
		memcpy(&ds->workEntry, workEntry, sizeof(TextureWorkEntry));
	else
		memset(&ds->workEntry, 0, sizeof(TextureWorkEntry));

	TKResult res = KR_FILENOTFOUND;
	if (canonicalTextureName.GetLength() != 0 && canonicalTextureName.GetLength() != -1)
	{
		for(INT32 i = 0; i < numTexExtensions; ++i)
		{
			canonicalTextureName.Append(texExtensions[i]);

			if (static_cast<const char *>(canonicalTextureName) == NULL)
			{
				int i = 0;
			}
			res = krfRequestKCPFile(static_cast<const char *>(canonicalTextureName), &textureDownloadCallback, ds);

			canonicalTextureName.TruncateAt(canonicalTextureName.LastIndexOf("."));

			if (res != KR_FILENOTFOUND)
			{
				break;
			}
		}
	}
	if (!KRF_SUCCESS(res))
	{
		m_filesDownloaded[canonicalTextureName] = FAILED;
		if (queueWork)
		{
			if (workEntry)
				m_streamingTextures.Add(canonicalTextureName, *workEntry);

			// Failed using KRF, queue for loading so it can set the fail flag.
			m_textureWorkQueue->QueueWork(canonicalTextureName);
		}

		delete ds;  
	}
	else
	{
		g_ruResourceManager->AddPending();
		if (res == KR_IO_PENDING)
			m_filesDownloaded[canonicalTextureName] = queueWork ? PENDING_QUEUE : PENDING;
	}
}

void CRuEngine_ResourceManager::objectDownloadCallback(const char* fileName, TKResult result, void* id)
{
	CDownloadStruct* ds = reinterpret_cast<CDownloadStruct*>(id);

	if (KRF_FAILURE(result))
	{
		if (--ds->tries > 0)
		{
			// try again
			krfRequestKCPFile(fileName, &textureDownloadCallback, id);
			return;
		}
	}
	g_ruResourceManager->RemovePending();

	if (ds->queueWork)
		ds->resourceManager->m_objectWorkQueue->QueueWork(ds->workName);

	ds->resourceManager->m_filesDownloaded[ds->workName] = KRF_SUCCESS(result)? YES : NO;

	delete ds;
}

void CRuEngine_ResourceManager::KalydoQueueObject(CRuString& entityName, BOOL queueWork)
{
	switch (m_filesDownloaded[entityName])
	{
	case NO:
		break;
	case YES:
		if (queueWork)
			m_objectWorkQueue->QueueWork(entityName);
		return;
	case PENDING:
		if (!queueWork)
			break;
	case PENDING_QUEUE:
	case FAILED:
		return;
	}

	CDownloadStruct* ds = new CDownloadStruct;
	ds->workName = entityName;
	ds->resourceManager = this;
	ds->queueWork = queueWork;
	ds->tries = 10;

	TKResult res = krfRequestKCPFile(static_cast<const char *>(entityName), &objectDownloadCallback, ds);
	
	if (KRF_SUCCESS(res))
	{
		if (res == KR_IO_PENDING)
			m_filesDownloaded[entityName] = queueWork ? PENDING_QUEUE : PENDING;
		g_ruResourceManager->AddPending();
	}
	else
	{
		m_filesDownloaded[entityName] = FAILED;
		if (queueWork)
			// Failed using KRF, queue for loading so it can set the fail flag.
			m_objectWorkQueue->QueueWork(entityName);

		delete ds;  
	}
	
}

void CRuEngine_ResourceManager::streamDownloadCallback(const char* fileName, TKResult result, void* id)
{
	CDownloadStruct* ds = reinterpret_cast<CDownloadStruct*>(id);

	if (KRF_FAILURE(result))
	{
		if (--ds->tries > 0)
		{
			// try again
			krfRequestKCPFile(fileName, &textureDownloadCallback, id);
			return;
		}
	}
	g_ruResourceManager->RemovePending();

	if (ds->queueWork)
		ds->resourceManager->m_streamWorkQueue->QueueWork(ds->workName);

	ds->resourceManager->m_filesDownloaded[ds->workName] = KRF_SUCCESS(result)?YES:NO;

	delete ds;
}

void CRuEngine_ResourceManager::KalydoQueueStream(CRuString& streamName, BOOL queueWork)
{
	switch (m_filesDownloaded[streamName])
	{
	case NO:
		// continue loading
		break;
	case PENDING:
	case YES:
		if (!queueWork)
			// do nothing ...
			return;
		// make sure it is queued
		break;
	case PENDING_QUEUE:
	case FAILED:
		// do nothing
		return;
	}
	CDownloadStruct* ds = new CDownloadStruct;
	ds->workName = streamName;
	ds->resourceManager = this;
	ds->queueWork = queueWork;
	ds->tries = 10;

	TKResult res = krfRequestKCPFile(static_cast<const char *>(streamName), &streamDownloadCallback, ds);
	if (KRF_SUCCESS(res))
	{
		if (res == KR_IO_PENDING)
			m_filesDownloaded[streamName] = queueWork ? PENDING_QUEUE : PENDING;
		g_ruResourceManager->AddPending();
	}
	else
	{
		m_filesDownloaded[streamName] = FAILED;

		if (queueWork)
			// Failed using KRF, queue for loading so it can set the fail flag.
			m_streamWorkQueue->QueueWork(streamName);

		delete ds;  
	}
	
}

void CRuEngine_ResourceManager::animationDownloadCallback(const char* fileName, TKResult result, void* id)
{
	CDownloadStruct* ds = reinterpret_cast<CDownloadStruct*>(id);

	if (KRF_FAILURE(result))
	{
		if (--ds->tries > 0)
		{
			// try again
			krfRequestKCPFile(fileName, &textureDownloadCallback, id);
			return;
		}
	}
	g_ruResourceManager->RemovePending();	

	CRuAnimation *animation = NULL;

	const char* animationName = static_cast<const char*>(ds->workName);
	ds->resourceManager->m_filesDownloaded[ds->workName] = KRF_SUCCESS(result)?YES:NO;

	delete ds;
}

void CRuEngine_ResourceManager::KalydoQueueAnimation(CRuString& animationName)
{
	switch (m_filesDownloaded[animationName])
	{
	case NO:
		// continue loading
		break;
	case PENDING:
	case PENDING_QUEUE:
	case YES:
	case FAILED:
		// do nothing ...
		return;
	}
	m_filesDownloaded[animationName] = PENDING;

	CDownloadStruct* ds = new CDownloadStruct;
	ds->workName = animationName;
	ds->resourceManager = this;
	ds->tries = 10;

	TKResult res = krfRequestKCPFile(static_cast<const char *>(animationName), &animationDownloadCallback, ds);
	
	if (KRF_SUCCESS(res))
	{
		g_ruResourceManager->AddPending();
	}
	else
	{
		m_filesDownloaded[animationName] = FAILED;
		delete ds;  
	}
}

void CRuEngine_ResourceManager::backgroundDownloadCallback(const char* fileName, TKResult result, void* id)
{
	// when failed; just ignore the background download.

	//if (KRF_FAILURE(result))
	//{
	//	// try again
	//	krfRequestKCPFile(fileName, &backgroundDownloadCallback, id);
	//	return;
	//}

	CRuEngine_ResourceManager* rm = reinterpret_cast<CRuEngine_ResourceManager*>(id);

	if (KRF_SUCCESS(result))
		rm->m_filesDownloaded[fileName] = YES;

	EnterCriticalSection(&rm->m_backgroundDownloadCritSec);

#ifdef _DEBUG
	CRuString fileNameInQueue = rm->m_backgroundDownloadQueue.front();
	KASSERT(strcmp(static_cast<const char *>(fileNameInQueue), fileName) == 0, "Error in queue!", "Fix queue", ET_PROGGEN, SL_ERROR);
#endif
	// pop, and get the next one.
	rm->m_backgroundDownloadQueue.pop();

	while (!rm->m_backgroundDownloadQueue.empty())
	{
		CRuString nextFileName = rm->m_backgroundDownloadQueue.front();

		TKResult res = krfRequestKCPFile(static_cast<const char *>(nextFileName), &backgroundDownloadCallback, rm);
		// check if failure; if failure just go to the next one.
		if (KRF_SUCCESS(res))
		{
			if (res == KR_IO_PENDING)
				rm->m_filesDownloaded[nextFileName] = PENDING;
			// wait until the next is finished.
			break;
		}
		// ignore this one; maybe the file was not found.
		rm->m_backgroundDownloadQueue.pop();
	}

	LeaveCriticalSection(&rm->m_backgroundDownloadCritSec);
}


void CRuEngine_ResourceManager::BackgroundDownload(const char* fileName)
{
	EnterCriticalSection(&m_backgroundDownloadCritSec);
	bool wasEmpty = m_backgroundDownloadQueue.empty();
	m_backgroundDownloadQueue.push(fileName);
	if (wasEmpty)
	{
		// it is empty, so there were no running background downloads. download this one
		TKResult res = krfRequestKCPFile(static_cast<const char *>(fileName), &backgroundDownloadCallback, this);
		if (KRF_FAILURE(res))
		{
			// failure (file not found??) .. remove from queue (safe because of the critical section)
			m_backgroundDownloadQueue.pop();
		}

		if (res == KR_IO_PENDING)
			m_filesDownloaded[fileName] = PENDING;

		// no need to add pending; background downloading is not needed for startup.
	}
	LeaveCriticalSection(&m_backgroundDownloadCritSec);
}

unsigned int CRuEngine_ResourceManager::GetNumBackgroundDownloadsPending()
{
	unsigned int size;
	EnterCriticalSection(&m_backgroundDownloadCritSec);
	size = m_backgroundDownloadQueue.size();
	LeaveCriticalSection(&m_backgroundDownloadCritSec);
	return size;
}

DownloadState CRuEngine_ResourceManager::GetDownloadState(const char* fileName)
{
	stdext::hash_map< CRuString, DownloadState >::iterator i = m_filesDownloaded.find(fileName);
	if (i == m_filesDownloaded.end())
		return NO;
	return i->second;
}

void CRuEngine_ResourceManager::dependenciesDownloadCallback(const char* fileName, TKResult result, void* id)
{
	CDownloadDependenciesStruct* ds = reinterpret_cast<CDownloadDependenciesStruct*>(id);
	if (KRF_FAILURE(result))
	{
		if (--ds->tries > 0)
		{
			// try again
			krfRequestKCPFile(fileName, &dependenciesDownloadCallback, id);
			return;
		}
	}
	g_ruResourceManager->RemovePending();

	size_t fileLen = strlen(fileName);

	// for models, load all its dependencies
	if (fileLen > 4 && strcmp(fileName + fileLen - 4, ".ros") == 0)
	{
		// download dependencies
		std::vector<CRuString> dependencies;
		g_ruResourceManager->GetEntityDependencies(fileName, dependencies);

		for (std::vector<CRuString>::iterator i = dependencies.begin(); i != dependencies.end(); i++)
		{
			if (strcmp(fileName, static_cast<const char*>(*i)) != 0)
			{
				ds->dependencies++;
				if (KRF_SUCCESS(krfRequestKCPFile(static_cast<const char*>(*i), &dependenciesDownloadCallback, id)))
					g_ruResourceManager->AddPending();
				else
					ds->dependencies--;
			}
		}
			
		// exit this if, so we can return if there are dependencies and continue if there aren't.
	}

	ds->dependencies--;

	if (ds->dependencies != 0)
	{
		// wait for the rest of the dependencies
		return;
	}

	if (ds->callback)
		ds->callback(static_cast<const char*>(ds->workName), result, ds->userData);

	delete ds;
}

TKResult CRuEngine_ResourceManager::DownloadEntityAndDependencies(const char* fileName, TKCPRequestResultCallback* callback, void* id)
{
	CDownloadDependenciesStruct* ds = new CDownloadDependenciesStruct;

	ds->workName = fileName;
	ds->callback = callback;
	ds->userData = id;
	ds->dependencies = 1;
	ds->tries = 10;

	TKResult res = krfRequestKCPFile(fileName, &dependenciesDownloadCallback, ds);
	if (KRF_SUCCESS(res))
		g_ruResourceManager->AddPending();
	else
	{
		if (ds->callback)
			ds->callback(fileName, res, id);

		delete ds;		
	}

	return res;
}

void CRuEngine_ResourceManager::LoadingStart()
{
	m_downloadingStatus = 0.0f;
	m_filesPendingAtLoadStart = m_filesPending;
}

void CRuEngine_ResourceManager::AddPending(INT32 count)
{
	m_filesPendingAtLoadStart += count;
	m_filesPending += count;
}

void CRuEngine_ResourceManager::RemovePending(INT32 count)
{
	m_filesPending -= count;
}

float CRuEngine_ResourceManager::GetDownloadingStatus()
{
	if (m_filesPendingAtLoadStart == 0 || m_filesPending == 0)
		return 1.0f;

	float downloadingStatus = 1.0f - ((float)m_filesPending / (float)m_filesPendingAtLoadStart);

	// only go forwards when loading.
	if (downloadingStatus > m_downloadingStatus)
		m_downloadingStatus = downloadingStatus;

	return m_downloadingStatus;
}

#endif //KALYDO

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEngine_ResourceManager, IRuEngine_ResourceManager)
ruHEAP_DEFINE_SUBCLASS(CRuEngine_ResourceManager, "CRuEngine_ResourceManager", "IRuEngine_ResourceManager")
ruCLASSGUID_DEFINE(CRuEngine_ResourceManager, 0xEC2A9DFE, 0x2D7E4dc6, 0x9EA267B9, 0x7E6F5AC5)

// ************************************************************************************************************************************************************

CRuEngine_ResourceManager::CRuRenderTargetPool::CRuRenderTargetPool(IRuEngine *engine, INT32 rtWidth, INT32 rtHeight, RuSurfaceFormat rtFormat)
{
	// Store pool parameters
	m_rtWidth = rtWidth;
	m_rtHeight = rtHeight;
	m_rtFormat = rtFormat;
}

CRuEngine_ResourceManager::CRuRenderTargetPool::~CRuRenderTargetPool()
{
	ClearRenderTargetPool();
}

BOOL CRuEngine_ResourceManager::CRuRenderTargetPool::IsIdenticalPoolType(INT32 rtWidth, INT32 rtHeight, RuSurfaceFormat rtFormat)
{
	if(m_rtWidth == rtWidth && m_rtHeight == rtHeight && m_rtFormat == rtFormat)
	{
		return TRUE;
	}

	return FALSE;
}

INT32 CRuEngine_ResourceManager::CRuRenderTargetPool::GetMatchScore(INT32 rtWidth, INT32 rtHeight, RuSurfaceFormat rtFormat)
{
/*
	// If there are no free render targets, return a score of zero
	if(m_freeRenderTargets.IsEmpty())
	{
		return 0;
	}
*/
	// If the pool is a perfect match, return a score of 100
	if(m_rtWidth == rtWidth && m_rtHeight == rtHeight && m_rtFormat == rtFormat)
	{
		return 100;
	}

//	return 50;
	return 0;
}

IRuTexture *CRuEngine_ResourceManager::CRuRenderTargetPool::GetRenderTarget()
{
/*
	// If there are no free render targets, return NULL
	if(m_freeRenderTargets.IsEmpty())
	{
		return NULL;
	}
*/

	if(m_freeRenderTargets.IsEmpty())
	{
		IRuTexture *rt = g_ruEngine->CreateRenderTarget(m_rtWidth, m_rtHeight, 1, m_rtFormat);

		if(rt)
		{
			m_usedRenderTargets.Add(rt);
			return rt;
		}
		else
		{
			return NULL;
		}
	}

	// Pop a free render target
	IRuTexture *rt = m_freeRenderTargets.Pop();
	m_usedRenderTargets.Add(rt);

	return rt;
}

BOOL CRuEngine_ResourceManager::CRuRenderTargetPool::FreeRenderTarget(IRuTexture *renderTarget)
{
	// Find the given render target and place it in the free pool
	for(INT32 i = 0; i < m_usedRenderTargets.Count(); ++i)
	{
		if(m_usedRenderTargets[i] == renderTarget)
		{
			m_freeRenderTargets.Push(renderTarget);
			m_usedRenderTargets.RemoveAt(i);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuEngine_ResourceManager::CRuRenderTargetPool::ResetRenderTargetPool()
{
	// Put every render target back into the free pool
	for(INT32 i = 0; i < m_usedRenderTargets.Count(); ++i)
	{
		m_freeRenderTargets.Push(m_usedRenderTargets[i]);
	}

	m_usedRenderTargets.Clear();

	return TRUE;
}

BOOL CRuEngine_ResourceManager::CRuRenderTargetPool::ClearRenderTargetPool()
{
	// Release all render targets in the free pool
	while(m_freeRenderTargets.IsEmpty() == FALSE)
	{
		// Release the render target
		m_freeRenderTargets.Pop()->Release();
	}

	// Release all render targets in the used pool
	while(m_usedRenderTargets.Count() > 0)
	{
		// Release the render target
		m_usedRenderTargets[0]->Release();

		// Remove the render target entry
		m_usedRenderTargets.RemoveAt(0);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuEngine_ResourceManager::CRuEngine_ResourceManager()
{
	// Create file manager
	m_filePackage = ruNEW cFilePackage();

	// Create resource dictionary hash tables
	m_streamTree = ruNEW CRuTernaryStringTree<IRuStream *>();
	m_textureHash = ruNEW CRuTernaryStringTree<TextureDesc>();
	m_animationHash = ruNEW CRuStringHash<CRuAnimation *>(4096);
	m_animationHash_GR2 = ruNEW CRuStringHash<CRuAnimation_GR2 *>(4096);
	m_entityHash = ruNEW CRuTernaryStringTree<ObjectDesc>();
	m_genericObjectTree = ruNEW CRuTernaryStringTree<IRuObject *>();

	// Create render target atlas
	m_rtAtlas = ruNEW CRuTextureAtlas_Aggregate();

	// Set auto-bind textures to true
	m_enableTextureLoading = TRUE;

	// Create critical sections
	m_filePackage_CS = ruNEW CRuThread_CriticalSection();

	// Create performance probes
	m_probe_TextureLoads = g_ruProfiler->CreateProbe("TexRds", "Engine");
	m_probe_TextureLoadFails = g_ruProfiler->CreateProbe("Failed TexRds", "Engine");
	m_probe_TextureLoadTransientFails = g_ruProfiler->CreateProbe("Failed TexRds [T]", "Engine");
	m_probe_EntityLoads = g_ruProfiler->CreateProbe("EntRds", "Engine");
	m_probe_EntityLoadFails = g_ruProfiler->CreateProbe("Failed EntRds", "Engine");
	m_probe_EntityLoadTransientFails = g_ruProfiler->CreateProbe("Failed EntRds [T]", "Engine");

	// Create work queues
	m_streamWorkQueue = ruNEW CRuThread_WorkQueue<char *, IRuStream *>();
	m_objectWorkQueue = ruNEW CRuThread_WorkQueue<char *, IRuObject *>();
	m_textureWorkQueue = ruNEW CRuThread_WorkQueue<char *, IRuBaseTexture *>();

	// Journal
	m_journalOn = FALSE;

	// Register worker function with worker thread
	g_ruGlobalWorkerThread->RegisterWorkerFunction(m_workerFunctionGUID, boost::bind(&CRuEngine_ResourceManager::WorkerFunction, this));
#ifdef KALYDO
	InitializeCriticalSection(&m_backgroundDownloadCritSec);
	
	m_filesPendingAtLoadStart = 0;
	m_filesPending = 0;
#endif // KALYDO
}

CRuEngine_ResourceManager::~CRuEngine_ResourceManager()
{
	// Unregister worker function
	g_ruGlobalWorkerThread->UnregisterWorkerFunction(m_workerFunctionGUID);

	// Unregister device reset handler
	if(m_engine)
	{
		m_engine->Event_DeviceReset().UnregisterHandler(this->GetGUID());
	}

	// Release render target pools
	for(INT32 i = 0; i < m_renderTargetPools.Count(); ++i)
	{
		delete m_renderTargetPools[i];
	}

	// Unpin all textures
	UnpinAllTextures();

	// Release textures held by the dictionary
	ClearTextureDictionary();

	// Release animations held by the dictionary
	ClearAnimationDictionary();
	ClearAnimationDictionary_GR2();

	// Release entities held by the dictionary
	ClearEntityDictionary();

	// Clear generic object dictionary
	ClearGenericObjectDictionary();

	// Release file manager
	delete m_filePackage;

	// Release resource dictionaries
	delete m_textureHash;
	delete m_animationHash;
	delete m_animationHash_GR2;
	delete m_entityHash;
	delete m_genericObjectTree;

	// Release render target atlas
	ruSAFE_RELEASE(m_rtAtlas);

	// Destroy critical sections
	delete m_filePackage_CS;

	// Destroy work queues
	char *orphanedStreamKey;
	IRuStream *orphanedStream;
	while(m_streamWorkQueue->DequeueResult(&orphanedStreamKey, orphanedStream))
	{
		delete[] orphanedStreamKey;
		delete orphanedStream;
	}

	delete m_streamWorkQueue;

	char *orphanedObjectKey;
	IRuObject *orphanedObject;
	while(m_objectWorkQueue->DequeueResult(&orphanedObjectKey, orphanedObject))
	{
		delete[] orphanedObjectKey;
		ruSAFE_RELEASE(orphanedObject);
	}

	delete m_objectWorkQueue;

	char *orphanedTextureKey;
	IRuBaseTexture *orphanedTexture;
	while(m_textureWorkQueue->DequeueResult(&orphanedTextureKey, orphanedTexture))
	{
		delete[] orphanedTextureKey;
		ruSAFE_RELEASE(orphanedTexture);
	}

	delete m_textureWorkQueue;

#ifdef KALYDO
	DeleteCriticalSection(&m_backgroundDownloadCritSec);
#endif //KALYDO

}
					
INT32 CRuEngine_ResourceManager::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuEngine_ResourceManager::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	m_temp_Footprint = 0;
	m_temp_EnumHash = enumHash;
	m_textureHash->ForAllElements(boost::bind(&CRuEngine_ResourceManager::TextureDictionaryMemoryFootprintCallback, this, _1, _2));
	m_entityHash->ForAllElements(boost::bind(&CRuEngine_ResourceManager::EntityDictionaryMemoryFootprintCallback, this, _1, _2));
	footprint += m_temp_Footprint;

	return footprint;
}

BOOL CRuEngine_ResourceManager::SetEngine(IRuEngine *engine)
{
	// Set engine reference
	m_engine = engine;

	// Schedule work queue processing function
	g_ruFunctionSchedule->ScheduleFunction(m_guid_ProcessWorkQueues, 0, 0, 0, boost::bind(&CRuEngine_ResourceManager::ProcessWorkQueues, this, _1), NULL);

	// Schedule garbage collect function
	g_ruFunctionSchedule->ScheduleFunction(m_guid_GarbageCollect, 0, 0, 2500, boost::bind(&CRuEngine_ResourceManager::GarbageCollect, this, _1), NULL);

	// Register event handlers on engine
	m_engine->Event_DeviceReset().RegisterHandler(this->GetGUID(), boost::bind(&CRuEngine_ResourceManager::DeviceReset_Handler, this, _1));

	return TRUE;
}

IRuEngine *CRuEngine_ResourceManager::GetEngine()
{
	return m_engine;
}

const char *CRuEngine_ResourceManager::GetRootDirectory()
{
	return m_filePackage->GetDirectory();
}

void CRuEngine_ResourceManager::SetRootDirectory(const char *rootDirectory)
{
	m_filePackage_CS->Enter();
	{
		if(m_filePackage)
		{
			delete m_filePackage;
		}

		m_filePackage = ruNEW cFilePackage();
		m_filePackage->SetDirectory(rootDirectory);

		m_filePackage->GetDirectory();
	}
	m_filePackage_CS->Leave();
}

void CRuEngine_ResourceManager::LoadPackageFile(const char *filename, bool keepHandle)
{
	m_filePackage_CS->Enter();
	{
		if ( m_filePackage )
		{
			char fullPath[2048];
			sprintf(fullPath, "%s%s", m_filePackage->GetDirectory(), filename);

			m_filePackage->LoadFromFile(fullPath, keepHandle);
		}
	}
	m_filePackage_CS->Leave();
}

BOOL CRuEngine_ResourceManager::FileExists(const char* filename)
{
	BOOL retVal = FALSE;

	m_filePackage_CS->Enter();
	{
		if(m_filePackage && m_filePackage->FileExists(filename))
		{
			retVal = TRUE;
		}
	}
	m_filePackage_CS->Leave();

	return retVal;
}

BOOL CRuEngine_ResourceManager::SetObjectDequeueRate(REAL dequeueRate)
{
	return TRUE;
}

BOOL CRuEngine_ResourceManager::SetTextureDequeueRate(REAL dequeueRate)
{
	return TRUE;
}

BOOL CRuEngine_ResourceManager::EnableTextureLoading(BOOL enableTextureLoading)
{
	m_enableTextureLoading = enableTextureLoading;
	return m_enableTextureLoading;
}

BOOL CRuEngine_ResourceManager::OpenTempFileStream(const char *resourceName, CRuFileStream &fileStream)
{
	if(m_filePackage)
	{
		return m_filePackage->GetCRuTempFileStream(resourceName, fileStream);
	}

	return NULL;
}

IRuStream *CRuEngine_ResourceManager::LoadStream(const char *resourceName, bool boLoadAllData)
{
#if RU_ENGING_RESOURCE_MANAGER_DEBUG_PRINT_LOAD_TIME
	DWORD t0, t1, et;
	t0 = GetTickCount();
#endif

	IRuStream *stream = NULL;
	
	stream = LoadStream_Internal(resourceName, boLoadAllData);

#ifdef KALYDO
	if (stream == NULL)
		// oops, don't have the stream ... queue it for downloading so it is there the next time
		KalydoQueueStream(CRuString(resourceName), FALSE);
#endif // KALYDO
#if RU_ENGING_RESOURCE_MANAGER_DEBUG_PRINT_LOAD_TIME
    t1 = GetTickCount();
    et = t1 - t0;
    char outputStr[256];
    sprintf(outputStr, "Stream %s loaded in %d ms.\n", resourceName, et);
    OutputDebugString(outputStr);
#endif

	return stream;
}

BOOL CRuEngine_ResourceManager::LoadStream(const char *resourceName, IRuStream **streamOut)
{
	BOOL retVal = FALSE;

	if(resourceName == NULL)
	{
		return FALSE;
	}

	// Look for the entity in the hash
	if(m_streamTree->Get(resourceName, *streamOut) != false)
	{
		// Remove from stream tree
		m_streamTree->Remove(resourceName);

		// Return TRUE
		retVal = TRUE;
	}
	else
	{
		// Make sure the previous attempt to load the stream did not fail
		if(m_streamWorkQueue->WorkFailed(resourceName, FALSE) == FALSE)
		{
			// Queue texture for loading
#ifdef KALYDO
			KalydoQueueStream(CRuString(resourceName), TRUE);
#else
			m_streamWorkQueue->QueueWork(resourceName);
#endif // KALYDO

			// Return TRUE
			retVal = TRUE;
		}
		else
		{
			// Previous load attempt failed
			retVal = FALSE;
		}
	}

	return retVal;
}

BOOL CRuEngine_ResourceManager::AddTexture(IRuBaseTexture *texture)
{
	if(texture)
	{
		TextureDesc texDesc;

		// Look for the texture in the dictionary
		if(m_textureHash->Get(texture->GetTextureName(), texDesc) == FALSE)
		{
			// Increment texture reference count
			texture->AddRef();

			// Setup texture descriptor
			texDesc.m_texture = texture;

			// Add texture to the dictionary
			m_textureHash->Add(texture->GetTextureName(), texDesc);

			return TRUE;
		}
		else
		{
			char outputStr[2048];
			sprintf(outputStr, "ERROR: Attempting to add duplicate texture %s into dictionary.\n", texture->GetTextureName());
			OutputDebugString(outputStr);
		}
	}

	return FALSE;
}

BOOL CRuEngine_ResourceManager::RemoveTexture(IRuBaseTexture *texture)
{
	if(texture)
	{
		TextureDesc texDesc;

		if(m_textureHash->Get(texture->GetTextureName(), texDesc))
		{
			// Remove texture from the dictionary
			m_textureHash->Remove(texture->GetTextureName());

			// Release texture reference
			ruSAFE_RELEASE(texDesc.m_texture);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuEngine_ResourceManager::RemoveTexture(const char *textureName)
{
	if(textureName)
	{
		TextureDesc texDesc;

		if(m_textureHash->Get(textureName, texDesc))
		{
			// Remove texture from the dictionary
			m_textureHash->Remove(textureName);

			// Release texture reference
			ruSAFE_RELEASE(texDesc.m_texture);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuEngine_ResourceManager::TextureExistsInCache(const char *textureName)
{
	RUPROFILE_SAMPLE_ACCUMULATE(m_probe_TextureLoads, 1, 1)

	if(textureName)
	{
		// Make a copy of the texture name
		CRuString canonicalTextureName(textureName);

		INT32 lastSlashIndex = canonicalTextureName.LastIndexOf("\\");
		INT32 lastPeriodIndex = canonicalTextureName.LastIndexOf(".");

		if(lastPeriodIndex > lastSlashIndex)
		{
			// Strip extension from the texture name if necessary
			canonicalTextureName.TruncateAt(lastPeriodIndex);
		}

		if(m_textureHash->ContainsKey(canonicalTextureName))
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuEngine_ResourceManager::EnableTextureGarbageCollection(IRuBaseTexture *texture, BOOL enableGC)
{
	if(texture)
	{
		TextureDesc texDesc;

		if(m_textureHash->Get(texture->GetTextureName(), texDesc))
		{
			// Set new GC option
			texDesc.m_disableGC = !enableGC;

			// Replace the texture descriptor
			m_textureHash->Set(texture->GetTextureName(), texDesc);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuEngine_ResourceManager::PinTexture(IRuBaseTexture *texture, BOOL pinTexture)
{
	if(texture)
	{
		TextureDesc texDesc;

		if(m_textureHash->Get(texture->GetTextureName(), texDesc))
		{
			// Set new pin option
			texDesc.m_pinned = pinTexture;

			// Replace the texture descriptor
			m_textureHash->Set(texture->GetTextureName(), texDesc);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuEngine_ResourceManager::PurgeTexture(IRuBaseTexture *texture)
{
	if(texture)
	{
		TextureDesc texDesc;

		if(m_textureHash->Get(texture->GetTextureName(), texDesc))
		{
			// Set GC age to an arbitrarily large value to force purging
			texDesc.m_gcAge = 1000000;

			// Replace the texture descriptor
			m_textureHash->Set(texture->GetTextureName(), texDesc);

			return TRUE;
		}
	}

	return FALSE;
}

IRuTexture *CRuEngine_ResourceManager::LoadTexture(const char *textureName, RuSurfaceFormat surfaceFormat, UINT32 maxMipLevel, BOOL async)
{
	IRuTexture *texture = NULL;

	// Locate existing texture
	if(textureName && textureName[0])
	{
		// Make a copy of the texture name
		CRuString canonicalTextureName(textureName);

		INT32 lastSlashIndex = canonicalTextureName.LastIndexOf("\\");
		INT32 lastPeriodIndex = canonicalTextureName.LastIndexOf(".");

		if(lastPeriodIndex > lastSlashIndex)
		{
			// Strip extension from the texture name if necessary
			canonicalTextureName.TruncateAt(lastPeriodIndex);
		}

		// Look for the texture in the dictionary
		TextureDesc texDesc;

		if(m_textureHash->Get(canonicalTextureName, texDesc))
		{
			// Reset GC age and update object descriptor
			texDesc.m_gcAge = 0;
			m_textureHash->Set(canonicalTextureName, texDesc);

			if(texDesc.m_texture && texDesc.m_texture->GetType().IsSubClassOf(IRuTexture::Type()))
			{
				// Increment reference count on texture
				texDesc.m_texture->AddRef();

				// Fill out return parameters
				texture = static_cast<IRuTexture *>(texDesc.m_texture);
			}
		}

		if(texture == NULL)
		{
			if(async)
			{
				// Make sure that the load didn't fail before
				if(m_textureWorkQueue->WorkFailed(canonicalTextureName, FALSE) == FALSE)
				{
					// Allocate stub texture
					CRuTexture_Proxy *stubTexture = ruNEW CRuTexture_Proxy();
					stubTexture->Allocate(16, 16, 1, surfaceFormat == ruSURFACEFMT_DEFAULT ? ruSURFACEFMT_DXT1 : surfaceFormat);
					stubTexture->SetTextureName(canonicalTextureName);

					if(m_enableTextureLoading)
					{
						// Invalidate stub texture
						stubTexture->Invalidate();

						// Build a texture work entry
						TextureWorkEntry workEntry = { stubTexture, surfaceFormat, maxMipLevel };
#ifndef KALYDO
						// delay adding this to streaming texture map to the actual queueing work.
						m_streamingTextures.Add(canonicalTextureName, workEntry);
#endif
						stubTexture->AddRef();

						// Queue texture for loading
#ifdef KALYDO
						KalydoQueueTexture(canonicalTextureName, TRUE, &workEntry);
#else
						m_textureWorkQueue->QueueWork(canonicalTextureName);
#endif //KALYDO
					}

					// Insert texture into dictionary
					this->AddTexture(stubTexture);

					// Return stub texture
					texture = stubTexture;
				}
			}
			else
			{
				if(m_enableTextureLoading)
				{
#ifdef KALYDO
					DownloadState ds = GetDownloadState(canonicalTextureName);
					if (ds == PENDING || ds == PENDING_QUEUE || ds == FAILED)
						return texture;
#endif //KALYDO
					// not yet done
					IRuTexture *srcTexture = LoadTexture_Internal(canonicalTextureName);

					if(srcTexture)
					{
						// Create new target texture
						CRuTexture_Proxy *stubTexture = ruNEW CRuTexture_Proxy();
						stubTexture->SetTextureName(canonicalTextureName);

						// Convert texture
						ConvertTexture_Internal(srcTexture, stubTexture, surfaceFormat, maxMipLevel);

						// Release source texture
						ruSAFE_RELEASE(srcTexture);

						// Insert texture into dictionary
						this->AddTexture(stubTexture);

						// Return stub texture
						texture = stubTexture;
					}
#ifdef KALYDO
					else
					{
						// download it, and check when we have it.
						KalydoQueueTexture(canonicalTextureName, FALSE);
					}
#endif //KALYDO
				}
			}
		}
	}

	return texture;
}
/*
BOOL CRuEngine_ResourceManager::CreateRenderTargetPool(INT32 poolSize, INT32 rtSize, RuSurfaceFormat rtFormat)
{
	// Make sure there are no identical pools that already exist
	for(INT32 i = 0; i < m_renderTargetPools.Count(); ++i)
	{
		if(m_renderTargetPools[i]->IsIdenticalPoolType(rtSize, rtFormat))
		{
			return FALSE;
		}
	}

	// Create pool
	CRuRenderTargetPool *rtPool = ruNEW CRuRenderTargetPool(m_engine, poolSize, rtSize, rtFormat);

	if(rtPool)
	{
		m_renderTargetPools.Add(rtPool);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEngine_ResourceManager::ResetRenderTargetPools()
{
	for(INT32 i = 0; i < m_renderTargetPools.Count(); ++i)
	{
		if(m_renderTargetPools[i]->ResetRenderTargetPool() == FALSE)
		{
			return FALSE;
		}
	}

	return TRUE;
}

IRuTexture *CRuEngine_ResourceManager::GetPooledRenderTarget(INT32 rtSize, RuSurfaceFormat rtFormat)
{
	INT32 highestMatchScore = 0;
	INT32 highestMatchPoolIndex = 0;

	// Iterate through all render target pools to find the best match
	for(INT32 i = 0; i < m_renderTargetPools.Count(); ++i)
	{
		INT32 matchScore = m_renderTargetPools[i]->GetMatchScore(rtSize, rtFormat);

		if(matchScore > highestMatchScore)
		{
			highestMatchScore = matchScore;
			highestMatchPoolIndex = i;
		}
	}

	// If there is a match, return a render target from that pool
	if(highestMatchScore > 0)
	{
		return m_renderTargetPools[highestMatchPoolIndex]->GetRenderTarget();
	}

	return NULL;
}

BOOL CRuEngine_ResourceManager::FreePooledRenderTarget(IRuTexture *renderTarget)
{
	for(INT32 i = 0; i < m_renderTargetPools.Count(); ++i)
	{
		if(m_renderTargetPools[i]->FreeRenderTarget(renderTarget))
		{
			return TRUE;
		}
	}

	return FALSE;
}
*/

IRuTexture *CRuEngine_ResourceManager::CreateRenderTarget(INT32 width, INT32 height, RuSurfaceFormat format)
{
	INT32 highestMatchScore = 0;
	INT32 highestMatchPoolIndex = 0;

	// Iterate through all render target pools to find the best match
	for(INT32 i = 0; i < m_renderTargetPools.Count(); ++i)
	{
		INT32 matchScore = m_renderTargetPools[i]->GetMatchScore(width, height, format);

		if(matchScore > highestMatchScore)
		{
			highestMatchScore = matchScore;
			highestMatchPoolIndex = i;
		}
	}

	// If there is a match, return a render target from that pool
	if(highestMatchScore > 0)
	{
		return m_renderTargetPools[highestMatchPoolIndex]->GetRenderTarget();
	}
	else
	{
		// Create pool
		CRuRenderTargetPool *rtPool = ruNEW CRuRenderTargetPool(m_engine, width, height, format);

		if(rtPool)
		{
			m_renderTargetPools.Add(rtPool);

			return rtPool->GetRenderTarget();
		}
	}

	return NULL;
}

BOOL CRuEngine_ResourceManager::ResetRenderTargetPools()
{
	for(INT32 i = 0; i < m_renderTargetPools.Count(); ++i)
	{
		if(m_renderTargetPools[i]->ResetRenderTargetPool() == FALSE)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CRuEngine_ResourceManager::CreateRenderTargetSubAtlas(INT32 subAtlasSize, INT32 pageSize, RuSurfaceFormat rtFormat)
{
	// Create a render target texture for the sub atlas
	IRuTexture *subAtlasTexture = m_engine->CreateRenderTarget(subAtlasSize, subAtlasSize, 1, rtFormat);

	if(subAtlasTexture)
	{
		// Create the sub atlas
		CRuTextureAtlas_Paged *subAtlas = ruNEW CRuTextureAtlas_Paged();
		subAtlas->Initialize(subAtlasTexture, pageSize, pageSize, 0, 0);

		// Bind the sub atlas into the aggregate atlas
		m_rtAtlas->BindSubAtlas(subAtlas);

		// Release local reference to the sub atlas
		ruSAFE_RELEASE(subAtlas);

		// Release local reference to the sub atlas texture
		ruSAFE_RELEASE(subAtlasTexture);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEngine_ResourceManager::RestoreRenderTargetView(IRuTextureView *textureView)
{
	// Check to see if we can bind into the render target atlas
	if(m_rtAtlas->CanBindIntoAtlas(textureView))
	{
		INT32 textureHandle = m_rtAtlas->BindIntoAtlas(textureView, TRUE);

		if(textureHandle != -1)
		{
			// This should never, ever happen!
			if(textureView->GetTexture() == NULL)
			{
				return FALSE;
			}

			// Return TRUE to indicate binding success
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuEngine_ResourceManager::ResetRenderTargetViews()
{
	// Increment eviction marker on the atlas
	INT32 nextEvictionMarker = m_rtAtlas->IncrementEvictionMarker();

	return TRUE;
}

CRuAnimation *CRuEngine_ResourceManager::LoadAnimation(const char *animationName)
{
#if RU_ENGING_RESOURCE_MANAGER_DEBUG_PRINT_LOAD_TIME
    DWORD t0, t1, et;
    t0 = timeGetTime();
#endif

	CRuAnimation *animation = NULL;

	if(animationName)
	{
		if(m_animationHash->Get(animationName, animation))
		{
			animation->AddRef();

			return animation;
		}

#ifdef KALYDO
		switch (m_filesDownloaded[CRuString(animationName)])
		{
		case NO:
			KalydoQueueAnimation(CRuString(animationName));
			if (m_filesDownloaded[CRuString(animationName)] == YES)
				// continue loading
				break;
		case PENDING:
		case PENDING_QUEUE:
		case FAILED:
			return NULL;
		case YES:
			break;
		}
#endif // KALYDO
		IRuStream *srcStream = LoadStream_Internal(animationName);

		if(srcStream)
		{
			animation = LoadRuAnimation(srcStream);

			if(animation)
			{
				animation->AddRef();
				m_animationHash->Add(animationName, animation);
			}

			DestroyStream_Internal(srcStream);
		}
#if RU_ENGING_RESOURCE_MANAGER_DEBUG_PRINT_LOAD_TIME
        t1 = timeGetTime();
        et = t1 - t0;
        char outputStr[256];
        sprintf(outputStr, "Animation %s loaded in %u ms.\n", animationName, et);
        OutputDebugString(outputStr);
#endif
	}

	return animation;
}

CRuAnimation_GR2 *CRuEngine_ResourceManager::LoadAnimation_GR2(const char *animationName)
{
#if RU_ENGING_RESOURCE_MANAGER_DEBUG_PRINT_LOAD_TIME
	DWORD t0, t1, et;
	t0 = timeGetTime();
#endif

	CRuAnimation_GR2 *animation = NULL;

	if(animationName)
	{
		if(m_animationHash_GR2->Get(animationName, animation))
		{
			animation->AddRef();

			return animation;
		}

#ifdef KALYDO
		CRuString aName(animationName);
		switch (m_filesDownloaded[aName])
		{
		case NO:
			KalydoQueueAnimation(aName);
			if (m_filesDownloaded[aName] == YES)
				// continue loading
				break;
		case PENDING:
		case PENDING_QUEUE:
		case FAILED:
			return NULL;
		case YES:
			break;
		}
#endif //KALYDO
		IRuStream *srcStream = LoadStream_Internal(animationName);

		if(srcStream)
		{
			animation = ruNEW CRuAnimation_GR2();

			if(animation->SerializeFrom(srcStream) == FALSE)
			{
				ruSAFE_RELEASE(animation);
			}
			else
			{
				animation->SetObjectName(animationName);
				animation->AddRef();
				m_animationHash_GR2->Add(animationName, animation);
			}

			DestroyStream_Internal(srcStream);
		}
#if RU_ENGING_RESOURCE_MANAGER_DEBUG_PRINT_LOAD_TIME
		t1 = timeGetTime();
		et = t1 - t0;
		char outputStr[256];
		sprintf(outputStr, "Animation %s loaded in %u ms.\n", animationName, et);
		OutputDebugString(outputStr);
#endif
	}

	return animation;
}

BOOL CRuEngine_ResourceManager::AddEntity(const char *entityName, CRuEntity *entity)
{
	ObjectDesc objDesc;

	// Look for the entity in the hash
	if(m_entityHash->Get(entityName, objDesc) == FALSE)
	{
		entity->AddRef();
		objDesc.m_object = entity;

		// Set the object's resource name
		objDesc.m_object->SetResourceName(entityName);

		// Add entity to the entity hash
		m_entityHash->Add(entityName, objDesc);

		return TRUE;
	}

	return FALSE;
}

#ifdef KALYDO

BOOL CRuEngine_ResourceManager::GetEntityDependencies(const char *entityName, std::vector<CRuString>& dependencies)
{
	RUPROFILE_SAMPLE_ACCUMULATE(m_probe_EntityLoads, 1, 1)

	ObjectDesc objDesc;
	BOOL res = FALSE;

	if(entityName == NULL || entityName[0] == NULL)
	{
		return res;
	}

	// Look for the entity in the hash
	if(m_entityHash->Get(entityName, objDesc) == FALSE)
	{
#if RU_ENGING_RESOURCE_MANAGER_DEBUG_PRINT_LOAD_TIME
        DWORD t0, t1, et;
        t0 = GetTickCount();
#endif

		IRuStream *stream = LoadStream_Internal(entityName);

#if RU_ENGING_RESOURCE_MANAGER_DEBUG_PRINT_LOAD_TIME
        t1 = GetTickCount();
        et = t1 - t0;
        char outputStr[256];
        sprintf(outputStr, "Texture stream %s loaded in %d ms.\n", entityName, et);
        OutputDebugString(outputStr);
#endif

		// Attempt to retrieve the named stream from the data store
//		IRuStream *stream = LoadStream_Internal(entityName);

		if(stream)
		{
			// Create object using the object factory
			objDesc.m_object = RuObjectFactory().CreateObject(stream);

			if(objDesc.m_object)
			{
				// Set the object's resource name
				objDesc.m_object->SetResourceName(entityName);

				// Add entity to the entity hash
				m_entityHash->Add(entityName, objDesc);

				// Register event handler for object clone on the template object
				objDesc.m_object->Event_Cloned().RegisterHandler(this->GetGUID(), boost::bind(&CRuEngine_ResourceManager::HandleEvent_ObjectCloned, this, _1));
			}

			// Destroy the data stream
			DestroyStream_Internal(stream);
		}
	}

	// Custom processing based on object type
	if(objDesc.m_object)
	{
		// Reset GC age and update object descriptor
		objDesc.m_gcAge = 0;
		m_entityHash->Set(entityName, objDesc);

		if(objDesc.m_object->GetType().IsSubClassOf(CRuEntity::Type()))
		{
			res = TRUE;

			//// Enumerate all materials
			CRuArrayList<IRuMaterial *> materialList(64);
			RuEntity_EnumerateMaterials(static_cast<CRuEntity *>(objDesc.m_object), materialList);

			// Bind textures for all materials
			for(INT32 i = 0; i < materialList.Count(); ++i)
			{
				// get textures
				materialList[i]->GetTextures(dependencies);

			}
		}
		else if(objDesc.m_object->GetType().IsSubClassOf(CRuACTTemplate::Type()))
		{
			// request all dependencies
			res = static_cast<CRuACTTemplate *>(objDesc.m_object)->GetDependencies(dependencies);

		}
		else if(objDesc.m_object->GetType().IsSubClassOf(CRuPaperdollTemplate::Type()))
		{
			// TODO: download something here??
			res = TRUE;

			//// Construct an object from the paperdoll template and return it
			//returnedEntity = static_cast<CRuPaperdollTemplate *>(objDesc.m_object)->Construct();
		}
	}


	// Could not find object in the hash and could not read from data store, return NULL
	return res;
}

#endif //KALYDO

CRuEntity *CRuEngine_ResourceManager::LoadEntity(const char *entityName)
{
	RUPROFILE_SAMPLE_ACCUMULATE(m_probe_EntityLoads, 1, 1)

	ObjectDesc objDesc;
	CRuEntity *returnedEntity = NULL;

	if(entityName == NULL || entityName[0] == NULL)
	{
		return returnedEntity;
	}

	// Look for the entity in the hash
	if(m_entityHash->Get(entityName, objDesc) == FALSE)
	{
#if RU_ENGING_RESOURCE_MANAGER_DEBUG_PRINT_LOAD_TIME
        DWORD t0, t1, et;
        t0 = GetTickCount();
#endif

		IRuStream *stream = LoadStream_Internal(entityName);

#if RU_ENGING_RESOURCE_MANAGER_DEBUG_PRINT_LOAD_TIME
        t1 = GetTickCount();
        et = t1 - t0;
        char outputStr[256];
        sprintf(outputStr, "Texture stream %s loaded in %d ms.\n", entityName, et);
        OutputDebugString(outputStr);
#endif

		// Attempt to retrieve the named stream from the data store
//		IRuStream *stream = LoadStream_Internal(entityName);

		if(stream)
		{
			// Create object using the object factory
			objDesc.m_object = RuObjectFactory().CreateObject(stream);

			if(objDesc.m_object)
			{
				// Set the object's resource name
				objDesc.m_object->SetResourceName(entityName);

				// Add entity to the entity hash
				m_entityHash->Add(entityName, objDesc);

				// Register event handler for object clone on the template object
				objDesc.m_object->Event_Cloned().RegisterHandler(this->GetGUID(), boost::bind(&CRuEngine_ResourceManager::HandleEvent_ObjectCloned, this, _1));
			}

			// Destroy the data stream
			DestroyStream_Internal(stream);
		}
#ifdef KALYDO
		else
		{
			KalydoQueueObject(CRuString(entityName), false);
		}
#endif //KALYDO
	}

	// Custom processing based on object type
	if(objDesc.m_object)
	{
		// Reset GC age and update object descriptor
		objDesc.m_gcAge = 0;
		m_entityHash->Set(entityName, objDesc);

		if(objDesc.m_object->GetType().IsSubClassOf(CRuEntity::Type()))
		{
			// Enumerate all materials
			CRuArrayList<IRuMaterial *> materialList(64);
			RuEntity_EnumerateMaterials(static_cast<CRuEntity *>(objDesc.m_object), materialList);

			// Bind textures for all materials
			for(INT32 i = 0; i < materialList.Count(); ++i)
			{
				// Bind textures synchronously
				materialList[i]->BindTextures(FALSE);
			}

			// Return a clone of the entity
			returnedEntity = static_cast<CRuEntity *>(objDesc.m_object->Clone());

			// Register event handler for object dispose on the instanced(cloned) object
			returnedEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuEngine_ResourceManager::HandleEvent_ObjectDisposed, this, _1));
		}
		else if(objDesc.m_object->GetType().IsSubClassOf(CRuACTTemplate::Type()))
		{
			// Preload all dependencies for the ACT template synchronously
			static_cast<CRuACTTemplate *>(objDesc.m_object)->PreloadDependencies(FALSE);

			// Construct an object from the ACT template and return it
			returnedEntity = static_cast<CRuACTTemplate *>(objDesc.m_object)->Construct();
		}
		else if(objDesc.m_object->GetType().IsSubClassOf(CRuPaperdollTemplate::Type()))
		{
			// Construct an object from the paperdoll template and return it
			returnedEntity = static_cast<CRuPaperdollTemplate *>(objDesc.m_object)->Construct();
		}
	}

	if(returnedEntity)
	{
		RuEntity_UpdateSkinShaders(returnedEntity);
	}
	else
	{
		RUPROFILE_SAMPLE_ACCUMULATE(m_probe_EntityLoadFails, 1, 1)
	}

	// Could not find object in the hash and could not read from data store, return NULL
	return returnedEntity;
}

BOOL CRuEngine_ResourceManager::LoadEntity(const char *entityName, CRuEntity **entityOut)
{
	RUPROFILE_SAMPLE_ACCUMULATE(m_probe_EntityLoads, 1, 1)

	BOOL retVal = FALSE;
	ObjectDesc objDesc;

	if(entityName == NULL || entityName[0] == NULL)
	{
		return FALSE;
	}

	// Clear output parameters
	*entityOut = NULL;

	// Look for the entity in the hash
	if(m_entityHash->Get(entityName, objDesc) == FALSE)
	{
		// Make sure the previous attempt to load the object did not fail
		if(m_objectWorkQueue->WorkFailed(entityName, FALSE) == FALSE)
		{
			RUPROFILE_SAMPLE_ACCUMULATE(m_probe_EntityLoadTransientFails, 1, 1)

			// Queue Object for loading
#ifdef KALYDO
			KalydoQueueObject(CRuString(entityName), TRUE);
#else
			m_objectWorkQueue->QueueWork(entityName);
#endif //KALYDO

			// Mark return value as true
			retVal = TRUE;
		}
		else
		{
			RUPROFILE_SAMPLE_ACCUMULATE(m_probe_EntityLoadFails, 1, 1)
		}
	}

	// Custom processing based on object type
	if(objDesc.m_object)
	{
		// Reset GC age and update object descriptor
		objDesc.m_gcAge = 0;
		m_entityHash->Set(entityName, objDesc);

		if(objDesc.m_object->GetType().IsSubClassOf(CRuEntity::Type()))
		{
			// Enumerate all materials
			CRuArrayList<IRuMaterial *> materialList(64);
			RuEntity_EnumerateMaterials(static_cast<CRuEntity *>(objDesc.m_object), materialList);

			// Ensure that all materials have had their textures bound
			BOOL allTexturesAreBound = TRUE;

			for(INT32 i = 0; i < materialList.Count(); ++i)
			{
				// Bind textures asynchronously
				allTexturesAreBound = allTexturesAreBound & materialList[i]->BindTextures(TRUE);
			}

			// Wait until all textures are bound
			if(allTexturesAreBound)
			{
				// Return a clone of the entity
				*entityOut = static_cast<CRuEntity *>(objDesc.m_object->Clone());

				// Register event handler for object dispose on the instanced(cloned) object
				(*entityOut)->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuEngine_ResourceManager::HandleEvent_ObjectDisposed, this, _1));
			}
			else
			{
				// We are waiting for all textures to be loaded, return true
				retVal = TRUE;
			}
		}
		else if(objDesc.m_object->GetType().IsSubClassOf(CRuACTTemplate::Type()))
		{
			// Check dependencies and make sure they are loaded
			if(static_cast<CRuACTTemplate *>(objDesc.m_object)->PreloadDependencies(TRUE))
			{
				// Construct an object from the ACT template and return it
				*entityOut = static_cast<CRuACTTemplate *>(objDesc.m_object)->Construct();
			}
			else
			{
				// We are waiting for all dependencies to be loaded, return true
				retVal = TRUE;
			}
		}
		else if(objDesc.m_object->GetType().IsSubClassOf(CRuPaperdollTemplate::Type()))
		{
			// Construct an object from the paperdoll template and return it
			*entityOut = static_cast<CRuPaperdollTemplate *>(objDesc.m_object)->Construct();
		}
	}

	if(*entityOut)
	{
		retVal = TRUE;
	}

	return retVal;
}

BOOL CRuEngine_ResourceManager::AddGenericObject(const char *objectName, IRuObject *obj)
{
	IRuObject *existingObject = NULL;

	// Make sure the object does not already exist in the dictionary
	if(m_genericObjectTree->Get(objectName, existingObject) == FALSE)
	{
		// Add object to the dictionary
		obj->AddRef();
		m_genericObjectTree->Add(objectName, obj);

		return TRUE;
	}

	return FALSE;
}

IRuObject *CRuEngine_ResourceManager::GetGenericObject(const char *objectName)
{
	IRuObject *existingObject = NULL;

	// Look for generic object in the dictionary
	m_genericObjectTree->Get(objectName, existingObject);

	if(existingObject)
	{
		existingObject->AddRef();
	}

	return existingObject;
}

BOOL CRuEngine_ResourceManager::ClearTextureDictionary()
{
	// Clear pinned texture descriptor list
	m_temp_TextureDescList.Clear();

	// Release all textures held in the dictionary
	m_textureHash->ForAllElements(boost::bind(&CRuEngine_ResourceManager::ClearTextureDictionaryCallback, this, _1, _2));

	// Clear all texture entries
	m_textureHash->Clear();

	// Add pinned texture descriptors back into hash
	for(INT32 i = 0; i < m_temp_TextureDescList.Count(); ++i)
	{
		// Add texture to hash and set its GC & pin settings
		AddTexture(m_temp_TextureDescList[i].m_texture);
		EnableTextureGarbageCollection(m_temp_TextureDescList[i].m_texture, m_temp_TextureDescList[i].m_disableGC);
		PinTexture(m_temp_TextureDescList[i].m_texture, m_temp_TextureDescList[i].m_pinned);

		// Release local reference to the texture
		ruSAFE_RELEASE(m_temp_TextureDescList[i].m_texture);
	}

	return TRUE;
}

BOOL CRuEngine_ResourceManager::ClearAnimationDictionary()
{
	// Release all animations held in the dictionary
	m_animationHash->ForAllElements(ClearAnimationDictionaryCallback, this);

	// Clear all animation entries
	m_animationHash->Clear();

	return TRUE;
}

BOOL CRuEngine_ResourceManager::ClearAnimationDictionary_GR2()
{
	// Release all animations held in the dictionary
	m_animationHash_GR2->ForAllElements(ClearAnimationDictionaryCallback_GR2, this);

	// Clear all animation entries
	m_animationHash_GR2->Clear();

	return TRUE;
}

BOOL CRuEngine_ResourceManager::ClearEntityDictionary()
{
	// Release all entities held in the dictionary
	m_entityHash->ForAllElements(boost::bind(&CRuEngine_ResourceManager::ClearEntityDictionaryCallback, this, _1, _2));

	// Clear all entity entries
	m_entityHash->Clear();

	return TRUE;
}

BOOL CRuEngine_ResourceManager::ClearGenericObjectDictionary()
{
	// Release all generic objects held in the dictionary
	m_genericObjectTree->ForAllElements(boost::bind(&CRuEngine_ResourceManager::ClearGenericObjectDictionaryCallback, this, _1, _2));

	// Clear all entries
	m_genericObjectTree->Clear();

	return TRUE;
}

BOOL CRuEngine_ResourceManager::GarbageCollect()
{
	// Back-track...
	m_lastGCTime -= 10000;

	// Forcibly run garbage collect
	GarbageCollect(0);

	return TRUE;
}

BOOL CRuEngine_ResourceManager::IsObjectWorkQueueEmpty()
{
	return (m_objectWorkQueue->WorkCount() == 0) && (m_objectWorkQueue->ResultCount() == 0);
}

BOOL CRuEngine_ResourceManager::IsTextureWorkQueueEmpty()
{
	return (m_textureWorkQueue->WorkCount() == 0) && (m_textureWorkQueue->ResultCount() == 0);
}

BOOL CRuEngine_ResourceManager::BeginJournal(const char *journalName)
{
	m_journalOn = TRUE;
	m_journalName = journalName;
	m_journal.Clear();

	return TRUE;
}

BOOL CRuEngine_ResourceManager::EndJournal()
{
	m_journalOn = FALSE;

	CRuFileStream journalStream;
	journalStream.Create(m_journalName);

	// Write journal
	m_journalStream = &journalStream;
	m_journal.ForAllElements(boost::bind(&CRuEngine_ResourceManager::EndJournalCallback, this, _1, _2));

	journalStream.Close();

	return TRUE;
}

BOOL CRuEngine_ResourceManager::OutputResourceManifest()
{
	char moot[2048];

	OutputDebugString("----------------\n");
	OutputDebugString("Texture Manifest\n");
	OutputDebugString("----------------\n");
	m_temp_Footprint = 0;
	m_textureHash->ForAllElements(boost::bind(&CRuEngine_ResourceManager::TextureManifestCallback, this, _1, _2));

	sprintf(moot, "Total Texture Footprint: %d\n", m_temp_Footprint);
	OutputDebugString(moot);

	OutputDebugString("---------------\n");
	OutputDebugString("Entity Manifest\n");
	OutputDebugString("---------------\n");
	m_temp_Footprint = 0;
	m_temp_Footprint2 = 0;
	CRuKeyHash<PTRVALUE> hash(4096);
	m_temp_EnumHash = &hash;
	m_entityHash->ForAllElements(boost::bind(&CRuEngine_ResourceManager::EntityManifestCallback, this, _1, _2));

	sprintf(moot, "Total Mesh Footprint: %d\n", m_temp_Footprint);
	OutputDebugString(moot);

	sprintf(moot, "Total ColStruct Footprint: %d\n", m_temp_Footprint2);
	OutputDebugString(moot);

	return TRUE;
}

BOOL CRuEngine_ResourceManager::HandleEvent_ObjectCloned(RuEventArgs *eventArgs)
{
	RuObjectEvent_Dispose_Args *disposeArgs = static_cast<RuObjectEvent_Dispose_Args *>(eventArgs);
	const char *entityName = disposeArgs->m_object->GetResourceName();

	if(entityName)
	{
		ObjectDesc objDesc;

		if(m_entityHash->Get(entityName, objDesc))
		{
			// Increment clone count
			objDesc.m_cloneCount = (objDesc.m_cloneCount < 0) ? 1 : objDesc.m_cloneCount + 1;

			// Update object descriptor
			m_entityHash->Set(entityName, objDesc);
		}
	}

	return TRUE;
}

BOOL CRuEngine_ResourceManager::HandleEvent_ObjectDisposed(RuEventArgs *eventArgs)
{
	RuObjectEvent_Cloned_Args *clonedArgs = static_cast<RuObjectEvent_Cloned_Args *>(eventArgs);
	const char *entityName = clonedArgs->m_object->GetResourceName();

	if(entityName)
	{
		ObjectDesc objDesc;

		if(m_entityHash->Get(entityName, objDesc))
		{
			if(objDesc.m_cloneCount >= 0)
			{
				// Decrement clone count
				--objDesc.m_cloneCount;
			}
			else
			{
				// Do nothing, this object is not a cloned object
			}

			// Update object descriptor
			m_entityHash->Set(entityName, objDesc);
		}
	}

	return TRUE;
}

BOOL CRuEngine_ResourceManager::ProcessWorkQueues(PTRVALUE userData)
{
	// Process the preloaded streams queue
	char *dequeuedStreamKey = NULL;
	IRuStream *dequeuedStream = NULL;
	while(m_streamWorkQueue->DequeueResult(&dequeuedStreamKey, dequeuedStream))
	{
		// Add preloaded stream to the stream tree
		m_streamTree->Add(dequeuedStreamKey, dequeuedStream);

		// Delete the stream key
		delete[] dequeuedStreamKey;
	}

	// Process the object work queue
	char *dequeuedObjectKey = NULL;
	IRuObject *dequeuedObject = NULL;

	while(m_objectWorkQueue->DequeueResult(&dequeuedObjectKey, dequeuedObject))
	{
		delete[] dequeuedObjectKey;

		// Add preloaded object to the entity hash
		ObjectDesc objDesc;
		objDesc.m_object = dequeuedObject;
		m_entityHash->Add(dequeuedObject->GetResourceName(), objDesc);

		// Register event handler for object clone on the template object
		objDesc.m_object->Event_Cloned().RegisterHandler(this->GetGUID(), boost::bind(&CRuEngine_ResourceManager::HandleEvent_ObjectCloned, this, _1));
	}

	// Process the texture work queue
	char *dequeuedTextureKey = NULL;
	IRuBaseTexture *dequeuedTexture = NULL;

	while(m_textureWorkQueue->DequeueResult(&dequeuedTextureKey, dequeuedTexture))
	{
		TextureWorkEntry texWorkEntry = m_streamingTextures.Get(dequeuedTextureKey);
		m_streamingTextures.Remove(dequeuedTextureKey);

		CRuTexture_Proxy *stubTexture = texWorkEntry.m_stubTexture;
		RuSurfaceFormat surfaceFormat = texWorkEntry.m_surfaceFormat;
		INT32 maxMipLevel = texWorkEntry.m_maxMipLevel;

		if(dequeuedTexture)
		{
			// Perform texture conversion
			ConvertTexture_Internal(static_cast<IRuTexture *>(dequeuedTexture), stubTexture, surfaceFormat, maxMipLevel);

			// Release local reference to texture
			ruSAFE_RELEASE(dequeuedTexture);
		}
		else
		{
			// Discard the stub texture
			stubTexture->Discard();

			// Mark texture load as failed
			m_textureWorkQueue->MarkWorkFailed(dequeuedTextureKey);
		}

		// Free the texture key
		ruSAFE_DELETE_ARRAY(dequeuedTextureKey);

		// Release reference to stub texture
		ruSAFE_RELEASE(stubTexture);
	}

	return TRUE;
}

BOOL CRuEngine_ResourceManager::GarbageCollect(PTRVALUE userData)
{
/*
	char outputStr[2048];
	sprintf(outputStr, "GC Status [obj %d, tex %d]\n", m_objectWorkQueue->ResultCount(), m_textureWorkQueue->ResultCount());
	OutputDebugString(outputStr);
*/
	DWORD t = timeGetTime();
	m_timeSinceLastGC = t - m_lastGCTime;

	// Garbage collect stale objects
	m_gc_Object.Clear();
	m_entityHash->ForAllElements(boost::bind(&CRuEngine_ResourceManager::GCEntityDictionaryCallback, this, _1, _2));
	for(INT32 i = 0; i < m_gc_Object.Count(); ++i)
	{
/*
		char outputStr[2048];
		sprintf(outputStr, "GC %s\n", m_gc_Object[i]->GetResourceName());
		OutputDebugString(outputStr);
*/
		m_entityHash->Remove(m_gc_Object[i]->GetResourceName());
		ruSAFE_RELEASE(m_gc_Object[i]);
	}

	// Garbage collect stale textures....
	m_gc_Texture.Clear();
	m_textureHash->ForAllElements(boost::bind(&CRuEngine_ResourceManager::GCTextureDictionaryCallback, this, _1, _2));
	for(INT32 i = 0; i < m_gc_Texture.Count(); ++i)
	{
/*
		char outputStr[2048];
		sprintf(outputStr, "GC %s\n", m_gc_Texture[i]->GetTextureName());
		OutputDebugString(outputStr);
*/
		m_textureHash->Remove(m_gc_Texture[i]->GetTextureName());
		ruSAFE_RELEASE(m_gc_Texture[i]);
	}

/*
	char outputStr[2048];
	sprintf(outputStr, "GC [%d textures @ %d] [%d objects @ %d]\n", m_gc_Texture.Count(), et0, m_gc_Object.Count(), et1);
	OutputDebugString(outputStr);
*/
	// Update last GC time
	m_lastGCTime = t;

	return TRUE;
}

BOOL CRuEngine_ResourceManager::TextureDictionaryMemoryFootprintCallback(const void *key, void *data)
{
	IRuBaseTexture *texture = *reinterpret_cast<IRuBaseTexture **>(data);

	if(texture)
	{
		m_temp_Footprint += RuTextureUtility_GetMemoryFootprint(texture, m_temp_EnumHash);
	}

	return TRUE;
}

BOOL CRuEngine_ResourceManager::EntityDictionaryMemoryFootprintCallback(const void *key, void *data)
{
	ObjectDesc &objDesc = *reinterpret_cast<ObjectDesc *>(data);

	if(objDesc.m_object)
	{
		m_temp_Footprint += objDesc.m_object->GetMemoryFootprint(m_temp_EnumHash);
	}

	return TRUE;
}

void CRuEngine_ResourceManager::UnpinAllTextures()
{
	m_textureHash->ForAllElements(boost::bind(&CRuEngine_ResourceManager::UnpinAllTexturesCallback, this, _1, _2));
}

BOOL CRuEngine_ResourceManager::UnpinAllTexturesCallback(const void *key, void *data)
{
	TextureDesc *texDesc = reinterpret_cast<TextureDesc *>(data);
	texDesc->m_pinned = FALSE;
	return TRUE;
}

BOOL CRuEngine_ResourceManager::ClearTextureDictionaryCallback(const void *key, void *data)
{
	TextureDesc *texDesc = reinterpret_cast<TextureDesc *>(data);

	if(texDesc->m_pinned)
	{
		// Store texture descriptor into the list of pinned textures
		m_temp_TextureDescList.Add(*texDesc);
	}
	else
	{
		// Release the texture
		ruSAFE_RELEASE(texDesc->m_texture);
	}

	return TRUE;
}

BOOL CRuEngine_ResourceManager::ClearAnimationDictionaryCallback(const void *key, const void *data, void *userData)
{
	// Release animation
	(*((CRuAnimation **) data))->Release();

	return TRUE;
}

BOOL CRuEngine_ResourceManager::ClearAnimationDictionaryCallback_GR2(const void *key, const void *data, void *userData)
{
	// Release animation
	(*((CRuAnimation_GR2 **) data))->Release();

	return TRUE;
}

BOOL CRuEngine_ResourceManager::ClearEntityDictionaryCallback(const void *key, void *data)
{
	ObjectDesc *objDesc = reinterpret_cast<ObjectDesc *>(data);

	ruSAFE_RELEASE(objDesc->m_object);

	return TRUE;
}

BOOL CRuEngine_ResourceManager::ClearGenericObjectDictionaryCallback(const void *key, void *data)
{
	IRuObject *objDesc = *reinterpret_cast<IRuObject **>(data);

	ruSAFE_RELEASE(objDesc);

	return TRUE;
}

BOOL CRuEngine_ResourceManager::GCTextureDictionaryCallback(const void *key, void *data)
{
	TextureDesc *texDesc = reinterpret_cast<TextureDesc *>(data);

	// If the texture's reference count is one, place it into the GC list
	if(texDesc->m_texture)
	{
		if(texDesc->m_texture->GetRefCount() <= 1 && !texDesc->m_disableGC)
		{
			// Update GC age
			texDesc->m_gcAge += m_timeSinceLastGC;

			if(texDesc->m_gcAge > 20000)
			{
				m_gc_Texture.Add(texDesc->m_texture);
			}
		}
	}

	return TRUE;
}

BOOL CRuEngine_ResourceManager::GCEntityDictionaryCallback(const void *key, void *data)
{
	ObjectDesc *objDesc = reinterpret_cast<ObjectDesc *>(data);

	if(objDesc->m_cloneCount == 0)
	{
		objDesc->m_gcAge += m_timeSinceLastGC;

		if(objDesc->m_gcAge > 20000)
		{
			m_gc_Object.Add(objDesc->m_object);
		}
	}
	else if(objDesc->m_cloneCount < 0)
	{
		if(objDesc->m_object->GetRefCount() == 1)
		{
			objDesc->m_gcAge += m_timeSinceLastGC;

			if(objDesc->m_gcAge > 20000)
			{
				m_gc_Object.Add(objDesc->m_object);
			}
		}
	}

	return TRUE;
}

BOOL CRuEngine_ResourceManager::TextureManifestCallback(const void *key, void *data)
{
	TextureDesc *texDesc = reinterpret_cast<TextureDesc *>(data);

	// If the texture's reference count is one, place it into the GC list
	if(texDesc->m_texture)
	{
		INT32 textureWidth = 0;
		INT32 textureHeight = 0;
		INT32 surfFormat = 0;
		INT32 bytesThisTexture = 0;

		if(texDesc->m_texture->GetType().IsSubClassOf(IRuTexture::Type()))
		{
			IRuTexture *texture = static_cast<IRuTexture *>(texDesc->m_texture);

			textureWidth = texture->GetLevelWidth(0);
			textureHeight = texture->GetLevelHeight(0);
			surfFormat = texture->GetSurfaceFormat();

			for(UINT32 i = 0; i < texture->GetLevelCount(); ++i)
			{
				INT32 bitsPerTexel = RuSurfaceFormat_GetBitsPerTexel(texture->GetSurfaceFormat());
				INT32 bytesThisLevel = texture->GetLevelWidth(i) * texture->GetLevelHeight(i) * bitsPerTexel / 8;
				bytesThisTexture += bytesThisLevel;
			}
		}

		m_temp_Footprint += bytesThisTexture;

		char moo[2048];
		sprintf(moo, "REF: %d, %s, Size: %d, %dx%d\n", texDesc->m_texture->GetRefCount(), texDesc->m_texture->GetTextureName(), bytesThisTexture, textureWidth, textureHeight);
		OutputDebugString(moo);
	}

	return TRUE;
}

BOOL CRuEngine_ResourceManager::EntityManifestCallback(const void *key, void *data)
{
	ObjectDesc *objDesc = reinterpret_cast<ObjectDesc *>(data);

	INT32 footprint = objDesc->m_object->GetMemoryFootprint(m_temp_EnumHash);

	char moo[2048];
	sprintf(moo, "REF: %d, CLONE: %d, SIZE: %d, %s\n", objDesc->m_object->GetRefCount(), objDesc->m_cloneCount, footprint, objDesc->m_object->GetResourceName());
	OutputDebugString(moo);

	if(objDesc->m_object->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		m_temp_Footprint += RuEntity_GetMeshFootprint(static_cast<CRuEntity *>(objDesc->m_object));
		m_temp_Footprint2 += RuEntity_GetExternalColStructFootprint(static_cast<CRuEntity *>(objDesc->m_object));
	}

	return TRUE;
}

IRuStream *CRuEngine_ResourceManager::LoadStream_Internal(const char *streamName, bool boLoadAllData)
{
	m_filePackage_CS->Enter();

	IRuStream *stream = m_filePackage->GetIRuStream(streamName, boLoadAllData);

	if(m_journalOn)
	{
		m_journal.Add(streamName, 0);
	}

	m_filePackage_CS->Leave();

	return stream;
}

BOOL CRuEngine_ResourceManager::DestroyStream_Internal(IRuStream *stream)
{
	m_filePackage_CS->Enter();

	delete stream;

	m_filePackage_CS->Leave();

	return TRUE;
}

RuSurfaceFormat CRuEngine_ResourceManager::GetImageSurfaceFormat(IRuStream *imgStream)
{
	INT32 numImageFormats = 8;
	INT32 imageFormatTable[][2] =	{ 
										{ IMAGE_R5G6B5,		ruSURFACEFMT_R5G6G5 },
										{ IMAGE_A1R5G5B5,	ruSURFACEFMT_A1R5G5B5 },
										{ IMAGE_R8G8B8,		ruSURFACEFMT_R8G8B8 },
										{ IMAGE_A8R8G8B8,	ruSURFACEFMT_A8R8G8B8 },
										{ IMAGE_DXT1,		ruSURFACEFMT_DXT1 },
										{ IMAGE_DXT1a,		ruSURFACEFMT_DXT1 },
										{ IMAGE_DXT3,		ruSURFACEFMT_DXT3 },
										{ IMAGE_DXT5,		ruSURFACEFMT_DXT5 }
									};

	cImgFile *imgFile = static_cast<cImgFile *>(imgStream);

	for(INT32 i = 0; i < numImageFormats; ++i)
	{
		if(imgFile->GetImageFormat() == imageFormatTable[i][0])
		{
			return static_cast<RuSurfaceFormat>(imageFormatTable[i][1]);
		}
	}

	return ruSURFACEFMT_DEFAULT;
}

INT32 CRuEngine_ResourceManager::GetImageMipLevels(IRuStream *imgStream)
{
	cImgFile *imgFile = static_cast<cImgFile *>(imgStream);
	INT32 levels = max(1, imgFile->GetMipmapLevels());

	return levels;
}

IRuTexture *CRuEngine_ResourceManager::CreateTextureToImageSpec(IRuStream *imgStream)
{
	cImgFile *imgFile = static_cast<cImgFile *>(imgStream);
	INT32 levels = GetImageMipLevels(imgStream);

	RuSurfaceFormat surfFmt = GetImageSurfaceFormat(imgStream);

	if(surfFmt != ruSURFACEFMT_DEFAULT)
	{
		CRuTexture_Proxy *texture = ruNEW CRuTexture_Proxy();
		texture->Allocate(imgFile->GetXSize(), imgFile->GetYSize(), levels, surfFmt);
		return texture;
	}

	return NULL;
}

BOOL CRuEngine_ResourceManager::CopyImageToTexture(IRuStream *imgStream, IRuTexture *texture)
{
	cImgFile *imgFile = static_cast<cImgFile *>(imgStream);
	BOOL retVal = FALSE;

	switch(imgFile->GetImageFormat())
	{
		case IMAGE_R5G6B5:
		case IMAGE_A1R5G5B5:
		case IMAGE_R8G8B8:
			break;

		case IMAGE_A8R8G8B8:
			{
				INT32 levels = max(1, imgFile->GetMipmapLevels());
				INT32 xSize = imgFile->GetXSize();
				INT32 ySize = imgFile->GetYSize();

				for(INT32 i = 0; i < levels; ++i)
				{
					void *levelBits;
					INT32 levelPitch;

					if(texture->Lock(i, &levelBits, &levelPitch))
					{
						imgFile->BltToRGB(levelBits, imgFile->GetImageFormat(), 0, 0, xSize, ySize, levelPitch, i);

						texture->Unlock(i);
					}

					xSize /= 2;
					ySize /= 2;
				}

				retVal = TRUE;
			}

			break;

		case IMAGE_DXT1:
		case IMAGE_DXT1a:
		case IMAGE_DXT3:
		case IMAGE_DXT5:
			{
				INT32 levels = max(1, imgFile->GetMipmapLevels());
				INT32 xSize = imgFile->GetXSize();
				INT32 ySize = imgFile->GetYSize();

				for(INT32 i = 0; i < levels; ++i)
				{
					void *levelBits;
					INT32 levelPitch;

					if(texture->Lock(i, &levelBits, &levelPitch))
					{
						// Make sure the surface area is still at least 4x4 (the size of a DXT block) - anything smaller makes no sense
						if(xSize >= 4 && ySize >= 4)
						{
							imgFile->BltToDXT(levelBits, imgFile->GetImageFormat(), 0, 0, xSize, ySize, i);
						}

						texture->Unlock(i);
					}

					xSize /= 2;
					ySize /= 2;
				}

				retVal = TRUE;
			}

			break;
	}

	return retVal;
}

IRuTexture *CRuEngine_ResourceManager::LoadTexture_Internal(const char *textureName)
{
	IRuTexture *texture = NULL;
	IRuStream *stream = NULL;
	CRuString canonicalTextureName(textureName);

	// Iterate through all texture file extensions
	const INT32 numTexExtensions = 4;
	const char *texExtensions[] = { ".dds", ".tga", ".png", ".bmp" };

	for(INT32 i = 0; i < numTexExtensions; ++i)
	{
		canonicalTextureName.Append(texExtensions[i]);

		stream = LoadStream_Internal(static_cast<const char *>(canonicalTextureName));

		canonicalTextureName.TruncateAt(canonicalTextureName.LastIndexOf("."));

		if(stream)
		{
			break;
		}
	}

	if(stream)
	{
		cFileObj *fileObj = (cFileObj *) stream;

		if(fileObj->GetType() == FILE_TYPE_IMAGE)
		{
			cImgFile *imgObj = (cImgFile *) fileObj;

			texture = CreateTextureToImageSpec(imgObj);

			if(texture)
			{
				// Set texture name
				texture->SetTextureName(static_cast<const char *>(canonicalTextureName));

				// Copy image contents to texture
				CopyImageToTexture(imgObj, texture);
			}
		}

		DestroyStream_Internal(stream);
	}
	else
	{
		char outputStr[2048];
		sprintf(outputStr, "Cannot load texture %s.\n", textureName);
//		OutputDebugString(outputStr);
	}

	return texture;
}

BOOL CRuEngine_ResourceManager::ConvertTexture_Internal(IRuTexture *srcTexture, CRuTexture_Proxy *dstTexture, RuSurfaceFormat surfaceFormat, INT32 maxMipLevel)
{
	// Stub processing
	if(maxMipLevel >= static_cast<INT32>(srcTexture->GetLevelCount()))
	{
		char moo[2048];
		sprintf(moo, "Insufficient mip levels for reduction: %s\n", srcTexture->GetTextureName());
		OutputDebugString(moo);
	}

	// Cap max mip level so it is always in the valid range
	maxMipLevel = min(maxMipLevel, static_cast<INT32>(srcTexture->GetLevelCount()) - 1);

	// Set surface format
	if(surfaceFormat == ruSURFACEFMT_DEFAULT)
		surfaceFormat = srcTexture->GetSurfaceFormat();

	// Create new texture with the specified load parameters
	UINT32 convertedMipLevels = srcTexture->GetLevelCount() - maxMipLevel;

	// Re-allocate destination texture and restore it
	dstTexture->Allocate(srcTexture->GetLevelWidth(maxMipLevel), srcTexture->GetLevelHeight(maxMipLevel), convertedMipLevels, surfaceFormat);
	dstTexture->Restore();

//	dstTexture->SetProperty("texture:loaded mip level", maxMipLevel + loadedMipLevel);

	// Convert all mip levels
	for(UINT32 i = 0, j = maxMipLevel; i < convertedMipLevels; ++i, ++j)
	{
		IRuSurface *srcSurface = srcTexture->GetLevelSurface(j);
		IRuSurface *dstSurface = dstTexture->GetLevelSurface(i);

		RuSurfaceUtility_Convert(srcSurface, dstSurface);
	}

	return TRUE;
}

BOOL CRuEngine_ResourceManager::DeviceReset_Handler(RuEventArgs *eventArgs)
{
	// Clear all render target pools
	for(INT32 i = 0; i < m_renderTargetPools.Count(); ++i)
	{
		m_renderTargetPools[i]->ClearRenderTargetPool();
	}

	return TRUE;
}

BOOL CRuEngine_ResourceManager::EndJournalCallback(const char *name, void *data)
{
	char *newline = "\n";

	if(name)
	{
		m_journalStream->Write(name, (DWORD)strlen(name));
		m_journalStream->Write(newline, 1);
	}

	return TRUE;
}

BOOL CRuEngine_ResourceManager::WorkerFunction()
{
	char *streamName = NULL;
	if(m_streamWorkQueue->DequeueWork(&streamName))
	{
		// Load the stream
		IRuStream *stream = LoadStream_Internal(streamName);

		if(stream)
		{
			// Push stream onto preloaded queue
			m_streamWorkQueue->QueueResult(streamName, stream);
			streamName = NULL;
		}
		else
		{
			// Push stream name onto the missing queue
			m_streamWorkQueue->MarkWorkFailed(streamName);
		}
	}

	char *entityName = NULL;
	if(m_objectWorkQueue->DequeueWork(&entityName))
	{
		// Load the entity stream
		IRuStream *stream = LoadStream_Internal(entityName);

		if(stream)
		{
			// Create object using the object factory
			IRuObject *entityObject = RuObjectFactory().CreateObject(stream);

			if(entityObject)
			{
				// Set the object's resource name
				entityObject->SetResourceName(entityName);

				// Push object onto preloaded queue
				m_objectWorkQueue->QueueResult(entityName, entityObject);
				entityName = NULL;
			}
			else
			{
				// Push object name onto the missing queue
				m_objectWorkQueue->MarkWorkFailed(entityName);
			}

			// Destroy the data stream
			DestroyStream_Internal(stream);
		}
		else
		{
			m_objectWorkQueue->MarkWorkFailed(entityName);
		}

		delete[] entityName;
	}

	char *textureName = NULL;
	if(m_textureWorkQueue->DequeueWork(&textureName))
	{
		IRuTexture *texture = LoadTexture_Internal(textureName);

		// Queue resulting texture
		m_textureWorkQueue->QueueResult(textureName, texture);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
