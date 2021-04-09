#pragma once

#include "ObjectDataFile/ObjectDataBaseFile.h"
#include "RoleData/RoleDataEx.h"
#include "basefunction/BaseFunction.h"
#include <unordered_map> 
#include <map>
#include <vector>
using namespace std;
using namespace stdext;
using namespace NSObjectDataFile;
class RuStreamFile;
//////////////////////////////////////////////////////////////////////////
struct LessCharStr : public binary_function <const char*, const char*, bool> 
{
	bool operator()( const char* _Left, const char* _Right	) const
	{
		return ( strcmp( _Left , _Right) < 0 );
	}
};
//////////////////////////////////////////////////////////////////////////
enum CharacterNameRulesENUM
{
	EM_CharacterNameRules_Rightful	= 0,			// �X�k
	EM_CharacterNameRules_Short,					// �W�٤ӵu,���X�k
	EM_CharacterNameRules_Long,						// �W�٤Ӫ�,���X�k
	EM_CharacterNameRules_Wrongful,					// �����~���Ÿ��Φr��
	EM_CharacterNameRules_Special,					// �S��W�٤����
	EM_CharacterNameRules_BasicLetters,				// �u��ϥΰ򥻭^��r��
	EM_CharacterNameRules_EndLetter,				// �����r�����~(',")
};

//////////////////////////////////////////////////////////////////////////
struct CreateCharacterColor
{
	int numColors;
	struct 
	{
		union
		{
			struct  
			{
				unsigned char a , b, g, r ;
			};
			int color;

		};
		
	}color[80];
};
//////////////////////////////////////////////////////////////////////////
struct AccountStoreItemTable
{
	int index;
	int itemID;
	int count;
	int cost;
};
//////////////////////////////////////////////////////////////////////////
struct SWorldMapInfoTable
{
	SWorldMapInfoTable( int id ):WorldID(id),WorldZone(NULL){}
	int WorldID;
	const WorldMapInfoStruct* WorldZone; //�@�� �j��

	//vector<const WorldMapInfoStruct*> ZoneTable;
  	//vector<const WorldMapInfoStruct*> CityTable;
	//vector<const WorldMapInfoStruct*> InstanceTable;

	map< int, vector<const WorldMapInfoStruct*> > MapTables;
};
//////////////////////////////////////////////////////////////////////////
enum PowerRateTypeENUM
{
	EM_PowerRateType_HQ		,
	EM_PowerRateType_Tpexp	,
};
//////////////////////////////////////////////////////////////////////////
//���e�w�q�ƭ� (������n)
struct  GuildPKBaseInfoStruct
{
	int Time;
	int	Money;
	int Scroe;
};
//////////////////////////////////////////////////////////////////////////
struct NameAllLanguageStruct
{
	int	GUID;
	string Language[ EM_LanguageType_Count ];
};
//////////////////////////////////////////////////////////////////////////
class ObjectDataClass
{
public:
	//��l��
	static bool		InitShareMemory();
    static bool		Init( const char* InitPath, const char* datalanguage="TW", const char* stringlanguage="TW" , bool LoadAll = true );
    static bool		Release( );
	static int		RelationCount;
//	static bool		IsLoadAll;

	static map<string,MoneyExchangeKeyValueStruct>	_MoneyExchangeKeyValue;
	static vector<HouseEnergyCostStruct>			_HouseEnergyCost;
	static vector<wstring>							_SpecialName;
	static map< string, int >						_LanguageNameIDMap;
	static map< string, int >						_CountryNameIDMap;
	static string									_ErrorStr;		
	static bool										_LoadAll;
	string											_Datalanguage;
	string											_Stringlanguage;

	//������
    ObjectDataFileClass<GameObjDbStructEx,RuStreamFile>	*_ObjectDB;     

	//�I�ưӫ~
	vector<const char*>						_AccountStoreTypeName;

	//�������O
	vector<MotionTableStruct>				_MotionInfo;
	vector<TeachTableStruct>				_TeachInfo;

	vector<ColoringShopStruct>				_ColoringShop;

	//�a�ϸ�T
	vector<WorldMapInfoStruct>				_WorldMapInfo;
	map<string,const WorldMapInfoStruct*>	_WofldMapNameMap;
 	map<int,const WorldMapInfoStruct*>		_WofldMapIDMap;
	vector<SWorldMapInfoTable>				_WorldMapInfoTables;
	bool									_IsWorldMapInfoInited;

