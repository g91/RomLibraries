#pragma once
#include "malloc/MyAlloc.h"
#include "ReaderClass.h"

namespace DTF
{
	class ReaderExClass;

	struct ChildReaderStruct
	{
		int	Address;
		ReaderExClass* Reader;
		int	Group;

		char* GetAddress( void* point )
		{
			return *(char**)((char*)point+Address);
		}

		ChildReaderStruct()
		{
			Group = -1;
		}
	};

	class ReaderExClass : public ReaderClass 
	{
	protected:
		map< string , ChildReaderStruct >	_ChildReader;
		char*								_InitData;
		int									_Size;
	public:
		ReaderExClass( void* data , int size );
		~ReaderExClass( );

		//���o�зǸ��
		char* StdData(){ return _InitData; }

		map< string , ChildReaderStruct >&	ChildReader() { return _ChildReader;};
		//��Ҧ����Ъ��O���鳣�t�m�X�ӡA
		//�s�W strNot�A����string���Φۤv���͡A�a�~�����O����
		void	AllocAllMem( void* obj , bool strNot = false );
		//�ƻs����A�`�N�n�ƨ����obj�A���t�����Ф����new�X�A�|�۰�new�X�ӡC
		void	CopyAllMem( void* obj , void *obj1 , bool isnew = true );
		void	CopyAllMem( void* obj , void *obj1 , int startsize , int endsize , bool isnew = true );
		//�����AllocAllMem�t�m�X���O����
		void	FreeAllMem( void* obj , bool ischild = true );

		void	SetReader( const char* name , void* dataPoint , ReaderExClass* reader , int group = -1 );
		int		Size();

		double	GetValue_Double( void* obj ,const char* valueName );

		template< class ValueTemp >
		bool	GetValue( void* obj ,const char*  valueName , ValueTemp& ret );
		template< class ValueTemp >
		bool	GetPointer( void* obj ,const char*  valueName , ValueTemp*& ret );
		bool	GetType( const char* valueName , PointTypeENUM& type );
		template< class ValueTemp >
		bool	SetValue( void* obj ,const char*  valueName , ValueTemp value );
		template< class ValueTemp >
		bool	SetValueEx( const char*  valueMem , ValueTemp value );
		template< class ValueTemp >
		bool	SetMemory( void* obj ,const char*  valueName , ValueTemp* value , int size );
		bool	GetSize(  const char* valueName , int& size );

		bool	SetString( void* obj ,const char*  valueName , const char* str , int size = -1 );
		bool	SetString( void* obj ,const char*  valueName , const wchar_t* wstr , int sizse = -1 );
		bool	SetStringEx( const char**  valueMem , const char* str );
		bool	SetStringEx( const char**  valueMem , const wchar_t* wstr );
		

		//��Ҧ����вM����0
		void	ClearAllPoint();
		//�t�lreader���`�Xsize
		void		TotalSize( short &size );
	};

	//////////////////////////////////////////////////////////////////////////
	template< class ValueTemp >
	bool	ReaderExClass::GetValue( void* obj ,const char*  valueName , ValueTemp& ret )
	{
//		std::transform( valueName.begin(), valueName.end(), valueName.begin(), ::tolower );

		if( ReaderClass::GetValue( obj , valueName , ret) )
			return true;

		map< string , ChildReaderStruct >::iterator iter;
		for( iter = _ChildReader.begin() ; iter != _ChildReader.end() ; iter++ )
		{
			ChildReaderStruct& info = iter->second;
			if( info.Reader->GetValue( info.GetAddress( obj ) , valueName , ret ) )
				return true;
		}

		return false;
	};
	//////////////////////////////////////////////////////////////////////////
	template< class ValueTemp >
	bool	ReaderExClass::GetPointer( void* obj ,const char*  valueName , ValueTemp *&ret )
	{
		//		std::transform( valueName.begin(), valueName.end(), valueName.begin(), ::tolower );

		if( ReaderClass::GetPointer( obj , valueName , ret) )
			return true;

		map< string , ChildReaderStruct >::iterator iter;
		for( iter = _ChildReader.begin() ; iter != _ChildReader.end() ; iter++ )
		{
			ChildReaderStruct& info = iter->second;
			if( info.Reader->GetPointer( info.GetAddress( obj ) , valueName , ret ) )
				return true;
		}

		return false;
	};
	//////////////////////////////////////////////////////////////////////////
	template< class ValueTemp >
	bool	ReaderExClass::SetValue( void* obj ,const char*  valueName , ValueTemp value )
	{
//		std::transform( valueName.begin(), valueName.end(), valueName.begin(), ::tolower );

		if( ReaderClass::SetValue( obj , valueName , value) )
			return true;

		map< string , ChildReaderStruct >::iterator iter;
		for( iter = _ChildReader.begin() ; iter != _ChildReader.end() ; iter++ )
		{
			ChildReaderStruct& info = iter->second;
			if( info.Reader->SetValue( info.GetAddress( obj ) , valueName , value ) )
				return true;
		}

		return false;
	};

	//////////////////////////////////////////////////////////////////////////
	template< class ValueTemp >
	bool	ReaderExClass::SetValueEx( const char*  valueMem , ValueTemp value )
	{
		if( value == NULL )
			return false;

		if( ReaderClass::SetValueEx( valueMem , value) )
			return true;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	template< class ValueTemp >
	bool	ReaderExClass::SetMemory( void* obj ,const char*  valueName , ValueTemp* value , int size )
	{
		//		std::transform( valueName.begin(), valueName.end(), valueName.begin(), ::tolower );

		if( ReaderClass::SetMemory( obj , valueName , value , size )  )
			return true;

		map< string , ChildReaderStruct >::iterator iter;
		for( iter = _ChildReader.begin() ; iter != _ChildReader.end() ; iter++ )
		{
			ChildReaderStruct& info = iter->second;
			if( info.Reader->SetMemory( info.GetAddress( obj ) , valueName , value , size )  )
				return true;
		}

		return false;
	};
};
