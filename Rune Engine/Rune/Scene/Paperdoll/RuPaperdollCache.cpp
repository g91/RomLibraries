/*!
	@file RuPaperdollCache.cpp

	Copyright (c) 2004-2007 All rights reserved
*/

#include "../../Scene/Paperdoll/RuPaperdollCache.h"

#include "../../Helper/RuHelper_Entity.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuPaperdollCache, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuPaperdollCache, "CRuPaperdollCache", "IRuObject")
ruCLASSGUID_DEFINE(CRuPaperdollCache, 0xEE975CDD, 0xC6534d9b, 0xAC0A77DB, 0xF887C1A2)

// ************************************************************************************************************************************************************

CRuPaperdollCache::CRuPaperdollCache()
:	m_cachePath(NULL),
	m_elapsedTimeSinceLastUpdate(0.0f)
{
}

CRuPaperdollCache::~CRuPaperdollCache()
{
	delete[] m_cachePath;

	m_paperdollTree.ForAllElements(boost::bind(&CRuPaperdollCache::ClearPaperdollTreeCallback, this, _1, _2));
}

void CRuPaperdollCache::SetCachePath(const char *cachePath)
{
	delete[] m_cachePath;
	m_cachePath = ruNEW char [strlen(cachePath) + 1];
	strcpy(m_cachePath, cachePath);
}

BOOL CRuPaperdollCache::CachePaperdoll(const char *rootName, CRuPaperdoll *paperdoll)
{
	// Convert paperdoll to standard model
	CRuModel *paperdollModel = paperdoll->GetCompositeModel()->ConvertToStandardModel();

	RuModel_SetTextureIsEmbedded(paperdollModel, ruTEXCHANNEL_DIFFUSEMAP, TRUE);
	RuModel_SetTextureIsEmbedded(paperdollModel, ruTEXCHANNEL_PAPERDOLL_MASK_COMPOSITE, TRUE);
	RuModel_SetTextureIsEmbedded(paperdollModel, ruTEXCHANNEL_PAPERDOLL_COLORMAP, TRUE);
	RuModel_SetTextureIsEmbedded(paperdollModel, ruTEXCHANNEL_GLOSSMAP, TRUE);

	// Generate hashing text
	char hashingText[2048];
	CRuPaperdollTemplate *paperdollTemplate = paperdoll->GetPaperdollTemplate();
	INT32 numParts = paperdollTemplate->GetNumParts();

	sprintf(hashingText, "%s", rootName);

	for(INT32 i = 0; i < numParts; ++i)
	{
		const char *componentName = paperdoll->GetComponent(i);

		strcat(hashingText, "#");
		if(componentName)
		{
			strcat(hashingText, componentName);
		}

		char colorCode[1024];
		BOOL layerActivated[4];
		RUCOLOR layerColor[4];
		paperdoll->GetComponentColors(i, layerActivated, layerColor);
		sprintf(colorCode, "%d-%d-%d-%d-%x-%x-%x-%x", layerActivated[0], layerActivated[1], layerActivated[2], layerActivated[3], layerColor[0], layerColor[1], layerColor[2], layerColor[3]);

		strcat(hashingText, "~");
		strcat(hashingText, colorCode);
	}

	// Generate MD5 digest
	char *md5Digest = m_md5Gen.CalculateMD5Digest(hashingText);

	// Generate full cache path
	char fullCachePath[2048];
	sprintf(fullCachePath, "%s\\%s\\%s.ros", g_ruResourceManager->GetRootDirectory(), m_cachePath, md5Digest);

	// Open output stream
	CRuFileStream *outputStream = ruNEW CRuFileStream();
	outputStream->Create(fullCachePath);

	// Write class GUID
	outputStream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write the hashing text
	INT32 hashingTextLen = (INT32)strlen(hashingText) + 1;
	outputStream->Write(&hashingTextLen, sizeof(INT32));
	outputStream->Write(hashingText, sizeof(char) * hashingTextLen);

	// Serialize model
	paperdollModel->SerializeTo(outputStream);

	// Delete output stream
	delete outputStream;

	// Free allocated memory
	delete[] md5Digest;

	// Release converted paperdoll model
	ruSAFE_RELEASE(paperdollModel);

	return TRUE;
}

