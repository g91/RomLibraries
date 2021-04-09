#ifndef _RUCORE_H_
#define _RUCORE_H_

// Object and type system
#include "../../Core/Type/RuType_Object.h"

// Thread system
#include "../../Core/Thread/RuThread_Core.h"
#include "../../Core/Thread/RuThread_SyncObject.h"
#include "../../Core/Thread/RuThread_CriticalSection.h"
#include "../../Core/Thread/RuThread_Worker.h"
#include "../../Core/Thread/RuThread_WorkQueue.h"

// Math system
#include "../../Core/Math/RuMath.h"
#include "../../Core/Math/RuMathBase.h"
#include "../../Core/Math/RuMathVector.h"
#include "../../Core/Math/RuMathGeometry.h"
#include "../../Core/Math/RuMathCollision.h"
#include "../../Core/Math/RuMathSSE.h"
#include "../../Core/Math/RuMath_Spline.h"
#include "../../Core/Math/RuMath_UVTransform.h"

// Utilities
#include "../../Core/Utility/RuUtility_Profile.h"
#include "../../Core/Utility/RuUtility_Stream.h"

// Collision system
#include "../../Collision/RuCollision_Base.h"
#include "../../Collision/RuCollision_kdTree.h"
#include "../../Collision/RuCollision_kdTree_16.h"
#include "../../Collision/RuCollision_Simple.h"
#include "../../Collision/RuCollisionHierarchy_Base.h"
#include "../../Collision/RuCollisionHierarchy_Simple.h"

// ImageFX system
#include "../../ImageFX/RuImageFX_Resample.h"

// Base engine types
#include "../../Engine/Type/RuType_Core.h"
#include "../../Engine/Type/RuType_Flags.h"
#include "../../Engine/Type/RuType_Vertex.h"

// Base engine
#include "../../Engine/Base/RuEngine_Engine.h"

// Camera system
#include "../../Engine/RuCamera.h"

// Texture system
#include "../../Engine/Texture/RuTexture_Proxy.h"

// Other helpers
#include "../../Helper/RuHelper_StreamIO.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	RuLightProperty
	@author John Tang
	@version 2008/07/28
*/
class RuLightSettings
{
public:
	RuLightType					m_type;												//!< Light type
	CRuColor4					m_diffuse;											//!< Diffuse color of light
	CRuColor4					m_specular;											//!< Specular color of light
	CRuVector3					m_position;											//!< Position of light
	CRuVector3					m_direction;										//!< Direction of light
	REAL						m_range;											//!< Maximum effect range of light
	REAL						m_falloff;											//!< Falloff between the inner and outer cone of a spotlight
	REAL						m_attenuation0;										//!< Constant attenuation term
	REAL						m_attenuation1;										//!< Linear attenuation term
	REAL						m_attenuation2;										//!< Squared attenuation term
	REAL						m_theta;											//!< Angle of a spotlight's inner cone
	REAL						m_phi;												//!< Angle of a spotlight's outer cone

public:
								RuLightSettings()
								{
									m_type = ruLIGHTTYPE_POINT;
									m_diffuse.Set(1.0f, 1.0f, 1.0f, 1.0f);
									m_specular.Set(1.0f, 1.0f, 1.0f, 1.0f);
									m_position.Set(0.0f, 0.0f, 0.0f);
									m_direction.Set(0.0f, 1.0f, 0.0f);
									m_range = 1.0f;
									m_falloff = 1.0f;
									m_attenuation0 = 0.0f;
									m_attenuation1 = 1.0f;
									m_attenuation2 = 0.0f;
									m_theta = 0.0f;
									m_phi = 0.0f;
								}
};

// ************************************************************************************************************************************************************

/*!
	Specifies a group of shader parameters
	@author John Tang
	@version 2004/10/17
*/
class CRuParameterBlock
{
protected:
	struct ParameterSet
	{
		RuParameterType								m_parameterType;	// Type of the parameter
		UINT64										m_parameter;		// Parameter

		ParameterSet(RuParameterType parameterType = ruPARAMTYPE_INVALID, UINT64 parameter = 0)
		:	m_parameterType(parameterType),
			m_parameter(parameter)
		{
		}
	};

	CRuArrayList<ParameterSet>						m_parameters;		// List of parameters stored by this block

public:
													CRuParameterBlock();
													~CRuParameterBlock();

