#include "../../Scene/Terrain/RuWorld_Water.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuWorld_Water, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuWorld_Water, "CRuWorld_Water", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuWorld_Water, 0x9E1F53AC, 0xD1414667, 0x87C43210, 0x4CEDB85F)

// ************************************************************************************************************************************************************

CRuWorld_Water::CRuWorld_Water()
{
	m_waterBodyGUID = CRuGUID::Invalid;

	m_waterHF = NULL;
	m_waterCF = NULL;
	m_waterFlags = NULL;

	m_waterMesh = NULL;
	m_waterMaterial = ruNEW CRuMaterial();
	m_waterMaterial->SetShader(g_ruShaderManager->GetShader("ruSHADER_NULL"));

	m_runtime_WaterBodyType = RuWorld::ruWATERBODYTYPE_INVALID;
}

CRuWorld_Water::~CRuWorld_Water()
{
	ruSAFE_RELEASE(m_waterHF);
	ruSAFE_RELEASE(m_waterCF);
	ruSAFE_RELEASE(m_waterFlags);

	ruSAFE_RELEASE(m_waterMesh);
	ruSAFE_RELEASE(m_waterMaterial);
}

IRuObject *CRuWorld_Water::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuWorld_Water();
	CRuWorld_Water *waterLayer = static_cast<CRuWorld_Water *>(clonedObject);

	// Chain cloning call to parent class
	IRuEntity_Renderable::Clone(clonedObject);

	// Clone data fields
	waterLayer->m_waterBodyGUID = m_waterBodyGUID;
	waterLayer->m_waterHF = static_cast<CRuMojo_AttributeParcel<REAL> *>(m_waterHF->Clone());
	waterLayer->m_waterCF = static_cast<CRuMojo_AttributeParcel<RUCOLOR> *>(m_waterCF->Clone());
	waterLayer->m_waterFlags = static_cast<CRuMojo_AttributeParcel<BYTE> *>(m_waterFlags->Clone());

	// Build mesh for the cloned water layer
	waterLayer->BuildWaterLayerMesh();

	// Set material
	if(m_waterMaterial)
		waterLayer->SetWaterLayerMaterial(m_waterMaterial);

	return clonedObject;
}

BOOL CRuWorld_Water::SerializeFrom(IRuStream *stream)
{
	CRuGUID classGUID;

	// Read class GUID
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version;
	stream->Read(&version, sizeof(INT32));

	// Read water body GUID
	RuReadGUID(stream, m_waterBodyGUID);

	switch(version)
	{
		case 1:
			{
				// Read height field
				Mojo_SerializeFrom(&m_waterHF, stream);

				// Read flag field
				m_waterFlags = ruNEW CRuMojo_AttributeParcel<BYTE>();
				m_waterFlags->SerializeFrom(stream);

				m_waterCF = ruNEW CRuMojo_AttributeParcel<RUCOLOR>(m_waterFlags->GetParcelWidth(), m_waterFlags->GetParcelHeight());
				m_waterCF->SetParcelOrigin(m_waterFlags->GetParcelOrigin());
				m_waterCF->SetParcelOverlapGap(m_waterFlags->GetParcelOverlapGap());
				m_waterCF->SetParcelSpacing(m_waterFlags->GetParcelSpacing());
			}

			break;

		case 2:
			{
				// Read height field
				Mojo_SerializeFrom(&m_waterHF, stream);

				// Read color field
				m_waterCF = ruNEW CRuMojo_AttributeParcel<RUCOLOR>();
				m_waterCF->SerializeFrom(stream);

				// Read flag field
				m_waterFlags = ruNEW CRuMojo_AttributeParcel<BYTE>();
				m_waterFlags->SerializeFrom(stream);
			}

			break;

		case 3:
			{
				// Chain serialization to base class
				IRuEntity_Renderable::SerializeFrom(stream);

				// Read height field
				Mojo_SerializeFrom(&m_waterHF, stream);

				// Read color field
				m_waterCF = ruNEW CRuMojo_AttributeParcel<RUCOLOR>();
				m_waterCF->SerializeFrom(stream);

				// Read flag field
				m_waterFlags = ruNEW CRuMojo_AttributeParcel<BYTE>();
				m_waterFlags->SerializeFrom(stream);
			}

			break;
	}

	if(version < 3)
	{
		// Correct parcel origin... this is because we changed from absolute to relative coordinates - remove this code snippet when i have the chance...
		REAL gridSize = m_waterHF->GetParcelSpacing();
		INT32 overlapGap = m_waterHF->GetParcelOverlapGap();
		CRuVector3 translation = m_waterCF->GetParcelOrigin();
		m_waterHF->SetParcelOrigin(CRuVector3(-overlapGap * gridSize, 0.0f, -overlapGap * gridSize));

		m_waterCF->SetParcelOrigin(CRuVector3(0.0f, 0.0f, 0.0f));

		m_waterFlags->SetParcelOrigin(CRuVector3(0.0f, 0.0f, 0.0f));

		this->SetTranslation(translation);
	}

	// Build mesh for the cloned water layer
	BuildWaterLayerMesh();

	return TRUE;
}

