#ifndef _RUMESHBASE_H_
#define _RUMESHBASE_H_

// Rune system core
#include "../../Engine/Base/RuCore.h"
#include "../../Engine/Geometry/RuMeshCollision.h"

#include "../../ExtLink/RuExtLink_Granny.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

// triangle enumeration interface
class IRuTriangleEnumeration
{
public:
	virtual BOOL								EnumerateIntersectingTriangles(const CRuAABB &box, CRuArrayList<CRuVector3> &vertexList) = 0;
};

// ************************************************************************************************************************************************************

/*
	NOTES
	-----
	In terms of meshes, we take the approach of storing their data as a structure
	of arrays. Meshes cannot instance themselves. The interface provides accessors
	to the structure of arrays, which the mesh "instancer" can use to create
	the actual video/AGP memory mesh instance. All video/AGP memory management is
	left up to somebody else. This will help keep the mesh interface tidy, and any
	inheritor of the IRuMesh interface only needs to provide the minimal accessors
	to work with the existing renderer, without modification. No worries about
	keeping track of things when the device gets lost, either. We just reinstance
	everything on render automatically since the cache gets flushed.
*/

class IRuMorphTarget : public IRuObject
{
	ruRTTI_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual										~IRuMorphTarget();

	// Data array accessors
	virtual CRuVector3*							GetPosition() = 0;										// Retrieves the vertex positions array
	virtual CRuVector3*							GetNormal() = 0;										// Retrieves the vertex normals array
	virtual DWORD*								GetDiffuse() = 0;										// Retrieves the vertex diffuse colors array
	virtual DWORD*								GetSpecular() = 0;										// Retrieves the vertex specular colors array
	virtual float*								GetBlendWeight() = 0;									// Retrieves the vertex blend weights array
	virtual UINT16*								GetBlendIndex() = 0;									// Gets the bone blending indices

	virtual float*								GetTextureCoordinate(RuTextureChannel type) = 0;		// Retrieves the vertex texture coordinates array for the given texture type
	virtual float*								GetTextureCoordinateByIndex(INT32 index) = 0;

	virtual const CRuVector3*					GetPosition() const = 0;								// Retrieves the vertex positions array
	virtual const CRuVector3*					GetNormal() const = 0;									// Retrieves the vertex normals array
	virtual const DWORD*						GetDiffuse() const = 0;									// Retrieves the vertex diffuse colors array
	virtual const DWORD*						GetSpecular() const = 0;								// Retrieves the vertex specular colors array
	virtual const float*						GetBlendWeight() const = 0;								// Retrieves the vertex blend weights array
	virtual const UINT16*						GetBlendIndex() const = 0;								// Gets the bone blending indices

	virtual INT32								GetNumTextureCoordinates() const = 0;
	virtual const float*						GetTextureCoordinate(RuTextureChannel type) const = 0;	// Retrieves the vertex texture coordinates array for the given texture type
																										//   (Up to four floats per vertex, may be NULL)
	virtual const float*						GetTextureCoordinateByIndex(INT32 index) const = 0;
	virtual const RuTextureChannel				GetTextureChannelByIndex(INT32 index) const = 0;
};

// ************************************************************************************************************************************************************

class IRuMeshTopology : public IRuObject
{
	ruRTTI_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual										~IRuMeshTopology();

	// Iteration interface
	virtual INT32								GetIteration() const = 0;
	virtual INT32								IncrementIteration() = 0;

	virtual RuPrimitiveType						GetPrimitiveType() const = 0;							// Gets the type of primitive stored by the mesh
	virtual INT32								GetNumPrimitives() const = 0;							// Gets the number of primitives in the mesh
	virtual INT32								GetNumIndices() const = 0;								// Gets the number of indices in the mesh

	virtual UINT16*								GetIndices() = 0;
	virtual const UINT16*						GetIndices() const = 0;

	// Batch interface
	virtual INT32								AddBatch(INT32 offset, INT32 numPrimitives, UINT16 *boneMap, INT32 boneMapSize) = 0;
	virtual BOOL								SetBatch(INT32 index, INT32 offset, INT32 numPrimitives) = 0;
	virtual	void								ClearBatches() = 0;
	virtual BOOL								GetBatch(INT32 index, INT32 &offsetOut, INT32 &numPrimitivesOut) const = 0;
	virtual CRuParameterBlock*					GetBatchParameterBlock(INT32 index) = 0;
};

