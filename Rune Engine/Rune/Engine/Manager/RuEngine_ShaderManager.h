/*!
	@file RuEngine_ShaderManager.h

	Copyright (c) 2004-2009 All rights reserved
*/

#pragma once

#include "../../Engine/Base/RuEngine_Engine.h"
#include "../../Engine/Base/RuShaderBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Engine shader manager.

	@author John Tang
	@version 2008.11.13
*/
class CRuEngine_ShaderManager : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct ShaderCollection
	{
		CRuString								m_collectionName;				//!< Collection name
		CRuTernaryStringTree<IRuShader *>		m_shaders;						//!< Collection shaders
	};

	IRuEngine*									m_engine;
	CRuString									m_defaultCollection;
	CRuTernaryStringTree<ShaderCollection *>	m_shaderCollections;

public:
												CRuEngine_ShaderManager(IRuEngine *engine);
	virtual										~CRuEngine_ShaderManager();

	void										SetDefaultCollection(const char *collectionName);

	BOOL										AddShader(const char *collectionName, const char *shaderName, IRuShader *shader);
	BOOL										AddShader(const char *collectionName, IRuShader *shader);
	BOOL										RemoveShader(const char *collectionName, const char *shaderName, IRuShader *shader);
	BOOL										RemoveShader(const char *collectionName, IRuShader *shader);
	IRuShader*									GetShader(const char *collectionName, const char *shaderName);
	IRuShader*									GetShader(const char *shaderName);

protected:
	ShaderCollection*							GetShaderCollection(const char *collectionName);

	BOOL										HandleEvent_BeginScene(RuEventArgs *eventArgs);
	BOOL										HandleEvent_DeviceReset(RuEventArgs *eventArgs);
	BOOL										HandleEvent_DeviceRestore(RuEventArgs *eventArgs);
	BOOL										HandleEvent_Shutdown(RuEventArgs *eventArgs);
};

// ************************************************************************************************************************************************************

extern CRuEngine_ShaderManager*					g_ruShaderManager;

// ************************************************************************************************************************************************************

#pragma managed(pop)
