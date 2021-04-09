#ifndef _RUUTILITY_H_
#define _RUUTILITY_H_

// Standard C Library Includes
#include <memory.h>

// Base Includes
#include "../../Core/Type/RuType_Base.h"
#include "../../Core/Exception/RuExceptions.h"
#include "../../Core/Memory/RuMemoryManager.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

// Forward declarations
class CRuStringTokenizer;
class CRuGUID;
class CRuGUIDEx;

// ************************************************************************************************************************************************************

inline INT32 RoundUpToPow2(INT32 x)
{
	INT32 output = 0x00000001;
	INT32 nonZeroBits = 0;

	while(x != 0)
	{
		if(x & 0x00000001)
			nonZeroBits++;

		x = x >> 1;
		output = output << 1;
	}
	output = output >> 1;

	if(nonZeroBits > 1)
		output = output << 1;

	return output;
}

// ************************************************************************************************************************************************************

inline UINT64 RuCastFloatToINT64(float sourceFloat)
{
	return *((UINT64 *) (&sourceFloat));
}

// ************************************************************************************************************************************************************

inline float RuCastINT64ToFloat(UINT64 sourceDWORD)
{
	return *((float *) (&sourceDWORD));
}

// ************************************************************************************************************************************************************

inline char *RuUserEnvironment_GetDocumentDirectory()
{
	char path[4096];
	if(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path) == 0)
	{
		char *outputPath = ruNEW char [strlen(path) + 1];
		memcpy(outputPath, path, strlen(path) + 1);
		return outputPath;
	}

	return NULL;
}

// ************************************************************************************************************************************************************

class CRuPerformanceTimer
{
private:
	INT32				m_maxSamples;
	INT32				m_curSampleIndex;
	DWORD*				m_samples;

	LARGE_INTEGER		m_qpcFrequency;

	LARGE_INTEGER		m_lastTimeStamp;

	BOOL				m_useQPC;
	BOOL				m_timerInitialized;

public:
						CRuPerformanceTimer();
						~CRuPerformanceTimer();

	void				Release();

	void				Initialize(INT32 maxSamples);
	void				ReinitializeQPC();

	DWORD				ElapsedTime(BOOL addToSample = FALSE);
	DWORD				GetTime();

	LARGE_INTEGER		GetQPCFrequency();
	LARGE_INTEGER		QueryPerformanceCounter();

	void				AddTimeSample(DWORD newSample);
	float				GetAverageTimeSample();
};

// ************************************************************************************************************************************************************

class CRuString
{
private:
	char*									m_string;
	INT32									m_stringLength;
	DWORD									m_hashCode;

public:
											CRuString();
											CRuString(const char *str);
											CRuString(const char *str, INT32 startIndex, INT32 endIndex);
											CRuString(const CRuString &byteString);
											~CRuString();

	DWORD									GetHashCode() const;
	INT32									GetLength() const;

	// Searching
	INT32									IndexOf(const char *strValue) const;
	INT32									LastIndexOf(const char *strValue) const;

	// String manipulation
	CRuString&								Append(const char *strValue);
	CRuString&								InsertAt(INT32 index, const char *strValue);
	CRuString&								Replace(const char *oldValue, const char *newValue);
	CRuString&								TruncateAt(INT32 index);
	CRuString&								TruncateAfter(INT32 index);
	CRuString&								TruncateBefore(INT32 index);
	CRuString&								Trim();

	CRuString&								ToLower();
	CRuString&								ToUpper();

	// Tokenization
	CRuString**								Tokenize(char *separators) const;

	// Casting operators
											operator char*() const;

	// Assignment
	CRuString&								operator=(const char *str);
	CRuString&								operator=(const CRuString &byteString);

	// Equality test
	BOOL									operator==(const char *str) const;
	BOOL									operator==(const CRuString &byteString) const;
	BOOL									operator!=(const char *str) const;
	BOOL									operator!=(const CRuString &byteString) const;
	BOOL									operator<(const CRuString &byteString) const;
	BOOL									operator>(const CRuString &byteString) const;

	friend									CRuStringTokenizer;

#ifdef KALYDO
	// to put rustrings in hash_maps
	operator size_t() const { return static_cast<size_t>(GetHashCode()); }
#endif //KALYDO
};

// ************************************************************************************************************************************************************

