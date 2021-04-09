#include "../../Scene/Base/RuEntityACT.h"

#include "../../Scene/Controller/RuController_AnimCompensator.h"
#include "../../Scene/Controller/RuController_Entity.h"
#include "../../Scene/Controller/RuController_EventTrigger.h"
#include "../../Scene/Controller/RuController_GlobalAlpha.h"
#include "../../Scene/Controller/RuController_GlobalColor.h"
#include "../../Scene/Controller/RuController_Hierarchy.h"
#include "../../Scene/Controller/RuController_Morph.h"
#include "../../Scene/Controller/RuController_ParticleSystem.h"

#include "../../Helper/RuHelper_Entity.h"
#include "../../Helper/RuHelper_IRuEntity_Renderable.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuACT_Anchorable, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuACT_Anchorable, "CRuACT_Anchorable", "IRuObject")
ruCLASSGUID_DEFINE(CRuACT_Anchorable, 0xEEE44F31, 0x2D794a17, 0x8A95BF59, 0x6CB1F79B)

ruRTTI_DEFINE_SUBCLASS(CRuACTTemplateNode, CRuACT_Anchorable)
ruHEAP_DEFINE_SUBCLASS(CRuACTTemplateNode, "CRuACTTemplateNode", "CRuACT_Anchorable")
ruCLASSGUID_DEFINE(CRuACTTemplateNode, 0x0EE56AE9, 0x55B14f4e, 0xAF22A1EF, 0xC8D1FB7B)

ruRTTI_DEFINE_SUBCLASS(CRuACTTemplateModifier, CRuACT_Anchorable)
ruHEAP_DEFINE_SUBCLASS(CRuACTTemplateModifier, "CRuACTTemplateModifier", "CRuACT_Anchorable")
ruCLASSGUID_DEFINE(CRuACTTemplateModifier, 0xABE45A8B, 0xE5174429, 0x8AB9DC91, 0x7926B293)

ruRTTI_DEFINE_SUBCLASS(CRuACTMotionNode, CRuACT_Anchorable)
ruHEAP_DEFINE_SUBCLASS(CRuACTMotionNode, "CRuACTMotionNode", "CRuACT_Anchorable")
ruCLASSGUID_DEFINE(CRuACTMotionNode, 0xC6CB0FBD, 0xFE6D479c, 0x9800EFC7, 0x64894CB7)

ruRTTI_DEFINE_SUBCLASS(CRuACTMotion, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuACTMotion, "CRuACTMotion", "IRuObject")
ruCLASSGUID_DEFINE(CRuACTMotion, 0x1AAAE4D8, 0x06A34279, 0xA0EC4755, 0xF7D79787)

ruRTTI_DEFINE_SUBCLASS(CRuACTTemplate, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuACTTemplate, "CRuACTTemplate", "IRuObject")
ruCLASSGUID_DEFINE(CRuACTTemplate, 0xE743F431, 0x29EC4748, 0xB3A626CC, 0xF291DEB3)

ruRTTI_DEFINE_SUBCLASS(CRuACT_ActionEvents, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuACT_ActionEvents, "CRuACT_ActionEvents", "IRuObject")
ruCLASSGUID_DEFINE(CRuACT_ActionEvents, 0x1D08FE47, 0xD8784354, 0xAA25F4DF, 0x97AC021E)

ruRTTI_DEFINE_SUBCLASS(CRuACTEntity, CRuEntity_Container_Impl)
ruHEAP_DEFINE_SUBCLASS(CRuACTEntity, "CRuACTEntity", "CRuEntity_Container_Impl")
ruCLASSGUID_DEFINE(CRuACTEntity, 0xF003E140, 0xF1C24f03, 0xACCC780D, 0x01190100)

// ************************************************************************************************************************************************************

// Used for motion sorting function
static int StrCompare(const void *str0, const void *str1)
{
	return strcmp(* ( char** ) str0, * ( char** ) str1);
}

// NULL-safe _stricmp
static int SafeStricmp(const char *str0, const char *str1)
{
	if(str0 && str1)
	{
		return _stricmp(str0, str1);
	}

	if(str0 == NULL)
	{
		if(str1)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}

	return 1;
}

// ************************************************************************************************************************************************************

CRuACT_Anchorable::CRuACT_Anchorable()
:	m_anchorTarget(NULL)
{
}

CRuACT_Anchorable::~CRuACT_Anchorable()
{
	delete[] m_anchorTarget;
}

// IRuObject serialization
IRuObject *CRuACT_Anchorable::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuACT_Anchorable();

	// Chain cloning call to parent class
	IRuObject::Clone(clonedObject);

	// Shortcut reference to the cloned entity
	CRuACT_Anchorable *clonedAnchorable = static_cast<CRuACT_Anchorable *>(clonedObject);

	// Copy anchor settings
	clonedAnchorable->SetAnchorTarget(this->GetAnchorTarget());

	return clonedObject;
}

BOOL CRuACT_Anchorable::SerializeFrom(IRuStream *stream)
{
	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	delete[] m_anchorTarget;
	m_anchorTarget = NULL;

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to parent class
	IRuObject::SerializeFrom(stream);

	INT32 pos = stream->GetPosition();

	switch(version)
	{
		case 1:
			{
				// Read length of anchor name including NULL followed by the name
				UINT32 nameLength;
				stream->Read(&nameLength, sizeof(nameLength));

				if(nameLength >= 1)
				{
					m_anchorTarget = ruNEW char [nameLength];
					stream->Read(m_anchorTarget, sizeof(char) * nameLength);
				}
				else if(nameLength == 1)
				{
					char dummy;
					stream->Read(&dummy, sizeof(char));
				}

				INT32 dummy = 0;
				stream->Read(&dummy, sizeof(INT32));
				stream->Read(&dummy, sizeof(INT32));
			}

			break;
	}

	return TRUE;
}

BOOL CRuACT_Anchorable::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to parent class
	IRuObject::SerializeTo(stream);

	INT32 pos = stream->GetPosition();

	// Write length of anchor name including NULL followed by the name
	if(m_anchorTarget)
	{
		UINT32 nameLength = (UINT32)strlen(m_anchorTarget) + 1;

		stream->Write(&nameLength, sizeof(nameLength));
		stream->Write(m_anchorTarget, sizeof(char) * nameLength);
	}
	else
	{
		UINT32 nameLength = 0;

		stream->Write(&nameLength, sizeof(nameLength));
	}

	INT32 dummy = 0;
	stream->Write(&dummy, sizeof(INT32));
	stream->Write(&dummy, sizeof(INT32));

	return TRUE;
}

const char *CRuACT_Anchorable::GetAnchorTarget() const
{
	return m_anchorTarget;
}

void CRuACT_Anchorable::SetAnchorTarget(const char *anchorTarget)
{
	if(anchorTarget != m_anchorTarget)
	{
		delete[] m_anchorTarget;

		if(anchorTarget)
		{
			m_anchorTarget = ruNEW char [strlen(anchorTarget) + 1];
			strcpy(m_anchorTarget, anchorTarget);
		}
		else
		{
			m_anchorTarget = NULL;
		}

	}
}

// ************************************************************************************************************************************************************

CRuACTTemplateNode_Runtime::CRuACTTemplateNode_Runtime(CRuACTTemplateNode *srcTemplateNode, CRuArrayList<CRuEntity *> &interiorEntities)
:	m_sourceTemplateNode(NULL),
	m_numEntities(0),
	m_entities(NULL),
	m_entityAnchorPaths(NULL),
	m_anchorNode(NULL)
{
	srcTemplateNode->AddRef();
	m_sourceTemplateNode = srcTemplateNode;
	m_numEntities = interiorEntities.Count();
	m_entities = ruNEW CRuEntity * [m_numEntities];
	memcpy(m_entities, interiorEntities.GetArray(), sizeof(CRuEntity *) * m_numEntities);

	BuildAnchorPaths();
}

CRuACTTemplateNode_Runtime::~CRuACTTemplateNode_Runtime()
{
	ruSAFE_RELEASE(m_sourceTemplateNode);

	if(m_numEntities > 0 && m_entities)
	{
		// Release references to the root entity (This also releases all interior entities since we do not keep references for them)
		ruSAFE_RELEASE(m_entities[0]);

		// Delete constructed entity array
		delete[] m_entities;

		for(INT32 i = 0; i < m_numEntities; ++i)
		{
			delete[] m_entityAnchorPaths[i];
		}

		delete[] m_entityAnchorPaths;
	}

	m_numEntities = 0;
	m_entities = NULL;
	m_entityAnchorPaths = NULL;
}

RuACTTemplateNodeType CRuACTTemplateNode_Runtime::GetNodeType() const
{
	return m_sourceTemplateNode->GetNodeType();
}

const char *CRuACTTemplateNode_Runtime::GetName() const
{
	return m_sourceTemplateNode->GetName();
}

CRuACTTemplateNode *CRuACTTemplateNode_Runtime::GetTemplateNode()
{
	return m_sourceTemplateNode;
}

BOOL CRuACTTemplateNode_Runtime::AnchorTo(CRuACTTemplateNode_Runtime *anchorNode)
{
	if(anchorNode)
	{
		// Get the interior entity that will serve as the anchor
		const char *anchorPath = this->GetTemplateNode()->GetAnchorTarget();
		CRuEntity *anchorEntity = anchorNode->GetInteriorEntity(anchorPath);

		if(anchorEntity)
		{
			// Attach entity
			anchorEntity->AddChild(this->GetRootEntity());

			// Rebuild anchor paths (is this really necessary?)
			BuildAnchorPaths();

			// Store pointer to anchor node
			m_anchorNode = anchorNode;

			return TRUE;
		}
	}

	return FALSE;
}

INT32 CRuACTTemplateNode_Runtime::IndexOf(CRuEntity *entity) const
{
	for(INT32 i = 0; i < m_numEntities; ++i)
	{
		if(m_entities[i] == entity)
		{
			return i;
		}
	}

	return -1;
}

INT32 CRuACTTemplateNode_Runtime::GetNumEntities()
{
	return m_numEntities;
}

CRuEntity *CRuACTTemplateNode_Runtime::GetRootEntity()
{
	if(m_entities)
	{
		return m_entities[0];
	}

	return NULL;
}

const char *CRuACTTemplateNode_Runtime::GetRootAnchorName()
{
	if(m_entityAnchorPaths)
	{
		return m_entityAnchorPaths[0];
	}

	return NULL;
}

CRuEntity *CRuACTTemplateNode_Runtime::GetInteriorEntity(const char *anchorPath)
{
	if(anchorPath)
	{
		for(INT32 i = 0; i < m_numEntities; ++i)
		{
			if(SafeStricmp(m_entityAnchorPaths[i], anchorPath) == 0)
			{
				return m_entities[i];
			}
		}
	}

	return NULL;
}

const char *CRuACTTemplateNode_Runtime::GetInteriorAnchorName(CRuEntity *entity)
{
	for(INT32 i = 0; i < m_numEntities; ++i)
	{
		if(m_entities[i] == entity)
		{
			return m_entityAnchorPaths[i];
		}
	}

	return NULL;
}

CRuEntity *CRuACTTemplateNode_Runtime::GetInteriorEntity(INT32 interiorIndex)
{
	if(m_entities && interiorIndex >= 0 && interiorIndex < m_numEntities)
	{
		return m_entities[interiorIndex];
	}

	return NULL;
}

const char *CRuACTTemplateNode_Runtime::GetAnchorPath(INT32 interiorIndex)
{
	if(m_entityAnchorPaths && interiorIndex >= 0 && interiorIndex < m_numEntities)
	{
		return m_entityAnchorPaths[interiorIndex];
	}

	return NULL;
}

char *CRuACTTemplateNode_Runtime::GetReAnchoredRootAnchorName(const char *newAnchorName)
{
	CRuEntity *rootEntity = this->GetRootEntity();

	if(rootEntity)
	{
		const char *rootObjName = rootEntity->GetObjectName();

		char *reAnchoredRootAnchorName = ruNEW char [strlen(rootObjName) + strlen(newAnchorName) + 2];
		sprintf(reAnchoredRootAnchorName, "%s/%s", newAnchorName, rootObjName);

		return reAnchoredRootAnchorName;
	}

	return NULL;
}

CRuACTTemplateNode_Runtime *CRuACTTemplateNode_Runtime::GetAnchorNode()
{
	return m_anchorNode;
}

BOOL CRuACTTemplateNode_Runtime::IsPartOfSubTree(CRuEntity *entity)
{
	for(INT32 i = 0; i < m_numEntities; ++i)
	{
		if(m_entities[i] == entity)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuACTTemplateNode_Runtime::UpdateTemplateNode(BOOL replaceTemplateNodes)
{
	if(m_numEntities > 0 && m_entities)
	{
		if(replaceTemplateNodes)
		{
			// Replace the embedded entity completely
			m_sourceTemplateNode->UpdateEmbeddedEntity(m_entities[0]);
		}
		else
		{
			// Set orientation information into source template
			m_sourceTemplateNode->SetOrientation(m_entities[0]->GetScale(), m_entities[0]->GetRotation(), m_entities[0]->GetTranslation());
		}
	}

	return TRUE;
}

void CRuACTTemplateNode_Runtime::BuildAnchorPaths()
{
	// Delete existing anchor paths
	if(m_entityAnchorPaths)
	{
		for(INT32 i = 0; i < m_numEntities; ++i)
		{
			delete[] m_entityAnchorPaths[i];
		}

		delete[] m_entityAnchorPaths;
	}

	// Build anchor paths for each interior entity
	m_entityAnchorPaths = ruNEW char * [m_numEntities];
	for(INT32 i = 0; i < m_numEntities; ++i)
	{
		// Initialize empty path buffers
		char pathBuffers[2][2048];
		char *pathBuffer0 = pathBuffers[0];
		char *pathBuffer1 = pathBuffers[1];
		pathBuffer0[0] = NULL;
		pathBuffer1[0] = NULL;

		// Traverse up tree from current entity
		CRuEntity *curEntity = m_entities[i];
		while(curEntity)
		{
			// Concatenate name
			if(curEntity == m_entities[i])
				sprintf(pathBuffer0, "%s", curEntity->GetObjectName());
			else
				sprintf(pathBuffer0, "%s/%s", curEntity->GetObjectName(), pathBuffer1);

			// Swap buffers
			char *tempBuf = pathBuffer0;
			pathBuffer0 = pathBuffer1;
			pathBuffer1 = tempBuf;

			// Go on to the parent entity
			curEntity = curEntity->GetParent();
		}

		// Copy result
		m_entityAnchorPaths[i] = ruNEW char [strlen(pathBuffer1) + 1];
		strcpy(m_entityAnchorPaths[i], pathBuffer1);
	}
}

// ************************************************************************************************************************************************************

CRuACTTemplateNode::CRuACTTemplateNode()
:	m_nodeType(ruACTTEMPLATENODETYPE_DEFAULT),
	m_default_EntityName(NULL),
	m_default_PreloadedEntity(NULL),
	m_embedded_Entity(NULL),
	m_embedded_ACT(NULL)
{
	m_frame_Scale.Set(1.0f, 1.0f, 1.0f);
	m_frame_Rotation.Set(0.0f, 0.0f, 0.0f, 1.0f);
	m_frame_Translation.Set(0.0f, 0.0f, 0.0f);
}

CRuACTTemplateNode::~CRuACTTemplateNode()
{
	// Clear old node data
	Clear();
}

IRuObject *CRuACTTemplateNode::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuACTTemplateNode();

	// Chain cloning call to parent class
	CRuACT_Anchorable::Clone(clonedObject);

	// Shortcut reference to the cloned entity
	CRuACTTemplateNode *clonedNode = static_cast<CRuACTTemplateNode *>(clonedObject);

	clonedNode->m_nodeType = m_nodeType;

	clonedNode->m_frame_Scale = m_frame_Scale;
	clonedNode->m_frame_Rotation = m_frame_Rotation;
	clonedNode->m_frame_Translation = m_frame_Translation;

	if(m_default_EntityName)
	{
		INT32 entityNameLength = (INT32)strlen(m_default_EntityName) + 1;
		clonedNode->m_default_EntityName = ruNEW char [entityNameLength];
		memcpy(clonedNode->m_default_EntityName, m_default_EntityName, entityNameLength);
	}

	if(m_embedded_Entity)
	{
		clonedNode->m_embedded_Entity = static_cast<CRuEntity *>(m_embedded_Entity->Clone());
	}

	if(m_embedded_ACT)
	{
		clonedNode->m_embedded_ACT = static_cast<CRuACTTemplate *>(m_embedded_ACT->Clone());
	}

	return clonedNode;
}

BOOL CRuACTTemplateNode::SerializeFrom(IRuStream *stream)
{
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Clear old node data
	Clear();

	// Read signature and version
	INT32 signature;
	INT32 version = 1;
	stream->Read(&signature, sizeof(INT32));

	if(signature == 0xFFFFFFFF)
	{
		// Signature found, read version 
		stream->Read(&version, sizeof(INT32));
	}
	else
	{
		// No signature, this is an old model stream, so rewind
		stream->Seek(-4, ruSSM_Current);
	}

	// Assert on older unsupported versions
	assert(version >= 3);

	switch(version)
	{
		case 3:
			{
				// Chain serialization to parent class
				CRuACT_Anchorable::SerializeFrom(stream);

				// Read node type
				stream->Read(&m_nodeType, sizeof(m_nodeType));

				switch(m_nodeType)
				{
					case ruACTTEMPLATENODETYPE_EMBEDDEDENTITY:
						{
							// Create object using the object factory
							IRuObject *entityObject = RuObjectFactory().CreateObject(stream);

							if(!entityObject || !entityObject->GetType().IsSubClassOf(CRuEntity::Type()))
							{
								return FALSE;
							}

							// Assign embedded entity
							m_embedded_Entity = static_cast<CRuEntity *>(entityObject);
						}

						break;

					case ruACTTEMPLATENODETYPE_EMBEDDEDACT:
						{
							IRuObject *entityObject = RuObjectFactory().CreateObject(stream);

							if(!entityObject || !entityObject->GetType().IsSubClassOf(CRuACTTemplate::Type()))
							{
								return FALSE;
							}

							// Assign embedded ACT template
							m_embedded_ACT = static_cast<CRuACTTemplate *>(entityObject);
						}

						break;

					case ruACTTEMPLATENODETYPE_DEFAULT:
					default:
						{
							// Read length of entity name including NULL followed by the name
							UINT32 nameLength;
							stream->Read(&nameLength, sizeof(nameLength));

							m_default_EntityName = ruNEW char [nameLength];
							stream->Read(m_default_EntityName, sizeof(char) * nameLength);

						}

						break;
				}

				// Read scale
				stream->Read(&m_frame_Scale.m_x, sizeof(REAL));
				stream->Read(&m_frame_Scale.m_y, sizeof(REAL));
				stream->Read(&m_frame_Scale.m_z, sizeof(REAL));

				// Read rotation
				stream->Read(&m_frame_Rotation.m_x, sizeof(REAL));
				stream->Read(&m_frame_Rotation.m_y, sizeof(REAL));
				stream->Read(&m_frame_Rotation.m_z, sizeof(REAL));
				stream->Read(&m_frame_Rotation.m_w, sizeof(REAL));

				// Read translation
				stream->Read(&m_frame_Translation.m_x, sizeof(REAL));
				stream->Read(&m_frame_Translation.m_y, sizeof(REAL));
				stream->Read(&m_frame_Translation.m_z, sizeof(REAL));
			}

			break;
	}

	return TRUE;
}

BOOL CRuACTTemplateNode::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write signature
	INT32 signature = 0xFFFFFFFF;
	stream->Write(&signature, sizeof(INT32));

	// Write version
	INT32 version = 3;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to parent class
	CRuACT_Anchorable::SerializeTo(stream);

	// Write node type
	stream->Write(&m_nodeType, sizeof(m_nodeType));

	switch(m_nodeType)
	{
		case ruACTTEMPLATENODETYPE_FRAME:
			{
				// This should never happen - we are converting all frame node types to embedded entities
				assert(FALSE);
			}

			break;

		case ruACTTEMPLATENODETYPE_EMBEDDEDENTITY:
			{
				// Write entity
				if(m_embedded_Entity)
				{
					m_embedded_Entity->SerializeTo(stream);
				}
				else
				{
					return FALSE;
				}
			}

			break;

		case ruACTTEMPLATENODETYPE_EMBEDDEDACT:
			{
				// Write ACT template
				if(m_embedded_ACT)
				{
					m_embedded_ACT->SerializeTo(stream);
				}
				else
				{
					return FALSE;
				}
			}

			break;

		case ruACTTEMPLATENODETYPE_DEFAULT:
		default:
			{
				// Write length of entity name including NULL followed by the name
				if(m_default_EntityName)
				{
					UINT32 nameLength = (UINT32)strlen(m_default_EntityName) + 1;

					stream->Write(&nameLength, sizeof(nameLength));
					stream->Write(m_default_EntityName, sizeof(char) * nameLength);
				}
				else
				{
					UINT32 nameLength = 1;
					char *dummyName = "";

					stream->Write(&nameLength, sizeof(nameLength));
					stream->Write(dummyName, sizeof(char) * nameLength);
				}
			}

			break;
	}

	// Write scale
	stream->Write(&m_frame_Scale.m_x, sizeof(REAL));
	stream->Write(&m_frame_Scale.m_y, sizeof(REAL));
	stream->Write(&m_frame_Scale.m_z, sizeof(REAL));

	// Write rotation
	stream->Write(&m_frame_Rotation.m_x, sizeof(REAL));
	stream->Write(&m_frame_Rotation.m_y, sizeof(REAL));
	stream->Write(&m_frame_Rotation.m_z, sizeof(REAL));
	stream->Write(&m_frame_Rotation.m_w, sizeof(REAL));

	// Write translation
	stream->Write(&m_frame_Translation.m_x, sizeof(REAL));
	stream->Write(&m_frame_Translation.m_y, sizeof(REAL));
	stream->Write(&m_frame_Translation.m_z, sizeof(REAL));

	return TRUE;
}

