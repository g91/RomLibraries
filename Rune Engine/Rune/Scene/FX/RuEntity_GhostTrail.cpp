/*!
	@file RuEntity_GhostTrail.cpp

	Copyright (c) 2004-2006 All rights reserved
*/

#include "../../Scene/FX/RuEntity_GhostTrail.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_GhostTrail, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_GhostTrail, "CRuEntity_GhostTrail", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuEntity_GhostTrail, 0x47DE9263, 0x1FF14be3, 0xBD7156BD, 0x9DE5763C)

// ************************************************************************************************************************************************************

CRuEntity_GhostTrail::CRuEntity_GhostTrail()
:	m_frameType(ruFRAMETYPE_LOCAL),
	m_tInterval(0.025f),
	m_segmentInterval(0.016f),
	m_maxAge(1.0f),
	m_maxControlPoints(32),
	m_maxTrailSegments(32),
	m_activeDuration(0.0f),
	m_fadeOutDuration(0.0f),
	m_trailTextureFrameCount(1),
	m_trailTextureFrameRate(30.0f),
	m_numSubdivisions(8),
	m_numControlPoints(0),
	m_controlSegmentHead(0),
	m_controlSegmentTail(0),
	m_controlPoints(NULL),
	m_numSegments(0),
	m_segments(NULL),
	m_anchor0(NULL),
	m_anchor1(NULL),
	m_t(0.0f),
	m_accumT(0.0f),
	m_trailAccumT(0.0f),
	m_trailTextureFrameIndex(0),
	m_alphaFactor(1.0f),
	m_visible(FALSE),
	m_mesh(NULL),
	m_material(NULL)
{
	INT32 maxVertices = m_maxTrailSegments * m_numSubdivisions;
	INT32 maxPrimitives = 2 * (m_numSubdivisions - 1) * (m_maxTrailSegments - 1);

	m_controlPoints = ruNEW ControlPointDesc [m_maxControlPoints];
	for(INT32 i = 0; i < m_maxControlPoints; ++i)
	{
		m_controlPoints[i].m_age = 0.0f;
	}

	m_segments = ruNEW ControlPointDesc [m_maxTrailSegments];
	for(INT32 i = 0; i < m_maxTrailSegments; ++i)
	{
		m_segments[i].m_age = 0.0f;
	}

	m_mesh = ruNEW CRuDynamicMesh();
	m_mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, maxVertices, maxPrimitives, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);
	m_mesh->AddTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
	m_mesh->SetNumPrimitives(0);
	m_mesh->SetNumVertices(0);

	m_material = ruNEW CRuMaterial();
	m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_FX_GHOSTTRAIL"));
}

CRuEntity_GhostTrail::~CRuEntity_GhostTrail()
{
	delete[] m_controlPoints;
	delete[] m_segments;

	ruSAFE_RELEASE(m_anchor0);
	ruSAFE_RELEASE(m_anchor1);
	ruSAFE_RELEASE(m_mesh);
	ruSAFE_RELEASE(m_material);
}

void CRuEntity_GhostTrail::SetFrameType(RuFrameType frameType)
{
	m_frameType = frameType;
}

void CRuEntity_GhostTrail::SetUpdateInterval(REAL updateInterval)
{
	m_tInterval = updateInterval;
}

void CRuEntity_GhostTrail::SetSegmentInterval(REAL segmentInterval)
{
	m_segmentInterval = segmentInterval;
}

void CRuEntity_GhostTrail::SetMaxAge(REAL maxAge)
{
	m_maxAge = maxAge;
}

void CRuEntity_GhostTrail::SetFadeCoefficients(REAL linear, REAL quadratic, REAL cubic)
{
	m_fadeCoefficients[0] = linear;
	m_fadeCoefficients[1] = quadratic;
	m_fadeCoefficients[2] = cubic;
}