// ************************************************************************************************************************************************************

class IRuMesh : public IRuObject
{
	ruRTTI_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual										~IRuMesh();

	// Mesh properties
	virtual INT32								GetMeshIteration() const = 0;
	virtual BOOL								IsMeshDynamic() const = 0;								// Is the mesh a dynamic mesh?
																										//   IMPLICATION: If a mesh is dynamic, then shaders must not allocate
																										//   a cached version of the mesh, since the mesh changes every frame.
																										//   Instead, the mesh must be re-instanced every frame into dynamic
																										//   device buffers.

	virtual INT32								GetTopologyIteration() const = 0;
	virtual BOOL								IsTopologyDynamic() const = 0;							// Is the topology a dynamic topology?

	// Morph target interface
	virtual INT32								GetNumMorphTargets() const = 0;
	virtual IRuMorphTarget*						GetMorphTarget(INT32 morphTargetIndex) = 0;
	virtual BOOL								SetMorphCurveTime(REAL morphCurveTime) = 0;

	// Mesh topology
	virtual IRuMeshTopology*					GetMeshTopology() = 0;
	virtual const IRuMeshTopology*				GetMeshTopology() const = 0;

	// Data array accessors
	virtual const UINT16*						GetIndices() const = 0;									// Gets the index array if available

	// Batch interface
	virtual BOOL								GetBatch(INT32 index, INT32 &offsetOut, INT32 &numPrimitivesOut) const = 0;
	virtual CRuParameterBlock*					GetBatchParameterBlock(INT32 index) = 0;

	// Geometry information
	virtual RuPrimitiveType						GetPrimitiveType() const = 0;							// Gets the type of primitive stored by the mesh
	virtual INT32								GetNumPrimitives() const = 0;							// Gets the number of primitives in the mesh
	virtual INT32								GetNumVertices() const = 0;								// Gets the number of vertices in the mesh
	virtual INT32								GetNumIndices() const = 0;								// Gets the number of indices in the mesh
	virtual const CRuSphere&					GetBounds() const = 0;									// Gets the bounds for the mesh

	virtual DWORD								GetVertexFormat() const = 0;

	// Collision interface
	virtual BOOL								QueryCollision(CRuCollisionQuery *colQuery) = 0;		// Query for collision
};

// ************************************************************************************************************************************************************

class CRuMorphTarget : public IRuMorphTarget
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32										m_iterationNumber;

	DWORD										m_vertexFormat;											//!< Vertex format
	INT32										m_vertexCount;											//!< Number of vertices

	CRuVector3*									m_position;												//!< Position array
	CRuVector3*									m_normal;												//!< Normal array
	DWORD*										m_diffuse;												//!< Diffuse array
	DWORD*										m_specular;												//!< Specular array
	float*										m_blendWeight;											//!< Blend weights array
	UINT16*										m_blendIndex;											//!< Blend indices array

	CRuArrayList<RuTextureChannel>				m_textureChannels;										//!< Texture channels list
	CRuArrayList<float *>						m_textureCoordinates;									//!< Texture coordinates list

public:
												CRuMorphTarget();
	virtual										~CRuMorphTarget();

	BOOL										Initialize(INT32 numVertices, DWORD vertexFormat);		//
	BOOL										SetVertexFormat(DWORD vertexFormat);

	// IRuObject serialization
	virtual IRuObject*							Clone(IRuObject *clonedObject = NULL);
	virtual BOOL								SerializeFrom(IRuStream *stream);
	virtual BOOL								SerializeTo(IRuStream *stream);

	// Memory reporting
	virtual INT32								GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	// Data array accessors
	virtual CRuVector3*							GetPosition();
	virtual CRuVector3*							GetNormal();
	virtual DWORD*								GetDiffuse();
	virtual DWORD*								GetSpecular();
	virtual float*								GetBlendWeight();
	virtual UINT16*								GetBlendIndex();
	virtual float*								GetTextureCoordinate(RuTextureChannel type);
	virtual float*								GetTextureCoordinateByIndex(INT32 index);

	virtual const CRuVector3*					GetPosition() const;
	virtual const CRuVector3*					GetNormal() const;
	virtual const DWORD*						GetDiffuse() const;
	virtual const DWORD*						GetSpecular() const;
	virtual const float*						GetBlendWeight() const;
	virtual const UINT16*						GetBlendIndex() const;
	virtual INT32								GetNumTextureCoordinates() const;
	virtual const float*						GetTextureCoordinate(RuTextureChannel type) const;
	virtual const float*						GetTextureCoordinateByIndex(INT32 index) const;
	virtual const RuTextureChannel				GetTextureChannelByIndex(INT32 index) const;

	BOOL										AddTextureCoordinate(RuTextureChannel type);

	BOOL										CopyFrom(IRuMorphTarget *morphTarget);
};

