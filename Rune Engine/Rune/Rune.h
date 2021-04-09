#ifndef _RUNE_H_
#define _RUNE_H_

// Algorithms
#include "Core/Algorithm/RuAlgorithm_PatternMatch_FJS.h"

// Data store system
#include "Core/DataStore/RuDataStore_Disk.h"

// Profiler and logging
#include "Core/Utility/RuUtility_Profile.h"

// Other utilities
#include "Core/Utility/RuUtility_PropertyContainer.h"

// Engine base
#include "Engine/Base/RuEngine_Engine.h"
#include "Engine/Base/RuShader_Composite.h"
#include "Engine/Manager/RuEngine_ShaderManager.h"
#include "Engine/Texture/RuTexture_Proxy.h"
#include "Engine/Texture/RuTextureGenerator.h"
#include "Engine/Texture/RuTextureUtility.h"

// ODE
#include "Core/Math/RuODESolver_RK4.h"

// UI
#include "UI/RuUI_Font.h"

// Path Engine
#include "PathEngine/RuPathRover_Base.h"
#include "PathEngine/RuPathRover_Simple.h"
#include "PathEngine/RuPathGraph_Base.h"
#include "PathEngine/RuPathGraph_AStar.h"
#include "PathEngine/RuPathHierarchy_Base.h"
#include "PathEngine/RuPathHierarchy_DS.h"

// Scene system
#include "Scene/Container/RuContainer_Collision.h"
#include "Scene/Container/RuContainer_Portal.h"
#include "Scene/Occlusion/RuEntity_AntiPortal.h"
#include "Renderer/RuRenderer.h"
#include "Renderer/RendererNode/RuRendererNode_RenderContext.h"
#include "Renderer/RendererNode/RuRendererNode_SetScissor.h"
#include "Renderer/RendererNode/RuRendererNode_TextureCopy.h"
#include "Renderer/RendererNode/RuRendererNode_UI.h"
#include "Renderer/RendererNode/RuRendererNode_ViewportClear.h"

// Miscellaneous
#include "Scene/ColVis/RuEntity_ColVis.h"
#include "Scene/PathEngineVis/RuEntity_PathEngineVis.h"

#include "Scene/Base/RuHierarchy_GR2.h"

// Controllers
#include "Scene/Controller/RuController_Bone.h"
#include "Scene/Controller/RuController_Camera.h"
#include "Scene/Controller/RuController_Decal.h"
#include "Scene/Controller/RuController_DistanceFader.h"
#include "Scene/Controller/RuController_Entity.h"
#include "Scene/Controller/RuController_EventTrigger.h"
#include "Scene/Controller/RuController_Frame.h"
#include "Scene/Controller/RuController_GhostTrail.h"
#include "Scene/Controller/RuController_GlobalAlpha.h"
#include "Scene/Controller/RuController_GlobalColor.h"
#include "Scene/Controller/RuController_Hierarchy.h"
#include "Scene/Controller/RuController_Highlighter.h"
#include "Scene/Controller/RuController_MaterialTexture.h"
#include "Scene/Controller/RuController_Morph.h"
#include "Scene/Controller/RuController_Motion.h"
#include "Scene/Controller/RuController_ParticleSystem.h"
#include "Scene/Controller/RuController_Particle_Emitter.h"
#include "Scene/Controller/RuController_Particle_Emitter_Enviro.h"
#include "Scene/Controller/RuController_Particle_Emitter_Line.h"
#include "Scene/Controller/RuController_Particle_Visualizer_Entity.h"
#include "Scene/Controller/RuController_Particle_Visualizer_Sprite.h"
#include "Scene/Controller/RuController_PolyLine.h"
#include "Scene/Controller/RuController_UVFX.h"
#include "Scene/Controller/RuController_VisibilityChain.h"
#include "Scene/Controller/RuController_WaterFX.h"

// Effect systems
#include "Scene/FX/RuEntity_CollectorFX.h"
#include "Scene/FX/RuEntity_Decal.h"
#include "Scene/FX/RuEntity_DecalShadow.h"
#include "Scene/FX/RuEntity_GhostTrail.h"
#include "Scene/FX/RuEntity_PolyLine.h"

// FX2D systems
#include "Scene/FX2D/RuFX2D_Model.h"
#include "Scene/FX2D/RuFX2D_UIFX.h"

// Particle systems
#include "Scene/Particle/RuEntity_Particle_Doodad.h"
#include "Scene/Particle/RuEntity_Particle_Sprite.h"
#include "Scene/Particle/RuEntity_Particle_Emitter.h"
#include "Scene/Particle/RuEntity_Particle_Emitter_Enviro.h"
#include "Scene/Particle/RuEntity_Particle_Emitter_Line.h"
#include "Scene/Particle/RuEntity_Particle_Visualizer_Entity.h"
#include "Scene/Particle/RuEntity_Particle_Visualizer_Sprite.h"

