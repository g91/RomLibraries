/*!
	@file RuEntity_Particle_Base.cpp

	Copyright (c) 2004-2005 All rights reserved
*/

#include "../../Scene/Particle/RuEntity_Particle_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(IRuEntity_Particle_Emitter, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(IRuEntity_Particle_Emitter, "IRuEntity_Particle_Emitter", "CRuEntity")
ruCLASSGUID_DEFINE(IRuEntity_Particle_Emitter, 0xE0060CE3, 0xF3C94c4d, 0x9184DD61, 0xC729F2AE)

ruRTTI_DEFINE_SUBCLASS(IRuEntity_Particle_Visualizer, IRuEntity_Container)
ruHEAP_DEFINE_SUBCLASS(IRuEntity_Particle_Visualizer, "IRuEntity_Particle_Visualizer", "IRuEntity_Container")
ruCLASSGUID_DEFINE(IRuEntity_Particle_Visualizer, 0xF53B4BBE, 0x89AD4fa6, 0xB59C8B34, 0xBF4C12BF)

// ************************************************************************************************************************************************************

IRuEntity_Particle_Emitter::IRuEntity_Particle_Emitter()
{
}

IRuEntity_Particle_Emitter::~IRuEntity_Particle_Emitter()
{
}

// ************************************************************************************************************************************************************

IRuEntity_Particle_Visualizer::IRuEntity_Particle_Visualizer()
{
}

IRuEntity_Particle_Visualizer::~IRuEntity_Particle_Visualizer()
{
}

// ************************************************************************************************************************************************************

#pragma managed
