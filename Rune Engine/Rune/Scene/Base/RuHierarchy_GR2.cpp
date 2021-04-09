#include "../../Scene/Base/RuHierarchy_GR2.h"

#include "../../Helper/RuHelper_Entity.h"
#include "../../Helper/RuHelper_GR2.h"
#include "../../Helper/RuHelper_IRuEntity_Renderable.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuHierarchy_GR2, CRuEntity_Container_Impl)
ruHEAP_DEFINE_SUBCLASS(CRuHierarchy_GR2, "CRuHierarchy_GR2", "CRuEntity_Container_Impl")
ruCLASSGUID_DEFINE(CRuHierarchy_GR2, 0x53578D10, 0xE5A04bf7, 0xA300C807, 0x8113EE5D)

// ************************************************************************************************************************************************************

CRuHierarchy_GR2::CRuHierarchy_GR2()
{
	m_requiresInitialUpdate = TRUE;
	m_ignoreVisibilityCheck = FALSE;

	m_useAlternateSkinnedMeshColPath = FALSE;

	m_gr2SkeletonIsDeprecated = FALSE;
	m_gr2SkeletonInstanceDirty = FALSE;

	m_gr2ModelTemplate = ruNEW granny_model;
	m_gr2ModelTemplate->Name = "";
	m_gr2ModelTemplate->Skeleton = NULL;
	m_gr2ModelTemplate->MeshBindingCount = 0;
	m_gr2ModelTemplate->MeshBindings = NULL;
	GrannyMakeIdentity(&m_gr2ModelTemplate->InitialPlacement);

	m_grannyModel = ruNEW granny_model;
	m_grannyModel->Name = "";
	m_grannyModel->Skeleton = NULL;
	m_grannyModel->MeshBindingCount = 0;
	m_grannyModel->MeshBindings = NULL;
	GrannyMakeIdentity(&m_grannyModel->InitialPlacement);

	m_grannyModelInstance = NULL;

	m_grRestAnimationStream = NULL;
	m_grRestAnimationFile = NULL;
	m_grRestAnimation = NULL;

	m_gr2SparseBoneArrayRequiresUpdate = TRUE;
	m_gr2SparseBoneArraySize = 0;
	m_gr2SparseBoneArray = NULL;
	m_gr2SparseBoneArrayReverse = NULL;

	m_grannyLocalPose = NULL;
	m_grannyWorldPose = NULL;

	m_numSubNodes = 0;
	m_subNodes = NULL;
	m_subNodeTransforms = NULL;
	m_subNodeTransforms_World = NULL;
	m_restPoseTransforms_World = NULL;

	m_rootBlendNode = NULL;

	m_enableMotionCompensators = TRUE;

	if(g_ruProfiler)
	{
		m_probe_HierarchyUpdate = g_ruProfiler->CreateProbe("Hierarchy Update");
	}

//	this->SetBoneModifier("Bip01 Spine1", CRuVector3(1.0f, 1.5f, 1.5f));
//	this->SetBoneModifier("Bip01 Neck", CRuVector3(1.0f, 0.67f, 0.67f));

//	this->SetBoneModifier("Bip01 Spine1", CRuVector3(2.0f, 2.0f, 2.0f));
//	this->SetBoneModifier("Bip01 Neck", CRuVector3(0.5f, 0.5f, 0.5f));
}

CRuHierarchy_GR2::~CRuHierarchy_GR2()
{
	if(m_gr2ModelTemplate->Skeleton)
	{
		GR2_FreeSkeleton(m_gr2ModelTemplate->Skeleton);
		m_gr2ModelTemplate->Skeleton = NULL;
	}

	FreeGR2SkeletonInstance();
	FreeGR2ModelInstance();
	FreeSubNodeStructure();

	delete m_gr2ModelTemplate;
	delete m_grannyModel;

	delete m_grRestAnimationStream;

	if(m_grRestAnimationFile)
	{
		GrannyFreeFile(m_grRestAnimationFile);
		m_grRestAnimationFile = NULL;
	}

	delete[] m_gr2SparseBoneArray;
	delete[] m_gr2SparseBoneArrayReverse;

	for(INT32 i = 0; i < m_linkFrames.Count(); ++i)
	{
		ruSAFE_RELEASE(m_linkFrames[i].m_linkFrame);
	}

	// Enter critical section
	RuExtLink_Granny_GlobalCS()->Enter();
	{
		for(INT32 i = 0; i < m_motionChannels.Count(); ++i)
		{
			GrannyFreeTrackMask(m_motionChannels[i].m_modelMask);

			ruSAFE_RELEASE(m_motionChannels[i].m_animation);

			if(m_motionChannels[i].m_control)
			{
				GrannyFreeControl(m_motionChannels[i].m_control);
			}

			GrannyFreeModelInstance(m_motionChannels[i].m_controlModelInstance);

			GrannyFreeLocalPose(m_motionChannels[i].m_fadePose);

			GrannyFreeBlendDagEntireTree(m_motionChannels[i].m_dagCrossFadeNode);
			GrannyFreeLocalPose(m_motionChannels[i].m_blendedPose);
		}

		for(INT32 i = 0; i < m_subNodeDeltaKeys.Count(); ++i)
		{
			delete m_subNodeDeltaKeys[i].m_subNodeStopIndices;
		}

		// Free root blend node's entire tree
		GrannyFreeBlendDagEntireTree(m_rootBlendNode);
	}
	// Leave critical section
	RuExtLink_Granny_GlobalCS()->Leave();

	// Clear off bone modifiers
	for(INT32 i = 0; i < m_boneModifiers.Count(); ++i)
	{
		ruSAFE_DELETE_ARRAY(m_boneModifiers[i].m_boneName);

		if(m_boneModifiers[i].m_terminatorBoneNames)
		{
			for(INT32 j = 0; m_boneModifiers[i].m_terminatorBoneNames[j]; ++j)
			{
				ruSAFE_DELETE_ARRAY(m_boneModifiers[i].m_terminatorBoneNames[j]);
			}
		}

		ruSAFE_DELETE_ARRAY(m_boneModifiers[i].m_terminatorBoneNames);
	}
}

CRuEvent &CRuHierarchy_GR2::Event_BonesUpdated()
{
	return m_event_BonesUpdated;
}

BOOL CRuHierarchy_GR2::SetSkeleton(granny_skeleton *skeleton)
{
	if(skeleton)
	{
		// Free existing skeleton template
		if(m_gr2ModelTemplate->Skeleton)
		{
			GR2_FreeSkeleton(m_gr2ModelTemplate->Skeleton);
			m_gr2ModelTemplate->Skeleton = NULL;
		}

		// Clone the source skeleton
		m_gr2ModelTemplate->Skeleton = GR2_CloneSkeleton(skeleton);

		// Instantiate the base skeleton
		InstantiateGR2Skeleton();

		// Instantiate supporting structures for the skeleton
		InstantiateGR2Model();

		// Instantiate supporting subnode structures
		InstantiateSubNodeStructure();

		// Parse extended data for bone information
		UINT32 supportedMotionChannels = 0x00000000;
		for(INT32 i = 0; i < m_numSubNodes; ++i)
		{
			// Get bone descriptor
			GrBoneDescriptor boneDesc;
			GrannyConvertSingleObject(skeleton->Bones[i].ExtendedData.Type, skeleton->Bones[i].ExtendedData.Object, GrBoneDescriptorType, &boneDesc);

			CRuString prop_BoneID = GR2_GetProperty(boneDesc.m_userProps, "bone:boneID");
			CRuString prop_GroupID = GR2_GetProperty(boneDesc.m_userProps, "bone:groupID");
			CRuString prop_InvertRotation = GR2_GetProperty(boneDesc.m_userProps, "bone:invertRotation");
			CRuString prop_IsPoint = GR2_GetProperty(boneDesc.m_userProps, "misc:isPoint");

			if(prop_BoneID.GetLength() > 0)
			{
				// Parse bone ID
				m_subNodes[i].m_id = atoi(prop_BoneID);
			}
			else
			{
				m_subNodes[i].m_id = 0;
			}

			if(prop_GroupID.GetLength() > 0)
			{
				// Parse group ID
				m_subNodes[i].m_groupID = atoi(prop_GroupID);

				// Append group ID to supported channels mask
				supportedMotionChannels = supportedMotionChannels | m_subNodes[i].m_groupID;
			}
			else
			{
				m_subNodes[i].m_groupID = 0x00000000;
			}

			// If the bone is a Point object, create a link frame on it
			if(stricmp(prop_IsPoint, "true") == 0)
			{
				CreateLinkFrameOnBone(skeleton->Bones[i].Name, TRUE);
			}
		}

		// Construct motion channels
		ConstructMotionChannels();

		return TRUE;
	}

	return FALSE;
}

BOOL CRuHierarchy_GR2::SetRestAnimationStream(IRuStream *stream)
{
	// Release resources used by previous rest animation
	delete m_grRestAnimationStream;
	m_grRestAnimationStream = NULL;

	if(m_grRestAnimationFile)
	{
		GrannyFreeFile(m_grRestAnimationFile);
		m_grRestAnimationFile = NULL;
	}

	m_grRestAnimation = NULL;

	// Make a copy of the stream
	m_grRestAnimationStream = ruNEW CRuMemoryStream();
	*m_grRestAnimationStream = *stream;

	// Read granny file
	m_grRestAnimationFile = GrannyReadEntireFileFromMemory(m_grRestAnimationStream->GetStreamSize(), m_grRestAnimationStream->GetBuffer());

	// Critical section block
	RuExtLink_Granny_GlobalCS()->Enter();
	{
		// Get track group data
		granny_variant gVariant;
		GrannyGetDataTreeFromFile(m_grRestAnimationFile, &gVariant);
		m_grRestAnimation = (granny_animation *) gVariant.Object;
	}
	RuExtLink_Granny_GlobalCS()->Leave();

	return TRUE;
}

CRuLinkFrame *CRuHierarchy_GR2::CreateLinkFrameOnBone(const char *boneName, BOOL useWorldSpaceTransform)
{
	// Search existing link frames for the bone
	for(INT32 i = 0; i < m_linkFrames.Count(); ++i)
	{
		if(stricmp(m_linkFrames[i].m_linkFrame->GetLinkName(), boneName) == 0)
		{
			return m_linkFrames[i].m_linkFrame;
		}
	}

	// Search bone array for the bone
	for(INT32 i = 0; i < m_grannyModel->Skeleton->BoneCount; ++i)
	{
		if(stricmp(m_grannyModel->Skeleton->Bones[i].Name, boneName) == 0)
		{
			// Create new link frame
			LinkFrameDesc lfDesc;
			lfDesc.m_linkIndex = i;
			lfDesc.m_useWorldSpaceTransform = useWorldSpaceTransform;
			lfDesc.m_linkFrame = ruNEW CRuLinkFrame();
			lfDesc.m_linkFrame->SetLinkName(boneName);
			lfDesc.m_linkFrame->SetOption(ruENTITYOPTION_NOCLONE, TRUE);
			lfDesc.m_linkFrame->SetOption(ruENTITYOPTION_NOSERIALIZE, TRUE);

			// Insert link frame into tree
			this->AddChild(lfDesc.m_linkFrame);

			// Add link frame descriptor to list
			m_linkFrames.Add(lfDesc);

			return lfDesc.m_linkFrame;
		}
	}

	return NULL;
}

BOOL CRuHierarchy_GR2::ResetBoneModifiers()
{
	// Mark skeleton instance as dirty
	m_gr2SkeletonInstanceDirty = TRUE;

	for(INT32 i = 0; i < m_boneModifiers.Count(); ++i)
	{
		m_boneModifiers[i].m_scale = CRuVector3(1.0f, 1.0f, 1.0f);
	}

	return TRUE;
}

