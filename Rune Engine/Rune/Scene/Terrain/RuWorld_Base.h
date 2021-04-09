#ifndef _RUWORLD_BASE_H_
#define _RUWORLD_BASE_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Gizmo/RuGizmo_Box.h"
#include "../../Scene/Gizmo/RuGizmo_Plane.h"
#include "../../PathEngine/RuPathHierarchy_DS.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

namespace RuWorld
{
	enum TriggerType
	{
		ruTRIGGERTYPE_ZONE					= 0,								//!< Zone trigger
		ruTRIGGERTYPE_SCRIPT				= 1,								//!< Script trigger
		ruTRIGGERTYPE_WARP					= 2,								//!< Warp/teleport trigger
		ruTRIGGERTYPE_ZONETYPE				= 3,								//!< Zone type trigger
		ruTRIGGERTYPE_DEADZONE				= 4,								//!< Dead zone trigger
		ruTRIGGERTYPE_SAFEZONE				= 5,								//!< Safe zone trigger
		ruTRIGGERTYPE_MUSIC					= 6,								//!< Music trigger
		ruTRIGGERTYPE_RANDOMAMBIENCE		= 7,								//!< Random ambience trigger
		ruTRIGGERTYPE_AMBIENCE				= 8,								//!< Ambience trigger
		ruTRIGGERTYPE_SUPPLEMENTAMBIENCE	= 9,								//!< Supplement ambience trigger
		ruTRIGGERTYPE_3DAMBIENCE			= 10,								//!< 3D ambience trigger, acts like a point sound source which attenuates with distance

		ruTRIGGERTYPE_FORCE_DWORD			= 0x7FFFFFFF
	};

	// ************************************************************************************************************************************************************

	enum TriangleFlags
	{
		ruTRIFLAGS_WALKABLE					= 0x00100000,				//!< Walkable triangle
		ruTRIFLAGS_WATER					= 0x00200000,				//!< Water surface
		ruTRIFLAGS_LOCKED					= 0x00400000,				//!< Triangle is locked
		ruTRIFLAGS_FOLIAGE					= 0x00800000,				//!< Foliage triangle
		ruTRIFLAGS_EXTENTMARKER				= 0x01000000,				//!< Water extent marker

		ruTRIFLAGS_FORCE_DWORD				= 0x7FFFFFFF
	};

	// ************************************************************************************************************************************************************

	enum WaterBodyType
	{
		ruWATERBODYTYPE_INVALID						= 0x7FFFFFFF,					//!< Invalid water body
		ruWATERBODYTYPE_NORMAL						= 0,							//!< Normal water body
		ruWATERBODYTYPE_EXTENTMARKER				= 1,							//!< Water body extents marker (normally invisible)
		ruWATERBODYTYPE_SOLID						= 2,							//!< Solid water body (lava, ice)

		ruWATERBODYTYPE_FORCE_DWORD					= 0x7FFFFFFF
	};

	// ************************************************************************************************************************************************************

	enum DetailClass
	{
		ruDETAILCLASS_NORMAL				= 0,
		ruDETAILCLASS_DECORATIVE			= 1,

		ruDETAILCLASS_FORCE_DWROD			= 0x7FFFFFFF
	};

	// ************************************************************************************************************************************************************

	enum ZoneType
	{
		ruZONETYPE_OUTDOOR					= 0,
		ruZONETYPE_INDOOR					= 1,
		ruZONETYPE_EXTENSION				= 2,

		ruZONETYPE_FORCE_DWORD				= 0x7FFFFFFF
	};
}

// ************************************************************************************************************************************************************

inline INT32 RuWorld_ZoneFlags_GetID(UINT32 zoneFlags)
{
	return zoneFlags & 0x000FFFFF;
}

// ************************************************************************************************************************************************************

inline UINT32 RuWorld_ZoneFlags_SetID(UINT32 zoneFlags, INT32 zoneID)
{
	return (zoneFlags & ~0x000FFFFF) | (zoneID & 0x000FFFFF);
}

// ************************************************************************************************************************************************************

