#include <windows.h>
#include <stdio.h>
#include "jdiffos.h"
#include "MDiff.h"
#include "debug.h"
#include "FileStream.h"

 int MDiff::g_verbose=0;
   /* Compare even if data not in buffer?             */



       
MDiff::MDiff():
 m_bSourceBacktrace(true),/* Backtrace on sourcefile allowed?                */
m_outByteCtrl(0),
m_bOutEsc(false),
m_outByteData(0),
m_outByteEscape(0),
m_outByteEqual(0),
m_outByteDelete(0),
m_outByteBacktraced(0),
m_srcScan ( 0) ,         /* Prescan source file: 0=no, 1=do, 2=done         */
m_bIni(false),
m_EqualThreshold(20),
m_hashErrorCount(0)
{
m_OutPut=&MDiff::OutByteBin;
}

 MDiff::~MDiff()
 {
	Finish();
 }

void MDiff::SetMatchRange(int min,int max)
{

	m_matchMap.m_matchMin=min;
	m_matchMap.m_matchMax=max;

}
void MDiff::Finish(void)
{

	m_hashtable.Finish();
}
void MDiff::SetPrimeIndex(int index)
{

m_hashtable.SetIndex(index);
}

void  MDiff:: SetSourceBacktrace(bool bValue)
{

	m_bSourceBacktrace=bValue;

}
void  MDiff:: SetPrime(int hashPrime)
{

	m_hashtable.SetPrime(hashPrime);
}
 /* Prescan source file: 0=no, 1=do, 2=done         */
void MDiff::SetPreScan(int iscan)
{
	m_srcScan=iscan;
}
void MDiff::SetOutput(OUTTYPE type)
{
	switch(type)
	{
	case OT_TEXT:
		m_OutPut=&MDiff::OutByteAscii;
		break;
	case OT_REGION:
		m_OutPut=&MDiff::OutByteRegion;
		break;
	case OT_BINARY:
		m_OutPut=&MDiff::OutByteBin;
		break;

	}

}

/* ---------------------------------------------------------------
* ufPutByt outputs a byte, prefixing a data sequence <esc> <opcode>
* with an addition <esc> byte.
* ---------------------------------------------------------------*/
void  MDiff::PutByte ( int aByteValue, IDataStream *aFileOut )
{
	if (m_bOutEsc) 
	{
		m_bOutEsc = false;
		if (aByteValue >= BKT && aByteValue <= ESC) 
		{
			/* output an additional <esc> byte */

			int opcode=ESC;

			aFileOut->write(&opcode,1);


		
			m_outByteEscape++ ;
		}
		int opcode=ESC;

		aFileOut->write(&opcode,1);
		m_outByteData++;
	}
	if (aByteValue == ESC) 
	{
		m_bOutEsc = true ;
	}
	else 
	{
		

		aFileOut->write(&aByteValue,1);
		
		m_outByteData++;
	}
}