	INT32											GetNumParameters() const;
	BOOL											GetParameter(RuParameterType parameterType, UINT64 &parameterOut) const;
	BOOL											GetParameter(INT32 index, RuParameterType &parameterTypeOut, UINT64 &parameterOut) const;
	void											SetParameter(RuParameterType parameterType, UINT64 parameter);
	BOOL											ClearParameter(RuParameterType parameterType);

	CRuParameterBlock&								operator=(const CRuParameterBlock &paramBlock);
};

// ************************************************************************************************************************************************************

class CRuParameterCache
{
public:
	CRuBitField										m_parameterOK;
	INT64											m_parameter[ruPARAMTYPE_MAX];

public:
													CRuParameterCache();
													~CRuParameterCache();

	BOOL											GetParameter(RuParameterType parameterType, UINT64 &parameterOut) const;
	void											CacheParameter(RuParameterType parameterType, UINT64 parameter);
	void											CacheParameterBlock(const CRuParameterBlock *paramBlock);
	void											ClearParameterCache();
};

// ************************************************************************************************************************************************************

/*!
	CRuParameterBlock::CRuParameterBlock()
	Standard constructor
*/
inline CRuParameterBlock::CRuParameterBlock()
{
}

/*!
	CRuParameterBlock::~CRuParameterBlock()
	Standard destructor
*/
inline CRuParameterBlock::~CRuParameterBlock()
{
	for(INT32 i = 0; i < m_parameters.Count(); ++i)
	{
		switch(m_parameters[i].m_parameterType)
		{
			// Default case, don't do anything
			default:
				break;

			// Float-4 case
			case ruPARAMTYPE_SPLAT_ALPHAUVMODIFIER:
				{
					delete[] ((float *) m_parameters[i].m_parameter);
				}

				break;

			// CRuVector3 case
			case ruPARAMTYPE_LIGHT_SPECULAR_DIR:
				{
					delete reinterpret_cast<CRuVector3 *>(m_parameters[i].m_parameter);
				}

				break;

			// CRuPlane4 case
			case ruPARAMTYPE_WATER_SPLITPLANE:
				{
					delete reinterpret_cast<CRuPlane4 *>(m_parameters[i].m_parameter);
				}

				break;
		}
	}
}

inline INT32 CRuParameterBlock::GetNumParameters() const
{
	return m_parameters.Count();
}

inline BOOL CRuParameterBlock::GetParameter(RuParameterType parameterType, UINT64 &parameterOut) const
{
	for(INT32 i = 0; i < m_parameters.Count(); ++i)
	{
		if(m_parameters[i].m_parameterType == parameterType)
		{
			return GetParameter(i, parameterType, parameterOut);
		}
	}

	return FALSE;
}

/*!
	BOOL CRuParameterBlock::GetParameter(INT32 index, RuParameterType &parameterTypeOut, INT64 &parameterOut)
	Gets the shader parameter at the specified index
	@param index Index to get the parameter from
	@param parameterTypeOut Parameter type output
	@param parameterOut Parameter output
	@return BOOL
	@retval FALSE No parameter at the specified index
*/
inline BOOL CRuParameterBlock::GetParameter(INT32 index, RuParameterType &parameterTypeOut, UINT64 &parameterOut) const
{
	// Output parameter
	parameterTypeOut = m_parameters[index].m_parameterType;
	parameterOut = m_parameters[index].m_parameter;

	return TRUE;
}

