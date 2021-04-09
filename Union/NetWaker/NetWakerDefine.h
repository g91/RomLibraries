#pragma once
/************************************
NetWaker ��Ʃw�q��ơ@
Revision By: hsiang
Revised on 2004/10/13 
Comments: 
************************************/
#include "NetWakerPackageDef.h"
#include "MyTypeDef.h"

#pragma unmanaged

#pragma	warning (disable:4200)
#pragma warning (disable:4949)
#include <string>
#include <vector>
#include <set>

using namespace std;

//////////////////////////////////////////////////////////////////////////

#define _DEF_MAX_ROLETNAME_SIZE_	32
#define _DEF_MAX_ACCOUNTNAME_SIZE_  64
#define _DEF_MAX_PASSWORD_SIZE_     256
#define _DEF_MAX_MAC_ADDRESS_SIZE_  256
//#define _DEF_MAX_GSRVNAME_SIZE_     32
#define _DEF_MAX_IPSTR_SIZE_        64
#define _DEF_MAX_TIMESTR_SIZE_      28
#define	_DEF_MAX_ITEM_KEY_SIZE_		32
/*
// �q���ϥΧI���ӫ~�n�D
enum	CheckExchangeItemTypeENUIM
{
	EM_CheckExchangeItemType_ItemUsed	= -2	,	//���~�w�Q�ϥ�
	EM_CheckExchangeItemType_Failed		= -1	,	//���ѡA�L������
	EM_CheckExchangeItemType_GameMoney			,	//�C����
	EM_CheckExchangeItemType_AccountMoney		,	//�b����
	EM_CheckExchangeItemType_Item				,	//���~
};
*/
//-----------------------------------------------------------------------------------
//�n�J���ѦC�|
//-----------------------------------------------------------------------------------
enum	SYSLogFailedENUM
{	
    EM_NetLogFailed_OK					= 0 ,   //�S���~
    EM_NetLogFailed_PasswordError		,		//�K�X���~
    EM_NetLogFailed_ConnectFail			,		//�s�u���� 
    EM_NetLogFailed_ConnectError		,		//�s����m���~ 
    EM_NetLogFailed_NotFindError		,		//�䤣��b��
	EM_NetLogFailed_ProxyNotExist		,		//Proxy���s�b
    EM_NetLogFailed_SwitchDisconnect    ,		//���������U�λP Switch �_�u
	EM_NetLogFailed_ProxyLoginFailed    ,		//���������U�λP Proxy �s�u����
	EM_NetLogFailed_AccountExist		,		//�b�����еn�J
	EM_NetLogFailed_UnknowError		    ,		//�Ȯɥ��W�������~

	//�䥦
	EM_NetLogFailed_LoginTwice			,		//�n�J�⦸
	EM_NetLogFailed_LoginByOtherSrv		,		//����n�J��L�@��
	EM_NetLogFailed_SrvDisconnect		,		//�ȮɨS��
	EM_NetLogFailed_NotEnoughPoint		,		//�ȮɨS��
	EM_NetLogFailed_NotCorrectChargType	,		//�ȮɨS��
	EM_NetLogFailed_Suspended			,		//�b��ĵ�i
	EM_NetLogFailed_Banned				,		//�b���Q��
	EM_NetLogFailed_AccountStrError		,		//�b���ۦ��J�����D
	EM_NetLogFailed_SwitchError			,		//Switch�����D
	EM_NetLogFailed_FreezeType_Self		,		//�ۧڧN��
	EM_NetLogFailed_FreezeType_Official	,		//�x��N��
	EM_NetLogFailed_FreezeType1_Other	,		//�䥦�N��
	EM_NetLogFailed_llegalMacAddress	,		//llegalMacAddress
	EM_NetLogFailed_CommLock			,		//�q�T����w��

    EM_SYSLogFailedEM_FORCE_DWORD		= 0x7fffffff ,
};
//-----------------------------------------------------------------------------------
namespace   NetWakerPackage
{
    
    #define     _Sys_Package_Number_        0xfffffffe 

    #define     _Max_Package_Size_		    0xfffff	//�̤j�ʥ]�j�p
    #define     _Max_Cli_Package_Size_	    0x20000	//�̤j�ʥ]�j�p
    #define     _Max_Proxy_Count_           256
    #define     _Max_GSrv_Count_            256
    #define     _Max_Cli_Count_             65535
//    #define     _Sys_Max_CallbackCount_     256

    #define     _Max_GSrv_Name_Size_        32      //�̤jGSrv �W�W����
    #define     _Max_CliAccount_Name_Size_  64      //�̤jClient Account �W�W����
    #define     _Max_CliPassword_Name_Size_ 256      //�̤jClient Password �W�W����
	#define     _Max_CliMac_Address_Size_	256     //�̤jClient Mac Address ����

