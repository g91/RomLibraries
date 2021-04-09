/*!
	@file RuEntity_DecalShadow.cpp

	Copyright (c) 2004-2006 All rights reserved
*/

#include "../../Engine/Texture/RuTexture_Proxy.h"

#include "../../Scene/Container/RuContainer_Collision.h"
#include "../../Scene/FX/RuEntity_DecalShadow.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_DecalShadow, IRuEntity_Renderable)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_DecalShadow, "CRuEntity_DecalShadow", "IRuEntity_Renderable")
ruCLASSGUID_DEFINE(CRuEntity_DecalShadow, 0xDE74870E, 0xEC4A4731, 0xA0954E0C, 0xE30B225E)

// ************************************************************************************************************************************************************

CRuEntity_DecalShadow::CRuEntity_DecalShadow()
:	m_maximumDecalRadius(240),
	m_maximumDecalFaces(2000),
	m_enumerationPadding(20.0f),
	m_shadowMode(ruDECALSHADOWMODE_NORMAL),
	m_shadowDirection(0.0f, -1.0f, 0.0f),
	m_shadowZoneSize(10.0f),
	m_dotShadowSize(5.0f),
	m_receiver(NULL),
	m_mesh(NULL),
	m_material(NULL)
{
	m_shadowCamera = ruNEW CRuCamera();

	m_mesh = ruNEW CRuDynamicMesh();
	m_mesh->Initialize(ruPRIMTYPE_TRIANGLELIST, m_maximumDecalFaces * 3, m_maximumDecalFaces, ruVERTEXFORMAT_POSITION | ruVERTEXFORMAT_DIFFUSE, 1);
	m_mesh->AddTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
	m_mesh->SetNumPrimitives(0);
	m_mesh->SetNumVertices(0);

	m_material = ruNEW CRuMaterial();
	m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_FX_DECALSHADOW"));
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHATESTMODE, ruALPHATESTMODE_NONE);
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODE, ruALPHAMODE_NORMAL);
	m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_TRANSLUCENT, TRUE);

	m_shadowRenderMaterial_Static = ruNEW CRuMaterial();
	m_shadowRenderMaterial_Static->SetShader(g_ruShaderManager->GetShader("ruSHADER_NULL"));

	m_shadowRenderMaterial_Skin = ruNEW CRuMaterial();
	m_shadowRenderMaterial_Skin->SetShader(g_ruShaderManager->GetShader("ruSHADER_SKIN"));
	m_shadowRenderMaterial_Skin->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATIONCOLOR, RUCOLOR_ARGB(0, 255, 255, 255));
	m_shadowRenderMaterial_Skin->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_SELFILLUMINATION, RuCastFloatToINT64(1.0f));

	RUCOLOR color = RUCOLOR_ARGB(255, 255, 255, 255);
	m_shadowRenderMaterial_Skin->GetParameterBlock()->SetParameter(ruPARAMTYPE_DWORD_0, color);

	// Register event handlers
	Event_Submitted().RegisterHandler(this->GetGUID(), boost::bind(&CRuEntity_DecalShadow::HandleEvent_Submitted, this, _1));

	// Set default shadow direction
	m_shadowDirection.Set(-1.0f, -2.0f, -1.0f);
	m_shadowDirection.Normalize();

	m_textureView = NULL;

	if(g_ruEngine)
	{
		m_textureView = g_ruEngine->CreateTextureView();
		m_textureView->SetViewSize(128, 128);
		m_textureView->SetViewFormat(ruSURFACEFMT_X1R5G5B5);
	}

	// Initialize dot texture
	m_dotTexture = g_ruResourceManager->LoadTexture("ruDECALSHADOW_DOTTEXTURE");

	if(m_dotTexture == NULL)
	{
		m_dotTexture = ruNEW CRuTexture_Proxy();
		static_cast<CRuTexture_Proxy *>(m_dotTexture)->Allocate(64, 64, 1, ruSURFACEFMT_A8R8G8B8);
		m_dotTexture->SetTextureName("ruDECALSHADOW_DOTTEXTURE");
		g_ruResourceManager->AddTexture(m_dotTexture);

		DWORD *texData;
		INT32 texPitch;

		m_dotTexture->Lock(0, reinterpret_cast<void **>(&texData), &texPitch);

		for(INT32 z = 0; z < 64; ++z)
		{
			for(INT32 x = 0; x < 64; ++x)
			{
				REAL xVal = (x - 32) / 31.0f;
				REAL zVal = (z - 32) / 31.0f;
				REAL t = RuSqrt(xVal * xVal + zVal * zVal);

				BYTE value = static_cast<BYTE>(RuFalloff_Cubic(t, 1.0f) * 255.0f);
				texData[x] = RUCOLOR_ARGB(value, 0, 0, 0);
			}

			texData += texPitch / 4;
		}

		m_dotTexture->Unlock(0);
	}

	// Create a render context
	m_renderContext = RuCreateRenderContext();
	m_renderContext->SetRenderQueueSize(64);

	// Pre-allocate space for enumeration array
	m_triEnumQuery.m_colTriVerts->SetArraySize(1024);
	m_triEnumQuery.m_colTriNormals->SetArraySize(1024);
	m_triEnumQuery.m_colTriFlags->SetArraySize(1024);
}