CRuModel *CRuPaperdollCache::LoadCachedPaperdoll(const char *rootName, CRuPaperdoll *paperdoll)
{
	CRuModel *paperdollModel = NULL;

	// Generate hashing text
	char hashingText[2048];
	CRuPaperdollTemplate *paperdollTemplate = paperdoll->GetPaperdollTemplate();
	INT32 numParts = paperdollTemplate->GetNumParts();

	sprintf(hashingText, "%s", rootName);

	for(INT32 i = 0; i < numParts; ++i)
	{
		const char *componentName = paperdoll->GetComponent(i);

		strcat(hashingText, "#");
		if(componentName)
		{
			strcat(hashingText, componentName);
		}

		char colorCode[1024];
		BOOL layerActivated[4];
		RUCOLOR layerColor[4];
		paperdoll->GetComponentColors(i, layerActivated, layerColor);
		sprintf(colorCode, "%d-%d-%d-%d-%x-%x-%x-%x", layerActivated[0], layerActivated[1], layerActivated[2], layerActivated[3], layerColor[0], layerColor[1], layerColor[2], layerColor[3]);

		strcat(hashingText, "~");
		strcat(hashingText, colorCode);
	}

	// Generate MD5 digest
	char *md5Digest = m_md5Gen.CalculateMD5Digest(hashingText);

	// Generate full cache path
	char fullCachePath[2048];
	sprintf(fullCachePath, "%s\\%s.ros", m_cachePath, md5Digest);

	// Attempt to locate an already-loaded model
	if(m_paperdollTree.ContainsKey(fullCachePath))
	{
		// Get the model descriptor
		ModelDesc *modelDesc = NULL;
		m_paperdollTree.Get(fullCachePath, modelDesc);

		// Reset time since last use
		modelDesc->m_timeSinceLastUse = -m_elapsedTimeSinceLastUpdate;

		// Assign to output paperdoll model
		paperdollModel = modelDesc->m_model;
	}
	else
	{
		// Open input stream
		IRuStream *inputStream = g_ruResourceManager->LoadStream(fullCachePath);
		if(inputStream)
		{
			if(RuReadAndValidateGUID(inputStream, ClassGUID(), this->GetClassGUID()))
			{
				// Read the cached hashing text
				INT32 cachedHashingTextLen = 0;
				inputStream->Read(&cachedHashingTextLen, sizeof(INT32));

				char cachedHashingText[2048];
				inputStream->Read(cachedHashingText, sizeof(char) * cachedHashingTextLen);

				// Validate the cached hashing text against the expected hashing text
				if(stricmp(hashingText, cachedHashingText) == 0)
				{
					// Serialize model
					IRuObject *modelObject = RuObjectFactory().CreateObject(inputStream);
					if(modelObject && modelObject->GetType().IsSubClassOf(CRuModel::Type()))
					{
						paperdollModel = static_cast<CRuModel *>(modelObject);
					}
					else
					{
						ruSAFE_RELEASE(modelObject);
					}
				}
				else
				{
					// MD5 collision
					OutputDebugString("MD5 collision\b");
				}
			}
		}

		// Delete output stream
		delete inputStream;

		// Create the model descriptor
		ModelDesc *modelDesc = ruNEW ModelDesc();
		modelDesc->m_timeSinceLastUse = -m_elapsedTimeSinceLastUpdate;
		modelDesc->m_model = paperdollModel;

		// Insert model descriptor into tree
		m_paperdollTree.Add(fullCachePath, modelDesc);
	}

	// Clone the model if it was successfully loaded
	if(paperdollModel)
	{
		paperdollModel = static_cast<CRuModel *>(paperdollModel->Clone());
	}

	// Free allocated memory
	delete[] md5Digest;

	return paperdollModel;
}

