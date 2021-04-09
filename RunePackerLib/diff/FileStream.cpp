#include <windows.h>
#include <stdio.h>
#include "jdiffos.h"
#include "FileStream.h"
#include "debug.h"
/* -----------------------------------------------------------------------------
* FileStream::FileStream: 
* ---------------------------------------------------------------------------*/
 ulong FileStream::g_buffSizes = 256*1024;
 int FileStream::g_fabSeek;//=0;         /* Number of times an fseek operation was performed  */
FileStream::FileStream():m_pFile(NULL),m_pBuff(NULL)
{


}

FileStream::~FileStream()
{

	Close();

}



/* -----------------------------------------------------------------------------
* FileStream::Open: open buffered lookahead file
* ---------------------------------------------------------------------------*/
bool FileStream::Open (  IDataStream *data, char *acFid )
{
	m_FileStreamID = acFid ;
	m_pFile = data ;
	if ( m_pFile == NULL ) 
		return false ; 

	m_pBuff =( unsigned char *) new char [g_buffSizes] ;
	if ( m_pBuff == NULL ) 
		return(false) ;
	m_pBuffMax = m_pBuff + g_buffSizes ;
	m_pInputPos = m_pBuff;
	m_buffSizes = 0;
	m_currentReadPos = 0;
	m_currentPosSeek = 0;
	m_posEof = -1;

#ifdef _DEBUG
	DBG_Printf(DBGBUF 
		, "%s->Open():(buf=%x,max=%x,sze=%d)\n",
		acFid, m_pBuff, m_pBuffMax, g_buffSizes);
#endif

	return true;
}

/* -----------------------------------------------------------------------------
* FileStream::Close: closes buffered lookahead file
* ---------------------------------------------------------------------------*/
void  FileStream::Close ( void )
{
	if (m_pFile)
	{
		m_pFile->close();
	//	delete m_pFile;
		m_pFile=0;
	}

	
	if (m_pBuff)
		delete m_pBuff ;

	m_pBuff=NULL;

}



/* -----------------------------------------------------------------------------
* FileStream::Get: gets one byte from the lookahead file
*
* Read mode consumes bytes: old bytes in the buffer get overwritten.
* Hard look-ahead mode performs unbuffered reading when the buffer is full.
* Soft look-ahead mode returns EOB when the buffer is full.
* ---------------------------------------------------------------------------*/


/* alPos position to read from                */
/* aiTyp  0=read, 1=hard ahead, 2=soft ahead   */
int FileStream::Get ( int& aPos,  int aiTyp ) 	     			
{ 
	int lcInp =0;
	unsigned char *lpDta ;

	/* Eof ? */
	if (m_posEof != -1)
	{
		if (aPos >= m_posEof) 
		{
#ifdef _DEBUG

			DBG_Printf(DBGRED ,
				"EOF=%s->Get(%d,%d) (mem).\n", 
				m_FileStreamID, aPos, aiTyp);
#endif
			return EOF ;
		}
	}

	/* Get data from buffer? */ 
	if ((aPos >= m_lastInputPos - m_buffSizes ) && (aPos < m_lastInputPos))
	{
		lpDta = m_pInputPos - (m_lastInputPos - aPos) ;
		if ( lpDta < m_pBuff )
			lpDta += g_buffSizes ;

		/* store actual read position */
		if (aiTyp == 0)
			m_currentReadPos = aPos + 1 ;

#ifdef _DEBUG

		DBG_Printf(DBGRED, "FileStream::Get(%s,%d,%d)->%2x (mem %x).\n", 
			m_FileStreamID, aPos, aiTyp, *lpDta, lpDta);
#endif

		(aPos)++ ;
		return *lpDta ;
	}

	/* Soft ahead: continue only if getting at input position and buffer is not full */
	if (aiTyp == 2 && (aPos != m_lastInputPos || aPos - m_currentReadPos > g_buffSizes))  
	{
#ifdef _DEBUG
		DBG_Printf(DBGRED ,
			"EOB=%s->Get(%d,%d).\n", 
			m_FileStreamID, aPos, aiTyp);
#endif
		return EOB ;
	}

	/* Seek if file position is not at the correct position */
	if (aPos != m_currentPosSeek) 
	{
#ifdef _DEBUG
		DBG_Printf(DBGBUF , "FileStream::Get: Inner seek %d.\n", m_lastInputPos);
#endif

		g_fabSeek++ ;
		if (m_pFile->seek( aPos, SEEK_SET) == EOF ) 
		{
			DBG_Printf(-1, "Could not reposition on file (inner seek %d)!", m_lastInputPos);
			exit(6);
		}
		m_currentPosSeek = aPos ;
	}

	/* Read data from file */
	int bytes=	m_pFile->read(&lcInp,1);
	 
	if (bytes == 0) 
	{
#ifdef _DEBUG
		DBG_Printf(DBGRED,
			"EOF=%s->Get(%d,%d).\n", 
			m_FileStreamID, aPos, aiTyp);
#endif

		m_posEof = aPos ;
		return EOF ;
	}
	else
		m_currentPosSeek++;

	/* Read mode: reset buffer */
	if (aiTyp == 0) 
	{
		if (aPos != m_lastInputPos) 
		{
			m_buffSizes = 0 ;
			m_lastInputPos = aPos ;
		}

		/* Set read position */
		m_currentReadPos = aPos ;
	}

	/* Store within buffer */
	if (aPos == m_lastInputPos && aPos - m_currentReadPos < g_buffSizes) 
	{
#ifdef _DEBUG
		DBG_Printf(DBGRED,
			"%d=%s->Get(%d,%d) (sto %x).\n", 
			lcInp,m_FileStreamID, aPos, aiTyp,  m_pInputPos);
#endif

		*m_pInputPos = (unsigned char) lcInp;

		m_lastInputPos ++ ;
		m_pInputPos ++ ;
		if ( m_pInputPos == m_pBuffMax )
			m_pInputPos = m_pBuff ;
		if ( m_buffSizes < g_buffSizes   ) 
			m_buffSizes ++ ;
	} 
	else
	{
#ifdef _DEBUG
		DBG_Printf(DBGRED,
			"%2x=%s->Get(%d,%d) (dirty).\n", lcInp,
			m_FileStreamID, aPos, aiTyp );
#endif
	}

	/* Set new read position */
	if (aiTyp == 0) 
		m_currentReadPos ++ ; 
	(aPos)++ ;

	return lcInp ;
} 





