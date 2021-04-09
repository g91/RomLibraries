#include "Rune Engine D3D.h"

// Shaders - Universal
#include "Direct3D/Shader/SMUNI/RuShader_SMUNI_Base.h"
#include "Direct3D/Shader/SMUNI/RuShader_SMUNI_2DFX.h"
#include "Direct3D/Shader/SMUNI/RuShader_SMUNI_Editor.h"
#include "Direct3D/Shader/SMUNI/RuShader_SMUNI_Editor_GraphVis.h"
#include "Direct3D/Shader/SMUNI/RuShader_SMUNI_FX.h"
#include "Direct3D/Shader/SMUNI/RuShader_SMUNI_FX2D_UIFX.h"
#include "Direct3D/Shader/SMUNI/RuShader_SMUNI_Particle.h"
#include "Direct3D/Shader/SMUNI/RuShader_SMUNI_Sky.h"
#include "Direct3D/Shader/SMUNI/RuShader_SMUNI_Widget.h"

// Shaders - Shader Model 2.0 EZ
#include "Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Base.h"
#include "Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Doodad.h"
#include "Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Paperdoll.h"
#include "Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Skin_Standard.h"
#include "Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Standard.h"
#include "Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Terrain.h"
#include "Direct3D/Shader/SM20EZ/RuShader_SM20EZ_Water.h"

// Mesh instantiators
#include "Engine/Instantiator/RuMeshInstantiators_Common.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

#define RuShaderSys_CreateDefaultShader(shaderClass)								\
	{																				\
		shaderClass *shader = ruNEW shaderClass(engine);							\
		g_ruShaderManager->AddShader("default", shader);							\
		shader->Release();															\
	}

// ************************************************************************************************************************************************************

#define RuShaderSys_CreateShader(collectionName, shaderClass)						\
	{																				\
		shaderClass *shader = ruNEW shaderClass(engine);							\
		g_ruShaderManager->AddShader(collectionName, shader);						\
		shader->Release();															\
	}

// ************************************************************************************************************************************************************

#define RuShaderSys_CreateShaderAlt(collectionName, shaderName, shaderClass)		\
	{																				\
		shaderClass *shader = ruNEW shaderClass(engine);							\
		g_ruShaderManager->AddShader(collectionName, shaderName, shader);			\
		shader->Release();															\
	}

// ************************************************************************************************************************************************************

#define RuVertDecl_RegisterInstantiator(instantiatorName)							\
	{																				\
		instantiatorName *instantiator = ruNEW instantiatorName(engine);			\
		g_ruInstanceManager->RegisterMeshInstantiator(instantiator);				\
		instantiator->Release();													\
	}

// ************************************************************************************************************************************************************

IRuSwapChain* RuInitialize_D3D(HWND primaryWindow, HWND focusWindow, UINT32 resourceArenaSize)
{
	CRuEngineD3D *engine = NULL;
	IRuSwapChain *swapChain = NULL;

	// Initialize and startup engine
	engine = ruNEW CRuEngineD3D();
	engine->Startup(primaryWindow);

	// Enumerate display modes
	if(engine->EnumerateAdapters() && engine->SelectInitialDisplaySettings(FALSE))
	{
		// Create a default swap chain
		swapChain = engine->CreateSwapChain(800, 600, focusWindow);

		if(swapChain)
		{
			// Set engine into global
			g_ruEngine = engine;
			g_ruEngineD3D = engine;

			// Initialize instance manager
			g_ruInstanceManager = ruNEW CRuEngine_InstanceManager();
			((CRuEngine_InstanceManager *) g_ruInstanceManager)->SetEngine(engine);

			// Initialize resource manager
			g_ruResourceManager = ruNEW CRuEngine_ResourceManager();
			((CRuEngine_ResourceManager *) g_ruResourceManager)->SetEngine(engine);

			// Initialize shader manager
			g_ruShaderManager = ruNEW CRuEngine_ShaderManager(engine);

			// Initialize 2D shaders
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_ru2DFXTYPE_FLAT);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_ru2DFXTYPE_FLAT_MASKED);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_ru2DFXTYPE_BLIT);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_ru2DFXTYPE_BLIT_ADD);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_ru2DFXTYPE_BLIT_FILTER);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_ru2DFXTYPE_TEXTBLIT_FILTER);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASK);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASKED);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_ru2DFXTYPE_BLIT_LUMINANCE);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_ru2DFXTYPE_BLIT_MASK_Z);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_ru2DFXTYPE_DARKEN);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_ru2DFXTYPE_EXPONENTIATE);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_ru2DFXTYPE_GAUSSIAN);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_ru2DFXTYPE_DISTORTION);

			// Initialize NULL shader
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_NULL);

			// Sky shaders
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_SkyDome);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_CloudDome);

			// Particle shaders
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_Particle_SpriteEx);

			// FX shaders
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_FX_Decal);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_FX_DecalShadow);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_FX_GhostTrail);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_FX_Lightning);

			// UI shaders
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_Widget_Label);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_FX2D_UIFX_Darken);

			// Editor-specific shaders
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_Editor_GraphVis);
			RuShaderSys_CreateDefaultShader(CRuShader_SMUNI_Editor_Translucent);

			// Initialize Shader Model 2.0EZ Shaders
			// Initialize model shaders
			RuShaderSys_CreateShader("SM20EZ", CRuShader_SM20EZ_Static);

			RuShaderSys_CreateShader("SM20EZ", CRuShader_SM20EZ_Skin);

			RuShaderSys_CreateShader("SM20EZ", CRuShader_SM20EZ_Paperdoll);

			// Initialize terrain shaders
			RuShaderSys_CreateShader("SM20EZ", CRuShader_SM20EZ_Terrain);
			RuShaderSys_CreateShader("SM20EZ", CRuShader_SM20EZ_Terrain_Packed);
			RuShaderSys_CreateShader("SM20EZ", CRuShader_SM20EZ_Terrain_Packed_Backdrop);

			// Initialize water shaders
			RuShaderSys_CreateShader("SM20EZ", CRuShader_SM20EZ_Water_Full);
			RuShaderSys_CreateShader("SM20EZ", CRuShader_SM20EZ_Water_Minimal);
			RuShaderSys_CreateShader("SM20EZ", CRuShader_SM20EZ_Water_Lava);

			// Initialize doodad shader
			RuShaderSys_CreateShader("SM20EZ", CRuShader_SM20EZ_Particle_Doodad);

			// Register mesh instantiators
			RuVertDecl_RegisterInstantiator(CRuMeshInstantiator_Static);
			RuVertDecl_RegisterInstantiator(CRuMeshInstantiator_StaticColored);
			RuVertDecl_RegisterInstantiator(CRuMeshInstantiator_StaticColoredOld);
			RuVertDecl_RegisterInstantiator(CRuMeshInstantiator_Prelit);
			RuVertDecl_RegisterInstantiator(CRuMeshInstantiator_PrelitNoTex);
			RuVertDecl_RegisterInstantiator(CRuMeshInstantiator_Skinned);
			RuVertDecl_RegisterInstantiator(CRuMeshInstantiator_CompressedTerrain);

			RuVertDecl_RegisterInstantiator(CRuMeshInstantiator_Position);
			RuVertDecl_RegisterInstantiator(CRuMeshInstantiator_Normal);
			RuVertDecl_RegisterInstantiator(CRuMeshInstantiator_Normal_UBYTE4);
			RuVertDecl_RegisterInstantiator(CRuMeshInstantiator_Diffuse);
			RuVertDecl_RegisterInstantiator(CRuMeshInstantiator_UV0);
		}

		return swapChain;
	}

	return NULL;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