void CRuACTTemplateNode::Set(char *entityName, const char *anchorName)
{
	// Clear old node data
	Clear();

	// Setup template node information
	m_nodeType = ruACTTEMPLATENODETYPE_DEFAULT;

	m_default_EntityName = ruNEW char [strlen(entityName) + 1];
	strcpy(m_default_EntityName, entityName);

	// Setup anchor
	this->SetAnchorTarget(anchorName);
}

void CRuACTTemplateNode::Set(CRuEntity *entity, const char *anchorName)
{
	// Clear old node data
	Clear();

	// Setup template node information
	if(entity->GetType() == CRuACTEntity::Type())
	{
		m_nodeType = ruACTTEMPLATENODETYPE_EMBEDDEDACT;

		m_embedded_ACT = static_cast<CRuACTTemplate *>(static_cast<CRuACTEntity *>(entity)->GetTemplate()->Clone());
	}
	else
	{
		m_nodeType = ruACTTEMPLATENODETYPE_EMBEDDEDENTITY;

		m_embedded_Entity = static_cast<CRuEntity *>(entity->Clone());
	}

	// Setup anchor
	this->SetAnchorTarget(anchorName);
}

void CRuACTTemplateNode::SetOrientation(const CRuVector3 &scale, const CRuQuaternion &rotation, const CRuVector3 &translation)
{
	// Store transform information from the constructed frame entity back into the template node
	m_frame_Scale = scale;
	m_frame_Rotation = rotation;
	m_frame_Translation = translation;
}

void CRuACTTemplateNode::SetOrientation(REAL scale, const CRuQuaternion &rotation, const CRuVector3 &translation)
{
	// Store transform information from the constructed frame entity back into the template node
	m_frame_Scale.Set(scale, scale, scale);
	m_frame_Rotation = rotation;
	m_frame_Translation = translation;
}

const char *CRuACTTemplateNode::GetNodeName() const
{
	switch(m_nodeType)
	{
		case ruACTTEMPLATENODETYPE_EMBEDDEDENTITY:
			{
				if(m_embedded_Entity)
				{
					return m_embedded_Entity->GetObjectName();
				}
			}

			break;

		case ruACTTEMPLATENODETYPE_EMBEDDEDACT:
			{
				if(m_embedded_ACT)
				{
					return m_embedded_ACT->GetObjectName();
				}
			}

			break;

		default:
			return "";
	}

	return "";
}

BOOL CRuACTTemplateNode::SetNodeName(const char *nodeName)
{
	switch(m_nodeType)
	{
		case ruACTTEMPLATENODETYPE_EMBEDDEDENTITY:
			{
				if(m_embedded_Entity)
				{
					m_embedded_Entity->SetObjectName(nodeName);
					return TRUE;
				}
			}

			break;

		default:
			return FALSE;
	}

	return FALSE;
}

BOOL CRuACTTemplateNode::IsNodeNameMutable()
{
	switch(m_nodeType)
	{
		case ruACTTEMPLATENODETYPE_EMBEDDEDENTITY:
			{
				if(m_embedded_Entity)
				{
					return TRUE;
				}
			}

			break;

		default:
			return FALSE;
	}

	return FALSE;
}

RuACTTemplateNodeType CRuACTTemplateNode::GetNodeType() const
{
	return m_nodeType;
}

const char *CRuACTTemplateNode::GetName() const
{
	return m_default_EntityName;
}

BOOL CRuACTTemplateNode::UpdateEmbeddedEntity(CRuEntity *embeddedEntity)
{
	if(embeddedEntity && m_nodeType == ruACTTEMPLATENODETYPE_EMBEDDEDENTITY)
	{
		// Release original embedded entity and clone the specified entity for use as the new template node
		ruSAFE_RELEASE(m_embedded_Entity);
		m_embedded_Entity = static_cast<CRuEntity *>(embeddedEntity->Clone());

		return TRUE;
	}

	return FALSE;
}

BOOL CRuACTTemplateNode::PreloadDependencies(BOOL enableAsyncLoad)
{
	switch(m_nodeType)
	{
		case ruACTTEMPLATENODETYPE_DEFAULT:
			{
				if(m_default_PreloadedEntity == NULL)
				{
					if(m_default_EntityName)
					{
						CRuEntity *dependency = NULL;

						if(enableAsyncLoad)
						{
							BOOL loadResult = g_ruResourceManager->LoadEntity(m_default_EntityName, &dependency);

							// If the resource manager is still loading the entity, return false
							if(loadResult != false && dependency == NULL)
							{
								return FALSE;
							}
						}
						else
						{
							dependency = g_ruResourceManager->LoadEntity(m_default_EntityName);
						}

						// Store reference to the dependency if it has been loaded successfully
						if(dependency)
						{
							m_default_PreloadedEntity = dependency;
						}
					}
				}
			}

			break;

		case ruACTTEMPLATENODETYPE_FRAME:
		case ruACTTEMPLATENODETYPE_EMBEDDEDENTITY:
			{
				if(m_embedded_Entity)
				{
					// Enumerate all materials
					CRuArrayList<IRuMaterial *> materialList(64);
					RuEntity_EnumerateMaterials(m_embedded_Entity, materialList);

					if(enableAsyncLoad)
					{
						// Ensure that all materials have had their textures bound
						BOOL allTexturesAreBound = TRUE;

						for(INT32 i = 0; i < materialList.Count(); ++i)
						{
							// Bind textures asynchronously
							allTexturesAreBound = allTexturesAreBound & materialList[i]->BindTextures(enableAsyncLoad);
						}

						return allTexturesAreBound;
					}
					else
					{
						// Bind textures for all materials
						for(INT32 i = 0; i < materialList.Count(); ++i)
						{
							// Bind textures synchronously
							materialList[i]->BindTextures(enableAsyncLoad);
						}
					}
				}
			}

			break;

		case ruACTTEMPLATENODETYPE_EMBEDDEDACT:
			{
				if(m_embedded_ACT)
				{
					// Recursively check dependencies
					return m_embedded_ACT->PreloadDependencies(enableAsyncLoad);
				}
			}

			break;
	}

	return TRUE;
}

CRuACTTemplateNode_Runtime *CRuACTTemplateNode::Construct()
{
	// Attempt to instance the template node
	CRuEntity *templateEntity = NULL;
	switch(m_nodeType)
	{
		case ruACTTEMPLATENODETYPE_DEFAULT:
			{
				if(m_default_EntityName)
				{
					// Load entity using entity name and parse
#ifdef KALYDO
					g_ruResourceManager->LoadEntity(m_default_EntityName, &templateEntity);
#else
					templateEntity = g_ruResourceManager->LoadEntity(m_default_EntityName);
#endif // KALYDO
				}
			}

			break;

		case ruACTTEMPLATENODETYPE_FRAME:
			{
				templateEntity = ruNEW CRuFrame();
			}

			break;

		case ruACTTEMPLATENODETYPE_EMBEDDEDENTITY:
			{
				if(m_embedded_Entity)
				{
					// Clone the embedded entity
					templateEntity = static_cast<CRuEntity *>(m_embedded_Entity->Clone());
				}
			}

			break;

		case ruACTTEMPLATENODETYPE_EMBEDDEDACT:
			{
				if(m_embedded_ACT)
				{
					// Clone the embedded entity
					templateEntity = m_embedded_ACT->Construct();
				}
			}

			break;
	}

	// If the template node was successfully instanced, expand its structure into the entity list
	if(templateEntity)
	{
		// Disable cloning and serialization
		templateEntity->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
		templateEntity->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);

		// Set the template entity's transform
		templateEntity->ResetAllTransforms();
		templateEntity->Scale(m_frame_Scale);
		templateEntity->Rotate(m_frame_Rotation);
		templateEntity->Translate(m_frame_Translation);

		CRuStack<CRuEntity *> entityStack;
		CRuArrayList<CRuEntity *> entityArray;

		// Parse the entity's hierarchy into a depth-first array
		entityStack.Push(templateEntity);
		entityArray.Clear();
		while(!entityStack.IsEmpty())
		{
			CRuEntity *curEntity = entityStack.Pop();

			// Add current entity to array
			entityArray.Add(curEntity);

			// Push children onto stack
			CRuEntity *curChild = curEntity->GetFirstChild();
			while(curChild)
			{
				entityStack.Push(curChild);

				// Advance to next child
				curChild = curChild->GetNextSibling();
			}
		}

		// Setup node runtime data
		CRuACTTemplateNode_Runtime *nodeRuntime = ruNEW CRuACTTemplateNode_Runtime(this, entityArray);

		return nodeRuntime;
	}

	return NULL;
}

void CRuACTTemplateNode::Clear()
{
	// Release template data
	delete[] m_default_EntityName;
	ruSAFE_RELEASE(m_default_PreloadedEntity);
	ruSAFE_RELEASE(m_embedded_Entity);
	ruSAFE_RELEASE(m_embedded_ACT);

	m_nodeType = ruACTTEMPLATENODETYPE_DEFAULT;
	m_default_EntityName = NULL;

	this->SetAnchorTarget(NULL);
}

// ************************************************************************************************************************************************************

CRuACTTemplateModifier::CRuACTTemplateModifier()
:	m_modifier(ruACTTEMPLATEMODIFIERTYPE_DEFAULT),
	m_texOverride_TextureName(NULL),
	m_texOverride_PreloadedTexture(NULL),
	m_shaderOverride_ShaderName(NULL)
{
}

CRuACTTemplateModifier::~CRuACTTemplateModifier()
{
	if(m_texOverride_TextureName)
	{
		delete[] m_texOverride_TextureName;
	}

	ruSAFE_RELEASE(m_texOverride_PreloadedTexture);

	if(m_shaderOverride_ShaderName)
	{
		delete[] m_shaderOverride_ShaderName;
	}
}

IRuObject *CRuACTTemplateModifier::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuACTTemplateModifier();

	// Chain cloning call to parent class
	CRuACT_Anchorable::Clone(clonedObject);

	// Shortcut reference to the cloned object
	CRuACTTemplateModifier *clonedModifier = static_cast<CRuACTTemplateModifier *>(clonedObject);

	clonedModifier->m_modifier = m_modifier;

	// Common stuff
	clonedModifier->m_meshOverride_Index = m_meshOverride_Index;

	// Type-specific data (m_nodeType = ruACTTEMPLATENODEMODTYPE_TEXTURE)
	clonedModifier->m_texOverride_Channel = m_texOverride_Channel;
	if(m_texOverride_TextureName)
	{
		INT32 textureNameLength = (INT32)strlen(m_texOverride_TextureName) + 1;
		clonedModifier->m_texOverride_TextureName = ruNEW char [textureNameLength];
		memcpy(clonedModifier->m_texOverride_TextureName, m_texOverride_TextureName, textureNameLength);
	}

	// Type-specific data (m_nodeType = ruACTTEMPLATENODEMODTYPE_SHADER)
	if(m_shaderOverride_ShaderName)
	{
		INT32 shaderNameLength = (INT32)strlen(m_shaderOverride_ShaderName) + 1;
		clonedModifier->m_shaderOverride_ShaderName = ruNEW char [shaderNameLength];
		memcpy(clonedModifier->m_shaderOverride_ShaderName, m_shaderOverride_ShaderName, shaderNameLength);
	}

	// Type-specific data (m_nodeType = ruACTTEMPLATEMODIFIERTYPE_MATPRIORITY)
	clonedModifier->m_matPriorityOverride_Priority = m_matPriorityOverride_Priority;

	// Type-specific data (m_nodeType = ruACTTEMPLATEMODIFIERTYPE_BILLBOARD)
	clonedModifier->m_billboardTypeOverride_BillboardType = m_billboardTypeOverride_BillboardType;

	return clonedModifier;
}

BOOL CRuACTTemplateModifier::SerializeFrom(IRuStream *stream)
{
	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Assert on older unsupported versions
	assert(version >= 2);

	switch(version)
	{
		case 2:
			{
				// Chain serialization to parent class
				CRuACT_Anchorable::SerializeFrom(stream);

				// Read modifier type
				stream->Read(&m_modifier, sizeof(m_modifier));

				switch(m_modifier)
				{
					case ruACTTEMPLATEMODIFIERTYPE_TEXTURE:
						{
							// Read override mesh index
							stream->Read(&m_meshOverride_Index, sizeof(INT32));

							// Read override texture channel
							stream->Read(&m_texOverride_Channel, sizeof(RuTextureChannel));

							// Read override texture name length including NULL
							INT32 texNameLength = 0;
							stream->Read(&texNameLength, sizeof(INT32));

							// Read override texture name
							if(texNameLength > 0)
							{
								m_texOverride_TextureName = ruNEW char [texNameLength];
								stream->Read(m_texOverride_TextureName, sizeof(char) * texNameLength);
							}
							else
							{
								m_texOverride_TextureName = ruNEW char [1];
								m_texOverride_TextureName[0] = NULL;
							}
						}

						break;

					case ruACTTEMPLATEMODIFIERTYPE_SHADER:
						{
							// Read override mesh index
							stream->Read(&m_meshOverride_Index, sizeof(INT32));

							// Read override texture name length including NULL
							INT32 shaderNameLength = 0;
							stream->Read(&shaderNameLength, sizeof(INT32));

							// Read override texture name
							if(shaderNameLength > 0)
							{
								m_shaderOverride_ShaderName = ruNEW char [shaderNameLength];
								stream->Read(m_shaderOverride_ShaderName, sizeof(char) * shaderNameLength);
							}
							else
							{
								m_shaderOverride_ShaderName = ruNEW char [1];
								m_shaderOverride_ShaderName[0] = NULL;
							}
						}

						break;

					case ruACTTEMPLATEMODIFIERTYPE_MATPRIORITY:
						{
							// Read override mesh index
							stream->Read(&m_meshOverride_Index, sizeof(INT32));

							// Read override priority
							stream->Read(&m_matPriorityOverride_Priority, sizeof(INT32));
						}

						break;

					case ruACTTEMPLATEMODIFIERTYPE_BILLBOARD:
						{
							// Read override mesh index
							stream->Read(&m_meshOverride_Index, sizeof(INT32));

							// Read override priority
							stream->Read(&m_billboardTypeOverride_BillboardType, sizeof(RuBillboardType));
						}

						break;

					default:
					case ruACTTEMPLATEMODIFIERTYPE_ORIENTATION:
						{
							// Read override mesh index
							stream->Read(&m_meshOverride_Index, sizeof(INT32));
						}

						break;
					}
			}

			break;
	}

	return TRUE;
}