BOOL CRuWorld_Water::SerializeTo(IRuStream *stream)
{
	INT32 version = 3;

	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	stream->Write(&version, sizeof(INT32));

	// Write water body GUID
	stream->Write(m_waterBodyGUID.Data(), m_waterBodyGUID.Size());

	// Chain serialization to base class
	IRuEntity_Renderable::SerializeTo(stream);

	// Write height field
	Mojo_SerializeTo(m_waterHF, stream);

	// Write color field
	m_waterCF->SerializeTo(stream);

	// Write flag field
	m_waterFlags->SerializeTo(stream);

	return TRUE;
}

INT32 CRuWorld_Water::GetNumMeshes() const
{
	return 1;
}

BOOL CRuWorld_Water::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if(meshIndex == 0)
	{
		*meshOut = m_waterMesh;
		*materialOut = m_waterMaterial;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuWorld_Water::QueryCollision(CRuCollisionQuery *colQuery)
{
	// Save previous collision transform
	CRuMatrix4x4 *originalWorldTransform = colQuery->m_worldTransform;
	CRuMatrix4x4 *originalInvWorldTransform = colQuery->m_invWorldTransform;

	// Store the inverse of the current world transform into the collision query
	colQuery->m_worldTransform = const_cast<CRuMatrix4x4 *>(&this->GetWorldTransform());
	colQuery->m_invWorldTransform = const_cast<CRuMatrix4x4 *>(&this->GetInverseWorldTransform());

	if(m_waterMesh)
	{
		// Build collision structure if necessary
		if(m_waterMesh->GetCollisionStructure() == NULL)
		{
			this->BuildCollisionStructure();
		}

		// Perform collision query
		m_waterMesh->QueryCollision(colQuery);
	}

	// Restore original collision transform
	colQuery->m_worldTransform = originalWorldTransform;
	colQuery->m_invWorldTransform = originalInvWorldTransform;

	return TRUE;
}

BOOL CRuWorld_Water::Allocate(const CRuGUID &waterBodyGUID, const CRuVector3 &layerOrigin, INT32 dimension, REAL gridSpacing, INT32 overlapGap)
{
	m_waterBodyGUID = waterBodyGUID;

	// Set local translation
	this->SetTranslation(CRuVector3(layerOrigin.m_x, 0.0f, layerOrigin.m_z));

	// Set water height field parameters
	m_waterHF = ruNEW CRuMojo_AttributeParcel<REAL>(dimension + 2 * overlapGap, dimension + 2 * overlapGap);
	m_waterHF->SetParcelOrigin(CRuVector3(-gridSpacing * overlapGap, 0.0f, -gridSpacing * overlapGap));
	m_waterHF->SetParcelSpacing(gridSpacing);
	m_waterHF->SetParcelOverlapGap(overlapGap);

	Mojo_ResetHeight(m_waterHF, layerOrigin.m_y);

	// Set water color parcel parameters
	m_waterCF = ruNEW CRuMojo_AttributeParcel<RUCOLOR>(dimension, dimension);
	m_waterCF->SetParcelOrigin(CRuVector3(0.0f, 0.0f, 0.0f));
	m_waterCF->SetParcelSpacing(gridSpacing);
	m_waterCF->SetParcelOverlapGap(0);

	// Set water flag parcel parameters
	m_waterFlags = ruNEW CRuMojo_AttributeParcel<BYTE>(dimension, dimension);
	m_waterFlags->SetParcelOrigin(CRuVector3(0.0f, 0.0f, 0.0f));
	m_waterFlags->SetParcelSpacing(gridSpacing);
	m_waterFlags->SetParcelOverlapGap(0);

	Mojo_ResetFlags(m_waterFlags, 0x00, 0x80);
	Mojo_ResetFlags(m_waterFlags, 0x3F, 0x3F);

	return TRUE;
}

BOOL CRuWorld_Water::PaintHeight(const CRuVector3 &center, float radius, float delta, RuFalloffFunction falloffFunction)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	// Perform painting
	Mojo_PaintHeight(m_waterHF, localCenter, radius, delta, 0.0f, FALSE, falloffFunction);

	// Rebuild water model
	BuildWaterLayerMesh();

	return TRUE;
}

