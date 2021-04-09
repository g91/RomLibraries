#pragma once

#include "../../Engine/Manager/RuEngine_ResourceManager_Base.h"

#include "../../Engine/Texture/RuTextureAtlas_Aggregate.h"
#include "../../Engine/Texture/RuTextureAtlas_Paged.h"

#ifdef KALYDO
#include <unordered_map>
#include <queue>
#endif // KALYDO

#define RU_ENGING_RESOURCE_MANAGER_DEBUG_PRINT_LOAD_TIME 0

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class cFilePackage;

class CRuEngine_ResourceManager : public IRuEngine_ResourceManager
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	class CRuRenderTargetPool
	{
	protected:
		INT32								m_rtWidth;							//!< Render target width
		INT32								m_rtHeight;							//!< Render target height
		RuSurfaceFormat						m_rtFormat;							//!< Render target surface format

		CRuStack<IRuTexture *>				m_freeRenderTargets;				//!< Stack of free render targets
		CRuArrayList<IRuTexture *>			m_usedRenderTargets;				//!< List of used render targets

	public:
											CRuRenderTargetPool(IRuEngine *engine, INT32 rtWidth, INT32 rtHeight, RuSurfaceFormat rtFormat);
											~CRuRenderTargetPool();

		BOOL								IsIdenticalPoolType(INT32 rtWidth, INT32 rtHeight, RuSurfaceFormat rtFormat);
		INT32								GetMatchScore(INT32 rtWidth, INT32 rtHeight, RuSurfaceFormat rtFormat);

		IRuTexture*							GetRenderTarget();
		BOOL								FreeRenderTarget(IRuTexture *renderTarget);
		BOOL								ResetRenderTargetPool();
		BOOL								ClearRenderTargetPool();
	};

	struct TextureDesc
	{
		DWORD								m_gcAge;							//!< Garbage collection age marker
		BOOL								m_disableGC;						//!< Disable GC on this texture?
		BOOL								m_pinned;							//!< Is texture pinned? (When pinned, the texture is never cleared from the cache)
		IRuBaseTexture*						m_texture;							//!< Texture

		TextureDesc()
		{
			m_gcAge = 0;
			m_disableGC = FALSE;
			m_pinned = FALSE;
			m_texture = NULL;
		}
	};

	struct ObjectDesc
	{
		INT32								m_cloneCount;
		DWORD								m_gcAge;
		IRuObject*							m_object;

		ObjectDesc()
		{
			m_cloneCount = -1;
			m_gcAge = 0;
			m_object = NULL;
		}
	};

	CRuGUID									m_guid_ProcessWorkQueues;			//!< GUID used for registering work queue processing function
	CRuGUID									m_guid_GarbageCollect;				//!< GUID used for registering garbage collection function

	IRuEngine*								m_engine;							// Engine core

	cFilePackage*							m_filePackage;						// File packager Entity

	CRuTernaryStringTree<IRuStream *>*		m_streamTree;

	CRuTernaryStringTree<TextureDesc>*		m_textureHash;

	CRuStringHash<CRuAnimation *>*			m_animationHash;					// Animation hash
	CRuStringHash<CRuAnimation_GR2 *>*		m_animationHash_GR2;				// Animation hash

	CRuTernaryStringTree<ObjectDesc>*		m_entityHash;

	CRuTernaryStringTree<IRuObject *>*		m_genericObjectTree;				//!< Generic object tree

	CRuArrayList<CRuRenderTargetPool *>		m_renderTargetPools;				//!< List of render target pools

	CRuTextureAtlas_Aggregate*				m_rtAtlas;							//!< Render target atlas

	DWORD									m_lastGCTime;
	DWORD									m_timeSinceLastGC;
	CRuArrayList<IRuBaseTexture *>			m_gc_Texture;
	CRuArrayList<IRuObject *>				m_gc_Object;

	// Settings
	BOOL									m_enableTextureLoading;

	// Critical sections
	CRuThread_CriticalSection*				m_filePackage_CS;

	// Probes
	INT32									m_probe_TextureLoads;
	INT32									m_probe_TextureLoadFails;
	INT32									m_probe_TextureLoadTransientFails;
	INT32									m_probe_EntityLoads;
	INT32									m_probe_EntityLoadFails;
	INT32									m_probe_EntityLoadTransientFails;

	// Work queues
	CRuThread_WorkQueue<char *, IRuStream *>*		m_streamWorkQueue;
	CRuThread_WorkQueue<char *, IRuObject *>*		m_objectWorkQueue;
	CRuThread_WorkQueue<char *, IRuBaseTexture *>*	m_textureWorkQueue;

	struct TextureWorkEntry
	{
		CRuTexture_Proxy*					m_stubTexture;
		RuSurfaceFormat						m_surfaceFormat;
		INT32								m_maxMipLevel;
	};

	CRuTernaryStringTree<TextureWorkEntry>	m_streamingTextures;
	CRuThread_CriticalSection				m_cs_StreamingTextures;



	// Worker thread
	CRuGUID									m_workerFunctionGUID;

	// Temp
	INT32									m_temp_Footprint;
	INT32									m_temp_Footprint2;
	CRuKeyHash<PTRVALUE>*					m_temp_EnumHash;

	CRuArrayList<TextureDesc>				m_temp_TextureDescList;

	// Journal
	BOOL									m_journalOn;
	CRuString								m_journalName;
	CRuTernaryStringTree<INT32>				m_journal;
	IRuStream*								m_journalStream;