// ************************************************************************************************************************************************************

class CRuMeshTopology : public IRuMeshTopology
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	struct BatchInfo
	{
		INT32									m_offset;													// Offset of the batch, in number of primitives
		INT32									m_numPrimitives;											// Size of the batch
		UINT16*									m_boneMap;
		INT32									m_boneMapSize;												//!< Size of the bone remapping array
		CRuParameterBlock						m_parameterBlock;
		
		BatchInfo()
		:	m_offset(0),
			m_numPrimitives(0),
			m_boneMap(NULL),
			m_boneMapSize(0)
		{
		}

		~BatchInfo()
		{
			delete[] m_boneMap;
		}
	};

	INT32										m_iteration;

	RuPrimitiveType								m_primitiveType;
	INT32										m_numPrimitives;
	INT32										m_numIndices;												// Number of faces
	UINT16*										m_indices;

	// Batch bookkeeping
	CRuArrayList<BatchInfo *>					m_batches;													// List of batches

public:
												CRuMeshTopology();
												CRuMeshTopology(RuPrimitiveType primType, INT32 numPrimitives);
	virtual										~CRuMeshTopology();

	BOOL										Initialize(RuPrimitiveType primType, INT32 numPrimitives);

	// IRuObject serialization
	virtual IRuObject*							Clone(IRuObject *clonedObject = NULL);

	// Memory reporting
	virtual INT32								GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	// Iteration interface
	virtual INT32								GetIteration() const;
	virtual INT32								IncrementIteration();

	virtual RuPrimitiveType						GetPrimitiveType() const;									// Gets the type of primitive stored by the mesh
	virtual INT32								GetNumPrimitives() const;									// Gets the number of primitives in the mesh
	virtual INT32								GetNumIndices() const;										// Gets the number of indices in the mesh

	virtual UINT16*								GetIndices();
	virtual const UINT16*						GetIndices() const;

	// Batch interface
	virtual INT32								AddBatch(INT32 offset, INT32 numPrimitives, UINT16 *boneMap, INT32 boneMapSize);
	virtual BOOL								SetBatch(INT32 index, INT32 offset, INT32 numPrimitives);
	virtual	void								ClearBatches();
	virtual BOOL								GetBatch(INT32 index, INT32 &offsetOut, INT32 &numPrimitivesOut) const;
	virtual CRuParameterBlock*					GetBatchParameterBlock(INT32 index);
};

// ************************************************************************************************************************************************************

class CRuMesh : public IRuMesh
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32										m_meshIterationNumber;

	INT32										m_numVertices;												// Number of vertices
	DWORD										m_vertexFormat;												// Vertex format used by the mesh

	CRuArrayList<CRuMorphTarget *>				m_morphTargets;												// Morph target array list

	IRuMeshTopology*							m_meshTopology;

	// Bounds
	CRuSphere									m_bounds;

	// Collision structure
	IRuCollisionStructure*						m_collisionStructure;