BOOL CRuHierarchy_GR2::SetBoneModifier(const char *boneName, const CRuVector3 &scale, char **terminatorBoneNames)
{
	// Mark skeleton instance as dirty
	m_gr2SkeletonInstanceDirty = TRUE;

	// Calculate inverse of the scale - currently this is a major hack, since
	// Granny's track group transform basis does not work properly. So instead,
	// we modify the world inverse transform for the skeleton directly, but
	// this requires that the scales be inverted to achieve the desired effect.
	CRuVector3 inverseScale(1.0f / scale.m_x, 1.0f / scale.m_y, 1.0f / scale.m_z);

	// Search existing modifiers for one with the same target bone
	for(INT32 i = 0; i < m_boneModifiers.Count(); ++i)
	{
		if(stricmp(m_boneModifiers[i].m_boneName, boneName) == 0)
		{
			m_boneModifiers[i].m_scale = m_boneModifiers[i].m_scale * inverseScale;
			return TRUE;
		}
	}

	// No existing modifiers match, create a new modifier
	BoneModifierDesc boneModDesc;
	boneModDesc.m_boneName = ruNEW char [strlen(boneName) + 1];
	memcpy(boneModDesc.m_boneName, boneName, strlen(boneName) + 1);
	boneModDesc.m_scale = inverseScale;

	if(terminatorBoneNames)
	{
		// Count the number of terminator bones
		INT32 numTerminatorBones = 0;
		for( ; terminatorBoneNames[numTerminatorBones]; ++numTerminatorBones);

		// Copy terminator bone names
		boneModDesc.m_terminatorBoneNames = ruNEW char * [numTerminatorBones + 1];
		memset(boneModDesc.m_terminatorBoneNames, 0, sizeof(char *) * (numTerminatorBones + 1));
		for(INT32 i = 0; terminatorBoneNames[i]; ++i)
		{
			INT32 nameLength = (INT32)strlen(terminatorBoneNames[i]) + 1;
			boneModDesc.m_terminatorBoneNames[i] = ruNEW char [nameLength];
			memcpy(boneModDesc.m_terminatorBoneNames[i], terminatorBoneNames[i], nameLength);
		}
	}
	else
	{
		boneModDesc.m_terminatorBoneNames = NULL;
	}

	m_boneModifiers.Add(boneModDesc);

	return TRUE;
}

BOOL CRuHierarchy_GR2::SetSubNodeBoundsFromMesh(IRuMesh *mesh, BOOL resetExistingBounds)
{
	if(mesh)
	{
		IRuMorphTarget *morphTarget = mesh->GetMorphTarget(0);

		if(morphTarget)
		{
			CRuVector3 *position = morphTarget->GetPosition();
			UINT16 *blendIndices = morphTarget->GetBlendIndex();
			REAL *blendWeights = morphTarget->GetBlendWeight();

			if(position && blendIndices && blendWeights)
			{
				CRuArrayList<CRuVector3> *subNodePositions = ruNEW CRuArrayList<CRuVector3> [m_numSubNodes];

				// Classify all weights and indices
				for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; ++i)
				{
					for(INT32 k = 0; k < 4; ++j, ++k)
					{
						if(blendWeights[j] > ruEPSILON)
						{
							INT32 subNodeIndex = blendIndices[j];
							subNodePositions[subNodeIndex].Add(position[i]);
						}
					}
				}

				// Build minimum sphere for each sub node
				CRuMinSphereBuilder minSphereBuilder;

				for(INT32 i = 0; i < m_numSubNodes; ++i)
				{
					CRuSphere minSphere;
					if(minSphereBuilder.BuildSphere(subNodePositions[i].GetArray(), subNodePositions[i].Count(), minSphere))
					{
						if(resetExistingBounds)
						{
							m_subNodes[i].m_localBounds = minSphere;
						}
						else
						{
							m_subNodes[i].m_localBounds.Merge(minSphere);
						}
					}

					CRuAABB minAABB;
					if(subNodePositions[i].Count() > 0)
					{
						minAABB.Minima() = subNodePositions[i][0];
						minAABB.Maxima() = subNodePositions[i][0];

						for(INT32 j = 1; j < subNodePositions[i].Count(); ++j)
						{
							minAABB.Merge(subNodePositions[i][j]);
						}
					}

					if(resetExistingBounds)
					{
						m_subNodes[i].m_localOBB.Set(minAABB);
					}
					else
					{
						// Build generic AABB
						CRuVector3 extents(m_subNodes[i].m_localOBB.Extents()[0], m_subNodes[i].m_localOBB.Extents()[1], m_subNodes[i].m_localOBB.Extents()[2]);
						CRuAABB box(m_subNodes[i].m_localOBB.Center() - extents, m_subNodes[i].m_localOBB.Center() + extents);

						// Only perform merge if the OBB is non-empty
						if(fabs(extents.m_x) > ruEPSILON && fabs(extents.m_y) > ruEPSILON && fabs(extents.m_z) > ruEPSILON)
						{
							// Merge min AABB and set as local OBB
							box.Merge(minAABB);
							m_subNodes[i].m_localOBB.Set(box);
						}
						else
						{
							m_subNodes[i].m_localOBB.Set(minAABB);
						}
					}
				}

				delete[] subNodePositions;

				// Build overall bounding sphere for the mesh
				minSphereBuilder.BuildSphere(position, mesh->GetNumVertices(), m_alternateSkinnedMeshColSphere);

				m_useAlternateSkinnedMeshColPath = TRUE;

				return TRUE;
			}
		}
	}

	return FALSE;
}

INT32 CRuHierarchy_GR2::SetSubNodeChannelMaskFromBone(const char *boneName, DWORD channel, BOOL exclusive)
{
	INT32 numAffectedSubNodes = 0;

	// Find the root bone
	INT32 rootBoneIndex = -1;
	for(INT32 i = 0; i < m_numSubNodes; ++i)
	{
		if(stricmp(m_grannyModel->Skeleton->Bones[i].Name, boneName) == 0)
		{
			rootBoneIndex = i;
			break;
		}
	}

	// If root bone was found
	if(rootBoneIndex >= 0)
	{
		for(INT32 i = 0; i < m_numSubNodes; ++i)
		{
			// Determine whether one of the bone's ancestors is the root bone
			INT32 curAncestorIdx = i;
			while(curAncestorIdx != -1)
			{
				// One of the bone's ancestors is the root bone, apply the channel mask
				if(curAncestorIdx == rootBoneIndex)
				{
					// Apply the channel mask
					if(exclusive)
					{
						m_subNodes[i].m_groupID = channel;
					}
					else
					{
						m_subNodes[i].m_groupID = m_subNodes[i].m_groupID | channel;
					}

					// Increment affected bone count
					++numAffectedSubNodes;

					break;
				}

				// Up to next ancestor
				curAncestorIdx = m_grannyModel->Skeleton->Bones[curAncestorIdx].ParentIndex;
			}
		}
	}

	if(numAffectedSubNodes > 0)
	{
		// Construct motion channels
		ConstructMotionChannels();
	}

	return numAffectedSubNodes;
}

granny_model_instance *CRuHierarchy_GR2::GetGR2ModelInstance()
{
	return m_grannyModelInstance;
}

void CRuHierarchy_GR2::SetIgnoreVisibilityCheck(BOOL ignoreVisibilityCheck)
{
	m_ignoreVisibilityCheck = ignoreVisibilityCheck;
}

INT32 CRuHierarchy_GR2::GetNumSubNodes()
{
	return m_numSubNodes;
}

INT32 CRuHierarchy_GR2::GetSubNodeIndex(INT32 subNodeID)
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

INT32 CRuHierarchy_GR2::GetSubNodeIndexByName(const char *subNodeName)
{
	if(m_grannyModel && m_grannyModel->Skeleton)
	{
		granny_int32x boneIndex;
		if(GrannyFindBoneByName(m_grannyModel->Skeleton, subNodeName, &boneIndex))
		{
			return boneIndex;
		}
	}

	return -1;
}

CRuSphere CRuHierarchy_GR2::GetSubNodeBounds(INT32 subNodeIndex, RuFrameType frameType)
{
	switch(frameType)
	{
		case ruFRAMETYPE_LOCAL:
			{
				const CRuSphere &localBounds = m_subNodes[subNodeIndex].m_localBounds;
				const CRuMatrix4x4 &subNodeTransform = m_subNodeTransforms[subNodeIndex];
				CRuSphere localSphere;

				subNodeTransform.TransformPoint(localBounds.Center(), localSphere.Center());
				localSphere.Radius() = localBounds.Radius();

				return localSphere;
			}

			break;

		case ruFRAMETYPE_WORLD:
			{
				const CRuSphere &localBounds = m_subNodes[subNodeIndex].m_localBounds;
				const CRuMatrix4x4 &subNodeTransform = m_subNodeTransforms[subNodeIndex];
				CRuSphere worldSphere;

				subNodeTransform.TransformPoint(localBounds.Center(), worldSphere.Center());

				const CRuMatrix4x4 &mat = this->GetWorldTransform();

				mat.TransformPoint(worldSphere.Center());
				worldSphere.Radius() = localBounds.Radius();

				return worldSphere;
			}

			break;
	}

	return CRuSphere();
}

CRuMatrix4x4 *CRuHierarchy_GR2::GetSubNodeTransforms(RuFrameType frameType)
{
	switch(frameType)
	{
		case ruFRAMETYPE_WORLD:
			return m_subNodeTransforms_World;

		case ruFRAMETYPE_COMPOSITE:
		default:
			return m_subNodeTransforms;
	}
}

CRuMatrix4x4 *CRuHierarchy_GR2::GetSubNodeRestPoseTransforms()
{
	return m_restPoseTransforms_World;
}

