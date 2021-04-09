#include "../Engine/RuCamera.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuCamera, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuCamera, "CRuCamera", "IRuObject")
ruCLASSGUID_DEFINE(CRuCamera, 0xCE24B22D, 0x8F55406b, 0x80AACECE, 0xEBFC079F)

// ************************************************************************************************************************************************************

REAL CameraJiggleF_V(REAL x, REAL v, REAL k, REAL b, REAL m)
{
	return v;
}

REAL CameraJiggleF_A(REAL x, REAL v, REAL k, REAL b, REAL m)
{
	return -(k / m) * x - (b / m) * v;
}

// ************************************************************************************************************************************************************

void CRuCamera::Update(REAL deltaT)
{
	if(m_transitionT >= 0.0f)
	{
		m_transitionT -= deltaT;

		if(m_transitionT <= 0.0f)
		{
			m_transitionT = -1.0f;
		}

		m_cameraPos = (m_cameraPosOld * (m_transitionT / m_transitionDuration)) + (m_cameraPosNew * ((1.0f - m_transitionT) / m_transitionDuration));
		m_transformsDirty = TRUE;
	}

	// Do jiggle, cap t delta to the limit
	m_rk4Solver->Solve(0.0f, min(deltaT, m_jiggle_tDeltaLimit));

	CRuVector3 newJiggleOffset =m_jiggle_Direction * (m_rk4Solver->GetValue(0) - m_jiggle_R);

	if((newJiggleOffset - m_jiggle_Offset).Magnitude() > ruEPSILON)
	{
		m_jiggle_Offset = newJiggleOffset;

		m_transformsDirty = TRUE;
	}

	if(m_ySmoothEnabled)
	{
		if(m_targetY > 10.0f)
		{
			m_cameraPos.m_y += m_targetY;
			m_targetPos.m_y += m_targetY;

			m_targetY = 0.0f;
		}
		else
		{
			REAL factor = 0.2f * deltaT;

			m_cameraPos.m_y += m_targetY * factor;
			m_targetPos.m_y += m_targetY * factor;

			m_targetY *= (1.0f - factor);
		}
	}
}

void CRuCamera::SetCameraPositionSoft(const CRuVector3 &position, REAL duration)
{
	if(m_transitionT >= 0.0f)
	{
		m_transitionT = duration;
		m_transitionDuration = duration;
	}

	m_cameraPosOld = m_cameraPos;
	m_cameraPosNew = position;
}

void CRuCamera::SetObliqueProjection(REAL nearPlane, REAL farPlane, REAL viewportWidth, REAL viewportHeight, REAL orientation)
{
	SetOrthographicProjection(nearPlane, farPlane, viewportWidth, viewportHeight);

	m_orientation = orientation;
}

void CRuCamera::SetOrthographicProjection(REAL nearPlane, REAL farPlane, REAL viewportWidth, REAL viewportHeight)
{
	// Store frustum properties
	m_nearPlaneDistance = nearPlane;
	m_farPlaneDistance = farPlane;
	m_viewportWidth = viewportWidth;
	m_viewportHeight = viewportHeight;
	if(m_viewportHeight > ruEPSILON || m_viewportHeight < -ruEPSILON)
		m_aspectRatio = m_viewportWidth / m_viewportHeight;
	else
		m_aspectRatio = 1.0f;

	m_parallelProjection = TRUE;

	m_orientation = 0.0f;

	// Mark transforms as dirty
	m_transformsDirty = TRUE;
}

void CRuCamera::SetPerspectiveProjection(REAL nearPlane, REAL farPlane, REAL viewportWidth, REAL viewportHeight)
{
	// Store frustum properties
	m_nearPlaneDistance = nearPlane;
	m_farPlaneDistance = farPlane;
	m_viewportWidth = viewportWidth;
	m_viewportHeight = viewportHeight;
	if(m_viewportHeight > ruEPSILON || m_viewportHeight < -ruEPSILON)
		m_aspectRatio = m_viewportWidth / m_viewportHeight;
	else
		m_aspectRatio = 1.0f;

	m_parallelProjection = FALSE;

	// Mark transforms as dirty
	m_transformsDirty = TRUE;
}

void CRuCamera::SetPerspectiveProjectionFOV(REAL nearPlane, REAL farPlane, REAL FOV, REAL aspectRatio)
{
	REAL FOVh = FOV, viewportWidth, viewportHeight;

	if(aspectRatio > ruEPSILON || aspectRatio < -ruEPSILON)
		FOVh = FOV / aspectRatio;

	viewportHeight = 2.0f * nearPlane * sin(FOV / 2.0f);
	viewportWidth = viewportHeight * aspectRatio;

	SetPerspectiveProjection(nearPlane, farPlane, viewportWidth, viewportHeight);
}

void CRuCamera::SetViewDistance(REAL farPlane)
{
	// Update frustum far plane
	m_farPlaneDistance = farPlane;

	m_parallelProjection = FALSE;

	// Mark transforms as dirty
	m_transformsDirty = TRUE;
}

BOOL CRuCamera::IsBackdropEnabled() const
{
	return m_backdropEnabled;
}

void CRuCamera::SetBackdropDistance(REAL nearPlane, REAL farPlane)
{
	m_backdropEnabled = TRUE;
	m_backdropNearDistance = nearPlane;
	m_backdropFarDistance = farPlane;

	// Mark transforms as dirty
	m_transformsDirty = TRUE;
}

BOOL CRuCamera::EnableYSmoothing(BOOL enable)
{
	m_ySmoothEnabled = enable;
	return TRUE;
}








void CRuCamera::LinCombVector3(const CRuVector3 &pos, const CRuVector3 &dir, REAL t, CRuVector3 &result)
{
	for(INT32 i = 0; i < 3; ++i)
	{
		result[i] = pos[i] + t * dir[i];
	}
}

INT32 CRuCamera::ClipTest(REAL p, REAL q, REAL *u1, REAL *u2)
{
	// Return value is 'true' if line segment intersects the current test
	// plane.  Otherwise 'false' is returned in which case the line segment
	// is entirely clipped.
	if(0 == u1 || 0 == u2)
	{
		return 0;
	}

	if(p < 0.0f)
	{
		REAL r = q / p;

		if(r > (*u2))
		{
			return 0;
		}
		else
		{
			if(r > (*u1))
			{
				(*u1) = r;
			}

			return 1;
		}
	}
	else
	{
		if(p > 0.0f)
		{
			REAL r = q / p;
			if(r < (*u1))
			{
				return 0;
			}
			else
			{	
				if(r < (*u2))
				{
					(*u2) = r;
				}

				return 1;
			}
		}
		else
		{
			return q >= 0.0f;
		}
	}
}

INT32 CRuCamera::IntersectionLineAABox(const CRuVector3 &p, const CRuVector3 &dir, const CRuAABB &b, CRuVector3 &vOut)
{
	REAL t1 = 0.0f;
	REAL t2 = FLT_MAX;

	int intersect =	ClipTest(-dir[2], p[2] - b.Minima()[2], &t1, &t2) && ClipTest(dir[2], b.Maxima()[2] - p[2], &t1, &t2) &&
					ClipTest(-dir[1], p[1] - b.Minima()[1], &t1, &t2) && ClipTest(dir[1], b.Maxima()[1] - p[1], &t1, &t2) &&
					ClipTest(-dir[0], p[0] - b.Minima()[0], &t1, &t2) && ClipTest(dir[0], b.Maxima()[0] - p[0], &t1, &t2);

	if(!intersect)
	{
		return 0;
	}

	intersect = 0;
	if(0 <= t1)
	{
		LinCombVector3(p, dir, t1, vOut);
		intersect = 1;
	}

	if(0 <= t2)
	{
		LinCombVector3(p, dir, t2, vOut);
		intersect = 1;
	}

	return intersect;
}