	map<int,SysKeyFunctionStruct>			_SysKeyFunction;
	map<string, int>						_SysKeyValeMap;

//	vector<GuildShopInfoStruct>				_GuildShop;
	vector<AccountShopInfoStruct>			_AccountShop;
//	vector<SysColorDefineStruct>			_SysColorDefine;
	vector<LotteryInfoTableStruct>			_LotteryInfoTable;
	vector<InherentRateTableStruct>			_InherentRateTable;

	vector<HouseShopInfoStruct>				_HouseShop;
	vector<RentFunctionTableStruct>			_RentFunctionTable;
	vector<MapPointStruct>					_MapMapInfo;
	vector< RelationMagicStruct >			_RelationMagic[ EM_RelationType_MaxRelation ];
	
	//vector<GuildFlagTableStruct>			_GuildFlagTable;
	vector< GuildLvInfoStruct >				_GuildLvTable;
	vector<std::string>						_GuildFlagInsigniaTable;
 	vector<std::string>						_GuildFlagBannerTable;

	vector<GuildHouseItemPageCostTableStruct>			_GuildHousePageCostTable;
	map< int , map< int , int > >						_GuildHouseNpcReplace;
	map< int , GuildHouseNpcReplaceCostTableStruct >	_GuildHouseNpcReplaceCost;
	vector<RuneSystemStruct>				_RuneSystemTable;


	vector<int>								_Tips;

	//vector< int >							_GlobalDropList;			//�Ҧ��Ǫ����|����

	//�r��Map 
	// �D�n�O�� KeyStr ���o�r��A�睊�ɥu�n���r���
	/*
#ifdef _DEBUG
	map< string, string >					_StringMap;
#else
	hash_map< string , string >				_StringMap;
#endif
	*/
	map< const char* , const char* , LessCharStr >	_StringMap;
	vector< char>							_AllString;
		
	//static hash_map< string , string >		_DBStringMap;		//��B�ӧ諸�r���
	static map< string , string >			_DBStringMap;		//��B�ӧ諸�r���

	vector< PowerRateTableStruct >			_PowerRate;

	map< int , GuildBuildingInfoTableStruct >	_GuildBuildingTable;
	vector< GuildBaseBuildingTableStruct >		_GuildBaseBuildingTable;

	//�Ш����
	CreateRoleInfoTableStruct				_CreateRoleTable;
	CreateCharacterColor					_MainColor;
	CreateCharacterColor					_SecondColor;

	vector<int>								_CreateHair;
	vector<int>								_CreateFace;
	vector<int>								_Hair;
	vector<int>								_Face;	

	CreateCharacterColor					_SkinColor[4];
	CreateCharacterColor					_HairColor[4];	

	vector<int>								_CreateRoleHair[6];		// �Ш��v��[0]�H�k [1]�H�k [2]���F�k [3]���F�k [4]�G�H�k [5]�G�H�k
	vector<int>								_CreateRoleFace[6];		// �Ш��y��[0]�H�k [1]�H�k [2]���F�k [3]���F�k [4]�G�H�k [5]�G�H�k
	vector<int>								_BeautyShopHair[6];		// �y���v��[0]�H�k [1]�H�k [2]���F�k [3]���F�k [4]�G�H�k [5]�G�H�k
	vector<int>								_BeautyShopFace[6];		// �y���y��[0]�H�k [1]�H�k [2]���F�k [3]���F�k [4]�G�H�k [5]�G�H�k

	int										_BaseEQ[EM_Max_Race][EM_Max_Vocation];		// [0]�H�� [1]���F
	SysKeyValueStruct						_SysValue;

	map< int , vector<GameObjDbStructEx*> >	_KillCountTitleInfo;

	//int										_RuneExchangeTable[50][50];		//�Ť�I����
	map< int , int >						_RuneExchangeTable;
	vector< int >							_RuneTable[200];				//�Ť��
//	string									_Language;						// �y������
	

//	vector< BaseGuildBuildingPointStruct >	_BaseGuildBuildingInfo;
	vector< GuildHouseBuildingInfoStruct >	_BaseGuildBuildingInfo;

