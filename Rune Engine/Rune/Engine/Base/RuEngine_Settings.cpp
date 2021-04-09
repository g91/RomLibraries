/*!
	@file RuEngine_Settings.cpp

	Copyright (c) 2004-2006 All rights reserved
*/

#include "../../Engine/Base/RuEngine_Settings.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEngine_Settings, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuEngine_Settings, "CRuEngine_Settings", "IRuObject")
ruCLASSGUID_DEFINE(CRuEngine_Settings, 0xDB534F52, 0x79634777, 0x9D70149E, 0x482F8AB6)

// ************************************************************************************************************************************************************

CRuEngine_Settings::CRuEngine_Settings()
:	m_propertyContainer(NULL)
{
	m_propertyContainer = ruNEW CRuPropertyContainer();
}

CRuEngine_Settings::~CRuEngine_Settings()
{
	for(INT32 i = 0; i < m_settingsChangedHandlers.Count(); ++i)
	{
		delete m_settingsChangedHandlers[i];
	}

	ruSAFE_RELEASE(m_propertyContainer);
}

BOOL CRuEngine_Settings::GetSetting(const char *settingName, RuPropertyType &settingOut)
{
	return m_propertyContainer->GetProperty(settingName, settingOut);
}

BOOL CRuEngine_Settings::SetSetting(const char *settingName, const RuPropertyType &setting)
{
	// Set setting
	BOOL retVal = m_propertyContainer->SetProperty(settingName, setting);

	// Invoke setting change handler
	for(INT32 i = 0; i < m_settingsChangedHandlers.Count(); ++i)
	{
		m_settingsChangedHandlers[i]->m_handler(settingName, setting);
	}

	return retVal;
}

BOOL CRuEngine_Settings::RegisterSettingsChangeHandler(const CRuGUID &guid, SettingsChangedHandler handler)
{
	// Search existing handlers for matching GUID
	for(INT32 i = 0; i < m_settingsChangedHandlers.Count(); ++i)
	{
		// If GUID matches, replace handler
		if(m_settingsChangedHandlers[i]->m_guid == guid)
		{
			m_settingsChangedHandlers[i]->m_handler = handler;
			return TRUE;
		}
	}

	// Create new descriptor entry for the new handler
	ChangeHandlerDesc *handlerDesc = ruNEW ChangeHandlerDesc();
	handlerDesc->m_guid = guid;
	handlerDesc->m_handler = handler;
	m_settingsChangedHandlers.Add(handlerDesc);

	return TRUE;
}

BOOL CRuEngine_Settings::UnregisterSettingsChangeHandler(const CRuGUID &guid)
{
	// Search existing handlers for matching GUID
	for(INT32 i = 0; i < m_settingsChangedHandlers.Count(); ++i)
	{
		// If GUID matches, remove handler
		if(m_settingsChangedHandlers[i]->m_guid == guid)
		{
			delete m_settingsChangedHandlers[i];
			m_settingsChangedHandlers.RemoveAt(i);
			return TRUE;
		}
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

CRuEngine_Settings*			g_ruEngineSettings = NULL;

// ************************************************************************************************************************************************************

#pragma managed(pop)
