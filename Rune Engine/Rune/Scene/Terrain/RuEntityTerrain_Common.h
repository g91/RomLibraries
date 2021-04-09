#ifndef _RUENTITYTERRAIN_COMMON_H_
#define _RUENTITYTERRAIN_COMMON_H_

// Rune entity base
#include "../../Core/DataStore/RuDataStore_Base.h"
#include "../../Core/Fractal/RuFractal_Base.h"
#include "../../Engine/Texture/RuTexture_Proxy.h"
#include "../../Scene/Base/RuEntityBase.h"
#include "../../Mojo/RuMojo_Base.h"
#include "../../Mojo/RuMojo_Helper.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

#define ruWORLD_MAKECHUNKKEY(x, z)		((((x) & 0x0000FFFF) << 16) | ((z) & 0x0000FFFF))
#define ruWORLD_CHUNKKEYX(chunkKey)		(((chunkKey) >> 16) & 0x0000FFFF)
#define ruWORLD_CHUNKKEYZ(chunkKey)		((chunkKey) & 0x0000FFFF)


// ************************************************************************************************************************************************************

class CRuSplatIDMap;
class CRuSplatMapBuilder;

// ************************************************************************************************************************************************************

namespace RuWorld
{
	/*
		Neighbor ID Defined
		-------------------

		| 0 | 1 | 2 |
		-------------
		| 3 | X | 4 |
		-------------
		| 5 | 6 | 7 |

		Neighbor ID is defined as an integer, specifying the spatial location
		relative to the current chunk. For instance, 0 specifies the neighboring
		chunk at (-1, -1). 
	*/

	typedef boost::function<CRuSplatIDMap * (INT32, INT32)> GetNeighborSplat;		// Parameters: (chunk key, neighbor ID)
}

// ************************************************************************************************************************************************************

enum RuTerrainUtility_PaintCondition
{
	ruTUPAINTCONDITION_ALL					= 0,
	ruTUPAINTCONDITION_AFFECTEDONLY			= 1,