    //-----------------------------------------------------------------------------------
    //���A�C�|
    //-----------------------------------------------------------------------------------
    enum	SYSNetStatENUM
    {	
        EM_NetStat_Unknow     = 0       ,   
        EM_NetStat_Connect	= 10	    ,	//�s�u���A
		EM_NetStat_Login                ,   //�n�J��	
		EM_NetStat_Login_CheckAccount   ,   //�n�J���q�� Account �ˬd�b��
		EM_NetStat_LoginSrvOK           ,   //�PSwitch�`���n �]�q���LProxy Addr
        EM_NetStat_ACLoginOK            ,    
        EM_NetStat_OnDestory	    	,	//�ǳƵ����s�u
        EM_NetStat_Relogin  	    	,	//���s�n�J
        EM_SYSNetStatEM_FORCE_DWORD   = 0x7fffffff ,
    };
    //-----------------------------------------------------------------------------------
    //�޲z���ŦC�|
    //-----------------------------------------------------------------------------------
    enum	SYSNetManageENUM
    {	
        EM_Manage_Normal	= 0		,	//�@��
        EM_Manage_GM		= 10	,	//GM
        EM_Manage_GC		= 20	,	//�C���гy��
        EM_SYSNetManageEM_FORCE_DWORD   = 0x7fffffff ,
    };
    //-----------------------------------------------------------------------------------
    //Server ���O�C�|
    //-----------------------------------------------------------------------------------
    enum	SYSGSrvTypeENUM
    {	
        EM_GSrvType_Passive		= 0	,	//�Q�ʫ�
        EM_GSrvType_Active		= 1	,	//�D�ʫ�
        EM_SYSGSrvTypeEM_FORCE_DWORD   = 0x7fffffff ,
    };
    //-----------------------------------------------------------------------------------
    //	GSrv & Cli �ݳ]�w���ƥ�CallBack
    //-----------------------------------------------------------------------------------
    class NewWakerEventListener
    { 
    public:
        //��ܵn�JGateWay Server
        virtual void  OnLogin( string info ) = 0;

        virtual	void  OnLogout( ) = 0;

        //��ܵn�JGateWay Server����
        virtual void  OnLoginFailed(SYSLogFailedENUM) = 0;
		virtual void  OnLoginFailedData(SYSLogFailedENUM, void* data, unsigned long size){}

        //�P�Y�@��Server���߳s�u
        virtual void  OnGSrvConnected( int id , string GSrvName ) = 0;
        //�P�Y�@��Server�_�u
        virtual void  OnGSrvDisconnected( int id ) = 0;
        //����ʥ]
        virtual bool  OnGSrvPacket( int id, int size , void* data ) = 0;	

        //����Client ���ʥ]
        virtual bool  OnCliPacket( int id, int size , void* data ){return false;};	

		//��LWorld�e�Ӫ��ʥ]
		virtual bool  OnOtherWorldPacket( int FromWorldID , int FromNetID , int size , void* data ){ return false;};	

        //�Y�� Client �s�u
        virtual void  OnClientConnected( int id , string accountname ){};
        //�Y�� Client �_�u
        virtual void  OnClientDisconnected( int id ){};
        //Server �ǰe�L�Ӫ��T�� ( Proxy or Switch)
        virtual void  OnServerMsg( char* Msg){};

        virtual void  OnCliReloginByOther( int IPNum ){ };

		virtual void  OnProxyPingLog( int ProxyID , int Time ){};
    };
    
    //-----------------------------------------------------------------------------------
    //�ӷ����c
    //-----------------------------------------------------------------------------------
    struct SYSNetSourceStruct
    {	
        union 
        {
            short  Type;
            struct 
            {
                bool Client  : 1;
                bool GSrv    : 1;
                bool Proxy   : 1;
                bool Switch  : 1;
                bool AccountAgent : 1;
				bool Gateway : 1;
            };
        };
        SYSNetSourceStruct()
        {
            Type = 0;
        }
    };
    //-----------------------------------------------------------------------------------
    struct  PGSysNULL
    {
        PacketCommandEnum Command;

        PGSysNULL()
        {
            Command = EM_NOUSED;
        }
    };
    //---------------------------------------------------------------------------
    //�ʥ]�򥻫�
    struct PacketBase
    {
        PacketCommandEnum	Command;
        char    Data[0];

    };
	//-----------------------------------------------------------------------------------
	// Gateway �� Switch ���s�u���
	struct SwitchInfoStruct
	{
		unsigned int		ID;			//�s�u���X
		unsigned int		WorldID;
		string				IPStr;		//IP String or DNS
		int                 Port;		//��Client�p����Port

		SYSNetStatENUM		Stat;			//���A

		SwitchInfoStruct()	{	Init();	};
		void	Init()
		{
			ID = 0;
			Stat = EM_NetStat_Unknow;
		}		
	};	
    //-----------------------------------------------------------------------------------
    // Switch �� Proxy ���s�u���
    struct ProxyInfoStruct
    {
        unsigned int		ID;				//�s�u���X
		string				CliIPStr;		//IP String or DNS
		string				GSrvIPStr;		//IP String or DNS
        int                 ClientPort;     //��Client�p����Port
        int                 GSrvPort;       //��Server�p����Port

