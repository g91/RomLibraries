#pragma once
#include "matchMap.h"
#include "primes.h"
struct FileStream ;

struct IDataStream ;

struct MDiff
{
	enum  OUTTYPE
	{
		OT_TEXT,
		OT_REGION,
		OT_BINARY

	};

	MDiff();
	~MDiff();

	void SetOutput(OUTTYPE type);


	int GetHashErrorCount(){return m_hashErrorCount;}
	void SetPreScan(int iscan);

	void SetSourceBacktrace(bool value);

	void SetMatchRange(int min,int max);
	void SetPrime(int hashPrime);
	void SetPrimeIndex(int index);
	void SetCmpAll(bool bValue){m_matchMap.m_bCmpAll=bValue;}
	int diff ( FileStream *asFilOrg, FileStream *asFilNew, IDataStream *asFilOut );

	
	void PutLength ( int aiLen, IDataStream *asFileOut );
	void PutOpcode ( int aiOpr, IDataStream *asFilOut );
	void PutByte ( int aiByt, IDataStream *asFilOut );

	void Initial (FileStream* aFileOrg);
	void Finish(void);

	int GetHashHit();

	int GetHashCollisionsSubsequentCount();
	int GetHashSize();

	int GetHashCollisions(void);
	int GetHashElements(void);

	
	int GetHashRepairedCount(void);


	void HashPrint(void) ;
	void FindAheadScan(FileStream *aFileOrg);

	bool FindAhead (
		FileStream *aFileOrg,
		FileStream *aFileNew,
		int aReadOrgPos,       /* read position in original file                     */
		int aReadNewPos,       /* read position in new file                          */
		int &aSkipOrgCount,      /* number of bytes to skip (delete) in original file  */
		int &aSkipNewCount,      /* number of bytes to skip (insert) in new file       */
		int &aAheadCount          /* number of bytes to go before similarity is reached */
		);


	typedef void (MDiff::*OutPut)(
		int   aiOpr, 
		int aiLen,
		int   aiOrg, 
		int   aiNew,
		FileStream   *asFilOrg, 
		FileStream   *asFilNew,
		IDataStream  *asFilOut,
		int alPosOrg,
		int alPosNew );    

	void OutByteRegion 
		( 
		int   aiOpr, 
		int aiLen,
		int   aiOrg, 
		int   aiNew,
		FileStream   *asFilOrg, 
		FileStream   *asFilNew,
		IDataStream  *asFilOut,
		int alPosOrg,
		int alPosNew 
		);

	void OutByteAscii ( 
		int   aiOpr, 
		int aiLen,
		int   aiOrg, 
		int   aiNew,
		FileStream   *asFilOrg, 
		FileStream   *asFilNew,
		IDataStream  *asFilOut,
		int alPosOrg,
		int alPosNew 
		);

	void OutByteBin ( 
		int   aiOpr,
		int aiLen, 
		int   aiOrg, 
		int   aiNew,
		FileStream   *asFilOrg, 
		FileStream   *asFilNew,
		IDataStream  *asFilOut,
		int alPosOrg,
		int alPosNew 
		);
	int GetOutByteDelete(){return m_outByteDelete;}
	int GetOutByteEqual(){return m_outByteEqual;}
	int GetOutByteData(){return m_outByteData;}
	int GetOutByteCtrl(){return m_outByteCtrl;}
	int GetOutByteEscape(){return m_outByteEscape;}
	int GetOutByteBacktraced(){return m_outByteBacktraced;}
	int GetPrimeIndex(){return m_hashtable.m_hashIndex;}

	OutPut m_OutPut;
	int m_outByteDelete;//=0;    /* Number of data    bytes deleted                   */
	int m_outByteBacktraced;//=0;    /* Number of data    bytes backtraced                */

	bool m_bOutEsc;

	int m_outByteEqual;//=0;    /* Number of data    bytes not written (gain)        */
	int m_outByteData;//=0;    /* Number of data    bytes written                   */
	int m_outByteCtrl;//=0;    /* Number of control bytes written (overhead)        */
	int m_outByteEscape;//=0;    /* Number of escape  bytes written (overhead)        */
	bool m_bIni;// = false ; 


	int m_EqualThreshold;// = 20 ;        /* Equal threshold: throw samples if > 32 eql. chr.*/

	static int g_verbose;
	bool m_bSourceBacktrace;
	int m_hashErrorCount;//=0;         /* Number of false hash hits                         */
	int m_srcScan;///* Prescan source file: 0=no, 1=do, 2=done         */
	hashTable m_hashtable;
	MatchMap m_matchMap;
};