public:
												CRuMesh();
	virtual										~CRuMesh();

	BOOL										Initialize(RuPrimitiveType primitiveType, INT32 numVertices, INT32 numPrimitives, DWORD vertexFormat, INT32 numMorphTargets = 1);

	// IRuObject serialization
	virtual IRuObject*							Clone(IRuObject *clonedObject = NULL);
	virtual BOOL								SerializeFrom(IRuStream *stream);
	virtual BOOL								SerializeTo(IRuStream *stream);

	// Memory reporting
	virtual INT32								GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	// Mesh properties
	virtual INT32								GetMeshIteration() const;
	virtual BOOL								IsMeshDynamic() const;
	INT32										IncrementMeshIteration();

	virtual INT32								GetTopologyIteration() const;
	virtual BOOL								IsTopologyDynamic() const;
	INT32										IncrementTopologyIteration();

	// Morph target interface
	virtual INT32								GetNumMorphTargets() const;
	virtual IRuMorphTarget*						GetMorphTarget(INT32 morphTargetIndex);
	virtual BOOL								SetMorphCurveTime(REAL morphCurveTime);

	// Mesh topology
	virtual IRuMeshTopology*					GetMeshTopology();
	virtual const IRuMeshTopology*				GetMeshTopology() const;
	void										OverrideMeshTopology(IRuMeshTopology *meshTopology);

	// Data array accessors
	virtual const UINT16*						GetIndices() const;

	UINT16*										GetIndices();

	BOOL										AddTextureCoordinate(RuTextureChannel type);

	// Batch interface
	INT32										AddBatch(INT32 offset, INT32 numPrimitives);
	void										ClearBatches();
	virtual BOOL								GetBatch(INT32 index, INT32 &offsetOut, INT32 &numPrimitivesOut) const;
	virtual CRuParameterBlock*					GetBatchParameterBlock(INT32 index);

	// Geometry information
	virtual RuPrimitiveType						GetPrimitiveType() const;
	virtual INT32								GetNumPrimitives() const;
	virtual INT32								GetNumVertices() const;
	virtual INT32								GetNumIndices() const;
	virtual const CRuSphere&					GetBounds() const;

	virtual DWORD								GetVertexFormat() const;

	// Collision interface
	IRuCollisionStructure*						GetCollisionStructure();
	BOOL										SetCollisionStructure(IRuCollisionStructure *collisionStructure);
	virtual BOOL								QueryCollision(CRuCollisionQuery *colQuery);

	// Primitive type setting and other modifications
	void										CalculateBounds();
	void										SetBounds(const CRuSphere &bounds);

	// Skinned mesh utility
	BOOL										SkinnedMesh_IsSkinned();
	INT32										SkinnedMesh_GetNumBones();
	void										SkinnedMesh_Split(INT32 maxBones);
	void										SkinnedMesh_Subdivide(INT32 maxBones, CRuArrayList<IRuMesh *> *subMeshListOut);

	BOOL										SkinnedMesh_GenerateBoneUsageMap(CRuBitField &boneUsageMap);
	void										SkinnedMesh_GenerateBoneMap(CRuArrayList<UINT8> &boneMap);

	// Mesh subset utility
	BOOL										MeshUtility_MeshCopy(IRuMesh *srcMesh);
	BOOL										MeshUtility_DataSwap(CRuMesh *srcMesh);
	BOOL										MeshUtility_CopyMeshSubset(const CRuBitField &copyMask, CRuMesh *dstMesh, INT32 vertexOffset, INT32 indexOffset, INT32 &verticesCopiedOut, INT32 &indicesCopiedOut);
	BOOL										MeshUtility_RemapBones(INT32 startVertex, INT32 vertexCount, const UINT16 *boneMap);
};

// ************************************************************************************************************************************************************

class CRuSkinnableMesh : public IRuMesh
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Options
	BOOL										m_ignoreNormalTransforms;

	IRuMesh*									m_srcMesh;											// Source mesh from which this mesh is skinned from

	INT32										m_numVertexGroups;
	INT32										m_cachedSrcIteration;								// Iteration number of the cached SSE vertex-normal arrays
	__m128*										m_cachedX;
	__m128*										m_cachedY;
	__m128*										m_cachedZ;
	__m128*										m_cachedNX;
	__m128*										m_cachedNY;
	__m128*										m_cachedNZ;

	UINT16*										m_cachedBlendIndex;
	float*										m_cachedBlendWeight;

	INT32										m_cachedBoneMatrixArraySize;
	__m128*										m_cachedBoneMatrixArray;

	INT32										m_iterationNumber;
	CRuMorphTarget*								m_skinnedMT;										// Skinned morph target object

	BOOL										m_setPoseEnabled;

