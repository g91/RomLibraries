#include "Rune.h"

#pragma managed(push, off)

BOOL							g_ruObjectFactoryInitialized = FALSE;

CRuRendererPlugin_AquaFX*		g_ruRendererPlugin_AquaFX = NULL;
CRuRendererPlugin_BlendFX*		g_ruRendererPlugin_BlendFX = NULL;
CRuRendererPlugin_GlowFX*		g_ruRendererPlugin_GlowFX = NULL;
CRuRendererPlugin_RTLightMap*	g_ruRendererPlugin_RTLightMap = NULL;
CRuRendererPlugin_VizCon*		g_ruRendererPlugin_VizCon = NULL;

char							g_ruGlobalMessage[2048] = "";


void RuInitializeObjectFactory()
{
	// Do not initialize the object factory a second time
	if(g_ruObjectFactoryInitialized)
		return;

	// Initialize global function schedule
	g_ruFunctionSchedule = ruNEW CRuUtility_FunctionScheduler();

	// Initialize object factory
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Material());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_MorphTarget());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Mesh());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_DynamicMesh());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_MorphingMesh());

	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Texture_Proxy());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_VolumeTexture_Proxy());

	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Entity());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Entity_Camera());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Entity_Light());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Entity_Container());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Entity_Decal());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Entity_PolyLine());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Frame());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_LinkFrame());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_FrameHierarchy());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Entity_Renderable());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Model());

	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Hierarchy_GR2());

	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_CompositeModelTemplate());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_PaperdollTemplate());

	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_ACTEntity());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_ACTTemplate());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_ACTTemplateNode());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_ACTTemplateModifier());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_ACTMotion());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_ACTMotionNode());

	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_Decal());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_Entity());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_EventTrigger());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_Frame());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_GhostTrail());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_GlobalAlpha());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_GlobalColor());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_Hierarchy());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_MaterialTexture());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_Morph());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_Motion());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_ParticleSystem());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_Particle_Emitter());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_Particle_Emitter_Enviro());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_Particle_Emitter_Line());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_Particle_Visualizer_Entity());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_Particle_Visualizer_Sprite());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_PolyLine());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_UVFX());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_VisibilityChain());
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Controller_WaterFX());

	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Container_Portal());

	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Entity_AntiPortal());

	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_MeshCollision_Simple);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_MeshCollision_Quadtree);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_MeshCollision_kdTree);

	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_SplatIDMap);

	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_OptimalTerrain_Chunk);

	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_PolySoup_Quadtree);

	// Collision modules
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Collision_kdTree);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Collision_kdTree_16);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Collision_Simple);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_CollisionHierarchy_Simple);

	// PathEngine modules
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_PathGraph_AStar);

	// Particle system modules
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Entity_Particle_Emitter);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Entity_Particle_Emitter_Enviro);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Entity_Particle_Emitter_Line);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Entity_Particle_Visualizer_Entity);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Entity_Particle_Visualizer_Sprite);

	// Noise generator modules
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Noise_Perlin);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Noise_Texture);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Noise_Value);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Noise_Worley);

	// Fractal modules
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Fractal_Composite);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Fractal_fBm);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Fractal_HeteroTerrain);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Fractal_HybridMultifractal);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Fractal_Multifractal);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Fractal_RidgedMultifractal);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Fractal_Shapes);

	// Fractal post-process modules
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Fractal_PostFX_ABS);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Fractal_PostFX_Cutoff);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Fractal_PostFX_Gain);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Fractal_PostFX_Remap);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Fractal_PostFX_Square);

	// World system modules
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_RuWorld_CellLink);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_RuWorld_ObjectCell);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_RuWorld_EntityDescriptor);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_RuWorld_Trigger);

	// Other modules
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Entity_TileMap);

	// Renderer nodes
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_RendererNode_RenderContext);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_RendererNode_SetScissor);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_RendererNode_TextureCopy);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_RendererNode_UI);
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_RendererNode_ViewportClear);

	// Deprecated
	RuObjectFactory().AttachModule(ruNEW CRuObjectModule_Entity_Particle_Sprite);

}

BOOL RuInitialize_Core()
{
	// Initialize profiler
	RUPROFILE_INITIALIZE

	// Initialize object factory
	RuInitializeObjectFactory();

	// Initialize global thread worker
	if(g_ruGlobalWorkerThread == NULL)
	{
		g_ruGlobalWorkerThread = ruNEW CRuThread_Worker();
	}

	// Initialize engine settings object
	if(g_ruEngineSettings == NULL)
	{
		g_ruEngineSettings = ruNEW CRuEngine_Settings();
	}

	// Initialize global profiler
	if(g_ruProfiler == NULL)
	{
		g_ruProfiler = ruNEW CRuProfiler();
	}

	// Initialize default probes
	g_ruProbe_TransformUpdateCount = g_ruProfiler->CreateProbe("Transform UpdCnt");
	g_ruProbe_TransformUpdateTime = g_ruProfiler->CreateProbe("Transform Updates");
	g_ruProbe_BoundingUpdateCount = g_ruProfiler->CreateProbe("Bounding UpdCnt");
	g_ruProbe_BoundingUpdateTime = g_ruProfiler->CreateProbe("Bounding Updates");

	// Test setting values
	g_ruEngineSettings->SetSetting("gfx:water reflection", FALSE);
	g_ruEngineSettings->SetSetting("gfx:water refraction", FALSE);

	g_ruEngineSettings->SetSetting("ui:scale", 1.0f);

	g_ruEngineSettings->SetSetting("world:view distance", 2000.0f);

	return TRUE;
}

