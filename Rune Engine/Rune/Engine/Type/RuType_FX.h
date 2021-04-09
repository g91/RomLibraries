#ifndef _RUTYPE_FX_H_
#define _RUTYPE_FX_H_

#pragma managed(push, off)

// ************************************************************************************************************************************************************

enum RuWaterEffectType
{
	ruWATERFXTYPE_FULL							= 0,							//!< Full reflection + refraction
	ruWATERFXTYPE_DIFFUSE						= 1,							//!< Diffuse map only
	ruWATERFXTYPE_DIRECT						= 2,							//!< Use bump map directly as texture
	ruWATERFXTYPE_FORCE_DWORD					= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
