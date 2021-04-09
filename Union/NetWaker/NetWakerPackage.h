#pragma once

#pragma	warning (disable:4200)
#pragma warning (disable:4949)
/************************************
NetWaker 系統封包設定
Revision By: hsiang
Revised on 2004/8/23 下午 04:22:48
Comments: 
************************************/
#include "NetWakerDefine.h"
//#include "NetWakerPackageDef.h"
#pragma unmanaged




namespace   NetWakerPackage
{
    //-----------------------------------------------------------------------------------
    //狀態列舉
    //-----------------------------------------------------------------------------------
    enum	SysPacketResult
    {	
        EM_Sys_PG_Result_OK       = 1     ,
        EM_Sys_PG_Result_Falise   = -1000 , 
        
    };
    //--------------------------------------------------------------------------------------  
    //Switch -> Proxy 	通知建立Proxy ID
    struct SysNet_Switch_Proxy_ConnectNotify
    {
        PacketCommandEnum Command;
        int     ProxyID;

        SysNet_Switch_Proxy_ConnectNotify()
        {            
            Command = EM_SysNet_Switch_Proxy_ConnectNotify;
        }
    };
    //Proxy  -> Switch 	建立OK 
    struct SysNet_Proxy_Switch_Connect
    {
        PacketCommandEnum Command;
        int     GSrvPort;
        int     CliPort;
		char	CliIPStr[64];
		char	GSrvIPStr[64];

        SysNet_Proxy_Switch_Connect()
        {
            Command = EM_SysNet_Proxy_Switch_Connect;
        }
    };
    //--------------------------------------------------------------------------------------    
    //GSrv 登入 NetSlot 通知建立連線
    //Switch -> GSrv 通知成立GSrv連線
    struct SysNet_Switch_GSrv_NotifyConnect
    {	
        PacketCommandEnum Command;

        int     GSrvID;
		int		WorldID;
        SysNet_Switch_GSrv_NotifyConnect()
        {
            Command = EM_SysNet_Switch_GSrv_NotifyConnect;
        }
    };
    //--------------------------------------------------------------------------------------    
    // GSrv 回應 Switch 登入
    // GSrv -> Switch
    struct SysNet_GSrv_Switch_Connect
    {	
        PacketCommandEnum Command;
        char    GSrvName[ _Max_GSrv_Name_Size_ ];
		char	IPStr[ 32 ];

        SysNet_GSrv_Switch_Connect()
        {
            GSrvName[0] = 0;
            Command = EM_SysNet_GSrv_Switch_Connect;
        }
    };
    //--------------------------------------------------------------------------------------    
    // Switch -> Proxy      通知proxy Game Server 將對他聯結
    struct SysNet_Switch_Proxy_GSrvConnect
    {	
        PacketCommandEnum Command;
        int		GSrvID;

        SysNet_Switch_Proxy_GSrvConnect()
        {
            Command = EM_SysNet_Switch_Proxy_GSrvConnect;
        }
    };
    //--------------------------------------------------------------------------------------    
	// Proxy -> Switch	回應 Prxoy已知道Game Server 將對他聯結
	struct SysNet_Proxy_Switch_GSrvConnectPrepareOK
	{	
		PacketCommandEnum Command;
		int		GSrvID;

		SysNet_Proxy_Switch_GSrvConnectPrepareOK()
		{
			Command = EM_SysNet_Proxy_Switch_GSrvConnectPrepareOK;
		}
	};
    //--------------------------------------------------------------------------------------    
     
    // Switch -> GSrv    Proxy List... (單一封包送出)
    struct SysNet_Switch_GSrv_ProxyAddr
    {	
        PacketCommandEnum Command;
        int				ProxyID;
        //unsigned int	IPNum;
		char			GSrvIPStr[64];
        int				Port;

        SysNet_Switch_GSrv_ProxyAddr()
        {
            Command = EM_SysNet_Switch_GSrv_ProxyAddr;
        }
    };
    //--------------------------------------------------------------------------------------    
    //Proxy -> GSrv     通知聯結
    struct SysNet_Proxy_GSrv_ConnectNotify
    {	
        PacketCommandEnum Command;
        int     ProxyID;

        SysNet_Proxy_GSrv_ConnectNotify()
        {
            Command = EM_SysNet_Proxy_GSrv_ConnectNotify;
        }
    };