BOOL RuInitialize_Scene(IRuSwapChain *primarySwapChain)
{
	// Initialize renderer
	g_ruRenderer = ruNEW CRuRenderer();

	// Initialize default render context
	g_ruDefaultRenderContext = ruNEW CRuRendererNode_RenderContext();
	g_ruDefaultRenderContext->SetObjectName("Default Render Context");
	g_ruDefaultRenderContext->PersistAcrossFrames(TRUE);
	g_ruDefaultRenderContext->SetPriority(0);
	g_ruDefaultRenderContext->SetRenderTarget(primarySwapChain);
	g_ruDefaultRenderContext->SetDefaultClearParameters(RUCLEAR_TARGET | RUCLEAR_ZBUFFER | RUCLEAR_STENCIL, 0xFF333333, 1.0f, 0);

	// Create default UI render node
	g_ruUIRenderer = ruNEW CRuRendererNode_UI();
	g_ruUIRenderer->SetObjectName("UI Renderer");
	g_ruUIRenderer->PersistAcrossFrames(TRUE);
	g_ruUIRenderer->SetPriority(-1);
	g_ruUIRenderer->SetRenderTarget(primarySwapChain);

	// Make the UI renderer node dependent on the default render context
	g_ruUIRenderer->AddDependency(g_ruDefaultRenderContext, FALSE);

	// Attach renderer nodes
	g_ruRenderer->AttachRendererNode(g_ruDefaultRenderContext);
	g_ruRenderer->AttachRendererNode(g_ruUIRenderer);

	// Initialize AlphaZ plugin
	g_ruRendererPlugin_AlphaZ = ruNEW CRuRendererPlugin_AlphaZ();
	g_ruRendererPlugin_AlphaZ->RegisterPlugin(g_ruDefaultRenderContext);

	// Initialize AquaFX plugin
	g_ruRendererPlugin_AquaFX = ruNEW CRuRendererPlugin_AquaFX();
	g_ruRendererPlugin_AquaFX->RegisterPlugin(g_ruDefaultRenderContext);

	// Initialize BlendFX plugin
	g_ruRendererPlugin_BlendFX = ruNEW CRuRendererPlugin_BlendFX();
	g_ruRendererPlugin_BlendFX->RegisterPlugin(g_ruDefaultRenderContext);

	// Initialize GlowFX plugin
	g_ruRendererPlugin_GlowFX = ruNEW CRuRendererPlugin_GlowFX();
	g_ruRendererPlugin_GlowFX->RegisterPlugin(g_ruDefaultRenderContext);

	// Initialize RTLightMap plugin
	g_ruRendererPlugin_RTLightMap = ruNEW CRuRendererPlugin_RTLightMap();
	g_ruRendererPlugin_RTLightMap->RegisterPlugin(g_ruDefaultRenderContext);

	// Initialize VizCon plugin
	g_ruRendererPlugin_VizCon = ruNEW CRuRendererPlugin_VizCon();
	g_ruRendererPlugin_VizCon->RegisterPlugin(g_ruDefaultRenderContext);

	return TRUE;
}

IRuSwapChain* RuCreateSwapChain(UINT32 width, UINT32 height, HWND focusWindow)
{
	if(g_ruEngine)
	{
		return g_ruEngine->CreateSwapChain(width, height, focusWindow);
	}

	return NULL;
}

BOOL RuSetResolution(IRuSwapChain *swapChain, UINT32 width, UINT32 height, BOOL fullscreen)
{
	if(g_ruEngine && swapChain)
	{
		if(g_ruEngine->SetResolution(swapChain, width, height, fullscreen))
		{
			g_ruEngine->ClearStateCache();

			return TRUE;
		}
	}

	return FALSE;
}

BOOL RuShutdown()
{
	if(g_ruProfiler)
	{
		delete g_ruProfiler;
		g_ruProfiler = NULL;
	}

	ruSAFE_RELEASE(g_ruFunctionSchedule);

	ruSAFE_RELEASE(g_ruRendererPlugin_AlphaZ);
	ruSAFE_RELEASE(g_ruRendererPlugin_AquaFX);
	ruSAFE_RELEASE(g_ruRendererPlugin_BlendFX);
	ruSAFE_RELEASE(g_ruRendererPlugin_GlowFX);
	ruSAFE_RELEASE(g_ruRendererPlugin_RTLightMap);
	ruSAFE_RELEASE(g_ruRendererPlugin_VizCon);

	ruSAFE_RELEASE(g_ruUIRenderer);
	ruSAFE_RELEASE(g_ruDefaultRenderContext);
	ruSAFE_RELEASE(g_ruRenderer);

	ruSAFE_RELEASE(g_ruResourceManager);
	ruSAFE_RELEASE(g_ruShaderManager);
	ruSAFE_RELEASE(g_ruInstanceManager);

	ruSAFE_RELEASE(g_ruEngineSettings);

	RuObjectFactory().ClearModuleDictionary();

	delete g_ruGlobalWorkerThread;

	delete g_ruEngine;

	return TRUE;
}

#pragma managed(pop)