void CRuCamera::CalculateViewFrustumPoints(CRuCamera *viewCamera, CRuVector3 *pointsOut)
{
	CRuAABB box(-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	// Build the canonical view volume in camera space
	pointsOut[0].Set(box.Minima().x, box.Minima().y, box.Minima().z);//     7+------+6
	pointsOut[1].Set(box.Maxima().x, box.Minima().y, box.Minima().z);//     /|     /|
	pointsOut[2].Set(box.Maxima().x, box.Maxima().y, box.Minima().z);//    / |    / |
	pointsOut[3].Set(box.Minima().x, box.Maxima().y, box.Minima().z);//   / 4+---/--+5
	pointsOut[4].Set(box.Minima().x, box.Minima().y, box.Maxima().z);// 3+------+2 /    y   z
	pointsOut[5].Set(box.Maxima().x, box.Minima().y, box.Maxima().z);//  | /    | /     |  /
	pointsOut[6].Set(box.Maxima().x, box.Maxima().y, box.Maxima().z);//  |/     |/      |/
	pointsOut[7].Set(box.Minima().x, box.Maxima().y, box.Maxima().z);// 0+------+1      *---x

	// Transform the canonical view volume into world space
	const CRuMatrix4x4 &invCameraTransform = viewCamera->GetInverseFrustumTransform();
	for(INT32 i = 0; i < 8; ++i)
	{
		invCameraTransform.TransformPointProjected(pointsOut[i]);
	}
}

void CRuCamera::CalculateViewFrustumGeometry(CRuVector3 *points, CRuArrayList<CRuArrayList<CRuVector3> *> &frustumGeometryOut)
{
	// Initialize six vector lists for each of the frustum sides
	for(INT32 i = 0; i < 6; ++i)
	{
		frustumGeometryOut.Add(ruNEW CRuArrayList<CRuVector3>());
	}

	// Near polygon counter-clockwise
	for(INT32 i = 0; i < 4; ++i)
	{
		frustumGeometryOut[0]->Add(points[i]);
	}

	// Far polygon counter-clockwise
	for(INT32 i = 0; i < 4; ++i)
	{
		frustumGeometryOut[1]->Add(points[7 - i]);
	}

	// Left polygon counter-clockwise
	frustumGeometryOut[2]->Add(points[0]);
	frustumGeometryOut[2]->Add(points[3]);
	frustumGeometryOut[2]->Add(points[7]);
	frustumGeometryOut[2]->Add(points[4]);

	// Right polygon counter-clockwise
	frustumGeometryOut[3]->Add(points[1]);
	frustumGeometryOut[3]->Add(points[5]);
	frustumGeometryOut[3]->Add(points[6]);
	frustumGeometryOut[3]->Add(points[2]);

	// Bottom polygon counter-clockwise
	frustumGeometryOut[4]->Add(points[4]);
	frustumGeometryOut[4]->Add(points[5]);
	frustumGeometryOut[4]->Add(points[1]);
	frustumGeometryOut[4]->Add(points[0]);

	// Top polygon counter-clockwise
	frustumGeometryOut[5]->Add(points[6]);
	frustumGeometryOut[5]->Add(points[7]);
	frustumGeometryOut[5]->Add(points[3]);
	frustumGeometryOut[5]->Add(points[2]);
}

void CRuCamera::IncludeObjectLightVolume(CRuCamera *viewCamera, CRuArrayList<CRuArrayList<CRuVector3> *> &frustumGeometry, CRuArrayList<CRuVector3> &pointsOut)
{
	const CRuAABB &sweepBounds = viewCamera->GetSweepBounds();
	CRuVector3 invLightDir = m_cameraLookAt * -1.0f;

	for(INT32 i = 0; i < frustumGeometry.Count(); ++i)
	{
		for(INT32 j = 0; j < frustumGeometry[i]->Count(); ++j)
		{
/*
			// This code makes no sense.. it does not calculate intersection body at all! everything turns out into a flat plane.
			CRuRay3 ray(frustumGeometry[i]->Get(j), invLightDir);
			CRuVector3 intPt;

			if(IntersectionLineAABox(ray.Origin(), ray.Direction(), sweepBounds, intPt))
			{
				pointsOut.Add(intPt);
			}
*/

			pointsOut.Add(frustumGeometry[i]->Get(j));

		}
	}
}

void CRuCamera::CalculateFocusedLightVolumePoints(CRuCamera *viewCamera, CRuArrayList<CRuVector3> &pointsOut)
{
	CRuVector3 points[8];
	CalculateViewFrustumPoints(viewCamera, points);

	CRuArrayList<CRuArrayList<CRuVector3> *> frustumGeometry;
	CalculateViewFrustumGeometry(points, frustumGeometry);

	IncludeObjectLightVolume(viewCamera, frustumGeometry, pointsOut);

/*
	Vector3x8 pts;
	struct Object obj = OBJECT_NULL;

	calcViewFrustumWorldCoord(pts,invEyeProjView);
	calcViewFrustObject(&obj, pts);
	clipObjectByAABox(&obj,sceneAABox);
	includeObjectLightVolume(points,obj,lightDir,sceneAABox);
	emptyObject(&obj);
*/
}

void CRuCamera::AppendToCubicHull(const CRuVector3 &v, CRuVector3 &minima, CRuVector3 &maxima)
{
	for(INT32 i = 0; i < 3; ++i)
	{
		if(v[i] < minima[i])
		{
			minima[i] = v[i];
		}
		else if(v[i] > maxima[i])
		{
			maxima[i] = v[i];
		}
	}
}

void CRuCamera::CalculateCubicHull(CRuArrayList<CRuVector3> &points, CRuVector3 &minima, CRuVector3 &maxima)
{
	if(points.Count() > 0)
	{
		minima = points[0];
		maxima = points[0];

		for(INT32 i = 1; i < points.Count(); ++i)
		{
			AppendToCubicHull(points[i], minima, maxima);
		}
	}
}

void CRuCamera::ScaleTranslateToFit(const CRuVector3 &vMin, const CRuVector3 &vMax, CRuMatrix4x4 &transformOut)
{
	transformOut._11 = 2.0f / (vMax[0] - vMin[0]);
	transformOut._21 = 0.0f;
	transformOut._31 = 0.0f;
	transformOut._41 = -(vMax[0] + vMin[0]) / (vMax[0] - vMin[0]);

	transformOut._12 = 0.0f;
	transformOut._22 = 2.0f / (vMax[1] - vMin[1]);
	transformOut._32 = 0.0f;
	transformOut._42 = -(vMax[1] + vMin[1]) / (vMax[1] - vMin[1]);

	transformOut._13 = 0.0f;
	transformOut._23 = 0.0f;
	transformOut._33 = 1.0f / (vMax[2] - vMin[2]);
	transformOut._43 = -vMin[2] / (vMax[2] - vMin[2]);

	transformOut._14 = 0.0f;
	transformOut._24 = 0.0f;
	transformOut._34 = 0.0f;
	transformOut._44 = 1.0f;
}

void CRuCamera::SetUniformSMProjection(CRuCamera *viewCamera, const CRuVector3 &lightVec)
{
	// Force view camera update
	viewCamera->GetFrustumTransform();

	// Calculates the view frustum object in world space. Clips this object by the view sweep bounds and extrudes the object into -lightDir.
	// The defining points are returned in points.
	CRuArrayList<CRuVector3> points;
	CalculateFocusedLightVolumePoints(viewCamera, points);

	// Light position = eye position
	m_cameraPos = viewCamera->m_cameraPos;

	// Set light target/direction
	m_targetPos = m_cameraPos + lightVec;

	// Up vector = view up
	m_cameraUp = viewCamera->GetCameraLookAtVector();

	// Update camera transforms
	UpdateBasis();
	UpdateTransforms();

	// Transform the view frustum points from world space into light space
	CRuArrayList<CRuVector3> lightSpacePoints;
	for(INT32 i = 0; i < points.Count(); ++i)
	{
		lightSpacePoints.Add(points[i]);
		m_cameraTransform.TransformPointProjected(lightSpacePoints[i]);
	}

	// Calculate the cubic hull (an AABB) of the light space extents of the intersection body B and save the two extreme points min and max.
	CRuVector3 minima, maxima;
	CalculateCubicHull(lightSpacePoints, minima, maxima);

	// Get scale transform
	CRuMatrix4x4 scaleTransform;
	ScaleTranslateToFit(minima, maxima, scaleTransform);

	m_cameraTransform = m_cameraTransform * scaleTransform;
	m_inverseCameraTransform = m_cameraTransform.GetInverse();

	m_projectionTransform.SetToIdentity();
	m_inverseCameraTransform.SetToIdentity();

	m_frustumTransform = m_cameraTransform;
	m_inverseFrustumTransform = m_inverseCameraTransform;
}

void CRuCamera::SetLiSPSMProjection(CRuCamera *viewCamera, const CRuVector3 &lightVec)
{
	// Force view camera update
	viewCamera->GetFrustumTransform();

	// Calculates the view frustum object in world space. Clips this object by the view sweep bounds and extrudes the object into -lightDir.
	// The defining points are returned in points.
	CRuArrayList<CRuVector3> points;
	CalculateFocusedLightVolumePoints(viewCamera, points);

	// Light position = eye position
	m_cameraPos = viewCamera->m_cameraPos;

	// Set light target/direction
	m_targetPos = m_cameraPos + lightVec;

	// Up vector = view up
	m_cameraUp = viewCamera->GetCameraLookAtVector();

	// Update camera transforms
	UpdateBasis();
	UpdateTransforms();

	// Transform the view frustum points from world space into light space
	CRuArrayList<CRuVector3> lightSpacePoints;
	for(INT32 i = 0; i < points.Count(); ++i)
	{
		lightSpacePoints.Add(points[i]);
		m_cameraTransform.TransformPointProjected(lightSpacePoints[i]);
	}

	// Calculate the cubic hull (an AABB) of the light space extents of the intersection body B and save the two extreme points min and max.
	CRuVector3 minima, maxima;
	CalculateCubicHull(lightSpacePoints, minima, maxima);

	CRuMatrix4x4 liSPSMTransform;
	liSPSMTransform.SetToIdentity();

	REAL dotProd = DotProduct(viewCamera->GetCameraLookAtVector(), lightVec * -1.0f);
	REAL sinGamma;
	REAL nearDist = 1.0f;

	sinGamma = RuSqrt(1.0f - dotProd * dotProd);

	{
		// use the formulas of the paper to get n (and f)
		const REAL factor = 1.0f / sinGamma;
		const REAL z_n = factor * nearDist;				// often 1 
		const REAL d = fabs(maxima[1] - minima[1]);		// perspective transform depth //light space y extents
		const REAL z_f = z_n + d * sinGamma;
		const REAL n = (z_n + RuSqrt(z_f * z_n)) / sinGamma;
		const REAL f = n + d;

		// Light position = eye position
		LinCombVector3(viewCamera->m_cameraPos, m_cameraUp, -(n - nearDist), m_cameraPos);

		// Set light target/direction
		m_targetPos = m_cameraPos + lightVec;

		// Up vector = view up
		m_cameraUp = viewCamera->GetCameraLookAtVector();

		// Update camera transforms
		UpdateBasis();
		UpdateTransforms();

		// one possibility for a simple perspective transformation matrix
		// with the two parameters n(near) and f(far) in y direction
		liSPSMTransform.SetToIdentity();					// a = (f + n) / (f - n); b = -2 * f * n / (f - n);

		liSPSMTransform._22 = (f + n) / (f - n);			// [ 1 0 0 0]
		liSPSMTransform._42 = -2.0f * f * n / (f - n);		// [ 0 a 0 b]
		liSPSMTransform._24 = 1.0f;							// [ 0 0 1 0]
		liSPSMTransform._44 = 0.0f;							// [ 0 1 0 0]

		// temporal arrangement for the transformation of the points to post-perspective space
		m_frustumTransform = m_cameraTransform * liSPSMTransform;

		// transform the light volume points from world into the distorted light space
		CRuArrayList<CRuVector3> distortedLightSpacePoints;
		for(INT32 i = 0; i < points.Count(); ++i)
		{
			distortedLightSpacePoints.Add(points[i]);
			m_frustumTransform.TransformPointProjected(distortedLightSpacePoints[i]);
		}

		// calculate the cubic hull (an AABB) 
		// of the light space extents of the intersection body B
		// and save the two extreme points min and max
		CalculateCubicHull(distortedLightSpacePoints, minima, maxima);
	}

	// Get scale transform
	CRuMatrix4x4 scaleTransform;
	ScaleTranslateToFit(minima, maxima, scaleTransform);

	m_projectionTransform = liSPSMTransform * scaleTransform;
	m_inverseProjectionTransform = m_projectionTransform.GetInverse();

	m_frustumTransform = m_cameraTransform * m_projectionTransform;
	m_inverseFrustumTransform = m_frustumTransform.GetInverse();
}

// isLeft(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2 on the line
//            <0 for P2 right of the line
inline REAL IsLeft(CRuVector3 &p0, CRuVector3 &p1, CRuVector3 &p2)
{
	return (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
}

// chainHull_2D(): Andrew's monotone chain 2D convex hull algorithm
//     Input:  points[] = an array of 2D points 
//                   presorted by increasing x- and y-coordinates
//             n = the number of points in points[]
//     Output: hullPointsOut[] = an array of the convex hull vertices (max is n)
//     Return: the number of points in hullPointsOut[]
INT32 ChainHull_2D(CRuVector3 *points, INT32 n, CRuVector3 *hullPointsOut)
{
	// the output array hullPointsOut[] will be used as the stack
	INT32 bot = 0, top = -1;	// indices for bottom and top of the stack
	INT32 i;					// array scan index

	// Get the indices of poINT32s with min x-coord and min|max y-coord
	INT32 minmin = 0, minmax;
	REAL xmin = points[0].x;

	for(i = 1; i < n; i++)
	{
		if(points[i].x != xmin)
			break;
	}

	minmax = i - 1;

	if(minmax == n - 1)
	{
		// degenerate case: all x-coords == xmin
		hullPointsOut[++top] = points[minmin];

		// a nontrivial segment
		if(points[minmax].y != points[minmin].y)
		{
			hullPointsOut[++top] = points[minmax];
		}

		// add polygon endpoint
		hullPointsOut[++top] = points[minmin];
		return top + 1;
	}

	// Get the indices of points with max x-coord and min|max y-coord
	INT32 maxmin, maxmax = n - 1;
	REAL xmax = points[n - 1].x;

	for(i = n - 2; i >= 0; i--)
	{
		if(points[i].x != xmax)
			break;
	}
	maxmin = i + 1;

	// Compute the lower hull on the stack hullPointsOut

	// push minmin point onto stack
	hullPointsOut[++top] = points[minmin];
	i = minmax;
	while(++i <= maxmin)
	{
		// the lower line joins points[minmin] with points[maxmin]
		if(IsLeft(points[minmin], points[maxmin], points[i]) >= 0 && i < maxmin)
		{
			continue;          // ignore points[i] above or on the lower line
		}

		while(top > 0)        // there are at least 2 points on the stack
		{
			// test if points[i] is left of the line at the stack top
			if(IsLeft(hullPointsOut[top - 1], hullPointsOut[top], points[i]) > 0)
			{
				break;         // points[i] is a new hull vertex
			}
			else
			{
				top--;         // pop top point off stack
			}
		}
		hullPointsOut[++top] = points[i];       // push points[i] onto stack
	}

	// Next, compute the upper hull on the stack hullPointsOut above the bottom hull
	if(maxmax != maxmin)      // if distinct xmax points
	{
		hullPointsOut[++top] = points[maxmax];  // push maxmax point onto stack
	}

	bot = top;                 // the bottom point of the upper hull stack
	i = maxmin;
	while(--i >= minmax)
	{
		// the upper line joins points[maxmax] with points[minmax]
		if(IsLeft( points[maxmax], points[minmax], points[i]) >= 0 && i > minmax)
		{
			continue;          // ignore points[i] below or on the upper line
		}

		while(top > bot)    // at least 2 poINT32s on the upper stack
		{
			// test if points[i] is left of the line at the stack top
			if(IsLeft(hullPointsOut[top - 1], hullPointsOut[top], points[i]) > 0)
			{
				break;         // points[i] is a new hull vertex
			}
			else
			{
				top--;         // pop top point off stack
			}
		}

		hullPointsOut[++top] = points[i];       // push points[i] onto stack
	}

	if(minmax != minmin)
	{
		hullPointsOut[++top] = points[minmin];  // push joining endpoint onto stack
	}

	return top + 1;
}

BOOL FindIntersection_2D(const CRuVector3 &p1, const CRuVector3 &p2, const CRuVector3 &p3, const CRuVector3 &p4, CRuVector3 &intPtOut)
{
	REAL ua_num = (p4.x - p3.x) * (p1.y - p3.y) - (p4.y - p3.y) * (p1.x - p3.x);
	REAL ub_num = (p2.x - p1.x) * (p1.y - p3.y) - (p2.y - p1.y) * (p1.x - p3.x);
	REAL denom = (p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y);

	if(abs(denom) <= ruEPSILON)
	{
		return FALSE;
	}

	REAL ua = ua_num / denom;
	REAL ub = ub_num / denom;

	intPtOut.x = p1.x + ua * (p2.x - p1.x);
	intPtOut.y = p1.y + ua * (p2.y - p1.y);

	return TRUE;
}

void CRuCamera::CalculateTrapezoid(CRuCamera *viewCamera, CRuVector3 &t0, CRuVector3 &t1, CRuVector3 &t2, CRuVector3 &t3, CRuVector3 &apexOut, BOOL worldSpace)
{
	REAL xValues[8];
	REAL yValues[8];

	// Get frustum points in world space
	CRuVector3 frustumPoints[9];
	CalculateViewFrustumPoints(viewCamera, frustumPoints);
	frustumPoints[8] = viewCamera->GetCameraPosition();

	// Transform frustum points into post-projection light space
	const CRuMatrix4x4 &lightTransform = GetFrustumTransform();
	for(INT32 i = 0; i < 9; ++i)
	{
		lightTransform.TransformPointProjected(frustumPoints[i]);
		frustumPoints[i].m_z = 0.0f;
	}

	// Derive the frustum's center line
	CRuVector3 topCenter = (frustumPoints[0] + frustumPoints[1] + frustumPoints[2] + frustumPoints[3]) / 4.0f;
	CRuVector3 baseCenter = (frustumPoints[4] + frustumPoints[5] + frustumPoints[6] + frustumPoints[7]) / 4.0f;
	CRuVector3 centerLine = baseCenter - topCenter;
	centerLine.Normalize();

	// Build transform to get center line's origin to coincide with light space origin and pointing toward negative y
	CRuMatrix4x4 alignTransform;
	alignTransform.SetToTranslation(-0.5f * (topCenter.x + baseCenter.x), -0.5f * (topCenter.y + baseCenter.y), 0.0f);

	CRuQuaternion quat;
	quat.RotateIntoVector(CRuVector3(0.0f, -1.0f, 0.0f), centerLine);
	alignTransform = alignTransform * quat.GetMatrix();

	// Transform frustum points
	for(INT32 i = 0; i < 9; ++i)
	{
		alignTransform.TransformPointProjected(frustumPoints[i]);
	}

	for(INT32 i = 0; i < 8; ++i)
	{
		xValues[i] = frustumPoints[i].m_x;
		yValues[i] = frustumPoints[i].m_y;
	}

	// Calculate 2D convex hull TODO: Use internal sorter
	RuGlobalRadixSort().Sort(yValues, 8, FALSE);
	RuGlobalRadixSort().Sort(xValues, 8, TRUE);

	CRuVector3 sortedFrustumPoints[8];
	for(INT32 i = 0; i < 8; ++i)
	{
		sortedFrustumPoints[i] = frustumPoints[RuGlobalRadixSort().GetSortedIndices()[i]];
	}

	CRuVector3 convexHullPoints[8];
	INT32 numHullPoints = ChainHull_2D(sortedFrustumPoints, 8, convexHullPoints);

	// Find top line y value by taking the maxima along the y axis
	// Find base line y value by taking the minima along the y axis
	REAL minima_Y = convexHullPoints[0].y;
	REAL maxima_Y = convexHullPoints[0].y;
	for(INT32 i = 1; i < numHullPoints; ++i)
	{
		minima_Y = min(minima_Y, convexHullPoints[i].y);
		maxima_Y = max(maxima_Y, convexHullPoints[i].y);
	}

	// Derive n (this is so magical) and q
	REAL epsilon = -0.6f;
	REAL lambda = maxima_Y - minima_Y;
	REAL delta = lambda * 0.2f;
	REAL tau = (lambda * delta + lambda * delta * epsilon) / (lambda - 2 * delta - lambda * epsilon);
	CRuVector3 q(0.0f, maxima_Y + tau, 0.0f);

	// Determine leftmost point and rightmost point to use
	REAL leftPtDot = 1.0f;
	REAL rightPtDot = 1.0f;
	CRuVector3 leftPt = convexHullPoints[0];
	CRuVector3 rightPt = convexHullPoints[0];
	for(INT32 i = 0; i < numHullPoints; ++i)
	{
		REAL dot = DotProduct((convexHullPoints[i] - q).Normalize(), CRuVector3(0.0f, -1.0f, 0.0f));

		if(convexHullPoints[i].x < q.x && dot < leftPtDot)
		{
			leftPtDot = dot;
			leftPt = convexHullPoints[i];
		}

		if(convexHullPoints[i].x > q.x && dot < rightPtDot)
		{
			rightPtDot = dot;
			rightPt = convexHullPoints[i];
		}
	}

	// Calculate points of the trapezoid
	FindIntersection_2D(q, leftPt, CRuVector3(0.0f, maxima_Y, 0.0f), CRuVector3(1.0f, maxima_Y, 0.0f), t3);
	FindIntersection_2D(q, rightPt, CRuVector3(0.0f, maxima_Y, 0.0f), CRuVector3(1.0f, maxima_Y, 0.0f), t2);
	FindIntersection_2D(q, leftPt, CRuVector3(0.0f, minima_Y, 0.0f), CRuVector3(1.0f, minima_Y, 0.0f), t0);
	FindIntersection_2D(q, rightPt, CRuVector3(0.0f, minima_Y, 0.0f), CRuVector3(1.0f, minima_Y, 0.0f), t1);

	// Transform output back into post-projection light space
	CRuMatrix4x4 invAlignTransform = alignTransform.GetInverse();
	invAlignTransform.TransformPointProjected(t3);
	invAlignTransform.TransformPointProjected(t2);
	invAlignTransform.TransformPointProjected(t0);
	invAlignTransform.TransformPointProjected(t1);
	invAlignTransform.TransformPointProjected(frustumPoints[8], apexOut);

	if(worldSpace)
	{
		t3.z = 0.1f;
		t2.z = 0.1f;
		t0.z = 0.1f;
		t1.z = 0.1f;

		// temp: transform into world space
		const CRuMatrix4x4 &invCameraTransform = GetInverseFrustumTransform();
		invCameraTransform.TransformPointProjected(t3);
		invCameraTransform.TransformPointProjected(t2);
		invCameraTransform.TransformPointProjected(t0);
		invCameraTransform.TransformPointProjected(t1);
		invCameraTransform.TransformPointProjected(apexOut);
	}
}

void CRuCamera::SetTSMProjection(CRuCamera *viewCamera)
{
	CRuVector3 t0, t1, t2, t3, apex;
	CRuMatrix4x4 projection;
	CRuMatrix4x4 matrix;

	// Make sure transforms are up-to-date
	UpdateTransforms();

	// Calculate trapezoid
	CalculateTrapezoid(viewCamera, t0, t1, t2, t3, apex);

	// Translate apex to light space origin
	CRuVector3 topLineCenter = (t2 + t3) / 2.0f;
	projection.SetToTranslation(-topLineCenter.x, -topLineCenter.y, 0.0f);

	// Align top line with light space x axis
	CRuVector3 topLine = t2 - t3;
	if(topLine.SquaredMagnitude() <= ruEPSILON)
	{
		return;
	}
	topLine.Normalize();

	CRuQuaternion quat;
	quat.RotateIntoVector(CRuVector3(-1.0f, 0.0f, 0.0f), topLine);
	projection = projection * quat.GetMatrix();

	// 3.
	if(!FindIntersection_2D(t0, t3, t1, t2, apex))
	{
		return;
	}

	projection.TransformPointProjected(apex);
	matrix.SetToTranslation(-apex.x, -apex.y, 0.0f);
	projection = projection * matrix;

	// 4.
	CRuVector3 p0 = (t2 + t3) / 2.0f;
	projection.TransformPointProjected(p0);
	matrix.SetToIdentity();
	matrix._21 = -p0.x / p0.y;
	projection = projection * matrix;

	// 5.
	projection.TransformPointProjected(t2, p0);
	matrix.SetToScaling(1.0f / p0.x, 1.0f / p0.y, 1.0f);
	projection = projection * matrix;

	// 6.
	matrix._11 = matrix._22 = matrix._33 = 1.0f;
	matrix._12 = matrix._13 = matrix._14 = matrix._21 = matrix._23 = 0.0f;
	matrix._31 = matrix._32 = matrix._34 = matrix._41 = matrix._43 = matrix._44 = 0.0f;
	matrix._42 = 1.0f;
	matrix._24 = 1.0f;
	projection = projection * matrix;

	// 7.
	CRuVector4 point1, point2;
	projection.TransformPoint(t0, point1);
	projection.TransformPoint(t2, point2);
	matrix.SetToTranslation(0.0f, -(point1.y / point1.w + point2.y / point2.w) / 2.0f, 0.0f);
	projection = projection * matrix;

	// 8.
	CRuVector4 point3;
	projection.TransformPoint(t0, point3);
	matrix.SetToIdentity();
	matrix._22 = -point3.w / point3.y;
	projection = projection * matrix;

	// Multiply N_T into projection
	m_projectionTransform = m_projectionTransform * projection;
	m_inverseProjectionTransform = m_projectionTransform.GetInverse();

	// Calculate frustum transform
	m_frustumTransform = m_cameraTransform * m_projectionTransform;

	// Calculate inverse frustum transform
	m_inverseFrustumTransform = m_inverseProjectionTransform * m_inverseCameraTransform;
}












const CRuPlane4 *CRuCamera::GetFrustumPlanes()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_frustumPlanes;
}

const CRuPlane4 *CRuCamera::GetBackdropFrustumPlanes()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_backdropFrustumPlanes;
}