CRuEntity_DecalShadow::~CRuEntity_DecalShadow()
{
	SetReceiver(NULL);

	ruSAFE_RELEASE(m_shadowCamera);
	ruSAFE_RELEASE(m_mesh);
	ruSAFE_RELEASE(m_material);
	ruSAFE_RELEASE(m_shadowRenderMaterial_Static);
	ruSAFE_RELEASE(m_shadowRenderMaterial_Skin);

	ruSAFE_RELEASE(m_textureView);
	ruSAFE_RELEASE(m_renderContext);
	ruSAFE_RELEASE(m_dotTexture);
}

RuDecalShadowMode CRuEntity_DecalShadow::GetShadowMode() const
{
	return m_shadowMode;
}

void CRuEntity_DecalShadow::SetShadowMode(RuDecalShadowMode shadowMode)
{
	if(m_shadowMode != shadowMode)
	{
		m_shadowMode = shadowMode;

		switch(m_shadowMode)
		{
			case ruDECALSHADOWMODE_NORMAL:
				{
					m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_FX_DECALSHADOW"));
				}

				break;

			case ruDECALSHADOWMODE_SIMPLE:
				{
					m_material->SetShader(g_ruShaderManager->GetShader("ruSHADER_FX_DECAL"));
				}

				break;
		}
	}
}

REAL CRuEntity_DecalShadow::GetDotShadowSize() const
{
	return m_dotShadowSize;
}

void CRuEntity_DecalShadow::SetDotShadowSize(REAL dotShadowSize)
{
	m_dotShadowSize = dotShadowSize;
}

void CRuEntity_DecalShadow::AddCaster(CRuEntity *caster, BOOL primaryCaster)
{
	for(INT32 i = 0; i < m_casters.Count(); ++i)
	{
		if(m_casters[i] == caster)
		{
			return;
		}
	}

	if(primaryCaster)
	{
		m_primaryCasterIdx = m_casters.Count();
	}
	m_casters.Add(caster);
}

void CRuEntity_DecalShadow::RemoveCaster(CRuEntity *caster)
{
	for(INT32 i = 0; i < m_casters.Count(); ++i)
	{
		if(m_casters[i] == caster)
		{
			if(m_primaryCasterIdx == i)
			{
				m_primaryCasterIdx = 0;
			}

			m_casters.RemoveAt(i);
			return;
		}
	}
}

void CRuEntity_DecalShadow::RemoveAllCasters()
{
	m_casters.Clear();
	m_primaryCasterIdx = 0;
}

