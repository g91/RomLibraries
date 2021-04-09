#include "../../Scene/Base/RuEntityBase.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuEntity, "CRuEntity", "IRuObject")
ruCLASSGUID_DEFINE(CRuEntity, 0xD3AF51F4, 0x40584436, 0x836DB5A3, 0xA524CC66)

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Light, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Light, "CRuEntity_Light", "CRuEntity")
ruCLASSGUID_DEFINE(CRuEntity_Light, 0x6786C143, 0xFD3F4cde, 0xA2E24057, 0xE96069B8)

ruRTTI_DEFINE_SUBCLASS(IRuEntity_Container, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(IRuEntity_Container, "IRuEntity_Container", "CRuEntity")
ruCLASSGUID_DEFINE(IRuEntity_Container, 0x2D74617C, 0xA805461c, 0xA4057409, 0xB1717FA8)

ruRTTI_DEFINE_SUBCLASS(IRuEntity_Container_Mutable, IRuEntity_Container)
ruHEAP_DEFINE_SUBCLASS(IRuEntity_Container_Mutable, "IRuEntity_Container_Mutable", "IRuEntity_Container")
ruCLASSGUID_DEFINE(IRuEntity_Container_Mutable, 0xE783EC1D, 0xFC73433c, 0xAC542B0C, 0x707D7CCD)

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Container_Impl, IRuEntity_Container_Mutable)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Container_Impl, "CRuEntity_Container_Impl", "IRuEntity_Container_Mutable")
ruCLASSGUID_DEFINE(CRuEntity_Container_Impl, 0x2C284038, 0x66F74a72, 0x871B9FA4, 0xF0B4B543)