const CRuPlane4 *CRuCamera::GetExtendedFrustumPlanes()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_extendedFrustumPlanes;
}

REAL CRuCamera::GetSweepRadius()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_sweepRadius;
}

REAL CRuCamera::GetBackdropSweepRadius()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_backdropSweepRadius;
}

REAL CRuCamera::GetBackdropExclusionRadius()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_backdropExclusionRadius;
}

REAL CRuCamera::GetBackdropNearDistance()
{
	return m_backdropNearDistance;
}

REAL CRuCamera::GetBackdropFarDistance()
{
	return m_backdropFarDistance;
}

const CRuAABB &CRuCamera::GetSweepBounds()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_sweepBounds;
}

const CRuAABB &CRuCamera::GetBackdropSweepBounds()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_backdropSweepBounds;
}

const CRuAABB &CRuCamera::GetBackdropExclusionBounds()
{
	if(m_transformsDirty)
		UpdateTransforms();

	return m_backdropExclusionBounds;
}

void CRuCamera::SetCameraPosition(const CRuVector3 &position)
{
	m_cameraPos = position;
	UpdateBasis();
}

void CRuCamera::SetTargetPosition(const CRuVector3 &position)
{
	m_targetPos = position;
	UpdateBasis();
}

void CRuCamera::SetCameraUpVector(const CRuVector3 &upVector)
{
	m_cameraUp = upVector;
	UpdateBasis();
}

