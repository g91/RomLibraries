#ifndef _RUODESOLVER_RK4_H_
#define _RUODESOLVER_RK4_H_

#include <math.h>

#include "../../Core/Type/RuType_Base.h"
#include "../../Core/Type/RuType_Object.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

typedef boost::function<REAL (REAL t, REAL *x)> RuODESolver_DiffEqFunctor;		// Parameters: (time, array of variables)

// ************************************************************************************************************************************************************

class CRuODESolver_RK4
{
protected:
	INT32									m_numValues;
	INT32									m_numDiffEqs;
	RuODESolver_DiffEqFunctor				m_diffEqs[16];

	// Runtime
	REAL*									m_vars;
	REAL*									m_inp;
	REAL*									m_k1;
	REAL*									m_k2;
	REAL*									m_k3;
	REAL*									m_k4;

public:
											CRuODESolver_RK4();
											~CRuODESolver_RK4();

	void									SetNumValues(INT32 numValues);
	void									SetNumDiffEqs(INT32 numDiffEqs);
	void									SetDiffEq(INT32 index, RuODESolver_DiffEqFunctor diffEq);

	REAL									GetValue(INT32 index);
	void									SetValue(INT32 index, REAL value);
	void									SetValues(REAL *values);

	void									Solve(REAL t, REAL h);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
