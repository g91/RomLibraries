/*!
	@file RuRendererPlugin_VizCon.cpp

	Copyright (c) 2004-2007 All rights reserved
*/

#include "../../Renderer/Plugin/RuRendererPlugin_VizCon.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuRendererPlugin_VizCon, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuRendererPlugin_VizCon, "CRuRendererPlugin_VizCon", "IRuObject")
ruCLASSGUID_DEFINE(CRuRendererPlugin_VizCon, 0x4F712A90, 0xBC7E462c, 0x80187BE6, 0xA8DE33A5)

// ************************************************************************************************************************************************************

CRuRendererPlugin_VizCon::CRuRendererPlugin_VizCon()
:	m_enabled(TRUE)
{
	// Apply settings
	GetSettingsFromEngine();

	// Register listener on engine settings
	g_ruEngineSettings->RegisterSettingsChangeHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_VizCon::EngineSettingsChangedHandler, this, _1, _2));
}

CRuRendererPlugin_VizCon::~CRuRendererPlugin_VizCon()
{
}

BOOL CRuRendererPlugin_VizCon::RegisterPlugin(CRuRendererNode_RenderContext *renderContext)
{
	// Register event handlers
	renderContext->Event_Submit().RegisterHandler(this->GetGUID(), boost::bind(&CRuRendererPlugin_VizCon::HandleEvent_Submit, this, _1));

	return TRUE;
}

BOOL CRuRendererPlugin_VizCon::HandleEvent_Submit(RuEventArgs *eventArgs)
{
	INT32 numDistanceClasses = 8;
	REAL distanceClassSizes[] =		{ 150.000f, 125.000f, 100.000f,  50.000f,  25.000f,  12.500f,  6.250f,  0.000f };
	REAL distanceClasses[] =		{   1.000f,   0.900f,   0.750f,   0.625f,   0.400f,   0.200f,  0.150f,  0.125f };
	REAL distanceClassBuffer[] =	{   0.050f,   0.050f,   0.050f,   0.050f,   0.050f,   0.025f,  0.013f,  0.010f };
/*
	INT32 numDistanceClasses = 6;
	REAL distanceClassSizes[] =		{  100.0f,   50.0f,  25.0f,  12.5f,  6.25f,   0.0f };
	REAL distanceClasses[] =		{ 1500.0f, 1250.0f, 800.0f, 400.0f, 300.0f, 250.0f };
	REAL distanceClassBuffer[] =	{  100.0f,  100.0f,  75.0f,  50.0f,  25.0f,  20.0f };
*/

	RuRendererEvent_Submit_Args *submittedArgs = static_cast<RuRendererEvent_Submit_Args *>(eventArgs);
	CRuRendererNode_RenderContext *renderContext = submittedArgs->m_renderContext;
	CRuCamera *camera = submittedArgs->m_camera;
	CRuEntity *entity = submittedArgs->m_entity;

	RuEntityCullMode cullMode = GetEffectiveCullMode(entity);

	if(m_enabled && entity->GetOption(ruENTITYOPTION_ENABLEDISTANCEFADE))
	{
		if(cullMode == ruENTITYCULLMODE_NORMAL)
		{
			const CRuSphere &entityWorldBounds = entity->GetWorldBounds();

			// Determine distance class
			INT32 distanceClass = 0;
			for(INT32 i = 0; i < numDistanceClasses; ++i)
			{
				if(entityWorldBounds.Radius() > distanceClassSizes[i])
				{
					break;
				}

				++distanceClass;
			}

			REAL distance = Distance(entityWorldBounds, camera->GetCameraPosition()) / camera->GetFarPlaneDistance();

			if(distance < distanceClasses[distanceClass])
			{
				REAL alpha = min(1.0f, (distanceClasses[distanceClass] - distance) / distanceClassBuffer[distanceClass]);
				entity->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODIFIER, RuCastFloatToINT64(alpha));

				entity->SetOption(ruENTITYOPTION_HIDDENBYDISTANCE, FALSE);
			}
			else
			{
				entity->SetOption(ruENTITYOPTION_HIDDENBYDISTANCE, TRUE);
			}
		}

		if(entity->GetStatusFlag(ruENTITYSTATUS_VISIBLE) == FALSE)
		{
			entity->SetMinUpdateInterval(5.0f);
		}
		else
		{
			entity->SetMinUpdateInterval(0.033f);
		}
	}
	else if(entity->GetOption(ruENTITYOPTION_ENABLEDISTANCEFADE))
	{
		REAL alpha = 1.0f;
		entity->GetParameterBlock()->SetParameter(ruPARAMTYPE_MATERIAL_ALPHAMODIFIER, RuCastFloatToINT64(alpha));
		entity->SetOption(ruENTITYOPTION_HIDDENBYDISTANCE, FALSE);
	}

	return TRUE;
}

RuEntityCullMode CRuRendererPlugin_VizCon::GetEffectiveCullMode(CRuEntity *entity)
{
	RuEntityCullMode cullMode = ruENTITYCULLMODE_NORMAL;

	while(entity)
	{
		RuEntityCullMode curCullMode = static_cast<RuEntityCullMode>(entity->GetOption(ruENTITYOPTION_CULLMODE));

		switch(curCullMode)
		{
			case ruENTITYCULLMODE_NORMAL:
				break;

			case ruENTITYCULLMODE_NEVER:
				cullMode = curCullMode;
				break;

			case ruENTITYCULLMODE_ATTACHED:
				{
					switch(cullMode)
					{
						case ruENTITYCULLMODE_NORMAL:
							cullMode = curCullMode;
							break;

						case ruENTITYCULLMODE_ATTACHED:
							break;
					}
				}

				break;
		}

		if(cullMode == ruENTITYCULLMODE_NEVER)
			break;

		entity = entity->GetParent();
	}

	return cullMode;
}

void CRuRendererPlugin_VizCon::GetSettingsFromEngine()
{
	RuPropertyType propertyValue;

	if(g_ruEngineSettings->GetSetting("vizCon:enable", propertyValue))
	{
		m_enabled = boost::get<INT32>(propertyValue);
	}
}

BOOL CRuRendererPlugin_VizCon::EngineSettingsChangedHandler(const char *settingName, const RuPropertyType &setting)
{
	GetSettingsFromEngine();
	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