/* ---------------------------------------------------------------
* MDiff::PutLen outputs a length as follows
* byte1  following      formula              if number is
* -----  ---------      -------------------- --------------------
* 0-251                 1-252                between 1 and 252
* 252    x              253 + x              between 253 and 508
* 253    xx             253 + 256 + xx       a 16-bit number
* 254    xxxx           253 + 256 + xxxx     a 32-bit number
* 255    xxxxxxxx       253 + 256 + xxxxxxxx a 64-bit number
* ---------------------------------------------------------------*/
void MDiff::PutLength( int aLength, IDataStream *aFileOut )
{

	BYTE data[128];

	
	if (aLength <= 252) 
	{
		data[0]=aLength-1;
		

		aFileOut->write(data,1);
	
		m_outByteCtrl += 1;
	} 
	else if (aLength <= 508) 
	{

		data[0]=252;
		data[1]=aLength-253;
		aFileOut->write(data,2);
	
		m_outByteCtrl += 2;
	}
	else if (aLength <= 0xffff) 
	{

		data[0]=253;
		data[1]=(aLength >>  8)&0xff;
		data[2]=aLength&0xff;

		aFileOut->write(data,3);


		m_outByteCtrl += 3;
	}
	else if (aLength <= 0xffffffff) 
	{

		data[0]=254;
		data[1]=(aLength >>  24) & 0xff;
		data[2]=(aLength >>  16) & 0xff;
		data[3]=(aLength >>  8 )& 0xff;
		data[4]=aLength&0xff;


	

		aFileOut->write(data,5);

		m_outByteCtrl += 5;
	} 
#if defined(_JDIFF_LARGEFILE)
	else 
	{
		

		data[0]=255;
		data[1]=(aLength >>  56) & 0xff;
		data[2]=(aLength >>  48) & 0xff;
		data[3]=(aLength >>  40) & 0xff;
		data[4]=(aLength >>  32) & 0xff;
		data[5]=(aLength >>  24) & 0xff;
		data[6]=(aLength >>  16) & 0xff;
		data[7]=(aLength >>  8) & 0xff;
		data[8]=(aLength) & 0xff;
		




		aFileOut->write(data,9);

		m_outByteCtrl += 9;
	}
#endif
}



/* ---------------------------------------------------------------
* PutOpr outputs a new opcode and closes the previous
* data stream.
* ---------------------------------------------------------------*/
void MDiff::PutOpcode ( int opcode, IDataStream *aFileOut )
{ 
	if (m_bOutEsc) 
	{
		int oudate=ESC;

		aFileOut->write(&oudate,1);
		aFileOut->write(&oudate,1);

	
		m_bOutEsc = false ;
		m_outByteEscape++ ;
		m_outByteData++ ;
	}
	if ( opcode != ESC ) 
	{
		int oudate=ESC;

		aFileOut->write(&oudate,1);

		aFileOut->write(&opcode,1);
	
		m_outByteCtrl+=2;
	}
}
void _fprintf(IDataStream  * a,char* fmt,...)
{

}

void MDiff::OutByteAscii ( 
						  int aOpcode, 
						  int aLength,
						  int aOrgValue, 
						  int aNewValue,
						  FileStream   *aFileOrg, 
						  FileStream   *aFilNew,
						  IDataStream  *aFileOut,
						  int aOrgPos,
						  int aNewPos 
						  )
{
	static int currentOpcode ;

	if (aOrgPos > 0) 
		_fprintf(aFileOut, "%d", aOrgPos) ;
	else
		_fprintf(aFileOut,"         ");
	if (aNewPos > 0) 
		_fprintf(aFileOut, "%d", aNewPos) ;
	else
		_fprintf(aFileOut,"         ");

	switch (aOpcode)
	{
	case (MOD) :
		_fprintf(aFileOut, "MOD %3o %3o %c-%c\n", aOrgValue, aNewValue, 
			((aOrgValue >= 32 && aOrgValue <= 127)?(char) aOrgValue:' '),
			((aNewValue >= 32 && aNewValue <= 127)?(char) aNewValue:' '));

		if (currentOpcode != aOpcode) 
		{
			currentOpcode = aOpcode ;
			m_outByteCtrl+=2;
		}
		if (aNewValue == ESC)
			m_outByteEscape++;
		m_outByteData++;
		break;

	case (INS) :
		_fprintf(aFileOut, "INS     %3o  -%c\n", aNewValue,
			((aNewValue >= 32 && aNewValue <= 127)?(char) aNewValue:' '));

		if (currentOpcode != aOpcode) 
		{
			currentOpcode = aOpcode ;
			m_outByteCtrl+=2;
		}
		if (aNewValue == ESC)
			m_outByteEscape++;
		m_outByteData++;
		break;

	case (DEL) :
		_fprintf(aFileOut, "DEL %ld\n", aLength);

		currentOpcode=DEL;
		m_outByteCtrl+=2+sizeof(aLength);
		m_outByteDelete+=aLength;
		break;

	case (BKT) :
		_fprintf(aFileOut, "BKT %ld\n", aLength);

		currentOpcode=BKT;
		m_outByteCtrl+=2+sizeof(aLength);
		m_outByteBacktraced+=aLength;
		break;

	case (EQL) :
		_fprintf(aFileOut, "EQL %3o %3o %c-%c\n", aOrgValue, aNewValue, 
			((aOrgValue >= 32 && aOrgValue <= 127)?(char) aOrgValue:' '),
			((aNewValue >= 32 && aNewValue <= 127)?(char) aNewValue:' '));

		if (currentOpcode != aOpcode)
		{
			currentOpcode = aOpcode ;
			m_outByteCtrl+=2;
		}
		if (aNewValue == ESC)
			m_outByteEscape++;
		m_outByteData++;
		break;
	}
}


