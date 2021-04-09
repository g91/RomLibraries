#include "../../Scene/Base/RuEntityContainer.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFrame, CRuEntity_Container_Impl)
ruHEAP_DEFINE_SUBCLASS(CRuFrame, "CRuFrame", "CRuEntity_Container_Impl")
ruCLASSGUID_DEFINE(CRuFrame, 0xB18438B4, 0x43C84504, 0xB81805B7, 0xA0DB6532)

ruRTTI_DEFINE_SUBCLASS(CRuLinkFrame, CRuEntity_Container_Impl)
ruHEAP_DEFINE_SUBCLASS(CRuLinkFrame, "CRuLinkFrame", "CRuEntity_Container_Impl")
ruCLASSGUID_DEFINE(CRuLinkFrame, 0x1B97A125, 0xA0394d4a, 0xB7F992B9, 0x043863DC)

ruRTTI_DEFINE_SUBCLASS(CRuFrameHierarchy, CRuEntity_Container_Impl)
ruHEAP_DEFINE_SUBCLASS(CRuFrameHierarchy, "CRuFrameHierarchy", "CRuEntity_Container_Impl")
ruCLASSGUID_DEFINE(CRuFrameHierarchy, 0x56852D0A, 0x8337418a, 0xA6B1AFCE, 0x765F5806)

// ************************************************************************************************************************************************************

CRuFrame::CRuFrame()
{
}

CRuFrame::~CRuFrame()
{
}

IRuObject *CRuFrame::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuFrame();

	// Chain cloning call to parent class
	CRuEntity_Container_Impl::Clone(clonedObject);

	// Shortcut reference to the cloned entity
	CRuFrame *clonedFrame = static_cast<CRuFrame *>(clonedObject);

	return clonedFrame;
}

BOOL CRuFrame::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	INT32 entityCount;
	CRuVector3 scale;
	CRuQuaternion rotation;
	CRuVector3 translation;

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

	switch(version)
	{
		case 1:
			{
				// Read number of entities
				stream->Read(&entityCount, sizeof(INT32));

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
				this->ResetLocalTransform();
				this->Scale(CRuVector3(scale.x, scale.y, scale.z));
				this->Rotate(rotation);
				this->Translate(translation);

				// For each entity held by the frame, read it in
				for(INT32 i = 0; i < entityCount; ++i)
				{
					IRuObject *object = RuObjectFactory().CreateObject(stream);

					if(object && object->GetType().IsSubClassOf(CRuEntity::Type()))
					{
						AddEntity((CRuEntity *) object);
					}

					ruSAFE_RELEASE(object);
				}
			}

			break;

		case 2:
			{
				// Read options
				stream->Read(&m_optionFlags0, sizeof(UINT32));

				// Read number of entities
				stream->Read(&entityCount, sizeof(INT32));

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
				this->ResetLocalTransform();
				this->Scale(CRuVector3(scale.x, scale.y, scale.z));
				this->Rotate(rotation);
				this->Translate(translation);

				// For each entity held by the frame, read it in
				for(INT32 i = 0; i < entityCount; ++i)
				{
					IRuObject *object = RuObjectFactory().CreateObject(stream);

					if(object && object->GetType().IsSubClassOf(CRuEntity::Type()))
					{
						AddEntity((CRuEntity *) object);
					}

					ruSAFE_RELEASE(object);
				}
			}

			break;

		case 3:
			{
				// Chain serialization to base class
				CRuEntity_Container_Impl::SerializeFrom(stream);
			}

			break;
	}

	return TRUE;
}

BOOL CRuFrame::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write signature
	INT32 signature = 0xFFFFFFFF;
	stream->Write(&signature, sizeof(INT32));

	// Write version
	INT32 version = 3;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuEntity_Container_Impl::SerializeTo(stream);

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuLinkFrame::CRuLinkFrame()
:	m_linkName(NULL)
{
}

CRuLinkFrame::~CRuLinkFrame()
{
	delete[] m_linkName;
}

IRuObject *CRuLinkFrame::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuLinkFrame();

	// Chain cloning call to parent class
	CRuEntity_Container_Impl::Clone(clonedObject);

	// Shortcut reference to the cloned entity
	CRuLinkFrame *clonedFrame = static_cast<CRuLinkFrame *>(clonedObject);

	if(m_linkName)
	{
		INT32 linkNameLength = (INT32)strlen(m_linkName) + 1;
		clonedFrame->m_linkName = ruNEW char [linkNameLength];
		memcpy(clonedFrame->m_linkName, m_linkName, linkNameLength);
	}

	return clonedFrame;
}

BOOL CRuLinkFrame::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	INT32 entityCount;
	CRuVector3 scale;
	CRuQuaternion rotation;
	CRuVector3 translation;

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	switch(version)
	{
		case 1:
			{
				// Read options
				stream->Read(&m_optionFlags0, sizeof(UINT32));

				// Read number of entities
				stream->Read(&entityCount, sizeof(INT32));

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

				// Read link name length including NULL
				INT32 linkNameLength;
				stream->Read(&linkNameLength, sizeof(INT32));

				// Read link name
				if(linkNameLength > 0)
				{
					m_linkName = ruNEW char [linkNameLength];
					stream->Read(m_linkName, sizeof(char) * linkNameLength);
				}

				// Update local transform matrix
				this->ResetLocalTransform();
				this->Scale(CRuVector3(scale.x, scale.y, scale.z));
				this->Rotate(rotation);
				this->Translate(translation);

				// For each entity held by the frame, read it in
				for(INT32 i = 0; i < entityCount; ++i)
				{
					IRuObject *object = RuObjectFactory().CreateObject(stream);

					if(object && object->GetType().IsSubClassOf(CRuEntity::Type()))
					{
						AddEntity((CRuEntity *) object);
					}

					ruSAFE_RELEASE(object);
				}
			}

			break;

		case 2:
			{
				// Chain serialization to base class
				CRuEntity_Container_Impl::SerializeFrom(stream);

				// Read link name length including NULL
				INT32 linkNameLength;
				stream->Read(&linkNameLength, sizeof(INT32));

				// Read link name
				if(linkNameLength > 0)
				{
					m_linkName = ruNEW char [linkNameLength];
					stream->Read(m_linkName, sizeof(char) * linkNameLength);
				}
			}

			break;
	}

	// Set object name to be the link name
	this->SetObjectName(m_linkName);

	return TRUE;
}

