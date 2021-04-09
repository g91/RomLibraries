#ifndef CLASS_FILEPACKAGE_H
#define CLASS_FILEPACKAGE_H

#define FILEPACKAGE_MAGIC		0x46444201					// �ɮ������ˬd�X

#include "cImgFile.h"
#include <vector>
#include <map>
#include <string>
//#include "wcxhead.h"

using namespace std;
class cFilePackage;
struct FileDbIndex
{
	FileType		m_fileType;					// �ɮ�����
	FILETIME		m_fileTime;					// �ɮ׮ɶ�
	HANDLE			m_hFile;					// �ɮ׫���
	bool			m_isFromDB;					// �Ӧ۸�Ʈw
	DWORD				m_offset;					// �ӵ���Ʀb�ɮפ�������
	string          m_Sourcename;
	string          m_shortFile;
	//cFilePackage    *m_other;

	HANDLE m_event;

	// �s�ɮ榡
	CompressFormat	m_comFmt;					// ���Y�榡
};
struct FileDataBaseHeader
{	
	FileType	m_fileType;					// �ɮ�����

	FILETIME	m_fileTime;					// �ɮ׮ɶ�
	DWORD		m_offset;					// �ӵ���Ʀb�ɮפ�������	
};

typedef map<string, FileDbIndex> FileDBList;
class cFilePackage
{
public:
	cFilePackage();
	virtual ~cFilePackage();

	/*
	struct lstr
	{
		operator () (const char* p1, const char* p2) const
		{
			return strcmp(p2, p2) < 0;
		}
	};
	*/

	/**
	* ����Ҧ����
	*/
	void Destroy();
	/**
	* �ɮ׬O�_�s�b���
	* @param filename  �ɮצW��
	*
	* @return true ���\�ɻ��ˬdcFileObj�ɮ�����,�H�θ�ƬO�_�����Y�榡  false ����
	*/
	bool FileExists(const char* filename);


	/**
	* ���o�ɮ׸��
	*
	* @param filename	�ɮצW��
	* @param isRdt		�O�_�� *.rdt �ɮ׸��
	*
	* @return true ���\�ɻ��ˬdcFileObj�ɮ�����,�H�θ�ƬO�_�����Y�榡  false ����
	*/
	cFileObj* GetFileObj(const char *filename, bool isRdt=false);

#if RUSTREAMIO
	/**
	* ���o IRuStream �ɭ����O����,�ϥΧ���� Delete
	*
	* @param keyStr  �ɮצW��
	*
	* @return ���ѶǦ^NULL, ���\�Ǧ^���O����,�b�ϥήɻ��ˬd�ɮ������O�_���ϧ��ɮ�
	*/
	IRuStream *GetIRuStream( const char *  keyStr );
#endif

	/**
	* �ʺA�W�[�ɮצܯ��ު�椺
	*
	* @param fileNameStr  �ɮצW��
	* @param pSpecialData  ���J�ɮװѼƳ]�w
	* @param format  ���Y�榡
	*
	* @return true ���\  false ����
	*/
	bool AddFile( const char * fileNameStr, void *pSpecialData = NULL, CompressFormat format = COMPRESS_FMT_NORMAL );


	bool AddFileEX( const char * fileNameStr, const char * SourceName , CompressFormat format = COMPRESS_FMT_ZLIB );
	//bool AddFile( cFileObj *Obj );
	void merge(cFilePackage *other);


	bool DelFile(const char * cfileNameStr);
	/**
	* ���J��Ʈw�ɮ�
	*
	* @param fileNameStr  �ɮצW��
	*
	* @return true ���\  false ����
	*/
	bool LoadFromFile( const char * fileNameStr );

	void CheckDDSRecudeMips(void );

	/**
	* �g�J��Ʈw�ɮ�
	*
	* @param fileNameStr  �ɮצW��
	*
	* @return ���ѶǦ^ -1��Ʈw���L��� -2�}�ɥ���  ���\�Ǧ^�`�x�s����
	*/
	int SaveToFile(const char * fileNameStr , tProcessDataProc ProcessDataProc,void *data=NULL);
	int SaveToFile_C( const char * fileNameStr , tProcessDataProc ProcessDataProc,void *data=NULL);

	/**
	* �]�w�u�@���|,�u���b���J�ʺA�ɮ׮ɤ~�|�Ψ�
	*/
	void SetDirectory(const char * dirStr );

	/**
	* ���o�u�@���|
	*/
	const char * GetDirectory() { return m_dirStr; }

	/**
	* ���oTABLE���
	*/
	FileDBList& GetTable() { return m_table; }
	UINT getSize();
		void Stop()
		{
			m_cancel=true;
		}
		bool isCancel()
		{
			return m_cancel;
		}
	
	/**
	*/
	//FileDbIndex *GetFirstItem();

	/**
	*/
	//FileDbIndex *GetNextItem();

	/**
	* �W�[�ɮצܯ��ު�椺
	*
	* @param pInfo  �ɮת�浲�c����
	*
	* @return true  ���\  false ����
	*/
	bool AddToTable(const char * filename, FileDbIndex &fileInfo);
protected:

	

	/**
	*/
	FileDbIndex *Find(const char * filename);

	/**
	* ���o�����ɮצW��
	*/
	const char * GetFullNameStr( const char * fileNameStr );


protected:
	bool m_bRecudeMinps;

	bool m_bfirst;
	bool m_cancel;
	char  m_dirStr[MAX_PATH];							// �u�@���|
	FileDBList		m_table;
	vector<HANDLE>					m_fileHandle;
};

#endif //CLASS_FILEPACKAGE_H




