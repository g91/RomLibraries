#include "../Fusion/RuFusion_Sojourner.h"

#include "../Scene/Container/RuContainer_Collision.h"
#include "../Scene/Terrain/RuWorld_Base.h"
#include "../Scene/Widget/RuEntity_Widget_Line.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFusion_Sojourner, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuFusion_Sojourner, "CRuFusion_Sojourner", "IRuObject")
ruCLASSGUID_DEFINE(CRuFusion_Sojourner, 0x73F25A4E, 0x6FC94fb5, 0xAAFA0019, 0x9553B2C7)

// ************************************************************************************************************************************************************

CRuFusion_Sojourner::CRuFusion_Sojourner(IRuObject *collisionObject)
{
	m_movementType = ruSOJMOVETYPE_FULL;
	m_enableWalkingSurfaceSnapping = TRUE;
	m_slopeSlideThreshold = 50.0f;
	m_restingHeightSearchUpLimit = 15.0f;
	m_restingHeightSearchDownLimit = 7.0f;

	m_collisionObject = NULL;
	m_collisionMask = 0x00800000;
	m_triEnumIteration = 0;
	m_lastQueryBounds.Set(CRuVector3(0.0f, 0.0f, 0.0f), 0.0f);
	m_sphere.Set(CRuVector3(0.0f, 0.0f, 0.0f), 1.0f);
	m_frictionConstant = 0.0f;

	m_waterTri_LastQueryBounds.Center().Set(0.0f, 0.0f, 0.0f);
	m_waterTri_LastQueryBounds.Extents()[0] = 0.0f;
	m_waterTri_LastQueryBounds.Extents()[1] = 0.0f;
	m_waterTri_LastQueryBounds.Extents()[2] = 0.0f;

	m_lastRestingPointNormal.Set(0.0f, 1.0f, 0.0f);

	SetCollisionObject(collisionObject);
/*
	// --- VISUALIZER MESH
	m_visualizerMesh = ruNEW CRuDynamicMesh();
	m_visualizerMesh->Initialize(ruPRIMTYPE_TRIANGLELIST, 3000, 3000, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);

	m_visualizerMaterial = ruNEW CRuMaterial();
	m_visualizerMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_EDITOR_TRANSLUCENT"));

	m_visualizerModel = ruNEW CRuModel();
	m_visualizerModel->AddSubModel(m_visualizerMesh, m_visualizerMaterial);
*/
}

CRuFusion_Sojourner::~CRuFusion_Sojourner()
{
	SetCollisionObject(NULL);
}

void CRuFusion_Sojourner::SetFrictionParameters(const CRuVector3 &frictionSurfaceNormal, REAL frictionSurfaceThreshold, REAL frictionConstant)
{
	m_frictionSurfaceNormal = frictionSurfaceNormal;
	m_frictionSurfaceThreshold = frictionSurfaceThreshold;
	m_frictionConstant = frictionConstant;
}

void CRuFusion_Sojourner::SetRestingHeightSearchLimits(REAL upLimit, REAL downLimit)
{
	m_restingHeightSearchUpLimit = upLimit;
	m_restingHeightSearchDownLimit = downLimit;
}

void CRuFusion_Sojourner::SetMovementType(CRuSojournerMovementType movementType)
{
	m_movementType = movementType;
}

void CRuFusion_Sojourner::SetWalkingSurfaceSnapping(BOOL enableSnapping)
{
	m_enableWalkingSurfaceSnapping = enableSnapping;
}

void CRuFusion_Sojourner::SetCollisionObject(IRuObject *collisionObject)
{
	// Unregister event handlers from the old collision object
	if(m_collisionObject)
	{
		if(m_collisionObject->GetType().IsSubClassOf(CRuContainer_Collision::Type()))
		{
			static_cast<CRuContainer_Collision *>(m_collisionObject)->Event_Changed().UnregisterHandler(this->GetGUID());
		}

		m_collisionObject->Event_Dispose().UnregisterHandler(this->GetGUID());
	}

	// Save reference to new collision object
	m_collisionObject = collisionObject;

	// Register event handlers on the collision object
	if(m_collisionObject)
	{
		if(m_collisionObject->GetType().IsSubClassOf(CRuContainer_Collision::Type()))
		{
			static_cast<CRuContainer_Collision *>(m_collisionObject)->Event_Changed().RegisterHandler(this->GetGUID(), boost::bind(&CRuFusion_Sojourner::CollisionContainerChangedHandler, this, _1));
		}

		m_collisionObject->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuFusion_Sojourner::CollisionContainerDisposeHandler, this, _1));
	}
}

void CRuFusion_Sojourner::SetCollisionMask(DWORD mask)
{
	m_collisionMask = mask;
}

/*
CRuModel *CRuFusion_Sojourner::GetVisualizer()
{
	return m_visualizerModel;
//	return NULL;
}
*/
void CRuFusion_Sojourner::ResetCollisionData()
{
	// Reset all query bounds
	m_lastQueryBounds.Set(CRuVector3(0.0f, 0.0f, 0.0f), 0.0f);
	m_waterTri_LastQueryBounds.Center().Set(0.0f, 0.0f, 0.0f);
	m_waterTri_LastQueryBounds.Extents()[0] = 0.0f;
	m_waterTri_LastQueryBounds.Extents()[1] = 0.0f;
	m_waterTri_LastQueryBounds.Extents()[2] = 0.0f;
}

BOOL CRuFusion_Sojourner::Move(const CRuVector3 &origin, const CRuVector3 &velocity, const CRuVector3 &radius, CRuVector3 &adjustedPositionOut)
{
	switch(m_movementType)
	{
		case ruSOJMOVETYPE_FULL:
			return Slide(origin, velocity, radius, adjustedPositionOut);

		case ruSOJMOVETYPE_NPC:
			return NPCSlide(origin, velocity, radius, adjustedPositionOut);
	}

	return FALSE;
}

BOOL CRuFusion_Sojourner::MoveWithClimbAssist(const CRuVector3 &origin, const CRuVector3 &velocity, const CRuVector3 &radius, CRuVector3 &adjustedPositionOut)
{
/*
	CRuVector3 moveDir = velocity;
	moveDir.m_y = 0.0f;

	if(moveDir.Magnitude() > ruEPSILON)
	{
		moveDir.Normalize();
		CRuVector3 expectedDest = origin + moveDir * radius.m_x * 1.05f;
		CRuVector3 assistedOrigin;
		if(FindClimbAssistOrigin(expectedDest, assistedOrigin))
		{
			assistedOrigin.m_x = origin.m_x;
			assistedOrigin.m_y += radius.m_y;
			assistedOrigin.m_z = origin.m_z;

			CRuVector3 diff(0.0f, min(0.0f, origin.m_y - assistedOrigin.m_y), 0.0f);

//			char moo[200];
//			sprintf(moo, "%f -> %f = %f\n", origin.m_y, assistedOrigin.m_y, assistedOrigin.m_y - origin.m_y);
//			OutputDebugString(moo);

			return Move(assistedOrigin, velocity + diff, radius, adjustedPositionOut);
		}
	}
*/
	return Move(origin, velocity, radius, adjustedPositionOut);
}