BOOL CRuLinkFrame::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 2;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuEntity_Container_Impl::SerializeTo(stream);

	if(m_linkName)
	{
		// Write link name length including NULL
		INT32 linkNameLength = (INT32)strlen(m_linkName) + 1;
		stream->Write(&linkNameLength, sizeof(INT32));

		// Write link name
		stream->Write(m_linkName, sizeof(char) * linkNameLength);
	}
	else
	{
		// Write link name length including NULL
		INT32 linkNameLength = 1;
		stream->Write(&linkNameLength, sizeof(INT32));

		// Write link name
		stream->Write("", sizeof(char) * linkNameLength);
	}

	return TRUE;
}

const char *CRuLinkFrame::GetLinkName() const
{
	if(m_linkName)
	{
		return m_linkName;
	}

	return "";
}

void CRuLinkFrame::SetLinkName(const char *linkName)
{
	delete[] m_linkName;

	m_linkName = ruNEW char [strlen(linkName) + 1];
	memcpy(m_linkName, linkName, sizeof(char) * (strlen(linkName) + 1));

	// Set object name to be the link name
	this->SetObjectName(m_linkName);
}

// ************************************************************************************************************************************************************

CRuFrameHierarchy::CRuFrameHierarchy()
:	m_numSubNodes(0),
	m_subNodes(NULL),
	m_numLinks(0),
	m_links(NULL),
	m_skinToBoneTransforms(NULL),
	m_subNodeTransforms(NULL),
	m_animationUpdateInterval(0.0166667f),
	m_timeSinceLastAnimationUpdate(0.0166667f),
	m_animationAccumulatedT(0.0f),
	m_interpolator(NULL)
{
}

CRuFrameHierarchy::~CRuFrameHierarchy()
{
	Reset();
}

BOOL CRuFrameHierarchy::Initialize(INT32 numSubNodes, INT32 numLinks, UINT32 *nodeIDs, UINT32 *groupIDs, UINT32 *nodeFlags)
{
	Reset();

	// Construct hierarchy structure
	m_numSubNodes = numSubNodes;
	m_subNodes = ruNEW SubNode [m_numSubNodes];

	m_numLinks = numLinks;
	m_links = ruNEW LinkNode [m_numLinks];

	m_skinToBoneTransforms = ruNEW CRuMatrix4x4 [m_numSubNodes];
	m_subNodeTransforms = ruNEW CRuMatrix4x4 [m_numSubNodes];

	// Initialize the animation interpolator with enough nodes to accommodate both bone nodes and link nodes
	m_interpolator = ruNEW CRuAnimationInterpolator(m_numSubNodes + m_numLinks);

	if(groupIDs)
	{
		for(INT32 i = 0; i < m_numSubNodes; ++i)
		{
			// Construct the i-th node
			m_subNodes[i].m_id = nodeIDs[i];
			m_subNodes[i].m_groupID = groupIDs[i];
			m_subNodes[i].m_flags = nodeFlags[i];
			m_subNodes[i].m_initialScale.Set(1.0f, 1.0f, 1.0f);
			m_subNodes[i].m_initialRotation.Set(0.0f, 0.0f, 0.0f, 1.0f);
			m_subNodes[i].m_initialTranslation.Set(0.0f, 0.0f, 0.0f);
		}

		for(INT32 i = 0; i < m_numLinks; ++i)
		{
			// Construct the i-th node
			m_links[i].m_linkID = -1;
			m_links[i].m_linkGroupID = groupIDs[m_numSubNodes + i];
			m_links[i].m_linkIndex = 0;
			m_links[i].m_linkFrame = NULL;
		}
	}
	else
	{
		for(INT32 i = 0; i < m_numSubNodes; ++i)
		{
			// Construct the i-th node
			m_subNodes[i].m_id = nodeIDs[i];
			m_subNodes[i].m_groupID = 0xFFFFFFFF;
			m_subNodes[i].m_flags = nodeFlags[i];
			m_subNodes[i].m_initialScale.Set(1.0f, 1.0f, 1.0f);
			m_subNodes[i].m_initialRotation.Set(0.0f, 0.0f, 0.0f, 1.0f);
			m_subNodes[i].m_initialTranslation.Set(0.0f, 0.0f, 0.0f);
		}

		for(INT32 i = 0; i < m_numLinks; ++i)
		{
			// Construct the i-th node
			m_links[i].m_linkID = -1;
			m_links[i].m_linkGroupID = 0xFFFFFFFF;
			m_links[i].m_linkIndex = 0;
			m_links[i].m_linkFrame = NULL;
		}
	}

	return TRUE;
}

BOOL CRuFrameHierarchy::InitializeDefaultPose(CRuVector3 *scaleList, CRuQuaternion *rotationList, CRuVector3 *translationList)
{
	// Clear scratch pad
	m_scratch_ParentStack.Clear();

	for(INT32 i = 0, currentParent = 0; i < m_numSubNodes; ++i)
	{
		// Set the initial transforms into the subnodes
		m_subNodes[i].m_initialScale.Set(scaleList[i]);
		m_subNodes[i].m_initialRotation.Set(rotationList[i]);
		m_subNodes[i].m_initialTranslation.Set(translationList[i]);

		// Set the initial pose into the interpolator key frame array, so models show up properly even without an animation
		m_interpolator->SetInitialOrientation(i, scaleList[i], rotationList[i], translationList[i]);

		// Construct local bone transform
		m_skinToBoneTransforms[i].SetToBoneSRT(scaleList[i], rotationList[i], translationList[i]);

		if(currentParent != i)
		{
			// Set parent index for the subnode
			m_subNodes[i].SetParentIndex(currentParent);

			// Multiply the local transform with the parent's
			m_skinToBoneTransforms[i] = m_skinToBoneTransforms[i] * m_skinToBoneTransforms[currentParent];
		}
		else
		{
			// Set parent index for the subnode (0xFF to denote a root node)
			m_subNodes[i].SetParentIndex(0xFF);
		}

		// Handle traversal of the hierarchy
		if(m_subNodes[i].m_flags & ruFRAMEHIERARCHY_PUSH)
		{
			m_scratch_ParentStack.Push(currentParent);
		}

		if(m_subNodes[i].m_flags & ruFRAMEHIERARCHY_POP)
		{
			// If the stack is empty when we hit a pop flag, that means we are starting a new tree
			if(m_scratch_ParentStack.IsEmpty())
			{
				// Set current parent to the next node
				currentParent = i + 1;
			}
			else
			{
				currentParent = m_scratch_ParentStack.Pop();
			}
		}
		else
		{
			currentParent = i;
		}
	}

	// Invert local transform to yield the skin-to-node transform for each node
	for(INT32 i = 0; i < m_numSubNodes; ++i)
	{
		m_skinToBoneTransforms[i] = m_skinToBoneTransforms[i].GetInverse();
	}

	return TRUE;
}