void CRuEntity_GhostTrail::SetMaxControlPoints(INT32 maxControlPoints)
{
	delete[] m_controlPoints;

	m_numControlPoints = 0;
	m_controlSegmentHead = 0;
	m_controlSegmentTail = 0;
	m_maxControlPoints = maxControlPoints;

	m_controlPoints = ruNEW ControlPointDesc [m_maxControlPoints];
	for(INT32 i = 0; i < m_maxControlPoints; ++i)
	{
		m_controlPoints[i].m_age = 0.0f;
	}
}

void CRuEntity_GhostTrail::SetMaxSegments(INT32 maxSegments)
{
	ControlPointDesc *newSegments = ruNEW ControlPointDesc [maxSegments];
	for(INT32 i = 0; i < maxSegments; ++i)
	{
		newSegments[i].m_age = 0.0f;
	}

	if(m_segments)
	{
		for(INT32 i = 0; i < min(m_maxTrailSegments, maxSegments); ++i)
		{
			newSegments[i] = m_segments[i];
		}
	}

	delete[] m_segments;
	ruSAFE_RELEASE(m_mesh);

	m_segments = newSegments;
	m_maxTrailSegments = maxSegments;

	INT32 maxVertices = m_maxTrailSegments * m_numSubdivisions;
	INT32 maxPrimitives = 2 * (m_numSubdivisions - 1) * (m_maxTrailSegments - 1);

	m_mesh = ruNEW CRuDynamicMesh();
	m_mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, maxVertices, maxPrimitives, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);
	m_mesh->AddTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
	m_mesh->SetNumPrimitives(0);
	m_mesh->SetNumVertices(0);
}

void CRuEntity_GhostTrail::SetActiveDuration(REAL duration)
{
	m_activeDuration = duration;
}

void CRuEntity_GhostTrail::SetFadeOutDuration(REAL duration)
{
	m_fadeOutDuration = duration;
}

BOOL CRuEntity_GhostTrail::SetTrailAnchors(CRuEntity *p0, CRuEntity *p1)
{
	// Make sure input is valid
	if(p0 == NULL || p1 == NULL)
	{
		return FALSE;
	}

	// Release previous anchor entities
	ruSAFE_RELEASE(m_anchor0);
	ruSAFE_RELEASE(m_anchor1);

	// Assign anchor
	m_anchor0 = p0;
	m_anchor1 = p1;

	// Increment references
	m_anchor0->AddRef();
	m_anchor1->AddRef();

	return TRUE;
}

BOOL CRuEntity_GhostTrail::SetTrailTexture(IRuTexture *texture)
{
	if(texture)
	{
		m_material->SetTexture(ruTEXCHANNEL_DIFFUSEMAP, texture);
	}

	return TRUE;
}

void CRuEntity_GhostTrail::SetTrailFrameCount(INT32 frameCount)
{
	m_trailTextureFrameCount = frameCount;
}

void CRuEntity_GhostTrail::SetTrailFrameRate(REAL frameRate)
{
	m_trailTextureFrameRate = frameRate;
}

CRuEntity *CRuEntity_GhostTrail::Clone(CRuEntity *clonedEntity)
{
	return NULL;
}