	int										_tombID;						// �ӸO�s��
	bool									_OKFlag;
	//////////////////////////////////////////////////////////////////////////
	//�i���d�����
	vector<CultivatePetLearnMagicTableStruct>	_CultivatePetLearnMagicTable;
	vector<CultivatePetLifeSkillTableStruct>	_CultivatePetLifeSkillTable;
	//////////////////////////////////////////////////////////////////////////
	//�شӸ��
	//////////////////////////////////////////////////////////////////////////
//	vector< vector< PlantGrowActionTableStruct > >	_PlantGrowActionTable;
	map< int , vector<PlantProductTableStruct> >	_PlantProductTable;
	//////////////////////////////////////////////////////////////////////////
	//���~�ݩʽ�ժ�
	vector< GambleItemStateTableStruct >		_GambleItemStateTable;
	//////////////////////////////////////////////////////////////////////////
	ReaderClass< DB_GameObjDbStruct >*			_Reader_Base[ EM_ObjectClass_Max ];
	ReaderClass< DB_GameObjDbStruct >*			_Reader_Item[ EM_Item_Max ];
	//////////////////////////////////////////////////////////////////////////
	vector< bool >								_DisableTag;

	//////////////////////////////////////////////////////////////////////////
	//�������Ȫ�����
	//vector<GameObjDbStructEx*>					_QuestDetailTable;

	//////////////////////////////////////////////////////////////////////////
	//�k�����
	vector< ServantEventTableStruct >			_ServantEventList;
//	vector< ServantHireTableStruct >			_ServantHireList;

	map< int , ServantStartValueTableStruct >	_ServantStartValueMap;

	vector< int	>								_ServantHire_NPCObjID;
	vector< int	>								_ServantHire_LookNPCObjID;	//�ѷ����NPCID
	map< int , vector<int> >					_ServantHire_Character;
	map< int , vector<int> >					_ServantHire_FaceID;
	map< int , vector<int> >					_ServantHire_HairID;
	map< int , vector<int> >					_ServantHire_SkinColor;
	map< int , vector<int> >					_ServantHire_HairColor;
	map< int , vector<int> >					_ServantHire_ClothColor1;
	map< int , vector<int> >					_ServantHire_ClothColor2;

	//////////////////////////////////////////////////////////////////////////
	map< int , LifeSkillRareExStruct >			_LifeSkillRareExMap;
	//////////////////////////////////////////////////////////////////////////
	vector<int>									_PetCardList;				//�d���d�C��
	//////////////////////////////////////////////////////////////////////////
	string									_comma;

	////////////////////////////////////////////////////////
	//����Q�N�o���
	bool										_NotColdownMagicJob[DEF_MAX_COLDOWN_COUNT_JOB_];
protected:
	//�]�w�p��
//    void SetImageObjLink( );
    void SetMagicLink();
	//void SetQuestLink();
	void SetStringLink( );
	void SetRecipeLink();
	void SetLifeSillKeyItem();
	void SetMagicJobInfo();
	bool CheckInLearnMagic(int IgnoreID, int MagicID);
	void SetTitleLink();
	void SetRuneLink();



	//////////////////////////////////////////////////////////////////////////
	void InitReader();
	bool LoadObjectFile();
	bool LoadTableFile();
	bool LoadStringFile();
	void InitSysValue();
	//////////////////////////////////////////////////////////////////////////
	void SetStringLink( GameObjDbStructEx* ObjDB );
	void SetQuestLink( GameObjDbStructEx* ObjDB );
	//////////////////////////////////////////////////////////////////////////
	void LoadBodyObject();
	bool LoadDisableTagFile();

	//void InitQuestDetailTable();

	void InitWorldMapInfo();
	////////////////////////////////////
	//�k�N���o�Qcolddown
	void InitNotColdownJob();
public:
    ObjectDataClass( const char* InitPath, const char* datalanguage, const char* stringlanguage );
    ~ObjectDataClass( );
 
	
    GameObjDbStructEx*                      GetObj( int ObjID );
    ObjectDataFileClass<GameObjDbStructEx,RuStreamFile>*	ObjectDB();     

	// ���o���~�~���C��
	int GetItemQuality(GameObjDbStruct* itemDB, ItemFieldStruct* itemField=NULL);
	int GetItemQualityColor(int quality);
	const char* GetItemQualityTextColor(int quality);
	string GetItemColorName(ItemFieldStruct* itemField, GameObjDbStructEx* itemDB=NULL);
	int	GetGoodEvilColor(GoodEvilTypeENUM type);

	//�]�w�ѽ���
	//�̷� ���[�ݩʪ� ���o�Y���Ū����[���ݩ�
	//�D�n�Ω󪫫~����
//	int		GetItemAttribute( int LV , GameObjDbStructEx* EqDB  );	