IRuObject *CRuFrameHierarchy::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuFrameHierarchy();

	// Chain cloning call to parent class
	CRuEntity_Container_Impl::Clone(clonedObject);

	// Make a copy of node IDs and node flags
	UINT32 *nodeIDs = ruNEW UINT32 [m_numSubNodes];
	UINT32 *groupIDs = ruNEW UINT32 [m_numSubNodes + m_numLinks];
	UINT32 *nodeFlags = ruNEW UINT32 [m_numSubNodes];

	for(INT32 i = 0; i < m_numSubNodes; ++i)
	{
		nodeIDs[i] = m_subNodes[i].m_id;
		groupIDs[i] = m_subNodes[i].m_groupID;
		nodeFlags[i] = m_subNodes[i].m_flags;
	}

	for(INT32 i = 0; i < m_numLinks; ++i)
	{
		groupIDs[i] = m_links[i].m_linkGroupID;
	}

	// Shortcut reference to the cloned entity
	CRuFrameHierarchy *clonedHierarchy = static_cast<CRuFrameHierarchy *>(clonedObject);

	// Initialize the new frame hierarchy
	clonedHierarchy->Initialize(m_numSubNodes, m_numLinks, nodeIDs, groupIDs, nodeFlags);
	clonedHierarchy->m_interpolator->SetGroupIDs(groupIDs);

	// Copy transforms and SubNodes to the clone
	for(INT32 i = 0; i < m_numSubNodes; ++i)
	{
		// Copy the subnode
		clonedHierarchy->m_subNodes[i] = m_subNodes[i];

		// Copy transforms
		clonedHierarchy->m_skinToBoneTransforms[i] = m_skinToBoneTransforms[i];
		clonedHierarchy->m_subNodeTransforms[i] = m_subNodeTransforms[i];
	}

	// Clone interpolator
	ruSAFE_RELEASE(clonedHierarchy->m_interpolator);
	clonedHierarchy->m_interpolator = m_interpolator->Clone();

	// Clone links
	for(INT32 i = 0; i < m_numLinks; ++i)
	{
		clonedHierarchy->m_links[i].m_linkIndex = m_links[i].m_linkIndex;
		clonedHierarchy->m_links[i].m_linkID = m_links[i].m_linkID;
		clonedHierarchy->m_links[i].m_linkGroupID = m_links[i].m_linkGroupID;
		clonedHierarchy->m_links[i].m_linkName = m_links[i].m_linkName;
		clonedHierarchy->m_links[i].m_linkFrame = static_cast<CRuEntity *>(m_links[i].m_linkFrame->Clone());

		// Disable cloning and serialization
		clonedHierarchy->m_links[i].m_linkFrame->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
		clonedHierarchy->m_links[i].m_linkFrame->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);

		// Attach link frame
		clonedHierarchy->AddChild(clonedHierarchy->m_links[i].m_linkFrame);
	}

	delete[] nodeIDs;
	delete[] groupIDs;
	delete[] nodeFlags;

	return clonedHierarchy;
}