void CRuCamera::Move(const CRuVector3 &movementVector, BOOL relativeMove)
{
	if(relativeMove)
	{
		// The movement vector is in local space
		// Calculate world space delta vector from the local space movement vector
		CRuVector3 deltaVector = m_cameraRight * movementVector.x + m_cameraUp * movementVector.y + m_cameraLookAt * movementVector.z;

		m_cameraPos += deltaVector;
		m_targetPos += deltaVector;
	}
	else
	{
		if(m_ySmoothEnabled)
		{
			m_targetY += movementVector.m_y;

			m_cameraPos += movementVector;
			m_targetPos += movementVector;

			m_cameraPos.m_y -= m_targetY;
			m_targetPos.m_y -= m_targetY;
		}
		else
		{
			m_cameraPos += movementVector;
			m_targetPos += movementVector;
		}
	}

	UpdateBasis();
}

void CRuCamera::MoveCamera(const CRuVector3 &movementVector, BOOL relativeMove)
{
	if(relativeMove)
	{
		// The movement vector is in local space
		// Calculate world space delta vector from the local space movement vector
		CRuVector3 deltaVector = m_cameraRight * movementVector.x + m_cameraUp * movementVector.y + m_cameraLookAt * movementVector.z;

		m_cameraPos += deltaVector;
	}
	else
	{
		m_cameraPos += movementVector;
	}

	UpdateBasis();
}