class CRuWorld_ChunkTree : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct ChunkDescriptor
	{
	public:
		INT32								m_x;
		INT32								m_z;
		CRuAABB								m_bounds;
	};

	struct ChunkTreeNode
	{
	public:
		CRuAABB								m_bounds;						// Bounds of the chunk tree node

		INT32								m_chunkArraySize;				// Maximum number of chunks allowed at this node
		INT32								m_numChunks;					// Number of chunks at this node
		ChunkDescriptor*					m_chunks;						// Array of ChunkDescriptors

		ChunkTreeNode*						m_children[4];					// Children of the chunk tree node

	public:
											ChunkTreeNode();
											~ChunkTreeNode();

		BOOL								IsLeaf();
	};

	ChunkTreeNode*							m_root;							//!< Root of the chunk tree
	INT32									m_chunksPerNode;				//!< Maximum number of chunks per node

public:
											CRuWorld_ChunkTree();
	virtual									~CRuWorld_ChunkTree();

	BOOL									Initialize(const CRuAABB &treeBounds, INT32 chunksPerNode);

	BOOL									InsertChunk(INT32 x, INT32 z, const CRuAABB &bounds);

	// Enumeration
	void									EnumerateChunks(const CRuPlane4 *frustumPlanes, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut = NULL);
	void									EnumerateChunks(const CRuAABB &box, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut = NULL);
	void									EnumerateChunks(const CRuOBB &box, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut = NULL);
	void									EnumerateChunks(const CRuSphere &sphere, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut = NULL);
	void									EnumerateChunks(const CRuRay3 &ray, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut = NULL);
	void									EnumerateChunks(const CRuSegment3 &segment, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut = NULL);

	// Picking
	BOOL									PickNearestChunk(const CRuSegment3 &segment, DWORD &chunkKeyOut, CRuAABB &chunkBoundsOut);

	// Memory reporting
	virtual INT32							GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

protected:
	BOOL									InsertChunk(ChunkTreeNode *node, INT32 x, INT32 z, const CRuAABB &bounds);
	BOOL									Subdivide(ChunkTreeNode *node);

	void									EnumerateChunks(ChunkTreeNode *node, const CRuPlane4 *frustumPlanes, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut);
	void									EnumerateChunks(ChunkTreeNode *node, const CRuAABB &box, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut);
	void									EnumerateChunks(ChunkTreeNode *node, const CRuOBB &box, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut);
	void									EnumerateChunks(ChunkTreeNode *node, const CRuSphere &sphere, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut);
	void									EnumerateChunks(ChunkTreeNode *node, const CRuRay3 &ray, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut);
	void									EnumerateChunks(ChunkTreeNode *node, const CRuSegment3 &segment, CRuArrayList<DWORD> &chunkKeyOut, CRuArrayList<CRuAABB> *chunkBoundsOut);

	void									PickNearestChunk(ChunkTreeNode *node, const CRuSegment3 &segment, DWORD &chunkKeyOut, CRuAABB &chunkBoundsOut, REAL &distanceOut);

	INT32									GetNodeMemoryFootprint(ChunkTreeNode *node);
};

// ************************************************************************************************************************************************************

/*!
	CRuWorld_EntityDescriptor
	@author John Tang
	@version 2008.09.10

	Stores information about entities to be instanced in the world.

	The object's properties array contains the following additional properties:

	Property Name                                                Semantics                      Description
	-------------------------------------------------------------------------------------------------------------------------------------
	CRuWorld_EntityDescriptor:Special Event                      const char *                   Used to classify the special event this entity is used for (e.g. christmas, halloween)
	CRuWorld_EntityDescriptor:Locked                             BOOL                           Is the object locked? If locked, it may not be moved.

*/

class CRuWorld_EntityDescriptor : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	// Identifying data
	char*									m_resourceName;						//!< Resource name of the entity
	UINT32									m_zoneFlags;						//!< Zone flags -- see RuGaia_Base for details

	// Options
	RuWorld::DetailClass					m_detailClass;						//!< Detail class of the entity
	RuWorld::ZoneType						m_zoneType;							//!< Zone type of the entity

	// Visibility chain
	CRuGUID									m_visibilityChainSource;			//!< Object to slave visibility to, or CRuGUID::Invalid to disable
	CRuGUID									m_visibilityChainSubPath;			//!< Sub-patch to visibility chain source, set to NULL to target root entity

	// Orientation
	CRuVector3								m_scale;							//!< Scale of the entity
	CRuQuaternion							m_rotation;							//!< Rotation of the entity
	CRuVector3								m_translation;						//!< Translation of the entity

	// Bounding information
	BOOL									m_worldBoundsDirty;					//!< Is world bounds dirty?
	CRuSphere								m_localBounds;						//!< Local bounds of the entity
	CRuSphere								m_worldBounds;						//!< Bounds of the entity

	// Chunk relation information
	CRuArrayList<INT32>						m_linkedChunks;						//!< ID of chunks that associate with this entity