BOOL CRuFrameHierarchy::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	INT32 numSubNodes;
	INT32 numLinks;
	INT32 entityCount;

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

	switch(version)
	{
		case 1:
			{
				stream->Read(&numSubNodes, sizeof(INT32));
				stream->Read(&numLinks, sizeof(INT32));
				stream->Read(&entityCount, sizeof(INT32));

				// Setup data arrays
				UINT32 *idList = ruNEW UINT32 [numSubNodes];
				UINT32 *flagList = ruNEW UINT32 [numSubNodes];
				CRuVector3 *scaleList = ruNEW CRuVector3 [numSubNodes];
				CRuQuaternion *rotationList = ruNEW CRuQuaternion [numSubNodes];
				CRuVector3 *translationList = ruNEW CRuVector3 [numSubNodes];

				// Load hierarchy structure and initial pose
				for(INT32 i = 0; i < numSubNodes; ++i)
				{
					stream->Read(&idList[i], sizeof(UINT32));
					stream->Read(&flagList[i], sizeof(UINT32));

					stream->Read(&scaleList[i].x, sizeof(REAL));
					stream->Read(&scaleList[i].y, sizeof(REAL));
					stream->Read(&scaleList[i].z, sizeof(REAL));

					stream->Read(&rotationList[i].x, sizeof(REAL));
					stream->Read(&rotationList[i].y, sizeof(REAL));
					stream->Read(&rotationList[i].z, sizeof(REAL));
					stream->Read(&rotationList[i].w, sizeof(REAL));

					stream->Read(&translationList[i].x, sizeof(REAL));
					stream->Read(&translationList[i].y, sizeof(REAL));
					stream->Read(&translationList[i].z, sizeof(REAL));
				}

				// Construct frame hierarchy
				Initialize(numSubNodes, numLinks, idList, NULL, flagList);

				// Load link points
				for(INT32 i = 0; i < numLinks; ++i)
				{
					// Read link index and ID
					stream->Read(&m_links[i].m_linkIndex, sizeof(INT32));
					stream->Read(&m_links[i].m_linkID, sizeof(INT32));

					// Construct link frame using object factory
					IRuObject *frameObj = RuObjectFactory().CreateObject(stream);

					if(frameObj)
					{
						if(frameObj->GetType() == CRuFrame::Type())
						{
							m_links[i].m_linkFrame = static_cast<CRuFrame *>(frameObj);

							// Disable cloning and serialization
							m_links[i].m_linkFrame->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
							m_links[i].m_linkFrame->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);

							// Attach link frame
							this->AddChild(m_links[i].m_linkFrame);

							// Decompose orientation transform
							CRuVector3 scale, shear, rotation, translation;
							CRuQuaternion rotationQuat;

							m_links[i].m_linkFrame->GetLocalTransform().GetDecomposition(scale, shear, rotation, translation);
							rotationQuat.MakeEulerRotation(rotation);

							// Setup default pose for the link
							m_links[i].m_initialScale = scale;
							m_links[i].m_initialRotation = rotationQuat;
							m_links[i].m_initialTranslation = translation;
						}
						else
						{
							frameObj->Release();
						}
					}
					else
					{
						m_links[i].m_linkFrame = NULL;
					}
				}

				// For each entity held by the frame, read it in
				for(INT32 i = 0; i < entityCount; ++i)
				{
					IRuObject *object = RuObjectFactory().CreateObject(stream);

					if(object->GetType().IsSubClassOf(CRuEntity::Type()))
					{
						AddEntity((CRuEntity *) object);
					}

					object->Release();
				}

				// Set skin-to-bone transforms
				InitializeDefaultPose(scaleList, rotationList, translationList);

				delete[] idList;
				delete[] flagList;
				delete[] scaleList;
				delete[] rotationList;
				delete[] translationList;
			}

			break;

		case 2:
			{
				// Read number of subnodes, links and entities
				stream->Read(&numSubNodes, sizeof(INT32));
				stream->Read(&numLinks, sizeof(INT32));
				stream->Read(&entityCount, sizeof(INT32));

				// Setup data arrays
				UINT32 *idList = ruNEW UINT32 [numSubNodes];
				UINT32 *flagList = ruNEW UINT32 [numSubNodes];
				CRuVector3 *scaleList = ruNEW CRuVector3 [numSubNodes];
				CRuQuaternion *rotationList = ruNEW CRuQuaternion [numSubNodes];
				CRuVector3 *translationList = ruNEW CRuVector3 [numSubNodes];

				// Load hierarchy structure and initial pose
				for(INT32 i = 0; i < numSubNodes; ++i)
				{
					stream->Read(&idList[i], sizeof(UINT32));
					stream->Read(&flagList[i], sizeof(UINT32));

					stream->Read(&scaleList[i].x, sizeof(REAL));
					stream->Read(&scaleList[i].y, sizeof(REAL));
					stream->Read(&scaleList[i].z, sizeof(REAL));

					stream->Read(&rotationList[i].x, sizeof(REAL));
					stream->Read(&rotationList[i].y, sizeof(REAL));
					stream->Read(&rotationList[i].z, sizeof(REAL));
					stream->Read(&rotationList[i].w, sizeof(REAL));

					stream->Read(&translationList[i].x, sizeof(REAL));
					stream->Read(&translationList[i].y, sizeof(REAL));
					stream->Read(&translationList[i].z, sizeof(REAL));
				}

				// Construct frame hierarchy
				Initialize(numSubNodes, numLinks, idList, NULL, flagList);

				// Load link points
				for(INT32 i = 0; i < numLinks; ++i)
				{
					// Read link index and ID
					stream->Read(&m_links[i].m_linkIndex, sizeof(INT32));
					stream->Read(&m_links[i].m_linkID, sizeof(INT32));

					// Read link name length including NULL
					INT32 linkNameLength = 0;
					stream->Read(&linkNameLength, sizeof(INT32));

					// Read link name
					if(linkNameLength > 0)
					{
						char *linkName = ruNEW char [linkNameLength];
						stream->Read(linkName, sizeof(char) * linkNameLength);
						m_links[i].m_linkName = linkName;
						delete[] linkName;
					}
					else
					{
						m_links[i].m_linkName = "unnamed";
					}

					// Construct link frame using object factory
					IRuObject *frameObj = RuObjectFactory().CreateObject(stream);

					if(frameObj)
					{
						if(frameObj->GetType() == CRuFrame::Type())
						{
							m_links[i].m_linkFrame = static_cast<CRuFrame *>(frameObj);

							// Disable cloning and serialization
							m_links[i].m_linkFrame->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
							m_links[i].m_linkFrame->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);

							// Attach link frame
							this->AddChild(m_links[i].m_linkFrame);

							// Decompose orientation transform
							CRuVector3 scale, shear, rotation, translation;
							CRuQuaternion rotationQuat;

							m_links[i].m_linkFrame->GetLocalTransform().GetDecomposition(scale, shear, rotation, translation);
							rotationQuat.MakeEulerRotation(rotation);

							// Setup default pose for the link
							m_links[i].m_initialScale = scale;
							m_links[i].m_initialRotation = rotationQuat;
							m_links[i].m_initialTranslation = translation;
						}
						else
						{
							frameObj->Release();
						}
					}
					else
					{
						m_links[i].m_linkFrame = NULL;
					}
				}

				// For each entity held by the frame, read it in
				for(INT32 i = 0; i < entityCount; ++i)
				{
					IRuObject *object = RuObjectFactory().CreateObject(stream);

					if(object->GetType().IsSubClassOf(CRuEntity::Type()))
					{
						AddEntity((CRuEntity *) object);
					}

					object->Release();
				}

				// Set skin-to-bone transforms
				InitializeDefaultPose(scaleList, rotationList, translationList);

				delete[] idList;
				delete[] flagList;
				delete[] scaleList;
				delete[] rotationList;
				delete[] translationList;
			}

			break;

		case 3:
			{
				// Read options
				stream->Read(&m_optionFlags0, sizeof(UINT32));

				// Read number of subnodes, links and entities
				stream->Read(&numSubNodes, sizeof(INT32));
				stream->Read(&numLinks, sizeof(INT32));
				stream->Read(&entityCount, sizeof(INT32));

				// Setup data arrays
				UINT32 *idList = ruNEW UINT32 [numSubNodes];
				UINT32 *flagList = ruNEW UINT32 [numSubNodes];
				CRuVector3 *scaleList = ruNEW CRuVector3 [numSubNodes];
				CRuQuaternion *rotationList = ruNEW CRuQuaternion [numSubNodes];
				CRuVector3 *translationList = ruNEW CRuVector3 [numSubNodes];

				// Load hierarchy structure and initial pose
				for(INT32 i = 0; i < numSubNodes; ++i)
				{
					stream->Read(&idList[i], sizeof(UINT32));
					stream->Read(&flagList[i], sizeof(UINT32));

					stream->Read(&scaleList[i].x, sizeof(REAL));
					stream->Read(&scaleList[i].y, sizeof(REAL));
					stream->Read(&scaleList[i].z, sizeof(REAL));

					stream->Read(&rotationList[i].x, sizeof(REAL));
					stream->Read(&rotationList[i].y, sizeof(REAL));
					stream->Read(&rotationList[i].z, sizeof(REAL));
					stream->Read(&rotationList[i].w, sizeof(REAL));

					stream->Read(&translationList[i].x, sizeof(REAL));
					stream->Read(&translationList[i].y, sizeof(REAL));
					stream->Read(&translationList[i].z, sizeof(REAL));
				}

				// Construct frame hierarchy
				Initialize(numSubNodes, numLinks, idList, NULL, flagList);

				// Load link points
				for(INT32 i = 0; i < numLinks; ++i)
				{
					// Read link index and ID
					stream->Read(&m_links[i].m_linkIndex, sizeof(INT32));
					stream->Read(&m_links[i].m_linkID, sizeof(INT32));

					// Read link name length including NULL
					INT32 linkNameLength = 0;
					stream->Read(&linkNameLength, sizeof(INT32));

					// Read link name
					if(linkNameLength > 0)
					{
						char *linkName = ruNEW char [linkNameLength];
						stream->Read(linkName, sizeof(char) * linkNameLength);
						m_links[i].m_linkName = linkName;
						delete[] linkName;
					}
					else
					{
						m_links[i].m_linkName = "unnamed";
					}

					// Construct link frame using object factory
					IRuObject *frameObj = RuObjectFactory().CreateObject(stream);

					if(frameObj)
					{
						if(frameObj->GetType() == CRuFrame::Type())
						{
							m_links[i].m_linkFrame = static_cast<CRuFrame *>(frameObj);

							// Disable cloning and serialization
							m_links[i].m_linkFrame->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
							m_links[i].m_linkFrame->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);

							// Attach link frame
							this->AddChild(m_links[i].m_linkFrame);

							// Decompose orientation transform
							CRuVector3 scale, shear, rotation, translation;
							CRuQuaternion rotationQuat;

							m_links[i].m_linkFrame->GetLocalTransform().GetDecomposition(scale, shear, rotation, translation);
							rotationQuat.MakeEulerRotation(rotation);

							// Setup default pose for the link
							m_links[i].m_initialScale = scale;
							m_links[i].m_initialRotation = rotationQuat;
							m_links[i].m_initialTranslation = translation;
						}
						else
						{
							frameObj->Release();
						}
					}
					else
					{
						m_links[i].m_linkFrame = NULL;
					}
				}

				// For each entity held by the frame, read it in
				for(INT32 i = 0; i < entityCount; ++i)
				{
					IRuObject *object = RuObjectFactory().CreateObject(stream);

					if(object->GetType().IsSubClassOf(CRuEntity::Type()))
					{
						AddEntity((CRuEntity *) object);
					}

					object->Release();
				}

				// Set skin-to-bone transforms
				InitializeDefaultPose(scaleList, rotationList, translationList);

				delete[] idList;
				delete[] flagList;
				delete[] scaleList;
				delete[] rotationList;
				delete[] translationList;
			}

			break;

		case 4:
			{
				// Read options
				stream->Read(&m_optionFlags0, sizeof(UINT32));

				// Read number of subnodes, links and entities
				stream->Read(&numSubNodes, sizeof(INT32));
				stream->Read(&numLinks, sizeof(INT32));
				stream->Read(&entityCount, sizeof(INT32));

				// Setup data arrays
				UINT32 *idList = ruNEW UINT32 [numSubNodes];
				UINT32 *groupIDList = ruNEW UINT32 [numSubNodes + numLinks];
				UINT32 *flagList = ruNEW UINT32 [numSubNodes];
				CRuVector3 *scaleList = ruNEW CRuVector3 [numSubNodes];
				CRuQuaternion *rotationList = ruNEW CRuQuaternion [numSubNodes];
				CRuVector3 *translationList = ruNEW CRuVector3 [numSubNodes];

				// Load hierarchy structure and initial pose
				for(INT32 i = 0; i < numSubNodes; ++i)
				{
					stream->Read(&idList[i], sizeof(UINT32));
					stream->Read(&groupIDList[i], sizeof(UINT32));
					stream->Read(&flagList[i], sizeof(UINT32));

					stream->Read(&scaleList[i].x, sizeof(REAL));
					stream->Read(&scaleList[i].y, sizeof(REAL));
					stream->Read(&scaleList[i].z, sizeof(REAL));

					stream->Read(&rotationList[i].x, sizeof(REAL));
					stream->Read(&rotationList[i].y, sizeof(REAL));
					stream->Read(&rotationList[i].z, sizeof(REAL));
					stream->Read(&rotationList[i].w, sizeof(REAL));

					stream->Read(&translationList[i].x, sizeof(REAL));
					stream->Read(&translationList[i].y, sizeof(REAL));
					stream->Read(&translationList[i].z, sizeof(REAL));
				}

				// Construct frame hierarchy
				Initialize(numSubNodes, numLinks, idList, groupIDList, flagList);

				// Load link points
				for(INT32 i = 0; i < numLinks; ++i)
				{
					// Read link index and ID
					stream->Read(&m_links[i].m_linkIndex, sizeof(INT32));
					stream->Read(&m_links[i].m_linkID, sizeof(INT32));
					stream->Read(&m_links[i].m_linkGroupID, sizeof(INT32));

					// Store group ID into the temporary array
					groupIDList[numSubNodes + i] = m_links[i].m_linkGroupID;

					// Read link name length including NULL
					INT32 linkNameLength = 0;
					stream->Read(&linkNameLength, sizeof(INT32));

					// Read link name
					if(linkNameLength > 0)
					{
						char *linkName = ruNEW char [linkNameLength];
						stream->Read(linkName, sizeof(char) * linkNameLength);
						m_links[i].m_linkName = linkName;
						delete[] linkName;
					}
					else
					{
						m_links[i].m_linkName = "unnamed";
					}

					// Construct link frame using object factory
					IRuObject *frameObj = RuObjectFactory().CreateObject(stream);

					if(frameObj)
					{
						if(frameObj->GetType() == CRuFrame::Type())
						{
							m_links[i].m_linkFrame = static_cast<CRuFrame *>(frameObj);

							// Disable cloning and serialization
							m_links[i].m_linkFrame->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
							m_links[i].m_linkFrame->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);

							// Attach link frame
							this->AddChild(m_links[i].m_linkFrame);

							// Decompose orientation transform
							CRuVector3 scale, shear, rotation, translation;
							CRuQuaternion rotationQuat;

							m_links[i].m_linkFrame->GetLocalTransform().GetDecomposition(scale, shear, rotation, translation);
							rotationQuat.MakeEulerRotation(rotation);

							// Setup default pose for the link
							m_links[i].m_initialScale = scale;
							m_links[i].m_initialRotation = rotationQuat;
							m_links[i].m_initialTranslation = translation;
						}
						else
						{
							frameObj->Release();
						}
					}
					else
					{
						m_links[i].m_linkFrame = NULL;
					}
				}

				// Update interpolator with correct group mask array
				m_interpolator->SetGroupIDs(groupIDList);

				// For each entity held by the frame, read it in
				for(INT32 i = 0; i < entityCount; ++i)
				{
					IRuObject *object = RuObjectFactory().CreateObject(stream);

					if(object->GetType().IsSubClassOf(CRuEntity::Type()))
					{
						AddEntity((CRuEntity *) object);
					}

					object->Release();
				}

				// Set skin-to-bone transforms
				InitializeDefaultPose(scaleList, rotationList, translationList);

				delete[] idList;
				delete[] groupIDList;
				delete[] flagList;
				delete[] scaleList;
				delete[] rotationList;
				delete[] translationList;
			}

			break;

		case 5:
			{
				// Chain serialization to base class
				CRuEntity_Container_Impl::SerializeFrom(stream);

				// Read number of subnodes and links
				stream->Read(&numSubNodes, sizeof(INT32));
				stream->Read(&numLinks, sizeof(INT32));

				// Setup data arrays
				UINT32 *idList = ruNEW UINT32 [numSubNodes];
				UINT32 *groupIDList = ruNEW UINT32 [numSubNodes + numLinks];
				UINT32 *flagList = ruNEW UINT32 [numSubNodes];
				CRuVector3 *scaleList = ruNEW CRuVector3 [numSubNodes];
				CRuQuaternion *rotationList = ruNEW CRuQuaternion [numSubNodes];
				CRuVector3 *translationList = ruNEW CRuVector3 [numSubNodes];

				// Load hierarchy structure and initial pose
				for(INT32 i = 0; i < numSubNodes; ++i)
				{
					stream->Read(&idList[i], sizeof(UINT32));
					stream->Read(&groupIDList[i], sizeof(UINT32));
					stream->Read(&flagList[i], sizeof(UINT32));

					stream->Read(&scaleList[i].x, sizeof(REAL));
					stream->Read(&scaleList[i].y, sizeof(REAL));
					stream->Read(&scaleList[i].z, sizeof(REAL));

					stream->Read(&rotationList[i].x, sizeof(REAL));
					stream->Read(&rotationList[i].y, sizeof(REAL));
					stream->Read(&rotationList[i].z, sizeof(REAL));
					stream->Read(&rotationList[i].w, sizeof(REAL));

					stream->Read(&translationList[i].x, sizeof(REAL));
					stream->Read(&translationList[i].y, sizeof(REAL));
					stream->Read(&translationList[i].z, sizeof(REAL));
				}

				// Construct frame hierarchy
				Initialize(numSubNodes, numLinks, idList, groupIDList, flagList);

				// Load link points
				for(INT32 i = 0; i < numLinks; ++i)
				{
					// Read link index and ID
					stream->Read(&m_links[i].m_linkIndex, sizeof(INT32));
					stream->Read(&m_links[i].m_linkID, sizeof(INT32));
					stream->Read(&m_links[i].m_linkGroupID, sizeof(INT32));

					// Store group ID into the temporary array
					groupIDList[numSubNodes + i] = m_links[i].m_linkGroupID;

					// Read link name length including NULL
					INT32 linkNameLength = 0;
					stream->Read(&linkNameLength, sizeof(INT32));

					// Read link name
					if(linkNameLength > 0)
					{
						char *linkName = ruNEW char [linkNameLength];
						stream->Read(linkName, sizeof(char) * linkNameLength);
						m_links[i].m_linkName = linkName;
						delete[] linkName;
					}
					else
					{
						m_links[i].m_linkName = "unnamed";
					}

					// Construct link frame using object factory
					IRuObject *frameObj = RuObjectFactory().CreateObject(stream);

					if(frameObj)
					{
						if(frameObj->GetType().IsSubClassOf(CRuEntity::Type()))
						{
							m_links[i].m_linkFrame = static_cast<CRuEntity *>(frameObj);

							// Disable cloning and serialization
							m_links[i].m_linkFrame->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
							m_links[i].m_linkFrame->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);

							// Attach link frame
							this->AddChild(m_links[i].m_linkFrame);

							// Decompose orientation transform
							CRuVector3 scale, shear, rotation, translation;
							CRuQuaternion rotationQuat;

							m_links[i].m_linkFrame->GetLocalTransform().GetDecomposition(scale, shear, rotation, translation);
							rotationQuat.MakeEulerRotation(rotation);

							// Setup default pose for the link
							m_links[i].m_initialScale = scale;
							m_links[i].m_initialRotation = rotationQuat;
							m_links[i].m_initialTranslation = translation;
						}
						else
						{
							frameObj->Release();
						}
					}
					else
					{
						m_links[i].m_linkFrame = NULL;
					}
				}

				// Update interpolator with correct group mask array
				m_interpolator->SetGroupIDs(groupIDList);

				// Set skin-to-bone transforms
				InitializeDefaultPose(scaleList, rotationList, translationList);

				delete[] idList;
				delete[] groupIDList;
				delete[] flagList;
				delete[] scaleList;
				delete[] rotationList;
				delete[] translationList;
			}

			break;
	}

	// Convert link frames if necessary
	for(INT32 i = 0; i < numLinks; ++i)
	{
		if(m_links[i].m_linkFrame->GetType() == CRuFrame::Type())
		{
			CRuFrame *oldFrame = static_cast<CRuFrame *>(m_links[i].m_linkFrame);
			CRuLinkFrame *newFrame = ruNEW CRuLinkFrame();

			// Disable cloning and serialization
			newFrame->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
			newFrame->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);

			// Set link name
			newFrame->SetLinkName(m_links[i].m_linkName);

			// Transfer all children from frame to link frame
			while(oldFrame->GetFirstChild())
			{
				CRuEntity *curChild = oldFrame->GetFirstChild();

				curChild->AddRef();
				curChild->DetachFromParent();
				newFrame->AddChild(curChild);
				ruSAFE_RELEASE(curChild);
			}

			// Destroy old frame
			m_links[i].m_linkFrame->DetachFromParent();
			ruSAFE_RELEASE(m_links[i].m_linkFrame);

			// Store new link frame
			m_links[i].m_linkFrame = newFrame;
			this->AddChild(m_links[i].m_linkFrame);
		}
	}

	return TRUE;
}