#ifdef KALYDO
	// a struct for downloading files.
	struct CDownloadStruct
	{
		CRuEngine_ResourceManager* resourceManager;
		CRuString workName;
		BOOL queueWork;
		int tries;

		// for textures
		CRuEngine_ResourceManager::TextureWorkEntry workEntry;
	};

	struct CDownloadDependenciesStruct
	{
		CRuString workName;
		int dependencies;
		TKCPRequestResultCallback* callback;
		void* userData;
		int tries;
	};
#endif

public:
											CRuEngine_ResourceManager();
	virtual									~CRuEngine_ResourceManager();

	// Memory reporting
	virtual INT32							GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	// Engine association
	BOOL									SetEngine(IRuEngine *engine);
	virtual IRuEngine*						GetEngine();

	// Manager setup
	virtual const char*						GetRootDirectory();
	virtual void							SetRootDirectory(const char *rootDirectory);
	virtual	void							LoadPackageFile(const char *filename, bool keepHandle = true);
	virtual BOOL							FileExists(const char* filename);
	virtual BOOL							SetObjectDequeueRate(REAL dequeueRate);
	virtual BOOL							SetTextureDequeueRate(REAL dequeueRate);
	virtual BOOL							EnableTextureLoading(BOOL enableTextureLoading);

	// Generic stream loading
	virtual BOOL							OpenTempFileStream(const char *resourceName, CRuFileStream &fileStream);
	virtual IRuStream*						LoadStream(const char *resourceName, bool boLoadAllData=true);
	virtual BOOL							LoadStream(const char *resourceName, IRuStream **streamOut);

	// Texture management
	virtual BOOL							AddTexture(IRuBaseTexture *texture);
	virtual BOOL							RemoveTexture(IRuBaseTexture *texture);
	virtual BOOL							RemoveTexture(const char *textureName);
	virtual BOOL							TextureExistsInCache(const char *textureName);
	virtual BOOL							EnableTextureGarbageCollection(IRuBaseTexture *texture, BOOL enableGC);
	virtual BOOL							PinTexture(IRuBaseTexture *texture, BOOL pinTexture);
	virtual BOOL							PurgeTexture(IRuBaseTexture *texture);

	virtual	IRuTexture*						LoadTexture(const char *textureName, RuSurfaceFormat surfaceFormat = ruSURFACEFMT_DEFAULT, UINT32 maxMipLevel = 0, BOOL async = FALSE);

	virtual IRuTexture*						CreateRenderTarget(INT32 width, INT32 height, RuSurfaceFormat format);
	virtual BOOL							ResetRenderTargetPools();

	virtual BOOL							CreateRenderTargetSubAtlas(INT32 subAtlasSize, INT32 pageSize, RuSurfaceFormat rtFormat);
	virtual BOOL							RestoreRenderTargetView(IRuTextureView *textureView);
	virtual BOOL							ResetRenderTargetViews();

	// Animation management
	virtual CRuAnimation*					LoadAnimation(const char *animationName);
	virtual CRuAnimation_GR2*				LoadAnimation_GR2(const char *animationName);

	// Entity management
	// TODO: Pre-load function, garbage collection of stale entities(clone count = 1), etc
	virtual BOOL							AddEntity(const char *entityName, CRuEntity *entity);

#ifdef KALYDO
	virtual TKResult						DownloadEntityAndDependencies(const char* fileName, TKCPRequestResultCallback* callback, void* id);
	virtual BOOL							GetEntityDependencies(const char *entityName, std::vector<CRuString>& dependencies);
	virtual void							BackgroundDownload(const char* fileName);
	virtual unsigned int					GetNumBackgroundDownloadsPending();
	virtual DownloadState					GetDownloadState(const char* fileName);
	virtual void							LoadingStart();
	virtual void							AddPending(INT32 count = 1);
	virtual void							RemovePending(INT32 count = 1);
	virtual float							GetDownloadingStatus();
