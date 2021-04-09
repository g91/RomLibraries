
/*******************************************************************************
* Hash table functions:
*  hashTableInitial     Initialize hash table (allocate and memset)
*  ufHshAdd     Insert value into hashtable
*  ufHshGet     Lookup value into hashtable 
*  ufHshNxt     Incremental hash function on array of bytes
*
* The hash table stores positions within files. The key reflects the contents
* of the file at that posistion. This way, we can efficiently find regions
* that are equal between both files.
*
* Hash function on array of bytes:
*
* Principle:
* ----------
* Input:  a[32]  24 8-bit values
*         p      prime number
* Output: h = (a[31] x 2^31 + a[30] . 2^30 + .. + a[0]) % 2^32 % p
*
* 
* Largest n-bit primes: 251, 509, 1021, 2039, 4093, 8191, 16381, 32749, 65521, 
*                       131071 (17 bit), ..., 4294967291 (32 bit)
*
* Table entries contain a 32-bit hash value and a file position.
*
* The collision strategy tries to create a uniform distributed set of samples
* over the investigated region, which is either the whole file or the 
* look-ahead region.
* This is achieved by overwriting the original entry only if
* - the original entry lies before the investigatd region (the base position)
* - a number of subsequent non-overwriting collisions occur where
*   number = (region size / hashtable size) + 2
*
* Only samples from the original file are stored. 
* Samples from the new file are looked up.
*
* The investigated region is either
* - the whole file when the prescan/backtrace option is used (default)
* - the look-ahead region otherwise (options -f or -ff)
*
* With prescan/backtrace enabled, the algorithm behaves like a kind of 
* copy/insert algorithm (simulated with insert/delete/modify and backtrace
* instructions). Without prescan/backtrace, the algorithm behaves like an
* insert/delete algorithm.
*
*******************************************************************************/
#pragma once
int hashTableGetPrime(int index);
class hashTable
{

public:
	hashTable();
	virtual ~hashTable();

	void SetIndex(int index);
	void SetPrime(int Prime);
	void Initial (void);
	void Finish(void);
	void Print(void); 
	void Add (ulong alCurHsh, int alPos, int alBse );
	void GetNext(int aiCurVal, ulong *alCurHsh ); 
	int Get (ulong alCurHsh);

	int GetHashSize(void){return m_hashSizes;}

	/* -----------------------------------------------------------------------------
	* Hashtable variables
	* ---------------------------------------------------------------------------*/

	int   m_hashIndex;// = 17 ;      /* choosen prime from g_PrimeTables                        */
	int   m_hashCollisionsMax;          /* max number of collisions before override       */
	ulong m_hashCollisionsMaxPos;       /* increment m_hashCollisionsMax if position gets larger  */
	int	m_hashCollisionsSubsequent;        /* Number of subsequent collisions.                  */
	int m_hashCollisionsSubsequentCount;   /* Number of times max number of collisions occurred.*/
	int m_hashHit;         /* Number of hash hits       */

	int m_hashCollisions;         /* Number of collisions                              */

	int   m_hashPrime  ;   /* default largest 16-bit prime for hashing        */

	int *m_phashTablePos ; /* Position within the file                       */
	ulong *m_pHashKey  ; /* Hash key                                       */
	int m_hashElements;         /* Number of elements added to hashtable             */

	int   m_hashSizes ;            /* Actual size of the hashtable                   */

};



