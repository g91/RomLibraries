#ifndef _RUFUSION_UIELEMENT_H_
#define _RUFUSION_UIELEMENT_H_

// Base types
#include "../Renderer/RuRenderer.h"
#include "../Renderer/RendererNode/RuRendererNode_UI.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuFusion_UIElement : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
									CRuFusion_UIElement();
	virtual							~CRuFusion_UIElement();

	virtual void					Display() = 0;
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