void CRuEntity_DecalShadow::SetReceiver(IRuObject *receiver)
{
	// Unregister the container changed handler from the old collision object
	if(m_receiver)
	{
		if(m_receiver->GetType().IsSubClassOf(CRuContainer_Collision::Type()))
		{
			static_cast<CRuContainer_Collision *>(m_receiver)->Event_Changed().UnregisterHandler(this->GetGUID());
		}

		m_receiver->Event_Dispose().UnregisterHandler(this->GetGUID());
	}

	// Save reference to new receiver
	m_receiver = receiver;

	// Register the container changed handler
	if(m_receiver)
	{
		if(m_receiver->GetType().IsSubClassOf(CRuContainer_Collision::Type()))
		{
			static_cast<CRuContainer_Collision *>(m_receiver)->Event_Changed().RegisterHandler(this->GetGUID(), boost::bind(&CRuEntity_DecalShadow::CollisionContainerChangedHandler, this, _1));
		}

		m_receiver->Event_Dispose().RegisterHandler(this->GetGUID(), boost::bind(&CRuEntity_DecalShadow::CollisionContainerDisposeHandler, this, _1));
	}
}

IRuObject *CRuEntity_DecalShadow::Clone(IRuObject *clonedObject)
{
	// Create a new cloned entity if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW CRuEntity_DecalShadow();

	return clonedObject;
}

BOOL CRuEntity_DecalShadow::SerializeFrom(IRuStream *stream)
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

	return TRUE;
}

BOOL CRuEntity_DecalShadow::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 1;
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuEntity_Renderable::SerializeTo(stream);

	return TRUE;
}