public:
											CRuWorld_EntityDescriptor();
	virtual									~CRuWorld_EntityDescriptor();

	RUAPI virtual IRuObject*				Clone(IRuObject *clonedObject = NULL);

	// Serialization interface
	RUAPI virtual BOOL						SerializeFrom(IRuStream *stream);
	RUAPI virtual BOOL						SerializeTo(IRuStream *stream);

	// Memory reporting
	RUAPI virtual INT32						GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	RUAPI const char*						GetResourceName() const;
	RUAPI void								SetResourceName(const char *resName);

	RUAPI UINT32							GetZoneFlags() const;
	RUAPI void								SetZoneFlags(UINT32 zoneFlags);

	RUAPI RuWorld::DetailClass				GetDetailClass() const;
	RUAPI void								SetDetailClass(RuWorld::DetailClass detailClass);

	RUAPI RuWorld::ZoneType					GetZoneType() const;
	RUAPI void								SetZoneType(RuWorld::ZoneType zoneType);

	RUAPI const CRuGUID&					GetVisibilityChainSource() const;
	RUAPI void								SetVisibilityChainSource(const CRuGUID &guid);

	RUAPI const CRuGUID&					GetVisibilityChainSubPath() const;
	RUAPI void								SetVisibilityChainSubPath(const CRuGUID &guid);

	RUAPI const CRuVector3&					GetScale() const;
	RUAPI void								SetScale(const CRuVector3 &scale);

	RUAPI const CRuQuaternion&				GetRotation() const;
	RUAPI void								SetRotation(const CRuQuaternion &rotation);

	RUAPI const CRuVector3&					GetTranslation() const;
	RUAPI void								SetTranslation(const CRuVector3 &translation);

	RUAPI const CRuSphere&					GetLocalBounds() const;
	RUAPI void								SetLocalBounds(const CRuSphere &bounds);

	RUAPI const CRuSphere&					GetWorldBounds();

	// Linked chunks
	RUAPI INT32								GetNumLinkedChunks() const;
	RUAPI INT32								GetLinkedChunk(INT32 index) const;
	RUAPI BOOL								InsertLinkedChunk(INT32 chunkID);
	RUAPI BOOL								DeleteLinkedChunk(INT32 chunkID);
	RUAPI void								ClearLinkedChunks();
};

// ************************************************************************************************************************************************************

/*!
	CRuWorld_Trigger
	@author John Tang
	@version 2008.09.16

	Stores information about triggers.

	The object's properties array contains the following additional properties:

	Property Name                                                Semantics                      Description
	-------------------------------------------------------------------------------------------------------------------------------------
	CRuWorld_Trigger:Always Trigger Music                        BOOL                           Music always triggers on first-time trigger entry?
	CRuWorld_Trigger:Music Occurrence Rate                       REAL                           Music occurrence rate (1.0 = 100% occurrence every second)
	CRuWorld_Trigger:Dawn Music                                  const char *                   Music to use for the dawn segment
	CRuWorld_Trigger:Day Music                                   const char *                   Music to use for the day segment
	CRuWorld_Trigger:Dusk Music                                  const char *                   Music to use for the dusk segment
	CRuWorld_Trigger:Night Music                                 const char *                   Music to use for the night segment
	CRuWorld_Trigger:Ambience Dawn                               const char *                   Ambience to use for the dawn segment
	CRuWorld_Trigger:Ambience Day                                const char *                   Ambience to use for the day segment
	CRuWorld_Trigger:Ambience Dusk                               const char *                   Ambience to use for the dusk segment
	CRuWorld_Trigger:Ambience Night                              const char *                   Ambience to use for the night segment
	CRuWorld_Trigger:Supplement Ambience Dawn                    const char *                   Supplement ambience to use for the dawn segment
	CRuWorld_Trigger:Supplement Ambience Day                     const char *                   Supplement ambience to use for the day segment
	CRuWorld_Trigger:Supplement Ambience Dusk                    const char *                   Supplement ambience to use for the dusk segment
	CRuWorld_Trigger:Supplement Ambience Night                   const char *                   Supplement ambience to use for the night segment
	CRuWorld_Trigger:Random Ambience Always Trigger              BOOL                           Random ambience always triggers on first-time trigger entry?
	CRuWorld_Trigger:Random Ambience Occurrence Rate             REAL                           Random ambience occurrence rate (1.0 = 100% occurrence every second)
	CRuWorld_Trigger:Random Ambience Dawn                        const char *                   Random ambience to use for the dawn segment
	CRuWorld_Trigger:Random Ambience Day                         const char *                   Random ambience to use for the day segment
	CRuWorld_Trigger:Random Ambience Dusk                        const char *                   Random ambience to use for the dusk segment
	CRuWorld_Trigger:Random Ambience Night                       const char *                   Random ambience to use for the night segment

*/
class CRuWorld_Trigger : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
//	CRuGizmo_Plane*							m_planeGizmo;						//!< Plane gizmo that defines the area of the trigger
	CRuGizmo_Box*							m_boxGizmo;
	RuWorld::TriggerType					m_triggerType;
	INT32									m_zoneID;							//!< Trigger zone ID
	RuWorld::ZoneType						m_zoneType;							//!< Zone type
	char*									m_commandString;

