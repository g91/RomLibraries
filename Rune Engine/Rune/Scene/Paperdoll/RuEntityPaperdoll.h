#pragma once

#include "../../Core/Collection/RuCollectionsTree.h"
#include "../../Scene/Paperdoll/RuEntityCompositeModel.h"

#pragma managed(push, off)

extern "C"
{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

// ************************************************************************************************************************************************************

class IRuPaperdoll;
class CRuPaperdoll;

// ************************************************************************************************************************************************************

class RuPaperdollEvent_BuildComplete_Args : public RuEventArgs
{
public:
	IRuPaperdoll*								m_paperdoll;					//!< Paperdoll

	RuPaperdollEvent_BuildComplete_Args(IRuPaperdoll *paperdoll)
	:	m_paperdoll(paperdoll)
	{
	}
};

// ************************************************************************************************************************************************************

class CRuPaperdollTemplate : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	struct FindItemQuery
	{
		// Query parameters
		INT32								m_maxMipLevel;
		INT32								m_partIdx;
		const char*							m_itemName;
		CRuArrayList<char *>				m_acceptableItemCodeList;
		const char*							m_baseItemKeyOverride;

		// Run-time state
		INT32								m_queryState;				//!< 0 = stopped, 1 = running, 2 = complete
		BOOL								m_instanceComplete;			//!< Item has finished instancing
		BOOL								m_itemIsAvailable;
		INT32								m_baseItemIdx;
		INT32								m_acceptableItemCodeIdx;
		INT32								m_validItemCodeIdx;

		// Result

		CRuEntity*							m_resultItem;
		const char*							m_baseItemKeyOut;
		const char*							m_itemCodeOut;
		const char*							m_itemAuxCodeOut;

		// Runtime workspace
		char*								m_modelPath;
		char*								m_diffuseTexturePath;
		char*								m_layerTexturePath[4];
		char*								m_compositeLayerTexturePath;
		char*								m_glossTexturePath;
		char*								m_glowTexturePath;

											FindItemQuery();
											~FindItemQuery();

		// Helpers
		void								StartQuery(INT32 maxMipLevel, INT32 partIndex, const char *itemName, CRuArrayList<const char *> *acceptableItemCodeList, const char *baseItemKeyOverride);
		void								StopQuery();

		BOOL								InstanceItem(BOOL enableDelayedLoad);

		BOOL								QueryIsComplete();
		BOOL								QueryIsRunning();
		BOOL								QueryIsStopped();

		BOOL								ItemIsInstanced();

		BOOL								ApplyTextureToItem(CRuEntity *item, RuTextureChannel channel, const char *textureName, RuSurfaceFormat targetSurfaceFormat, INT32 maxMipLevel, BOOL enableDelayedLoad) const;
		IRuMaterial*						GetItemMaterial(CRuEntity *item) const;
	};

protected:
	struct ColorCode
	{
		INT32								m_color0Code;					//!< Color 0 = skin color
		INT32								m_color1Code;					//!< Color 1 = hair color
	};

	struct Item
	{
		CRuString							m_itemPrefix;					//!< Item prefix to use when generating resource names for textures
		CRuArrayList<ColorCode>				m_validColorCodes;				//!< List of valid color codes
	};

	struct BaseItem
	{
		// Base item information
		CRuString							m_baseItemPrefix;				//!< Item prefix to use when generating resource names for part models
		CRuArrayList<const char *>			m_validBaseItemCodes;			//!< List of item codes available for this base item
		CRuString							m_baseItemAuxCode;				//!< Auxiliary code for the base item

		// Derived item information
		CRuBinaryTree<CRuString, Item *>	m_items;						//!< Tree of items that derive from this base item
	};

	struct Part
	{
		INT32								m_partID;						//!< Part ID
		CRuBinaryTree<CRuString, BaseItem *>m_baseItems;					//!< Tree of items belonging to this part
	};

	// Internal settings
	CRuString								m_resourceNamePrefix;			//!< Prefix to use when generating resource names for part models
	CRuString								m_resourceNamePostfix;			//!< Postfix to use when generating resource names for part models

	CRuString								m_baseItemResourcePath;			//!< Default resource path for base items
	CRuString								m_itemResourcePath;				//!< Default resource path for items

	// Template data
	CRuCompositeModelTemplate*				m_modelTemplate;				//!< Source template for the paperdoll
	CRuArrayList<Part *>					m_parts;						//!< Parts list

	// Runtime temporaries
	CRuStack<char *>						m_unusedResourceNameBuffers;

public:
											CRuPaperdollTemplate();
	virtual									~CRuPaperdollTemplate();

	// IRuObject serialization
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Initialization
	BOOL									Initialize(CRuCompositeModelTemplate *modelTemplate);

	// Template management
	const char*								GetBaseItemResourcePath() const;
	void									SetBaseItemResourcePath(const char *baseItemResourcePath);
	const char*								GetItemResourcePath() const;
	void									SetItemResourcePath(const char *itemResourcePath);

	BOOL									AddBaseItem(INT32 partID, const char *baseItemName, const char *baseItemCode, const char *baseItemAuxCode, const char *baseItemPrefix);
	BOOL									AddItem(const char *partName, const char *baseItemName, const char *itemName, const char *itemPrefix, INT32 color0Code, INT32 color1Code);

	// Template query
	INT32									GetNumParts() const;
	INT32									GetPartIndex(INT32 partID) const;
	INT32									GetPartIndex(const char *partName) const;
	INT32									GetPartID(INT32 partIndex) const;
	INT32									GetPartID(const char *partName) const;
	const char*								GetPartName(INT32 partIndex) const;

	INT32									GetNumBaseItems(INT32 partIndex) const;
	const char*								GetBaseItemName(INT32 partIndex, INT32 baseItemIndex) const;
	const char*								GetBaseItemPrefix(INT32 partIndex, INT32 baseItemIndex) const;
	char*									GetBaseItemCodesAsString(INT32 partIndex, INT32 baseItemIndex) const;

	INT32									GetNumItems(INT32 partIndex, INT32 baseItemIndex) const;
	const char*								GetItemName(INT32 partIndex, INT32 baseItemIndex, INT32 itemIndex) const;
	char*									GetItemColorCodesAsString(INT32 partIndex, INT32 baseItemIndex, INT32 itemIndex) const;

	// Item build helpers
	char*									CreateResourceNameBuffer();
	void									DestroyResourceNameBuffer(char *buffer);
	char*									BuildModelResourceName(const char *prefix, const char *baseItemPrefix, const char *partName, const char *baseItemKey, const char *itemCode0, const char *itemCode1, const char *postfix);
	char*									BuildBaseTextureResourceName(const char *prefix, const char *partName, const char *baseItemKey,	const char *itemKey);
	char*									BuildLayerMaskResourceName(const char *baseTextureResourceName, INT32 layerIdx);
	char*									BuildCompositeLayerMaskResourceName(const char *baseTextureResourceName);
	char*									BuildGlossTextureResourceName(const char *baseTextureResourceName);
	char*									BuildGlowTextureResourceName(const char *baseTextureResourceName);
/*
	BOOL									ApplyTextureToItem(CRuEntity *item, RuTextureChannel channel, const char *textureName, RuSurfaceFormat targetSurfaceFormat, INT32 maxMipLevel, BOOL *asyncLoadInProgress) const;
	IRuMaterial*							GetItemMaterial(CRuEntity *item) const;
*/
	// Paperdoll interface
	CRuPaperdoll*							Construct();

	BOOL									FindSuitableItem(FindItemQuery &itemQuery, BOOL enableDelayedLoad);

protected:
	void									InitializeDefaultAssemblyRule(lua_State *luaState);
};

