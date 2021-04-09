#ifndef _RECYCLEBIN_
#define _RECYCLEBIN_
#include <vector>
#pragma unmanaged
//---------------------------------------------------------------------------
/**
 * @file recyclebin.h 
 * �B�z����O����t�m�P�^��\n
 * �[�t�T�w���c�O����t�m�P����\n
 * Last update [9/9/2004]
 * @author: hsiangren@runewaker.com.tw
 */
//---------------------------------------------------------------------------


//----------------------------------------------------------------------------------
//�귽�^������
//----------------------------------------------------------------------------------
#define _RecycleBin_MaxSize_		0xffff	///<�̤j�Ŷ��t�m�j�p
#define _RecycleBin_PackageBlock_	100		///<�]�w�t�m�O������Φ��h�֭Ӱ϶�
#define _RecycleBin_MemCheckData	0xccddeeff

using namespace std;

template<class Temp>
class RecycleBin
{
    struct BaseStruct
    {
        int     CheckBegin;
        Temp    Data;
        int     CheckEnd;
        BaseStruct()
        {
            CheckBegin  = NULL;
            CheckEnd    = NULL;
        }
    };
	//��l�Ƴ̤jSize;
	int				_MaxSize;
	//�Ȧs��(�|�i�t�m��)
	vector <Temp *> _UnUsed;
	vector <Temp *> _AllData;

	vector <BaseStruct *> _AllocList;
	
    int             _MemGrowUPSize;
	//�V�t�έn�O����
	void _Alloc();

public:
	///�غc�禡
	///�]�w�귽�̦h�|�n�X��
	///@param MaxSize �]�w�귽�̦h�i�t�m�q
	RecycleBin(int MaxSize = _RecycleBin_MaxSize_);
	~RecycleBin();

	///��Ҧ����귽���񱼡A������l���A
	void	Clear();

    ///�^�ǳѤU���Ψ쪺�O����j�p
	///@return �t�m���ϥΪ�����U��
    int     MemSize()const {return _UnUsed.size();};

	///�t�m�s�O����
	///@return �t�m�쪺�����} 
	Temp*	NewObj();

	///�^���O����
	///@param DelObj �n���񪺪������ 
	void	DeleteObj(Temp *DelObj);

	///�ˬd�Ҧ����O����
	//@return ture OK false �O����X���D
	bool	CheckAllMem();

    ///�]�w�����j�p
    void    SetMemGrowUpSize( int Size ){ _MemGrowUPSize = _Size; };
};
//----------------------------------------------------------------------------------
//
//					Source Code
//
//----------------------------------------------------------------------------------
template<class Temp>
Temp* RecycleBin<Temp>::NewObj()
{
	if(_UnUsed.size()==0)
	{
		_Alloc();
		if(_UnUsed.size()==0)
			return NULL;
	}
	
	
	Temp *Ret=_UnUsed[_UnUsed.size()-1];
	_UnUsed.pop_back();

	return Ret;
}
//----------------------------------------------------------------------------------
template<class Temp>
void RecycleBin<Temp>::DeleteObj(Temp *Obj)
{
	if(Obj==NULL)
		return;

	_UnUsed.push_back(Obj);	
}
//----------------------------------------------------------------------------------
template<class Temp>
RecycleBin<Temp>::RecycleBin(int MaxSize)
{
	_MaxSize=MaxSize;
    _MemGrowUPSize = 100;
}
//----------------------------------------------------------------------------------
template<class Temp>
RecycleBin<Temp>::~RecycleBin()
{
	Clear();

}
//----------------------------------------------------------------------------------
template<class Temp>
void RecycleBin<Temp>::_Alloc()
{
	int i;
	BaseStruct*		_AllocMemory;
	Temp*			_Addr;

	if(_RecycleBin_PackageBlock_ <= _AllocList.size())
		return;
	
    //_AllocMemory = new BaseStruct[ ( sizeof(Temp) + 8 ) * ( _MaxSize/_RecycleBin_PackageBlock_+1 ) ];
    _AllocMemory = new BaseStruct[ _MemGrowUPSize ];

	_AllocList.push_back(_AllocMemory);

	//for(i=0;i<=_MaxSize/_RecycleBin_PackageBlock_;i++)
    for(i=0;i<_MemGrowUPSize;i++)
	{
		_Addr = &_AllocMemory[i].Data;
		_UnUsed.push_back( _Addr );
		_AllData.push_back( _Addr );
	}

}
//----------------------------------------------------------------------------------
template<class Temp>
void RecycleBin<Temp>::Clear()
{
	int i;
	for(i=0;i<(int)_AllocList.size();i++)
		delete[] (_AllocList[i]);

	_UnUsed.clear();
	_AllocList.clear();
	_AllData.clear();

}
//----------------------------------------------------------------------------------	
///�ˬd�Ҧ����O����
template<class Temp>
bool RecycleBin<Temp>::CheckAllMem()
{
	BaseStruct*		Data;
	int		i;
	for( i = 0 ; i < (int)_AllData.size(); i++ )
	{
		Data = (BaseStruct*)( ((char*)(_AllData[i])) - 4 );
        if(     Data->CheckBegin != _RecycleBin_MemCheckData 
            ||  Data->CheckEnd   != _RecycleBin_MemCheckData )
            return false;
	}

	return true;
}
#pragma managed
#endif