BOOL CRuWorld_Water::SetHeight(const CRuVector3 &center, float radius, float height, float brushStrength, RuFalloffFunction falloffFunction)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	// Perform painting
	Mojo_SetHeight(m_waterHF, localCenter, radius, height, brushStrength, falloffFunction);

	// Rebuild water model
	BuildWaterLayerMesh();

	return TRUE;
}

BOOL CRuWorld_Water::SmoothHeight(const CRuVector3 &center, float radius, float delta)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	// Perform painting
	Mojo_SmoothHeight(m_waterHF, localCenter, radius, delta);

	// Rebuild water model
	BuildWaterLayerMesh();

	return TRUE;
}

BOOL CRuWorld_Water::PaintSurface(const CRuVector3 &center, float radius, BOOL waterVisible)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	BYTE waterVisibilityFlag = waterVisible ? 0x80 : 0x00;
	BYTE waterVisibilityMask = 0x80;

	// Perform flag painting
	Mojo_PaintFlags(m_waterFlags, localCenter, radius, waterVisibilityFlag, waterVisibilityMask);

	// Rebuild water model
	BuildWaterLayerMesh();

	return TRUE;
}

BOOL CRuWorld_Water::PaintAlpha(const CRuVector3 &center, REAL radius, REAL alpha, REAL delta, RuFalloffFunction falloffFunction)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	// Perform painting
	Mojo_PaintValue(m_waterFlags, localCenter, radius, alpha, delta, falloffFunction, 0x3F);

	// Rebuild water model
	BuildWaterLayerMesh();

	return TRUE;
}

BOOL CRuWorld_Water::PaintColor(const CRuVector3 &center, REAL radius, RUCOLOR color, REAL delta, RuFalloffFunction falloffFunction)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	// Perform painting
	Mojo_PaintColor(m_waterCF, localCenter, radius, color, delta, falloffFunction);

	// Rebuild water model
	BuildWaterLayerMesh();

	return TRUE;
}

RUCOLOR CRuWorld_Water::PickColor(const CRuVector3 &center)
{
	// Transform paint center to local coordinates
	CRuVector3 localCenter = center - this->GetTranslation(ruFRAMETYPE_WORLD);

	return Mojo_PickColor(m_waterCF, center);
}

BOOL CRuWorld_Water::StitchWaterLayer(INT32 neighborID, CRuWorld_Water *neighborLayer)
{
	// If the spacing of the two layers are different, we cannot perform stitching
	if(m_waterHF->GetParcelSpacing() != neighborLayer->m_waterHF->GetParcelSpacing())
		return FALSE;

	// Stitch water layer
	m_waterHF->Stitch(neighborID, neighborLayer->m_waterHF);

	return TRUE;
}

BOOL CRuWorld_Water::SetWaterLayerMaterial(IRuMaterial *material)
{
	// Store reference to new material
	material->AddRef();
	ruSAFE_RELEASE(m_waterMaterial);
	m_waterMaterial = material;

	return TRUE;
}

