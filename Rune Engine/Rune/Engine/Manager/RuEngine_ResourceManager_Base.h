#pragma once

// Rune system core
#include "../../Engine/Base/RuEngine_Engine.h"
#include "../../Engine/Base/RuCore.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class IRuShader;
class CRuAnimation;
class CRuAnimation_GR2;
class CRuEntity;

// ************************************************************************************************************************************************************

#ifdef KALYDO
// for downloading DLC
#include "CompactIntegration/Include/KRFReadLib.h"
#include "../../core/datastore/RuDataStore_Disk.h"
#endif // KALYDO

class IRuEngine_ResourceManager : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual								~IRuEngine_ResourceManager() { }

	// Manager setup
	virtual const char*					GetRootDirectory() = 0;
	virtual void						SetRootDirectory(const char *rootDirectory) = 0;
	virtual	void						LoadPackageFile(const char *filename, bool keepHandle = true) = 0;
	virtual BOOL						FileExists(const char* filename) = 0;
	virtual BOOL						SetObjectDequeueRate(REAL dequeueRate) = 0;
	virtual BOOL						SetTextureDequeueRate(REAL dequeueRate) = 0;

	// Generic stream loading
	virtual BOOL						OpenTempFileStream(const char *resourceName, CRuFileStream &fileStream) = 0;
	virtual IRuStream*					LoadStream(const char *resourceName, bool boLoadAllData=true) = 0;
	virtual BOOL						LoadStream(const char *resourceName, IRuStream **streamOut) = 0;

	// Texture management
	virtual BOOL						AddTexture(IRuBaseTexture *texture) = 0;			// Adds texture to the manager's dictionary
	virtual BOOL						RemoveTexture(IRuBaseTexture *texture) = 0;			// Removes texture from the manager's dictionary
	virtual BOOL						RemoveTexture(const char *textureName) = 0;			// Removes texture from the manager's dictionary
	virtual BOOL						TextureExistsInCache(const char *textureName) = 0;
	virtual BOOL						EnableTextureGarbageCollection(IRuBaseTexture *texture, BOOL enableGC) = 0;
	virtual BOOL						PinTexture(IRuBaseTexture *texture, BOOL pinTexture) = 0;
	virtual BOOL						PurgeTexture(IRuBaseTexture *texture) = 0;			// Sets texture for purging at the first available opportunity

	virtual	IRuTexture*					LoadTexture(const char *textureName, RuSurfaceFormat surfaceFormat = ruSURFACEFMT_DEFAULT, UINT32 maxMipLevel = 0, BOOL async = FALSE) = 0;

	// Render target pool management
	virtual IRuTexture*					CreateRenderTarget(INT32 width, INT32 height, RuSurfaceFormat format) = 0;
	virtual BOOL						ResetRenderTargetPools() = 0;

	// More RT pool management
	virtual BOOL						CreateRenderTargetSubAtlas(INT32 subAtlasSize, INT32 pageSize, RuSurfaceFormat rtFormat) = 0;
	virtual BOOL						RestoreRenderTargetView(IRuTextureView *textureView) = 0;
	virtual BOOL						ResetRenderTargetViews() = 0;

	// Animation management
	virtual CRuAnimation*				LoadAnimation(const char *animationName) = 0;
	virtual CRuAnimation_GR2*			LoadAnimation_GR2(const char *animationName) = 0;

	// Entity management
	virtual BOOL						AddEntity(const char *entityName, CRuEntity *entity) = 0;

#ifdef KALYDO
	virtual TKResult					DownloadEntityAndDependencies(const char* fileName, TKCPRequestResultCallback* callback, void* id) = 0;
	virtual BOOL						GetEntityDependencies(const char *entityName, std::vector<CRuString>& dependencies) = 0;
	virtual void						BackgroundDownload(const char* fileName) = 0;
	virtual unsigned int				GetNumBackgroundDownloadsPending() = 0;
	virtual DownloadState				GetDownloadState(const char* fileName) = 0;
	virtual void						LoadingStart() = 0;
	virtual void						AddPending(INT32 count = 1) = 0;
	virtual void						RemovePending(INT32 count = 1) = 0;
	virtual float						GetDownloadingStatus() = 0;
#endif

	virtual CRuEntity*					LoadEntity(const char *entityName) = 0;
	virtual BOOL						LoadEntity(const char *entityName, CRuEntity **entityOut) = 0;

	// Generic object dictionary
	virtual BOOL						AddGenericObject(const char *objectName, IRuObject *obj) = 0;
	virtual IRuObject*					GetGenericObject(const char *objectName) = 0;

	// Dictionary management
	virtual BOOL						ClearTextureDictionary() = 0;						// Clears the texture dictionary
	virtual BOOL						ClearAnimationDictionary() = 0;						// Clears the animation dictionary
	virtual BOOL						ClearEntityDictionary() = 0;						// Clears the Entity dictionary
	virtual BOOL						ClearGenericObjectDictionary() = 0;

	virtual BOOL						GarbageCollect() = 0;

	// Status report
	virtual BOOL						IsObjectWorkQueueEmpty() = 0;
	virtual BOOL						IsTextureWorkQueueEmpty() = 0;

	// Journaling
	virtual BOOL						BeginJournal(const char *journalName) = 0;
	virtual BOOL						EndJournal() = 0;

	// Debugging
	virtual BOOL						OutputResourceManifest() = 0;
};

// ************************************************************************************************************************************************************

extern RUAPI IRuEngine_ResourceManager*		g_ruResourceManager;

// ************************************************************************************************************************************************************

#pragma managed(pop)