BOOL CRuFrameHierarchy::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write signature
	INT32 signature = 0xFFFFFFFF;
	stream->Write(&signature, sizeof(INT32));

	// Write version
	INT32 version = 5;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuEntity_Container_Impl::SerializeTo(stream);

	// Write number of subnodes
	stream->Write(&m_numSubNodes, sizeof(INT32));

	// Write number of links
	stream->Write(&m_numLinks, sizeof(INT32));

	// Write hierarchy structure
	for(INT32 i = 0; i < m_numSubNodes; ++i)
	{
		stream->Write(&m_subNodes[i].m_id, sizeof(INT32));
		stream->Write(&m_subNodes[i].m_groupID, sizeof(INT32));
		stream->Write(&m_subNodes[i].m_flags, sizeof(UINT32));

		stream->Write(&m_subNodes[i].m_initialScale.x, sizeof(REAL));
		stream->Write(&m_subNodes[i].m_initialScale.y, sizeof(REAL));
		stream->Write(&m_subNodes[i].m_initialScale.z, sizeof(REAL));

		stream->Write(&m_subNodes[i].m_initialRotation.x, sizeof(REAL));
		stream->Write(&m_subNodes[i].m_initialRotation.y, sizeof(REAL));
		stream->Write(&m_subNodes[i].m_initialRotation.z, sizeof(REAL));
		stream->Write(&m_subNodes[i].m_initialRotation.w, sizeof(REAL));

		stream->Write(&m_subNodes[i].m_initialTranslation.x, sizeof(REAL));
		stream->Write(&m_subNodes[i].m_initialTranslation.y, sizeof(REAL));
		stream->Write(&m_subNodes[i].m_initialTranslation.z, sizeof(REAL));
	}

	// Write link points
	for(INT32 i = 0; i < m_numLinks; ++i)
	{
		// Write link index and ID
		stream->Write(&m_links[i].m_linkIndex, sizeof(INT32));
		stream->Write(&m_links[i].m_linkID, sizeof(INT32));
		stream->Write(&m_links[i].m_linkGroupID, sizeof(UINT32));

		// Write link name length including NULL
		INT32 linkNameLength = m_links[i].m_linkName.GetLength() + 1;
		stream->Write(&linkNameLength, sizeof(INT32));

		// Write link name
		if(linkNameLength > 1)
		{
			stream->Write(static_cast<char *>(m_links[i].m_linkName), sizeof(char) * linkNameLength);
		}
		else
		{
			stream->Write("", sizeof(char) * 1);
		}

		// Serialize the link frame
		m_links[i].m_linkFrame->SerializeTo(stream);
	}

	return TRUE;
}