class CRuStringTokenizer
{
private:
	// Tokenizer settings
//	CRuString								m_separators;				// String of separators - e.g. space, comma
//	CRuString								m_tagSpecifiers;			// String of tag specifiers - e.g. <xxx>, always specified as pairs: "<>[]()"

	// Tokenized strings
	INT32									m_numTokens;
	INT32									m_tokenArraySize;
	CRuString**								m_tokens;

public:
											CRuStringTokenizer();
											~CRuStringTokenizer();

	void									Tokenize(const CRuString &ruString, const CRuString &separators);

	// Token access
	INT32									GetNumTokens() const;
	CRuString&								GetToken(INT32 index);
	INT32									GetTokenAsInteger(INT32 index);

private:
	void									Reset();
};

// ************************************************************************************************************************************************************

// Mersenne Twister Pseudo-random Number Generator
class CRuMTRandom
{
private:
	// Constants
	const static int		N;
	const static int		M;
	const static DWORD		MATRIX_A;
	const static DWORD		UMASK;
	const static DWORD		LMASK;

	BOOL					m_initialized;								// Is the generator initialized?
	DWORD*					m_state;									// Array for the state vector
	int						m_left;										// How many numbers remain in the seed array
	DWORD*					m_next;										// Pointer to the next seed number

public:
							CRuMTRandom(DWORD seed = 0);
							~CRuMTRandom();

	// Seed setting
	void					SetSeed(DWORD seed);

	// Random number generation functions
	UINT32					GenerateUINT32();
	INT32					GenerateINT32();
	float					GenerateFloat();

private:
	void					NextState();
	__forceinline DWORD		MixBits(DWORD u, DWORD v) { return (u & CRuMTRandom::UMASK) | (v & CRuMTRandom::LMASK); }
	__forceinline DWORD		Twist(DWORD u, DWORD v) { return (MixBits(u, v) >> 1) ^ ((v) & 1UL ? CRuMTRandom::MATRIX_A : 0UL); }
};


// Radix sort
class CRuRadixSort
{
private:
	// Input-independent data
	UINT32*					m_histogram;						// Histogram of values
	UINT32*					m_offsets;							// Array offsets for each radix
	
	// Indices data
	UINT32					m_currentIndicesSize;				// Current size of the indices list
	UINT32*					m_indicesA;							// Indices list A, swapped with list B on each pass
	UINT32*					m_indicesB;							// Indices list B, swapped with list A on each pass

public:
							CRuRadixSort();
							~CRuRadixSort();

	CRuRadixSort&			Sort(const INT32 *values, UINT32 numValues, BOOL preserveLastSortResults = FALSE);
	CRuRadixSort&			Sort(const UINT32 *values, UINT32 numValues, BOOL signedValues = FALSE, BOOL preserveLastSortResults = FALSE);
	CRuRadixSort&			Sort(const float *floatValues, UINT32 numValues, BOOL preserveLastSortResults = FALSE);
	CRuRadixSort&			Sort(const INT64 *values, UINT32 numValues, BOOL preserveLastSortResults = FALSE);
	CRuRadixSort&			Sort(const UINT64 *values,UINT32 numValues, BOOL signedValues = FALSE, BOOL preserveLastSortResults = FALSE);
	UINT32*					GetSortedIndices();
	void					SetWorkspaceSize(UINT32 maxElements);

private:
	BOOL					Resize(UINT32 newSize);
	void					ResetIndices();
	BOOL					CreateHistogram(const INT32 *values, UINT32 numValues);
	BOOL					CreateHistogram(const UINT32 *values, UINT32 numValues);
	BOOL					CreateHistogram(const float *floatValues, UINT32 numValues);
	BOOL					CreateHistogram(const INT64 *values, UINT32 numValues);
	BOOL					CreateHistogram(const UINT64 *values, UINT32 numValues);
};

// ************************************************************************************************************************************************************

class CRuInsertionSort
{
private:
	// Indices data
	INT32					m_currentIndicesSize;				// Current size of the indices list
	UINT32*					m_indices;							// Indices list

public:
							CRuInsertionSort();
							~CRuInsertionSort();

