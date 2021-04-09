#pragma once
/************************************
NetWaker 資料定義資料　
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
// 通知使用兌換商品要求
enum	CheckExchangeItemTypeENUIM
{
	EM_CheckExchangeItemType_ItemUsed	= -2	,	//物品已被使用
	EM_CheckExchangeItemType_Failed		= -1	,	//失敗，無此物件
	EM_CheckExchangeItemType_GameMoney			,	//遊戲幣
	EM_CheckExchangeItemType_AccountMoney		,	//帳號幣
	EM_CheckExchangeItemType_Item				,	//物品
};
*/
//-----------------------------------------------------------------------------------
//登入失敗列舉
//-----------------------------------------------------------------------------------
enum	SYSLogFailedENUM
{	
    EM_NetLogFailed_OK					= 0 ,   //沒錯誤
    EM_NetLogFailed_PasswordError		,		//密碼錯誤
    EM_NetLogFailed_ConnectFail			,		//連線不成 
    EM_NetLogFailed_ConnectError		,		//連結位置錯誤 
    EM_NetLogFailed_NotFindError		,		//找不到帳號
	EM_NetLogFailed_ProxyNotExist		,		//Proxy不存在
    EM_NetLogFailed_SwitchDisconnect    ,		//未完成註冊及與 Switch 斷線
	EM_NetLogFailed_ProxyLoginFailed    ,		//未完成註冊及與 Proxy 連線失敗
	EM_NetLogFailed_AccountExist		,		//帳號重覆登入
	EM_NetLogFailed_UnknowError		    ,		//暫時未規類的錯誤

	//其它
	EM_NetLogFailed_LoginTwice			,		//登入兩次
	EM_NetLogFailed_LoginByOtherSrv		,		//角色登入其他世界
	EM_NetLogFailed_SrvDisconnect		,		//暫時沒用
	EM_NetLogFailed_NotEnoughPoint		,		//暫時沒用
	EM_NetLogFailed_NotCorrectChargType	,		//暫時沒用
	EM_NetLogFailed_Suspended			,		//帳號警告
	EM_NetLogFailed_Banned				,		//帳號被踢
	EM_NetLogFailed_AccountStrError		,		//帳號自串輸入有問題
	EM_NetLogFailed_SwitchError			,		//Switch有問題
	EM_NetLogFailed_FreezeType_Self		,		//自我冷凍
	EM_NetLogFailed_FreezeType_Official	,		//官方冷凍
	EM_NetLogFailed_FreezeType1_Other	,		//其它冷凍
	EM_NetLogFailed_llegalMacAddress	,		//llegalMacAddress
	EM_NetLogFailed_CommLock			,		//通訊鎖鎖定中

    EM_SYSLogFailedEM_FORCE_DWORD		= 0x7fffffff ,
};
//-----------------------------------------------------------------------------------
namespace   NetWakerPackage
{
    
    #define     _Sys_Package_Number_        0xfffffffe 

    #define     _Max_Package_Size_		    0xfffff	//最大封包大小
    #define     _Max_Cli_Package_Size_	    0x20000	//最大封包大小
    #define     _Max_Proxy_Count_           256
    #define     _Max_GSrv_Count_            256
    #define     _Max_Cli_Count_             65535
//    #define     _Sys_Max_CallbackCount_     256

    #define     _Max_GSrv_Name_Size_        32      //最大GSrv 名偁長度
    #define     _Max_CliAccount_Name_Size_  64      //最大Client Account 名偁長度
    #define     _Max_CliPassword_Name_Size_ 256      //最大Client Password 名偁長度
	#define     _Max_CliMac_Address_Size_	256     //最大Client Mac Address 長度