BOOL CRuEntity_GhostTrail::Update(REAL elapsedTime)
{
	m_t += elapsedTime;
	m_accumT += elapsedTime;

	REAL frameTime = 1.0f / m_trailTextureFrameRate;
	m_trailAccumT += elapsedTime;
	while(m_trailAccumT >= frameTime)
	{
		m_trailAccumT -= frameTime;
		m_trailTextureFrameIndex = (m_trailTextureFrameIndex + 1) % m_trailTextureFrameCount;
	}

	// Update when we hit the specified update interval
	if(m_accumT >= m_tInterval)
	{
		// Generate new control points
		if(m_t < m_activeDuration || m_activeDuration == 0.0f)
		{
			// Find tail of the segment
			m_controlSegmentTail = (m_controlSegmentHead + m_numControlPoints) % m_maxControlPoints;

			// Insert new segment
			m_controlPoints[m_controlSegmentTail].m_age = -m_accumT;
			m_controlPoints[m_controlSegmentTail].m_u = 1.0f;
			m_controlPoints[m_controlSegmentTail].m_alpha = 1.0f;
			if(m_frameType == ruFRAMETYPE_LOCAL)
			{
				m_controlPoints[m_controlSegmentTail].m_p0 = CRuVector3(0.0f, 0.0f, 0.0f) * m_anchor0->GetLocalTransform();
				m_controlPoints[m_controlSegmentTail].m_p1 = CRuVector3(0.0f, 0.0f, 0.0f) * m_anchor1->GetLocalTransform();

				this->SetOption(ruENTITYOPTION_TRANSFORMMODE, ruENTITYTRANSFORMMODE_NORMAL);
			}
			else
			{
				m_controlPoints[m_controlSegmentTail].m_p0 = CRuVector3(0.0f, 0.0f, 0.0f) * m_anchor0->GetWorldTransform();
				m_controlPoints[m_controlSegmentTail].m_p1 = CRuVector3(0.0f, 0.0f, 0.0f) * m_anchor1->GetWorldTransform();

				this->SetOption(ruENTITYOPTION_TRANSFORMMODE, ruENTITYTRANSFORMMODE_OVERRIDE);

				// Disable scene hierarchy role
				this->SetOption(ruENTITYOPTION_HRDISABLE, TRUE);
			}

			if(m_numControlPoints >= m_maxControlPoints)
			{
				++m_controlSegmentHead;
			}
			else
			{
				++m_numControlPoints;
			}
		}

		if(m_t < m_activeDuration || m_activeDuration == 0.0f)
		{
			REAL t0 = FLT_MAX, t1 = 0.0f;

			// Clamp segment head and tail
			m_controlSegmentHead = m_controlSegmentHead % m_maxControlPoints;
			m_controlSegmentTail = m_controlSegmentTail % m_maxControlPoints;

			// Update each control point
			for(INT32 j = 0, i = m_controlSegmentHead; j < m_numControlPoints; ++j, i = (i + 1) % m_maxControlPoints)
			{
				// Increment age
				m_controlPoints[i].m_age += m_accumT;

				// Fade alpha based on control point age and global fade coefficients
				REAL n = min(1.0f, m_controlPoints[i].m_age / m_maxAge);
				REAL sqrN = n * n;
				REAL cubeN = n * sqrN;

				m_controlPoints[i].m_alpha = min(1.0f, max(0.0f, 1.0f - (n * m_fadeCoefficients[0] + sqrN * m_fadeCoefficients[1] + cubeN * m_fadeCoefficients[2]))) * m_alphaFactor;

				if(m_controlPoints[i].m_age < m_maxAge)
				{
					// Track age range for active sections
					t0 = min(m_controlPoints[i].m_age, t0);
					t1 = max(m_controlPoints[i].m_age, t1);
				}
				else
				{
					// Control point is older than allowed age, increment segment head
					m_controlSegmentHead = i + 1;

					// Decrement total number of active control points, and decrement j to keep loop count correct
					--m_numControlPoints;
					--j;
				}
			}

			// Clamp segment head and tail
			m_controlSegmentHead = m_controlSegmentHead % m_maxControlPoints;
			m_controlSegmentTail = m_controlSegmentTail % m_maxControlPoints;

			// Calculate u coordinate for each segment
			for(INT32 j = 0, i = m_controlSegmentHead; j < m_numControlPoints; ++j, i = (i + 1) % m_maxControlPoints)
			{
				m_controlPoints[i].m_u = (m_controlPoints[i].m_age - t0) / (t1 - t0);
			}

			if(m_numControlPoints < 4)
			{
				// Copy shit over
				m_numSegments = m_numControlPoints;
				for(INT32 j = 0, i = m_controlSegmentHead; j < m_numControlPoints; ++j, i = (i + 1) % m_maxControlPoints)
				{
					m_segments[j] = m_controlPoints[i];
				}
			}
			else
			{
				REAL numSmoothedSegments = (t1 - t0) / m_segmentInterval;

				INT32 iP[4] = { m_numControlPoints, m_numControlPoints - 1, m_numControlPoints - 2, m_numControlPoints - 3 };

				m_numSegments = 0;

				for(REAL smoothT = t0; smoothT < t1; smoothT += m_segmentInterval)
				{
					INT32 i0 = (((iP[0] >= 0) ? ((iP[0] < m_numControlPoints) ? iP[0] : m_numControlPoints - 1) : 0) + m_controlSegmentHead) % m_maxControlPoints;
					INT32 i1 = (((iP[1] >= 0) ? ((iP[1] < m_numControlPoints) ? iP[1] : m_numControlPoints - 1) : 0) + m_controlSegmentHead) % m_maxControlPoints;
					INT32 i2 = (((iP[2] >= 0) ? ((iP[2] < m_numControlPoints) ? iP[2] : m_numControlPoints - 1) : 0) + m_controlSegmentHead) % m_maxControlPoints;
					INT32 i3 = (((iP[3] >= 0) ? ((iP[3] < m_numControlPoints) ? iP[3] : m_numControlPoints - 1) : 0) + m_controlSegmentHead) % m_maxControlPoints;

					// Create references to each control point
					ControlPointDesc &controlPt0 = m_controlPoints[i0];
					ControlPointDesc &controlPt1 = m_controlPoints[i1];
					ControlPointDesc &controlPt2 = m_controlPoints[i2];
					ControlPointDesc &controlPt3 = m_controlPoints[i3];

					// Insert new segment
					REAL relativeT = (smoothT - controlPt1.m_age) / (controlPt2.m_age - controlPt1.m_age);

					m_segments[m_numSegments].m_age = 0.0f;
					m_segments[m_numSegments].m_alpha = CatmullRomSpline(controlPt0.m_alpha, controlPt1.m_alpha, controlPt2.m_alpha, controlPt3.m_alpha, relativeT);
					m_segments[m_numSegments].m_p0 = CatmullRomSpline(controlPt0.m_p0, controlPt1.m_p0, controlPt2.m_p0, controlPt3.m_p0, relativeT);
					m_segments[m_numSegments].m_p1 = CatmullRomSpline(controlPt0.m_p1, controlPt1.m_p1, controlPt2.m_p1, controlPt3.m_p1, relativeT);

					if(m_numSegments > 0)
					{
						m_segments[m_numSegments].m_u = (m_segments[m_numSegments].m_p0 - m_segments[m_numSegments - 1].m_p0).Magnitude() + m_segments[m_numSegments - 1].m_u;
					}
					else
					{
						m_segments[m_numSegments].m_u = 0.0f;
					}

					// If we have exceeded the time range of the current control point set, advance the control point indices
					if(smoothT + m_segmentInterval >= controlPt2.m_age)
					{
						for(INT32 i = 0; i < 4; ++i)
						{
							iP[i] -= 1;
						}
					}

					// Increment number of segments
					++m_numSegments;

					// Re-generate segment buffer by doubling it if we have exceeded the maximum allowed trail segments
					if(m_numSegments >= m_maxTrailSegments)
					{
						SetMaxSegments(m_maxTrailSegments * 2);
					}
				}

				REAL arcLength = m_segments[m_numSegments - 1].m_u;

				for(INT32 i = 0; i < m_numSegments; ++i)
				{
					m_segments[i].m_u = m_segments[i].m_u / arcLength;
				}
			}
		}

		// If the ghost trail's activation time has expired, begin the global fade
		if(m_t >= m_activeDuration)
		{
			if(m_fadeOutDuration > 0.0f)
			{
				m_alphaFactor = max(0.0f, 1.0f - (m_t - m_activeDuration) / m_fadeOutDuration);
			}
			else
			{
				m_alphaFactor = 0.0f;
			}
		}

		// Apply alpha factor
		this->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(m_alphaFactor));

		// Build geometry
		CRuMorphTarget *morphTarget = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0));
		CRuVector3 *position = morphTarget->GetPosition();
		DWORD *diffuse = morphTarget->GetDiffuse();
		REAL *uv = morphTarget->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
		UINT16 *indices = m_mesh->GetIndices();

		REAL frameVSize = 1.0f / m_trailTextureFrameCount;
		REAL v0 = m_trailTextureFrameIndex * frameVSize;
		REAL v1 = v0 + frameVSize;

		INT32 j = 0, k = 0, m = 0;

		INT32 vCount = 0;
		INT32 pCount = 0;

		// Insert the head segment
		for(INT32 n = 0; n < m_numSubdivisions; ++n)
		{
			REAL t = (REAL) n / (m_numSubdivisions - 1);

			position[j] = m_segments[0].m_p0 * (1.0f - t) + m_segments[0].m_p1 * t;
			diffuse[j] = RUCOLOR_ARGB((BYTE) (m_segments[0].m_alpha * 255.0f), 0xFF, 0xFF, 0xFF);
			uv[k++] = m_segments[0].m_u;
			uv[k++] = v0 * (1.0f - t) + v1 * t;
			this->SetLocalEntityBounds(CRuSphere(position[j], 0.0f));
			++j;

			++vCount;
		}

		for(INT32 i = 1; i < m_numSegments; ++i)
		{
			// Insert current segment
			for(INT32 n = 0; n < m_numSubdivisions; ++n)
			{
				REAL t = (REAL) n / (m_numSubdivisions - 1);

				position[j] = m_segments[i].m_p0 * (1.0f - t) + m_segments[i].m_p1 * t;
				diffuse[j] = RUCOLOR_ARGB((BYTE) (m_segments[i].m_alpha * 255.0f), 0xFF, 0xFF, 0xFF);
				uv[k++] = m_segments[i].m_u;
				uv[k++] = v0 * (1.0f - t) + v1 * t;
				this->SetLocalEntityBounds(CRuSphere(position[j], 0.0f));
				++j;

				++vCount;
			}

			for(INT32 l = 1; l < m_numSubdivisions; ++l)
			{
				INT32 segOffset = (i - 1) * m_numSubdivisions;
				INT32 colOffset = m_numSubdivisions - 2;
				INT32 rowOffset = l - 1;

				indices[m++] = segOffset + rowOffset + 0;
				indices[m++] = segOffset + rowOffset + 1;
				indices[m++] = segOffset + rowOffset + 3 + colOffset;

				indices[m++] = segOffset + rowOffset + 0;
				indices[m++] = segOffset + rowOffset + 3 + colOffset;
				indices[m++] = segOffset + rowOffset + 2 + colOffset;

				++pCount;
				++pCount;
			}
		}

		INT32 maxVertices = m_numSegments * m_numSubdivisions;
		INT32 maxPrimitives = 2 * (m_numSubdivisions - 1) * (m_numSegments - 1);

		m_visible = TRUE;
		m_mesh->SetNumPrimitives(pCount);
		m_mesh->SetNumVertices(vCount);

		// Increment dynamic mesh iteration
		m_mesh->IncrementMeshIteration();
		m_mesh->IncrementTopologyIteration();

		// Reset t accumulator
		m_accumT = 0.0f;
	}

	return IRuEntity_Renderable::Update(elapsedTime);
}
/*
void CRuEntity_GhostTrail::UpdateEntityBounds()
{
	if(m_numSegments > 0)
	{
		m_entityBounds.Center() = m_segments[0].m_p0;
		m_entityBounds.Merge(m_segments[0].m_p1, 0.0f);

		for(INT32 i = 1; i < m_maxTrailSegments; ++i)
		{
			m_entityBounds.Merge(m_segments[i].m_p0, 0.0f);
			m_entityBounds.Merge(m_segments[i].m_p1, 0.0f);
		}
	}
}
*/
INT32 CRuEntity_GhostTrail::GetNumMeshes() const
{
	if(m_visible)
	{
		return 1;
	}

	return 0;
}

BOOL CRuEntity_GhostTrail::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex == 0)
	{
		*meshOut = m_mesh;
		*materialOut = m_material;

		return TRUE;
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

#pragma managed