void CRuCamera::Cycle(REAL cycleAngle, const CRuVector3 &cycleAxis)
{
	CRuVector3 relativeCamPos;
	CRuQuaternion cycleQuat;
	CRuMatrix4x4 cycleMatrix;

	/*
	Steps to arbitrary cycle of camera position
		1. Construct quaternion for cycle
		2. Determine camera position relative to center of cycle
		3. Rotate the relative camera position
		4. Translate camera position by the center of cycle
	*/

	// Construct quaternion for cycle
	cycleQuat.FromAngleAxis(cycleAxis, cycleAngle);
	cycleQuat.GetMatrix(cycleMatrix);

	// Get the camera position relative to center of cycle
	relativeCamPos = m_cameraPos - m_targetPos;

	// Rotate camera
	relativeCamPos = relativeCamPos * cycleMatrix;

	// Rotate camera up vector
	m_cameraUp = m_cameraUp * cycleMatrix;

	// Calculate absolute camera position
	m_cameraPos = relativeCamPos + m_targetPos;

	UpdateBasis();
}

void CRuCamera::Rotate(REAL rotationAngle, const CRuVector3 &rotationAxis)
{
	CRuVector3 relativeTargetPos;
	CRuQuaternion rotationQuat;
	CRuMatrix4x4 rotationMatrix;

	/*
	Steps to arbitrary rotation of camera position
		1. Construct quaternion for rotation
		2. Determine camera position relative to center of rotation
		3. Rotate the relative camera position
		4. Translate camera position by the center of rotation
	*/

	// Construct quaternion for rotation
	rotationQuat.FromAngleAxis(rotationAxis, rotationAngle);
	rotationQuat.GetMatrix(rotationMatrix);

	// Get the target position relative to center of rotation
	relativeTargetPos = m_targetPos - m_cameraPos;

	// Rotate target position
	relativeTargetPos = relativeTargetPos * rotationMatrix;

	// Rotate camera up vector
	m_cameraUp = m_cameraUp * rotationMatrix;

	// Calculate absolute target position
	m_targetPos = relativeTargetPos + m_cameraPos;

	UpdateBasis();
}