ruRTTI_DEFINE_SUBCLASS(IRuEntity_Renderable, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(IRuEntity_Renderable, "IRuEntity_Renderable", "CRuEntity")
ruCLASSGUID_DEFINE(IRuEntity_Renderable, 0x325BA010, 0x16C54542, 0x995BC624, 0xAB011DF4)

ruRTTI_DEFINE_SUBCLASS(IRuEntity_Controller, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(IRuEntity_Controller, "IRuEntity_Controller", "CRuEntity")
ruCLASSGUID_DEFINE(IRuEntity_Controller, 0x3B7F47AD, 0x47844843, 0x972EE7C1, 0xB7503CB3)

ruRTTI_DEFINE_SUBCLASS(IRuEntity_KeyableController, IRuEntity_Controller)
ruHEAP_DEFINE_SUBCLASS(IRuEntity_KeyableController, "IRuEntity_KeyableController", "IRuEntity_Controller")
ruCLASSGUID_DEFINE(IRuEntity_KeyableController, 0x718CEED4, 0x06B84d00, 0xB3835874, 0x376A20DF)

// ************************************************************************************************************************************************************

INT32 g_ruProbe_TransformUpdateCount = -1;
INT32 g_ruProbe_TransformUpdateTime = -1;
INT32 g_ruProbe_BoundingUpdateCount = -1;
INT32 g_ruProbe_BoundingUpdateTime = -1;

// ************************************************************************************************************************************************************

INT32 CRuEntity::s_updateInvokeCount = 0;
INT32 CRuEntity::s_eventRaiseCount = 0;
INT32 CRuEntity::s_listenerInvokeCount = 0;

// ************************************************************************************************************************************************************

INT32 CRuEntity::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	INT32 footprint = IRuObject::GetMemoryFootprint(enumHash);

	if(footprint < 0)
	{
		return -1;
	}

	footprint += sizeof(m_entityParent);
	footprint += sizeof(m_firstChild) + (m_firstChild ? m_firstChild->GetMemoryFootprint(enumHash) : 0);
	footprint += sizeof(m_prevSibling);
	footprint += sizeof(m_nextSibling) + (m_nextSibling ? m_nextSibling->GetMemoryFootprint(enumHash) : 0);

	footprint += sizeof(m_localTransform_Dirty);
	footprint += sizeof(m_transform_Dirty);
	footprint += sizeof(m_inverseTransform_Dirty);
	footprint += sizeof(m_transform_Local_Scale);
	footprint += sizeof(m_transform_Local_Rotation);
	footprint += sizeof(m_transform_Local_Translation);
	footprint += sizeof(m_transform_World_Scale);
	footprint += sizeof(m_transform_World_Rotation);
	footprint += sizeof(m_transform_World_Translation);

	footprint += sizeof(m_bounds_EntityDirty);
	footprint += sizeof(m_bounds_Entity);

	footprint += sizeof(m_bounds_Dirty);
	footprint += sizeof(m_bounds_Local);
	footprint += sizeof(m_bounds_World);

	footprint += sizeof(m_localTransform);
	footprint += sizeof(m_worldTransform);
	footprint += sizeof(m_inverseWorldTransform);

	footprint += sizeof(m_collisionObject) + (m_collisionObject ? m_collisionObject->GetMemoryFootprint(enumHash) : 0);

	footprint += sizeof(m_statusFlags);
	footprint += sizeof(m_optionFlags0);

	footprint += sizeof(m_parameterBlock);

	return footprint;
}

/*!
	Retrieves the entity's parent container.

	@return A pointer to the entity's parent container, or NULL if none exist.
*/
CRuEntity *CRuEntity::GetParent()
{
	return m_entityParent;
}

/*!
	Sets the parent for this entity. Typically used by container objects when a child entity
	is added to the container. Once an entity has a parent, its world transform will be
	calculated relative to its parent.

	@param parent Entity to attach to.
*/
void CRuEntity::SetParent(CRuEntity *parent)
{
	// Assign new parent
	m_entityParent = parent;
}

INT32 CRuEntity::GetStatusFlag(RuEntityStatusFlag flag)
{
	switch(flag)
	{
		case ruENTITYSTATUS_VISIBLE:
			return (m_statusFlags & 0x00000001);
	}

	return 0;
}

INT32 CRuEntity::SetStatusFlag(RuEntityStatusFlag flag, INT32 value)
{
	switch(flag)
	{
		case ruENTITYSTATUS_VISIBLE:
			m_statusFlags = (m_statusFlags & ~0x00000001) | ((value << 0) & 0x00000001);
			break;

		default:
			return 0;
	}

	return value;
}

/*!
	Gets the specified option value for this entity.

	@param option Option to get.

	@return The option value.
*/
INT32 CRuEntity::GetOption(RuEntityOption option)
{
	switch(option)
	{
		case ruENTITYOPTION_SKINNED:
			return (m_optionFlags0 & 0x00000001);

		case ruENTITYOPTION_COLLISION:
			return (m_optionFlags0 & 0x00000002) >> 1;

		case ruENTITYOPTION_DISTANCECLASS:
			return (m_optionFlags0 & 0x0000003C) >> 2;

		case ruENTITYOPTION_REFLECTCLASS:
			return (m_optionFlags0 & 0x000000C0) >> 6;

		case ruENTITYOPTION_CASTERCLASS:
			return (m_optionFlags0 & 0x00000300) >> 8;

		case ruENTITYOPTION_RECEIVERCLASS:
			return (m_optionFlags0 & 0x00000C00) >> 10;

		case ruENTITYOPTION_BILLBOARD:
			return (m_optionFlags0 & 0x00007000) >> 12;

		case ruENTITYOPTION_HIDDEN:
			return (m_optionFlags0 & 0x00008000) >> 15;

		case ruENTITYOPTION_NOCLONE:
			return (m_optionFlags0 & 0x00010000) >> 16;

		case ruENTITYOPTION_NOSERIALIZE:
			return (m_optionFlags0 & 0x00020000) >> 17;

		case ruENTITYOPTION_TRANSFORMMODE:
			return (m_optionFlags0 & 0x000C0000) >> 18;

		case ruENTITYOPTION_HRDISABLE:
			return (m_optionFlags0 & 0x00100000) >> 20;

		case ruENTITYOPTION_CULLMODE:
			return (m_optionFlags0 & 0x00E00000) >> 21;

		case ruENTITYOPTION_EXCLUDEBOUNDS:
			return (m_optionFlags1 & 0x00000001);

		case ruENTITYOPTION_NOCLIP:
			return (m_optionFlags0 & 0x02000000) >> 25;

		case ruENTITYOPTION_USEPARENTLIGHTING:
			return (m_optionFlags1 & 0x00000002) >> 1;

		case ruENTITYOPTION_BACKDROP:
			return (m_optionFlags0 & 0x80000000) >> 31;

		case ruENTITYOPTION_IGNOREVISIBILITY:
			return (m_optionFlags0 & 0x40000000) >> 30;

		case ruENTITYOPTION_DECALTARGET:
			return (m_optionFlags0 & 0x20000000) >> 29;

		case ruENTITYOPTION_ENABLEDISTANCEFADE:
			return (m_optionFlags0 & 0x10000000) >> 28;

		case ruENTITYOPTION_HIDDENBYDISTANCE:
			return (m_optionFlags0 & 0x08000000) >> 27;

		case ruENTITYOPTION_NOMOTIONPROPAGATE:
			return (m_optionFlags0 & 0x04000000) >> 26;
	}

	return 0;
}

/*!
	Sets the specified option value for this entity.

	@param option Option to set.
	@param value Value to set.

	@return The new option value.
*/
INT32 CRuEntity::SetOption(RuEntityOption option, INT32 value, BOOL applyToChildren)
{
	switch(option)
	{
		case ruENTITYOPTION_SKINNED:
			m_optionFlags0 = (m_optionFlags0 & ~0x00000001) | ((value << 0) & 0x00000001);
			break;

		case ruENTITYOPTION_COLLISION:
			m_optionFlags0 = (m_optionFlags0 & ~0x00000002) | ((value << 1) & 0x00000002);
			break;

		case ruENTITYOPTION_DISTANCECLASS:
			m_optionFlags0 = (m_optionFlags0 & ~0x0000003C) | ((value << 2) & 0x0000003C);
			break;

		case ruENTITYOPTION_REFLECTCLASS:
			m_optionFlags0 = (m_optionFlags0 & ~0x000000C0) | ((value << 6) & 0x000000C0);
			break;

		case ruENTITYOPTION_CASTERCLASS:
			m_optionFlags0 = (m_optionFlags0 & ~0x00000300) | ((value << 8) & 0x00000300);
			break;

		case ruENTITYOPTION_RECEIVERCLASS:
			m_optionFlags0 = (m_optionFlags0 & ~0x00000C00) | ((value << 10) & 0x00000C00);
			break;

		case ruENTITYOPTION_BILLBOARD:
			m_optionFlags0 = (m_optionFlags0 & ~0x00007000) | ((value << 12) & 0x00007000);
			break;

		case ruENTITYOPTION_HIDDEN:
			m_optionFlags0 = (m_optionFlags0 & ~0x00008000) | ((value << 15) & 0x00008000);
			break;

		case ruENTITYOPTION_NOCLONE:
			m_optionFlags0 = (m_optionFlags0 & ~0x00010000) | ((value << 16) & 0x00010000);
			break;

		case ruENTITYOPTION_NOSERIALIZE:
			m_optionFlags0 = (m_optionFlags0 & ~0x00020000) | ((value << 17) & 0x00020000);
			break;

		case ruENTITYOPTION_TRANSFORMMODE:
			m_optionFlags0 = (m_optionFlags0 & ~0x000C0000) | ((value << 18) & 0x000C0000);
			break;

		case ruENTITYOPTION_HRDISABLE:
			m_optionFlags0 = (m_optionFlags0 & ~0x00100000) | ((value << 20) & 0x00100000);
			break;

		case ruENTITYOPTION_CULLMODE:
			m_optionFlags0 = (m_optionFlags0 & ~0x00E00000) | ((value << 21) & 0x00E00000);
			break;

		case ruENTITYOPTION_EXCLUDEBOUNDS:
			m_optionFlags1 = (m_optionFlags1 & ~0x00000001) | ((value << 0) & 0x00000001);
			break;

		case ruENTITYOPTION_NOCLIP:
			m_optionFlags0 = (m_optionFlags0 & ~0x02000000) | ((value << 25) & 0x02000000);
			break;

		case ruENTITYOPTION_USEPARENTLIGHTING:
			m_optionFlags1 = (m_optionFlags1 & ~0x00000002) | ((value << 1) & 0x00000002);
			break;

		case ruENTITYOPTION_BACKDROP:
			m_optionFlags0 = (m_optionFlags0 & ~0x80000000) | ((value << 31) & 0x80000000);
			break;

		case ruENTITYOPTION_IGNOREVISIBILITY:
			m_optionFlags0 = (m_optionFlags0 & ~0x40000000) | ((value << 30) & 0x40000000);
			break;

		case ruENTITYOPTION_DECALTARGET:
			m_optionFlags0 = (m_optionFlags0 & ~0x20000000) | ((value << 29) & 0x20000000);
			break;

		case ruENTITYOPTION_ENABLEDISTANCEFADE:
			m_optionFlags0 = (m_optionFlags0 & ~0x10000000) | ((value << 28) & 0x10000000);
			break;

		case ruENTITYOPTION_HIDDENBYDISTANCE:
			m_optionFlags0 = (m_optionFlags0 & ~0x08000000) | ((value << 27) & 0x08000000);
			break;

		case ruENTITYOPTION_NOMOTIONPROPAGATE:
			m_optionFlags0 = (m_optionFlags0 & ~0x04000000) | ((value << 26) & 0x04000000);
			break;

		default:
			return 0;
	}

	// Raise the option changed event
	RuEntityEvent_OptionChanged_Args optionChangedArgs(this);
	Event_OptionChanged().Trigger(&optionChangedArgs);

	// If apply to children is specified, apply the set option to children
	if(applyToChildren)
	{
		CRuEntity *curChild = this->GetFirstChild();
		while(curChild)
		{
			curChild->SetOption(option, value, applyToChildren);
			curChild = curChild->GetNextSibling();
		}
	}

	return value;
}

/*!
	Gets the system-set data tag. Reserved for use by the IRuSceneRenderer.

	@return The entity's tag.
*/
POINTER CRuEntity::GetTag(INT32 tagID)
{
	TagDesc *prevTag = NULL;
	TagDesc *curTag = m_tags;
	while(curTag)
	{
		// If specified tag ID is found, return the tag
		if(curTag->m_tagID  == tagID)
		{
			return curTag->m_tag;
		}

		// Advance to next listener
		prevTag = curTag;
		curTag = curTag->m_next;
	}

	return NULL;
}

/*!
	Sets the system-set data tag. Reserved for use by the IRuSceneRenderer.

	@param tag Tag to set.
*/
void CRuEntity::SetTag(INT32 tagID, POINTER tag)
{
	TagDesc *prevTag = NULL;
	TagDesc *curTag = m_tags;
	while(curTag)
	{
		// If specified tag ID is found, set the tag
		if(curTag->m_tagID  == tagID)
		{
			curTag->m_tag = tag;
			return;
		}

		// Advance to next listener
		prevTag = curTag;
		curTag = curTag->m_next;

		// If next tag is NULL, append new tag
		if(curTag == NULL)
		{
			TagDesc *tagDesc = ruNEW TagDesc();
			tagDesc->m_tagID = tagID;
			tagDesc->m_tag = tag;
			tagDesc->m_next = NULL;

			prevTag->m_next = tagDesc;

			return;
		}
	}

	// If there are no tags at all, append new tag

	TagDesc *tagDesc = ruNEW TagDesc();
	tagDesc->m_tagID = tagID;
	tagDesc->m_tag = tag;
	tagDesc->m_next = NULL;

	m_tags = tagDesc;
}

BOOL CRuEntity::AddChild(CRuEntity *entity)
{
	assert(entity != this);

	// Do nothing if the entity parent is already self
	if(entity->m_entityParent == this)
	{
		return TRUE;
	}

	// Increment reference count on new child
	entity->AddRef();

	// If entity already has a parent, detach it
	if(entity->m_entityParent)
	{
		// Detach the entity from its parent
		entity->DetachFromParent();
	}

	if(m_firstChild)
	{
		entity->m_entityParent = this;
		entity->m_prevSibling = NULL;
		entity->m_nextSibling = m_firstChild;

		m_firstChild->m_prevSibling = entity;
		m_firstChild = entity;
	}
	else
	{
		entity->m_entityParent = this;
		entity->m_prevSibling = NULL;
		entity->m_nextSibling = NULL;

		m_firstChild = entity;
	}

	// Invalid new child entity's transform chain
	entity->InvalidateTransformChain();

	// Reset time to next local bound update
	this->m_timeToNextLocalBoundUpdate = 0.0f;

	return TRUE;
}

BOOL CRuEntity::RemoveChild(CRuEntity *entity)
{
	if(m_firstChild)
	{
		CRuEntity *curChild = m_firstChild;

		// Find the child
		while(curChild && curChild != entity)
		{
			curChild = curChild->m_nextSibling;
		}

		// Child node is found
		if(curChild)
		{
			// Invalidate removed child's transform chain
			curChild->InvalidateTransformChain();

			// Detach child entity from the tree
			if(curChild->m_prevSibling)
			{
				curChild->m_prevSibling->m_nextSibling = curChild->m_nextSibling;

				if(curChild->m_nextSibling)
				{
					curChild->m_nextSibling->m_prevSibling = curChild->m_prevSibling;
				}
			}
			else if(curChild->m_nextSibling)
			{
				curChild->m_nextSibling->m_prevSibling = NULL;

				m_firstChild = curChild->m_nextSibling;
			}
			else
			{
				m_firstChild = NULL;
			}

			// Clear child's structure info
			curChild->m_entityParent = NULL;
			curChild->m_prevSibling = NULL;
			curChild->m_nextSibling = NULL;

			// Release reference to child entity
			ruSAFE_RELEASE(curChild);

			return TRUE;
		}

		// Child node is not found, recursively call on all children
		curChild = m_firstChild;
		while(curChild)
		{
			if(curChild->RemoveChild(entity))
			{
				return TRUE;
			}

			curChild = curChild->m_nextSibling;
		}
	}

	return FALSE;
}

BOOL CRuEntity::RemoveAllChildren()
{
	CRuEntity *curChild = m_firstChild;

	while(curChild)
	{
		CRuEntity *nextChild = curChild->m_nextSibling;

		// Invalidate removed child's transform chain
		curChild->InvalidateTransformChain();

		// Detach child node
		curChild->m_entityParent = NULL;
		curChild->m_prevSibling = NULL;
		curChild->m_nextSibling = NULL;

		// Release reference to child
		ruSAFE_RELEASE(curChild);

		// Advance to next sibling
		curChild = nextChild;
	}

	m_firstChild = NULL;

	return TRUE;
}

BOOL CRuEntity::DetachFromParent()
{
	if(m_entityParent)
	{
		// Invalidate removed child's transform chain
		this->InvalidateTransformChain();

		// Reassign links
		if(m_prevSibling)
		{
			if(m_nextSibling)
			{
				m_prevSibling->m_nextSibling = m_nextSibling;
				m_nextSibling->m_prevSibling = m_prevSibling;

				// Clear next sibling pointer
				m_nextSibling = NULL;
			}
			else
			{
				m_prevSibling->m_nextSibling = NULL;
			}

			// Clear previous sibling pointer
			m_prevSibling = NULL;
		}
		else
		{
			if(m_nextSibling)
			{
				m_nextSibling->m_prevSibling = NULL;
				m_entityParent->m_firstChild = m_nextSibling;

				// Clear next sibling pointer
				m_nextSibling = NULL;
			}
			else
			{
				m_entityParent->m_firstChild = NULL;
			}
		}

		// Reset time to next local bound update for the parent
		m_entityParent->m_timeToNextLocalBoundUpdate = 0.0f;

		// Clear entity parent pointer
		m_entityParent = NULL;

		// Release parent's reference count
		this->Release();

		return TRUE;
	}

	return FALSE;
}

CRuEntity *CRuEntity::GetFirstChild()
{
	return m_firstChild;
}

CRuEntity *CRuEntity::GetNextSibling()
{
	return m_nextSibling;
}

CRuEntity *CRuEntity::GetPreviousSibling()
{
	return m_prevSibling;
}

CRuEvent &CRuEntity::Event_Update()
{
	return m_event_Update;
}

CRuEvent &CRuEntity::Event_UpdateStep()
{
	return m_event_UpdateStep;
}

CRuEvent &CRuEntity::Event_Submitted()
{
	return m_event_Submitted;
}

CRuEvent &CRuEntity::Event_Camera()
{
	return m_event_Camera;
}

CRuEvent &CRuEntity::Event_Transform()
{
	return m_event_Transform;
}

CRuEvent &CRuEntity::Event_DecalTarget()
{
	return m_event_DecalTarget;
}

CRuEvent &CRuEntity::Event_Trigger()
{
	return m_event_Trigger;
}

CRuEvent &CRuEntity::Event_OptionChanged()
{
	return m_event_OptionChanged;
}

REAL CRuEntity::GetMinUpdateInterval() const
{
	return m_minUpdateInterval;
}

void CRuEntity::SetMinUpdateInterval(REAL minUpdateInterval)
{
	m_minUpdateInterval = minUpdateInterval;
}

void CRuEntity::ResetUpdateInvokeCount()
{
	s_updateInvokeCount = 0;
}

INT32 CRuEntity::GetUpdateInvokeCount()
{
	return s_updateInvokeCount;
}

void CRuEntity::ResetEventRaiseCount()
{
	s_eventRaiseCount = 0;
}

INT32 CRuEntity::GetEventRaiseCount()
{
	return s_eventRaiseCount;
}

void CRuEntity::ResetListenerInvokeCount()
{
	s_listenerInvokeCount = 0;
}

INT32 CRuEntity::GetListenerInvokeCount()
{
	return s_listenerInvokeCount;
}

CRuParameterBlock *CRuEntity::GetParameterBlock()
{
	return &m_parameterBlock;
}

void CRuEntity::Dispose()
{
	// Chain dispose up to parent
	IRuObject::Dispose();
}

/*!
	Clones the entity. As a general rule, clone should never duplicate the m_tag
	and m_parameterBlocks members, since these are entity-specific data and it
	does not make sense to clone them. Even if they were cloned, there would be
	problems upon destruction of one of the cloned entities. This is because
	m_tag is always set by the scene renderer, and it is reassigned every frame.
	m_parameterBlocks is typically set by controllers, and those are never cloned
	as part of an entity hierarchy. So it makes no sense to clone the parameter
	blocks.

	@param clonedObject Pointer to a cloned object previously created by a subclass.

	@return A pointer to the newly cloned object.
*/
IRuObject *CRuEntity::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuEntity();
	CRuEntity *clonedEntity = static_cast<CRuEntity *>(clonedObject);

	// Chain cloning call to parent class
	IRuObject::Clone(clonedObject);

	// Copy transform
	clonedEntity->m_localTransform_Dirty = TRUE;
	clonedEntity->m_transform_Dirty = TRUE;
	clonedEntity->m_inverseTransform_Dirty = TRUE;
	clonedEntity->m_transform_Local_Scale = m_transform_Local_Scale;
	clonedEntity->m_transform_Local_Rotation = m_transform_Local_Rotation;
	clonedEntity->m_transform_Local_Translation = m_transform_Local_Translation;

	// Copy local entity bounds
	clonedEntity->m_bounds_Entity = m_bounds_Entity;

	// Copy collision object
	clonedEntity->m_collisionObject = m_collisionObject;
	if(clonedEntity->m_collisionObject)
	{
		clonedEntity->m_collisionObject->AddRef();
	}

	// Copy options flags
	clonedEntity->m_optionFlags0 = m_optionFlags0;
	clonedEntity->m_optionFlags1 = m_optionFlags1;

	// Clone node tree
	CRuEntity *curChild = m_firstChild;
	while(curChild)
	{
		// Clone the node only if it is not marked no-clone
		if(curChild->GetOption(ruENTITYOPTION_NOCLONE) == FALSE)
		{
			// Clone the node
			IRuObject *clonedChild = curChild->Clone();
			if(clonedChild && clonedChild->GetType().IsSubClassOf(CRuEntity::Type()))
			{
				// Attach cloned node to parent
				clonedEntity->AddChild(static_cast<CRuEntity *>(clonedChild));

				// Release local reference
				ruSAFE_RELEASE(clonedChild);
			}
		}

		// Advance to next sibling
		curChild = curChild->m_nextSibling;
	}

	return clonedObject;
}