    //--------------------------------------------------------------------------------------    
    // GSrv -> Proxy	回應聯結
    struct SysNet_GSrv_Proxy_Connect
    {	
        PacketCommandEnum Command;

        int     GSrvID;
        char    GSrvName[ _Max_GSrv_Name_Size_ ];

        SysNet_GSrv_Proxy_Connect()
        {
            Command = EM_SysNet_GSrv_Proxy_Connect;
        }
    };
    //--------------------------------------------------------------------------------------    
    // Proxy -> Switch GSrv連線成 
    struct SysNet_Proxy_Switch_GSrvConnectOK
	{
        PacketCommandEnum Command;
        int     GSrvID;

        SysNet_Proxy_Switch_GSrvConnectOK()
        {
            Command = EM_SysNet_Proxy_Switch_GSrvConnectOK;
        }
    };

    //--------------------------------------------------------------------------------------    
    //當 GSrv 與 Switch 斷線時
    // Switch -> Proxy 要求 Proxy 與某 GSrv 斷線
    struct SysNet_Switch_Proxy_GSrvDisconnectNotify
    {	
        PacketCommandEnum Command;
        int     GSrvID;

        SysNet_Switch_Proxy_GSrvDisconnectNotify()
        {
            Command = EM_SysNet_Switch_Proxy_GSrvDisconnectNotify;
        }
    };
    //--------------------------------------------------------------------------------------    
    // Proxy -> Switch GSrv 斷線
    struct SysNet_Proxy_Switch_GSrvDisconnect
    {	
        PacketCommandEnum Command;
        int     GSrvID;

        SysNet_Proxy_Switch_GSrvDisconnect()
        {
            Command = EM_SysNet_Proxy_Switch_GSrvDisconnect;
        }
    };
    //--------------------------------------------------------------------------------------    
    //Proxy 與 GSrv 斷線時 通知 Switch
    // GSrv -> Switch
    struct SysNet_GSrv_Switch_ProxyDisconnect
    {	
        PacketCommandEnum Command;
        int     ProxyID;

        SysNet_GSrv_Switch_ProxyDisconnect()
        {
            Command = EM_SysNet_GSrv_Switch_ProxyDisconnect;
        }
    };
    //--------------------------------------------------------------------------------------    
    // 要求 GSrv 與 某Proxy 斷線
    // Switch -> GSrv
    struct SysNet_Switch_GSrv_ProxyDisconnectRequest
    {	
        PacketCommandEnum Command;
        int     ProxyID;

        SysNet_Switch_GSrv_ProxyDisconnectRequest()
        {
            Command = EM_SysNet_Switch_GSrv_ProxyDisconnectRequest;
        }
    };
    //--------------------------------------------------------------------------------------    
    // 當 GSrv 登入 Switch 時 通知其他的 GSrv 此 GSrv 登入
    // Switch -> GSrv
    struct SysNet_Switch_GSrv_OnGSrvConnect
    {	
        PacketCommandEnum Command;
        int     GSrvID;
        char    GSrvName[ _Max_GSrv_Name_Size_ ];

        SysNet_Switch_GSrv_OnGSrvConnect()
        {
            Command = EM_SysNet_Switch_GSrv_OnGSrvConnect;
        }
    };
    //--------------------------------------------------------------------------------------    
    // 當 GSrv 離開 Switch 時 通知其他的 GSrv 此 GSrv 登出
    // Switch -> GSrv
    struct SysNet_Switch_GSrv_OnGSrvDisConnect
    {	
        PacketCommandEnum Command;

        int     GSrvID;

        SysNet_Switch_GSrv_OnGSrvDisConnect()
        {
            Command = EM_SysNet_Switch_GSrv_OnGSrvDisConnect;
        }
    };
    //--------------------------------------------------------------------------------------    
    //設定某GSrv　Active or Inactive
    //GSrv -> Switch 	
    struct SysNet_GSrv_Switch_SetActive
    {
        PacketCommandEnum Command;

        SYSGSrvTypeENUM Type;

        SysNet_GSrv_Switch_SetActive()
        {
            Command = EM_SysNet_GSrv_Switch_SetActive;
        }
    };
    //--------------------------------------------------------------------------------------    
    //設定某GSrv　Active or Inactive
    //Switch -> Proxy
    struct SysNet_Switch_Proxy_SetActive
    {
        PacketCommandEnum Command;