BOOL CRuWorld_Water::BuildCollisionStructure()
{
	if(m_waterMesh->GetCollisionStructure() == NULL)
	{
		// Build collision structure
		CRuCollisionStructure_Quadtree *colStructure = ruNEW CRuCollisionStructure_Quadtree();
		colStructure->BuildCollisionStructure(m_waterMesh, NULL);

		// Set surface flags to conform to that of water surfaces
		CRuCollisionQuery_User_Flag_Set flagSetQuery;
		flagSetQuery.m_markingFlag = RuWorld::ruTRIFLAGS_WATER;
		flagSetQuery.m_flagMask = 0xFFFFFFFF;
		colStructure->QueryCollision(&flagSetQuery);

		// Set as collision structure for the mesh
		m_waterMesh->SetCollisionStructure(colStructure);

		// Release local copy of collision structure
		ruSAFE_RELEASE(colStructure);
	}

	return TRUE;
}

void CRuWorld_Water::UpdateCollisionFlags(RuWorld::WaterBodyType waterBodyType)
{
	// Build collision structure if necessary
	if(m_waterMesh->GetCollisionStructure() == NULL)
	{
		this->BuildCollisionStructure();
	}

	if(m_waterMesh->GetCollisionStructure() && m_runtime_WaterBodyType != waterBodyType)
	{
		// Cache the corrent water body type
		m_runtime_WaterBodyType = waterBodyType;

		// Set surface flags to conform to that of water surfaces
		CRuCollisionQuery_User_Flag_Set flagSetQuery;

		switch(waterBodyType)
		{
			case RuWorld::ruWATERBODYTYPE_NORMAL:
				flagSetQuery.m_markingFlag = RuWorld::ruTRIFLAGS_WATER;
				break;

			case RuWorld::ruWATERBODYTYPE_EXTENTMARKER:
				flagSetQuery.m_markingFlag = RuWorld::ruTRIFLAGS_EXTENTMARKER;
				break;

			case RuWorld::ruWATERBODYTYPE_SOLID:
				flagSetQuery.m_markingFlag = 0;
				break;
		}

		flagSetQuery.m_flagMask = 0xFFFFFFFF;
		m_waterMesh->GetCollisionStructure()->QueryCollision(&flagSetQuery);
	}
}

const CRuGUID &CRuWorld_Water::GetWaterBodyGUID() const
{
	return m_waterBodyGUID;
}

BOOL CRuWorld_Water::ValidateMesh() const
{
	if(m_waterMesh == NULL || m_waterMesh->GetNumPrimitives() == 0)
		return FALSE;

	return TRUE;
}

void CRuWorld_Water::BuildWaterLayerMesh()
{
	// Rebuild the water layer mesh
	ruSAFE_RELEASE(m_waterMesh);
	m_waterMesh = Mojo_BuildMeshFromHeightField(m_waterHF, m_waterCF, m_waterFlags, 0x80);

	// Set local entity bounds based on mesh bounds
	this->SetLocalEntityBounds(m_waterMesh->GetBounds());

	// Use first vertex height as the split plane height
	if(m_waterMesh->GetNumVertices() > 0)
	{
		const CRuVector3 *position = m_waterMesh->GetMorphTarget(0)->GetPosition();
		CRuVector3 worldPosition;
		this->GetWorldTransform().TransformPoint(position[0], worldPosition);
		REAL meanHeight = worldPosition.m_y;

		m_splitPlane.SetPlane(CRuVector3(0.0f, 1.0f, 0.0f), CRuVector3(0.0f, meanHeight, 0.0f));
		this->GetParameterBlock()->SetParameter(ruPARAMTYPE_WATER_SPLITPLANE, reinterpret_cast<UINT64>(&m_splitPlane));
	}

	// Reset the runtime water body type
	m_runtime_WaterBodyType = RuWorld::ruWATERBODYTYPE_INVALID;
}

// ************************************************************************************************************************************************************

#pragma managed