BOOL CRuEntity_DecalShadow::Update(REAL elapsedTime)
{
	// Chain update to base class
	IRuEntity_Renderable::Update(elapsedTime);

	// Setup basic decal shadow material parameters
	RUCOLOR color = RUCOLOR_ARGB(255, 100, 100, 100);
	this->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(0.5f));
	this->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_DIFFUSE, color);

	if((m_casters.Count() > 0) && m_receiver)
	{
		// Determine decal enumeration bounds
		CRuVector3 decalCenterOffsetVector = m_shadowDirection * m_shadowZoneSize;
//		const CRuSphere &casterBounds = GetCasterBounds();
		CRuSphere casterBounds = GetCasterBounds();
		CRuSphere curDecalEnumBounds(m_casters[m_primaryCasterIdx]->GetTranslation(ruFRAMETYPE_WORLD) + decalCenterOffsetVector, casterBounds.Radius() + m_shadowZoneSize);

		// Cap radius to the maximum decal radius
		curDecalEnumBounds.Radius() = min(m_maximumDecalRadius, curDecalEnumBounds.Radius());
/*
		// Get collision object's iteration number
		INT32 colObjIteration = m_triEnumIteration;

		if(m_receiver->GetType().IsSubClassOf(CRuContainer_Collision::Type()))
		{
			colObjIteration = static_cast<CRuContainer_Collision *>(m_receiver)->GetContainerIteration();
		}
*/
		// Check if it is necessary to re-enumerate decal triangles
		if(m_decalEnumerationBounds.Contains(curDecalEnumBounds) == FALSE)// || colObjIteration != m_triEnumIteration)
		{
			// Record new enumeration iteration
//			m_triEnumIteration = colObjIteration;

			// Set new enumeration bounds to include extra padding
			m_decalEnumerationBounds.Set(curDecalEnumBounds.Center(), curDecalEnumBounds.Radius() + m_enumerationPadding);

			// Build triangle enumeration query
			m_triEnumQuery.m_exclusionMask = 0x00A00000;		// Exclude water and foliage surfaces
			m_triEnumQuery.m_queryOnOriginalGeometry = TRUE;
			m_triEnumQuery.m_colSphere = m_decalEnumerationBounds;
			m_triEnumQuery.m_colTriVerts->Clear();
			m_triEnumQuery.m_colTriNormals->Clear();
			m_triEnumQuery.m_colTriFlags->Clear();

			// Perform enumeration query
			if(m_receiver->GetType().IsSubClassOf(CRuEntity::Type()))
			{
				static_cast<CRuEntity *>(m_receiver)->QueryCollision(&m_triEnumQuery);
			}
			else if(m_receiver->GetType().IsSubClassOf(IRuCollisionObject::Type()))
			{
				static_cast<IRuCollisionObject *>(m_receiver)->QueryCollision(&m_triEnumQuery);
			}

			// Iterate through colliding triangles and add them to the decal mesh
			CRuMorphTarget *morphTarget = static_cast<CRuMorphTarget *>(m_mesh->GetMorphTarget(0));
			CRuVector3 *position = morphTarget->GetPosition();
			DWORD *diffuse = morphTarget->GetDiffuse();
			UINT16 *indices = m_mesh->GetIndices();

			INT32 vIndex = 0;
			INT32 vSrcIdx = 0;
			INT32 iIndex = 0;
			INT32 tIndex = 0;

			INT32 maxVertsToProcess = min(m_maximumDecalFaces * 3, m_triEnumQuery.m_colTriVerts->Count());

			CRuVector3 casterPos = casterBounds.Center();

			for(vIndex = 0, vSrcIdx = 0, iIndex = 0, tIndex = 0; vSrcIdx < maxVertsToProcess; vSrcIdx += 3, ++tIndex)
			{
				CRuVector3 &triNormal = m_triEnumQuery.m_colTriNormals->Get(tIndex);

				CRuVector3 nearestPtOnTri = ClosestPointOnTriangle(m_triEnumQuery.m_colTriVerts->Get(vSrcIdx), m_triEnumQuery.m_colTriVerts->Get(vSrcIdx + 1), m_triEnumQuery.m_colTriVerts->Get(vSrcIdx + 2), casterPos);
				CRuVector3 triFacingDir = (nearestPtOnTri - casterPos);

				if(DotProduct(triFacingDir, m_shadowDirection) > 0.0f)
				{
					position[vIndex] = m_triEnumQuery.m_colTriVerts->Get(vSrcIdx);
					position[vIndex + 1] = m_triEnumQuery.m_colTriVerts->Get(vSrcIdx + 1);
					position[vIndex + 2] = m_triEnumQuery.m_colTriVerts->Get(vSrcIdx + 2);

					diffuse[vIndex] = 0xFFFFFFFF;
					diffuse[vIndex + 1] = 0xFFFFFFFF;
					diffuse[vIndex + 2] = 0xFFFFFFFF;

					indices[iIndex] = iIndex;
					indices[iIndex + 1] = iIndex + 1;
					indices[iIndex + 2] = iIndex + 2;

					vIndex += 3;
					iIndex += 3;
				}
			}

			// Set primitive and vertex count for the mesh
			m_mesh->SetNumPrimitives(iIndex / 3);
			m_mesh->SetNumVertices(vIndex);

			// Increment dynamic mesh iteration
			m_mesh->IncrementMeshIteration();
			m_mesh->IncrementTopologyIteration();

			// Set local entity bounds to be the enumeration sphere
			m_mesh->SetBounds(m_decalEnumerationBounds);
			this->SetLocalEntityBounds(m_decalEnumerationBounds);

			// Enable world transform override
			this->SetOption(ruENTITYOPTION_TRANSFORMMODE, ruENTITYTRANSFORMMODE_OVERRIDE);
		}

		// Calculate shadow camera
		REAL moo = 25.0f;
	/*
		m_shadowCamera->SetOrthographicProjection(1.0f, 10000.0f, moo, moo);
		m_shadowCamera->SetCameraPosition(casterBounds.Center() - m_shadowDirection * moo);
		m_shadowCamera->SetTargetPosition(casterBounds.Center());
	*/
		m_shadowCamera->SetOrthographicProjection(1.0f, 10000.0f, casterBounds.Radius() * 2, casterBounds.Radius() * 2);
		m_shadowCamera->SetCameraPosition(casterBounds.Center() - m_shadowDirection * casterBounds.Radius());
		m_shadowCamera->SetTargetPosition(casterBounds.Center());
	}

	return TRUE;
}

