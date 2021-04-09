#pragma once
#pragma warning (disable:4949)
#pragma unmanaged
//各區資料
struct ZoneInfoStruct
{
    int     GUID;           //區域號碼
    char    ZoneName[32];   //區域名稱
    int     ObjectCount;    //區域物件總量
    int     PlayerCount;    //玩家數量
    int     NPCCount;       //NPC數量

    ZoneInfoStruct(){memset(this,0,sizeof(*this));}
};

//玩家資料
struct PlayerInfoStruct
{
    int     ZoneID;        //區域號碼
    int     GUID;          //區域物件id
    int     DBID;
    int 	Race;			  //種族
    int 	Job;    		  //職業
    int     Job_Sub;          //副職
    char    Account[64];      //帳號名稱
    char    RoleName[32];     //角色名稱
    char    IP[20];           //登入IP
    int     LV;               //玩家等級
    int     LV_Sub;             

    PlayerInfoStruct(){memset(this,0,sizeof(*this));}
};

//NPC資料
struct NPCInfoStruct
{
    int     ZoneID;           //區域號碼
    int     GUID;             //區域物件id
    char    Name[32];         //角色名稱
    int     OrgDBID;          //原始物件ID
    int     DBID;
    int		LV;				  //等級

    NPCInfoStruct(){memset(this,0,sizeof(*this));}
};

/*
//屍體資料
struct CropseInfo
{
    int     SectionID;        //區域號碼
    int     LocalID;          //區域物件id
    char    Name[20];         //屍體角色名稱
    char    Time[28];         //產生時間

    CropseInfo(){memset(this,0,sizeof(*this));}
};
*/

#pragma managed