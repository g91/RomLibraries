/*!
	@project Rune
	@file RuHelper_Entity.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/04/13
*/

#ifndef _RUHELPER_ENTITY_H_
#define _RUHELPER_ENTITY_H_

#include "../Scene/Base/RuEntityBase.h"
#include "../Scene/Base/RuEntityACT.h"
#include "../Scene/Base/RuHierarchy_GR2.h"
#include "../Scene/Container/RuContainer_Portal.h"
#include "../Scene/Controller/RuController_Camera.h"
#include "../Scene/Particle/RuEntity_Particle_Visualizer_Sprite.h"
#include "../Fusion/RuFusion_Mime.h"
#include "../Fusion/RuFusion_Sojourner.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

// Transform utilities
extern RUAPI CRuMatrix4x4			RuMatrixUtility_BuildOrientationTransform(const CRuVector3 &defaultForward, const CRuVector3 &curForward, const CRuVector3 &curUp, REAL curScale);
extern RUAPI CRuMatrix4x4			RuMatrixUtility_BuildConstrainedOrientationTransform(const CRuVector3 &defaultForward, const CRuVector3 &curForward, const CRuVector3 &curUp, REAL curScale, REAL upRotRate, REAL &upRotAngle);

// Movement and orientation
extern RUAPI REAL					RuEntity_FindAnchorPoint(const CRuVector3 &entityPosition, IRuObject *anchorObject, REAL entityMassOffset = 5.0f, REAL testIncrement = 1.0f, REAL testRange = 5.0f);
extern RUAPI REAL					RuEntity_FindAnchorPoint(const CRuVector3 &entityPosition, CRuArrayList<IRuObject *> &anchorObjects, REAL entityMassOffset = 5.0f, REAL testIncrement = 1.0f, REAL testRange = 5.0f);
extern RUAPI BOOL					RuEntity_CalculateMoveVector(CRuEntity *entity, const CRuVector3 &defaultForward, REAL forwardDelta, REAL rightDelta, CRuVector3 *moveVectorOut);
extern RUAPI BOOL					RuEntity_Move(CRuEntity *entity, const CRuVector3 &defaultForward, REAL forwardDelta, REAL rightDelta);
extern RUAPI BOOL					RuEntity_MoveEx(CRuEntity *entity, IRuObject *anchorObject, const CRuVector3 &defaultForward, const CRuVector3 &entityForward, const CRuVector3 &moveDelta, REAL moveSpeed, REAL elapsedTime);

extern RUAPI BOOL					RuEntity_Orient(CRuEntity *entity, const CRuVector3 &defaultForward, const CRuVector3 &entityForward, REAL entityScale);
extern RUAPI BOOL					RuEntity_Orient(CRuEntity *entity, const CRuVector3 &defaultForward, const CRuVector3 &entityForward, const CRuVector3 &entityUp, REAL entityScale);
extern RUAPI BOOL					RuEntity_OrientConstrained(CRuEntity *entity, const CRuVector3 &defaultForward, const CRuVector3 &entityForward, const CRuVector3 &entityUp, REAL entityScale, REAL upRotRate, REAL &upRotAngle);

extern RUAPI CRuVector3				RuEntity_CalculateForwardVector(CRuEntity *entity, const CRuVector3 &defaultForward);
extern RUAPI REAL					RuEntity_FindRestingHeight(const CRuVector3 &entityPosition, IRuObject *anchorObject, REAL testLimit = 4.0, REAL addHeight=0.0f);
extern RUAPI CRuVector3 			RuEntity_FindRestingNormal(const CRuVector3 &entityPosition, IRuObject *anchorObject);

// Miscellaneous
extern RUAPI INT32					RuEntity_GetMaxMorphTargetCount(CRuEntity *entity);
extern RUAPI BOOL					RuEntity_PickTest(CRuEntity *entity, CRuCamera *camera, IRuSwapChain *swapChain, INT32 x, INT32 y, BOOL allowRoughPicking, REAL *pickDistanceOut = NULL);