/*!
	Serializes the entity from the specified stream. Default implementation does
	nothing and immediately returns.

	@param stream Stream to serialize from.

	@retval TRUE The operation succeeded.
	@retval FALSE The operation failed.
*/
BOOL CRuEntity::SerializeFrom(IRuStream *stream)
{
	CRuVector3 scale;
	CRuQuaternion rotation;
	CRuVector3 translation;

	// Read class GUID
	CRuGUID classGUID;
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeFrom(stream);

	// Read scale
	stream->Read(&scale.x, sizeof(REAL));
	stream->Read(&scale.y, sizeof(REAL));
	stream->Read(&scale.z, sizeof(REAL));

	// Read rotation
	stream->Read(&rotation.x, sizeof(REAL));
	stream->Read(&rotation.y, sizeof(REAL));
	stream->Read(&rotation.z, sizeof(REAL));
	stream->Read(&rotation.w, sizeof(REAL));

	// Read translation
	stream->Read(&translation.x, sizeof(REAL));
	stream->Read(&translation.y, sizeof(REAL));
	stream->Read(&translation.z, sizeof(REAL));

	// Update local transform matrix
	this->ResetAllTransforms();
	this->Scale(scale);
	this->Rotate(rotation);
	this->Translate(translation);

	// Read options
	stream->Read(&m_optionFlags0, sizeof(UINT32));

	if(version >= 2)
	{
		stream->Read(&m_optionFlags1, sizeof(UINT32));
	}

	// Read whether collision data exists
	INT32 colDataExists = 0;
	stream->Read(&colDataExists, sizeof(INT32));

	// If collision data exists, read it
	if(colDataExists)
	{
		IRuObject *object = RuObjectFactory().CreateObject(stream);

		if(object->GetType().IsSubClassOf(IRuCollisionObject::Type()))
		{
			// Attach collision structure
			m_collisionObject = static_cast<IRuCollisionObject *>(object);
		}
	}

	return TRUE;
}