/*!
	void CRuParameterBlock::SetParameter(RuParameterType parameterType, INT64 parameter)
	Sets a parameter for a given parameter type
	@param parameterTypeOut Parameter type
	@param parameterOut Parameter
	@return None
*/
inline void CRuParameterBlock::SetParameter(RuParameterType parameterType, UINT64 parameter)
{
	switch(parameterType)
	{
		// Default case, don't do any special storage, just store the parameter as a DWORD
		default:
			{
				// Find the parameter type in the existing parameter list
				for(INT32 i = 0; i < m_parameters.Count(); ++i)
				{
					if(m_parameters[i].m_parameterType == parameterType)
					{
						m_parameters[i].m_parameter = parameter;
						return;
					}
				}

				// The parameter type does not exist, add a new one
				m_parameters.Add(ParameterSet(parameterType, parameter));
			}

			break;

		// Float-4 case
		case ruPARAMTYPE_SPLAT_ALPHAUVMODIFIER:
			{
				// Find the parameter type in the existing parameter list
				for(INT32 i = 0; i < m_parameters.Count(); ++i)
				{
					if(m_parameters[i].m_parameterType == parameterType)
					{
						memcpy((float *) m_parameters[i].m_parameter, (float *) parameter, sizeof(float) * 4);
						return;
					}
				}

				// The parameter type does not exist, add a new one
				float *internalFloatArray = ruNEW float [4];
				memcpy(internalFloatArray, (float *) parameter, sizeof(float) * 4);
				m_parameters.Add(ParameterSet(parameterType, (UINT64) internalFloatArray));
			}

			break;

		// CRuVector3 case
		case ruPARAMTYPE_LIGHT_SPECULAR_DIR:
			{
				// Find the parameter type in the existing parameter list
				for(INT32 i = 0; i < m_parameters.Count(); ++i)
				{
					if(m_parameters[i].m_parameterType == parameterType)
					{
						*reinterpret_cast<CRuVector3 *>(m_parameters[i].m_parameter) = *reinterpret_cast<CRuVector3 *>(parameter);
						return;
					}
				}

				// The parameter type does not exist, add a new one
				CRuVector3 *internalVector = ruNEW CRuVector3(*reinterpret_cast<CRuVector3 *>(parameter));
				m_parameters.Add(ParameterSet(parameterType, (UINT64) internalVector));
			}

			break;

		// CRuPlane4 case
		case ruPARAMTYPE_WATER_SPLITPLANE:
			{
				// Find the parameter type in the existing parameter list
				for(INT32 i = 0; i < m_parameters.Count(); ++i)
				{
					if(m_parameters[i].m_parameterType == parameterType)
					{
						*reinterpret_cast<CRuPlane4 *>(m_parameters[i].m_parameter) = *reinterpret_cast<CRuPlane4 *>(parameter);
						return;
					}
				}

				// The parameter type does not exist, add a new one
				CRuPlane4 *internalPlane = ruNEW CRuPlane4(*reinterpret_cast<CRuPlane4 *>(parameter));
				m_parameters.Add(ParameterSet(parameterType, (UINT64) internalPlane));
			}

			break;
	}
}

inline BOOL CRuParameterBlock::ClearParameter(RuParameterType parameterType)
{
	switch(parameterType)
	{
		// Default case, doesn't use any special storage, just remove the parameter
		default:
			{
				// Find the parameter type in the existing parameter list
				for(INT32 i = 0; i < m_parameters.Count(); ++i)
				{
					if(m_parameters[i].m_parameterType == parameterType)
					{
						m_parameters.RemoveAt(i);

						return TRUE;
					}
				}
			}

			break;

		// Float-4 case
		case ruPARAMTYPE_SPLAT_ALPHAUVMODIFIER:
			{
				// Find the parameter type in the existing parameter list
				for(INT32 i = 0; i < m_parameters.Count(); ++i)
				{
					if(m_parameters[i].m_parameterType == parameterType)
					{
						delete[] ((float *) m_parameters[i].m_parameter);

						m_parameters.RemoveAt(i);

						return TRUE;
					}
				}
			}

			break;

		// CRuVector3 case
		case ruPARAMTYPE_LIGHT_SPECULAR_DIR:
			{
				// Find the parameter type in the existing parameter list
				for(INT32 i = 0; i < m_parameters.Count(); ++i)
				{
					if(m_parameters[i].m_parameterType == parameterType)
					{
						delete reinterpret_cast<CRuVector3 *>(m_parameters[i].m_parameter);

						m_parameters.RemoveAt(i);

						return TRUE;
					}
				}
			}

			break;

		// CRuPlane4 case
		case ruPARAMTYPE_WATER_SPLITPLANE:
			{
				// Find the parameter type in the existing parameter list
				for(INT32 i = 0; i < m_parameters.Count(); ++i)
				{
					if(m_parameters[i].m_parameterType == parameterType)
					{
						delete reinterpret_cast<CRuPlane4 *>(m_parameters[i].m_parameter);

						m_parameters.RemoveAt(i);

						return TRUE;
					}
				}
			}

			break;
	}

	return FALSE;
}

