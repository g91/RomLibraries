#ifndef _RUALGORITHM_PATTERNMATCH_FJS_
#define _RUALGORITHM_PATTERNMATCH_FJS_

// Base types
#include "../../Core/Type/RuType_Object.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuAlgorithm_PatternMatch_FJS : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	INT32							m_alphabetSize;

	INT32							m_patternLength;
	BYTE*							m_pattern;

	INT32*							m_beta;
	INT32*							m_delta;

	CRuArrayList<INT32>				m_matchPoints;

public:
									CRuAlgorithm_PatternMatch_FJS();
	virtual							~CRuAlgorithm_PatternMatch_FJS();

	BOOL							SetPattern(const BYTE *pattern, INT32 patternLength);
	const BYTE*						GetPattern();
	INT32							GetPatternLength();
	
	void							Match(const BYTE *text, INT32 textLength);
	INT32							GetNumMatches();
	INT32							GetMatchPosition(INT32 matchIndex);

protected:
	void							MakeBeta(const BYTE *pattern, INT32 patternLength);
	void							MakeDelta(const BYTE *pattern, INT32 patternLength);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