	CRuInsertionSort&		Sort(const INT32 *values, INT32 numValues);
	CRuInsertionSort&		Sort(const UINT32 *values, INT32 numValues);
	CRuInsertionSort&		Sort(const float *floatValues, INT32 numValues);
	UINT32*					GetSortedIndices();
	void					SetWorkspaceSize(INT32 maxElements);

private:
	BOOL					Resize(INT32 newSize);
	void					ResetIndices();
};

// ************************************************************************************************************************************************************

// Power-of-2 Hash table hashing function
#define CRuHashGenerator_MIX(a, b, c)	\
{									\
	a -= b; a -= c; a ^= (c >> 13);	\
	b -= c; b -= a; b ^= (a << 8);	\
	c -= a; c -= b; c ^= (b >> 13);	\
	a -= b; a -= c; a ^= (c >> 12);	\
	b -= c; b -= a; b ^= (a << 16);	\
	c -= a; c -= b; c ^= (b >> 5);	\
	a -= b; a -= c; a ^= (c >> 3);	\
	b -= c; b -= a; b ^= (a << 10);	\
	c -= a; c -= b; c ^= (b >> 15);	\
}

/*!
	Implements a hash generator using a hashing function described by Bob Jenkins.
	See documentation on GetHashCode for implementation details.

	Thread safe.

	@version 2004/08/01
*/
class CRuHashGenerator
{
private:
	DWORD				m_initVal;
	
public:
						CRuHashGenerator() : m_initVal(0) { }
						CRuHashGenerator(DWORD initVal) : m_initVal(initVal) { }

	// NOTE: These functions MUST return identical values for the same input!
	DWORD				GetHashCode(const BYTE *k, DWORD length);
	inline DWORD		GetHashCode(DWORD k);
	inline DWORD		GetHashCode(INT32 k);
	inline DWORD		GetHashCode(UINT32 k);
	inline DWORD		GetHashCode(INT64 k);
	inline DWORD		GetHashCode(UINT64 k);
	inline DWORD		GetHashCode(POINTER k);
//	inline DWORD		GetHashCode(const CRuGUID &ruGUID) { return GetHashCode(ruGUID.Data(), ruGUID.Size()); }
};

// ************************************************************************************************************************************************************

// This thing stitches together arrays into a big 2D grid to allow simple XY coordinate access
template <class ElementType>
class CRuVirtualGrid2D
{
private:
	UINT32						m_xChunks;					// Number of chunks on the x axis
	UINT32						m_yChunks;					// Number of chunks on the y axis
	UINT32						m_chunkWidth;				// Width of each chunk
	UINT32						m_chunkHeight;				// Height of each chunk
	INT32						m_xOffset;					// Offset to the X coordinate
	INT32						m_yOffset;					// Offset to the X coordinate
	BOOL						m_chunksOverlap;			// If TRUE, chunks in the grid has one overlapping row and column on the bottom and right edges
															// Note that chunk width and height do NOT include the extra padding row/column.

	ElementType**				m_chunks;					// Chunks

public:
								CRuVirtualGrid2D() : m_chunks(NULL) { }
								~CRuVirtualGrid2D() { delete[] m_chunks; }

	void						SetGridParameters(UINT32 chunkWidth, UINT32 chunkHeight, UINT32 xChunks, UINT32 yChunks, INT32 xOffset, INT32 yOffset, BOOL overlap);
	BOOL						SetChunk(UINT32 x, UINT32 y, ElementType *chunk);

	BOOL						IsValidGrid() const;
	const ElementType&			Get(INT32 x, INT32 y) const;

	void						Set(INT32 x, INT32 y, ElementType &newData);
};

// ************************************************************************************************************************************************************

// TODO: Convert all of the above to use an accessor function!
inline CRuHashGenerator &RuGlobalHashGen()
{
	static CRuHashGenerator hashGen(0x71C4812B);

	return hashGen;
};

// ************************************************************************************************************************************************************

/*!
	CRuGUID
	@author Ceranos
	@version Initial
*/
class CRuGUID
{
private:
	UINT32					m_data[4];												// 128-bit unique identifier
	DWORD					m_hashCode;

public:
	static CRuGUID			Invalid;

public:
	inline					CRuGUID() { Generate(); }

	inline					CRuGUID(UINT32 long0, UINT32 long1, UINT32 long2, UINT32 long3)
							{
								Set(long0, long1, long2, long3);
							}

