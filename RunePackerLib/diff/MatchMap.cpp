#include <windows.h>
#include <stdio.h>
#include "jdiffos.h"
#include "MatchMap.h"
#include "debug.h"
#include "FileStream.h"






MatchMap::MatchMap():m_matchMax(8),m_matchMin(4),m_matchDistance(1024),m_hashRepairedCount(0),m_bCmpAll(false)
{

}
void MatchMap::SetMatchRange(int min,int max)
{
	m_matchMax=max;
	m_matchMin=min;

}


void MatchMap::Initial (void)
{
	memset(m_matchData, 0,  sizeof(m_matchData)) ; 
}







/* -----------------------------------------------------------------------------
* Auxiliary function: ufMchAdd
* Add given match to the array of matches:
* - add to colliding match if possible, or
* - add at the end of the list, or 
* - override an old match otherwise
* Returns 
*  -1   if regions are found different
*   0   if a new entry has been added and table is full
*   1   if a new entry has been added
*   2   if an existing entry has been enlarged
* ---------------------------------------------------------------------------*/
int MatchMap::Add (
				   FileStream   *aFileOrg,
				   FileStream   *aFileNew,
				   int aFindOrgAdd,      /* match to add               */
				   int aFindNewAdd,
				   int aBaseNew
				   )
{
	int index;    /* scrolls through all matches                  */
	int firstFree=-1; /* points to the first free entry               */
	int next=-1; /* points to the next  free entry               */

	aFindOrgAdd -= 24 ;
	aFindNewAdd -= 24 ;

	/* entries before aBaseNew do not make any sense */
	/* if (aFindNewAdd < aBaseNew) return -1 ; */

	/* add to existing match or find oldest match */
	for (index=0; index < m_matchMax ; index++)
	{

		MatchData &data=m_matchData[index];
		if (data.m_matchCount == 0)
		{ 
			if (firstFree == -1 ||m_matchData[firstFree].m_matchCount != 0)
			{
				next = firstFree ;
				firstFree = index ;
			}
			else if (next == -1)
			{ 
				next = index ;
			}
		} 
		else
		{ 
			/* add to colliding match if possible */
			if ((aFindOrgAdd -data.m_lastMatchOrg == aFindNewAdd - data.m_lastMatchNew) && 
				(aFindNewAdd <= data.m_lastMatchNew + m_matchDistance) &&
				(aFindNewAdd + m_matchDistance >=data.m_matchBegin))
			{
#ifdef _DEBUG
				DBG_Printf(DBGMCH,
					"Match (%d,%d) Col (%2d,%d,%d) Base (%d) %s \n",
					aFindOrgAdd, aFindNewAdd, index,data.m_lastMatchOrg, data.m_lastMatchNew, aBaseNew,
					(data.m_lastMatchNew == aFindNewAdd)?"Best":"   ") ; 
#endif

				data.m_matchCount ++ ; 
				if (aFindNewAdd >data.m_lastMatchNew)
				{ 
					data.m_lastMatchNew = aFindNewAdd ;
					data.m_lastMatchOrg = aFindOrgAdd ;
				}
				else if (aFindNewAdd < data.m_matchBegin)
				{
					data.m_matchBegin = aFindNewAdd ;
				}

				return 2 ;  
			}

			/* get oldest match */
			if (data.m_lastMatchNew < aBaseNew) 
			{
				if ((firstFree == -1) || (m_matchData[firstFree].m_matchCount!= 0 && data.m_lastMatchNew < m_matchData[firstFree].m_lastMatchNew))
				{
					next = firstFree ;
					firstFree = index ;
				}
				else if (next == -1)
				{
					next = index ;
				}
			}
		}
	}

	/* override an old entry if there's no more place */
	if (firstFree == -1)
	{
#ifdef _DEBUG
		DBG_Printf(DBGMCH,  
			"Mch (%d, %d) Ful\n", aFindOrgAdd, aFindNewAdd) ;
#endif
		return 0 ;
	}
	else 
	{ 
		MatchData &newdata=m_matchData[firstFree];

		newdata.m_lastMatchOrg = aFindOrgAdd ;
		newdata.m_lastMatchNew = aFindNewAdd ;
		newdata.m_matchBegin = aFindNewAdd ;
		newdata.m_lastMatchTest = 0 ;
		newdata.m_lastMatchFind = 0 ;
		newdata.m_MatchCmp = 0 ;
		newdata.m_matchCount = 1 ;

#ifdef _DEBUG
		DBG_Printf(DBGMCH,
			"Mch (%d,%d) New (%2d,%d,%d) Base (%d) %s\n",
			aFindOrgAdd, 
			aFindNewAdd, 
			firstFree,
			newdata.m_lastMatchOrg,
			newdata.m_lastMatchNew,
			aBaseNew,
			(newdata.m_lastMatchNew == aFindNewAdd)?"Best":"   ") ;
#endif

		if (next != -1)
			return 1 ; /* added, still room for new entries */
		else
			return 0 ; /* added, table if now full */
	}
}



/* -----------------------------------------------------------------------------
* ufMchFre: check if there is free space in the table of matches
* ---------------------------------------------------------------------------*/
bool MatchMap::haveFree ( int aBaseNew )
{ 
	int Index ;
	for (Index=0; Index < m_matchMax ; Index++)
	{
		MatchData &data=m_matchData[Index];
		if (data.m_matchCount == 0 || data.m_lastMatchNew< aBaseNew)
			return true ;
	}
	return false ;
}