// ************************************************************************************************************************************************************

class IRuPaperdoll : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
											IRuPaperdoll() { }
	virtual									~IRuPaperdoll() { }

	// Paperdoll query
	virtual CRuPaperdollTemplate*			GetPaperdollTemplate() = 0;

	// Paperdoll composition
	virtual const char*						GetComponent(const char *partName) = 0;
	virtual BOOL							SetComponent(const char *partName, const char *itemName) = 0;

	virtual BOOL							GetComponentColors(const char *partName, BOOL *layerActivatedOut, RUCOLOR *layerColorOut) = 0;
	virtual BOOL							SetComponentColors(const char *partName, const BOOL *layerActivated, const RUCOLOR *layerColor) = 0;

	virtual BOOL							BuildPaperdoll() = 0;
	virtual BOOL							BuildPaperdollEx() = 0;

	virtual BOOL							IsPaperdollBuilding() const = 0;
};

// ************************************************************************************************************************************************************

class CRuPaperdoll_Placeholder : public IRuPaperdoll
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct ComponentSetting
	{
		char*								m_itemName;
		BOOL								m_layerActivated[4];
		RUCOLOR								m_layerColor[4];

											ComponentSetting()
											{
												m_itemName = NULL;
												memset(m_layerActivated, 0, sizeof(BOOL) * 4);
												memset(m_layerColor, 0, sizeof(RUCOLOR) * 4);
											}
	};

	CRuTernaryStringTree<ComponentSetting>	m_componentTree;

	BOOL									m_paperdollBuilding;

public:
											CRuPaperdoll_Placeholder();
	virtual									~CRuPaperdoll_Placeholder();

	// Paperdoll query
	virtual CRuPaperdollTemplate*			GetPaperdollTemplate();

	// Paperdoll composition
	virtual const char*						GetComponent(const char *partName);
	virtual BOOL							SetComponent(const char *partName, const char *itemName);

	virtual BOOL							GetComponentColors(const char *partName, BOOL *layerActivatedOut, RUCOLOR *layerColorOut);
	virtual BOOL							SetComponentColors(const char *partName, const BOOL *layerActivated, const RUCOLOR *layerColor);

	virtual BOOL							BuildPaperdoll();
	virtual BOOL							BuildPaperdollEx();

	virtual BOOL							IsPaperdollBuilding() const;

protected:
	void									ClearComponentTree();
	BOOL									ClearComponentTreeCallback(const char *key, void *data);
};