void  MDiff::OutByteRegion 
( 
 int aOpcode, 
 int aLength,
 int aOrgValue, 
 int aNewValue,
 FileStream *aFileOrg, 
 FileStream *aFileNew,
 IDataStream *aFileOut,
 int aOrgPos,
 int aNewPos 
 )
{ 
	static int currentOpcode=ESC ;
	static int opcodeCount ;
	static int equalCount = 0 ;

	/* equal count pre-buffer */
	switch (aOpcode)
	{ 
	case (EQL) :
		if (equalCount < 4 && currentOpcode != EQL)
		{
			equalCount ++ ;
			return ;
		}
		break ;
	case (MOD) :
		break ;
	default : 
		if (equalCount > 0)
		{ 
			_fprintf(aFileOut, "%d %d EQL %ld\n", aOrgPos - equalCount, aNewPos - equalCount, equalCount);
			m_outByteCtrl+=2;
			m_outByteEqual+=equalCount;
			equalCount = 0;
		}
	}

	/* write output when operation code changes */
	if (aOpcode != currentOpcode) 
	{
		switch (currentOpcode) 
		{
		case (MOD) :
			m_outByteCtrl+=2;
			m_outByteData+=opcodeCount ;
			_fprintf(aFileOut, " %d %d MOD %ld\n", aOrgPos - opcodeCount - equalCount, aNewPos - opcodeCount - equalCount, opcodeCount) ;
			break;

		case (INS) :
			m_outByteCtrl+=2;
			m_outByteData+=opcodeCount ;
			_fprintf(aFileOut,  " %d  INS %ld\n", aNewPos - opcodeCount - equalCount, opcodeCount) ;
			break;

		case (DEL) :
			m_outByteCtrl+=2;
			m_outByteDelete+=opcodeCount;
			_fprintf(aFileOut,  " %d %d DEL %ld\n", aOrgPos - opcodeCount - equalCount, aNewPos - equalCount, opcodeCount);
			break;

		case (BKT) :
			m_outByteCtrl+=2;
			m_outByteBacktraced+=opcodeCount;
			_fprintf(aFileOut, " %d %d BKT %ld\n", aOrgPos + opcodeCount - equalCount, aNewPos - equalCount, opcodeCount);
			break;

		case (EQL) :
			m_outByteCtrl+=2;
			m_outByteEqual+=opcodeCount ;
			_fprintf(aFileOut, " %d %d EQL %ld\n", aOrgPos - opcodeCount, aNewPos - opcodeCount, opcodeCount);
			break;
		}

		currentOpcode = aOpcode;
		opcodeCount = 0;
	}

	/* accumulate operation codes */
	switch (aOpcode)
	{
	case(DEL):
		opcodeCount += aLength ;
		break ;
	case(BKT):
		opcodeCount += aLength ;
		break ;
	case(INS):
		opcodeCount++ ;
		if (aNewValue == ESC) 
			m_outByteEscape++;
		break;
	case(MOD):
		if (equalCount > 0)
		{
			opcodeCount += equalCount ;
			equalCount =  0 ;
		} 
		opcodeCount++ ;
		if (aNewValue == ESC) 
			m_outByteEscape++;
		break;
	case(EQL): 
		if (equalCount > 0)
		{ 
			opcodeCount += equalCount ;
			equalCount =  0 ;
		} 
		opcodeCount++ ;
		break;
	}
}