/* -----------------------------------------------------------------------------
* Auxiliary function: ufMchBst
* Get the best match from the array of matches
* ---------------------------------------------------------------------------*/
int MatchMap::Best (
					FileStream   *aFileOrg,      /* files                */
					FileStream   *aFileNew,
					int alBseOrg,       /* base positions       */
					int aBaseNew,
					int &alBstOrg,      /* best position found  */
					int &alBstNew
					)
{
	int index ;           /* investigated  match.                         */
	int best=-1 ;        /* selected best match.                         */
	int check ;           /* check match to investigate                   */
	int next ;           /* first match to investigate                   */
	int compareStatus ;           /* compare status                               */

	int testNew ;      /* test positions                               */
	int llTstOrg ;
	int testBase ;
	int testNext ;

	bool lbStr =false;

	


	for (index=0, next=0; index < m_matchMax && next >= 0; index ++)
	{  
		/* find first valuable entry */
		if (best == -1)
			next = -1 ;
		else
		{
			next    = best ;
			testNext = alBstNew + 1 ;
		}

		for (check = index; check < m_matchMax && m_matchData[check].m_matchCount> 0; check ++)
		{

			MatchData& checkData=  m_matchData[check];
			if (checkData.m_matchCount > 0)
			{
				/* calculate the test position */
				if (checkData.m_lastMatchNew + m_matchDistance >= aBaseNew) 
				{ 
					if (checkData.m_matchBegin<= aBaseNew + m_matchDistance - 24)
						testNew = aBaseNew ;
					else 
						testNew =checkData. m_matchBegin - m_matchDistance + 24 ;
				}
				else
					testNew = 0;

				/* is this entry the first valuable? */
				if (testNew > 0)
				{ 
					if (next == -1) 
					{ 
						testNext = testNew ;
						next    = check ;
					} 
					else if (testNew < testNext) 
					{ 
						testNext = testNew ;
						next    = check ;
					} 
				}
			}
		}

		MatchData& data=m_matchData[index];
		/* valuable entry found? */
		if (next <= best)
			next = -1 ; /* exit from loop */
		else
		{ 
			lbStr = false ;
			compareStatus = 0 ;

			/* swap next entry to current position */
			if (next != index)
			{ 
				MatchData llSwp=m_matchData[index];
				m_matchData[index]=m_matchData[next] ;
				m_matchData[next] =llSwp;

			}

			/* calculate corresponding original position */
			testNew = testNext ;
			testBase = testNext ;
			if (compareStatus == 0)
			{ 
				if (data.m_lastMatchOrg + testNew >= data.m_lastMatchNew)
					llTstOrg = data.m_lastMatchOrg + testNew - data.m_lastMatchNew ;
				else
					compareStatus = 4 ;
			}

			/* compare */
			if (compareStatus == 0) 
			{
				/* do not compare a same entry twice */
				if (data.m_lastMatchTest > 0 && testNew >= data.m_lastMatchTest && testNew <= data.m_lastMatchFind)
				{
					/* use stored result */
					lbStr = true ;
					compareStatus = data.m_MatchCmp ;
					testNew = data.m_lastMatchFind ;
					llTstOrg = data.m_lastMatchOrg + testNew - data.m_lastMatchNew;
				}
				else
				{ 
					/* find equality */
					compareStatus = FileStream::Compare(aFileOrg, aFileNew, llTstOrg, testNew, m_matchDistance, m_bCmpAll?1:2) ;

					/* soft eof reached, then rely on hash function */
					if (compareStatus == 1 && data.m_matchCount <= 3) 
						compareStatus = 7 ;

					/* remove false matches */
					if (compareStatus == 2 &&data.m_matchCount == 1 && testBase <= data.m_matchBegin && testNew >data.m_matchBegin)
					{
						data.m_matchCount = 0 ;
						compareStatus = 2 ;
						m_hashRepairedCount++ ;
					}

					/* keep the result for later */
					if (compareStatus <= 1)
					{
						data.m_lastMatchTest = testBase ;
						data.m_lastMatchFind = testNew ;
						data.m_MatchCmp = compareStatus ;
					}
				}
			}

			/* evaluate */
			if (compareStatus <= 1)
			{
				if ((best == -1) || (testNew + 4 <= alBstNew) || ((testNew <= alBstNew + 4) && (data.m_matchCount > data.m_matchCount))) 
				{
					alBstNew = testNew ;
					alBstOrg = llTstOrg ;
					best     = index ;
				}
			}

			/* show table */
#ifdef _DEBUG
			DBG_Printf(DBGMCH,
				"Match %2d:%2d:%1d%c%c[%d,%d,%d,%4d,%d] F=%d=%d\n", 
				index, next, compareStatus, 
				(best == index)?'*':
				(compareStatus==0)    ?'=':
				(compareStatus==1)  ?'?':' ', 
				(lbStr)?'!':' ',
				data.m_matchBegin, 
				data.m_lastMatchOrg, 
				data.m_lastMatchNew,
				data.m_matchCount,
				data.m_lastMatchOrg - data.m_lastMatchNew, 
				testBase, testNew) ;
#endif
		} 
	} 

	/* show table */
#ifdef _DEBUG
	for (; index < m_matchMax ; index ++)
	{
		MatchData& data=  m_matchData[index];
		DBG_Printf(DBGMCH,
			"Match %2d:  :   [%d,%d,%d,%4d,%d]\n", 
			index, 
			data.m_matchBegin,data. m_lastMatchOrg,data. m_lastMatchNew, data.m_matchCount,
			data.m_lastMatchOrg - data.m_lastMatchNew) ;
	}
#endif

	if (best == -1)
	{ 
#ifdef _DEBUG
		DBG_Printf(DBGMCH,"Match Error\n") ;		
#endif
		return -1 ;
	}
	else 
		return best ;
}