	inline					CRuGUID(const CRuGUID &ruGUID)
							{
								memcpy(m_data, ruGUID.m_data, 16);
								m_hashCode = ruGUID.m_hashCode;
							}

	// Generation
	inline CRuGUID&			Generate()
							{
								CoCreateGuid(reinterpret_cast<GUID *>(m_data));
								m_hashCode = RuGlobalHashGen().GetHashCode((BYTE *) m_data, 16);
								return *this;
							}

	// Property accessors
	inline const BYTE*		Data() const { return (BYTE *) m_data; }
	inline UINT32			Size() const { return 16; }
	inline DWORD			HashCode() const { return m_hashCode; }

	inline void				Set(UINT32 long0, UINT32 long1, UINT32 long2, UINT32 long3)
							{
								m_data[0] = long0;
								m_data[1] = long1;
								m_data[2] = long2;
								m_data[3] = long3;

								m_hashCode = RuGlobalHashGen().GetHashCode((BYTE *) m_data, 16);
							}

	// Conversion
	inline char*			ToString() const
							{
								char *outStr = ruNEW char [36];

								for(INT32 i = 0, charIdx = 0; i < 4; ++i)
								{
									for(INT32 j = 7; j >= 0; --j, ++charIdx)
									{
										INT32 subValue = ((m_data[i] >> (j * 4)) & 0xF);
										if(subValue <= 9)
										{
											outStr[charIdx] = subValue + 48;
										}
										else
										{
											outStr[charIdx] = subValue + 55;
										}
									}

									if(i < 3)
									{
										outStr[charIdx] = '-';
										++charIdx;
									}
								}

								outStr[35] = 0;

								return outStr;
							}

	inline char*			ToString(char *outStr) const
							{
								for(INT32 i = 0, charIdx = 0; i < 4; ++i)
								{
									for(INT32 j = 7; j >= 0; --j, ++charIdx)
									{
										INT32 subValue = ((m_data[i] >> (j * 4)) & 0xF);
										if(subValue <= 9)
										{
											outStr[charIdx] = subValue + 48;
										}
										else
										{
											outStr[charIdx] = subValue + 55;
										}
									}

									if(i < 3)
									{
										outStr[charIdx] = '-';
										++charIdx;
									}
								}

								outStr[35] = 0;

								return outStr;
							}

	// Assignment operator overload
	inline CRuGUID&			operator=(const CRuGUID &ruGUID)
							{
								memcpy(m_data, ruGUID.m_data, 16);
								m_hashCode = ruGUID.m_hashCode;
								return *this;
							}

	// Comparison operations and operator overloads
	inline BOOL				operator==(const CRuGUID &ruGUID) const { return (m_data[0] == ruGUID.m_data[0] && m_data[1] == ruGUID.m_data[1] && m_data[2] == ruGUID.m_data[2] && m_data[3] == ruGUID.m_data[3]); }
	inline BOOL				operator!=(const CRuGUID &ruGUID) const { return (m_data[0] != ruGUID.m_data[0] || m_data[1] != ruGUID.m_data[1] || m_data[2] != ruGUID.m_data[2] || m_data[3] != ruGUID.m_data[3]); }

	friend CRuGUIDEx;
};

// ************************************************************************************************************************************************************

/*!
	CRuGUIDEx
	@author Ceranos
	@version 2004-07-31
*/
class CRuGUIDEx
{
private:
	UINT32					m_data[5];												// 128-bit unique identifier + 32-bit user-specified identifier
	DWORD					m_hashCode;

public:
	inline					CRuGUIDEx(UINT32 identifier = 0) { Generate(identifier); }
	inline					CRuGUIDEx(const CRuGUIDEx &ruGUID)
							{
								memcpy(m_data, ruGUID.m_data, 20);
								m_hashCode = ruGUID.m_hashCode;
							}

	inline void				SetIdentifier(UINT32 identifier)
							{
								m_data[4] = identifier;
								m_hashCode = RuGlobalHashGen().GetHashCode((BYTE *) m_data, 20);
							}

	// Generation
	inline CRuGUIDEx&		Generate(UINT32 identifier)
							{
								CoCreateGuid(reinterpret_cast<GUID *>(m_data));
								m_data[4] = identifier;
								m_hashCode = RuGlobalHashGen().GetHashCode((BYTE *) m_data, 20);
								return *this;
							}