    //-----------------------------------------------------------------------------------
    //狀態列舉
    //-----------------------------------------------------------------------------------
    enum	SYSNetStatENUM
    {	
        EM_NetStat_Unknow     = 0       ,   
        EM_NetStat_Connect	= 10	    ,	//連線狀態
		EM_NetStat_Login                ,   //登入中	
		EM_NetStat_Login_CheckAccount   ,   //登入中通知 Account 檢查帳號
		EM_NetStat_LoginSrvOK           ,   //與Switch注測好 也通知過Proxy Addr
        EM_NetStat_ACLoginOK            ,    
        EM_NetStat_OnDestory	    	,	//準備結束連線
        EM_NetStat_Relogin  	    	,	//重新登入
        EM_SYSNetStatEM_FORCE_DWORD   = 0x7fffffff ,
    };
    //-----------------------------------------------------------------------------------
    //管理等級列舉
    //-----------------------------------------------------------------------------------
    enum	SYSNetManageENUM
    {	
        EM_Manage_Normal	= 0		,	//一般
        EM_Manage_GM		= 10	,	//GM
        EM_Manage_GC		= 20	,	//遊戲創造者
        EM_SYSNetManageEM_FORCE_DWORD   = 0x7fffffff ,
    };
    //-----------------------------------------------------------------------------------
    //Server 型別列舉
    //-----------------------------------------------------------------------------------
    enum	SYSGSrvTypeENUM
    {	
        EM_GSrvType_Passive		= 0	,	//被動型
        EM_GSrvType_Active		= 1	,	//主動型
        EM_SYSGSrvTypeEM_FORCE_DWORD   = 0x7fffffff ,
    };
    //-----------------------------------------------------------------------------------
    //	GSrv & Cli 端設定的事件CallBack
    //-----------------------------------------------------------------------------------
    class NewWakerEventListener
    { 
    public:
        //表示登入GateWay Server
        virtual void  OnLogin( string info ) = 0;

        virtual	void  OnLogout( ) = 0;

        //表示登入GateWay Server失敗
        virtual void  OnLoginFailed(SYSLogFailedENUM) = 0;
		virtual void  OnLoginFailedData(SYSLogFailedENUM, void* data, unsigned long size){}

        //與某一個Server成立連線
        virtual void  OnGSrvConnected( int id , string GSrvName ) = 0;
        //與某一個Server斷線
        virtual void  OnGSrvDisconnected( int id ) = 0;
        //收到封包
        virtual bool  OnGSrvPacket( int id, int size , void* data ) = 0;	

        //收到Client 的封包
        virtual bool  OnCliPacket( int id, int size , void* data ){return false;};	

		//其他World送來的封包
		virtual bool  OnOtherWorldPacket( int FromWorldID , int FromNetID , int size , void* data ){ return false;};	

        //某個 Client 連線
        virtual void  OnClientConnected( int id , string accountname ){};
        //某個 Client 斷線
        virtual void  OnClientDisconnected( int id ){};
        //Server 傳送過來的訊息 ( Proxy or Switch)
        virtual void  OnServerMsg( char* Msg){};

        virtual void  OnCliReloginByOther( int IPNum ){ };

		virtual void  OnProxyPingLog( int ProxyID , int Time ){};
    };
    
    //-----------------------------------------------------------------------------------
    //來源結構
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
    //封包基本型
    struct PacketBase
    {
        PacketCommandEnum	Command;
        char    Data[0];

    };
	//-----------------------------------------------------------------------------------
	// Gateway 內 Switch 的連線資料
	struct SwitchInfoStruct
	{
		unsigned int		ID;			//連線號碼
		unsigned int		WorldID;
		string				IPStr;		//IP String or DNS
		int                 Port;		//給Client聯結的Port

		SYSNetStatENUM		Stat;			//狀態

		SwitchInfoStruct()	{	Init();	};
		void	Init()
		{
			ID = 0;
			Stat = EM_NetStat_Unknow;
		}		
	};	
    //-----------------------------------------------------------------------------------
    // Switch 內 Proxy 的連線資料
    struct ProxyInfoStruct
    {
        unsigned int		ID;				//連線號碼
		string				CliIPStr;		//IP String or DNS
		string				GSrvIPStr;		//IP String or DNS
        int                 ClientPort;     //給Client聯結的Port
        int                 GSrvPort;       //給Server聯結的Port

