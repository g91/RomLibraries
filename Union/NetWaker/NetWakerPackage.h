#pragma once

#pragma	warning (disable:4200)
#pragma warning (disable:4949)
/************************************
NetWaker �t�Ϋʥ]�]�w
Revision By: hsiang
Revised on 2004/8/23 �U�� 04:22:48
Comments: 
************************************/
#include "NetWakerDefine.h"
//#include "NetWakerPackageDef.h"
#pragma unmanaged




namespace   NetWakerPackage
{
    //-----------------------------------------------------------------------------------
    //���A�C�|
    //-----------------------------------------------------------------------------------
    enum	SysPacketResult
    {	
        EM_Sys_PG_Result_OK       = 1     ,
        EM_Sys_PG_Result_Falise   = -1000 , 
        
    };
    //--------------------------------------------------------------------------------------  
    //Switch -> Proxy 	�q���إ�Proxy ID
    struct SysNet_Switch_Proxy_ConnectNotify
    {
        PacketCommandEnum Command;
        int     ProxyID;

        SysNet_Switch_Proxy_ConnectNotify()
        {            
            Command = EM_SysNet_Switch_Proxy_ConnectNotify;
        }
    };
    //Proxy  -> Switch 	�إ�OK 
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
    //GSrv �n�J NetSlot �q���إ߳s�u
    //Switch -> GSrv �q������GSrv�s�u
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
    // GSrv �^�� Switch �n�J
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
    // Switch -> Proxy      �q��proxy Game Server �N��L�p��
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
	// Proxy -> Switch	�^�� Prxoy�w���DGame Server �N��L�p��
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
     
    // Switch -> GSrv    Proxy List... (��@�ʥ]�e�X)
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
    //Proxy -> GSrv     �q���p��
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
    // GSrv -> Proxy	�^���p��
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
    // Proxy -> Switch GSrv�s�u�� 
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
    //�� GSrv �P Switch �_�u��
    // Switch -> Proxy �n�D Proxy �P�Y GSrv �_�u
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
    // Proxy -> Switch GSrv �_�u
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
    //Proxy �P GSrv �_�u�� �q�� Switch
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
    // �n�D GSrv �P �YProxy �_�u
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
    // �� GSrv �n�J Switch �� �q����L�� GSrv �� GSrv �n�J
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
    // �� GSrv ���} Switch �� �q����L�� GSrv �� GSrv �n�X
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
    //�]�w�YGSrv�@Active or Inactive
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
    //�]�w�YGSrv�@Active or Inactive
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
    //�� Client �s�u�� Switch �� 
    //�e�n�D Client �ݸ�T���ʥ]
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
    //�e Client �n�J��T�� Switch
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
    //Switch �q�� Proxy  �� Cli�n�n�J
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
	//Proxy �q�� Switch �� Cli�n�n�J�ǳ�OK
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
    //Switch �q�� Client �s���� Proxy
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
    //�� Client ���p�� Proxy ( Proxy OnConnect )
    //Proxy �q�� Cli �n�J
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
    //Client �q�� Proxy �n�J���
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
	//Proxy �q�� Client �n�J���
	// Proxy -> Client �n�J���OK
	struct SysNet_Proxy_Cli_LoginInfoOK
	{
		PacketCommandEnum Command;

		SysNet_Proxy_Cli_LoginInfoOK()
		{
			Command = EM_SysNet_Proxy_Cli_LoginInfoOK;
		}
	};   

    //--------------------------------------------------------------------------------------    
    //�� cli �n�J Proxy ok �� 
    //�q�� Switch Cli �n�J ok
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
    //�� cli �P Proxy �_�u��
    //�q�� Switch Cli �n�X
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
    //�� Cli �n�J Proxy ok ��
    //�q�� Active GSrv �� Cli �n�J
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
    //�� Cli �n�J Proxy ok ��
    //�q�� Active GSrv �� Cli �n�J
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
    //�� �q�� GSrv �Y Cli �_�u
    //�q�� Active GSrv �� Cli �n�J
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
    //�� Cli �n�J Proxy ok ��
    //�q�� Active GSrv �� Cli �n�J
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
    //�����e�ʥ]
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
    //�����e�ʥ] GSrv->GSrv
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
    //�����e�ʥ] GSrv->Cli
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
    //���Y��GSrv �P Cli �s�u( ���ɭn�D���ͳs�u�� GSrv �n�P Client ���s�� )
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
    //���Y��GSrv �P Cli �_�u
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
    //���Y��Cli �n�X
    //GSrv or Switch -> Proxy
    struct SysNet_Proxy_CliLogoutRequest
    {
        PacketCommandEnum Command;
        short				CliID;
		char				Reason[128];				// �� Client ����]

        SysNet_Proxy_CliLogoutRequest()
        {
			Command = EM_SysNet_Proxy_CliLogoutRequest;
			Reason[0] = 0;
        }
    };   
    //--------------------------------------------------------------------------------------  
    //���Y�� Srv �n�X
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
    //�w�ɳq�� Switch ���� Client �٦s�b
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
    //�� Client �b�����еn�J�ɳq��proxy ���൹ �w�g�n�J��Client
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
	//(Master)�w�ɵ��UServer���A���
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
	// GateWay �������ʥ]
	//--------------------------------------------------------------------------------------  
	//Switch -> Gateway ���U�Ҧb��WorldId
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

	//Gateway -> Switch  ���@�ɳs�u (�q���Ҫ��� WorldID �C�� �A�óq����L�Ҧ���World ���s���@�ɲ���)
	struct SysNet_Gateway_Switch_WorldConnect
	{	
		PacketCommandEnum Command;
		int		WorldID;
		SysNet_Gateway_Switch_WorldConnect()
		{
			Command = EM_SysNet_Gateway_Switch_WorldConnect;
		}
	};

	//Gateway -> Switch �q�����@���_�u
	struct SysNet_Gateway_Switch_WorldDisconnect
	{	
		PacketCommandEnum Command;
		int		WorldID;
		SysNet_Gateway_Switch_WorldDisconnect()
		{
			Command = EM_SysNet_Gateway_Switch_WorldDisconnect;
		}
	};

	//GSrv -> Switch ���U���N�zServer
	struct SysNet_GSrv_Switch_PrxoyRegister
	{	
		PacketCommandEnum Command;
		SysNet_GSrv_Switch_PrxoyRegister()
		{
			Command = EM_SysNet_GSrv_Switch_PrxoyRegister;
		}
	};
	//--------------------------------------------------------------------------------------  
	//�ʥ]��e
	//--------------------------------------------------------------------------------------  
	enum	FromServerTypeENUM
	{
		EM_FromServerType_GSrv		,
		EM_FromServerType_Switch	,
		EM_FromServerType_Gateway	,

	};
	//��ӥ@����e���](Server->Server)
	//GSrv -> Switch -> Gateway -> Switch ->GSrv	��e
	struct SysNet_X_DataTransmitToWorldGSrv
	{
		PacketCommandEnum	Command;
		FromServerTypeENUM	FromSrvType;
		short				FromWorldID;
		short				FromNetID;
		short				ToWorldID;
		short				ToNetID;		//ToNetID = -1  ��
		int                 DataSize;
		char				Data[0];

		SysNet_X_DataTransmitToWorldGSrv()
		{
			Command = EM_SysNet_X_DataTransmitToWorldGSrv;
		}
	};

	//��ӥ@����e���](Server->Server)
	//GSrv -> Switch -> Gateway -> Switch ->GSrv	��e
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
	//���U��Server ��e�� GSrvID;
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
	//�n�D��Y�ӱb����
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
	//GSrv -> Proxy PingLog �O��
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
	//Proxy -> GSrv PingLog �O��
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
	//Client -> Proxy client �ݭn�D�_�u
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
	//Switch -> Cli �b���n�J����, �B�~�ǵ�Client�����
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
