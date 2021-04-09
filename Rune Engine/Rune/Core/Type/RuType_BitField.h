#pragma once

// Base types
#include "../../Core/Type/RuType_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuBitField
{
private:
	INT32							m_numBits;
	INT32							m_bitFieldSize;
	INT32							m_bitFieldArraySize;
	UINT32*							m_bitFieldArray;

public:
									CRuBitField();
									CRuBitField(INT32 numBits);
									~CRuBitField();

	INT32							GetNumBits() const;
	void							SetNumBits(INT32 numBits);

	INT32							GetBitFieldSize();
	UINT32*							GetBitFieldArray();
	INT32							GetBitFieldArraySize();

	void							ClearAll();
	void							Clear(INT32 index);
	void							Set(INT32 index);


	BOOL							operator[](INT32 index) const;

	CRuBitField&					operator=(CRuBitField &srcBitField);
};

// ************************************************************************************************************************************************************

inline void CRuBitField::ClearAll()
{
	memset(m_bitFieldArray, 0, (m_bitFieldSize >> 5) * sizeof(UINT32));
}

inline void CRuBitField::Clear(INT32 index)
{
	assert(index >= 0 && index < m_numBits);

	INT32 dwordIndex = index >> 5;
	UINT32 dwordMask = 1 << (index & 0x0000001F);

	m_bitFieldArray[dwordIndex] = m_bitFieldArray[dwordIndex] & ~dwordMask;
}

inline void CRuBitField::Set(INT32 index)
{
	assert(index >= 0 && index < m_numBits);

	INT32 dwordIndex = index >> 5;
	UINT32 dwordMask = 1 << (index & 0x0000001F);

	m_bitFieldArray[dwordIndex] = m_bitFieldArray[dwordIndex] | dwordMask;
}

inline BOOL CRuBitField::operator[](INT32 index) const
{
	assert(index >= 0 && index < m_numBits);

	INT32 dwordIndex = index >> 5;
	UINT32 dwordMask = 1 << (index & 0x0000001F);

	return m_bitFieldArray[dwordIndex] & dwordMask;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