/*!
	Serializes the entity to the specified stream. Default implementation does
	nothing and immediately returns.

	@param stream Stream to serialize to.

	@retval TRUE The operation succeeded.
	@retval FALSE The operation failed.
*/
BOOL CRuEntity::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 2;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeTo(stream);

	// Write scale (X, Y, Z)
	stream->Write(&m_transform_Local_Scale, sizeof(REAL));
	stream->Write(&m_transform_Local_Scale, sizeof(REAL));
	stream->Write(&m_transform_Local_Scale, sizeof(REAL));

	// Write rotation
	stream->Write(&m_transform_Local_Rotation.x, sizeof(REAL));
	stream->Write(&m_transform_Local_Rotation.y, sizeof(REAL));
	stream->Write(&m_transform_Local_Rotation.z, sizeof(REAL));
	stream->Write(&m_transform_Local_Rotation.w, sizeof(REAL));

	// Write translation
	stream->Write(&m_transform_Local_Translation.x, sizeof(REAL));
	stream->Write(&m_transform_Local_Translation.y, sizeof(REAL));
	stream->Write(&m_transform_Local_Translation.z, sizeof(REAL));

	// Write options
	stream->Write(&m_optionFlags0, sizeof(UINT32));
	stream->Write(&m_optionFlags1, sizeof(UINT32));

	// Write collision data existence flag
	INT32 colDataExists = (m_collisionObject != NULL);
	stream->Write(&colDataExists, sizeof(INT32));

	// Write collision data
	if(m_collisionObject)
	{
		m_collisionObject->SerializeTo(stream);
	}

	return TRUE;
}

/*!
	Resets any frame-specific states.

	@retval TRUE The operation succeeded.
	@retval FALSE The operation failed.
*/
BOOL CRuEntity::ResetStates()
{
	// Call reset on all children
	CRuEntity *curChild = this->GetFirstChild(), *nextChild = NULL;
	while(curChild)
	{
		// Store pointer to next child
		nextChild = curChild->GetNextSibling();

		// Invoke update
		curChild->ResetStates();

		// Advance to next sibling
		curChild = nextChild;
	}

	return TRUE;
}

/*!
	Causes the entity to update its state, including its world transform.

	@retval TRUE The operation succeeded.
	@retval FALSE The operation failed.
*/
BOOL CRuEntity::Update(REAL elapsedTime)
{
	// Track update count
	++s_updateInvokeCount;

	// Accumulate elapsed time
	m_accumElapsedTime += elapsedTime;
	m_timeToNextLocalBoundUpdate -= elapsedTime;

	if(m_accumElapsedTime >= m_minUpdateInterval)
	{
		// Trigger the entity update event
		RuEntityEvent_Update_Args updateArgs(this, elapsedTime);
		Event_Update().Trigger(&updateArgs);

		// Call update on all children
		CRuEntity *curChild = this->GetFirstChild(), *nextChild = NULL;
		while(curChild)
		{
			// Store pointer to next child
			nextChild = curChild->GetNextSibling();

			// Invoke update
			curChild->Update(m_accumElapsedTime);

			// Advance to next sibling
			curChild = nextChild;
		}

		// Reset accumulated time
		m_accumElapsedTime = 0.0f;
	}

	return TRUE;
}

/*!
	Retrieves the world transform for the entity.

	@return A reference to a CRuMatrix4x4 containing the entity's world transform.
*/
const CRuMatrix4x4 &CRuEntity::GetWorldTransform()
{
	if(m_transform_Dirty)
		UpdateTransformation();

	return m_worldTransform;
}

/*!
	Retrieves the inverse world transform for the entity.

	@return A reference to a CRuMatrix4x4 containing the entity's inverse world transform.
*/
const CRuMatrix4x4 &CRuEntity::GetInverseWorldTransform()
{
	if(m_transform_Dirty)
		UpdateTransformation();

	if(m_inverseTransform_Dirty)
	{
		// Clear dirty flag
		m_inverseTransform_Dirty = FALSE;

		// Calculate inverse world transform
		m_inverseWorldTransform = m_worldTransform.GetInverse();
	}

	return m_inverseWorldTransform;
}

const CRuMatrix4x4 &CRuEntity::GetLocalTransform()
{
	if(m_transform_Dirty)
		UpdateTransformation();

	return m_localTransform;
}

void CRuEntity::ResetLocalTransform()
{
	m_transform_Local_Scale = 1.0f;
	m_transform_Local_Rotation.Set(0.0f, 0.0f, 0.0f, 1.0f);
	m_transform_Local_Translation.Set(0.0f, 0.0f, 0.0f);

	// Invalidate entity's transform chain
	this->InvalidateTransformChain();
}

void CRuEntity::ResetAllTransforms()
{
	m_transform_Local_Scale = 1.0f;
	m_transform_Local_Rotation.Set(0.0f, 0.0f, 0.0f, 1.0f);
	m_transform_Local_Translation.Set(0.0f, 0.0f, 0.0f);

	// Invalidate entity's transform chain
	this->InvalidateTransformChain();
}

void CRuEntity::ResetLocal3x3Transform()
{
	m_transform_Local_Scale = 1.0f;
	m_transform_Local_Rotation.Set(0.0f, 0.0f, 0.0f, 1.0f);

	// Invalidate entity's transform chain
	this->InvalidateTransformChain();
}

void CRuEntity::SetLocalTransform(const CRuMatrix4x4 &transform)
{
	// Decompose orientation transform
	CRuVector3 scale, shear, rotation, translation;
	CRuQuaternion rotationQuat;

	transform.GetDecomposition(scale, shear, rotation, translation);
	rotationQuat.MakeEulerRotation(rotation);

	REAL effectiveScale = max(max(scale.m_x, scale.m_y), scale.m_z);

	m_transform_Local_Scale = effectiveScale;
	m_transform_Local_Rotation = rotationQuat;
	m_transform_Local_Translation = translation;

	// Invalidate entity's transform chain
	this->InvalidateTransformChain();
}