BOOL CRuHierarchy_GR2::ForceLinkFrameUpdate(CRuEntity *linkFrame)
{
	for(INT32 i = 0; i < m_linkFrames.Count(); ++i)
	{
		if(m_linkFrames[i].m_linkFrame == linkFrame)
		{
			// Sample composite matrix for the link bone
			granny_real32 *worldPoseMatrix = NULL;

			worldPoseMatrix = GrannyGetWorldPoseComposite4x4(m_grannyWorldPose, m_linkFrames[i].m_linkIndex);

			// Copy composite matrix
			CRuMatrix4x4 worldPoseMat;
			memcpy(worldPoseMat.m_elements, worldPoseMatrix, sizeof(REAL) * 16);

			// Set transform into link frame
			m_linkFrames[i].m_linkFrame->ResetLocalTransform();
			m_linkFrames[i].m_linkFrame->Transform(m_linkFrames[i].m_refTransform * worldPoseMat, ruTRANSFORMOP_SET);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuHierarchy_GR2::PlayAnimation(UINT32 motionChannel, CRuAnimation_GR2 *animation, REAL loopDelay, REAL timeMultiplier)
{
	for(INT32 i = 0; i < m_motionChannels.Count(); ++i)
	{
		if(m_motionChannels[i].m_channelMask == motionChannel)
		{
			// Set time multiplier
			m_motionChannels[i].m_timeMultiplier = timeMultiplier;

			// Critical section block
			RuExtLink_Granny_GlobalCS()->Enter();
			{
				// If there is an existing animation playing, sample its pose
				if(m_motionChannels[i].m_control)
				{
					// Copy existing motion to fade pose
					GrannyCopyLocalPose(m_motionChannels[i].m_blendedPose, m_motionChannels[i].m_fadePose);

					// Set crossfade weight to zero (sample directly from fade pose)
					m_motionChannels[i].m_fadeTransitionSpeed = 5.0f;	// this is a default transition speed
					m_motionChannels[i].m_fadeWeight = m_motionChannels[i].m_controlTTL >= 0 ? 0.0f : 1.0f;
//					m_motionChannels[i].m_fadeWeight = 1.0f;
					GrannySetBlendDagNodeCrossfadeWeights(m_motionChannels[i].m_dagCrossFadeNode, 0.0f, m_motionChannels[i].m_fadeWeight);

					// Release existing animation
					ruSAFE_RELEASE(m_motionChannels[i].m_animation);

					// Terminate/free control
					GrannyFreeControl(m_motionChannels[i].m_control);
					m_motionChannels[i].m_control = NULL;

					// Store reference to new animation
					m_motionChannels[i].m_animation = animation;
					m_motionChannels[i].m_animation->AddRef();

					// Store loop delay
					m_motionChannels[i].m_loopDelay = loopDelay;

					// Build controlled animation
					granny_controlled_animation_builder *builder = GrannyBeginControlledAnimation(0.0f, animation->GetGR2Animation());
					GrannySetTrackGroupTarget(builder, 0, m_motionChannels[i].m_controlModelInstance);
					m_motionChannels[i].m_control = GrannyEndControlledAnimation(builder);
					m_motionChannels[i].m_controlTTL = -1;

					// Set the control's loop count to 1 and its completion time to length of animation
					GrannySetControlLoopCount(m_motionChannels[i].m_control, 1);
					GrannyCompleteControlAt(m_motionChannels[i].m_control, m_motionChannels[i].m_animation->GetGR2Animation()->Duration);
				}
				else
				{
/*
					if(m_motionChannels[i].m_channelMask== 2)
					{
						OutputDebugString("NULL --> ");
						OutputDebugString(animation->GetObjectName());
						OutputDebugString("\n");
					}
*/
					// Set crossfade weight
					GrannySetBlendDagNodeCrossfadeWeights(m_motionChannels[i].m_dagCrossFadeNode, 0.0f, m_motionChannels[i].m_fadeWeight);

					// Release existing animation
					ruSAFE_RELEASE(m_motionChannels[i].m_animation);

					// Store reference to new animation
					m_motionChannels[i].m_animation = animation;
					m_motionChannels[i].m_animation->AddRef();

					// Store loop delay
					m_motionChannels[i].m_loopDelay = loopDelay;

					// Build controlled animation
					granny_controlled_animation_builder *builder = GrannyBeginControlledAnimation(0.0f, animation->GetGR2Animation());
					GrannySetTrackGroupTarget(builder, 0, m_motionChannels[i].m_controlModelInstance);
					m_motionChannels[i].m_control = GrannyEndControlledAnimation(builder);
					m_motionChannels[i].m_controlTTL = -1;

					// Set the control's loop count to 1 and its completion time to length of animation
					GrannySetControlLoopCount(m_motionChannels[i].m_control, 1);
					GrannyCompleteControlAt(m_motionChannels[i].m_control, m_motionChannels[i].m_animation->GetGR2Animation()->Duration);
				}
			}
			RuExtLink_Granny_GlobalCS()->Leave();

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuHierarchy_GR2::UpdateAnimationTimeMultiplier(UINT32 motionChannel, REAL timeMultiplier)
{
	for(INT32 i = 0; i < m_motionChannels.Count(); ++i)
	{
		if(m_motionChannels[i].m_channelMask == motionChannel)
		{
			// Set time multiplier
			m_motionChannels[i].m_timeMultiplier = timeMultiplier;
		}
	}

	return TRUE;
}

BOOL CRuHierarchy_GR2::ClearAnimation(UINT32 motionChannel, REAL transitionTime)
{
	for(INT32 i = 0; i < m_motionChannels.Count(); ++i)
	{
		if(m_motionChannels[i].m_channelMask == motionChannel)
		{
			if(m_motionChannels[i].m_control)
			{
				// Copy current pose to the fade pose
				GrannyCopyLocalPose(m_motionChannels[i].m_blendedPose, m_motionChannels[i].m_fadePose);

/*
				if(transitionTime < ruEPSILON)
				{
				char moo[200];
				sprintf(moo, "Motion cleared with transition time of zero!\n");
				OutputDebugString(moo);
				}
*/
				// Set crossfade weight to one (sample directly from fade pose)
				if(transitionTime > 0.0f)
				{
					m_motionChannels[i].m_fadeTransitionSpeed = 1.0f / transitionTime;
					m_motionChannels[i].m_fadeWeight = 1.0f;
				}
				else
				{
					m_motionChannels[i].m_fadeTransitionSpeed = 0.0f;
					m_motionChannels[i].m_fadeWeight = 0.0f;
				}

				// Critical section block
				RuExtLink_Granny_GlobalCS()->Enter();
				{
					GrannySetBlendDagNodeCrossfadeWeights(m_motionChannels[i].m_dagCrossFadeNode, 0.0f, m_motionChannels[i].m_fadeWeight);

					// Release existing animation
					ruSAFE_RELEASE(m_motionChannels[i].m_animation);

					// Terminate/free control
					GrannyFreeControl(m_motionChannels[i].m_control);
					m_motionChannels[i].m_control = NULL;
				}
				RuExtLink_Granny_GlobalCS()->Leave();

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CRuHierarchy_GR2::SynchronizeMotionChannel(UINT32 motionChannel)
{

	// Find the target motion channel index
	INT32 targetMotionChannelIndex = -1;
	for(INT32 i = 0; i < m_motionChannels.Count(); ++i)
	{
		if(m_motionChannels[i].m_channelMask == motionChannel)
		{
			targetMotionChannelIndex = i;
			break;
		}
	}

	if(targetMotionChannelIndex >= 0)
	{
		// Find first motion channel that is playing the same animation as the target motion channel and sync up the clocks
		for(INT32 i = 0; i < m_motionChannels.Count(); ++i)
		{
			if(targetMotionChannelIndex == i)
				continue;

			if(m_motionChannels[i].m_animation == m_motionChannels[targetMotionChannelIndex].m_animation)
			{
				if(m_motionChannels[i].m_control && m_motionChannels[targetMotionChannelIndex].m_control)
				{
					// Critical section block
					RuExtLink_Granny_GlobalCS()->Enter();
					{
						REAL controlClock = GrannyGetControlClock(m_motionChannels[i].m_control);
						GrannySetControlClock(m_motionChannels[targetMotionChannelIndex].m_control, controlClock);
					}
					RuExtLink_Granny_GlobalCS()->Leave();

					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL CRuHierarchy_GR2::SetSubNodeDeltaKey(PTRVALUE deltaKeyID, INT32 subNodeIndex, const CRuVector3 &scale, const CRuQuaternion &rotation, const CRuVector3 &position)
{
	INT32 deltaKeyIndex = -1;

	// Look for an existing descriptor for the subnode
	for(INT32 i = 0; i < m_subNodeDeltaKeys.Count(); ++i)
	{
		if(m_subNodeDeltaKeys[i].m_deltaKeyID == deltaKeyID && m_subNodeDeltaKeys[i].m_subNodeIndex == subNodeIndex)
		{
			deltaKeyIndex = i;
			break;
		}
	}

	// Create a new descriptor for the subnode if one does not already exist
	if(deltaKeyIndex == -1)
	{
		SubNodeDeltaKeyDesc subNodeDeltaKey;
		subNodeDeltaKey.m_deltaKeyID = deltaKeyID;
		subNodeDeltaKey.m_subNodeIndex = subNodeIndex;
		subNodeDeltaKey.m_scale.Set(1.0f, 1.0f, 1.0f);
		subNodeDeltaKey.m_rotation.Set(0.0f, 0.0f, 0.0f, 1.0f);
		subNodeDeltaKey.m_position.Set(0.0f, 0.0f, 0.0f);

		subNodeDeltaKey.m_subNodeStopIndices = ruNEW CRuArrayList<INT32>();

		deltaKeyIndex = m_subNodeDeltaKeys.Count();
		m_subNodeDeltaKeys.Add(subNodeDeltaKey);
	}

	// Set delta key for the subnode
	m_subNodeDeltaKeys[deltaKeyIndex].m_scale = scale;
	m_subNodeDeltaKeys[deltaKeyIndex].m_rotation = rotation;
	m_subNodeDeltaKeys[deltaKeyIndex].m_position = position;

	return TRUE;
}

BOOL CRuHierarchy_GR2::AddSubNodeDeltaKeyStopIndex(PTRVALUE deltaKeyID, INT32 subNodeIndex, INT32 stopIndex)
{
	INT32 deltaKeyIndex = -1;

	// Look for an existing descriptor for the subnode
	for(INT32 i = 0; i < m_subNodeDeltaKeys.Count(); ++i)
	{
		if(m_subNodeDeltaKeys[i].m_deltaKeyID == deltaKeyID && m_subNodeDeltaKeys[i].m_subNodeIndex == subNodeIndex)
		{
			deltaKeyIndex = i;
			break;
		}
	}

	if(deltaKeyIndex >= 0)
	{
		if(m_subNodeDeltaKeys[deltaKeyIndex].m_subNodeStopIndices->Contains(stopIndex) == FALSE)
		{
			m_subNodeDeltaKeys[deltaKeyIndex].m_subNodeStopIndices->Add(stopIndex);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CRuHierarchy_GR2::EnableMotionCompensators(BOOL enable)
{
	m_enableMotionCompensators = enable;
	return TRUE;
}

BOOL CRuHierarchy_GR2::AddMotionCompensator(UINT32 motionChannel, INT32 subNodeIndex, REAL compensatorWeight)
{
	// Create new motion compensator
	MotionCompensatorDesc compensatorDesc;
	compensatorDesc.m_motionChannel = motionChannel;
	compensatorDesc.m_subNodeIndex = subNodeIndex;
	compensatorDesc.m_compensatorWeight = compensatorWeight;
	m_motionCompensators.Add(compensatorDesc);

	// Sparse bone array requires update because motion compensator bones must be in the bone map
	m_gr2SparseBoneArrayRequiresUpdate = TRUE;

	return TRUE;
}

BOOL CRuHierarchy_GR2::SetChannelWeightMultiplier(UINT32 motionChannel, REAL weightMultiplier)
{
	for(INT32 i = 0; i < m_motionChannels.Count(); ++i)
	{
		if(m_motionChannels[i].m_channelMask == motionChannel)
		{
			m_motionChannels[i].m_weightMultiplier = weightMultiplier;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuHierarchy_GR2::SupportsMotionChannel(UINT32 channel)
{
	for(INT32 i = 0; i < m_motionChannels.Count(); ++i)
	{
		if(m_motionChannels[i].m_channelMask == channel)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuHierarchy_GR2::AddChild(CRuEntity *entity)
{
	// Sparse bone array requires update
	m_gr2SparseBoneArrayRequiresUpdate = TRUE;

	// Chain to base class
	return CRuEntity_Container_Impl::AddChild(entity);
}

BOOL CRuHierarchy_GR2::RemoveChild(CRuEntity *entity)
{
	// Sparse bone array requires update
	m_gr2SparseBoneArrayRequiresUpdate = TRUE;

	// Chain to base class
	return CRuEntity_Container_Impl::RemoveChild(entity);
}

BOOL CRuHierarchy_GR2::RemoveAllChildren()
{
	// Sparse bone array requires update
	m_gr2SparseBoneArrayRequiresUpdate = TRUE;

	// Chain to base class
	return CRuEntity_Container_Impl::RemoveAllChildren();;
}

IRuObject *CRuHierarchy_GR2::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuHierarchy_GR2();
	CRuHierarchy_GR2 *clonedHierarchy = static_cast<CRuHierarchy_GR2 *>(clonedObject);

	// Chain cloning call to parent class
	CRuEntity_Container_Impl::Clone(clonedObject);

	// Copy visibility ignore flag
	clonedHierarchy->m_ignoreVisibilityCheck = m_ignoreVisibilityCheck;

	// Save collision path flag
	clonedHierarchy->m_useAlternateSkinnedMeshColPath = m_useAlternateSkinnedMeshColPath;

	// Clone the skeleton template
	clonedHierarchy->m_gr2ModelTemplate->Skeleton = GR2_CloneSkeleton(m_gr2ModelTemplate->Skeleton);

	// Clone the skeleton instance
	clonedHierarchy->m_grannyModel->Skeleton = GR2_CloneSkeleton(m_grannyModel->Skeleton);

	// Instantiate supporting structures for the skeleton
	clonedHierarchy->InstantiateGR2Model();

	// Instantiate supporting subnode structures
	clonedHierarchy->InstantiateSubNodeStructure();

	// Copy subnode descriptors
	for(INT32 i = 0; i < m_numSubNodes; ++i)
	{
		clonedHierarchy->m_subNodes[i] = m_subNodes[i];
	}

	// Clone link frames
	for(INT32 i = 0; i < m_linkFrames.Count(); ++i)
	{
		LinkFrameDesc lfDesc;
		lfDesc.m_linkIndex = m_linkFrames[i].m_linkIndex;
		lfDesc.m_useWorldSpaceTransform = m_linkFrames[i].m_useWorldSpaceTransform;
		lfDesc.m_linkFrame = static_cast<CRuLinkFrame *>(m_linkFrames[i].m_linkFrame->Clone());

		// Insert link frame into tree
		clonedHierarchy->AddChild(lfDesc.m_linkFrame);

		// Add link frame descriptor to list
		clonedHierarchy->m_linkFrames.Add(lfDesc);
	}

	if(m_grRestAnimationStream)
	{
		// Clone rest-state animation stream
		clonedHierarchy->m_grRestAnimationStream = ruNEW CRuMemoryStream();
		clonedHierarchy->m_grRestAnimationStream->SetBufferSize(m_grRestAnimationStream->GetStreamSize());
		clonedHierarchy->m_grRestAnimationStream->SetStreamSize(m_grRestAnimationStream->GetStreamSize());
		memcpy(clonedHierarchy->m_grRestAnimationStream->GetBuffer(), m_grRestAnimationStream->GetBuffer(), m_grRestAnimationStream->GetStreamSize());

		// Build rest-state animation from stream
		// Read granny file
		clonedHierarchy->m_grRestAnimationFile = GrannyReadEntireFileFromMemory(clonedHierarchy->m_grRestAnimationStream->GetStreamSize(), clonedHierarchy->m_grRestAnimationStream->GetBuffer());

		// Critical section block
		RuExtLink_Granny_GlobalCS()->Enter();
		{
			// Get track group data
			if(clonedHierarchy->m_grRestAnimationFile)
			{
				granny_variant gVariant;
				GrannyGetDataTreeFromFile(clonedHierarchy->m_grRestAnimationFile, &gVariant);
				clonedHierarchy->m_grRestAnimation = (granny_animation *) gVariant.Object;
			}
		}
		RuExtLink_Granny_GlobalCS()->Leave();
	}

	// Construct motion channels
	clonedHierarchy->ConstructMotionChannels();

	return clonedHierarchy;
}

BOOL CRuHierarchy_GR2::SerializeFrom(IRuStream *stream)
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

	// Chain serialization to base class
	CRuEntity_Container_Impl::SerializeFrom(stream);

	// Read skeleton from stream
	m_gr2ModelTemplate->Skeleton = GR2_SerializeSkeletonFrom(stream, &m_gr2SkeletonIsDeprecated);

	// Instantiate the base skeleton
	InstantiateGR2Skeleton();

	// Instantiate supporting structures for the skeleton
	InstantiateGR2Model();

	// Instantiate supporting subnode structures
	InstantiateSubNodeStructure();

	// Read subnode descriptors
	for(INT32 i = 0; i < m_numSubNodes; ++i)
	{
		stream->Read(&m_subNodes[i].m_id, sizeof(UINT32));
		stream->Read(&m_subNodes[i].m_groupID, sizeof(UINT32));
	}

	// Read number of link frames
	INT32 numLinkFrames = 0;
	stream->Read(&numLinkFrames, sizeof(INT32));

	// Read link frames
	m_linkFrames.SetArraySize(numLinkFrames);
	m_linkFrames.SetNumEntries(numLinkFrames);
	for(INT32 i = 0; i < numLinkFrames; ++i)
	{
		// Read link index
		stream->Read(&m_linkFrames[i].m_linkIndex, sizeof(INT32));

		stream->Read(&m_linkFrames[i].m_useWorldSpaceTransform, sizeof(BOOL));

		// Read link frame
		m_linkFrames[i].m_linkFrame = ruNEW CRuLinkFrame();
		m_linkFrames[i].m_linkFrame->SerializeFrom(stream);

		// Set to do not auto-clone
		m_linkFrames[i].m_linkFrame->SetOption(ruENTITYOPTION_NOCLONE, TRUE);

		// Attach as child
		this->AddChild(m_linkFrames[i].m_linkFrame);
	}

	if(version >= 2)
	{
		// Read rest-state animation stream size
		INT32 restAnimationStreamSize = 0;
		stream->Read(&restAnimationStreamSize, sizeof(INT32));

		// Read rest-state animation stream
		if(restAnimationStreamSize > 0)
		{
			m_grRestAnimationStream = ruNEW CRuMemoryStream();
			m_grRestAnimationStream->SetBufferSize(restAnimationStreamSize);
			m_grRestAnimationStream->SetStreamSize(restAnimationStreamSize);
			stream->Read(m_grRestAnimationStream->GetBuffer(), restAnimationStreamSize);
		}

		// Build rest-state animation from stream
		if(m_grRestAnimationStream)
		{
			// Read granny file
			m_grRestAnimationFile = GrannyReadEntireFileFromMemory(m_grRestAnimationStream->GetStreamSize(), m_grRestAnimationStream->GetBuffer());

			if(m_grRestAnimationFile)
			{
				// Critical section block
				RuExtLink_Granny_GlobalCS()->Enter();
				{
					// Get track group data
					granny_variant gVariant;
					GrannyGetDataTreeFromFile(m_grRestAnimationFile, &gVariant);

					// Perform version check
					UINT32 fileTag = GrannyGetFileTypeTag(m_grRestAnimationFile);

					if(fileTag == GrannyCurrentGRNStandardTag)
					{
						// Same version, so we simply assign the pointer
						m_grRestAnimation = (granny_animation *) gVariant.Object;
					}
					else
					{
						// Old version, perform data conversion
						m_grRestAnimation = (granny_animation *) GrannyConvertTree(gVariant.Type, gVariant.Object, GrannyAnimationType);

						// Convert animation to stream
						CRuMemoryStream *memStream = GR2_ConvertVariantToStream(GrannyAnimationType, m_grRestAnimation);

						// Free old animation data
						GrannyFreeBuilderResult(m_grRestAnimation);
						m_grRestAnimation = NULL;

						delete m_grRestAnimationStream;
						m_grRestAnimationStream = memStream;

						// Replace with converted animation data
						m_grRestAnimationFile = GrannyReadEntireFileFromMemory(m_grRestAnimationStream->GetStreamSize(), m_grRestAnimationStream->GetBuffer());
						GrannyGetDataTreeFromFile(m_grRestAnimationFile, &gVariant);
						m_grRestAnimation = (granny_animation *) gVariant.Object;
					}
				}
				RuExtLink_Granny_GlobalCS()->Leave();
			}
		}
	}

	// Construct motion channels
	ConstructMotionChannels();

	return TRUE;
}

BOOL CRuHierarchy_GR2::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 2;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	CRuEntity_Container_Impl::SerializeTo(stream);

	// Write skeleton (put check here)
	GR2_SerializeSkeletonTo(stream, m_gr2ModelTemplate->Skeleton, &m_gr2SkeletonIsDeprecated);

	// Write subnode descriptors
	for(INT32 i = 0; i < m_numSubNodes; ++i)
	{
		stream->Write(&m_subNodes[i].m_id, sizeof(UINT32));
		stream->Write(&m_subNodes[i].m_groupID, sizeof(UINT32));
	}

	// Write number of link frames
	INT32 numLinkFrames = m_linkFrames.Count();
	stream->Write(&numLinkFrames, sizeof(INT32));

	// Write link frames
	for(INT32 i = 0; i < numLinkFrames; ++i)
	{
		// Write link index
		stream->Write(&m_linkFrames[i].m_linkIndex, sizeof(INT32));

		stream->Write(&m_linkFrames[i].m_useWorldSpaceTransform, sizeof(BOOL));

		// Write link frame
		m_linkFrames[i].m_linkFrame->SerializeTo(stream);
	}

	// Write rest-state animation stream
	if(m_grRestAnimationStream)
	{
		// Write rest-state animation stream size
		INT32 restAnimationStreamSize = m_grRestAnimationStream->GetStreamSize();
		stream->Write(&restAnimationStreamSize, sizeof(INT32));

		// Write rest-state animation stream
		stream->Write(m_grRestAnimationStream->GetBuffer(), restAnimationStreamSize);
	}
	else
	{
		// Write rest-state animation stream size
		INT32 restAnimationStreamSize = 0;
		stream->Write(&restAnimationStreamSize, sizeof(INT32));
	}

	return TRUE;
}

BOOL CRuHierarchy_GR2::ResetStates()
{
	return TRUE;
}

BOOL CRuHierarchy_GR2::Update(REAL elapsedTime)
{
	RUPROFILE_SAMPLE_BEGIN(m_probe_HierarchyUpdate, 3)

	REAL curLOD = 0.0f;

	// Update skeleton instance if necessary
	if(m_gr2SkeletonInstanceDirty)
	{
		InstantiateGR2Skeleton();
		InstantiateGR2Model();

		ConstructMotionChannels();
	}

	if(m_gr2SparseBoneArrayRequiresUpdate)
	{
		m_gr2SparseBoneArrayRequiresUpdate = FALSE;
		UpdateSparseBoneArray();
	}

	// Only update hierarchy if it is visible
	if(m_requiresInitialUpdate || m_ignoreVisibilityCheck || RuEntity_GetVisibleStatusFlag(this))
	{
		REAL m_fadeSpeed = 5.000f;	// yea.. move me

		INT32 numUniqueMotions = 0;
		INT32 lastUniqueMotion = -1;

		// Critical section block
		RuExtLink_Granny_GlobalCS()->Enter();
		{
			// Advance motion control clocks
			for(INT32 i = 0; i < m_motionChannels.Count(); ++i)
			{
				BOOL motionIsActive = FALSE;

				if(m_motionChannels[i].m_control)
				{
					// Set clock for the active control
					REAL controlClock = GrannyGetControlClock(m_motionChannels[i].m_control);
					GrannySetControlClock(m_motionChannels[i].m_control, controlClock + (elapsedTime * m_motionChannels[i].m_timeMultiplier));

					// Enable blending of the DAG node
					GrannySetBlendDagNodeResultWeight(m_motionChannels[i].m_dagBlendNode, m_motionChannels[i].m_weightMultiplier);

					// Flag that there is an active motion control
					motionIsActive = TRUE;
				}
				else
				{
					// Disable blending of the DAG node
					GrannySetBlendDagNodeResultWeight(m_motionChannels[i].m_dagBlendNode, 0.0f);
				}

				if(m_motionChannels[i].m_fadeWeight > 0.0f)
				{
					// Handle blending between the active control and the fade pose
		//			m_motionChannels[i].m_fadeWeight -= m_fadeSpeed * elapsedTime;
					m_motionChannels[i].m_fadeWeight -= m_motionChannels[i].m_fadeTransitionSpeed * elapsedTime;
					if(m_motionChannels[i].m_fadeWeight < 0.0f)
						m_motionChannels[i].m_fadeWeight = 0.0f;
					GrannySetBlendDagNodeCrossfadeWeights(m_motionChannels[i].m_dagCrossFadeNode, 0.0f, m_motionChannels[i].m_fadeWeight);
				}
				else
				{
					// If the active control is finished, terminate and release it
					if(m_motionChannels[i].m_control && GrannyControlIsComplete(m_motionChannels[i].m_control))
					{
						if(m_motionChannels[i].m_loopDelay >= 0.0f)
						{
							// Calculate new control clock
							REAL controlClock = GrannyGetControlClock(m_motionChannels[i].m_control);
							controlClock = fmod(controlClock, m_motionChannels[i].m_animation->GetGR2Animation()->Duration + m_motionChannels[i].m_loopDelay);

							// Set control clock
							GrannySetControlClock(m_motionChannels[i].m_control, controlClock);
						}
						else
						{
							// Allow the control to live for 100 update cycles before it is deleted
							if(m_motionChannels[i].m_controlTTL < 0)
							{
								m_motionChannels[i].m_controlTTL = 100;
							}
							else
							{
								--m_motionChannels[i].m_controlTTL;

								if(m_motionChannels[i].m_controlTTL == 0)
								{
									// Reset TTL
									m_motionChannels[i].m_controlTTL = -1;

									// Release existing animation
									ruSAFE_RELEASE(m_motionChannels[i].m_animation);

									// Terminate/free control
									GrannyFreeControl(m_motionChannels[i].m_control);
									m_motionChannels[i].m_control = NULL;

									// Mark motion as inactive
									motionIsActive = FALSE;
								}
							}
						}
					}
				}

				// Sample local pose if applicable
				if(motionIsActive)
				{
					// Search prior motions for one with the same animation playing
					BOOL copied = FALSE;
					for(INT32 j = i - 1; j >= 0; --j)
					{
						// If there is a prior motion that is playing the same animation, simply copy its local pose
						if(m_motionChannels[i].m_animation == m_motionChannels[j].m_animation && m_motionChannels[i].m_fadeWeight == m_motionChannels[j].m_fadeWeight)
	//					if(m_motionChannels[i].m_animation == m_motionChannels[j].m_animation && m_motionChannels[i].m_fadeWeight == 0.0f)
						{
							GrannyCopyLocalPose(m_motionChannels[j].m_blendedPose, m_motionChannels[i].m_blendedPose);
							copied = TRUE;
							break;
						}
					}

					// If there is no prior motion with matching animation, sample the motion blend DAG tree
					if(copied == FALSE)
					{
						granny_local_pose *channelLocalPose = GrannySampleBlendDagTreeLODSparse(m_motionChannels[i].m_dagCrossFadeNode, m_gr2SparseBoneArraySize, curLOD, m_gr2SparseBoneArray);
						GrannySparseBoneArrayExpand(m_grannyModel->Skeleton, channelLocalPose, m_gr2SparseBoneArraySize, m_gr2SparseBoneArray, m_gr2SparseBoneArrayReverse, m_grannyModel->Skeleton->BoneCount, m_motionChannels[i].m_blendedPose);

						// Keep track of the number of unique motions
						++numUniqueMotions;
						lastUniqueMotion = i;
					}
				}
			}

			// If there is an active motion, we no longer need the initial update
			if(m_requiresInitialUpdate && numUniqueMotions > 0)
			{
				m_requiresInitialUpdate = FALSE;
			}

			if(m_grannyModel && m_grannyModel->Skeleton && m_grannyModelInstance && m_grannyLocalPose && m_grannyWorldPose && numUniqueMotions > 0)
			{
				if(numUniqueMotions == 1)
				{
					// Sample local pose
					GrannyCopyLocalPoseSparse(m_motionChannels[lastUniqueMotion].m_blendedPose, m_grannyLocalPose, m_gr2SparseBoneArraySize, m_gr2SparseBoneArray);
				}
				else
				{
					// Sample local pose
					granny_local_pose *localPose = GrannySampleBlendDagTreeLODSparse(m_rootBlendNode, m_gr2SparseBoneArraySize, curLOD, m_gr2SparseBoneArray);
					GrannyCopyLocalPose(localPose, m_grannyLocalPose);
				}

				// Apply subnode delta keys
				for(INT32 i = 0; i < m_subNodeDeltaKeys.Count(); ++i)
				{
					SubNodeDeltaKeyDesc &deltaKey = m_subNodeDeltaKeys[i];
					granny_transform *subNodeTransform = GrannyGetLocalPoseTransform(m_grannyLocalPose, m_gr2SparseBoneArrayReverse[deltaKey.m_subNodeIndex]);

					if(subNodeTransform)
					{
						// Work up the bone tree and get the world transform
						INT32 boneIndex = deltaKey.m_subNodeIndex;

						granny_transform boneWorldTransform;
						GrannyMakeIdentity(&boneWorldTransform);

						while(boneIndex != -1)
						{
							granny_transform *stepTransform = GrannyGetLocalPoseTransform(m_grannyLocalPose, m_gr2SparseBoneArrayReverse[boneIndex]);
							GrannyPreMultiplyBy(&boneWorldTransform, stepTransform);
							boneIndex = m_grannyModel->Skeleton->Bones[boneIndex].ParentIndex;
						}

						// Build transform to go from scaling 
						granny_transform fromScaleBasisTransform;
						GrannyMakeIdentity(&fromScaleBasisTransform);

						fromScaleBasisTransform.Orientation[0] = boneWorldTransform.Orientation[0];
						fromScaleBasisTransform.Orientation[1] = boneWorldTransform.Orientation[1];
						fromScaleBasisTransform.Orientation[2] = boneWorldTransform.Orientation[2];
						fromScaleBasisTransform.Orientation[3] = boneWorldTransform.Orientation[3];

						// Scale transform
						granny_transform scaleTransform;
						GrannyMakeIdentity(&scaleTransform);
						scaleTransform.ScaleShear[0][0] = deltaKey.m_scale.m_x;
						scaleTransform.ScaleShear[1][1] = deltaKey.m_scale.m_y;
						scaleTransform.ScaleShear[2][2] = deltaKey.m_scale.m_z;

						scaleTransform.Orientation[0] = -deltaKey.m_rotation.m_x;
						scaleTransform.Orientation[1] = -deltaKey.m_rotation.m_y;
						scaleTransform.Orientation[2] = -deltaKey.m_rotation.m_z;
						scaleTransform.Orientation[3] = deltaKey.m_rotation.m_w;

						granny_transform toScaleBasisTransform;
						GrannyBuildInverse(&toScaleBasisTransform, &fromScaleBasisTransform);

						// Build delta key transform
						granny_transform deltaKeyTransform;
						GrannyMakeIdentity(&deltaKeyTransform);
						GrannyPostMultiplyBy(&deltaKeyTransform, &toScaleBasisTransform);
						GrannyPostMultiplyBy(&deltaKeyTransform, &scaleTransform);
						GrannyPostMultiplyBy(&deltaKeyTransform, &fromScaleBasisTransform);

						// Build inverse delta key transform
						granny_transform inverseDeltaKeyTransform;
						GrannyBuildInverse(&inverseDeltaKeyTransform, &deltaKeyTransform);

						// Figure out the stop points
						for(INT32 j = 0; j < deltaKey.m_subNodeStopIndices->Count(); ++j)
						{
							INT32 stopNodeIndex = deltaKey.m_subNodeStopIndices->Get(j);

							granny_transform *stopNodeInitialTransform = GrannyGetLocalPoseTransform(m_grannyLocalPose, m_gr2SparseBoneArrayReverse[stopNodeIndex]);

							// Work up the bone tree and get the partial transform from the delta bone to the stop bone
							INT32 parentBoneIndex = stopNodeIndex;

							granny_transform partialTransform;
							GrannyMakeIdentity(&partialTransform);

							while(parentBoneIndex != -1)
							{
								granny_transform *stepTransform = GrannyGetLocalPoseTransform(m_grannyLocalPose, m_gr2SparseBoneArrayReverse[parentBoneIndex]);
								GrannyPreMultiplyBy(&partialTransform, stepTransform);
								parentBoneIndex = m_grannyModel->Skeleton->Bones[parentBoneIndex].ParentIndex;
							}


							// Build transform to go from scaling 
							granny_transform fromScaleBasisTransform2;
							GrannyMakeIdentity(&fromScaleBasisTransform2);

							fromScaleBasisTransform2.Orientation[0] = partialTransform.Orientation[0];
							fromScaleBasisTransform2.Orientation[1] = partialTransform.Orientation[1];
							fromScaleBasisTransform2.Orientation[2] = partialTransform.Orientation[2];
							fromScaleBasisTransform2.Orientation[3] = partialTransform.Orientation[3];

							// Scale transform
							granny_transform scaleTransform2;
							GrannyMakeIdentity(&scaleTransform2);
							scaleTransform2.ScaleShear[0][0] = 1.0f / deltaKey.m_scale.m_x;
							scaleTransform2.ScaleShear[1][1] = 1.0f / deltaKey.m_scale.m_y;
							scaleTransform2.ScaleShear[2][2] = 1.0f / deltaKey.m_scale.m_z;

							scaleTransform2.Orientation[0] = deltaKey.m_rotation.m_x;
							scaleTransform2.Orientation[1] = deltaKey.m_rotation.m_y;
							scaleTransform2.Orientation[2] = deltaKey.m_rotation.m_z;
							scaleTransform2.Orientation[3] = deltaKey.m_rotation.m_w;

							granny_transform toScaleBasisTransform2;
							GrannyBuildInverse(&toScaleBasisTransform2, &fromScaleBasisTransform2);

							// Build the stop node transform
							granny_transform stopNodeTransform;
							GrannyMakeIdentity(&stopNodeTransform);

							GrannyPostMultiplyBy(&stopNodeTransform, &toScaleBasisTransform2);
							GrannyPostMultiplyBy(&stopNodeTransform, &scaleTransform2);
							GrannyPostMultiplyBy(&stopNodeTransform, &fromScaleBasisTransform2);

							// Set final stop node transform
							GrannyPostMultiplyBy(stopNodeInitialTransform, &stopNodeTransform);
						}

						// Set final sub node transform
						GrannyPostMultiplyBy(subNodeTransform, &deltaKeyTransform);
					}
				}

				// Run compensators
				if(m_enableMotionCompensators && numUniqueMotions > 1)
				{
					for(INT32 i = 0; i < m_motionCompensators.Count(); ++i)
					{
						MotionChannelDesc *channel = FindChannelDescriptor(m_motionCompensators[i].m_motionChannel);

						if(channel && m_motionCompensators[i].m_subNodeIndex >= 0)
						{
							// Sample target channel local pose
							granny_local_pose *channelLocalPose = channel->m_blendedPose;

							// Get the desired world transform from the target channel
							CRuMatrix4x4 desiredTransform;
							GrannyGetWorldMatrixFromLocalPose(m_grannyModel->Skeleton, m_motionCompensators[i].m_subNodeIndex, channelLocalPose, NULL, &desiredTransform.m_elements[0][0], NULL, NULL);

							// Get the actual world transform
							CRuMatrix4x4 actualTransform;
							granny_transform *subNodeTransform = GrannyGetLocalPoseTransform(m_grannyLocalPose, m_gr2SparseBoneArrayReverse[m_motionCompensators[i].m_subNodeIndex]);
							GrannyGetWorldMatrixFromLocalPose(m_grannyModel->Skeleton, m_motionCompensators[i].m_subNodeIndex, m_grannyLocalPose, NULL, &actualTransform.m_elements[0][0], m_gr2SparseBoneArray, m_gr2SparseBoneArrayReverse);

							// -- This is the CORRECT thing to do... completely match transform compensation

							// Expected transform is then ...?
							CRuMatrix4x4 invActualTransform = actualTransform.GetInverse();
							CRuMatrix4x4 expectedTransform = desiredTransform * invActualTransform;

							// Decompose expected transform and weigh the rotation
							CRuVector3 scale, shear, rotation, translation;
							expectedTransform.GetDecomposition(scale, shear, rotation, translation);

							// Gotta tweak this guard value....... against wrap-arounds, but also not to get stuck in one compensated pose
							REAL guardValue = ruPI;
							if(	fabs(rotation.m_x - m_motionCompensators[i].m_lastRotation.m_x) >= guardValue ||
								fabs(rotation.m_y - m_motionCompensators[i].m_lastRotation.m_y) >= guardValue ||
								fabs(rotation.m_z - m_motionCompensators[i].m_lastRotation.m_z) >= guardValue)
							{
								rotation = m_motionCompensators[i].m_lastRotation;
							}
							else
							{
								m_motionCompensators[i].m_lastRotation = rotation;
							}

							rotation = rotation * m_motionCompensators[i].m_compensatorWeight;

							CRuQuaternion rotQuat;
							rotQuat.MakeEulerRotation(rotation);

							// Multiply transform with existing node transform
							CRuQuaternion quat0(-subNodeTransform->Orientation[0], -subNodeTransform->Orientation[1], -subNodeTransform->Orientation[2], subNodeTransform->Orientation[3]);
							quat0 = quat0 * rotQuat;

							// Invert rotation, since Granny seems to be right-handed
							subNodeTransform->Orientation[0] = -quat0.m_x;
							subNodeTransform->Orientation[1] = -quat0.m_y;
							subNodeTransform->Orientation[2] = -quat0.m_z;
							subNodeTransform->Orientation[3] = quat0.m_w;
						}
					}
				}

				// Sample world pose
				GrannyBuildWorldPoseSparse(m_grannyModel->Skeleton, 0, m_grannyModel->Skeleton->BoneCount, m_gr2SparseBoneArray, m_gr2SparseBoneArrayReverse, m_grannyLocalPose, NULL, m_grannyWorldPose);

				// Update link frames
				CRuMatrix4x4 blah;
				for(INT32 i = 0; i < m_linkFrames.Count(); ++i)
				{
					// Only update the link frame if it has children
					if(m_linkFrames[i].m_linkFrame->GetFirstChild())
					{
						// Sample composite matrix for the link bone
						granny_real32 *worldPoseMatrix = NULL;

						worldPoseMatrix = GrannyGetWorldPoseComposite4x4(m_grannyWorldPose, m_linkFrames[i].m_linkIndex);

						// Copy composite matrix
						memcpy(blah.m_elements, worldPoseMatrix, sizeof(REAL) * 16);

						// Set transform into link frame
						m_linkFrames[i].m_linkFrame->ResetLocalTransform();
						m_linkFrames[i].m_linkFrame->Transform(m_linkFrames[i].m_refTransform * blah, ruTRANSFORMOP_SET);
					}
				}

				// Update sub node transforms (perform direct memory copy from Granny)
				memcpy(m_subNodeTransforms, GrannyGetWorldPoseComposite4x4Array(m_grannyWorldPose), sizeof(REAL) * 16 * m_numSubNodes);
				memcpy(m_subNodeTransforms_World, GrannyGetWorldPose4x4Array(m_grannyWorldPose), sizeof(REAL) * 16 * m_numSubNodes);

				// Trigger bones updated event
				RuHierarchyEvent_BonesUpdated_Args eventArgs(this);
				m_event_BonesUpdated.Trigger(&eventArgs);
			}
		}
		RuExtLink_Granny_GlobalCS()->Leave();
	}

	RUPROFILE_SAMPLE_END(m_probe_HierarchyUpdate, 3)

	// Chain update call to base class
	return CRuEntity_Container_Impl::Update(elapsedTime);
}

BOOL CRuHierarchy_GR2::QueryCollision(CRuCollisionQuery *colQuery)
{
	if(m_useAlternateSkinnedMeshColPath)
	{
		switch(colQuery->m_queryType)
		{
			case ruCOLQUERY_RAY_PICK:
				{
					CRuCollisionQuery_Ray_Pick *rayQuery = static_cast<CRuCollisionQuery_Ray_Pick *>(colQuery);

					// Get the ray in local coordinates
					CRuRay3 oldRay = rayQuery->m_colRay;
					rayQuery->m_colRay = CRuRay3(rayQuery->m_colRay, *(rayQuery->m_invWorldTransform));

					// Make rough collision query
					if(Intersects(rayQuery->m_colRay, this->GetLocalBounds()))
					{
						if(rayQuery->m_allowRoughPicking)
						{
							REAL intT;
							if(Intersects(rayQuery->m_colRay, m_alternateSkinnedMeshColSphere))
							{
								if(FindIntersection(rayQuery->m_colRay, m_alternateSkinnedMeshColSphere, intT))
								{
									if(rayQuery->m_positiveHit)
									{
										if(intT < rayQuery->m_colDistance)
										{
											rayQuery->m_colDistance = intT;
										}
									}
									else
									{
										rayQuery->m_positiveHit = TRUE;
										rayQuery->m_colDistance = intT;
									}
								}
							}
						}
						else
						{
							// Perform test against local bone OBBs (spheres for now)
							for(INT32 i = 0; i < m_numSubNodes; ++i)
							{
								// Get bind-pose local OBB
								CRuOBB curOBB = m_subNodes[i].m_localOBB;

								// Transform into current pose
								const CRuMatrix4x4 &subNodeTransform = m_subNodeTransforms[i];
								curOBB.Transform(subNodeTransform);

								if(Intersects(curOBB, rayQuery->m_colRay))
								{
									CRuVector3 intPt;
									REAL intT;

									if(FindIntersection(rayQuery->m_colRay, curOBB, intPt, intT))
									{
										if(rayQuery->m_positiveHit)
										{
											if(intT < rayQuery->m_colDistance)
											{
												rayQuery->m_colDistance = intT;
											}
										}
										else
										{
											rayQuery->m_positiveHit = TRUE;
											rayQuery->m_colDistance = intT;
										}
									}
								}

/*
								// Get bind-pose local bounds
								CRuSphere curSphere = m_subNodes[i].m_localBounds;

								// Transform into current pose
								const CRuMatrix4x4 &subNodeTransform = m_subNodeTransforms[i];
								subNodeTransform.TransformPoint(curSphere.Center());

								// Test
								REAL intT;
								if(Intersects(rayQuery->m_colRay, curSphere))
								{
									if(FindIntersection(rayQuery->m_colRay, curSphere, intT))
									{
										if(rayQuery->m_positiveHit)
										{
											if(intT < rayQuery->m_colDistance)
											{
												rayQuery->m_colDistance = intT;
											}
										}
										else
										{
											rayQuery->m_positiveHit = TRUE;
											rayQuery->m_colDistance = intT;
										}
									}
								}
*/
							}
						}
					}

					// Restore old collision ray
					rayQuery->m_colRay = oldRay;
				}

				break;

			default:
				return CRuEntity_Container_Impl::QueryCollision(colQuery);
		}
	}
	else
	{
		return CRuEntity_Container_Impl::QueryCollision(colQuery);
	}

	return TRUE;
}

BOOL CRuHierarchy_GR2::InstantiateGR2Skeleton()
{
	if(m_gr2ModelTemplate->Skeleton)
	{
		m_gr2SkeletonInstanceDirty = FALSE;

		FreeGR2SkeletonInstance();

		if(m_gr2SkeletonIsDeprecated)
		{
			m_grannyModel->Skeleton = GR2_CloneSkeleton(m_gr2ModelTemplate->Skeleton);
		}
		else
		{
			// Make a copy of the skeleton
			granny_skeleton *skeleton = GR2_CloneSkeleton(m_gr2ModelTemplate->Skeleton);

			// Enter critical section
			RuExtLink_Granny_GlobalCS()->Enter();
			{
				// Apply bone modifiers
				for(INT32 i = 0; i < m_boneModifiers.Count(); ++i)
				{
					granny_int32x boneIndex;
					if(GrannyFindBoneByName(skeleton, m_boneModifiers[i].m_boneName, &boneIndex))
					{
						// Build delta transform
						granny_transform deltaTransform;
						GrannyMakeIdentity(&deltaTransform);
						deltaTransform.ScaleShear[0][0] = m_boneModifiers[i].m_scale.m_x;
						deltaTransform.ScaleShear[1][1] = m_boneModifiers[i].m_scale.m_y;
						deltaTransform.ScaleShear[2][2] = m_boneModifiers[i].m_scale.m_z;

						// Pre-multiply bone local transform by the delta transform
//						GrannyPreMultiplyBy(&skeleton->Bones[boneIndex].LocalTransform, &deltaTransform);
						GrannyPostMultiplyBy(&skeleton->Bones[boneIndex].LocalTransform, &deltaTransform);

						// Process terminator bones
						if(m_boneModifiers[i].m_terminatorBoneNames)
						{
							for(INT32 j = 0; m_boneModifiers[i].m_terminatorBoneNames[j]; ++j)
							{
								granny_int32x terminatorBoneIdx;
								if(GrannyFindBoneByName(skeleton, m_boneModifiers[i].m_terminatorBoneNames[j], &terminatorBoneIdx))
								{
									// Work up the bone tree and get the partial transform from delta bone to stop bone
									INT32 parentBoneIndex = skeleton->Bones[terminatorBoneIdx].ParentIndex;

									granny_transform forwardDeltaTransform;
									GrannyMakeIdentity(&forwardDeltaTransform);

									granny_transform originalTransformChain;
									GrannyMakeIdentity(&originalTransformChain);

									while(parentBoneIndex != -1)
									{
										// 1
										granny_transform *stepTransform = &skeleton->Bones[parentBoneIndex].LocalTransform;
										GrannyPreMultiplyBy(&forwardDeltaTransform, stepTransform);

										// 2
										GrannyPreMultiplyBy(&originalTransformChain, &m_gr2ModelTemplate->Skeleton->Bones[parentBoneIndex].LocalTransform);

										// Traverse to next ancestor
										parentBoneIndex = skeleton->Bones[parentBoneIndex].ParentIndex;
									}

									// Invert the partial transform
									granny_transform inverseDeltaTransform;
									GrannyBuildInverse(&inverseDeltaTransform, &forwardDeltaTransform);

									// Apply transforms
									GrannyPreMultiplyBy(&skeleton->Bones[terminatorBoneIdx].LocalTransform, &originalTransformChain);
									GrannyPreMultiplyBy(&skeleton->Bones[terminatorBoneIdx].LocalTransform, &inverseDeltaTransform);
								}
							}
						}


/*
						skeleton->Bones[boneIndex].LocalTransform.ScaleShear[0][0] *= m_boneModifiers[i].m_scale.m_x;
						skeleton->Bones[boneIndex].LocalTransform.ScaleShear[1][1] *= m_boneModifiers[i].m_scale.m_y;
						skeleton->Bones[boneIndex].LocalTransform.ScaleShear[2][2] *= m_boneModifiers[i].m_scale.m_z;
*/
					}
				}

				// Rebuild skeleton
				granny_skeleton_builder *skeletonBuilder = GrannyBeginSkeleton(skeleton->BoneCount);

				for(INT32 i = 0; i < skeleton->BoneCount; ++i)
				{
					granny_transform &localTransform = skeleton->Bones[i].LocalTransform;
					granny_transform worldTransform = GR2_Skeleton_GetBoneWorldTransform(skeleton, i);

					GrannyAddBone(skeletonBuilder, (granny_real32 *) &localTransform.Position, (granny_real32 *) &localTransform.Orientation, (granny_real32 *) &localTransform.ScaleShear,
						(granny_real32 *) &worldTransform.Position, (granny_real32 *) &worldTransform.Orientation, (granny_real32 *) &worldTransform.ScaleShear);

					if(skeleton->Bones[i].ParentIndex >= 0)
					{
						GrannySetBoneParent(skeletonBuilder, i, skeleton->Bones[i].ParentIndex);
					}

					GrannySetBoneLODError(skeletonBuilder, i, skeleton->Bones[i].LODError);
				}

				granny_int32x *remappingTable = ruNEW granny_int32x [skeleton->BoneCount];
				granny_skeleton *builtSkeleton = GrannyEndSkeleton(skeletonBuilder, remappingTable);

				// Copy modified skeleton transforms back into the skeleton instance
				for(INT32 i = 0; i < skeleton->BoneCount; ++i)
				{
					skeleton->Bones[i].LocalTransform = builtSkeleton->Bones[remappingTable[i]].LocalTransform;
					memcpy(skeleton->Bones[i].InverseWorld4x4, builtSkeleton->Bones[remappingTable[i]].InverseWorld4x4, sizeof(float) * 16);
				}

				// Store pointer
				m_grannyModel->Skeleton = skeleton;

				// Free builder skeleton
				GrannyFreeBuilderResult(builtSkeleton);
				delete[] remappingTable;
			}
			// Leave critical section
			RuExtLink_Granny_GlobalCS()->Leave();
		}
	}

	return TRUE;
}

BOOL CRuHierarchy_GR2::FreeGR2SkeletonInstance()
{
	if(m_grannyModel && m_grannyModel->Skeleton)
	{
		GR2_FreeSkeleton(m_grannyModel->Skeleton);
		m_grannyModel->Skeleton = NULL;
	}

	return TRUE;
}

BOOL CRuHierarchy_GR2::InstantiateGR2Model()
{
	// Free existing skeleton structures
	FreeGR2ModelInstance();

	// Enter critical section
	RuExtLink_Granny_GlobalCS()->Enter();
	{
		// Instantiate the model
		m_grannyModelInstance = GrannyInstantiateModel(m_grannyModel);

		// Create local pose buffer
		m_grannyLocalPose = GrannyNewLocalPose(m_grannyModel->Skeleton->BoneCount);

		// Create world pose buffer
		m_grannyWorldPose = GrannyNewWorldPose(m_grannyModel->Skeleton->BoneCount);
	}
	// Leave critical section
	RuExtLink_Granny_GlobalCS()->Leave();

	return TRUE;
}

BOOL CRuHierarchy_GR2::FreeGR2ModelInstance()
{
	// Enter critical section
	RuExtLink_Granny_GlobalCS()->Enter();
	{
		// Free existing structures
		if(m_grannyModelInstance)
		{
			GrannyFreeModelInstance(m_grannyModelInstance);
			m_grannyModelInstance = NULL;
		}

		if(m_grannyLocalPose)
		{
			GrannyFreeLocalPose(m_grannyLocalPose);
			m_grannyLocalPose = NULL;
		}

		if(m_grannyWorldPose)
		{
			GrannyFreeWorldPose(m_grannyWorldPose);
			m_grannyWorldPose = NULL;
		}
	}
	// Leave critical section
	RuExtLink_Granny_GlobalCS()->Leave();

	return TRUE;
}

BOOL CRuHierarchy_GR2::InstantiateSubNodeStructure()
{
	FreeSubNodeStructure();

	// Create supporting subnode structures
	m_numSubNodes = m_grannyModel->Skeleton->BoneCount;
	m_subNodes = ruNEW SubNodeDesc [m_numSubNodes];
	m_subNodeTransforms = ruNEW CRuMatrix4x4 [m_numSubNodes];
	m_subNodeTransforms_World = ruNEW CRuMatrix4x4 [m_numSubNodes];
	m_restPoseTransforms_World = ruNEW CRuMatrix4x4 [m_numSubNodes];

	return TRUE;
}

BOOL CRuHierarchy_GR2::FreeSubNodeStructure()
{
	delete[] m_subNodes;
	delete[] m_subNodeTransforms;
	delete[] m_subNodeTransforms_World;
	delete[] m_restPoseTransforms_World;

	m_numSubNodes = 0;
	m_subNodes = NULL;
	m_subNodeTransforms = NULL;
	m_subNodeTransforms_World = NULL;
	m_restPoseTransforms_World = NULL;

	return TRUE;
}

BOOL CRuHierarchy_GR2::ConstructMotionChannels()
{
	// Make a copy of the existing motion channels
	CRuArrayList<MotionChannelDesc> oldMotionChannels(m_motionChannels);

	// Clear motion channel list
	m_motionChannels.Clear();

	// Enter critical section
	RuExtLink_Granny_GlobalCS()->Enter();
	{
		// Free root blend node's entire tree
		if(m_rootBlendNode)
		{
			GrannyFreeBlendDagEntireTree(m_rootBlendNode); // GRANNY: UNSAFE
			m_rootBlendNode = NULL;
		}

		// Build supported motion channel mask
		UINT32 supportedMotionChannels = 0x00000000;
		for(INT32 i = 0; i < m_numSubNodes; ++i)
		{
			// Append group ID to supported channels mask
			supportedMotionChannels = supportedMotionChannels | m_subNodes[i].m_groupID;
		}

		// Construct default channel (0xFFFFFFFF)
		{
			MotionChannelDesc defaultChannelDesc;
			defaultChannelDesc.m_channelMask = 0xFFFFFFFF;
			defaultChannelDesc.m_modelMask = GrannyNewTrackMask(1.0f, m_numSubNodes);
			defaultChannelDesc.m_animation = NULL;
			defaultChannelDesc.m_loopDelay = -1.0f;
			defaultChannelDesc.m_weightMultiplier = 1.0f;
			defaultChannelDesc.m_timeMultiplier = 1.0f;

			defaultChannelDesc.m_fadePose = GrannyNewLocalPose(m_grannyModel->Skeleton->BoneCount);
			defaultChannelDesc.m_fadePoseNode = GrannyCreateBlendDagNodeLocalPose(defaultChannelDesc.m_fadePose, false); // GRANNY: UNSAFE

			defaultChannelDesc.m_control = NULL;
			defaultChannelDesc.m_controlModelInstance = GrannyInstantiateModel(m_grannyModel);	// GRANNY: UNSAFE
			defaultChannelDesc.m_controlNode = GrannyCreateBlendDagNodeAnimationBlend(defaultChannelDesc.m_controlModelInstance, false); // GRANNY: UNSAFE
			defaultChannelDesc.m_controlTTL = -1;

			defaultChannelDesc.m_dagCrossFadeNode = GrannyCreateBlendDagNodeCrossfade(defaultChannelDesc.m_controlNode, defaultChannelDesc.m_fadePoseNode, 0.0f, 0.0f, NULL, false, m_grannyModel->Skeleton->BoneCount); // GRANNY: UNSAFE
			defaultChannelDesc.m_blendedPose = GrannyNewLocalPose(m_grannyModel->Skeleton->BoneCount);

			defaultChannelDesc.m_dagBlendNode = GrannyCreateBlendDagNodeLocalPose(defaultChannelDesc.m_blendedPose, false); // GRANNY: UNSAFE

			// Set node's blend weight to zero
			GrannySetBlendDagNodeResultWeight(defaultChannelDesc.m_dagBlendNode, 0.0f); // GRANNY: UNSAFE

			m_motionChannels.Add(defaultChannelDesc);
		}

		// Construct channel descriptor for all unique channels
		for(UINT32 i = 0, channelMask = 0x00000001; i < 32; ++i, channelMask = channelMask << 1)
		{
			if(channelMask & supportedMotionChannels)
			{
				MotionChannelDesc channelDesc;

				channelDesc.m_channelMask = channelMask;
				channelDesc.m_modelMask = GrannyNewTrackMask(0.0f, m_numSubNodes);
				channelDesc.m_animation = NULL;
				channelDesc.m_loopDelay = -1.0f;
				channelDesc.m_weightMultiplier = 1.0f;
				channelDesc.m_timeMultiplier = 1.0f;

				// Scan through all subnodes
				for(INT32 j = 0; j < m_numSubNodes; ++j)
				{
					// If the subnode is included in the current channel, enable it in the track mask
					if(m_subNodes[j].m_groupID & channelMask)
					{
						GrannySetTrackMaskBoneWeight(channelDesc.m_modelMask, j, 1.0f);
					}
				}

				channelDesc.m_fadeWeight = 0.0f;
				channelDesc.m_fadePose = GrannyNewLocalPose(m_grannyModel->Skeleton->BoneCount);
				channelDesc.m_fadePoseNode = GrannyCreateBlendDagNodeLocalPose(channelDesc.m_fadePose, false); // GRANNY: UNSAFE

				channelDesc.m_control = NULL;
				channelDesc.m_controlModelInstance = GrannyInstantiateModel(m_grannyModel); // GRANNY: UNSAFE
				channelDesc.m_controlNode = GrannyCreateBlendDagNodeAnimationBlend(channelDesc.m_controlModelInstance, false); // GRANNY: UNSAFE
				channelDesc.m_controlTTL = -1;

				channelDesc.m_dagCrossFadeNode = GrannyCreateBlendDagNodeCrossfade(channelDesc.m_controlNode, channelDesc.m_fadePoseNode, 0.0f, 0.0f, NULL, false, m_grannyModel->Skeleton->BoneCount); // GRANNY: UNSAFE
				channelDesc.m_blendedPose = GrannyNewLocalPose(m_grannyModel->Skeleton->BoneCount);

				channelDesc.m_dagBlendNode = GrannyCreateBlendDagNodeLocalPose(channelDesc.m_blendedPose, false); // GRANNY: UNSAFE

				// Set node's blend weight to zero
				GrannySetBlendDagNodeResultWeight(channelDesc.m_dagBlendNode, 0.0f); // GRANNY: UNSAFE

				// Insert channel descriptor to the channel list
				m_motionChannels.Add(channelDesc);
			}
		}

		// Create master blending node
		m_rootBlendNode = GrannyCreateBlendDagNodeWeightedBlend(m_grannyModel->Skeleton, GrannyDefaultLocalPoseFillThreshold, GrannyBlendQuaternionNeighborhooded, m_grannyModel->Skeleton->BoneCount); // GRANNY: UNSAFE

		for(INT32 i = 0; i < m_motionChannels.Count(); ++i)
		{
			GrannyAddBlendDagNodeChild(m_rootBlendNode, m_motionChannels[i].m_dagBlendNode); // GRANNY: UNSAFE
			GrannySetBlendDagNodeResultTrackMask(m_motionChannels[i].m_dagBlendNode, m_motionChannels[i].m_modelMask, false); // GRANNY: UNSAFE
		}

		// If there is a rest-state animation, use it to build initial pose
		if(m_grRestAnimation && m_motionChannels.Count() > 0)
		{
			// Set cross fade to use pose from control
			m_motionChannels[0].m_fadeWeight = 0.0f;
			GrannySetBlendDagNodeCrossfadeWeights(m_motionChannels[0].m_dagCrossFadeNode, 0.0f, m_motionChannels[0].m_fadeWeight); // GRANNY: UNSAFE

			// Build controlled animation for the rest-state animation
			granny_controlled_animation_builder *builder = GrannyBeginControlledAnimation(0.0f, m_grRestAnimation); // GRANNY: UNSAFE
			GrannySetTrackGroupTarget(builder, 0, m_motionChannels[0].m_controlModelInstance); // GRANNY: UNSAFE
			m_motionChannels[0].m_control = GrannyEndControlledAnimation(builder); // GRANNY: UNSAFE

			// Set control clock to a known state
			GrannySetControlClock(m_motionChannels[0].m_control, 0.0f);

			// Sample local pose
			granny_local_pose *channelLocalPose = GrannySampleBlendDagTree(m_motionChannels[0].m_dagCrossFadeNode, m_grannyModel->Skeleton->BoneCount); // GRANNY: UNSAFE
			GrannyCopyLocalPose(channelLocalPose, m_motionChannels[0].m_blendedPose);

			CRuMatrix4x4 boo;
			GrannyGetWorldMatrixFromLocalPose(m_grannyModel->Skeleton, 0, channelLocalPose, NULL, (granny_real32 *) &boo, NULL, NULL);

			// Set node's blend weight to one
			GrannySetBlendDagNodeResultWeight(m_motionChannels[0].m_dagBlendNode, 1.0f); // GRANNY: UNSAFE

			// Sample local pose
			granny_local_pose *localPose = GrannySampleBlendDagTree(m_rootBlendNode, m_grannyModel->Skeleton->BoneCount); // GRANNY: UNSAFE
			GrannyCopyLocalPose(localPose, m_grannyLocalPose);

			// Sample world pose
			GrannyBuildWorldPose(m_grannyModel->Skeleton, 0, m_grannyModel->Skeleton->BoneCount, m_grannyLocalPose, NULL, m_grannyWorldPose);

			GrannyGetWorldMatrixFromLocalPose(m_grannyModel->Skeleton, 0, channelLocalPose, NULL, (granny_real32 *) &boo, NULL, NULL);

			// Update link frames
			for(INT32 i = 0; i < m_linkFrames.Count(); ++i)
			{
				// Sample composite matrix for the link bone
				granny_real32 *worldPoseMatrix = NULL;

				if(m_linkFrames[i].m_useWorldSpaceTransform)
				{
					worldPoseMatrix = GrannyGetWorldPose4x4(m_grannyWorldPose, m_linkFrames[i].m_linkIndex);
				}
				else
				{
					worldPoseMatrix = GrannyGetWorldPoseComposite4x4(m_grannyWorldPose, m_linkFrames[i].m_linkIndex);
				}

				// Copy composite matrix
				CRuMatrix4x4 blah;
				memcpy(blah.m_elements, worldPoseMatrix, sizeof(REAL) * 16);

				// Set transform into link frame
				m_linkFrames[i].m_linkFrame->ResetLocalTransform();
				m_linkFrames[i].m_linkFrame->Transform(blah);

				m_linkFrames[i].m_refTransform = blah;
			}

			// Update sub node transforms
			for(INT32 i = 0; i < m_numSubNodes; ++i)
			{
				granny_real32 *worldPoseMatrixComposite = GrannyGetWorldPoseComposite4x4(m_grannyWorldPose, i);
				memcpy(m_subNodeTransforms[i].m_elements, worldPoseMatrixComposite, sizeof(REAL) * 16);

				granny_real32 *worldPoseMatrix = GrannyGetWorldPose4x4(m_grannyWorldPose, i);
				memcpy(m_subNodeTransforms_World[i].m_elements, worldPoseMatrix, sizeof(REAL) * 16);
				memcpy(m_restPoseTransforms_World[i].m_elements, worldPoseMatrix, sizeof(REAL) * 16);
			}

			// Free control
			GrannyFreeControl(m_motionChannels[0].m_control); // GRANNY: UNSAFE
			m_motionChannels[0].m_control = NULL;
		}

		// Iterate through all motion channels and re-setup previously running animations
		for(INT32 i = 0; i < m_motionChannels.Count(); ++i)
		{
			for(INT32 j = 0; j < oldMotionChannels.Count(); ++j)
			{
				if(m_motionChannels[i].m_channelMask == oldMotionChannels[j].m_channelMask)
				{
					if(oldMotionChannels[j].m_control && oldMotionChannels[j].m_animation)
					{
						// Set crossfade weight
						GrannySetBlendDagNodeCrossfadeWeights(m_motionChannels[i].m_dagCrossFadeNode, 0.0f, m_motionChannels[i].m_fadeWeight); // GRANNY: UNSAFE

						// Release existing animation
						ruSAFE_RELEASE(m_motionChannels[i].m_animation);

						// Store reference to new animation
						m_motionChannels[i].m_animation = oldMotionChannels[j].m_animation;
						m_motionChannels[i].m_animation->AddRef();

						// Store loop delay
						m_motionChannels[i].m_loopDelay = oldMotionChannels[j].m_loopDelay;

						// Build controlled animation
						granny_controlled_animation_builder *builder = GrannyBeginControlledAnimation(0.0f, m_motionChannels[i].m_animation->GetGR2Animation()); // GRANNY: UNSAFE
						GrannySetTrackGroupTarget(builder, 0, m_motionChannels[i].m_controlModelInstance); // GRANNY: UNSAFE
						m_motionChannels[i].m_control = GrannyEndControlledAnimation(builder); // GRANNY: UNSAFE
						m_motionChannels[i].m_controlTTL = -1;

						// Set the control's loop count to 1 and its completion time to length of animation
						GrannySetControlLoopCount(m_motionChannels[i].m_control, 1);
						GrannyCompleteControlAt(m_motionChannels[i].m_control, m_motionChannels[i].m_animation->GetGR2Animation()->Duration);

						// Set control clock
						REAL controlClock = GrannyGetControlClock(oldMotionChannels[j].m_control); // GRANNY: UNSAFE
						GrannySetControlClock(m_motionChannels[i].m_control, controlClock);

						break;
					}
				}
			}
		}
	}
	// Leave critical section
	RuExtLink_Granny_GlobalCS()->Leave();

	// Free previously active motion channels
	FreeMotionChannels(oldMotionChannels);

	return TRUE;
}

BOOL CRuHierarchy_GR2::FreeMotionChannels(CRuArrayList<MotionChannelDesc> &motionChannels)
{
	// Enter critical section
	RuExtLink_Granny_GlobalCS()->Enter();
	{
		// Iterate through all motion channel descriptors
		for(INT32 i = 0; i < motionChannels.Count(); ++i)
		{
			// Free model mask
			GrannyFreeTrackMask(motionChannels[i].m_modelMask);

			// Release animation
			ruSAFE_RELEASE(motionChannels[i].m_animation);

			// Release active control
			if(motionChannels[i].m_control)
			{
				GrannyFreeControl(motionChannels[i].m_control); // GRANNY: UNSAFE
			}

			// Release model instance used by the motion channel
			GrannyFreeModelInstance(motionChannels[i].m_controlModelInstance); // GRANNY: UNSAFE

			// Release fade-out pose
			GrannyFreeLocalPose(motionChannels[i].m_fadePose);

			// Release blending DAG tree
			GrannyFreeBlendDagEntireTree(motionChannels[i].m_dagCrossFadeNode); // GRANNY: UNSAFE

			// Release local blended pose
			GrannyFreeLocalPose(motionChannels[i].m_blendedPose);
		}
	}
	// Leave critical section
	RuExtLink_Granny_GlobalCS()->Leave();

	// Clear all motion channel descriptors
	motionChannels.Clear();

	return TRUE;
}

BOOL CRuHierarchy_GR2::UpdateSparseBoneArray()
{
	// Initialize bone use bitfield
	CRuBitField boneUsed(m_numSubNodes);
	boneUsed.ClearAll();

	// Enumerate all meshes
	CRuArrayList<IRuMesh *> meshList;
	RuEntity_EnumerateMeshes(this, meshList, NULL);

	for(INT32 i = 0; i < meshList.Count(); ++i)
	{
		IRuMorphTarget *morphTarget = meshList[i]->GetMorphTarget(0);

		if(morphTarget)
		{
			// Iterate through all blend indices and set each bone as used
			INT32 blendIndexCount = meshList[i]->GetNumVertices() * 4;
			const UINT16 *blendIndices = morphTarget->GetBlendIndex();

			if(blendIndices)
			{
				for(INT32 j = 0; j < blendIndexCount; ++j)
				{
					boneUsed.Set(blendIndices[j]);
				}
			}
		}
	}

/*
	IRuMesh *skinnableMesh = RuEntity_FindFirstSkinnableMesh(this);

	// Use the first skinnable mesh to initialize the used bones array
	if(skinnableMesh)
	{
		IRuMorphTarget *morphTarget = skinnableMesh->GetMorphTarget(0);

		if(morphTarget)
		{
			// Iterate through all blend indices and set each bone as used
			INT32 blendIndexCount = skinnableMesh->GetNumVertices() * 4;
			const UINT16 *blendIndices = morphTarget->GetBlendIndex();

			if(blendIndices)
			{
				for(INT32 j = 0; j < blendIndexCount; ++j)
				{
					boneUsed.Set(blendIndices[j]);
				}
			}
		}
	}
*/
	// Iterate through link frames
	for(INT32 i = 0; i < m_linkFrames.Count(); ++i)
	{
		boneUsed.Set(m_linkFrames[i].m_linkIndex);
	}

	// Mark all motion compensator bones as used in the bone map
	for(INT32 i = 0; i < m_motionCompensators.Count(); ++i)
	{
		boneUsed.Set(m_motionCompensators[i].m_subNodeIndex);
	}

	// Iterate through all used bones and ensure that their ancestors are also marked as used
	// We iterate backwards from the bottom of the tree, so that no inner loop is necessary
	for(INT32 i = m_numSubNodes - 1; i >= 0; --i)
	{
		if(boneUsed[i])
		{
			if(m_grannyModel->Skeleton->Bones[i].ParentIndex > -1)
			{
				boneUsed.Set(m_grannyModel->Skeleton->Bones[i].ParentIndex);
			}
		}
	}

	// Count the number of used bones
	m_gr2SparseBoneArraySize = 0;
	for(INT32 i = 0; i < m_numSubNodes; ++i)
	{
		if(boneUsed[i])
		{
			++m_gr2SparseBoneArraySize;
		}
	}

	// Construct the sparse bone array along with its reverse mapping
	delete[] m_gr2SparseBoneArray;
	delete[] m_gr2SparseBoneArrayReverse;

	m_gr2SparseBoneArray = ruNEW granny_int32x [m_gr2SparseBoneArraySize];
	m_gr2SparseBoneArrayReverse = ruNEW granny_int32x [m_numSubNodes];
	for(INT32 i = 0, j = 0; i < m_numSubNodes; ++i)
	{
		if(boneUsed[i])
		{
			m_gr2SparseBoneArray[j] = i;
			m_gr2SparseBoneArrayReverse[i] = j;
			++j;
		}
		else
		{
			m_gr2SparseBoneArrayReverse[i] = -1;
		}
	}

	return TRUE;
}

CRuHierarchy_GR2::MotionChannelDesc *CRuHierarchy_GR2::FindChannelDescriptor(UINT32 motionChannel)
{
	for(INT32 i = 0; i < m_motionChannels.Count(); ++i)
	{
		if(m_motionChannels[i].m_channelMask == motionChannel)
		{
			return &m_motionChannels[i];
		}
	}

	return NULL;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