	ruTUPAINTCONDITION_FORCE_DWORD			= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuTerrainUtility_BooleanType
{
	ruTUBOOLEANTYPE_OVERWRITE				= 0,
	ruTUBOOLEANTYPE_UNION					= 1,
	ruTUBOOLEANTYPE_INTERSECTION			= 2,

	ruTUBOOLEANTYPE_FORCE_DWORD				= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

enum RuTerrainUtility_FalloffType
{
	ruTUFALLOFFTYPE_NONE					= 0,
	ruTUFALLOFFTYPE_ALL						= 1,
	ruTUFALLOFFTYPE_LINEAR					= 2,
	ruTUFALLOFFTYPE_QUADRATIC				= 3,
	ruTUFALLOFFTYPE_CUBIC					= 4,
	ruTUFALLOFFTYPE_QUARTIC					= 5,
	ruTUFALLOFFTYPE_QUINTIC					= 6,
	ruTUFALLOFFTYPE_BEVEL					= 7,
	ruTUFALLOFFTYPE_SMOOTHBEVEL				= 8,

	ruTUFALLOFFTYPE_FORCE_DWORD				= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

class CRuSplatIDMap_PaintSetting
{
public:
	INT32									m_splat_0;
	INT32									m_splat_1;
	INT32									m_splat_2;
	INT32									m_splat_3;
	INT32									m_splat_4;
	INT32									m_splat_5;

	RuTerrainUtility_FalloffType			m_threshold_Falloff_0;
	RuTerrainUtility_FalloffType			m_threshold_Falloff_1;
	RuTerrainUtility_FalloffType			m_threshold_Falloff_2;
	RuTerrainUtility_FalloffType			m_threshold_Falloff_3;
	RuTerrainUtility_FalloffType			m_threshold_Falloff_4;
	RuTerrainUtility_FalloffType			m_threshold_Falloff_5;

	REAL									m_threshold_Upper_0;
	REAL									m_threshold_Upper_1;
	REAL									m_threshold_Upper_2;
	REAL									m_threshold_Upper_3;
	REAL									m_threshold_Upper_4;
	REAL									m_threshold_Upper_5;

	REAL									m_threshold_Lower_0;
	REAL									m_threshold_Lower_1;
	REAL									m_threshold_Lower_2;
	REAL									m_threshold_Lower_3;
	REAL									m_threshold_Lower_4;
	REAL									m_threshold_Lower_5;

	RuTerrainUtility_FalloffType			m_slope_Falloff_0;
	RuTerrainUtility_FalloffType			m_slope_Falloff_1;
	RuTerrainUtility_FalloffType			m_slope_Falloff_2;
	RuTerrainUtility_FalloffType			m_slope_Falloff_3;
	RuTerrainUtility_FalloffType			m_slope_Falloff_4;
	RuTerrainUtility_FalloffType			m_slope_Falloff_5;

	REAL									m_slope_Upper_0;
	REAL									m_slope_Upper_1;
	REAL									m_slope_Upper_2;
	REAL									m_slope_Upper_3;
	REAL									m_slope_Upper_4;
	REAL									m_slope_Upper_5;

	REAL									m_slope_Lower_0;
	REAL									m_slope_Lower_1;
	REAL									m_slope_Lower_2;
	REAL									m_slope_Lower_3;
	REAL									m_slope_Lower_4;
	REAL									m_slope_Lower_5;

	REAL									m_heightMultiplier;
	REAL									m_heightThreshold;
	REAL									m_heightOffset;

	REAL									m_brushCenterX;
	REAL									m_brushCenterY;
	REAL									m_brushLimit;

	RuTerrainUtility_PaintCondition			m_paintCondition;
	RuTerrainUtility_BooleanType			m_heightBooleanType;
	RuTerrainUtility_FalloffType			m_heightFalloffType;
	RuTerrainUtility_FalloffType			m_noiseFalloffType;

	// Run-time only data
	BOOL									m_heightAutoLevelOn;
	REAL									m_heightAutoLevel;

											CRuSplatIDMap_PaintSetting()
											{
												m_splat_0 = -1;
												m_splat_1 = -1;
												m_splat_2 = -1;
												m_splat_3 = -1;
												m_splat_4 = -1;
												m_splat_5 = -1;

												m_threshold_Falloff_0 = ruTUFALLOFFTYPE_NONE;
												m_threshold_Falloff_1 = ruTUFALLOFFTYPE_NONE;
												m_threshold_Falloff_2 = ruTUFALLOFFTYPE_NONE;
												m_threshold_Falloff_3 = ruTUFALLOFFTYPE_NONE;
												m_threshold_Falloff_4 = ruTUFALLOFFTYPE_NONE;
												m_threshold_Falloff_5 = ruTUFALLOFFTYPE_NONE;

												m_threshold_Upper_0 = 0.00f;
												m_threshold_Upper_1 = 0.00f;
												m_threshold_Upper_2 = 0.00f;
												m_threshold_Upper_3 = 0.00f;
												m_threshold_Upper_4 = 0.00f;
												m_threshold_Upper_5 = 0.00f;

												m_threshold_Lower_0 = 0.00f;
												m_threshold_Lower_1 = 0.00f;
												m_threshold_Lower_2 = 0.00f;
												m_threshold_Lower_3 = 0.00f;
												m_threshold_Lower_4 = 0.00f;
												m_threshold_Lower_5 = 0.00f;

												m_slope_Falloff_0 = ruTUFALLOFFTYPE_NONE;
												m_slope_Falloff_1 = ruTUFALLOFFTYPE_NONE;
												m_slope_Falloff_2 = ruTUFALLOFFTYPE_NONE;
												m_slope_Falloff_3 = ruTUFALLOFFTYPE_NONE;
												m_slope_Falloff_4 = ruTUFALLOFFTYPE_NONE;
												m_slope_Falloff_5 = ruTUFALLOFFTYPE_NONE;

												m_slope_Upper_0 = 0.00f;
												m_slope_Upper_1 = 0.00f;
												m_slope_Upper_2 = 0.00f;
												m_slope_Upper_3 = 0.00f;
												m_slope_Upper_4 = 0.00f;
												m_slope_Upper_5 = 0.00f;

												m_slope_Lower_0 = 0.00f;
												m_slope_Lower_1 = 0.00f;
												m_slope_Lower_2 = 0.00f;
												m_slope_Lower_3 = 0.00f;
												m_slope_Lower_4 = 0.00f;
												m_slope_Lower_5 = 0.00f;

												m_heightMultiplier = 0.0f;
												m_heightThreshold = 0.0f;
												m_heightOffset = 0.0f;

												m_brushCenterX = 0.0f;
												m_brushCenterY = 0.0f;
												m_brushLimit = -1.0f;

												m_paintCondition = ruTUPAINTCONDITION_ALL;
												m_heightBooleanType = ruTUBOOLEANTYPE_OVERWRITE;
												m_heightFalloffType = ruTUFALLOFFTYPE_NONE;
												m_noiseFalloffType = ruTUFALLOFFTYPE_NONE;

												m_heightAutoLevelOn = TRUE;
												m_heightAutoLevel = 0.0f;
											}

	void									SerializeFrom(IRuStream *stream)
											{
												INT32 version = 1;
												if(stream->Read(&version, sizeof(INT32)) == sizeof(INT32))
												{
													switch(version)
													{
														case 1:
															{
																stream->Read(&m_splat_0, sizeof(INT32));
																stream->Read(&m_splat_1, sizeof(INT32));
																stream->Read(&m_splat_2, sizeof(INT32));
																stream->Read(&m_splat_3, sizeof(INT32));

																stream->Read(&m_threshold_Lower_0, sizeof(REAL));
																stream->Read(&m_threshold_Lower_1, sizeof(REAL));
																stream->Read(&m_threshold_Lower_2, sizeof(REAL));
																stream->Read(&m_threshold_Lower_3, sizeof(REAL));

																stream->Read(&m_heightMultiplier, sizeof(REAL));
																stream->Read(&m_heightThreshold, sizeof(REAL));
																stream->Read(&m_heightOffset, sizeof(REAL));

																m_threshold_Upper_0 = 10000.0f;
																m_threshold_Upper_1 = 10000.0f;
																m_threshold_Upper_2 = 10000.0f;
																m_threshold_Upper_3 = 10000.0f;
																m_threshold_Upper_4 = 10000.0f;
																m_threshold_Upper_5 = 10000.0f;

																m_slope_Upper_0 = 10000.0f;
																m_slope_Upper_1 = 10000.0f;
																m_slope_Upper_2 = 10000.0f;
																m_slope_Upper_3 = 10000.0f;
																m_slope_Upper_4 = 10000.0f;
																m_slope_Upper_5 = 10000.0f;
															}

															break;

														case 2:
															{
																stream->Read(&m_splat_0, sizeof(INT32));
																stream->Read(&m_splat_1, sizeof(INT32));
																stream->Read(&m_splat_2, sizeof(INT32));
																stream->Read(&m_splat_3, sizeof(INT32));
																stream->Read(&m_splat_4, sizeof(INT32));
																stream->Read(&m_splat_5, sizeof(INT32));

																stream->Read(&m_threshold_Lower_0, sizeof(REAL));
																stream->Read(&m_threshold_Lower_1, sizeof(REAL));
																stream->Read(&m_threshold_Lower_2, sizeof(REAL));
																stream->Read(&m_threshold_Lower_3, sizeof(REAL));
																stream->Read(&m_threshold_Lower_4, sizeof(REAL));
																stream->Read(&m_threshold_Lower_5, sizeof(REAL));

																stream->Read(&m_slope_Lower_0, sizeof(REAL));
																stream->Read(&m_slope_Lower_1, sizeof(REAL));
																stream->Read(&m_slope_Lower_2, sizeof(REAL));
																stream->Read(&m_slope_Lower_3, sizeof(REAL));
																stream->Read(&m_slope_Lower_4, sizeof(REAL));
																stream->Read(&m_slope_Lower_5, sizeof(REAL));

																stream->Read(&m_heightMultiplier, sizeof(REAL));
																stream->Read(&m_heightThreshold, sizeof(REAL));
																stream->Read(&m_heightOffset, sizeof(REAL));

																stream->Read(&m_brushCenterX, sizeof(REAL));
																stream->Read(&m_brushCenterY, sizeof(REAL));
																stream->Read(&m_brushLimit, sizeof(REAL));

																stream->Read(&m_paintCondition, sizeof(RuTerrainUtility_PaintCondition));
																stream->Read(&m_heightBooleanType, sizeof(RuTerrainUtility_BooleanType));
																stream->Read(&m_heightFalloffType, sizeof(RuTerrainUtility_FalloffType));
																stream->Read(&m_noiseFalloffType, sizeof(RuTerrainUtility_FalloffType));

																m_threshold_Upper_0 = 10000.0f;
																m_threshold_Upper_1 = 10000.0f;
																m_threshold_Upper_2 = 10000.0f;
																m_threshold_Upper_3 = 10000.0f;
																m_threshold_Upper_4 = 10000.0f;
																m_threshold_Upper_5 = 10000.0f;

																m_slope_Upper_0 = 10000.0f;
																m_slope_Upper_1 = 10000.0f;
																m_slope_Upper_2 = 10000.0f;
																m_slope_Upper_3 = 10000.0f;
																m_slope_Upper_4 = 10000.0f;
																m_slope_Upper_5 = 10000.0f;
															}

															break;

														case 3:
															{
																stream->Read(&m_splat_0, sizeof(INT32));
																stream->Read(&m_splat_1, sizeof(INT32));
																stream->Read(&m_splat_2, sizeof(INT32));
																stream->Read(&m_splat_3, sizeof(INT32));
																stream->Read(&m_splat_4, sizeof(INT32));
																stream->Read(&m_splat_5, sizeof(INT32));

																stream->Read(&m_threshold_Falloff_0, sizeof(RuTerrainUtility_FalloffType));
																stream->Read(&m_threshold_Falloff_1, sizeof(RuTerrainUtility_FalloffType));
																stream->Read(&m_threshold_Falloff_2, sizeof(RuTerrainUtility_FalloffType));
																stream->Read(&m_threshold_Falloff_3, sizeof(RuTerrainUtility_FalloffType));
																stream->Read(&m_threshold_Falloff_4, sizeof(RuTerrainUtility_FalloffType));
																stream->Read(&m_threshold_Falloff_5, sizeof(RuTerrainUtility_FalloffType));

																stream->Read(&m_threshold_Upper_0, sizeof(REAL));
																stream->Read(&m_threshold_Upper_1, sizeof(REAL));
																stream->Read(&m_threshold_Upper_2, sizeof(REAL));
																stream->Read(&m_threshold_Upper_3, sizeof(REAL));
																stream->Read(&m_threshold_Upper_4, sizeof(REAL));
																stream->Read(&m_threshold_Upper_5, sizeof(REAL));

																stream->Read(&m_threshold_Lower_0, sizeof(REAL));
																stream->Read(&m_threshold_Lower_1, sizeof(REAL));
																stream->Read(&m_threshold_Lower_2, sizeof(REAL));
																stream->Read(&m_threshold_Lower_3, sizeof(REAL));
																stream->Read(&m_threshold_Lower_4, sizeof(REAL));
																stream->Read(&m_threshold_Lower_5, sizeof(REAL));

																stream->Read(&m_slope_Falloff_0, sizeof(RuTerrainUtility_FalloffType));
																stream->Read(&m_slope_Falloff_1, sizeof(RuTerrainUtility_FalloffType));
																stream->Read(&m_slope_Falloff_2, sizeof(RuTerrainUtility_FalloffType));
																stream->Read(&m_slope_Falloff_3, sizeof(RuTerrainUtility_FalloffType));
																stream->Read(&m_slope_Falloff_4, sizeof(RuTerrainUtility_FalloffType));
																stream->Read(&m_slope_Falloff_5, sizeof(RuTerrainUtility_FalloffType));

																stream->Read(&m_slope_Upper_0, sizeof(REAL));
																stream->Read(&m_slope_Upper_1, sizeof(REAL));
																stream->Read(&m_slope_Upper_2, sizeof(REAL));
																stream->Read(&m_slope_Upper_3, sizeof(REAL));
																stream->Read(&m_slope_Upper_4, sizeof(REAL));
																stream->Read(&m_slope_Upper_5, sizeof(REAL));

																stream->Read(&m_slope_Lower_0, sizeof(REAL));
																stream->Read(&m_slope_Lower_1, sizeof(REAL));
																stream->Read(&m_slope_Lower_2, sizeof(REAL));
																stream->Read(&m_slope_Lower_3, sizeof(REAL));
																stream->Read(&m_slope_Lower_4, sizeof(REAL));
																stream->Read(&m_slope_Lower_5, sizeof(REAL));

																stream->Read(&m_heightMultiplier, sizeof(REAL));
																stream->Read(&m_heightThreshold, sizeof(REAL));
																stream->Read(&m_heightOffset, sizeof(REAL));

																stream->Read(&m_brushCenterX, sizeof(REAL));
																stream->Read(&m_brushCenterY, sizeof(REAL));
																stream->Read(&m_brushLimit, sizeof(REAL));

																stream->Read(&m_paintCondition, sizeof(RuTerrainUtility_PaintCondition));
																stream->Read(&m_heightBooleanType, sizeof(RuTerrainUtility_BooleanType));
																stream->Read(&m_heightFalloffType, sizeof(RuTerrainUtility_FalloffType));
																stream->Read(&m_noiseFalloffType, sizeof(RuTerrainUtility_FalloffType));
															}

															break;
													}
												}
											}

	void									SerializeTo(IRuStream *stream)
											{
												INT32 version = 3;
												stream->Write(&version, sizeof(INT32));

												stream->Write(&m_splat_0, sizeof(INT32));
												stream->Write(&m_splat_1, sizeof(INT32));
												stream->Write(&m_splat_2, sizeof(INT32));
												stream->Write(&m_splat_3, sizeof(INT32));
												stream->Write(&m_splat_4, sizeof(INT32));
												stream->Write(&m_splat_5, sizeof(INT32));

												stream->Write(&m_threshold_Falloff_0, sizeof(RuTerrainUtility_FalloffType));
												stream->Write(&m_threshold_Falloff_1, sizeof(RuTerrainUtility_FalloffType));
												stream->Write(&m_threshold_Falloff_2, sizeof(RuTerrainUtility_FalloffType));
												stream->Write(&m_threshold_Falloff_3, sizeof(RuTerrainUtility_FalloffType));
												stream->Write(&m_threshold_Falloff_4, sizeof(RuTerrainUtility_FalloffType));
												stream->Write(&m_threshold_Falloff_5, sizeof(RuTerrainUtility_FalloffType));

												stream->Write(&m_threshold_Upper_0, sizeof(REAL));
												stream->Write(&m_threshold_Upper_1, sizeof(REAL));
												stream->Write(&m_threshold_Upper_2, sizeof(REAL));
												stream->Write(&m_threshold_Upper_3, sizeof(REAL));
												stream->Write(&m_threshold_Upper_4, sizeof(REAL));
												stream->Write(&m_threshold_Upper_5, sizeof(REAL));

												stream->Write(&m_threshold_Lower_0, sizeof(REAL));
												stream->Write(&m_threshold_Lower_1, sizeof(REAL));
												stream->Write(&m_threshold_Lower_2, sizeof(REAL));
												stream->Write(&m_threshold_Lower_3, sizeof(REAL));
												stream->Write(&m_threshold_Lower_4, sizeof(REAL));
												stream->Write(&m_threshold_Lower_5, sizeof(REAL));

												stream->Write(&m_slope_Falloff_0, sizeof(RuTerrainUtility_FalloffType));
												stream->Write(&m_slope_Falloff_1, sizeof(RuTerrainUtility_FalloffType));
												stream->Write(&m_slope_Falloff_2, sizeof(RuTerrainUtility_FalloffType));
												stream->Write(&m_slope_Falloff_3, sizeof(RuTerrainUtility_FalloffType));
												stream->Write(&m_slope_Falloff_4, sizeof(RuTerrainUtility_FalloffType));
												stream->Write(&m_slope_Falloff_5, sizeof(RuTerrainUtility_FalloffType));

												stream->Write(&m_slope_Upper_0, sizeof(REAL));
												stream->Write(&m_slope_Upper_1, sizeof(REAL));
												stream->Write(&m_slope_Upper_2, sizeof(REAL));
												stream->Write(&m_slope_Upper_3, sizeof(REAL));
												stream->Write(&m_slope_Upper_4, sizeof(REAL));
												stream->Write(&m_slope_Upper_5, sizeof(REAL));

												stream->Write(&m_slope_Lower_0, sizeof(REAL));
												stream->Write(&m_slope_Lower_1, sizeof(REAL));
												stream->Write(&m_slope_Lower_2, sizeof(REAL));
												stream->Write(&m_slope_Lower_3, sizeof(REAL));
												stream->Write(&m_slope_Lower_4, sizeof(REAL));
												stream->Write(&m_slope_Lower_5, sizeof(REAL));

												stream->Write(&m_heightMultiplier, sizeof(REAL));
												stream->Write(&m_heightThreshold, sizeof(REAL));
												stream->Write(&m_heightOffset, sizeof(REAL));

												stream->Write(&m_brushCenterX, sizeof(REAL));
												stream->Write(&m_brushCenterY, sizeof(REAL));
												stream->Write(&m_brushLimit, sizeof(REAL));

												stream->Write(&m_paintCondition, sizeof(RuTerrainUtility_PaintCondition));
												stream->Write(&m_heightBooleanType, sizeof(RuTerrainUtility_BooleanType));
												stream->Write(&m_heightFalloffType, sizeof(RuTerrainUtility_FalloffType));
												stream->Write(&m_noiseFalloffType, sizeof(RuTerrainUtility_FalloffType));
											}

	inline REAL								EvaluateFalloff(RuTerrainUtility_FalloffType falloffType, REAL t, REAL range) const
											{
												switch(falloffType)
												{
													case ruTUFALLOFFTYPE_NONE:
														return RuFalloff_None(t, range);
														break;

													case ruTUFALLOFFTYPE_ALL:
														return RuFalloff_All(t, range);
														break;

													case ruTUFALLOFFTYPE_LINEAR:
														return RuFalloff_Linear(t, range);
														break;

													case ruTUFALLOFFTYPE_QUADRATIC:
														return RuFalloff_Quadratic(t, range);
														break;

													case ruTUFALLOFFTYPE_CUBIC:
														return RuFalloff_Cubic(t, range);
														break;

													case ruTUFALLOFFTYPE_QUARTIC:
														return RuFalloff_Quartic(t, range);
														break;

													case ruTUFALLOFFTYPE_QUINTIC:
														return RuFalloff_Quintic(t, range);
														break;

													case ruTUFALLOFFTYPE_BEVEL:
														return RuFalloff_Bevel(t, range);
														break;

													case ruTUFALLOFFTYPE_SMOOTHBEVEL:
														return RuFalloff_SmoothBevel(t, range);
														break;
												}

												return 1.0f;
											}
};

// ************************************************************************************************************************************************************

class CRuTerrainUtility_ChunkInfo
{
protected:
	CRuVector3*								m_normalField;
	BYTE*									m_autoPaintField;
	CRuVector3								m_origin;
	INT32									m_fieldWidth;
	REAL									m_gridSize;

public:
											CRuTerrainUtility_ChunkInfo(CRuVector3 *normalField, BYTE *autoPaintField, CRuVector3 &origin, INT32 fieldWidth, REAL gridSize);

	REAL									EvaluateSlope(const CRuVector3 &position, REAL range);
	BOOL									PaintOK(const CRuVector3 &position);
};

// ************************************************************************************************************************************************************

class CRuImageConvolver
{
protected:
	INT32									m_kernelSize;						// Convolution kernel size
	float*									m_kernel;							// Convolution kernel

	INT32									m_dimension;						// Dimensions of the image
	INT32									m_paddingSize;						// Padding size of the image
	INT32									m_realDimension;					// Real dimensions of the image data
	UINT8*									m_image;							// Image data
	UINT8*									m_workBuffer;						// Work buffer for convolution

public:
											CRuImageConvolver(INT32 kernelSize, float *kernel, INT32 imageSize);
	virtual									~CRuImageConvolver();

	void									ClearImageData();
	UINT8*									GetImageData();
	UINT8*									GetRawImageData();
	INT32									GetImagePitch();

	BOOL									Convolve();
};

// ************************************************************************************************************************************************************

class CRuSplatIDMap : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct FloodFillDesc
	{
		INT32								m_chunkKey;
		CRuSplatIDMap*						m_splatIDMap;
		CRuStack<INT32>						m_texelStack;
	};

	struct SplatLayerDesc
	{
		INT32								m_id;								//!< Texture ID
		UINT8*								m_map;								//!< Alpha map
	};

	INT32									m_mapType;
	INT32									m_dimension;						//!< Dimensions of the splat ID map
	INT32									m_splatLayerSize;					//!< Dimensions of each splat layer
	CRuRectangle							m_splatRect;						//!< Area of real space the splat covers

	INT32									m_texelAlterationIteration;
	UINT8*									m_texelAlterationMap;
	CRuArrayList<SplatLayerDesc>			m_splatLayers;

	// Currently constructed composition data
	BOOL									m_isFiltered;						// Splat map is filtered properly
	BOOL									m_isDirty;							// Splat map is dirty
	CRuArrayList<UINT16>					m_layerIDs;
	INT32									m_numAlphaMaps;
	CRuTexture_Proxy**						m_alphaMaps;

	// Temporaries for supporting multi-stage loading
	INT32									m_maximumAlphaMapSize;
	INT32									m_alphaMapDataVersion;
	CRuMemoryStream*						m_alphaMapData;

	// Other temporaries
	DWORD									m_lastCleanupTime;

public:
											CRuSplatIDMap();
	virtual									~CRuSplatIDMap();

	// IRuObject serialization
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);
	virtual BOOL							SerializeToAlphaMaps(IRuStream *stream);
	static BOOL								SkipStream(IRuStream *stream);

	BOOL									Initialize(INT32 dimension, const CRuRectangle &splatRect);
	BOOL									SetSplatRect(const CRuRectangle &splatRect);

	BOOL									FinishAlphaMapLoad();

	INT32									GetNumLayers() const;
	void									GetIDListAsString(char *splatIDListOut);
	UINT16									GetLayerID(INT32 layerIndex) const;
	INT32									GetNumAlphaMaps() const;
	IRuTexture*								GetAlphaMap(INT32 mapIndex);

	INT32									GetDimension() const;
	INT32									GetSplatLayerSize() const;

	// Conversion
	BOOL									ReduceSizeToPowerOfTwo();

	// Modification-related utility
	void									ClearTexelAlterationMap(INT32 iteration);
	BOOL									MarkAsDirty();
	BOOL									FreeAlphaMaps();

	// Internal utility
	INT32									GetSplatLayerIndex(INT32 splatID);
	void									SumSplatTexelToOne(INT32 excludedLayerIdx, INT32 texelIdx);
	void									RemoveUnusedSplatLayers();

	// Painting
	BOOL									PaintSplat(const CRuVector3 &center, REAL radius, REAL power, RuFalloffFunction falloffFunc, UINT16 splatID);
	BOOL									SprayPaintSplat(const CRuVector3 &center, float radius, REAL power, RuFalloffFunction falloffFunc, UINT16 splatID, float sprayProbability);
	BOOL									BlockFillSplat(const CRuVector3 &center, float radius, UINT16 splatID);
	BOOL									FloodFillSplat(const CRuVector3 &center, float radius, UINT16 splatID, INT32 chunkKey, RuWorld::GetNeighborSplat getSplatCallback);
	BOOL									SmudgeSplat(const CRuVector3 &center, float radius, float smudgeProbability);
	BOOL									ReplaceSplat(const CRuVector3 &center, float radius, UINT16 splatID);
	BOOL									AttractSplat(const CRuVector3 &center, float radius, UINT16 attractedSplatID, float attractProbability);
	UINT16									PickSplatID(const CRuVector3 &center);

	// Auto & fractal painting
	BOOL									MojoPaint(const CRuVector3 &center, float radius, CRuMojo_AttributeParcel<REAL> *refHF, CRuMojo_AttributeParcel<REAL> *chunkHF, CRuFractal_Base *srcFractal, const CRuSplatIDMap_PaintSetting &paintSetting);

	// Metrics
	virtual INT32							GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	// Static helpers
	static INT32							GetSplatIDMapSize(INT32 dimension);

	friend									CRuSplatMapBuilder;
};

// ************************************************************************************************************************************************************

class CRuSplatMapBuilder : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32									m_alphaMapDimension;				//