/* ---------------------------------------------------------------
* ufOutBytBin: binary output function for generating patch files
* ---------------------------------------------------------------*/
void MDiff::OutByteBin ( 
				  int aOpcode,
				  int aLength, 
				  int aOrgValue, 
				  int aNewValue,
				  FileStream   *aFileOrg, 
				  FileStream   *aFileNew,
				  IDataStream  *aFileOut,
				  int aOrgPos,
				  int aNewPos 
				  )
{ 
	static int currentOpcode = ESC ; 
	static int equalCount = 0;    /* number of subsequent equal bytes */
	static int equallBuff[4];     /* first four equal bytes */
	int liCnt ;

	/* Output a pending EQL operand (if more than 4 equal bytes) */
	if (equalCount > 0 && aOpcode != EQL)
	{
		if (equalCount > 4 || (currentOpcode != MOD && aOpcode != MOD)) 
		{
			currentOpcode = EQL;
			PutOpcode(EQL, aFileOut) ;
			PutLength(equalCount, aFileOut);

			m_outByteEqual+=equalCount;
		} 
		else 
		{
			if (currentOpcode != MOD) 
			{
				currentOpcode = MOD ;
				PutOpcode(MOD, aFileOut) ;
			}
			for (liCnt=0; liCnt < equalCount; liCnt++) 
				PutByte(equallBuff[liCnt], aFileOut) ;
		}
		equalCount=0;
	}

	/* Handle current operand */
	switch (aOpcode)
	{
	case ESC : /* before closing the output */
		PutOpcode(ESC, aFileOut);
		currentOpcode = ESC ;
		break;

	case MOD :
	case INS :
		if (currentOpcode != aOpcode) 
		{
			currentOpcode = aOpcode ;
			PutOpcode(aOpcode, aFileOut) ;
		}
		PutByte(aNewValue, aFileOut) ;
		break;

	case DEL :
		PutOpcode(DEL, aFileOut) ;
		PutLength(aLength, aFileOut);

		currentOpcode=DEL;
		m_outByteDelete+=aLength;
		break;

	case BKT :
		PutOpcode(BKT, aFileOut) ;
		PutLength(aLength, aFileOut);

		currentOpcode=BKT;
		m_outByteBacktraced+=aLength;
		break;

	case EQL :
		if (equalCount < 4)
			equallBuff[equalCount] = aOrgValue ; 
		equalCount ++ ;
		break;
	}
}