        int             GSrvID;
        SYSGSrvTypeENUM Type;

        SysNet_Switch_Proxy_SetActive()
        {
            Command = EM_SysNet_Switch_Proxy_SetActive;
        }
    };
    //--------------------------------------------------------------------------------------    
    //當 Client 連線到 Switch 時 
    //送要求 Client 端資訊的封包
    //Switch -> Client
    struct SysNet_Switch_Cli_LoginInfoRequest
    {
        PacketCommandEnum Command;
        int     CliID;

        SysNet_Switch_Cli_LoginInfoRequest()
        {
            Command = EM_SysNet_Switch_Cli_LoginInfoRequest;
        }
    };
    //--------------------------------------------------------------------------------------    
    //送 Client 登入資訊給 Switch
    //Client -> Switch
    struct SysNet_Cli_Switch_LoginInfo
    {
        PacketCommandEnum Command;
        int     CliID;
        char    Account[ _Max_CliAccount_Name_Size_ ];
		char    Password[ _Max_CliPassword_Name_Size_ ];
		char	MacAddress[ _Max_CliMac_Address_Size_ ];
		bool	HashedPassword;

        SysNet_Cli_Switch_LoginInfo()
        {
            Command = EM_SysNet_Cli_Switch_LoginInfo;
        }
    };    
    //--------------------------------------------------------------------------------------    
	struct CliConnectInfoStruct
	{
		int		SwitchNetID;
		int     ProxyID;
		char    ProxyIPStr[32];

		int     ProxyPort;
		int		PlayTimeQuota;	

		char    LastLoginIP     [ 20 ];
		char    LastLoginTime   [ 28 ];	
	};
    //Switch 通知 Proxy  有 Cli要登入
    //Switch -> Proxy
    struct SysNet_Switch_Proxy_CliConnect
    {	
        PacketCommandEnum Command;
        int		CliID;
        char    Account[ _Max_CliAccount_Name_Size_ ];
		CliConnectInfoStruct CliInfo;

        SysNet_Switch_Proxy_CliConnect()
        {
            Command = EM_SysNet_Switch_Proxy_CliConnect;
        }
    };
	//--------------------------------------------------------------------------------------    
	//Proxy 通知 Switch 有 Cli要登入準備OK
	// Proxy -> Switch
	struct SysNet_Proxy_Switch_CliConnectPrepareOK
	{	
		PacketCommandEnum Command;
		CliConnectInfoStruct CliInfo;

		SysNet_Proxy_Switch_CliConnectPrepareOK()
		{
			Command = EM_SysNet_Proxy_Switch_CliConnectPrepareOK;
		}
	};

    //--------------------------------------------------------------------------------------    
    //Switch 通知 Client 連結到 Proxy
    //Switch -> Client
    struct SysNet_Switch_Cli_ConnectPorxyNotify
    {
        PacketCommandEnum   Command;
        SYSLogFailedENUM    Result;
        int     ProxyID;
		char    ProxyIPStr[32];

		int     ProxyPort;
		int		PlayTimeQuota;

        char    LoginInfo[32];

		char    LastLoginIP     [ 20 ];
		char    LastLoginTime   [ 28 ];	

        SysNet_Switch_Cli_ConnectPorxyNotify()
        {
            Result = EM_NetLogFailed_OK;
            Command = EM_SysNet_Switch_Cli_ConnectPorxyNotify;
        }
    };    
    //--------------------------------------------------------------------------------------    
    //當 Client 剛聯到 Proxy ( Proxy OnConnect )
    //Proxy 通知 Cli 登入
    //Proxy -> Client 
    struct SysNet_Proxy_Cli_LoginInfoRequest
    {
        PacketCommandEnum Command;

        SysNet_Proxy_Cli_LoginInfoRequest()
        {
            Command = EM_SysNet_Proxy_Cli_LoginInfoRequest;
        }
    };
    //--------------------------------------------------------------------------------------    
    //Client 通知 Proxy 登入資料
    //Client -> Proxy
    struct SysNet_Cli_Proxy_LoginInfo
    {
        PacketCommandEnum Command;
        int     CliID;