public:
												CRuSkinnableMesh();
	virtual										~CRuSkinnableMesh();

	void										EnableSetPose();

	void										SetSourceMesh(IRuMesh *srcMesh);
	void										SetPose(CRuMatrix4x4 *skinMatrices, INT32 numSkinMatrices);

	// Mesh properties
	virtual INT32								GetMeshIteration() const;
	virtual BOOL								IsMeshDynamic() const;								// Is the mesh a dynamic mesh?

	virtual INT32								GetTopologyIteration() const;
	virtual BOOL								IsTopologyDynamic() const;							// Is the topology a dynamic topology?

	// Morph target interface
	virtual INT32								GetNumMorphTargets() const;
	virtual IRuMorphTarget*						GetMorphTarget(INT32 morphTargetIndex);
	virtual BOOL								SetMorphCurveTime(REAL morphCurveTime);

	// Data array accessors
	virtual const UINT16*						GetIndices() const;									// Gets the index array if available

	// Batch interface
	virtual BOOL								GetBatch(INT32 index, INT32 &offsetOut, INT32 &numPrimitivesOut) const;
	virtual CRuParameterBlock*					GetBatchParameterBlock(INT32 index);

	// Geometry information
	virtual RuPrimitiveType						GetPrimitiveType() const;							// Gets the type of primitive stored by the mesh
	virtual INT32								GetNumPrimitives() const;							// Gets the number of primitives in the mesh
	virtual INT32								GetNumVertices() const;								// Gets the number of vertices in the mesh
	virtual INT32								GetNumIndices() const;								// Gets the number of indices in the mesh
	virtual const CRuSphere&					GetBounds() const;									// Gets the bounding box for the mesh

	virtual DWORD								GetVertexFormat() const;

	// Collision interface
	virtual BOOL								QueryCollision(CRuCollisionQuery *colQuery);

protected:
	void										FreeResources();
};

// ************************************************************************************************************************************************************

class CRuDynamicMesh : public CRuMesh
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	BOOL										m_forceStaticPool;									//!< Forces mesh into static mesh pool if TRUE

	INT32										m_actualNumVertices;								//!< Number of vertices actually used
	INT32										m_actualNumIndices;									//!< Number of primitives actually used

public:
												CRuDynamicMesh(BOOL forceStaticPool = FALSE);
	virtual										~CRuDynamicMesh();

	// IRuObject interface
	virtual IRuObject*							Clone(IRuObject *clonedObject = NULL);

	// Mesh properties
	virtual BOOL								IsMeshDynamic() const;
	virtual BOOL								IsTopologyDynamic() const;

	// Geometry information
	INT32										GetMaxPrimitives() const;
	INT32										GetMaxVertices() const;
	INT32										GetMaxIndices() const;

	virtual INT32								GetNumPrimitives() const;
	virtual INT32								GetNumVertices() const;
	virtual INT32								GetNumIndices() const;

	BOOL										SetNumPrimitives(INT32 numPrimitives);
	BOOL										SetNumVertices(INT32 numVertices);
};

// ************************************************************************************************************************************************************

class CRuMorphingMesh : public CRuMesh
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	REAL										m_curveTimeScale;

	granny_curve2**								m_positionCurves;
	granny_curve2**								m_normalCurves;
	granny_curve2**								m_diffuseCurves;

	CRuArrayList<RuTextureChannel>				m_textureChannels;										//!< Texture channels list
	CRuArrayList<granny_curve2**>				m_textureCurves;										//!< Texture coordinates list

public:
												CRuMorphingMesh();
	virtual										~CRuMorphingMesh();

	// IRuObject serialization
	virtual BOOL								SerializeFrom(IRuStream *stream);
	virtual BOOL								SerializeTo(IRuStream *stream);

	BOOL										SetCurveTimeScale(REAL curveTimeScale);

	BOOL										SetPositionCurve(INT32 vertexIndex, granny_curve2 *curve);
	BOOL										SetNormalCurve(INT32 vertexIndex, granny_curve2 *curve);
	BOOL										SetDiffuseCurve(INT32 vertexIndex, granny_curve2 *curve);
	BOOL										SetTextureCurve(RuTextureChannel texChannel, INT32 vertexIndex, granny_curve2 *curve);

	// Mesh properties
	virtual BOOL								IsMeshDynamic() const;

	// Morph target controls
	virtual BOOL								SetMorphCurveTime(REAL morphCurveTime);

protected:
	CRuMemoryStream*							ConvertCurveToStream(granny_curve2 *curve);
	granny_curve2*								ConvertStreamToCurve(CRuMemoryStream *stream);
};

// ************************************************************************************************************************************************************

