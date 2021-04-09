#include "../../Core/Utility/RuUtility.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

// *** Desc : Default constructor, initializes timer object
//     Ret  : N/A
CRuPerformanceTimer::CRuPerformanceTimer()
:	m_maxSamples(0),
	m_curSampleIndex(0),
	m_samples(0),
	m_useQPC(FALSE),
	m_timerInitialized(FALSE)
{
	Initialize(10);
}

// *** Desc : Destructor
//     Ret  : N/A
CRuPerformanceTimer::~CRuPerformanceTimer()
{
	Release();
}

// *** Desc : Timer de-initialization
//     Ret  : None
void CRuPerformanceTimer::Release()
{
	if(m_timerInitialized != false)
	{
		if(m_samples != NULL)
			delete[] m_samples;

		timeEndPeriod(1);

		m_samples = NULL;
		m_maxSamples = 0;
		m_curSampleIndex = 0;
		m_useQPC = FALSE;
		m_timerInitialized = FALSE;
	}
}

// *** Desc : Timer initialization, initializes the number of time samples to maxSamples
//     Ret  : None
void CRuPerformanceTimer::Initialize(INT32 maxSamples)
{
	Release();

	if(maxSamples <= 0)
		maxSamples = 1;

	m_maxSamples = maxSamples;
	m_curSampleIndex = 0;
	m_samples = ruNEW DWORD [maxSamples];
	memset(m_samples, 0, sizeof(DWORD) * maxSamples);
	
	// Determine whether to use Performance Counter
	m_timerInitialized = TRUE;
	if(QueryPerformanceFrequency(&m_qpcFrequency) == 0 || ((m_qpcFrequency.HighPart == 0) && (m_qpcFrequency.LowPart == 0)))
	{
		// If Performance Counter is unavailable, use the multimedia timer
		m_useQPC = FALSE;
	}
	else
	{
		m_useQPC = TRUE;
	}

	timeBeginPeriod(1);
}

void CRuPerformanceTimer::ReinitializeQPC()
{
	// Query the performance frequency
	QueryPerformanceFrequency(&m_qpcFrequency);

	if((m_qpcFrequency.HighPart == 0) && (m_qpcFrequency.LowPart == 0))
	{
		// If Performance Counter is unavailable, use the multimedia timer
		m_useQPC = FALSE;
	}
	else
	{
		m_useQPC = TRUE;
	}
}

// *** Desc : Returns the elapsed time, in milliseconds, since the last call to elapsedTime
//     Ret  : Elapsed time in milliseconds
DWORD CRuPerformanceTimer::ElapsedTime(BOOL AddToSample)
{
	LARGE_INTEGER curTime;
	DWORD timeLen = 0;

	// Performance Counter is available, so make use of it
	if(m_useQPC != false)
	{
		// ??? 64 bit integer arithmetic - Possible portability problem?
		::QueryPerformanceCounter(&curTime);
		timeLen = (DWORD) ((curTime.QuadPart - m_lastTimeStamp.QuadPart) / (m_qpcFrequency.QuadPart / (float) 1000));
		m_lastTimeStamp = curTime;
	}

	// Performance Counter is not available, so use the multimedia timer
	else
	{
		curTime.LowPart = timeGetTime();
		timeLen = curTime.LowPart - m_lastTimeStamp.LowPart;
		m_lastTimeStamp.HighPart = 0;
		m_lastTimeStamp.LowPart = curTime.LowPart;
	}

	// Add time sample to sample pool if applicable
	if(AddToSample != false)
		AddTimeSample(timeLen);

	return timeLen;
}

// *** Desc : Returns the system time, in milliseconds
//     Ret  : System time in milliseconds
DWORD CRuPerformanceTimer::GetTime()
{
	return timeGetTime();
}

LARGE_INTEGER CRuPerformanceTimer::GetQPCFrequency()
{
	return m_qpcFrequency;
}

LARGE_INTEGER CRuPerformanceTimer::QueryPerformanceCounter()
{
	LARGE_INTEGER queryResult;

	::QueryPerformanceCounter(&queryResult);

	return queryResult;
}

// *** Desc : Adds NewSample to the list of stored time samples
//     Ret  : None
void CRuPerformanceTimer::AddTimeSample(DWORD NewSample)
{
	m_samples[m_curSampleIndex] = NewSample;
	m_curSampleIndex++;

	if(m_curSampleIndex >= m_maxSamples)
		m_curSampleIndex = 0;
}

// *** Desc : Returns the current average of the list of stored time samples
//     Ret  : None
float CRuPerformanceTimer::GetAverageTimeSample()
{
	if(m_maxSamples > 0)
	{
		DWORD Total = 0;

		for(INT32 CurSample = 0; CurSample < m_maxSamples; CurSample++)
		{
			Total += m_samples[CurSample];
		}

		return (float) Total / m_maxSamples;
	}

	return 0.0;
}

// ************************************************************************************************************************************************************

/*!
	CRuString::CRuString()
	Standard constructor. 標準建構式.
*/
CRuString::CRuString()
:	m_string(NULL),
	m_stringLength(0),
	m_hashCode(0)
{
}

CRuString::CRuString(const char *str)
:	m_string(NULL),
	m_stringLength(0),
	m_hashCode(0)
{
	if(str)
	{
		m_stringLength = (INT32)strlen(str);
		m_string = ruNEW char [m_stringLength + 1];
		memcpy(m_string, str, m_stringLength + 1);
		m_hashCode = RuGlobalHashGen().GetHashCode((BYTE *) m_string, m_stringLength);
	}
	else
	{
		m_string = ruNEW char [1];
		m_string[0] = 0;
		m_stringLength = 0;
		m_hashCode = 0;
	}
}

CRuString::CRuString(const char *str, INT32 startIndex, INT32 endIndex)
:	m_string(NULL),
	m_stringLength(0),
	m_hashCode(0)
{
	if(str)
	{
		m_stringLength = endIndex - startIndex + 1;
		m_string = ruNEW char [m_stringLength + 1];
		memcpy(m_string, &str[startIndex], m_stringLength + 1);
		m_hashCode = RuGlobalHashGen().GetHashCode((BYTE *) m_string, m_stringLength);
		m_string[m_stringLength] = 0;
	}
}

CRuString::CRuString(const CRuString &byteString)
:	m_string(NULL),
	m_stringLength(0),
	m_hashCode(0)
{
	if(byteString.m_string)
	{
		m_stringLength = byteString.m_stringLength;
		m_string = ruNEW char [m_stringLength + 1];
		memcpy(m_string, byteString.m_string, m_stringLength + 1);
		m_hashCode = byteString.m_hashCode;
	}
}

