#pragma once 



/*******************************************************************************
* Matching table functions
*
* The matching table contains a series of possibly matching regions between
* the two files. 
* 
* Because of the statistical nature of the hash table, we're not sure that the
* best solution will be found first. Consider, for example, that the samplerate 
* is at 10% on files of about 10Mb and equal regions exist at positions 1000-2000 
* and 1500-1500. Because 10% of 10Mb means one sample every 1024 bytes, 
* it can happen that the 1000-2000 region is only discovered at positions 2000-3000 
* (1000 bytes later). If the 1500-1500 region gets found first at say 1700-1700, 
* and if we would not optimize the found solutions, then 500 equal bytes would 
* get lost.
*
* Therefore we first memorize a number of matching postions found with the hashtable, 
* optimize them (look 1024 bytes back) and then select the first matching solution.
*
* MatchMap::Initial      Initializes the matching table
* MatchMap::Add      Adds a potential solution to the mach table
* MatchMap::Free      Checks if there is room for new solutions
* MatchMap::Best      Optimizes the matches and returns the best one
* 
*******************************************************************************/
struct FileStream;

struct MatchData
{
	int m_matchCount;/* number of colliding matches                       */
	int m_matchBegin;/* first found match                                 */ 
	int m_lastMatchOrg; /* last  found match (original file position)        */
	int m_lastMatchNew; /* last  found match (new      file position)        */
	int m_lastMatchTest; /* result of last find for this entry                */
	int m_lastMatchFind;   
	int m_MatchCmp;  
};
struct MatchMap
{/*
	int m_matchCount[MCH_MAX];   

	int m_matchBegin[MCH_MAX];   
	int m_lastMatchOrg[MCH_MAX];  
	int m_lastMatchNew[MCH_MAX];  

	int m_lastMatchTest[MCH_MAX];  
	int m_lastMatchFind[MCH_MAX];   
	int m_MatchCmp[MCH_MAX];  
	*/



	MatchMap(); 

void SetMatchRange(int min,int max);
	void Initial (void);
	/* match to add               */
	int Add ( FileStream   *asFilOrg, FileStream   *asFilNew, int alFndOrgAdd, int alFndNewAdd, int alBseNew);   

	bool haveFree ( int alBseNew );

	int Best (FileStream   *asFilOrg,  FileStream   *asFilNew,   	int alBseOrg,int alBseNew,	int &alBstOrg,  int &alBstNew);


	int m_matchMax;
	int m_matchMin;
	int m_matchDistance;  /* Max distance between equals in same match.      */
	MatchData m_matchData[MCH_MAX];
	
	 bool m_bCmpAll;
	 int m_hashRepairedCount;
};
