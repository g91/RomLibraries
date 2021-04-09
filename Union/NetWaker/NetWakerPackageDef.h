
/************************************
NetWaker 封包定義資料　
Revision By: hsiang
Revised on 2004/8/23 下午 04:31:19
Comments: 
************************************/

#pragma once
#pragma warning (disable:4949)
#pragma unmanaged

namespace   NetWakerPackage
{

    //封包命令列舉
    enum	PacketCommandEnum
    { 
        EM_NOUSED                                  = 0    ,
        EM_SysNet_Switch_Proxy_ConnectNotify       = 100  , 
        EM_SysNet_Proxy_Switch_Connect                    , 
        EM_SysNet_Switch_GSrv_NotifyConnect               ,     
        EM_SysNet_GSrv_Switch_Connect                     ,
        EM_SysNet_Switch_Proxy_GSrvConnect                ,
        EM_SysNet_Switch_GSrv_ProxyAddr                   , 

        EM_SysNet_Proxy_GSrv_ConnectNotify                ,
        EM_SysNet_GSrv_Proxy_Connect                      ,
        EM_SysNet_Proxy_Switch_GSrvConnectOK              ,
        EM_SysNet_Proxy_GSrv_ConnectOK                    ,

        EM_SysNet_Switch_Proxy_GSrvDisconnectNotify       ,
        EM_SysNet_Proxy_Switch_GSrvDisconnect             ,
        EM_SysNet_GSrv_Switch_ProxyDisconnect             ,
        EM_SysNet_Switch_GSrv_ProxyDisconnectRequest      ,

        EM_SysNet_Switch_GSrv_OnGSrvConnect               ,
        EM_SysNet_Switch_GSrv_OnGSrvDisConnect            ,

        EM_SysNet_GSrv_Switch_SetActive                   ,
        EM_SysNet_Switch_Proxy_SetActive                  ,

        EM_SysNet_Switch_Cli_LoginInfoRequest             ,
        EM_SysNet_Cli_Switch_LoginInfo                    ,
        EM_SysNet_Switch_Cli_ConnectPorxyNotify           ,

        EM_SysNet_Proxy_Cli_LoginInfoRequest              ,
        EM_SysNet_Cli_Proxy_LoginInfo                     ,
        EM_SysNet_Switch_Proxy_CliConnect                 ,
        EM_SysNet_Proxy_Switch_CliLoginOK                 ,
        EM_SysNet_Proxy_Switch_CliLogout                  ,

        EM_SysNet_Proxy_GSrv_CliOnConnect                 ,
        EM_SysNet_Proxy_Cli_GSrvOnConnect                 ,
        EM_SysNet_Proxy_GSrv_CliOnDisConnect              ,
        EM_SysNet_Proxy_Cli_GSrvOnDisConnect              ,

        EM_SysNet_Cli_Proxy_DataCliToGSrv                 ,
        EM_SysNet_Proxy_GSrv_DataCliToGSrv                ,

        EM_SysNet_GSrv_Switch_DataGSrvToGSrv              ,
        EM_SysNet_Switch_GSrv_DataGSrvToGSrv              ,

        EM_SysNet_GSrv_Proxy_DataGSrvToCli                ,
        EM_SysNet_Proxy_Cli_DataGSrvToCli                 ,

        EM_SysNet_GSrv_Switch_Proxy_ConnectCliAndGSrv     ,
        EM_SysNet_GSrv_Switch_Proxy_DisconnectCliAndGSrv  ,
        EM_SysNet_Proxy_CliLogoutRequest                  ,
        EM_SysNet_GSrv_Switch_SrvLogoutRequest            ,

        EM_SysNet_Proxy_Switch_NotifyCliIDLive            ,

        EM_SysNet_Switch_Proxy_NotifyCliIDRelogin         ,
        EM_SysNet_Proxy_Cli_NotifyCliIDRelogin            ,
		EM_SysNet_GSrv_Switch_ServerListInfo			  ,

		EM_SysNet_Proxy_Switch_GSrvConnectPrepareOK		  ,
		EM_SysNet_Proxy_Switch_CliConnectPrepareOK		  ,


		EM_SysNet_Switch_Gateway_Register				,
		EM_SysNet_Gateway_Switch_RegisterResult			,
		EM_SysNet_Gateway_Switch_WorldConnect			,
		EM_SysNet_Gateway_Switch_WorldDisconnect		,
		EM_SysNet_GSrv_Switch_PrxoyRegister				,
		EM_SysNet_X_DataTransmitToWorldGSrv				,
		EM_SysNet_X_DataTransmitToWorldGSrv_GSrvID		,
		EM_SysNet_GSrv_Switch_RegisterGSrvID			,

		EM_SysNet_GSrv_Switch_FreezeAccount				,

		EM_SysNet_GSrv_Proxy_PingLog					,
		EM_SysNet_Proxy_GSrv_PingLog					,

		EM_SysNet_Proxy_Cli_LoginInfoOK					,
		EM_SysNet_Cli_Proxy_NetClose					,

		EM_SysNet_GSrv_Switch_DataGSrvToGSrv_Zip        ,
		EM_SysNet_Switch_GSrv_DataGSrvToGSrv_Zip        ,

		EM_SysNet_Switch_Cli_LoginFailedData			,

		EM_SysNet_Packet_Count							,

        EM_SysNet_MAX                              = 0xffffffff      ,
    };


};

#pragma managed