        SYSNetStatENUM		Stat;			//���A

        set<int>			GSrvConnect;	//�P����Server���@�s��
        set<int>			CliConnect;	    //�P����Cliaaaaa���@�s��

        ProxyInfoStruct()	{	Init();	};
        void	Init();
    };	
    //-----------------------------------------------------------------------------------
    // Switch �� GSrv ���s�u���
    struct GSrvInfoStruct
    {
        unsigned int		ID;				//�s�u���X
        unsigned int		IPnum;			//ip number
        SYSGSrvTypeENUM     Type;           //GSrv���Ҧ�

        char                GSrvName[ _Max_GSrv_Name_Size_ ];   

        SYSNetStatENUM		Stat;			//���A

        set<int>			ProxyConnect;	//GSrv �P����Proxy���@�s��
        set<int>			CliConnect;	    //GSrv �P����Cli���@�s��

        GSrvInfoStruct()	{	Init();	};
		void	Init();
    };
    //-----------------------------------------------------------------------------------
	enum ClientNetCloseTypeENUM
	{
		EM_ClientNetCloseType_None			,
		EM_ClientNetCloseType_ChangeWorld	,				//�﨤���@��
		EM_ClientNetCloseType_CloseWindow	,				//��������
		EM_ClientNetCloseType_Relogin		,				//���s�n�J
		EM_ClientNetCloseType_ReturnAccount	,				//��j�n�J�ɭ�(�έ��s�n�J)
		EM_ClientNetCloseType_ServerClose	,				//Server�n�D�_�u(��j�n�J�ɭ�)
		EM_ClientNetCloseType_ServerError	,				//Server�n�D�_�u(�s�u�����D)
		EM_ClientNetCloseType_ServerClose_ReloginKick	,	//Server�n�D�_�u(���Ƶn�J)
		EM_ClientNetCloseType_LeaveBattleWorld,				//���}�Գ�
		EM_ClientNetCloseType_End
	};

    // Switch �� Client ���s�u���
    struct CliInfoStruct
    {
        unsigned int		ID;				//�s�u���X
        unsigned int		IPnum;			//ip number
        int                 ProxyID;        //�p����ProxyID(  �Y�� Switch �惡Proxy�p�u�� NetID )
        int                 SwitchNetID;    //�p����Switch
        
        string              Account;
        string              Password;
		string				MacAddress;

        SYSNetStatENUM		Stat;			//���A

        set<int>			GSrvConnect;	//Cli �P���� GSrv ���s�u

        int                 LastCheckTime;
		int					TempScID;		//�B�z�ȮɱƵ{���()
		int					WaitLoginCount;	//���ݵn�J����
		ClientNetCloseTypeENUM NetCloseType;

		bool				HashedPassword; //�K�X�O�_�w�g�g�LMD5�B�z

        CliInfoStruct()	{	Init();	};
        void	Init();
    };
    //-----------------------------------------------------------------------------------
    // GSrv �� Client ���s�u���
    struct GSrv_CliInfoStruct
    {
        unsigned int		ID;				//�s�u���X
        unsigned int		IPnum;			//ip number
        int                 ProxyID;        //�p����Proxy(  �Y�� Switch �惡Proxy�p�u�� NetID )

        string              Account;

        GSrv_CliInfoStruct()	{	Init();	};
        void	Init();
    };
    //-----------------------------------------------------------------------------------
    //�ʥ]�v����� �P �ӷ����� �ʥ]�j�p����T 

    struct	PGBaseInfo
    {
        SYSNetSourceStruct		Type;			//�ӷ�����
        SYSNetManageENUM	    ManageLV;		//�v��

        unsigned int			MinPGSize;	
        unsigned int			MaxPGSize;	

        PGBaseInfo()
        {
            ManageLV	= EM_Manage_Normal;
			MinPGSize	= 0;
            MaxPGSize	= _Max_Package_Size_;
        };
    };
    //-----------------------------------------------------------------------------------
    //�ֳt�j�M A -> B �P B -> A
    //A �P B ���b 0x10000 �H��   
	#define _Max_Number_    0x10000
    class Map_NetIDtoIDClass
    {    
		std::vector< int > _IDToNetList;
        std::vector< int > _NetToIDList;
		int				   _LastErrID;
    public:
        //�]�w  NetID ���p ID
        bool    Set( int NetID , int ID );
        bool    Cls( int NetID , int ID );
        int     GetID( int NetID );
        int     GetNetID( int ID );
		int		MaxID() { return (int)_IDToNetList.size(); }
		int		MaxNetID() { return (int)_NetToIDList.size(); }
		int		LastErrID(){ return _LastErrID; }
    };
    //---------------------------------------------------------------------------
} 

#pragma managed