void CRuCamera::Reflect(BOOL reflectOn, const CRuPlane4 &reflectPlane)
{
	m_reflectCamera = reflectOn;
	m_reflectPlane = reflectPlane;

	m_transformsDirty = TRUE;
}

void CRuCamera::Jiggle(REAL strength, REAL springConstant, REAL dampingFactor)
{
	m_jiggle_Direction.Set(0.0f, 1.0f, 0.0f);
	m_jiggle_Direction = m_jiggle_Direction;

	m_jiggle_m = 1.0f;
	m_jiggle_R = 0.0f;
	m_jiggle_k = springConstant;
	m_jiggle_b = dampingFactor;

	m_rk4Solver->SetValue(1, strength);
}

BOOL CRuCamera::Coord_ScreenToHomogeneous(INT32 sX, INT32 sY, INT32 viewportWidth, INT32 viewportHeight, CRuVector3 &ptOut)
{
	ptOut.x = (sX * 2.0f / viewportWidth - 1.0f);
	ptOut.y = -(sY * 2.0f / viewportHeight - 1.0f);
	ptOut.z = 1.0f;

	return TRUE;
}

BOOL CRuCamera::Coord_HomogeneousToView(CRuVector3 &pt, CRuVector3 &ptOut)
{
	if(m_transformsDirty)
		UpdateTransforms();

	ptOut.x = pt.x / m_projectionTransform._11;
	ptOut.y = pt.y / m_projectionTransform._22;
	ptOut.z = pt.z;

	return TRUE;
}

BOOL CRuCamera::Coord_ViewToWorld(CRuVector3 &pt, CRuVector3 &ptOut)
{
	if(m_transformsDirty)
		UpdateTransforms();

	ptOut.x = pt.x;		ptOut.y = pt.y;		ptOut.z = pt.z;
	m_inverseCameraTransform.TransformPoint(ptOut);

	return TRUE;
}

/*!
	Gets the scale ratio between screen and world space coordinate systems at
	the specified point. This works by creating a unit-length vector in screen
	space, and then transforming this screen space unit-length vector into world
	space. This vector is then used to form a segment in world space, which is
	finally projected back to screen space. The length of the transformed segment
	is the SCREEN:WORLD ratio.

	@param origin Location of the world space point to find the scale ratio for.
	@return The SCREEN:WORLD scale ratio at the specified location.

*/
REAL CRuCamera::Utility_GetScreenToWorldScale(const CRuVector3 &origin)
{
	// Update transforms if necessary
	if(m_transformsDirty)
		UpdateTransforms();

	// Get the screen space side vector into world space
	CRuVector3 worldSideVec(1.0f, 0.0f, 0.0f);
	m_inverseFrustumTransform.TransformVectorProjected(worldSideVec);
	worldSideVec.Normalize();

	// Get the two points that make up the unit segment in world space
	CRuVector3 worldPt0(origin);
	CRuVector3 worldPt1 = worldPt0 + worldSideVec;

	// Transform the world space unit segment end-points into screen space
	CRuVector3 screenPt0, screenPt1;

	m_frustumTransform.TransformPointProjected(worldPt0, screenPt0);
	m_frustumTransform.TransformPointProjected(worldPt1, screenPt1);

	// Return the length of the unit segment
	return (screenPt0 - screenPt1).Magnitude();
}

void CRuCamera::Utility_GetBillboardTransform(const CRuVector3 &geometryForward, const CRuVector3 &geometryUp, CRuMatrix4x4 &matOut)
{
	CRuVector3 geometrySide;
	CRuVector3 invViewDir, viewUp, ivXd;

	// Figure out the geometry's side vector
	geometrySide = CrossProduct(geometryForward, geometryUp);

	// Get camera forward and up vectors
	invViewDir = m_cameraLookAt;
	viewUp = m_cameraUp;

	// Invert camera forward vector
	invViewDir.x = -invViewDir.x;
	invViewDir.y = -invViewDir.y;
	invViewDir.z = -invViewDir.z;

	// Calculate side vector
	ivXd = CrossProduct(invViewDir, viewUp);

	// Build normalizing matrix
	CRuMatrix4x4 normMatrix;
	normMatrix._11 = geometryForward.x;		normMatrix._12 = geometryUp.x;			normMatrix._13 = geometrySide.x;	normMatrix._14 = 0.0f;
	normMatrix._21 = geometryForward.y;		normMatrix._22 = geometryUp.y;			normMatrix._23 = geometrySide.y;	normMatrix._24 = 0.0f;
	normMatrix._31 = geometryForward.z;		normMatrix._32 = geometryUp.z;			normMatrix._33 = geometrySide.z;	normMatrix._34 = 0.0f;
	normMatrix._41 = 0.0f;					normMatrix._42 = 0.0f;					normMatrix._43 = 0.0f;				normMatrix._44 = 1.0f;

	// Build billboard matrix (Transforms +x axis into -z axis)
	CRuMatrix4x4 bbMatrix;
	bbMatrix._11 = invViewDir.x;			bbMatrix._12 = invViewDir.y;			bbMatrix._13 = invViewDir.z;		bbMatrix._14 = 0.0f;
	bbMatrix._21 = viewUp.x;				bbMatrix._22 = viewUp.y;				bbMatrix._23 = viewUp.z;			bbMatrix._24 = 0.0f;
	bbMatrix._31 = ivXd.x;					bbMatrix._32 = ivXd.y;					bbMatrix._33 = ivXd.z;				bbMatrix._34 = 0.0f;
	bbMatrix._41 = 0.0f;					bbMatrix._42 = 0.0f;					bbMatrix._43 = 0.0f;				bbMatrix._44 = 1.0f;

	// Output billboard matrix
	matOut = normMatrix * bbMatrix;
}

BOOL CRuCamera::UpdateBasis()
{
	CRuVector3 xBasis, yBasis, zBasis;

	// Calculate the z basis vector (camera look-at)
	zBasis = m_targetPos - m_cameraPos;

	// Make sure the z basis vector is not almost zero
	if(zBasis.Magnitude() < ruEPSILON)
		return FALSE;

	// Normalize Z basis vector
	zBasis.Normalize();

	// Calculate the x basis vector (camera right)
	xBasis.SetToCrossProduct(m_cameraUp, zBasis);

	// If the x basis vector is near zero, that means the z basis vector is almost parallel
	// with the current camera up vector, so find a different up vector
	if(xBasis.Magnitude() < ruEPSILON)
	{
		// Try a different up vector
		m_cameraUp = CRuVector3(1.0f, 0.0f, 0.0f);

		// Calculate the x basis vector (camera right)
		xBasis.SetToCrossProduct(m_cameraUp, zBasis);

		// If the magnitude of the x basis vector is still near zero, try another up vector
		if(xBasis.Magnitude() < ruEPSILON)
		{
			m_cameraUp = CRuVector3(0.0f, 1.0f, 0.0f);

			// Calculate the x basis vector (camera right)
			xBasis.SetToCrossProduct(m_cameraUp, zBasis);
		}
	}

	// Normalize the x basis vector
	xBasis.Normalize();

	// Find the correct camera up vector
	yBasis.SetToCrossProduct(zBasis, xBasis);
	yBasis.Normalize();

	// Save the orientation vectors
	m_cameraRight = xBasis;
	m_cameraUp = yBasis;
	m_cameraLookAt = zBasis;

	// Mark transforms as dirty
	m_transformsDirty = TRUE;

	return TRUE;
}

