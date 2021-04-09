#ifndef _RUDATASTORE_PATCHER_
#define _RUDATASTORE_PATCHER_

// Base types
#include "../../Core/DataStore/RuDataStore_Disk.h"
#include "../../Core/Crypto/RuCrypto_MD5.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuDataStore_Patcher;

// ************************************************************************************************************************************************************

class CRuDataStore_Patcher_PatchState
{
protected:
	CRuFileStream*						m_patchStream;
	DWORD								m_patchStreamSize;
	CRuDataStore_Disk*					m_dataStore;
	CRuString							m_relativeDataStorePath;
	BOOL								m_compact;
	BOOL								m_compacting;
	BOOL								m_compactingStarted;
	CRuDataStore_CopyToState*			m_copyToState;
	CRuDataStore_Disk*					m_copyToDataStore;
	BOOL								m_done;

	REAL								m_patchProgress;					//!< 0.0f to 1.0f specifying the level of completion

public:
										CRuDataStore_Patcher_PatchState();
										~CRuDataStore_Patcher_PatchState();

	BOOL								IsDataStoreOpen();
	BOOL								IsPatchDone();
	REAL								GetPatchProgress();

	friend CRuDataStore_Patcher;
};

// ************************************************************************************************************************************************************

class CRuDataStore_Patcher_BuildState
{
protected:
	CRuFileStream*						m_patchStream;						//!< Output patch stream
	CRuDataStore_Disk*					m_srcDataStore;						//!< Source data store to go from
	CRuDataStore_Disk*					m_tgtDataStore;						//!< Target data store to get to

	INT32								m_buildStage;						//!< Build stage: 0 - Start, 1 - Scanning target DS, 2 - Scanning source DS
	INT32								m_tabIndex;							//!< Current table index
	INT32								m_recIndex;							//!< Current record index

	const char*							m_tabName;							//!< Current table name
	INT32								m_tabNameLen;						//!< Current table name length
	INT32								m_tabKeyLen;
	INT32								m_tabNumRecords;
	INT32								m_srcTabHandle;
	INT32								m_tgtTabHandle;

	BYTE*								m_keyBuffer;

	BOOL								m_done;

	REAL								m_patchProgress;					//!< 0.0f to 1.0f specifying the level of completion

public:
										CRuDataStore_Patcher_BuildState();
										~CRuDataStore_Patcher_BuildState();

	BOOL								IsPatchDone();
	REAL								GetPatchProgress();

	friend CRuDataStore_Patcher;
};

// ************************************************************************************************************************************************************

class CRuDataStore_Patcher : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	enum PatchAction
	{
		PATCHACTION_UPDATE				= 0,
		PATCHACTION_DELETE				= 1,
		PATCHACTION_FINISH				= 2,
		PATCHACTION_DELETETABLE			= 3,
		PATCHACTION_CREATETABLE			= 4,

		PATCHACTION_FORCE_DWORD			= 0x7FFFFFFF
	};

public:
										CRuDataStore_Patcher();
	virtual								~CRuDataStore_Patcher();

	BOOL								ApplyPatch(const char *patchPath, INT32 patchOffset, BOOL compact);

	CRuDataStore_Patcher_PatchState*	ApplyPatch_Begin(const char *rootPath, const char *patchPath, INT32 patchOffset, BOOL compact);
	BOOL								ApplyPatch_Step(CRuDataStore_Patcher_PatchState *patchState);
	BOOL								ApplyPatch_End(CRuDataStore_Patcher_PatchState *patchState);

	void								BuildPatch(const char *patchPath, const char *srcDSPath, const char *tgtDSPath, const char *relativeDSPath);

	CRuDataStore_Patcher_BuildState*	BuildPatch_Begin(const char *patchPath, const char *srcDSPath, const char *tgtDSPath, const char *relativeDSPath);
	BOOL								BuildPatch_Step(CRuDataStore_Patcher_BuildState *buildState);
	BOOL								BuildPatch_End(CRuDataStore_Patcher_BuildState *buildState);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