BOOL CRuFusion_Sojourner::Slide(const CRuVector3 &origin, const CRuVector3 &velocity, const CRuVector3 &radius, CRuVector3 &adjustedPositionOut)
{
	// Clear movement end point list
	m_movementEndPoints.Clear();

	// If no collision object is specified, abort
	if(m_collisionObject == NULL)
	{
		return FALSE;
	}

	BOOL forceIteration = FALSE;
	if(m_lastQueryBounds.Contains(CRuSphere(origin, max(max(radius.x, radius.y), radius.z))) == FALSE)
	{
		forceIteration = TRUE;
	}

	// If velocity has magnitude zero, do nothing
	REAL speed = velocity.Magnitude();
	if(speed <= ruEPSILON && !forceIteration)
	{
		adjustedPositionOut = origin;
		m_movementEndPoints.Add(adjustedPositionOut);
		return TRUE;
	}

	CRuVector3 travelDirection = velocity;

	if(travelDirection.Magnitude() > ruEPSILON)
	{
		travelDirection.Normalize();
	}
	else
	{
		travelDirection.Set(0.0f, 1.0f, 0.0f);
	}

	CRuVector3 currentOrigin = origin;

	REAL evaluationIncrement = 10.0f;
	REAL distanceRemaining = velocity.Magnitude();
/*
	if(distanceRemaining < ruEPSILON)
	{
		distanceRemaining = 0.1f;
	}
*/
	INT32 iterationIdx = 0;
	REAL threshold = 0.001f;

	// If the iteration count exceeds abort threshold, do not move
	m_iterationAbortThreshold = 5.0f;

	REAL iterationCount = distanceRemaining / evaluationIncrement;

	if(iterationCount >= m_iterationAbortThreshold)
	{
//		adjustedPositionOut = origin;
//		return FALSE;
	}

	// Clear iteration result
	m_lastIterationCoverage = 0.0f;
	m_lastIterationCollided = FALSE;
	m_lastCollisionNormal.Set(0.0f, 1.0f, 0.0f);

	while((distanceRemaining > threshold && iterationIdx < m_iterationAbortThreshold) || forceIteration)
	{
		forceIteration = FALSE;
		++iterationIdx;

		REAL speed = min(distanceRemaining, evaluationIncrement);
		distanceRemaining = distanceRemaining - speed;

		CRuVector3 evaluationVelocity = travelDirection * speed;

		// Calculate the bounding area -- use the ellipsoid plus velocity radius and a little bit of extra padding
		REAL boundingRadius = max(max(radius.x, radius.y), radius.z);
		CRuSphere boundingSphere(currentOrigin, boundingRadius);

		// Enumerate potential colliding triangles
		EnumeratePotentialColliders(boundingSphere, 10 + speed);

		// Construct transform to deform ellipsoid into unit sphere
		m_ellipsoidOrigin = currentOrigin;
		m_ellipsoidDeformation.SetToScaling(1.0f / radius.x, 1.0f / radius.y, 1.0f / radius.z);

		// Modify all colliding triangles as follows: Set origin at (0, 0, 0), and warp space such that the ellipsoid becomes a sphere
		m_triangleVerts.SetArraySize(m_triangleVerts0.Count());
		m_triangleVerts.SetNumEntries(m_triangleVerts0.Count());
		for(INT32 i = 0; i < m_triangleVerts0.Count(); ++i)
		{
			m_triangleVerts[i] = m_triangleVerts0[i] - m_ellipsoidOrigin;
			m_ellipsoidDeformation.TransformPoint(m_triangleVerts[i]);
		}

		// Modify all colliding normals and calculate triangle planes
		m_triangleNormals.SetArraySize(m_triangleNormals0.Count());
		m_triangleNormals.SetNumEntries(m_triangleNormals0.Count());
		m_trianglePlanes.SetArraySize(m_triangleNormals0.Count());
		m_trianglePlanes.SetNumEntries(m_triangleNormals0.Count());
		for(INT32 i = 0; i < m_triangleNormals0.Count(); ++i)
		{
			// Calculate normal in ellipsoid space
			CRuVector3 &v0 = m_triangleVerts[i * 3];
			CRuVector3 &v1 = m_triangleVerts[i * 3 + 1];
			CRuVector3 &v2 = m_triangleVerts[i * 3 + 2];

			CRuVector3 e0 = v1 - v0;
			CRuVector3 e1 = v2 - v1;

			if(e0.Magnitude() < ruEPSILON)
			{
				// Force an assert for debug mode
				assert(false);

				// Alter v0 arbitrarily
				v0.m_x += 0.5f;

				e0 = v1 - v0;
			}
			if(e1.Magnitude() < ruEPSILON)
			{
				// Force an assert for debug mode
				assert(false);

				// Alter v2 arbitrarily
				v2.m_x += 0.5f;

				e1 = v2 - v1;
			}

			CRuVector3 e0N = e0;
			CRuVector3 e1N = e1;

			e0N.Normalize();
			e1N.Normalize();

			if(fabs(e0N.Dot(e1N)) > (1.0f - ruEPSILON))
			{
				m_triangleNormals[i].Set(0.0f, 1.0f, 0.0f);
			}
			else
			{
				m_triangleNormals[i] = CrossProduct(e0, e1).Normalize();
			}

			// Insert triangle plane into list
			m_trianglePlanes[i].SetPlane(m_triangleNormals[i], v0);
		}

		if(evaluationVelocity.Magnitude() > ruEPSILON)
		{
			// Build collision ray centered at (0, 0, 0) and scale it
			CRuRay3 collisionRay(CRuVector3(0.0f, 0.0f, 0.0f), evaluationVelocity, FALSE);
			m_ellipsoidDeformation.TransformVector(collisionRay.Direction());
			collisionRay.Direction().Normalize();

			CRuVector3 scaledVelocity = evaluationVelocity;
			m_ellipsoidDeformation.TransformVector(scaledVelocity);

			// Hand-off to collision processor
			adjustedPositionOut = CollideAndSlide(collisionRay, scaledVelocity.Magnitude());

			// Post-process adjusted position
			CRuMatrix4x4 invEllipsoidDeformation;
			invEllipsoidDeformation.SetToScaling(radius.x, radius.y, radius.z);
			invEllipsoidDeformation.TransformPoint(adjustedPositionOut);
			adjustedPositionOut = adjustedPositionOut + m_ellipsoidOrigin;

			invEllipsoidDeformation.TransformPoint(m_lastCollisionPoint);
			m_lastCollisionPoint = m_lastCollisionPoint + m_ellipsoidOrigin;

			invEllipsoidDeformation.TransformPoint(m_lastRestingPoint);
			m_lastRestingPoint = m_lastRestingPoint + m_ellipsoidOrigin;

			currentOrigin = adjustedPositionOut;
			m_movementEndPoints.Add(adjustedPositionOut);
		}
		else
		{
			m_movementEndPoints.Add(currentOrigin);
		}
	}

	// Final processing
	m_lastIterationCoverage = m_lastIterationCoverage / velocity.Magnitude();

	return TRUE;
}