BOOL CRuPaperdollCache::LoadCachedPaperdoll(const char *rootName, CRuPaperdoll *paperdoll, CRuModel **modelOut)
{
	BOOL retVal = FALSE;
	CRuModel *paperdollModel = NULL;

	// Generate hashing text
	char hashingText[2048];
	CRuPaperdollTemplate *paperdollTemplate = paperdoll->GetPaperdollTemplate();
	INT32 numParts = paperdollTemplate->GetNumParts();

	sprintf(hashingText, "%s", rootName);

	for(INT32 i = 0; i < numParts; ++i)
	{
		const char *componentName = paperdoll->GetComponent(i);

		strcat(hashingText, "#");
		if(componentName)
		{
			strcat(hashingText, componentName);
		}

		char colorCode[1024];
		BOOL layerActivated[4];
		RUCOLOR layerColor[4];
		paperdoll->GetComponentColors(i, layerActivated, layerColor);
		sprintf(colorCode, "%d-%d-%d-%d-%x-%x-%x-%x", layerActivated[0], layerActivated[1], layerActivated[2], layerActivated[3], layerColor[0], layerColor[1], layerColor[2], layerColor[3]);

		strcat(hashingText, "~");
		strcat(hashingText, colorCode);
	}

	// Generate MD5 digest
	char *md5Digest = m_md5Gen.CalculateMD5Digest(hashingText);

	// Generate full cache path
	char fullCachePath[2048];
	sprintf(fullCachePath, "%s\\%s.ros", m_cachePath, md5Digest);

	// Attempt to locate an already-loaded model
	if(m_paperdollTree.ContainsKey(fullCachePath))
	{
		// Get the model descriptor
		ModelDesc *modelDesc = NULL;
		m_paperdollTree.Get(fullCachePath, modelDesc);

		// Reset time since last use
		modelDesc->m_timeSinceLastUse = -m_elapsedTimeSinceLastUpdate;

		// Assign to output paperdoll model
		paperdollModel = modelDesc->m_model;

		// return true
		retVal = TRUE;
	}
	else
	{
		// Open input stream
		IRuStream *inputStream = NULL;
		if(g_ruResourceManager->LoadStream(fullCachePath, &inputStream))
		{
			if(inputStream)
			{
				DWORD t0, t1, et;

				t0 = timeGetTime();

				if(RuReadAndValidateGUID(inputStream, ClassGUID(), this->GetClassGUID()))
				{
					// Read the cached hashing text
					INT32 cachedHashingTextLen = 0;
					inputStream->Read(&cachedHashingTextLen, sizeof(INT32));

					char cachedHashingText[2048];
					inputStream->Read(cachedHashingText, sizeof(char) * cachedHashingTextLen);

					// Validate the cached hashing text against the expected hashing text
					if(stricmp(hashingText, cachedHashingText) == 0)
					{
						// Serialize model
						IRuObject *modelObject = RuObjectFactory().CreateObject(inputStream);
						if(modelObject && modelObject->GetType().IsSubClassOf(CRuModel::Type()))
						{
							paperdollModel = static_cast<CRuModel *>(modelObject);
						}
						else
						{
							ruSAFE_RELEASE(modelObject);
						}
					}
					else
					{
						// MD5 collision
						OutputDebugString("MD5 collision\b");
					}
				}

				t1 = timeGetTime();
				et = t1 - t0;
				char outputStr[256];
				sprintf(outputStr, "Cached paperdoll loaded in in %u ms.\n", et);
				OutputDebugString(outputStr);

				// Delete output stream
				delete inputStream;

				// Load finished
				retVal = TRUE;
			}
			else
			{
				// Still waiting for load
				retVal = TRUE;
			}
		}
		else
		{
			// Load stream failed
			retVal = FALSE;
		}
	}

	if(paperdollModel)
	{
		// Create the model descriptor
		ModelDesc *modelDesc = ruNEW ModelDesc();
		modelDesc->m_timeSinceLastUse = -m_elapsedTimeSinceLastUpdate;
		modelDesc->m_model = paperdollModel;

		// Insert model descriptor into tree
		m_paperdollTree.Add(fullCachePath, modelDesc);

		// Clone the paperdoll model
		*modelOut = static_cast<CRuModel *>(paperdollModel->Clone());
	}

	return retVal;
}

void CRuPaperdollCache::Update(REAL elapsedTime)
{
	m_elapsedTimeSinceLastUpdate += elapsedTime;

	// Periodically scan cache to clean it up
	if(m_elapsedTimeSinceLastUpdate > 5.0f)
	{
		// 
		m_paperdollTree.ForAllElements(boost::bind(&CRuPaperdollCache::GCPaperdollTreeCallback, this, _1, _2));

		// Reset elapsed time
		m_elapsedTimeSinceLastUpdate = 0.0f;
	}
}

BOOL CRuPaperdollCache::ClearPaperdollTreeCallback(const void *key, void *data)
{
	// Cast to model object
	ModelDesc *modelDesc = *reinterpret_cast<ModelDesc **>(data);

	// Release the model
	ruSAFE_RELEASE(modelDesc->m_model);

	// Delete the model descriptor
	delete modelDesc;

	return TRUE;
}

BOOL CRuPaperdollCache::GCPaperdollTreeCallback(const void *key, void *data)
{
	// Cast to model object
	ModelDesc *modelDesc = *reinterpret_cast<ModelDesc **>(data);

	// Update time since last use
	modelDesc->m_timeSinceLastUse += m_elapsedTimeSinceLastUpdate;

	// If time since last use exceeds some threshold, remove
	// TODO: Check clone count.... this way it's far more accurate
	if(modelDesc->m_timeSinceLastUse > 30.0f)
	{
		// Release the model
		ruSAFE_RELEASE(modelDesc->m_model);

		// Delete the model descriptor
		delete modelDesc;

		// Remove from tree
		m_paperdollTree.Remove(reinterpret_cast<const char *>(key));
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