INT32 CRuEntity_DecalShadow::GetNumMeshes() const
{
	if((m_casters.Count() > 0) && m_receiver && m_decalEnabled && m_shadowMode != ruDECALSHADOWMODE_NONE)
	{
		return 1;
	}

	return 0;
}

BOOL CRuEntity_DecalShadow::GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut)
{
	if((m_casters.Count() > 0) && m_receiver && m_decalEnabled && meshIndex == 0 && m_shadowMode != ruDECALSHADOWMODE_NONE)
	{
		*meshOut = m_mesh;
		*materialOut = m_material;

		return TRUE;
	}

	return FALSE;
}

BOOL CRuEntity_DecalShadow::HandleEvent_Submitted(RuEventArgs *eventArgs)
{
	switch(m_shadowMode)
	{
		case ruDECALSHADOWMODE_NORMAL:
			{
				if(m_casters.Count() > 0)
				{
					RuEntityEvent_Submitted_Args *submittedEventArgs = static_cast<RuEntityEvent_Submitted_Args *>(eventArgs);
					CRuCamera *camera = submittedEventArgs->m_camera;

					// Do some tests with the camera.... probably inflate camera frustum planes and test against that

					BOOL result = g_ruResourceManager->RestoreRenderTargetView(m_textureView);

					if(result)
					{
						// Set render target texture into decal material
						m_material->SetTexture(ruTEXCHANNEL_DIFFUSEMAP, m_textureView->GetTexture());

						// Set clear parameters for the texture view
						IRuViewport *viewport = m_textureView->GetViewport();
						viewport->SetClearFlags(RUCLEAR_TARGET | RUCLEAR_ZBUFFER | RUCLEAR_STENCIL);
						viewport->SetClearColor(0x00000000);
						viewport->SetClearStencil(0);
						viewport->SetClearZ(1.0f);

						// Setup the render context
						m_renderContext->SetParentRenderer(g_ruRenderer);
						m_renderContext->SetRenderTarget(m_textureView);
						m_renderContext->PersistAcrossFrames(FALSE);

						// Make the original render context dependent on this one
						submittedEventArgs->m_renderContext->AddDependency(m_renderContext, TRUE);

						// Forcibly call begin on the new render context
						m_renderContext->Begin();

						// Push camera into render context
						m_renderContext->Camera_Push(m_shadowCamera);

						// Push suppressed events
						m_renderContext->EventSuppression_Push(ruENTITYEVENT_BROADCAST_SUBMITTED);

						// Push material override callback into scene manager
						m_renderContext->MaterialOverrideCallback_Push(boost::bind(&CRuEntity_DecalShadow::MaterialOverrideFunctor, this, _1));

						// Clear entity stack and then push caster root onto entity stack
						m_entityStack.Clear();

						for(INT32 i = 0; i < m_casters.Count(); ++i)
						{
							m_entityStack.Push(m_casters[i]);
						}

						// Enumerate objects for rendering
						while(!m_entityStack.IsEmpty())
						{
							CRuEntity *curEntity = m_entityStack.Pop();

							const std::type_info &info = typeid(curEntity);

							// Push all children onto entity stack
							CRuEntity *curChild = curEntity->GetFirstChild();
							while(curChild)
							{
								// Push child onto work stack
								m_entityStack.Push(curChild);

								// Advance to next child
								curChild = curChild->GetNextSibling();
							}

							if(curEntity->GetOption(ruENTITYOPTION_CASTERCLASS) > 0)
							{
								// If the entity is renderable, process it
								if(curEntity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
								{
									// Submit caster for render using light-view camera
									m_renderContext->Submit(curEntity);
								}
							}
						}

						// Pop material override callback
						m_renderContext->MaterialOverrideCallback_Pop();

						// Pop suppressed events
						m_renderContext->EventSuppression_Pop();

						// Attach render context
						g_ruRenderer->AttachRendererNode(m_renderContext);

						// Enable decal
						m_decalEnabled = TRUE;
					}
					else
					{
						m_material->SetTexture(ruTEXCHANNEL_DIFFUSEMAP, NULL);

						// Disable decal
						m_decalEnabled = FALSE;
					}

					m_clampingTransform.SetToIdentity();

					m_clampingTransform._11 = 0.5f;
					m_clampingTransform._22 = -0.5f;
					m_clampingTransform._41 = 0.5f;
					m_clampingTransform._42 = 0.5f;

					m_clampingTransform = m_shadowCamera->GetFrustumTransform() * m_clampingTransform;

					m_mappingTransform = m_clampingTransform * m_textureView->GetTextureViewTransform();

					// Set UV transform into material
					m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_TEXTURE_TRANSFORM_0, reinterpret_cast<UINT64>(&m_mappingTransform));
					m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_TEXTURE_TRANSFORM_1, reinterpret_cast<UINT64>(&m_clampingTransform));
				}
			}

			break;

		case ruDECALSHADOWMODE_SIMPLE:
			{
				if(m_primaryCasterIdx >= 0 && m_primaryCasterIdx < m_casters.Count())
				{
					// Enable decal
					m_decalEnabled = TRUE;

					// Set dot shadow texture into decal material
					m_material->SetTexture(ruTEXCHANNEL_DIFFUSEMAP, m_dotTexture);

					CRuMatrix4x4 identityTransform;
					identityTransform.SetToIdentity();

					CRuVector3 decalUVBasis[3] = { CRuVector3(-1.0f, 0.0f, 0.0f), CRuVector3(0.0f, 0.0f, 1.0f), CRuVector3(0.0f, 1.0f, 0.0f) };

					m_clampingTransform = RuUVTransform_Projection(m_casters[m_primaryCasterIdx]->GetTranslation(ruFRAMETYPE_WORLD), identityTransform, decalUVBasis, m_dotShadowSize, 0.5f, 0.5f);
					m_mappingTransform = m_clampingTransform;

					// Set UV transform into material
					m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_TEXTURE_TRANSFORM_0, reinterpret_cast<UINT64>(&m_mappingTransform));
					m_material->GetParameterBlock()->SetParameter(ruPARAMTYPE_TEXTURE_TRANSFORM_1, reinterpret_cast<UINT64>(&m_clampingTransform));
				}
				else
				{
					// Disable decal
					m_decalEnabled = FALSE;
				}
			}

			break;
	}

	return TRUE;
}

IRuMaterial *CRuEntity_DecalShadow::MaterialOverrideFunctor(IRuMaterial *sourceMaterial)
{
	if(sourceMaterial->IsSkinned())
	{
		return m_shadowRenderMaterial_Skin;
	}

	return m_shadowRenderMaterial_Static;
}

BOOL CRuEntity_DecalShadow::CollisionContainerChangedHandler(RuEventArgs *eventArgs)
{
	RuColContainerEvent_Changed_Args *changedArgs = static_cast<RuColContainerEvent_Changed_Args *>(eventArgs);

	if(Intersects(changedArgs->m_changeBounds, m_decalEnumerationBounds))
	{
		m_decalEnumerationBounds.Radius() = 0.0f;
	}

	return TRUE;
}

BOOL CRuEntity_DecalShadow::CollisionContainerDisposeHandler(RuEventArgs *eventArgs)
{
	SetReceiver(NULL);
	return TRUE;
}

CRuSphere CRuEntity_DecalShadow::GetCasterBounds()
{
	CRuSphere casterBounds(CRuVector3(0.0f, 0.0f, 0.0f), 1.0f);

	if(m_casters.Count() > 0)
	{
		casterBounds = m_casters[0]->GetWorldBounds();

		for(INT32 i = 1; i < m_casters.Count(); ++i)
		{
			casterBounds.Merge(m_casters[i]->GetWorldBounds());
		}
	}
/*
	char moo[2000];
	sprintf(moo, "%f %f %f %f\n", casterBounds.Center().m_x, casterBounds.Center().m_y, casterBounds.Center().m_z, casterBounds.Radius());
	OutputDebugString(moo);
*/
	return casterBounds;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