	int				GetTreasureObjID()							{ return _SysValue.TreasureBaseObjID; }
	int				GetTeleportID()								{ return _SysValue.Teleport_PointObjID; }
	int				GetTombID()									{ return _tombID; }
	
	//�� KeyStr ���o�r��
	bool			CheckedString( const char* Key );
	const char*		GetString( const char* Key );
	const char*		GetSpecialString(const char* Key);

	// ���o¾�~�N���C��
	int				GetClassColorByID(Voc_ENUM voc);

	// ���o¾�~�ѧO�W��
	const char*		GetClassTokenByID(Voc_ENUM voc);

	// ���o¾�~�W��
	const char*		GetClassNameByID(Voc_ENUM voc);

	// ���o¾�~�s��
	Voc_ENUM		GetClassEnumByToken(const char* name);

	//���oEmote��T
	vector<MotionTableStruct>&  GetEmoteTable();
	int							GetEmoteInfoCount();
	MotionTableStruct*			GetEmoteInfoByIndex( int index );
	MotionTableStruct*			GetEmoteInfoByID( int ID );

	vector<TeachTableStruct>&	GetTeachInfo();

	// �j�M�I�ưӫ~
//	int AccountStoreSearchItem(int type, const char* keyStr=NULL);	
//	vector<AccountStoreItemTable>& GetAccountStoreTable()	{ return _AccountStoreTable; }
	vector<const char*>& GetAccountStoreTypeName()	{ return _AccountStoreTypeName; }

	// �Ш����
	CreateRoleInfoTableStruct*	CreateRoleTable(){ return &_CreateRoleTable; };
	vector<int>&				GetCreateHair()		{ return _CreateHair; };
	vector<int>&				GetCreateFace()		{ return _CreateFace; };
	CreateCharacterColor& GetMainColor()			{ return _MainColor; }
	CreateCharacterColor& GetSecondColor()			{ return _SecondColor; }	
	vector<int>& GetHairInfo()						{ return _Hair; }
	vector<int>& GetFaceInfo()						{ return _Face; }
	int GetCreateBaseEQ(Race_ENUM race, Voc_ENUM voc, int index, int* outColor);

	CreateCharacterColor&		GetCreateSkinColor(Race_ENUM race);
	CreateCharacterColor&		GetCreateHairColor(Race_ENUM race);

	SysKeyValueStruct&						SysValue(){ return _SysValue;};
	//�V��ө�
	vector<ColoringShopStruct>&				ColoringShop(){ return _ColoringShop;};
	ColoringShopStruct*						GetColoringShop( int ID );
	
 	//�a�ϸ�T
	vector<WorldMapInfoStruct>&				WorldMapInfo() { return _WorldMapInfo; };

	const WorldMapInfoStruct*				GetWorldMapInfoByName( const char* szName );
	const WorldMapInfoStruct*				GetWorldMapInfoByID( int mapID );	

	SWorldMapInfoTable&	GetWorldMapInfoTableByID( int WorldID );
	int					GetWorldMapInfoTableCount();
	SWorldMapInfoTable&	GetWorldMapInfoTableByIndex( int index );

	//���|�ө�
//	vector<GuildShopInfoStruct>&			GuildShop(){ return _GuildShop;};
	vector<HouseShopInfoStruct>&			HouseShop(){ return _HouseShop;};
	vector<HouseEnergyCostStruct>&			HouseEnergyCost(){ return _HouseEnergyCost;};
	vector<RentFunctionTableStruct>&		RentFunction(){ return _RentFunctionTable;};
	//vector<GuildFlagTableStruct>&			GuildFlagTable(){ return _GuildFlagTable; };
  	vector<std::string>&					GuildFlagInsigniaTable(){ return _GuildFlagInsigniaTable; };
	vector<std::string>&					GuildFlagBannerTable(){ return _GuildFlagBannerTable; };
	vector< GuildLvInfoStruct >&			GuildLvTable(){ return _GuildLvTable; };


	vector<CultivatePetLearnMagicTableStruct>&	GetCultivatePetLearnMagicTable()	{ return _CultivatePetLearnMagicTable; }

	int										GetGuildFlagInsigniaCount();
	int										GuildFlagBannerCount();
	const char*								GetGuildFlagInsigniaType( int index );
  	const char*								GuildFlagBannerType( int index );

