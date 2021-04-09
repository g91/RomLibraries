#include "../../Core/Algorithm/RuAlgorithm_PatternMatch_FJS.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuAlgorithm_PatternMatch_FJS, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuAlgorithm_PatternMatch_FJS, "CRuAlgorithm_PatternMatch_FJS", "IRuObject")
ruCLASSGUID_DEFINE(CRuAlgorithm_PatternMatch_FJS, 0xDFDBB750, 0x23B34fdb, 0xB27CD4A5, 0xAED5C694)

// ************************************************************************************************************************************************************

CRuAlgorithm_PatternMatch_FJS::CRuAlgorithm_PatternMatch_FJS()
:	m_alphabetSize(256),
	m_patternLength(0),
	m_pattern(NULL),
	m_beta(NULL),
	m_delta(NULL)
{
	m_delta = ruNEW INT32 [m_alphabetSize];
}

CRuAlgorithm_PatternMatch_FJS::~CRuAlgorithm_PatternMatch_FJS()
{
	delete[] m_pattern;
	delete[] m_beta;
	delete[] m_delta;
}

BOOL CRuAlgorithm_PatternMatch_FJS::SetPattern(const BYTE *pattern, INT32 patternLength)
{
	if(patternLength < 1)
		return FALSE;

	// Re-initialize pattern array
	delete[] m_pattern;
	delete[] m_beta;

	m_patternLength = patternLength;
	m_pattern = ruNEW BYTE [m_patternLength];
	memcpy(m_pattern, pattern, m_patternLength);
	m_beta = ruNEW INT32 [m_patternLength + 1];

	// Pre-compute
	MakeBeta(pattern, patternLength);
	MakeDelta(pattern, patternLength);

	return TRUE;
}

const BYTE* CRuAlgorithm_PatternMatch_FJS::GetPattern()
{
	return m_pattern;
}

INT32 CRuAlgorithm_PatternMatch_FJS::GetPatternLength()
{
	return m_patternLength;
}

void CRuAlgorithm_PatternMatch_FJS::Match(const BYTE *text, INT32 textLength)
{
	INT32 i = 0, j = 0, mp = m_patternLength - 1, ip = mp;

	m_matchPoints.Clear();

	while(ip < textLength)
	{
		if(j <= 0)
		{
			while(m_pattern[mp] != text[ip])
			{
				ip += m_delta[text[ip + 1]];
				if(ip >= textLength)
				{
					return;
				}
			}

			j = 0;
			i = ip - mp;

			while((j < mp) && (text[i] == m_pattern[j]))
			{
				++i;
				++j;
			}

			if(j == mp)
			{
				m_matchPoints.Add(i - mp);
				++i;
				++j;
			}

			if(j <= 0)
			{
				++i;
			}
			else
			{
				j = m_beta[j];
			}
		}
		else
		{
			while((j < m_patternLength) && (text[i] == m_pattern[j]))
			{
				++i;
				++j;
			}

			if(j == m_patternLength)
			{
				m_matchPoints.Add(i - m_patternLength);
			}

			j = m_beta[j];
		}

		ip = i + mp - j;
	}
}

INT32 CRuAlgorithm_PatternMatch_FJS::GetNumMatches()
{
	return m_matchPoints.Count();
}

INT32 CRuAlgorithm_PatternMatch_FJS::GetMatchPosition(INT32 matchIndex)
{
	return m_matchPoints[matchIndex];
}

void CRuAlgorithm_PatternMatch_FJS::MakeBeta(const BYTE *pattern, INT32 patternLength)
{
	INT32 i = 0, j = m_beta[0] = -1;

	while(i < patternLength)
	{
		while((j > -1) && (pattern[i] != pattern[j]))
		{
			j = m_beta[j];
		}

		if(pattern[++i] == pattern[++j])
		{
			m_beta[i] = m_beta[j];
		}
		else
		{
			m_beta[i] = j;
		}
	}
}

void CRuAlgorithm_PatternMatch_FJS::MakeDelta(const BYTE *pattern, INT32 patternLength)
{
	INT32 i;

	for(i = 0; i < m_alphabetSize; ++i)
	{
		m_delta[i] = patternLength + 1;
	}

	for(i = 0; i < patternLength; ++i)
	{
		m_delta[pattern[i]] = patternLength - i;
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