BOOL CRuFusion_Sojourner::NPCSlide(const CRuVector3 &origin, const CRuVector3 &velocity, const CRuVector3 &radius, CRuVector3 &adjustedPositionOut)
{
	// If no collision object is specified, abort
	if(m_collisionObject == NULL)
	{
		return FALSE;
	}

	// If velocity has magnitude zero, do nothing
	REAL speed = velocity.Magnitude();
	if(speed <= ruEPSILON)
	{
		adjustedPositionOut = origin;
		return TRUE;
	}

	CRuVector3 travelDirection = velocity;
	travelDirection.Normalize();

	REAL evaluationIncrement = 2.5f;
	REAL distanceRemaining = velocity.Magnitude();

	// If the iteration count exceeds abort threshold, do not move
	m_iterationAbortThreshold = 50.0f;

	REAL iterationCount = distanceRemaining / evaluationIncrement;
	if(iterationCount >= m_iterationAbortThreshold)
	{
		adjustedPositionOut = origin;
		return TRUE;
	}

	// Clear iteration result
	m_lastIterationCoverage = 0.0f;
	m_lastIterationCollided = FALSE;
	m_lastCollisionNormal.Set(0.0f, 1.0f, 0.0f);

	REAL threshold = 0.001f;
	CRuVector3 currentOrigin(origin.m_x, origin.m_y - radius.m_y, origin.m_z);

	while(distanceRemaining > threshold)
	{
		REAL speed = min(distanceRemaining, evaluationIncrement);
		distanceRemaining = distanceRemaining - speed;

		CRuVector3 evaluationVelocity = travelDirection * speed;

		// Calculate the bounding area -- use the ellipsoid plus velocity radius and a little bit of extra padding
		REAL boundingRadius = max(max(radius.x, radius.y), radius.z);
		CRuSphere boundingSphere(currentOrigin, boundingRadius);

		// Enumerate potential colliding triangles
		EnumeratePotentialColliders(boundingSphere, 10 + speed);

		// Construct transform to deform ellipsoid into unit sphere (always identity here)
		m_ellipsoidOrigin = currentOrigin;
		m_ellipsoidDeformation.SetToScaling(1.0, 1.0f, 1.0f);

		// Modify all colliding triangles as follows: Set origin at (0, 0, 0), and warp space such that the ellipsoid becomes a sphere
		m_triangleVerts.SetArraySize(m_triangleVerts0.Count());
		m_triangleVerts.SetNumEntries(m_triangleVerts0.Count());
		for(INT32 i = 0; i < m_triangleVerts0.Count(); ++i)
		{
			m_triangleVerts[i] = m_triangleVerts0[i] - m_ellipsoidOrigin;
			m_ellipsoidDeformation.TransformPoint(m_triangleVerts[i]);
		}

		// Modify all colliding normals and calculate triangle planes
		m_triangleNormals.SetArraySize(m_triangleNormals0.Count());
		m_triangleNormals.SetNumEntries(m_triangleNormals0.Count());
		m_trianglePlanes.SetArraySize(m_triangleNormals0.Count());
		m_trianglePlanes.SetNumEntries(m_triangleNormals0.Count());
		for(INT32 i = 0; i < m_triangleNormals0.Count(); ++i)
		{
			// Calculate normal in ellipsoid space
			CRuVector3 &v0 = m_triangleVerts[i * 3];
			CRuVector3 &v1 = m_triangleVerts[i * 3 + 1];
			CRuVector3 &v2 = m_triangleVerts[i * 3 + 2];

			CRuVector3 e0 = v1 - v0;
			CRuVector3 e1 = v2 - v1;

			m_triangleNormals[i] = CrossProduct(e0, e1).Normalize();

			// Insert triangle plane into list
			m_trianglePlanes[i].SetPlane(m_triangleNormals[i], v0);
		}

		// Adjust destination height only if snapping is enabled
		if(m_enableWalkingSurfaceSnapping)
		{
			if(FindRestingHeight(currentOrigin + evaluationVelocity, adjustedPositionOut))
			{
				m_lastIterationCollided = TRUE;
			}
			else
			{
				m_lastIterationCollided = FALSE;
			}

			// Post-process adjusted position
			CRuMatrix4x4 invEllipsoidDeformation;
			invEllipsoidDeformation.SetToScaling(1.0, 1.0f, 1.0f);
			invEllipsoidDeformation.TransformPoint(adjustedPositionOut);
			adjustedPositionOut = adjustedPositionOut + m_ellipsoidOrigin;

			m_lastCollisionPoint = adjustedPositionOut;
			m_lastRestingPointNormal.Set(0.0f, 1.0f, 0.0f);
		}
		else
		{
			adjustedPositionOut = currentOrigin + evaluationVelocity;
			m_lastIterationCollided = FALSE;

			m_lastCollisionPoint = adjustedPositionOut;
			m_lastRestingPointNormal.Set(0.0f, 1.0f, 0.0f);
		}

		currentOrigin = adjustedPositionOut;
	}

	adjustedPositionOut.m_y += radius.m_y;

	m_lastIterationCoverage = 1.0f;

	return TRUE;
}