BOOL CRuCamera::UpdateTransforms()
{
	CRuVector3 xBasis(m_cameraRight), yBasis(m_cameraUp), zBasis(m_cameraLookAt);

	// Reflect the camera transform if necessary
	if(m_reflectCamera)
	{
		CRuMatrix4x4 translationMatrix, reflectionMatrix;

		reflectionMatrix.SetToReflection(m_reflectPlane);

		// Build camera translation (transform camera to origin)
		translationMatrix.SetToTranslation(-m_cameraPos.x, -m_cameraPos.y, -m_cameraPos.z);

		// Build the camera transform
		m_cameraTransform.SetToIdentity();
		m_cameraTransform._11 = xBasis.x; m_cameraTransform._12 = yBasis.x; m_cameraTransform._13 = zBasis.x;
		m_cameraTransform._21 = xBasis.y; m_cameraTransform._22 = yBasis.y; m_cameraTransform._23 = zBasis.y;
		m_cameraTransform._31 = xBasis.z; m_cameraTransform._32 = yBasis.z; m_cameraTransform._33 = zBasis.z;
		m_cameraTransform = reflectionMatrix * translationMatrix * m_cameraTransform;

		// Build the inverse camera transform
		m_inverseCameraTransform = m_cameraTransform.GetInverse();

/*
		TODO: Find out which of these functions is broken
		m_cameraTransform.SetToReflection(m_reflectPlane);
		m_cameraTransform.Translate(CRuVector3(-m_cameraPos.x, -m_cameraPos.y, -m_cameraPos.z));
		m_cameraTransform.ChangeBasis(xBasis, yBasis, zBasis);

		m_inverseCameraTransform = m_cameraTransform.GetInverse();
*/
	}
	else
	{
		CRuMatrix4x4 offsetMatrix;
		offsetMatrix.SetToTranslation(m_jiggle_Offset.m_x, m_jiggle_Offset.m_y, m_jiggle_Offset.m_z);

		CRuMatrix4x4 translationMatrix;

		// Build camera translation (transform camera to origin)
		translationMatrix.SetToTranslation(-m_cameraPos.x, -m_cameraPos.y, -m_cameraPos.z);

		// Build the camera transform
		m_cameraTransform.SetToIdentity();
		m_cameraTransform._11 = xBasis.x; m_cameraTransform._12 = yBasis.x; m_cameraTransform._13 = zBasis.x;
		m_cameraTransform._21 = xBasis.y; m_cameraTransform._22 = yBasis.y; m_cameraTransform._23 = zBasis.y;
		m_cameraTransform._31 = xBasis.z; m_cameraTransform._32 = yBasis.z; m_cameraTransform._33 = zBasis.z;
//		m_cameraTransform = translationMatrix * m_cameraTransform;
		m_cameraTransform = translationMatrix * m_cameraTransform * offsetMatrix;

		m_inverseCameraTransform = m_cameraTransform.GetInverse();
/*
		// Build the inverse camera transform
		m_inverseCameraTransform.SetToIdentity();
		m_inverseCameraTransform._11 = xBasis.x; m_inverseCameraTransform._12 = xBasis.y; m_inverseCameraTransform._13 = xBasis.z;
		m_inverseCameraTransform._21 = yBasis.x; m_inverseCameraTransform._22 = yBasis.y; m_inverseCameraTransform._23 = yBasis.z;
		m_inverseCameraTransform._31 = zBasis.x; m_inverseCameraTransform._32 = zBasis.y; m_inverseCameraTransform._33 = zBasis.z;
		m_inverseCameraTransform._41 = m_cameraPos.x;
		m_inverseCameraTransform._42 = m_cameraPos.y;
		m_inverseCameraTransform._43 = m_cameraPos.z;
*/
	}

	// Build the projection transform
	m_projectionTransform.SetToIdentity();
	if(m_parallelProjection)
	{
		m_projectionTransform._11 = 2.0f / m_viewportWidth;
		m_projectionTransform._22 = 2.0f / m_viewportHeight;
		m_projectionTransform._33 = 1.0f / (m_farPlaneDistance - m_nearPlaneDistance);
		m_projectionTransform._43 = m_nearPlaneDistance / (m_nearPlaneDistance - m_farPlaneDistance);
		m_projectionTransform._44 = 1.0f;

		if(m_orientation != 0.0f)
		{
			CRuMatrix4x4 arf;
			arf.SetToIdentity();
			arf._13 = cos(m_orientation);
			arf._23 = sin(m_orientation);

			m_projectionTransform = m_projectionTransform * arf;
		}
	}
	else
	{
		REAL w = 2.0f * m_nearPlaneDistance / m_viewportWidth;
		REAL h = 2.0f * m_nearPlaneDistance / m_viewportHeight;

		m_projectionTransform._11 = w;
		m_projectionTransform._22 = h;
		m_projectionTransform._33 = m_farPlaneDistance / (m_farPlaneDistance - m_nearPlaneDistance);
		m_projectionTransform._34 = 1.0f;
		m_projectionTransform._43 = -1.0f * m_nearPlaneDistance * m_farPlaneDistance / (m_farPlaneDistance - m_nearPlaneDistance);
		m_projectionTransform._44 = 0.0f;
	}

	// Calculate the inverse projection transform
	m_inverseProjectionTransform = m_projectionTransform.GetInverse();

	// Calculate frustum transform
	m_frustumTransform = m_cameraTransform * m_projectionTransform;

	// Calculate inverse frustum transform
	m_inverseFrustumTransform = m_inverseProjectionTransform * m_inverseCameraTransform;

	// Store frustum planes
	GetFrustumPlanes(m_frustumTransform, m_frustumPlanes);

	// Build the backdrop projection transform
	if(m_backdropEnabled)
	{
		m_backdropProjectionTransform.SetToIdentity();
		if(m_parallelProjection)
		{
			m_backdropProjectionTransform._11 = 2.0f / m_viewportWidth;
			m_backdropProjectionTransform._22 = 2.0f / m_viewportHeight;
			m_backdropProjectionTransform._33 = 1.0f / (m_backdropFarDistance - m_nearPlaneDistance);
			m_backdropProjectionTransform._43 = m_nearPlaneDistance / (m_nearPlaneDistance - m_backdropFarDistance);
			m_backdropProjectionTransform._44 = 1.0f;
		}
		else
		{

			REAL w = 2.0f * m_nearPlaneDistance / m_viewportWidth;
			REAL h = 2.0f * m_nearPlaneDistance / m_viewportHeight;

			m_backdropProjectionTransform._11 = w;
			m_backdropProjectionTransform._22 = h;
			m_backdropProjectionTransform._33 = m_backdropFarDistance / (m_backdropFarDistance - m_backdropNearDistance);
			m_backdropProjectionTransform._34 = 1.0f;
			m_backdropProjectionTransform._43 = -1.0f * m_backdropNearDistance * m_backdropFarDistance / (m_backdropFarDistance - m_backdropNearDistance);
			m_backdropProjectionTransform._44 = 0.0f;

			m_extendedProjectionTransform._11 = w;
			m_extendedProjectionTransform._22 = h;
			m_extendedProjectionTransform._33 = m_backdropFarDistance / (m_backdropFarDistance - m_nearPlaneDistance);
			m_extendedProjectionTransform._34 = 1.0f;
			m_extendedProjectionTransform._43 = -1.0f * m_nearPlaneDistance * m_backdropFarDistance / (m_backdropFarDistance - m_nearPlaneDistance);
			m_extendedProjectionTransform._44 = 0.0f;

/*
			REAL w = 2.0f * m_nearPlaneDistance / m_viewportWidth;
			REAL h = 2.0f * m_nearPlaneDistance / m_viewportHeight;

			m_backdropProjectionTransform._11 = w;
			m_backdropProjectionTransform._22 = h;
			m_backdropProjectionTransform._33 = m_backdropFarDistance / (m_backdropFarDistance - m_nearPlaneDistance);
			m_backdropProjectionTransform._34 = 1.0f;
			m_backdropProjectionTransform._43 = -1.0f * m_nearPlaneDistance * m_backdropFarDistance / (m_backdropFarDistance - m_nearPlaneDistance);
			m_backdropProjectionTransform._44 = 0.0f;
*/
		}

		// Calculate backdrop frustum transform
		m_backdropFrustumTransform = m_cameraTransform * m_backdropProjectionTransform;
		m_extendedFrustumTransform = m_cameraTransform * m_extendedProjectionTransform;

		// Store frustum planes
		GetFrustumPlanes(m_backdropFrustumTransform, m_backdropFrustumPlanes);
		GetFrustumPlanes(m_extendedFrustumTransform, m_extendedFrustumPlanes);
	}
	else
	{
		REAL w = 2.0f * m_nearPlaneDistance / m_viewportWidth;
		REAL h = 2.0f * m_nearPlaneDistance / m_viewportHeight;

		m_extendedProjectionTransform._11 = w;
		m_extendedProjectionTransform._22 = h;
		m_extendedProjectionTransform._33 = m_farPlaneDistance / (m_farPlaneDistance - m_nearPlaneDistance);
		m_extendedProjectionTransform._34 = 1.0f;
		m_extendedProjectionTransform._43 = -1.0f * m_nearPlaneDistance * m_farPlaneDistance / (m_farPlaneDistance - m_nearPlaneDistance);
		m_extendedProjectionTransform._44 = 0.0f;

		// Calculate backdrop frustum transform
		m_extendedFrustumTransform = m_cameraTransform * m_extendedProjectionTransform;

		// Store frustum planes
		GetFrustumPlanes(m_extendedFrustumTransform, m_extendedFrustumPlanes);
	}

	if(m_parallelProjection)
	{
		REAL halfViewportWidth = m_viewportWidth / 2.0f;
		REAL halfViewportHeight = m_viewportHeight / 2.0f;

		m_sweepRadius = max(m_farPlaneDistance, max(halfViewportWidth, halfViewportHeight));
		m_backdropSweepRadius = m_sweepRadius;
		m_backdropExclusionRadius= m_sweepRadius;
	}
	else
	{
		REAL halfViewportWidth = m_viewportWidth / 2.0f;
		REAL halfViewportHeight = m_viewportHeight / 2.0f;

		REAL v0 = max(halfViewportWidth, halfViewportHeight);
		REAL d0 = max(1.0f, m_nearPlaneDistance);
		REAL h0 = RuSqrt(v0 * v0 + d0 * d0);

		REAL hRatio = h0 / d0;

		m_sweepRadius = hRatio * m_farPlaneDistance;
		m_backdropSweepRadius = hRatio * m_backdropFarDistance;
		m_backdropExclusionRadius = hRatio * m_backdropNearDistance;
	}

	// Update sweep bounds
	m_sweepBounds.Set(	m_cameraPos.x - m_sweepRadius, m_cameraPos.y - m_sweepRadius, m_cameraPos.z - m_sweepRadius,
						m_cameraPos.x + m_sweepRadius, m_cameraPos.y + m_sweepRadius, m_cameraPos.z + m_sweepRadius);

	m_backdropSweepBounds.Set(	m_cameraPos.x - m_backdropSweepRadius, m_cameraPos.y - m_backdropSweepRadius, m_cameraPos.z - m_backdropSweepRadius,
								m_cameraPos.x + m_backdropSweepRadius, m_cameraPos.y + m_backdropSweepRadius, m_cameraPos.z + m_backdropSweepRadius);

	m_backdropExclusionBounds.Set(	m_cameraPos.x - m_backdropExclusionRadius, m_cameraPos.y - m_backdropExclusionRadius, m_cameraPos.z - m_backdropExclusionRadius,
									m_cameraPos.x + m_backdropExclusionRadius, m_cameraPos.y + m_backdropExclusionRadius, m_cameraPos.z + m_backdropExclusionRadius);

	m_transformsDirty = FALSE;

	return TRUE;
}

