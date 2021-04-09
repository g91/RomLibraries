/*!
	@file RuEntity_PolyLine.cpp

	Copyright (c) 2004-2007 All rights reserved
*/

#include "../../Scene/FX/RuEntity_PolyLine.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_PolyLine, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_PolyLine, "CRuEntity_PolyLine", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuEntity_PolyLine, 0x4B219CD9, 0x99564309, 0x9BDD9EF8, 0xAD201A27)

// ************************************************************************************************************************************************************

CRuEntity_PolyLine::CRuEntity_PolyLine()
:	m_maximumSegmentCount(1024),
	m_segmentLength(5.0f),
	m_segmentWidth(1.0f),
	m_deviation(0.05f),
	m_uvRatio(10.0f),
	m_parabolaHeight(0.0f),
	m_vSpeed(0.0f),
	m_targetEntity(NULL),
	m_dummyFrame(NULL),
	m_currentV(0.0f),
	m_mesh(NULL),
	m_material(NULL)
{
	// Initialize dummy frame
	m_dummyFrame = ruNEW CRuFrame();

	// Initialize mesh
	m_mesh = ruNEW CRuDynamicMesh();
	m_mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, m_maximumSegmentCount * 2, (m_maximumSegmentCount - 1) * 2, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);
	m_mesh->AddTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
	m_mesh->SetNumPrimitives(0);
	m_mesh->SetNumVertices(0);

	IRuMeshTopology *meshTopology = m_mesh->GetMeshTopology();
	UINT16 *indices = meshTopology->GetIndices();

	for(INT32 i = 1, j = 0; i < m_maximumSegmentCount; ++i)
	{
		INT32 k = (i - 1) * 2;

		indices[j++] = k;
		indices[j++] = k + 1;
		indices[j++] = k + 2;

		indices[j++] = k + 1;
		indices[j++] = k + 2;
		indices[j++] = k + 3;
	}

	// Initialize material
	m_material = ruNEW CRuMaterial();
	m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_FX_LIGHTNING"));

	// By default, set material as translucent
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_TRANSLUCENT, TRUE);
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_GREATER);
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTVALUE, 0);

	// Disable rotation
	this->SetOption(ruENTITYOPTION_TRANSFORMMODE, ruENTITYTRANSFORMMODE_NOROTATE);

	// Register event handlers
	Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuEntity_PolyLine::HandleEvent_Dispose, this, _1), NULL);
	Event_Camera().RegisterHandler(this->GetGUID(), boost::bind(&CRuEntity_PolyLine::HandleEvent_Camera, this, _1));
}

CRuEntity_PolyLine::~CRuEntity_PolyLine()
{
	ruSAFE_RELEASE(m_dummyFrame);
	ruSAFE_RELEASE(m_mesh);
	ruSAFE_RELEASE(m_material);
}

REAL CRuEntity_PolyLine::GetSegmentLength()
{
	return m_segmentLength;
}

void CRuEntity_PolyLine::SetSegmentLength(REAL segmentLength)
{
	m_segmentLength = segmentLength;
}

REAL CRuEntity_PolyLine::GetSegmentWidth()
{
	return m_segmentWidth;
}

void CRuEntity_PolyLine::SetSegmentWidth(REAL segmentWidth)
{
	m_segmentWidth = segmentWidth;
}

REAL CRuEntity_PolyLine::GetDeviation()
{
	return m_deviation;
}

void CRuEntity_PolyLine::SetDeviation(REAL deviation)
{
	m_deviation = deviation;
}

REAL CRuEntity_PolyLine::GetUVRatio()
{
	return m_uvRatio;
}

void CRuEntity_PolyLine::SetUVRatio(REAL uvRatio)
{
	m_uvRatio = uvRatio;
}

REAL CRuEntity_PolyLine::GetParabolaHeight()
{
	return m_parabolaHeight;
}

void CRuEntity_PolyLine::SetParabolaHeight(REAL parabolaHeight)
{
	m_parabolaHeight = parabolaHeight;
}

REAL CRuEntity_PolyLine::GetVSpeed()
{
	return m_vSpeed;
}

void CRuEntity_PolyLine::SetVSpeed(REAL vSpeed)
{
	m_vSpeed = vSpeed;
}

void CRuEntity_PolyLine::SetTarget(CRuEntity *target)
{
	if(target == m_targetEntity)
	{
		return;
	}

	// If there is an existing target, unregister from its list of listeners
	if(m_targetEntity)
	{
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
	}

	// Store pointer to new target
	m_targetEntity = target;

	if(m_targetEntity)
	{
		// Attach dummy to the target entity
		m_targetEntity->AddChild(m_dummyFrame);

		// Register self as an event listener
		m_targetEntity->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuEntity_PolyLine::HandleEvent_Dispose, this, _1), NULL);
	}
}