// Entity hierarchy tools
extern BOOL							RuEntity_Traverse_BFS(CRuEntity *entity, CRuQueue<CRuEntity *> &bfsQueueOut);

// Link management
extern RUAPI BOOL					RuEntity_Attach(CRuEntity *parentEntity, CRuEntity *childEntity, const char *linkName);
extern RUAPI BOOL					RuEntity_Attach_BFS(CRuEntity *parentEntity, CRuEntity *childEntity, const char *linkName);
extern RUAPI BOOL					RuEntity_Detach(CRuEntity *parentEntity, CRuEntity *childEntity, const char *linkName);
extern RUAPI BOOL					RuEntity_DetachAll(CRuEntity *parentEntity, const char *linkName);
extern RUAPI CRuEntity*				RuEntity_FindLinkFrame(CRuEntity *entity, const char *linkName, BOOL forceLinkFrameUpdate = FALSE);
extern RUAPI void					RuEntity_EnumerateLinkFrames(CRuEntity *entity, CRuArrayList<CRuLinkFrame *> &linkFramesOut);

extern RUAPI void					RuEntity_EnumerateFrames(CRuEntity *entity, CRuArrayList<CRuFrame *> &framesOut);
extern RUAPI void					RuEntity_EnumerateEntities(CRuEntity *entity, const CRuRTTI &entityType, BOOL exactTypeMatch, CRuArrayList<CRuEntity *> &entitiesOut);

// Utility functions
extern BOOL							RuEntity_UseParentLighting(CRuEntity *entity, BOOL useParentLighting);
extern RUAPI BOOL					RuEntity_SetOption(CRuEntity *entity, RuEntityOption option, INT32 value);
extern RUAPI BOOL					RuEntity_SetOptionWithTypeFilter(CRuEntity *entity, RuEntityOption option, INT32 value, const CRuRTTI &objType);
extern RUAPI CRuModel*				RuEntity_FindFirstModel(CRuEntity *entity);
extern RUAPI CRuFrameHierarchy*		RuEntity_FindFirstFrameHierarchy(CRuEntity *entity);
extern RUAPI CRuHierarchy_GR2*		RuEntity_FindFirstHierarchy_GR2(CRuEntity *entity);
extern RUAPI CRuACTEntity*			RuEntity_FindFirstACTEntity(CRuEntity *entity);
extern RUAPI IRuPaperdoll*			RuEntity_FindFirstPaperdoll(CRuEntity *entity);
extern RUAPI CRuEntity*				RuEntity_FindFirstSkinnedModel(CRuEntity *entity);
extern RUAPI CRuContainer_Portal*	RuEntity_FindFirstPortalContainer(CRuEntity *entity);
extern RUAPI CRuACTEntity*			RuEntity_FindParentACTEntity(CRuEntity *entity);
extern RUAPI CRuHierarchy_GR2*		RuEntity_FindParentHierarchy_GR2(CRuEntity *entity);
extern RUAPI CRuEntity*				RuEntity_FindNearestEntityByName(CRuEntity *entity, const char *name, CRuEntity *excludedEntity = NULL);

extern RUAPI BOOL					RuEntity_ReplaceInteriorEntity(CRuEntity *entity, CRuEntity *replacedEntity, CRuEntity *newEntity);

// Generic search routines
extern RUAPI CRuEntity*				RuEntity_FindFirstEntityOfType(CRuEntity *entity, const CRuRTTI &objType, const CRuRTTI **allowableAncestorTypes = NULL, BOOL skipRoot = FALSE);

// Hierarchy support
extern RUAPI BOOL					RuEntity_RefreshHierarchySubNodeBounds(CRuEntity *entity);

// Transform functions
extern RUAPI BOOL					RuEntity_TransformVector_WorldToLocal(CRuEntity *entity, const CRuVector3 &v0, CRuVector3 &v0Out);

// Material functions
extern RUAPI BOOL					RuEntity_GetVisibleStatusFlag(CRuEntity *entity);

extern RUAPI BOOL					RuEntity_EnableCelHighlight(CRuEntity *entity, RUCOLOR highlightColor);
extern RUAPI BOOL					RuEntity_DisableCelHighlight(CRuEntity *entity);
extern RUAPI BOOL					RuEntity_ClearShaderOverrides(CRuEntity *entity);