	INT32									m_maxLayers;
	CRuImageConvolver**						m_imageConvolvers;					// Image convolvers used for generating alpha maps

public:
											CRuSplatMapBuilder();
	virtual									~CRuSplatMapBuilder();

	BOOL									Initialize(INT32 alphaMapDimension);

	BOOL									BuildAlphaMaps(CRuSplatIDMap *splatIDMap, BOOL enableFilter = TRUE, RuSurfaceFormat surfaceFormat = ruSURFACEFMT_X1R5G5B5, BOOL useProxyTexture = FALSE);
};

// ************************************************************************************************************************************************************

class CRuTextureList : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct TextureDescriptor
	{
		// Persistent data
		char*								m_textureName;				//!< Texture resource name
		REAL								m_tileScale;				//!< Texture tiling scale

		// Runtime
		INT32								m_gcMarker;					//!< Garbage collection marker
		BOOL								m_dirty;					//!< Is the descriptor dirty?
		IRuBaseTexture*						m_texture;					//!< Instanced texture
	};

	BOOL									m_enableDelayedBinding;		//!< Enable delayed binding?
	BOOL									m_enableGarbageCollection;	//!< Enable garbage collection?

	CRuArrayList<TextureDescriptor>			m_textureList;				//!< Texture list

	INT32									m_currentGCCycle;			//!< Current GC cycle