public:
											CRuWorld_Trigger();
	virtual									~CRuWorld_Trigger();

	// IRuObject interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Memory reporting
	virtual INT32							GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	void									Initialize(const CRuVector3 &origin);

	CRuGizmo_Box*							GetGizmo();

	CRuOBB									GetTriggerBounds();
	void									SetTriggerBounds(const CRuOBB &bounds);

	RuWorld::TriggerType					GetTriggerType();
	void									SetTriggerType(RuWorld::TriggerType triggerType);

	INT32									GetZoneID();
	void									SetZoneID(INT32 zoneID);

	RuWorld::ZoneType						GetZoneType() const;
	void									SetZoneType(RuWorld::ZoneType zoneType);

	const char*								GetCommandString();
	void									SetCommandString(const char *commandString);
};

// ************************************************************************************************************************************************************

extern BOOL									RuTrigger_GetWarpParameters(CRuWorld_Trigger *trigger, char *targetMapOut, INT32 targetMapBufLen, CRuVector3 *targetPositionOut);

// ************************************************************************************************************************************************************

/*!
	CRuWorld_ZoneSettings
	@author John Tang
	@version 2008.09.10

	Stores information about zone settings.

	The object's properties array contains the following additional properties:

	Property Name                                                Semantics                      Description
	-------------------------------------------------------------------------------------------------------------------------------------
	CRuWorld_ZoneSettings:Supplement Ambient FX 0                const char *                   Supplemental ambient sound FX 0
	CRuWorld_ZoneSettings:Supplement Ambient FX 1                const char *                   Supplemental ambient sound Fx 1
	CRuWorld_ZoneSettings:Supplement Ambient FX 2                const char *                   Supplemental ambient sound FX 2
	CRuWorld_ZoneSettings:Supplement Ambient FX 3                const char *                   Supplemental ambient sound FX 3
	CRuWorld_ZoneSettings:Supplement Ambient FX 4                const char *                   Supplemental ambient sound FX 4
	CRuWorld_ZoneSettings:Supplement Ambient Occurrence Rate     REAL                           Supplemental ambient sound FX occurrence rate (1.0 = 100% occurrence every second)

*/