// Debugging and enumeration
extern RUAPI char*					RuEntity_EnumerateTypeStatistics(CRuEntity *entity);
extern RUAPI INT32					RuEntity_GetMeshFootprint(CRuEntity *entity);
extern RUAPI INT32					RuEntity_GetExternalColStructFootprint(CRuEntity *entity);

// Other helpers
extern RUAPI POINTER				RuEntity_GetTagFromAnyDescendant(CRuEntity *entity, INT32 tagID);
extern RUAPI BOOL					RuEntity_SetHierarchyIgnoreVisibilityCheck(CRuEntity *entity, BOOL ignoreVisibilityCheck);

extern RUAPI BOOL					RuEntity_DisablePortalFunction(CRuEntity *entity, BOOL disablePortalFunction);

extern RUAPI BOOL					RuEntity_EnableAlphaZ(CRuEntity *entity);

extern RUAPI BOOL					RuModel_SetTextureIsEmbedded(CRuModel *model, RuTextureChannel channel, BOOL isEmbedded);

// Skeletal helpers
extern RUAPI CRuMatrix4x4&			RuHierarchy_GetSubNodeTransform(CRuFrameHierarchy *hierarchy, CRuAnimationInterpolator *interpolator, INT32 nodeIndex, CRuMatrix4x4 &transformOut);
extern RUAPI CRuMatrix4x4&			RuHierarchy_GetSubNodeParentTransform(CRuFrameHierarchy *hierarchy, CRuAnimationInterpolator *interpolator, INT32 nodeIndex, CRuMatrix4x4 &transformOut);

// Collision functions
extern RUAPI BOOL					RuCollision_TestSphere(const CRuSphere &testSphere, IRuObject *anchorObject);

// ACT functions
extern RUAPI BOOL					RuACT_HasMotion(CRuEntity *entity, const char *motionName, BOOL firstACTOnly = TRUE);

extern RUAPI BOOL					RuACT_SupportsMotionChannel(CRuEntity *entity, UINT32 channel);
extern RUAPI BOOL					RuACT_PauseChannel(CRuEntity *entity, UINT32 channel);
extern RUAPI BOOL					RuACT_ResumeChannel(CRuEntity *entity, UINT32 channel);

extern RUAPI BOOL					RuACT_PlayPersistentMotion(CRuEntity *entity, BOOL forceReset = FALSE);
extern RUAPI BOOL					RuACT_PlayMotion(CRuEntity *entity, const char *motionName, REAL blendTime = 0.0f, BOOL forceReset = FALSE, CRuACT_ActionEvents *actionEventsOut = NULL, BOOL rootACT = TRUE);
extern RUAPI BOOL					RuACT_PlayMotion(CRuEntity *entity, INT32 motionID, REAL blendTime = 0.0f, BOOL forceReset = FALSE, CRuACT_ActionEvents *actionEventsOut = NULL, BOOL rootACT = TRUE);
extern RUAPI BOOL					RuACT_PlayMotionEx(CRuEntity *entity, UINT32 channel, const char *motionName, REAL blendTime = 0.0f, BOOL forceReset = FALSE, REAL channelWeight = 1.0f, REAL leadIn = 0.0f, REAL leadOut = 0.0f, REAL expectedMotionScale = 0.0f, CRuACT_ActionEvents *actionEventsOut = NULL, BOOL rootACT = TRUE);
extern RUAPI BOOL					RuACT_SwapMotion(CRuEntity *entity, UINT32 srcChannel, UINT32 destChannel, BOOL rootACT = TRUE);
extern RUAPI BOOL					RuACT_ClearMotion(CRuEntity *entity, UINT32 channel, REAL fadeDuration, BOOL rootACT = TRUE);
extern RUAPI BOOL					RuACT_IsMotionComplete(CRuEntity *entity, UINT32 channel = 0xFFFFFFFF, BOOL ignoreLoopingControllers = FALSE);
extern RUAPI REAL					RuACT_GetMotionLength(CRuEntity *entity, const char *motionName);
extern RUAPI INT32					RuACT_GetMotionIndexByName(CRuEntity *entity, const char *motionName);
extern RUAPI const char*			RuACT_GetActiveMotionName(CRuEntity *entity, UINT32 channel = 0xFFFFFFFF);
extern RUAPI REAL					RuACT_GetActiveMotionTime(CRuEntity *entity, UINT32 channel = 0xFFFFFFFF);
extern RUAPI BOOL					RuACT_CheckForController(CRuEntity *entity, UINT32 channel, const CRuRTTI &controllerType, BOOL firstACTOnly = TRUE);
extern RUAPI BOOL					RuACT_CheckForEvent(CRuEntity *entity, UINT32 channel, RuEntityEvent eventType, REAL *startTimeOut = NULL, BOOL firstACTOnly = TRUE);
extern RUAPI BOOL					RuACT_EnumerateMotionNames(CRuEntity *entity, CRuArrayList<const char *> &motionNamesOut);
extern RUAPI BOOL					RuACT_PlugMissingMotions(CRuEntity *entity, const char *commandStream, BOOL firstACTOnly = TRUE);

