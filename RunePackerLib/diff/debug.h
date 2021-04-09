/*
* DEBUG definitions
*/


void DBG_Printf(int layer,char * fmt, ...);
void DBG_Set(int layer,bool value );
bool DBG_Get(int layer);
#ifdef _DEBUG
#define AreWeHere DBG_Printf(-1, "test\n") ; //fflush(stderr) ;

#define DBGHSH 0  // Debug Hash          -dh
#define DBGAHD 1  // Debug Ahead         -da
#define DBGCMP 2  // Debug Compare       -dc
#define DBGPRG 3  // Debug Progress      -dp
#define DBGBUF 4  // Debug Ahead Buffer  -db
#define DBGAHH 5  // Debug Ahead Hash    -dahh
#define DBGHSK 6  // Debug ufHshNxt      -dk
#define DBGBKT 7  // Debug ufFileStreamSek      -dbkt
#define DBGRED 8  // Debug FileStream::Get      -dred
#define DBGMCH 9  // Debug ufMch...      -dm


#else
#define AreWeHere ;
#endif