BOOL CRuFrameHierarchy::ResetStates()
{
	// Clear any animations playing in the interpolator
	m_interpolator->ClearAnimations(0.0f);

	// Chain call to parent
	return CRuEntity_Container_Impl::ResetStates();
}

BOOL CRuFrameHierarchy::Update(REAL elapsedTime)
{
	// Update animation time
	m_timeSinceLastAnimationUpdate += elapsedTime;
	m_animationAccumulatedT += elapsedTime;

	// Update animation if necessary
	if(m_timeSinceLastAnimationUpdate >= m_animationUpdateInterval)
	{
		m_interpolator->UpdateAnimationTime(m_animationAccumulatedT);

		m_timeSinceLastAnimationUpdate = 0.0f;
		m_animationAccumulatedT = 0.0f;
	}

	// Chain update call to parent class
	CRuEntity_Container_Impl::Update(elapsedTime);

	// Clear scratch pad
	m_scratch_ParentStack.Clear();

	CRuAnimKeyFrame *keyFrames = m_interpolator->GetKeyFrames();

	for(INT32 i = 0, currentParent = 0; i < m_numSubNodes; ++i)
	{
		// Construct local bone transform
		m_subNodeTransforms[i].SetToBoneSRT(keyFrames[i].m_scale, keyFrames[i].m_rotation, keyFrames[i].m_translation);

		// Multiply the local transform with the parent's if necessary
		if(currentParent != i)
		{
			m_subNodeTransforms[i] = m_subNodeTransforms[i] * m_subNodeTransforms[currentParent];
		}

		// Handle traversal of the hierarchy
		if(m_subNodes[i].m_flags & ruFRAMEHIERARCHY_PUSH)
		{
			m_scratch_ParentStack.Push(currentParent);
		}

		if(m_subNodes[i].m_flags & ruFRAMEHIERARCHY_POP)
		{
			// If the stack is empty when we hit a pop flag, that means we are starting a new tree
			if(m_scratch_ParentStack.IsEmpty())
			{
				// Set current parent to the next node
				currentParent = i + 1;
			}
			else
			{
				currentParent = m_scratch_ParentStack.Pop();
			}
		}
		else
		{
			currentParent = i;
		}
	}

	// Call update on all of the link nodes
	for(INT32 i = 0; i < m_numLinks; ++i)
	{
		// Transform link frame by its local transform
		m_links[i].m_linkFrame->ResetLocalTransform();
		m_links[i].m_linkFrame->Scale(keyFrames[i + m_numSubNodes].m_scale);
		m_links[i].m_linkFrame->Rotate(keyFrames[i + m_numSubNodes].m_rotation);
		m_links[i].m_linkFrame->Translate(keyFrames[i + m_numSubNodes].m_translation);

		// Transform link frame by its parent subnode transform
		m_links[i].m_linkFrame->Transform(m_subNodeTransforms[m_links[i].m_linkIndex]);
	}

	// Multiply each bone transform by the skin-to-bone transform
	for(INT32 i = 0; i < m_numSubNodes; ++i)
	{
		m_subNodeTransforms[i] = m_skinToBoneTransforms[i] * m_subNodeTransforms[i];
	}

	return TRUE;
}