// ************************************************************************************************************************************************************

class CRuPaperdoll : public IRuPaperdoll
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	typedef boost::function<BOOL (CRuPaperdoll *)> PaperdollBuildCompleteCallback;		// Signature: BOOL PaperdollBuildCompleteCallback(CRuPaperdoll *data)

	struct ComponentDesc
	{
		BOOL								m_layerActivated[4];					//!< Is each layer activated?
		RUCOLOR								m_layerColor[4];						//!< Layer colors
		INT32								m_overrideUsingIndex;					//!< Override color usage using the component at given index, -1 to disable

											ComponentDesc()
											{
												for(INT32 i = 0; i < 4; ++i)
												{
													m_layerActivated[i] = FALSE;
													m_layerColor[i] = 0xFFFFFFFF;
												}

												m_overrideUsingIndex = -1;
											}
	};

	CRuEvent								m_event_BuildComplete;

	INT32									m_maxMipLevel;
	INT32									m_maxMipLevelOverride;

	CRuPaperdollTemplate*					m_paperdollTemplate;					//!< Source paperdoll template
	CRuCompositeModel*						m_paperdollModel;						//!< Constructed paperdoll model

	CRuArrayList<CRuString>					m_components;							//!< Items worn on the paperdoll
	CRuArrayList<CRuEntity *>				m_componentEntities;					//!< Resolved item entities
	CRuArrayList<const char *>				m_componentItemCodes;					//!< Resolved item codes
	CRuArrayList<ComponentDesc *>			m_componentDescriptors;

	CRuArrayList<CRuPaperdollTemplate::FindItemQuery>	m_componentQueries;

	PaperdollBuildCompleteCallback			m_buildCompleteCallback;
	BOOL									m_enableDelayedBuild;
	BOOL									m_buildInProgress;
	BOOL									m_rebuildRequired;
	INT32									m_stageIndex;
	BOOL									m_paperdollIsValid;

	// Lua VM
	lua_State*								m_luaState;						//!< Lua VM

public:
											CRuPaperdoll();
	virtual									~CRuPaperdoll();

	CRuEvent&								Event_BuildComplete();

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	// Callbacks
	BOOL									SetPaperdollBuildCompleteCallback(PaperdollBuildCompleteCallback callback);

	// Paperdoll query
	virtual CRuPaperdollTemplate*			GetPaperdollTemplate();

	// Composite model query
	CRuCompositeModel*						GetCompositeModel();

	// Settings
	void									SetMaxMipLevelOverride(INT32 maxMipLevel);

	// Paperdoll composition
	virtual const char*						GetComponent(const char *partName);
	virtual const char*						GetComponent(INT32 partIndex);
	virtual BOOL							SetComponent(const char *partName, const char *itemName);
	virtual BOOL							SetComponent(INT32 partIndex, const char *itemName);
	
	virtual BOOL							GetComponentColors(const char *partName, BOOL *layerActivatedOut, RUCOLOR *layerColorOut);
	virtual BOOL							GetComponentColors(INT32 partIndex, BOOL *layerActivatedOut, RUCOLOR *layerColorOut);
	virtual BOOL							SetComponentColors(const char *partName, const BOOL *layerActivated, const RUCOLOR *layerColor);
	virtual BOOL							SetComponentColors(INT32 partIndex, const BOOL *layerActivated, const RUCOLOR *layerColor);
	virtual BOOL							SetComponentColorOverride(INT32 partIndex, INT32 overrideIndex);

	virtual BOOL							BuildPaperdoll();
	virtual BOOL							BuildPaperdollEx();

	virtual BOOL							IsPaperdollBuilding() const;
	virtual BOOL							IsPaperdollInValidState() const;

protected:
	BOOL									BuildPaperdollEx_Internal(BOOL enableDelayedLoad);
	INT32									AssembleComponents(BOOL enableDelayedLoad);

	INT32									GetBuildStage();
	INT32									SetBuildStage(INT32 buildStage);

	BOOL									ComponentExists(INT32 partIndex);
	BOOL									MatchComponentItemCode(INT32 partIndex, const char *itemCode);
	BOOL									MatchComponentItemAuxCode(INT32 partIndex, const char *auxCode);
	const char*								GetBaseItemKey(INT32 partIndex);

	BOOL									StopQuery(INT32 partIdx);
	BOOL									QueryForItem(INT32 partIndex, const char *itemName, const char *acceptableItemCodeList, const void *dummy, BOOL enableDelayedLoad);
	BOOL									QueryForItem(INT32 partIndex, const char *itemName, const char *acceptableItemCodeList, const char *baseItemKeyOverride, BOOL enableDelayedLoad);
	BOOL									QueryForItem(INT32 partIndex, const char *itemName, CRuArrayList<const char *> *acceptableItemCodeList, const char *baseItemKeyOverride, BOOL enableDelayedLoad);

	// Event handlers
	void									GetSettingsFromEngine();
	BOOL									EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting);

	friend									CRuPaperdollTemplate;
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_PaperdollTemplate, CRuPaperdollTemplate)

// ************************************************************************************************************************************************************

#pragma managed(pop)