	vector<AccountShopInfoStruct>&			AccountShop(){ return _AccountShop;};
	//vector<SysColorDefineStruct>&			SysColorDefine() { _SysColorDefine; };
	int SysKeyFunction( int KeyID );

	vector<GameObjDbStructEx*>&	KillCountTitleInfo( int KillGroupID );

	LotteryInfoTableStruct*	LotteryInfoTable( int SelectCount );

	int		GetSysKeyValue( string Key );
	int		GetSysKeyValueArg( char* keystr , int arg );

	int 	GetKeyMagicID( int iID );

	//���o��[��O�}���׵���
	int		GetInherentRate( int MonsterLv );

	// ���o�ɶ����
	void	SecondsToTimeAbbrev(float second, char* outText);

	// ���oICON�ϥ��ɮצW��
	// const char* GetIconFilePath(GameObjDbStructEx* objDB);

	// ���o�ĵ�����
	const char* GetEquipRefineFilename(int level);

	//���o�¤O�ഫ���v��
	float	PowerRate( int Power , PowerRateTypeENUM Type );

	// ���o���[��O��
	bool	GetWearEqTypeString(WearEqTypeENUM type, float value, char* inbuf, int insize);

	void	GetGuildWarDeclareTypeInfo( int Type , int& outTime , int& outMoney );

	// ���o���ܤ�r
	const char* GetTipText(int index = -1);

	// ���o�Ҧb�ϰ�W��
	const char*	GetZoneNameByID(int zoneID);

	void GetMoneyText(int money, char* outStr, int outSize);

	vector<int>& GlobalDropList() { return _SysValue.GlobalItemDrop; };
	vector<int>& GlobalDropList_BigMonster() { return _SysValue.GlobalItemDrop_BigMonster; };
	vector<int>& GlobalDropList_King() { return _SysValue.GlobalItemDrop_King; };
	vector<int>& GlobalDropList_WorldKing() { return _SysValue.GlobalItemDrop_WorldKing; };
	vector<int>& GlobalDropList_DepartmentStroe(){ return _SysValue.GlobalItemDrop_DepartmentStore; };

	int	GetMonetItemID(PriceTypeENUM type);

	bool GetItemFieldName(ItemFieldStruct* item, char* outStr, int outSize);

	bool GetItemFieldTradeState(ItemFieldStruct* item);

	// ��磌�~�ƦC����
	int CompareItemOrder(GameObjDbStruct* src, GameObjDbStruct* dest);

	//���o���Y�k�N
	RelationMagicStruct* RelationMagicInfo( RelationTypeENUM , int Lv );


	int	RuneExchange_Type( int Type1 , int Type2);	//�Ť�I����
	int RuneID( int Type , int Lv );			//�Ť��

	static GameObjDbStructEx*				RuneExchange( GameObjDbStructEx* Rune1 , GameObjDbStructEx* Rune2 );
	static GameObjDbStructEx*				NextRuneLv( GameObjDbStructEx* Rune );
	static SysKeyValueStruct*				St_SysValue();
	static int								St_GetSysKeyValue( const char* Str );

	map< int , GuildBuildingInfoTableStruct >&	GuildBuildingTable() { return _GuildBuildingTable; } ;
	GuildBuildingInfoTableStruct*	GuildBuilding( int ID );

	vector<GuildHouseItemPageCostTableStruct>& GetGuildHousePageCostTable(){ return _GuildHousePageCostTable; };
	GuildHouseNpcReplaceCostTableStruct* GetGuildHouseNpcReplaceCost( int type );

	//////////////////////////////////////////////////////////////////////////
	// �]�w �P Ū�� ExhcnageMoneyKeyValue
	//////////////////////////////////////////////////////////////////////////
	static bool	SetExhcnageMoneyKeyValue( int Count , int DataSize , char* Data );
	static bool	SetDBStringTable( int Count , int DataSize , char* Data );
	static int	GetMoneyKeyValue( const char* Key , int Pos );

	// �ˬd�W�٬O�_�X�k	
	static wchar_t tolowerUnic(wchar_t c);
	static wchar_t toupperUnic(wchar_t c);
	static void	tolowerUnicStr(wchar_t *p);
	static void	toupperUnicStr(wchar_t *p);
	
	static bool LoadSpecialCharacterName(const char* filename);
	static CharacterNameRulesENUM CheckGuildNameRules(const char* name, CountryTypeENUM countryType, std::wstring& outString);
	