CRuAnimationInterpolator *CRuFrameHierarchy::GetInterpolator()
{
	return m_interpolator;
}

void CRuFrameHierarchy::ClearAnimations(REAL transitionDuration)
{
	m_interpolator->ClearAnimations(transitionDuration);
}

BOOL CRuFrameHierarchy::PlayAnimation(CRuAnimation *animation, REAL startTime, REAL blendInTime, REAL blendOutTime, REAL loopBlendDuration)
{
	// Play the animation on the internal interpolator
	return m_interpolator->PlayAnimation(animation, startTime, blendInTime, blendOutTime, loopBlendDuration);
}

BOOL CRuFrameHierarchy::SetAnimationTime(REAL animTime)
{
	return m_interpolator->SetAnimationTime(animTime);
}

CRuEntity *CRuFrameHierarchy::GetLinkFrameByID(INT32 linkID)
{
	for(INT32 i = 0; i < m_numLinks; ++i)
	{
		if(m_links[i].m_linkID == linkID)
		{
			return m_links[i].m_linkFrame;
		}
	}

	return NULL;
}

CRuEntity *CRuFrameHierarchy::GetLinkFrameByIndex(INT32 linkIndex)
{
	if(linkIndex >= 0 && linkIndex < m_numLinks)
	{
		return m_links[linkIndex].m_linkFrame;
	}

	return NULL;
}