// Environmental systems
#include "Scene/Terrain/RuEntitySkyDome.h"
#include "Scene/Terrain/RuEntityOptimalTerrain.h"
#include "Scene/Terrain/RuEntity_Environment.h"		// this depends on GAIA (BAD, REMOVE GAIA ASAP!)
#include "Scene/Terrain/RuWorld_CloudPlane.h"

// Map systems
#include "Scene/TileMap/RuEntity_TileMap.h"

// Gizmos
#include "Scene/Gizmo/RuGizmo_Box.h"
#include "Scene/Gizmo/RuGizmo_Mover.h"
#include "Scene/Gizmo/RuGizmo_Plane.h"

// Widgets
#include "Scene/Widget/RuEntityGizmo.h"
#include "Scene/Widget/RuEntity_Widget_AABB.h"
#include "Scene/Widget/RuEntity_Widget_BoneVis.h"
#include "Scene/Widget/RuEntity_Widget_ClipRect.h"
#include "Scene/Widget/RuEntity_Widget_Flag.h"
#include "Scene/Widget/RuEntity_Widget_FrustumVis.h"
#include "Scene/Widget/RuEntity_Widget_Grid.h"
#include "Scene/Widget/RuEntity_Widget_Label.h"
#include "Scene/Widget/RuEntity_Widget_Line.h"
#include "Scene/Widget/RuEntity_Widget_Sphere.h"
#include "Scene/Widget/RuEntity_Widget_Tetrahedron.h"
#include "Scene/Widget/RuEntity_Widget_Text.h"
#include "Scene/Widget/RuEntity_Widget_TileGrid.h"

// Paperdoll system
#include "Scene/Paperdoll/RuEntityPaperdoll.h"
#include "Scene/Paperdoll/RuPaperdollCache.h"

// Proxy
#include "Scene/Proxy/RuEntity_Proxy.h"

// Plugin
#include "Renderer/Plugin/RuRendererPlugin_AlphaZ.h"
#include "Renderer/Plugin/RuRendererPlugin_AquaFX.h"
#include "Renderer/Plugin/RuRendererPlugin_BlendFX.h"
#include "Renderer/Plugin/RuRendererPlugin_GlowFX.h"
#include "Renderer/Plugin/RuRendererPlugin_RTLightMap.h"
#include "Renderer/Plugin/RuRendererPlugin_VizCon.h"

// Noise Generator
#include "Core/Noise/RuNoise_Base.h"
#include "Core/Noise/RuNoise_Perlin.h"
#include "Core/Noise/RuNoise_Texture.h"
#include "Core/Noise/RuNoise_Value.h"
#include "Core/Noise/RuNoise_Worley.h"

// Fractal Generator
#include "Core/Fractal/RuFractal_Base.h"
#include "Core/Fractal/RuFractal_Composite.h"
#include "Core/Fractal/RuFractal_fBm.h"
#include "Core/Fractal/RuFractal_HeteroTerrain.h"
#include "Core/Fractal/RuFractal_HybridMultifractal.h"
#include "Core/Fractal/RuFractal_Multifractal.h"
#include "Core/Fractal/RuFractal_RidgedMultifractal.h"
#include "Core/Fractal/RuFractal_Shapes.h"

// Fractal post-processing
#include "Core/Fractal/RuFractal_PostFX_ABS.h"
#include "Core/Fractal/RuFractal_PostFX_Cutoff.h"
#include "Core/Fractal/RuFractal_PostFX_Gain.h"
#include "Core/Fractal/RuFractal_PostFX_Remap.h"
#include "Core/Fractal/RuFractal_PostFX_Square.h"

// Helper layer
#include "Helper/RuHelper_Entity.h"
#include "Helper/RuHelper_IRuMaterial.h"
#include "Helper/RuHelper_IRuEntity_Renderable.h"
#include "Helper/RuHelper_Math.h"

// MOJO
#include "Mojo/RuMojo_Base.h"
#include "Mojo/RuMojo_Helper.h"

// FUSION Game Components
#include "Fusion/RuFusion_Actor.h"
#include "Fusion/RuFusion_Mime.h"
#include "Fusion/RuFusion_Minimap.h"
#include "Fusion/RuFusion_Sojourner.h"
#include "Fusion/RuFusion_UIElement.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

extern RUAPI BOOL						RuInitialize_Core();
extern RUAPI BOOL						RuInitialize_Scene(IRuSwapChain *primarySwapChain);
extern RUAPI IRuSwapChain*				RuCreateSwapChain(UINT32 width, UINT32 height, HWND focusWindow);
extern RUAPI BOOL						RuSetResolution(IRuSwapChain *swapChain, UINT32 width, UINT32 height, BOOL fullscreen);
extern RUAPI BOOL						RuShutdown();

// ************************************************************************************************************************************************************

extern CRuRendererPlugin_AquaFX*		g_ruRendererPlugin_AquaFX;
extern CRuRendererPlugin_BlendFX*		g_ruRendererPlugin_BlendFX;
extern CRuRendererPlugin_GlowFX*		g_ruRendererPlugin_GlowFX;
extern CRuRendererPlugin_VizCon*		g_ruRendererPlugin_VizCon;

extern char								g_ruGlobalMessage[2048];

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