public:
											CRuTextureList();
	virtual									~CRuTextureList();

	// IRuObject serialization
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	BOOL									Load(CRuDataStore_Base *dataStore);
	BOOL									Save(CRuDataStore_Base *dataStore, BOOL forceSave);

	// Settings
	void									EnableDelayedBinding(BOOL enableDelayedBinding);
	void									EnableGarbageCollection(BOOL enableGarbageCollection);

	// Texture management
	INT32									AddTexture(IRuBaseTexture *texture);
	INT32									GetNumTextures() const;
	IRuBaseTexture*							GetTexture(INT32 index);
	BOOL									SetTexture(INT32 index, IRuBaseTexture *texture);
	BOOL									SetTextureByName(INT32 index, const char *textureName);

	REAL									GetTileScale(INT32 index);
	void									SetTileScale(INT32 index, REAL tileScale);

	void									ClearTextureDirty();
	void									SetTextureDirty();
	BOOL									IsTextureDirty(INT32 index) const;

	BOOL									GarbageCollect();
};

// ************************************************************************************************************************************************************

extern INT32								RuTerrainUtility_BuildGridIndices(INT32 chunkDimension, INT32 gridDimension, INT32 vertexGap, INT32 stride, BYTE *vertexFlags, UINT16 *indicesOut, INT32 *numMainBodyIndices = NULL, BOOL ignoreSkirt = FALSE);
extern INT32								RuTerrainUtility_BuildGridIndicesEx(INT32 gridDimension, INT32 stride, INT32 pitch, INT32 vertexOffset, BYTE *vertexFlags, UINT16 *indicesOut);
extern INT32								RuTerrainUtility_BuildIndices_TileLink(INT32 linkMask, INT32 gridDimension, INT32 stride, INT32 pitch, INT32 originalPitch, BYTE *vertexFlags, UINT16 *indicesOut);
extern INT32								RuTerrainUtility_BuildIndices_Skirt(INT32 gridDimension, INT32 stride, INT32 pitch, INT32 originalPitch, BYTE *vertexFlags, UINT16 *indicesOut);

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_SplatIDMap, CRuSplatIDMap)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
