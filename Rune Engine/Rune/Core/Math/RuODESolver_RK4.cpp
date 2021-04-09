#include "../../Core/Math/RuODESolver_RK4.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuODESolver_RK4::CRuODESolver_RK4()
:	m_numValues(0),
	m_numDiffEqs(0),
	m_vars(NULL),
	m_inp(NULL),
	m_k1(NULL),
	m_k2(NULL),
	m_k3(NULL),
	m_k4(NULL)
{
}

CRuODESolver_RK4::~CRuODESolver_RK4()
{
	delete[] m_vars;
	delete[] m_inp;
	delete[] m_k1;
	delete[] m_k2;
	delete[] m_k3;
	delete[] m_k4;
}

void CRuODESolver_RK4::SetNumValues(INT32 numValues)
{
	m_numValues = numValues;

	delete[] m_vars;
	delete[] m_inp;
	delete[] m_k1;
	delete[] m_k2;
	delete[] m_k3;
	delete[] m_k4;

	m_vars = ruNEW REAL [m_numValues];
	m_inp = ruNEW REAL [m_numValues];
	m_k1 = ruNEW REAL [m_numValues];
	m_k2 = ruNEW REAL [m_numValues];
	m_k3 = ruNEW REAL [m_numValues];
	m_k4 = ruNEW REAL [m_numValues];

	memset(m_vars, 0, sizeof(REAL) * m_numValues);
	memset(m_inp, 0, sizeof(REAL) * m_numValues);
	memset(m_k1, 0, sizeof(REAL) * m_numValues);
	memset(m_k2, 0, sizeof(REAL) * m_numValues);
	memset(m_k3, 0, sizeof(REAL) * m_numValues);
	memset(m_k4, 0, sizeof(REAL) * m_numValues);
}

void CRuODESolver_RK4::SetNumDiffEqs(INT32 numDiffEqs)
{
	m_numDiffEqs = numDiffEqs;
}

void CRuODESolver_RK4::SetDiffEq(INT32 index, RuODESolver_DiffEqFunctor diffEq)
{
	m_diffEqs[index] = diffEq;
}

REAL CRuODESolver_RK4::GetValue(INT32 index)
{
	return m_vars[index];
}

void CRuODESolver_RK4::SetValue(INT32 index, REAL value)
{
	m_vars[index] = value;
}

void CRuODESolver_RK4::SetValues(REAL *values)
{
	memcpy(m_vars, values, sizeof(REAL) * m_numValues);
}

void CRuODESolver_RK4::Solve(REAL t, REAL h)
{
	// Evaluate at time t
	for(INT32 i = 0; i < m_numValues; ++i)
	{
		m_k1[i] = m_diffEqs[i](t, m_vars);
	}

	// Setup input to differential equations
	for(INT32 i = 0; i < m_numValues; ++i)
	{
		m_inp[i] = m_vars[i] + m_k1[i] * h / 2.0f;
	}

	// Evaluate at time t + h / 2
	for(INT32 i = 0; i < m_numValues; ++i)
	{
		m_k2[i] = m_diffEqs[i](t + h / 2.0f, m_inp);
	}

	// Setup input to differential equations
	for(INT32 i = 0; i < m_numValues; ++i)
	{
		m_inp[i] = m_vars[i] + m_k2[i] * h / 2.0f;
	}

	// Evaluate at time t + h / 2
	for(INT32 i = 0; i < m_numValues; ++i)
	{
		m_k3[i] = m_diffEqs[i](t + h / 2.0f, m_inp);
	}

	// Setup input to differential equations
	for(INT32 i = 0; i < m_numValues; ++i)
	{
		m_inp[i] = m_vars[i] + m_k3[i] * h;
	}

	// Evaluate at time t + h
	for(INT32 i = 0; i < m_numValues; ++i)
	{
		m_k4[i] = m_diffEqs[i](t + h, m_inp);
	}

	// Final combination
	for(INT32 i = 0; i < m_numValues; ++i)
	{
		m_vars[i] = m_vars[i] + (m_k1[i] + 2.0f * m_k2[i] + 2.0f * m_k3[i] + m_k4[i]) * h / 6.0f;
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
