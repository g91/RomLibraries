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
	EM_CharacterNameRules_Rightful	= 0,			// 合法
	EM_CharacterNameRules_Short,					// 名稱太短,不合法
	EM_CharacterNameRules_Long,						// 名稱太長,不合法
	EM_CharacterNameRules_Wrongful,					// 有錯誤的符號或字元
	EM_CharacterNameRules_Special,					// 特殊名稱不能用
	EM_CharacterNameRules_BasicLetters,				// 只能使用基本英文字母
	EM_CharacterNameRules_EndLetter,				// 結尾字元錯誤(',")
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
	const WorldMapInfoStruct* WorldZone; //世界 大陸

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
//企畫定義數值 (比較重要)
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
	//初始化
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

	//物件資料
    ObjectDataFileClass<GameObjDbStructEx,RuStreamFile>	*_ObjectDB;     

	//點數商品
	vector<const char*>						_AccountStoreTypeName;

	//情緒指令
	vector<MotionTableStruct>				_MotionInfo;
	vector<TeachTableStruct>				_TeachInfo;

	vector<ColoringShopStruct>				_ColoringShop;

	//地圖資訊
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

	//vector< int >							_GlobalDropList;			//所有怪物都會掉的

	//字串Map 
	// 主要是用 KeyStr 取得字串，改版時只要換字串表
	/*
#ifdef _DEBUG
	map< string, string >					_StringMap;
#else
	hash_map< string , string >				_StringMap;
#endif
	*/
	map< const char* , const char* , LessCharStr >	_StringMap;
	vector< char>							_AllString;
		
	//static hash_map< string , string >		_DBStringMap;		//營運商改的字串表
	static map< string , string >			_DBStringMap;		//營運商改的字串表

	vector< PowerRateTableStruct >			_PowerRate;

	map< int , GuildBuildingInfoTableStruct >	_GuildBuildingTable;
	vector< GuildBaseBuildingTableStruct >		_GuildBaseBuildingTable;

	//創角資料
	CreateRoleInfoTableStruct				_CreateRoleTable;
	CreateCharacterColor					_MainColor;
	CreateCharacterColor					_SecondColor;

	vector<int>								_CreateHair;
	vector<int>								_CreateFace;
	vector<int>								_Hair;
	vector<int>								_Face;	

	CreateCharacterColor					_SkinColor[4];
	CreateCharacterColor					_HairColor[4];	

	vector<int>								_CreateRoleHair[6];		// 創角髮型[0]人男 [1]人女 [2]精靈男 [3]精靈女 [4]矮人男 [5]矮人女
	vector<int>								_CreateRoleFace[6];		// 創角臉型[0]人男 [1]人女 [2]精靈男 [3]精靈女 [4]矮人男 [5]矮人女
	vector<int>								_BeautyShopHair[6];		// 造型髮型[0]人男 [1]人女 [2]精靈男 [3]精靈女 [4]矮人男 [5]矮人女
	vector<int>								_BeautyShopFace[6];		// 造型臉型[0]人男 [1]人女 [2]精靈男 [3]精靈女 [4]矮人男 [5]矮人女

	int										_BaseEQ[EM_Max_Race][EM_Max_Vocation];		// [0]人類 [1]精靈
	SysKeyValueStruct						_SysValue;

	map< int , vector<GameObjDbStructEx*> >	_KillCountTitleInfo;

	//int										_RuneExchangeTable[50][50];		//符文兌換表
	map< int , int >						_RuneExchangeTable;
	vector< int >							_RuneTable[200];				//符文表
//	string									_Language;						// 語言版本
	

//	vector< BaseGuildBuildingPointStruct >	_BaseGuildBuildingInfo;
	vector< GuildHouseBuildingInfoStruct >	_BaseGuildBuildingInfo;

	int										_tombID;						// 墓碑編號
	bool									_OKFlag;
	//////////////////////////////////////////////////////////////////////////
	//養成寵物資料
	vector<CultivatePetLearnMagicTableStruct>	_CultivatePetLearnMagicTable;
	vector<CultivatePetLifeSkillTableStruct>	_CultivatePetLifeSkillTable;
	//////////////////////////////////////////////////////////////////////////
	//種植資料
	//////////////////////////////////////////////////////////////////////////
//	vector< vector< PlantGrowActionTableStruct > >	_PlantGrowActionTable;
	map< int , vector<PlantProductTableStruct> >	_PlantProductTable;
	//////////////////////////////////////////////////////////////////////////
	//物品屬性賭博表
	vector< GambleItemStateTableStruct >		_GambleItemStateTable;
	//////////////////////////////////////////////////////////////////////////
	ReaderClass< DB_GameObjDbStruct >*			_Reader_Base[ EM_ObjectClass_Max ];
	ReaderClass< DB_GameObjDbStruct >*			_Reader_Item[ EM_Item_Max ];
	//////////////////////////////////////////////////////////////////////////
	vector< bool >								_DisableTag;

	//////////////////////////////////////////////////////////////////////////
	//全部任務的指標
	//vector<GameObjDbStructEx*>					_QuestDetailTable;

	//////////////////////////////////////////////////////////////////////////
	//女僕資料
	vector< ServantEventTableStruct >			_ServantEventList;
