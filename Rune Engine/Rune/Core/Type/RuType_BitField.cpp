#include "../../Core/Type/RuType_BitField.h"

// Memory manager
#include "../../Core/Memory/RuMemoryManager.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

CRuBitField::CRuBitField()
:	m_numBits(0),
	m_bitFieldSize(0),
	m_bitFieldArraySize(0),
	m_bitFieldArray(NULL)
{
}

CRuBitField::CRuBitField(INT32 numBits)
:	m_numBits(0),
	m_bitFieldSize(0),
	m_bitFieldArraySize(0),
	m_bitFieldArray(NULL)
{
	SetNumBits(numBits);
}

CRuBitField::~CRuBitField()
{
	delete[] m_bitFieldArray;
}

INT32 CRuBitField::GetNumBits() const
{
	return m_numBits;
}

void CRuBitField::SetNumBits(INT32 numBits)
{
	if(numBits > m_bitFieldSize)
	{
		// Allocate new bitfield
		INT32 newBitFieldSizeDWORDs = (numBits >> 5) + (numBits & 0x0000001F ? 1 : 0);
		UINT32 *newBitFieldArray = ruNEW UINT32 [newBitFieldSizeDWORDs];

		// Clear memory
		memset(newBitFieldArray, 0, newBitFieldSizeDWORDs * sizeof(UINT32));

		if(m_bitFieldArray)
		{
			// Copy contents of old bitfield
			memcpy(newBitFieldArray, m_bitFieldArray, m_bitFieldArraySize * sizeof(UINT32));

			// Delete old bitfield
			delete[] m_bitFieldArray;
		}

		// Store new references
		m_numBits = numBits;
		m_bitFieldSize = newBitFieldSizeDWORDs << 5;
		m_bitFieldArraySize = newBitFieldSizeDWORDs;
		m_bitFieldArray = newBitFieldArray;
	}
	else
	{
		m_numBits = numBits;
	}
}

INT32 CRuBitField::GetBitFieldSize()
{
	return m_bitFieldSize;
}

UINT32 *CRuBitField::GetBitFieldArray()
{
	return m_bitFieldArray;
}

INT32 CRuBitField::GetBitFieldArraySize()
{
	return m_bitFieldSize >> 5;
}

CRuBitField &CRuBitField::operator=(CRuBitField &srcBitField)
{
	delete[] m_bitFieldArray;

	m_numBits = srcBitField.m_numBits;
	m_bitFieldSize = srcBitField.m_bitFieldSize;
	m_bitFieldArraySize = srcBitField.m_bitFieldArraySize;
	m_bitFieldArray = ruNEW UINT32 [m_bitFieldArraySize];
	memcpy(m_bitFieldArray, srcBitField.m_bitFieldArray, m_bitFieldArraySize * sizeof(UINT32));

	return *this;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
