/*!
	@file RuUtility_PropertyContainer.h

	Copyright (c) 2004-2006 All rights reserved
*/

#ifndef _RUUTILITY_PROPERTYCONTAINER_H_
#define _RUUTILITY_PROPERTYCONTAINER_H_

// Base Includes
#include "../../Core/Type/RuType_Object.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	A collection class for properties.

	@version 2006/12/10
*/
class CRuPropertyContainer : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuStringHash<RuPropertyType>*			m_propertyHash;

public:
											CRuPropertyContainer();
	virtual									~CRuPropertyContainer();

	BOOL									GetProperty(const char *propertyName, RuPropertyType &propertyValueOut);
	BOOL									SetProperty(const char *propertyName, const RuPropertyType &propertyValue);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