/*******************************************************************************
* Difference function
*
* Takes two files as arguments and writes out the differences
*
* Principle: 
*   Take one byte from each file and compare. If they are equal, then continue.
*   If they are different, start lookahead to find next equal blocks within file.
*   If equal blocks are found, 
*   - first insert or delete the specified number of bytes,
*   - then continue reading on both files until equal blocks are reached,
*
*******************************************************************************/
int MDiff::diff ( FileStream *aFileOrg, FileStream *aFileNew, IDataStream *aFileOut )
{
	int aOrgValue ;           /* byte from original file */
	int aNewValue ;           /* byte from new file */

	int orgPos = 0 ;  /* read position on original file */
	int llPosNew = 0 ;  /* read position on new file */

	bool   bFind=false;    /* offsets are pointing to a valid solution?*/
	int liAhd=0;        /* number of bytes to go ahead after a find */
	int skipOrg ;      /* number of bytes to skip in original file */ 
	int liSkpNew ;      /* number of bytes to skip in new file */

	/* Take one byte from each file ... */
	aOrgValue = aFileOrg->Get(orgPos, 0);
	aNewValue = aFileNew->Get(llPosNew, 0);
	while (aNewValue > EOF)
	{
#ifdef _DEBUG
		DBG_Printf(DBGPRG, 
			"Input %d->%2x %d->%2x liAhd=%d.\n",
			orgPos - 1, aOrgValue, llPosNew - 1, aNewValue, liAhd)  ;
#endif

		if(liAhd > 0) 
		{
			/* 
			* If equal blocks are found
			* Continue reading on both files until equal blocks are reached
			*/
			liAhd--;

		} 
		else 
		{
			/* 
			* If lbFnd is set, we should now have reached an equal block.
			* If not, this is a serious malfunction of the algorithm !!!
			*/
			if (bFind)
			{
				if (aOrgValue != aNewValue)
				{
					if (bFind && (g_verbose>2 || m_matchMap.m_bCmpAll))
					{
						DBG_Printf(-1, "Hash miss!\n");
						m_hashErrorCount++ ;
					}
				}
				bFind = false ;
			}

			/*
			* Find the next equal block
			*/
			if (aOrgValue != aNewValue) 
			{
				/* go back one byte */
				if (aOrgValue > EOF) 
					orgPos -- ;
				llPosNew -- ;

				/* flush output when DEBUGing */
#ifdef _DEBUG
				if (DBG_Get(DBGMCH) || DBG_Get(DBGAHD) )
					(this->*m_OutPut)(ESC, 0, 0, 0, 0, 0, aFileOut, orgPos, llPosNew);
#endif

				/*
				* If they are different, start lookahead to find next equal blocks within file.
				*/
				bFind = FindAhead(aFileOrg, aFileNew, orgPos, llPosNew, skipOrg, liSkpNew, liAhd) ;
#ifdef _DEBUG

				DBG_Printf(DBGAHD,

					"Findahead on %d %d skip %d %d ahead %d\n", 
					orgPos - 1, llPosNew - 1, skipOrg, liSkpNew, liAhd)  ;
				DBG_Printf(DBGAHD, 
					"Current position in new file= %d\n", llPosNew) ;
#endif

				if (bFind)
				{
					/*
					* If equal blocks are found
					* First insert, delete or backtrace the specified number of bytes
					*/
					if (skipOrg > 0)
					{
						(this->*m_OutPut)(DEL, skipOrg, 0, 0, aFileOrg, aFileNew, aFileOut, orgPos + 1, llPosNew + 1) ;
						orgPos += skipOrg ;
						aOrgValue = aFileOrg->Get( orgPos, 0);
					} 
					else if (skipOrg < 0)
					{
						(this->*m_OutPut)(BKT, - skipOrg, 0, 0, aFileOrg, aFileNew, aFileOut, orgPos + 1, llPosNew + 1) ;
						orgPos += skipOrg ;
						aOrgValue = aFileOrg->Get( orgPos, 0);
					}
					else if (aOrgValue > EOF)
						orgPos ++ ;

					llPosNew ++ ;
					if (liSkpNew > 0) 
					{
						while (liSkpNew > 0) 
						{
							(this->*m_OutPut)(INS, 1, 0, aNewValue, aFileOrg, aFileNew, aFileOut, orgPos, llPosNew);
							liSkpNew-- ;
							aNewValue = aFileNew->Get(llPosNew, 0);
						}
					}
				}
				else 
				{
					if (g_verbose>1)
					{
						DBG_Printf(-1, "No valid matches found at positions %%8zd %%8zd, skipping %ld bytes.\n", orgPos, llPosNew, liAhd) ; 
					}
					if (aOrgValue > EOF) 
						orgPos ++ ;
					llPosNew ++ ;
				} 
			}
		}

		/*
		* Output the result
		*/
		if(aOrgValue == aNewValue)
		{
			(this->*m_OutPut)(EQL, 1, aOrgValue, aNewValue, aFileOrg, aFileNew, aFileOut, orgPos, llPosNew);
		} else {
			(this->*m_OutPut)(MOD, 1, aOrgValue, aNewValue, aFileOrg, aFileNew, aFileOut, orgPos, llPosNew);
		}

		/* Take next byte from each file ... */
		aOrgValue = aFileOrg->Get( orgPos, 0) ;
		aNewValue = aFileNew->Get( llPosNew, 0) ;
	}
	(this->*m_OutPut)(ESC, 0, 0, 0, 0, 0, aFileOut, orgPos, llPosNew);
	return (1);
}

