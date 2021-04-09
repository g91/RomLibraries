
#pragma once


#define INTERFACE_IDataStream(T) \
	public:\
	virtual bool open(const char *name,int opFlag)##T\
	virtual void close(void)##T\
	virtual int len(void)##T\
	virtual int tell(void)##T\
	virtual int read(void* buff,int count)##T\
	virtual int write(void* buff,int count)##T\
	virtual int seek(int offset,int org)##T 


#define BASE_IDataStream INTERFACE_IDataStream(=0;)
#define DERIVED_IDataStream INTERFACE_IDataStream(;)

struct IDataStream
{
	BASE_IDataStream;

};
/*******************************************************************************
* Look-ahead buffered file abstraction (FAB = File Ahead Buffer)
*
* A look-ahead buffered file is a file with two read modes: the "normal" read
* mode and the look-ahead mode.
*
* In read mode, data is "consumed" so old data is removed from the buffer.
* In look-ahead mode, non-read data in the buffer will not be removed.
*
* FileStream::Open      Open  file and allocate read-ahead buffer.
* FileStream::Close      Close file and deallocate read-ahead buffer.
* FileStream::Get      Read one byte from file.
* FileStream::Compare      Compare and find equal blocks within two files.
*
*******************************************************************************/
struct FileStream
{ 
	char *m_FileStreamID;          /* file id (for debugging)                      */
	IDataStream *m_pFile;          /* file handle                                  */
	unsigned char *m_pBuff; /* read-ahead buffer                            */
	unsigned char *m_pBuffMax; /* read-ahead buffer end                        */
	unsigned char *m_pInputPos; /* current input position (null=out of buf)     */
	int m_currentReadPos;       /* current file position                        */
	int m_lastInputPos;       /* last position in buffer                      */
	int m_currentPosSeek;       /* current position in file                     */
	int m_posEof;       /* last    position in file                     */
	long m_buffSizes;        /* number of bytes in buffer                    */
	//

	FileStream();
	virtual ~FileStream();

	bool Open (  IDataStream *data, char *acFid );
	int Get ( int &aPos,  int aiTyp ) 	;
	void Close ( void );

	static int Compare( FileStream *asFilOrg,FileStream *asFilNew,int &aPosOrg,int &aPosNew,int aLen, int aSft);

	static ulong g_buffSizes ;//= 256*1024; /* File lookahead buffer size                      */

	static int g_fabSeek;//=0;         /* Number of times an fseek operation was performed  */
}  ;
