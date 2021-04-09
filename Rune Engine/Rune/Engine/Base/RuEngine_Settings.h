/*!
	@file RuEngine_Settings.h

	Copyright (c) 2004-2006 All rights reserved
*/

#ifndef _RUENGINE_SETTINGS_H_
#define _RUENGINE_SETTINGS_H_

// Rune Engine Base
#include "../../Core/Utility/RuUtility_PropertyContainer.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Engine settings object. Used for global engine settings.

	@author John Tang
	@version 2006/12/10
*/
class CRuEngine_Settings : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	typedef boost::function<BOOL (const char *, const RuPropertyType &)> SettingsChangedHandler;		// Definition: BOOL SettingsCangedHandler(const char *, const RuPropertyType &setting)

protected:
	struct ChangeHandlerDesc
	{
		CRuGUID								m_guid;						//!< Globally unique identifier for the handler
		SettingsChangedHandler				m_handler;					//!< Handler
	};

	CRuPropertyContainer*					m_propertyContainer;
	CRuArrayList<ChangeHandlerDesc *>		m_settingsChangedHandlers;

public:
											CRuEngine_Settings();
	virtual									~CRuEngine_Settings();

	BOOL									GetSetting(const char *settingName, RuPropertyType &settingOut);
	BOOL									SetSetting(const char *settingName, const RuPropertyType &setting);

	BOOL									RegisterSettingsChangeHandler(const CRuGUID &guid, SettingsChangedHandler handler);
	BOOL									UnregisterSettingsChangeHandler(const CRuGUID &guid);
};

// ************************************************************************************************************************************************************

// Globals
extern RUAPI CRuEngine_Settings*			g_ruEngineSettings;

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