void MDiff::Initial (FileStream* aFileOrg)
{

	m_hashtable.Initial();
	m_hashErrorCount=0;
	/* 
	* Initialize table of matches on first call 
	*/
	if (! m_bIni)
	{ 
		m_matchMap.Initial();

		m_bIni=true;
	}
	/* Prescan the original file? */
	if (m_srcScan == 1)
	{
		FindAheadScan(aFileOrg) ;
		m_srcScan = 2 ;
	}


}
/* -----------------------------------------------------------------------------
* Auxiliary function:
* Get next character from file (lookahead) and count number of equal chars
* in current cluster 
* ---------------------------------------------------------------------------*/
void _FindAheadGet(FileStream *aFile, int &aPos, int &aValue, int &aPrenValue, int &aEqualCount, int aSft) 
{
	aPrenValue = aValue ;
	aValue =aFile->Get(aPos, aSft) ;
	if (aPrenValue == aValue) 
	{ 
		if (aEqualCount < 32) 
			aEqualCount++ ; 
	}
	else    
	{
		if (aEqualCount > 0) 
			aEqualCount-- ; 
	}
}
int MDiff:: GetHashCollisionsSubsequentCount()
{
return m_hashtable.m_hashCollisionsSubsequentCount;
}
int MDiff::GetHashSize()
{

	return m_hashtable.GetHashSize();
}

int MDiff:: GetHashRepairedCount(void)
{

	return m_matchMap.m_hashRepairedCount;
}


int MDiff::GetHashElements(void)
{

	return m_hashtable.m_hashElements;
}
int MDiff::GetHashHit()
{
	return m_hashtable.m_hashHit;
}

int MDiff::GetHashCollisions(void)
{

	return m_hashtable.m_hashCollisions;
}
void  MDiff::HashPrint(void) 
{

	m_hashtable.Print();
}