/*!
	CRuString::~CRuString()
	Standard destructor. 標準解構式.
*/
CRuString::~CRuString()
{
	delete[] m_string;
}

INT32 CRuString::IndexOf(const char *strValue) const
{
	INT32 strValueLength = (INT32)strlen(strValue);

	// If the string to find is longer than this string, abort
	if(strValueLength > m_stringLength)
		return -1;

	// Iterate through the string, starting from the first character, to find the specified substring
	for(INT32 i = 0, checkLength = m_stringLength - strValueLength; i <= checkLength; ++i)
	{
		if(strncmp(&m_string[i], strValue, strValueLength) == 0)
		{
			return i;
		}
	}

	return -1;
}

INT32 CRuString::LastIndexOf(const char *strValue) const
{
	INT32 strValueLength = (INT32)strlen(strValue);

	// If the string to find is longer than this string, abort
	if(strValueLength > m_stringLength)
		return -1;

	// Iterate through the string, starting from the last character, to find the specified substring
	for(INT32 i = m_stringLength - strValueLength; i >= 0; --i)
	{
		if(strncmp(&m_string[i], strValue, strValueLength) == 0)
		{
			return i;
		}
	}

	return -1;
}

CRuString &CRuString::Append(const char *strValue)
{
	INT32 strValueLength = (INT32)strlen(strValue);

	if(m_stringLength > 0)
	{
		char *newString = ruNEW char [m_stringLength + strValueLength + 1];
		memcpy(newString, m_string, m_stringLength + 1);
		delete[] m_string;

		m_string = newString;
		m_stringLength = m_stringLength + strValueLength;

		strcat(m_string, strValue);
	}
	else
	{
		delete[] m_string;
		m_string = ruNEW char [strValueLength + 1];
		m_stringLength = strValueLength;

		strcpy(m_string, strValue);
	}

	// Generate hash code
	m_hashCode = RuGlobalHashGen().GetHashCode((BYTE *) m_string, m_stringLength);

	return *this;
}

CRuString &CRuString::InsertAt(INT32 index, const char *strValue)
{
	INT32 strValueLength = (INT32)strlen(strValue);

	if(strValueLength > 0 && index >= 0 && index <= m_stringLength)
	{
		INT32 newStringLength = m_stringLength + strValueLength;
		char *newString = ruNEW char [newStringLength + 1];

		// Copy section before the insertion point
		if(index > 0)
		{
			memcpy(newString, m_string, index);
		}

		// Copy inserted string value
		memcpy(&newString[index], strValue, strValueLength);

		// Copy remainder of string
		memcpy(&newString[index + strValueLength], &m_string[index], newStringLength - (index + strValueLength));

		newString[newStringLength] = 0;

		delete[] m_string;

		m_string = newString;
		m_stringLength = newStringLength;
	}

	// Generate hash code
	m_hashCode = RuGlobalHashGen().GetHashCode((BYTE *) m_string, m_stringLength);

	return *this;
}

CRuString &CRuString::Replace(const char *oldValue, const char *newValue)
{
	if(m_stringLength > 0)
	{
		INT32 oldValueIdx = IndexOf(oldValue);
		INT32 oldValueLength = (INT32)strlen(oldValue);
		INT32 newValueLength = (INT32)strlen(newValue);

		if(oldValueIdx >= 0)
		{
			INT32 newStringLength = m_stringLength - oldValueLength + newValueLength;

			char *newString = ruNEW char [newStringLength + 1];

			if(oldValueIdx > 0)
			{
				memcpy(newString, m_string, oldValueIdx);
			}

			memcpy(&newString[oldValueIdx], newValue, newValueLength);

			memcpy(&newString[oldValueIdx + newValueLength], &m_string[oldValueIdx + oldValueLength], newStringLength - (oldValueIdx + newValueLength));

			newString[newStringLength] = 0;

			delete[] m_string;

			m_string = newString;
			m_stringLength = newStringLength;
		}
	}

	// Generate hash code
	m_hashCode = RuGlobalHashGen().GetHashCode((BYTE *) m_string, m_stringLength);

	return *this;
}

// Truncates string starting at index
CRuString &CRuString::TruncateAt(INT32 index)
{
	if(index >= 0 && index < m_stringLength)
	{
		char *newString = ruNEW char [index + 1];

		memcpy(newString, m_string, index);
		newString[index] = 0;

		delete[] m_string;

		m_string = newString;
		m_stringLength = index;
	}

	// Generate hash code
	m_hashCode = RuGlobalHashGen().GetHashCode((BYTE *) m_string, m_stringLength);

	return *this;
}

// Truncates string after the index
CRuString &CRuString::TruncateAfter(INT32 index)
{
	if(index >= 0 && index < m_stringLength)
	{
		char *newString = ruNEW char [index + 2];

		memcpy(newString, m_string, index + 1);
		newString[index + 1] = 0;

		delete[] m_string;

		m_string = newString;
		m_stringLength = index + 1;
	}

	// Generate hash code
	m_hashCode = RuGlobalHashGen().GetHashCode((BYTE *) m_string, m_stringLength);

	return *this;
}

// Truncates string before the index
CRuString &CRuString::TruncateBefore(INT32 index)
{
	if(index >= 0 && index < m_stringLength)
	{
		char *newString = ruNEW char [m_stringLength - index + 1];

		memcpy(newString, &m_string[index], m_stringLength - index);
		newString[m_stringLength - index] = 0;

		delete[] m_string;

		m_string = newString;
		m_stringLength = m_stringLength - index;
	}

	// Generate hash code
	m_hashCode = RuGlobalHashGen().GetHashCode((BYTE *) m_string, m_stringLength);

	return *this;
}

CRuString &CRuString::Trim()
{
	INT32 i = 0;
	for(i = 0; i < m_stringLength; ++i)
	{
		if(m_string[i] != ' ')
			break;
	}

	TruncateBefore(i);

	for(i = m_stringLength - 1; i >= 0; --i)
	{
		if(m_string[i] != ' ')
			break;
	}

	TruncateAfter(i);

	// Generate hash code
	m_hashCode = RuGlobalHashGen().GetHashCode((BYTE *) m_string, m_stringLength);

	return *this;
}

CRuString &CRuString::ToLower()
{
	// Convert whole string to lower case
	_strlwr_s(m_string, m_stringLength + 1);

	// Generate hash code
	m_hashCode = RuGlobalHashGen().GetHashCode((BYTE *) m_string, m_stringLength);

	return *this;
}

CRuString &CRuString::ToUpper()
{
	// Convert whole string to upper case
	_strupr_s(m_string, m_stringLength + 1);

	// Generate hash code
	m_hashCode = RuGlobalHashGen().GetHashCode((BYTE *) m_string, m_stringLength);

	return *this;
}