        SysNet_Cli_Proxy_LoginInfo()
        {
            Command = EM_SysNet_Cli_Proxy_LoginInfo;
        }
    };   
	//--------------------------------------------------------------------------------------    
	//Proxy 通知 Client 登入資料
	// Proxy -> Client 登入資料OK
	struct SysNet_Proxy_Cli_LoginInfoOK
	{
		PacketCommandEnum Command;

		SysNet_Proxy_Cli_LoginInfoOK()
		{
			Command = EM_SysNet_Proxy_Cli_LoginInfoOK;
		}
	};   

    //--------------------------------------------------------------------------------------    
    //當 cli 登入 Proxy ok 時 
    //通知 Switch Cli 登入 ok
    //Proxy -> Switch
    struct SysNet_Proxy_Switch_CliLoginOK
    {
        PacketCommandEnum Command;
        int     CliID;

        SysNet_Proxy_Switch_CliLoginOK()
        {
            Command = EM_SysNet_Proxy_Switch_CliLoginOK;
        }
    };    
    //--------------------------------------------------------------------------------------    
    //當 cli 與 Proxy 斷線時
    //通知 Switch Cli 登出
    //Proxy -> Switch
    struct SysNet_Proxy_Switch_CliLogout
    {
        PacketCommandEnum Command;
        int     CliID;

        SysNet_Proxy_Switch_CliLogout()
        {
            Command = EM_SysNet_Proxy_Switch_CliLogout;
        }
    };    
    //--------------------------------------------------------------------------------------    
    //當 Cli 登入 Proxy ok 時
    //通知 Active GSrv 有 Cli 登入
    struct SysNet_Proxy_GSrv_CliOnConnect
    {
        PacketCommandEnum Command;
        int     CliID;
        char    Account[ _Max_CliAccount_Name_Size_ ];
		int		IpNun;

        SysNet_Proxy_GSrv_CliOnConnect()
        {
            Command = EM_SysNet_Proxy_GSrv_CliOnConnect;
        }
    };    
    //--------------------------------------------------------------------------------------    
    //當 Cli 登入 Proxy ok 時
    //通知 Active GSrv 有 Cli 登入
    struct SysNet_Proxy_Cli_GSrvOnConnect
    {
        PacketCommandEnum Command;
        int     GSrvID;
        char    GSrvName[ _Max_GSrv_Name_Size_ ];

        SysNet_Proxy_Cli_GSrvOnConnect()
        {
            Command = EM_SysNet_Proxy_Cli_GSrvOnConnect;
        }
    };    
    //--------------------------------------------------------------------------------------    
    //當 通知 GSrv 某 Cli 斷線
    //通知 Active GSrv 有 Cli 登入
    struct SysNet_Proxy_GSrv_CliOnDisConnect
    {
        PacketCommandEnum Command;
        int     CliID;

        SysNet_Proxy_GSrv_CliOnDisConnect()
        {
            Command = EM_SysNet_Proxy_GSrv_CliOnDisConnect;
        }
    };    
    //--------------------------------------------------------------------------------------    
    //當 Cli 登入 Proxy ok 時
    //通知 Active GSrv 有 Cli 登入
    struct SysNet_Proxy_Cli_GSrvOnDisConnect
    {
        PacketCommandEnum Command;
        int     GSrvID;

        SysNet_Proxy_Cli_GSrvOnDisConnect()
        {
            Command = EM_SysNet_Proxy_Cli_GSrvOnDisConnect;
        }
    };    
    //--------------------------------------------------------------------------------------  
    //資料轉送封包
    //Client -> Proxy
    struct SysNet_Cli_Proxy_DataCliToGSrv
    {
        PacketCommandEnum Command;
        short	GSrvID;
		short	ClientDataKey;
        int		Size;
        char    Data[0];

        SysNet_Cli_Proxy_DataCliToGSrv()
        {
			ClientDataKey = -1;
            Command = EM_SysNet_Cli_Proxy_DataCliToGSrv;
        }

    };    
    
    //Proxy -> GSrv
    struct SysNet_Proxy_GSrv_DataCliToGSrv
    {
        PacketCommandEnum Command;
        short	CliID;
		short	ClientDataKey;
        int		Size;
        char    Data[0];

        SysNet_Proxy_GSrv_DataCliToGSrv()
        {
			ClientDataKey = -1;
            Command = EM_SysNet_Proxy_GSrv_DataCliToGSrv;
        }
    };
    
    //--------------------------------------------------------------------------------------  
    //資料轉送封包 GSrv->GSrv
    //GSrv -> Switch
    struct SysNet_GSrv_Switch_DataGSrvToGSrv
    {
        PacketCommandEnum Command;
		short	GSrvID;
        
        int		Size;
        char    Data[0];

        SysNet_GSrv_Switch_DataGSrvToGSrv()
        {
            Command = EM_SysNet_GSrv_Switch_DataGSrvToGSrv;
        }
    };

	struct SysNet_GSrv_Switch_DataGSrvToGSrv_Zip
	{
		PacketCommandEnum Command;
		short	GSrvID;
		int		Size;

		int		ZipSize;
		char    Data[0x20000];

		int PGSize()
		{
			return sizeof(*this) - sizeof(Data) + ZipSize;
		}

		SysNet_GSrv_Switch_DataGSrvToGSrv_Zip()
		{
			Command = EM_SysNet_GSrv_Switch_DataGSrvToGSrv_Zip;
		}
	};
    
    //Switch -> GSrv
    struct SysNet_Switch_GSrv_DataGSrvToGSrv
    {
        PacketCommandEnum Command;
        short	GSrvID;
        int	    Size;
        char    Data[0];

        SysNet_Switch_GSrv_DataGSrvToGSrv()
        {
            Command = EM_SysNet_Switch_GSrv_DataGSrvToGSrv;
        }
    };    
    
	struct SysNet_Switch_GSrv_DataGSrvToGSrv_Zip
	{
		PacketCommandEnum Command;
		short	GSrvID;
		int		Size;

		int		ZipSize;
		char    Data[0x20000];

		SysNet_Switch_GSrv_DataGSrvToGSrv_Zip()
		{
			Command = EM_SysNet_Switch_GSrv_DataGSrvToGSrv_Zip;
		}
	};    

    //--------------------------------------------------------------------------------------  
    //資料轉送封包 GSrv->Cli
    //GSrv -> Proxy
    struct SysNet_GSrv_Proxy_DataGSrvToCli
    {
        PacketCommandEnum Command;
        short	CliID;
		short	ClientDataKey;
        int		Size;
        char    Data[0];

        SysNet_GSrv_Proxy_DataGSrvToCli()
        {
			ClientDataKey = -1;
            Command = EM_SysNet_GSrv_Proxy_DataGSrvToCli;
        }
    };    
    
    //Proxy -> Cli
    struct SysNet_Proxy_Cli_DataGSrvToCli
    {
        PacketCommandEnum Command;
        short	GSrvID;
		short	ClientDataKey;
        int	    Size;
        char    Data[0];

        SysNet_Proxy_Cli_DataGSrvToCli()
        {
			ClientDataKey = -1;
            Command = EM_SysNet_Proxy_Cli_DataGSrvToCli;
        }
    };    

    //--------------------------------------------------------------------------------------  
    //讓某個GSrv 與 Cli 連線( 此時要求產生連線的 GSrv 要與 Client 有連結 )
    //GSrv -> Switch -> Proxy
    struct SysNet_GSrv_Switch_Proxy_ConnectCliAndGSrv
    {
        PacketCommandEnum Command;
        short  CliID;
        short  GSrvID;

        SysNet_GSrv_Switch_Proxy_ConnectCliAndGSrv()
        {
            Command = EM_SysNet_GSrv_Switch_Proxy_ConnectCliAndGSrv;
        }
    };    
    //--------------------------------------------------------------------------------------  
    //讓某個GSrv 與 Cli 斷線
    //GSrv -> Switch  -> Proxy
    struct SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv
    {
        PacketCommandEnum Command;
        short  CliID;
        short  GSrvID;

        SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv()
        {
            Command = EM_SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv;
        }
    };   
    //--------------------------------------------------------------------------------------  
    //讓某個Cli 登出
    //GSrv or Switch -> Proxy
    struct SysNet_Proxy_CliLogoutRequest
    {
        PacketCommandEnum Command;
        short				CliID;
		char				Reason[128];				// 踢掉 Client 的原因

        SysNet_Proxy_CliLogoutRequest()
        {
			Command = EM_SysNet_Proxy_CliLogoutRequest;
			Reason[0] = 0;
        }
    };   
    //--------------------------------------------------------------------------------------  
    //讓某個 Srv 登出
    //GSrv  -> Switch
    struct SysNet_GSrv_Switch_SrvLogoutRequest
    {
        PacketCommandEnum Command;
        short   SrvID;

        SysNet_GSrv_Switch_SrvLogoutRequest()
        {
            Command = EM_SysNet_GSrv_Switch_SrvLogoutRequest;
        }
    };   
    //--------------------------------------------------------------------------------------  
    //定時通知 Switch 那些 Client 還存在
    //Proxy -> Switch
    struct SysNet_Proxy_Switch_NotifyCliIDLive
    {
        PacketCommandEnum Command;
        short   CliID;

        SysNet_Proxy_Switch_NotifyCliIDLive()
        {
            Command = EM_SysNet_Proxy_Switch_NotifyCliIDLive;
        }
    };   

    //--------------------------------------------------------------------------------------  
    //當 Client 帳號重覆登入時通知proxy 並轉給 已經登入的Client
    //Proxy -> Switch
    struct SysNet_Switch_Proxy_NotifyCliIDRelogin
    {
        PacketCommandEnum Command;
        short   CliID;
        int     IPNum;

        SysNet_Switch_Proxy_NotifyCliIDRelogin()
        {
            Command = EM_SysNet_Switch_Proxy_NotifyCliIDRelogin;
        }
    };   
    struct SysNet_Proxy_Cli_NotifyCliIDRelogin
    {
        PacketCommandEnum Command;
        int     IPNum;

        SysNet_Proxy_Cli_NotifyCliIDRelogin()
        {
            Command = EM_SysNet_Proxy_Cli_NotifyCliIDRelogin;
        }
    };   
    //--------------------------------------------------------------------------------------  
	//(Master)定時註冊Server狀態資料
	// GSrv -> Switch
	struct SysNet_GSrv_Switch_ServerListInfo
	{	
		PacketCommandEnum Command;
		int MaxPlayerCount;
		int	ParallelZoneCount;
		int	PlayerCount[100];
		float TotalPlayerRate;

		SysNet_GSrv_Switch_ServerListInfo()
		{
			Command = EM_SysNet_GSrv_Switch_ServerListInfo;
		}
	};
	//--------------------------------------------------------------------------------------    
	// GateWay 相關的封包
	//--------------------------------------------------------------------------------------  
	//Switch -> Gateway 註冊所在的WorldId
	struct SysNet_Switch_Gateway_Register
	{	
		PacketCommandEnum Command;
		int		WorldID;

		SysNet_Switch_Gateway_Register()
		{
			Command = EM_SysNet_Switch_Gateway_Register;
		}
	};
	//Gateway -> Switch
	struct SysNet_Gateway_Switch_RegisterResult
	{	
		PacketCommandEnum Command;
		int		WorldID;
		bool	Result;

		SysNet_Gateway_Switch_RegisterResult()
		{
			Command = EM_SysNet_Gateway_Switch_RegisterResult;
		}
	};

	//Gateway -> Switch  有世界連線 (通知所的的 WorldID 列表 ，並通知其他所有的World 有新的世界產生)
	struct SysNet_Gateway_Switch_WorldConnect
	{	
		PacketCommandEnum Command;
		int		WorldID;
		SysNet_Gateway_Switch_WorldConnect()
		{
			Command = EM_SysNet_Gateway_Switch_WorldConnect;
		}
	};

	//Gateway -> Switch 通知有世界斷線
	struct SysNet_Gateway_Switch_WorldDisconnect
	{	
		PacketCommandEnum Command;
		int		WorldID;
		SysNet_Gateway_Switch_WorldDisconnect()
		{
			Command = EM_SysNet_Gateway_Switch_WorldDisconnect;
		}
	};

	//GSrv -> Switch 註冊為代理Server
	struct SysNet_GSrv_Switch_PrxoyRegister
	{	
		PacketCommandEnum Command;
		SysNet_GSrv_Switch_PrxoyRegister()
		{
			Command = EM_SysNet_GSrv_Switch_PrxoyRegister;
		}
	};
	//--------------------------------------------------------------------------------------  
	//封包轉送
	//--------------------------------------------------------------------------------------  
	enum	FromServerTypeENUM
	{
		EM_FromServerType_GSrv		,
		EM_FromServerType_Switch	,
		EM_FromServerType_Gateway	,

	};
	//兩個世界轉送風包(Server->Server)
	//GSrv -> Switch -> Gateway -> Switch ->GSrv	轉送
	struct SysNet_X_DataTransmitToWorldGSrv
	{
		PacketCommandEnum	Command;
		FromServerTypeENUM	FromSrvType;
		short				FromWorldID;
		short				FromNetID;
		short				ToWorldID;
		short				ToNetID;		//ToNetID = -1  比
		int                 DataSize;
		char				Data[0];

		SysNet_X_DataTransmitToWorldGSrv()
		{
			Command = EM_SysNet_X_DataTransmitToWorldGSrv;
		}
	};

	//兩個世界轉送風包(Server->Server)
	//GSrv -> Switch -> Gateway -> Switch ->GSrv	轉送
	struct SysNet_X_DataTransmitToWorldGSrv_GSrvID
	{
		PacketCommandEnum	Command;
		FromServerTypeENUM	FromSrvType;
		short				FromWorldID;
		short				FromNetID;
		short				ToWorldID;
		short				ToGSrvID;
		short				ToGSrvType;
		int                 DataSize;
		char				Data[0];

		SysNet_X_DataTransmitToWorldGSrv_GSrvID()
		{
			Command = EM_SysNet_X_DataTransmitToWorldGSrv_GSrvID;
		}
	};
	//--------------------------------------------------------------------------------------  
	//註冊跨Server 轉送的 GSrvID;
	//GSrv -> Switch
	struct SysNet_GSrv_Switch_RegisterGSrvID
	{
		PacketCommandEnum	Command;
		int					SrvID;
		int					SrvType;

		SysNet_GSrv_Switch_RegisterGSrvID()
		{
			Command = EM_SysNet_GSrv_Switch_RegisterGSrvID;
		}
	};
	//--------------------------------------------------------------------------------------  
	//要求把某個帳號踢除
	//GSrv -> Switch
	struct SysNet_GSrv_Switch_FreezeAccount
	{
		PacketCommandEnum	Command;
		char	Account[_Max_CliAccount_Name_Size_];
		int		FreezeType;	

		SysNet_GSrv_Switch_FreezeAccount()
		{
			Command = EM_SysNet_GSrv_Switch_FreezeAccount;
		}
	};
	//--------------------------------------------------------------------------------------  
	//GSrv -> Proxy PingLog 記錄
	struct SysNet_GSrv_Proxy_PingLog
	{
		PacketCommandEnum	Command;
		int					Time;

		SysNet_GSrv_Proxy_PingLog()
		{
			Command = EM_SysNet_GSrv_Proxy_PingLog;
		}
	};
	//--------------------------------------------------------------------------------------  
	//Proxy -> GSrv PingLog 記錄
	struct SysNet_Proxy_GSrv_PingLog
	{
		PacketCommandEnum	Command;
		int					Time;
		int					ProxyID;

		SysNet_Proxy_GSrv_PingLog()
		{
			Command = EM_SysNet_Proxy_GSrv_PingLog;
		}
	};
	//--------------------------------------------------------------------------------------  
	//Client -> Proxy client 端要求斷線
	struct SysNet_Cli_Proxy_NetClose
	{
		PacketCommandEnum	Command;
			
		ClientNetCloseTypeENUM NetCloseType;
		SysNet_Cli_Proxy_NetClose()
		{
			Command = EM_SysNet_Cli_Proxy_NetClose;
		}
	};
	//--------------------------------------------------------------------------------------  

	//--------------------------------------------------------------------------------------
	//Switch -> Cli 帳號登入失敗, 額外傳給Client的資料
	union LoginFailedDataUnion
	{
		struct
		{
			int Count;
			int Max;
			int FreezeTime;
		}PasswordError;

		struct
		{
			int RemainingTime;
		}Freeze;
	};

	struct SysNet_Switch_Cli_LoginFailedData
	{
		PacketCommandEnum	Command;
		SYSLogFailedENUM    Result;
		LoginFailedDataUnion Data;

		SysNet_Switch_Cli_LoginFailedData()
		{
			memset(this, 0, sizeof(*this));
			Command = EM_SysNet_Switch_Cli_LoginFailedData;
		}
	};
	//--------------------------------------------------------------------------------------
}

#pragma managed
