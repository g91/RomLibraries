/*!
	@project Rune
	@file RuHelper_Entity.cpp

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/04/13
*/

#include "../Helper/RuHelper_Entity.h"

#include "../Engine/Base/RuShader_Composite.h"
#include "../Renderer/Plugin/RuRendererPlugin_AlphaZ.h"
#include "../Scene/Controller/RuController_Highlighter.h"
#include "../Scene/FX/RuEntity_PolyLine.h"
#include "../Scene/Particle/RuEntity_Particle_Emitter.h"

// this one is temporary... make it so we are not inter-dependent
#include "../Helper/RuHelper_IRuEntity_Renderable.h"

//#include "../../../Interface/chat/ChatFrame.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

static CRuThread_CriticalSection local_EntityQueueCS;
static CRuQueue<CRuEntity *> local_EntityQueue;

static CRuQueue<CRuEntity *> &RuHelperInternal_LockEntityQueue()
{
	local_EntityQueueCS.Enter();

	local_EntityQueue.Clear();

	return local_EntityQueue;
}

static BOOL RuHelperInternal_UnlockEntityQueue()
{
	local_EntityQueueCS.Leave();

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuMatrix4x4 RuMatrixUtility_BuildOrientationTransform(const CRuVector3 &defaultForward, const CRuVector3 &curForward, const CRuVector3 &curUp, REAL curScale)
{
	CRuMatrix4x4 scaleTransform;
	CRuMatrix4x4 objToCanonicalTransform;
	CRuMatrix4x4 canonicalToOrientationTransform;
	CRuMatrix4x4 basisChangeTransform;

	CRuVector3 defaultRight;
	CRuVector3 defaultUp(0.0f, 1.0f, 0.0f);

	CRuVector3 entityRight;

	// Do nothing if any of the orientation vectors are invalid
	if(curForward.SquaredMagnitude() < ruEPSILON || defaultForward.SquaredMagnitude() < ruEPSILON || curUp.SquaredMagnitude() < ruEPSILON)
	{
		return basisChangeTransform;
	}

	// Find angle between default forward and current forward
	REAL forwardAngleDiff;
	CalculateAngleDifferential(defaultForward, curForward, defaultUp, forwardAngleDiff);	

	// Build the scale transform
	scaleTransform.SetToScaling(curScale, curScale, curScale);

	// Build the basis to go from entity's object space to canonical space (forward = +X, up = +Y)
	defaultRight = CrossProduct(defaultUp, defaultForward);
	defaultRight.Normalize();
	objToCanonicalTransform.SetToInverseBasis(defaultRight, defaultUp, defaultForward);

	// Build the basis to go from canonical space to the orientation space
	entityRight = CrossProduct(curUp, defaultForward);
	if(entityRight.Magnitude() <= ruEPSILON)
	{
		CRuVector3 alternateForward;
		alternateForward = CrossProduct(defaultRight, curUp);
		alternateForward.Normalize();
		entityRight = CrossProduct(curUp, alternateForward);
		entityRight.Normalize();
		canonicalToOrientationTransform.SetToBasis(entityRight, curUp, alternateForward);
	}
	else
	{
		entityRight.Normalize();
		canonicalToOrientationTransform.SetToBasis(entityRight, curUp, defaultForward);
	}

	// Build transform to rotate about the curUp vector
	CRuQuaternion forwardOrientationQuat;
	forwardOrientationQuat.FromAngleAxis(curUp, forwardAngleDiff);

	// Combine basis transforms
	basisChangeTransform = scaleTransform * objToCanonicalTransform * canonicalToOrientationTransform * forwardOrientationQuat.GetMatrix();

	return basisChangeTransform;
}

// ************************************************************************************************************************************************************

CRuMatrix4x4 RuMatrixUtility_BuildConstrainedOrientationTransform(const CRuVector3 &defaultForward, const CRuVector3 &curForward, const CRuVector3 &curUp, REAL curScale, REAL upRotRate, REAL &upRotAngle)
{
	CRuMatrix4x4 scaleTransform;
	CRuMatrix4x4 objToCanonicalTransform;
	CRuMatrix4x4 canonicalToOrientationTransform;
	CRuMatrix4x4 basisChangeTransform;

	CRuVector3 defaultRight;
	CRuVector3 defaultUp(0.0f, 1.0f, 0.0f);

	CRuVector3 entityRight;

	// Do nothing if any of the orientation vectors are invalid
	if(curForward.SquaredMagnitude() < ruEPSILON || defaultForward.SquaredMagnitude() < ruEPSILON || curUp.SquaredMagnitude() < ruEPSILON)
	{
		return basisChangeTransform;
	}

	if(curUp.m_y > 0.5f && curUp.m_y < 0.9f)
	{
		char moo[200];
		sprintf(moo, "up: %f, %f, %f, forward: %f, %f, %f\n", curUp.m_x, curUp.m_y, curUp.m_z, curForward.m_x, curForward.m_y, curForward.m_z);
//		OutputDebugString(moo);
	}



	// Build the scale transform
	scaleTransform.SetToScaling(curScale, curScale, curScale);

	// Build the basis to go from entity's object space to canonical space (forward = +X, up = +Y)
	defaultRight = CrossProduct(defaultUp, defaultForward);
	defaultRight.Normalize();
	objToCanonicalTransform.SetToInverseBasis(defaultRight, defaultUp, defaultForward);

	// Build the basis to go from canonical space to the orientation space
	CRuVector3 forwardVector = curForward;

	entityRight = CrossProduct(defaultUp, forwardVector);
	if(entityRight.Magnitude() <= ruEPSILON)
	{
		forwardVector = CrossProduct(defaultRight, defaultUp);
		forwardVector.Normalize();
		entityRight = CrossProduct(defaultUp, forwardVector);
	}

	entityRight.Normalize();
	canonicalToOrientationTransform.SetToBasis(entityRight, defaultUp, forwardVector);

	// Find angle between default up and current up
	REAL upAngleDiff;
	CalculateAngleDifferential(defaultUp, curUp, entityRight, upAngleDiff);

	REAL w1 = min(1.0f, upRotRate);
	REAL w0 = 1.0f - w1;
	upRotAngle = upRotAngle * w0 + upAngleDiff * w1;

	// Build transform to rotate about the right vector
	CRuQuaternion upOrientationQuat;
	upOrientationQuat.FromAngleAxis(entityRight, upRotAngle);

	// Combine basis transforms
	basisChangeTransform = scaleTransform * objToCanonicalTransform * canonicalToOrientationTransform * upOrientationQuat.GetMatrix();

	return basisChangeTransform;
}

// ************************************************************************************************************************************************************

/*!
	Finds the anchor point for an entity on the specified anchor object. Anchoring is the
	process where a character is affixed to a walking surface. This is done along the Y axis
	only. This function will take entity's current location and cast rays downward to locate
	the entity's anchored Y position.

	Assumption is made that the entity is the root frame. That is, the entity's local
	transform is identical to its world transform.

	@param entity Position of the entity to be anchored.
	@param anchorObject Entity to serve as the anchoring surface.
	@param entityMassOffset Entity's center of mass offset.
	@param testIncrement Increment by which to raise the test ray origin at each iteration.
	@param testRange Maximum test height.

	@return Y position of the anchoring point, or -FLT_MAX if no anchor point was found.
*/
REAL RuEntity_FindAnchorPoint(const CRuVector3 &entityPosition, IRuObject *anchorObject, REAL entityMassOffset, REAL testIncrement, REAL testRange)
{
	CRuCollisionQuery_Segment_Nearest colQuery;

	CRuVector3 p0 = entityPosition + CRuVector3(0.0f, entityMassOffset, 0.0f);
	CRuVector3 p1 = p0 - CRuVector3(0.0f, testRange, 0.0f);
	colQuery.m_colSegment.SetFromPoints(p0, p1);
	colQuery.m_exclusionMask = 0x00A00000;		// Exclude water and foliage surfaces

	// Perform collision query
	if(anchorObject->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		static_cast<CRuEntity *>(anchorObject)->QueryCollision(&colQuery);
	}
	else if(anchorObject->GetType().IsSubClassOf(IRuCollisionHierarchy::Type()))
	{
		static_cast<IRuCollisionHierarchy *>(anchorObject)->QueryCollision(&colQuery);
	}

	if(colQuery.m_positiveHit)
	{
		return colQuery.m_collisionPoint.m_y;
	}

	return -FLT_MAX;
}

// ************************************************************************************************************************************************************

REAL RuEntity_FindAnchorPoint(const CRuVector3 &entityPosition, CRuArrayList<IRuObject *> &anchorObjects, REAL entityMassOffset, REAL testIncrement, REAL testRange)
{
	REAL curAnchorPt = -FLT_MAX;

	for(INT32 i = 0; i < anchorObjects.Count(); ++i)
	{
		REAL anchorPt = RuEntity_FindAnchorPoint(entityPosition, anchorObjects[i], entityMassOffset, testIncrement, testRange);

		curAnchorPt = max(curAnchorPt, anchorPt);
	}

	return curAnchorPt;
}

// ************************************************************************************************************************************************************

/*!
	Calculates the movement vector for the specified entity given its forward vector by the specified delta.

	@param entity Entity to be moved.
	@param defaultForward Default forward orientation of the entity to be moved.
	@param forwardDelta Forward movement delta.
	@param rightDelta Right movement delta.
	@param moveVectorOut Output vector.

	@return TRUE if the entity was successfully moved, FALSE otherwise.
*/
BOOL RuEntity_CalculateMoveVector(CRuEntity *entity, const CRuVector3 &defaultForward, REAL forwardDelta, REAL rightDelta, CRuVector3 *moveVectorOut)
{
	// Calculate entity forward vector
	CRuVector3 entityForward = RuEntity_CalculateForwardVector(entity, defaultForward);

	// Forward/backward movement
	*moveVectorOut = entityForward * forwardDelta;

	// Right/left movement
	*moveVectorOut = *moveVectorOut + CrossProduct(CRuVector3(0.0f, 1.0f, 0.0f), entityForward) * rightDelta;

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Moves the specified entity along its forward and right vectors by the specified delta.

	@param entity Entity to be moved.
	@param defaultForward Default forward orientation of the entity to be moved.
	@param forwardDelta Forward movement delta.
	@param rightDelta Right movement delta.

	@return TRUE if the entity was successfully moved, FALSE otherwise.
*/
BOOL RuEntity_Move(CRuEntity *entity, const CRuVector3 &defaultForward, REAL forwardDelta, REAL rightDelta)
{
	// Calculate entity forward vector
	CRuVector3 entityForward = RuEntity_CalculateForwardVector(entity, defaultForward);

	// Forward/backward movement
	CRuVector3 moveVec = entityForward * forwardDelta;

	// Right/left movement
	moveVec = moveVec + CrossProduct(CRuVector3(0.0f, 1.0f, 0.0f), entityForward) * rightDelta;

	entity->Translate(moveVec);

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Entity movement sample.

	Movement Procedure:

	1. Abort if both of the movement deltas are zero.
	2. Save entity position as the entity's previous position.
	3. Check collision against the glide sphere. If collision occurs, push entity back along
	   the collision normal. If no collision occurs, do nothing.
	4. Move entity along movement deltas.
	5. Check for collision. If there is no collision, save new location as the entity's previous position.
	6. Anchor entity.
	7. Check for collision. If there is no collision, accept new location. If there is location,
	   restore location to the entity's previous position.

	@param entity Entity to be moved.
	@param anchorObject Entity to serve as the anchoring surface.
	@param defaultForward Default forward orientation of the entity to be moved.
	@param forwardDelta Forward movement delta.
	@param rightDelta Right movement delta.
	@param moveSpeed Movement speed of the entity in distance per second.
	@param elapsedTime Elapsed time since last entity move.

	@return TRUE if the entity was successfully moved, FALSE otherwise.
*/
BOOL RuEntity_MoveEx(CRuEntity *entity, IRuObject *anchorObject, const CRuVector3 &defaultForward, const CRuVector3 &entityForward, const CRuVector3 &moveDelta, REAL moveSpeed, REAL elapsedTime)
{
//	REAL absForwardDelta = fabs(forwardDelta);
//	REAL absRightDelta = fabs(rightDelta);

	CRuVector3 normalizedEntityForward = entityForward;
	normalizedEntityForward.y = 0;
	normalizedEntityForward.Normalize();

	// Save entity position
	CRuVector3 entityPosition = entity->GetTranslation();
	CRuVector3 oldEntityPosition = entity->GetTranslation();
/*
	// Check offset
	CRuVector3 offsetVector;
	if(RuEntity_CalculateCollideOffsetVector(CRuSphere(entity->GetTranslation() + CRuVector3(0.0f, 10.0f, 0.0f), 5.0f), anchorObject, &offsetVector))
	{
		REAL moveDistance = moveSpeed * elapsedTime;
		offsetVector = offsetVector * moveDistance;

		// Move entity in direction of offset
		entity->Translate(offsetVector);
	}
*/
	// Get entity movement FSM state
	INT32 movementFSMState = reinterpret_cast<INT32>(entity->GetTag(ruENTITYTAG_FSMSTATE_MOVEMENT));

	switch(movementFSMState)
	{
		case 0:		// Surface movement
			{
				// Save old entity position
				oldEntityPosition = entity->GetTranslation();

				// Move the entity
				RuEntity_Move(entity, defaultForward, moveDelta.m_z, moveDelta.m_x);

				// Translate by vertical movement
				entity->Translate(CRuVector3(0.0f, moveDelta.m_y, 0.0f));

				// Anchor entity
				entityPosition = entity->GetTranslation();

				REAL anchorPoint = RuEntity_FindAnchorPoint(entityPosition, anchorObject);
				REAL anchorOffset = (anchorPoint - entity->GetTranslation().y);

				if(anchorOffset > 0.0f)
				{
					anchorOffset = min(anchorOffset, moveSpeed * elapsedTime);
				}
				else
				{
					anchorOffset = max(anchorOffset, -moveSpeed * elapsedTime);
				}

				// Adjust entity vertical positioning
				entityPosition.y += anchorOffset;

				CRuVector3 velocity = entityPosition - oldEntityPosition;

				if(velocity.Magnitude() < 0.001f)
				{
					entityPosition = oldEntityPosition;
				}
				else
				{
					CRuVector3 adjustedPosition;

					CRuFusion_Sojourner slider(anchorObject);
					slider.Slide(oldEntityPosition + CRuVector3(0.0f, 9.0f, 0.0f), velocity, CRuVector3(4.0f, 8.5f, 4.0f), adjustedPosition);
					
					entityPosition = adjustedPosition - CRuVector3(0.0f, 9.0f, 0.0f);
				}

				entity->SetTranslation(entityPosition);

				// Orient entity
				RuEntity_Orient(entity, defaultForward, normalizedEntityForward, 1.0f);	// hack

				// Test to see if we are in water
				CRuCollisionQuery_Segment_Nearest waterQuery;
				waterQuery.m_colSegment = CRuSegment3(CRuVector3(entityPosition.x, entityPosition.y + 15.0f, entityPosition.z), CRuVector3(entityPosition.x, entityPosition.y + 17.0f, entityPosition.z));
				waterQuery.m_ignoreBackface = FALSE;

				if(anchorObject->GetType().IsSubClassOf(CRuEntity::Type()))
				{
					static_cast<CRuEntity *>(anchorObject)->QueryCollision(&waterQuery);
				}
				else if(anchorObject->GetType().IsSubClassOf(IRuCollisionHierarchy::Type()))
				{
					static_cast<IRuCollisionHierarchy *>(anchorObject)->QueryCollision(&waterQuery);
				}

				if(waterQuery.m_positiveHit && (waterQuery.m_collisionFlag & 0x00200000))
				{
					entity->SetTag(ruENTITYTAG_FSMSTATE_MOVEMENT, reinterpret_cast<POINTER>(1));
				}
			}

			break;

		case 1:		// Water surface movement
			{
				// Forward/backward movement
				CRuVector3 moveVec = entityForward * moveDelta.m_z;

				if(fabs(entityForward.y) < 0.5f)
				{
					moveVec = normalizedEntityForward * moveDelta.m_z;
				}

				// Right/left movement
				moveVec = moveVec + CrossProduct(CRuVector3(0.0f, 1.0f, 0.0f), entityForward) * moveDelta.m_x;

				entity->Translate(moveVec);

				// Orient the entity
				RuEntity_Orient(entity, defaultForward, normalizedEntityForward, 1.0f);	// hack

				// Test to see if we are diving
				{
					CRuCollisionQuery_Segment_Nearest waterQuery;
					waterQuery.m_colSegment = CRuSegment3(CRuVector3(entityPosition.x, entityPosition.y + 17.0f, entityPosition.z), CRuVector3(entityPosition.x, entityPosition.y + 20.0f, entityPosition.z));
					waterQuery.m_ignoreBackface = FALSE;

					if(anchorObject->GetType().IsSubClassOf(CRuEntity::Type()))
					{
						static_cast<CRuEntity *>(anchorObject)->QueryCollision(&waterQuery);
					}
					else if(anchorObject->GetType().IsSubClassOf(IRuCollisionHierarchy::Type()))
					{
						static_cast<IRuCollisionHierarchy *>(anchorObject)->QueryCollision(&waterQuery);
					}

					if(waterQuery.m_positiveHit && (waterQuery.m_collisionFlag & 0x00200000))
					{
						entity->SetTag(ruENTITYTAG_FSMSTATE_MOVEMENT, reinterpret_cast<POINTER>(2));
					}
				}

				// Test to see if we are exiting water
				{
					CRuCollisionQuery_Segment_Nearest waterQuery;
					waterQuery.m_colSegment = CRuSegment3(CRuVector3(entityPosition.x, entityPosition.y + 10.0f, entityPosition.z), CRuVector3(entityPosition.x, entityPosition.y + 15.0f, entityPosition.z));
					waterQuery.m_ignoreBackface = FALSE;

					if(anchorObject->GetType().IsSubClassOf(CRuEntity::Type()))
					{
						static_cast<CRuEntity *>(anchorObject)->QueryCollision(&waterQuery);
					}
					else if(anchorObject->GetType().IsSubClassOf(IRuCollisionHierarchy::Type()))
					{
						static_cast<IRuCollisionHierarchy *>(anchorObject)->QueryCollision(&waterQuery);
					}

					if(waterQuery.m_positiveHit && (waterQuery.m_collisionFlag & 0x00200000))
					{
						entity->SetTag(ruENTITYTAG_FSMSTATE_MOVEMENT, reinterpret_cast<POINTER>(0));
					}
				}
			}

			break;

		case 2:		// Diving
			{
				// Forward/backward movement
				CRuVector3 moveVec = entityForward * moveDelta.m_z;

				// Right/left movement
				moveVec = moveVec + CrossProduct(CRuVector3(0.0f, 1.0f, 0.0f), entityForward) * moveDelta.m_x;

				entity->Translate(moveVec);

				// Orient the entity
				RuEntity_Orient(entity, defaultForward, normalizedEntityForward, 1.0f);	// hack

				// Test to see if we are surfacing
				{
					CRuCollisionQuery_Segment_Nearest waterQuery;
					waterQuery.m_colSegment = CRuSegment3(CRuVector3(entityPosition.x, entityPosition.y + 15.0f, entityPosition.z), CRuVector3(entityPosition.x, entityPosition.y + 17.0f, entityPosition.z));
					waterQuery.m_ignoreBackface = FALSE;

					if(anchorObject->GetType().IsSubClassOf(CRuEntity::Type()))
					{
						static_cast<CRuEntity *>(anchorObject)->QueryCollision(&waterQuery);
					}
					else if(anchorObject->GetType().IsSubClassOf(IRuCollisionHierarchy::Type()))
					{
						static_cast<IRuCollisionHierarchy *>(anchorObject)->QueryCollision(&waterQuery);
					}

					if(waterQuery.m_positiveHit && (waterQuery.m_collisionFlag & 0x00200000))
					{
						entity->SetTag(ruENTITYTAG_FSMSTATE_MOVEMENT, reinterpret_cast<POINTER>(1));
					}
				}
			}

			break;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_Orient(CRuEntity *entity, const CRuVector3 &defaultForward, const CRuVector3 &entityForward, REAL entityScale)
{
	CRuVector3 defaultUp(0.0f, 1.0f, 0.0f);

	return RuEntity_Orient(entity, defaultForward, entityForward, defaultUp, entityScale);
}

// ************************************************************************************************************************************************************

/*!
	Orients an entity to the specified forward direction. +Y is always assumed to be the
	upward orientation.

	@param entity Entity to be oriented
	@param defaultForward Default forward orientation of the entity to be anchored.
	@param entityForward Current forward orientation of the entity to be anchored.

	@return TRUE if the entity was successfully oriented, FALSE otherwise.
*/
BOOL RuEntity_Orient(CRuEntity *entity, const CRuVector3 &defaultForward, const CRuVector3 &entityForward, const CRuVector3 &entityUp, REAL entityScale)
{	
	entity->SetLocal3x3Transform(RuMatrixUtility_BuildOrientationTransform(defaultForward, entityForward, entityUp, entityScale));

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_OrientConstrained(CRuEntity *entity, const CRuVector3 &defaultForward, const CRuVector3 &entityForward, const CRuVector3 &entityUp, REAL entityScale, REAL upRotRate, REAL &upRotAngle)
{
	entity->SetLocal3x3Transform(RuMatrixUtility_BuildConstrainedOrientationTransform(defaultForward, entityForward, entityUp, entityScale, upRotRate, upRotAngle));

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Calculates the forward vector for the specified entity.

	@param entity Entity to calculate forward vector for.
	@param defaultForward Default forward orientation of the entity.

	@return The entity's forward vector.
*/
CRuVector3 RuEntity_CalculateForwardVector(CRuEntity *entity, const CRuVector3 &defaultForward)
{
	// Calculate entity forward vector
	CRuVector3 entityForward;
	entity->GetLocalTransform().TransformVector(defaultForward, entityForward);

	return entityForward;
}

// ************************************************************************************************************************************************************

REAL RuEntity_FindRestingHeight(const CRuVector3 &entityPosition, IRuObject *anchorObject, REAL testLimit, REAL addHeight)
{
	REAL restingHeight = entityPosition.m_y;	

	CRuMatrix4x4 identityMatrix;

	CRuVector3 testPosition = entityPosition;
	testPosition.m_y += addHeight;

	// Setup up & down queries
	CRuCollisionQuery_Segment_Nearest upSegQuery;
	upSegQuery.m_worldTransform = &identityMatrix;
	upSegQuery.m_invWorldTransform = &identityMatrix;
	upSegQuery.m_exclusionMask = 0x00800000 | 0x00200000;		// Exclude foliage surfaces
	upSegQuery.m_ignoreBackface = FALSE;
	upSegQuery.m_colSegment = CRuSegment3(testPosition, testPosition + CRuVector3(0.0f, testLimit, 0.0f));

	CRuCollisionQuery_Segment_Nearest downSegQuery;
	downSegQuery.m_worldTransform = &identityMatrix;
	downSegQuery.m_invWorldTransform = &identityMatrix;
	upSegQuery.m_exclusionMask = 0x00800000 | 0x00200000;		// Exclude foliage surfaces
	downSegQuery.m_ignoreBackface = FALSE;
	downSegQuery.m_colSegment = CRuSegment3(testPosition, testPosition + CRuVector3(0.0f, -testLimit, 0.0f));

	// Query anchor object for hit location
	if(anchorObject->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		static_cast<CRuEntity *>(anchorObject)->QueryCollision(&upSegQuery);
		static_cast<CRuEntity *>(anchorObject)->QueryCollision(&downSegQuery);
	}
	else if(anchorObject->GetType().IsSubClassOf(IRuCollisionHierarchy::Type()))
	{
		static_cast<IRuCollisionHierarchy *>(anchorObject)->QueryCollision(&upSegQuery);
		static_cast<IRuCollisionHierarchy *>(anchorObject)->QueryCollision(&downSegQuery);
	}

	REAL compareFactor = 0.5f;
	REAL nearestUpT = upSegQuery.m_positiveHit ? upSegQuery.m_collisionT : FLT_MAX;
	REAL nearestDownT = downSegQuery.m_positiveHit ? downSegQuery.m_collisionT : FLT_MAX;

	if(nearestUpT * compareFactor < nearestDownT && nearestUpT < testLimit)
	{
		restingHeight = upSegQuery.m_collisionPoint.m_y;
	}
	else if(nearestDownT < FLT_MAX && nearestDownT < testLimit)
	{
		restingHeight = downSegQuery.m_collisionPoint.m_y;
	}

	return restingHeight;
}

// ************************************************************************************************************************************************************

CRuVector3 RuEntity_FindRestingNormal(const CRuVector3 &entityPosition, IRuObject *anchorObject)
{
	CRuVector3 restingNormal(0.0f, 1.0f, 0.0f);

	CRuMatrix4x4 identityMatrix;

	// Setup up & down queries
	CRuCollisionQuery_Segment_Nearest upSegQuery;
	upSegQuery.m_worldTransform = &identityMatrix;
	upSegQuery.m_invWorldTransform = &identityMatrix;
	upSegQuery.m_exclusionMask = 0x00800000;		// Exclude foliage surfaces
	upSegQuery.m_ignoreBackface = FALSE;
	upSegQuery.m_colSegment = CRuSegment3(entityPosition, entityPosition + CRuVector3(0.0f, 20.0f, 0.0f));

	CRuCollisionQuery_Segment_Nearest downSegQuery;
	downSegQuery.m_worldTransform = &identityMatrix;
	downSegQuery.m_invWorldTransform = &identityMatrix;
	downSegQuery.m_exclusionMask = 0x00800000;		// Exclude foliage surfaces
	downSegQuery.m_ignoreBackface = FALSE;
	downSegQuery.m_colSegment = CRuSegment3(entityPosition, entityPosition + CRuVector3(0.0f, -20.0f, 0.0f));

	// Query anchor object for hit location
	if(anchorObject->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		static_cast<CRuEntity *>(anchorObject)->QueryCollision(&upSegQuery);
		static_cast<CRuEntity *>(anchorObject)->QueryCollision(&downSegQuery);
	}
	else if(anchorObject->GetType().IsSubClassOf(IRuCollisionHierarchy::Type()))
	{
		static_cast<IRuCollisionHierarchy *>(anchorObject)->QueryCollision(&upSegQuery);
		static_cast<IRuCollisionHierarchy *>(anchorObject)->QueryCollision(&downSegQuery);
	}

	REAL compareFactor = 0.5f;
	REAL nearestUpT = upSegQuery.m_positiveHit ? upSegQuery.m_collisionT : FLT_MAX;
	REAL nearestDownT = downSegQuery.m_positiveHit ? downSegQuery.m_collisionT : FLT_MAX;

	if(nearestUpT * compareFactor < nearestDownT && nearestUpT < 4.0f)
	{
		restingNormal = upSegQuery.m_collisionNormal;
	}
	else if(nearestDownT < FLT_MAX && nearestDownT < 4.0f)
	{
		restingNormal = downSegQuery.m_collisionNormal;
	}

	if(restingNormal.Magnitude() > ruEPSILON)
	{
		restingNormal.Normalize();
	}
	else
	{
		restingNormal.Set(0.0f, 1.0f, 0.0f);
	}

	return restingNormal;
}

// ************************************************************************************************************************************************************

/*!
	Calculates the highest morph target count among all of the entity's children.

	@param entity Entity to examine.

	@return The highest morph target count.
*/
INT32 RuEntity_GetMaxMorphTargetCount(CRuEntity *entity)
{
	INT32 maxMTCount = 0;

	if(entity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
	{
		// Iterate through all meshes
		for(INT32 i = 0, numMeshes = static_cast<IRuEntity_Renderable *>(entity)->GetNumMeshes(); i < numMeshes; ++i)
		{
			IRuMesh *mesh;
			IRuMaterial *material;
			
			if(static_cast<IRuEntity_Renderable *>(entity)->GetMesh(i, &mesh, &material))
			{
				maxMTCount = max(maxMTCount, mesh->GetNumMorphTargets());
			}
		}

		return maxMTCount;
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		maxMTCount = max(maxMTCount, RuEntity_GetMaxMorphTargetCount(curChild));

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return maxMTCount;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_PickTest(CRuEntity *entity, CRuCamera *camera, IRuSwapChain *swapChain, INT32 x, INT32 y, BOOL allowRoughPicking, REAL *pickDistanceOut)
{
	// Get the target position for the pick ray in world coordinates
	CRuVector3 homPt, viewPt, worldPt;
	camera->Coord_ScreenToHomogeneous(x, y, swapChain->GetWidth(), swapChain->GetHeight(), homPt);
	camera->Coord_HomogeneousToView(homPt, viewPt);
	camera->Coord_ViewToWorld(viewPt, worldPt);

	// Build collision query
	CRuCollisionQuery_Ray_Pick rayQuery;
	rayQuery.m_colRay = CRuRay3(camera->GetCameraPosition(), worldPt - camera->GetCameraPosition(), TRUE);
	rayQuery.m_allowRoughPicking = allowRoughPicking;

	// Make collision query
	entity->QueryCollision(&rayQuery);

	if(pickDistanceOut)
	{
		*pickDistanceOut = rayQuery.m_colDistance;
	}

	return rayQuery.m_positiveHit;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_Traverse_BFS(CRuEntity *entity, CRuQueue<CRuEntity *> &bfsQueueOut)
{
	CRuQueue<CRuEntity *> workQueue(256);
	workQueue.Queue(entity);

	while(!workQueue.IsEmpty())
	{
		// Dequeue next entity
		CRuEntity *curEntity = workQueue.Dequeue();

		CRuEntity *curChild = curEntity->GetFirstChild();
		while(curChild)
		{
			// Push onto work queue
			workQueue.Queue(curChild);

			// Advance to next sibling
			curChild = curChild->GetNextSibling();
		}

		// Queue entity
		bfsQueueOut.Queue(curEntity);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Attaches the child entity to the parent entity at the first link matching linkName.
	This function will search the parent entity for available frame hierarchies, and
	use the first link it finds.

	@param parentEntity Entity to attach to.
	@param childEntity Entity to be attached.
	@param linkName Name of the link to attach to.

	@return TRUE if the entity was successfully attached, FALSE otherwise.
*/
BOOL RuEntity_Attach(CRuEntity *parentEntity, CRuEntity *childEntity, const char *linkName)
{
	if(parentEntity->GetType().IsSubClassOf(CRuFrameHierarchy::Type()))
	{
		CRuEntity *linkFrame = static_cast<CRuFrameHierarchy *>(parentEntity)->GetLinkFrameByName(linkName);

		// If a matching link was found, attach the child entity to it
		if(linkFrame)
		{
			linkFrame->AddChild(childEntity);

			return TRUE;
		}
	}

	// If the current object is matches the search specification, use it
	if(parentEntity->GetType().IsSubClassOf(CRuLinkFrame::Type()))
	{
		if(stricmp(static_cast<CRuLinkFrame *>(parentEntity)->GetLinkName(), linkName) == 0)
		{
			static_cast<CRuLinkFrame *>(parentEntity)->AddEntity(childEntity);

			return TRUE;
		}
	}

	CRuEntity *curChild = parentEntity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		if(RuEntity_Attach(curChild, childEntity, linkName))
		{
			return TRUE;
		}

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

/*!
	Attaches the child entity to the parent entity at the first link matching linkName.
	This function will search the parent entity for available frame hierarchies, and
	use the first link it finds. This version does so in breadth-first fashion

	@param parentEntity Entity to attach to.
	@param childEntity Entity to be attached.
	@param linkName Name of the link to attach to.

	@return TRUE if the entity was successfully attached, FALSE otherwise.
*/
BOOL RuEntity_Attach_BFS(CRuEntity *parentEntity, CRuEntity *childEntity, const char *linkName)
{
	// Build a BFS queue for the entity hierarchy
	CRuQueue<CRuEntity *> bfsQueue(256);
	RuEntity_Traverse_BFS(parentEntity, bfsQueue);

	while(!bfsQueue.IsEmpty())
	{
		// Dequeue
		CRuEntity *curEntity = bfsQueue.Dequeue();

		if(curEntity->GetType().IsSubClassOf(CRuFrameHierarchy::Type()))
		{
			CRuEntity *linkFrame = static_cast<CRuFrameHierarchy *>(curEntity)->GetLinkFrameByName(linkName);

			// If a matching link was found, attach the child entity to it
			if(linkFrame)
			{
				linkFrame->AddChild(childEntity);

				return TRUE;
			}
		}

		// If the current object is matches the search specification, use it
		if(curEntity->GetType().IsSubClassOf(CRuLinkFrame::Type()))
		{
			if(stricmp(static_cast<CRuLinkFrame *>(curEntity)->GetLinkName(), linkName) == 0)
			{
				static_cast<CRuLinkFrame *>(curEntity)->AddEntity(childEntity);

				return TRUE;
			}
		}

		// If the current object is matches the search specification, use it
		if(curEntity->GetType().IsSubClassOf(CRuFrame::Type()))
		{
			if(stricmp(static_cast<CRuFrame *>(curEntity)->GetObjectName(), linkName) == 0)
			{
				static_cast<CRuFrame *>(curEntity)->AddEntity(childEntity);

				return TRUE;
			}
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

/*!
	Detaches the child entity from the parent entity at the first link matching linkName.
	This function will search the parent entity for available frame hierarchies, and
	use the first link it finds.

	@param parentEntity Entity to detach from.
	@param childEntity Entity to be detached.
	@param linkName Name of the link to detach from.

	@return TRUE if the entity was successfully detached, FALSE otherwise.
*/
BOOL RuEntity_Detach(CRuEntity *parentEntity, CRuEntity *childEntity, const char *linkName)
{
	if(parentEntity->GetType().IsSubClassOf(CRuFrameHierarchy::Type()))
	{
		CRuEntity *linkFrame = static_cast<CRuFrameHierarchy *>(parentEntity)->GetLinkFrameByName(linkName);

		// If a matching link was found, look for the specified child entity
		if(linkFrame)
		{
			if(linkFrame->RemoveChild(childEntity))
			{
				return TRUE;
			}
		}
	}

	// If the current object is matches the search specification, use it
	if(parentEntity->GetType().IsSubClassOf(CRuLinkFrame::Type()))
	{
		if(stricmp(static_cast<CRuLinkFrame *>(parentEntity)->GetLinkName(), linkName) == 0)
		{
			for(INT32 i = 0, numEntities = static_cast<CRuLinkFrame *>(parentEntity)->GetNumEntities(); i < numEntities; ++i)
			{
				if(static_cast<CRuLinkFrame *>(parentEntity)->GetEntity(i) == childEntity)
				{
					static_cast<CRuLinkFrame *>(parentEntity)->RemoveEntityAt(i);

					return TRUE;
				}
			}
		}
	}

	CRuEntity *curChild = parentEntity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		if(RuEntity_Detach(curChild, childEntity, linkName))
		{
			return TRUE;
		}

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

/*!
	Detaches all child entities at the link name of the parent entity.
	This function will search the parent entity for available frame hierarchies, and
	use the first link it finds.

	@param parentEntity Entity that holds the link.
	@param linkName Name of the link to detach from.

	@return TRUE if the link was successfully cleared, FALSE otherwise.
*/
BOOL RuEntity_DetachAll(CRuEntity *parentEntity, const char *linkName)
{
	if(parentEntity->GetType().IsSubClassOf(CRuFrameHierarchy::Type()))
	{
		CRuEntity *linkFrame = static_cast<CRuFrameHierarchy *>(parentEntity)->GetLinkFrameByName(linkName);

		// If a matching link was found, detach all child entities linked to it
		if(linkFrame)
		{
			return linkFrame->RemoveAllChildren();
		}
	}

	// If the current object is matches the search specification, use it
	if(parentEntity->GetType().IsSubClassOf(CRuLinkFrame::Type()))
	{
		if(stricmp(static_cast<CRuLinkFrame *>(parentEntity)->GetLinkName(), linkName) == 0)
		{
			return static_cast<CRuLinkFrame *>(parentEntity)->ClearEntities();
		}
	}

	CRuEntity *curChild = parentEntity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		if(RuEntity_DetachAll(curChild, linkName))
		{
			return TRUE;
		}

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

/*!
	Finds the specified link frame attached to the entity.

	@param entity Entity that holds the link.
	@param linkName Name of the link to find.

	@return Pointer to link frame if it was found. NULL otherwise.
*/
CRuEntity *RuEntity_FindLinkFrame(CRuEntity *entity, const char *linkName, BOOL forceLinkFrameUpdate)
{
	if(entity->GetType().IsSubClassOf(CRuFrameHierarchy::Type()))
	{
		CRuEntity *linkFrame = static_cast<CRuFrameHierarchy *>(entity)->GetLinkFrameByName(linkName);

		// If a matching link was found, return it
		if(linkFrame)
		{
			return linkFrame;
		}
	}

	// If the current object is matches the search specification, return it
	if(entity->GetType().IsSubClassOf(CRuLinkFrame::Type()))
	{
		if(stricmp(static_cast<CRuLinkFrame *>(entity)->GetLinkName(), linkName) == 0)
		{
			if(forceLinkFrameUpdate && entity->GetParent() && entity->GetParent()->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
			{
				static_cast<CRuHierarchy_GR2 *>(entity->GetParent())->ForceLinkFrameUpdate(entity);
			}

			return entity;
		}
	}

	if(entity->GetType().IsSubClassOf(CRuFrame::Type()))
	{
		if(stricmp(static_cast<CRuFrame *>(entity)->GetObjectName(), linkName) == 0)
		{
			return entity;
		}
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		CRuEntity *linkFrame = RuEntity_FindLinkFrame(curChild, linkName, forceLinkFrameUpdate);

		if(linkFrame)
		{
			return linkFrame;
		}

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return NULL;
}

// ************************************************************************************************************************************************************

void RuEntity_EnumerateLinkFrames(CRuEntity *entity, CRuArrayList<CRuLinkFrame *> &linkFramesOut)
{
	// If current entity is a link frame, add it to output list
	if(entity->GetType().IsSubClassOf(CRuLinkFrame::Type()))
	{
		linkFramesOut.Add(static_cast<CRuLinkFrame *>(entity));
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuEntity_EnumerateLinkFrames(curChild, linkFramesOut);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}
}

// ************************************************************************************************************************************************************

void RuEntity_EnumerateFrames(CRuEntity *entity, CRuArrayList<CRuFrame *> &framesOut)
{
	// If current entity is a frame, add it to output list
	if(entity->GetType().IsSubClassOf(CRuFrame::Type()))
	{
		framesOut.Add(static_cast<CRuFrame *>(entity));
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuEntity_EnumerateFrames(curChild, framesOut);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}
}

// ************************************************************************************************************************************************************

void RuEntity_EnumerateEntities(CRuEntity *entity, const CRuRTTI &entityType, BOOL exactTypeMatch, CRuArrayList<CRuEntity *> &entitiesOut)
{
	// If current entity is an entity of the specified type, add it to output list
	if(exactTypeMatch)
	{
		if(entity->GetType() == entityType)
		{
			entitiesOut.Add(entity);
		}
	}
	else
	{
		if(entity->GetType().IsSubClassOf(entityType))
		{
			entitiesOut.Add(entity);
		}
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuEntity_EnumerateEntities(curChild, entityType, exactTypeMatch, entitiesOut);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}
}

// ************************************************************************************************************************************************************

BOOL RuEntity_UseParentLighting(CRuEntity *entity, BOOL useParentLighting)
{
	// Debug assertion
	assert(entity);

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Set specified option on the entity
		entity->SetOption(ruENTITYOPTION_USEPARENTLIGHTING, useParentLighting);

		// Recurse to child entities
		RuEntity_UseParentLighting(curChild, useParentLighting);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_SetOption(CRuEntity *entity, RuEntityOption option, INT32 value)
{
	// Debug assertion
	assert(entity);

	// Set specified option on the entity
	entity->SetOption(option, value);

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuEntity_SetOption(curChild, option, value);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_SetOptionWithTypeFilter(CRuEntity *entity, RuEntityOption option, INT32 value, const CRuRTTI &objType)
{
	// Debug assertion
	assert(entity);

	// Set specified option on the entity
	if(entity->GetType().IsSubClassOf(objType))
	{
		entity->SetOption(option, value);
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuEntity_SetOptionWithTypeFilter(curChild, option, value, objType);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuModel *RuEntity_FindFirstModel(CRuEntity *entity)
{
	// Debug assertion
	assert(entity);

	// Initialize entity queue
	CRuQueue<CRuEntity *> &entityQueue = RuHelperInternal_LockEntityQueue();
	entityQueue.Queue(entity);

	// Perform breadth-first traversal of entity tree
	while(entityQueue.IsEmpty() == FALSE)
	{
		CRuEntity *curEntity = entityQueue.Dequeue();

		// Check if entity is a CRuModel
		if(curEntity->GetType().IsSubClassOf(CRuModel::Type()))
		{
			RuHelperInternal_UnlockEntityQueue();
			return static_cast<CRuModel *>(curEntity);
		}

		// Iterate through child entities
		CRuEntity *curChild = curEntity->GetFirstChild();
		while(curChild)
		{
			// Push child onto stack
			entityQueue.Queue(curChild);

			// Advance to next sibling
			curChild = curChild->GetNextSibling();
		}
	}

	RuHelperInternal_UnlockEntityQueue();
	return NULL;
}

// ************************************************************************************************************************************************************

CRuFrameHierarchy *RuEntity_FindFirstFrameHierarchy(CRuEntity *entity)
{
	// Debug assertion
	assert(entity);

	// Initialize entity queue
	CRuQueue<CRuEntity *> &entityQueue = RuHelperInternal_LockEntityQueue();
	entityQueue.Queue(entity);

	// Perform breadth-first traversal of entity tree
	while(entityQueue.IsEmpty() == FALSE)
	{
		CRuEntity *curEntity = entityQueue.Dequeue();

		// Check if entity is a CRuFrameHierarchy
		if(curEntity->GetType().IsSubClassOf(CRuFrameHierarchy::Type()))
		{
			RuHelperInternal_UnlockEntityQueue();
			return static_cast<CRuFrameHierarchy *>(curEntity);
		}

		// Iterate through child entities
		CRuEntity *curChild = curEntity->GetFirstChild();
		while(curChild)
		{
			// Push child onto stack
			entityQueue.Queue(curChild);

			// Advance to next sibling
			curChild = curChild->GetNextSibling();
		}
	}

	RuHelperInternal_UnlockEntityQueue();
	return NULL;
}

// ************************************************************************************************************************************************************

CRuHierarchy_GR2 *RuEntity_FindFirstHierarchy_GR2(CRuEntity *entity)
{
	// Debug assertion
	assert(entity);

	// Initialize entity queue
	CRuQueue<CRuEntity *> &entityQueue = RuHelperInternal_LockEntityQueue();
	entityQueue.Queue(entity);

	// Perform breadth-first traversal of entity tree
	while(entityQueue.IsEmpty() == FALSE)
	{
		CRuEntity *curEntity = entityQueue.Dequeue();

		// Check if entity is a CRuHierarchy_GR2
		if(curEntity->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
		{
			RuHelperInternal_UnlockEntityQueue();
			return static_cast<CRuHierarchy_GR2 *>(curEntity);
		}

		// Iterate through child entities
		CRuEntity *curChild = curEntity->GetFirstChild();
		while(curChild)
		{
			// Push child onto stack
			entityQueue.Queue(curChild);

			// Advance to next sibling
			curChild = curChild->GetNextSibling();
		}
	}

	RuHelperInternal_UnlockEntityQueue();
	return NULL;
}

// ************************************************************************************************************************************************************

CRuACTEntity *RuEntity_FindFirstACTEntity(CRuEntity *entity)
{
	// Debug assertion
	assert(entity);

	// Initialize entity queue
	CRuQueue<CRuEntity *> &entityQueue = RuHelperInternal_LockEntityQueue();
	entityQueue.Queue(entity);

	// Perform breadth-first traversal of entity tree
	while(entityQueue.IsEmpty() == FALSE)
	{
		CRuEntity *curEntity = entityQueue.Dequeue();

		// Check if entity is a CRuACTEntity
		if(curEntity->GetType().IsSubClassOf(CRuACTEntity::Type()))
		{
			RuHelperInternal_UnlockEntityQueue();
			return static_cast<CRuACTEntity *>(curEntity);
		}

		// Iterate through child entities
		CRuEntity *curChild = curEntity->GetFirstChild();
		while(curChild)
		{
			// Push child onto stack
			entityQueue.Queue(curChild);

			// Advance to next sibling
			curChild = curChild->GetNextSibling();
		}
	}

	RuHelperInternal_UnlockEntityQueue();
	return NULL;
}

// ************************************************************************************************************************************************************

IRuPaperdoll *RuEntity_FindFirstPaperdoll(CRuEntity *entity)
{
	// Debug assertion
	assert(entity);

	// Initialize entity queue
	CRuQueue<CRuEntity *> &entityQueue = RuHelperInternal_LockEntityQueue();
	entityQueue.Queue(entity);

	// Perform breadth-first traversal of entity tree
	while(entityQueue.IsEmpty() == FALSE)
	{
		CRuEntity *curEntity = entityQueue.Dequeue();

		// Check if entity is a paperdoll
		if(curEntity->GetType().IsSubClassOf(IRuPaperdoll::Type()))
		{
			RuHelperInternal_UnlockEntityQueue();
			return static_cast<IRuPaperdoll *>(curEntity);
		}

		// Iterate through child entities
		CRuEntity *curChild = curEntity->GetFirstChild();
		while(curChild)
		{
			// Push child onto stack
			entityQueue.Queue(curChild);

			// Advance to next sibling
			curChild = curChild->GetNextSibling();
		}
	}

	RuHelperInternal_UnlockEntityQueue();
	return NULL;
}

// ************************************************************************************************************************************************************

/*!
	Finds the first skinned model in the specified entity tree. A skinned model
	is defined as a skinnable model parented by a hierarchy. This function will
	perform a breadth-first search for the first hierarchy, then perform another
	breadth-first search for the first skinnable model under that hierarchy.

	@param entity Root entity to search from.

	@return Pointer to the found skinned model.
*/
CRuEntity *RuEntity_FindFirstSkinnedModel(CRuEntity *entity)
{
	CRuEntity *hierarchy = RuEntity_FindFirstHierarchy_GR2(entity);

	if(hierarchy == NULL)
	{
		hierarchy = RuEntity_FindFirstFrameHierarchy(entity);
	}

	if(hierarchy)
	{
		// NOTE: Prolly should check whether the model is actually skinnable...
		CRuEntity *skinnedModel = RuEntity_FindFirstPaperdoll(hierarchy);

		if(skinnedModel == NULL)
		{
			skinnedModel = RuEntity_FindFirstModel(hierarchy);
		}

		return skinnedModel;
	}

	return NULL;
}

// ************************************************************************************************************************************************************

CRuContainer_Portal *RuEntity_FindFirstPortalContainer(CRuEntity *entity)
{
	// Debug assertion
	assert(entity);

	// Initialize entity queue
	CRuQueue<CRuEntity *> &entityQueue = RuHelperInternal_LockEntityQueue();
	entityQueue.Queue(entity);

	// Perform breadth-first traversal of entity tree
	while(entityQueue.IsEmpty() == FALSE)
	{
		CRuEntity *curEntity = entityQueue.Dequeue();

		// Check if entity is a CRuContainer_Portal
		if(curEntity->GetType().IsSubClassOf(CRuContainer_Portal::Type()))
		{
			RuHelperInternal_UnlockEntityQueue();
			return static_cast<CRuContainer_Portal *>(curEntity);
		}

		// Iterate through child entities
		CRuEntity *curChild = curEntity->GetFirstChild();
		while(curChild)
		{
			// Push child onto stack
			entityQueue.Queue(curChild);

			// Advance to next sibling
			curChild = curChild->GetNextSibling();
		}
	}

	RuHelperInternal_UnlockEntityQueue();
	return NULL;
}

// ************************************************************************************************************************************************************

CRuACTEntity *RuEntity_FindParentACTEntity(CRuEntity *entity)
{
	// Debug assertion
	assert(entity);

	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		return static_cast<CRuACTEntity *>(entity);
	}

	if(entity->GetParent())
	{
		return RuEntity_FindParentACTEntity(entity->GetParent());
	}

	return NULL;
}

// ************************************************************************************************************************************************************

CRuHierarchy_GR2 *RuEntity_FindParentHierarchy_GR2(CRuEntity *entity)
{
	// Debug assertion
	assert(entity);

	if(entity->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
	{
		return static_cast<CRuHierarchy_GR2 *>(entity);
	}

	if(entity->GetParent())
	{
		return RuEntity_FindParentHierarchy_GR2(entity->GetParent());
	}

	return NULL;
}

// ************************************************************************************************************************************************************

CRuEntity *RuEntity_FindNearestEntityByName(CRuEntity *entity, const char *name, CRuEntity *excludedEntity)
{
	// Debug assertion
	assert(entity);

	if(entity != excludedEntity)
	{
		// If the current entity matches, return immediately
		if(stricmp(entity->GetObjectName(), name) == 0)
		{
			return entity;
		}

		// Search through descendants
		CRuEntity *curChild = entity->GetFirstChild();
		while(curChild)
		{
			CRuEntity *result = NULL;
			if(result = RuEntity_FindNearestEntityByName(curChild, name, entity))
			{
				return result;
			}

			curChild = curChild->GetNextSibling();
		}

		// Search through parent
		CRuEntity *parent = entity->GetParent();
		if(parent && parent != excludedEntity)
		{
			return RuEntity_FindNearestEntityByName(entity->GetParent(), name, entity);
		}
	}

	return NULL;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_ReplaceInteriorEntity(CRuEntity *entity, CRuEntity *replacedEntity, CRuEntity *newEntity)
{
	// Debug assertion
	assert(entity);

	// Initialize entity queue
	CRuQueue<CRuEntity *> &entityQueue = RuHelperInternal_LockEntityQueue();
	entityQueue.Queue(entity);

	// Perform breadth-first traversal of entity tree
	while(entityQueue.IsEmpty() == FALSE)
	{
		CRuEntity *curEntity = entityQueue.Dequeue();

		if(curEntity == replacedEntity)
		{
			CRuEntity *parentEntity = curEntity->GetParent();

			if(parentEntity)
			{
				// Detach entity from parent
				curEntity->DetachFromParent();

				// Attach replacement entity
				parentEntity->AddChild(newEntity);

				RuHelperInternal_UnlockEntityQueue();
				return TRUE;
			}
		}

		// Iterate through child entities
		CRuEntity *curChild = curEntity->GetFirstChild();
		while(curChild)
		{
			// Push child onto stack
			entityQueue.Queue(curChild);

			// Advance to next sibling
			curChild = curChild->GetNextSibling();
		}
	}

	RuHelperInternal_UnlockEntityQueue();
	return FALSE;
}

// ************************************************************************************************************************************************************

CRuEntity *RuEntity_FindFirstEntityOfType(CRuEntity *entity, const CRuRTTI &objType, const CRuRTTI **allowableAncestorTypes, BOOL skipRoot)
{
	// Debug assertion
	assert(entity);

	// If the current entity matches, return immediately
	if(!skipRoot && entity->GetType().IsSubClassOf(objType))
	{
		return entity;
	}

	// Ensure the entity is one of the allowable ancestor types
	if(allowableAncestorTypes)
	{
		BOOL allowedAncestorType = FALSE;

		for(INT32 i = 0; allowableAncestorTypes[i] != NULL; ++i)
		{
			if(entity->GetType().IsSubClassOf(*allowableAncestorTypes[i]))
			{
				allowedAncestorType = TRUE;
				break;
			}
		}

		if(allowedAncestorType == FALSE)
		{
			return NULL;
		}
	}

	// Search through descendants
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		CRuEntity *result = NULL;
		if(result = RuEntity_FindFirstEntityOfType(curChild, objType, allowableAncestorTypes, FALSE))
		{
			return result;
		}

		curChild = curChild->GetNextSibling();
	}

	return NULL;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_RefreshHierarchySubNodeBounds(CRuEntity *entity)
{
	CRuEntity *skinnableModel = RuEntity_FindFirstSkinnedModel(entity);

	if(skinnableModel)
	{
		CRuHierarchy_GR2 *hierarchy = RuEntity_FindParentHierarchy_GR2(skinnableModel);

		if(hierarchy)
		{
			CRuArrayList<IRuMesh *> skinnableMeshes(16);
			if(RuEntity_EnumerateSkinnableMeshes(skinnableModel, skinnableMeshes))
			{
				if(skinnableMeshes.Count() > 0)
				{
					for(INT32 i = 0; i < skinnableMeshes.Count(); ++i)
					{
						hierarchy->SetSubNodeBoundsFromMesh(skinnableMeshes[i], i == 0 ? TRUE : FALSE);
					}

					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

/*!
	Transforms the specified vector from world space into the entity's local space.

	@param entity Entity to transform to.
	@param v0 Vector to transform.
	@param v0Out Output vector.

	@return TRUE if successful, FALSE otherwise.
*/
BOOL RuEntity_TransformVector_WorldToLocal(CRuEntity *entity, const CRuVector3 &v0, CRuVector3 &v0Out)
{
	// Debug assertion
	assert(entity);

	CRuMatrix4x4 inverseWorldTransform = entity->GetWorldTransform().GetInverse();

	inverseWorldTransform.TransformVector(v0, v0Out);

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_GetVisibleStatusFlag(CRuEntity *entity)
{
	CRuEntity *curEntity = entity;

	while(curEntity)
	{
		if(curEntity->GetStatusFlag(ruENTITYSTATUS_VISIBLE) == FALSE)
		{
			return FALSE;
		}

		curEntity = curEntity->GetParent();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_EnableCelHighlight(CRuEntity *entity, RUCOLOR highlightColor)
{
	// Debug assertion
	assert(entity);

	if(entity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
	{
		// Cast generic entity into a renderable entity
		IRuEntity_Renderable *renderableEntity = static_cast<IRuEntity_Renderable *>(entity);

		// Create a highlighter controller and set its target to be the renderable entity
		CRuController_Highlighter *highlighter = ruNEW CRuController_Highlighter();
		highlighter->UseCelHighlight(TRUE);
		highlighter->SetPulsePeriod(2.0f);
		highlighter->SetHighlightColor0(CRuColor4(highlightColor));
		highlighter->SetHighlightColor1(CRuColor4(highlightColor));
		highlighter->SetTarget(renderableEntity);

		// Add controller to entity as child and release local reference
		entity->AddChild(highlighter);
		ruSAFE_RELEASE(highlighter);
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuEntity_EnableCelHighlight(curChild, highlightColor);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_DisableCelHighlight(CRuEntity *entity)
{
	CRuEntity *curChild = entity->GetFirstChild();
	CRuEntity *nextChild = curChild ? curChild->GetNextSibling() : NULL;
	while(curChild)
	{
		// If the current child is a highlighter, remove it
		if(curChild->GetType() == CRuController_Highlighter::Type())
		{
			curChild->DetachFromParent();
		}
		else
		{
			// Recurse to child entities
			RuEntity_DisableCelHighlight(curChild);
		}

		// Advance to next child
		curChild = nextChild;
		nextChild = curChild ? curChild->GetNextSibling() : NULL;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_ClearShaderOverrides(CRuEntity *entity)
{
	// Debug assertion
	assert(entity);

	if(entity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
	{
		// Cast generic entity into a renderable entity
		IRuEntity_Renderable *renderableEntity = static_cast<IRuEntity_Renderable *>(entity);

		for(INT32 i = 0; i < renderableEntity->GetNumMeshes(); ++i)
		{
			IRuMesh *mesh = NULL;
			IRuMaterial *material = NULL;

			if(renderableEntity->GetMesh(i, &mesh, &material) && mesh && material)
			{
				if(material->GetShaderOverride())
				{
					material->SetShaderOverride(NULL);
				}
			}
		}
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuEntity_ClearShaderOverrides(curChild);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

char *RuEntity_EnumerateTypeStatistics(CRuEntity *entity)
{
	INT32 particleCount = 0;

	struct TypeContainer
	{
		const CRuRTTI*				m_type;
		CRuArrayList<CRuEntity *>	m_objects;
	};

	CRuArrayList<TypeContainer *> typeList;

	CRuStack<CRuEntity *> workStack;
	workStack.Push(entity);

	while(workStack.IsEmpty() == FALSE)
	{
		CRuEntity *curEntity = workStack.Pop();

		// Push entity's children onto work stack
		CRuEntity *curChild = curEntity->GetFirstChild();
		while(curChild)
		{
			workStack.Push(curChild);
			curChild = curChild->GetNextSibling();
		}

		// Get the current entity's type
		const CRuRTTI *entityType = &curEntity->GetType();

		if(entityType->IsSubClassOf(CRuEntity_Particle_Emitter::Type()))
		{
			particleCount += static_cast<CRuEntity_Particle_Emitter *>(curEntity)->GetParticleDescriptors()->Count();
		}

		// Find the type container for the current entity's type
		TypeContainer *typeContainer = NULL;

		for(INT32 i = 0; i < typeList.Count(); ++i)
		{
			if(typeList[i]->m_type == entityType)
			{
				typeContainer = typeList[i];
				break;
			}
		}

		if(typeContainer == NULL)
		{
			typeContainer = ruNEW TypeContainer();
			typeContainer->m_type = entityType;
			typeList.Add(typeContainer);
		}

		// Add current entity to the type container
		typeContainer->m_objects.Add(curEntity);
	}

	CRuString resultString("");
	char strBuffer[1024];

	for(INT32 i = 0; i < typeList.Count(); ++i)
	{
		sprintf(strBuffer, "%s - %d objects\n", typeList[i]->m_type->GetTypeName(), typeList[i]->m_objects.Count());
		resultString.Append(strBuffer);
	}

	sprintf(strBuffer, "---\n%d particles\n", particleCount);
	resultString.Append(strBuffer);

	char *outputString = ruNEW char [resultString.GetLength() + 1];
	strcpy(outputString, resultString);

	return outputString;
}

// ************************************************************************************************************************************************************

INT32 RuEntity_GetMeshFootprint(CRuEntity *entity)
{
	INT32 footprint = 0;

	// Debug assertion
	assert(entity);

	if(entity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
	{
		IRuEntity_Renderable *renderableEntity = static_cast<IRuEntity_Renderable *>(entity);

		for(INT32 i = 0; i < renderableEntity->GetNumMeshes(); ++i)
		{
			IRuMesh *mesh;
			IRuMaterial *material;
			if(renderableEntity->GetMesh(i, &mesh, &material))
			{
				INT32 meshFootprint = mesh->GetMemoryFootprint();
				if(meshFootprint > 0)
				{
					footprint += meshFootprint;
				}
			}
		}
	}

	// Recursively iterate through descendants
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		footprint += RuEntity_GetMeshFootprint(curChild);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return footprint;
}

// ************************************************************************************************************************************************************

INT32 RuEntity_GetExternalColStructFootprint(CRuEntity *entity)
{
	INT32 footprint = 0;

	// Debug assertion
	assert(entity);

	IRuCollisionObject *colObj = entity->GetCollisionObject();

	if(colObj)
	{
		footprint += colObj->GetMemoryFootprint();
	}

	// Recursively iterate through descendants
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		footprint += RuEntity_GetExternalColStructFootprint(curChild);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return footprint;
}

// ************************************************************************************************************************************************************

POINTER RuEntity_GetTagFromAnyDescendant(CRuEntity *entity, INT32 tagID)
{
	// Debug assertion
	assert(entity);

	// Attempt to get tag from current entity
	POINTER tagValue = entity->GetTag(tagID);
	if(tagValue)
	{
		return tagValue;
	}

	// Recursively iterate through descendants
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		tagValue = RuEntity_GetTagFromAnyDescendant(curChild, tagID);
		if(tagValue)
		{
			return tagValue;
		}

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return NULL;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_SetHierarchyIgnoreVisibilityCheck(CRuEntity *entity, BOOL ignoreVisibilityCheck)
{
	// Debug assertion
	assert(entity);

	// Set
	if(entity->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
	{
		static_cast<CRuHierarchy_GR2 *>(entity)->SetIgnoreVisibilityCheck(ignoreVisibilityCheck);
		return TRUE;
	}

	// Search through descendants
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		RuEntity_SetHierarchyIgnoreVisibilityCheck(curChild, ignoreVisibilityCheck);
		curChild = curChild->GetNextSibling();
	}

	return NULL;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_DisablePortalFunction(CRuEntity *entity, BOOL disablePortalFunction)
{
	// Debug assertion
	assert(entity);

	// Set
	if(entity->GetType().IsSubClassOf(CRuContainer_Portal::Type()))
	{
		static_cast<CRuContainer_Portal *>(entity)->DisablePortalFunction(disablePortalFunction);
		return TRUE;
	}

	// Search through descendants
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		RuEntity_DisablePortalFunction(curChild, disablePortalFunction);
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_EnableAlphaZ(CRuEntity *entity)
{
	// Debug assertion
	assert(entity);

	if(g_ruRendererPlugin_AlphaZ && entity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
	{
		g_ruRendererPlugin_AlphaZ->RegisterEntity(entity);
	}

	// Search through descendants
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		RuEntity_EnableAlphaZ(curChild);

		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuModel_SetTextureIsEmbedded(CRuModel *model, RuTextureChannel channel, BOOL isEmbedded)
{
	for(INT32 i = 0; i < model->GetNumMeshes(); ++i)
	{
		IRuMesh *mesh;
		IRuMaterial *material;

		if(model->GetMesh(i, &mesh, &material))
		{
			if(material && material->ChannelExists(channel))
			{
				material->SetTextureIsEmbedded(channel, TRUE);
			}
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuMatrix4x4 &RuHierarchy_GetSubNodeTransform(CRuFrameHierarchy *hierarchy, CRuAnimationInterpolator *interpolator, INT32 nodeIndex, CRuMatrix4x4 &transformOut)
{
	// Clear output transform
	transformOut.SetToIdentity();

	// Traverse up the node tree
	CRuMatrix4x4 localTransform;

	while(nodeIndex >= 0)
	{
		// Build the local transform
		CRuAnimKeyFrame &localKeyFrame = interpolator->GetKeyFrames()[nodeIndex];
		localTransform.SetToBoneSRT(localKeyFrame.m_scale, localKeyFrame.m_rotation, localKeyFrame.m_translation);

		// Concatenate into output transform
		transformOut = transformOut * localTransform;

		// Move up to the parent node
		nodeIndex = hierarchy->GetSubNodeParentIndex(nodeIndex);
	}

	return transformOut;
}

// ************************************************************************************************************************************************************

CRuMatrix4x4 &RuHierarchy_GetSubNodeParentTransform(CRuFrameHierarchy *hierarchy, CRuAnimationInterpolator *interpolator, INT32 nodeIndex, CRuMatrix4x4 &transformOut)
{
	// Clear output transform
	transformOut.SetToIdentity();

	// Traverse up the node tree
	CRuMatrix4x4 localTransform;

	// Move up to the parent node
	nodeIndex = hierarchy->GetSubNodeParentIndex(nodeIndex);

	while(nodeIndex >= 0)
	{
		// Build the local transform
		CRuAnimKeyFrame &localKeyFrame = interpolator->GetKeyFrames()[nodeIndex];
		localTransform.SetToBoneSRT(localKeyFrame.m_scale, localKeyFrame.m_rotation, localKeyFrame.m_translation);

		// Concatenate into output transform
		transformOut = transformOut * localTransform;

		// Move up to the parent node
		nodeIndex = hierarchy->GetSubNodeParentIndex(nodeIndex);
	}

	return transformOut;
}

// ************************************************************************************************************************************************************

/*!
	Performs a collision test between the sphere and the anchor object.

	@param testSphere The test collision sphere.
	@param anchorObject The anchoring object.

	@return TRUE if there is a collision, FALSE otherwise.
*/
BOOL RuCollision_TestSphere(const CRuSphere &testSphere, IRuObject *anchorObject)
{
	CRuCollisionQuery_Sphere_HitTest sphereQuery;
	sphereQuery.m_exclusionMask = 0x00200000;		// Exclude water surfaces
	sphereQuery.m_colSphere = testSphere;

	if(anchorObject->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		static_cast<CRuEntity *>(anchorObject)->QueryCollision(&sphereQuery);
	}
	else if(anchorObject->GetType().IsSubClassOf(IRuCollisionHierarchy::Type()))
	{
		static_cast<IRuCollisionHierarchy *>(anchorObject)->QueryCollision(&sphereQuery);
	}

	if(sphereQuery.m_positiveHit)
	{
		return TRUE;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

/*!
	Determines whether the specified ACT entity has the requested motion available. If firstACTOnly
	is true, this function will stop checking as soon as it encounters the first ACT entity. Otherwise,
	this function will return TRUE if any child entities has the requested motion available.

	@param entity Entity to act upon.
	@param motionName Motion name.
	@param firstACTOnly Check first ACT entity only?

	@return TRUE if the entity has the requested motion, FALSE otherwise.
*/
BOOL RuACT_HasMotion(CRuEntity *entity, const char *motionName, BOOL firstACTOnly)
{
	BOOL hasMotion = FALSE;

	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		INT32 motionID = static_cast<CRuACTEntity *>(entity)->GetMotionIDByName(motionName);

		if(motionID >= 0)
		{
			hasMotion = TRUE;
		}

		if(firstACTOnly)
		{
			return hasMotion;
		}
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		if(RuACT_HasMotion(curChild, motionName, firstACTOnly) != false)
		{
			hasMotion = TRUE;
		}

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return hasMotion;
}

// ************************************************************************************************************************************************************

/*!
	Determines whether the specified entity supports the specified motion channel. This function
	works by finding all frame hierarchies and checking them to make sure they have bones with
	acceptable group ID bitfields.

	@param entity Entity to be act upon.
	@param channel Channel to check for.

	@return TRUE if successful, FALSE otherwise.
*/
BOOL RuACT_SupportsMotionChannel(CRuEntity *entity, UINT32 channel)
{
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		if(RuACT_SupportsMotionChannel(curChild, channel) == FALSE)
		{
			return FALSE;
		}

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	if(entity->GetType().IsSubClassOf(CRuFrameHierarchy::Type()))
	{
		if(static_cast<CRuFrameHierarchy *>(entity)->SupportsMotionChannel(channel) == FALSE)
		{
			return FALSE;
		}
	}

	if(entity->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
	{
		if(static_cast<CRuHierarchy_GR2 *>(entity)->SupportsMotionChannel(channel) == FALSE)
		{
			return FALSE;
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuACT_PauseChannel(CRuEntity *entity, UINT32 channel)
{
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuACT_PauseChannel(curChild, channel);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		static_cast<CRuACTEntity *>(entity)->PauseChannel(channel);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuACT_ResumeChannel(CRuEntity *entity, UINT32 channel)
{
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuACT_ResumeChannel(curChild, channel);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		static_cast<CRuACTEntity *>(entity)->ResumeChannel(channel);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuACT_PlayPersistentMotion(CRuEntity *entity, BOOL forceReset)
{
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuACT_PlayPersistentMotion(curChild, forceReset);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		static_cast<CRuACTEntity *>(entity)->PlayPersistentMotion(forceReset);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Plays the specified motion on the entity. If forceReset is true, then the motion
	is restarted even if it is already playing.

	@param entity Entity to be act upon.
	@param motionName Name of the motion to play.
	@param blendTime Time to spend blending into the new motion
	@param forceReset Restart motion play if the specified motion ID is already playing?

	@return TRUE if successful, FALSE otherwise.
*/
BOOL RuACT_PlayMotion(CRuEntity *entity, const char *motionName, REAL blendTime, BOOL forceReset, CRuACT_ActionEvents *actionEventsOut, BOOL rootACT)
{
	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		if(!entity->GetOption(ruENTITYOPTION_NOMOTIONPROPAGATE) || rootACT)
		{
			INT32 motionID = static_cast<CRuACTEntity *>(entity)->GetMotionIDByName(motionName);

			static_cast<CRuACTEntity *>(entity)->PlayMotion(0xFFFFFFFF, motionID, blendTime, forceReset, 1.0f, 0.0f, 0.0f, 0.0f, actionEventsOut);
		}

		// The children ACTs are no longer root ACTs
		rootACT = FALSE;
	}

	// NOTE: If the container is an ACT, invoking play motion on the child entities first has the effect
	//       that any higher-level motion controllers owned by the parent ACT container has a chance
	//       to override any motions played by its control target.
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuACT_PlayMotion(curChild, motionName, blendTime, forceReset, actionEventsOut, rootACT);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Plays the specified motion on the entity. If forceReset is true, then the motion
	is restarted even if it is already playing.

	@param entity Entity to be act upon.
	@param motionID ID of the motion to play.
	@param blendTime Time to spend blending into the new motion
	@param forceReset Restart motion play if the specified motion ID is already playing?

	@return TRUE if successful, FALSE otherwise.
*/
BOOL RuACT_PlayMotion(CRuEntity *entity, INT32 motionID, REAL blendTime, BOOL forceReset, CRuACT_ActionEvents *actionEventsOut, BOOL rootACT)
{
	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		if(!entity->GetOption(ruENTITYOPTION_NOMOTIONPROPAGATE) || rootACT)
		{
			static_cast<CRuACTEntity *>(entity)->PlayMotion(0xFFFFFFFF, motionID, blendTime, forceReset, 1.0f, 0.0f, 0.0f, 0.0f, actionEventsOut);
		}
	
		// The children ACTs are no longer root ACTs
		rootACT = FALSE;
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuACT_PlayMotion(curChild, motionID, blendTime, forceReset, actionEventsOut, rootACT);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Plays the specified motion on the entity. If forceReset is true, then the motion
	is restarted even if it is already playing.

	@param entity Entity to be act upon.
	@param channel Channel to play the motion on.
	@param motionName Name of the motion to play.
	@param blendTime Time to spend blending into the new motion
	@param forceReset Restart motion play if the specified motion ID is already playing?
	@param channelWeight Blending weight of the channel.
	@param leadIn Lead-in time during which the motion is blended in
	@param leadOut Lead-out time during which the motion is blended out
	@param expectedMotionScale Expected motion scale for the motion, 0.0f to disable
	@param actionEventsOut Optional action event object to be filled with event triggers

	@return TRUE if successful, FALSE otherwise.
*/
BOOL RuACT_PlayMotionEx(CRuEntity *entity, UINT32 channel, const char *motionName, REAL blendTime, BOOL forceReset, REAL channelWeight, REAL leadIn, REAL leadOut, REAL expectedMotionScale, CRuACT_ActionEvents *actionEventsOut, BOOL rootACT)
{
	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		if(!entity->GetOption(ruENTITYOPTION_NOMOTIONPROPAGATE) || rootACT)
		{
			INT32 motionID = static_cast<CRuACTEntity *>(entity)->GetMotionIDByName(motionName);

			static_cast<CRuACTEntity *>(entity)->PlayMotion(channel, motionID, blendTime, forceReset, channelWeight, leadIn, leadOut, expectedMotionScale, actionEventsOut);
		}

		// The children ACTs are no longer root ACTs
		rootACT = FALSE;
	}

	// NOTE: If the container is an ACT, invoking play motion on the child entities first has the effect
	//       that any higher-level motion controllers owned by the parent ACT container has a chance
	//       to override any motions played by its control target.
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuACT_PlayMotionEx(curChild, channel, motionName, blendTime, forceReset, channelWeight, leadIn, leadOut, expectedMotionScale, actionEventsOut, rootACT);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Swaps motions playing on the two specified channels.

	@param entity Entity to be act upon.
	@param srcChannel First channel to swap.
	@param destChannel Second channel to swap.

	@return TRUE if motion has been successfully swapped, FALSE otherwise.
*/
BOOL RuACT_SwapMotion(CRuEntity *entity, UINT32 srcChannel, UINT32 destChannel, BOOL rootACT)
{
	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		if(!entity->GetOption(ruENTITYOPTION_NOMOTIONPROPAGATE) || rootACT)
		{
			static_cast<CRuACTEntity *>(entity)->SwapMotion(srcChannel, destChannel);
		}

		// The children ACTs are no longer root ACTs
		rootACT = FALSE;
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuACT_SwapMotion(curChild, srcChannel, destChannel, rootACT);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Clears any motions playing on the specified group mask.

	@param entity Entity to be act upon.
	@param channel channel to act upon.
	@param fadeDuration Duration of the motion fade. Motion fade is the period
	                    where there is no motion playing but the old motion is blended slowly to zero.

	@return TRUE if successful, FALSE otherwise.
*/
BOOL RuACT_ClearMotion(CRuEntity *entity, UINT32 channel, REAL fadeDuration, BOOL rootACT)
{
	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		if(!entity->GetOption(ruENTITYOPTION_NOMOTIONPROPAGATE) || rootACT)
		{
			static_cast<CRuACTEntity *>(entity)->ClearMotion(channel, fadeDuration);
		}

		// The children ACTs are no longer root ACTs
		rootACT = FALSE;
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuACT_ClearMotion(curChild, channel, fadeDuration, rootACT);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Determines whether the specified channel has completed its motion playback. This
	function will recursively iterate through all children, and return TRUE only if every
	child has completed its motion playback. If any of the motions being played back are
	looping, then this function will never return TRUE.

	@param entity Entity to be act upon.
	@param channel Channel to determine playback status for.

	@return TRUE if motion has finished played, FALSE otherwise.
*/
BOOL RuACT_IsMotionComplete(CRuEntity *entity, UINT32 channel, BOOL ignoreLoopingControllers)
{
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		if(RuACT_IsMotionComplete(curChild, channel, ignoreLoopingControllers) == FALSE)
		{
			return FALSE;
		}

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		return static_cast<CRuACTEntity *>(entity)->IsMotionComplete(channel, ignoreLoopingControllers);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!

	@param entity Entity to act upon.
	@param motionName Motion name

	@return Playback length of the specified motion, returns FLT_MAX if any sub motions are looping
*/
REAL RuACT_GetMotionLength(CRuEntity *entity, const char *motionName)
{
	REAL maxMotionLength = 0.0f;

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		maxMotionLength = max(maxMotionLength, RuACT_GetMotionLength(curChild, motionName));

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		maxMotionLength = max(maxMotionLength, static_cast<CRuACTEntity *>(entity)->GetMotionLength(motionName));
	}

	return maxMotionLength;
}

// ************************************************************************************************************************************************************

INT32 RuACT_GetMotionIndexByName(CRuEntity *entity, const char *motionName)
{
	CRuACTEntity *actEntity = RuEntity_FindFirstACTEntity(entity);

	if(actEntity)
	{
		return actEntity->GetMotionIndexByName(motionName);
	}

	return -1;
}

// ************************************************************************************************************************************************************

/*!
	Gets the active motion name for the specified channel. This function
	will return the active motion name for the first ACT entity it finds.

	@param entity Entity to act upon.
	@param channel Channel to get motion name from.

	@return Active motion name, or NULL if there is no active motion on the specified channel.
*/
const char *RuACT_GetActiveMotionName(CRuEntity *entity, UINT32 channel)
{
	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		return static_cast<CRuACTEntity *>(entity)->GetActiveMotionName(channel);
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		const char *name = RuACT_GetActiveMotionName(curChild, channel);

		if(name)
		{
			return name;
		}

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return NULL;
}

// ************************************************************************************************************************************************************

/*!
	Gets the active motion time for the specified channel. This function
	will return the active motion time for the first ACT entity it finds.

	@param entity Entity to act upon.
	@param channel Channel to get motion time from.

	@return Active motion time, or a negative value if there is no active motion on the specified channel.
*/
REAL RuACT_GetActiveMotionTime(CRuEntity *entity, UINT32 channel)
{
	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		return static_cast<CRuACTEntity *>(entity)->GetActiveMotionTime(channel);
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		REAL motionTime = RuACT_GetActiveMotionTime(curChild, channel);

		if(motionTime >= 0.0f)
		{
			return motionTime;
		}

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return -1.0f;
}

// ************************************************************************************************************************************************************

/*!
	Determines whether the ACT entity has the requested controller instanced in the currently playing
	motion on the specified channel. If firstACTOnly is true, this function will stop checking as
	soon as it encounters the first ACT entity. Otherwise, this function will return TRUE if any child
	entities has the requested controller instanced.

	@param entity Entity to act upon.
	@param channel Channel mask.
	@param controllerType Run-time type of the controller to check for.
	@param firstACTOnly Check first ACT entity only?

	@return TRUE if the entity has the requested controller, FALSE otherwise.
*/
BOOL RuACT_CheckForController(CRuEntity *entity, UINT32 channel, const CRuRTTI &controllerType, BOOL firstACTOnly)
{
	BOOL hasController = FALSE;

	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		hasController = static_cast<CRuACTEntity *>(entity)->CheckForController(channel, controllerType);

		if(firstACTOnly)
		{
			return hasController;
		}
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		if(RuACT_CheckForController(curChild, channel, controllerType, firstACTOnly) != false)
		{
			hasController = TRUE;
		}

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return hasController;
}

// ************************************************************************************************************************************************************

/*!
	Determines whether the ACT entity has the requested event instanced in the currently playing
	motion on the specified channel. If firstACTOnly is true, this function will stop checking as
	soon as it encounters the first ACT entity. Otherwise, this function will return TRUE if any child
	entities has the requested event instanced.

	@param entity Entity to act upon.
	@param channel Channel mask.
	@param eventType Event to check for.
	@param startTimeOut [OUT] Start time of the event.
	@param firstACTOnly Check first ACT entity only?

	@return TRUE if the entity has the requested event, FALSE otherwise.
*/
BOOL RuACT_CheckForEvent(CRuEntity *entity, UINT32 channel, RuEntityEvent eventType, REAL *startTimeOut, BOOL firstACTOnly)
{
	BOOL hasEvent = FALSE;

	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		hasEvent = static_cast<CRuACTEntity *>(entity)->CheckForEvent(channel, eventType, startTimeOut);

		if(firstACTOnly)
		{
			return hasEvent;
		}
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		if(RuACT_CheckForEvent(curChild, channel, eventType, startTimeOut, firstACTOnly) != false)
		{
			hasEvent = TRUE;
		}

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return hasEvent;
}
// ************************************************************************************************************************************************************

BOOL RuACT_EnumerateMotionNames(CRuEntity *entity, CRuArrayList<const char *> &motionNamesOut)
{
	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		CRuACTEntity *actEntity = static_cast<CRuACTEntity *>(entity);
		CRuACTTemplate *actTemplate = actEntity->GetTemplate();

		if(actTemplate)
		{
			for(INT32 i = 0; i < actTemplate->GetNumMotions(); ++i)
			{
				const char *motionName = actTemplate->GetMotion(i)->GetMotionName();
				BOOL motionExists = FALSE;

				// Verify that the motion does not exist in the output list
				for(INT32 j = 0; j < motionNamesOut.Count(); ++j)
				{
					if(stricmp(motionNamesOut[j], motionName) == 0)
					{
						motionExists = TRUE;
						break;
					}
				}

				// If motion does not exist in the output list, insert it into the list
				if(motionExists == FALSE)
				{
					motionNamesOut.Add(motionName);
				}
			}
		}
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuACT_EnumerateMotionNames(curChild, motionNamesOut);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Scans the specified entity for missing motions, and plugs the missing motions with
	replacement motion IDs. Motion list is specified by the command stream. The command
	stream is a simple list as follows:

		[motion to scan for] [replacement motion 1] [replacement motion 2] ... \n\r

	@param entity Entity to act upon.
	@param commandStream The command stream.
	@param firstACTOnly Check first ACT entity only?

	@return TRUE if the entity was plugged, FALSE otherwise.
*/
BOOL RuACT_PlugMissingMotions(CRuEntity *entity, const char *commandStream, BOOL firstACTOnly)
{
	BOOL returnValue = FALSE;

	if(entity->GetType().IsSubClassOf(CRuACTEntity::Type()))
	{
		// Parse command stream into multiple commands
		CRuString commandString = commandStream;
		CRuString **commands = commandString.Tokenize("\n\r");

		// Scan through commands
		for(INT32 i = 0; TRUE; ++i)
		{
			if(commands[i] == NULL)
			{
				break;
			}

			// Tokenize command
			CRuString **commandTokens = commands[i]->Tokenize(" \t");

			if(commandTokens[0])
			{
				if(static_cast<CRuACTEntity *>(entity)->GetMotionIDByName(static_cast<const char *>(*commandTokens[0])) < 0)
				{
					for(INT32 j = 1; TRUE; ++j)
					{
						if(commandTokens[j] == NULL)
						{
							break;
						}

						// Attempt to find the current replacement motion
						INT32 replacementMotionID = static_cast<CRuACTEntity *>(entity)->GetMotionIDByName(static_cast<const char *>(*commandTokens[j]));

						if(replacementMotionID >= 0)
						{
							// Create new entry in the ACT template
							static_cast<CRuACTEntity *>(entity)->GetTemplate()->AddPlaceHolderMotion(replacementMotionID, static_cast<const char *>(*commandTokens[0]));
							break;
						}
					}
				}
			}

			// Delete tokens
			for(INT32 j = 0; TRUE; ++j)
			{
				if(commandTokens[j] == NULL)
				{
					break;
				}

				delete commandTokens[j];
			}
			// Delete token array
			delete[] commandTokens;

			// Delete command
			delete commands[i];
		}

		// Delete command array
		delete[] commands;

		returnValue = TRUE;

		// If specified to scan only the first ACT, return immediately
		if(firstACTOnly)
		{
			return returnValue;
		}
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		if(RuACT_PlugMissingMotions(curChild, commandStream, firstACTOnly) != false)
		{
			returnValue = TRUE;
		}

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return returnValue;
}

// ************************************************************************************************************************************************************

BOOL RuPaperdoll_SetMaxMipLevelOverride(CRuEntity *entity, INT32 maxMipLevel)
{
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuPaperdoll_SetMaxMipLevelOverride(curChild, maxMipLevel);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	if(entity->GetType().IsSubClassOf(CRuPaperdoll::Type()))
	{
		static_cast<CRuPaperdoll *>(entity)->SetMaxMipLevelOverride(maxMipLevel);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Sets the specified color codes into any paperdolls contained by the entity. Generally,
	color code 0 is skin color and color code 1 is hair color.

	@param entity Entity to act upon.
	@param color0Code Color code 0.
	@param color1Code Color code 1.

	@return TRUE if successful, FALSE otherwise.
*/
BOOL RuPaperdoll_SetColorCodes(CRuEntity *entity, INT32 color0Code, INT32 color1Code)
{
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuPaperdoll_SetColorCodes(curChild, color0Code, color1Code);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	if(entity->GetType().IsSubClassOf(IRuPaperdoll::Type()))
	{
		// Relay call to paperdoll
//		static_cast<IRuPaperdoll *>(entity)->SetColorCodes(color0Code, color1Code);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Sets the specified component into the part for all paperdolls contained by the entity.

	@param entity Entity to act upon.
	@param partName Part name.
	@param componentName Component name.

	@return TRUE if successful, FALSE otherwise.
*/
BOOL RuPaperdoll_SetComponent(CRuEntity *entity, const char *partName, const char *componentName)
{
#ifdef KALYDO
	if (!entity)
		return FALSE;
#endif
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuPaperdoll_SetComponent(curChild, partName, componentName);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	if(entity->GetType().IsSubClassOf(IRuPaperdoll::Type()))
	{
		// Relay call to paperdoll
		return static_cast<IRuPaperdoll *>(entity)->SetComponent(partName, componentName);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuPaperdoll_SetComponentColors(CRuEntity *entity, const char *partName, BOOL layerActivated[4], RUCOLOR layerColor[4])
{
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuPaperdoll_SetComponentColors(curChild, partName, layerActivated, layerColor);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	if(entity->GetType().IsSubClassOf(IRuPaperdoll::Type()))
	{
		// Relay call to paperdoll
		return static_cast<IRuPaperdoll *>(entity)->SetComponentColors(partName, layerActivated, layerColor);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Builds all paperdolls contained by the specified entity.

	@param entity Entity to act upon.

	@return TRUE if successful, FALSE otherwise.
*/
BOOL RuPaperdoll_Build(CRuEntity *entity, BOOL allowDelayedLoad)
{
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuPaperdoll_Build(curChild, allowDelayedLoad);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	if(entity->GetType().IsSubClassOf(CRuPaperdoll::Type()))
	{
		// Relay call to paperdoll
		if(allowDelayedLoad)
		{
			static_cast<CRuPaperdoll *>(entity)->SetPaperdollBuildCompleteCallback(boost::bind(RuPaperdoll_BuildCompleteCallback, _1));

			return static_cast<CRuPaperdoll *>(entity)->BuildPaperdollEx();
		}
		else
		{
			return static_cast<CRuPaperdoll *>(entity)->BuildPaperdoll();
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuPaperdoll_IsBuilding(CRuEntity *entity)
{
	if(entity)
	{
		CRuEntity *curChild = entity->GetFirstChild();
		while(curChild)
		{
			// Recurse to child entities
			if(RuPaperdoll_IsBuilding(curChild))
			{
				return TRUE;
			}

			// Advance to next sibling
			curChild = curChild->GetNextSibling();
		}

		if(entity->GetType().IsSubClassOf(IRuPaperdoll::Type()))
		{
			return static_cast<IRuPaperdoll *>(entity)->IsPaperdollBuilding();
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

BOOL RuPaperdoll_BuildCompleteCallback(IRuPaperdoll *paperdoll)
{
	// Enable alpha-Z
	RuEntity_EnableAlphaZ(paperdoll->GetParent());

	// Refresh hierarchy bounds
	CRuEntity *curNode = paperdoll->GetParent();
	while(curNode)
	{
		if(curNode->GetType().IsSubClassOf(CRuHierarchy_GR2::Type()))
		{
			RuEntity_RefreshHierarchySubNodeBounds(curNode);
			return TRUE;
		}

		curNode = curNode->GetParent();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuMime_SetBoneScale(CRuFusion_Mime *mime, const char *scalerName, REAL scale)
{
	if(mime)
	{
		if(stricmp(scalerName, "Body") == 0)
		{
			mime->SetBoneScalerParameters("Body Scale", CRuVector3(scale, scale, scale));
		}
		else if(stricmp(scalerName, "Head") == 0)
		{
			mime->SetBoneScalerParameters("Head Scale", CRuVector3(scale, scale, scale));
		}
		else if(stricmp(scalerName, "Chest") == 0)
		{
			mime->SetBoneScalerParameters("Chest Scale", CRuVector3(1.0f, scale, scale));
		}
		else if(stricmp(scalerName, "Pelvis") == 0)
		{
			mime->SetBoneScalerParameters("Butt Scale", CRuVector3(1.0f, scale, scale));
		}
		else if(stricmp(scalerName, "Arms") == 0)
		{
			mime->SetBoneScalerParameters("Left Arm Scale", CRuVector3(1.0f, scale, scale));
			mime->SetBoneScalerParameters("Right Arm Scale", CRuVector3(1.0f, scale, scale));
		}
		else if(stricmp(scalerName, "Forearm") == 0)
		{
			mime->SetBoneScalerParameters("Left Forearm Scale", CRuVector3(1.0f, scale, scale));
			mime->SetBoneScalerParameters("Right Forearm Scale", CRuVector3(1.0f, scale, scale));
		}
		else if(stricmp(scalerName, "Hand") == 0)
		{
			mime->SetBoneScalerParameters("Left Hand Scale", CRuVector3(1.0f, scale, scale));
			mime->SetBoneScalerParameters("Right Hand Scale", CRuVector3(1.0f, scale, scale));
		}
		else if(stricmp(scalerName, "Legs") == 0)
		{
			mime->SetBoneScalerParameters("Left Leg Scale", CRuVector3(1.0f, scale, scale));
			mime->SetBoneScalerParameters("Right Leg Scale", CRuVector3(1.0f, scale, scale));
		}
		else if(stricmp(scalerName, "Calf") == 0)
		{
			mime->SetBoneScalerParameters("Left Calf Scale", CRuVector3(1.0f, scale, scale));
			mime->SetBoneScalerParameters("Right Calf Scale", CRuVector3(1.0f, scale, scale));
		}
		else if(stricmp(scalerName, "Foot") == 0)
		{
			mime->SetBoneScalerParameters("Left Foot Scale", CRuVector3(1.0f, scale, scale));
			mime->SetBoneScalerParameters("Right Foot Scale", CRuVector3(1.0f, scale, scale));
		}
		else if(stricmp(scalerName, "Breasts") == 0)
		{
			mime->SetBoneScalerParameters("Left Breast Scale", CRuVector3(scale, scale, scale));
			mime->SetBoneScalerParameters("Right Breast Scale", CRuVector3(scale, scale, scale));
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

BOOL RuPolyLine_SetTarget(CRuEntity *entity, CRuEntity *targetEntity)
{
	BOOL retVal = FALSE;

	// if the current entity is a poly line object, set its target
	if(entity->GetType().IsSubClassOf(CRuEntity_PolyLine::Type()))
	{
		static_cast<CRuEntity_PolyLine *>(entity)->SetTarget(targetEntity);
		retVal = TRUE;
	}

	// Iterate through all children
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		retVal = RuPolyLine_SetTarget(curChild, targetEntity) | retVal;

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return retVal;
}

// ************************************************************************************************************************************************************

BOOL RuPrtSys_SetUpdateInterval(CRuEntity *entity, REAL updateInterval)
{
	BOOL retVal = FALSE;

	// if the current entity is a sprite visualizer object, set its update interval
	if(entity->GetType().IsSubClassOf(CRuEntity_Particle_Visualizer_Sprite::Type()))
	{
		static_cast<CRuEntity_Particle_Visualizer_Sprite *>(entity)->SetUpdateInterval(updateInterval);
		retVal = TRUE;
	}

	// Iterate through all children
	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		retVal = RuPrtSys_SetUpdateInterval(curChild, updateInterval) | retVal;

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return retVal;
}

// ************************************************************************************************************************************************************

BOOL RuVizCon_SetupVizConState(CRuEntity *entity)
{
	assert(entity);

	RuPropertyType propertyValue;
	if(entity->GetProperty("vizCon:enable", propertyValue))
	{
		if(boost::get<BOOL>(propertyValue) != false)
		{
			RuEntity_SetOption(entity, ruENTITYOPTION_ENABLEDISTANCEFADE, TRUE);
		}
		else
		{
			RuEntity_SetOption(entity, ruENTITYOPTION_ENABLEDISTANCEFADE, FALSE);
		}
	}
	else
	{
		RuEntity_SetOption(entity, ruENTITYOPTION_ENABLEDISTANCEFADE, TRUE);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_SelectCamera(CRuEntity *entity, const char *cameraName, CRuController_Camera *cameraController)
{
	return TRUE;
}

// ************************************************************************************************************************************************************

const char *RuEnumConverter_ToString(RuTextureChannel texChannel)
{
	switch(texChannel)
	{
		case ruTEXCHANNEL_DIFFUSEMAP:		return "Diffuse Map";
		case ruTEXCHANNEL_DETAILMAP:		return "Detail Map";
		case ruTEXCHANNEL_LIGHTMAP:			return "Light Map";
		case ruTEXCHANNEL_DIFFUSECUBEMAP:	return "Diffuse Cube Map";
		case ruTEXCHANNEL_ENVIRONMENTMAP:	return "Environment Map";
		case ruTEXCHANNEL_CAUSTICMAP:		return "Caustic Map";
		case ruTEXCHANNEL_BUMPMAP:			return "Bump Map";
		case ruTEXCHANNEL_NORMALMAP:		return "Normal Map";
		case ruTEXCHANNEL_GLOSSMAP:			return "Gloss Map";
		case ruTEXCHANNEL_GLOWMAP:			return "Glow Map";
	}

	return "Unknown";
}

// ************************************************************************************************************************************************************

char *g_ruACT_DefaultMotionChart = "\
	unarmed_idle			1h_idle																		\n\
	unarmed_attack01		1h_attack01																	\n\
	unarmed_attack02		unarmed_attack01															\n\
	unarmed_attack03		unarmed_attack02															\n\
	unarmed_attack04		unarmed_attack03															\n\
	unarmed_attack05		unarmed_attack04															\n\
	unarmed_slow_attack		unarmed_attack01															\n\
	unarmed_slow_attack01	unarmed_attack01															\n\
	unarmed_sp01			unarmed_attack01															\n\
																										\n\
	1h_idle					unarmed_idle																\n\
	1h_parry				unarmed_parry																\n\
	1h_attack01				unarmed_attack01															\n\
	1h_attack02				1h_attack01			unarmed_attack02										\n\
	1h_attack03				1h_attack01			unarmed_attack03										\n\
	1h_attack04				1h_attack02			unarmed_attack04										\n\
	1h_attack05				1h_attack02			unarmed_attack05										\n\
	1h_slow_attack			1h_attack01																	\n\
	1h_slow_attack01		1h_attack01																	\n\
	1h_sp01					1h_attack01																	\n\
	1h_pierce01				1h_attack01																	\n\
	oh_attack01				1h_attack01																	\n\
	oh_pierce01				oh_attack01																	\n\
																										\n\
	2h_idle					1h_idle																		\n\
	2h_parry				1h_parry																	\n\
	2h_attack01				1h_attack01																	\n\
	2h_attack02				2h_attack01																	\n\
	2h_attack03				2h_attack02																	\n\
	2h_attack04				2h_attack03																	\n\
	2h_attack05				2h_attack04																	\n\
	2h_slow_attack			2h_attack01																	\n\
	2h_slow_attack01		2h_attack01																	\n\
	2h_sp01					2h_attack01																	\n\
																										\n\
	polearm_idle			unarmed_idle																\n\
	polearm_parry			unarmed_parry																\n\
	polearm_attack01		1h_attack01																	\n\
	polearm_attack02		polearm_attack01															\n\
	polearm_attack03		polearm_attack02															\n\
	polearm_attack04		polearm_attack03															\n\
	polearm_attack05		polearm_attack04															\n\
	polearm_slow_attack		polearm_attack01															\n\
	polearm_slow_attack01	polearm_attack01															\n\
	polearm_sp01			polearm_attack01															\n\
																										\n\
	dw_idle					1h_idle																		\n\
	dw_parry				1h_parry																	\n\
	dw_attack01				1h_attack01																	\n\
	dw_attack02				1h_attack02																	\n\
	dw_attack03				1h_attack03																	\n\
	dw_slow_attack			1h_attack01																	\n\
	dw_slow_attack01		1h_attack01																	\n\
	dw_sp01					dw_attack01																	\n\
																										\n\
	bow_idle				gun_idle																	\n\
	bow_parry				gun_parry																	\n\
	bow_attack01			gun_attack01																\n\
	bow_attack02			bow_attack01																\n\
	bow_attack03			bow_attack02																\n\
	bow_sp01				bow_attack01																\n\
																										\n\
	gun_idle				bow_idle																	\n\
	gun_parry				bow_parry																	\n\
	gun_attack01			bow_attack01																\n\
	gun_attack02			bow_attack02																\n\
	gun_attack03			bow_attack03																\n\
	gun_sp01				gun_attack01																\n\
																										\n\
	casting02				casting01																	\n\
	casting03				casting02			casting01												\n\
																										\n\
	buff01					casting01																	\n\
	cast01					casting02																	\n\
	continuous_buff			casting_delay																\n\
																										\n\
	buff_sp01				buff01																		\n\
	buff_sp02				buff01																		\n\
	buff_sp03				buff01																		\n\
																										\n\
	cast_sp01				cast01																		\n\
	cast_sp02				cast01																		\n\
	cast_sp03				cast01																		\n\
																										\n\
	stand_idle01			idle																		\n\
	stand_idle02			stand_idle01																\n\
	idle03					idle02																		\n\
	idle04					idle03				idle02													\n\
																										\n\
	activate_begin			death																		\n\
	activate_loop			dead																		\n\
	activate_end			reviving																	\n\
																										\n\
	charge					run_forward																	\n\
	charge02				charge																		\n\
																										\n\
	sneak_forward			walk_forward																\n\
	sneak_back				run_back																	\n\
																										\n\
	swim_forward			run_forward																	\n\
	swim_back				run_back																	\n\
	swim_idle				stand_idle01																\n\
	swim_left				run_forward																	\n\
	swim_right				run_forward																	\n\
																										\n\
	emote_play_guitar		emote_play_guiter															\n\
	emote_speak01			speak01																		\n\
	emote_speak02			speak02				emote_speak01											\n\
	";

// ************************************************************************************************************************************************************

#pragma managed(pop)