CRuString **CRuString::Tokenize(char *separators) const
{
	INT32 numSeparators = (INT32) strlen(separators);

	INT32 tokenArraySize = 16, numTokens = 0;
	CRuString **tokenArray = ruNEW CRuString * [tokenArraySize];
	memset(tokenArray, 0, sizeof(CRuString *) * tokenArraySize);

	// Loop
	INT32 i = 0, j = 0, subStringStart = 0, subStringLength = 0;
	while(i < m_stringLength)
	{
		// Compare with each separator
		for(j = 0; j < numSeparators; ++j)
		{
			if(m_string[i] == separators[j])
			{
				break;
			}
		}

		++i;

		// If the current character is not a separator, increment substring length
		if(j >= numSeparators)
		{
			++subStringLength;
		}

		// If we have reached the end of the string, or if the current character is a separator, split this token
		if(i >= m_stringLength || j < numSeparators)
		{
			// Ensure the substring has length greater than zero
			if(subStringLength > 0)
			{
				if((numTokens + 1) >= tokenArraySize)
				{
					// Create new token array
					CRuString **newTokenArray = ruNEW CRuString * [tokenArraySize + 16];
					memset(newTokenArray, 0, sizeof(CRuString *) * (tokenArraySize + 16));

					// Copy old tokens
					memcpy(newTokenArray, tokenArray, sizeof(CRuString *) * tokenArraySize);

					delete[] tokenArray;
					tokenArraySize += 16;
					tokenArray = newTokenArray;
				}

				// Create token from the substring and add to token array
				tokenArray[numTokens] = ruNEW CRuString(m_string, subStringStart, subStringStart + subStringLength - 1);
				++numTokens;
			}

			// Reset substring length to zero
			subStringStart = i;
			subStringLength = 0;
		}
	}

	return tokenArray;
}

CRuString &CRuString::operator=(const char *str)
{
	delete[] m_string;

	if(str)
	{
		m_stringLength = (INT32)strlen(str);
		m_string = ruNEW char [m_stringLength + 1];
		memcpy(m_string, str, m_stringLength + 1);
		m_hashCode = RuGlobalHashGen().GetHashCode((BYTE *) m_string, m_stringLength);
	}
	else
	{
		m_string = ruNEW char [1];
		m_string[0] = 0;
		m_stringLength = 0;
		m_hashCode = 0;
	}

	return *this;
}

CRuString &CRuString::operator=(const CRuString &byteString)
{
	if(byteString.m_string)
	{
		delete[] m_string;
		m_stringLength = byteString.m_stringLength;
		m_string = ruNEW char [m_stringLength + 1];
		memcpy(m_string, byteString.m_string, m_stringLength + 1);
		m_hashCode = byteString.m_hashCode;
	}

	return *this;
}