BOOL CRuFusion_Sojourner::IntersectsWaterSurface(const CRuSegment3 &segment)
{
	// Convert segment into ellipsoid coordinate frame
	CRuSegment3 localSegment = segment;
	localSegment.Origin() -= m_ellipsoidOrigin;;
	m_ellipsoidDeformation.TransformPoint(localSegment.Origin());
	m_ellipsoidDeformation.TransformVector(localSegment.Direction());

	// Iterate through all potential colliding triangles and find first intersection
	for(INT32 i = 0, j = 0; i < m_triangleVerts.Count(); i += 3, ++j)
	{
		CRuVector3 intPt;
		REAL intT;

		// Ignore non-water surfaces
		UINT32 &flags = m_triangleFlags0[j];
		if((flags & 0x00200000) == 0)
			continue;

		// Return immediately on intersection
		if(FindIntersection(localSegment, m_triangleVerts[i], m_triangleVerts[i + 1], m_triangleVerts[i + 2], intPt, intT))
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuFusion_Sojourner::IsAboveWaterSurface(const CRuVector3 &origin)
{
	// Temporaries
	CRuVector3 intPt;
	REAL intT = FLT_MAX, nearestT = FLT_MAX;
	BOOL isUnderWater = FALSE;

	// Build collision segment
	CRuSegment3 colSegment;
	colSegment.Origin() = origin;
	colSegment.Direction().Set(0.0f, -50.0f, 0.0f);

	// Enumerate all potential colliders
	WaterTri_EnumeratePotentialColliders(origin, 20.0f);

	// Iterate through all potential colliding triangles and find nearest intersection
	for(INT32 i = 0, j = 0; i < m_waterTri_Vertices.Count(); i += 3, ++j)
	{
		// Ignore non-water surfaces
		if(((m_waterTri_Flags[j] & RuWorld::ruTRIFLAGS_WATER) == 0) && ((m_waterTri_Flags[j] & RuWorld::ruTRIFLAGS_EXTENTMARKER) == 0))
			continue;

		// Return immediately on intersection
		if(FindIntersection(colSegment, m_waterTri_Vertices[i], m_waterTri_Vertices[i + 1], m_waterTri_Vertices[i + 2], intPt, intT))
		{
			if(intT < nearestT)
			{
				nearestT = intT;
				isUnderWater = (m_waterTri_Flags[j] & RuWorld::ruTRIFLAGS_WATER) ? TRUE : FALSE;
			}
		}
	}

	return isUnderWater;
}

BOOL CRuFusion_Sojourner::IsUnderWaterSurface(const CRuVector3 &origin)
{
	// Temporaries
	CRuVector3 intPt;
	REAL intT = FLT_MAX, nearestT = FLT_MAX;
	BOOL isUnderWater = FALSE;

	// Build collision segment
	CRuSegment3 colSegment;
	colSegment.Origin() = origin;
	colSegment.Direction().Set(0.0f, 10000.0f, 0.0f);

	// Enumerate all potential colliders
	WaterTri_EnumeratePotentialColliders(origin, 20.0f);

	// Iterate through all potential colliding triangles and find nearest intersection
	for(INT32 i = 0, j = 0; i < m_waterTri_Vertices.Count(); i += 3, ++j)
	{
		// Ignore non-water surfaces
		if(((m_waterTri_Flags[j] & RuWorld::ruTRIFLAGS_WATER) == 0) && ((m_waterTri_Flags[j] & RuWorld::ruTRIFLAGS_EXTENTMARKER) == 0))
			continue;

		// Return immediately on intersection
		if(FindIntersection(colSegment, m_waterTri_Vertices[i], m_waterTri_Vertices[i + 1], m_waterTri_Vertices[i + 2], intPt, intT))
		{
			if(intT < nearestT)
			{
				nearestT = intT;
				isUnderWater = (m_waterTri_Flags[j] & RuWorld::ruTRIFLAGS_WATER) ? TRUE : FALSE;
			}
		}
	}

	return isUnderWater;
	

/*
	CRuCollisionQuery_Segment_Nearest surfaceQuery;
	surfaceQuery.m_colSegment.Origin() = origin;
	surfaceQuery.m_colSegment.Direction().Set(0.0f, 10000.0f, 0.0f);
	surfaceQuery.m_exclusionMask = ~(RuWorld::ruTRIFLAGS_WATER | RuWorld::ruTRIFLAGS_EXTENTMARKER);
	surfaceQuery.m_ignoreBackface = FALSE;

	// Query static collision object for hit location
	if(m_collisionObject->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		static_cast<CRuEntity *>(m_collisionObject)->QueryCollision(&surfaceQuery);
	}
	else if(m_collisionObject->GetType().IsSubClassOf(IRuCollisionObject::Type()))
	{
		static_cast<IRuCollisionObject *>(m_collisionObject)->QueryCollision(&surfaceQuery);
	}

	if(surfaceQuery.m_positiveHit)
	{
		if(surfaceQuery.m_collisionFlag & RuWorld::ruTRIFLAGS_WATER)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	return FALSE;
*/
}

BOOL CRuFusion_Sojourner::FindRestingHeight(const CRuVector3 &origin, CRuVector3 &restingPtOut)
{
	CRuVector3 originOffset(0.0f, 1.0f, 0.0f);

	// Get collision rays in local coordinate frame
	CRuRay3 rayUp(origin, CRuVector3(0.0f, 1.0f, 0.0f));
	CRuRay3 rayDown(origin, CRuVector3(0.0f, -1.0f, 0.0f));

	rayUp.Origin() -= m_ellipsoidOrigin;
	rayUp.Origin() += originOffset;
	m_ellipsoidDeformation.TransformPoint(rayUp.Origin());
	m_ellipsoidDeformation.TransformVector(rayUp.Direction());

	rayDown.Origin() -= m_ellipsoidOrigin;
	rayDown.Origin() += originOffset;
	m_ellipsoidDeformation.TransformPoint(rayDown.Origin());
	m_ellipsoidDeformation.TransformVector(rayDown.Direction());

	// Find nearest potential anchor points in both up and down directions
	REAL nearestUpT = FLT_MAX, nearestDownT = FLT_MAX;
	CRuVector3 nearestUpPt, nearestDownPt;
	CRuVector3 nearestUpNormal, nearestDownNormal;

	for(INT32 i = 0, j = 0; i < m_triangleVerts.Count(); i += 3, ++j)
	{
		CRuVector3 intPt;
		REAL intT;

		// Ignore water surfaces
		UINT32 &flags = m_triangleFlags0[j];
		if((flags & 0x00200000))
			continue;

		if(FindIntersection(rayUp, m_triangleVerts[i], m_triangleVerts[i + 1], m_triangleVerts[i + 2], intPt, intT))
		{
			if(intT < nearestUpT)
			{
				if(DotProduct(CRuVector3(0.0f, 1.0f, 0.0f), m_triangleNormals0[j]) >= 0.0f)
				{
					nearestUpT = intT;
					nearestUpPt = intPt;
					nearestUpNormal = m_triangleNormals0[j];
				}
			}
		}
		else if(FindIntersection(rayDown, m_triangleVerts[i], m_triangleVerts[i + 1], m_triangleVerts[i + 2], intPt, intT))
		{
			if(intT < nearestDownT)
			{
				if(DotProduct(CRuVector3(0.0f, 1.0f, 0.0f), m_triangleNormals0[j]) >= 0.0f)
				{
					nearestDownT = intT;
					nearestDownPt = intPt;
					nearestDownNormal = m_triangleNormals0[j];
				}
			}
		}
	}

	REAL compareFactor = 0.5f;

	if(nearestUpT * compareFactor < nearestDownT && nearestUpT < m_restingHeightSearchUpLimit)
	{
		restingPtOut = nearestUpPt;
		m_lastCollisionNormal = nearestUpNormal;
		return TRUE;
	}
	else if(nearestDownT < FLT_MAX && nearestDownT < m_restingHeightSearchDownLimit)
	{
		restingPtOut = nearestDownPt;
		m_lastCollisionNormal = nearestDownNormal;
		return TRUE;
	}

	restingPtOut = rayUp.Origin();
	m_lastCollisionNormal.Set(0.0f, 1.0f, 0.0f);

	return FALSE;
}

BOOL CRuFusion_Sojourner::FindRestingHeightAlt(const CRuVector3 &origin, CRuVector3 &restingPtOut, CRuVector3 &restingPtNormalOut)
{
	// Get collision rays in local coordinate frame
	CRuRay3 rayDown(origin + CRuVector3(0.0f, 2.0f, 0.0f), CRuVector3(0.0f, -1.0f, 0.0f));

	m_ellipsoidDeformation.TransformPoint(rayDown.Origin());
	m_ellipsoidDeformation.TransformVector(rayDown.Direction());

	// Find nearest potential anchor points in the downward direction
	REAL nearestDownT = FLT_MAX;
	CRuVector3 nearestDownPt;
	CRuVector3 nearestDownNormal;

	for(INT32 i = 0, j = 0; i < m_triangleVerts.Count(); i += 3, ++j)
	{
		CRuVector3 intPt;
		REAL intT;

		// Ignore water surfaces (wtf happened to foliage?)
		UINT32 &flags = m_triangleFlags0[j];
		if((flags & 0x00200000))
			continue;

		if(FindIntersection(rayDown, m_triangleVerts[i], m_triangleVerts[i + 1], m_triangleVerts[i + 2], intPt, intT))
		{
			if(intT < nearestDownT)
			{
				nearestDownT = intT;
				nearestDownPt = intPt;
				nearestDownNormal = m_triangleNormals0[j];
			}
		}
	}

	if(nearestDownT < FLT_MAX)
	{
		restingPtOut = nearestDownPt;

		if(nearestDownNormal.Magnitude() > ruEPSILON)
		{
			restingPtNormalOut = nearestDownNormal.Normalize();
		}
		else
		{
			restingPtNormalOut.Set(0.0f, 1.0f, 0.0f);
		}

		return TRUE;
	}

	// Set output values to something reasonable
	restingPtOut.Set(origin.m_x, origin.m_y - 10.0f, origin.m_z);
	restingPtNormalOut = CRuVector3(0.0f, 1.0f, 0.0f);

	return FALSE;
}

REAL CRuFusion_Sojourner::GetLastIterationCoverage()
{
	return m_lastIterationCoverage;
}

BOOL CRuFusion_Sojourner::LastIterationCollided()
{
	return m_lastIterationCollided;
}

const CRuVector3 &CRuFusion_Sojourner::GetLastCollisionNormal()
{
	return m_lastCollisionNormal;
}

const CRuVector3 &CRuFusion_Sojourner::GetLastCollisionPoint()
{
	return m_lastCollisionPoint;
}

const CRuVector3 &CRuFusion_Sojourner::GetLastRestingPoint()
{
	return m_lastRestingPoint;
}

const CRuVector3 &CRuFusion_Sojourner::GetLastRestingPointNormal()
{
	return m_lastRestingPointNormal;
}

CRuArrayList<CRuVector3> &CRuFusion_Sojourner::GetMovementEndPoints()
{
	return m_movementEndPoints;
}

BOOL CRuFusion_Sojourner::FindClimbAssistOrigin(const CRuVector3 &origin, CRuVector3 &assistedOriginOut)
{
	// Setup down query
	CRuMatrix4x4 identityMatrix;
	CRuCollisionQuery_Segment_Nearest downSegQuery;
	downSegQuery.m_worldTransform = &identityMatrix;
	downSegQuery.m_invWorldTransform = &identityMatrix;
	downSegQuery.m_exclusionMask = 0x00800000;		// Exclude foliage surfaces
	downSegQuery.m_ignoreBackface = FALSE;
	downSegQuery.m_colSegment = CRuSegment3(origin, origin + CRuVector3(0.0f, -10.0f, 0.0f));

	// Query static collision object for hit location
	if(m_collisionObject->GetType().IsSubClassOf(CRuEntity::Type()))
	{
		static_cast<CRuEntity *>(m_collisionObject)->QueryCollision(&downSegQuery);
	}
	else if(m_collisionObject->GetType().IsSubClassOf(IRuCollisionObject::Type()))
	{
		static_cast<IRuCollisionObject *>(m_collisionObject)->QueryCollision(&downSegQuery);
	}

	if(downSegQuery.m_positiveHit && DotProduct(downSegQuery.m_collisionNormal, CRuVector3(0.0f, 1.0f, 0.0f)) > 0.75f)
	{
		assistedOriginOut = downSegQuery.m_collisionPoint;
		return TRUE;
	}

	return FALSE;
}

BOOL CRuFusion_Sojourner::CollisionContainerChangedHandler(RuEventArgs *eventArgs)
{
	RuColContainerEvent_Changed_Args *changedArgs = static_cast<RuColContainerEvent_Changed_Args *>(eventArgs);

	if(Intersects(changedArgs->m_changeBounds, m_lastQueryBounds))
	{
		m_lastQueryBounds.Radius() = 0.0f;
	}

	return TRUE;
}

BOOL CRuFusion_Sojourner::CollisionContainerDisposeHandler(RuEventArgs *eventArgs)
{
	SetCollisionObject(NULL);
	return TRUE;
}

BOOL CRuFusion_Sojourner::EnumeratePotentialColliders(const CRuSphere &bounds, REAL boundsPadding)
{
	// Build triangle enumeration query based on the bounding area
	CRuCollisionQuery_Sphere_TriEnum sphereQuery;

	delete sphereQuery.m_colTriVerts;
	delete sphereQuery.m_colTriNormals;
	delete sphereQuery.m_colTriFlags;

	sphereQuery.m_colSphere = m_lastQueryBounds;
	sphereQuery.m_colTriVerts = &m_triangleVerts0;
	sphereQuery.m_colTriNormals = &m_triangleNormals0;
	sphereQuery.m_colTriFlags = &m_triangleFlags0;
	sphereQuery.m_exclusionMask = 0;

	// If bounds is outside last query's bounds, or container iteration has changed, re-enumerate triangless
	if(m_lastQueryBounds.Contains(bounds) == FALSE)
	{
		// Store the query bounds plus padding
		CRuSphere paddedBounds(bounds.Center(), bounds.Radius() + boundsPadding);
		m_lastQueryBounds = paddedBounds;

		// Clear prior data
		m_triangleVerts0.Clear();
		m_triangleNormals0.Clear();
		m_triangleFlags0.Clear();
/*
		// Build triangle enumeration query based on the bounding area
		CRuCollisionQuery_Sphere_TriEnum sphereQuery;
		delete sphereQuery.m_colTriVerts;
		delete sphereQuery.m_colTriNormals;
		delete sphereQuery.m_colTriFlags;
//		sphereQuery.m_exclusionMask = 0x00A00000;		// Exclude water and foliage surfaces
		sphereQuery.m_exclusionMask = 0x00800000;		// Exclude foliage surfaces
		sphereQuery.m_colSphere = m_lastQueryBounds;
		sphereQuery.m_colTriVerts = &m_triangleVerts0;
		sphereQuery.m_colTriNormals = &m_triangleNormals0;
		sphereQuery.m_colTriFlags = &m_triangleFlags0;
*/

		// Set query bounds
		sphereQuery.m_colSphere = m_lastQueryBounds;

		// Perform enumeration query
		if(m_collisionObject->GetType().IsSubClassOf(CRuEntity::Type()))
		{
			static_cast<CRuEntity *>(m_collisionObject)->QueryCollision(&sphereQuery);
		}
		else if(m_collisionObject->GetType().IsSubClassOf(IRuCollisionObject::Type()))
		{
			static_cast<IRuCollisionObject *>(m_collisionObject)->QueryCollision(&sphereQuery);
		}

/*
		// Cleanup
		sphereQuery.m_colTriVerts = NULL;
		sphereQuery.m_colTriNormals = NULL;
		sphereQuery.m_colTriFlags = NULL;
*/
/*
		// Visualizer thingy
		CRuVector3 *position = static_cast<CRuMorphTarget *>(m_visualizerMesh->GetMorphTarget(0))->GetPosition();
		DWORD *diffuse = static_cast<CRuMorphTarget *>(m_visualizerMesh->GetMorphTarget(0))->GetDiffuse();
		UINT16 *indices = m_visualizerMesh->GetIndices();
		for(INT32 i = 0; i < m_triangleVerts0.Count(); ++i)
		{
			position[i] = m_triangleVerts0[i];
			diffuse[i] = 0xFFFFFFFF;
			indices[i] = i;
		}

		m_visualizerMesh->SetNumVertices(m_triangleVerts0.Count());
		m_visualizerMesh->SetNumPrimitives(m_triangleVerts0.Count() / 3);
		m_visualizerMesh->IncrementMeshIteration();
		m_visualizerMesh->IncrementTopologyIteration();

		m_visualizerMesh->CalculateBounds();

		m_visualizerModel->MarkLocalEntityBoundsDirty();
*/
/*
		m_visualizerModel->RemoveAllChildren();

		for(INT32 i = 0, j = 0; i < m_triangleNormals0.Count(); ++i)
		{
			CRuVector3 &v0 = m_triangleVerts0[j++];
			CRuVector3 &v1 = m_triangleVerts0[j++];
			CRuVector3 &v2 = m_triangleVerts0[j++];

			CRuVector3 centroid = (v0 + v1 + v2) / 3.0f;

			CRuEntity_Widget_Line *lineWidget = ruNEW CRuEntity_Widget_Line();

			lineWidget->SetOption(ruENTITYOPTION_BILLBOARD, ruBILLBOARDTYPE_AXIAL);
			lineWidget->SetLine(centroid, centroid + m_triangleNormals0[i] * 5.0f, FALSE, TRUE, 0xFFFF0000);
			m_visualizerModel->AddChild(lineWidget);
			ruSAFE_RELEASE(lineWidget);
		}
*/
	}

	// Cleanup
	sphereQuery.m_colTriVerts = NULL;
	sphereQuery.m_colTriNormals = NULL;
	sphereQuery.m_colTriFlags = NULL;

/*
	CRuVector3 *position = static_cast<CRuMorphTarget *>(m_visualizerMesh->GetMorphTarget(0))->GetPosition();
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_visualizerMesh->GetMorphTarget(0))->GetDiffuse();
	UINT16 *indices = m_visualizerMesh->GetIndices();
	for(INT32 i = 0; i < m_triangleVerts0.Count(); ++i)
	{
		diffuse[i] = 0xFFFFFFFF;
	}
*/
	return TRUE;
}

BOOL CRuFusion_Sojourner::WaterTri_EnumeratePotentialColliders(const CRuVector3 &position, REAL boundsPadding)
{
	if(	m_waterTri_LastQueryBounds.Extents()[0] < boundsPadding ||
		m_waterTri_LastQueryBounds.Extents()[1] < boundsPadding ||
		m_waterTri_LastQueryBounds.Extents()[2] < boundsPadding ||
		m_waterTri_LastQueryBounds.Contains(position) == FALSE)
	{
		// Store the query bounds plus padding
		m_waterTri_LastQueryBounds.Center() = position;
		m_waterTri_LastQueryBounds.Extents()[0] = boundsPadding;
		m_waterTri_LastQueryBounds.Extents()[1] = 5000.0f;
		m_waterTri_LastQueryBounds.Extents()[2] = boundsPadding;

		// Clear prior data
		m_waterTri_Vertices.Clear();
		m_waterTri_Normals.Clear();
		m_waterTri_Flags.Clear();

		// Build triangle enumeration query based on the bounding area
		CRuCollisionQuery_OBB_TriEnum colQuery;

		delete colQuery.m_colTriVerts;
		delete colQuery.m_colTriNormals;
		delete colQuery.m_colTriFlags;

		colQuery.m_colOBB = m_waterTri_LastQueryBounds;
		colQuery.m_colTriVerts = &m_waterTri_Vertices;
		colQuery.m_colTriNormals = &m_waterTri_Normals;
		colQuery.m_colTriFlags = &m_waterTri_Flags;
		colQuery.m_exclusionMask = 0x00800000;		// Exclude foliage surfaces

		// Perform enumeration query
		if(m_collisionObject->GetType().IsSubClassOf(CRuEntity::Type()))
		{
			static_cast<CRuEntity *>(m_collisionObject)->QueryCollision(&colQuery);
		}
		else if(m_collisionObject->GetType().IsSubClassOf(IRuCollisionObject::Type()))
		{
			static_cast<IRuCollisionObject *>(m_collisionObject)->QueryCollision(&colQuery);
		}

		// Cleanup
		colQuery.m_colTriVerts = NULL;
		colQuery.m_colTriNormals = NULL;
		colQuery.m_colTriFlags = NULL;
	}
	else
	{
//		OutputDebugString("Moo\n");
	}

	return TRUE;
}

CRuVector3 CRuFusion_Sojourner::CollideAndStop(const CRuRay3 &collisionRay, const REAL speed)
{
	CRuVector3 result;

	// Process colliders
	ProcessColliders(collisionRay, speed);

	if(m_colliders.Count() > 0)
	{
		char outputStr[2048];
		sprintf(outputStr, "[%d] %f, %d triangles examined. [%d:%d:%d:%d] %d.\n", m_colliders[0].m_collisionType, m_colliders[0].m_collisionDistance, m_triangleNormals.Count(), m_path_Embedded, m_path_Surface, m_path_Point, m_path_Edge, m_passed);
//		g_pChatFrame->AddMessage(eMessage_Channel_System, outputStr);
	}
	else
	{
		char outputStr[2048];
		sprintf(outputStr, "NO COL, %d triangles examined. [%d:%d:%d:%d] %d.\n", m_triangleNormals.Count(), m_path_Embedded, m_path_Surface, m_path_Point, m_path_Edge, m_passed);
//		g_pChatFrame->AddMessage(eMessage_Channel_System, outputStr);
	}

	// Set result to the entire traversal length
	result = collisionRay.Origin() + collisionRay.Direction() * speed;

	// If there was a valid collision, modify the result to fit
//	if(m_colliders.Count() > 0 && m_colliders[0].m_collisionDistance < speed)
	if(m_colliders.Count() > 0)
	{

		if(m_colliders[0].m_collisionType == ruCOLTYPE_EMBEDDED)
		{
//			result = collisionRay.Origin() - m_colliders[0].m_trianglePlane.GetNormal() * (speed * m_colliders[0].m_collisionDistance);
			result = collisionRay.Origin() - m_colliders[0].m_trianglePlane.GetNormal() * m_colliders[0].m_collisionDistance;
		}
		else
		{
			result = collisionRay.Origin() + collisionRay.Direction() * (speed * m_colliders[0].m_collisionDistance);
		}
/*
		char outputStr[2048];
		sprintf(outputStr, "Distance = %f, Type = %d.\n", m_colliders[0].m_collisionDistance, m_colliders[0].m_collisionType);
		OutputDebugStr(outputStr);
*/
//		if(fabs(m_colliders[0].m_collisionDistance) > 0.001f)
//			result = collisionRay.Origin() + collisionRay.Direction() * m_colliders[0].m_collisionDistance;
//		else
//			result = collisionRay.Origin();
//			result = collisionRay.Origin() + collisionRay.Direction() * (speed * m_colliders[0].m_collisionDistance);
	}

	return result;
}

CRuVector3 CRuFusion_Sojourner::CollideAndSlide(const CRuRay3 &collisionRay, const REAL speed)
{
	INT32 maxLoops = 30;

	CRuSegment3 inputSegment(collisionRay.Origin(), collisionRay.Origin() + collisionRay.Direction() * speed);
	CRuSegment3 outputSegment(inputSegment);

	if(m_triangleVerts.Count() == 0)
	{
		outputSegment.Origin() = outputSegment.Origin() + outputSegment.Direction();

		return outputSegment.Origin();
	}

	// Set last collision point to the bottom of the unit sphere
	m_lastCollisionPoint = outputSegment.Origin();
	m_lastCollisionPoint.m_y -= 1.0f;

	// Clear skip list
	m_skipList.Clear();

	while(maxLoops-- && outputSegment.Direction().Magnitude() > ruEPSILON)
	{
		// Prepare output ray based on output segment
		CRuRay3 outputRay(outputSegment.Origin(), outputSegment.Direction(), TRUE);

		// Process colliders
		REAL outputSpeed = outputSegment.Direction().Magnitude();
		ProcessColliders(outputRay, outputSpeed);

		// Clear skip list
		m_skipList.Clear();

		// If we didn't hit anything just return the destination
		if(m_colliders.Count() == 0)
		{
			outputSegment.Origin() = outputSegment.Origin() + outputSegment.Direction();

			// Set collision result
			m_lastIterationCoverage += outputSegment.Direction().Magnitude();

			break;
		}

		// If it was embedded, push away
		if(m_colliders[0].m_collisionType == ruCOLTYPE_EMBEDDED)
		{
			// If it's embedded, back up along the collision plane normal
			outputSegment.Origin() = outputSegment.Origin() - m_colliders[0].m_trianglePlane.GetNormal() * m_colliders[0].m_collisionDistance;

			// Set collision result
			m_lastIterationCoverage += fabs(m_colliders[0].m_collisionDistance);
			m_lastIterationCollided = TRUE;
			m_lastCollisionNormal = m_colliders[0].m_trianglePlane.GetNormal();
			m_lastCollisionPoint = m_colliders[0].m_collisionPoint;
		}
		else
		{
			// We hit something, move as far as we can
			REAL colDist = m_colliders[0].m_collisionDistance * (1.0f - ruEPSILON);
			REAL frictionSurfaceAngle = m_colliders[0].m_trianglePlane.DotNormal(m_frictionSurfaceNormal);

			outputSegment.Origin() = outputSegment.Origin() + outputRay.Direction() * colDist;

			// Friction calculation
			if(frictionSurfaceAngle >= m_frictionSurfaceThreshold)
			{
				outputSegment.Direction() = outputRay.Direction() * (outputSegment.Direction().Magnitude() - colDist) * (1.0f - m_frictionConstant);
			}
			else
			{
				outputSegment.Direction() = outputRay.Direction() * (outputSegment.Direction().Magnitude() - colDist);
			}

			// Set collision result
			m_lastIterationCoverage += fabs(colDist);
			m_lastIterationCollided = TRUE;
			m_lastCollisionNormal = m_colliders[0].m_trianglePlane.GetNormal();
			m_lastCollisionPoint = m_colliders[0].m_collisionPoint;
/*
			// Attempt to slide along the colliding triangle
			CRuVector3 slideSrc = m_colliders[0].m_trianglePlane.GetClosestPointOnPlane(outputSegment.Origin());
			CRuVector3 slideDst = m_colliders[0].m_trianglePlane.GetClosestPointOnPlane(outputSegment.Origin() + outputSegment.Direction());

			CRuVector3 v0(slideSrc.m_x, 0.0f, slideSrc.m_z);
			CRuVector3 v1(slideDst.m_x, 0.0f, slideDst.m_z);

			// If the sliding magnitude is below some hack-ish threshold, build an alternate sliding plane
			if((v0 - v1).Magnitude() < speed * 0.25f)
			{
				// Verify that the colliding triangle is lower than the origin by some hack-ish threshold
				m_colliders[0].m_triangleIndex

				// Build a sliding plane
				CRuVector3 slidePlaneNormal = outputRay.Origin() - m_colliders[0].m_collisionPoint;
				slidePlaneNormal.Normalize();

				CRuPlane4 slidePlane;
				slidePlane.SetPlane(slidePlaneNormal, m_colliders[0].m_collisionPoint);

				slideSrc = slidePlane.GetClosestPointOnPlane(outputSegment.Origin());
 				slideDst = slidePlane.GetClosestPointOnPlane(outputSegment.Origin() + outputSegment.Direction());
			}

			// Save new movement direction
			outputSegment.Direction() = slideDst - slideSrc;
*/

			// Catch the edge cases where we are not really sliding at all
			if(outputSegment.Direction().Magnitude() < ruEPSILON)
			{
				break;
			}

			// Calculate the expected slide direction vector
			CRuVector3 expectedSlideDir = outputSegment.Direction();
			expectedSlideDir.Normalize();

			// Build an alternate sliding plane for the secondary sliding vector
			CRuVector3 slidePlaneNormal1 = outputRay.Origin() - m_colliders[0].m_collisionPoint;
			slidePlaneNormal1.Normalize();

			CRuPlane4 slidePlane1;
			slidePlane1.SetPlane(slidePlaneNormal1, m_colliders[0].m_collisionPoint);

			// Calculate primary sliding vector
			CRuVector3 slideSrc0 = m_colliders[0].m_trianglePlane.GetClosestPointOnPlane(outputSegment.Origin());
			CRuVector3 slideDst0 = m_colliders[0].m_trianglePlane.GetClosestPointOnPlane(outputSegment.Origin() + outputSegment.Direction());
			CRuVector3 slideDir0 = slideDst0 - slideSrc0;
			REAL dot0 = DotProduct(expectedSlideDir, slideDir0);

			// Calculate secondary sliding vector
			CRuVector3 slideSrc1 = slidePlane1.GetClosestPointOnPlane(outputSegment.Origin());
			CRuVector3 slideDst1 = slidePlane1.GetClosestPointOnPlane(outputSegment.Origin() + outputSegment.Direction());
			CRuVector3 slideDir1 = slideDst1 - slideSrc1;
			REAL dot1 = DotProduct(expectedSlideDir, slideDir1);

			REAL collisionHeight = m_colliders[0].m_collisionPoint.m_y - outputSegment.Origin().m_y;

			if(dot0 > dot1 || collisionHeight > -0.40f)
			{
				// Save new movement direction
				outputSegment.Direction() = slideDir0;
			}
			else
			{
				// Save new movement direction
				outputSegment.Direction() = slideDir1;
			}

			// Slope prohibition test code
			CRuVector3 canonicalNormal(0.0f, 1.0f, 0.0f);

			if(DotProduct(m_lastRestingPointNormal, canonicalNormal) < 0.999f)
			{
				CRuVector3 orthoVec = CrossProduct(m_lastRestingPointNormal, canonicalNormal);
				if(orthoVec.Magnitude() > ruEPSILON)
				{
					orthoVec.Normalize();

					REAL angle = 0.0f;
					CalculateAngleDifferential(canonicalNormal, m_lastRestingPointNormal, orthoVec, angle);

					if(angle > ruPI / 180.0f * m_slopeSlideThreshold)
					{
						outputSegment.Direction().m_y = min(0.0f, outputSegment.Direction().m_y);
					}
				}
			}
/*
			char moo[200];
			sprintf(moo, "%f -> %f = %f\n", outputSegment.Origin().m_y, m_colliders[0].m_collisionPoint.m_y, m_colliders[0].m_collisionPoint.m_y - outputSegment.Origin().m_y);
			OutputDebugString(moo);
*/
			// Don't collide with this one next time
			m_skipList.Add(m_colliders[0].m_triangleIndex);
		}
	}

	// Find proper resting point for the current position
	FindRestingHeightAlt(outputSegment.Origin(), m_lastRestingPoint, m_lastRestingPointNormal);

	return outputSegment.Origin();
}

BOOL CRuFusion_Sojourner::IsEmbedded(INT32 triangleIndex, const CRuVector3 &sphereCenter, CRuVector3 &closestPointOut)
{
	// Shortcut references
	const CRuVector3 &v0 = m_triangleVerts[triangleIndex * 3];
	const CRuVector3 &v1 = m_triangleVerts[triangleIndex * 3 + 1];
	const CRuVector3 &v2 = m_triangleVerts[triangleIndex * 3 + 2];
	const CRuPlane4 &plane = m_trianglePlanes[triangleIndex];

	// Find the closest point on the triangle to the sphere center
	closestPointOut = ClosestPointOnTriangle(v0, v1, v2, sphereCenter);

	// Find the vector v from the sphere center to the closest point on the triangle
	CRuVector3 v = closestPointOut - sphereCenter;

	// Sphere is embedded if the v has length less than the radius of the unit sphere
	return DotProduct(v, v) < (1.0f - ruEPSILON);
}

void CRuFusion_Sojourner::ProcessColliders(const CRuRay3 &collisionRay, const REAL speed)
{
	// Clear all colliders
	m_colliders.Clear();

	// Iterate through all colliding triangles and process each collision
	for(INT32 i = 0, j = 0; i < m_triangleVerts.Count(); i += 3, ++j)
	{
		REAL collisionDistance;
		CRuPlane4 collisionPlane;
		RuCollisionType	collisionType = ruCOLTYPE_NONE;

		const CRuVector3 &v0 = m_triangleVerts[i];
		const CRuVector3 &v1 = m_triangleVerts[i + 1];
		const CRuVector3 &v2 = m_triangleVerts[i + 2];
		const CRuVector3 &n = m_triangleNormals[j];
		const CRuPlane4 &plane = m_trianglePlanes[j];

		// Ignore water surfaces
		UINT32 &flags = m_triangleFlags0[j];
		if(flags & m_collisionMask)
			continue;

		// Ignore polygons in the skip list
		if(m_skipList.Contains(j))
			continue;

		// Ignore back-facing triangles
		if(plane.GetDistance(collisionRay.Origin()) <= 0.0f)
			continue;

		// At this point, we know that the sphere is in front of the plane
		// Find sphere's collision point with the triangle plane
		CRuSphere collisionSphere(collisionRay.Origin(), 1.0f);
		CRuVector3 planeIntersectionPoint;
		if(!FindIntersection(collisionSphere, collisionRay.Direction(), plane, collisionDistance, planeIntersectionPoint))
			continue;

		CRuVector3 closestPoint;
		if(IsEmbedded(j, collisionRay.Origin(), closestPoint))
		{
			// Set collision distance and plane to be the shortest path for unembedding the collision sphere
			collisionDistance = (closestPoint - collisionRay.Origin()).Magnitude() - 1.0f;

			if(fabs(collisionDistance + 1.0f) <= ruEPSILON)
			{
				collisionPlane.SetPlane(CRuVector3(0.0f, 1.0f, 0.0f), closestPoint);
			}
			else
			{
				collisionPlane.SetPlane((collisionRay.Origin() - closestPoint).Normalize(), closestPoint);
			}

			collisionType = ruCOLTYPE_EMBEDDED;
		}
		else if(InsideTriangle(v0, v1, v2, n, planeIntersectionPoint))
		{
			// Our collision plane is our polygon's plane
			collisionPlane = plane;
			collisionType = ruCOLTYPE_SURFACE;

			// Is this an actual collision?
			if(collisionDistance > speed)
				continue;
		}
		else
		{
			BOOL found = FALSE;

			collisionPlane = plane;

			REAL lowerLimit = -10000.0f;
			REAL upperLimit = 50000.0f;

			collisionDistance = FLT_MAX;

			// Test against triangle edge (0, 1)
			CRuCylinder cyl_0_1(CRuSegment3(v0, v1), 1.0f);
			REAL t_0_1;
			if(FindIntersection(collisionRay, cyl_0_1, t_0_1) && (t_0_1 >= lowerLimit && t_0_1 <= upperLimit))
			{
				found = TRUE;
				collisionDistance = t_0_1;
				collisionType = ruCOLTYPE_EDGE;
			}

			// Test against triangle edge (1, 2)
			CRuCylinder cyl_1_2(CRuSegment3(v1, v2), 1.0f);
			REAL t_1_2;
			if(FindIntersection(collisionRay, cyl_1_2, t_1_2) && (t_1_2 >= lowerLimit && t_1_2 <= upperLimit) && (t_1_2 < collisionDistance || found == FALSE))
			{
				found = TRUE;
				collisionDistance = t_1_2;
				collisionType = ruCOLTYPE_EDGE;
			}

			// Test against triangle edge (2, 0)
			CRuCylinder cyl_2_0(CRuSegment3(v2, v0), 1.0f);
			REAL t_2_0;
			if(FindIntersection(collisionRay, cyl_2_0, t_2_0) && (t_2_0 >= lowerLimit && t_2_0 <= upperLimit) && (t_2_0 < collisionDistance || found == FALSE))
			{
				found = TRUE;
				collisionDistance = t_2_0;
				collisionType = ruCOLTYPE_EDGE;
			}

			// Test against triangle vertex 0
			CRuSphere sph_0(v0, 1.0f);
			REAL t_v_0;
			if(FindIntersection(collisionRay, sph_0, t_v_0) && (t_v_0 >= lowerLimit && t_v_0 <= upperLimit) && (t_v_0 < collisionDistance || found == FALSE))
			{
				found = TRUE;
				collisionDistance = t_v_0;
				collisionType = ruCOLTYPE_POINT;
			}

			// Test against triangle vertex 1
			CRuSphere sph_1(v1, 1.0f);
			REAL t_v_1;
			if(FindIntersection(collisionRay, sph_1, t_v_1) && (t_v_1 >= lowerLimit && t_v_1 <= upperLimit) && (t_v_1 < collisionDistance || found == FALSE))
			{
				found = TRUE;
				collisionDistance = t_v_1;
				collisionType = ruCOLTYPE_POINT;
			}

			// Test against triangle vertex 2
			CRuSphere sph_2(v2, 1.0f);
			REAL t_v_2;
			if(FindIntersection(collisionRay, sph_2, t_v_2) && (t_v_2 >= lowerLimit && t_v_2 <= upperLimit) && (t_v_2 < collisionDistance || found == FALSE))
			{
				found = TRUE;
				collisionDistance = t_v_2;
				collisionType = ruCOLTYPE_POINT;
			}

			if(found == FALSE)
			{
				continue;
			}
		}

		// Determine whether the collision is real
		switch(collisionType)
		{
			// Always treat embedded collisions as real
			case ruCOLTYPE_EMBEDDED:
				break;

			// Allow more leeway for point collisions so we do not get stuck on sharp edges
			case ruCOLTYPE_POINT:
				if(collisionDistance > speed || collisionDistance <= ruEPSILON)
					continue;
				break;

			// Edge and surface collisions have less leeway so we do not constantly sink into the floor
			case ruCOLTYPE_EDGE:
			case ruCOLTYPE_SURFACE:
				if(collisionDistance > speed || collisionDistance <= -ruEPSILON)
					continue;
				break;
		}

		// Ignore collision planes that we're traveling away from
		if(collisionType != ruCOLTYPE_EMBEDDED && DotProduct(collisionPlane.GetNormal(), collisionRay.Direction()) >= 0.0f)
			continue;

		// Is it closer, farther away, or the same distance as what we've found so far?
		if(m_colliders.Count())
		{
			REAL d = fabs(collisionDistance - m_colliders[0].m_collisionDistance);
			if(d > ruEPSILON)
			{
				if(collisionDistance < m_colliders[0].m_collisionDistance)
					m_colliders.Clear();
				else if (collisionDistance > m_colliders[0].m_collisionDistance)
					continue;
			}
			// Make sure it's not already in the list (this plane's normal)
			else
			{
				BOOL skipMe = FALSE;
				for(INT32 k = 0; k < m_colliders.Count(); ++k)
				{
					if(DotProduct(m_colliders[k].m_trianglePlane.GetNormal(), collisionPlane.GetNormal()) > 1.0f - ruEPSILON)
					{
						skipMe = TRUE;
						break;
					}
				}

				if(skipMe)
					continue;
			}
		}

		// Add this collider to the list
		RuCollider nearest;
		nearest.m_triangleIndex = j;
		nearest.m_trianglePlane = collisionPlane;
		nearest.m_collisionDistance = collisionDistance;
		nearest.m_collisionPoint = closestPoint;
		nearest.m_collisionType = collisionType;
		m_colliders.Add(nearest);
	}
/*
	// --- VISUALIZER MESH
	DWORD *diffuse = static_cast<CRuMorphTarget *>(m_visualizerMesh->GetMorphTarget(0))->GetDiffuse();

	for(INT32 i = 0; i < m_colliders.Count(); ++i)
	{
		INT32 j = m_colliders[i].m_triangleIndex * 3;

		switch(m_colliders[i].m_collisionType)
		{
			case ruCOLTYPE_EMBEDDED:
				{
					// --- VISUALIZER MESH
					diffuse[j + 0] = 0xFFFF00FF;
					diffuse[j + 1] = 0xFFFF00FF;
					diffuse[j + 2] = 0xFFFF00FF;
				}

				break;

			case ruCOLTYPE_POINT:
				{
					// --- VISUALIZER MESH
					diffuse[j + 0] = 0xFFFF0000;
					diffuse[j + 1] = 0xFFFF0000;
					diffuse[j + 2] = 0xFFFF0000;
				}

				break;

			case ruCOLTYPE_EDGE:
				{
					// --- VISUALIZER MESH
					diffuse[j + 0] = 0xFF00FF00;
					diffuse[j + 1] = 0xFF00FF00;
					diffuse[j + 2] = 0xFF00FF00;
				}

				break;

			case ruCOLTYPE_SURFACE:
				{
					// --- VISUALIZER MESH
					diffuse[j + 0] = 0xFF0000FF;
					diffuse[j + 1] = 0xFF0000FF;
					diffuse[j + 2] = 0xFF0000FF;
				}

				break;
		}
	}

	// --- VISUALIZER MESH
	m_visualizerMesh->IncrementMeshIteration();
	m_visualizerMesh->IncrementTopologyIteration();
*/
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