inline CRuParameterBlock &CRuParameterBlock::operator=(const CRuParameterBlock &paramBlock)
{
	// Clear parameters
	m_parameters.Clear();

	// Copy parameters from source block
	for(INT32 i = 0; i < paramBlock.m_parameters.Count(); ++i)
	{
		m_parameters.Add(paramBlock.m_parameters[i]);
	}

	return *this;
}

// ************************************************************************************************************************************************************

inline CRuParameterCache::CRuParameterCache()
:	m_parameterOK(ruPARAMTYPE_MAX)
{
}

inline CRuParameterCache::~CRuParameterCache()
{
}

inline BOOL CRuParameterCache::GetParameter(RuParameterType parameterType, UINT64 &parameterOut) const
{
	if(m_parameterOK[parameterType])
	{
		parameterOut = m_parameter[parameterType];

		return TRUE;
	}

	return FALSE;
}

inline void CRuParameterCache::CacheParameter(RuParameterType parameterType, UINT64 parameter)
{
	// Perform special processing for specific parameters
	switch(parameterType)
	{
		case ruPARAMTYPE_MATERIAL_ALPHAMODE:
			{
				// If there is an existing alpha mode parameter, combine with it
				if(m_parameterOK[parameterType])
				{
					switch(m_parameter[parameterType])
					{
						default:
						case ruALPHAMODE_NORMAL:
							break;

						case ruALPHAMODE_ADD:
						case ruALPHAMODE_SUBTRACT:
							parameter = m_parameter[parameterType];
							break;

						case ruALPHAMODE_PREMULTIPLIED_NORMAL:
							switch(parameter)
							{
								default:
								case ruALPHAMODE_NORMAL:
									parameter = m_parameter[parameterType];
									break;

								case ruALPHAMODE_ADD:
									parameter = ruALPHAMODE_PREMULTIPLIED_NORMAL;
									break;

								case ruALPHAMODE_SUBTRACT:
									parameter = ruALPHAMODE_SUBTRACT;
									break;

								case ruALPHAMODE_PREMULTIPLIED_NORMAL:
								case ruALPHAMODE_PREMULTIPLIED_ADD:
									break;
							}

							break;

						case ruALPHAMODE_PREMULTIPLIED_ADD:
							switch(parameter)
							{
								default:
								case ruALPHAMODE_NORMAL:
								case ruALPHAMODE_ADD:
								case ruALPHAMODE_PREMULTIPLIED_NORMAL:
									parameter = m_parameter[parameterType];
									break;

								case ruALPHAMODE_SUBTRACT:
									parameter = ruALPHAMODE_SUBTRACT;
									break;

								case ruALPHAMODE_PREMULTIPLIED_ADD:
									break;
							}

							break;
					}
				}
			}

			break;

		case ruPARAMTYPE_MATERIAL_ALPHA:
		case ruPARAMTYPE_MATERIAL_ALPHAMODIFIER:
			{
				// If there is an existing alpha parameter, combine with it
				if(m_parameterOK[parameterType])
				{
					REAL originalAlpha = RuCastINT64ToFloat(m_parameter[parameterType]);
					REAL additionalAlpha = RuCastINT64ToFloat(parameter);
					parameter = RuCastFloatToINT64(originalAlpha * additionalAlpha);
				}
			}

			break;

		case ruPARAMTYPE_MATERIAL_DIFFUSE:
			{
				CRuColor4 additionalDiffuse((RUCOLOR)parameter);

				// If there is an existing diffuse parameter, combine with it
				if(m_parameterOK[parameterType])
				{
					CRuColor4 originalDiffuse((RUCOLOR)m_parameter[parameterType]);
					parameter = (additionalDiffuse * originalDiffuse).GetARGB();
				}
			}

			break;
	}

	// Set parameter into shader
	m_parameterOK.Set(parameterType);
	m_parameter[parameterType] = parameter;
}

inline void CRuParameterCache::CacheParameterBlock(const CRuParameterBlock *paramBlock)
{
	RuParameterType parameterType;
	UINT64 parameter;

	for(INT32 curParamIndex = 0, numParams = paramBlock->GetNumParameters(); curParamIndex < numParams; ++curParamIndex)
	{
		// Retrieve parameter from the parameter block
		paramBlock->GetParameter(curParamIndex, parameterType, parameter);

		// Cache the parameter
		CacheParameter(parameterType, parameter);
	}
}

inline void CRuParameterCache::ClearParameterCache()
{
	m_parameterOK.ClearAll();
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
