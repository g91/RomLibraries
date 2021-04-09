#pragma once
#include <map>
#include <vector>
#include <string>
#define DEF_ITEMOBJ_FILE_ID 0x6396

class cFileObj;
struct DBHead
{
	char    m_FileInfoStr[ 128 ];		//檔案資訊
	long    m_Idenitfy;				//識別字
	long    m_Count;//幾筆
	long    m_Size;//每筆大小


	DBHead( )
	{
		m_Idenitfy = DEF_ITEMOBJ_FILE_ID;
		strcpy( m_FileInfoStr , "Item Obj File" );
	};

	bool Check(); 

};


struct DBInfo
{
	DBHead m_head;

};
struct RowObj
{
	int DBID;
	char data[1];
};
struct CStringLua
{
	struct ValueData
	{
		const char *pValue;
		const char *pComment;

	};
	BYTE *m_Buff;

	bool Load(cFileObj *obj); 

	std::map<std::string,ValueData> m_dbMap;
};
struct CStringDB 
{
	bool Load(cFileObj *obj); 
	bool LoadLua(cFileObj *obj); 
	std::map<std::string ,std::string > m_dbMap;
};
struct CDataBase 
{

	CDataBase();
	~CDataBase();
	DBInfo m_info;

	bool Load(cFileObj *obj); 
	std::map<int ,RowObj* > m_dbMap;

	int m_tailSize;
	char *m_tail;
};


struct DiffObj
{
	DiffObj();
	
	int m_newCRC;
	std::vector<char> m_data;
	int  Load(cFileObj *obj);
};
struct CDiffDataBase
{

	char m_md5[128];

	std::map<int ,DiffObj* > m_dbMap;
	bool Load(cFileObj *obj);
	~CDiffDataBase();
};

struct CStringDiffDB
{

	char m_md5[128];

	std::map<std::string ,DiffObj* > m_dbMap;
	bool Load(cFileObj *obj); 
	bool LoadLua(cFileObj *obj); 


	~CStringDiffDB();
};