// Paperdoll functions
extern RUAPI BOOL					RuPaperdoll_SetMaxMipLevelOverride(CRuEntity *entity, INT32 maxMipLevel);
extern RUAPI BOOL					RuPaperdoll_SetColorCodes(CRuEntity *entity, INT32 color0Code, INT32 color1Code);
extern RUAPI BOOL					RuPaperdoll_SetComponent(CRuEntity *entity, const char *partName, const char *componentName);
extern RUAPI BOOL					RuPaperdoll_SetComponentColors(CRuEntity *entity, const char *partName, BOOL layerActivated[4], RUCOLOR layerColor[4]);
extern RUAPI BOOL					RuPaperdoll_Build(CRuEntity *entity, BOOL allowDelayedLoad = TRUE);
extern RUAPI BOOL					RuPaperdoll_IsBuilding(CRuEntity *entity);
extern RUAPI BOOL					RuPaperdoll_BuildCompleteCallback(IRuPaperdoll *paperdoll);

// Mime functions
extern RUAPI BOOL					RuMime_SetBoneScale(CRuFusion_Mime *mime, const char *scalerName, REAL scale);

// Poly line stuff
extern RUAPI BOOL					RuPolyLine_SetTarget(CRuEntity *entity, CRuEntity *targetEntity);

// Particle system helper
extern RUAPI BOOL					RuPrtSys_SetUpdateInterval(CRuEntity *entity, REAL updateInterval);

// VizCon helper
extern RUAPI BOOL					RuVizCon_SetupVizConState(CRuEntity *entity);

// Camera functions
extern RUAPI BOOL					RuEntity_SelectCamera(CRuEntity *entity, const char *cameraName, CRuController_Camera *cameraController);

// Miscellaneous
extern RUAPI const char*			RuEnumConverter_ToString(RuTextureChannel texChannel);

// ************************************************************************************************************************************************************

extern char*				g_ruACT_DefaultMotionChart;

// ************************************************************************************************************************************************************


inline void test(CRuVector3 entityPosition, IRuObject *colObject)
{
	for(int i = 0; i < 10000; ++i)
	{
//		moo = RuEntity_FindAnchorPoint(entityPosition, colObject);

		CRuCollisionQuery_Segment_Nearest colQuery;
		colQuery.m_colSegment = CRuSegment3(entityPosition + CRuVector3(0.0f, 5.0f, 0.0f), entityPosition - CRuVector3(0.0f, 5.0f, 0.0f));
		if(colObject->GetType().IsSubClassOf(CRuEntity::Type()))
		{
			static_cast<CRuEntity *>(colObject)->QueryCollision(&colQuery);
		}
		else if(colObject->GetType().IsSubClassOf(IRuCollisionHierarchy::Type()))
		{
			static_cast<IRuCollisionHierarchy *>(colObject)->QueryCollision(&colQuery);
		}
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