void CRuEntity::SetLocal3x3Transform(const CRuMatrix4x4 &transform)
{
	// Decompose orientation transform
	CRuVector3 scale, shear, rotation, translation;
	CRuQuaternion rotationQuat;

	transform.GetDecomposition(scale, shear, rotation, translation);
	rotationQuat.MakeEulerRotation(rotation);

	REAL effectiveScale = max(max(scale.m_x, scale.m_y), scale.m_z);

	m_transform_Local_Scale = effectiveScale;
	m_transform_Local_Rotation = rotationQuat;

	// Invalidate entity's transform chain
	this->InvalidateTransformChain();
}

void CRuEntity::Scale(const CRuVector3 &scale, RuTransformOp transformOp)
{
	this->Scale(max(max(scale.m_x, scale.m_y), scale.m_z), transformOp);
}

void CRuEntity::Scale(REAL scale, RuTransformOp transformOp)
{
	switch(transformOp)
	{
		case ruTRANSFORMOP_SET:
			{
				m_transform_Local_Scale = scale;
			}

			break;

		case ruTRANSFORMOP_PRECONCAT:
			{
				m_transform_Local_Scale = m_transform_Local_Scale * scale;
			}

			break;

		case ruTRANSFORMOP_POSTCONCAT:
			{
				m_transform_Local_Scale = m_transform_Local_Scale * scale;
			}

			break;
	}

	// Invalidate entity's transform chain
	this->InvalidateTransformChain();
}

void CRuEntity::Rotate(const CRuQuaternion &quat, RuTransformOp transformOp)
{
	switch(transformOp)
	{
		case ruTRANSFORMOP_SET:
			{
				m_transform_Local_Rotation = quat;
			}

			break;

		case ruTRANSFORMOP_PRECONCAT:
			{
				m_transform_Local_Rotation = quat * m_transform_Local_Rotation;
				quat.TransformVector(m_transform_Local_Translation);
			}

			break;

		case ruTRANSFORMOP_POSTCONCAT:
			{
				m_transform_Local_Rotation = m_transform_Local_Rotation * quat;
			}

			break;
	}

	// Invalidate entity's transform chain
	this->InvalidateTransformChain();
}

void CRuEntity::Translate(const CRuVector3 &translation, RuTransformOp transformOp)
{
	switch(transformOp)
	{
		case ruTRANSFORMOP_SET:
			{
				m_transform_Local_Translation = translation;
			}

			break;

		case ruTRANSFORMOP_PRECONCAT:
			{
				REAL s1 = m_transform_Local_Scale;

				CRuQuaternion r1 = m_transform_Local_Rotation;

				CRuVector3 t0 = translation;
				CRuVector3 t1 = m_transform_Local_Translation;

				m_transform_Local_Translation = t1 + r1.TransformVector(t0 * s1);
			}

			break;

		case ruTRANSFORMOP_POSTCONCAT:
			{
				CRuVector3 t0 = m_transform_Local_Translation;
				CRuVector3 t1 = translation;

				m_transform_Local_Translation = t1 + t0;
			}

			break;
	}

	// Invalidate entity's transform chain
	this->InvalidateTransformChain();
}

void CRuEntity::Transform(const CRuMatrix4x4 &transform, RuTransformOp transformOp)
{
	// Decompose orientation transform
	CRuVector3 scale, shear, rotation, translation;
	CRuQuaternion rotationQuat;

	transform.GetDecomposition(scale, shear, rotation, translation);
	rotationQuat.MakeEulerRotation(rotation);

	REAL effectiveScale = max(max(scale.m_x, scale.m_y), scale.m_z);

	switch(transformOp)
	{
		case ruTRANSFORMOP_SET:
			{
				m_transform_Local_Scale = effectiveScale;
				m_transform_Local_Rotation = rotationQuat;
				m_transform_Local_Translation = translation;
			}

			break;

		case ruTRANSFORMOP_PRECONCAT:
			{
				REAL s0 = effectiveScale;
				REAL s1 = m_transform_Local_Scale;

				CRuQuaternion r0 = rotationQuat;
				CRuQuaternion r1 = m_transform_Local_Rotation;

				CRuVector3 t0 = translation;
				CRuVector3 t1 = m_transform_Local_Translation;

				m_transform_Local_Scale = s0 * s1;
				m_transform_Local_Rotation = r0 * r1;
				m_transform_Local_Translation = t1 + r1.TransformVector(t0 * s1);
			}

			break;

		case ruTRANSFORMOP_POSTCONCAT:
			{
				REAL s0 = m_transform_Local_Scale;
				REAL s1 = effectiveScale;

				CRuQuaternion r0 = m_transform_Local_Rotation;
				CRuQuaternion r1 = rotationQuat;

				CRuVector3 t0 = m_transform_Local_Translation;
				CRuVector3 t1 = translation;

				m_transform_Local_Scale = s0 * s1;
				m_transform_Local_Rotation = r0 * r1;
				m_transform_Local_Translation = t1 + r1.TransformVector(t0 * s1);
			}

			break;
	}

	// Invalidate entity's transform chain
	this->InvalidateTransformChain();
}

void CRuEntity::SetTranslation(const CRuVector3 &translation)
{
	m_transform_Local_Translation = translation;

	// Invalidate entity's transform chain
	this->InvalidateTransformChain();
}

REAL CRuEntity::GetScale(RuFrameType frameType)
{
	switch(frameType)
	{
		case ruFRAMETYPE_WORLD:
			{
				if(m_transform_Dirty)
					UpdateTransformation();

				return m_transform_World_Scale;
			}

		case ruFRAMETYPE_LOCAL:
			{
				return m_transform_Local_Scale;
			}
	}

	return m_transform_Local_Scale;
}

const CRuQuaternion &CRuEntity::GetRotation(RuFrameType frameType)
{
	switch(frameType)
	{
		case ruFRAMETYPE_WORLD:
			{
				if(m_transform_Dirty)
					UpdateTransformation();

				return m_transform_World_Rotation;
			}

		case ruFRAMETYPE_LOCAL:
			{
				return m_transform_Local_Rotation;
			}
	}

	return m_transform_Local_Rotation;
}

const CRuVector3 &CRuEntity::GetTranslation(RuFrameType frameType)
{
	switch(frameType)
	{
		case ruFRAMETYPE_WORLD:
			{
				if(m_transform_Dirty)
					UpdateTransformation();

				return m_transform_World_Translation;
			}

		case ruFRAMETYPE_LOCAL:
			{
				return m_transform_Local_Translation;
			}
	}

	return m_transform_Local_Translation;
}

void CRuEntity::MarkLocalEntityBoundsDirty()
{
	// Mark local entity bounds as dirty
	m_bounds_EntityDirty = TRUE;

	// Force bounding update
	m_timeToNextLocalBoundUpdate = 0.0f;

	// Invalidate entity's bounding chain
	this->InvalidateBoundingChain();
}

void CRuEntity::SetLocalEntityBounds(const CRuSphere &localEntityBounds)
{
	m_bounds_Entity = localEntityBounds;

	// Invalidate entity's bounding chain
	this->InvalidateBoundingChain();
}

const CRuSphere &CRuEntity::GetLocalEntityBounds()
{
	CRuEntity *ancestor = this;

	while(ancestor->GetOption(ruENTITYOPTION_HRDISABLE) && ancestor->m_entityParent)
	{
		ancestor = ancestor->m_entityParent;
	}

	ancestor->UpdateEntityBoundingInformation();

	return ancestor->m_bounds_Entity;
}

const CRuSphere &CRuEntity::GetLocalBounds()
{
	CRuEntity *ancestor = this;

	while(ancestor->GetOption(ruENTITYOPTION_HRDISABLE) && ancestor->m_entityParent)
	{
		ancestor = ancestor->m_entityParent;
	}

	ancestor->UpdateBoundingInformation();

	return ancestor->m_bounds_Local;
}

const CRuSphere &CRuEntity::GetWorldBounds()
{
	CRuEntity *ancestor = this;

	while(ancestor->GetOption(ruENTITYOPTION_HRDISABLE) && ancestor->m_entityParent)
	{
		ancestor = ancestor->m_entityParent;
	}

	ancestor->UpdateBoundingInformation();

	return ancestor->m_bounds_World;
}

