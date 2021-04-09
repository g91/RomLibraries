/*!
	@file RuEntity_Proxy.cpp

	Copyright (c) 2004-2008 All rights reserved
*/

#include "../../Scene/Proxy/RuEntity_Proxy.h"
#include "../../Helper/RuHelper_Entity.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Proxy, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Proxy, "CRuEntity_Proxy", "CRuEntity")
ruCLASSGUID_DEFINE(CRuEntity_Proxy, 0x94FC6602, 0x78C84f81, 0xAC552C11, 0x6400EA5E)

// ************************************************************************************************************************************************************

CRuEntity_Proxy::CRuEntity_Proxy()
:	m_proxiedEntity(NULL)
{
}

CRuEntity_Proxy::~CRuEntity_Proxy()
{
	ruSAFE_RELEASE(m_proxiedEntity);
}

IRuObject *CRuEntity_Proxy::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuEntity_Proxy();

	// Chain cloning call to parent class
	CRuEntity::Clone(clonedObject);

	// Shortcut
	CRuEntity_Proxy *clonedEntityProxy = static_cast<CRuEntity_Proxy *>(clonedObject);

	return clonedObject;
}

BOOL CRuEntity_Proxy::SerializeFrom(IRuStream *stream)
{
	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuEntity::SerializeFrom(stream);

	return TRUE;
}

BOOL CRuEntity_Proxy::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuEntity::SerializeTo(stream);

	return TRUE;
}

BOOL CRuEntity_Proxy::Update(REAL elapsedTime)
{
	// Attempt to exit load phase
	ExitLoadPhase();

	// Chain update to base class
	return CRuEntity::Update(elapsedTime);
}

BOOL CRuEntity_Proxy::LoadEntity(const char *resourceName)
{
	// Detach & release proxied entity
	ruSAFE_DETACH(m_proxiedEntity);
	ruSAFE_RELEASE(m_proxiedEntity);

	// Store proxied entity name as the resource name
	this->SetResourceName(resourceName);

	// Attempt to exit load phase immediately
	ExitLoadPhase();

	return TRUE;
}

CRuEvent &CRuEntity_Proxy::Event_LoadComplete()
{
	return m_event_LoadComplete;
}

void CRuEntity_Proxy::ExitLoadPhase()
{
	// Load the target entity
	if(!m_proxiedEntity)
	{
		const char *resourceName = this->GetResourceName();

		if(resourceName && resourceName[0] != NULL)
		{
			BOOL loadResult = g_ruResourceManager->LoadEntity(resourceName, &m_proxiedEntity);
			if(loadResult == FALSE)
			{
				// Load failed, but trigger load complete event anyway
				RuEventArgs eventArgs;
				m_event_LoadComplete.Trigger(&eventArgs);
			}
			else if(m_proxiedEntity != NULL)
			{
				// Transfer paperdoll settings if applicable
				IRuPaperdoll *sourcePaperdoll = RuEntity_FindFirstPaperdoll(this);
				IRuPaperdoll *targetPaperdoll = RuEntity_FindFirstPaperdoll(m_proxiedEntity);

				if(sourcePaperdoll && targetPaperdoll)
				{
					CRuPaperdollTemplate *paperdollTemplate = targetPaperdoll->GetPaperdollTemplate();
					if(paperdollTemplate)
					{
						for(INT32 i = 0, numParts = paperdollTemplate->GetNumParts(); i < numParts; ++i)
						{
							// Set component name
							const char *partName = paperdollTemplate->GetPartName(i);
							const char *itemName = sourcePaperdoll->GetComponent(partName);
							targetPaperdoll->SetComponent(partName, itemName);

							// Set component colors
							BOOL layerActivated[4] = { FALSE, FALSE, FALSE, FALSE };
							RUCOLOR layerColor[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
							sourcePaperdoll->GetComponentColors(partName, layerActivated, layerColor);
							targetPaperdoll->SetComponentColors(partName, layerActivated, layerColor);
						}
					}
				}

				// Transfer all entities attached to link frames
				CRuEntity *curChild = this->GetFirstChild();
				while(curChild)
				{
					if(curChild->GetType().IsSubClassOf(CRuLinkFrame::Type()))
					{
						CRuEntity *attachedChild = curChild->GetFirstChild();
						while(attachedChild)
						{
							// Re-attach child to the proxied entity
							CRuEntity *targetLinkFrame = RuEntity_FindLinkFrame(m_proxiedEntity, static_cast<CRuLinkFrame *>(curChild)->GetLinkName());
							if(targetLinkFrame)
							{
								attachedChild->AddRef();
								attachedChild->DetachFromParent();
								targetLinkFrame->AddChild(attachedChild);
								ruSAFE_RELEASE(attachedChild);
							}
							else
							{
								attachedChild->DetachFromParent();
							}

							// Advance to next child
							attachedChild = curChild->GetFirstChild();
						}
					}

					// Advance to next sibling
					curChild = curChild->GetNextSibling();
				}

				// Remove all children from the entity tree
				curChild = this->GetFirstChild();
				while(curChild)
				{
					// Detach child from parent (this will destroy the child)
					curChild->DetachFromParent();

					// Advance to next child
					curChild = this->GetFirstChild();
				}

				// Attach proxied entity as sole child
				this->AddChild(m_proxiedEntity);

				// Trigger load complete event
				RuEventArgs eventArgs;
				m_event_LoadComplete.Trigger(&eventArgs);
			}
		}
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