class CRuWorld_ZoneSettings : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	INT32									m_timeGroup;											//!< Time group
	INT32									m_weatherGroup;											//!< Weather group

	// Color setup
	DWORD									m_color_Primary;										//!< Primary directional light color
	DWORD									m_color_Secondary;										//!< Secondary directional light color
	DWORD									m_color_Ambient;										//!< Ambient color
	DWORD									m_color_Specular;										//!< Specular color
	DWORD									m_color_Fog;											//!< Fog color

	// Fog setup
	REAL									m_fog_Constant;											//!< Constant fog value
	REAL									m_fog_Start;											//!< Fog start
	REAL									m_fog_End;												//!< Fog end

	// Lighting direction setup
	REAL									m_theta_Standard;										//!< Theta... what is this? elevation?
	REAL									m_phi_Standard;

	REAL									m_theta_Specular;
	REAL									m_phi_Specular;

	// Environment setup
	char*									m_weatherSystem;										//!< Weather system

	// Sky and cloud setup
	char*									m_skyGradientTextureName;								//!< Texture name to use for sky gradient

	REAL									m_cloudCover;											//!< Cloud cover, in [0, 1], larger values give fewer clouds
	REAL									m_cloudSharpness;										//!< Cloud sharpness constant, in [0, 1], larger values give fuzzier clouds
	RUCOLOR									m_cloudColor0;											//!< Cloud color
	RUCOLOR									m_cloudColor1;											//!< Cloud color

	INT32									m_baseOctaveSize;										//!< Base texture size required for 1:1 scale
	INT32									m_octaveSize[8];										//!< Texture size of each octave
	REAL									m_octaveScale[8];										//!< Noise scale of each octave
	REAL									m_octaveWeight[8];										//!< Weight of each octave
	REAL									m_octaveScrollSpeed[8];									//!< UV scrolling speed of each octave, in units per second

	// Custom sky box
	char*									m_customSkyBoxName;										//!< Name of the custom sky box to use in place of the standard one
	REAL									m_frameRateMultiplier;									//!< Multiplier for the frame rate

	// Sound environment
	char*									m_ambientSound;											//!< Ambient sound name

	char*									m_themeMusic;											//!< Theme music name
	REAL									m_themeMusicOccurrenceRate;								//!< Rate at which theme music is played (.20 = 20% of the time)

public:
											CRuWorld_ZoneSettings();
	virtual									~CRuWorld_ZoneSettings();

	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	const char*								GetWeatherSystem() const;
	void									SetWeatherSystem(const char *weatherSystem);

	void									SetSkyGradientTextureName(const char *skyGradientTextureName);
	void									SetCustomSkyBoxName(const char *customSkyBoxName);

	const char*								GetAmbientSound() const;
	void									SetAmbientSound(const char *ambientSound);

	const char*								GetThemeMusic() const;
	void									SetThemeMusic(const char *themeMusic);

	REAL									GetThemeMusicOccurrenceRate();
	void									SetThemeMusicOccurrenceRate(REAL occuranceRate);

	void									CopySettings(const CRuWorld_ZoneSettings &zoneSettings);
	CRuWorld_ZoneSettings&					operator=(const CRuWorld_ZoneSettings &zoneSettings);
};

// ************************************************************************************************************************************************************

class CRuWorld_ZoneDescriptor : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32									m_parentID;												//!< Parent zone ID, or -1 if this is a root zone
	INT32									m_id;													//!< Zone ID
	char*									m_name;													//!< Zone name
	RuWorld::ZoneType						m_zoneType;												//!< Zone type

	CRuVector3								m_respawnPoint;											//!< Respawn point

	CRuArrayList<CRuWorld_ZoneSettings *>	m_zoneSettings;											//!< Zone settings for each individual time group etc

public:
											CRuWorld_ZoneDescriptor();
	virtual									~CRuWorld_ZoneDescriptor();

	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	INT32									GetParentZoneID();
	void									SetParentZoneID(INT32 parentZoneID);

	INT32									GetZoneID();
	void									SetZoneID(INT32 zoneID);

	const char*								GetName() const;
	void									SetName(const char *name);

	RuWorld::ZoneType						GetZoneType() const;
	void									SetZoneType(RuWorld::ZoneType zoneType);

	const CRuVector3&						GetRespawnPoint() const;
	void									SetRespawnPoint(const CRuVector3 &respawnPoint);

	INT32									GetNumZoneSettings();
	CRuWorld_ZoneSettings*					GetZoneSettings(INT32 timeGroup, INT32 weatherGroup, BOOL absoluteMatch = FALSE);
	CRuWorld_ZoneSettings*					GetZoneSettingsByIndex(INT32 index);

	BOOL									InsertZoneSettings(const CRuWorld_ZoneSettings *zoneSettings);
	BOOL									DeleteZoneSettings(INT32 timeGroup, INT32 weatherGroup);

	void									CopySettings(const CRuWorld_ZoneDescriptor &zoneDesc);
	CRuWorld_ZoneDescriptor&				operator=(const CRuWorld_ZoneDescriptor &zoneDesc);

	// Upgrade stuff
	BOOL									SerializeFromOldVersion(IRuStream *stream);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_RuWorld_EntityDescriptor, CRuWorld_EntityDescriptor)
ruOBJECTMODULE_DEFINE(CRuObjectModule_RuWorld_Trigger, CRuWorld_Trigger)

// ************************************************************************************************************************************************************

#pragma managed

#endif