BOOL CRuString::operator==(const char *str) const
{
	if(m_string && str)
	{
		UINT32 strLength = (UINT32)strlen(str);

		if(m_stringLength == strLength)
		{
			if(memcmp(m_string, str, m_stringLength) == 0)
				return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuString::operator!=(const CRuString &byteString) const
{
	if(m_string && byteString.m_stringLength)
	{
		if(m_stringLength == byteString.m_stringLength)
		{
			if(memcmp(m_string, byteString.m_string, m_stringLength) == 0)
				return FALSE;
		}
	}

	return TRUE;
}

BOOL CRuString::operator!=(const char *str) const
{
	if(m_string && str)
	{
		UINT32 strLength = (UINT32)strlen(str);

		if(m_stringLength == strLength)
		{
			if(memcmp(m_string, str, m_stringLength) == 0)
				return FALSE;
		}
	}

	return TRUE;
}

BOOL CRuString::operator==(const CRuString &byteString) const
{
	if(m_string && byteString.m_stringLength)
	{
		if(m_stringLength == byteString.m_stringLength)
		{
			if(memcmp(m_string, byteString.m_string, m_stringLength) == 0)
				return TRUE;
		}
	}

	return FALSE;
}

BOOL CRuString::operator<(const CRuString &byteString) const
{
	if(m_stringLength > 0 && byteString.m_stringLength > 0)
	{
		return (strcmp(m_string, byteString.m_string) < 0);
	}

	return FALSE;
}

BOOL CRuString::operator>(const CRuString &byteString) const
{
	if(m_stringLength > 0 && byteString.m_stringLength > 0)
	{
		return (strcmp(m_string, byteString.m_string) > 0);
	}

	return FALSE;
}

// ************************************************************************************************************************************************************

CRuStringTokenizer::CRuStringTokenizer()
:	m_numTokens(0),
	m_tokenArraySize(0),
	m_tokens(NULL)
{
}

CRuStringTokenizer::~CRuStringTokenizer()
{
	Reset();
}

void CRuStringTokenizer::Tokenize(const CRuString &ruString, const CRuString &separators)
{
	Reset();

	m_tokenArraySize = 16;
	m_tokens = ruNEW CRuString * [m_tokenArraySize];
	memset(m_tokens, 0, sizeof(CRuString *) * m_tokenArraySize);

	// Loop
	INT32 numSeparators = separators.m_stringLength;
	INT32 i = 0, j = 0, subStringStart = 0, subStringLength = 0;
	while(i < ruString.m_stringLength)
	{
		// Compare with each separator
		for(j = 0; j < numSeparators; ++j)
		{
			if(ruString.m_string[i] == separators.m_string[j])
			{
				break;
			}
		}

		++i;
		if(i >= ruString.m_stringLength)
		{
			j = 0;
			++subStringLength;
		}

		if(j == numSeparators)
		{
			// If the current character is not a separator, increment substring length
			++subStringLength;
		}
		else
		{
			// Ensure the substring has length greater than zero
			if(subStringLength > 0)
			{
				if(m_numTokens >= m_tokenArraySize)
				{
					// Create new token array
					CRuString **newTokenArray = ruNEW CRuString * [m_tokenArraySize + 16];
					memset(newTokenArray, 0, sizeof(CRuString *) * m_tokenArraySize + 16);

					// Copy old tokens
					memcpy(newTokenArray, m_tokens, sizeof(CRuString *) * m_tokenArraySize);

					delete[] m_tokens;
					m_tokenArraySize += 16;
					m_tokens = newTokenArray;
				}

				// Create token from the substring and add to token array
				m_tokens[m_numTokens] = ruNEW CRuString(ruString.m_string, subStringStart, subStringStart + subStringLength - 1);
				++m_numTokens;
			}

			// Reset substring length to zero
			subStringStart = i;
			subStringLength = 0;
		}
	}
}

INT32 CRuStringTokenizer::GetNumTokens() const
{
	return m_numTokens;
}

CRuString &CRuStringTokenizer::GetToken(INT32 index)
{
	return *m_tokens[index];
}

INT32 CRuStringTokenizer::GetTokenAsInteger(INT32 index)
{
	return atoi((char *) *m_tokens[index]);
}

void CRuStringTokenizer::Reset()
{
	for(INT32 i = 0; i < m_numTokens; ++i)
	{
		delete m_tokens[i];
	}

	delete[] m_tokens;

	m_numTokens = 0;
	m_tokenArraySize = 0;
	m_tokens = NULL;
}

// ************************************************************************************************************************************************************

const int		CRuMTRandom::N							= 624;
const int		CRuMTRandom::M							= 397;
const DWORD		CRuMTRandom::MATRIX_A					= 0x9908B0DF;
const DWORD		CRuMTRandom::UMASK						= 0x80000000;
const DWORD		CRuMTRandom::LMASK						= 0x7FFFFFFF;

/*!
	CRuMTRandom::CRuMTRandom(DWORD seed)
	Standard constructor, must specify seed.
	標準建構式, 需指定種子值.
*/
CRuMTRandom::CRuMTRandom(DWORD seed)
{
	m_state = ruNEW DWORD [CRuMTRandom::N];
	m_initialized = FALSE;
	m_left = 1;

	SetSeed(seed);
}

/*!
	CRuMTRandom::~CRuMTRandom()
	Standard destructor.
	標準解構式
*/
CRuMTRandom::~CRuMTRandom()
{
	delete[] m_state;
}

/*!
	void CRuMTRandom::SetSeed(DWORD seed)
	Sets the random seed.
	設定種子值.
	@param seed Random number seed (種子值)
	@return void
*/
void CRuMTRandom::SetSeed(DWORD seed)
{
	m_state[0]= seed & 0xFFFFFFFF;

	for(int j = 1; j < N; j++)
		m_state[j] = (1812433253UL * (m_state[j - 1] ^ (m_state[j - 1] >> 30)) + j); 

	m_left = 1;
	m_initialized = TRUE;
}

/*!
	UINT32 CRuMTRandom::GenerateUINT32()
	Generates a random number in the unsigned 32 bit integer range.
	產生一個UINT32亂數值.
	@return A number in the numeric range of a 32 bit unsigned integer
*/
UINT32 CRuMTRandom::GenerateUINT32()
{
    UINT32 y;

    if(--m_left == 0)
		NextState();

    y = *m_next++;

    // Tempering
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9D2C5680UL;
    y ^= (y << 15) & 0xEFC60000UL;
    y ^= (y >> 18);

    return y;
}

/*!
	INT32 CRuMTRandom::GenerateINT32()
	Generates a random number in the signed 32 bit integer range.
	產生一個INT32亂數值.
	@return A number in the numeric range of a 32 bit signed integer
*/
INT32 CRuMTRandom::GenerateINT32()
{
    UINT32 y;

    if(--m_left == 0)
		NextState();

    y = *m_next++;

    // Tempering
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9D2C5680UL;
    y ^= (y << 15) & 0xEFC60000UL;
    y ^= (y >> 18);

    return (INT32) (y >> 1);
}

/*!
	float CRuMTRandom::GenerateFloat()
	Generates a random number in the range [0, 1]
	產生一個浮點亂數值.
	@return A number in the range [0, 1]
*/
float CRuMTRandom::GenerateFloat()
{
    UINT32 y;

    if(--m_left == 0)
		NextState();

    y = *m_next++;

    // Tempering
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9D2C5680UL;
    y ^= (y << 15) & 0xEFC60000UL;
    y ^= (y >> 18);

    return (float) y * (1.0f / 4294967295.0f);
}

/*!
	void CRuMTRandom::NextState()
	Generates the next set of states for the generator.
	@return void
*/
void CRuMTRandom::NextState()
{
	DWORD *p = m_state;
	int j;

	// Initialize the seed if it has not already been initialized
	if(!m_initialized)
		SetSeed(57154UL);

	m_left = CRuMTRandom::N;
	m_next = m_state;

	for(j = CRuMTRandom::N - CRuMTRandom::M + 1; --j; p++) 
		*p = p[CRuMTRandom::M] ^ Twist(p[0], p[1]);

	for(j = CRuMTRandom::M; --j; p++) 
		*p = p[CRuMTRandom::M - CRuMTRandom::N] ^ Twist(p[0], p[1]);

	*p = p[CRuMTRandom::M - CRuMTRandom::N] ^ Twist(p[0], m_state[0]);
}

// ************************************************************************************************************************************************************

CRuInsertionSort::CRuInsertionSort()
{
	m_currentIndicesSize = 0;
	m_indices = NULL;

	// Initialize indices
	ResetIndices();
}

CRuInsertionSort::~CRuInsertionSort()
{
	delete[] m_indices;
}

CRuInsertionSort &CRuInsertionSort::Sort(const INT32 *values, INT32 numValues)
{
	// Resize the indices buffers if necessary
	if(numValues > m_currentIndicesSize)
		Resize(numValues);
	else
		ResetIndices();

	INT32 i, j, index;

	for(i = 1; i < numValues; ++i)
	{
		index = values[m_indices[i]];
		j = i;

		while((j > 0) && (values[m_indices[j - 1]] > index))
		{
			m_indices[j] = m_indices[j - 1];
			j = j - 1;
		}

		m_indices[j] = i;
	}

	return *this;
}

CRuInsertionSort &CRuInsertionSort::Sort(const UINT32 *values, INT32 numValues)
{
	// Resize the indices buffers if necessary
	if(numValues > m_currentIndicesSize)
		Resize(numValues);
	else
		ResetIndices();

	INT32 i, j;
	UINT32 index;

	for(i = 1; i < numValues; ++i)
	{
		index = values[m_indices[i]];
		j = i;

		while((j > 0) && (values[m_indices[j - 1]] > index))
		{
			m_indices[j] = m_indices[j - 1];
			j = j - 1;
		}

		m_indices[j] = i;
	}

	return *this;
}

CRuInsertionSort &CRuInsertionSort::Sort(const float *floatValues, INT32 numValues)
{
	// Resize the indices buffers if necessary
	if(numValues > m_currentIndicesSize)
		Resize(numValues);
	else
		ResetIndices();

	INT32 i, j;
	float index;

	for(i = 1; i < numValues; ++i)
	{
		index = floatValues[m_indices[i]];
		j = i;

		while((j > 0) && (floatValues[m_indices[j - 1]] > index))
		{
			m_indices[j] = m_indices[j - 1];
			j = j - 1;
		}

		m_indices[j] = i;
	}

	return *this;
}

UINT32 *CRuInsertionSort::GetSortedIndices()
{
	return m_indices;
}

void CRuInsertionSort::SetWorkspaceSize(INT32 maxElements)
{
	Resize(maxElements);
}

BOOL CRuInsertionSort::Resize(INT32 newSize)
{
	// Free previous allocations
	delete[] m_indices;

	// Reallocate according to new memory requirements
	m_currentIndicesSize = newSize;
	m_indices = ruNEW UINT32 [newSize];

	// Reset indices
	ResetIndices();

	return TRUE;
}

void CRuInsertionSort::ResetIndices()
{
	if(m_indices == NULL)
		return;

	// Resets the current indices back into increasing order
	for(INT32 i = 0; i < m_currentIndicesSize; ++i)
		m_indices[i] = i;
}

// ************************************************************************************************************************************************************

/*
--------------------------------------------------------------------
GetHashCode() -- hash a variable-length key into a 32-bit value
  k       : the key (the unaligned variable-length array of bytes)
  len     : the length of the key, counting by bytes
  initval : can be any 4-byte value
Returns a 32-bit value.  Every bit of the key affects every bit of
the return value.  Every 1-bit and 2-bit delta achieves avalanche.
About 6*len+35 instructions.

The best hash table sizes are powers of 2.  There is no need to do
mod a prime (mod is sooo slow!).  If you need less than 32 bits,
use a bitmask.  For example, if you need only 10 bits, do
  h = (h & hashmask(10));
In which case, the hash table should have hashsize(10) elements.

If you are hashing n strings (BYTE **)k, do it like this:
  for (i=0, h=0; i<n; ++i) h = hash( k[i], len[i], h);

By Bob Jenkins, 1996.  bob_jenkins@burtleburtle.net.  You may use this
code any way you wish, private, educational, or commercial.  It's free.

See http://burtleburtle.net/bob/hash/evahash.html
Use for hash table lookup, or anything where one collision in 2^^32 is
acceptable.  Do NOT use for cryptographic purposes.
--------------------------------------------------------------------
*/
DWORD CRuHashGenerator::GetHashCode(const BYTE *k, DWORD length)
{
	DWORD a, b, c, len;

	// Set up the internal state
	len = length;
	a = b = 0x9e3779b9;		// The golden ratio; an arbitrary value
	c = m_initVal;			// The previous hash value

	// Handle most of the key
	while (len >= 12)
	{
		a += (k[0] +((DWORD) k[1] << 8) + ((DWORD) k[2] << 16) + ((DWORD) k[3] << 24));
		b += (k[4] +((DWORD) k[5] << 8) + ((DWORD) k[6] << 16) + ((DWORD) k[7] << 24));
		c += (k[8] +((DWORD) k[9] << 8) + ((DWORD) k[10] << 16)+ ((DWORD) k[11] << 24));
		CRuHashGenerator_MIX(a, b, c);
		k += 12; len -= 12;
	}

	// Handle the last 11 bytes
	c += length;
	switch(len)				// All the case statements fall through
	{
		case 11:
			c += ((DWORD) k[10] << 24);
		case 10:
			c += ((DWORD) k[9] << 16);
		case 9:
			c += ((DWORD) k[8] << 8);
		// The first byte of c is reserved for the length
		case 8:
			b += ((DWORD) k[7] << 24);
		case 7:
			b += ((DWORD) k[6] << 16);
		case 6:
			b += ((DWORD) k[5] << 8);
		case 5:
			b += k[4];
		case 4:
			a += ((DWORD) k[3] << 24);
		case 3:
			a += ((DWORD) k[2] << 16);
		case 2:
			a += ((DWORD) k[1] << 8);
		case 1:
			a += k[0];
		// case 0: nothing left to add
	}

	CRuHashGenerator_MIX(a, b, c);

	// Return the result
	return c;
}

// ************************************************************************************************************************************************************

CRuGUID CRuGUID::Invalid(0, 0, 0, 0);


// ************************************************************************************************************************************************************
CRuRadixSort::CRuRadixSort()
{
	m_histogram = ruNEW UINT32 [256 * 8];
	m_offsets = ruNEW UINT32 [256];
	m_indicesA = NULL;
	m_indicesB = NULL;
	m_currentIndicesSize = 0;

	// Initialize indices
	ResetIndices();
}

/*!
	CRuRadixSort::~CRuRadixSort()
	Standard destructor.
	???P????c|?
*/
CRuRadixSort::~CRuRadixSort()
{
	delete[] m_histogram;
	delete[] m_offsets;
	delete[] m_indicesA;
	delete[] m_indicesB;
}

/*!
	CRuRadixSort &CRuRadixSort::Sort(const UINT32 *values, UINT32 numValues, BOOL signedValues, BOOL preserveLastSortResults)
	Sorts the specified array of integer values using radix sort
	@param values Value array
	@param numValues Number of values in the array
	@param preserveLastSortResults Whether to preserve temporal data from the previous sort
	@return A reference to the sorting object
*/
CRuRadixSort &CRuRadixSort::Sort(const INT32 *values, UINT32 numValues, BOOL preserveLastSortResults)
{
	return Sort((UINT32 *) values, numValues, TRUE, preserveLastSortResults);
}

/*!
	CRuRadixSort &CRuRadixSort::Sort(const UINT32 *values, UINT32 numValues, BOOL signedValues, BOOL preserveLastSortResults)
	Sorts the specified array of UINT32eger values using radix sort
	@param values Value array
	@param numValues Number of values in the array
	@param signedValues Whether to treat input values as signed
	@param preserveLastSortResults Whether to preserve temporal data from the previous sort
	@return A reference to the sorting object
*/
CRuRadixSort &CRuRadixSort::Sort(const UINT32 *values, UINT32 numValues, BOOL signedValues, BOOL preserveLastSortResults)
{
	UINT32 i, j;

	if(values == NULL || numValues == 0)
		return *this;

	// Resize the indices buffers if necessary
	if(numValues > m_currentIndicesSize)
		Resize(numValues);
	else if(preserveLastSortResults == FALSE)
		ResetIndices();
	m_currentIndicesSize = numValues;

	// Construct histogram
	BOOL process;
	if(!signedValues)
		process = CreateHistogram(values, numValues);
	else
		process = CreateHistogram((const INT32 *) values, numValues);
	if(!process)
		return *this;

	// Compute number of negative values by going through the MSB histogram
	UINT32 numNegativeValues = 0;
	if(signedValues)
	{
		UINT32 *h3 = &m_histogram[768];
		for(i = 128; i < 256; i++)
			numNegativeValues += h3[i];
	}

	// Radix sort
	for(j = 0; j < 4; j++)
	{
		// If all values are the same, then there is no need to perform this pass
		UINT32 *curCount = &m_histogram[j << 8];
		BYTE uniqueValue = *(((BYTE *) values) + j);
		BOOL performPass = TRUE;
		if(curCount[uniqueValue] == numValues)
			performPass = FALSE;

		if(performPass)
		{
			if(j != 3 || !signedValues)
			{
				// Create offsets
				m_offsets[0] = 0;
				for(i = 1; i < 256; i++)
					m_offsets[i] = m_offsets[i - 1] + curCount[i - 1];
			}
			else
			{
				// Create biased offsets so we order negative numbers properly
				m_offsets[0] = numNegativeValues;
				for(i = 1; i < 128; i++)
					m_offsets[i] = m_offsets[i - 1] + curCount[i - 1];

				// Fix wrong place
				m_offsets[128] = 0;
				for(i = 129; i < 256; i++)
					m_offsets[i] = m_offsets[i - 1] + curCount[i - 1];
			}

			// Perform radix sort
			BYTE *inputBytes = (BYTE *) values;
			UINT32 *indices = m_indicesA;
			UINT32 *indicesEnd = &m_indicesA[numValues];
			
			inputBytes += j;	// Offset the input pointer
			while(indices != indicesEnd)
			{
				UINT32 id = *indices++;
				m_indicesB[m_offsets[inputBytes[id << 2]]++] = id;
			}

			// Swap pointers for next pass
			UINT32 *temp = m_indicesA;
			m_indicesA = m_indicesB;
			m_indicesB = temp;
		}
	}

	return *this;
}

/*!
	CRuRadixSort &CRuRadixSort::Sort(const float *floatValues, UINT32 numValues, BOOL preserveLastSortResults)
	Sorts the specified array of float values using radix sort
	@param floatValues Value array
	@param numValues Number of values in the array
	@param preserveLastSortResults Whether to preserve temporal data from the previous sort
	@return A reference to the sorting object
*/
CRuRadixSort &CRuRadixSort::Sort(const float *floatValues, UINT32 numValues, BOOL preserveLastSortResults)
{
	UINT32 i, j;

	if(floatValues == NULL || numValues == 0)
		return *this;

	// Recast floats to DWORDs
	UINT32 *values = (UINT32 *) floatValues;

	// Resize the indices buffers if necessary
	if(numValues > m_currentIndicesSize)
		Resize(numValues);
	else if(preserveLastSortResults == FALSE)
		ResetIndices();
	m_currentIndicesSize = numValues;

	// Construct histogram
	if(!CreateHistogram(floatValues, numValues))
		return *this;

	// Compute number of negative values by going through the MSB histogram
	UINT32 numNegativeValues = 0;
	UINT32 *h3 = &m_histogram[768];
	for(i = 128; i < 256; i++)
		numNegativeValues += h3[i];

	// Radix sort
	for(j = 0; j < 4; j++)
	{
		// If all values are the same, then there is no need to perform this pass
		UINT32 *curCount = &m_histogram[j << 8];
		BYTE uniqueValue = *(((BYTE *) values) + j);
		BOOL performPass = TRUE;
		if(curCount[uniqueValue] == numValues)
			performPass = FALSE;

		if(j != 3)
		{
			if(performPass)
			{
				// Create offsets
				m_offsets[0] = 0;
				for(i = 1; i < 256; i++)
					m_offsets[i] = m_offsets[i - 1] + curCount[i - 1];

				// Perform radix sort
				BYTE *inputBytes = (BYTE *) values;
				UINT32 *indices = m_indicesA;
				UINT32 *indicesEnd = &m_indicesA[numValues];
				
				inputBytes += j;	// Offset the input pointer
				while(indices != indicesEnd)
				{
					UINT32 id = *indices++;
					m_indicesB[m_offsets[inputBytes[id << 2]]++] = id;
				}

				// Swap pointers for next pass
				UINT32 *temp = m_indicesA;
				m_indicesA = m_indicesB;
				m_indicesB = temp;
			}
		}
		else
		{
			if(performPass)
			{
				// Create biased offsets so we order negative numbers properly
				m_offsets[0] = numNegativeValues;
				for(i = 1; i < 128; i++)
					m_offsets[i] = m_offsets[i - 1] + curCount[i - 1];

				// Reverse sorting order for negative numbers
				m_offsets[255] = 0;
				for(i = 0; i < 127; i++)	// Fix wrong ordering
					m_offsets[254 - i] = m_offsets[255 - i] + curCount[255 - i];
				for(i = 128; i < 256; i++)	// Fix wrong place
					m_offsets[i] += curCount[i];

				// Perform radix sort
				for(i = 0; i < numValues; i++)
				{
					UINT32 radix = values[m_indicesA[i]] >> 24;
					if(radix < 128)
						m_indicesB[m_offsets[radix]++] = m_indicesA[i];		// Number is positive
					else
						m_indicesB[--m_offsets[radix]] = m_indicesA[i];		// Number is negative

				}

				// Swap pointers for next pass
				UINT32 *temp = m_indicesA;
				m_indicesA = m_indicesB;
				m_indicesB = temp;
			}
			else
			{
				// The pass is useless, yet we still have to reverse the order of current list if all values are negative
				if(uniqueValue >= 128)
				{
					for(i = 0; i < numValues; i++)
						m_indicesB[i] = m_indicesA[numValues - i - 1];

					// Swap pointers for next pass
					UINT32 *temp = m_indicesA;
					m_indicesA = m_indicesB;
					m_indicesB = temp;
				}
			}
		}
	}

	return *this;
}

/*!
	CRuRadixSort &CRuRadixSort::Sort(const UINT64 *values, UINT32 numValues, BOOL signedValues, BOOL preserveLastSortResults)
	Sorts the specified array of integer values using radix sort
	@param values Value array
	@param numValues Number of values in the array
	@param preserveLastSortResults Whether to preserve temporal data from the previous sort
	@return A reference to the sorting object
*/
CRuRadixSort &CRuRadixSort::Sort(const INT64 *values, UINT32 numValues, BOOL preserveLastSortResults)
{
	return Sort((UINT64 *) values, numValues, TRUE, preserveLastSortResults);
}

/*!
	CRuRadixSort &CRuRadixSort::Sort(const UINT64 *values, UINT32 numValues, BOOL signedValues, BOOL preserveLastSortResults)
	Sorts the specified array of UINT32eger values using radix sort
	@param values Value array
	@param numValues Number of values in the array
	@param signedValues Whether to treat input values as signed
	@param preserveLastSortResults Whether to preserve temporal data from the previous sort
	@return A reference to the sorting object
*/
CRuRadixSort &CRuRadixSort::Sort(const UINT64 *values, UINT32 numValues, BOOL signedValues, BOOL preserveLastSortResults)
{
	UINT32 i, j;

	if(values == NULL || numValues == 0)
		return *this;

	// Resize the indices buffers if necessary
	if(numValues > m_currentIndicesSize)
		Resize(numValues);
	else if(preserveLastSortResults == FALSE)
		ResetIndices();
	m_currentIndicesSize = numValues;

	// Construct histogram
	BOOL process;
	if(!signedValues)
		process = CreateHistogram(values, numValues);
	else
		process = CreateHistogram((const INT64*) values, numValues);
	if(!process)
		return *this;

	// Compute number of negative values by going through the MSB histogram
	UINT32 numNegativeValues = 0;
	if(signedValues)
	{
		UINT32 *h7 = &m_histogram[1792];
		for(i = 128; i < 256; i++)
			numNegativeValues += h7[i];
	}

	// Radix sort
	for(j = 0; j < 8; j++)
	{
		// If all values are the same, then there is no need to perform this pass
		UINT32 *curCount = &m_histogram[j << 8];
		BYTE uniqueValue = *(((BYTE *) values) + j);
		BOOL performPass = TRUE;
		if(curCount[uniqueValue] == numValues)
			performPass = FALSE;

		if(performPass)
		{
			if(j != 7 || !signedValues)
			{
				// Create offsets
				m_offsets[0] = 0;
				for(i = 1; i < 256; i++)
					m_offsets[i] = m_offsets[i - 1] + curCount[i - 1];
			}
			else
			{
				// Create biased offsets so we order negative numbers properly
				m_offsets[0] = numNegativeValues;
				for(i = 1; i < 128; i++)
					m_offsets[i] = m_offsets[i - 1] + curCount[i - 1];

				// Fix wrong place
				m_offsets[128] = 0;
				for(i = 129; i < 256; i++)
					m_offsets[i] = m_offsets[i - 1] + curCount[i - 1];
			}

			// Perform radix sort
			BYTE *inputBytes = (BYTE *) values;
			UINT32 *indices = m_indicesA;
			UINT32 *indicesEnd = &m_indicesA[numValues];
			
			inputBytes += j;	// Offset the input pointer
			while(indices != indicesEnd)
			{
				UINT32 id = *indices++;
				m_indicesB[m_offsets[inputBytes[id << 3]]++] = id;
			}

			// Swap pointers for next pass
			UINT32 *temp = m_indicesA;
			m_indicesA = m_indicesB;
			m_indicesB = temp;
		}
	}

	return *this;
}

/*!
	UINT32 *CRuRadixSort::GetSortedIndices()
	Retrieves the sorted indices from the last sorting operation
	@return A pointer to the sorted indices
*/
UINT32 *CRuRadixSort::GetSortedIndices()
{
	return m_indicesA;
}

/*!
	void CRuRadixSort::SetWorkspaceSize(UINT32 maxElements)
	Sets the workspace size for the radix sorter
	@return void
*/
void CRuRadixSort::SetWorkspaceSize(UINT32 maxElements)
{
	Resize(maxElements);
}

/*!
	BOOL CRuRadixSort::Resize(UINT32 newSize)
	Sets the workspace size for the radix sorter
	@return TRUE if successful
*/
BOOL CRuRadixSort::Resize(UINT32 newSize)
{
	// Free previous allocations
	delete[] m_indicesA;
	delete[] m_indicesB;

	// Reallocate according to new memory requirements
	m_indicesA = ruNEW UINT32 [newSize];
	m_indicesB = ruNEW UINT32 [newSize];
	m_currentIndicesSize = newSize;

	// Reset indices
	ResetIndices();

	return TRUE;
}

/*!
	void CRuRadixSort::ResetIndices()
	Clears the sorted indices array.
	@return void
*/
void CRuRadixSort::ResetIndices()
{
	if(m_indicesA == NULL)
		return;

	// Resets the current indices back into increasing order
	for(UINT32 i = 0; i < m_currentIndicesSize; i++)
		m_indicesA[i] = i;
}

BOOL CRuRadixSort::CreateHistogram(const INT32 *values, UINT32 numValues)
{
	// Reset counters
	memset(m_histogram, 0, sizeof(UINT32) * 256 * 4);

	// Prepare for temporal coherence
	INT32 prevValue = values[m_indicesA[0]];
	BOOL alreadySorted = TRUE;
	UINT32 *indices = m_indicesA;

	// Prepare to count
	BYTE *p = (BYTE *) values;
	BYTE *pe = &p[numValues * 4];
	UINT32 *h0 = &m_histogram[0];				// Histogram for first pass (LSB)
	UINT32 *h1 = &m_histogram[256];				// Histogram for second pass
	UINT32 *h2 = &m_histogram[512];				// Histogram for third pass
	UINT32 *h3 = &m_histogram[768];				// Histogram for fourth pass (MSB)

	while(p != pe)
	{
		// Read input buffer in previous sorted order
		INT32 value = values[*indices++];

		// Check whether already sorted
		if(value < prevValue)
		{
			alreadySorted = FALSE;
			break;
		}

		// Update for next iteration
		prevValue = value;

		// Create histograms
		h0[*p++]++;
		h1[*p++]++;
		h2[*p++]++;
		h3[*p++]++;
	}

	// If already sorted, we just return and leave the list unchanged
	if(alreadySorted)
	{
		return FALSE;
	}

	// Otherwise finish constructing the histograms
	while(p != pe)
	{
		// Create histograms without previous overhead
		h0[*p++]++;
		h1[*p++]++;
		h2[*p++]++;
		h3[*p++]++;
	}

	return TRUE;
}

BOOL CRuRadixSort::CreateHistogram(const UINT32 *values, UINT32 numValues)
{
	// Reset counters
	memset(m_histogram, 0, sizeof(UINT32) * 256 * 4);

	// Prepare for temporal coherence
	UINT32 prevValue = values[m_indicesA[0]];
	BOOL alreadySorted = TRUE;
	UINT32 *indices = m_indicesA;

	// Prepare to count
	BYTE *p = (BYTE *) values;
	BYTE *pe = &p[numValues * 4];
	UINT32 *h0 = &m_histogram[0];				// Histogram for first pass (LSB)
	UINT32 *h1 = &m_histogram[256];				// Histogram for second pass
	UINT32 *h2 = &m_histogram[512];				// Histogram for third pass
	UINT32 *h3 = &m_histogram[768];				// Histogram for fourth pass (MSB)

	while(p != pe)
	{
		// Read input buffer in previous sorted order
		UINT32 value = values[*indices++];

		// Check whether already sorted
		if(value < prevValue)
		{
			alreadySorted = FALSE;
			break;
		}

		// Update for next iteration
		prevValue = value;

		// Create histograms
		h0[*p++]++;
		h1[*p++]++;
		h2[*p++]++;
		h3[*p++]++;
	}

	// If already sorted, we just return and leave the list unchanged
	if(alreadySorted)
	{
		return FALSE;
	}

	// Otherwise finish constructing the histograms
	while(p != pe)
	{
		// Create histograms without previous overhead
		h0[*p++]++;
		h1[*p++]++;
		h2[*p++]++;
		h3[*p++]++;
	}

	return TRUE;
}

BOOL CRuRadixSort::CreateHistogram(const float *floatValues, UINT32 numValues)
{
	// Reset counters
	memset(m_histogram, 0, sizeof(UINT32) * 256 * 4);

	// Prepare for temporal coherence
	float prevValue = floatValues[m_indicesA[0]];
	BOOL alreadySorted = TRUE;
	UINT32 *indices = m_indicesA;

	// Prepare to count
	BYTE *p = (BYTE *) floatValues;
	BYTE *pe = &p[numValues * 4];
	UINT32 *h0 = &m_histogram[0];				// Histogram for first pass (LSB)
	UINT32 *h1 = &m_histogram[256];				// Histogram for second pass
	UINT32 *h2 = &m_histogram[512];				// Histogram for third pass
	UINT32 *h3 = &m_histogram[768];				// Histogram for fourth pass (MSB)

	while(p != pe)
	{
		// Read input buffer in previous sorted order
		float value = floatValues[*indices++];

		// Check whether already sorted
		if(value < prevValue)
		{
			alreadySorted = FALSE;
			break;
		}

		// Update for next iteration
		prevValue = value;

		// Create histograms
		h0[*p++]++;
		h1[*p++]++;
		h2[*p++]++;
		h3[*p++]++;
	}

	// If already sorted, we just return and leave the list unchanged
	if(alreadySorted)
	{
		return FALSE;
	}

	// Otherwise finish constructing the histograms
	while(p != pe)
	{
		// Create histograms without previous overhead
		h0[*p++]++;
		h1[*p++]++;
		h2[*p++]++;
		h3[*p++]++;
	}

	return TRUE;
}

BOOL CRuRadixSort::CreateHistogram(const INT64 *values, UINT32 numValues)
{
	// Reset counters
	memset(m_histogram, 0, sizeof(UINT32) * 256 * 8);

	// Prepare for temporal coherence
	INT64 prevValue = values[m_indicesA[0]];
	BOOL alreadySorted = TRUE;
	UINT32 *indices = m_indicesA;

	// Prepare to count
	BYTE *p = (BYTE *) values;
	BYTE *pe = &p[numValues * 8];
	UINT32 *h0 = &m_histogram[0];					// Histogram for first pass (LSB)
	UINT32 *h1 = &m_histogram[256];				// Histogram for second pass
	UINT32 *h2 = &m_histogram[512];				// Histogram for third pass
	UINT32 *h3 = &m_histogram[768];				// Histogram for fourth pass (MSB)

	UINT32 *h4 = &m_histogram[1024];				// Histogram for fifth pass (MSB)
	UINT32 *h5 = &m_histogram[1280];				// Histogram for sixth pass (MSB)
	UINT32 *h6 = &m_histogram[1536];				// Histogram for seventh pass (MSB)
	UINT32 *h7 = &m_histogram[1792];				// Histogram for eighth pass (MSB)

	while(p != pe)
	{
		// Read input buffer in previous sorted order
		INT64 value = values[*indices++];

		// Check whether already sorted
		if(value < prevValue)
		{
			alreadySorted = FALSE;
			break;
		}

		// Update for next iteration
		prevValue = value;

		// Create histograms
		h0[*p++]++;
		h1[*p++]++;
		h2[*p++]++;
		h3[*p++]++;
		h4[*p++]++;
		h5[*p++]++;
		h6[*p++]++;
		h7[*p++]++;
	}

	// If already sorted, we just return and leave the list unchanged
	if(alreadySorted)
	{
		return FALSE;
	}

	// Otherwise finish constructing the histograms
	while(p != pe)
	{
		// Create histograms without previous overhead
		h0[*p++]++;
		h1[*p++]++;
		h2[*p++]++;
		h3[*p++]++;
		h4[*p++]++;
		h5[*p++]++;
		h6[*p++]++;
		h7[*p++]++;
	}

	return TRUE;
}

BOOL CRuRadixSort::CreateHistogram(const UINT64 *values, UINT32 numValues)
{
	// Reset counters
	memset(m_histogram, 0, sizeof(UINT32) * 256 * 8);

	// Prepare for temporal coherence
	UINT64 prevValue = values[m_indicesA[0]];
	BOOL alreadySorted = TRUE;
	UINT32 *indices = m_indicesA;

	// Prepare to count
	BYTE *p = (BYTE *) values;
	BYTE *pe = &p[numValues * 8];
	UINT32 *h0 = &m_histogram[0];					// Histogram for first pass (LSB)
	UINT32 *h1 = &m_histogram[256];				// Histogram for second pass
	UINT32 *h2 = &m_histogram[512];				// Histogram for third pass
	UINT32 *h3 = &m_histogram[768];				// Histogram for fourth pass (MSB)
	UINT32 *h4 = &m_histogram[1024];				// Histogram for fifth pass (MSB)
	UINT32 *h5 = &m_histogram[1280];				// Histogram for sixth pass (MSB)
	UINT32 *h6 = &m_histogram[1536];				// Histogram for seventh pass (MSB)
	UINT32 *h7 = &m_histogram[1792];				// Histogram for eighth pass (MSB)

	while(p != pe)
	{
		// Read input buffer in previous sorted order
		UINT64 value = values[*indices++];

		// Check whether already sorted
		if(value < prevValue)
		{
			alreadySorted = FALSE;
			break;
		}

		// Update for next iteration
		prevValue = value;

		// Create histograms
		h0[*p++]++;
		h1[*p++]++;
		h2[*p++]++;
		h3[*p++]++;
		h4[*p++]++;
		h5[*p++]++;
		h6[*p++]++;
		h7[*p++]++;
	}

	// If already sorted, we just return and leave the list unchanged
	if(alreadySorted)
	{
		return FALSE;
	}

	// Otherwise finish constructing the histograms
	while(p != pe)
	{
		// Create histograms without previous overhead
		h0[*p++]++;
		h1[*p++]++;
		h2[*p++]++;
		h3[*p++]++;
		h4[*p++]++;
		h5[*p++]++;
		h6[*p++]++;
		h7[*p++]++;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
