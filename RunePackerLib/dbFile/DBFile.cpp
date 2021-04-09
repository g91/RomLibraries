#include <windows.h>
#include "DBFile.h"
#include "..\Rune\ZLib\Zlib.h"

#include "..\rune\FilePackage\cFileObj.h"


bool DBHead::Check()
{
	return m_Idenitfy==DEF_ITEMOBJ_FILE_ID;
}
int  DiffObj::Load(cFileObj *obj)
{
	int dataSize;

	obj->Read(&m_newCRC,4);
	obj->Read(&dataSize,4);
	m_data.resize(dataSize);
	if (dataSize>0)
		obj->Read(&m_data[0],dataSize);

	return dataSize;
}

bool CDiffDataBase::Load(cFileObj *obj)
{
	bool ret=true;
	int count;
	obj->Read(m_md5,32);
	m_md5[32]=0;


	obj->Read(&count,4);
	int DBID;
	

	for(int i=0;i<count;i++)
	{
		obj->Read(&DBID,4);
		

		DiffObj *pDiffObj=new DiffObj;

		m_dbMap[DBID]=pDiffObj;

		pDiffObj->Load(obj);

	
	}


	return ret;


}
CDiffDataBase::~CDiffDataBase()
{
	std::map<int ,DiffObj* >::iterator it=m_dbMap.begin();
	for (;it!=m_dbMap.end();++it)
	{
		DiffObj * pObj=it->second;
		delete pObj;

	}

	m_dbMap.clear();


}
CStringDiffDB::~CStringDiffDB()
{
	std::map<std::string ,DiffObj* >::iterator it=m_dbMap.begin();
	for (;it!=m_dbMap.end();++it)
	{
		DiffObj * pObj=it->second;
		delete pObj;

	}

	m_dbMap.clear();
}
CDataBase::CDataBase():m_tail(NULL),m_tailSize(0)
{


}
CDataBase::~CDataBase()
{

	std::map<int ,RowObj* >::iterator it=m_dbMap.begin();
	for (;it!=m_dbMap.end();++it)
	{
		char * pMem=(char *)it->second;
		delete []pMem;

	}

	m_dbMap.clear();
	if (m_tail)
		delete [] m_tail;

	m_tail=NULL;



}
bool CDataBase::Load(cFileObj *obj)
{
	bool ret=true;
	int allSize=obj->GetStreamSize();

	obj->Read(&m_info.m_head,sizeof(DBHead));
	ret=m_info.m_head.Check();
	if (ret)
	{
			

		int size=m_info.m_head.m_Size;
		int count=m_info.m_head.m_Count;
		 m_tailSize=allSize-(sizeof (DBHead)+size*count);


		for (int i=0;i<count;i++)
		{

			RowObj * col=(RowObj *) new char [size];

			obj->Read(col,size);
			std::map<int ,RowObj* >::iterator it=m_dbMap.find(col->DBID);
			if (it==m_dbMap.end())
			{
				m_dbMap[col->DBID]=col;
			}
			else
			{
				delete []col;
			}

		}
		if (m_tailSize>0)
		{

			m_tail=new char [m_tailSize];

			obj->Read(m_tail,m_tailSize);
		}
	}
	return ret;
}
bool  CStringDiffDB:: Load(cFileObj *obj)
{
	bool ret=true;
	int count;
	obj->Read(m_md5,32);
	m_md5[32]=0;


	obj->Read(&count,4);
	char KeyID[65];
	

	for(int i=0;i<count;i++)
	{
		memset(KeyID,0,65);
		obj->Read(KeyID,64);
	

		DiffObj *pDiffObj=new DiffObj;

		m_dbMap[KeyID]=pDiffObj;

		pDiffObj->Load(obj);

	

	}
	return ret;
}

DiffObj::DiffObj()
{
	
	m_newCRC= crc32(0L, NULL, 0);
}

char * trimString(char *pStr)
{

	if (!pStr)
		return NULL;

	int len=strlen(pStr);
	for(int i=len-1;i>0;--i)
	{
		if (pStr[i]==0xa||pStr[i]==0xd||pStr[i]==0x20||pStr[i]=='\t')
			pStr[i]=0;
		else
			break;


	}
	while(1)
	{

		if (pStr[0]==0xa||pStr[0]==0xd||pStr[0]==0x20||pStr[0]=='\t')
			pStr++;
		else
			break;
	}
	return pStr;


}

bool CStringDB::LoadLua(cFileObj *obj)
{
	bool ret=true;

	int dataSize=obj->GetDataSize();

	int stringlen;

	char* pBuff=(char *)obj->OpenStreamMapping();



	
	char *pKey;
	char * pValue;


	pKey=strtok(pBuff,"=");

	
	int size=(pKey-pBuff);
	while(pKey&&(size<dataSize))
	{
		
	

		pKey=trimString(pKey);

	
		pValue=strtok(NULL,"\n\r");
		if (pValue)
		{
			pValue=trimString(pValue);
			
			m_dbMap[pKey]=pValue;
		}
	

		pKey=strtok(NULL,"=");

		size=(pKey-pBuff);
	}
return ret;
}
bool CStringDiffDB::LoadLua(cFileObj *obj)
{

	bool ret=true;
	int count;
	obj->Read(m_md5,32);
	m_md5[32]=0;
	obj->ConvertFormat(COMPRESS_FMT_NORMAL);
	obj->Read(&count,4);
	char KeyID[1024];


	int size=0;
	for(int i=0;i<count;i++)
	{
		memset(KeyID,0,1024);
		obj->Read(&size,4);

		obj->Read(&KeyID,size);


		DiffObj *pDiffObj=new DiffObj;

		m_dbMap[KeyID]=pDiffObj;

		pDiffObj->Load(obj);



	}
	return ret;

}
bool CStringDB::Load(cFileObj *obj)
{
	bool ret=true;

	char temp[128];
	char stringdata[10006];
	int stringlen;

	obj->Seek(0,ruSSM_Begin);

	std::string key;
	std::string value;

	while (obj->Read(temp,64)==64) 
	{

		obj->Read(&stringlen,4);
		if (stringlen>8196)
			stringlen=8196;
		
		obj->Read(stringdata,stringlen);
		stringdata[stringlen]=0;


		key=temp;
		value=stringdata;
		if (stringlen>0)
		{

			std::map<std::string ,std::string >::iterator it=m_dbMap.find(key);
			if (it==m_dbMap.end())
				m_dbMap[key]=value;
			else
				m_dbMap[key]=value;


		}
		
	};
	



	
	
	
	
	return ret;
}