void CRuEntity::UpdateTransformation()
{
	RUPROFILE_SAMPLE_BEGIN(g_ruProbe_TransformUpdateTime, 4);

	if(m_localTransform_Dirty)
	{
		// Mark local transform as not dirty
		m_localTransform_Dirty = FALSE;

		// Update local transform matrix
		m_localTransform.SetToBoneSRT(m_transform_Local_Scale, m_transform_Local_Rotation, m_transform_Local_Translation);
	}

	// Update world transform
	RuEntityTransformMode transformMode = static_cast<RuEntityTransformMode>(GetOption(ruENTITYOPTION_TRANSFORMMODE));

	if(m_entityParent != NULL && transformMode != ruENTITYTRANSFORMMODE_OVERRIDE)
	{
		REAL parentScale = m_entityParent->GetScale(ruFRAMETYPE_WORLD);
		const CRuQuaternion &parentRotation = m_entityParent->GetRotation(ruFRAMETYPE_WORLD);
		const CRuVector3 &parentTranslation = m_entityParent->GetTranslation(ruFRAMETYPE_WORLD);

		switch(transformMode)
		{
			default:
			case ruENTITYTRANSFORMMODE_NORMAL:
				{
					if(m_transform_Local_Scale == 1.0f && m_transform_Local_Rotation.IsIdentity() && m_transform_Local_Translation.m_x == 0.0f && m_transform_Local_Translation.m_y == 0.0f && m_transform_Local_Translation.m_z == 0.0f)
					{
						m_transform_World_Scale = parentScale;
						m_transform_World_Rotation = parentRotation;
						m_transform_World_Translation = parentTranslation;

						m_worldTransform = m_entityParent->GetWorldTransform();
					}
					else
					{
						m_transform_World_Scale = m_transform_Local_Scale * parentScale;
						m_transform_World_Rotation = m_transform_Local_Rotation * parentRotation;
						m_transform_World_Translation = parentRotation.TransformVector(m_transform_Local_Translation * parentScale) + parentTranslation;

						// Update world transform matrix
						m_worldTransform.SetToBoneSRT(m_transform_World_Scale, m_transform_World_Rotation, m_transform_World_Translation);
					}
				}
				break;

			case ruENTITYTRANSFORMMODE_NOROTATE:
				{
					m_transform_World_Scale = m_transform_Local_Scale * parentScale;
					m_transform_World_Rotation = m_transform_Local_Rotation;
					m_transform_World_Translation = m_transform_Local_Translation * m_transform_World_Scale + parentTranslation;

					// Update world transform matrix
					m_worldTransform.SetToBoneSRT(m_transform_World_Scale, m_transform_World_Rotation, m_transform_World_Translation);
				}
				break;
		}
	}
	else
	{
		// Update source transforms for world transformation
		m_transform_World_Scale = m_transform_Local_Scale;
		m_transform_World_Rotation = m_transform_Local_Rotation;
		m_transform_World_Translation = m_transform_Local_Translation;

		// Update world transform matrix
		m_worldTransform = m_localTransform;
	}

	// Reset transform dirty flag
	m_transform_Dirty = FALSE;

	RUPROFILE_SAMPLE_END(g_ruProbe_TransformUpdateTime, 4);

	// Profiling
	RUPROFILE_SAMPLE_ACCUMULATE(g_ruProbe_TransformUpdateCount, 2, 1.0f);
}

void CRuEntity::UpdateBoundingInformation()
{
	if(m_bounds_Dirty || m_bounds_EntityDirty)
	{
		RUPROFILE_SAMPLE_BEGIN(g_ruProbe_BoundingUpdateTime, 3);

		if(m_timeToNextLocalBoundUpdate < ruEPSILON)
		{
			// Reset bounds dirty flag
			m_bounds_Dirty = FALSE;

			// Reset time to next local bound update
			m_timeToNextLocalBoundUpdate = 0.250f;

			// Set local bounds equal to entity bounds
			m_bounds_Local = this->GetLocalEntityBounds();

			// Combine local bounds with local bounds of children...
			CRuEntity *curChild = m_firstChild;
			BOOL firstEffectiveChild = TRUE;
			while(curChild)
			{
				if(curChild->GetOption(ruENTITYOPTION_HRDISABLE) == FALSE)
				{
					// Transform child's local bounds by its local transform (alter this later)
					CRuSphere localChildBounds = curChild->GetLocalBounds();

					switch(curChild->GetOption(ruENTITYOPTION_TRANSFORMMODE))
					{
						default:
						case ruENTITYTRANSFORMMODE_NORMAL:
						case ruENTITYTRANSFORMMODE_NOROTATE:
							{
								curChild->GetLocalTransform().TransformPoint(localChildBounds.Center());
								localChildBounds.Radius() *= curChild->GetScale();
							}

							break;

						case ruENTITYTRANSFORMMODE_OVERRIDE:
							{
								// If the child has overridden its world transform, transform its world bounds into local space of the parent
								localChildBounds = curChild->GetWorldBounds();
								this->GetInverseWorldTransform().TransformPoint(localChildBounds.Center());
								localChildBounds.Radius() *= 1.0f / this->GetScale(ruFRAMETYPE_WORLD);
							}

							break;
					}

					// Merge child's local bounds into aggregate local bounds
					if(firstEffectiveChild && this->GetOption(ruENTITYOPTION_EXCLUDEBOUNDS))
					{
						m_bounds_Local = localChildBounds;
					}
					else
					{
						m_bounds_Local.Merge(localChildBounds);
					}

					firstEffectiveChild = FALSE;
				}

				// Advance to next child
				curChild = curChild->m_nextSibling;
			}
		}

		// Calculate aggregate world bounds
		this->GetWorldTransform().TransformPoint(m_bounds_Local.Center(), m_bounds_World.Center());
		m_bounds_World.Radius() = m_bounds_Local.Radius() * this->GetScale(ruFRAMETYPE_WORLD);

		RUPROFILE_SAMPLE_END(g_ruProbe_BoundingUpdateTime, 3);

		// Profiling
		RUPROFILE_SAMPLE_ACCUMULATE(g_ruProbe_BoundingUpdateCount, 2, 1.0f);
	}
}

void CRuEntity::UpdateEntityBoundingInformation()
{
	if(m_bounds_EntityDirty)
	{
		// Call subclass update function
		this->CalculateEntityBounds();

		// Reset entity bounds dirty flag
		m_bounds_EntityDirty = FALSE;
	}
}

void CRuEntity::CalculateEntityBounds()
{
}

/*!
	Performs a collision query on the entity. Default implementation returns immediately.

	@param colQuery Collision query.

	@retval TRUE The operation succeeded.
	@retval FALSE The operation failed.
*/
BOOL CRuEntity::QueryCollision(CRuCollisionQuery *colQuery)
{
	// If collision query is disabled, return immediately
	if(colQuery->m_ignoreCollisionFlag == FALSE && GetOption(ruENTITYOPTION_COLLISION) == 0)
		return TRUE;

	// Save previous collision transform
	CRuMatrix4x4 *originalWorldTransform = colQuery->m_worldTransform;
	CRuMatrix4x4 *originalInvWorldTransform = colQuery->m_invWorldTransform;

	// Store the inverse of the current world transform into the collision query
	colQuery->m_worldTransform = const_cast<CRuMatrix4x4 *>(&this->GetWorldTransform());
	colQuery->m_invWorldTransform = const_cast<CRuMatrix4x4 *>(&this->GetInverseWorldTransform());

	// Return value
	BOOL retVal = TRUE;

	if(m_collisionObject && !colQuery->m_queryOnOriginalGeometry)
	{
		// Query attached collision object
		m_collisionObject->QueryCollision(colQuery);
	}
	else
	{
		// Query collision on all of the children
		CRuEntity *curChild = this->GetFirstChild();
		while(curChild)
		{
			// Invoke transform update on child entity
			if(curChild->QueryCollision(colQuery) == FALSE)
			{
				retVal = FALSE;
				break;
			}

			// Advance to next sibling
			curChild = curChild->GetNextSibling();
		}
	}

	// Restore original collision transform
	colQuery->m_worldTransform = originalWorldTransform;
	colQuery->m_invWorldTransform = originalInvWorldTransform;

	return retVal;
}

