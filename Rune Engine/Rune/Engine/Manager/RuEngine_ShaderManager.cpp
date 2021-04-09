/*!
	@file RuEngine_ShaderManager.cpp

	Copyright (c) 2004-2009 All rights reserved
*/

#include "../../Engine/Manager/RuEngine_ShaderManager.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEngine_ShaderManager, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuEngine_ShaderManager, "CRuEngine_ShaderManager", "IRuObject")
ruCLASSGUID_DEFINE(CRuEngine_ShaderManager, 0x02325217, 0x75164834, 0xBD5E3182, 0x1A01AB22)

// ************************************************************************************************************************************************************

CRuEngine_ShaderManager::CRuEngine_ShaderManager(IRuEngine *engine)
:	m_engine(engine),
	m_defaultCollection("SM20EZ")
{
	// Install event handlers
	m_engine->Event_BeginScene().RegisterHandler(this->GetGUID(), boost::bind(&CRuEngine_ShaderManager::HandleEvent_BeginScene, this, _1));
	m_engine->Event_DeviceReset().RegisterHandler(this->GetGUID(), boost::bind(&CRuEngine_ShaderManager::HandleEvent_DeviceReset, this, _1));
	m_engine->Event_DeviceRestore().RegisterHandler(this->GetGUID(), boost::bind(&CRuEngine_ShaderManager::HandleEvent_DeviceRestore, this, _1));
	m_engine->Event_Shutdown().RegisterHandler(this->GetGUID(), boost::bind(&CRuEngine_ShaderManager::HandleEvent_Shutdown, this, _1));
}

CRuEngine_ShaderManager::~CRuEngine_ShaderManager()
{
	HandleEvent_Shutdown(NULL);
}

void CRuEngine_ShaderManager::SetDefaultCollection(const char *collectionName)
{
	m_defaultCollection = collectionName;
}

BOOL CRuEngine_ShaderManager::AddShader(const char *collectionName, const char *shaderName, IRuShader *shader)
{
	// Get shader collection
	ShaderCollection *collection = GetShaderCollection(collectionName);

	// Add shader to collection
	collection->m_shaders.Add(shaderName, shader);

	// Increment reference count on shader
	ruSAFE_ADDREF(shader);

	return TRUE;
}

BOOL CRuEngine_ShaderManager::AddShader(const char *collectionName, IRuShader *shader)
{
	return AddShader(collectionName, shader->GetShaderName(), shader);
}

BOOL CRuEngine_ShaderManager::RemoveShader(const char *collectionName, const char *shaderName, IRuShader *shader)
{
	// Get shader collection
	ShaderCollection *collection = GetShaderCollection(collectionName);

	if(collection->m_shaders.ContainsKey(shaderName))
	{
		// Remove shader from collection
		collection->m_shaders.Remove(shaderName);

		// Decrement reference count on shader
		ruSAFE_RELEASE(shader);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEngine_ShaderManager::RemoveShader(const char *collectionName, IRuShader *shader)
{
	return RemoveShader(collectionName, shader->GetShaderName(), shader);
}

IRuShader *CRuEngine_ShaderManager::GetShader(const char *collectionName, const char *shaderName)
{
	// Get shader collection
	ShaderCollection *collection = GetShaderCollection(collectionName);

	// Get shader from collection
	IRuShader *shader = NULL;
	collection->m_shaders.Get(shaderName, shader);

	// If no shader was found, then try to get the shader from the default collection
	if(shader == NULL)
	{
		// Get default shader collection
		collection = GetShaderCollection("default");

		// Get shader from collection
		collection->m_shaders.Get(shaderName, shader);
	}
/*
	OutputDebugString(shaderName);
	OutputDebugString("\n");
*/
	return shader;
}

IRuShader *CRuEngine_ShaderManager::GetShader(const char *shaderName)
{
	return GetShader(m_defaultCollection, shaderName);
}

CRuEngine_ShaderManager::ShaderCollection *CRuEngine_ShaderManager::GetShaderCollection(const char *collectionName)
{
	if(m_shaderCollections.ContainsKey(collectionName) == FALSE)
	{
		ShaderCollection *collection = ruNEW ShaderCollection();
		collection->m_collectionName = collectionName;
		m_shaderCollections.Add(collectionName, collection);
	}

	return m_shaderCollections.Get(collectionName);
}

BOOL CRuEngine_ShaderManager::HandleEvent_BeginScene(RuEventArgs *eventArgs)
{
	return TRUE;
}

BOOL CRuEngine_ShaderManager::HandleEvent_DeviceReset(RuEventArgs *eventArgs)
{
	return TRUE;
}

BOOL CRuEngine_ShaderManager::HandleEvent_DeviceRestore(RuEventArgs *eventArgs)
{
	return TRUE;
}

BOOL CRuEngine_ShaderManager::HandleEvent_Shutdown(RuEventArgs *eventArgs)
{
	// Release all shaders

	// Unregister all event handlers
	if(m_engine)
	{
		m_engine->Event_BeginScene().UnregisterHandler(this->GetGUID());
		m_engine->Event_DeviceReset().UnregisterHandler(this->GetGUID());
		m_engine->Event_DeviceRestore().UnregisterHandler(this->GetGUID());
		m_engine->Event_Shutdown().UnregisterHandler(this->GetGUID());
		m_engine = NULL;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuEngine_ShaderManager*			g_ruShaderManager = NULL;

// ************************************************************************************************************************************************************

#pragma managed(pop)