class CRuProxyMesh : public IRuMesh
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Original mesh to act as a proxy for
	IRuMesh*									m_mesh;												// Mesh for which this object is a proxy of
	INT32										m_proxiedBatchIndex;								// Batch index to proxy for

	INT32										m_proxiedBatchOffset;								// Offset of the batch, in number of primitives
	INT32										m_proxiedBatchNumPrimitives;						// Size of the batch
	CRuParameterBlock*							m_proxiedBatchParameterBlock;

	BOOL										m_useProxiedMeshBounds;
	CRuSphere									m_proxiedBatchBounds;

public:
												CRuProxyMesh();
	virtual										~CRuProxyMesh();

	BOOL										SetProxiedMesh(IRuMesh *proxiedMesh, INT32 proxiedBatchIndex);
	BOOL										SetProxiedMesh(IRuMesh *proxiedMesh, INT32 proxiedBatchOffset, INT32 proxiedBatchNumPrimitives);
	INT32										GetProxiedBatchIndex() const;

	// Memory reporting
	virtual INT32								GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	virtual const CRuGUID&						GetGUID() const;									// Override the GUID retrieval function

	// Mesh properties
	virtual INT32								GetMeshIteration() const;
	virtual BOOL								IsMeshDynamic() const;								// Is the mesh a dynamic mesh?

	virtual INT32								GetTopologyIteration() const;
	virtual BOOL								IsTopologyDynamic() const;							// Is the topology a dynamic topology?

	// Morph target interface
	virtual INT32								GetNumMorphTargets() const;
	virtual IRuMorphTarget*						GetMorphTarget(INT32 morphTargetIndex);
	virtual BOOL								SetMorphCurveTime(REAL morphCurveTime);

	// Mesh topology
	virtual IRuMeshTopology*					GetMeshTopology();
	virtual const IRuMeshTopology*				GetMeshTopology() const;

	// Data array accessors
	virtual const UINT16*						GetIndices() const;									// Gets the index array if available

	// Batch interface
	virtual BOOL								GetBatch(INT32 index, INT32 &offsetOut, INT32 &numPrimitivesOut) const;
	virtual CRuParameterBlock*					GetBatchParameterBlock(INT32 index);

	// Geometry information
	virtual RuPrimitiveType						GetPrimitiveType() const;							// Gets the type of primitive stored by the mesh
	virtual INT32								GetNumPrimitives() const;							// Gets the number of primitives in the mesh
	virtual INT32								GetNumVertices() const;								// Gets the number of vertices in the mesh
	virtual INT32								GetNumIndices() const;								// Gets the number of indices in the mesh
	virtual const CRuSphere&					GetBounds() const;									// Gets the bounding box for the mesh

	virtual DWORD								GetVertexFormat() const;

	// Collision interface
	virtual BOOL								QueryCollision(CRuCollisionQuery *colQuery);

	void										SetBounds(const CRuSphere &bounds);
};

// ************************************************************************************************************************************************************

class CRuSkinMap : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuBitField									m_boneUsageMap;
	UINT16*										m_boneMap;
	UINT16*										m_invBoneMap;

	INT32										m_usedBoneCount;

public:
												CRuSkinMap();
	virtual										~CRuSkinMap();

	BOOL										Initialize(IRuMesh *mesh);
	BOOL										Merge(CRuSkinMap *srcSkinMap, INT32 boneLimit = 0);
	BOOL										Split(INT32 boneLimit, CRuArrayList<CRuSkinMap *> &skinMapsOut);

	IRuMesh*									GenerateRemappedMesh(IRuMesh *mesh);

	INT32										GetBoneCount();
	INT32										GetUsedBoneCount();

protected:
	BOOL										HasSkinningData(IRuMesh *mesh);
	INT32										GetBoneCount(IRuMesh *mesh);

	BOOL										UpdateBoneMap();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_MorphTarget, CRuMorphTarget)
ruOBJECTMODULE_DEFINE(CRuObjectModule_Mesh, CRuMesh)
ruOBJECTMODULE_DEFINE(CRuObjectModule_DynamicMesh, CRuDynamicMesh)
ruOBJECTMODULE_DEFINE(CRuObjectModule_MorphingMesh, CRuMorphingMesh)

// ************************************************************************************************************************************************************

inline IRuMorphTarget::~IRuMorphTarget()
{
}

// ************************************************************************************************************************************************************

inline IRuMeshTopology::~IRuMeshTopology()
{
}

// ************************************************************************************************************************************************************

inline IRuMesh::~IRuMesh()
{
}

// ************************************************************************************************************************************************************