BOOL CRuCamera::GetFrustumPlanes(const CRuMatrix4x4 &frustumMatrix, CRuPlane4 *planesOut)
{
	// Left clipping plane
	planesOut[0].A = frustumMatrix._14 + frustumMatrix._11;
	planesOut[0].B = frustumMatrix._24 + frustumMatrix._21;
	planesOut[0].C = frustumMatrix._34 + frustumMatrix._31;
	planesOut[0].D = frustumMatrix._44 + frustumMatrix._41;

	// Right clipping plane
	planesOut[1].A = frustumMatrix._14 - frustumMatrix._11;
	planesOut[1].B = frustumMatrix._24 - frustumMatrix._21;
	planesOut[1].C = frustumMatrix._34 - frustumMatrix._31;
	planesOut[1].D = frustumMatrix._44 - frustumMatrix._41;

	// Top clipping plane
	planesOut[2].A = frustumMatrix._14 - frustumMatrix._12;
	planesOut[2].B = frustumMatrix._24 - frustumMatrix._22;
	planesOut[2].C = frustumMatrix._34 - frustumMatrix._32;
	planesOut[2].D = frustumMatrix._44 - frustumMatrix._42;

	// Bottom clipping plane
	planesOut[3].A = frustumMatrix._14 + frustumMatrix._12;
	planesOut[3].B = frustumMatrix._24 + frustumMatrix._22;
	planesOut[3].C = frustumMatrix._34 + frustumMatrix._32;
	planesOut[3].D = frustumMatrix._44 + frustumMatrix._42;

	// Near clipping plane
	planesOut[4].A = frustumMatrix._13;
	planesOut[4].B = frustumMatrix._23;
	planesOut[4].C = frustumMatrix._33;
	planesOut[4].D = frustumMatrix._43;

	// Far clipping plane
	planesOut[5].A = frustumMatrix._14 - frustumMatrix._13;
	planesOut[5].B = frustumMatrix._24 - frustumMatrix._23;
	planesOut[5].C = frustumMatrix._34 - frustumMatrix._33;
	planesOut[5].D = frustumMatrix._44 - frustumMatrix._43;

	// Normalize planesOut
	planesOut[0].Normalize();
	planesOut[1].Normalize();
	planesOut[2].Normalize();
	planesOut[3].Normalize();
	planesOut[4].Normalize();
	planesOut[5].Normalize();

	return TRUE;
}

CRuCamera &CRuCamera::operator=(const CRuCamera &camera)
{
	m_parallelProjection = camera.m_parallelProjection;
	m_nearPlaneDistance = camera.m_nearPlaneDistance;
	m_farPlaneDistance = camera.m_farPlaneDistance;
	m_viewportWidth = camera.m_viewportWidth;
	m_viewportHeight = camera.m_viewportHeight;
	m_aspectRatio = camera.m_aspectRatio;

	m_sweepRadius = camera.m_sweepRadius;
	m_sweepBounds = camera.m_sweepBounds;

	m_cameraPos = camera.m_cameraPos;
	m_targetPos = camera.m_targetPos;
	m_cameraRight = camera.m_cameraRight;
	m_cameraUp = camera.m_cameraUp;
	m_cameraLookAt = camera.m_cameraLookAt;

	m_reflectCamera = camera.m_reflectCamera;
	m_reflectPlane = camera.m_reflectPlane;

	m_transformsDirty = camera.m_transformsDirty;
	m_projectionTransform = camera.m_projectionTransform;
	m_inverseProjectionTransform = camera.m_inverseProjectionTransform;
	m_cameraTransform = camera.m_cameraTransform;
	m_inverseCameraTransform = camera.m_inverseCameraTransform;
	m_frustumTransform = camera.m_frustumTransform;
	m_inverseFrustumTransform = camera.m_inverseFrustumTransform;

	for(INT32 i = 0; i < 6; ++i)
	{
		m_frustumPlanes[i] = camera.m_frustumPlanes[i];
	}

	return *this;
}

// ************************************************************************************************************************************************************

#pragma managed