IRuObject *CRuEntity_PolyLine::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuEntity_PolyLine();

	// Chain cloning call to parent class
	IRuEntity_Renderable::Clone(clonedObject);

	// Shortcut
	CRuEntity_PolyLine *clonedPolyLine = static_cast<CRuEntity_PolyLine *>(clonedObject);

	// Copy settings
	clonedPolyLine->m_segmentLength = m_segmentLength;
	clonedPolyLine->m_segmentWidth = m_segmentWidth;
	clonedPolyLine->m_deviation = m_deviation;
	clonedPolyLine->m_uvRatio = m_uvRatio;

	clonedPolyLine->SetTarget(m_targetEntity);

	return clonedObject;
}

BOOL CRuEntity_PolyLine::SerializeFrom(IRuStream *stream)
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
	IRuEntity_Renderable::SerializeFrom(stream);

	// Read settings
	stream->Read(&m_segmentLength, sizeof(REAL));
	stream->Read(&m_segmentWidth, sizeof(REAL));
	stream->Read(&m_deviation, sizeof(REAL));
	stream->Read(&m_uvRatio, sizeof(REAL));

	// Forcibly disable rotation
	this->SetOption(ruENTITYOPTION_TRANSFORMMODE, ruENTITYTRANSFORMMODE_NOROTATE);

	return TRUE;
}

BOOL CRuEntity_PolyLine::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Renderable::SerializeTo(stream);

	// Write settings
	stream->Write(&m_segmentLength, sizeof(REAL));
	stream->Write(&m_segmentWidth, sizeof(REAL));
	stream->Write(&m_deviation, sizeof(REAL));
	stream->Write(&m_uvRatio, sizeof(REAL));

	return TRUE;
}

BOOL CRuEntity_PolyLine::Update(REAL elapsedTime)
{
	// Chain update to base class
	IRuEntity_Renderable::Update(elapsedTime);

	CRuVector3 p0(0.0f, 0.0f, 0.0f);
	CRuVector3 p1;
	CRuVector3 p2 = (m_targetEntity ? m_targetEntity->GetTranslation(ruFRAMETYPE_WORLD) : CRuVector3(0.0f, 0.0f, 0.0f)) - this->GetTranslation(ruFRAMETYPE_WORLD);

	CRuVector3 e0(1.0f, 0.0f, 0.0f);
	CRuVector3 e1(0.0f, 1.0f, 0.0f);

	this->SetLocalEntityBounds(CRuSphere(p0, (p2 - p0).Magnitude()));

	IRuMorphTarget *morphTarget = m_mesh->GetMorphTarget(0);
	CRuVector3 *position = morphTarget->GetPosition();
	DWORD *diffuse = morphTarget->GetDiffuse();

	BOOL finished = FALSE;
	INT32 vIdx = 0;
	INT32 sIdx = 0;

	while(!finished && sIdx < m_maximumSegmentCount)
	{
		CRuVector3 centerAxis;

		// If distance remaining is larger than segment length, we can do a little bit of deviation work
		REAL distanceRemaining = (p2 - p0).Magnitude();
		if(distanceRemaining > m_segmentLength && distanceRemaining > ruEPSILON)
		{
			centerAxis = (p2 - p0).Normalize();

			// Randomize center axis
			CRuVector3 devLateral = CrossProduct(e0, centerAxis);

			if(devLateral.Magnitude() <= ruEPSILON)
			{
				devLateral = CrossProduct(e1, centerAxis);
			}

			devLateral.Normalize();

			CRuQuaternion devRot;
			devRot.FromAngleAxis(centerAxis, m_randGen.GenerateFloat() * 2.0f * ruPI);
			devRot.TransformVector(devLateral);

			devLateral = devLateral * m_deviation;

			centerAxis = centerAxis + devLateral;

			if(sIdx < m_segments.Count())
			{
				SegmentDesc &segDesc = m_segments[sIdx];

				segDesc.m_centerAxis = segDesc.m_centerAxis * 0.5f + centerAxis * 0.5f;

				centerAxis = segDesc.m_centerAxis;
			}
			else
			{
				SegmentDesc segDesc;
				segDesc.m_centerAxis = centerAxis;

				m_segments.Add(segDesc);
			}

			p1 = p0 + centerAxis * m_segmentLength;
		}
		else
		{
			centerAxis = p2 - p0;

			if(sIdx >= m_segments.Count())
			{
				m_segments.Add(SegmentDesc());
			}

			if(centerAxis.Magnitude() <= ruEPSILON)
			{
				SegmentDesc &segDesc = m_segments[sIdx];
				segDesc.m_centerAxis = centerAxis;

				finished = TRUE;
			}
			else
			{
				centerAxis.Normalize();

				SegmentDesc &segDesc = m_segments[sIdx];
				segDesc.m_centerAxis = centerAxis;

				p1 = p2;
			}
		}

		// Update segment descriptor
		m_segments[sIdx].m_p0 = p0;

		// Advance p0 to p1
		p0 = p1;
		++sIdx;
	}

	// Update number of segments
	m_segments.SetNumEntries(sIdx);

	if(m_parabolaHeight != 0.0f)
	{
		for(INT32 i = 0; i < m_segments.Count(); ++i)
		{
			// Scale T such that its range is in [-1, 1]
			REAL t = (REAL) i / (m_segments.Count() - 1) * 2.0f - 1.0f;

			REAL adjustedY = m_parabolaHeight - t * t * m_parabolaHeight;

			m_segments[i].m_p0.m_y = m_segments[i].m_p0.m_y + adjustedY;
		}
	}

	// Update current V
	m_currentV = m_currentV + m_vSpeed * elapsedTime;

	return TRUE;
}