//	vector< ServantHireTableStruct >			_ServantHireList;

	map< int , ServantStartValueTableStruct >	_ServantStartValueMap;

	vector< int	>								_ServantHire_NPCObjID;
	vector< int	>								_ServantHire_LookNPCObjID;	//參照顯示NPCID
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
	vector<int>									_PetCardList;				//寵物卡列表
	//////////////////////////////////////////////////////////////////////////
	string									_comma;

	////////////////////////////////////////////////////////
	//不能被冷卻資料
	bool										_NotColdownMagicJob[DEF_MAX_COLDOWN_COUNT_JOB_];
protected:
	//設定聯結
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
	//法術不得被colddown
	void InitNotColdownJob();
public:
    ObjectDataClass( const char* InitPath, const char* datalanguage, const char* stringlanguage );
    ~ObjectDataClass( );
 
	
    GameObjDbStructEx*                      GetObj( int ObjID );
    ObjectDataFileClass<GameObjDbStructEx,RuStreamFile>*	ObjectDB();     

	// 取得物品品質顏色
	int GetItemQuality(GameObjDbStruct* itemDB, ItemFieldStruct* itemField=NULL);
	int GetItemQualityColor(int quality);
	const char* GetItemQualityTextColor(int quality);
	string GetItemColorName(ItemFieldStruct* itemField, GameObjDbStructEx* itemDB=NULL);
	int	GetGoodEvilColor(GoodEvilTypeENUM type);

	//設定天賦資料
	//依照 附加屬性表 取得某等級的附加的屬性
	//主要用於物品掉落
//	int		GetItemAttribute( int LV , GameObjDbStructEx* EqDB  );	

	int				GetTreasureObjID()							{ return _SysValue.TreasureBaseObjID; }
	int				GetTeleportID()								{ return _SysValue.Teleport_PointObjID; }
	int				GetTombID()									{ return _tombID; }
	
	//用 KeyStr 取得字串
	bool			CheckedString( const char* Key );
	const char*		GetString( const char* Key );
	const char*		GetSpecialString(const char* Key);

	// 取得職業代表顏色
	int				GetClassColorByID(Voc_ENUM voc);

	// 取得職業識別名稱
	const char*		GetClassTokenByID(Voc_ENUM voc);

	// 取得職業名稱
	const char*		GetClassNameByID(Voc_ENUM voc);

	// 取得職業編號
	Voc_ENUM		GetClassEnumByToken(const char* name);

	//取得Emote資訊
	vector<MotionTableStruct>&  GetEmoteTable();
	int							GetEmoteInfoCount();
	MotionTableStruct*			GetEmoteInfoByIndex( int index );
	MotionTableStruct*			GetEmoteInfoByID( int ID );

	vector<TeachTableStruct>&	GetTeachInfo();

	// 搜尋點數商品
//	int AccountStoreSearchItem(int type, const char* keyStr=NULL);	
//	vector<AccountStoreItemTable>& GetAccountStoreTable()	{ return _AccountStoreTable; }
	vector<const char*>& GetAccountStoreTypeName()	{ return _AccountStoreTypeName; }

	// 創角資料
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
	//染色商店
	vector<ColoringShopStruct>&				ColoringShop(){ return _ColoringShop;};
	ColoringShopStruct*						GetColoringShop( int ID );
	
 	//地圖資訊
	vector<WorldMapInfoStruct>&				WorldMapInfo() { return _WorldMapInfo; };

	const WorldMapInfoStruct*				GetWorldMapInfoByName( const char* szName );
	const WorldMapInfoStruct*				GetWorldMapInfoByID( int mapID );	

	SWorldMapInfoTable&	GetWorldMapInfoTableByID( int WorldID );
	int					GetWorldMapInfoTableCount();
	SWorldMapInfoTable&	GetWorldMapInfoTableByIndex( int index );

	//公會商店
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

	//取得賦加能力稀有度等級
	int		GetInherentRate( int MonsterLv );

	// 取得時間單位
	void	SecondsToTimeAbbrev(float second, char* outText);

	// 取得ICON圖示檔案名稱
	// const char* GetIconFilePath(GameObjDbStructEx* objDB);

	// 取得衝等等級
	const char* GetEquipRefineFilename(int level);

	//取得威力轉換倍率值
	float	PowerRate( int Power , PowerRateTypeENUM Type );

	// 取得附加能力值
	bool	GetWearEqTypeString(WearEqTypeENUM type, float value, char* inbuf, int insize);

	void	GetGuildWarDeclareTypeInfo( int Type , int& outTime , int& outMoney );

	// 取得提示文字
	const char* GetTipText(int index = -1);

	// 取得所在區域名稱
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

	// 比對物品排列順序
	int CompareItemOrder(GameObjDbStruct* src, GameObjDbStruct* dest);

	//取得關係法術
	RelationMagicStruct* RelationMagicInfo( RelationTypeENUM , int Lv );


	int	RuneExchange_Type( int Type1 , int Type2);	//符文兌換表
	int RuneID( int Type , int Lv );			//符文表

	static GameObjDbStructEx*				RuneExchange( GameObjDbStructEx* Rune1 , GameObjDbStructEx* Rune2 );
	static GameObjDbStructEx*				NextRuneLv( GameObjDbStructEx* Rune );
	static SysKeyValueStruct*				St_SysValue();
	static int								St_GetSysKeyValue( const char* Str );

	map< int , GuildBuildingInfoTableStruct >&	GuildBuildingTable() { return _GuildBuildingTable; } ;
	GuildBuildingInfoTableStruct*	GuildBuilding( int ID );

	vector<GuildHouseItemPageCostTableStruct>& GetGuildHousePageCostTable(){ return _GuildHousePageCostTable; };
	GuildHouseNpcReplaceCostTableStruct* GetGuildHouseNpcReplaceCost( int type );

	//////////////////////////////////////////////////////////////////////////
	// 設定 與 讀取 ExhcnageMoneyKeyValue
	//////////////////////////////////////////////////////////////////////////
	static bool	SetExhcnageMoneyKeyValue( int Count , int DataSize , char* Data );
	static bool	SetDBStringTable( int Count , int DataSize , char* Data );
	static int	GetMoneyKeyValue( const char* Key , int Pos );

	// 檢查名稱是否合法	
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
	//種植資料
	//////////////////////////////////////////////////////////////////////////