	// Property accessors
	inline const BYTE*		Data() const { return (BYTE *) m_data; }
	inline UINT32			Size() const { return 20; }
	inline DWORD			HashCode() const { return m_hashCode; }

	// Assignment operator overload
	inline CRuGUIDEx&		operator=(const CRuGUID &ruGUID) { memcpy(m_data, ruGUID.m_data, 16); m_data[4] = 0x00000000; return *this; }
	inline CRuGUIDEx&		operator=(const CRuGUIDEx &ruGUID) { memcpy(m_data, ruGUID.m_data, 20); return *this; }

	// Comparison operations and operator overloads
	inline BOOL				operator==(const CRuGUIDEx &ruGUID) const { return (m_data[0] == ruGUID.m_data[0] && m_data[1] == ruGUID.m_data[1] && m_data[2] == ruGUID.m_data[2] && m_data[3] == ruGUID.m_data[3] && m_data[4] == ruGUID.m_data[4]); }
	inline BOOL				operator!=(const CRuGUIDEx &ruGUID) const { return (m_data[0] != ruGUID.m_data[0] || m_data[1] != ruGUID.m_data[1] || m_data[2] != ruGUID.m_data[2] || m_data[3] != ruGUID.m_data[3] || m_data[4] != ruGUID.m_data[4]); }
};

// ************************************************************************************************************************************************************

// Globals
inline CRuMTRandom &RuGlobalMTRand()
{
	static CRuMTRandom mtRand(0x71C4812B);

	return mtRand;
};

inline CRuRadixSort &RuGlobalRadixSort()
{
	static CRuRadixSort radixSort;

	return radixSort;
};

inline CRuInsertionSort &RuGlobalInsertionSort()
{
	static CRuInsertionSort insertionSort;

	return insertionSort;
};

// ************************************************************************************************************************************************************

inline DWORD CRuString::GetHashCode() const
{
	return m_hashCode;
}

inline INT32 CRuString::GetLength() const
{
	return m_stringLength;
}

inline CRuString::operator char*() const
{
	if(m_string)
	{
		return (char *) m_string;
	}

	return "";
}

// ************************************************************************************************************************************************************

inline DWORD CRuHashGenerator::GetHashCode(DWORD k)
{
	DWORD a, b, c;

	// Set up the internal state
	a = b = 0x9e3779b9;		// The golden ratio; an arbitrary value
	c = m_initVal + 4;		// The previous hash value

	// Mix!
	a += k;
	CRuHashGenerator_MIX(a, b, c);

	// Return the result
	return c;
}

inline DWORD CRuHashGenerator::GetHashCode(INT32 k)
{
	DWORD a, b, c;

	// Set up the internal state
	a = b = 0x9e3779b9;		// The golden ratio; an arbitrary value
	c = m_initVal + 4;		// The previous hash value

	// Mix!
	a += ((UINT32) k);
	CRuHashGenerator_MIX(a, b, c);

	// Return the result
	return c;
}

inline DWORD CRuHashGenerator::GetHashCode(UINT32 k)
{
	DWORD a, b, c;

	// Set up the internal state
	a = b = 0x9e3779b9;		// The golden ratio; an arbitrary value
	c = m_initVal + 4;		// The previous hash value

	// Mix!
	a += k;
	CRuHashGenerator_MIX(a, b, c);

	// Return the result
	return c;
}

inline DWORD CRuHashGenerator::GetHashCode(INT64 k)
{
	return this->GetHashCode(reinterpret_cast<BYTE *>(&k), sizeof(INT64));
}

inline DWORD CRuHashGenerator::GetHashCode(UINT64 k)
{
	return this->GetHashCode(reinterpret_cast<BYTE *>(&k), sizeof(UINT64));
}

inline DWORD CRuHashGenerator::GetHashCode(POINTER k)
{
	return this->GetHashCode(reinterpret_cast<BYTE *>(&k), sizeof(POINTER));
}

// ************************************************************************************************************************************************************