CRuEntity *CRuFrameHierarchy::GetLinkFrameByName(const char *linkName)
{
	for(INT32 i = 0; i < m_numLinks; ++i)
	{
		if(m_links[i].m_linkName == linkName)
		{
			return m_links[i].m_linkFrame;
		}
	}

	return NULL;
}

INT32 CRuFrameHierarchy::GetNumLinks() const
{
	return m_numLinks;
}

INT32 CRuFrameHierarchy::GetLinkIDByIndex(INT32 linkIndex) const
{
	if(linkIndex >= 0 && linkIndex < m_numLinks)
	{
		return m_links[linkIndex].m_linkID;
	}

	return -1;
}

const char *CRuFrameHierarchy::GetLinkNameByIndex(INT32 linkIndex) const
{
	if(linkIndex >= 0 && linkIndex < m_numLinks)
	{
		return static_cast<char *>(m_links[linkIndex].m_linkName);
	}

	return "";
}

INT32 CRuFrameHierarchy::GetNumSubNodes()
{
	return m_numSubNodes;
}

INT32 CRuFrameHierarchy::GetSubNodeIndex(INT32 subNodeID)
{
	for(INT32 i = 0; i < m_numSubNodes; ++i)
	{
		if(m_subNodes[i].m_id == subNodeID)
		{
			return i;
		}
	}

	return -1;
}

INT32 CRuFrameHierarchy::GetSubNodeParentIndex(INT32 subNodeIndex)
{
	if(subNodeIndex >= 0 && subNodeIndex < m_numSubNodes)
	{
		return m_subNodes[subNodeIndex].GetParentIndex();
	}

	return -1;
}

INT32 CRuFrameHierarchy::GetSubTreeSize(INT32 subNodeIndex)
{
	INT32 treeDepth = 0;
	INT32 currentSubNode = subNodeIndex;
	INT32 subTreeSize = 0;

	while(treeDepth >= 0)
	{	
		if(m_subNodes[currentSubNode].m_flags & ruFRAMEHIERARCHY_PUSH)
		{
			++treeDepth;
		}

		if(m_subNodes[currentSubNode].m_flags & ruFRAMEHIERARCHY_POP)
		{
			--treeDepth;
		}

		++currentSubNode;
		++subTreeSize;
	}

	return subTreeSize;
}

UINT32 CRuFrameHierarchy::GetSubNodeFlags(INT32 subNodeIndex)
{
	return m_subNodes[subNodeIndex].m_flags;
}

const CRuMatrix4x4 &CRuFrameHierarchy::GetSubNodeTransform(INT32 subNodeIndex)
{
	return m_subNodeTransforms[subNodeIndex];
}

const CRuMatrix4x4 &CRuFrameHierarchy::GetSubNodeSkinToBoneTransform(INT32 subNodeIndex)
{
	return m_skinToBoneTransforms[subNodeIndex];
}

BOOL CRuFrameHierarchy::SupportsMotionChannel(UINT32 channel)
{
	for(INT32 i = 0; i < m_numSubNodes; ++i)
	{
		if(m_subNodes[i].m_groupID == 0xFFFFFFFF)
		{
			return FALSE;
		}

		if(m_subNodes[i].m_groupID & channel)
		{
			return TRUE;
		}
	}

	for(INT32 i = 0; i < m_numLinks; ++i)
	{
		if(m_links[i].m_linkGroupID == 0xFFFFFFFF)
		{
			return FALSE;
		}

		if(m_links[i].m_linkGroupID & channel)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CRuFrameHierarchy::Reset()
{
	// TODO: This is broken!

	// Release references to links held by the frame
	for(INT32 i = 0; i < m_numLinks; ++i)
	{
		ruSAFE_DETACH(m_links[i].m_linkFrame);
		ruSAFE_RELEASE(m_links[i].m_linkFrame);
//		ruENTITY_SAFE_RELEASE_CHILD(m_links[i].m_linkFrame);
	}

	delete[] m_subNodes;
	delete[] m_links;

	delete[] m_skinToBoneTransforms;
	delete[] m_subNodeTransforms;

	ruSAFE_RELEASE(m_interpolator);
}

// ************************************************************************************************************************************************************

#pragma managed