#endif // KALYDO
	virtual CRuEntity*						LoadEntity(const char *entityName);
	virtual BOOL							LoadEntity(const char *entityName, CRuEntity **entityOut);

	// Generic object dictionary
	virtual BOOL							AddGenericObject(const char *objectName, IRuObject *obj);
	virtual IRuObject*						GetGenericObject(const char *objectName);

	// Dictionary management
	virtual BOOL							ClearTextureDictionary();
	virtual BOOL							ClearAnimationDictionary();
	virtual BOOL							ClearAnimationDictionary_GR2();
	virtual BOOL							ClearEntityDictionary();
	virtual BOOL							ClearGenericObjectDictionary();

	virtual BOOL							GarbageCollect();

	// Status report
	virtual BOOL							IsObjectWorkQueueEmpty();
	virtual BOOL							IsTextureWorkQueueEmpty();

	// Journaling
	virtual BOOL							BeginJournal(const char *journalName);
	virtual BOOL							EndJournal();

	// Debugging
	virtual BOOL							OutputResourceManifest();

protected:
	BOOL									HandleEvent_ObjectCloned(RuEventArgs *eventArgs);
	BOOL									HandleEvent_ObjectDisposed(RuEventArgs *eventArgs);

	// Garbage collection and other queue processing
	BOOL									ProcessWorkQueues(PTRVALUE userData);
	BOOL									GarbageCollect(PTRVALUE userData);

	BOOL									TextureDictionaryMemoryFootprintCallback(const void *key, void *data);
	BOOL									EntityDictionaryMemoryFootprintCallback(const void *key, void *data);

	void									UnpinAllTextures();
	BOOL									UnpinAllTexturesCallback(const void *key, void *data);

	BOOL									ClearTextureDictionaryCallback(const void *key, void *data);
	static BOOL								ClearAnimationDictionaryCallback(const void *key, const void *data, void *userData);
	static BOOL								ClearAnimationDictionaryCallback_GR2(const void *key, const void *data, void *userData);
	BOOL									ClearEntityDictionaryCallback(const void *key, void *data);
	BOOL									ClearGenericObjectDictionaryCallback(const void *key, void *data);

	BOOL									GCTextureDictionaryCallback(const void *key, void *data);
	BOOL									GCEntityDictionaryCallback(const void *key, void *data);

	BOOL									TextureManifestCallback(const void *key, void *data);
	BOOL									EntityManifestCallback(const void *key, void *data);

	// Common access to the file manager
	IRuStream*								LoadStream_Internal(const char *streamName, bool boLoadAllData=true);
	BOOL									DestroyStream_Internal(IRuStream *stream);

	// Other helpers
	RuSurfaceFormat							GetImageSurfaceFormat(IRuStream *imgStream);
	INT32									GetImageMipLevels(IRuStream *imgStream);
	IRuTexture*								CreateTextureToImageSpec(IRuStream *imgStream);
	BOOL									CopyImageToTexture(IRuStream *imgStream, IRuTexture *texture);

	IRuTexture*								LoadTexture_Internal(const char *textureName);
	BOOL									ConvertTexture_Internal(IRuTexture *srcTexture, CRuTexture_Proxy *dstTexture, RuSurfaceFormat surfaceFormat, INT32 maxMipLevel);

	// Event Handlers
	BOOL									DeviceReset_Handler(RuEventArgs *eventArgs);

	BOOL									EndJournalCallback(const char *name, void *data);

	BOOL									WorkerFunction();

#ifdef KALYDO
	// Kalydo queueing for downloadable content
	void									KalydoQueueStream(CRuString& canonicalTextureName, BOOL queueWork);
	void									KalydoQueueTexture(CRuString& canonicalTextureName, BOOL queueWork, TextureWorkEntry* workEntry = NULL);
	void									KalydoQueueObject(CRuString& entityName, BOOL queueWork);
	void									KalydoQueueAnimation(CRuString& animationName);

	static void								streamDownloadCallback(const char* fileName, TKResult result, void* id);
	static void								textureDownloadCallback(const char* fileName, TKResult result, void* id);
	static void								objectDownloadCallback(const char* fileName, TKResult result, void* id);
	static void								animationDownloadCallback(const char* fileName, TKResult result, void* id);

	static void								backgroundDownloadCallback(const char* fileName, TKResult result, void* id);
	static void								dependenciesDownloadCallback(const char* fileName, TKResult result, void* id);

	stdext::hash_map< CRuString, DownloadState >	m_filesDownloaded;
	//std::map< CRuString, DownloadState > 	m_animationsDownloaded;
	//std::map< CRuString, DownloadState > 	m_texturesDownloaded;
	//std::map< CRuString, DownloadState > 	m_modelsDownloaded;

	CRITICAL_SECTION						m_backgroundDownloadCritSec;
	std::queue< CRuString >					m_backgroundDownloadQueue;

	INT32									m_filesPendingAtLoadStart;
	INT32									m_filesPending;
	float									m_downloadingStatus;
#endif // KALYDO
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