template <class ElementType>
void CRuVirtualGrid2D<ElementType>::SetGridParameters(UINT32 chunkWidth, UINT32 chunkHeight, UINT32 xChunks, UINT32 yChunks, INT32 xOffset, INT32 yOffset, BOOL overlap)
{
	delete[] m_chunks;

	m_xChunks = xChunks;
	m_yChunks = yChunks;
	m_chunkWidth = chunkWidth;
	m_chunkHeight = chunkHeight;
	m_xOffset = xOffset;
	m_yOffset = yOffset;
	m_chunksOverlap = overlap;

	m_chunks = ruNEW ElementType * [m_xChunks * m_yChunks];
}

template <class ElementType>
BOOL CRuVirtualGrid2D<ElementType>::SetChunk(UINT32 x, UINT32 y, ElementType *chunk)
{
	if(x >= m_xChunks || y >= m_yChunks)
		return FALSE;

	m_chunks[x + y * m_xChunks] = chunk;

	return TRUE;
}

template <class ElementType>
BOOL CRuVirtualGrid2D<ElementType>::IsValidGrid() const
{
	UINT32 totalChunks = m_xChunks * m_yChunks, i;

	for(i = 0; i < totalChunks; i++)
	{
		if(m_chunks[i] == NULL)
		{
			return FALSE;
		}
	}

	return TRUE;
}

template <class ElementType>
const ElementType &CRuVirtualGrid2D<ElementType>::Get(INT32 x, INT32 y) const
{
	if(m_chunks)
	{
		// Offset (x, y) coordinates
		x = x + m_xOffset;
		y = y + m_yOffset;

		// Calculate actual element coordinates
		INT32 chunkX = x / m_chunkWidth, chunkY = y / m_chunkHeight;
		INT32 relativeX = x % m_chunkWidth, relativeY = y % m_chunkHeight;

		if(m_chunksOverlap)
		{
			return m_chunks[chunkX + chunkY * m_xChunks][relativeX + relativeY * (m_chunkWidth + 1)];
		}
		else
		{
			return m_chunks[chunkX + chunkY * m_xChunks][relativeX + relativeY * m_chunkWidth];
		}
	}

	throw CRuArgumentOutOfRangeException();
}

template <class ElementType>
void CRuVirtualGrid2D<ElementType>::Set(INT32 x, INT32 y, ElementType &newData)
{
	if(m_chunks)
	{
		// Offset (x, y) coordinates
		x = x + m_xOffset;
		y = y + m_yOffset;

		// Calculate actual element coordinates
		INT32 chunkX = x / m_chunkWidth, chunkY = y / m_chunkHeight;
		INT32 relativeX = x % m_chunkWidth, relativeY = y % m_chunkHeight;

		if(m_chunksOverlap)
		{
			if(relativeX == 0 && relativeY == 0)
			{
				// Element to be changed is in upper-left corner - three potential overlapping elements
				UINT32 elementOffset = (m_chunkWidth + 1) * (m_chunkHeight + 1) - 1;
				if(chunkX > 0 && chunkY > 0)
				{
					m_chunks[chunkX - 1 + (chunkY - 1) * m_xChunks][elementOffset] = newData;
				}
				if(chunkX > 0)
				{
					m_chunks[chunkX - 1 + chunkY * m_xChunks][m_chunkWidth + relativeY * (m_chunkWidth + 1)] = newData;
				}
				if(chunkY > 0)
				{
					m_chunks[chunkX + (chunkY - 1) * m_xChunks][relativeX + m_chunkHeight * (m_chunkWidth + 1)] = newData;
				}
			}
			else if(relativeX == 0)
			{
				// Element to be changed is along the left edge, one potential overlap
				if(chunkX > 0)
				{
					m_chunks[chunkX - 1 + chunkY * m_xChunks][m_chunkWidth + relativeY * (m_chunkWidth + 1)] = newData;
				}
			}
			else if(relativeY == 0)
			{
				// Element to be changed is along the top edge, one potential overlap
				if(chunkY > 0)
				{
					m_chunks[chunkX + (chunkY - 1) * m_xChunks][relativeX + m_chunkHeight * (m_chunkWidth + 1)] = newData;
				}
			}

			// Alter the final element
			m_chunks[chunkX + chunkY * m_xChunks][relativeX + relativeY * (m_chunkWidth + 1)] = newData;
		}
		else
		{
			m_chunks[chunkX + chunkY * m_xChunks][relativeX + relativeY * m_chunkWidth] = newData;
		}

		return;
	}

	throw CRuArgumentOutOfRangeException();
}

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