/* -----------------------------------------------------------------------------
* FileStream::Compare: compare files at given offsets
*
* Searches at given positions for a run of 24 equal bytes.
* Searching continues for the given length unless soft-reading is specified
* and the end-of-buffer is reached.
*
* Arguments:      asFilOrg    in      first file
*                 asFilNew    in      second file
*                 alPosOrg    in/out  position on first file
*                 alPosNew    in/out  position on second file
*                 aiLen       in      number of bytes to compare
*                 aiSft       in      1=hard read, 2=soft read
*
* Return value:   0 = run of 24 equal bytes found
*                 1 = end-of-buffer reached
*                 2 = no run of equal byes found
* ---------------------------------------------------------------------------*/
int FileStream::Compare( FileStream *asFilOrg,FileStream *asFilNew,int &aPosOrg,int &aPosNew,int aiLen, int aiSft)
{
	int liOrg ;
	int liNew ;
	int liEql=0 ;
	int liRet=0 ;

#ifdef _DEBUG

	DBG_Printf(DBGCMP,
		"Compare ("P8zd","P8zd",%4d,%d): ",
		aPosOrg, aPosNew, aiLen, aiSft) ;
#endif 

	/* Compare bytes */
	for (; aiLen > 24 && liRet == 0 && liEql < 24; aiLen--) 
	{
		liOrg = asFilOrg->Get(aPosOrg, aiSft) ;
		liNew = asFilNew->Get( aPosNew, aiSft) ;

		if (liOrg <= EOF || liNew <= EOF) 
			liRet = 1 ;
		else if (liOrg != liNew) 
			liEql = 0 ;
		else
			liEql ++ ;
	}

	/* Compare last 24 bytes */
	for (; aiLen > 0 && liRet == 0 && liEql < 24; aiLen--) 
	{
		liOrg = asFilOrg->Get(aPosOrg, aiSft) ;
		liNew = asFilOrg->Get( aPosNew, aiSft) ;

		if (liOrg <= EOF || liNew <= EOF) 
			liRet = 1 ;
		else if (liOrg != liNew) 
			liRet = 2 ;
		else
			liEql ++ ;
	}

#ifdef _DEBUG
	DBG_Printf(DBGCMP,
		"%d %d %2d %s (%c)%3o == (%c)%3o\n", 
		aPosOrg - liEql, aPosNew - liEql, liEql,  
		(liRet==0)?"OK!":(liRet==1)?"EOF":"NOK",
		(liOrg>=32 && liOrg <= 127)?liOrg:' ',liOrg,  
		(liNew>=32 && liNew <= 127)?liNew:' ',liNew);
#endif

	switch (liRet) 
	{
	case 0: 
		aPosOrg = aPosOrg - liEql ;
		aPosNew = aPosNew - liEql ;
		return 0 ; /* equality found */
	case 1:
		if (liOrg == EOF || liNew == EOF)
			return 2 ; /* surely different (hard eof reached) */
		else
		{ 
			aPosOrg = aPosOrg + aiLen ;
			aPosNew = aPosNew + aiLen ;
			return 1 ; /* may be different (soft eof reached) */
		} 
	case 2: 
		return 2 ; /* surely different */
	}

	return -1 ; /* should not happen, only to avoid compiler warnings */
}