/* -----------------------------------------------------------------------------
* Prescan the original file
* ---------------------------------------------------------------------------*/
void MDiff::FindAheadScan (  FileStream *aFileOrg)
{ 
	ulong orgHash=0;     // Current hash value for original file
	int   orgValue;       // Current file value
	int   liPrvOrg;       // Previous file value
	int   liEqlOrg=0;     // Number of times current value occurs in hash value
	int orgPos=0;     // Position within original file

	int   liIdx ;

	/* Initialize hash function */
	orgValue = aFileOrg->Get( orgPos, 1) ;
	for (liIdx=0;(liIdx < 31) && (orgValue > EOF); liIdx++) 
	{

		m_hashtable.GetNext(orgValue, &orgHash) ;
		_FindAheadGet(aFileOrg, orgPos, orgValue, liPrvOrg, liEqlOrg, 1) ;
	}

	/* Build hashtable */
	while (orgValue > EOF) 
	{
		m_hashtable.GetNext(orgValue, &orgHash) ;
		if ( liEqlOrg < m_EqualThreshold ) 
		{
			m_hashtable.Add(orgHash, orgPos, 0) ;
#ifdef _DEBUG
			DBG_Printf(DBGAHH,
				"hashTable->Add(%2x -> %8x, %d, %d)\n", 
				orgValue, orgHash, orgPos, 0);
#endif
		}
		_FindAheadGet(aFileOrg, orgPos, orgValue, liPrvOrg, liEqlOrg, 1) ;
	}
}
/*******************************************************************************
* Find Ahead function
*
* Read ahead on both files until we possibly found an equal series of 32 bytes 
* in both files.
* Then calculate the deplacement vector between two files:
* - positive if characters need to be inserted in the original file,
* - negative if characters need to be removed from the original file.
*******************************************************************************/
/* -----------------------------------------------------------------------------
* The findahead function
* ---------------------------------------------------------------------------*/
bool MDiff::FindAhead (
				FileStream *aFileOrg,
				FileStream *aFileNew,
				int aReadOrgPos,       /* read position in original file                     */
				int aReadNewPos,       /* read position in new file                          */
				int &aSkipOrgCount,      /* number of bytes to skip (delete) in original file  */
				int &aSkipNewCount,      /* number of bytes to skip (insert) in new file       */
				int &aAheadCount          /* number of bytes to go before similarity is reached */
				)
{
	static int aheadOrgPos=0;   // Current ahead position on original file
	static int aheadNewPos=0;   // Current ahead position on new file
	static ulong currentHashOrg=0;   // Current hash value for original file
	static ulong currentHashNew=0;   // Current hash value for new file
	static int orgValue;       // Current file value
	static int liValNew;       // Current file value
	static int liPrvOrg;       // Previous file value
	static int liPrvNew;       // Previous file value
	static int liEqlOrg=0;     // Number of times current value occurs in hash value
	static int liEqlNew=0;     // Number of times current value occurs in hash value
	static// Table of matches initialized?

	int FindOrg=0;   /* Found position within original file                 */
	int FindNew=0;   /* Found position within new file                      */
	int baseOrgPos;     /* Base position on original file: g_bSourceBacktrace?0:aReadOrgPos */

	int Index;          /* Index for initializing                         */
	int Find=0;        /* Number of matches found                        */
	int Best;          /* Selected match                                 */
	int Sft;          /* 1 = hard look-ahead, 2 = soft look-ahead       */

	
	Sft = 1 ; /* find at least one match */



	/* 
	* Re-Initialize hash function (read 31 bytes) if 
	* - ahead position has been reset, or
	* - read position has passed the ahead position
	*/
	if (m_srcScan == 0 && (aheadOrgPos == 0 || aheadOrgPos < aReadOrgPos)) 
	{
		aheadOrgPos = aReadOrgPos ;
		liEqlOrg = 0 ;
		currentHashOrg = 0 ;

		orgValue = aFileOrg->Get( aheadOrgPos, Sft) ;
		for (Index=0;(Index < 31) && (orgValue > EOF); Index++)
		{ 
			m_hashtable.GetNext(orgValue, &currentHashOrg) ;
			_FindAheadGet(aFileOrg, aheadOrgPos, orgValue, liPrvOrg, liEqlOrg, Sft) ;
		}
	}
	if (aheadNewPos == 0 || aheadNewPos < aReadNewPos) 
	{
		aheadNewPos = aReadNewPos ;
		liEqlNew = 0 ;
		currentHashNew = 0 ;

		liValNew = aFileNew->Get( aheadNewPos, Sft) ;
		for (Index=0;(Index < 31) && (liValNew > EOF); Index++)
		{ 
			m_hashtable.GetNext(liValNew, &currentHashNew) ;
			_FindAheadGet(aFileNew, aheadNewPos, liValNew, liPrvNew, liEqlNew, Sft) ;
		}
	}

	/*
	* Build the table of matches
	*/
	if (m_matchMap.haveFree(aReadNewPos))
	{
		/* Do not backtrace before baseOrgPos */
		baseOrgPos = (m_bSourceBacktrace?0:aReadOrgPos) ;

		/* Do not read from original file if it has been prescanned */
		if (m_srcScan > 0)
			orgValue = EOB ;

		/* Scroll through both files until an equal hash value has been found */
		while (((orgValue > EOF) || (liValNew > EOF )) && (Find < m_matchMap.m_matchMax))
		{
			/* insert original file's value into hashtable (if no prescanning has been done) */
			if (orgValue > EOF) 
			{ 
				/* hash the new value */
				m_hashtable.GetNext(orgValue, &currentHashOrg) ;

				/* add to hash table */
				if ( liEqlOrg < m_EqualThreshold )
				{ 
					m_hashtable.Add(currentHashOrg, aheadOrgPos, baseOrgPos) ;

#ifdef _DEBUG
					DBG_Printf(DBGAHH,
						"ufHshAdd(%2x -> %8x, %8d, %8d)\n", 
						orgValue, currentHashOrg, aheadOrgPos, baseOrgPos);
#endif
				}

				/* get next value from file */
				_FindAheadGet(aFileOrg, aheadOrgPos, orgValue, liPrvOrg, liEqlOrg, Sft) ;
			}

			/* check new file against original file */
			if (liValNew > EOF)
			{ /* hash the new value and lookup in hashtable */
				m_hashtable.GetNext(liValNew, &currentHashNew) ;
				if ( liEqlNew < m_EqualThreshold )
					FindOrg = m_hashtable.Get(currentHashNew) ;
				else
					FindOrg = 0 ;

				/* add found position into table of matches */
				if (FindOrg > baseOrgPos)
				{ 
					/* add solution to the table of matches */ 
					switch (m_matchMap.Add(aFileOrg, aFileNew, FindOrg, aheadNewPos, aReadNewPos))
					{ 
					case 0: /* table is full */
						Find = m_matchMap.m_matchMax; 
						break;
					case 1: /* alternative added */
						Find ++ ;
						Sft = (Find >=m_matchMap.m_matchMin)?2:1 ;
						break ;
					case 2:  ; /* alternative colided */
					case -1: ;/* compare failed      */
					}
				}

				/* get next value from file */
				_FindAheadGet(aFileNew, aheadNewPos, liValNew, liPrvNew, liEqlNew, Sft) ;
			}
		}
	}

	/* 
	* Get the best match and calculate the offsets 
	*/ 
	Best = m_matchMap.Best(aFileOrg, aFileNew, aReadOrgPos, aReadNewPos, FindOrg, FindNew) ;
	if (Best == -1)
	{
		aSkipOrgCount = 0 ;    
		aSkipNewCount = 0 ;   
		aAheadCount    = (aheadNewPos - aReadNewPos) / 4 ;
		if (aAheadCount < 256)
			aAheadCount = 256 ;
		return false ; 
	}
	else
	{
		if (FindOrg >= aReadOrgPos)
		{
			if (FindOrg - aReadOrgPos >= FindNew - aReadNewPos) 
			{
				/* go forward on original file */
				aSkipOrgCount = FindOrg - aReadOrgPos + aReadNewPos - FindNew ;
				aSkipNewCount = 0 ;
				aAheadCount    = FindNew - aReadNewPos ; 
			}
			else
			{
				/* go forward on new file */
				aSkipOrgCount = 0;
				aSkipNewCount = FindNew - aReadNewPos + aReadOrgPos - FindOrg ;
				aAheadCount    = FindOrg - aReadOrgPos ;
			}
		}
		else
		{
			/* backtrack on original file */
			aSkipOrgCount = (int) aReadOrgPos - FindOrg + FindNew - aReadNewPos ;
			if (aSkipOrgCount < aReadOrgPos) 
			{
				aSkipNewCount = 0 ;
				aSkipOrgCount = - aSkipOrgCount ;
				aAheadCount = FindNew - aReadNewPos ;
			}
			else /* do not bactrace before beginning of file */
			{
				aSkipNewCount = aSkipOrgCount - (int) aReadOrgPos ;
				aSkipOrgCount = - (int) aReadOrgPos ;
				aAheadCount = (FindNew - aReadNewPos) - aSkipNewCount ;
			}

			/* reset ahead position when backtracking */
			aheadOrgPos = 0 ;
		}
		return true ;
	}
}