	static CharacterNameRulesENUM CheckCharacterNameRules(const char* name, CountryTypeENUM countryType, std::wstring& outString);

	static	int		QueryCountryID			( const char* pszRegionString );
	static	int		QueryLanguageID			( const char* pszRegionString );
	static	string	QueryCountryString		( int iID );
	static	string	QueryLanguageString		( int iID );

	static	void	SetLanguageID			( int iDataLanguageID, int iStringLanguageID );
	
	static	int		GetDataLanguageID		();
	static	int		GetStringLanguageID		();

	static	int		m_iDataLanguageID;
	static	int		m_iStringLanguageID;

	static bool		InitRuneEngine( const char* FilePath );

	static int		GetLanguageType( const char* LanguageStr );

	static void		SetCharacterEquipColor(GameObjDbStruct* itemDB, GameObjectImageArmorPosENUM armorPos, int color[2]);

	static	Sex_ENUM	GetModelSex( const char* filename );

	//////////////////////////////////////////////////////////////////////////
	//�شӸ��
	//////////////////////////////////////////////////////////////////////////
//	vector< vector< PlantGrowActionTableStruct > >&	GetPlantGrowActionTable(){ return _PlantGrowActionTable; };
	map< int , vector<PlantProductTableStruct> >&	GetPlantProductTable(){ return _PlantProductTable; };
	//////////////////////////////////////////////////////////////////////////
	// ���o�Y���ؤl���������q����檺�O��
//	PlantGrowActionTableStruct&	GetPlantGrowAction( int Type , int GrowRate );
	PlantProductTableStruct&	GetPlantProduct( int SeedObjID , int Env_GrowPoint , int Env_RarePoint );
	//////////////////////////////////////////////////////////////////////////
	//���o�d���ǲߪk�N��
	CultivatePetLearnMagicTableStruct*	GetCultivatePetLearnMagic( int MagicID	, int MagicLv );	
	//���o�U�@���d���ǲ߳N�����
	CultivatePetLearnMagicTableStruct*	GetNextCultivatePetLearnMagic( int MagicID , int PreMagicLv );	

	//////////////////////////////////////////////////////////////////////////
	//���o�Ҧ��y�t������٦r�� 
	static void		GetNameAllLanguage( map<int,NameAllLanguageStruct> &Result );

	int									GetCultivatePetNumLifeSkill()				{ return (int)_CultivatePetLifeSkillTable.size(); }
	CultivatePetLifeSkillTableStruct*	GetCultivatePetLifeSkill(int index);

	static void		St_InitObjFunc	( void* ClassObj , void* ObjDB );
	//////////////////////////////////////////////////////////////////////////
	//�w�ɳB�z
	void	OnTimeProc( );

	const char* GetSkillValueString(SkillValueTypeENUM value);
	SkillValueTypeENUM GetSkillValueTypeENUM(const char* keyStr);
	//////////////////////////////////////////////////////////////////////////
	
	//�p�⪫�~��ժ��B
	static void	GetGamebleItemStateCost(  int Rare , int ItemStateAvgLv , int EqLv , int& OutCost , int& OutCostDiscount );
	//////////////////////////////////////////////////////////////////////////

	vector<int>& GetCreateRoleHair(Race_ENUM race, Sex_ENUM sex);
	vector<int>& GetCreateRoleFace(Race_ENUM race, Sex_ENUM sex);
	vector<int>& GetBeautyShopHair(Race_ENUM race, Sex_ENUM sex);
	vector<int>& GetBeautyShopFace(Race_ENUM race, Sex_ENUM sex);
	//////////////////////////////////////////////////////////////////////////

	//�վ�ƭ���ܦr��
	static void IntegerString (float value, char* inStr, int size);

	// �]�w�ƭȤ��j�Ÿ�
	void		SetCommaSymbol(const char* comma);

	// �[�J���j�Ÿ�
	void		InsertComma(int value, char* outStr, int outSize);


	static	HANDLE		m_ghShareMemory;

	//�k�N���o�Qcolddown
	bool GetNotColdownJob(int magicserial);

	int			GetLifeSkillRareExID( int ItemID , int AddRare );

	int			GetGuildHouseReplaceNpc( int type , int orgNpcID );

//	vector< ItemCombineStruct >*	GetItemCombineInfo( int hashCode );
	//�^�s�d���d����ID �p�G�^��-1 ��O�S���
	int			GetPetCardID( int orgObjID );
};


extern ObjectDataClass*    g_ObjectData;