BOOL CRuACTTemplateModifier::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 2;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to parent class
	CRuACT_Anchorable::SerializeTo(stream);

	// Write node type
	stream->Write(&m_modifier, sizeof(m_modifier));

	switch(m_modifier)
	{
		case ruACTTEMPLATEMODIFIERTYPE_TEXTURE:
			{
				// Write override mesh index
				stream->Write(&m_meshOverride_Index, sizeof(INT32));

				// Write override texture channel
				stream->Write(&m_texOverride_Channel, sizeof(RuTextureChannel));

				if(m_texOverride_TextureName)
				{
					// Write override texture name length including NULL
					INT32 texNameLength = (INT32)strlen(m_texOverride_TextureName) + 1;
					stream->Write(&texNameLength, sizeof(INT32));

					// Write override texture name
					stream->Write(m_texOverride_TextureName, sizeof(char) * texNameLength);
				}
				else
				{
					// Write override texture name length including NULL
					INT32 texNameLength = 1;
					stream->Write(&texNameLength, sizeof(INT32));

					// Write override texture name
					stream->Write("", sizeof(char) * texNameLength);
				}
			}

			break;

		case ruACTTEMPLATEMODIFIERTYPE_SHADER:
			{
				// Write override mesh index
				stream->Write(&m_meshOverride_Index, sizeof(INT32));

				if(m_shaderOverride_ShaderName)
				{
					// Write override texture name length including NULL
					INT32 shaderNameLength = (INT32)strlen(m_shaderOverride_ShaderName) + 1;
					stream->Write(&shaderNameLength, sizeof(INT32));

					// Write override texture name
					stream->Write(m_shaderOverride_ShaderName, sizeof(char) * shaderNameLength);
				}
				else
				{
					// Write override texture name length including NULL
					INT32 shaderNameLength = 1;
					stream->Write(&shaderNameLength, sizeof(INT32));

					// Write override texture name
					stream->Write("", sizeof(char) * shaderNameLength);
				}
			}

			break;

		case ruACTTEMPLATEMODIFIERTYPE_MATPRIORITY:
			{
				// Write override mesh index
				stream->Write(&m_meshOverride_Index, sizeof(INT32));

				// Write override priority
				stream->Write(&m_matPriorityOverride_Priority, sizeof(INT32));
			}

			break;

		case ruACTTEMPLATEMODIFIERTYPE_BILLBOARD:
			{
				// Write override mesh index
				stream->Write(&m_meshOverride_Index, sizeof(INT32));

				// Write override priority
				stream->Write(&m_billboardTypeOverride_BillboardType, sizeof(RuBillboardType));
			}

			break;

		default:
		case ruACTTEMPLATEMODIFIERTYPE_ORIENTATION:
			{
				// Write override mesh index
				stream->Write(&m_meshOverride_Index, sizeof(INT32));
			}

			break;
	}

	return TRUE;
}

RuACTTemplateModifierType CRuACTTemplateModifier::GetModifierType() const
{
	return m_modifier;
}

BOOL CRuACTTemplateModifier::IsEquivalentModifier(CRuACTTemplateModifier *modifier)
{
	if(modifier->m_modifier == this->m_modifier && modifier->m_meshOverride_Index == this->m_meshOverride_Index && SafeStricmp(modifier->GetAnchorTarget(), this->GetAnchorTarget()) == 0)
	{
		switch(this->m_modifier)
		{
			case ruACTTEMPLATEMODIFIERTYPE_TEXTURE:
				if(this->m_texOverride_Channel == modifier->m_texOverride_Channel)
				{
					return TRUE;
				}

				break;

			default:
				return TRUE;
		}
	}

	return FALSE;
}

void CRuACTTemplateModifier::Set(RuACTTemplateModifierType modifierType)
{
	m_modifier = modifierType;
}

INT32 CRuACTTemplateModifier::GetMeshIndex()
{
	return m_meshOverride_Index;
}

void CRuACTTemplateModifier::SetMeshIndex(INT32 meshIndex)
{
	m_meshOverride_Index = meshIndex;
}

const char *CRuACTTemplateModifier::Texture_GetName() const
{
	if(m_texOverride_TextureName)
	{
		return m_texOverride_TextureName;
	}

	return "";
}

void CRuACTTemplateModifier::Texture_SetName(const char *textureName)
{
	delete[] m_texOverride_TextureName;
	m_texOverride_TextureName = ruNEW char [strlen(textureName) + 1];
	memcpy(m_texOverride_TextureName, textureName, strlen(textureName) + 1);
}

RuTextureChannel CRuACTTemplateModifier::Texture_GetChannel() const
{
	return m_texOverride_Channel;
}

void CRuACTTemplateModifier::Texture_SetChannel(RuTextureChannel channel)
{
	m_texOverride_Channel = channel;
}

const char *CRuACTTemplateModifier::Shader_GetName() const
{
	if(m_shaderOverride_ShaderName)
	{
		return m_shaderOverride_ShaderName;
	}

	return "";
}

void CRuACTTemplateModifier::Shader_SetName(const char *shaderName)
{
	delete[] m_shaderOverride_ShaderName;
	m_shaderOverride_ShaderName = ruNEW char [strlen(shaderName) + 1];
	memcpy(m_shaderOverride_ShaderName, shaderName, strlen(shaderName) + 1);
}

INT32 CRuACTTemplateModifier::MatPriority_GetPriority() const
{
	return m_matPriorityOverride_Priority;
}

void CRuACTTemplateModifier::MatPriority_SetPriority(INT32 priority)
{
	m_matPriorityOverride_Priority = priority;
}

RuBillboardType CRuACTTemplateModifier::BillboardType_GetBillboardType() const
{
	return m_billboardTypeOverride_BillboardType;
}

void CRuACTTemplateModifier::BillboardType_SetBillboardType(RuBillboardType billboardType)
{
	m_billboardTypeOverride_BillboardType = billboardType;
}

BOOL CRuACTTemplateModifier::PreloadDependencies(BOOL enableAsyncLoad)
{
	switch(m_modifier)
	{
		case ruACTTEMPLATEMODIFIERTYPE_TEXTURE:
			if(m_texOverride_PreloadedTexture == NULL)
			{
				m_texOverride_PreloadedTexture = g_ruResourceManager->LoadTexture(m_texOverride_TextureName, ruSURFACEFMT_DEFAULT, 0, enableAsyncLoad);
			}
			break;

		case ruACTTEMPLATEMODIFIERTYPE_SHADER:
		case ruACTTEMPLATEMODIFIERTYPE_MATPRIORITY:
		case ruACTTEMPLATEMODIFIERTYPE_BILLBOARD:
		case ruACTTEMPLATEMODIFIERTYPE_ORIENTATION:
		case ruACTTEMPLATEMODIFIERTYPE_PROPERTIES:
		case ruACTTEMPLATEMODIFIERTYPE_MATPROPERTIES:
			break;
	}

	return TRUE;
}

BOOL CRuACTTemplateModifier::Construct(CRuACTTemplateNode_Runtime *target)
{
	// Locate the entity to operate upon
	CRuEntity *targetEntity = target->GetInteriorEntity(this->GetAnchorTarget());

	if(targetEntity)
	{
		switch(m_modifier)
		{
			case ruACTTEMPLATEMODIFIERTYPE_TEXTURE:
				{
					// Enumerate list of meshes and materials
					CRuArrayList<IRuMaterial *> materialList;
					RuEntity_EnumerateMaterials(targetEntity, materialList);

					// Override the texture channel in the target material
					if(m_meshOverride_Index >= 0 && m_meshOverride_Index < materialList.Count())
					{
						IRuTexture *texture = g_ruResourceManager->LoadTexture(m_texOverride_TextureName, ruSURFACEFMT_DEFAULT, 0, TRUE);
						materialList[m_meshOverride_Index]->SetTexture(m_texOverride_Channel, texture);
						ruSAFE_RELEASE(texture);
					}
				}

				break;

			case ruACTTEMPLATEMODIFIERTYPE_SHADER:
				{
					// Enumerate list of meshes and materials
					CRuArrayList<IRuMaterial *> materialList;
					RuEntity_EnumerateMaterials(targetEntity, materialList);

					// Override the texture channel in the target material
					if(m_meshOverride_Index >= 0 && m_meshOverride_Index < materialList.Count())
					{
						IRuShader *shader = g_ruShaderManager->GetShader(m_shaderOverride_ShaderName);
						materialList[m_meshOverride_Index]->SetShader(shader);
					}
				}

				break;

			case ruACTTEMPLATEMODIFIERTYPE_MATPRIORITY:
				{
					// Enumerate list of meshes and materials
					CRuArrayList<IRuMaterial *> materialList;
					RuEntity_EnumerateMaterials(targetEntity, materialList);

					// Override the priority in the target material
					if(m_meshOverride_Index >= 0 && m_meshOverride_Index < materialList.Count())
					{
						materialList[m_meshOverride_Index]->SetMaterialPriority(m_matPriorityOverride_Priority);
					}
				}

				break;

			case ruACTTEMPLATEMODIFIERTYPE_BILLBOARD:
				{
					RuEntity_SetOption(targetEntity, ruENTITYOPTION_BILLBOARD, m_billboardTypeOverride_BillboardType);
				}

				break;

			case ruACTTEMPLATEMODIFIERTYPE_ORIENTATION:
				{
					CRuController_Orientation *orientationController = ruNEW CRuController_Orientation();
					orientationController->SetTarget(targetEntity);

					// Attach orientation controller to target
					targetEntity->AddChild(orientationController);
					ruSAFE_RELEASE(orientationController);
				}

				break;

			case ruACTTEMPLATEMODIFIERTYPE_PROPERTIES:
				{
					// Copy properties to target entity
					targetEntity->CopyPropertiesFrom(this);
				}

				break;

			case ruACTTEMPLATEMODIFIERTYPE_MATPROPERTIES:
				{
					// Enumerate list of meshes and materials
					CRuArrayList<IRuMaterial *> materialList;
					RuEntity_EnumerateMaterials(targetEntity, materialList);

					// Copy properties to the target material
					if(m_meshOverride_Index >= 0 && m_meshOverride_Index < materialList.Count())
					{
						materialList[m_meshOverride_Index]->CopyPropertiesFrom(this);
					}
				}

				break;
		}

		return TRUE;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

CRuACTMotionNode::CRuACTMotionNode()
:	m_controller(NULL)
{
}

CRuACTMotionNode::~CRuACTMotionNode()
{
	// Release controller template
	ruSAFE_RELEASE(m_controller);
}

IRuObject *CRuACTMotionNode::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuACTMotionNode();

	// Chain cloning call to parent class
	CRuACT_Anchorable::Clone(clonedObject);

	// Shortcut reference to the cloned object
	CRuACTMotionNode *clonedMotionNode = static_cast<CRuACTMotionNode *>(clonedObject);

	if(m_controller)
	{
		clonedMotionNode->m_controller = static_cast<IRuEntity_Controller *>(m_controller->Clone());
	}

	return clonedMotionNode;
}

BOOL CRuACTMotionNode::SerializeFrom(IRuStream *stream)
{
	// Read class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read signature and version
	INT32 signature;
	INT32 version = 1;
	stream->Read(&signature, sizeof(INT32));

	if(signature == 0xFFFFFFFF)
	{
		// Signature found, read version 
		stream->Read(&version, sizeof(INT32));
	}
	else
	{
		// No signature, this is an old model stream, so rewind
		stream->Seek(-4, ruSSM_Current);
	}

	// Assert on older unsupported versions
	assert(version >= 3);

	switch(version)
	{
		case 3:
			{
				// Chain serialization to parent class
				CRuACT_Anchorable::SerializeFrom(stream);

				// Construct controller from object factory
				IRuObject *controllerObject = RuObjectFactory().CreateObject(stream);

				if(controllerObject && controllerObject->GetType().IsSubClassOf(IRuEntity_Controller::Type()))
				{
					m_controller = static_cast<IRuEntity_Controller *>(controllerObject);
					return TRUE;
				}
			}

			return FALSE;
	}

	return TRUE;
}

BOOL CRuACTMotionNode::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write signature
	INT32 signature = 0xFFFFFFFF;
	stream->Write(&signature, sizeof(INT32));

	// Write version
	INT32 version = 3;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to parent class
	CRuACT_Anchorable::SerializeTo(stream);

	// Serialize controller
	m_controller->SerializeTo(stream);

	return TRUE;
}

void CRuACTMotionNode::Set(IRuEntity_Controller *controller)
{
	m_controller = static_cast<IRuEntity_Controller *>(controller->Clone());
}

IRuEntity_Controller *CRuACTMotionNode::Template_GetController()
{
	return m_controller;
}

IRuEntity_Controller *CRuACTMotionNode::GetController()
{
	if(m_controller)
	{
		// Create new color controller from the template controller
		return static_cast<IRuEntity_Controller *>(m_controller->Clone());
	}

	return NULL;
}

// ************************************************************************************************************************************************************

CRuACTMotion::CRuACTMotion()
:	m_motionID(-1),
	m_motionName(NULL),
	m_motionScale(0.0f),
	m_isPlaceHolderMotion(FALSE)
{
}

CRuACTMotion::~CRuACTMotion()
{
	for(INT32 i = 0; i < m_motionNodes.Count(); ++i)
	{
		m_motionNodes[i]->Release();
	}
}

IRuObject *CRuACTMotion::Clone(IRuObject *clonedObject)
{
	CRuACTMotion *clonedMotion = ruNEW CRuACTMotion();

	clonedMotion->m_motionID = m_motionID;
	clonedMotion->m_motionName = m_motionName;
	clonedMotion->m_motionScale = m_motionScale;

	for(INT32 i = 0; i < m_motionNodes.Count(); ++i)
	{
		clonedMotion->m_motionNodes.Add(static_cast<CRuACTMotionNode *>(m_motionNodes[i]->Clone()));
	}

	return clonedMotion;
}