BOOL CRuEntity::HandleEvent_Trigger_Propagate(RuEventArgs *eventArgs)
{
	// Propagate the event to parent entity
	if(m_entityParent)
	{
		m_entityParent->Event_Trigger().Trigger(eventArgs);
	}

	return TRUE;
}

void CRuEntity::InvalidateTransformChain(BOOL invalidationRoot)
{
	// Invalidate transform at this node
	this->m_localTransform_Dirty = invalidationRoot ? TRUE : this->m_localTransform_Dirty;
	this->m_transform_Dirty = TRUE;
	this->m_inverseTransform_Dirty = TRUE;

	// Invalidate transforms down the tree
	CRuEntity *curChild = this->GetFirstChild();
	while(curChild)
	{
		// Recursively invoke on child
		curChild->InvalidateTransformChain(FALSE);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	this->InvalidateBoundingChain();
}

void CRuEntity::InvalidateBoundingChain()
{
	if(this->GetOption(ruENTITYOPTION_HRDISABLE) == FALSE)
	{
		// Invalidate bounding volume at this node
		this->m_bounds_Dirty = TRUE;

		// Invalidate bounding volumes up the tree
		CRuEntity *curNode = this->GetParent();
		while(curNode)
		{
			curNode->m_bounds_Dirty = TRUE;
			curNode = curNode->GetParent();
		}
	}
}
/*
void CRuEntity::InvalidateTransformChain(BOOL transform, BOOL entityBounds, BOOL bounds)
{
	// Invalidate transforms up the tree
	CRuEntity *curNode = this;
	while(curNode)
	{
		if(transform)
		{
			curNode->m_transform_Dirty = TRUE;
		}

		if(entityBounds)
		{
			curNode->m_bounds_EntityDirty = TRUE;
		}

		if(bounds)
		{
			curNode->m_bounds_Dirty = TRUE;
		}

		curNode = curNode->GetParent();
	}

	// Invaludate transforms down the tree
	this->PropagateInvalidationDown(transform, entityBounds, bounds);
}

void CRuEntity::PropagateInvalidationUp(BOOL transform, BOOL entityBounds, BOOL bounds)
{
	// Invalidate transforms up the tree
	CRuEntity *curNode = this;
	while(curNode)
	{
		if(transform)
		{
			curNode->m_transform_Dirty = TRUE;
		}

		if(entityBounds)
		{
			curNode->m_bounds_EntityDirty = TRUE;
		}

		if(bounds)
		{
			curNode->m_bounds_Dirty = TRUE;
		}

		curNode = curNode->GetParent();
	}

	CRuEntity *curChild = this->GetFirstChild();
	while(curChild)
	{
		if(transform)
		{
			curChild->m_transform_Dirty = TRUE;
		}

		if(entityBounds)
		{
			curChild->m_bounds_EntityDirty = TRUE;
		}

		if(bounds)
		{
			curChild->m_bounds_Dirty = TRUE;
		}

		// Recursively invoke on child
		curChild->PropagateInvalidationDown(transform, entityBounds, bounds);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}
}

void CRuEntity::PropagateInvalidationDown(BOOL transform, BOOL entityBounds, BOOL bounds)
{
	CRuEntity *curChild = this->GetFirstChild();
	while(curChild)
	{
		if(transform)
		{
			curChild->m_transform_Dirty = TRUE;
		}

		if(entityBounds)
		{
			curChild->m_bounds_EntityDirty = TRUE;
		}

		if(bounds)
		{
			curChild->m_bounds_Dirty = TRUE;
		}

		// Recursively invoke on child
		curChild->PropagateInvalidationDown(transform, entityBounds, bounds);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}
}
*/
// ************************************************************************************************************************************************************

CRuEntity_Light::CRuEntity_Light()
{
}

CRuEntity_Light::~CRuEntity_Light()
{
}

BOOL CRuEntity_Light::SerializeFrom(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuEntity_Light::SerializeTo(IRuStream *stream)
{
	return TRUE;
}

const RuLightSettings &CRuEntity_Light::GetLight() const
{
	return m_light;
}

void CRuEntity_Light::SetLight(const RuLightSettings &light)
{
	m_light = light;
	UpdateLightBounds();
}

RuLightType CRuEntity_Light::GetLightType() const
{
	return m_light.m_type;
}

void CRuEntity_Light::SetLightType(RuLightType lightType)
{
	m_light.m_type = lightType;
	UpdateLightBounds();
}

const CRuColor4 &CRuEntity_Light::GetDiffuse() const
{
	return m_light.m_diffuse;
}

void CRuEntity_Light::SetDiffuse(const CRuColor4 &diffuse)
{
	m_light.m_diffuse = diffuse;
}

const CRuColor4 &CRuEntity_Light::GetSpecular() const
{
	return m_light.m_specular;
}

void CRuEntity_Light::SetSpecular(const CRuColor4 &specular)
{
	m_light.m_specular = specular;
}

const CRuVector3 &CRuEntity_Light::GetPosition() const
{
	return m_light.m_position;
}

void CRuEntity_Light::SetPosition(const CRuVector3 &position)
{
	m_light.m_position = position;
	UpdateLightBounds();
}

const CRuVector3 &CRuEntity_Light::GetDirection() const
{
	return m_light.m_direction;
}

void CRuEntity_Light::SetDirection(const CRuVector3 &direction)
{
	m_light.m_direction = direction;
}

REAL CRuEntity_Light::GetRange() const
{
	return m_light.m_range;
}

void CRuEntity_Light::SetRange(REAL range)
{
	m_light.m_range = range;
	UpdateLightBounds();
}

REAL CRuEntity_Light::GetFalloff() const
{
	return m_light.m_falloff;
}

void CRuEntity_Light::SetFalloff(REAL falloff)
{
	m_light.m_falloff = falloff;
}

REAL CRuEntity_Light::GetAttenuation0() const
{
	return m_light.m_attenuation0;
}

void CRuEntity_Light::SetAttenuation0(REAL attenuation0)
{
	m_light.m_attenuation0 = attenuation0;
}

REAL CRuEntity_Light::GetAttenuation1() const
{
	return m_light.m_attenuation1;
}

void CRuEntity_Light::SetAttenuation1(REAL attenuation1)
{
	m_light.m_attenuation1 = attenuation1;
}

REAL CRuEntity_Light::GetAttenuation2() const
{
	return m_light.m_attenuation2;
}

void CRuEntity_Light::SetAttenuation2(REAL attenuation2)
{
	m_light.m_attenuation2 = attenuation2;
}

REAL CRuEntity_Light::GetTheta() const
{
	return m_light.m_theta;
}

void CRuEntity_Light::SetTheta(REAL theta)
{
	m_light.m_theta = theta;
}

REAL CRuEntity_Light::GetPhi() const
{
	return m_light.m_phi;
}

void CRuEntity_Light::SetPhi(REAL phi)
{
	m_light.m_phi = phi;
}

void CRuEntity_Light::UpdateLightBounds()
{
	// Alter bounding sphere of the light
	switch(m_light.m_type)
	{
		case ruLIGHTTYPE_AMBIENT:
		case ruLIGHTTYPE_DIRECTIONAL:
			this->SetLocalEntityBounds(CRuSphere(CRuVector3(0.0f, 0.0f, 0.0f), ruMAXFLOATRANGE));

			break;

		default:
			this->SetLocalEntityBounds(CRuSphere(m_light.m_position, m_light.m_range));

			break;
	}
}

// ************************************************************************************************************************************************************

IRuEntity_Container::IRuEntity_Container()
{
}

IRuEntity_Container::~IRuEntity_Container()
{
}

INT32 IRuEntity_Container::GetNumEntities()
{
	return 0;
}

CRuEntity *IRuEntity_Container::GetEntity(INT32 index)
{
	return NULL;
}

// ************************************************************************************************************************************************************

IRuEntity_Container_Mutable::IRuEntity_Container_Mutable()
{
}

IRuEntity_Container_Mutable::~IRuEntity_Container_Mutable()
{
}

void IRuEntity_Container_Mutable::AddEntity(CRuEntity *entity)
{
}

BOOL IRuEntity_Container_Mutable::RemoveEntity(CRuEntity *entity)
{
	return FALSE;
}

BOOL IRuEntity_Container_Mutable::RemoveEntityAt(INT32 index)
{
	return FALSE;
}

BOOL IRuEntity_Container_Mutable::ClearEntities()
{
	return FALSE;
}

// ************************************************************************************************************************************************************

CRuEntity_Container_Impl::CRuEntity_Container_Impl()
{
}

CRuEntity_Container_Impl::~CRuEntity_Container_Impl()
{
}

IRuObject *CRuEntity_Container_Impl::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuEntity_Container_Impl();
	CRuEntity_Container_Impl *clonedContainer = static_cast<CRuEntity_Container_Impl *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_Container_Mutable::Clone(clonedObject);

	return clonedContainer;
}

BOOL CRuEntity_Container_Impl::SerializeFrom(IRuStream *stream)
{
	// Read class GUID
	CRuGUID classGUID;
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Container_Mutable::SerializeFrom(stream);

	// Read number of entities
	INT32 entityCount;
	stream->Read(&entityCount, sizeof(INT32));

	// Read each entity held by the container
	for(INT32 i = 0; i < entityCount; ++i)
	{
		IRuObject *object = RuObjectFactory().CreateObject(stream);

		if(object && object->GetType().IsSubClassOf(CRuEntity::Type()))
		{
			AddChild(static_cast<CRuEntity *>(object));
		}

		ruSAFE_RELEASE(object);
	}

	return TRUE;
}

BOOL CRuEntity_Container_Impl::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Container_Mutable::SerializeTo(stream);

	// Write number of entities
	INT32 entityCount = this->GetNumEntities();
	stream->Write(&entityCount, sizeof(INT32));

	// Write each entity held by the container
	CRuEntity *curChild = this->GetFirstChild();
	while(curChild)
	{
		// Serialize node only if it is not marked no-serialize
		if(curChild->GetOption(ruENTITYOPTION_NOSERIALIZE) == FALSE)
		{
			curChild->SerializeTo(stream);
		}

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

INT32 CRuEntity_Container_Impl::GetNumEntities()
{
	INT32 numEntities = 0;
	CRuEntity *curChild = this->GetFirstChild();

	while(curChild)
	{
		++numEntities;
		curChild = curChild->GetNextSibling();
	}

	return numEntities;
}

CRuEntity *CRuEntity_Container_Impl::GetEntity(INT32 index)
{
	INT32 entityIndex = 0;
	CRuEntity *curChild = this->GetFirstChild();

	while(curChild)
	{
		if(entityIndex == index)
		{
			return curChild;
		}

		++entityIndex;
		curChild = curChild->GetNextSibling();
	}

	return NULL;
}

void CRuEntity_Container_Impl::AddEntity(CRuEntity *entity)
{
	AddChild(entity);
}

BOOL CRuEntity_Container_Impl::RemoveEntity(CRuEntity *entity)
{
	return RemoveChild(entity);
}

BOOL CRuEntity_Container_Impl::RemoveEntityAt(INT32 index)
{
	INT32 entityIndex = 0;
	CRuEntity *curChild = this->GetFirstChild();

	while(curChild)
	{
		if(entityIndex == index)
		{
			return this->RemoveChild(curChild);
		}

		++entityIndex;
		curChild = curChild->GetNextSibling();
	}

	return FALSE;
}

BOOL CRuEntity_Container_Impl::ClearEntities()
{
	return this->RemoveAllChildren();
}

// ************************************************************************************************************************************************************

IRuEntity_Renderable::IRuEntity_Renderable()
{
}

IRuEntity_Renderable::~IRuEntity_Renderable()
{
}

INT32 IRuEntity_Renderable::GetNumMeshes() const
{
	return 0;
}

BOOL IRuEntity_Renderable::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	return FALSE;
}

// ************************************************************************************************************************************************************

IRuEntity_Controller::IRuEntity_Controller()
:	m_description(NULL),
	m_lifetime(-1.0f),
	m_age(0.0f),
	m_startTime(0.0f),
	m_duration(0.0f),
	m_loopTime(-1.0f),
	m_enabled(TRUE),
	m_overrideClockUpdate(FALSE)
{
}

IRuEntity_Controller::~IRuEntity_Controller()
{
	delete[] m_description;
}

const char *IRuEntity_Controller::GetDescription()
{
	if(m_description)
	{
		return m_description;
	}

	return "";
}

void IRuEntity_Controller::SetDescription(const char *description)
{
	delete[] m_description;

	m_description = ruNEW char [strlen(description) + 1];
	strcpy(m_description, description);
}

void IRuEntity_Controller::SetSelfDestructTimer(REAL lifetime)
{
	m_lifetime = lifetime;
}

void IRuEntity_Controller::Enable(BOOL enable)
{
	m_enabled = enable;
}

void IRuEntity_Controller::SelfDestruct()
{
	// If this controller has been attached to something, remove the link and release the entity
	this->DetachFromParent();

	// Release controller
	this->Release();
}

IRuObject *IRuEntity_Controller::Clone(IRuObject *clonedObject)
{
	if(clonedObject == NULL)
		return NULL;

	IRuEntity_Controller *controller = static_cast<IRuEntity_Controller *>(clonedObject);

	// Chain cloning call to parent class
	CRuEntity::Clone(clonedObject);

	// Clone basic parameters
	controller->m_lifetime = m_lifetime;
	controller->m_startTime = m_startTime;
	controller->m_duration = m_duration;
	controller->m_loopTime = m_loopTime;
	controller->m_enabled = m_enabled;

	return clonedObject;
}

BOOL IRuEntity_Controller::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Read controller parameters
	stream->Read(&m_lifetime, sizeof(REAL));
	stream->Read(&m_startTime, sizeof(REAL));
	stream->Read(&m_duration, sizeof(REAL));
	stream->Read(&m_loopTime, sizeof(REAL));
	stream->Read(&m_enabled, sizeof(BOOL));

	return TRUE;
}

