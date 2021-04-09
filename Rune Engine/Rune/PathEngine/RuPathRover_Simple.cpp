/*!
	@project Rune
	@file RuPathRover_Simple.cpp

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2006/02/09
*/

#include "../PathEngine/RuPathRover_Simple.h"
#include "../Scene/Base/RuEntityBase.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuPathRover_Simple, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuPathRover_Simple, "CRuPathRover_Simple", "IRuPathRover_Base")
ruCLASSGUID_DEFINE(CRuPathRover_Simple, 0xEF791660, 0xA69446b2, 0xB9D775DB, 0x6A0EFB2B)

// ************************************************************************************************************************************************************

CRuPathRover_Simple::CRuPathRover_Simple()
:	m_colObject(NULL),
	m_simpleRoving(FALSE),
	m_sojourner(NULL)
{
	m_sojourner = ruNEW CRuFusion_Sojourner(NULL);
}

CRuPathRover_Simple::~CRuPathRover_Simple()
{
	ruSAFE_RELEASE(m_colObject);
	ruSAFE_RELEASE(m_sojourner);
}

void CRuPathRover_Simple::EnableSimpleRoving(BOOL enable)
{
	m_simpleRoving = enable;
}

BOOL CRuPathRover_Simple::SetCollisionObject(IRuObject *colObject)
{
	colObject->AddRef();
	ruSAFE_RELEASE(m_colObject);
	m_colObject = colObject;

	m_sojourner->SetCollisionObject(colObject);

	return TRUE;
}

BOOL CRuPathRover_Simple::ReAnchorPoint(CRuVector3 &point)
{
	// Build nearest point collision query
	CRuCollisionQuery_Segment_Nearest segQuery;
//	segQuery.m_colSegment = CRuSegment3(point + CRuVector3(0.0f, 10.0f, 0.0f), point - CRuVector3(0.0f, 10.0f, 0.0f));
	segQuery.m_colSegment = CRuSegment3(point, point - CRuVector3(0.0f, 20.0f, 0.0f));

	// Perform collision query
	if(m_colObject->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		static_cast<CRuEntity *>(m_colObject)->QueryCollision(&segQuery);
	}
	else if(m_colObject->GetType().IsSubClassOf(IRuCollisionHierarchy::Type()))
	{
		static_cast<IRuCollisionHierarchy *>(m_colObject)->QueryCollision(&segQuery);
	}

	// If there is a collision, return the point of collision are anchor point
	if(segQuery.m_positiveHit)
	{
		point = segQuery.m_collisionPoint;
		return TRUE;
	}

	return FALSE;
}

BOOL CRuPathRover_Simple::IsPathValid(const CRuVector3 &startPt, const CRuVector3 &endPt)
{
	CRuVector3 actorOffset(0.0f, 9.0f, 0.0f);
	CRuVector3 actorEllipsoid(4.0f, 9.0f, 4.0f);

	CRuVector3 p0 = startPt;
	CRuVector3 p1 = endPt;

	if(m_simpleRoving)
	{
		if(m_colObject)
		{
			CRuCollisionQuery_Segment_HitTest colQuery;
			colQuery.m_exclusionMask = 0x00A00000;			// Ignore water and foliage
			colQuery.m_ignoreBackface = FALSE;				// Do not ignore backfacing triangles
			colQuery.m_colSegment.Origin() = p0 + actorOffset;
			colQuery.m_colSegment.Direction() = p1 - p0;

			if(m_colObject->GetType().IsSubClassOf(CRuEntity::Type()))
			{
				static_cast<CRuEntity *>(m_colObject)->QueryCollision(&colQuery);
			}
			else if(m_colObject->GetType().IsSubClassOf(IRuCollisionObject::Type()))
			{
				static_cast<IRuCollisionObject *>(m_colObject)->QueryCollision(&colQuery);
			}

			if(colQuery.m_positiveHit)
			{
				return FALSE;
			}
		}

		return TRUE;
	}
	else
	{
		INT32 iterationCount = 0;

		while(iterationCount < 5)
		{
			// Calculate movement vector and remove y component
			CRuVector3 movementVector = p1 - p0;
			movementVector.m_y = 0.0f;

			// Calculate movement distance
			REAL movementDistance = movementVector.Magnitude();

			// Calculate gravity speed based on movement distance
			REAL m_gravityAccelleration = -98.0f;
			REAL m_movementSpeed = 40.0f;

			REAL moveTime = movementDistance / m_movementSpeed;
			REAL gravitySpeed = m_gravityAccelleration * moveTime;
			CRuVector3 gravityVector(0.0f, gravitySpeed, 0.0f);

			// Move toward the goal
			CRuVector3 p0_new;
			m_sojourner->Slide(p0 + actorOffset, movementVector + gravityVector, actorEllipsoid, p0_new);
			p0_new -= actorOffset;

			// Calculate delta distance to start
			REAL p0Delta = (p0_new - p0).Magnitude();

			// If delta distance to start is below threshold, the destination is considered unreachable
			if(p0Delta <= movementDistance * 0.1f)
			{
				return FALSE;
			}

			// Calculate delta distance to goal
			if(fabs(p0_new.m_y - p1.m_y) < 4.0f) // note: this value needs some adjustment to make it work well
			{
				CRuVector3 pA(p0_new.m_x, 0.0f, p0_new.m_z);
				CRuVector3 pB(p1.m_x, 0.0f, p1.m_z);
				REAL p1Delta = (pA - pB).Magnitude();

				if(p1Delta <= 1.0f)
				{
					return TRUE;
				}
			}

			// Set new p0
			p0 = p0_new;

			// Increment iteration count
			++iterationCount;
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed
