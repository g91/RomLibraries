#include "../../Scene/Terrain/RuEntity_Environment.h"

#include "../../Helper/RuHelper_Math.h"
#include "../../Helper/RuHelper_Entity.h"

#include "../../Renderer/RuRenderer.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Environment, CRuEntity_Container_Impl)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Environment, "CRuEntity_Environment", "CRuEntity_Container_Impl")
ruCLASSGUID_DEFINE(CRuEntity_Environment, 0x9DA68307, 0xBE0244d4, 0x9321679F, 0xC5FF96FD)

// ************************************************************************************************************************************************************

CRuEntity_Environment::CRuEntity_Environment()
{
	m_iteration = 1;
	m_lastApplyIteration = 0;

	m_camera = NULL;

	m_activeZoneDesc = NULL;

	m_activeZoneSettings = NULL;
	m_queuedZoneSettings = NULL;

	m_zoneSettingsDefaultTransitionDuration = 3.0f;
	m_zoneSettingsTransitionT = 0.0f;
	m_zoneSettingsTransitionDuration = 0.0f;
	m_zoneSettings0 = NULL;
	m_zoneSettings1 = NULL;

	m_fogOverride_Enable = FALSE;
	m_fogOverride_Color = 0xFFFFFFFF;
	m_fogOverride_Start = 0.0f;
	m_fogOverride_End = 0.5f;
	m_fogOverride_Constant = 0.5f;

	m_lightFrame = NULL;
	m_ambientLight = NULL;
	m_envLight0 = NULL;
	m_envLight1 = NULL;
	m_envLight2 = NULL;

	m_skyDome = NULL;
	m_sky_Gradient = NULL;
	m_sky_Horizon = NULL;
	m_sky_Overhead = NULL;

	m_customSkyBox0 = NULL;
	m_customSkyBox1 = NULL;

	// Ensure that the environment object is not included in bounding volume
	this->SetOption(ruENTITYOPTION_EXCLUDEBOUNDS, TRUE);
}

CRuEntity_Environment::~CRuEntity_Environment()
{
	ruSAFE_RELEASE(m_camera);

	ruSAFE_RELEASE(m_activeZoneDesc);

	ruSAFE_RELEASE(m_activeZoneSettings);
	ruSAFE_RELEASE(m_queuedZoneSettings);

	ruSAFE_RELEASE(m_zoneSettings0);
	ruSAFE_RELEASE(m_zoneSettings1);

	ruSAFE_RELEASE(m_lightFrame);
	ruSAFE_RELEASE(m_ambientLight);
	ruSAFE_RELEASE(m_envLight0);
	ruSAFE_RELEASE(m_envLight1);
	ruSAFE_RELEASE(m_envLight2);

	ruSAFE_RELEASE(m_skyDome);
	ruSAFE_RELEASE(m_sky_Gradient);
	ruSAFE_RELEASE(m_sky_Horizon);
	ruSAFE_RELEASE(m_sky_Overhead);

	ruSAFE_RELEASE(m_customSkyBox0);
	ruSAFE_RELEASE(m_customSkyBox1);
}

BOOL CRuEntity_Environment::SerializeFrom(IRuStream *stream)
{
	return FALSE;
}

BOOL CRuEntity_Environment::SerializeTo(IRuStream *stream)
{
	return FALSE;
}

BOOL CRuEntity_Environment::ResetStates()
{
	if(m_skyDome)
	{
		m_skyDome->ResetStates();
	}

	return TRUE;
}

BOOL CRuEntity_Environment::Update(float elapsedTime)
{
	if(m_zoneSettings0)
	{
		CRuColor4 color_Primary;
		CRuColor4 color_Secondary;
		CRuColor4 color_Ambient;
		CRuColor4 color_Specular;
		CRuColor4 color_Fog;

		REAL fog_Start;
		REAL fog_End;
		REAL fog_Constant;

		REAL theta_Standard;
		REAL phi_Standard;

		REAL theta_Specular;
		REAL phi_Specular;

		// Interpolate lighting values
		if(m_zoneSettingsTransitionDuration > 0.0f && m_zoneSettings1)
		{
			REAL t1 = m_zoneSettingsTransitionT / m_zoneSettingsTransitionDuration;
			REAL t0 = 1.0f - t1;

			color_Primary = CRuColor4(m_zoneSettings0->m_color_Primary) * t0 + CRuColor4(m_zoneSettings1->m_color_Primary) * t1;
			color_Secondary = CRuColor4(m_zoneSettings0->m_color_Secondary) * t0 + CRuColor4(m_zoneSettings1->m_color_Secondary) * t1;
			color_Ambient = CRuColor4(m_zoneSettings0->m_color_Ambient) * t0 + CRuColor4(m_zoneSettings1->m_color_Ambient) * t1;
			color_Specular = CRuColor4(m_zoneSettings0->m_color_Specular) * t0 + CRuColor4(m_zoneSettings1->m_color_Specular) * t1;
			color_Fog = CRuColor4(m_zoneSettings0->m_color_Fog) * t0 + CRuColor4(m_zoneSettings1->m_color_Fog) * t1;

			fog_Start = m_zoneSettings0->m_fog_Start * t0 + m_zoneSettings1->m_fog_Start * t1;
			fog_End = m_zoneSettings0->m_fog_End * t0 + m_zoneSettings1->m_fog_End * t1;
			fog_Constant = m_zoneSettings0->m_fog_Constant * t0 + m_zoneSettings1->m_fog_Constant * t1;

			theta_Standard = m_zoneSettings0->m_theta_Standard * t0 + m_zoneSettings1->m_theta_Standard * t1;
			phi_Standard = m_zoneSettings0->m_phi_Standard * t0 + m_zoneSettings1->m_phi_Standard * t1;

			theta_Specular = m_zoneSettings0->m_theta_Specular * t0 + m_zoneSettings1->m_theta_Specular * t1;
			phi_Specular = m_zoneSettings0->m_phi_Specular * t0 + m_zoneSettings1->m_phi_Specular * t1;

			if(m_customSkyBox0)
			{
				m_customSkyBox0->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(t0));
			}

			if(m_customSkyBox1)
			{
				m_customSkyBox1->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHA, RuCastFloatToINT64(t1));
			}

			// Update transition time and terminate transition if criteria are met
			m_zoneSettingsTransitionT += elapsedTime;
			if(m_zoneSettingsTransitionT >= m_zoneSettingsTransitionDuration)
			{
				m_zoneSettingsTransitionT = 0.0f;
				m_zoneSettingsTransitionDuration = 0.0f;

				ruSAFE_RELEASE(m_zoneSettings0);
				m_zoneSettings0 = m_zoneSettings1;
				m_zoneSettings1 = NULL;

				ruSAFE_DETACH(m_customSkyBox0);
				ruSAFE_RELEASE(m_customSkyBox0);
				m_customSkyBox0 = m_customSkyBox1;
				m_customSkyBox1 = NULL;
			}

			++m_iteration;
		}
		else
		{
			color_Primary = CRuColor4(m_zoneSettings0->m_color_Primary);
			color_Secondary = CRuColor4(m_zoneSettings0->m_color_Secondary);
			color_Ambient = CRuColor4(m_zoneSettings0->m_color_Ambient);
			color_Specular = CRuColor4(m_zoneSettings0->m_color_Specular);
			color_Fog = CRuColor4(m_zoneSettings0->m_color_Fog);

			fog_Start = m_zoneSettings0->m_fog_Start;
			fog_End = m_zoneSettings0->m_fog_End;
			fog_Constant = m_zoneSettings0->m_fog_Constant;

			theta_Standard = m_zoneSettings0->m_theta_Standard;
			phi_Standard = m_zoneSettings0->m_phi_Standard;

			theta_Specular = m_zoneSettings0->m_theta_Specular;
			phi_Specular = m_zoneSettings0->m_phi_Specular;

			// If there is a queued zone transition, apply it now
			if(m_queuedZoneSettings)
			{
				// Apply transition
				TransitionZoneSettings(m_queuedZoneSettings, m_zoneSettingsDefaultTransitionDuration);

				// Reset queued zone ID
				ruSAFE_RELEASE(m_activeZoneSettings);
				m_activeZoneSettings = m_queuedZoneSettings;
				m_queuedZoneSettings = NULL;
			}
		}

		// Apply lighting values into system
		if(m_iteration != m_lastApplyIteration)
		{
			// Set light direction
			m_envLight0->SetDirection(RuMathHelper_GenerateLightingVector(theta_Standard, phi_Standard));

			// Make secondary light direction opposite primary light
			REAL fraction = ruPI * 2.0f / 3.0f;
			m_envLight1->SetDirection(RuMathHelper_GenerateLightingVector(theta_Standard + fraction, 0.0f));
			m_envLight2->SetDirection(RuMathHelper_GenerateLightingVector(theta_Standard + fraction * 2, 0.0f));

			// Set light colors
			m_ambientLight->SetDiffuse(color_Ambient);
			m_envLight0->SetDiffuse(color_Primary);
			m_envLight1->SetDiffuse(color_Secondary * 0.80f);
			m_envLight2->SetDiffuse(color_Secondary * 0.80f);

			// Update lights
			m_ambientLight->Update(elapsedTime);
			m_envLight0->Update(elapsedTime);
			m_envLight1->Update(elapsedTime);
			m_envLight2->Update(elapsedTime);

			// Update lighting tree
			m_lightFrame->Update(elapsedTime);

			// Convert fog values
			if(fog_End > 1.0f)
			{
				fog_Start = fog_Start / 1600.0f;
				fog_End = fog_End / 1600.0f;
			}

			fog_Start = fog_Start * m_camera->GetFarPlaneDistance();
			fog_End = fog_End * m_camera->GetFarPlaneDistance();

			// Set fog parameters into the scene parameter block
			if(m_fogOverride_Enable)
			{
				g_ruRenderer->GetDefaultParameterBlock().SetParameter(ruPARAMTYPE_FOG_START, RuCastFloatToINT64(m_fogOverride_Start * m_camera->GetFarPlaneDistance()));
				g_ruRenderer->GetDefaultParameterBlock().SetParameter(ruPARAMTYPE_FOG_END, RuCastFloatToINT64(m_fogOverride_End * m_camera->GetFarPlaneDistance()));
				g_ruRenderer->GetDefaultParameterBlock().SetParameter(ruPARAMTYPE_FOG_CONSTANT, RuCastFloatToINT64(m_fogOverride_Constant));
				g_ruRenderer->GetDefaultParameterBlock().SetParameter(ruPARAMTYPE_FOG_COLOR, m_fogOverride_Color);
			}
			else
			{
				g_ruRenderer->GetDefaultParameterBlock().SetParameter(ruPARAMTYPE_FOG_START, RuCastFloatToINT64(fog_Start));
				g_ruRenderer->GetDefaultParameterBlock().SetParameter(ruPARAMTYPE_FOG_END, RuCastFloatToINT64(fog_End));
				g_ruRenderer->GetDefaultParameterBlock().SetParameter(ruPARAMTYPE_FOG_CONSTANT, RuCastFloatToINT64(fog_Constant));
				g_ruRenderer->GetDefaultParameterBlock().SetParameter(ruPARAMTYPE_FOG_COLOR, color_Fog.GetARGB());
			}

			// Set specular color
			g_ruRenderer->SetSpecularColor(color_Specular.GetARGB());

			// Set specular direction
			g_ruRenderer->SetSpecularDirection(RuMathHelper_GenerateLightingVector(theta_Specular, phi_Specular) * -1.0f);

			// Set lighting prop
			g_ruRenderer->SetLightingProp(m_lightFrame);

			// Update iteration
			m_lastApplyIteration = m_iteration;
		}
	}

	REAL multiplier0 = 1.0f;
	REAL multiplier1 = 1.0f;

	if(m_zoneSettings0)
	{
		multiplier0 = m_zoneSettings0->m_frameRateMultiplier;
	}

	if(m_zoneSettings1)
	{
		multiplier1 = m_zoneSettings1->m_frameRateMultiplier;
	}

	if(m_skyDome)
	{
		m_skyDome->Update(elapsedTime);
	}

	if(m_customSkyBox0)
	{
		m_customSkyBox0->Update(elapsedTime * multiplier0);
	}

	if(m_customSkyBox1)
	{
		m_customSkyBox1->Update(elapsedTime * multiplier0);
	}

	// Chain update call to parent class
	return CRuEntity_Container_Impl::Update(elapsedTime);
}

BOOL CRuEntity_Environment::Initialize(INT32 horzSegments, INT32 vertSegments, float vertSweep, float radius, float vertScale, float horzCompression, INT32 cloudPlaneSegments)
{
	// Create lighting set
	m_lightFrame = ruNEW CRuFrame();

	m_ambientLight = ruNEW CRuEntity_Light();
	m_envLight0 = ruNEW CRuEntity_Light();
	m_envLight1 = ruNEW CRuEntity_Light();
	m_envLight2 = ruNEW CRuEntity_Light();

	CRuVector3 direction0(0.5f, -1.0f, 0.5f);
	CRuVector3 direction1(0.0f, -0.5f, 1.0f);

	direction0.Normalize();
	direction1.Normalize();

	m_ambientLight->SetLightType(ruLIGHTTYPE_AMBIENT);
	m_ambientLight->SetDiffuse(CRuColor4(1.0f, 0.8f, 0.8f, 0.8f));
	m_ambientLight->Update(0.0f);

	m_envLight0->SetLightType(ruLIGHTTYPE_DIRECTIONAL);
	m_envLight0->SetDirection(direction0);
	m_envLight0->SetDiffuse(CRuColor4(1.0f, 0.8f, 0.8f, 0.8f));
	m_envLight0->Update(0.0f);

	m_envLight1->SetLightType(ruLIGHTTYPE_DIRECTIONAL);
	m_envLight1->SetDirection(direction1);
	m_envLight1->SetDiffuse(CRuColor4(1.0f, 0.2f, 0.2f, 0.2f));
	m_envLight1->Update(0.0f);

	m_envLight2->SetLightType(ruLIGHTTYPE_DIRECTIONAL);
	m_envLight2->SetDirection(direction1);
	m_envLight2->SetDiffuse(CRuColor4(1.0f, 0.2f, 0.2f, 0.2f));
	m_envLight2->Update(0.0f);

	m_lightFrame->AddChild(m_ambientLight);
	m_lightFrame->AddChild(m_envLight0);
	m_lightFrame->AddChild(m_envLight1);
	m_lightFrame->AddChild(m_envLight2);

	// Release old sky dome
	ruSAFE_RELEASE(m_skyDome);

	// Create new sky dome
	m_skyDome = ruNEW CRuSkyDome();
	m_skyDome->Initialize(horzSegments, vertSegments, vertSweep, radius, vertScale, horzCompression, cloudPlaneSegments);

	this->AddChild(m_skyDome);

	return TRUE;
}

void CRuEntity_Environment::SetCamera(CRuCamera *viewCamera)
{
	viewCamera->AddRef();
	ruSAFE_RELEASE(m_camera);
	m_camera = viewCamera;

	if(m_skyDome)
	{
		m_skyDome->SetCamera(viewCamera);
	}

	if(m_customSkyBox0)
	{
 		m_customSkyBox0->SetTranslation(viewCamera->GetCameraPosition());
	}

	if(m_customSkyBox1)
	{
		m_customSkyBox1->SetTranslation(viewCamera->GetCameraPosition());
	}
}

BOOL CRuEntity_Environment::SetZoneSettings(CRuWorld_ZoneSettings *zoneSettings)
{
	// Abort if the specified zone setting is invalid
	if(zoneSettings == NULL)
	{
		return FALSE;
	}

	// Increment iteration
	++m_iteration;

	// Store parameters
	m_zoneSettingsTransitionT = 0.0f;
	m_zoneSettingsTransitionDuration = 0.0f;

	// Store reference to zone transition target
	zoneSettings->AddRef();
	ruSAFE_RELEASE(m_zoneSettings0);
	m_zoneSettings0 = zoneSettings;

	ruSAFE_RELEASE(m_zoneSettings1);

	// Check fog settings
	if(zoneSettings->m_fog_End > 1.0f)
	{
		zoneSettings->m_fog_Start = max(0.001f, zoneSettings->m_fog_Start / 1600.0f);
		zoneSettings->m_fog_End = min(1.0f, zoneSettings->m_fog_End / 1600.0f);
	}

	// Load the specified sky gradient texture
	ruSAFE_RELEASE(m_sky_Gradient);
	if(m_zoneSettings0->m_skyGradientTextureName)
	{
		m_sky_Gradient = g_ruResourceManager->LoadTexture(m_zoneSettings0->m_skyGradientTextureName, ruSURFACEFMT_DEFAULT, 0, TRUE);
	}

	// Set the parameters into sky dome
	if(m_skyDome)
	{
		if(m_sky_Gradient)
		{
			m_skyDome->SetSkyDomeGradientTexture(m_sky_Gradient);
		}

		// Set cloud plane parameters
		m_skyDome->SetCloudCover(m_zoneSettings0->m_cloudCover);
		m_skyDome->SetCloudSharpness(m_zoneSettings0->m_cloudSharpness);
		m_skyDome->SetCloudColor0(CRuColor4(m_zoneSettings0->m_cloudColor0));
		m_skyDome->SetCloudColor1(CRuColor4(m_zoneSettings0->m_cloudColor1));
		m_skyDome->SetOctaveWeight(m_zoneSettings0->m_octaveWeight);
		m_skyDome->SetOctaveScrollSpeed(m_zoneSettings0->m_octaveScrollSpeed);
	}

	// Load the custom sky box if it is available
	if(m_zoneSettings0->m_customSkyBoxName)
	{
		if(m_customSkyBox0 == NULL || stricmp(m_customSkyBox0->GetResourceName(), m_zoneSettings0->m_customSkyBoxName) != 0)
		{
			// Load sky box
			ruSAFE_RELEASE(m_customSkyBox0);
			m_customSkyBox0 = g_ruResourceManager->LoadEntity(m_zoneSettings0->m_customSkyBoxName);

			// Begin motion on the sky box
			if(m_customSkyBox0)
				RuACT_PlayMotionEx(m_customSkyBox0, 0xFFFFFFFF, "idle");
		}
	}

	// Reset custom sky box 1
	ruSAFE_RELEASE(m_customSkyBox1);

	// Remove all entities attached to the container
	this->RemoveAllChildren();

	// Insert custom sky box if it is available
	if(m_customSkyBox0)
	{
		this->AddChild(m_customSkyBox0);
	}

	// Insert default sky box if there is no custom sky box loaded
	if(m_customSkyBox0 == NULL && m_customSkyBox1 == NULL)
	{
		this->AddChild(m_skyDome);
	}

	return TRUE;
}

BOOL CRuEntity_Environment::TransitionZoneSettings(CRuWorld_ZoneSettings *zoneSettings, float transitionDuration)
{
	// Abort if the specified zone setting is invalid
	if(zoneSettings == NULL)
	{
		return FALSE;
	}

	// If there is no existing zone descriptor, set this one into the environment directly
	if(m_zoneSettings0 == NULL)
	{
		return SetZoneSettings(zoneSettings);
	}

	// Ignore call if the same transition is already taking place
	if(m_zoneSettings1 == zoneSettings)
	{
		return TRUE;
	}

	// Increment iteration
	++m_iteration;

	// Store parameters
	m_zoneSettingsTransitionT = 0.0f;
	m_zoneSettingsTransitionDuration = transitionDuration;

	// Store reference to zone transition target
	zoneSettings->AddRef();
	ruSAFE_RELEASE(m_zoneSettings1);
	m_zoneSettings1 = zoneSettings;

	// Check fog settings
	if(zoneSettings->m_fog_End > 1.0f)
	{
		zoneSettings->m_fog_Start = max(0.001f, zoneSettings->m_fog_Start / 1600.0f);
		zoneSettings->m_fog_End = min(1.0f, zoneSettings->m_fog_End / 1600.0f);
	}

	// Load the specified sky gradient texture
	ruSAFE_RELEASE(m_sky_Gradient);
	if(m_zoneSettings1->m_skyGradientTextureName)
	{
		m_sky_Gradient = g_ruResourceManager->LoadTexture(m_zoneSettings1->m_skyGradientTextureName, ruSURFACEFMT_DEFAULT, 0, TRUE);
	}

	// Set the textures into sky dome
	if(m_skyDome)
	{
		if(m_sky_Gradient)
		{
			m_skyDome->TransitionSkyDomeGradientTexture(m_sky_Gradient, transitionDuration);
		}

		// Set cloud plane parameters
		m_skyDome->TransitionCloudCover(m_zoneSettings1->m_cloudCover, transitionDuration);
		m_skyDome->TransitionCloudSharpness(m_zoneSettings1->m_cloudSharpness, transitionDuration);
		m_skyDome->TransitionCloudColor0(CRuColor4(m_zoneSettings1->m_cloudColor0), transitionDuration);
		m_skyDome->TransitionCloudColor1(CRuColor4(m_zoneSettings1->m_cloudColor1), transitionDuration);
		m_skyDome->TransitionOctaveWeight(m_zoneSettings1->m_octaveWeight, transitionDuration);
		m_skyDome->TransitionOctaveScrollSpeed(m_zoneSettings1->m_octaveScrollSpeed, transitionDuration);
	}

	// Load the custom sky box if it is available
	if(m_zoneSettings0->m_customSkyBoxName)
	{
		if(m_customSkyBox0 == NULL || strcmp(m_customSkyBox0->GetResourceName(), m_zoneSettings0->m_customSkyBoxName) != 0)
		{
			// Load sky box
			ruSAFE_RELEASE(m_customSkyBox0);
			m_customSkyBox0 = g_ruResourceManager->LoadEntity(m_zoneSettings0->m_customSkyBoxName);

			// Begin motion on the sky box
			if(m_customSkyBox0)
				RuACT_PlayMotionEx(m_customSkyBox0, 0xFFFFFFFF, "idle");
		}
	}

	// Load the custom sky box if it is available
	if(m_zoneSettings1->m_customSkyBoxName)
	{
		if(m_customSkyBox1 == NULL || strcmp(m_customSkyBox1->GetResourceName(), m_zoneSettings1->m_customSkyBoxName) != 0)
		{
			// Load sky box
			ruSAFE_RELEASE(m_customSkyBox1);
			m_customSkyBox1 = g_ruResourceManager->LoadEntity(m_zoneSettings1->m_customSkyBoxName);

			// Begin motion on the sky box
			if(m_customSkyBox1)
				RuACT_PlayMotionEx(m_customSkyBox1, 0xFFFFFFFF, "idle");
		}
	}

	// Remove all entities attached to the container
	this->RemoveAllChildren();

	// Insert custom sky box if it is available
	if(m_customSkyBox0)
	{
		this->AddChild(m_customSkyBox0);
	}
	if(m_customSkyBox1)
	{
		this->AddChild(m_customSkyBox1);
	}

	// Insert default sky box if there is no custom sky box loaded
	if(m_customSkyBox0 == NULL && m_customSkyBox1 == NULL)
	{
		this->AddChild(m_skyDome);
	}

	return TRUE;
}

void CRuEntity_Environment::SelectZoneDescriptor(CRuWorld_ZoneDescriptor *zoneDesc, INT32 timeGroup, INT32 weatherGroup)
{
	if(zoneDesc == NULL)
	{
		return;
	}

	// Get new zone settings
	CRuWorld_ZoneSettings *newZoneSettings = zoneDesc->GetZoneSettings(timeGroup, weatherGroup);

	// Chain call
	SelectZoneDescriptor(zoneDesc, newZoneSettings);
}

void CRuEntity_Environment::SelectZoneDescriptor(CRuWorld_ZoneDescriptor *zoneDesc, CRuWorld_ZoneSettings *zoneSettings)
{
	if(zoneDesc == NULL)
	{
		return;
	}

	// Store active zone descriptor
	if(zoneDesc != m_activeZoneDesc)
	{
		zoneDesc->AddRef();
		ruSAFE_RELEASE(m_activeZoneDesc);
		m_activeZoneDesc = zoneDesc;
	}

	// Set new zone settings if it is different from the current one
	if(zoneSettings && zoneSettings != m_activeZoneSettings)
	{
		if(m_zoneSettingsTransitionDuration > 0.0f || m_skyDome->IsInTransition())
		{
			// Queue the zone for transition
			zoneSettings->AddRef();
			ruSAFE_RELEASE(m_queuedZoneSettings);
			m_queuedZoneSettings = zoneSettings;
		}
		else
		{
			// Set new active zone ID
			zoneSettings->AddRef();
			ruSAFE_RELEASE(m_activeZoneSettings);
			m_activeZoneSettings = zoneSettings;

			// Start transition
			TransitionZoneSettings(zoneSettings, m_zoneSettingsDefaultTransitionDuration);
		}
	}
}

void CRuEntity_Environment::EnableFogOverride(BOOL enable)
{
	m_fogOverride_Enable = enable;
}

void CRuEntity_Environment::SetFogOverrideParameters(RUCOLOR fogColor, REAL fogStart, REAL fogEnd, REAL fogConstant)
{
	m_fogOverride_Color = fogColor;
	m_fogOverride_Start = fogStart;
	m_fogOverride_End = fogEnd;
	m_fogOverride_Constant = fogConstant;
}

INT32 CRuEntity_Environment::IncrementIteration()
{
	++m_iteration;

	return m_iteration;
}

CRuWorld_ZoneDescriptor *CRuEntity_Environment::GetCurrentZoneDescriptor()
{
	return m_activeZoneDesc;
}

CRuWorld_ZoneSettings *CRuEntity_Environment::GetCurrentZoneSettings()
{
	return m_activeZoneSettings;
}

// ************************************************************************************************************************************************************

#pragma managed