BOOL CRuACTMotion::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read signature and version
	INT32 signature;
	INT32 version = 1;
	stream->Read(&signature, sizeof(INT32));

	if(signature == 0xFFFFFFFF)
	{
		// Signature found, read version 
		stream->Read(&version, sizeof(INT32));
	}
	else
	{
		// No signature, this is an old model stream, so rewind
		stream->Seek(-4, ruSSM_Current);
	}

	// Assert on older unsupported versions
	assert(version >= 2);

	// Read motion ID
	stream->Read(&m_motionID, sizeof(m_motionID));

	// Read motion name length including NULL
	UINT32 nameLength;
	stream->Read(&nameLength, sizeof(nameLength));

	char *name = ruNEW char [nameLength];
	stream->Read(name, nameLength);
	m_motionName = name;
	delete[] name;

	// Read motion scale
	stream->Read(&m_motionScale, sizeof(REAL));

	// Read number of motion nodes
	INT32 motionNodeCount = m_motionNodes.Count();
	stream->Read(&motionNodeCount, sizeof(motionNodeCount));

	// Read motion nodes
	for(INT32 i = 0; i < motionNodeCount; ++i)
	{
		IRuObject *object = RuObjectFactory().CreateObject(stream);

		if(object && object->GetType() == CRuACTMotionNode::Type())
		{
			m_motionNodes.Add((CRuACTMotionNode *) object);
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CRuACTMotion::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write signature
	INT32 signature = 0xFFFFFFFF;
	stream->Write(&signature, sizeof(INT32));

	// Write version
	INT32 version = 2;
	stream->Write(&version, sizeof(INT32));

	// Write motion ID
	stream->Write(&m_motionID, sizeof(m_motionID));

	// Write motion name length including NULL
	UINT32 nameLength = m_motionName.GetLength() + 1;
	stream->Write(&nameLength, sizeof(nameLength));
	stream->Write((char *) m_motionName, nameLength);

	// Write motion scale
	stream->Write(&m_motionScale, sizeof(REAL));

	// Write number of motion nodes
	INT32 motionNodeCount = m_motionNodes.Count();
	stream->Write(&motionNodeCount, sizeof(motionNodeCount));

	// Write motion nodes
	for(INT32 i = 0; i < motionNodeCount; ++i)
	{
		m_motionNodes[i]->SerializeTo(stream);
	}

	return TRUE;
}

void CRuACTMotion::Set(INT32 motionID, const char *motionName, BOOL isPlaceHolder)
{
	m_motionID = motionID;
	m_motionName = motionName;
	m_isPlaceHolderMotion = isPlaceHolder;
}

BOOL CRuACTMotion::AddMotionNode(IRuEntity_Controller *controller, const char *anchorTarget)
{
	// Setup motion node
	CRuACTMotionNode *motionNode = ruNEW CRuACTMotionNode();
	motionNode->SetAnchorTarget(anchorTarget);
	motionNode->Set(controller);

	// Add motion node to motion node list
	m_motionNodes.Add(motionNode);

	return TRUE;
}

BOOL CRuACTMotion::RemoveMotionNodeAt(INT32 motionNodeIndex)
{
	if(motionNodeIndex >= 0 && motionNodeIndex < m_motionNodes.Count())
	{
		// Release motion node
		m_motionNodes[motionNodeIndex]->Release();

		// Remove motion node from the array list
		m_motionNodes.RemoveAt(motionNodeIndex);

		return TRUE;
	}

	return FALSE;
}

BOOL CRuACTMotion::Merge(CRuACTMotion *srcMotion)
{
	for(INT32 k = 0; k < srcMotion->GetNumMotionNodes(); ++k)
	{
		CRuACTMotionNode *clonedMotionNode = static_cast<CRuACTMotionNode *>(srcMotion->GetMotionNodeByIndex(k)->Clone());
		m_motionNodes.Add(clonedMotionNode);
	}

	return TRUE;
}

INT32 CRuACTMotion::GetMotionID() const
{
	return m_motionID;
}

const CRuString &CRuACTMotion::GetMotionName() const
{
	return m_motionName;
}

REAL CRuACTMotion::GetMotionScale() const
{
	return m_motionScale;
}

void CRuACTMotion::SetMotionID(INT32 motionID)
{
	m_motionID = motionID;
}

void CRuACTMotion::SetMotionName(const char *motionName)
{
	if(motionName)
	{
		m_motionName = motionName;
	}
}

void CRuACTMotion::SetMotionScale(REAL motionScale)
{
	if(motionScale >= 0.0f)
	{
		m_motionScale = motionScale;
	}
}

INT32 CRuACTMotion::GetNumMotionNodes() const
{
	return m_motionNodes.Count();
}

CRuACTMotionNode *CRuACTMotion::GetMotionNodeByIndex(INT32 index)
{
	return m_motionNodes[index];

}

BOOL CRuACTMotion::IsPlaceHolderMotion() const
{
	return m_isPlaceHolderMotion;
}

/*!
	BOOL CRuACTMotion::EnumerateControllers(const CRuRTTI *controllerType, CRuArrayList<IRuEntity_Controller *> &controllersOut)

	Enumerates controllers of the given subtype, or all controllers if no type is specified

	@param controllerType
	@param controllersOut

	@return BOOL
	@retval TRUE The enumeration succeeded
	@retval FALSE The enumeration failed
*/
BOOL CRuACTMotion::EnumerateControllers(const CRuRTTI *controllerType, CRuArrayList<IRuEntity_Controller *> &controllersOut)
{
	// Iterate through all motion nodes
	for(INT32 i = 0, numMotionNodes = this->GetNumMotionNodes(); i < numMotionNodes; ++i)
	{
		CRuACTMotionNode *motionNode = this->GetMotionNodeByIndex(i);

		// If the motion node controller is of the specified subtype, add it to the list of enumerated controllers
		if(controllerType == NULL || motionNode->Template_GetController()->GetType().IsSubClassOf(*controllerType))
		{
			controllersOut.Add(motionNode->Template_GetController());
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuACTTemplate::CRuACTTemplate()
:	m_classification(NULL),
	m_transformMode(ruENTITYTRANSFORMMODE_NORMAL),
	m_cullMode(ruENTITYCULLMODE_NORMAL),
	m_alignToWalkingSurface(FALSE)
{
}

CRuACTTemplate::~CRuACTTemplate()
{
	// Release references to all template nodes
	for(INT32 i = 0; i < m_templateNodes.Count(); ++i)
	{
		m_templateNodes[i]->Release();
	}

	// Release references to all template modifiers
	for(INT32 i = 0; i < m_templateModifiers.Count(); ++i)
	{
		ruSAFE_RELEASE(m_templateModifiers[i]);
	}

	// Release references to all motions
	for(INT32 i = 0; i < m_motions.Count(); ++i)
	{
		m_motions[i]->Release();
	}

	// Clear template node list
	m_templateNodes.Clear();

	// Clear template modifiers list
	m_templateModifiers.Clear();

	// Clear motion list
	m_motions.Clear();

	delete[] m_classification;
}

IRuObject *CRuACTTemplate::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuACTTemplate();
	CRuACTTemplate *clonedTemplate = static_cast<CRuACTTemplate *>(clonedObject);

	// Chain cloning call to parent class
	IRuObject::Clone(clonedObject);

	for(INT32 i = 0; i < m_templateNodes.Count(); ++i)
	{
		CRuACTTemplateNode *clonedNode = static_cast<CRuACTTemplateNode *>(m_templateNodes[i]->Clone());
		clonedTemplate->m_templateNodes.Add(clonedNode);
	}

	for(INT32 i = 0; i < m_templateModifiers.Count(); ++i)
	{
		CRuACTTemplateModifier *clonedModifier = static_cast<CRuACTTemplateModifier *>(m_templateModifiers[i]->Clone());
		clonedTemplate->m_templateModifiers.Add(clonedModifier);
	}

	for(INT32 i = 0; i < m_motions.Count(); ++i)
	{
		CRuACTMotion *clonedMotion = static_cast<CRuACTMotion *>(m_motions[i]->Clone());
		clonedTemplate->m_motions.Add(clonedMotion);
	}

	return clonedTemplate;
}

BOOL CRuACTTemplate::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read signature and version
	INT32 signature;
	INT32 version = 1;
	stream->Read(&signature, sizeof(INT32));

	if(signature == 0xFFFFFFFF)
	{
		// Signature found, read version 
		stream->Read(&version, sizeof(INT32));
	}
	else
	{
		// No signature, this is an old model stream, so rewind
		stream->Seek(-4, ruSSM_Current);
	}

	// Assert on older unsupported versions
	assert(version >= 7);

	switch(version)
	{
		case 7:
			{
				// Chain serialization to base class
				IRuObject::SerializeFrom(stream);

				INT32 templateNodeCount;
				INT32 modifierCount;
				INT32 motionCount;

				stream->Read(&templateNodeCount, sizeof(INT32));
				stream->Read(&modifierCount, sizeof(INT32));
				stream->Read(&motionCount, sizeof(INT32));

				// Read each template node
				for(INT32 i = 0; i < templateNodeCount; ++i)
				{
					IRuObject *object = RuObjectFactory().CreateObject(stream);

					if(object && object->GetType() == CRuACTTemplateNode::Type())
					{
						m_templateNodes.Add((CRuACTTemplateNode *) object);
					}
					else
					{
						return FALSE;
					}
				}

				// Read each modifier
				for(INT32 i = 0; i < modifierCount; ++i)
				{
					IRuObject *object = RuObjectFactory().CreateObject(stream);

					if(object && object->GetType() == CRuACTTemplateModifier::Type())
					{
						m_templateModifiers.Add((CRuACTTemplateModifier *) object);
					}
					else
					{
						return FALSE;
					}
				}

				// Read each motion
				for(INT32 i = 0; i < motionCount; ++i)
				{
					IRuObject *object = RuObjectFactory().CreateObject(stream);

					if(object && object->GetType() == CRuACTMotion::Type())
					{
						m_motions.Add((CRuACTMotion *) object);
					}
					else
					{
						return FALSE;
					}
				}

				// Read ACT classification
				INT32 classificationLength = 0;
				stream->Read(&classificationLength, sizeof(INT32));

				m_classification = ruNEW char [classificationLength];
				stream->Read(m_classification, classificationLength);

				// Read entity transform mode
				stream->Read(&m_transformMode, sizeof(RuEntityTransformMode));

				// Read entity cull mode
				stream->Read(&m_cullMode, sizeof(RuEntityCullMode));

				// Read align to walking surface
				stream->Read(&m_alignToWalkingSurface, sizeof(BOOL));
			}

			break;
	}

	return TRUE;
}

BOOL CRuACTTemplate::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write signature
	INT32 signature = 0xFFFFFFFF;
	stream->Write(&signature, sizeof(INT32));

	// Write version
	INT32 version = 7;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuObject::SerializeTo(stream);

	INT32 templateNodeCount = m_templateNodes.Count();
	INT32 modifierCount = m_templateModifiers.Count();
	INT32 motionCount = m_motions.Count();

	stream->Write(&templateNodeCount, sizeof(INT32));
	stream->Write(&modifierCount, sizeof(INT32));
	stream->Write(&motionCount, sizeof(INT32));

	// Write out each template node
	for(INT32 i = 0; i < templateNodeCount; ++i)
	{
		m_templateNodes[i]->SerializeTo(stream);
	}

	// Write out each template node
	for(INT32 i = 0; i < modifierCount; ++i)
	{
		m_templateModifiers[i]->SerializeTo(stream);
	}

	// Write out each motion
	for(INT32 i = 0; i < motionCount; ++i)
	{
		m_motions[i]->SerializeTo(stream);
	}

	// Write ACT classification
	if(m_classification)
	{
		INT32 classificationLength = (INT32)strlen(m_classification) + 1;
		stream->Write(&classificationLength, sizeof(INT32));
		stream->Write(m_classification, classificationLength);
	}
	else
	{
		INT32 classificationLength = 1;
		stream->Write(&classificationLength, sizeof(INT32));
		stream->Write("", classificationLength);
	}

	// Write entity transform mode
	stream->Write(&m_transformMode, sizeof(RuEntityTransformMode));

	// Write entity transform mode
	stream->Write(&m_cullMode, sizeof(RuEntityCullMode));

	// Write align to walking surface
	stream->Write(&m_alignToWalkingSurface, sizeof(BOOL));

	return TRUE;
}

BOOL CRuACTTemplate::AddTemplateNode(char *entityName, const char *anchorName)
{
	// Create new template node
	CRuACTTemplateNode *node = ruNEW CRuACTTemplateNode();
	node->Set(entityName, anchorName);

	// Add template node to the ACT
	m_templateNodes.Add(node);

	return TRUE;
}

BOOL CRuACTTemplate::AddTemplateNode(CRuEntity *entity, const char *anchorName)
{
	// Create new template node
	CRuACTTemplateNode *node = ruNEW CRuACTTemplateNode();
	node->Set(entity, anchorName);

	// Add template node to the ACT
	m_templateNodes.Add(node);

	return TRUE;
}

BOOL CRuACTTemplate::DeleteTemplateNode(CRuACTEntity *actEntity, CRuACTTemplateNode *node)
{
	// Find the corresponding runtime node
	CRuACTTemplateNode_Runtime *runtimeNode = FindRuntimeNode(actEntity->GetNumRuntimeNodes(), actEntity->GetRuntimeNodes(), node);

	if(runtimeNode)
	{
		// Find all dependent template nodes
		for(INT32 i = 0; i < m_templateNodes.Count(); ++i)
		{
			if(runtimeNode->GetInteriorEntity(m_templateNodes[i]->GetAnchorTarget()))
			{
				// Recursively delete the dependent template node
				DeleteTemplateNode(actEntity, m_templateNodes[i]);

				// Restart loop
				i = -1;
			}
		}

		// Find all dependent template modifiers
		for(INT32 i = 0; i < m_templateModifiers.Count(); ++i)
		{
			if(runtimeNode->GetInteriorEntity(m_templateModifiers[i]->GetAnchorTarget()))
			{
				// Delete the template modifier
				ruSAFE_RELEASE(m_templateModifiers[i]);
				m_templateModifiers.RemoveAt(i);
				--i;
			}
		}

		// Find all dependent motion nodes
		for(INT32 i = 0; i < m_motions.Count(); ++i)
		{
			CRuACTMotion *motion = m_motions[i];

			for(INT32 j = 0; j < motion->GetNumMotionNodes(); ++j)
			{
				CRuACTMotionNode *motionNode = motion->GetMotionNodeByIndex(j);

				if(runtimeNode->GetInteriorEntity(motionNode->GetAnchorTarget()))
				{
					// Delete the motion node
					motion->RemoveMotionNodeAt(j);
					--j;
				}
			}
		}

		// Delete template node
		for(INT32 i = 0; i < m_templateNodes.Count(); ++i)
		{
			if(m_templateNodes[i] == node)
			{
				ruSAFE_RELEASE(m_templateNodes[i]);
				m_templateNodes.RemoveAt(i);
				break;
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuACTTemplate::MoveTemplateNode(CRuACTEntity *actEntity, CRuACTTemplateNode *node, CRuEntity *anchorEntity)
{
	// Find the corresponding runtime node
	CRuACTTemplateNode_Runtime *runtimeNode = FindRuntimeNode(actEntity->GetNumRuntimeNodes(), actEntity->GetRuntimeNodes(), node);

	// Find the new anchor target
	const char *newAnchorTarget = FindAnchorTarget(actEntity->GetNumRuntimeNodes(), actEntity->GetRuntimeNodes(), anchorEntity);

	// Find the old anchor path to replace
	const char *oldRootAnchorName = runtimeNode->GetRootAnchorName();
	INT32 oldRootAnchorNameLen = (INT32)strlen(oldRootAnchorName);

	// Construct the new anchor path
	char *newRootAnchorName = runtimeNode->GetReAnchoredRootAnchorName(newAnchorTarget);

	// Process all dependent template nodes
	for(INT32 i = 0; i < m_templateNodes.Count(); ++i)
	{
		CRuString nodeAnchorTarget = m_templateNodes[i]->GetAnchorTarget();

		if(HasMatchingPrefix(oldRootAnchorName, nodeAnchorTarget))
		{
			// Perform a replace on the anchor target
			nodeAnchorTarget.Replace(oldRootAnchorName, newRootAnchorName);

			// Set new anchor target
			m_templateNodes[i]->SetAnchorTarget(nodeAnchorTarget);
		}
	}

	// Process all dependent template modifiers
	for(INT32 i = 0; i < m_templateModifiers.Count(); ++i)
	{
		CRuString nodeAnchorTarget = m_templateModifiers[i]->GetAnchorTarget();

		if(HasMatchingPrefix(oldRootAnchorName, nodeAnchorTarget))
		{
			// Perform a replace on the anchor target
			nodeAnchorTarget.Replace(oldRootAnchorName, newRootAnchorName);

			// Set new anchor target
			m_templateModifiers[i]->SetAnchorTarget(nodeAnchorTarget);
		}
	}

	// Process all dependent motion nodes
	for(INT32 i = 0; i < m_motions.Count(); ++i)
	{
		CRuACTMotion *motion = m_motions[i];

		for(INT32 j = 0; j < motion->GetNumMotionNodes(); ++j)
		{
			CRuACTMotionNode *motionNode = motion->GetMotionNodeByIndex(j);
			CRuString nodeAnchorTarget = motionNode->GetAnchorTarget();

			if(HasMatchingPrefix(oldRootAnchorName, nodeAnchorTarget))
			{
				// Perform a replace on the anchor target
				nodeAnchorTarget.Replace(oldRootAnchorName, newRootAnchorName);

				// Set new anchor target
				motionNode->SetAnchorTarget(nodeAnchorTarget);
			}
		}
	}

	// Set new anchor target
	node->SetAnchorTarget(newAnchorTarget);

	delete[] newRootAnchorName;

	return TRUE;
}

BOOL CRuACTTemplate::RenameTemplateNode(CRuACTEntity *actEntity, CRuACTTemplateNode *node, const char *name)
{
	if(node->IsNodeNameMutable())
	{
		// Find the corresponding runtime node
		CRuACTTemplateNode_Runtime *runtimeNode = FindRuntimeNode(actEntity->GetNumRuntimeNodes(), actEntity->GetRuntimeNodes(), node);

		// Find the old anchor path to replace
		const char *oldRootAnchorName = runtimeNode->GetRootAnchorName();
		INT32 oldRootAnchorNameLen = (INT32)strlen(oldRootAnchorName);

		// Construct the new anchor path
		CRuString newRootAnchorName;

		if(node->GetAnchorTarget())
		{
			newRootAnchorName = node->GetAnchorTarget();
			newRootAnchorName.Append("/");
			newRootAnchorName.Append(name);
		}
		else
		{
			newRootAnchorName = name;
		}

		// Process all dependent template nodes
		for(INT32 i = 0; i < m_templateNodes.Count(); ++i)
		{
			CRuString nodeAnchorTarget = m_templateNodes[i]->GetAnchorTarget();

			if(HasMatchingPrefix(oldRootAnchorName, nodeAnchorTarget))
			{
				// Perform a replace on the anchor target
				nodeAnchorTarget.Replace(oldRootAnchorName, newRootAnchorName);

				// Set new anchor target
				m_templateNodes[i]->SetAnchorTarget(nodeAnchorTarget);
			}
		}

		// Process all dependent template modifiers
		for(INT32 i = 0; i < m_templateModifiers.Count(); ++i)
		{
			CRuString nodeAnchorTarget = m_templateModifiers[i]->GetAnchorTarget();

			if(HasMatchingPrefix(oldRootAnchorName, nodeAnchorTarget))
			{
				// Perform a replace on the anchor target
				nodeAnchorTarget.Replace(oldRootAnchorName, newRootAnchorName);

				// Set new anchor target
				m_templateModifiers[i]->SetAnchorTarget(nodeAnchorTarget);
			}
		}

		// Process all dependent motion nodes
		for(INT32 i = 0; i < m_motions.Count(); ++i)
		{
			CRuACTMotion *motion = m_motions[i];

			for(INT32 j = 0; j < motion->GetNumMotionNodes(); ++j)
			{
				CRuACTMotionNode *motionNode = motion->GetMotionNodeByIndex(j);
				CRuString nodeAnchorTarget = motionNode->GetAnchorTarget();

				if(HasMatchingPrefix(oldRootAnchorName, nodeAnchorTarget))
				{
					// Perform a replace on the anchor target
					nodeAnchorTarget.Replace(oldRootAnchorName, newRootAnchorName);

					// Set new anchor target
					motionNode->SetAnchorTarget(nodeAnchorTarget);
				}
			}
		}

		node->SetNodeName(name);

		return TRUE;
	}

	return FALSE;
}

INT32 CRuACTTemplate::AddTemplateModifier(CRuACTTemplateModifier *modifier)
{
	// Iterate through all existing template modifiers and delete any which conflict with the new modifier
	for(INT32 i = 0; i < m_templateModifiers.Count(); ++i)
	{
		if(m_templateModifiers[i]->IsEquivalentModifier(modifier))
		{
			ruSAFE_RELEASE(m_templateModifiers[i]);
			m_templateModifiers.RemoveAt(i);
			--i;
		}
	}

	// Add new modifier to modifier list
	m_templateModifiers.Add(modifier);

	// Return modifier index
	return m_templateModifiers.Count() - 1;
}

BOOL CRuACTTemplate::DeleteTemplateModifier(CRuACTTemplateModifier *modifier)
{
	for(INT32 i = 0; i < m_templateModifiers.Count(); ++i)
	{
		if(m_templateModifiers[i] == modifier)
		{
			return DeleteTemplateModifier(i);
		}
	}

	return FALSE;
}

BOOL CRuACTTemplate::DeleteTemplateModifier(INT32 modifierIndex)
{
	if(modifierIndex >= 0 && modifierIndex < m_templateModifiers.Count())
	{
		// Release the modifier
		ruSAFE_RELEASE(m_templateModifiers[modifierIndex]);

		// Remove the pointer
		m_templateModifiers.RemoveAt(modifierIndex);

		return TRUE;
	}

	return FALSE;
}

CRuACTTemplateModifier *CRuACTTemplate::CreateTemplateModifier(const char *anchorTarget, INT32 meshIndex, RuACTTemplateModifierType modifierType)
{
	CRuACTTemplateModifier *templateModifier = FindTemplateModifier(anchorTarget, meshIndex, modifierType);

	if(templateModifier == NULL)
	{
		templateModifier = ruNEW CRuACTTemplateModifier();
		templateModifier->SetAnchorTarget(anchorTarget);
		templateModifier->Set(modifierType);
		templateModifier->SetMeshIndex(meshIndex);
		this->AddTemplateModifier(templateModifier);
	}

	return templateModifier;
}

CRuACTTemplateModifier *CRuACTTemplate::FindTemplateModifier(const char *anchorTarget, INT32 meshIndex, RuACTTemplateModifierType modifierType)
{
	// Iterate through all existing template modifiers and find the specified modifier
	for(INT32 i = 0; i < m_templateModifiers.Count(); ++i)
	{
		if( SafeStricmp(m_templateModifiers[i]->GetAnchorTarget(), anchorTarget) == 0 &&
			m_templateModifiers[i]->GetMeshIndex() == meshIndex &&
			m_templateModifiers[i]->GetModifierType() == modifierType)
		{
			return m_templateModifiers[i];
		}
	}

	return NULL;
}

void CRuACTTemplate::FindTemplateModifiers(const char *anchorTarget, INT32 meshIndex, RuACTTemplateModifierType modifierType, CRuArrayList<CRuACTTemplateModifier *> &modifiersOut)
{
	// Iterate through all existing template modifiers and find any modifiers that match the specified parameters
	for(INT32 i = 0; i < m_templateModifiers.Count(); ++i)
	{
		if( SafeStricmp(m_templateModifiers[i]->GetAnchorTarget(), anchorTarget) == 0 &&
			m_templateModifiers[i]->GetMeshIndex() == meshIndex &&
			m_templateModifiers[i]->GetModifierType() == modifierType)
		{
			modifiersOut.Add(m_templateModifiers[i]);
		}
	}
}

BOOL CRuACTTemplate::Merge(CRuACTTemplate *srcTemplate)
{
	for(INT32 i = 0; i < srcTemplate->GetNumTemplateNodes(); ++i)
	{
		CRuACTTemplateNode *clonedNode = static_cast<CRuACTTemplateNode *>(srcTemplate->GetTemplateNode(i)->Clone());
		m_templateNodes.Add(clonedNode);
	}

	for(INT32 i = 0; i < srcTemplate->GetNumTemplateModifiers(); ++i)
	{
		CRuACTTemplateModifier *clonedModifier = static_cast<CRuACTTemplateModifier *>(srcTemplate->GetTemplateModifier(i)->Clone());
		m_templateModifiers.Add(clonedModifier);
	}

	for(INT32 i = 0; i < srcTemplate->GetNumMotions(); ++i)
	{
		CRuACTMotion *srcMotion = srcTemplate->GetMotion(i);

		// Find appropriate motion to merge into
		BOOL motionMerged = FALSE;
		for(INT32 j = 0; j < m_motions.Count(); ++j)
		{
			CRuACTMotion *tgtMotion = m_motions[j];

			if(tgtMotion->GetMotionName() == srcMotion->GetMotionName())
			{
				// Merge source motion into target motion
				tgtMotion->Merge(srcMotion);
				motionMerged = TRUE;
				break;
			}
		}

		// If motion was not merged with another motion, add motion to the motion list
		if(motionMerged == FALSE)
		{
			CRuACTMotion *clonedMotion = static_cast<CRuACTMotion *>(srcTemplate->GetMotion(i)->Clone());
			m_motions.Add(clonedMotion);
		}
	}

/*
	INT32 templateIndexOffset = this->GetNumTemplateNodes();

	for(INT32 i = 0; i < srcTemplate->GetNumTemplateNodes(); ++i)
	{
		CRuACTTemplateNode *clonedNode = static_cast<CRuACTTemplateNode *>(srcTemplate->GetTemplateNode(i)->Clone());
		m_templateNodes.Add(clonedNode);

		// Adjust parent index for the node if necessary
		if(clonedNode->GetAnchorIndex() >= 0)
		{
			clonedNode->SetAnchorIndex(clonedNode->GetAnchorIndex() + templateIndexOffset);
			clonedNode->SetInteriorIndex(clonedNode->GetInteriorIndex());
		}
	}

	for(INT32 i = 0; i < srcTemplate->GetNumTemplateModifiers(); ++i)
	{
		CRuACTTemplateModifier *clonedModifier = static_cast<CRuACTTemplateModifier *>(srcTemplate->GetTemplateModifier(i)->Clone());
		m_templateModifiers.Add(clonedModifier);

		// Adjust template index for the modifier if necessary
		if(clonedModifier->GetAnchorIndex() >= 0)
		{
			clonedModifier->Set(clonedModifier->GetAnchorIndex() + templateIndexOffset, clonedModifier->GetInteriorIndex());
		}
	}

	for(INT32 i = 0; i < srcTemplate->GetNumMotions(); ++i)
	{
		// Find appropriate motion to merge into
		BOOL motionMerged = FALSE;
		for(INT32 j = 0; j < m_motions.Count(); ++j)
		{
			if(m_motions[j]->GetMotionName() == srcTemplate->GetMotion(i)->GetMotionName())
			{
				m_motions[j]->Merge(srcTemplate->GetMotion(i), templateIndexOffset);
				motionMerged = TRUE;
				break;
			}
		}

		if(motionMerged == FALSE)
		{
			CRuACTMotion *clonedMotion = static_cast<CRuACTMotion *>(srcTemplate->GetMotion(i)->Clone());
			m_motions.Add(clonedMotion);
			clonedMotion->AdjustTemplateIndex(templateIndexOffset);
		}
	}
*/
	return TRUE;
}

BOOL CRuACTTemplate::Sort()
{
	char **motionNames = ruNEW char * [m_motions.Count()];

	for(INT32 i = 0; i < m_motions.Count(); ++i)
	{
		motionNames[i] = m_motions[i]->GetMotionName();
	}

	qsort(motionNames, m_motions.Count(), sizeof(char *), StrCompare);

	CRuArrayList<CRuACTMotion *> sortedMotions;
	for(INT32 i = 0; i < m_motions.Count(); ++i)
	{
		for(INT32 j = 0; j < m_motions.Count(); ++j)
		{
			if(m_motions[j]->GetMotionID() == 0)
			{
				sortedMotions.Insert(0, m_motions[j]);
			}
			else if(strcmp(m_motions[j]->GetMotionName(), motionNames[i]) == 0)
			{
				sortedMotions.Add(m_motions[j]);
				break;
			}
		}
	}

	m_motions.CloneFrom(sortedMotions);

	// Renumber
	for(INT32 i = 0, j = 1; i < m_motions.Count(); ++i)
	{
		if(m_motions[i]->GetMotionID() != 0)
		{
			m_motions[i]->SetMotionID(j);
			++j;
		}
	}

	delete[] motionNames;

	return TRUE;
}

INT32 CRuACTTemplate::GetNumTemplateNodes() const
{
	return m_templateNodes.Count();
}

CRuACTTemplateNode *CRuACTTemplate::GetTemplateNode(INT32 index)
{
	return m_templateNodes[index];
}

INT32 CRuACTTemplate::GetTemplateNodeIndex(CRuACTTemplateNode *templateNode)
{
	for(INT32 i = 0; i < m_templateNodes.Count(); ++i)
	{
		if(templateNode == m_templateNodes[i])
		{
			return i;
		}
	}

	return -1;
}

INT32 CRuACTTemplate::GetNumTemplateModifiers() const
{
	return m_templateModifiers.Count();
}

CRuACTTemplateModifier *CRuACTTemplate::GetTemplateModifier(INT32 index)
{
	return m_templateModifiers[index];
}

BOOL CRuACTTemplate::NodeNameExists(const char *name)
{
	for(INT32 i = 0; i < m_templateNodes.Count(); ++i)
	{
		if(SafeStricmp(m_templateNodes[i]->GetNodeName(), name) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuACTTemplate::AddMotion(INT32 motionID, const char *motionName)
{
	// Create new motion
	CRuACTMotion *newMotion = ruNEW CRuACTMotion();
	newMotion->Set(motionID, motionName, FALSE);

	// Add motion to motion list
	m_motions.Add(newMotion);

	return TRUE;
}

BOOL CRuACTTemplate::AddPlaceHolderMotion(INT32 motionID, const char *motionName)
{
	// Create new motion
	CRuACTMotion *newMotion = ruNEW CRuACTMotion();
	newMotion->Set(motionID, motionName, TRUE);

	// Add motion to motion list
	m_motions.Add(newMotion);

	return TRUE;
}

BOOL CRuACTTemplate::DeleteMotion(INT32 index)
{
	if(index >= 0 && index < (INT32) m_motions.Count())
	{
		m_motions[index]->Release();
		m_motions.RemoveAt(index);

		return TRUE;
	}

	return FALSE;
}

INT32 CRuACTTemplate::GetNumMotions() const
{
	return m_motions.Count();
}

CRuACTMotion *CRuACTTemplate::GetMotion(INT32 index)
{
	if(index >= 0 && index < m_motions.Count())
	{
		if(m_motions[index]->IsPlaceHolderMotion())
		{
			for(INT32 i = 0; i < m_motions.Count(); i++)
			{
				if(m_motions[i]->GetMotionID() == m_motions[index]->GetMotionID() && m_motions[i]->IsPlaceHolderMotion() == FALSE)
				{
					return m_motions[i];
				}
			}
		}

		return m_motions[index];
	}

	return NULL;
}

CRuACTMotion *CRuACTTemplate::GetMotionByID(INT32 id)
{
	for(INT32 i = 0; i < m_motions.Count(); i++)
	{
		if(m_motions[i]->GetMotionID() == id && m_motions[i]->IsPlaceHolderMotion() == FALSE)
		{
			return m_motions[i];
		}
	}

	return NULL;
}

CRuACTMotion *CRuACTTemplate::GetMotionByName(const char *motionName)
{
	for(INT32 i = 0; i < m_motions.Count(); i++)
	{
		if(m_motions[i]->GetMotionName() == motionName)
		{
			// If the motion is a place holder motion, search for the motion with the same ID
			if(m_motions[i]->IsPlaceHolderMotion())
			{
				for(INT32 j = 0; j < m_motions.Count(); j++)
				{
					if(m_motions[j]->GetMotionID() == m_motions[i]->GetMotionID() && m_motions[j]->IsPlaceHolderMotion() == FALSE)
					{
						return m_motions[j];
					}
				}
			}

			return m_motions[i];
		}
	}

	return NULL;
}

INT32 CRuACTTemplate::GetNextMotionID() const
{
	INT32 largestMotionID = 0;

	for(INT32 i = 0; i < (INT32) m_motions.Count(); i++)
	{
		if(m_motions[i]->GetMotionID() > largestMotionID)
		{
			largestMotionID = m_motions[i]->GetMotionID();
		}
	}

	return largestMotionID + 1;
}

INT32 CRuACTTemplate::GetMotionIDByName(const char *motionName) const
{
	for(INT32 i = 0; i < (INT32) m_motions.Count(); i++)
	{
		if(m_motions[i]->GetMotionName() == motionName)
		{
			return m_motions[i]->GetMotionID();
		}
	}

	return -1;
}

const char *CRuACTTemplate::GetMotionNameByID(INT32 id) const
{
	for(INT32 i = 0; i < (INT32) m_motions.Count(); i++)
	{
		if(m_motions[i]->GetMotionID() == id)
		{
			return m_motions[i]->GetMotionName();
		}
	}

	return "";
}

INT32 CRuACTTemplate::GetMotionIndexByID(INT32 id) const
{
	for(INT32 i = 0; i < m_motions.Count(); i++)
	{
		if(m_motions[i]->GetMotionID() == id)
		{
			return i;
		}
	}

	return -1;
}

INT32 CRuACTTemplate::GetMotionIndexByName(const char *motionName) const
{
	for(INT32 i = 0; i < m_motions.Count(); i++)
	{
		if(m_motions[i]->GetMotionName() == motionName)
		{
			return i;
		}
	}

	return -1;
}

void CRuACTTemplate::SetMotion(INT32 index, CRuACTMotion *motion)
{
	if(index >= 0 && index < (INT32) m_motions.Count() && motion)
	{
		ruSAFE_RELEASE(m_motions[index]);
		m_motions[index] = motion;
		m_motions[index]->AddRef();
	}
}

const char *CRuACTTemplate::GetClassification() const
{
	if(m_classification)
	{
		return m_classification;
	}

	return "";
}

void CRuACTTemplate::SetClassification(const char *classification)
{
	if(classification)
	{
		delete[] m_classification;
		m_classification = ruNEW char [strlen(classification) + 1];
		strcpy(m_classification, classification);
	}
}

RuEntityTransformMode CRuACTTemplate::GetTransformMode() const
{
	return m_transformMode;
}

void CRuACTTemplate::SetTransformMode(RuEntityTransformMode transformMode)
{
	m_transformMode = transformMode;
}

RuEntityCullMode CRuACTTemplate::GetCullMode() const
{
	return m_cullMode;
}

void CRuACTTemplate::SetCullMode(RuEntityCullMode cullMode)
{
	m_cullMode = cullMode;
}

BOOL CRuACTTemplate::GetAlignToWalkingSurface() const
{
	return m_alignToWalkingSurface;
}

void CRuACTTemplate::SetAlignToWalkingSurface(BOOL align)
{
	m_alignToWalkingSurface = align;
}

BOOL CRuACTTemplate::PreloadDependencies(BOOL enableAsyncLoad)
{
	BOOL dependenciesLoaded = TRUE;

	// Iterate through template nodes
	for(INT32 i = 0; i < m_templateNodes.Count(); ++i)
	{
		if(m_templateNodes[i]->PreloadDependencies(enableAsyncLoad) == FALSE)
		{
			dependenciesLoaded = FALSE;
		}
	}

	// Iterate through modifiers
	for(INT32 i = 0; i < m_templateModifiers.Count(); ++i)
	{
		if(m_templateModifiers[i]->PreloadDependencies(enableAsyncLoad) == FALSE)
		{
			dependenciesLoaded = FALSE;
		}
	}

	return dependenciesLoaded;
}

#ifdef KALYDO
BOOL CRuACTTemplateNode::GetDependencies(std::vector<CRuString>& dependencies)
{
	switch(m_nodeType)
	{
		case ruACTTEMPLATENODETYPE_DEFAULT:
			{
				if(m_default_PreloadedEntity == NULL)
				{
					if(m_default_EntityName)
					{
						dependencies.push_back(m_default_EntityName);
					}
				}
			}

			break;

		case ruACTTEMPLATENODETYPE_FRAME:
		case ruACTTEMPLATENODETYPE_EMBEDDEDENTITY:
			{
				if(m_embedded_Entity)
				{
					// Enumerate all materials
					CRuArrayList<IRuMaterial *> materialList(64);
					RuEntity_EnumerateMaterials(m_embedded_Entity, materialList);

					for(INT32 i = 0; i < materialList.Count(); ++i)
					{
						materialList[i]->GetDependencies(dependencies);
					}
				}
			}

			break;

		case ruACTTEMPLATENODETYPE_EMBEDDEDACT:
			{
				if(m_embedded_ACT)
				{
					// Recursively check dependencies
					return m_embedded_ACT->GetDependencies(dependencies);
				}
			}

			break;
	}

	return TRUE;	
}

BOOL CRuACTTemplateModifier::GetDependencies(std::vector<CRuString>& dependencies)
{
	switch(m_modifier)
	{
		case ruACTTEMPLATEMODIFIERTYPE_TEXTURE:
			if(m_texOverride_PreloadedTexture == NULL)
			{
				dependencies.push_back(m_texOverride_TextureName);
			}
			break;

		case ruACTTEMPLATEMODIFIERTYPE_SHADER:
		case ruACTTEMPLATEMODIFIERTYPE_MATPRIORITY:
		case ruACTTEMPLATEMODIFIERTYPE_BILLBOARD:
		case ruACTTEMPLATEMODIFIERTYPE_ORIENTATION:
		case ruACTTEMPLATEMODIFIERTYPE_PROPERTIES:
		case ruACTTEMPLATEMODIFIERTYPE_MATPROPERTIES:
			break;
	}

	return TRUE;
}
BOOL CRuACTTemplate::GetDependencies(std::vector<CRuString>& dependencies)
{
	BOOL hasDependencies = FALSE;

	// Iterate through template nodes
	for(INT32 i = 0; i < m_templateNodes.Count(); ++i)
	{
		if(m_templateNodes[i]->GetDependencies(dependencies) == TRUE)
		{
			hasDependencies = TRUE;
		}
	}

	// Iterate through modifiers
	for(INT32 i = 0; i < m_templateModifiers.Count(); ++i)
	{
		if(m_templateModifiers[i]->GetDependencies(dependencies) == TRUE)
		{
			hasDependencies = TRUE;
		}
	}

	return hasDependencies;
}
#endif // KALYDO

CRuACTEntity *CRuACTTemplate::Construct()
{
	CRuArrayList<CRuACTTemplateNode_Runtime *> nodeRuntimeArray;
	CRuStack<CRuEntity *> entityStack;
	CRuArrayList<CRuEntity *> entityArray;

	// Construct ACT entity
	CRuACTEntity *actEntity = ruNEW CRuACTEntity();
	actEntity->m_template = this;
	actEntity->SetResourceName(this->GetResourceName());
	this->AddRef();										// Since the entity holds a reference to this template, increment reference count

	// Iterate through template nodes and construct each entity
	for(INT32 i = 0; i < m_templateNodes.Count(); ++i)
	{
		nodeRuntimeArray.Add(m_templateNodes[i]->Construct());
	}

	// Construct entity tree from the template nodes
	CRuArrayList<CRuEntity *> rootLevelNodes;
	CRuArrayList<CRuACTTemplateNode_Runtime *> failedNodes;

	// Fill the node queue with unprocessed nodes
	CRuQueue<CRuACTTemplateNode_Runtime *> nodeQueue;
	for(INT32 i = 0; i < nodeRuntimeArray.Count(); ++i)
	{
		nodeQueue.Queue(nodeRuntimeArray[i]);
	}

	INT32 failCounter = nodeQueue.Count();
	BOOL firstNode = TRUE;

 	while(nodeQueue.IsEmpty() == FALSE && failCounter > 0)
	{
		CRuACTTemplateNode_Runtime *runtimeNode = nodeQueue.Dequeue();

		if(runtimeNode)
		{
			CRuACTTemplateNode *templateNode = runtimeNode->GetTemplateNode();

			CRuEntity *rootEntity = runtimeNode->GetRootEntity();
			const char *anchorPath = templateNode->GetAnchorTarget();

			if(rootEntity) 
			{
				if(anchorPath == NULL)
				{
					// If the template node is a top-level node, add its entity to the root level
					rootLevelNodes.Add(rootEntity);
				}
				else
				{
					// Find the node to anchor to based on anchor path
					CRuACTTemplateNode_Runtime *anchorNode = FindRuntimeNode(nodeRuntimeArray.Count(), nodeRuntimeArray.GetArray(), anchorPath);

					if(anchorNode)
					{
						// Anchor to the target node
						runtimeNode->AnchorTo(anchorNode);
						firstNode = FALSE;
					}
					else
					{
						// Re-queue the node back onto queue
						nodeQueue.Queue(runtimeNode);
						--failCounter;
						continue;
					}
				}

				// Refresh fail counter
				failCounter = nodeQueue.Count();

				if(firstNode)
				{
					// Inherit option flags from the template node, always inheriting the least restrictive option
					actEntity->SetOption(ruENTITYOPTION_DISTANCECLASS, rootEntity->GetOption(ruENTITYOPTION_DISTANCECLASS));
					actEntity->SetOption(ruENTITYOPTION_REFLECTCLASS, rootEntity->GetOption(ruENTITYOPTION_REFLECTCLASS));
					actEntity->SetOption(ruENTITYOPTION_CASTERCLASS, rootEntity->GetOption(ruENTITYOPTION_CASTERCLASS));
					actEntity->SetOption(ruENTITYOPTION_RECEIVERCLASS, rootEntity->GetOption(ruENTITYOPTION_RECEIVERCLASS));
				}
				else
				{
					// Inherit option flags from the template node, always inheriting the least restrictive option
					actEntity->SetOption(ruENTITYOPTION_DISTANCECLASS, min(actEntity->GetOption(ruENTITYOPTION_DISTANCECLASS), rootEntity->GetOption(ruENTITYOPTION_DISTANCECLASS)));
					actEntity->SetOption(ruENTITYOPTION_REFLECTCLASS, min(actEntity->GetOption(ruENTITYOPTION_REFLECTCLASS), rootEntity->GetOption(ruENTITYOPTION_REFLECTCLASS)));
					actEntity->SetOption(ruENTITYOPTION_CASTERCLASS, min(actEntity->GetOption(ruENTITYOPTION_CASTERCLASS), rootEntity->GetOption(ruENTITYOPTION_CASTERCLASS)));
					actEntity->SetOption(ruENTITYOPTION_RECEIVERCLASS, min(actEntity->GetOption(ruENTITYOPTION_RECEIVERCLASS), rootEntity->GetOption(ruENTITYOPTION_RECEIVERCLASS)));
				}
			}
		}
	}

	// Attach all entities which failed to attach at the top-level
	while(nodeQueue.IsEmpty() == FALSE)
	{
		CRuACTTemplateNode_Runtime *runtimeNode = nodeQueue.Dequeue();

		if(runtimeNode)
		{
			CRuEntity *rootEntity = runtimeNode->GetRootEntity();

			if(rootEntity) 
			{
				CRuACTTemplateNode *templateNode = runtimeNode->GetTemplateNode();
				templateNode->SetAnchorTarget(NULL);

				// If the template node is a top-level node, add its entity to the root level
				rootLevelNodes.Add(rootEntity);

				// Inherit option flags from the template node, always inheriting the least restrictive option
				actEntity->SetOption(ruENTITYOPTION_DISTANCECLASS, min(actEntity->GetOption(ruENTITYOPTION_DISTANCECLASS), rootEntity->GetOption(ruENTITYOPTION_DISTANCECLASS)));
				actEntity->SetOption(ruENTITYOPTION_REFLECTCLASS, min(actEntity->GetOption(ruENTITYOPTION_REFLECTCLASS), rootEntity->GetOption(ruENTITYOPTION_REFLECTCLASS)));
				actEntity->SetOption(ruENTITYOPTION_CASTERCLASS, min(actEntity->GetOption(ruENTITYOPTION_CASTERCLASS), rootEntity->GetOption(ruENTITYOPTION_CASTERCLASS)));
				actEntity->SetOption(ruENTITYOPTION_RECEIVERCLASS, min(actEntity->GetOption(ruENTITYOPTION_RECEIVERCLASS), rootEntity->GetOption(ruENTITYOPTION_RECEIVERCLASS)));
			}
		}
	}

	// Construct modifiers
	CRuArrayList<CRuACTTemplateModifier *> failedModifiers;

	for(INT32 i = 0; i < m_templateModifiers.Count(); ++i)
	{
		CRuACTTemplateNode_Runtime *targetRuntimeNode = FindRuntimeNode(nodeRuntimeArray.Count(), nodeRuntimeArray.GetArray(), m_templateModifiers[i]->GetAnchorTarget());

		if(targetRuntimeNode)
		{
			if(m_templateModifiers[i]->Construct(targetRuntimeNode) == FALSE)
			{
				// Add the modifier to the list of modifiers that failed to attach
				failedModifiers.Add(m_templateModifiers[i]);
			}
		}
	}

	// Insert root level nodes into tree
	for(INT32 i = 0; i < rootLevelNodes.Count(); ++i)
	{
		actEntity->AddChild(rootLevelNodes[i]);
	}

	// Save runtime nodes into ACT entity
	actEntity->m_numRuntimeNodes = nodeRuntimeArray.Count();
	actEntity->m_runtimeNodes = ruNEW CRuACTTemplateNode_Runtime * [actEntity->m_numRuntimeNodes];
	for(INT32 i = 0; i < nodeRuntimeArray.Count(); ++i)
	{
		actEntity->m_runtimeNodes[i] = nodeRuntimeArray[i];
	}

	// Set ACT entity object name
	actEntity->SetObjectName(this->GetObjectName());

	// Set ACT entity options
	actEntity->SetOption(ruENTITYOPTION_TRANSFORMMODE, m_transformMode);
	actEntity->SetOption(ruENTITYOPTION_CULLMODE, m_cullMode);

	// Force update so we get the correct world transforms at start
	actEntity->Update(0.0f);

	return actEntity;
}

const char *CRuACTTemplate::FindAnchorTarget(INT32 numRuntimeNodes, CRuACTTemplateNode_Runtime **nodeRuntimeArray, CRuEntity *anchorEntity)
{
	if(anchorEntity)
	{
		for(INT32 j = 0; j < numRuntimeNodes; ++j)
		{
			if(nodeRuntimeArray[j] == NULL)
				continue;

			CRuACTTemplateNode_Runtime *runtimeNode = nodeRuntimeArray[j];
			const char *anchorName = runtimeNode->GetInteriorAnchorName(anchorEntity);

			if(anchorName)
			{
				return anchorName;
			}
		}
	}

	return NULL;
}

CRuACTTemplateNode_Runtime *CRuACTTemplate::FindRuntimeNode(INT32 numRuntimeNodes, CRuACTTemplateNode_Runtime **nodeRuntimeArray, const char *anchorPath)
{
	if(anchorPath)
	{
		for(INT32 j = 0; j < numRuntimeNodes; ++j)
		{
			if(nodeRuntimeArray[j] == NULL)
				continue;

			CRuACTTemplateNode_Runtime *runtimeNode = nodeRuntimeArray[j];
			CRuACTTemplateNode *templateNode = runtimeNode->GetTemplateNode();

			for(INT32 k = 0; k < runtimeNode->GetNumEntities(); ++k)
			{
				const char *nodePath = runtimeNode->GetAnchorPath(k);

				if(nodePath && SafeStricmp(anchorPath, nodePath) == 0)
				{
					return runtimeNode;
				}
			}
		}
	}

	return NULL;
}

CRuACTTemplateNode_Runtime *CRuACTTemplate::FindRuntimeNode(INT32 numRuntimeNodes, CRuACTTemplateNode_Runtime **nodeRuntimeArray, CRuACTTemplateNode *node)
{
	if(node)
	{
		for(INT32 j = 0; j < numRuntimeNodes; ++j)
		{
			if(nodeRuntimeArray[j] == NULL)
				continue;

			CRuACTTemplateNode_Runtime *runtimeNode = nodeRuntimeArray[j];

			if(runtimeNode->GetTemplateNode() == node)
			{
				return runtimeNode;
			}
		}
	}

	return NULL;
}

BOOL CRuACTTemplate::HasMatchingPrefix(const char *prefix, const char *fullString)
{
	// Maybe check if fullString is shorter than prefix -- if that is the case we can just return FALSE

	// Count the number of sub-paths in the prefix by counting '/' characters and then adding 1
	INT32 prefixSubPaths = 1;
	for(INT32 i = 0; prefix[i] != NULL; ++i)
	{
		if(prefix[i] == '/')
		{
			++prefixSubPaths;
		}
	}

	// Scan full string to see if it matches the prefix at the same number of sub-paths
	INT32 fullStringSubPaths = 0;
	INT32 charsToCompare = 0;
	for(INT32 i = 0; fullString[i] != NULL; ++i, ++charsToCompare)
	{
		if(fullString[i] == '/')
		{
			++fullStringSubPaths;

			// If the two strings now have the same number of sub-paths, do the substring comparison now
			if(fullStringSubPaths == prefixSubPaths)
			{
				if(_strnicmp(fullString, prefix, charsToCompare) == 0)
				{
					return TRUE;
				}

				break;
			}
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

CRuACT_ActionEvents::CRuACT_ActionEvents()
:	m_actEntity(NULL),
	m_disableEventTriggerCallback(FALSE),
	m_eventTriggerCallbacks(NULL)
//	m_triggeredEvents(32)
{
}

CRuACT_ActionEvents::~CRuACT_ActionEvents()
{
//	Reset();
}

void CRuACT_ActionEvents::Dispose()
{
	// Reset all states
	Reset();

	// Chain dispose to base class
	IRuObject::Dispose();
}

void CRuACT_ActionEvents::DisableEventTriggerCallback()
{
	m_disableEventTriggerCallback = TRUE;
}

void CRuACT_ActionEvents::Reset()
{
	// Remove all old event trigger callbacks
	EventTriggerCallbackDesc *curCallback = m_eventTriggerCallbacks;
	while(curCallback)
	{
		EventTriggerCallbackDesc *deletedCallback = curCallback;
		curCallback = curCallback->m_next;
		delete deletedCallback;
	}

	m_disableEventTriggerCallback = FALSE;
	m_eventTriggerCallbacks = NULL;

	// Reset action event data
	ResetActionEvents();
}

void CRuACT_ActionEvents::ResetActionEvents()
{
	// Clear all previously enumerated controllers
	for(INT32 i = 0; i < m_controllers.Count(); ++i)
	{
		ruSAFE_RELEASE(m_controllers[i]);
	}

	m_controllers.Clear();

	if(m_actEntity)
	{
		// Unregister event listener
		m_actEntity->Event_Trigger().UnregisterHandler(this->GetGUID());

		// Release reference to target ACT entity
		ruSAFE_RELEASE(m_actEntity);
	}
}

BOOL CRuACT_ActionEvents::InitializeActionEventsFromMotion(CRuACTEntity *actEntity, const char *motionName)
{
	// Abort if the entity is NULL
	if(actEntity == NULL)
	{
		return FALSE;
	}

	// Push root ACT entity onto work stack
	CRuStack<CRuEntity *> entityStack(32);
	entityStack.Push(actEntity);

	while(!entityStack.IsEmpty())
	{
		CRuEntity *entity = entityStack.Pop();

		if(entity->GetType() == CRuACTEntity::Type())
		{
			// Cast entity to ACT entity
			actEntity = static_cast<CRuACTEntity *>(entity);

			// Search for the specified motion name
			CRuACTMotion *motion = actEntity->GetTemplate()->GetMotionByName(motionName);

			if(motion)
			{
				return InitializeActionEventsFromMotion(actEntity, motion);
			}
		}

		// Push all children onto stack
		CRuEntity *curChild = entity->GetFirstChild();
		while(curChild)
		{
			entityStack.Push(curChild);
			curChild = curChild->GetNextSibling();
		}
	}

	return FALSE;
}

BOOL CRuACT_ActionEvents::InitializeActionEventsFromMotion(CRuACTEntity *actEntity, CRuACTMotion *motion)
{
	// Reset action event data
	ResetActionEvents();

	if(actEntity)
	{
		m_actEntity = actEntity;
		m_actEntity->AddRef();

		if(motion)
		{
			// Enumerate event triggers from the given motion
			motion->EnumerateControllers(&CRuController_EventTrigger::Type(), m_controllers);

			// Increment reference count on all event triggers
			for(INT32 i = 0; i < m_controllers.Count(); ++i)
			{
				m_controllers[i]->AddRef();
			}
		}

		// Register event listener
		m_actEntity->Event_Trigger().RegisterHandler(this->GetGUID(), boost::bind(&CRuACT_ActionEvents::HandleEvent_Trigger, this, _1));

		return TRUE;
	}

	return FALSE;
}

CRuACTEntity *CRuACT_ActionEvents::GetACTEntity()
{
	return m_actEntity;
}

CRuController_EventTrigger *CRuACT_ActionEvents::GetEventController(INT32 eventID)
{
	for(INT32 i = 0; i < m_controllers.Count(); ++i)
	{
		if(static_cast<CRuController_EventTrigger *>(m_controllers[i])->GetEventID() == eventID)
		{
			return static_cast<CRuController_EventTrigger *>(m_controllers[i]);
		}
	}

	return NULL;
}

CRuArrayList<IRuEntity_Controller *>& CRuACT_ActionEvents::GetControllers()
{
	return m_controllers;
}

BOOL CRuACT_ActionEvents::HandleEvent_Trigger(RuEventArgs *eventArgs)
{
	RuEntityEvent_Trigger_Args *triggerArgs = static_cast<RuEntityEvent_Trigger_Args *>(eventArgs);

	if(!m_disableEventTriggerCallback && triggerArgs->m_proxy && triggerArgs->m_proxy->GetType().IsSubClassOf(CRuController_EventTrigger::Type()))
	{
		// Cast into controller
		CRuController_EventTrigger *eventTrigger = static_cast<CRuController_EventTrigger *>(triggerArgs->m_proxy);

		// Make sure that the event is the specified event triggered by the event trigger controller
		if(eventTrigger->GetEventID() == triggerArgs->m_triggerID)
		{
			// Get the expected sequence number from self
			RuPropertyType propertyValue;
			INT32 expectedSeqNumber = this->GetProperty("mimesys:seqnumber", propertyValue) ? boost::get<INT32>(propertyValue) : 0;

			// Get sequence number from event source
			INT32 actualSeqNumber = triggerArgs->m_entity->GetProperty("mimesys:seqnumber", propertyValue) ? boost::get<INT32>(propertyValue) : 0;

			// Trigger callbacks only if the sequence numbers match -- this prevents one trigger from triggering multiple, different listeners
			if(expectedSeqNumber == actualSeqNumber)
			{
	/*
				if(m_triggeredEvents.ContainsKey(reinterpret_cast<PTRVALUE>(eventTrigger)))
				{
					OutputDebugString("Event already triggered!\n");
				}

				// Mark event trigger as having been triggered
				m_triggeredEvents.Add(reinterpret_cast<PTRVALUE>(eventTrigger));
	*/
				// Iterate through all callbacks
				EventTriggerCallbackDesc *curCallback = m_eventTriggerCallbacks;
				while(curCallback)
				{
					// If the callback is listening for this event ID, invoke it now
					if(curCallback->m_eventID == triggerArgs->m_triggerID)
					{
						curCallback->m_callback(eventTrigger, curCallback->m_userData);
					}

					// Go to next callback
					curCallback = curCallback->m_next;
				}
			}
		}
	}

	return TRUE;
}

BOOL CRuACT_ActionEvents::RegisterEventTriggerCallback(INT32 eventID, EventTriggerCallback callback, PTRVALUE userData)
{
	EventTriggerCallbackDesc *callbackDesc = ruNEW EventTriggerCallbackDesc();
	callbackDesc->m_eventID = eventID;
	callbackDesc->m_callback = callback;
	callbackDesc->m_userData = userData;
	callbackDesc->m_next = NULL;

	if(m_eventTriggerCallbacks == NULL)
	{
		m_eventTriggerCallbacks = callbackDesc;
	}
	else
	{
		EventTriggerCallbackDesc *curCallback = m_eventTriggerCallbacks;
		while(curCallback->m_next)
		{
			// Advance to next callback
			curCallback = curCallback->m_next;
		}

		curCallback->m_next = callbackDesc;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuACTEntity::ActiveMotionDesc::ActiveMotionDesc()
:	m_motionID(-1),
	m_channel(0xFFFFFFFF),
	m_channelWeight(1.0f),
	m_paused(FALSE)
{
}

CRuACTEntity::ActiveMotionDesc::~ActiveMotionDesc()
{
	Clear();
}

CRuAnimationInterpolator *CRuACTEntity::ActiveMotionDesc::GetInterpolator(CRuFrameHierarchy *hierarchy)
{
	for(INT32 i = 0; i < m_interpolators.Count(); ++i)
	{
		if(m_interpolators[i].m_boundHierarchy == hierarchy)
		{
			return m_interpolators[i].m_interpolator;
		}
	}

	InterpolatorDesc desc;
	desc.m_boundHierarchy = hierarchy;
	desc.m_interpolator = hierarchy->GetInterpolator()->Clone();
	desc.m_interpolator->SetChannel(m_channel);
	m_interpolators.Add(desc);

	return desc.m_interpolator;
}

void CRuACTEntity::ActiveMotionDesc::Synchronize(ActiveMotionDesc *keyedMotion)
{
	// Update local motion time
	m_motionTime = keyedMotion->m_motionTime;

	m_leadIn = keyedMotion->m_leadIn;
	m_leadOut = keyedMotion->m_leadOut;

	m_motionLength = keyedMotion->m_motionLength;
	m_fadeTime = keyedMotion->m_fadeTime;
	m_fadeDuration = keyedMotion->m_fadeDuration;

	// TODO: what do we do with other controllers?
	// Update controllers
	for(INT32 i = 0; i < m_controllers.Count(); ++i)
	{
		m_controllers[i]->Enable(FALSE);
		m_controllers[i]->Update(m_motionTime);

		// If the controller target is a CRuHierarchy_GR2, force it to synchronize motion times
		IRuObject *controllerTarget = m_controllers[i]->GetTarget();
		if(controllerTarget && controllerTarget->GetType() == CRuHierarchy_GR2::Type())
		{
			static_cast<CRuHierarchy_GR2 *>(controllerTarget)->SynchronizeMotionChannel(m_channel);
		}
	}

	// Update interpolators
	for(INT32 i = 0; i < m_interpolators.Count(); ++i)
	{
		// Synchronize to the keyed motion's interpolators
		m_interpolators[i].m_interpolator->Synchronize(keyedMotion->m_interpolators[i].m_interpolator);
	}
}

void CRuACTEntity::ActiveMotionDesc::Update(REAL elapsedTime, BOOL surpressControllers)
{
	// If paused, reset elapsed time to zero
	if(m_paused)
	{
		elapsedTime = 0.0f;
	}

	// Multiply elapsed time by the time multiplier
	elapsedTime *= m_timeMultiplier;

	// Update local motion time
	m_motionTime += elapsedTime;

	// Update controllers
	if(surpressControllers)
	{
		// When surpress controllers is enabled, only hierarchy controllers remain active
		for(INT32 i = 0; i < m_controllers.Count(); ++i)
		{
			if(m_controllers[i]->GetType() == CRuController_Hierarchy::Type())
			{
				m_controllers[i]->Enable(TRUE);
			}
			else
			{
				m_controllers[i]->Enable(FALSE);
			}

			m_controllers[i]->Update(elapsedTime);
		}
	}
	else
	{
		for(INT32 i = 0; i < m_controllers.Count(); ++i)
		{
			m_controllers[i]->Enable(TRUE);
			m_controllers[i]->Update(elapsedTime);
		}
	}

	// Update interpolators
	for(INT32 i = 0; i < m_interpolators.Count(); ++i)
	{
		// Update interpolator key frames
		m_interpolators[i].m_interpolator->UpdateAnimationTime(elapsedTime);
	}
}

void CRuACTEntity::ActiveMotionDesc::Mix(REAL elapsedTime)
{
	// If paused, reset elapsed time to zero
	if(m_paused)
	{
		elapsedTime = 0.0f;
	}

	if(m_motionID >= 0)
	{
		// Update interpolators
		for(INT32 i = 0; i < m_interpolators.Count(); ++i)
		{
			REAL leadInWeight = 1.0f;
			REAL leadOutWeight = 1.0f;

			if(m_leadIn > 0.0f)
			{
				if(m_motionTime < m_leadIn)
				{
					leadInWeight = max(0.0f, min(1.0f, m_motionTime / m_leadIn));
				}
			}

			if(m_leadOut > 0.0f)
			{
				REAL timeLeft = m_motionLength - m_motionTime;
				if(timeLeft < m_leadOut)
				{
					leadOutWeight = max(0.0f, min(1.0f, timeLeft / m_leadOut));
				}
			}

			// Mix interpolator into target hierarchy
			m_interpolators[i].m_boundHierarchy->GetInterpolator()->Mix(m_interpolators[i].m_interpolator, m_channel, m_channelWeight * leadInWeight * leadOutWeight);
		}
	}
	else
	{
		if(m_fadeDuration > 0.0f)
		{
			REAL weight = 1.0f - m_fadeTime / m_fadeDuration;

			// Update interpolators
			for(INT32 i = 0; i < m_interpolators.Count(); ++i)
			{
				// Mix interpolator into target hierarchy
				m_interpolators[i].m_boundHierarchy->GetInterpolator()->Mix(m_interpolators[i].m_interpolator, m_channel, weight * m_channelWeight);
			}

			// Update fade time
			m_fadeTime += elapsedTime;

			// If the motion fade is finished, reset all controllers and interpolators
			if(m_fadeTime >= m_fadeDuration)
			{
				m_fadeTime = 0.0f;
				m_fadeDuration = 0.0f;
			}
		}
	}
}

void CRuACTEntity::ActiveMotionDesc::UpdateMotionLength()
{
	// Find length of the animation, ignoring looping
	m_motionLength = 0.0f;
	for(INT32 i = 0; i < m_controllers.Count(); ++i)
	{
		m_motionLength = max(m_motionLength, m_controllers[i]->GetStartTime() + m_controllers[i]->GetDuration());
	}
}

BOOL CRuACTEntity::ActiveMotionDesc::IsMotionComplete(BOOL ignoreLoopingControllers) const
{
	REAL duration = 0.0f;

	// Find length of the animation
	for(INT32 i = 0; i < m_controllers.Count(); ++i)
	{
		// Look for a motion termination event
		if(m_controllers[i]->GetType() == CRuController_EventTrigger::Type())
		{
			if(static_cast<CRuController_EventTrigger *>(m_controllers[i])->GetEventID() == ruENTITYEVENT_INTERRUPTIBLE)
			{
				if(m_motionTime >= m_controllers[i]->GetStartTime())
				{
					return TRUE;
				}
			}
		}

		if(ignoreLoopingControllers)
		{
			if(m_controllers[i]->IsLooping())
			{
			}
			else
			{
				duration = max(duration, m_controllers[i]->GetStartTime() + m_controllers[i]->GetDuration());
			}
		}
		else
		{
			// If the controller is a looping controller, the motion never completes
			if(m_controllers[i]->IsLooping())
			{
				return FALSE;
			}

			duration = max(duration, m_controllers[i]->GetStartTime() + m_controllers[i]->GetDuration());
		}
	}

	// If local motion time exceeds duration, the motion is done
	if(m_motionTime >= duration)
	{
		return TRUE;
	}

	return FALSE;
}

void CRuACTEntity::ActiveMotionDesc::ClearMotion(REAL transitionDuration, REAL fadeDuration)
{
	if(m_motionID >= 0)
	{
		// Store fading parameters
		m_fadeTime = 0.0f;
		m_fadeDuration = fadeDuration;

		// Reset motion ID
		m_motionID = -1;

		// Reset all interpolators
		for(INT32 i = 0; i < m_interpolators.Count(); ++i)
		{
			m_interpolators[i].m_interpolator->ClearAnimations(transitionDuration);
		}

		// Remove all controllers
		for(INT32 i = 0; i < m_controllers.Count(); ++i)
		{
			IRuEntity_Controller *controller = m_controllers[i];
			IRuObject *target = m_controllers[i]->GetTarget();

			// If the controller is a hierarchy controller and the target is CRuHierarchy_GR2, clear its state
			if(target && controller->GetType().IsSubClassOf(CRuController_Hierarchy::Type()))
			{
				if(target->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
				{
					static_cast<CRuHierarchy_GR2 *>(target)->ClearAnimation(static_cast<CRuController_Hierarchy *>(controller)->GetMotionChannel(), transitionDuration);
				}
			}

			ruSAFE_RELEASE(m_controllers[i]);
		}
		m_controllers.Clear();
	}
}

void CRuACTEntity::ActiveMotionDesc::Clear()
{
	m_motionID = -1;

	for(INT32 i = 0; i < m_interpolators.Count(); ++i)
	{
		ruSAFE_RELEASE(m_interpolators[i].m_interpolator);
	}

	for(INT32 i = 0; i < m_controllers.Count(); ++i)
	{
		ruSAFE_RELEASE(m_controllers[i]);
	}
}

// ************************************************************************************************************************************************************

CRuACTEntity::CRuACTEntity()
:	m_template(NULL),
	m_numRuntimeNodes(0),
	m_runtimeNodes(NULL),
	m_motionTime(0.0f)
{
	m_persistentMotion = ruNEW ActiveMotionDesc();
	m_persistentMotion->m_channel = 0xFFFFFFFF;
}

CRuACTEntity::~CRuACTEntity()
{
	// Release the ACT template
	ruSAFE_RELEASE(m_template);

	if(m_runtimeNodes)
	{
		for(INT32 i = 0; i < m_numRuntimeNodes; ++i)
		{
			delete m_runtimeNodes[i];
		}

		delete[] m_runtimeNodes;
	}

	delete m_persistentMotion;

	for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
	{
		delete m_activeMotions[i];
	}
}

IRuObject *CRuACTEntity::Clone(IRuObject *clonedObject)
{
	if(m_template)
	{
		CRuACTEntity *clonedACT =m_template->Construct();

		if(clonedACT)
		{
			clonedACT->Transform(this->GetLocalTransform(), ruTRANSFORMOP_SET);

			return clonedACT;
		}
	}

	return NULL;
}

BOOL CRuACTEntity::SerializeFrom(IRuStream *stream)
{
	return TRUE;
}

BOOL CRuACTEntity::SerializeTo(IRuStream *stream)
{
	return m_template->SerializeTo(stream);
}

BOOL CRuACTEntity::ResetStates()
{
	// Reset all motions
	for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
	{
		m_activeMotions[i]->ClearMotion(0.0f, 0.0f);
	}

	// Chain to base class
	return CRuEntity_Container_Impl::ResetStates();
}

BOOL CRuACTEntity::Update(REAL elapsedTime)
{
	// Update motion time marker
	m_motionTime += elapsedTime;

	// Update persistent motion
	m_persistentMotion->Update(elapsedTime, FALSE);

	// Clear played motions list
	m_playedMotions.Clear();

	// Update active motions
	for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
	{
		BOOL surpressControllers = FALSE;

		for(INT32 j = 0; j < m_playedMotions.Count(); ++j)
		{
			if(m_playedMotions[j] == m_activeMotions[i]->m_motionID)
			{
				surpressControllers = TRUE;
				break;
			}
		}

		m_activeMotions[i]->Update(elapsedTime, surpressControllers);
		m_playedMotions.Add(m_activeMotions[i]->m_motionID);
	}

	for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
	{
		m_activeMotions[i]->Mix(elapsedTime);
	}

	// Create any pending animation compensators
	CreatePendingAnimationCompensators();

	// Chain update call to parent class
	return CRuEntity_Container_Impl::Update(elapsedTime);
}

CRuACTTemplate *CRuACTEntity::GetTemplate()
{
	return m_template;
}

INT32 CRuACTEntity::GetNumRuntimeNodes() const
{
	return m_numRuntimeNodes;
}

CRuACTTemplateNode_Runtime *CRuACTEntity::GetRuntimeNode(INT32 index)
{
	if(index >= 0 && index < m_numRuntimeNodes)
	{
		return m_runtimeNodes[index];
	}

	return NULL;
}

CRuACTTemplateNode_Runtime **CRuACTEntity::GetRuntimeNodes()
{
	return m_runtimeNodes;
}

const char *CRuACTEntity::GetAnchorName(CRuEntity *anchorEntity)
{
	for(INT32 i = 0; i < m_numRuntimeNodes; ++i)
	{
		if(m_runtimeNodes[i])
		{
			const char *anchorName = m_runtimeNodes[i]->GetInteriorAnchorName(anchorEntity);

			if(anchorName)
			{
				return anchorName;
			}
		}
	}

	return NULL;
}

CRuEntity *CRuACTEntity::FindEntity(const char *anchorTarget)
{
	for(INT32 i = 0; i < m_numRuntimeNodes; ++i)
	{
		if(m_runtimeNodes[i])
		{
			CRuEntity *interiorEntity = m_runtimeNodes[i]->GetInteriorEntity(anchorTarget);

			if(interiorEntity)
			{
				return interiorEntity;
			}
		}
	}

	return NULL;
}

CRuEntity *CRuACTEntity::FindEntity(INT32 parentIndex, INT32 connectivityIndex)
{
	if(parentIndex >= 0 && parentIndex < m_numRuntimeNodes && m_runtimeNodes[parentIndex])
	{
		return m_runtimeNodes[parentIndex]->GetInteriorEntity(connectivityIndex);
	}

	return NULL;
}

BOOL CRuACTEntity::UpdateTemplate(BOOL replaceTemplateNodes)
{
	// Iterate through template nodes and force each to update themselves
	for(INT32 i = 0; i < m_numRuntimeNodes; ++i)
	{
		if(m_runtimeNodes[i])
		{
			m_runtimeNodes[i]->UpdateTemplateNode(replaceTemplateNodes);
		}
	}

	return TRUE;
}

BOOL CRuACTEntity::ValidateAnchorPaths()
{
	for(INT32 i = 0; i < m_numRuntimeNodes; ++i)
	{
		CRuACTTemplateNode_Runtime *runtimeNode0 = m_runtimeNodes[i];

		if(runtimeNode0)
		{
			for(INT32 j = 0; j < runtimeNode0->GetNumEntities(); ++j)
			{
				// If entity is marked as no-enumerate, skip it
				CRuEntity *entity0 = runtimeNode0->GetInteriorEntity(j);

				RuPropertyType propertyValue;
				if(entity0->GetProperty("act:no enumerate", propertyValue))
				{
					if(boost::get<BOOL>(propertyValue) != false)
						continue;
				}

				const char *anchorPath0 = runtimeNode0->GetAnchorPath(j);

				for(INT32 k = 0; k < m_numRuntimeNodes; ++k)
				{
					CRuACTTemplateNode_Runtime *runtimeNode1 = m_runtimeNodes[k];

					if(runtimeNode1)
					{
						for(INT32 l = 0; l < runtimeNode1->GetNumEntities(); ++l)
						{
							// Skip comparison with self
							if(i == k && j == l)
								continue;

							const char *anchorPath1 = runtimeNode1->GetAnchorPath(l);

							if(SafeStricmp(anchorPath0, anchorPath1) == 0)
							{
								return FALSE;
							}
						}
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL CRuACTEntity::PlayPersistentMotion(BOOL forceOverride)
{
	if(m_persistentMotion->m_motionID != 0 || forceOverride)
	{
		// Store new motion ID
		m_persistentMotion->m_motionID = 0;

		if(m_template)
		{
			// Get persistent motion (motion ID 0 is always the persistent motion)
			CRuACTMotion *motion = m_template->GetMotionByID(0);

			if(motion)
			{
				// Clear persistent motion
				m_persistentMotion->ClearMotion(0.0f, 0.0f);

				// Store new motion ID
				m_persistentMotion->m_motionID = 0;

				// Clear local motion time
				m_persistentMotion->m_motionTime = 0.0f;

				// Set time multiplier to 1
				m_persistentMotion->m_timeMultiplier = 1.0f;

				// Set channel weight, lead-in, and lead-out times
				m_persistentMotion->m_channelWeight = 1.0f;
				m_persistentMotion->m_leadIn = 0.0f;
				m_persistentMotion->m_leadOut = 0.0f;

				// Iterate through each motion node and create a controller for each
				for(INT32 i = 0; i < motion->GetNumMotionNodes(); ++i)
				{
					CRuACTMotionNode *motionNode = motion->GetMotionNodeByIndex(i);
					IRuEntity_Controller *controller = motionNode->GetController();

					CRuEntity *targetEntity = this->FindEntity(motionNode->GetAnchorTarget());

					if(targetEntity && controller)
					{
						// If the target entity is a hierarchy and the controller is a hierarchy controller, perform pose blend out
						if(controller->GetType().IsSubClassOf(CRuController_Hierarchy::Type()))
						{
							if(targetEntity->GetType().IsSubClassOf(CRuFrameHierarchy::Type()))
							{
								// Setup target
								controller->SetTarget(m_persistentMotion->GetInterpolator(static_cast<CRuFrameHierarchy *>(targetEntity)));

								// Force update with zero delta
								controller->Update(0.0f);

								// Add controller to the list of controllers
								m_persistentMotion->m_controllers.Add(controller);
							}
							else if(targetEntity->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
							{
								// Setup target
								controller->SetTarget(targetEntity);

								// Setup channel
								static_cast<CRuController_Hierarchy *>(controller)->SetMotionChannel(0xFFFFFFFF);
								static_cast<CRuController_Hierarchy *>(controller)->SetTimeMultiplier(m_persistentMotion->m_timeMultiplier);

								// Force update with zero delta
								controller->Update(0.0f);

								// Add controller to the list of controllers
								m_persistentMotion->m_controllers.Add(controller);
							}
						}
						else
						{
							// Setup target
							controller->SetTarget(targetEntity);

							// Force update with zero delta
							controller->Update(0.0f);

							// Add controller to the list of controllers
							m_persistentMotion->m_controllers.Add(controller);
						}
					}

				}

				// Update motion length
				m_persistentMotion->UpdateMotionLength();

				return TRUE;
			}
		}

		return FALSE;
	}

	return TRUE;
}

BOOL CRuACTEntity::PlayMotion(UINT32 channel, INT32 motionID, REAL blendTime, BOOL forceOverride, REAL channelWeight, REAL leadIn, REAL leadOut, REAL expectedMotionScale, CRuACT_ActionEvents *actionEventsOut)
{
	INT32 motionIndex = m_template->GetMotionIndexByID(motionID);

	return PlayMotionByIndex(channel, motionIndex, blendTime, forceOverride, channelWeight, leadIn, leadOut, expectedMotionScale, actionEventsOut);
}

BOOL CRuACTEntity::PlayMotionByIndex(UINT32 channel, INT32 motionIndex, REAL blendTime, BOOL forceOverride, REAL channelWeight, REAL leadIn, REAL leadOut, REAL expectedMotionScale, CRuACT_ActionEvents *actionEventsOut)
{
	if(m_template)
	{
		// Attempt to get motion data for the specified motion ID
		CRuACTMotion *motion = m_template->GetMotion(motionIndex);

		if(motion)
		{
			// If an action events object is given initialize it now
			if(actionEventsOut)
			{
				actionEventsOut->InitializeActionEventsFromMotion(this, motion);
			}

			INT32 motionID = motion->GetMotionID();

			INT32 motionDescIdx = GetMotionDescriptorIndex(channel);
			ActiveMotionDesc *motionDesc = m_activeMotions[motionDescIdx];

			// Set the motion time multiplier
			if(expectedMotionScale < 0.0f)
			{
				motionDesc->m_timeMultiplier = expectedMotionScale * -1.0f;
			}
			else
			{
				if(expectedMotionScale == 0.0f || motion->GetMotionScale() == 0.0f)
				{
					motionDesc->m_timeMultiplier = 1.0f;
				}
				else
				{
					motionDesc->m_timeMultiplier = expectedMotionScale * motion->GetMotionScale();
				}
			}

			// Play motion only if the existing motion is not the same, or if an override is forced
			if(motionDesc->m_motionID != motionID || forceOverride)
			{
				// Clear global motion time
				m_motionTime = 0.0f;

				// Clear motion
				motionDesc->ClearMotion(blendTime, 0.0f);

				// Store new motion ID
				motionDesc->m_motionID = motionID;

				// Clear local motion time
				motionDesc->m_motionTime = 0.0f;

				// Set channel weight
				motionDesc->m_channelWeight = channelWeight;

				// Set lead-in and lead-out times
				motionDesc->m_leadIn = leadIn;
				motionDesc->m_leadOut = leadOut;

				// Iterate through each motion node and create a controller for each
				for(INT32 i = 0; i < motion->GetNumMotionNodes(); ++i)
				{
					CRuACTMotionNode *motionNode = motion->GetMotionNodeByIndex(i);
					IRuEntity_Controller *controller = motionNode->GetController();

					CRuEntity *targetEntity = this->FindEntity(motionNode->GetAnchorTarget());

					if(targetEntity && controller)
					{
						// If the target entity is a hierarchy and the controller is a hierarchy controller, perform pose blend out
						if(controller->GetType().IsSubClassOf(CRuController_Hierarchy::Type()))
						{
							if(targetEntity->GetType().IsSubClassOf(CRuFrameHierarchy::Type()))
							{
								// Setup target
								controller->SetTarget(motionDesc->GetInterpolator(static_cast<CRuFrameHierarchy *>(targetEntity)));

								// Force update with zero delta
								controller->Update(0.0f);

								// Add controller to the list of controllers
								motionDesc->m_controllers.Add(controller);
							}
							else if(targetEntity->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
							{
								// Setup target
								controller->SetTarget(targetEntity);

								// Setup channel
								static_cast<CRuController_Hierarchy *>(controller)->SetMotionChannel(channel);
								static_cast<CRuController_Hierarchy *>(controller)->SetTimeMultiplier(motionDesc->m_timeMultiplier);

								// Force update with zero delta
								controller->Update(0.0f);

								// Add controller to the list of controllers
								motionDesc->m_controllers.Add(controller);
							}
						}
						else
						{
							// Setup target
							controller->SetTarget(targetEntity);

							// Force update with zero delta
							controller->Update(0.0f);

							// Add controller to the list of controllers
							motionDesc->m_controllers.Add(controller);
						}
					}
					else
					{
						ruSAFE_RELEASE(controller);
					}
				}

				// Update motion length
				motionDesc->UpdateMotionLength();

				// Motion synchronization is only done if the motion is not forcibly set
				if(forceOverride == FALSE)
				{
					// Perform motion synchronization with existing motions
					SynchronizeMotion(motionDescIdx);
				}
			}
			else
			{
				// Update the time multiplier
				for(INT32 i = 0; i <  motionDesc->m_controllers.Count(); ++i)
				{
					if(motionDesc->m_controllers[i]->GetType().IsSubClassOf(CRuController_Hierarchy::Type()))
					{
						static_cast<CRuController_Hierarchy *>(motionDesc->m_controllers[i])->SetTimeMultiplier(motionDesc->m_timeMultiplier);
					}
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuACTEntity::SwapMotion(UINT32 srcchannel, UINT32 destchannel)
{
	if(m_template)
	{
		INT32 srcMotionIndex = GetMotionDescriptorIndex(srcchannel);
		INT32 destMotionIndex = GetMotionDescriptorIndex(destchannel);
		ActiveMotionDesc *srcMotionDesc = m_activeMotions[srcMotionIndex];
		ActiveMotionDesc *destMotionDesc = m_activeMotions[destMotionIndex];

		m_activeMotions[srcMotionIndex] = destMotionDesc;
		m_activeMotions[destMotionIndex] = srcMotionDesc;

		srcMotionDesc->m_channel = destchannel;
		destMotionDesc->m_channel = srcchannel;
	}

	return FALSE;
}

BOOL CRuACTEntity::ClearMotion(UINT32 channel, REAL fadeDuration)
{
	// Search for the specified group ID mask among currently active motions
	for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
	{
		if(m_activeMotions[i]->m_channel == channel)
		{
			m_activeMotions[i]->ClearMotion(0.0f, fadeDuration);

			return TRUE;
		}
	}

	return FALSE;
}

/*!
	Synchronizes the motion at syncIndex with any compatible motion on other channels.
	A compatible motion is defined as one which has the same motion ID as the one being
	synchronized. This function synchronizes time only. Controller timing will not
	necessarily be correct. It is also assumed that the motion to be synchronized has a
	motion time of zero.

	@param syncIndex Index of the motion to synchronize.

	@return TRUE if successful, FALSE otherwise.
*/
BOOL CRuACTEntity::SynchronizeMotion(INT32 syncIndex)
{
	if(syncIndex >= 0 && syncIndex < m_activeMotions.Count())
	{
		UINT32 channel = m_activeMotions[syncIndex]->m_channel;

		// If the motion has already been started, abort synchronization
		if(m_activeMotions[syncIndex]->m_motionTime > 0.0f)
		{
			return FALSE;
		}

		// Search for a compatible motion
		for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
		{
			if(i == syncIndex)
				continue;

			if(m_activeMotions[i]->m_motionID == m_activeMotions[syncIndex]->m_motionID)
			{
				m_activeMotions[syncIndex]->Synchronize(m_activeMotions[i]);
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CRuACTEntity::PauseChannel(UINT32 channel)
{
	// Search for the specified channel
	for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
	{
		if(m_activeMotions[i]->m_channel == channel)
		{
			m_activeMotions[i]->m_paused = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuACTEntity::ResumeChannel(UINT32 channel)
{
	// Search for the specified channel
	for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
	{
		if(m_activeMotions[i]->m_channel == channel)
		{
			m_activeMotions[i]->m_paused = FALSE;
			return TRUE;
		}
	}

	return FALSE;
}

void CRuACTEntity::CreatePendingAnimationCompensators()
{
	for(INT32 i = 0; i < m_pendingAnimCompensators.Count(); ++i)
	{
		// Search for the specified target channel
		for(INT32 j = 0; j < m_activeMotions.Count(); ++j)
		{
			if(m_activeMotions[j]->m_channel == m_pendingAnimCompensators[i].m_targetChannel)
			{
				// Iterate through all interpolators in the target channel and find one that has the specified node ID
				for(INT32 k = 0; k < m_activeMotions[k]->m_interpolators.Count(); ++k)
				{
					INT32 subNodeIndex = m_activeMotions[j]->m_interpolators[k].m_boundHierarchy->GetSubNodeIndex(m_pendingAnimCompensators[i].m_nodeID);

					if(subNodeIndex >= 0)
					{
						// Search for the source channel
						for(INT32 l = 0; l < m_activeMotions.Count(); ++l)
						{
							if(m_activeMotions[l]->m_channel == m_pendingAnimCompensators[i].m_sourceChannel)
							{
								// Construct animation compensator
								CRuController_AnimCompensator *compController = ruNEW CRuController_AnimCompensator();
								compController->SetTarget(m_activeMotions[j]->m_interpolators[k].m_boundHierarchy);
								compController->SetCompensatorSource(m_activeMotions[l]->m_interpolators[k].m_interpolator, m_activeMotions[j]->m_interpolators[k].m_interpolator, m_pendingAnimCompensators[i].m_nodeID, m_pendingAnimCompensators[i].m_strength);

								// Add animation compensator to entity list
								this->AddChild(compController);

								// Release local reference to animation compensator
								ruSAFE_RELEASE(compController);

								// Remove from pending list
								m_pendingAnimCompensators.RemoveAt(i);
								--i;

								break;
							}
						}

						break;
					}
				}

				break;
			}
		}
	}
}

BOOL CRuACTEntity::CreateAnimationCompensator(UINT32 sourceChannel, UINT32 targetChannel, INT32 nodeID, REAL strength)
{
	AnimCompensatorDesc cmpDesc;

	cmpDesc.m_sourceChannel = sourceChannel;
	cmpDesc.m_targetChannel = targetChannel;
	cmpDesc.m_nodeID = nodeID;
	cmpDesc.m_strength = strength;

	m_pendingAnimCompensators.Add(cmpDesc);

	return TRUE;
}

INT32 CRuACTEntity::GetActiveMotionID(UINT32 channel) const
{
	// Search for the specified group ID mask among currently active motions
	for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
	{
		if(m_activeMotions[i]->m_channel == channel)
		{
			return m_activeMotions[i]->m_motionID;
		}
	}

	return -1;
}

const char *CRuACTEntity::GetActiveMotionName(UINT32 channel) const
{
	// Scan through all active motion channels
	for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
	{
		if(m_activeMotions[i]->m_channel == channel)
		{
			return m_template->GetMotionNameByID(m_activeMotions[i]->m_motionID);
		}
	}

	return "";
}

REAL CRuACTEntity::GetActiveMotionTime(UINT32 channel) const
{
	// Scan through all active motion channels
	for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
	{
		if(m_activeMotions[i]->m_channel == channel)
		{
			return m_activeMotions[i]->m_motionTime;
		}
	}

	return -1.0f;
}

CRuArrayList<IRuEntity_Controller *> *CRuACTEntity::GetActiveMotionControllers(UINT32 channel)
{
	// Scan through all active motion channels
	for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
	{
		if(m_activeMotions[i]->m_channel == channel)
		{
			return &(m_activeMotions[i]->m_controllers);
		}
	}

	return NULL;
}

REAL CRuACTEntity::GetMotionTime() const
{
	return m_motionTime;
}

BOOL CRuACTEntity::IsActiveMotion(UINT32 channel, const char *motionName)
{
	INT32 motionID = m_template->GetMotionIDByName(motionName);

	if(motionID >= 0)
	{
		// Search for the specified group ID mask among currently active motions
		for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
		{
			if(m_activeMotions[i]->m_channel == channel)
			{
				return m_activeMotions[i]->m_motionID == motionID;
			}
		}
	}

	return FALSE;
}

BOOL CRuACTEntity::IsMotionComplete(UINT32 channel, BOOL ignoreLoopingControllers) const
{
	if(channel == 0xFFFFFFFF)
	{
		// If the specified group ID mask is 0xFFFFFFFF, then we make sure all motions are complete
		for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
		{
			if(m_activeMotions[i]->IsMotionComplete(ignoreLoopingControllers) == FALSE)
			{
				return FALSE;
			}
		}
	}
	else
	{
		// Search for the specified group ID mask among currently active motions
		for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
		{
			if(m_activeMotions[i]->m_channel == channel)
			{
				return m_activeMotions[i]->IsMotionComplete(ignoreLoopingControllers);
			}
		}
	}

	return TRUE;
}

INT32 CRuACTEntity::GetMotionIDByName(const char *motionName)
{
	return m_template->GetMotionIDByName(motionName);
}

INT32 CRuACTEntity::GetMotionIndexByName(const char *motionName)
{
	return m_template->GetMotionIndexByName(motionName);
}

REAL CRuACTEntity::GetMotionLength(const char *motionName)
{
	REAL motionLength = 0.0f;
	//char buf[4096];
	//sprintf(buf, "GetMotionLength motionName:%s\n", motionName);
	//OutputDebugString(buf);
	// Retrieve motion index
	INT32 motionIndex = m_template->GetMotionIndexByName(motionName);

	if(motionIndex >= 0)
	{
		// Attempt to get motion data for the specified motion index
		CRuACTMotion *motion = m_template->GetMotion(motionIndex);

		// Enumerate all controllers specified by the motion
		CRuArrayList<IRuEntity_Controller *> controllers;
		motion->EnumerateControllers(NULL, controllers);

		for(INT32 i = 0; i < controllers.Count(); ++i)
		{
			if(controllers[i]->IsLooping())
			{
				motionLength = FLT_MAX;
			}
			else
			{
				motionLength = max(motionLength, controllers[i]->GetStartTime() + controllers[i]->GetDuration());
			}
		}
	}

	return motionLength;
}

BOOL CRuACTEntity::CheckForController(UINT32 channel, const CRuRTTI &controllerType)
{
	// Search for the specified channel mask among currently active motions
	for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
	{
		if(m_activeMotions[i]->m_channel == channel)
		{
			for(INT32 j = 0; j < m_activeMotions[i]->m_controllers.Count(); ++j)
			{
				if(m_activeMotions[i]->m_controllers[j]->GetType() == controllerType)
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL CRuACTEntity::CheckForEvent(UINT32 channel, RuEntityEvent eventType, REAL *startTimeOut)
{
	// Search for the specified channel mask among currently active motions
	for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
	{
		if(m_activeMotions[i]->m_channel == channel)
		{
			for(INT32 j = 0; j < m_activeMotions[i]->m_controllers.Count(); ++j)
			{
				if(m_activeMotions[i]->m_controllers[j]->GetType() == CRuController_EventTrigger::Type())
				{
					if(static_cast<CRuController_EventTrigger *>(m_activeMotions[i]->m_controllers[j])->GetEventID() == eventType)
					{
						if(startTimeOut)
						{
							*startTimeOut = static_cast<CRuController_EventTrigger *>(m_activeMotions[i]->m_controllers[j])->GetStartTime();
						}

						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}

INT32 CRuACTEntity::GetMotionDescriptorIndex(UINT32 channel)
{
	INT32 motionIndex = -1;
	ActiveMotionDesc *motionDesc = NULL;

	// Search for the specified group ID mask among currently active motions
	for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
	{
		if(m_activeMotions[i]->m_channel == channel)
		{
			motionIndex = i;
			motionDesc = m_activeMotions[i];
			break;
		}
	}

	// If no matching mask is found, create a new active motion descriptor
	if(motionDesc == NULL)
	{
		motionIndex = m_activeMotions.Count();
		motionDesc = ruNEW ActiveMotionDesc();
		motionDesc->m_channel = channel;

		// Find appropriate location for the motion descriptor
		for(INT32 i = 0; i < m_activeMotions.Count(); ++i)
		{
			if(m_activeMotions[i]->m_channel > motionDesc->m_channel)
			{
				m_activeMotions.Insert(i, motionDesc);
				return motionIndex;
			}
		}

		m_activeMotions.Add(motionDesc);
	}

	return motionIndex;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