//	vector< vector< PlantGrowActionTableStruct > >&	GetPlantGrowActionTable(){ return _PlantGrowActionTable; };
	map< int , vector<PlantProductTableStruct> >&	GetPlantProductTable(){ return _PlantProductTable; };
	//////////////////////////////////////////////////////////////////////////
	// 取得某類種子的成長階段能執行的是件
//	PlantGrowActionTableStruct&	GetPlantGrowAction( int Type , int GrowRate );
	PlantProductTableStruct&	GetPlantProduct( int SeedObjID , int Env_GrowPoint , int Env_RarePoint );
	//////////////////////////////////////////////////////////////////////////
	//取得寵物學習法術表
	CultivatePetLearnMagicTableStruct*	GetCultivatePetLearnMagic( int MagicID	, int MagicLv );	
	//取得下一個寵物學習術的資料
	CultivatePetLearnMagicTableStruct*	GetNextCultivatePetLearnMagic( int MagicID , int PreMagicLv );	

	//////////////////////////////////////////////////////////////////////////
	//取得所有語系物件明稱字串 
	static void		GetNameAllLanguage( map<int,NameAllLanguageStruct> &Result );

	int									GetCultivatePetNumLifeSkill()				{ return (int)_CultivatePetLifeSkillTable.size(); }
	CultivatePetLifeSkillTableStruct*	GetCultivatePetLifeSkill(int index);

	static void		St_InitObjFunc	( void* ClassObj , void* ObjDB );
	//////////////////////////////////////////////////////////////////////////
	//定時處理
	void	OnTimeProc( );

	const char* GetSkillValueString(SkillValueTypeENUM value);
	SkillValueTypeENUM GetSkillValueTypeENUM(const char* keyStr);
	//////////////////////////////////////////////////////////////////////////
	
	//計算物品賭博金額
	static void	GetGamebleItemStateCost(  int Rare , int ItemStateAvgLv , int EqLv , int& OutCost , int& OutCostDiscount );
	//////////////////////////////////////////////////////////////////////////

	vector<int>& GetCreateRoleHair(Race_ENUM race, Sex_ENUM sex);
	vector<int>& GetCreateRoleFace(Race_ENUM race, Sex_ENUM sex);
	vector<int>& GetBeautyShopHair(Race_ENUM race, Sex_ENUM sex);
	vector<int>& GetBeautyShopFace(Race_ENUM race, Sex_ENUM sex);
	//////////////////////////////////////////////////////////////////////////

	//調整數值顯示字串
	static void IntegerString (float value, char* inStr, int size);

	// 設定數值分隔符號
	void		SetCommaSymbol(const char* comma);

	// 加入分隔符號
	void		InsertComma(int value, char* outStr, int outSize);


	static	HANDLE		m_ghShareMemory;

	//法術不得被colddown
	bool GetNotColdownJob(int magicserial);

	int			GetLifeSkillRareExID( int ItemID , int AddRare );

	int			GetGuildHouseReplaceNpc( int type , int orgNpcID );

//	vector< ItemCombineStruct >*	GetItemCombineInfo( int hashCode );
	//回存寵物卡對應ID 如果回傳-1 表是沒找到
	int			GetPetCardID( int orgObjID );
};


extern ObjectDataClass*    g_ObjectData;

