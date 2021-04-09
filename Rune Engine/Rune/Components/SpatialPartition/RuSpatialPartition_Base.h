#ifndef _RUSPATIALPARTITION_BASE_H_
#define _RUSPATIALPARTITION_BASE_H_

// Rune system core
#include "../../Core/Type/RuType_Object.h"
#include "../../Core/Math/RuMathVector.h"
#include "../../Core/Math/RuMathGeometry.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class IRuSpatialPartition : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual								~IRuSpatialPartition();

	BOOL								InsertObject(const CRuSphere &bounds, IRuObject *obj);
	BOOL								DeleteObject(const CRuSphere &bounds, IRuObject *obj);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