INT32 CRuEntity_PolyLine::GetNumMeshes() const
{
	return 1;
}

BOOL CRuEntity_PolyLine::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex == 0)
	{
		*meshOut = m_mesh;
		*materialOut = m_material;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_PolyLine::HandleEvent_Dispose(RuEventArgs *eventArgs)
{
	if(m_targetEntity)
	{
		m_targetEntity->Event_Dispose().UnregisterHandler(this->GetGUID());
		m_targetEntity = NULL;
	}

	return TRUE;
}

BOOL CRuEntity_PolyLine::HandleEvent_Camera(RuEventArgs *eventArgs)
{
	RuEntityEvent_Camera_Args *cameraArgs = static_cast<RuEntityEvent_Camera_Args *>(eventArgs); 
	CRuCamera *camera = cameraArgs->m_camera;

	CRuMatrix4x4 worldTransform;
	worldTransform.SetToIdentity();
	worldTransform.Rotate(this->GetRotation(ruFRAMETYPE_WORLD));
	worldTransform.Translate(this->GetTranslation(ruFRAMETYPE_WORLD));

	// Camera position
	CRuVector3 C = camera->GetCameraPosition();

	if(m_segments.Count() > 1)
	{
		IRuMorphTarget *morphTarget = m_mesh->GetMorphTarget(0);
		CRuVector3 *position = morphTarget->GetPosition();
		REAL *uv = morphTarget->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);

		REAL v = m_currentV;

		for(INT32 i = 0, j = 0, k = 0; i < m_segments.Count(); ++i)
		{
			CRuVector3 P;

			worldTransform.TransformPoint(m_segments[i].m_p0, P);

			CRuVector3 Z = (C - P).Normalize();
			CRuVector3 T;

			if(i == 0)
			{
				T = (m_segments[1].m_p0 - m_segments[0].m_p0).Normalize();
			}
			else if(i + 1 == m_segments.Count())
			{
				T = (m_segments[i].m_p0 - m_segments[i - 1].m_p0).Normalize();
				v = v + (m_segments[i].m_p0 - m_segments[i - 1].m_p0).Magnitude() / m_uvRatio;
			}
			else
			{
				T = (m_segments[i + 1].m_p0 - m_segments[i - 1].m_p0).Normalize();
				v = v + (m_segments[i].m_p0 - m_segments[i - 1].m_p0).Magnitude() / m_uvRatio;
			}

			CRuVector3 TxZ = CrossProduct(T, Z).Normalize();

			position[j++] = m_segments[i].m_p0 + TxZ * m_segmentWidth * 0.5f;
			uv[k++] = 0.0f;
			uv[k++] = v;

			position[j++] = m_segments[i].m_p0 - TxZ * m_segmentWidth * 0.5f;
			uv[k++] = 1.0f;
			uv[k++] = v;
		}

		m_mesh->SetNumVertices(m_segments.Count() * 2);
		m_mesh->SetNumPrimitives((m_segments.Count() - 1) * 2);

		m_mesh->IncrementMeshIteration();
		m_mesh->IncrementTopologyIteration();
	}
	else
	{
		m_mesh->SetNumVertices(0);
		m_mesh->SetNumPrimitives(0);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
