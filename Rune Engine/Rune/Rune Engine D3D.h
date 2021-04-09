#pragma once

// Engine - Direct3D
#include "Direct3D/Engine/RuEngineD3D.h"

// Managers
#include "Engine/Manager/RuEngine_ShaderManager.h"
#include "Engine/Manager/RuEngine_InstanceManager.h"
#include "Engine/Manager/RuEngine_ResourceManager.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

extern RUAPI IRuSwapChain*	RuInitialize_D3D(HWND primaryWindow, HWND focusWindow, UINT32 resourceArenaSize);

// ************************************************************************************************************************************************************

#pragma managed(pop)
