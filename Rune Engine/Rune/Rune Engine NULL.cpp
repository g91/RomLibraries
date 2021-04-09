#include "Rune Engine NULL.h"

#pragma unmanaged

BOOL RuInitialize_NULL()
{
	CRuEngine_NULL *engine = NULL;

	// Initialize and startup engine
	engine = ruNEW CRuEngine_NULL();

	// Initialize instance manager
	g_ruInstanceManager = ruNEW CRuEngine_InstanceManager();
	((CRuEngine_InstanceManager *) g_ruInstanceManager)->SetEngine(engine);

	// Initialize resource manager
	g_ruResourceManager = ruNEW CRuEngine_ResourceManager();
	((CRuEngine_ResourceManager *) g_ruResourceManager)->EnableTextureLoading(FALSE);
	((CRuEngine_ResourceManager *) g_ruResourceManager)->SetEngine(engine);

	// Initialize shader manager
	g_ruShaderManager = ruNEW CRuEngine_ShaderManager(engine);

	return TRUE;
}

#pragma managed