inline CRuVector3 *CRuMorphTarget::GetPosition()
{
	return m_position;
}

inline CRuVector3 *CRuMorphTarget::GetNormal()
{
	return m_normal;
}

inline DWORD *CRuMorphTarget::GetDiffuse()
{
	return m_diffuse;
}

inline DWORD *CRuMorphTarget::GetSpecular()
{
	return m_specular;
}

inline float *CRuMorphTarget::GetBlendWeight()
{
	return m_blendWeight;
}

inline UINT16 *CRuMorphTarget::GetBlendIndex()
{
	return m_blendIndex;
}

inline float *CRuMorphTarget::GetTextureCoordinate(RuTextureChannel type)
{
	if(m_textureChannels.Contains(type))
	{
		return m_textureCoordinates[m_textureChannels.IndexOf(type)];
	}

	return NULL;
}

inline float *CRuMorphTarget::GetTextureCoordinateByIndex(INT32 index)
{
	return m_textureCoordinates[index];
}

inline const CRuVector3 *CRuMorphTarget::GetPosition() const
{
	return m_position;
}

inline const CRuVector3 *CRuMorphTarget::GetNormal() const
{
	return m_normal;
}

inline const float *CRuMorphTarget::GetBlendWeight() const
{
	return m_blendWeight;
}

inline const UINT16 *CRuMorphTarget::GetBlendIndex() const
{
	return m_blendIndex;
}

inline const DWORD *CRuMorphTarget::GetDiffuse() const
{
	return m_diffuse;
}

inline const DWORD *CRuMorphTarget::GetSpecular() const
{
	return m_specular;
}

inline INT32 CRuMorphTarget::GetNumTextureCoordinates() const
{
	return m_textureCoordinates.Count();
}

inline const float *CRuMorphTarget::GetTextureCoordinate(RuTextureChannel type) const
{
	if(m_textureChannels.Contains(type))
	{
		return m_textureCoordinates[m_textureChannels.IndexOf(type)];
	}

	return NULL;
}

inline const float *CRuMorphTarget::GetTextureCoordinateByIndex(INT32 index) const
{
	return m_textureCoordinates[index];
}

inline const RuTextureChannel CRuMorphTarget::GetTextureChannelByIndex(INT32 index) const
{
	return m_textureChannels[index];
}

// ************************************************************************************************************************************************************

inline INT32 CRuMesh::GetMeshIteration() const
{
	return m_meshIterationNumber;
}

inline BOOL CRuMesh::IsMeshDynamic() const
{
	return FALSE;
}

inline INT32 CRuMesh::IncrementMeshIteration()
{
	return ++m_meshIterationNumber;
}

inline INT32 CRuMesh::GetTopologyIteration() const
{
	return m_meshTopology->GetIteration();
}

inline BOOL CRuMesh::IsTopologyDynamic() const
{
	return FALSE;
}

inline INT32 CRuMesh::IncrementTopologyIteration()
{
	return m_meshTopology->IncrementIteration();
}

inline INT32 CRuMesh::GetNumMorphTargets() const
{
	return m_morphTargets.Count();
}

inline IRuMorphTarget *CRuMesh::GetMorphTarget(INT32 morphTargetIndex)
{
	if(morphTargetIndex < m_morphTargets.Count())
		return m_morphTargets[morphTargetIndex];

	return NULL;
}

inline BOOL CRuMesh::SetMorphCurveTime(REAL morphCurveTime)
{
	return TRUE;
}

inline const UINT16 *CRuMesh::GetIndices() const
{
	return m_meshTopology->GetIndices();
}

inline UINT16 *CRuMesh::GetIndices()
{
	return m_meshTopology->GetIndices();
}

inline RuPrimitiveType CRuMesh::GetPrimitiveType() const
{
	return m_meshTopology->GetPrimitiveType();
}

inline INT32 CRuMesh::GetNumPrimitives() const
{
	return m_meshTopology->GetNumPrimitives();
}

inline INT32 CRuMesh::GetNumVertices() const
{
	return m_numVertices;
}

inline INT32 CRuMesh::GetNumIndices() const
{
	return m_meshTopology->GetNumIndices();
}

inline const CRuSphere &CRuMesh::GetBounds() const
{
	return m_bounds;
}

inline DWORD CRuMesh::GetVertexFormat() const
{
	return m_vertexFormat;
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
