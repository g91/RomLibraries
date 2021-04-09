#include <windows.h>
#include "debug.h"
#include "jdiffos.h"
#include "primes.h"

/* List of primes we select from when size is specified on commandline */
int   g_PrimeTables[24] = 
{ 
	2147483647, 1073741789, 536870909,  268435399,  
	134217689,   67108859,  33554393,   16777213,    
	8388593,    4194301,   2097143,    1048573,
	524287,     262139,    131071,      65521,
	32749,      16381,      8191,       4093,
	2039,       1021,       509,        251
} ;



/* -----------------------------------------------------------------------------
* "Constants", modifiable by commandline options
* ---------------------------------------------------------------------------*/








/* The hash table. Using a struct causes certain compilers (gcc) to align     */
/* fields on 64-bit boundaries, causing 25% memory loss. Therefore, I use     */
/* two arrays instead of an array of structs.                                 */

                        
int hashTableGetPrime(int index)
{
	return g_PrimeTables[index];

}


hashTable::hashTable():
m_hashCollisions(0),
m_hashHit(0),
m_hashCollisionsSubsequentCount(0),
m_hashPrime(16381),
m_phashTablePos(NULL),
m_pHashKey(NULL),
m_hashElements(0),
 m_hashIndex (17),      /* choosen prime from g_PrimeTables                        */
m_hashSizes(0)
{

}

hashTable:: ~hashTable()
{

	
}
void hashTable::SetIndex(int index)
{
	 m_hashIndex = index ;     

	 m_hashPrime = g_PrimeTables[m_hashIndex] ;

}

void hashTable::SetPrime(int Prime)
{
	int index;
	for (index=0; index < 23 && g_PrimeTables[index] > Prime; index++) 
		;
	m_hashIndex=index;
	m_hashPrime = g_PrimeTables[index] ;


}

/* -----------------------------------------------------------------------------
* The hash function
* ---------------------------------------------------------------------------*/
void hashTable::GetNext ( int aiCurVal, ulong *alCurHsh )
{ 
	*alCurHsh =  ((*alCurHsh) << 1) + aiCurVal ;
#ifdef _DEBUG
	DBG_Printf(DBGHSK,
		"Hash Key %x %x %c\n", *alCurHsh, aiCurVal,
		(aiCurVal>=32 && aiCurVal <= 127)?aiCurVal:' ');
#endif
}
/* -----------------------------------------------------------------------------
* Initialization
* ---------------------------------------------------------------------------*/
void hashTable::Initial (void)
{ 
	if ( m_phashTablePos == NULL ) 
	{
		m_hashSizes = m_hashPrime * (sizeof(int) + sizeof(ulong));
		m_phashTablePos =(int *) new char [m_hashSizes] ;
		m_pHashKey = (ulong *) &m_phashTablePos[m_hashPrime] ;

#ifdef _DEBUG
		DBG_Printf(DBGHSH, 
			"Hash Ini sizeof=%2d+%2d=%2d, %d samples, %d bytes, address=%x-%x,%x-%x.\n", 
			sizeof(ulong), sizeof(int), sizeof(ulong) + sizeof(int), 
			m_hashPrime, m_hashSizes, 
			m_phashTablePos, &m_phashTablePos[m_hashPrime], m_pHashKey, &m_pHashKey[m_hashPrime]) ;
#endif

		m_hashCollisionsSubsequent    = 0;
		m_hashCollisionsMax    = 2;
		m_hashCollisionsMaxPos = m_hashPrime;
	}
	if ( m_phashTablePos == NULL ) 
	{
		DBG_Printf(-1, "Insufficient memory!");
		exit(7) ;
	}
	memset(m_phashTablePos, 0, m_hashSizes) ;
}

void hashTable:: Finish(void)
{


	if (m_phashTablePos)
		delete []m_phashTablePos;
		m_phashTablePos=0;
}


/* -----------------------------------------------------------------------------
* The hashtable insert and lookup function
* ---------------------------------------------------------------------------*/
void hashTable::Add (ulong alCurHsh, int alPos, int alBse )
{ 
	int   llKey ;         /* alCurHsh % prime */

	/* calculate key and the corresponding entries' address */
	llKey    = (alCurHsh % m_hashPrime) ;

	/* keep m_hashCollisionsMax == m_hashCollisionsMaxPos / m_hashPrime + 2 */
	if ( alPos - alBse >= m_hashCollisionsMaxPos ) 
	{
		m_hashCollisionsMaxPos += m_hashPrime ;
		m_hashCollisionsMax ++ ;
	}

	/* store key and value
	*  existing entries are overwritten if they are "old", that is
	*  they are before the actual read position, or if more
	*  than m_hashCollisionsMax subsequent collisions occured  
	*/

	/* count elements and collisions */
	if ((m_phashTablePos[llKey] <= alBse))
		m_hashElements++ ; /* entry is emty, element will be added */
	else 
	{ 
		m_hashCollisions++ ; /* entry is not empty, collision occurs */
		m_hashCollisionsSubsequent++ ;
		if (m_hashCollisionsSubsequent >= m_hashCollisionsMax)
			m_hashCollisionsSubsequentCount++;
	}

	/* store key and value
	* existing entries are overwritten if
	* - they are old (older than alBse)
	* - more than a maximum number of subsequent collisions occurred
	*/
	if ((m_phashTablePos[llKey] <= alBse) || (m_hashCollisionsSubsequent >= m_hashCollisionsMax))
	{ 
#ifdef _DEBUG

			DBG_Printf(DBGHSH, 
				"Hash Add %8d  %d  %8x %c\n", 
			llKey, alPos, alCurHsh,
			(m_pHashKey[llKey] == 0)?'.':'!');
#endif

		m_pHashKey[llKey] = alCurHsh ;
		m_phashTablePos[llKey] = alPos ;
		m_hashCollisionsSubsequent   = 0 ;
	}
}



int hashTable::Get (ulong alCurHsh)
{ 
	int   llKey ;         /* alCurHsh % prime */
	/* calculate key and the corresponding entries' address */
	llKey    = (alCurHsh % m_hashPrime) ;
	/* lookup value into hashtable for new file */
	if (m_pHashKey[llKey] == alCurHsh) 
	{
		m_hashHit++;
		return m_phashTablePos[llKey];
	}
	return 0 ;
}

/* -----------------------------------------------------------------------------
* Print hashtable content (for debugging or auditing)
* ---------------------------------------------------------------------------*/
void hashTable::Print() 
{ 
	int hashIndex;
	int count=0;
	for (hashIndex = 0; hashIndex < m_hashPrime; hashIndex ++) 
	{
		if (m_phashTablePos[hashIndex] != 0) 
		{
			count ++ ;
			DBG_Printf(-1, "Hash Pnt %06d %8zd-%08lx\n", hashIndex, 
				m_phashTablePos[hashIndex], m_pHashKey[hashIndex]) ;
		}
	}
	DBG_Printf(-1, "Hash table load:       %d%%\n", count / m_hashPrime / 2) ;
	DBG_Printf(-1, "Hash table collisions: %d\n", m_hashCollisions) ;
}

