#pragma once
#include "ObjectStruct.h"
#include "ReaderClass/ReaderClass.h"
struct	DB_GameObjDbIndexStruct
{
	int     GUID;
	char    Name[32];
	int     ImageID;
	char	FuncName[32];
	char	DataGroup[32];
	int		NodeID;
};
struct DBProcRateStruct
{
	union
	{
		int Rate;
		struct
		{
			int SetDataOK : 1;
			int Set2DOK : 1;
			int Set3DOK : 1;
			int SetTextureOK : 1;
			int TestOK : 1;
		};
	};
};
struct DB_ScriptDBStruct
{
	int     Type;
	char    Script[51000];
	char    Name[64];
	char    Arg[128];
};
struct	DB_GameObjDbStruct
{
	GameObjDbStruct GameData;
	DB_ScriptDBStruct ScriptInfo;
	short   IsLock;
	short   IsModify;
	char    LastModifyDate[32];
	char    LastModifyAccount[32];
	char    LockAccount[32];
	char	DataGroup[32];
	int     Zone[3];
	DBProcRateStruct    ProcRate;
	int		ZoneID;
	int		TagID;
	int		VersionID;
	bool	Translate;
	void Init()
	{
	}
};
struct	GameObjDbStructEx : public GameObjDbStruct
{
	bool IsEmpty();
	bool IsNPC();
	bool IsQuestNPC();
	bool IsQuestDetail();
	bool IsItem();
	bool IsItem_Pure();
	bool IsWeapon();
	bool IsArmor();
	bool IsKeyItem();
	bool IsMine();
	bool IsMagicBase();
	bool IsMagicCollect();
	bool IsShop();
	bool IsAttribute();
	bool IsFlagObj();
	bool IsRune();
	bool IsSuit();
	bool IsQuest();
	bool IsBody();
	bool IsImage();
	bool IsLearnMagic();
	bool IsPlayer();
	bool IsTitle();
	bool IsRecipe();
	bool IsTreasure();
	bool IsEqRefineAbility();
	bool IsCamp();
	bool IsZone();
	bool IsCreateRole();
	bool IsCard();
	bool IsPhantom();
	bool IsEquip();
	bool IsRestore();
	bool IsSpecial();
	bool IsSeed();
	bool IsPot();
	bool IsManure();
	char* GetName();
	char* GetPluralName();
	bool IsAssistMagic();
	bool	CheckItemAndEqSkill(EqWearSkillStruct EqSkill);
	bool	CheckInherentAbility(GameObjDbStructEx* AbilityDB, bool CheckLv = true);
	bool	CheckInherentAbility(AddonPosTypeStruct& Pos, bool CheckLv = true);
	ObjectModeStruct  GetModeFlag();
	static  char* GetWearEqTypeENUMString(WearEqTypeENUM ID);
	static	char* GetWeaponTypeENUMString(GameObjectWeaponENUM Type);
	static	char* GetWeaponPosENUMString(GameObjectWeaponPosENUM Pos);
	static	char* GetArmorTypeENUMString(GameObjectArmorENUM Type);
	static	char* GetArmorPosENUMString(GameObjectArmorPosENUM Pos);
	static	char* GetItemTypeENUMString(ItemType_ENUM Type);
	static	int		GetandClearWearEqValue(GameObjectWearEqStruct& Ability, WearEqTypeENUM Type);
	static	int		GetWearEqValue(GameObjectWearEqStruct& Ability, WearEqTypeENUM Type, int begin = 0);
	static	int		GetWeaponDMG(GameObjectWearEqStruct& Ability);
	static	int		GetArmorDEF(GameObjectWearEqStruct& Ability);
	static	int		GetWeaponAtkSpeed(GameObjectWearEqStruct& Ability);
	static	int		GetWearEqDurable(GameObjectWearEqStruct& Ability);
	static	int		GetWeaponMATK(GameObjectWearEqStruct& Ability);
	static	int		GetArmorMDEF(GameObjectWearEqStruct& Ability);
	static  char* GetMagicCheckFunctionENUMString(MagicCheckFunctionENUM ID);
	static void ObjReader_InitBaseCommonObj(DB_GameObjDbStruct*& Pt, ReaderClass< DB_GameObjDbStruct >& Reader);
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_AddPower();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Armor();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Body();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Flag();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Image();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Item();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_KeyItem();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_LearnMagic();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_MagicBase();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Mine();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_NPC();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Player();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_QuestDetail();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_QuestNPC();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Recipe();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Rune();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Script();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Shop();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Suit();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Title();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Weapon();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Camp();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Treasure();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_MagicCollect();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_EqRefineAbility();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Zone();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_CreateRole();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Card();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_PE();
	static ReaderClass< DB_GameObjDbStruct >* ObjReader_Phantom();
	static ReaderClass< CultivatePetLearnMagicTableStruct >* ObjReader_CultivatePetLearnMagic();
	static ReaderClass< CultivatePetLifeSkillTableStruct >* ObjReader_CultivatePetLifeSkill();
	static ReaderClass< PlantProductTableStruct >* ObjReader_PlantProduct();
	static ReaderClass< GambleItemStateTableStruct >* ObjReader_GambleItemState();
	static ReaderClass< ServantEventTableStruct >* ObjReader_ServantEvent();
	static ReaderClass< ServantHireTableStruct >* ObjReader_ServantHire();
	static ReaderClass< ServantStartValueTableStruct >* ObjReader_ServantStartValue();
	static ReaderClass< ItemCombineStruct >* ObjReader_ItemCombine();
	static ReaderClass< RuneExchangeExTableStruct >* ObjReader_RuneExchangeEx();
	static ReaderClass< LifeSkillRareExStruct >* ObjReader_LifeSkillRareEx();
	static ReaderClass< GuildHouseNpcReplaceTableStruct >* ObjReader_GuildHouseNpcReplace();
	static ReaderClass< GuildHouseNpcReplaceCostTableStruct >* ObjReader_GuildHouseNpcReplaceCost();
	static ReaderClass< RuneSystemStruct >* ObjReader_RuneSystem();
	static ReaderClass< PetCollectionTableStruct >* ObjReader_PetCollection();
	static ReaderClass< AbiParametersTableStruct >* ObjReader_AbiParameters();
	static ReaderClass< PhantomRankTableStruct >* ObjReader_PhantomRank();
	static ReaderClass< PhantomLvTableStruct >* ObjReader_PhantomLv();
	static ReaderClass< PhantomSkillTableStruct >* ObjReader_PhantomSkill();
	static	string			m_tempString;
};