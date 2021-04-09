/*!
	@file RuGizmo_Base.h

	Copyright (c) 2004-2007 All rights reserved
*/

#ifndef _RUGIZMO_BASE_H_
#define _RUGIZMO_BASE_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

enum RuGizmoConstraint
{
	ruGIZMOCONSTRAINT_NONE					= 0,											//!< No constraints
	ruGIZMOCONSTRAINT_XY					= 1,											//!< Movement limited to X-Y plane
	ruGIZMOCONSTRAINT_XZ					= 2,											//!< Movement limited to X-Z plane
	ruGIZMOCONSTRAINT_YZ					= 3,											//!< Movement limited to Y-Z plane

	ruGIZMOCONSTRAINT_COUNT					= 4,											//!< Constraint count

	ruGIZMOCONSTRAINT_FORCE_DWORD			= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

/*!
	Base class for all gizmos. Gizmos are manipulator objects that act on CRuFrame entities to
	perform scaling, rotation, and translation. Once activated using the ActivateManipulator
	function, changes to the target CRuFrame entity are made by successive calls to the
	Manipulate function. All changes are made directly to the CRuFrame.

	Subclasses must implement the above two functions to provide proper manipulation capabilities.

	@version 2004.12.30
*/
class CRuGizmo_Base : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

public:
	virtual									~CRuGizmo_Base();

	// Gizmo queries
	virtual BOOL							SetConstraint(RuGizmoConstraint gizmoConstraint) = 0;
	virtual BOOL							HighlightManipulator(const CRuRay3 &pickRay) = 0;
	virtual BOOL							ActivateManipulator(const CRuRay3 &pickRay) = 0;
	virtual BOOL							Manipulate(const CRuRay3 &pickRay) = 0;
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