        SYSNetStatENUM		Stat;			//狀態

        set<int>			GSrvConnect;	//與哪些Server有作連結
        set<int>			CliConnect;	    //與哪些Cliaaaaa有作連結

        ProxyInfoStruct()	{	Init();	};
        void	Init();
    };	
    //-----------------------------------------------------------------------------------
    // Switch 內 GSrv 的連線資料
    struct GSrvInfoStruct
    {
        unsigned int		ID;				//連線號碼
        unsigned int		IPnum;			//ip number
        SYSGSrvTypeENUM     Type;           //GSrv的模式

        char                GSrvName[ _Max_GSrv_Name_Size_ ];   

        SYSNetStatENUM		Stat;			//狀態

        set<int>			ProxyConnect;	//GSrv 與哪些Proxy有作連結
        set<int>			CliConnect;	    //GSrv 與哪些Cli有作連結

        GSrvInfoStruct()	{	Init();	};
		void	Init();
    };
    //-----------------------------------------------------------------------------------
	enum ClientNetCloseTypeENUM
	{
		EM_ClientNetCloseType_None			,
		EM_ClientNetCloseType_ChangeWorld	,				//選角換世界
		EM_ClientNetCloseType_CloseWindow	,				//關閉視窗
		EM_ClientNetCloseType_Relogin		,				//重新登入
		EM_ClientNetCloseType_ReturnAccount	,				//返迴登入界面(或重新登入)
		EM_ClientNetCloseType_ServerClose	,				//Server要求斷線(返迴登入界面)
		EM_ClientNetCloseType_ServerError	,				//Server要求斷線(連線有問題)
		EM_ClientNetCloseType_ServerClose_ReloginKick	,	//Server要求斷線(重複登入)
		EM_ClientNetCloseType_LeaveBattleWorld,				//離開戰場
		EM_ClientNetCloseType_End
	};

    // Switch 內 Client 的連線資料
    struct CliInfoStruct
    {
        unsigned int		ID;				//連線號碼
        unsigned int		IPnum;			//ip number
        int                 ProxyID;        //聯結的ProxyID(  即為 Switch 對此Proxy聯線的 NetID )
        int                 SwitchNetID;    //聯結的Switch
        
        string              Account;
        string              Password;
		string				MacAddress;

        SYSNetStatENUM		Stat;			//狀態

        set<int>			GSrvConnect;	//Cli 與那些 GSrv 有連線

        int                 LastCheckTime;
		int					TempScID;		//處理暫時排程資料()
		int					WaitLoginCount;	//等待登入次數
		ClientNetCloseTypeENUM NetCloseType;

		bool				HashedPassword; //密碼是否已經經過MD5處理

        CliInfoStruct()	{	Init();	};
        void	Init();
    };
    //-----------------------------------------------------------------------------------
    // GSrv 內 Client 的連線資料
    struct GSrv_CliInfoStruct
    {
        unsigned int		ID;				//連線號碼
        unsigned int		IPnum;			//ip number
        int                 ProxyID;        //聯結的Proxy(  即為 Switch 對此Proxy聯線的 NetID )

        string              Account;

        GSrv_CliInfoStruct()	{	Init();	};
        void	Init();
    };
    //-----------------------------------------------------------------------------------
    //封包權限資料 與 來源限制 封包大小等資訊 

    struct	PGBaseInfo
    {
        SYSNetSourceStruct		Type;			//來源限制
        SYSNetManageENUM	    ManageLV;		//權限

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
    //快速搜尋 A -> B 與 B -> A
    //A 與 B 都在 0x10000 以內   
	#define _Max_Number_    0x10000
    class Map_NetIDtoIDClass
    {    
		std::vector< int > _IDToNetList;
        std::vector< int > _NetToIDList;
		int				   _LastErrID;
    public:
        //設定  NetID 關聯 ID
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