BOOL IRuEntity_Controller::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Write controller parameters
	stream->Write(&m_lifetime, sizeof(REAL));
	stream->Write(&m_startTime, sizeof(REAL));
	stream->Write(&m_duration, sizeof(REAL));
	stream->Write(&m_loopTime, sizeof(REAL));
	stream->Write(&m_enabled, sizeof(BOOL));

	return TRUE;
}

BOOL IRuEntity_Controller::Update(REAL elapsedTime)
{
	if(m_overrideClockUpdate == FALSE)
	{
		m_age += elapsedTime;
	}

	// If the lifetime has been exceeded, release the controller
	if(m_lifetime > 0.0f && m_age >= m_lifetime)
	{
		SelfDestruct();
	}

	return TRUE;
}

REAL IRuEntity_Controller::GetStartTime()
{
	return m_startTime;;
}

REAL IRuEntity_Controller::SetStartTime(REAL startTime)
{
	m_startTime = startTime;
	return m_startTime;
}

REAL IRuEntity_Controller::GetDuration()
{
	return m_duration;
}

REAL IRuEntity_Controller::SetDuration(REAL duration)
{
	m_duration = duration;
	return m_duration;
}

REAL IRuEntity_Controller::GetLoopTime()
{
	return m_loopTime;
}

REAL IRuEntity_Controller::SetLoopTime(REAL loopTime)
{
	m_loopTime = loopTime;
	return m_loopTime;
}

REAL IRuEntity_Controller::GetCurrentTime() const
{
	return m_age;
}

BOOL IRuEntity_Controller::HasExpired() const
{
	if(m_loopTime < 0.0f)
		return (m_age >= m_startTime + m_duration);
	else
		return FALSE;
}

BOOL IRuEntity_Controller::IsLooping() const
{
	return m_loopTime >= 0.0f;
}

IRuObject *IRuEntity_Controller::GetTarget()
{
	return NULL;
}

BOOL IRuEntity_Controller::SetTarget(IRuObject *target)
{
	return FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed
