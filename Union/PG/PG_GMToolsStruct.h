#pragma once
#pragma warning (disable:4949)
#pragma unmanaged
//�U�ϸ��
struct ZoneInfoStruct
{
    int     GUID;           //�ϰ츹�X
    char    ZoneName[32];   //�ϰ�W��
    int     ObjectCount;    //�ϰ쪫���`�q
    int     PlayerCount;    //���a�ƶq
    int     NPCCount;       //NPC�ƶq

    ZoneInfoStruct(){memset(this,0,sizeof(*this));}
};

//���a���
struct PlayerInfoStruct
{
    int     ZoneID;        //�ϰ츹�X
    int     GUID;          //�ϰ쪫��id
    int     DBID;
    int 	Race;			  //�ر�
    int 	Job;    		  //¾�~
    int     Job_Sub;          //��¾
    char    Account[64];      //�b���W��
    char    RoleName[32];     //����W��
    char    IP[20];           //�n�JIP
    int     LV;               //���a����
    int     LV_Sub;             

    PlayerInfoStruct(){memset(this,0,sizeof(*this));}
};

//NPC���
struct NPCInfoStruct
{
    int     ZoneID;           //�ϰ츹�X
    int     GUID;             //�ϰ쪫��id
    char    Name[32];         //����W��
    int     OrgDBID;          //��l����ID
    int     DBID;
    int		LV;				  //����

    NPCInfoStruct(){memset(this,0,sizeof(*this));}
};

/*
//������
struct CropseInfo
{
    int     SectionID;        //�ϰ츹�X
    int     LocalID;          //�ϰ쪫��id
    char    Name[20];         //���騤��W��
    char    Time[28];         //���ͮɶ�

    CropseInfo(){memset(this,0,sizeof(*this));}
};
*/

#pragma managed