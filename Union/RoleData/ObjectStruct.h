//----------------------------------------------------------------------------------------
//			№CАё©Т¦іЄ«Ґу°тҐ»ёк®Ж
//----------------------------------------------------------------------------------------
#ifndef	__OBJECTSTRUCT_H_2003_09_16__
#define __OBJECTSTRUCT_H_2003_09_16__
#pragma		warning (disable:4819)
#include	<windows.h>

//Є©Ґ»
#define		_DEF_VERSION	"v0.20"


#define		_MAX_NPC_QUEST_COUNT_			20			// NPC іМ¦hЇаµ№ЄєҐф°И
#define		_MAX_QUEST_COUNT_				30			// Ё¤¦віМ¦hҐiҐH±µЄє Quest јЖ¶q

#define		_MAX_QUEST_FLAG_COUNT_			8192		// Ё¤¦вҐШ«e§№¦Ё Quest ЄєјЖ¶q, ҐШ«eіМ¦hі]©w 2048
#define		_MAX_QUEST_KILLKIND_COUNT_		5			// ЁC¤@­У Quest «ь©wҐґ­ЛҐШјРЄєєШГю
#define		_MAX_QUEST_CLICKIND_COUNT_		5


//----------------------------------------------------------------------------------------
#define		_MAX_OBJ_NAME_STR_SIZE_		    32			//¦W¦r¦r¦кЄш«Ч©wёq
#define		_MAX_NPC_DROPITEM_COUNT_		5			//©wёqЄ««~іМ¦h±јёЁєШГюјЖ¶q
#define		_MAX_SKILL_COUNT_				30			//іМ¦h¦h¤ЦјЖ­И§ЮЇа(јфЅm«Ч)
#define		_MAX_RACE_COUNT_			    32			//32­УєШ±ЪјЖ¶q
#define		_MAX_VOCATION_COUNT_		    32			//32­УВѕ·~
#define		_MAX_SEX_COUNT_		            5			//©К§OјЖ¶q
#define		_MAX_FILENAME_SIZE_		        128			//АЙ®Ч¦WєЩ
#define     _MAX_LEVEL_                     200         //іМ°ЄµҐЇЕ
#define     _MAX_DROPITEM_COUNT_            15          //іМ¦h©ЗЄ«Ґi±јЄ««~јЖ¶q
#define     _MAX_DROP_ABILITY_COUNT_        6           //±јёЁЄ««~іМ¦h¦іµ№­УЄюҐ[ДЭ©К
#define     _MAX_NPC_EQSEL_COUNT_           5           //©ЗЄ«ҐiїпЄєЄZѕ№іМ¤j¶q
#define     _MAX_SPSkill_COUNT_             200          //іМ¤jВѕ·~±MДЭ§ЮЇа
#define     _MAX_NormalSkill_COUNT_         50          //іМ¤j¤@ЇлВѕ·~§ЮЇа

#define     _MAX_EQ_Skill_COUNT_		    20          //ЁдҐ¦Єє§ЮЇа
#define     _MAX_Lua_Skill_COUNT_			20          //ЁдҐ¦Єє§ЮЇа


#define     _MAX_Edit_SPSkill_COUNT_        200         //іМ¤jВѕ·~±MДЭ§ЮЇа
#define     _MAX_Edit_NormalSkill_COUNT_    300         //іМ¤j¤@ЇлВѕ·~§ЮЇа


#define     _MAX_PASSIVESKILL_COUNT_        30          //іМ¦h¦P®Й¦і¦h¤Ц­УіQ°К§ЮЇа
#define     _MAX_SELL_ITEM_SAVE_COUNT_      10          //©зЅжЄ««~Ё©ҐчіМ¤j¶q

#define     _MAX_SKILLVALUE_BASETYPE        20          //°тҐ»ЄZѕ№јфЅm«ЧЄє¤АєШБ`Гю (s+ s s- ..... )
#define     _MAX_SKILLVALUE_TYPE            30          //°тҐ»ЄZѕ№јфЅm«ЧЄєєШГю

#define		_MAX_QUESTLINK_COUNT			20			// ЁC­У NPC іМ¦h©M¦h¤Ц­УҐф°И¦і©ТГціs
#define		_MAX_SPEAKOPTION_COUNT_			10
#define		_MAX_SCRIPTBORDER_PAGE_			20			// ҐЫёO¤¶­±іМ¦h­¶јЖ
#define		_MAX_PET_SKILL_COUNT_			20			//ГdЄ«іМ¦h·|ґX­У§ЮЇа
//----------------------------------------------------------------------------------------
#define		_TABLE_BASE_VOCTABLE_			1000
#define		_TABLE_BASE_SEXTABLE_			2000
#define		_TABLE_BASE_FIXTABLE_			3000
//#define		_TABLE_BASE_VOCTABLE_			4000
#define		_TABLE_BASE_SKILLBASETABLE_		5000
#define		_TABLE_BASE_JOBSKILLTABLE_		6000
#define		_TABLE_BASE_JOBEQSKILLTABLE_	7000
#define		_TABLE_BASE_WEAPONARGTABLE_		8000
#define		_TABLE_BASE_ARMORARGTABLE_		9000
#define		_TABLE_BASE_BASEMAGICTABLE_		10000
//#define		_TABLE_BASE_RESISTARGTABLE_		11000

#define		_TABLE_BASE_EXPTABLE_			13000
//#define		_TABLE_BASE_EQREFINETABLE_		14000
#define		_TABLE_BASE_CREATEROLETABLE_	15000
//#define		_TABLE_BASE_DISSOLUTIONTABLE_	16000
#define		_TABLE_BASE_COMBOTABLETABLE_	17000
#define		_TABLE_BASE_MOTIONTABLE_		18000
#define		_TABLE_BASE_TEACHTABLE_			20000
#define		_TABLE_BASE_COLORINGSHOPTABLE_	21000
#define		_TABLE_BASE_WORLDMAPINFO_		22000
#define		_TABLE_BASE_GUILDLVINFO_		23000
#define		_TABLE_BASE_ACCOUNTSHOPINFO_	24000
//#define		_TABLE_BASE_SYSCOLORDEFINE_		25000
#define		_TABLE_BASE_SYSKEYFUNCTION_		26000
#define		_TABLE_BASE_LOTTERYINFO_		27000
#define		_TABLE_BASE_SYSKEYVALUE_		28000
#define		_TABLE_BASE_INHERENTRATE_		29000
#define		_TABLE_BASE_HOUSESHOPINFO_		30000
#define		_TABLE_BASE_HOUSEENERGRYCOST_	31000
#define		_TABLE_BASE_POWERRATE_			32000
#define		_TABLE_BASE_RENTFUNCTION_		33000
#define		_TABLE_BASE_GUIDFLAGTABLE_		34000
#define		_TABLE_BASE_MAPPOINT_			35000
#define		_TABLE_BASE_RELATIONMAGIC_		36000
#define		_TABLE_BASE_RUNEEXCHANGE_		37000
#define		_TABLE_BASE_GUIDBUILDING_		38000
#define		_TABLE_BASE_GUIDBASEBUILDING_	39000
#define		_TABLE_BASE_GUIDHOUSEITEMPAGE_	40000

#define		_TABLE_BASE_CULTIVATE_PET_BASE_			41000
#define		_TABLE_BASE_CULTIVATE_PET_LEARNMAGIC_	42000
#define		_TABLE_BASE_CULTIVATE_PET_LIFESKILL_	43000
//#define		_TABLE_BASE_CULTIVATE_PET_EVENT_		42000
//#define		_TABLE_BASE_CULTIVATE_PET_GROW_			43000
//#define		_TABLE_BASE_CULTIVATE_PET_TALK_			44000
//#define		_TABLE_BASE_CULTIVATE_PET_VALUE_EFFECT_	45000

#define		_TABLE_BASE_PLANT_GROW_ACTION_			46000
#define		_TABLE_BASE_PLANT_PRODUCT_				47000
#define		_TABLE_BASE_PLANT_GAMBLEITEMSTATE_		48000

#define		_TABLE_BASE_SERVANT_EVENT_				49000
#define		_TABLE_BASE_SERVANT_HIRE_				50000
#define		_TABLE_BASE_SERVANT_STRATVALUE_			51000
#define		_TABLE_BASE_ITEM_COMBINE_				52000
#define		_TABLE_BASE_LIFESKILL_RARE_EX_			53000
#define		_TABLE_BASE_GUILDHOUSE_NPC_REPLACE		54000
#define		_TABLE_BASE_GUILDHOUSE_NPC_REPLACE_COST	55000
#define		_TABLE_BASE_RUNESYSTEM_EVENT_			56000
#define		_TABLE_BASE_PETCOLLECTION_				57000
#define		_TABLE_BASE_ABIPARAMETER_				58000
#define		_TABLE_BASE_PHANTOMRANK_				59000
#define		_TABLE_BASE_PHANTOMLV_					60000
#define		_TABLE_BASE_PHANTOMSKILL_				61000
//----------------------------------------------------------------------------------------
#define Def_ObjectClass_Player				1000		// Є±®aЄ«Ґу
#define Def_ObjectClass_BodyObj				2000		// Ё­ЕйЄ«Ґу
#define Def_ObjectClass_NPC					100000		// NPC
#define Def_ObjectClass_QuestNPC			110000		// QuestNPC ( 110000-119999 )
#define Def_ObjectClass_Item				200000		// Є««~(200000) ёЛіЖ ЄZѕ№ Єчїъ 
#define Def_ObjectClass_Weapon				210000		// ЄZѕ№
#define Def_ObjectClass_Armor				220000		// ЁѕЁг
#define Def_ObjectClass_Phantom				250000		// ¤ЫЖF
#define Def_ObjectClass_Table				300000		// 
#define Def_ObjectClass_QuestCollect		400000		// ( 20000  ) Їаµ№¤© Quest Єє NPC ҐNё№
#define Def_ObjectClass_QuestDetail			420000		// ( 200000 ) Quest
#define Def_ObjectClass_MagicCollect		490000		// ЄkіN¶°¦Xёк®Ж
#define Def_ObjectClass_Magic				500000		// ЄkіNёк®Ж
#define Def_ObjectClass_Attribute   		510000		// ЄюҐ[Їа¤O
#define Def_ObjectClass_Rune           		520000		// ІЕ¤е
#define Def_ObjectClass_Title          		530000		// АY»О
#define Def_ObjectClass_KeyItem        		540000		// Key Item
#define Def_ObjectClass_KeyItemEx      		640000		// Key Item
#define Def_ObjectClass_Recipe        		550000		// °t¤иЄн
#define Def_ObjectClass_Mine          		560000		// ДqІЈ
#define Def_ObjectClass_Image          		570000		// №П§О
//#define Def_ObjectClass_Flag          		580000		// єX¤l
#define Def_ObjectClass_LearnMagic          590000		// ЄkіNѕЗІЯЄн
#define Def_ObjectClass_Shop                600000		// °У©±ёк°T
#define Def_ObjectClass_Suit                610000		// ®MёЛёк°T
#define Def_ObjectClass_MagicEx             620000		// ЄkіNёк®Ж ВXҐR
#define Def_ObjectClass_LuaScript           700000		// luaј@±ЎЄ«Ґу
#define Def_ObjectClass_Camp	            710000		// °}Азёк°T
#define Def_ObjectClass_Treasure            720000		// Д_Є«±јёЁЄн
#define Def_ObjectClass_CliLuaScript        730000		// luaј@±ЎЄ«Ґу
#define Def_ObjectClass_EqRefineAbility     740000		// ЅДµҐЇа¤O
#define Def_ObjectClass_Zone			    750000		// °П°мі]©wёк°T
#define Def_ObjectClass_CreateRole		    760000		// «ШЁ¤Є«Ґу
#define Def_ObjectClass_Card				770000		// Ґd¤щ
#define Def_ObjectClass_Flag          		780000		// єX¤l
#define Def_ObjectClass_PE          		790000		// ¤ЅІіҐф°И (790000 - 899999) PE : 790000 -791999 PH : 792000 - 809999, OB : 810000 - 854999, VAR : 855000 - 899999
#define Def_ObjectClass_ImageEx        		800000		// №П§О
#define Def_ObjectClass_MagicCollectEx		850000		// ЄkіN¶°¦Xёк®Ж
#define Def_ObjectClass_Item_Total			100000
#define Def_ObjectClass_QuestDetail_Total	 70000
#define Def_ObjectClass_QuestNPC_Total		 20000		// QuestNPC
#define Def_ObjectClass_KeyItem_Total		 10000
#define Def_ObjectClass_Recipe_Total		 10000
//----------------------------------------------------------------------------------------
// Quest ¬ЫГц
//----------------------------------------------------------------------------------------
#define _MAX_SIZE_QUEST_NAME				32
#define _MAX_SIZE_QUEST_DESC				256
#define _MAX_SIZE_QUEST_ACCEPT_DETAIL		1200
#define _MAX_SIZE_QUEST_UNCOMPLETE_DETAIL	400
#define _MAX_SIZE_QUEST_COMPLETE_DETAIL		1200
#define _MAX_SIZE_QUEST_NPC_TEXT			512
#define _MAX_SIZE_QUEST_CATALOG				32
#define _MAX_SIZE_QUEST_TYPE				8
#define _MAX_SIZE_QUEST_FLAG				3
#define _MAX_SIZE_QUEST_ITEM				5
#define _MAX_SIZE_QUEST_CONDITION			10
#define _MAX_SIZE_QUEST_SCRIPT				32
#define _MAX_SIZE64_QUEST_SCRIPT			64
//----------------------------------------------------------------------------------------
//AC Є««~·jґM¤АГю©w·N
//----------------------------------------------------------------------------------------
#define _DEF_ACSEARCHTYPE_EMPTY				-1

#define _DEF_ACSEARCHTYPE_WEAPON			0
#define _DEF_ACSEARCHTYPE_ARMOR				1
#define _DEF_ACSEARCHTYPE_SUPPLIES			2
#define _DEF_ACSEARCHTYPE_STUFF				3
#define _DEF_ACSEARCHTYPE_RECIPE			4
#define _DEF_ACSEARCHTYPE_POWERUP			5
#define _DEF_ACSEARCHTYPE_CARD				6
#define _DEF_ACSEARCHTYPE_RIDE				7
#define _DEF_ACSEARCHTYPE_HOUSE				8
#define _DEF_ACSEARCHTYPE_SPECIAL			9
#define _DEF_ACSEARCHTYPE_COLLECTION		10
#define _DEF_ACSEARCHTYPE_OTHER				11
#define _DEF_ACSEARCHTYPE_MONEY				12

#define _DEF_ACSEARCHTYPE_WEAPON_SWORD		0
#define _DEF_ACSEARCHTYPE_WEAPON_AXE		1
#define _DEF_ACSEARCHTYPE_WEAPON_HAMMER		2
#define _DEF_ACSEARCHTYPE_WEAPON_DAGGER		3
#define _DEF_ACSEARCHTYPE_WEAPON_STAFF		4
#define _DEF_ACSEARCHTYPE_WEAPON_LONGWEAPON	5
#define _DEF_ACSEARCHTYPE_WEAPON_THROW		6
#define _DEF_ACSEARCHTYPE_WEAPON_OTHER		7

#define _DEF_ACSEARCHTYPE_WEAPON_X_ONEHAND	0
#define _DEF_ACSEARCHTYPE_WEAPON_X_TWOEHAND	1

#define _DEF_ACSEARCHTYPE_WEAPON_X_BOW		0
#define _DEF_ACSEARCHTYPE_WEAPON_X_COSSBOW	1
#define _DEF_ACSEARCHTYPE_WEAPON_X_AMMO		2

#define _DEF_ACSEARCHTYPE_ARMOR_HEAVYARMED	0
#define _DEF_ACSEARCHTYPE_ARMOR_LIGHTARMED	1
#define _DEF_ACSEARCHTYPE_ARMOR_LEATHER		2
#define _DEF_ACSEARCHTYPE_ARMOR_CLOTHES		3
#define _DEF_ACSEARCHTYPE_ARMOR_ROBE		3
#define _DEF_ACSEARCHTYPE_ARMOR_DRESS		4
#define _DEF_ACSEARCHTYPE_ARMOR_SECONDHAND	5
#define _DEF_ACSEARCHTYPE_ARMOR_MAGICTOOL	6
#define _DEF_ACSEARCHTYPE_ARMOR_ORNAMENT	7

#define _DEF_ACSEARCHTYPE_ARMOR_X_HEAD		0
#define _DEF_ACSEARCHTYPE_ARMOR_X_CLOTHES	1
#define _DEF_ACSEARCHTYPE_ARMOR_X_BELT		2
#define _DEF_ACSEARCHTYPE_ARMOR_X_PANTS		3
#define _DEF_ACSEARCHTYPE_ARMOR_X_GLOVES	4
#define _DEF_ACSEARCHTYPE_ARMOR_X_SHOES		5
#define _DEF_ACSEARCHTYPE_ARMOR_X_BACK		6
#define _DEF_ACSEARCHTYPE_ARMOR_X_SHOULDER	7

#define _DEF_ACSEARCHTYPE_ARMOR_DRESS_EARRING	0
#define _DEF_ACSEARCHTYPE_ARMOR_DRESS_RING		1
#define _DEF_ACSEARCHTYPE_ARMOR_DRESS_NECKLACE	2

#define _DEF_ACSEARCHTYPE_ARMOR_SECONDHAND_SHIELD		0
#define _DEF_ACSEARCHTYPE_ARMOR_SECONDHAND_IMPLEMENT	1


#define _DEF_ACSEARCHTYPE_STUFF_MINE			0		
#define _DEF_ACSEARCHTYPE_STUFF_WOOD			1
#define _DEF_ACSEARCHTYPE_STUFF_HERB			2
#define _DEF_ACSEARCHTYPE_STUFF_FOOD			3
#define _DEF_ACSEARCHTYPE_STUFF_MANUFACTIRERUNE	4
#define _DEF_ACSEARCHTYPE_STUFF_OTHERSTUFF		5

#define _DEF_ACSEARCHTYPE_SUPPLIES_FOOD			0	
#define _DEF_ACSEARCHTYPE_SUPPLIES_SWEETFOOD	1
#define _DEF_ACSEARCHTYPE_SUPPLIES_MEDICINE		2

#define _DEF_ACSEARCHTYPE_RECIPE_BLACKSMITH		0
#define _DEF_ACSEARCHTYPE_RECIPE_CARPENTER		1
#define _DEF_ACSEARCHTYPE_RECIPE_ARMOR			2
#define _DEF_ACSEARCHTYPE_RECIPE_TAILOR			3
#define _DEF_ACSEARCHTYPE_RECIPE_ALCHEMY		4
#define _DEF_ACSEARCHTYPE_RECIPE_COOK			5

#define _DEF_ACSEARCHTYPE_RUNE_MANUFACTURE	0
#define _DEF_ACSEARCHTYPE_RUNE_WEAPON		1
#define _DEF_ACSEARCHTYPE_RUNE_HEAD			2
#define _DEF_ACSEARCHTYPE_RUNE_BODY			3
#define _DEF_ACSEARCHTYPE_RUNE_PANT			4
#define _DEF_ACSEARCHTYPE_RUNE_SPECIAL		5
#define _DEF_ACSEARCHTYPE_RUNE_ARMOR		6
#define _DEF_ACSEARCHTYPE_RUNE_ORNAMENT		7
#define _DEF_ACSEARCHTYPE_RUNE_SHIELD		8
#define _DEF_ACSEARCHTYPE_RUNE_MAGICTOOLS	9

#define _DEF_ACSEARCHTYPE_POWERUP_REFINE		0	
#define _DEF_ACSEARCHTYPE_POWERUP_RUNE			1	
#define _DEF_ACSEARCHTYPE_POWERUP_SMELTSTONE	2	

#define _DEF_ACSEARCHTYPE_HOUSE_CONTRACT	0
#define _DEF_ACSEARCHTYPE_HOUSE_FURNITURE	1
#define _DEF_ACSEARCHTYPE_HOUSE_STUDY		2

#define _DEF_ACSEARCHTYPE_MONEY_GAMEMONEY		0
#define _DEF_ACSEARCHTYPE_MONEY_ACCOUNTMONEY	1
#define _DEF_ACSEARCHTYPE_MONEY_BONUSMONEY		2
//----------------------------------------------------------------------------------------
//ЇS§OЁtІОЁПҐОЄєЄ««~
#define _DEF_ITEMID_MONEY_		200000
#define _DEF_ITEMID_LOTTERY_	200001
//----------------------------------------------------------------------------------------
enum    YesNoENUM
{
	EM_No,
	EM_Yes,
	EM_Unknow,
};
//-----------------------------------------------------------------------------
enum PriceTypeENUM
{
	EM_ACPriceType_None = -1,
	EM_ACPriceType_GameMoney,		//№CАё№ф
	EM_ACPriceType_AccountMoney,		//±bё№№ф
	EM_ACPriceType_BonusMoney,		//¬х§Q№ф
	EM_ACPriceType_Phirius,		//ҐІ¦Х¤B
	EM_ACPriceType_Honor,		//ѕФіхєaЕA
	EM_ACPriceType_Arena,		//Дv§ЮіхВIјЖ(Arena)
	EM_ACPriceType_GuildWarEnergy,		//¤Ѕ·|ѕФЇа¶q
	EM_ACPriceType_GuildWarHonor,		//¤Ѕ·|ѕФѕФБZ
	EM_ACPriceType_TrialBadge,		//ёХ·ТАІі№
	EM_ACPriceType_Relics,		//ҐjҐNїтЄ«
	EM_ACPriceType_Dreamland,		//№Ъ№ТҐэ»є¦L°O
	EM_ACPriceType_PhiriusShell,		//ҐІєёҐЕ¶кЁ©
	EM_ACPriceType_EnergyJustice,		//ҐїёqЇа¶q
	EM_ACPriceType_ProofLegend,		//¶З»Ў¤§ГТ
	EM_ACPriceType_DuelistReward,		//«i¤hѕ±і№
	EM_ACPriceType_MirrorCoin,		//Ги¤ЫАІ¦L

};
//-----------------------------------------------------------------------------
enum BaseMagicTypeENUM
{
	EM_BaseMagic_Unbeatable,
	EM_BaseMagic_Slow,
	EM_BaseMagic_ReviveWeak,		//­«ҐН°I®z
	EM_BaseMagic_NoSearchEnemy,		//­«ҐН¤ЈЇБјД
	EM_BaseMagic_PlayerUnbeatable,		//Є±®a­«ҐН®ЙµLјД
	EM_BaseMagic_Guilty,
	EM_BaseMagic_PKDead1,
	EM_BaseMagic_PKDead2,
	EM_BaseMagic_PKDead3,
	EM_BaseMagic_PKHunter,
	EM_BaseMagic_Reserve5,
	EM_BaseMagic_Reserve6,
	EM_BaseMagic_Reserve7,
	EM_BaseMagic_Reserve8,
	EM_BaseMagic_Reserve9,
	EM_BaseMagic_Reserve10,

	EM_BaseMagic_MaxCount,
};
//////////////////////////////////////////////////////////////////////////
enum RelationTypeENUM
{
	EM_RelationType_None = -1,	//µLГц«Y
	EM_RelationType_Stranger,	//ІцҐН¤H
	EM_RelationType_Friend,	//¦n¤Н
	EM_RelationType_Enemy,	//¤і¤H

	EM_RelationType_Student,	//ѕЗҐН
	EM_RelationType_Teacher,	//¦С®v
	EM_RelationType_Wife,	//©d¤l
	EM_RelationType_Husband,	//ҐэҐН
	EM_RelationType_Lover,	//±Ў¤H
	EM_RelationType_Family,	//їЛ¤H
	EM_RelationType_BestFriend,	//ј°¤Н
	EM_RelationType_BadCompany,	//·l¤Н

	EM_RelationType_MaxRelation,
};

//-----------------------------------------------------------------------------
//Ё¤¦вЄ¬єA
//----------------------------------------------------------------------------------------
struct	GameObjDbStruct;
typedef union {
	int value;
	struct {
		bool		isUnholster : 1;		// true ЁъҐXЄZѕ№	Script ҐNЅX 1
		bool		isWalk : 1;		// true Ё«ёф		Script ҐNЅX 2
		bool		isCombat : 1;		// true ѕФ°«¤¤		Script ҐNЅX 4
		bool		isDead : 1;		// true ¦є¤`¤¤		Script ҐNЅX 8
		bool		isSpell : 1;		// true ¬IЄk¤¤		Script ҐNЅX 16

		bool		isSit : 1;		// true §¤¤U		Script ҐNЅX 32
		bool		isSleep : 1;		// true єОД±
		bool		isCrouch : 1;		// true ГЫ¤U
		bool		isCrafting : 1;		// true Ґ[¤u
		bool		isSitChair : 1;		// true §¤¦bґИ¤l¤W
	};

}RoleSpriteStatus;
//----------------------------------------------------------------------------------------
//№CАёЄ«ҐуГю«¬¦CБ|
//----------------------------------------------------------------------------------------
enum	GameObjectClassificationENUM
{
	EM_ObjectClass_None = -1,
	EM_ObjectClass_Player = 0,
	EM_ObjectClass_NPC,		// 1	
	EM_ObjectClass_Item,		//Є««~	ёЛіЖ ЄZѕ№ Єчїъ 2
	EM_ObjectClass_MagicBase,		// 3 
	EM_ObjectClass_BodyObj,		// 4 
	EM_ObjectClass_Attribute,		//ґ_Ґ[ДЭ©К	
	EM_ObjectClass_QuestCollect,
	EM_ObjectClass_QuestDetail,
	EM_ObjectClass_Title,		//АY»О
	EM_ObjectClass_KeyItem,		//­«­nЄ««~
	EM_ObjectClass_Recipe,		//°t¤иЄн
	EM_ObjectClass_Mine,		//ДqІЈ
	EM_ObjectClass_Flag,		//єX¤lёк®Ж
	EM_ObjectClass_Image,		//№П¶Hёк®Ж
	EM_ObjectClass_QuestNPC,		//Ґф°И
	EM_ObjectClass_LearnMagic,		//ЄkіNѕЗІЯ
	EM_ObjectClass_Shop,		//°У©±
	EM_ObjectClass_Suit,		//®MёЛ
	EM_ObjectClass_LuaScript,		//ј@±Ў
	EM_ObjectClass_Camp,		//°}Аз
	EM_ObjectClass_Treasure,		//Д_Ѕc
	EM_ObjectClass_MagicCollect,		//ЄkіN¶°¦X
	EM_ObjectClass_EqRefineAbility,		//ёЛіЖєл·ТЄн
	EM_ObjectClass_Zone,		//°П°м
	EM_ObjectClass_CreateRole,		//«ШЁ¤Є«Ґу
	EM_ObjectClass_PE,		//PEјЛЄO
	EM_ObjectClass_Phantom,		//¤ЫЖF

	EM_ObjectClass_Max,
	EM_GameObjectClassificationEM_FORCE_DWORD = 0x7fffffff,
};
//-------------------------------------------------------------------------------------
//єШ±Ъ¦CБ|
enum	Race_ENUM
{
	EM_Race_None = -1,
	EM_Race1 = 0,
	EM_Race2,
	EM_Race3,
	EM_Race4,
	EM_Race5,
	EM_Race_Undead,
	EM_Race7,
	EM_Race8,
	EM_Race9,
	EM_Race10,
	EM_Race11,
	EM_Race12,
	EM_Race13,
	EM_Race14,
	EM_Race15,
	EM_Race16,
	EM_Max_Race,
	EM_Race_EM_FORCE_DWORD = 0x7fffffff,
};
//-------------------------------------------------------------------------------------
//©К§O¦CБ|
enum	Sex_ENUM
{
	EM_Sex_Boy = 0,
	EM_Sex_Girl,
	EM_Sex_BigMonster,		//єл­^
	EM_Sex_King,		//¤э
	EM_Sex_WorldKing,		//Ґ@¬Й¤э
	EM_Sex_Max_Sex,
	EM_Sex_EM_FORCE_DWORD = 0x7fffffff,
};
//-------------------------------------------------------------------------------------
//Вѕ·~¦CБ|
enum	Voc_ENUM
{
	EM_Vocation_None = -1,       		//ЁS¦іВѕ·~
	EM_Vocation_GameMaster = 0,
	EM_Vocation_Warrior,				// ѕФ¤h
	EM_Vocation_Ranger,				// №C«L
	EM_Vocation_Rogue,				// јv¦жЄМ
	EM_Vocation_Wizard,				// Єk®v
	EM_Vocation_Priest,				// ІЅҐq
	EM_Vocation_Knight,				// ГM¤h
	EM_Vocation_RuneDancer,				// ґЛЄLЄЄЄМ
	EM_Vocation_Druid,				// ¦ЫµMЇ«©x
	EM_Vocation_Harpsyn,				// ЖFЇаЄМ
	EM_Vocation_Psyron,				// ІЕ¤е¤u¦K
	EM_Vocation_Duelist,				// °«ґС¤h
	EM_Vocation13,
	EM_Vocation14,
	EM_Vocation15,
	EM_Vocation16,
	EM_Max_Vocation,
	EM_Vocation_EM_FORCE_DWORD = 0x7fffffff,
};
//----------------------------------------------------------------------------------------
//°}АзГю«¬
//----------------------------------------------------------------------------------------
enum CampID_ENUM
{
	EM_CampID_Unknow = -1,
	EM_CampID_NewHand,
	EM_CampID_Good,
	EM_CampID_Evil,
	EM_CampID_Monster,
	EM_CampID_Wagon,
	EM_CampID_SNPC,
	EM_CampID_Visitor,
	EM_CampID_WF_A,
	EM_CampID_WF_B,
	EM_CampID_WF_C,
	EM_CampID_Enemy,
	EM_CampID_9,
	EM_CampID_10,
	EM_CampID_11,
	EM_CampID_12,
	EM_CampID_13,
	EM_CampID_14,
};
//----------------------------------------------------------------------------------------
//Вѕ·~єXјР
//----------------------------------------------------------------------------------------
struct VocationFlagStruct
{
	union
	{
		int		_Voc;
		struct
		{
			bool	Voc0 : 1;
			bool	Voc1 : 1;
			bool	Voc2 : 1;
			bool	Voc3 : 1;
			bool	Voc4 : 1;
			bool	Voc5 : 1;
			bool	Voc6 : 1;
			bool	Voc7 : 1;
			bool	Voc8 : 1;
			bool	Voc9 : 1;
			bool	Voc10 : 1;
			bool	Voc11 : 1;
		};
	};
};
//----------------------------------------------------------------------------------------
//єШ±ЪєXјР
//----------------------------------------------------------------------------------------
struct RaceFlagStruct
{
	union
	{
		int		_Race;
		struct
		{
			bool	Race0 : 1;
			bool	Race1 : 1;
			bool	Race2 : 1;
			bool	Race3 : 1;
			bool	Race4 : 1;
			bool	Race5 : 1;
		};
	};
};
//----------------------------------------------------------------------------------------
//©К§OєXјР
struct SexFlagStruct
{
	union
	{
		int		_Sex;
		struct
		{
			bool	Boy : 1;
			bool	Girl : 1;
			bool    Unknow : 1;
		};
	};
};
//----------------------------------------------------------------------------------------
// јЖЅи­pєв°СјЖ
//----------------------------------------------------------------------------------------
//©T©w°СјЖ
struct FixFightArgumentStructOld
{
	union
	{
		float Arg[100];
		struct
		{
			float DmgAtkArg;			//ATKВаDMG¤сЁТ
			float MDmgMAtkArg;			//MATKВаMDmg¤сЁТ
			float Crit_DLV;				//­P©R¤@А»Іv µҐ®tјvЕT 
			float DMG_DLV;				//¶Л®` µҐ®tјvЕT 
			float Miss_DLV;				//©R¤¤Іv µҐ®tјvЕT
			float Miss_SecHand;			//°Ж¤в©R¤¤ІvЅХѕг
			float Dmg_SecHand;			//°Ж¤в§рА»ІvЅХѕг
			float BaseCritHit;			//°тҐ»ЄєCritial ѕчІv
			float BaseCritPower;		//°тҐ»ЄєCritial «В¤O
			float BaseMagicCritHit;		//°тҐ»ЄєЕ]Єk Critial ѕчІv
			float BaseMagicCritPower;	//°тҐ»ЄєЕ]Єk Critial «В¤O

			float Dmg_MagArg;			//Є«§р§ЮЇаГBҐ~јWҐ[( ¦№°СјЖ*DMG)
			float MDmg_MagArg;			//(¶Л®`)Е]§р(orҐ[¦е)§ЮЇаГBҐ~јWҐ[( ¦№°СјЖ*MDMG*¬IЄk¬нјЖ)
			float MDmg_DotArg;			//(¶Л®`)Dot§ЮЇаГBҐ~јWҐ[( ¦№°СјЖ*MDMG*¬IЄk¬нјЖ/dot¦ёјЖ)

			float Dmg_HealArg;			//(ЄvАш)Ґ[¦еГBҐ~јWҐ[( ¦№°СјЖ*MDMG*¬IЄk¬нјЖ )
			float Dmg_HealDotArg;		//(ЄvАш)Dot§ЮЇаГBҐ~јWҐ[( ¦№°СјЖ*MDMG*¬IЄk¬нјЖ/dot¦ёјЖ)

			float BaseRateArg;			//§рЁѕ¤с©і( ­P©R ®жАЙ ¬ЮАЙ )
			float BaseResistArg;		//§рЁѕ¤с©і( §Ь­P©R ¬п®жАЙ ¬п¬ЮАЙ )
			float DLvRateArg;			//§рЁѕ¤сµҐЇЕјvЕT( ­P©R ®жАЙ ¬ЮАЙ )
			float DLvResistArg;			//§рЁѕ¤сµҐЇЕјvЕT(  §Ь­P©R ¬п®жАЙ ¬п¬ЮАЙ )

			float PhyHit_Base;			//Є«Іz©R¤¤ °т©і
			float PhyHit_Range;			//Є«Іz©R¤¤ ЕЬ¤Ж¶q
			float PhyHit_AgiArg;		//Є«Іz©R¤¤ AgiјvЕT
			float PhyAvoid_AgiArg;		//Є«Іz°{БЧ AgiјvЕT

			float MagHit_Base;			//ЄkіN©R¤¤ °т©і
			float MagHit_Range;			//ЄkіN©R¤¤ ЕЬ¤Ж¶q
			float MagHit_MndArg;		//ЄkіN©R¤¤ MndјvЕT
			float MagAvoid_AgiArg;		//ЄkіN©R¤¤ AgiјvЕT
			float MagAvoid_MndArg;		//ЄkіN°{БЧ AgiјvЕT

			float PhantomRankArg;
			float PhantomLvArg;
		};
	};

};
// TODO:: NEW FIELD
struct FixFightArgumentStruct
{
	int GUID;
	union
	{
		float Arg[100];
		struct
		{
			float DmgAtkArg;			//ATKВаDMG¤сЁТ
			float MDmgMAtkArg;			//MATKВаMDmg¤сЁТ
			float Crit_DLV;				//­P©R¤@А»Іv µҐ®tјvЕT 
			float DMG_DLV;				//¶Л®` µҐ®tјvЕT 
			float Miss_DLV;				//©R¤¤Іv µҐ®tјvЕT
			float Miss_SecHand;			//°Ж¤в©R¤¤ІvЅХѕг
			float Dmg_SecHand;			//°Ж¤в§рА»ІvЅХѕг
			float BaseCritHit;			//°тҐ»ЄєCritial ѕчІv
			float BaseCritPower;		//°тҐ»ЄєCritial «В¤O
			float BaseMagicCritHit;		//°тҐ»ЄєЕ]Єk Critial ѕчІv
			float BaseMagicCritPower;	//°тҐ»ЄєЕ]Єk Critial «В¤O

			float Dmg_MagArg;			//Є«§р§ЮЇаГBҐ~јWҐ[( ¦№°СјЖ*DMG)
			float MDmg_MagArg;			//(¶Л®`)Е]§р(orҐ[¦е)§ЮЇаГBҐ~јWҐ[( ¦№°СјЖ*MDMG*¬IЄk¬нјЖ)
			float MDmg_DotArg;			//(¶Л®`)Dot§ЮЇаГBҐ~јWҐ[( ¦№°СјЖ*MDMG*¬IЄk¬нјЖ/dot¦ёјЖ)

			float Dmg_HealArg;			//(ЄvАш)Ґ[¦еГBҐ~јWҐ[( ¦№°СјЖ*MDMG*¬IЄk¬нјЖ )
			float Dmg_HealDotArg;		//(ЄvАш)Dot§ЮЇаГBҐ~јWҐ[( ¦№°СјЖ*MDMG*¬IЄk¬нјЖ/dot¦ёјЖ)

			float BaseRateArg;			//§рЁѕ¤с©і( ­P©R ®жАЙ ¬ЮАЙ )
			float BaseResistArg;		//§рЁѕ¤с©і( §Ь­P©R ¬п®жАЙ ¬п¬ЮАЙ )
			float DLvRateArg;			//§рЁѕ¤сµҐЇЕјvЕT( ­P©R ®жАЙ ¬ЮАЙ )
			float DLvResistArg;			//§рЁѕ¤сµҐЇЕјvЕT(  §Ь­P©R ¬п®жАЙ ¬п¬ЮАЙ )

			float PhyHit_Base;			//Є«Іz©R¤¤ °т©і
			float PhyHit_Range;			//Є«Іz©R¤¤ ЕЬ¤Ж¶q
			float PhyHit_AgiArg;		//Є«Іz©R¤¤ AgiјvЕT
			float PhyAvoid_AgiArg;		//Є«Іz°{БЧ AgiјvЕT

			float MagHit_Base;			//ЄkіN©R¤¤ °т©і
			float MagHit_Range;			//ЄkіN©R¤¤ ЕЬ¤Ж¶q
			float MagHit_MndArg;		//ЄkіN©R¤¤ MndјvЕT
			float MagAvoid_AgiArg;		//ЄkіN©R¤¤ AgiјvЕT
			float MagAvoid_MndArg;		//ЄkіN°{БЧ AgiјvЕT

			float PhantomRankArg;
			float PhantomLvArg;
		};
	};

};
//----------------------------------------------------------------------------------------
//ҐСєШЁ¬Вѕ·~µҐЇЕєвҐXЁУЄє­И
struct FightArgumentStructOld
{
	union
	{
		float Arg[100];
		struct
		{
			float BStrLv1, BStaLv1, BIntLv1, BMndLv1, BAgiLv1, BHpLv1, BMpLv1;
			float BStrArg, BStaArg, BIntArg, BMndArg, BAgiArg, BHpArg, BMpArg;
			float StrArg, StaArg, IntArg, MndArg;
			float AgiArg, HPArg, MPArg, HpStrArg, HpStaArg, MPIntArg;
			float MpMndArg, AtkStrArg, AtkIntArg, AtkAgiArg, DefArg;
			float BaseReHP, BaseReMP, ReHpArg, ReMpArg, MAtkArg;
			float MDefArg;
			float BaseMoveSpeed;
			float ReSPNormal_Move, ReSPNormal_Stop, ReSPAttack, ReSPUnderAttack;
			float ReSPAttackTime_Move, ReSPAttackTime_Stop;
			float MaxSP, DmgRate;
		};
	};
};
// TODO:: NEW FIELD
struct FightArgumentStruct
{
	int GUID;
	union
	{
		float Arg[100];
		struct
		{
			float BStrLv1, BStaLv1, BIntLv1, BMndLv1, BAgiLv1, BHpLv1, BMpLv1;
			float BStrArg, BStaArg, BIntArg, BMndArg, BAgiArg, BHpArg, BMpArg;
			float StrArg, StaArg, IntArg, MndArg;
			float AgiArg, HPArg, MPArg, HpStrArg, HpStaArg, MPIntArg;
			float MpMndArg, AtkStrArg, AtkIntArg, AtkAgiArg, DefArg;
			float BaseReHP, BaseReMP, ReHpArg, ReMpArg, MAtkArg;
			float MDefArg;
			float BaseMoveSpeed;
			float ReSPNormal_Move, ReSPNormal_Stop, ReSPAttack, ReSPUnderAttack;
			float ReSPAttackTime_Move, ReSPAttackTime_Stop;
			float MaxSP, DmgRate;
		};
	};
};
//----------------------------------------------------------------------------------------
//§Ь©КЄн°тҐ»µІєc
//----------------------------------------------------------------------------------------
enum	ResistArgType_ENUM
{
	EM_ResistArgType_Power0,
	EM_ResistArgType_Power25,
	EM_ResistArgType_Power50,
	EM_ResistArgType_Power75,
	EM_ResistArgType_Power100,

	EM_ResistArgType_Power_MaxCount,
};

struct ResistArgumentStruct
{
	int		Resist;
	union
	{
		int		PowerList[EM_ResistArgType_Power_MaxCount];
		struct
		{
			int		Power0;			//§Ь100%
			int		Power25;		//§Ь75%
			int		Power50;		//§Ь50%
			int		Power75;		//§Ь25%
			int		Power100;		//§№Ґю©УЁь
		};
	};
};
//-------------------------------------------------------------------------------------
//¤Ѕ·|ёк·Ѕ
struct GuildResourceStruct
{
	union
	{
		int		_Value[7];
		struct
		{
			int		Gold;
			int		BonusGold;
			int		Mine;
			int		Wood;
			int		Herb;
			int		GuildRune;
			int		GuildStone;
		};
	};

	bool IsEmpty()
	{
		for (int i = 0; i < 7; i++)
		{
			if (_Value[i] != 0)
				return false;
		}
		return true;
	}

	void Init()
	{
		memset(this, 0, sizeof(*this));
	};
};

//----------------------------------------------------------------------------------------
// Ё¤¦вЕv­­і]©w
//----------------------------------------------------------------------------------------
struct SystemFlagStruct
{
	union
	{
		int		_Value;
		struct
		{
			bool	DisableTrade : 1;		//Ґж©цёT¤о( AC Shop Mail Trade House)
			bool	DisableTalk : 1;		//ҐжЅНёT¤о
			bool	ChangeNameEnabled : 1;		//Ґi§у¦W
			bool	IsHero : 1;		//
			bool	IsDemon : 1;		//
			bool	IsHonorLeader : 1;		// єaЕAІХ¶¤Є¬єA( ¦pЄG¦№¤H¬°Party ¶¤Єш)
			bool	IsNeedSetPosition : 1;		//ґ«°П®Й­n­«·sі]©w¦мёm
			bool	FindParty : 1;
			bool	IsContorlPet : 1;		//Ґi±±ЁоГdЄ«
			bool	PassengerAttackable : 1;		//Ґi§рА»­ј«И
			bool	MoneySerialCodeError : 1;		//ЄчїъSerialCodeїщ»~
			bool	EnableGMCmd : 1;		//¶}±ТGM ©RҐO
		};
	};
};
//----------------------------------------------------------------------------------------
//°тҐ»Є«ҐуДЭ©К©wёq (Є«ҐуІЈҐНҐiі]©w¶µҐШ) ЁC­УЄ«ҐуЄє¦@ҐОі]©w
//----------------------------------------------------------------------------------------
struct	ObjectModeStruct
{
	union
	{
		int			_Mode;			//¦pЄG_Mode == -1 ЄнҐЬёк®ЖЁS¦іҐО
		struct		//npc ДЭ©К
		{
			//Ег№іЄ«Ґу
			bool	Show : 1;		    //0x00000001	ЁqҐX(¤ЈЅЧЁqҐX©ОБфВГ, јР°O/­PЇа)
			bool	Mark : 1;		    //0x00000002	ҐiјР°O/ВIїп(ҐiҐHЕэЄ±®aВIїп)
			bool	Obstruct : 1;		    //0x00000004	·|ЄэѕЧ(ЄэѕЧЁдҐLЄєЄ«Ґу)
			bool	Gravity : 1;		    //0x00000008	¦і­«¤O(¦a¤Я¤Ю¤O, ­pєвЄ«Ґу»P¦a­±ЄєёIјІ, §_«hЁМ·У Client °eЁУЄє®yјР, ¤Ј·|ЁМ¦a­±°Є«Ч§пЕЬ y)
			bool	Save : 1;		    //0x00000010	¦№Є«Ґу¬O§_­nАx¦s
			bool    Drag : 1;         //0x00000020    ¦іЄэ¤OЄє
			bool    GMHide : 1;			//0x00000040    §№ҐюБф¦ж
			bool    IsAllZoneVisible : 1;			//0x00000080    Ґю°мҐiµшЄ«Ґу

			bool	DisableRotate : 1;			//0x00000100	ёT¤о±ЫВа( ­±¦V¤Ј·|§пЕЬ ) 
			bool	GM_NpcHide : 1;			//0x00000200	ҐОGM©RҐOЕэ¬Y­УNPC hide
			bool	Remove : 1;			//0x00000400	Ґi§_Іѕ°Ј(¤Ѕ·|«Шїv)
			bool	RelyOwnerPower : 1;			//0x00000800	°С¦ТҐD¤HЄє«В¤O
			bool	IgnoreNpcKill : 1;			//0x00001000	©їІ¤npc±ю¦є
			bool	EnemyHide : 1;			//0x00002000	јД¤и¬Э¤Ј«Ш¦№Є«Ґу
			bool    NotShowHPMP : 1;         //0x00004000	¤ЈЕгҐЬ¦е±ш
			bool	NoFix_Y : 1;			//0x00008000	¤Ј­ЧҐїўз¶b

			//NPC¦ж¬°
			bool	Strikback : 1;			//0x00010000	¤ПА»
			bool	Move : 1;			//0x00020000	Іѕ°К
			bool	Searchenemy : 1;			//0x00040000	·jґMјД¤H
			bool	Revive : 1;			//0x00080000	Ґi­«ҐН(§iЄѕ SERVER, ¶}±Т¦№єXјРЄєЄ«Ґу»Э­n­«ҐН, ­«ҐН®Й¶Ў¬d¬Э Revive)
			bool	Fight : 1;			//0x00100000	Ґi¬е±ю§рА»(¶}±Т¦№єXјРЄєЄ«ҐуҐiҐHіQ¬е±ю)
			bool    Escape : 1;         //0x00200000    °k¶]
			bool    SearchenemyIgnore : 1;         //0x00400000    ¦№Є«Ґу¤Ј·|іQ·jґMЁм

			//°СјЖ
			bool	AlignToSurface : 1;			//0x00800000	¶K»фЄн­±
			bool	HideName : 1;			//0x01000000	NPC ©О Є«ҐуАY¤WЄє¦WєЩ¬O§_ЕгҐЬ ( Гю«¬Ґ~ЄєЁТҐ~ )
			bool	HideMinimap : 1;			//0x02000000	NPC ©О Є«Ґу¦b¤p¦a№П¤W­±¬O§_ЕгҐЬ ( Гю«¬Ґ~ЄєЁТҐ~ )
			bool	ShowRoleHead : 1;			//0x04000000	ВIА»Є«Ґу«б,¬O§_­nЕгҐЬАY№і®Ш( true ¬O show, Hide ¬O

			bool	CollisionPlot : 1;			//0x08000000	¬O§_ѕЦ¦іёIјІј@±Ў
			bool	Treasure : 1;			//0x10000000	Д_Ѕc
			bool	IsPet : 1;			//0x20000000	ГdЄ«
			bool	DisableAction : 1;			//0x40000000	јИ®Й§пЕЬ°К§@
			bool	DisableMovePG : 1;			//0x80000000	Ґi±±ёьЁг¤Ј°eІѕ°К«КҐ]
		};
		struct  //Є««~ДЭ©К
		{
			bool	DurableStart0 : 1;			//0x00000001	­@¤[«Ч­«0¶}©l
			bool	PkNoDrop : 1;			//0x00000002	pk®Й¤Ј·|±јёЁ
			bool	DepartmentStore : 1;			//0x00000004	°У«°Є««~
			bool	DepartmentStore_Free : 1;			//0x00000008	ҐІєё¤B°У«°Є««~
			bool	ItemDrop_AllParty : 1;			//0x00000010	Ґю¶¤±јёЁ
			bool	UseWithoutFail : 1;			//0x00000020	ЁПҐО¤Ј·|Ґў±С
			bool    SpecialSuit : 1;         //0x00000040	»ЕёЛ
			bool	ItemDrop_Depart : 1;			//0x00000080	Є««~±јёЁ¦Ы°К¤А°п
			//Є««~ёк®Ж
			bool	PickupBound : 1;			//0x00000100	АЛЁъёTЁо
			bool	Sell : 1;			//0x00000200	Ґi¶RЅж
			bool	Logoutdel : 1;			//0x00000400	ВчЅu®шҐў
			bool	Expense : 1;			//0x00000800	®шЇУ
			bool	Coloring : 1;			//0x00001000	Ґi¬V¦в
			bool	GuildMark : 1;			//0x00002000	¤Ѕ·|і№
			bool    EQSoulBound : 1;         //0x00004000	ёЛіЖёTЁо
			bool	HideCount : 1;			//0x00008000	¤ЈЕгҐЬјЖ¶q
			bool	ChangeZoneDel : 1;			//0x00010000	ґ«°П®шҐў
			bool	HideRefineLight : 1;			//0x00020000	ЅДµҐЄZѕ№¤ЈЕгҐЬЇS®Д
			bool	Unique : 1;			//0x00040000	°Я¤@Є««~
			bool	VariableRare : 1;			//0x00080000	(°t¤и±MҐО)µ}¦і«Ч·|§пЕЬ
			bool	NoNPCAbility : 1;			//0x00100000	Є««~±јёЁҐюѕaЄ««~Ґ»Ё­ДЭ©К
			bool	EnableLockedItem : 1;			//0x00200000	Ґi№пВк©wЄєЄ««~ЁПҐО
			bool	AutoUseItem : 1;			//0x00400000	¦Ы°КЁПҐОЄ««~
			bool	DisableRefine : 1;			//0x00800000	¤ЈҐiєл·Т
			bool	DisablePowerQuality : 1;			//0x01000000	¤ЈҐi¤Й¶Ґ
			bool	DisableMagicBox : 1;			//0x02000000	¤ЈҐiЁПҐОЕ]¤ЫД_І°
			bool	DisableDigHode : 1;			//0x04000000	µLЄkҐґ¤Х
			bool	HideMaxHeap : 1;			//0x08000000	¤ЈЕгҐЬіМ¤j°пЕ|¶q
		};
		struct //АY»ОДЭ©К
		{
			bool	PreViewTitle : 1;			//0x00000001	Ґi№wДэАY»О
		};
	};

	bool	GetMode(int i)
	{
		int T = 0x1 << i;
		return (_Mode & T) != 0;
	};


};
//----------------------------------------------------------------------------------------
//	ЄkіNДЭ©КјvЕT®ДЄG
//----------------------------------------------------------------------------------------
struct		MagicEffectStruct
{
	union
	{
		int			_Value[2];
		struct
		{
			//¤@Їл
			bool    UseMagicDisable : 1;     //¤ЈҐiҐОЄkіN§рА»
			bool    UsePhyDisable : 1;     //¤ЈҐiҐОЄ«Іz§рА»
			bool    BadMagicInvincible : 1;     //ГaЄєЄkіNµL®Д(ЄkіN§ЮЇаµL®Д)
			bool    BadPhyInvincible : 1;     //ГaЄєЄ«ІzµL®Д(Є«Іz§ЮЇа»P¤@Їл§рА»µL®Д)
			bool    UseItemDisable : 1;     //¤ЈҐiЁПҐОЄ««~
			bool    SearchEmenyDisable : 1;     //¤ЈҐiЇБјД
			bool    Stop : 1;     //¤ЈҐiІѕ°К

			//Client єЭ®ДЄG«¬
			bool    Cover : 1;     //Бф§О
			bool    DetectCover : 1;     //°»ґъБф§О
			bool	Sneak : 1;		//јз¦ж
			bool	DetectSneak : 1;		//°»ґъјз¦ж
			bool    Blind : 1;     //Ґў©ъ(їГ№хҐю¶В)
			bool	HitDown : 1;		//­Л¦a
			bool	Trace : 1;		//°lВЬ			

			//ЁдҐL
			bool	PlayDead : 1;	    //ёЛ¦є
			bool    Insure : 1;     //«OАI(©и¦є¤@¦ё)
			bool    Chaos : 1;     //ІV¶Г
			bool    Fear : 1;     //®ЈДЯ

			bool	LockTarget : 1;		//§рА»ҐШјРЇБ©w			
			bool	WeaponIgnore : 1;		//ЄZѕ№µL®Д¤Ж
			bool	ArmorIgnore : 1;		//ЁѕЁгµL®Д¤Ж			
			bool	Ride : 1;		//ГM­ј
			bool	Raise : 1;		//­«ҐН
			bool	ExpProtect : 1;		//¦є¤`ёgЕз­И¤Ј¦©

			bool	Client_Dizzy : 1;		//·wЇt(Client ¦ж¬°) 
			bool	Client_Sleep : 1;		//єОЇv(Client ¦ж¬°)
			bool	PKFlag : 1;		//PKЄ¬єA(¦і¶}±ТЄє¤HґNҐiҐHPK)
			bool	TeleportDisable : 1;		//ёT¤о¶З°e

			bool	GPS : 1;		//¦мёm©w¦мѕ№­PЇа
			bool	PKFlag_Disabled : 1;		//PKёTЁоҐO
			bool	AI_LowAttackPriority : 1;		//іQ©Зai§рА»-АuҐэ­°ЁміМ§C
			bool	StopOntimeCure : 1;		//°±¤оHPMPSP©w®Й¦^ґ_

			//////////////////////////////////////////////////////////////////////////
			bool	Silence : 1;		//ёT¤о»ЎёЬ(GM)
			bool	CliOutofContorl : 1;		//Client¤ЈҐi±±Ёо
			bool	Freeze : 1;		//ClientЄ±®a¦B­б
			bool    GoodMagicInvincible : 1;     //¦nЄєЄkіNµL®Д(ЄkіN§ЮЇаµL®Д)
			bool    GoodPhyInvincible : 1;     //¦nЄєЄ«ІzµL®Д(Є«Іz§ЮЇа»P¤@Їл§рА»µL®Д)
			bool	Guilty : 1;		//¦іёo
			bool	Critial : 1;		//µґ№п­P©R¤@А»
			bool	Hunter : 1;		//Вy±юЄМ
			bool	NoEscape : 1;		//¤ЈҐi°k¶]
			bool	DisableJobSkill : 1;		//¤ЈЇаЁПҐО­мҐ»Вѕ·~§ЮЇа
			bool	IgnoreDeadGoodEvil : 1;		//¤ЈіBІz¦є¤`µЅґc­И
			bool	ExchangeZoneDamageEvent : 1;	//ЁъҐN°П°м¶Л®`
			bool	Fly : 1;		//­ё¦ж
			bool	WaterWalk : 1;		//¤ф¤W¦жЁ«
			bool	GravityDisable : 1;		//јИ®ЙµL­«¤O
			bool    DisableStrikeBack : 1;		//ѕ_°hµL®Д
			bool	DisableWagon : 1;		//¤ЈЇа¤WёьЁг
			bool	IgnoreInstancePlayer : 1;		//©їІ¤°ЖҐ»¤HјЖ­pєв
			bool	TitleSysPowerUp : 1;		//·sАY»ОЁtІОЇа¤OЁв­ї
			bool	MagicSpell_ZeroTime : 1;		//©Т¦іЄkіN Аюµo
			bool	MagicSpell_Moveable : 1;		//©Т¦іЄkіN ҐiІѕ°К¬IЄk
			bool	MagicSpell_NoCost : 1;		//©Т¦іЄkіN µL®шЇУ
			bool	DetectPlayDead : 1;		//°»¬dёЛ¦є
			bool	ChangeMotion : 1;		//­Ч§п°К§@
			bool	RangeAttackInvincible : 1;	//ГaЄєЅdітЄkіNµL®Д
			bool	EqProtect : 1;		//ёЛіЖ­@¤[«Ч¤Ј·|¤U­°(ёЛіЖ«OЕ@)

		};
	};
};
// NPC Other Setting
//----------------------------------------------------------------------------------------
struct		StructNPCOtherFlag
{
	union
	{
		int			Value;
		struct
		{
			//¤@Їл
			bool    ForceAttachkWeapon : 1;     //±jЁоЄZѕ№ёj©w¦b¤в¤W
		};
	};
};
//----------------------------------------------------------------------------------------
//»І§UЄkіN¬I®Д±шҐу
//----------------------------------------------------------------------------------------
struct  MagicClearTimeStruct
{
	union
	{
		int			_Value;
		struct
		{
			bool    Attack : 1;    	//§рА»®шҐў
			bool    UnderAtk : 1;    	//іQ§рА»®шҐў
			bool    Move : 1;    	//Іѕ°К®шҐў
			bool    Spell : 1;    	//¬IЄk/ҐОЄ«Іz§Ю/ЁПҐОЄ««~®шҐў
			bool    ChangeZone : 1;    	//ґ«°П®шҐў
			bool    Logout : 1;    	//ВчЅu®шҐў
			bool	Random : 1;	   	//¶ГјЖ®шҐў Max/3 ~ Max
			bool    WarriorSP0 : 1;    	//ѕФ¤hSP¬°0®шҐў
			bool    RangerSP0 : 1;    	//№C«LSP¬°0®шҐў
			bool    RogueSP0 : 1;    	//µsёйSP¬°0®шҐў
			bool    HP0 : 1;    	//HP¬°0®шҐў
			bool    MP0 : 1;    	//MP¬°0®шҐў
			bool	Spell_MagicAttack : 1;		//¬I§рА»ЄkіN®шҐў
			bool	Spell_PhyAttack : 1;		//¬I§рА»§ЮЇа®шҐў
			bool	Spell_Cure : 1;		//¬I§р¦^ґ_Е]Єk®шҐў
			bool	TargetOnAttack : 1;		//ҐШјР¬O§рА»јТ¦Ўor¬ЭЄєЁмҐШјР §рА»®шҐў
			bool	OnStopAttackMode : 1;		//°±¤о§рА»
			bool	OnWater : 1;		//¤ф¤¤®шҐў
			bool	OnNoWater : 1;		//ёф¤W®шҐў
			bool	Spell_Magic : 1;		//¬I©сЄkіN§ЮЇа®шҐў
			bool	Spell_Phy : 1;		//¬I©сЄ«Іz§ЮЇа®шҐў
			bool	ChangeJob : 1;		//ґ«Вѕ·~®шҐў
			bool	ChangeSuitSkill : 1;		//¤Бґ«®MёЛ§ЮЇа®Й®шҐў
			bool	ChangeEQPhantom : 1;		//Е]ЖFёЛіЖ§пЕЬ
		};
	};
};
//----------------------------------------------------------------------------------------
//	Є««~¦CБ|
//----------------------------------------------------------------------------------------
enum	GameObjectItemENUM
{
	EM_Item_Normal = 0,
	EM_Item_Money,
	EM_Item_Weapon,
	EM_Item_Armor,
	EM_Item_Rune,
	EM_Item_Card,

	EM_Item_Max,
	EM_GameObjectItemEM_FORCE_DWORD = 0x7fffffff,
};
//----------------------------------------------------------------------------------------
//ЄZѕ№Б`Гю¦CБ|
//----------------------------------------------------------------------------------------

enum	GameObjectWeaponENUM
{
	EM_Weapon_None = -1,
	EM_Weapon_Unarmed,// UnarmedЄЕ¤в
	EM_Weapon_Blade,// јC
	EM_Weapon_Dagger,// ¤P­є
	EM_Weapon_Wand,// Еv§ъ
	EM_Weapon_Axe,// ©т
	EM_Weapon_Bludgeon,// ВсґТґО
	EM_Weapon_Claymore,// Вщ¤вјC
	EM_Weapon_Staff,// §ъ
	EM_Weapon_2H_Axe,// Вщ¤в©т
	EM_Weapon_2H_Hammer,// Вщ¤вВс
	EM_Weapon_Polearm,// єj(ЄшҐЩ)
	EM_Weapon_Bow,// »·µ{ЄZѕ№ ¤}
	EM_Weapon_CossBow,// »·µ{ЄZѕ№ ¤Q¦r¤}
	EM_Weapon_Gun,// »·µ{ЄZѕ№ єj
	EM_Weapon_Ammo_Bow,// ¤lјu ¤}
	EM_Weapon_Ammo_Gun,// ¤lјu єj
	EM_Weapon_Throw,// §лВYГюЄZѕ№
	EM_Weapon_Lumbering,// µ­¤м
	EM_Weapon_Herbalism,// ±ДГД
	EM_Weapon_Mining,// ±ДДq
	EM_Weapon_Fishing,// іЁіЅ

	EM_GameObjectWeaponEM_FORCE_DWORD = 0x7fffffff,
};

enum	GameObjectArmorENUM
{
	EM_Armor_None = -1,
	EM_Armor_HeavyArmed,
	EM_Armor_LightArmed,
	EM_Armor_Leather,
	EM_Armor_Clothes,
	EM_Armor_Robe,
	EM_Armor_Shield,

	EM_Armor_Implement,	// ¤в«щѕ№Ёг
	EM_Armor_Ornament,	// №ў«~Гю

	//Бn­µГю«¬
	EM_Armor_Wood,	//¤мАY
	EM_Armor_Stone,	//ҐЫАY
	EM_Armor_Iron,	//ЕK

	EM_Armor_MaxCount,

	EM_GameObjectArmorEM_FORCE_DWORD = 0x7fffffff,
};
//----------------------------------------------------------------------------------------
//ёЛіЖ§ЮЇаёк®Ж
struct EqWearSkillStruct
{
	union
	{
		int _Skill;
		struct
		{
			bool    Unarmed : 1;		//0x00000001
			bool    Blade : 1;		//0x00000002
			bool    Dagger : 1;		//0x00000004
			bool    Wand : 1;		//0x00000008
			bool    Axe : 1;		//0x00000010
			bool    Bludgeon : 1;		//0x00000020
			bool    Claymore : 1;		//0x00000040
			bool    Staff : 1;		//0x00000080
			bool    Axe_2H : 1;		//0x00000100
			bool    Hammer_2H : 1;		//0x00000200
			bool    Polearm : 1;		//0x00000400
			bool    Bow : 1;		//0x00000800
			bool    CossBow : 1;		//0x00001000
			bool    Gun : 1;		//0x00002000
			bool    Throw : 1;		//0x00004000
			bool    HeavyArmed : 1;		//0x00008000
			bool    LightArmed : 1;		//0x00010000
			bool    Leather : 1;		//0x00020000
			bool    Clothes : 1;		//0x00040000
			bool    Robe : 1;		//0x00080000
			bool	Shield : 1;		//0x00100000
			bool	Implement : 1;		//0x00200000
			bool	SecondHand : 1;		//0x00400000
		};
	};
};

//----------------------------------------------------------------------------------------
//ёЛіЖ¦мёm
//----------------------------------------------------------------------------------------
enum GameObjectWeaponPosENUM
{
	EM_WeaponPos_MainHand,   //ҐD¤в
	EM_WeaponPos_SecondHand,   //°Ж¤в
	EM_WeaponPos_OneHand,   //Вщ¤віЈҐiҐHёЛіЖ
	EM_WeaponPos_TwoHand,   //Ёв¤в
	EM_WeaponPos_Ammo,   //¤lјu
	EM_WeaponPos_Bow,   //¤}
	EM_WeaponPos_Manufacture,   //ҐНІЈ¤uЁг

	EM_GameObjectWeaponPosENUM_FORCE_DWORD = 0x7fffffff,
};

enum GameObjectArmorPosENUM
{
	EM_ArmorPos_Head,   //АY
	EM_ArmorPos_Gloves,   //¤в®M
	EM_ArmorPos_Shoes,   //ѕc¤l
	EM_ArmorPos_Clothes,   //¤W¦з
	EM_ArmorPos_Pants,   //їЗ¤l
	EM_ArmorPos_Back,   //­IіЎ
	EM_ArmorPos_Belt,   //ёy±a
	EM_ArmorPos_Shoulder,   //ЄУҐТ
	EM_ArmorPos_Necklace,   //¶µБе 
	EM_ArmorPos_Ring,   //§Щ¤l
	EM_ArmorPos_Earring,   //¦Х№ў
	EM_ArmorPos_SecondHand,   //°Ж¤в
	EM_ArmorPos_MagicTool,	//ЄkД_
	EM_ArmorPos_Ornament,	//ёЛ№ў«~


	EM_ArmorPos_MaxCount,
	EM_GameObjectArmorPosENUM_FORCE_DWORD = 0x7fffffff,
};
//-------------------------------------------------------------------------------------
//§Ь©КЇPБ|
enum    MagicResistENUM
{
	EM_MagicResist_None = -1,
	EM_MagicResist_Earth,
	EM_MagicResist_Water,
	EM_MagicResist_Fire,
	EM_MagicResist_Wind,
	EM_MagicResist_Light,
	EM_MagicResist_Darkness,
	EM_MagicResist_MAXCOUNT,

	EM_MagicResist_FORCE_DWORD = 0x7fffffff,
};
//-------------------------------------------------------------------------------------
//§Ь©КµІєc
struct  MagicResistStruct
{
	union
	{
		float     _ArrList[EM_MagicResist_MAXCOUNT];
		struct
		{
			float     Earth;
			float     Water;
			float     Fire;
			float     Wind;
			float     Light;
			float     Darkness;
		};
	};
};
//----------------------------------------------------------------------------------------
//NPC§Ь©Кёк®ЖЄн
//----------------------------------------------------------------------------------------
struct NpcResistBaseInfoStruct
{
	char				Name[32];
	MagicResistENUM		Type;
	MagicResistStruct	Resist;
};
//-------------------------------------------------------------------------------------
//		§ЮЇаµІєc
//-------------------------------------------------------------------------------------
enum	SkillValueTypeENUM
{
	EM_SkillValueType_None = -1,
	EM_SkillValueType_Unarmed = 0,// UnarmedЄЕ¤в
	EM_SkillValueType_Blade,// јC
	EM_SkillValueType_Dagger,// ¤P­є
	EM_SkillValueType_Wand,// Еv§ъ
	EM_SkillValueType_Axe,// ©т
	EM_SkillValueType_Bludgeon,// ВсґТґО
	EM_SkillValueType_Claymore,// Вщ¤вјC
	EM_SkillValueType_Staff,// §ъ
	EM_SkillValueType_2H_Axe,// Вщ¤в©т
	EM_SkillValueType_2H_Hammer,// Вщ¤вВс
	EM_SkillValueType_Polearm,// єj(ЄшҐЩ)
	EM_SkillValueType_Bow,// »·µ{ЄZѕ№ ¤}
	EM_SkillValueType_CossBow,// »·µ{ЄZѕ№ ¤Q¦r¤}
	EM_SkillValueType_Gun,// »·µ{ЄZѕ№ єj
	EM_SkillValueType_Define,// Ёѕїm

	EM_SkillValueType_BlackSmith,// ҐґЕK
	EM_SkillValueType_Carpenter,// ¤м¤u
	EM_SkillValueType_MakeArmor,// »sҐТ
	EM_SkillValueType_Tailor,// µфБ_
	EM_SkillValueType_Cook,// Іi¶№
	EM_SkillValueType_Alchemy,// ·ТЄч

	EM_SkillValueType_Mining,// «хДq
	EM_SkillValueType_Lumbering,// ¬е§ч
	EM_SkillValueType_Herblism,// ±ДГД
	EM_SkillValueType_Fishing,// іЁіЅ
	EM_SkillValueType_Plant,// єШґУ
	EM_SkillValueType_CatchPet,// ёЙ®»ГdЄ«
	EM_SkillValueType_MaxCount,
};

struct	SkillValueStruct
{

	union
	{
		float	Skill[_MAX_SKILL_COUNT_];
		struct
		{
			float Unarmed, Blade, Dagger, Wand, Axe;
			float Bludgeon, Claymore, Staff, Axe_2H, Hammer_2H;
			float Polearm, Bow, CossBow, Gun;
			float Defense;
			// ҐН¬Ў§Ю
			float BlackSmith, Carpenter, MakeArmor, Tailor, Cook, Alchemy;
			// ±Д¶°ҐН¬Ў§ЮЇа
			float Mining, Lumbering, Herblism, Fishing, Plant, CatchPet;
		};
	};
	float GetSkill(int ID)
	{
		if ((unsigned)ID < _MAX_SKILL_COUNT_)
			return Skill[ID];
		return 0;
	}
};
//----------------------------------------------------------------------------------------
//	NPC Title ЕгҐЬі]©w¦CБ|
//----------------------------------------------------------------------------------------
enum NpcTitleTypeENUM
{
	EM_NpcTitleTypeENUM_Nornal,		//¤@Їл
	EM_NpcTitleTypeENUM_Invisible,		//¤ЈЕгҐЬ
	EM_NpcTitleTypeENUM_QuestNPC,		//Ґф°И«¬npc
	EM_NpcTitleTypeENUM_Neutral,		//¤¤ҐЯ©З
};

//----------------------------------------------------------------------------------------
//	ЄkіN®шЇУ¦CБ|
//----------------------------------------------------------------------------------------
enum MagicSpellCostTypeENUM
{
	EM_MagicSpellCostType_None,		//µL®шЇУ
	EM_MagicSpellCostType_HP,		//®шЇУ HP
	EM_MagicSpellCostType_MP,		//®шЇУ MP
	EM_MagicSpellCostType_HP_Per,		//®шЇУ HP¦К¤А¤с
	EM_MagicSpellCostType_MP_Per,		//®шЇУ MP¦К¤А¤с
	EM_MagicSpellCostType_SP_Warrior,		//®шЇУ SP ѕФ¤h(-1ЄнҐЬҐюіЎ)
	EM_MagicSpellCostType_SP_Ranger,		//®шЇУ SP №C«L(-1ЄнҐЬҐюіЎ)
	EM_MagicSpellCostType_SP_Rogue,		//®шЇУ SP µsёй(-1ЄнҐЬҐюіЎ)
	EM_MagicSpellCostType_StomachPoint,		//®шЇУ №Ў­№«Ч
	EM_MagicSpellCostType_Item,		//®шЇУ Є««~
	EM_MagicSpellCostType_Ammo_Gun,		//®шЇУ ¤lјu
	EM_MagicSpellCostType_Ammo_Bow,		//®шЇУ ¤}ЅbҐЪ
	EM_MagicSpellCostType_Ammo_CossBow,		//®шЇУ ¤Q¦r¤}ЅbҐЪ
	EM_MagicSpellCostType_Ammo_Throw,		//®шЇУ §лВYЄ«
	EM_MagicSpellCostType_Ammo_All,		//®шЇУ ©Т¦іЄє»·µ{јuГД
	EM_MagicSpellCostType_SoulPoint,		//®шЇУ ЖF»о­И
};
//----------------------------------------------------------------------------------------
//	ЄkіN»ЭЁD¦CБ|
//----------------------------------------------------------------------------------------
enum MagicSpellNeedTypeENUM
{
	EM_MagicSpellNeedType_None,	//µL»ЭЁD
	EM_MagicSpellNeedType_Weapon,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№
	EM_MagicSpellNeedType_EQ,	//»ЭЁD ¦Ы¤vёЛ ёЛіЖ
	EM_MagicSpellNeedType_Suit,	//»ЭЁD ¦Ы¤vёЛ ®MёЛ
	EM_MagicSpellNeedType_Buf,	//»ЭЁD ¦Ы¤v¦і ЄkіNBuf
	EM_MagicSpellNeedType_Buf_Target,	//»ЭЁD ҐШјР¦і ЄkіNBuf
	EM_MagicSpellNeedType_NoBuf,	//»ЭЁD ¦Ы¤vЁS¦і ЄkіNBuf
	EM_MagicSpellNeedType_NoBuf_Target,	//»ЭЁD ҐШјРЁS¦і ЄkіNBuf
	EM_MagicSpellNeedType_WeaponType_Unarmed,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ ЄЕ¤в
	EM_MagicSpellNeedType_WeaponType_Blade,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ іж¤вјC
	EM_MagicSpellNeedType_WeaponType_Dagger,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ ¤P­є
	EM_MagicSpellNeedType_WeaponType_Wand,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ Еv§ъ
	EM_MagicSpellNeedType_WeaponType_Axe,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ іж¤в©т
	EM_MagicSpellNeedType_WeaponType_Bludgeon,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ ВсґТґО
	EM_MagicSpellNeedType_WeaponType_Claymore,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ Вщ¤вјC
	EM_MagicSpellNeedType_WeaponType_Staff,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ §ъ
	EM_MagicSpellNeedType_WeaponType_2H_Axe,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ Вщ¤в©т
	EM_MagicSpellNeedType_WeaponType_2H_Hammer,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ Вщ¤вВс
	EM_MagicSpellNeedType_WeaponType_Polearm,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ єj(ЄшҐЩ)
	EM_MagicSpellNeedType_WeaponType_SwordType,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ јCГю(іжВщ¤в)
	EM_MagicSpellNeedType_WeaponType_AxeType,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ ©тГю(іжВщ¤в)
	EM_MagicSpellNeedType_WeaponType_Shield,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ ¬Ю
	EM_MagicSpellNeedType_Distance,	//»ЭЁD ¶ZВчҐШјР
	EM_MagicSpellNeedType_NotAttak,	//»ЭЁD ¦Ы¤v«DѕФ°«
	EM_MagicSpellNeedType_Attack,	//»ЭЁD ¦Ы¤vѕФ°«
	EM_MagicSpellNeedType_Critical,	//»ЭЁD ¦Ы¤vГzА»
	EM_MagicSpellNeedType_BeCritical,	//»ЭЁD ¦Ы¤vіQГzА»
	EM_MagicSpellNeedType_Dodge,	//»ЭЁD ¦Ы¤v°{БЧ
	EM_MagicSpellNeedType_BeDodge,	//»ЭЁD ¦Ы¤vіQ°{БЧ
	EM_MagicSpellNeedType_Miss,	//»ЭЁD ¦Ы¤vMiss
	EM_MagicSpellNeedType_Parry,	//»ЭЁD ¦Ы¤v®жѕЧ
	EM_MagicSpellNeedType_BeParry,	//»ЭЁD ¦Ы¤vіQ®жѕЧ
	EM_MagicSpellNeedType_NotAttack_Target,	//»ЭЁD ҐШјР«DѕФ°«
	EM_MagicSpellNeedType_Attack_Target,	//»ЭЁD ҐШјРѕФ°«	
	EM_MagicSpellNeedType_SelfHp_Smaller_Percent,	//»ЭЁD HP¦К¤А¤с¤p©у
	EM_MagicSpellNeedType_SelfHp_Greater_Percent,	//»ЭЁD HP¦К¤А¤с¤j©у
	EM_MagicSpellNeedType_Self_Job,	//»ЭЁD Вѕ·~
	EM_MagicSpellNeedType_WeaponType_LongDistWeapon,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ Єш¶ZВчЄZѕ№
	EM_MagicSpellNeedType_WeaponType_2H_Weapon,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ Вщ¤вЄZѕ№
	EM_MagicSpellNeedType_WeaponType_Hammer,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ ВсГю(іжВщ¤в)
	EM_MagicSpellNeedType_BuffGroup,	//»ЭЁD ¦Ы¤vѕЦ¦і¬Y Buff Group
	EM_MagicSpellNeedType_ShieldBlock,	//»ЭЁD ¦Ы¤v¬ЮѕЧ
	EM_MagicSpellNeedType_BeShieldBlock,	//»ЭЁD ¦Ы¤vіQ¬ЮѕЧ
	EM_MagicSpellNeedType_WeaponType_1H_Weapon,	//»ЭЁD ¦Ы¤vёЛ ЄZѕ№Гю«¬ іж¤вЄZѕ№
	EM_MagicSpellNeedType_NoBuffGroup,	//»ЭЁD ¦Ы¤vЁS¦і¬Y Buff Group
	EM_MagicSpellNeedType_TargetBuffGroup,	//»ЭЁD ҐШјРѕЦ¦і¬Y Buff Group
	EM_MagicSpellNeedType_TargetNoBuffGroup,	//»ЭЁD ҐШјРЁS¦і¬Y Buff Group
	EM_MagicSpellNeedType_MagicCritical,	//»ЭЁD ¦Ы¤vГzА»
	EM_MagicSpellNeedType_BeMagicCritical,	//»ЭЁD ¦Ы¤vіQГzА»
	EM_MagicSpellNeedType_Self_MainJob,	//»ЭЁD ҐDВѕ·~
	EM_MagicSpellNeedType_Self_SoulPoint,	//»ЭЁD ЖF»о­И

};
//----------------------------------------------------------------------------------------
//ЁдҐLЄkіNі]©w
//----------------------------------------------------------------------------------------
struct	MagicSpellFlagStruct
{
	union
	{
		int _Type;
		struct
		{
			bool	GoodMagic : 1;	//¦nЄєЄkіN
			bool    ShowBuf : 1;	//ЕгҐЬBuf
			bool	ContinueMagic : 1;	//іsДтІЈҐНЄkіN
			bool	DotMagic : 1;	//DOTЄkіN
			bool	FaceOffMagic : 1;	//ЕЬЁ­іN 			
			bool	CancelBuff : 1;	//ҐiЁъ®ш
			bool	ShowBuffTime : 1;	//ClientєЭЕгҐЬBuff®Й¶Ў
			bool	SpecialMagic : 1;	//ІЈҐНЇS®нЄkіN
			bool	MagicShield : 1;	//Е]Єk¬Ю
			bool	DeadNotClear : 1;	//¦є¤`¤ЈІM°Ј
			bool	OfflineBuffTime : 1;	//¤UЅuBuff®Й¶Ў­pєв
			bool	OnPlayerAttackAddTime : 1;	//§рА»Є±®aјWҐ[buff®Й¶Ў
			bool	OnKillAddTime : 1;	//±ю¦єЄ«ҐујWҐ[buff®Й¶Ў
			bool	SelfBuff : 1;	//Ёp¤HBuf,¤Ј·|»PЁдҐL¤HЄєBuf¤¬Ёл
			bool	FaceOffMagic_ShowEq : 1;	//ЕЬЁ­®ЙЕгҐЬ­мҐ»ёЛіЖЄZѕ№
			bool	ShowPowerLv : 1;	//ЕгҐЬPowerLv
			bool	DotMagicNoKill : 1;	//DotЄkіN±ю¤Ј¦є¤H
			bool	BuffSkill : 1;	//BuffѕЦ¦і§ЮЇа
			bool	MaxMagicLvEquRoleLv : 1;	//BuffіМ°ЄµҐЇЕіМ¦h¦PЄ±®aµҐЇЕ
			bool	DisableShowMagicInfo : 1;	//Client єЭ¤ЈЕгҐЬҐ[ґо¦еЄєёк°V
			bool	FixDotPower : 1;	//Dot¤Ј­pєвёЛіЖЇа¤O(¤Јєв§рЁѕ¤с)
			bool	RideCanAttack : 1;	//§рА»¤Ј·|¤U°Ё
			bool	RidePetAttack : 1;	//ГM­јГdЄ«°µ§рА»°К§@
			bool	CancelBadMagic : 1;	//ІM°ЈҐШјР¬O¦Ы¤vЄєГaЄєЄkіN
			bool	NpcEndFight_NotClearBuff : 1;	//ѕФ°«µІ§ф¤Ј·|®шҐўЄєBuff
			bool	Dual_NoClearBuff : 1;	//¤сёХµІ§ф¤ЈІM°ЈЄєbuff
			bool	IgnoreSpellCureMagicClear : 1;	//©їІ¤¬I®iЄvАш®шҐў
			bool	IgnoreSpellAttackMagicClear : 1;	//©їІ¤¬I®i§рА»®шҐў
			bool	IgnoreBuffMessage : 1;	//©їІ¤¬I®i§рА»®шҐў(client єЭЕг№іҐО )
			bool	OnlyYouCanSee : 1;	//Ґu¦і¦Ы¤v¬ЭЄєЁмЇS®Д
			bool	NoEffect : 1;	//(Client ЕгҐЬіBІzҐО)
			bool	forceDisplayEffect : 1;	//±jЁоЕгҐЬЇS®Д
		};
	};
};

struct	MagicSpellFlagExStruct
{
	union
	{
		int _Type;
		struct
		{
			bool		IgnoreMagicAbsorb : 1;		//©їІ¤Е]Єk§l¦¬
			bool		IsDepartmentStore : 1;		//°У«°buff
		};
	};
};

//----------------------------------------------------------------------------------------
//	ЄZѕ№ЁѕЁг©ОЄkіN·|­Ч§пЄ±®aЇа¤OЄєµІєc
//----------------------------------------------------------------------------------------
enum WearEqTypeENUM
{
	EM_WearEqType_None,   // "µL®ДЄG",
	EM_WearEqType_Durable,   // "­@¤[«Ч",
	EM_WearEqType_STR,   // "ДЭ©К STR ¤O¶q",
	EM_WearEqType_STA,	// "ДЭ©К STA ­@¤O",			
	EM_WearEqType_INT,	// "ДЭ©К INT ґј¤O",			
	EM_WearEqType_MND,	// "ДЭ©К MND ЖF¤O",
	EM_WearEqType_AGI,	// "ДЭ©К AGI ±У±¶",			
	EM_WearEqType_AllState,   // "ДЭ©К ©Т¦і(°тҐ»¤­­УДЭ©К)",
	EM_WearEqType_MaxHP,	// "¤W­­ HP",
	EM_WearEqType_MaxMP,	// "¤W­­ MP",
	EM_WearEqType_ReHP,	// "¦^ґ_Іv HP",
	EM_WearEqType_ReMP,	// "¦^ґ_Іv MP",
	EM_WearEqType_ATK,	// "ATK §рА»¤O",
	EM_WearEqType_DEF,	// "DEF Ёѕїm¤O",
	EM_WearEqType_MDEF,	// "MDEF Е]ЄkЁѕїm",
	EM_WearEqType_MATK,	// "MATK Е]Єk§рА»",
	EM_WearEqType_HitPoint,	// "©R¤¤",
	EM_WearEqType_DodgePoint,	// "°jБЧ",
	EM_WearEqType_CritRate,   // "јЙА»µҐЇЕ",
	EM_WearEqType_CritPower,   // "јЙА»«В¤OҐ[±j(%)",
	EM_WearEqType_MagicCritRate,   // "ЄkіNјЙА»µҐЇЕ",
	EM_WearEqType_MagicCritPower,   // "ЄkіNјЙА»«В¤OҐ[±j(%)",
	EM_WearEqType_ParryRate,   // "®жѕЧµҐЇЕ",
	EM_WearEqType_AtkSpeed,   // "§рА»іt«Ч(%)",
	EM_WearEqType_MoveSpeed,	// "Іѕ°Кіt«Ч(%)",
	EM_WearEqType_DMG,   // "ЄZѕ№«В¤O ©Т¦іЄсѕФ©О»·µ{ ( DMG )",
	EM_WearEqType_AllResist,   // "§Ь©К ©Т¦і",
	EM_WearEqType_Resist_Earth,   // "§Ь©К ¦a",
	EM_WearEqType_Resist_Water,   // "§Ь©К ¤ф",
	EM_WearEqType_Resist_Fire,   // "§Ь©К ¤х",
	EM_WearEqType_Resist_Wind,   // "§Ь©К ­·",
	EM_WearEqType_Resist_Light,   // "§Ь©К Ґъ",
	EM_WearEqType_Resist_Darkness,   // "§Ь©К оо",
	EM_WearEqType_ManaDecrease,	// "MP®шЇУґо¤Ц(%)",
	EM_WearEqType_Exp,	// "ёgЕз­И",
	EM_WearEqType_Treasure,	// "±јД_Іv",
	EM_WearEqType_SecHand_HitRate,	// "°Ж¤в ©R¤¤Іv",
	EM_WearEqType_SecHand_DMG_Per,	// "°Ж¤в ¶Л®`¤O(DMG%)",
	EM_WearEqType_DEF_HeavyArmed,	// "ЁѕїmҐ[±j сZҐТ(%)",
	EM_WearEqType_DEF_LightArmed,	// "ЁѕїmҐ[±j ВкҐТ(%)",
	EM_WearEqType_DEF_Leather,	// "ЁѕїmҐ[±j ҐЦҐТ(%)",
	EM_WearEqType_DEF_Clothes,	// "ЁѕїmҐ[±j ¦зЄA(%)",
	EM_WearEqType_DEF_Robe,	// "ЁѕїmҐ[±j ЄшіT(%)",
	EM_WearEqType_DEF_Shield,	// "ЁѕїmҐ[±j ¬Ю(%)",
	EM_WearEqType_MagicPow_AllMagic, 	// "Е]Єk«В¤O ©Т¦і(%)",
	EM_WearEqType_MagicPow_Earth, 	// "Е]Єk«В¤O ¦a(%)",
	EM_WearEqType_MagicPow_Water, 	// "Е]Єk«В¤O ¤ф(%)",
	EM_WearEqType_MagicPow_Fire, 	// "Е]Єk«В¤O ¤х(%)",
	EM_WearEqType_MagicPow_Wind, 	// "Е]Єk«В¤O ­·(%)",
	EM_WearEqType_MagicPow_Light, 	// "Е]Єk«В¤O Ґъ(%)",
	EM_WearEqType_MagicPow_Darkness,	// "Е]Єk«В¤O оо(%)",
	EM_WearEqType_SpellSpeed_AllMagic, 	// "¬IЄkіt«Ч ©Т¦і(%)",
	EM_WearEqType_DMG_AllRange,	// "»·µ{ЄZѕ№«В¤O(%) ©Т¦і",
	EM_WearEqType_DMG_Bow,	// "»·µ{ЄZѕ№«В¤O(%) ¤}",
	EM_WearEqType_DMG_Cossbow,	// "»·µ{ЄZѕ№«В¤O(%) ¤Q¦r¤}",
	EM_WearEqType_DMG_Gun,	// "»·µ{ЄZѕ№«В¤O(%) єj",
	EM_WearEqType_DMG_AllWeapon,	// "ЄсѕФЄZѕ№«В¤O(%) ©Т¦і",
	EM_WearEqType_DMG_Unarmed,	// "ЄсѕФЄZѕ№«В¤O(%) ЄЕ¤в",
	EM_WearEqType_DMG_Blade,	// "ЄсѕФЄZѕ№«В¤O(%) јC",
	EM_WearEqType_DMG_Dagger,	// "ЄсѕФЄZѕ№«В¤O(%) ¤P­є",
	EM_WearEqType_DMG_Wand,	// "ЄсѕФЄZѕ№«В¤O(%) Еv§ъ",
	EM_WearEqType_DMG_Axe,	// "ЄсѕФЄZѕ№«В¤O(%) ©т",
	EM_WearEqType_DMG_Bludgeon,	// "ЄсѕФЄZѕ№«В¤O(%) ВсґТґО",
	EM_WearEqType_DMG_Claymore,	// "ЄсѕФЄZѕ№«В¤O(%) Вщ¤вјC",
	EM_WearEqType_DMG_Staff,	// "ЄсѕФЄZѕ№«В¤O(%) §ъ",
	EM_WearEqType_DMG_2H_Axe,	// "ЄсѕФЄZѕ№«В¤O(%) Вщ¤в©т",
	EM_WearEqType_DMG_2H_Hammer,	// "ЄсѕФЄZѕ№«В¤O(%) Вщ¤вВс",
	EM_WearEqType_DMG_Polearm,	// "ЄсѕФЄZѕ№«В¤O(%) єj(ЄшҐЩ)",
	EM_WearEqType_AttackSpeed_AllRange,	// "»·µ{ЄZѕ№іt«Ч(%) ©Т¦і",
	EM_WearEqType_AttackSpeed_Bow,	// "»·µ{ЄZѕ№іt«Ч(%) ¤}",
	EM_WearEqType_AttackSpeed_Cossbow,	// "»·µ{ЄZѕ№іt«Ч(%) ¤Q¦r¤}",
	EM_WearEqType_AttackSpeed_Gun,	// "»·µ{ЄZѕ№іt«Ч(%) єj", 
	EM_WearEqType_AttackSpeed_AllWeapon,	// "ЄсѕФЄZѕ№іt«Ч(%) ©Т¦і",
	EM_WearEqType_AttackSpeed_Unarmed,	// "ЄсѕФЄZѕ№іt«Ч(%) ЄЕ¤в",
	EM_WearEqType_AttackSpeed_Blade,	// "ЄсѕФЄZѕ№іt«Ч(%) јC",
	EM_WearEqType_AttackSpeed_Dagger,	// "ЄсѕФЄZѕ№іt«Ч(%) ¤P­є",
	EM_WearEqType_AttackSpeed_Wand,	// "ЄсѕФЄZѕ№іt«Ч(%) Еv§ъ",
	EM_WearEqType_AttackSpeed_Axe,	// "ЄсѕФЄZѕ№іt«Ч(%) ©т",
	EM_WearEqType_AttackSpeed_Bludgeon,	// "ЄсѕФЄZѕ№іt«Ч(%) ВсґТґО",
	EM_WearEqType_AttackSpeed_Claymore,	// "ЄсѕФЄZѕ№іt«Ч(%) Вщ¤вјC",
	EM_WearEqType_AttackSpeed_Staff,	// "ЄсѕФЄZѕ№іt«Ч(%) §ъ",
	EM_WearEqType_AttackSpeed_2H_Axe,	// "ЄсѕФЄZѕ№іt«Ч(%) Вщ¤в©т",
	EM_WearEqType_AttackSpeed_2H_Hammer,	// "ЄсѕФЄZѕ№іt«Ч(%) Вщ¤вВс",
	EM_WearEqType_AttackSpeed_Polearm,	// "ЄсѕФЄZѕ№іt«Ч(%) єj(ЄшҐЩ)",
	EM_WearEqType_WearEqAbility_Unarmed,	// "ёЛіЖЇа¤O ЄЕ¤в",
	EM_WearEqType_WearEqAbility_Blade,	// "ёЛіЖЇа¤O јC",	  	  	
	EM_WearEqType_WearEqAbility_Dagger,	// "ёЛіЖЇа¤O ¤P­є",
	EM_WearEqType_WearEqAbility_Wand,	// "ёЛіЖЇа¤O Еv§ъ",  	
	EM_WearEqType_WearEqAbility_Axe,	// "ёЛіЖЇа¤O ©т",	  	
	EM_WearEqType_WearEqAbility_Bludgeon,	// "ёЛіЖЇа¤O ВсґТґО",
	EM_WearEqType_WearEqAbility_Claymore,	// "ёЛіЖЇа¤O Вщ¤вјC",
	EM_WearEqType_WearEqAbility_Staff,	// "ёЛіЖЇа¤O §ъ",
	EM_WearEqType_WearEqAbility_Axe_2H,	// "ёЛіЖЇа¤O Вщ¤в©т",
	EM_WearEqType_WearEqAbility_Hammer_2H,	// "ёЛіЖЇа¤O Вщ¤вВс",
	EM_WearEqType_WearEqAbility_Polearm,	// "ёЛіЖЇа¤O єj(ЄшҐЩ)",
	EM_WearEqType_WearEqAbility_Bow,	// "ёЛіЖЇа¤O ¤}",
	EM_WearEqType_WearEqAbility_CossBow,	// "ёЛіЖЇа¤O ¤Q¦r¤}",
	EM_WearEqType_WearEqAbility_Gun,	// "ёЛіЖЇа¤O єj",
	EM_WearEqType_WearEqAbility_Throw,	// "ёЛіЖЇа¤O §лВY",
	EM_WearEqType_WearEqAbility_HeavyArmed,	// "ёЛіЖЇа¤O сZҐТ",
	EM_WearEqType_WearEqAbility_LightArmed,	// "ёЛіЖЇа¤O ВкҐТ",
	EM_WearEqType_WearEqAbility_Leather,	// "ёЛіЖЇа¤O ҐЦҐТ",
	EM_WearEqType_WearEqAbility_Clothes,	// "ёЛіЖЇа¤O ¦зЄA",
	EM_WearEqType_WearEqAbility_Robe,	// "ёЛіЖЇа¤O ЄшіT",
	EM_WearEqType_WearEqAbility_Shield,	// "ёЛіЖЇа¤O ¬Ю",
	EM_WearEqType_WearEqAbility_Implement,	// "ёЛіЖЇа¤O ¤в«щЄkѕ№",
	EM_WearEqType_WearEqAbility_SecondHand,	// "ёЛіЖЇа¤O Вщ¤в«щ",
	EM_WearEqType_SkillValue_Unarmed,	// "§ЮЇа¤W¤Й UnarmedЄЕ¤в",
	EM_WearEqType_SkillValue_Blade,	// "§ЮЇа¤W¤Й јC",
	EM_WearEqType_SkillValue_Dagger,	// "§ЮЇа¤W¤Й ¤P­є",
	EM_WearEqType_SkillValue_Wand,	// "§ЮЇа¤W¤Й Еv§ъ",
	EM_WearEqType_SkillValue_Axe,	// "§ЮЇа¤W¤Й ©т",
	EM_WearEqType_SkillValue_Bludgeon,	// "§ЮЇа¤W¤Й ВсґТґО",
	EM_WearEqType_SkillValue_Claymore,	// "§ЮЇа¤W¤Й Вщ¤вјC",
	EM_WearEqType_SkillValue_Staff,	// "§ЮЇа¤W¤Й §ъ",
	EM_WearEqType_SkillValue_2H_Axe,	// "§ЮЇа¤W¤Й Вщ¤в©т",
	EM_WearEqType_SkillValue_2H_Hammer,	// "§ЮЇа¤W¤Й Вщ¤вВс",
	EM_WearEqType_SkillValue_Polearm,	// "§ЮЇа¤W¤Й єj(ЄшҐЩ)",
	EM_WearEqType_SkillValue_Bow,	// "§ЮЇа¤W¤Й »·µ{ЄZѕ№ ¤}",
	EM_WearEqType_SkillValue_CossBow,	// "§ЮЇа¤W¤Й »·µ{ЄZѕ№ ¤Q¦r¤}",
	EM_WearEqType_SkillValue_Gun,	// "§ЮЇа¤W¤Й »·µ{ЄZѕ№ єj",
	EM_WearEqType_SkillValue_Define,	// "§ЮЇа¤W¤Й Ёѕїm",
	EM_WearEqType_SkillValue_BlackSmith,	// "§ЮЇа¤W¤Й ҐґЕK",
	EM_WearEqType_SkillValue_Carpenter,	// "§ЮЇа¤W¤Й ¤м¤u",
	EM_WearEqType_SkillValue_MakeArmor,	// "§ЮЇа¤W¤Й »sҐТ",
	EM_WearEqType_SkillValue_Tailor,	// "§ЮЇа¤W¤Й µфБ_",
	EM_WearEqType_SkillValue_Cook,	// "§ЮЇа¤W¤Й Іi¶№",
	EM_WearEqType_SkillValue_Alchemy,	// "§ЮЇа¤W¤Й ·ТЄч",
	EM_WearEqType_HQ_BlackSmith,	// "HQ«~ѕчІv¤W¤Й ҐґЕK(%)",
	EM_WearEqType_HQ_Carpenter,	// "HQ«~ѕчІv¤W¤Й¤W¤Й ¤м¤u(%)",
	EM_WearEqType_HQ_MakeArmor,	// "HQ«~ѕчІv¤W¤Й¤W¤Й »sҐТ(%)",
	EM_WearEqType_HQ_Tailor,	// "HQ«~ѕчІv¤W¤Й¤W¤Й µфБ_(%)",
	EM_WearEqType_HQ_Cook,	// "HQ«~ѕчІv¤W¤Й¤W¤Й Іi¶№(%)",
	EM_WearEqType_HQ_Alchemy,	// "HQ«~ѕчІv¤W¤Й¤W¤Й ·ТЄч(%)",	
	EM_WearEqType_ATK_Per,	// "ATK §рА»¤O (%)",
	EM_WearEqType_DEF_Per,	// "DEF Ёѕїm¤O (%)",
	EM_WearEqType_StealRate,
	EM_WearEqType_DropMoneyRate,	// "Єчїъ±јёЁІv (%)",
	EM_WearEqType_HateRate,	// "¤і«л«ЧјWҐ[ (%)",
	EM_WearEqType_ReSP_Per_Warrior,	// "ѕФ¤hSP ¦^ґ_Ґ[¦К¤А¤с",
	EM_WearEqType_ReSP_Per_Ranger,	// "№C«LSP ¦^ґ_Ґ[¦К¤А¤с",
	EM_WearEqType_ReSP_Per_Rogue,	// "µsёйSP ¦^ґ_Ґ[¦К¤А¤с",
	EM_WearEqType_MAbsorbRate,	// "Е]Єk§l¦¬Іv",
	EM_WearEqType_AbsorbRate,	// "Ёѕїm§l¦¬Іv",
	EM_WearEqType_HealAbsorbRate,	// "ЄvАш§l¦¬Іv",
	EM_WearEqType_MAbsorb,	// "Е]Єk§l¦¬ВIјЖ",
	EM_WearEqType_Absorb,	// "Ёѕїm§l¦¬ВIјЖ",
	EM_WearEqType_HealAbsorb,	// "ЄvАш§l¦¬ВIјЖ",
	EM_WearEqType_MDmgPoint,   // "Е]Єk¶Л®`Ґ[ВI",
	EM_WearEqType_HealPowerRate,	// "ЄvАш«В¤OјWҐ[Іv",
	EM_WearEqType_HealPoint,	// "ЄvАш«В¤OјWҐ[ВIјЖ",
	EM_WearEqType_RangeAttackRange,	// "»·µ{ЄZѕ№§рА»¶ZВчјWҐ[",

	EM_WearEqType_SkillValue_Lumbering,	// "§ЮЇа¤W¤Й µ­¤м",
	EM_WearEqType_SkillValue_Herbalism,	// "§ЮЇа¤W¤Й ±ДГД",
	EM_WearEqType_SkillValue_Mining,	// "§ЮЇа¤W¤Й «хДq",
	EM_WearEqType_SkillValue_Fishing,	// "§ЮЇа¤W¤Й іЁіЅ",

	EM_WearEqType_HQ_Lumbering,	// "HQ«~ѕчІv¤W¤Й µ­¤м",
	EM_WearEqType_HQ_Herbalism,	// "HQ«~ѕчІv¤W¤Й ±ДГД",
	EM_WearEqType_HQ_Mining,	// "HQ«~ѕчІv¤W¤Й «хДq",
	EM_WearEqType_HQ_Fishing,	// "HQ«~ѕчІv¤W¤Й іЁіЅ",

	EM_WearEqType_SkillValue_GatherRate,

	EM_WearEqType_STR_Rate,   // "ДЭ©К STR ¤O¶q(%)",
	EM_WearEqType_STA_Rate,	// "ДЭ©К STA ­@¤O(%)",			
	EM_WearEqType_INT_Rate,	// "ДЭ©К INT ґј¤O(%)",			
	EM_WearEqType_MND_Rate,	// "ДЭ©К MND ЖF¤O(%)",
	EM_WearEqType_AGI_Rate,	// "ДЭ©К AGI ±У±¶(%)",			
	EM_WearEqType_AllState_Rate,   // "ДЭ©К ©Т¦і(%)(°тҐ»¤­­УДЭ©К)",
	EM_WearEqType_MaxHP_Rate,	// "¤W­­ HP(%)",
	EM_WearEqType_MaxMP_Rate,	// "¤W­­ MP(%)",
	EM_WearEqType_RideSpeed,	// "ГM­јіt«Ч(%)",
	EM_WearEqType_MDEF_Rate,	// "MDEF(%) Е]ЄkЁѕїm",
	EM_WearEqType_MATK_Rate,	// "MATK(%) Е]Єk§рА»",
	EM_WearEqType_ShieldBlockRate,   // "¬ЮѕЧµҐЇЕ",
	EM_WearEqType_DMG_Per,   // "ЄZѕ№«В¤O ©Т¦іЄсѕФ©О»·µ{ ( DMG% )",
	EM_WearEqType_Exp_SubJob,	// "°ЖВѕёgЕз­И"
	EM_WearEqType_TP_Rate,	// "TPёgЕз­И"

	EM_WearEqType_SkillValue_LumberingRate,
	EM_WearEqType_SkillValue_HerbalismRate,
	EM_WearEqType_SkillValue_MiningRate,
	EM_WearEqType_SkillValue_FishingRate,
	EM_WearEqType_Gravity,	// "­«¤O­ЧҐї"  ,

	EM_WearEqType_DoubleAttack,	// "Ёв¦ё§рА»" ,

	EM_WearEqType_ResistCritRate,   // "§ЬјЙµҐА»",
	EM_WearEqType_ResistMagicCritRate,   // "©і§ЬЕ]Єk­P©R¤@А»Іv",

	//©T©wЄkіN®ДЄG
	EM_WearEqType_Magic_EarthPower,	// "¦aДЭ©К§рА»" ,
	EM_WearEqType_Magic_WaterhPower,	// "¤фДЭ©К§рА»" ,
	EM_WearEqType_Magic_FirePower,	// "¤хДЭ©К§рА»" ,
	EM_WearEqType_Magic_WindhPower,	// "­·ДЭ©К§рА»" ,
	EM_WearEqType_Magic_LightPower,	// "ҐъДЭ©К§рА»" ,
	EM_WearEqType_Magic_DarkPower,	// "·tДЭ©К§рА»" ,

	EM_WearEqType_JumpRate,	// "ёхЕD¤сІv"   ,
	EM_WearEqType_MDMG,   // "Е]ЄkјЬ®`ВIјЖ"	,
	EM_WearEqType_MDMG_Per,   // "Е]ЄkјЬ®`¤сІv"	,

	EM_WearEqType_ResistShieldBlock,   // "Ёл¬п¬ЮАЙ",
	EM_WearEqType_ResistParry,   // "Ёл¬п®жѕЧ",

	EM_WearEqType_MagicHitPoint,   // "Е]Єk©R¤¤",
	EM_WearEqType_MagicDodgePoint,   // "Е]Єk°{БЧ",

	EM_WearEqType_PhyHitRate,   // "Є«Іz©R¤¤Іv",
	EM_WearEqType_PhyDodgeRate,   // "Є«Іz°{БЧІv",
	EM_WearEqType_MagicHitRate,   // "Е]Єk©R¤¤Іv",
	EM_WearEqType_MagicDodgeRate,   // "Е]Єk°{БЧІv",

	EM_WearEqType_GatherGenerateRate,   // "±Д¶°ІЈҐX¶qјWҐ[Іv",
	EM_WearEqType_GatherSpeedRate,   // "±Д¶°іt«ЧјWҐ[Іv",
	EM_WearEqType_GatherExpRate,   // "±Д¶°ёgЕз­ИјWҐ[Іv",

	EM_WearEqType_AttackPlayerPowerRate,   // "(§р)§рА»Є±®a(orЄ±®aГdЄ«)«В¤OҐ[ґо",
	EM_WearEqType_AttackNPCPowerRate,   // "(§р)§рА»NPC«В¤OҐ[ґо",
	EM_WearEqType_PlayerDefRate,   // "(¦u)іQЄ±®a(orЄ±®aГdЄ«)§рА»«В¤OҐ[ґо",
	EM_WearEqType_NPCDefRate,   // "(¦u)іQNPC§рА»«В¤OҐ[ґо",
	EM_WearEqType_AEMagicPowerRate,   // "(§р)AEЄkіN§рА»«В¤OҐ[ґо",
	EM_WearEqType_AEMagicDefRate,   // "(¦u)іQACЄkіN§рА»«В¤OҐ[ґо",

	EM_WearEqType_CraftSpeedRate,	// "ҐНІЈіt«ЧјWҐ[Іv"
	EM_WearEqType_CraftExpRate,	// "ҐНІЈёgЕз­ИјWҐ[Іv"
	EM_WearEqType_AddPlotExpRate,	// "јWҐ[ј@±Ўµ№¤©ЄєёgЕз­ИІv"
	EM_WearEqType_AddPlotTPRate,	// "јWҐ[ј@±Ўµ№¤©ЄєTPІv"

	EM_WearEqType_SP_Warrior_Consume,	// "ѕФ·N®шЇУґо¤Ц(%)"
	EM_WearEqType_SP_Rogue_Consume,	// "єл¤O®шЇУґо¤Ц(%)"
	EM_WearEqType_SP_Ranger_Consume,	// "±MЄ`®шЇУґо¤Ц(%)"

	EM_WearEqType_NPC_EXP_Rate,	// "NPC ёgЕз­И­ЧҐї(%)"
	EM_WearEqType_NPC_TP_Rate,	// "NPC TPёgЕз­И­ЧҐї(%)"
	EM_WearEqType_AttackRange,	// "Єсµ{ЄZѕ№§рА»¶ZВчјWҐ[",

	EM_WearEqType_AllResist_Per,   // "§Ь©К ©Т¦і%",
	EM_WearEqType_Resist_Earth_Per,   // "§Ь©К ¦a%",
	EM_WearEqType_Resist_Water_Per,   // "§Ь©К ¤ф%",
	EM_WearEqType_Resist_Fire_Per,   // "§Ь©К ¤х%",
	EM_WearEqType_Resist_Wind_Per,   // "§Ь©К ­·%",
	EM_WearEqType_Resist_Light_Per,   // "§Ь©К Ґъ%",
	EM_WearEqType_Resist_Darkness_Per,   // "§Ь©К оо%",

	EM_WearEqType_ParryRate_Per,	// "®жѕЧ%"
	EM_WearEqType_Daily_EXP_Rate,	// "ЁC¤йҐф°ИёgЕз­ИҐ[­ј"
	EM_WearEqType_Daily_TP_Rate,	// "ЁC¤йҐф°ИTPҐ[­ј"

	EM_WearEqType_MaxCount,
};

#define Max_WearEqType_Count   10   //іМ¦h¦іґX­УДЭ©КҐiҐ[
struct  GameObjectWearEqStruct
{
	WearEqTypeENUM  EqType[Max_WearEqType_Count];
	int             EqTypeValue[Max_WearEqType_Count];

	int				OnAttackMagicRate;		//µoҐНѕчІv
	int				OnAttackMagicRank;
	int				OnAttackMagicID;		//ЄkіNё№ЅX
};
//----------------------------------------------------------------------------------------
//Є««~±јёЁЄєµІєc
//----------------------------------------------------------------------------------------
struct ItemDropStruct
{
	int		OrgID;					//Є««~ё№ЅX
	int		Rate;					//±јёЁѕчІv
};
//----------------------------------------------------------------------------------------
//ДЭ©уPlayerЄє°тҐ»ёк®Ж
//----------------------------------------------------------------------------------------
struct GameObjectPlayerStruct
{
	int     None;
};
//----------------------------------------------------------------------------------------
//BodyObjЄєҐNЄнГю«¬¦CБ|
//----------------------------------------------------------------------------------------
enum BodyObjectTypeENUM
{
	EM_BodyObjectType_Hair = 0,
	EM_BodyObjectType_Face,
};

//----------------------------------------------------------------------------------------
//ДЭ©уBodyObjЄє°тҐ»ёк®Ж
//----------------------------------------------------------------------------------------
struct GameObjectBodyObjStruct
{
	//int     None;
	BodyObjectTypeENUM Type;

	struct
	{
		union
		{
			int		Setting;
			struct
			{
				bool	CreateRole : 1;
				bool	DepartmentStore : 1;
			};
		};
		RaceFlagStruct      Race;
		SexFlagStruct       Sex;

	}Limit;

	int Priority;
};

//----------------------------------------------------------------------------------------
// Quest  ёФІУ¤є®eЄєёк®Ж, ґy­z¤F Quest ¶}АY / №Lµ{ / јъАy
//----------------------------------------------------------------------------------------
struct GameObjectQuestDetail
{
	// §PВ_іЎҐч
	// ҐО§@ Client §PВ_ User ¬O§_ҐiҐH±µ Quest ( ¤]ҐО°µ¦b Client¤О¤p¦a№П¤WЕгҐЬ )
	//---------------------------------------------------------
	int		iCheck_Class_Type;
	int		iCheck_Class1;
	int		iCheck_Class2;
	int		iCheck_Sex;
	int		iCheck_LV;
	int		iCheck_LowerLV;
	int		iCheck_HigherLV;
	int		iCheck_Title;
	int		iCheck_Group;
	int		iCheck_Flag[_MAX_SIZE_QUEST_FLAG];

	int		iCheck_RepGroup;	// Ґi±µҐф°ИЄєБn±жёsІХ
	int		iCheck_Rep;			// Бn±ж­nЁD


	int		iCheck_ObjID[_MAX_SIZE_QUEST_ITEM];
	int		iCheck_ObjVal[_MAX_SIZE_QUEST_ITEM];
	int		iCheck_Loop;


	// Ґф°И»Ў©ъ
	//---------------------------------------------------------
	int		iQuestID;
	char* szQuest_Name;
	char* szQuest_Desc;

	char* szQuest_Accept_Detail;
	char* szQuest_UnComplete_Detail;
	char* szQuest_Complete_Detail;

	int		iQuest_Catalog;
	int		iQuest_Type;

	// ±µЁьҐф°И«б©ТёmЄєЄ¬єA©Оµ№¤©ЄєЄ««~
	//--------------------------------------------------------
	int		iAccept_FlagOn[_MAX_SIZE_QUEST_FLAG];
	int		iAccept_FlagOff[_MAX_SIZE_QUEST_FLAG];
	int		iAccept_ItemID[_MAX_SIZE_QUEST_ITEM];
	int		iAccept_ItemVal[_MAX_SIZE_QUEST_ITEM];

	// §№¦ЁҐф°И©Т¶·­nЄє±шҐу
	//--------------------------------------------------------
	int		iRequest_ItemID[_MAX_SIZE_QUEST_CONDITION];
	int		iRequest_ItemVal[_MAX_SIZE_QUEST_CONDITION];
	int		iRequest_KillID[_MAX_SIZE_QUEST_ITEM];
	int		iRequest_KillVal[_MAX_SIZE_QUEST_ITEM];
	int		iRequest_Timer;
	int		iRequest_Money;
	int		iRequest_QuestID;

	// ±µЁьҐф°И«б¶i¦жҐф°И®Й, №пЇS©wҐШјР°К§@, ©ТІЈҐНЄєјvЕT
	//--------------------------------------------------------
	int		iProcess_ClickToGetItem_ObjID[_MAX_SIZE_QUEST_CONDITION];
	int		iProcess_ClickToGetItem_ProcPercent[_MAX_SIZE_QUEST_CONDITION];
	int		iProcess_ClickToGetItem_GetItem[_MAX_SIZE_QUEST_CONDITION];
	int		iProcess_ClickToGetItem_GetItemVal[_MAX_SIZE_QUEST_CONDITION];

	int		iProcess_KilltoGetItem_ObjID[_MAX_SIZE_QUEST_CONDITION];
	int		iProcess_KilltoGetItem_ProcPercent[_MAX_SIZE_QUEST_CONDITION];
	int		iProcess_KilltoGetItem_GetItem[_MAX_SIZE_QUEST_CONDITION];
	int		iProcess_KilltoGetItem_GetItemVal[_MAX_SIZE_QUEST_CONDITION];

	// §№¦ЁҐф°И«б, ©Т±µ¦¬ЄєјъЅа
	//--------------------------------------------------------
	int		iReward_ChoiceID[_MAX_SIZE_QUEST_ITEM];
	int		iReward_ChoiceVal[_MAX_SIZE_QUEST_ITEM];
	int		iReward_ItemID[_MAX_SIZE_QUEST_ITEM];
	int		iReward_ItemVal[_MAX_SIZE_QUEST_ITEM];
	int		iReward_Spell[_MAX_SIZE_QUEST_ITEM];
	int		iReward_Money;
	int		iReward_Exp;

	int		iReward_RepGroup[2];
	int		iReward_Rep[2];

	// §№¦ЁҐф°И«б, ©Ті]©wЄєЄ¬єA
	//--------------------------------------------------------
	int		iComplete_ShowQuest;
	int		iComplete_FlagOn[_MAX_SIZE_QUEST_FLAG];
	int		iComplete_FlagOff[_MAX_SIZE_QUEST_FLAG];

	// §№¦ЁҐф°И«б, ©Ті]©wЄєЄ¬єA
	//--------------------------------------------------------
	char	szScript_OnBegin[_MAX_SIZE64_QUEST_SCRIPT];
	char	szScript_OnAccept[_MAX_SIZE64_QUEST_SCRIPT];
	char	szScript_OnComplete[_MAX_SIZE64_QUEST_SCRIPT];
	char	szScript_OnClickObjBegin[_MAX_SIZE64_QUEST_SCRIPT];
	char	szScript_OnClickObjEnd[_MAX_SIZE64_QUEST_SCRIPT];

	// ЁC¤й­­Ёо«¬Ґф°И
	//--------------------------------------------------------
	int		iQuestGroup;			// Ґф°ИёsІХ­­Ёо 0 ¬°¤Ј­­ЁоёsІХ 1-20 ¬°ёsІХ
	int		iControlType;			// ­«ВР«¬Ґф°И§№¦Ё®Й¶ЎЄє­­Ёо	0 ¤Ј­­Ёо 1 ¬°ЁC¤й§№¦Ё¤@¦ё 2 ¬°ЁC©P§№¦Ё¤@¦ё

	int		iRequest_ItemStr[_MAX_SIZE_QUEST_CONDITION];
	int		iRequest_KillStr[_MAX_SIZE_QUEST_ITEM];
	int		iQuestGroupLV;			// ЁC¤йҐф°ИµҐЇЕ»ЭЁD
	int		iReward_Honor;
	int		iReward_TP;				// јъЅа TP Єє¤сЁТ ( №wі]¬O 10% )

	int		iQuestProvider[5];
	int		iDeleteQuestItem[5];
	int		iDeleteQuestItemCount[5];
	int		iRequestCompleteNPCID[3];		// ІД¤GІХ«ь©w§№¦Ё NPC
	int		iQuestLinkNPC[5];

	char	szScript_OnDelete[_MAX_SIZE64_QUEST_SCRIPT];	// ©с±уҐф°И©Т¶]Єєј@±Ў ( SERVER SIDE )
	int		iCheck_ObjStatus[_MAX_SIZE_QUEST_ITEM];	// АЛ¬d±µ±шҐу­nЁD©ОЄМ¬OёT¤о

	bool	bDisableTeam;			// ГцІ¦№О¶¤А»±юҐф°И©Зі]©w

	char	szScript_RequestString[_MAX_SIZE64_QUEST_SCRIPT];

	PriceTypeENUM	iReward_CoinType;		//ЇS®н№ф­ИГю«¬
	int				iReward_SpecialCoin;	//ЇS®н№ф­И

	PriceTypeENUM	iRequest_CoinType;		//ЇS®н№ф­ИГю«¬
	int				iRequest_SpecialCoin;	//ЇS®н№ф­И

};
//----------------------------------------------------------------------------------------
// »І§UЄkіNГю«¬
//----------------------------------------------------------------------------------------
enum	GameObjectMagicTypeENUM
{
	EM_Magic_Type_NULL = -1,
	EM_Magic_Type0,
	EM_Magic_Type1,
	EM_Magic_Type2,
	EM_Magic_Type3,
	EM_Magic_Type4,
	EM_Magic_Type5,
	EM_Magic_Type6,
	EM_Magic_Type7,
	EM_Magic_Type8,
	EM_Magic_Type9,
	EM_Magic_Type10,
	EM_Magic_Type11,
	EM_Magic_Type12,
	EM_Magic_Type13,
	EM_Magic_Type14,
	EM_Magic_Type15,
	EM_GameObjectMagicTypeEM_FORCE_DWORD = 0x7fffffff,
};
//----------------------------------------------------------------------------------------
//NPC¬IЄkЄєµІєc
//----------------------------------------------------------------------------------------
#define	_MAX_NPC_SPELL_MAGIC_COUNT_	8
#define	_MAX_NPC_STEAL_ITEM_COUNT_	3
#define	_MAX_NPC_AVOID_MAGIC_TYPE_COUNT_	5
//¬IЄk®Йѕч
enum NPCSpellRightTimeENUM
{
	EM_NPCSpellRightTime_None,//µL
	EM_NPCSpellRightTime_Normal,//¤@ЇлЄ¬єA   (60¬н)
	EM_NPCSpellRightTime_OnAttack,//ѕФ°«±Т°К
	EM_NPCSpellRightTime_Attack,//ѕФ°«Є¬єA   (1¬н)
	EM_NPCSpellRightTime_AttackHPHalf,//ѕФ°«HP1/2  (1¬н)
	EM_NPCSpellRightTime_AttackHPQuarter,//ѕФ°«HP1/4  (1¬н)
	EM_NPCSpellRightTime_FriendDead,//¤Н¤и¦є¤`   (1¬н)
	EM_NPCSpellRightTime_KeepDistance,//«O«щ¶ZВч   (1¬н)

};
//NPC¬IЄkҐШјР
enum NPCSpellTargetENUM
{
	EM_NPCSpellTarget_Self,//¦Ы¤v
	EM_NPCSpellTarget_Target,//ҐШјР
	EM_NPCSpellTarget_Target_HP_Half,//ҐШјР(HP 1/2)
	EM_NPCSpellTarget_Target_HP_Quarter,//ҐШјР(HP 1/4)
	EM_NPCSpellTarget_Target_Distance_30_,//ҐШјР(¶ZВч30ҐH¤W )
	EM_NPCSpellTarget_Target_Escape,//ҐШјР(°k¶])

	EM_NPCSpellTarget_Friend,//¤Н¤и
	EM_NPCSpellTarget_Friend_HP_Half,//¤Н¤и(HP 1/2)
	EM_NPCSpellTarget_Friend_HP_Quarter,//¤Н¤и(HP 1/4)
	EM_NPCSpellTarget_Enemy,//јД¤и
	EM_NPCSpellTarget_Enemy_HP_Half,//јД¤и(HP 1/2)
	EM_NPCSpellTarget_Enemy_HP_Quarter,//јД¤и(HP 1/4)

	EM_NPCSpellTarget_TargetSpell,//ҐШјР¬IЄk	  
	EM_NPCSpellTarget_Enemy_RangeEnemySpell,//¤і«лЄн¤єЄє¬IЄkҐШјР 
	EM_NPCSpellTarget_Enemy_Second,//¤і«лІД¤G
	EM_NPCSpellTarget_Enemy_Last,//¤і«ліМ¤p
	EM_NPCSpellTarget_Enemy_Nearest,//¤і«лЄніМЄсЄє
	EM_NPCSpellTarget_Enemy_Farest,//¤і«лЄніМ»·Єє

	EM_NPCSpellTarget_Master,//ҐD¤H
	EM_NPCSpellTarget_Master_HP_Half,//ҐD¤H(HP 1/2)
	EM_NPCSpellTarget_Master_HP_Quarter,//ҐD¤H(HP 1/4)

};

struct  NPCSpellBaseStruct
{
	NPCSpellRightTimeENUM	RigthTime;	//¬IЄk®Йѕч
	NPCSpellTargetENUM		TargetType;	//ҐШјРГю«¬
	int		Rate;						//µo°КѕчІv
	int		MagicID;					//ЄkіN
	int		MagicLv;					//ЄkіNµҐЇЕ
	char* SpellString;
};
//----------------------------------------------------------------------------------------
enum	PhyResistTypeENUM
{
	EM_PhyResist_NULL = -1,	//µL
	EM_PhyResist_Hit,		//ҐґА»
	EM_PhyResist_Cut,		//¬еА»
	EM_PhyResist_Puncture,		//¬пЁл
	EM_PhyResist_MAX,
};
//----------------------------------------------------------------------------------------
enum SpecialDropItemTypeENUM
{
	EM_SpecialDropItemType_None,
	EM_SpecialDropItemType_DepartmentStroe,
	EM_SpecialDropItemType_NoGlobalDrop,	//Ёъ®шҐ@¬Й±јёЁ
};
//----------------------------------------------------------------------------------------
enum NPCCarryTypeENUM
{
	EM_NPCCarryType_None,
	EM_NPCCarryType_NoControl,
	EM_NPCCarryType_Control,
};
//----------------------------------------------------------------------------------------
//ДЭ©уNPCЄє°тҐ»ёк®Ж
//----------------------------------------------------------------------------------------
struct GameObjectNpcStruct
{
	int			Race;					//єШ±Ъ
	int			Vocation;				//Вѕ·~
	int			Vocation_Sub;			//°ЖВѕ
	int         Sex;                    //©К§O
	int 		ExpRate;				//©ЗЄ«Ґґ¦є«бҐi±oЁмЄєёgЕз­И¦К¤А¤с
	int			TpRate;
	int			Level;					//°тҐ»µҐЇЕ
	int         LevelRange;


	int			STR;
	int			STA;
	int			INT;
	int			MND;
	int			AGI;

	int         MoveSpeed;
	int         SearchRange;

	int         DropMoney_Base;
	int         DropMoney_Rand;

	ItemDropStruct DropItem[_MAX_DROPITEM_COUNT_];
	ItemDropStruct DropItemAbility[_MAX_DROP_ABILITY_COUNT_];

	int         MainHandEQ[_MAX_NPC_EQSEL_COUNT_];        //ЁC¤@­У 20 %ѕч·|·|ёЛіЖ
	int         SecHandEQ[_MAX_NPC_EQSEL_COUNT_];          //ЁC¤@­У 20 %ѕч·|·|ёЛіЖ

	int			iQuestMode;

	char* szQuestNpcText;								// Ґф°И NPC ©Т»ЎЄє¶OёЬ
	char* szQuestDoingText;
	int			iQuestID[_MAX_NPC_QUEST_COUNT_];			// Ґф°И NPC ©ТЇаµ№Єє Quest

	int         Brave;              //«iґ±«Ч
	int         LinkType;           //©ЗЄ«іsВкГю«¬
	int			EscapeType;			//°k¶]Гю«¬


	NPCSpellBaseStruct		Spell[_MAX_NPC_SPELL_MAGIC_COUNT_];

	NpcTitleTypeENUM		TitleType;
	//GameObjDbStruct*		pQuestLink[ _MAX_QUESTLINK_COUNT ];
	int						iQuestLinkObj[_MAX_QUESTLINK_COUNT];


	int			iQuestNPCRefID;

	int					DB_ResistID;				//ёк®Ж®wГю«¬id(©IІ¤)
	MagicResistENUM		ResistType;					//NPCЄєДЭ©К	
	MagicResistStruct	Resist;						//§Ь©К­И

	char		szLuaScript[_MAX_SIZE64_QUEST_SCRIPT];						// Єм©l¤Жј@±Ў©О№пёЬ®Й©Тёь¤JЄє LUA

	//ҐiіQ°ЅЕСЄєЄ««~
	ItemDropStruct			StealItem[_MAX_NPC_STEAL_ITEM_COUNT_];

	//ҐiіQ°ЅЕСЄєЄчїъ
	int         StealMoney_Base;
	int         StealMoney_Rand;

	//µL®ДЄkіN
	GameObjectMagicTypeENUM	AvoidMagicType[_MAX_NPC_AVOID_MAGIC_TYPE_COUNT_];

	MagicEffectStruct	Effect;						//¦і®ДЄєЄєЄkіN®ДЄG

	int			RaceGroupType;						//іsВкА°§UЄєёsІХ
	int			iQuestObjType;						//ҐфЄ«Є«ҐуВIїп«б¬O§_·|®шҐў

	int			KeepDistance;						//ѕФ°«®Й «O«щ¶ZВч

	GameObjectWeaponENUM	OrgWeaponType;			//ЄZѕ№Гю«¬
	GameObjectArmorENUM     OrgArmorType;			//ЁѕЁгГю«¬

	int			FollowRange;						//Ґi°lіv¶ZВч

	int			RaiseChangeNPCID[2];
	int			RaiseChangeRate[2];					//¤QёU¤А¤§¤@

	bool		IsAbilityStandard;					//¬O§_¬°јР·ЗјЖ­И
	int			AbilityValue[2];					//°С¦ТјЖВѕ

	GameObjectWearEqStruct	Ability;				//ЄюҐ[Їа¤O	

//	bool		IsNoMPSP;							//ЁS¦іMPSP

	char* TitleName;							//npcАY»О

	float		PhyResistRate[EM_PhyResist_MAX];	//Є«Іz§Ь©К

	int			QuestObjCraftMode;					// Ґф°ИЄ«Ґу¬O§_¬OЁу§UЁоіy¤uЁг

	int			ZoneID;								//npc©Т¦bЄє¦мёm
	int			X, Y, Z;

	//	bool		IsAllZoneVisible;					//Ґю°ПЕгҐЬ
	//	bool		IsShowNpcPos;
	//	bool		AutoRotate;							//¬IЄk¦Ы°К­±¦V

	int			ZoneID_Bk;							//Ґшµeі]©wЄє°П°мёк®Ж
	int			BowEQ;								//ёЛіЖ¤}Дж¦м
	CampID_ENUM	CampID;
	int			KillGroupID;						//±ю¦є°OјЖ -1 ¤ЈіBІz	

	char		szLuaInitScript[_MAX_SIZE64_QUEST_SCRIPT];						// №wі]Єм©l¤Жј@±Ўёк®Ж
	int			AddPowerLV;							//±јёЁµҐЇЕјWҐ[

	char		LuaEvent_OnAttackMagic[_MAX_SIZE64_QUEST_SCRIPT];
	char		LuaEvent_OnAssistMagic[_MAX_SIZE64_QUEST_SCRIPT];
	char		LuaEvent_OnDead[_MAX_SIZE64_QUEST_SCRIPT];
	char		LuaEvent_OnKill[_MAX_SIZE64_QUEST_SCRIPT];

	StructNPCOtherFlag			OtherFlag;
	//bool		IsShowMaster;						//ЕгҐЬҐD¤Hёк°T
	int			DEqTypeValue[10];					//Їа¤O·LЅХ
	SpecialDropItemTypeENUM SpecialDropItemType;
	int			MinAbilityLv;
	int			OrnamentEQ;							//ёЛ№ў«~

	char		szLuaDisplayScript[_MAX_SIZE64_QUEST_SCRIPT];						// №wі]Єм©l¤Жј@±Ўёк®Ж

	union
	{
		int		_NpcMode;
		struct
		{
			bool		IsNoMPSP : 1;			//ЁS¦іMPSP
			bool		IsAllZoneVisible : 1;			//Ґю°ПЕгҐЬ
			bool		IsShowNpcPos : 1;
			bool		AutoRotate : 1;			//¬IЄk¦Ы°К­±¦V
			bool		IsShowMaster : 1;			//ЕгҐЬҐD¤Hёк°T
			bool		NoRandReborn : 1;			//¤Ј¶ГјЖ¦мёm­«ҐН
			bool		ClientDelNoWaitting : 1;			//Є«Ґу§R°Ј®ЙҐЯ§Y¤ЈЁЈ
			bool		Prompt : 1;			//±µЄс®ЙґЈҐЬ
			bool		Treasure : 1;			//Д_Ѕc
			bool		SearchEnemyAllDir : 1;			//ЇБјД©їІ¤­±¦V
			bool		HideRaceInfo : 1;			//¤ЈЕгҐЬєШ±Ъ
			bool		IgnoreNpcKill : 1;			//(±јД_)©їІ¤©ЗЄ«±ю¦є
			bool		IgnoreQuestObjEffect : 1;			//(Ґф°И)Ґф°ИЄ«Ґу®Й¤ЈµoҐъ
			bool		IsAttackFirst : 1;			//(Client)§рА»§PВ_АuҐэ
			bool		Invisible_InWagon : 1;			//(Client)ёьЁг¤є¬Э¤ЈЁЈ
			bool		ReCalPath : 1;			//¤Ѕ·|ѕФ»Э­n­«євёф®|ЄєЄ«Ґу
			bool		DisableTreasureDrop : 1;			//¤Ј·|±јД_
			bool		IsTrapObj : 1;			//(client)іґЁАЄ«Ґу
			bool		NoFadein : 1;			//(client)¤ЈІH¤J
			bool		IgnorePvPRule : 1;			//©їІ¤І{¦жҐЗіBІz
			bool		RideAttackable : 1;			//ёьЁг·|іQ§рА»
			bool		ScriptFlag : 1;			//¤p©ЗІM°ЈЁч,»Э­nІM°Ј
			bool		NoReturnAttackPoint : 1;			//¤Ј¦^Ём¶}©lѕФ°«ВI
			bool		IgnoreCollisionCheck : 1;			//(client)©їІ¤ёIјІАЛ¬d
		};
	};

	int					iTagID;							// Є©Ґ»±±Ёо TAG
	int					CarryCount;
	NPCCarryTypeENUM	CarryType;						//ёьЁгГю«¬
	int					SearchGroupID;
	char				CarryLuaScript[_MAX_SIZE64_QUEST_SCRIPT];	//¤W¤UёьЁгДІµoј@±Ў					
	int					CarryShootRange;
	int					CarryShootAngle;
	GameObjectWearEqStruct	AbilityEx;				//ЄюҐ[Їа¤O	

	char				LuaEvent_BeginAttack[_MAX_SIZE64_QUEST_SCRIPT];
	char				LuaEvent_EndAttack[_MAX_SIZE64_QUEST_SCRIPT];
	char				CheckCarryLuaScript[_MAX_SIZE64_QUEST_SCRIPT];	//¤W¤UёьЁгДІµoј@±Ў
	char				CheckCarryLuaPoint[_MAX_FILENAME_SIZE_];		//°С¦ТВI

	void		Init()
	{
		char* _szQuestNpcText = szQuestNpcText;
		char* _szQuestDoingText = szQuestDoingText;
		char				_szLuaScript[_MAX_SIZE64_QUEST_SCRIPT];

		char* _szTitleName = TitleName;


		//GameObjDbStruct*	_pQuestLink[ _MAX_QUESTLINK_COUNT ];
		int					_iQuestLinkObj[_MAX_QUESTLINK_COUNT];
		int					_iQuestID[20];

		int					_iQuestMod = iQuestMode;
		int					_iQuestNPCRefID = iQuestNPCRefID;
		int					_iQuestObjType = iQuestObjType;
		int					_QuestObjCraftMode = QuestObjCraftMode;

		int					_ZoneID = ZoneID;
		int					_X = X;
		int					_Y = Y;
		int					_Z = Z;
		//bool				_IsAllZoneVisible    = IsAllZoneVisible;
		int					_TempNpcMode = _NpcMode;

		memcpy(_iQuestLinkObj, iQuestLinkObj, sizeof(iQuestLinkObj));
		memcpy(_iQuestID, iQuestID, sizeof(_iQuestID));
		memcpy(_szLuaScript, szLuaScript, sizeof(szLuaScript));

		//memset( this , 0 , sizeof(*this) );							  

		szQuestNpcText = _szQuestNpcText;
		szQuestDoingText = _szQuestDoingText;
		iQuestMode = _iQuestMod;
		iQuestNPCRefID = _iQuestNPCRefID;
		iQuestObjType = _iQuestObjType;
		TitleName = _szTitleName;
		QuestObjCraftMode = _QuestObjCraftMode;
		//		IsAllZoneVisible    = _IsAllZoneVisible;
		_NpcMode = _TempNpcMode;

		ZoneID = _ZoneID;
		X = _X;
		Y = _Y;
		Z = _Z;

		memcpy(_iQuestLinkObj, iQuestLinkObj, sizeof(iQuestLinkObj));
		memcpy(iQuestID, _iQuestID, sizeof(_iQuestID));
		memcpy(szLuaScript, _szLuaScript, sizeof(_szLuaScript));
	}

};
//----------------------------------------------------------------------------------------
//ЄюҐ[ДЭ©КЄє°тҐ»ёк®Ж
//----------------------------------------------------------------------------------------
enum  MagicActionTypeENUM
{
	EM_MagicAction_Attack,
	EM_MagicAction_UnderAttack,
	EM_MagicAction_EQ,
	EM_MagicAction_Used,
};

enum    AttributeTypeENUM
{
	EM_AttributeType_AddPower,
	EM_AttributeType_Rune,
	EM_AttributeType_Title,
};

/*
enum AddonPosENUM
{
	EM_AddonPos_None = -1		,
	EM_AddonPos_Head    		,
	EM_AddonPos_Gloves			,
	EM_AddonPos_Shoes			,
	EM_AddonPos_Clothes			,
	EM_AddonPos_Pants			,
	EM_AddonPos_Back			,
	EM_AddonPos_Belt			,
	EM_AddonPos_Shoulder		,
	EM_AddonPos_Necklace		,
	EM_AddonPos_Ring    		,
	EM_AddonPos_Earring 		,
	EM_AddonPos_Shield			,
	EM_AddonPos_Weapon  		,
	EM_AddonPos_Max		  		,
};
/*
struct  AddonPosTypeStruct
{
	union
	{
		int _Type;
		struct
		{
			bool    Head        : 1;
			bool    Gloves	    : 1;
			bool    Shoes	    : 1;
			bool    Clothes	    : 1;
			bool    Pants	    : 1;
			bool    Back	    : 1;
			bool    Belt	    : 1;
			bool    Shoulder    : 1;
			bool    Necklace    : 1;
			bool    Ring        : 1;
			bool    Earring     : 1;
			bool	Shield		: 1;
			bool    Weapon      : 1;
		};
	};
};
*/

enum AddonPos_ArmorPosENUM
{
	EM_AddonPos_ArmorPos_None = -1,
	EM_AddonPos_ArmorPos_Head,
	EM_AddonPos_ArmorPos_Gloves,
	EM_AddonPos_ArmorPos_Shoes,
	EM_AddonPos_ArmorPos_Clothes,
	EM_AddonPos_ArmorPos_Pants,
	EM_AddonPos_ArmorPos_Belt,
	EM_AddonPos_ArmorPos_Shoulder,
	EM_AddonPos_ArmorPos_Max,
};

enum AddonPos_ArmorTypeENUM
{
	EM_AddonPos_ArmorType_None = -1,
	EM_AddonPos_ArmorType_HeavyArmed,
	EM_AddonPos_ArmorType_LightArmed,
	EM_AddonPos_ArmorType_Leather,
	EM_AddonPos_ArmorType_Clothes,
	EM_AddonPos_ArmorType_Robe,
	EM_AddonPos_ArmorType_Max,
};

enum AddonPos_WeaponLimitPosENUM
{
	EM_AddonPos_WeaponLimitPos_None,
	EM_AddonPos_WeaponLimitPos_MainHand,
	EM_AddonPos_WeaponLimitPos_OffHand,

};

enum AddonPos_WeaponTypeENUM
{
	EM_AddonPos_WeaponType_None = -1,
	EM_AddonPos_WeaponType_Unarmed,
	EM_AddonPos_WeaponType_Blade,
	EM_AddonPos_WeaponType_Dagger,
	EM_AddonPos_WeaponType_Wand,
	EM_AddonPos_WeaponType_Axe,
	EM_AddonPos_WeaponType_Bludgeon,
	EM_AddonPos_WeaponType_Claymore,
	EM_AddonPos_WeaponType_Staff,
	EM_AddonPos_WeaponType_Axe_2H,
	EM_AddonPos_WeaponType_Hammer_2H,
	EM_AddonPos_WeaponType_Polearm,
	EM_AddonPos_WeaponType_Bow,
	EM_AddonPos_WeaponType_CossBow,
	EM_AddonPos_WeaponType_Gun,
	EM_AddonPos_WeaponType_Max,
};

enum AddonPos_OtherENUM
{
	EM_AddonPos_Other_None = -1,
	EM_AddonPos_Other_Back,
	EM_AddonPos_Other_Necklace,
	EM_AddonPos_Other_Ring,
	EM_AddonPos_Other_Earring,
	EM_AddonPos_Other_Shield,
	EM_AddonPos_Other_Implement,
	EM_AddonPos_Other_Max,
};


struct AddonPosTypeStruct
{
	union
	{
		int _ArmorPos;
		struct
		{
			bool	Head : 1;
			bool    Gloves : 1;
			bool    Shoes : 1;
			bool    Clothes : 1;
			bool    Pants : 1;
			bool    Belt : 1;
			bool    Shoulder : 1;
		}ArmorPos;
	};

	union
	{
		int _ArmorType;
		struct
		{
			bool	HeavyArmed : 1;
			bool    LightArmed : 1;
			bool    Leather : 1;
			bool    Clothes : 1;
			bool    Robe : 1;
		}ArmorType;
	};

	union
	{
		int	_WeaponType;
		struct
		{
			bool	Unarmed : 1;
			bool    Blade : 1;
			bool    Dagger : 1;
			bool    Wand : 1;
			bool    Axe : 1;
			bool	Bludgeon : 1;
			bool    Claymore : 1;
			bool    Staff : 1;
			bool    Axe_2H : 1;
			bool    Hammer_2H : 1;
			bool	Polearm : 1;
			bool    Bow : 1;
			bool    CossBow : 1;
			bool    Gun : 1;
		}WeaponType;
	};

	AddonPos_WeaponLimitPosENUM  WeaponPosEnum;	// 0 ¤Ј­­Ёо 1 ҐD¤в 2Ў@Єю¤в
	int			Level;							// Є««~µҐЇЕ
	union
	{
		int _Other;
		struct
		{
			bool	Back : 1;
			bool	Necklace : 1;
			bool	Ring : 1;
			bool	Earring : 1;
			bool	Shield : 1;
			bool	Implement : 1;
			bool	Ornament : 1;
		}Other;
	};

	void Init()
	{
		memset(this, 0, sizeof(*this));
		//		_WeaponType = -1;
		//		_ArmorType	= -1;
		//		_ArmorPos	= -1;
		//		_Other		= -1;
	}


};



struct	GameObjectAttributeStruct
{
	AttributeTypeENUM   Type;   //ЄюҐ[Їа¤OГю«¬

//	int		InheritRate;		//Д~©УѕчІv(ёU¤А¤§¤@)
//  int		InheritLV;			//Д~©УµҐЇЕ
//  int     InheritLV_Range;    //Ѕdіт

	int     Inc_Magic;          //¤є§tЄєЄkіN
	int     Inc_Magic_ActionRate;//ЄkіNµoҐНѕчІv( §рА» іQ§рА» )

	MagicActionTypeENUM     Inc_MagicType;
	AddonPosTypeStruct      AddPos;             //ҐiёЛіЖіЎ¦м

	GameObjectWearEqStruct	Ability;
	//--------------------------------------------------------------------------------------
	//Їа¤OјР·З«¬
	bool					IsStandardAbility;
	int						StandardAbility_Lv;
	int						StandardAbility_ArgType[5];
	float					StandardAbility_Arg[5];
	//--------------------------------------------------------------------------------------
	int						StarValue;		//¬PµҐ­И
	bool					IsBirthAbility;
	int						InherentValue;	//Д~©УµҐЇЕ -1 ¤ЈД~©У
	int						Rarity;			//µ}¦і«Ч(0:µL 1:єс 2:¶А 3:ѕпЎK)
};
//----------------------------------------------------------------------------------------
// єлЅmЇа¤OЄн
//----------------------------------------------------------------------------------------
enum EqRefineAttributeTypeENUM
{
	EM_EqRefineAttributeType_Weapon,	//ЄZѕ№
	EM_EqRefineAttributeType_Staff,	//Єk§ъГю
	EM_EqRefineAttributeType_Bow,	//»·¶ZВчЄZѕ№
	EM_EqRefineAttributeType_Head,   //АY
	EM_EqRefineAttributeType_Gloves,   //¤в®M
	EM_EqRefineAttributeType_Shoes,   //ѕc¤l
	EM_EqRefineAttributeType_Clothes,   //¤W¦з
	EM_EqRefineAttributeType_Pants,   //їЗ¤l
	EM_EqRefineAttributeType_Back,   //­IіЎq
	EM_EqRefineAttributeType_Belt,   //ёy±a
	EM_EqRefineAttributeType_Shoulder,   //ЄУҐТ
	EM_EqRefineAttributeType_Necklace,   //¶µБе 
	EM_EqRefineAttributeType_Ring,   //§Щ¤l
	EM_EqRefineAttributeType_Earring,   //¦Х№ў
	EM_EqRefineAttributeType_SecondHand,   //°Ж¤в
};

struct	GameObjectEqRefineAttributeStruct
{
	EqRefineAttributeTypeENUM	Type;
	int							Level;
	GameObjectWearEqStruct		Ability;
	int							ExEqPowerRate;	//ГBҐ~јWҐ[ёЛіЖ°тҐ»Їа¤O¦К¤А¤с(DMG MDMG DEF MDEF )
};
//----------------------------------------------------------------------------------------
//ДqІЈі]©w
//----------------------------------------------------------------------------------------
enum GameObjectMineType
{
	EM_GameObjectMineType_Mine,
	EM_GameObjectMineType_Wood,
	EM_GameObjectMineType_Herb,
	EM_GameObjectMineType_Fish,
	EM_GameObjectMineTypeENUM_FORCE_DWORD = 0x7fffffff,
};

enum GameObjectCreateType
{
	EM_GameObjectCreateType_Always,           //ҐГ»·¦s¦b
	EM_GameObjectCreateType_EmptyDel,           //Ёъ§№§R°Ј
	EM_GameObjectCreateTypeENUM_FORCE_DWORD = 0x7fffffff,
};

struct	GameObjectMineStruct
{
	GameObjectMineType      Type;
	float                   BaseCount;                      // Єм©l¶q
	GameObjectCreateType    CreateType;                     // ¬O§_®шҐў
	int                     LvLimit;                        // µҐЇЕ­­Ёо

	int					    GetItem[7];                     // Ёъ±oЄ««~ ..(©ЗЄ««h¬OІЈҐН)
	int				        GetItemRate[7];                 // ІЈҐНІv
	int			            GetItemCount[7];                // јЖ¶q
	int						iFlagID;						// ­«ҐН°С¦ТєX¤l, ( µL§@ҐО )


	int						iReduceVal;						// ґо¤ЦЄм©l¶qЄє°ѕІѕ­И
	int						iGenerateObj[5];				// ­«ҐН«б, ІЈҐНЄєЄ«Ґу
	int						iGenerateObjRate[5];			// ­«ҐН«б, ІЈҐНЄєЄ«ҐуЄєѕчІv

	char					szLuaGetItemScript[_MAX_SIZE64_QUEST_SCRIPT];						// Єм©l¤Жј@±Ў©О№пёЬ®Й©Тёь¤JЄє LUA
};
//----------------------------------------------------------------------------------------
//­«­nЄ««~
//----------------------------------------------------------------------------------------
enum	KeyItemTypeENUM
{
	EM_KeyItemType_Invisible,
	EM_KeyItemType_Normal,	//¤@Їл
	EM_KeyItemType_Key,	//Ж_°Н
	EM_KeyItemType_Map,	//¦a№П
	EM_KeyItemType_SkillBook,	//§ЮЇа®С
	EM_KeyItemType_Recipe,	//°t¤иЄн
	EM_KeyItemType_LifeSkillUnLock,	//і]©wҐН
	EM_KeyItemType_DailyFlag,	//ЁC¤й­«Ёо­«­nЄ««~
};
//----------------------------------------------------------------------------------------
struct	GameObjectKeyItemStruct
{
	KeyItemTypeENUM     Type;
	int					QuestID;

	SkillValueTypeENUM	LifeSkillType;
	int					LifeSkillLearnLV;
	int					LifeSkillMaxLV;
};
//----------------------------------------------------------------------------------------
//°t¤иЄн
//----------------------------------------------------------------------------------------
#define _MAX_RECIPE_SOURCE_COUNT_	8
struct RecipeItemStruct
{
	int     OrgObjID;   //јЛЄOЄ«Ґу
	int     Count;      //јЖ¶q
	int     Rate;       //ІЈҐНІv(¦К¤А¤§¤@)
};
struct GameObjectRecipeStruct
{

	RecipeItemStruct Source[_MAX_RECIPE_SOURCE_COUNT_];                 //­м®Ж

	union
	{
		RecipeItemStruct ItemAll[9];              //ІЈҐНЄ««~¤@

		struct
		{
			union
			{
				RecipeItemStruct Item1[6];              //ІЈҐНЄ««~¤@
				struct
				{
					RecipeItemStruct Item1_Normal;
					RecipeItemStruct Item1_HQ;
					RecipeItemStruct Item1_Slot1;
					RecipeItemStruct Item1_Slot2;
					RecipeItemStruct Item1_Slot3;
					RecipeItemStruct Item1_Slot4;

				};
			};

			union
			{
				RecipeItemStruct ItemOther[3];                  //ІЈҐНЄ««~2-4
				struct
				{
					RecipeItemStruct Item2;                     //ІЈҐНЄ««~2
					RecipeItemStruct Item3;                     //ІЈҐНЄ««~3
					RecipeItemStruct Item4;                     //ІЈҐНЄ««~4
				};

			};
		};

	};

	//»ЭЁD
	struct
	{
		SkillValueTypeENUM		Skill;				//§ЮЇаєШГю
		int						SkillLV;			//§ЮЇаµҐЇЕ
		int						KeyItemID;			//­«­nЄ««~
	}Request;

	int	Coldown;	//§N«o®Й¶Ў(¬н)

	int	Type;		//Ґш№єі]©wЄєГю«¬
	int	RequestCraftObjType;

	float						ExpRate;			//¦X¦Ё©Т±oexp­їІv

	int	LearnFee;									// ѕЗІЯ°t¤и®Й, ©Т»Э¶OҐО
	int CraftFee;									// ЁоіyЄ««~®Й, ©Т»Э¶OҐО

};
//----------------------------------------------------------------------------------------
//АY»Оёк®Ж
//----------------------------------------------------------------------------------------
enum TitleClassifyENUM
{
	EM_TitleClassify_Quest,
	EM_TitleClassify_Challenge,
	EM_TitleClassify_Item,
	EM_TitleClassify_Sys,
	EM_TitleClassify_Activitie,
};
enum TitleNameTypeENUM
{
	EM_TitleNameType_Famous,
	EM_TitleNameType_Job,
	EM_TitleNameType_Plot,
	EM_TitleNameType_KillCount,

};
enum KillCountTypeENUM
{
	EM_KillCountType_KillCount,
	EM_KillCountType_DeadCount,
};
struct  GameObjectTitleStruct
{
	TitleNameTypeENUM     Type;                   //Ґш№є¤АГюҐО(јИ®ЙµLҐО)
	//=----------------------------------
	//»ЭЁD
	//=----------------------------------
	struct
	{
		int			Famous;
		Voc_ENUM    MainJob;
		int			MainJobLV;
		Voc_ENUM    SubJob;
		int			SubJobLV;
	};

	struct
	{
		KillCountTypeENUM		Type;			// 0 ±ю¦є©ЗЄ« 1 іQ©ЗЄ«±ю
		int						KillGroupID;	//
		int						Count;			//јЖ¶q

	}KillCount;
	//=----------------------------------
	GameObjectWearEqStruct	Ability;

	//ҐiЁПҐО§ЮЇа
	int		SkillID;							//ҐiЁПҐО§ЮЇа
	int		SkillLv;							//§ЮЇаµҐЇЕ

	int		Classify[2];

	char	LuaEvent[_MAX_SIZE64_QUEST_SCRIPT];

	GameObjectWearEqStruct	AbilityEx;			//·sАY»ОЁtІОјWҐ[Їа¤O
	char* TitleSysNote;						//·sАY»ОЁtІО µщёС

	char	TitleSysLuaEvent[_MAX_SIZE64_QUEST_SCRIPT];
};
//----------------------------------------------------------------------------------------
//№П«¬і]©w
//----------------------------------------------------------------------------------------
enum GameObjectImageTypENUM
{
	EM_GameObjectImageType_Normal,
	EM_GameObjectImageType_Magic,
	EM_GameObjectImageType_Armor,
	EM_GameObjectImageType_Weapon,
	EM_GameObjectImageType_Flag,
	EM_GameObjectImageType_Wing,

	EM_GameObjectImageTypeENUM_FORCE_DWORD = 0x7fffffff,
};

enum ArgumentPointENUM
{
	EM_Argument_Null = 0,
	EM_Argument_Hit_Point,
	EM_Argument_Back_Staff,
	EM_Argument_Back_Shield,
	EM_Argument_Back_Sworld,
	EM_Argument_Name_Title,
	EM_Argument_Hand_Shield,
	EM_Argument_Left_Hand,
	EM_Argument_Right_Hand,
	EM_Argument_Left_Foot,
	EM_Argument_Right_Foot,
	EM_Argument_Left_Weapon,
	EM_Argument_Right_Weapon,
};

/*
enum MotionENUM
{
	EM_Motion_Null			= 0	,
	EM_Motion_Casting_Delay		,				// "casting_delay" ¬IЄkµҐ«Э
	EM_Motion_Cast_01			,				// "cast01" ҐX¤в№п¦ЫЁ­
	EM_Motion_Cast_02			,				// "cast02" ҐX¤в№пҐШјР
};
*/
//----------------------------------------------------------------------------------------
enum ObjectImageTypeENUM
{
	EM_ObjectImage_2Foots,  //¤HГю
	EM_ObjectImage_4Foots,  //Є¦ВОГю
	EM_ObjectImage_4Foots2,  //°ЁГю
};
//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
struct MagicPartical
{
	char				Point[_MAX_FILENAME_SIZE_];	// ¦ТВI¦мёm
	char				Name[_MAX_FILENAME_SIZE_];	// ЇS®Д¦WєЩ
	bool				IsSoundEnabled;				// ҐН­µ¶}Гц
};
//----------------------------------------------------------------------------------------
struct ColorStruct
{
	union
	{
		unsigned int Color;
		struct
		{
			unsigned char B;
			unsigned char G;
			unsigned char R;
			unsigned char A;
		};
	};
};
//----------------------------------------------------------------------------------------
//ЇИ«Ѕ«Ѕёк®Ж
struct	PaperdollStruct
{
	char			Name[64];
	ColorStruct		MainColor;
	ColorStruct		OffColor;
};
//----------------------------------------------------------------------------------------
enum GameObjectImageArmorPosENUM
{
	EM_GameObjectImageArmorPos_Clothes,	//¤W¦з
	EM_GameObjectImageArmorPos_Gloves,	//¤в®M
	EM_GameObjectImageArmorPos_Shoulder,//ЄУҐТ
	EM_GameObjectImageArmorPos_Back,	//­IіЎ
	EM_GameObjectImageArmorPos_Belt,	//ёy±a
	EM_GameObjectImageArmorPos_Shoes,	//ѕc¤l
	EM_GameObjectImageArmorPos_Pants,	//їЗ¤l
	EM_GameObjectImageArmorPos_Head,	//АY
	EM_GameObjectImageArmorPos_Hair,	//АYѕv
	EM_GameObjectImageArmorPos_Face,	//Бy
};
/*
enum GameObjectImageArmorPosENUM
{
EM_GameObjectImageArmorPos_Hair,	//АYѕv
EM_GameObjectImageArmorPos_Face,	//Бy
EM_GameObjectImageArmorPos_Head,	//АY
EM_GameObjectImageArmorPos_Gloves,	//¤в®M
EM_GameObjectImageArmorPos_Shoes,	//ѕc¤l
EM_GameObjectImageArmorPos_Clothes,	//¤W¦з
EM_GameObjectImageArmorPos_Pants,	//їЗ¤l
EM_GameObjectImageArmorPos_Back,	//­IіЎ
EM_GameObjectImageArmorPos_Belt,	//ёy±a
EM_GameObjectImageArmorPos_Shoulder,//ЄУҐТ

};
*/

enum GameObjectImageIconTypeENUM
{
	EM_GameObjectImageIconType_Money,		// Єчїъ
	EM_GameObjectImageIconType_RuneMoney,	// ІЕ¤е№ф
	EM_GameObjectImageIconType_Bonus,		// ¬х§Q№ф
};

//----------------------------------------------------------------------------------------
enum GameObjectMagicShowTypeENUM
{
	EM_GameObjectMagicShowType_Single,				// ­ё¦жЇS®ДҐu№піж¤@ҐШјР
	EM_GameObjectMagicShowType_Multi,				// ­ё¦жЇS®Д№п¦h­УҐШјР
	EM_GameObjectMagicShowType_PolyLine,			// ¦кіsЇS®Д№п¦h­УҐШјР
	EM_GameObjectMagicShowType_Parabola,			// ©ЯЄ«Ѕu(іж¤@ҐШјР§лВY)
	EM_GameObjectMagicShowType_ParabolaMulti,		// ©ЯЄ«Ѕu(¦h№п¦VҐШјР§лВY)

	EM_GameObjectMagicShowType_FORCEINT = 0x7FFFFFFF,
};

enum GameObjectCursorAnchorENUM
{
	EM_GameObjectCursorAnchor_None,	//ЁS¦і
	EM_GameObjectCursorAnchor_Cursor,	//ґејР
	EM_GameObjectCursorAnchor_RightBottom,	//Ґk¤U
};

// ГM­ј®ЙЁ¤¦в°К§@
enum GameObjectMountRideMotion
{
	EM_GameObjectMountRideMotion_Ride,		// ­м©lГM­ј°К§@(Вщё}±i¶})
	EM_GameObjectMountRideMotion_Ride1,		// ­м©lГM­ј°К§@(Вщё}§Ёєт)
	EM_GameObjectMountRideMotion_Stand,		// ЇёҐЯ¦bІѕ°КЄ«Ґу
	EM_GameObjectMountRideMotion_Sit,		// §¤¤U­ёґаГю«¬	
	EM_GameObjectMountRideMotion_Ride2,		// ®yґИ§¤«єГю«¬
	EM_GameObjectMountRideMotion_FORCEINT = 0x7FFFFFFF,
};

enum ImageActTypeENUM
{
	EM_ImageActType_Gather,	// ¶°®р
	EM_ImageActType_Shoot,	// ҐX¤в
	EM_ImageActType_Fly,	// ­ё¦ж
	EM_ImageActType_Explode,	// ©R¤¤
	EM_ImageActType_Extend,	// ©µ¦щ№п¶H
	EM_ImageActType_BuffExtend,	// Buff©µ¦щ№п¶H
	EM_ImageActType_Max,
};

//----------------------------------------------------------------------------------------
struct GameObjectImageStruct
{
	GameObjectImageTypENUM          ImageType;                      //ЕгҐЬјТ¦Ў

	char							ACTField[_MAX_FILENAME_SIZE_];	//Є««~Дж¦мЄє ACT 2D
	char							ACTWorld[_MAX_FILENAME_SIZE_];	//Ег№іACT
	int 							ModelSize;			            //©с¤j­їІv
	int                             BoundRadiusX;                   //ёIјІҐb®|
	int                             BoundRadiusY;                   //ёIјІҐb®|
	//-----------------------------------------------------------------------------------------
	//ёЛіЖёк®Ж
	//-----------------------------------------------------------------------------------------
	GameObjectImageArmorPosENUM     ArmorPos;
	ColorStruct						ArmorMainColor;
	ColorStruct						ArmorOffColor;
	//-----------------------------------------------------------------------------------------
	//ЄkіN№П«¬ёк°T
	//-----------------------------------------------------------------------------------------
	// №п¬IЄkЄМ¦ЫЁ­
	char							GatherMotion[_MAX_FILENAME_SIZE_];		// ¶°®р°К§@
	MagicPartical					GatherPartical[3];						// ¶°®рЇS®Д

	// BeginACT №CАё¤єЁS¦іЁПҐОЁм
	char							BeginACT[_MAX_FILENAME_SIZE_];			// ҐX¤вЇS®ДACT

	// 
#pragma pack(1)
	union
	{
		char						BeginPoint[_MAX_OBJ_NAME_STR_SIZE_];	// №п¶H°С¦ТВI¦мёm
		struct
		{
			GameObjectMountRideMotion RideMotion;
		};
	};
#pragma pack()
	// ҐX¤в	
	char							ShootMotion[_MAX_FILENAME_SIZE_];		// ҐX¤в°К§@
	int 							ShootMotionScale;						// %
	char							ShootACT[_MAX_FILENAME_SIZE_];			// ҐX¤вЇS®ДACT
	char							ShootPoint[_MAX_OBJ_NAME_STR_SIZE_];	// №п¶H°С¦ТВI¦мёm

	// ­ё¦ж
	char							FlyBeginPoint[_MAX_OBJ_NAME_STR_SIZE_];	// «ь©w­ё¦жЇS®Д¶}©l©Т¦bЁ¤¦в°С¦ТВI¦мёm
	char							FlyACT[_MAX_FILENAME_SIZE_];			// ­ё¦жЇS®ДACT

	// ҐШЄє
	char							ExplodeACT[_MAX_FILENAME_SIZE_];		// ҐШЄєГz¬µ®ДЄGACT
	char							TargetPoint[_MAX_OBJ_NAME_STR_SIZE_];	// №п¶H°С¦ТВI¦мёm

	// ©µ¦щҐШЄєЇS®Д,ҐD­nҐОҐH·нҐШ¦aЇS®Дј·©с®Й,Б{ЄсЄ«ҐуЁьј·¤О©Т­n°µЄє®ДЄGЄнІ{
	// ¤с¦pҐШЄєІЈҐН¤jГz¬µЇS®Д,©µ¦щ«hІЈҐНїUїNЇS®Д
	char							ExtendACT[_MAX_FILENAME_SIZE_];			// ©µ¦щҐШЄєЇS®ДACT
	char							ExtenPoint[_MAX_OBJ_NAME_STR_SIZE_];	// ©µ¦щ№п¶H°С¦ТВI¦мёm

	// Buff
	char							BuffACT[_MAX_FILENAME_SIZE_];			// Buff®ДЄGACT	
	char							BuffPoint[_MAX_OBJ_NAME_STR_SIZE_];		// Buff®ДЄG№п¶H°С¦ТВI¦мёm
	//-----------------------------------------------------------------------------------------
	//Є«ҐуГю«¬
	//-----------------------------------------------------------------------------------------
	ObjectImageTypeENUM             ObjectType;                             //¬OЁвё}БЩ¬OҐ|Ё¤°КЄ«
	//-----------------------------------------------------------------------------------------
	char							Sound[_MAX_FILENAME_SIZE_];				//Бn­µ

	int								ActRate[EM_ImageActType_Max];			//Act©с¤jБY¤p¤сЁТ

	//NPCёЛіЖёк®Ж
	union
	{
		PaperdollStruct Paperdoll[10];
		struct
		{
			PaperdollStruct		PaperdollHair;
			PaperdollStruct		PaperdollFace;
			PaperdollStruct		PaperdollHead;
			PaperdollStruct		PaperdollGloves;
			PaperdollStruct		PaperdollShoes;
			PaperdollStruct		PaperdollClothes;
			PaperdollStruct		PaperdollPants;
			PaperdollStruct		PaperdollBack;
			PaperdollStruct		PaperdollBelt;
			PaperdollStruct		PaperdollShouder;
		};
	};
	//-----------------------------------------------------------------------------------------
	union
	{
		ColorStruct ColorArr[2];
		struct
		{
			ColorStruct SkinColor;
			ColorStruct HairColor;
		};
	};
	union
	{
		int			BoneScale[5];			//°©¬[БY©с
		struct
		{
			int	BoneScaleChest;				//ЇЭ
			int	BoneScaleButtock;			//Бv
			int	BoneScaleHand;				//¤в
			int	BoneScaleLeg;				//Ё¤
			int	BoneScaleBra;				//ЇЭёn
		};
	};
	int			Alpha;						//Ґbіz©ъ

	GameObjectCursorAnchorENUM	TipType;	//µщёС¤и¦Ў
	bool		IsShowShadow;
	bool		IsThrowType;				//¬O§_ҐX¤в°К§@­nҐОІХ¦XЄє¦r¦к
	int			ThrowTypePreviewID;

	struct
	{
		char	Type[16];
		char	Insignia[16];
		char	Banner[16];
		int		InsigniaColor[2];
		int		BannerColor[2];
	}Flag;

	SexFlagStruct					LimitSex;
	bool							IsPolyLineEffect;		//¬O§_¬°¦кіsЇS®Д
	GameObjectMagicShowTypeENUM		MagicShowType;			//ЄkіNЕгҐЬ¤и¦Ў
	bool							IsHideWeapon;			// БфВГЄZѕ№
	GameObjectImageIconTypeENUM		SmallIconType;			//Єчїъ¤p№ПҐЬ

	char							BuffACTEx[_MAX_FILENAME_SIZE_];			// Buff®ДЄGACT
	char							PhantomImg[_MAX_FILENAME_SIZE_];		//¤ЫЖF¤jАY№П


};


//----------------------------------------------------------------------------------------
//ЁПҐО­­ЁоµІєc
//----------------------------------------------------------------------------------------
struct LimitStruct
{
	VocationFlagStruct  Voc;
	RaceFlagStruct      Race;
	SexFlagStruct       Sex;
	int     Level;
	int     STR;
	int     STA;
	int     INT;
	int     MND;
	int     AGI;
};
//----------------------------------------------------------------------------------------
//Ґш№єЄ««~іWГю
//----------------------------------------------------------------------------------------
enum ItemType_ENUM
{
	EM_ItemType_Normal, //¤@Їл
	EM_ItemType_Plot, //ј@±ЎЄ««~
	EM_ItemType_Food, //­№Є«
	EM_ItemType_SweetFood, //Іў­№
	EM_ItemType_Water, //ГД¤ф
	EM_ItemType_Ore, //ДqҐЫ
	EM_ItemType_Wood, //¤м§ч
	EM_ItemType_Herb, //ГДЇу
	EM_ItemType_Hunt, //ВyЄ«
	EM_ItemType_Seed, //єШ¤l
	EM_ItemType_Flowerpot, //Єб¬Ц
	EM_ItemType_Crop, //§@Є«
	EM_ItemType_Fish, //іЅ
	EM_ItemType_Tool, //±Д¶°¤uЁг
	EM_ItemType_Stuff, //»sіy§ч®Ж
	EM_ItemType_Recipe_BlackSmith, //°t¤и ҐґЕK
	EM_ItemType_Recipe_Carpenter, //°t¤и ¤м¤u
	EM_ItemType_Recipe_Armor, //°t¤и »sҐТ
	EM_ItemType_Recipe_Tailor, //°t¤и µфБ_
	EM_ItemType_Recipe_Alchemy, //°t¤и ·ТЄч
	EM_ItemType_Recipe_Cook, //°t¤и Іi¶№
	EM_ItemType_Jewel, //ЅДµҐД_ҐЫ
	EM_ItemType_PowerLight, //®ДЄGҐъІy
	EM_ItemType_Money, //Єчїъ
	EM_ItemType_Rune, //ІЕ¤е
	EM_ItemType_PosRecord, //¦мёm°Oїэ
	EM_ItemType_GuildContribution, //¤Ѕ·|°^Дm«~
	EM_ItemType_Lottery, //јЦіz±mЁч
	EM_ItemType_Furniture, //®a­С
	EM_ItemType_SpecialRune, //ЇS®нІЕ¤е
	EM_ItemType_SmeltStone, //єІёСҐЫ
	EM_ItemType_Relation, //Гц«YЄ««~
	EM_ItemType_SummonHorse, //©Ыґ«®yГM
	EM_ItemType_Contract, //©Р«О«ґ¬щ
	EM_ItemType_Study, //­Ч·Т¦К¬м
	EM_ItemType_MagicStone, //Е]ҐЫ
	EM_ItemType_Collection, //©e°U¦¬¶°
	EM_ItemType_Packet, //Ґ]»qЄ««~
	EM_ItemType_Pet, //ГdЄ«Є««~
	EM_ItemType_Pet_Tools, //ГdЄ«¤uЁг
	EM_ItemType_Pet_Furniture, //ГdЄ«іГ­С
	EM_ItemType_Pet_Food, //ГdЄ«­№Є«
	EM_ItemType_Packet_DropList, //ҐґҐ]Є«(±јёЁЄн©Т¦іЄF¦и)
	EM_ItemType_Egg, //°У«°ВаіJ
	EM_ItemType_ViewPet, //Ж[ЅаҐОГdЄ«
	EM_ItemType_TitleItem, //АY»ОЁtІОЄ««~
};
enum RareType_ENUM
{
	EM_RareType_Normal,
	EM_RareType_Good,
	EM_RareType_Magic,
	EM_RareType_Legend,
	EM_RareType_FORCE_DWORD = 0x7fffffff,
};

enum RuneType_ENUM
{
	EM_RuneType_Manufacture,//ҐНІЈ
	EM_RuneType_Weapon,//ЄZѕ№
	EM_RuneType_Armor,//ЁѕЁг
	EM_RuneType_Head,//АYіЎ
	EM_RuneType_Body,//¤WЁ­
	EM_RuneType_Pant,//¤UЁ­
	EM_RuneType_Special,//ЇS®н
	EM_RuneType_Ornament,//№ў«~
	EM_RuneType_Shield,//¬Ю
	EM_RuneType_MagicTools,//Єkѕ№
	EM_RuneType_NewSystem,//АJ¤еЁtІО
};

enum ItemUseType_ENUM
{
	EM_ItemUseType_None,
	EM_ItemUseType_Magic,
	EM_ItemUseType_SrvScript,
	EM_ItemUseType_CliScript,
	EM_ItemUseType_EqRefine,	//ЅДµҐ
	EM_ItemUseType_FixEq,	//­ЧёЛ
	EM_ItemUseType_Dissolution,	//©оёСЄ««~
	EM_ItemUseType_Coloring,	//¬V¦в
	//EM_ItemUseType_ClsEqSoulbound	,	//ІM°ЈёЛіЖёj©w
	EM_ItemUseType_ClsTrade,	//ІM°ЈёЛіЖёj©w
	EM_ItemUseType_ResetAbility,	//­«і]°тҐ»јЖ­И
	EM_ItemUseType_ResetSkillPoint,	//­«і]§ЮЇаВIјЖ
	EM_ItemUseType_EQDigHole,	//ёЛіЖ«х¬}
	EM_ItemUseType_EQClearRune,	//ІM°ЈёЛіЖІЕ¤е
	EM_ItemUseType_ItemLock,	//ёЛіЖВк
	EM_ItemUseType_ItemUnLock,	//ёС°ЈёЛіЖВк
	EM_ItemUseType_EQProtect,	//ёЛіЖ«OЕ@
	EM_ItemUseType_EQLimitMax,	//ёЛіЖ­@¤[¤W­­ЁмMax
	EM_ItemUseType_IgnoreLimitLv,	//©їІ¤µҐЇЕ­­Ёо
	EM_ItemUseType_PK_EQProtect,	//PKёЛіЖ«OЕ@
	EM_ItemUseType_PK_EQProtect_Clear,	//PKёЛіЖ«OЕ@ІM°Ј
	EM_ItemUseType_UnPacket,	//Ґґ¶}Ґ]»qЄ««~
	EM_ItemUseType_UnPacket_DropList,	//ҐґҐ]Є«ёС¶}(±јёЁЄн©Т¦іЄF¦и)
	EM_ItemUseType_Egg,	//°У«°ВаіJ
	EM_ItemUseType_ClsEqSoulbound,	//ІM°ЈёЛіЖёj©w
	EM_ItemUseType_ClsEqAbility,	//ІM°Ј©Т¦іёЛіЖДЭ©К
//	EM_ItemUseType_ClsTrade_All		,	//ІM°Ј©Т¦іЄ««~ЄєҐж©цВк©w
EM_ItemUseType_ItemtoItemLua,	//Є««~№пЄ««~ДІµolua
};

enum EqRefineType_ENUM
{
	EM_EqRefineType_Weapon,
	EM_EqRefineType_Armor,
	EM_EqRefineType_Jewels,
	EM_EqRefineType_Ornament,
};

//Є««~ҐН¦s®Й¶ЎГю«¬
enum ItemLiveTimeTypeENUM
{
	EM_ItemLiveTimeType_None,	//¤Ј·|®шҐў
	EM_ItemLiveTimeType_RealTime,	//Їu№к®Й¶Ў
};

//Є««~ЇS®Д¤АГю
enum ImageEfficacyTypeENUM
{
	EM_ImageEfficacyType_None,	//µL
	EM_ImageEfficacyType_Throw_Sword,	//§лВY ­ёјC,­ё¤M
	EM_ImageEfficacyType_Throw_Axe,	//§лВY ­ё©т
	EM_ImageEfficacyType_Throw_Stone,	//§лВY ҐЫАY
	EM_ImageEfficacyType_Throw_Arrow,	//§лВY ЅbҐЪ
	EM_ImageEfficacyType_Throw_Ammo,	//§лВY ¤lјu
};


//іГ­С
enum					EnumItemFurnitureType
{
	EM_FurnitureType_Normal = 0,	// ¤@Їл, ¤ЈЁг®e¶q
	EM_FurnitureType_Storage = 1,	// ЁгАx¦sЄЕ¶ЎЄєіГ­С
	EM_FurnitureType_Coathanger = 2,	// ¦з¬[, ҐBҐiҐH®iҐЬ¦зЄ«, ёЛіЖ, ЁгАx¦sЄЕ¶ЎЄєіГ­С
	EM_FurnitureType_WeaponSet = 3,	// ЄZѕ№¬[, ҐiҐH±ѕёь, іжВщ¤вЄZѕ№©О¬Ю, ЁгАx¦sЄЕ¶ЎЄєіГ­С
	EM_FurnitureType_WeaponAndCloth = 4,	// ЄZѕ№¦з¬[, ҐiҐH±ѕёь, ЄZѕ№»PёЛіЖ
	EM_FurnitureType_Maid = 5,	// ¤k№І
};

enum					EnumItemFurnitureEffect
{
	EM_FurnitureEffect_None = 0,	// ¤ЈЁгґЈ¤Й®ДЄG
	EM_FurnitureEffect_EXP,		// ґЈ°Є EXP BOUNS
	EM_FurnitureEffect_TP,		// ґЈ°Є TP BOUNS
	EM_FurnitureEffect_HQ,		// ґЈ°Є¦UєШЁоіy§ЮЇа°µҐX HQ Є««~Єєѕч·|
};

enum					EnumItemFurniturePos
{
	EM_FurniturePos_NoRule = 0,	// ¤Ј­­©w
	EM_FurniturePos_Ground,		// ҐuЇа©с¦b¦aЄO¤W
	EM_FurniturePos_Wall,		// ҐuЇа©с¦bАр¤W
};

enum	DigHoleTypeENUM
{
	EM_DigHoleType_All,
	EM_DigHoleType_Weapon,
	EM_DigHoleType_Armor,
	EM_DigHoleType_Jewels,
	EM_DigHoleType_Ornament,
};

/*
enum AddPowerTypeENUM
{
	EM_AddPowerType_No			,		//¤ЈҐ[
	EM_AddPowerType_Standard	,		//јР·З
};
*/
enum	GuildResourceTypeENUM
{
	EM_GuildResourceType_None = -1,
	EM_GuildResourceType_Gold,
	EM_GuildResourceType_BonusGold,
	EM_GuildResourceType_Mine,
	EM_GuildResourceType_Wood,
	EM_GuildResourceType_Herb,
	EM_GuildResourceType_GuildRune,
	EM_GuildResourceType_GuildStone,
};

enum SeedTypeENUM
{
	EM_SeedType_None,	//¤Ј¬OєШ¤l
	EM_SeedType_Tree,	//¤м
	EM_SeedType_Grass,	//Їу
	EM_SeedType_Mine,	//Дq
};

enum ManureTypeENUM
{
	EM_ManureType_None,	//¤Ј¬OЄО®Ж
	EM_ManureType_Water,	//Дйµ@
	EM_ManureType_Topdress,	//¬IЄО
	EM_ManureType_MagicWater,	//Е]ЄkДйµ@
};

enum FurnitureTypeENUM
{
	EM_FurnitureType_All,
	EM_FurnitureType_GuildOnly,
	EM_FurnitureType_HouseOnly,
};

enum PetFoodEffectTypeENUM
{
	EM_PetFoodEffectType_None,	//µL
	EM_PetFoodEffectType_STR,	//STR
	EM_PetFoodEffectType_STA,	//STA
	EM_PetFoodEffectType_INT,	//INT
	EM_PetFoodEffectType_MND,	//MND
	EM_PetFoodEffectType_AGI,	//AGI
	EM_PetFoodEffectType_PetProperty,	//§пЕЬДЭ©К
	EM_PetFoodEffectType_PetObjID,	//§пЕЬҐ~Ж[

};

enum ExBagTypeENUM
{
	EM_ExBagType_None,
	EM_ExBagType_Pet,
	EM_ExBagType_Horse,
};
//----------------------------------------------------------------------------------------
//ДЭ©уЄ««~Єє°тҐ»ёк®Ж
//----------------------------------------------------------------------------------------
struct GameObjectItemStruct
{
	GameObjectItemENUM		Class;

	ItemType_ENUM           ItemType;

	int						IncMagic_Onuse;		    //ІЈҐНЄєЄkіN®ДЄG(ЁПҐОЄє®ДЄG)
	int                     IncMagic_OnAttack;
	int                     IncMagic_OnAttackRate;  //§рА»µoҐНЄєѕчІv

	int                     DisappearRate;      //¦X¦ЁҐў±С®шҐўІv
	float					DissolutionRate;	//¤АёСҐ[¦ЁІv
	int                     SrcRecipe;          //¦X¦Ё°t¤и

	union
	{
		GameObjectWeaponENUM	WeaponType;			//ЄZѕ№Гю«¬
		GameObjectArmorENUM     ArmorType;			//ЁѕЁгГю«¬
	};
	union
	{
		GameObjectWeaponPosENUM WeaponPos;
		GameObjectArmorPosENUM  ArmorPos;
	};

	GameObjectWearEqStruct	Ability;
	ItemDropStruct DropItemAbility[_MAX_DROP_ABILITY_COUNT_];

	int						AttackDistance;		//§рА»¶ZВч
	int                     HoleBase;           //°тҐ»¬}јЖ(ІЕ¤е®e¶q)
	int                     HoleRange;          //ҐiЕЬЅdіт(«OЇd)
//    AddPowerTypeENUM        AddPowerOn;         //Є««~±јёЁ¬O§_¶]¤@ЇлЄюҐ[ДЭ©КіW«h(orІЕ¤еЇа¤O) ( 0 ¤Ј¶] 1 ¶] 2 ҐІ©w¦і )
	int						MaxAttrCount;		//іМ¦hҐiЄюҐ[Їа¤O¦UјЖ


	AddonPosTypeStruct      AddPos;             //(ІЕ¤е)Ж^¤JёЛіЖіЎ¦м
	RuneType_ENUM           RuneType;           //ІЕ¤еГю«¬(Ґш№єҐО)
	int						RuneGroup;			//ІЕ¤е¤АГю
	int                     Durable;
	int						Level;				//ЁПҐОµҐЇЕ
	int						Volume;				//ІЕ¤е®e¶q
	int						ManuFactureType;	//»sіyГю«¬
	int						ManuFactureLV;		//»sіyµҐЇЕ

	int						SuitID;				//®MёЛID
	int						UltimateSuitID;

	int						AttackRange;		//§рА»Ѕdіт
	int						AttackSpeed;

	ItemUseType_ENUM		UseType;				//Є««~ЁПҐОјТ¦Ў
	char					SrvScript[128];			//ЁПҐО°х¦ж Server¬Yј@±Ў ҐІ¶·¦^¶З true=1 or false=0 ¤ЈЇаЁПҐОSleep
	char					CliScript[128];			//ЁПҐО°х¦ж Client¬Yј@±Ў
	char					CheckUseScript[128];	//ЁПҐОАЛ¬d lua
	//­ЧІzёк®Ж
	int						FixQuality;			//єы­Ч«~Ѕи

	//ЅДµҐёк®Ж
	EqRefineType_ENUM		EqRefineType;
	// TODO: New field
	int EqRefineTableId;
	float					EqRefineRate_Base;
	float					EqRefineRate_Fix;
	float					EqRefineRate_Per;

	int						EqRefineEqMinLv;		//ЁПҐОµҐЇЕ
	int						EqRefineEqMaxLv;
	int						EqRefineMinLv;			//Бе¤ЖµҐЇЕ
	int						EqRefineMaxLv;
	float					EqRefineFailed_LevelDownRate_Base;	//Ґў±С°hµҐѕчІv
	float					EqRefineFailed_LevelDownRate_Fix;	//Ґў±С°hµҐѕчІv
	float					EqRefineFailed_LevelDownRate_Per;	//Ґў±С°hµҐѕчІv
	float					EqRefineFailed_ExplodeRate_Base;	//Ґў±С®шҐўѕчІv
	float					EqRefineFailed_ExplodeRate_Fix;		//Ґў±С®шҐўѕчІv
	float					EqRefineFailed_ExplodeRate_Per;		//Ґў±С®шҐўѕчІv
	bool					IsItemToItem;			//Ґi№пЄ««~ЁПҐО

	bool					IsStandardCost;
	int						ReferenceLv;			//°С¦Т»щµҐЇЕ
	int						Coloring_OffColor;
	int						Coloring_MainColor;

	//Ґd¤щ±MҐОёк®Ж
	int						CardOrgObjID;			//Ґd¤щЇБ¤ЮЄєёк®Ж
	int						CardID;					//Ґd¤щЄєё№ЅX
	ImageEfficacyTypeENUM	ImageEfficacyType;		//ЇS®Д°СјЖ

	int						CastingTime;			//ЁПҐО®Й¶Ў
	//--------------------------------------------------------------------------------------
	//Їа¤OјР·З«¬
	bool					IsStandardAbility;
	int						StandardAbility_Lv;
	int						StandardAbility_ArgType[5];
	float					StandardAbility_Arg[5];
	//--------------------------------------------------------------------------------------
	int						CardPriorityID;			//Ґd¤щ±Ж§ЗАuҐэЕvЎ@
	int						GuildContributionValue;	//¤Ѕ·|°^Дm©ТҐ[Єєїn¤А
	//--------------------------------------------------------------------------------------
	int						RefineTableID;			//ЅДµҐ°С¦Тёк®Ж¦мёm
	// TODO: New field
	int RefineTableID1;
	//--------------------------------------------------------------------------------------
	char					MagicToolsScript[128];	//ЄkД_ЁC¤Q¬н©w®ЙЁПҐОЄєLua

	// іГ­С
	//--------------------------------------------------------------------------------------
	EnumItemFurnitureType		emFurnitureType;
	EnumItemFurnitureEffect		emFurnitureEffect;
	int							iFurnitureStorageSize;
	EnumItemFurniturePos		emFurniturePos;

	int							iCraftObjType;				// ЇаґЈЁСЁу§UҐНІЈ§ЮЇа
	int							iFurnitureSuitID;			// іГ­С®MёЛ®ДЄG

	int							ExpPoint;
	int							TpPoint;

	int 						BlackSmithHQPoint;
	int 						CarpenterHQPoint;
	int 						MakeArmorHQPoint;
	int 						TailorHQPoint;
	int 						CookHQPoint;
	int 						AlchemyHQPoint;
	BYTE						IDK0[16];
	//--------------------------------------------------------------------------------------
	//ёЛіЖҐґ¬}
	struct
	{
		int					MaxHoleCount;	//іМ¤j¬}јЖ
		DigHoleTypeENUM		Type;			//Гю«¬
		int					Rate;			//ѕчІv
	}DigHole;
	//--------------------------------------------------------------------------------------
	//ЁПҐО¶·­n2¦ё±KЅX
	bool	IsUseNeedPassword;
	bool	IsFixDurable;
	//--------------------------------------------------------------------------------------
	int		MagicToolsTime;
	int		MagicToolsColdown;
	//--------------------------------------------------------------------------------------
	int		AttrValue[8];
	//--------------------------------------------------------------------------------------
	RelationTypeENUM	SenderRelation;
	RelationTypeENUM	TargetRelation;
	//--------------------------------------------------------------------------------------
	int		StarValue;
	int		RuneLimetLv;		//ІЕ¤е­­ЁоµҐЇЕ
	//--------------------------------------------------------------------------------------
	GuildResourceTypeENUM GuildResourceType;
	int	GuildResourceValue;

	int		SelectBaseAttrCount;
	int		BaseAttrCount;
	char	MagicToolsTpExpScript[64];	//Ґґ©З±oЁмExp »P Tp
	char	OnDestroyScript[64];
	int		ItemUniqueType;				//ёЛіЖ°Я¤@і]©w( 0 ) ЄнҐЬЁSі]©w
	int		FurnitureNPCObjID;

	char	ItemUseOKScript[64];
	int		DropListPacket;				//Є««~ЁПҐО¬°±јёЁЄн¤є©Т¦іЄF¦и
	struct
	{
		struct
		{
			SeedTypeENUM	Type;
			float			GrowSpeed;
			int				ModelType;		//јТ«¬Гю«¬
			int				BestWater;		//іМЁО¤ф¤А
			int				BestTopdress;	//іМЁОѕi¤А
			int				RandSeed;		//¶ГјЖєШ¤l
		}Seed;
		struct
		{
			union
			{
				int	Mode;
				struct  //ҐiєШґУГю«¬
				{
					bool	Tree : 1;
					bool	Grass : 1;
					bool	Mine : 1;
				};
			};

			float	EvnPoint[3][2];
		}Pot;
		struct
		{
			ManureTypeENUM	Type;
			//			float			EvnPoint[2];
			int				AddRare;
			int				AddWater;
			int				AddTopdress;
			int				AddHealth;
			int				AddGrow;
			int				FeedPoint;

		}Manure;
		int	Level;			//єШґУµҐЇЕ
		float			DecWater;		//¤ф¤АЁC¤p®Й¤U­°
		float			DecTopdress;	//ѕi¤АЁC¤p®Й¤U­°
	}Plant;

	FurnitureTypeENUM Furniture_Type;

	struct
	{
		int PetObjID;
		int Lv;
		int SkillPoint;
		int PetType;
		int	Property;
		int	Talent;
		int STR, STA, INT, MND, AGI;
		int Spirit;
		char LuaEvent[64];
	}Pet;

	struct
	{
		int SkillPoint;
		int	Spirit;
		int Loyal;
		int Talent;
		int Hunger;
		int Exp;

		PetFoodEffectTypeENUM EffectType;
		int	EffectValue;
	}PetFood;

	AddonPosTypeStruct ItemToItemLimit;


	union
	{
		struct
		{
			int DDmg;			//ЄZѕ№·LЅХёк®Ж
			int DMDmg;
		};
		struct
		{
			int DDef;			//ЁѕЁг·LЅХёк®Ж
			int DMDef;
		};
	};

	int		CardAddpower;		//Ґd¤щ±aЄєЇа¤Oёк®Ж

	int		FurnitureSoundID;	//ЇdБnѕчID

	ExBagTypeENUM		ExBagType;			//©с¤JЇS®нҐ]»q«б¤АГю
	int					ExBagTypeSub;

	int		BodyMaxCount;		//Ё­ЕйіМ¦hґX­У
	int		DrawOutCost;		//ЁъҐX»щїъ

	struct
	{
		int Famous;
		int	Exp;
	}TitleSys;

	int		RefineLv;				//Єм©lєлБеµҐЇЕ
	int		AddPowerQuality;		//ГBҐ~јWҐ[«В¤O
	int		CollectionNPC;			//№ПЕІ°С¦ТNPC

	struct
	{
		int Type;
		int	Count;
		int	TargetType;
		int NewType;
	}CardStone;

	bool IsCustom;
	int MinPrice;
	int	MaxPrice;
	int	CardDropRate;
	bool NewSystem;

//	int		ColorType;
//	int		RuneSystemTableID;		//АJ¤е¦@»пІХ °С¦ТЄнID

};

//----------------------------------------------------------------------------------------
//¤ЫЖF
//----------------------------------------------------------------------------------------

struct AbiParametersTableStructOld
{
	int Type;
	float Value; //��O�Ѽ�
};
// TODO: NEW FIELD
struct AbiParametersTableStruct
{
	int GUID;
	int Type;
	float Value; //��O�Ѽ�
};
struct PhantomRankTableStructOld
{
	int	Rank;
	int MaxLevel;
	int NeedFragment;
};
// TODO: NEW FIELD
struct PhantomRankTableStruct
{
	int	GUID;
	int	Rank;
	int MaxLevel;
	int NeedFragment;
};
struct PhantomLvTableStructOld
{
	int	LV;
	int Type;
	int Material[4];
};
// TODO: NEW FIELD
struct PhantomLvTableStruct
{
	int	GUID;
	int	LV;
	int Type;
	int Material[4];
};
struct PhantomSkillTableStructOld
{
	int	Phantom[2];
	int SkillID;
};
// TODO: NEW FIELD
struct PhantomSkillTableStruct
{
	int	GUID;
	int	Phantom[2];
	int SkillID;
};
struct GameObjectPhantomStruct
{
	int						RankUpMaterial;		//ґЈ¤Й¶ҐЇЕ§ч®Ж
	int						LevelUpType;		//¤ЙЇЕ§ч®ЖГю§O
	GameObjectWearEqStruct	abi_Phantom;		//¤ЫЖFЇа¤O	
	bool					Release;			//Є©Ґ»ДАҐX
};
//----------------------------------------------------------------------------------------
//ЄkіNєШГю¦CБ|
//----------------------------------------------------------------------------------------
enum	GameObjectMagicENUM
{
	//ҐЯ§Y¤ПАі
	EM_Magic_Attack,	//§рА»
	EM_Magic_Cure,	//ЄvАш
	EM_Magic_Rebuff,	//ҐhBuff

	//«щДт®ДЄG
	EM_Magic_Assist,	//»І§U®ДЄGЄєЄkіN
	EM_Magic_Disease,	//¤Ј¦nЄєЄ¬єA
	EM_GameObjectMagicEM_FORCE_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------------
//	ЄkіNҐШјРі]©w
//----------------------------------------------------------------------------------------
enum	GameObjectRelationENUM
{
	EM_Releation_Self,	//¦Ы¤v
	EM_Releation_Member,	//¶¤¤Н

	EM_Releation_Self_Pet,	//¦Ы¤v(pet)
	EM_Releation_Member_Pet,	//¶¤¤Н(pet)

	EM_Releation_Friend,	//¤Н¤и,(№пЄ±®a «h¬°©Т¦іЄ±®a   №п¤ЈДЭ©уЄ±®aЄєNPC «h¬°¤ЈДЭ©уЄ±®aЄєNPC)
	EM_Releation_Friend_NPC,	//¤Н¤иЄєГdЄ«(¦pГdЄ«ЎAҐlікҐXЁУЄєЎAДЭ©уётАH¦Ы¤vЄєNPC)

	EM_Releation_Enemy,	//©Т¦іҐi¬е±юЄє№п¶µ
	EM_Releation_Player,	//©Т¦іЄ±®a
	EM_Releation_All,	//©Т¦іЄ«Ґу

	EM_Releation_GuildPlayer,	//¦P¤Ѕ·|Є±®a
	EM_Releation_NotGuildPlayer,	//¤Ј¦P¤Ѕ·|Є±®a

	EM_Releation_CountryPlayer,	//¦P°к®aЄ±®a
	EM_Releation_NoCountryPlayer,	//¤Ј¦P°к®aЄ±®a

	EM_Releation_Corpse,	//«НЕй
	EM_Releation_Npc_Corpse,	//NPC«НЕй
	EM_Releation_Player_Corpse,	//Є±®a«НЕй
	EM_Releation_Locatoin,	//«ь©w¦мёm( ЅdітЄkіN )

	EM_Releation_PetOwner,	//¦Ы¤vЄєҐD¤H
	EM_Releation_Wagon,	//ёьЁг
	EM_Releation_Plot,	//ј@±ЎїпЁъ

	EM_GameObjectRelationEM_FORCE_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------------
//¦і®ДЅdіт
//----------------------------------------------------------------------------------------
//(¦nЄє¦і®ДЅdіт)¦Ы¤v ¶¤¤Н ¦P¤u·|
//(ГaЄє¦і®ДЅdіт)ҐiҐH¬е±юЄМ
enum	GameObjectMagicEffectRangeENUM
{
	EM_MagicEffect_Target,	//ҐШјР¦і®Д
	EM_MagicEffect_Good_Range,	//¦nЄєЅdітЄkіN
	EM_MagicEffect_Good_Member,	//¦nЄє¦Ё­ыЄkіN
	EM_MagicEffect_Bad_Range,	//ГaЄєЅdітЄkіN
	EM_MagicEffect_AllObj,
	EM_MagicEffect_AllPlayer,
	EM_MagicEffect_AllMonster,
	EM_MagicEffect_AllPlayer_Enemy,	//©Т¦іјД¤иЄ±®a
	EM_MagicEffect_AllPlayer_Friend,	//©Т¦і§Ъ¤иЄ±®a
	EM_MagicEffect_Plot,	//ј@±ЎїпЁъ
	EM_GameObjectMagicEffectRangeEM_FORCE_DWORD = 0x7fffffff,
};
//----------------------------------------------------------------------------------------
enum MagicFuncENUM
{
	EM_MAGICFUN_HPMP = 0,
	EM_MAGICFUN_Assist,	//»І§U
	EM_MAGICFUN_Teleport,	//¶З°e
	EM_MAGICFUN_SummonCreature,	//©Ыґ«ҐНЄ«(ГdЄ«)
	EM_MAGICFUN_SummonItem,	//©Ыґ«Є««~
	EM_MAGICFUN_Steal,	//°ЅЕС
	EM_MAGICFUN_ItemRunPlot,	//ІЈҐНЄ««~ЁГҐB°х¦жј@±Ў
	EM_MAGICFUN_RunPlot,	//°х¦жј@±Ў
	EM_MAGICFUN_Raise,	//ґ_¬Ў
};
//----------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
enum    ShootItemType
{
	EM_ShootItem_None,
	EM_ShootItem_Ammunition,
	EM_ShootItem_Arrow,
	EM_ShootItem_Throw,
};
//----------------------------------------------------------------------------------------
//ЄkіN®шЇУµІєc
//----------------------------------------------------------------------------------------
struct MagicCostStruct
{
	MagicSpellCostTypeENUM	Type;		//®шЇУГю«¬
	int				Value;				//®шЇУјЖ­И
};
//----------------------------------------------------------------------------------------
//ЄkіN»ЭЁDµІєc
//----------------------------------------------------------------------------------------
struct MagicNeedStruct
{
	MagicSpellNeedTypeENUM	Type;		//»ЭЁDГю«¬
	int						Value;		//­И
};
//-----------------------------------------------------------------------
//¶З°eЕ]Єk
//-----------------------------------------------------------------------
struct	MagicTeleport
{
	int	ZoneID;
	int	X, Y, Z;
	int Dir;
};


//-----------------------------------------------------------------------
//	§рА»Е]Єk
//-----------------------------------------------------------------------
enum  MagicAttackCalENUM
{
	EM_MagicAttackCal_None,
	EM_MagicAttackCal_MPow,
	EM_MagicAttackCal_Weapon,// (Ґ»Ё­DMG + ҐD¤вDMG)
	EM_MagicAttackCal_Shoot,//  ®gА»Гю( Ґ»Ё­DMG + ¤}DMG + јuГДDMG)
	EM_MagicAttackCal_Throw,//  §лВYГю( Ґ»Ё­DMG + јuГДDMG)
	EM_MagicAttackCal_INT,
	EM_MagicAttackCal_STR,
	EM_MagicAttackCal_AGI,
	EM_MagicAttackCal_STA,
	EM_MagicAttackCal_MND,
	EM_MagicAttackCal_LastSkillDmg,
	EM_MagicAttackCal_LastPhyDmg,
	EM_MagicAttackCal_ShieldDef,
};

//§рА»Гю«¬
enum	MagicAttackTypeENUM
{
	EM_MagicAttackType_HP,
	EM_MagicAttackType_MP,
	EM_MagicAttackType_SP_Warrior,
	EM_MagicAttackType_SP_Ranger,
	EM_MagicAttackType_SP_Rogue,
	EM_MagicAttackType_StomachPoint,
};

struct MagicAttackStruct
{
	MagicAttackTypeENUM	AtkType;

	float		DmgPower;
	float		DmgPower_SkillLVArg;

	float				FixValue;
	MagicAttackCalENUM  FixType;
	float		FixDmg_SkillLVArg;

	float       Rand;           //¶Л®` ЕЬ¤ЖЅdіт

	int			CritialRate;	//јWҐ[­P©R¤@А»Іv
	float		HateRate;
};
//-----------------------------------------------------------------------
//ґ_¬ЎіN
//-----------------------------------------------------------------------
struct MagicRaise
{
	int		ExpPrecnt;
};
//-----------------------------------------------------------------------
//ҐlікҐНЄ«
//-----------------------------------------------------------------------
enum MagicSummonCreatureTypeENUM
{
	EM_MagicSummonCreatureType_Pet,
	EM_MagicSummonCreatureType_Guard,
	EM_MagicSummonCreatureType_Guard_NoAttack,
};

enum PetSpellRightTimeENUM
{
	EM_PetSpellRightTime_OwnerStartUsing,		//ҐD¤Hі]©wЁПҐО(ґ`АфЁПҐО)
	EM_PetSpellRightTime_Auto,		//¦Ы°КЁПҐО	
};

enum PetSpellTargetENUM
{
	EM_PetSpellTarget_Self,
	EM_PetSpellTarget_Owner,
	EM_PetSpellTarget_Party,
	EM_PetSpellTarget_Enemy,
};
/*
struct PetSkillStruct
{
	int						KeyItemID;
	PetSpellRightTimeENUM	SpellRightTime;
	PetSpellTargetENUM		TargetType;
	int						Rate;			//¬IЄkѕчІv
	int						SkillID;		//ЄkіNID
};
*/

struct MagicSummonCreature
{
	int		ObjID;
	int		Level;
	int		RangeLevel;
	int		LiveTime;				//ҐН¦sґБ(-1)ҐNЄн¤Ј®шҐў
	float	SkillLvArg;
	MagicSummonCreatureTypeENUM Type;
	int		GroupID;				//¦P¤@ёsІХҐuЇа¦і¤@°¦
	float	OwnerPowerRate;			//јWҐ[Є±®a¤@©w¤сЁТЄєЇа¤OЁмГdЄ«¤W

	//PetSkillStruct	Skill[ _MAX_PET_SKILL_COUNT_ ];
	int		SkillType[3];
	char	InitLua[32];
};
//-----------------------------------------------------------------------
//ҐlікЄ««~
//-----------------------------------------------------------------------
struct MagicSummonItem
{
	int		ObjID;
};
//-----------------------------------------------------------------------
//°ЅЕС
//-----------------------------------------------------------------------
/*
enum	MagicStealClassENUM
{
	EM_MagicSteal_NULL	= 0	,		//¦№µІєcёк®ЖµL®Д
	EM_MagicSteal_Money		,
	EM_MagicSteal_Item		,
};
*/
struct MagicSteal
{
	//	MagicStealClassENUM		Class;
	int						SuccessRate;
};
//-----------------------------------------------------------------------
//ІЈҐНЄ««~°х¦жј@±Ў
//-----------------------------------------------------------------------
enum	MagicItemRunPlotTargetTypeENUM
{
	EM_MagicItemRunPlotTargetType_Enemy,
	EM_MagicItemRunPlotTargetType_Friend,
	EM_MagicItemRunPlotTargetType_Player,
	EM_MagicItemRunPlotTargetType_NPC,
	EM_MagicItemRunPlotTargetType_All,
	EM_MagicItemRunPlotTargetType_Friend_NoSelf,
	EM_MagicItemRunPlotTargetType_Player_NoSelf,
	EM_MagicItemRunPlotTargetType_All_NoSelf,
};
enum	MagicItemRunPlotENUM
{
	EM_MagicItemRunPlot_Plot,	//ј@±ЎГю
	EM_MagicItemRunPlot_Mine,	//¦a№pГю
	EM_MagicItemRunPlot_Staff,	//ґЎ§ъГю
};
struct MagicItemRunPlot
{
	MagicItemRunPlotENUM	Type;	//Гю«¬
	int		ObjID;
	char	PlotName[64];
	int		LiveTime;				//ҐН¦sґБ(-1)ҐNЄн¤Ј®шҐў
	int		UseMagic;				//ЁПҐОЄkіN
	int		OnDeadMagic;			//¦є¤`¶¶¶ЎІЈҐНЄєЄkіN
	int		GroupID;				//ёsІХID	( 0  ҐNЄн¤Ј¤АёsІХ)
	union
	{
		int		Mode;
		//ObjectModeStruct	Mode;		//Є«ҐуЄє°тҐ»ДЭ©К
		struct
		{
			bool	Mark : 1;
			bool	Fight : 1;
			bool	NotShowHPMP : 1;
			bool	EnemyHide : 1;
			bool	NotRelyOwnerPower : 1;
		};
	};

	MagicItemRunPlotTargetTypeENUM		TargetType;
	//	int		Level;
};
//-----------------------------------------------------------------------
//°х¦жј@±Ў
//-----------------------------------------------------------------------
struct MagicRunPlot
{
	char	PlotName[64];
};

//----------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------
enum    MagicGoodorBadENUM
{
	EM_Magic_Good,       //¦nЄkіN
	EM_Magic_Bad,       //ГaЄєЄkіN
};
//----------------------------------------------------------------------------------------
//ЄkіN ©О Є«Іz
//----------------------------------------------------------------------------------------
enum    MagicorPhyENUM
{
	EM_Magic_Magic,       //ДЭ©уЕ]Єk§рА»
	EM_Magic_Phy,       //ДЭ©уЄ«Іz§рА»
	EM_Magic_Eq,       //ёЛіЖ( ¦Ы°К¬I®i )
	EM_Magic_PetEq,       //ёЛіЖ( №}ѕiГdЄ« )
};

//----------------------------------------------------------------------------------------
//DotЄkіNµІєc
//----------------------------------------------------------------------------------------
enum	DotMagicTypeENUM
{
	EM_DotMagicType_HP,			//HP
	EM_DotMagicType_MP,			//MP
	EM_DotMagicType_WarriorSP,	//ѕФ¤hSP
	EM_DotMagicType_RangerSP,	//№C«LSP
	EM_DotMagicType_RogueSP,	//µsёйSP
	EM_DotMagicType_HP_Per,	//HP¦К¤А¤с
	EM_DotMagicType_MP_Per,	//MP¦К¤А¤с

};

struct DotMagicStruct
{
	int					Time;			 //¦h¤[¤@¦ё
	DotMagicTypeENUM    Type;			 //Гю§О( 0 HP , 1 MP , 2 SPѕФ¤h )
	int					Base;			 //ЁC¦ё¶Л®`
	float				DotSkillLVArg;	 //§ЮЇаµҐЇЕјvЕT
	bool IsEmpty()
	{
		return (Base == 0 || Time <= 0);
	}
	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};
//----------------------------------------------------------------------------------------
// Єk¤O¬Юі]©wёк®Ж
//----------------------------------------------------------------------------------------
enum	MagicShieldTypeENUM
{
	EM_MagicShieldType_MP,		//MP©и®шHP
	EM_MagicShieldType_Times,		//©и®ш¦ёјЖ
	EM_MagicShieldType_Point,		//©и®шЄkјЖі]©wЄєВIјЖ
	EM_MagicShieldType_Percent,		//©иѕP¦К¤А¤с
	EM_MagicShieldType_Point_ByHealArg,		//©и®шВIјЖ
};

enum	MagicShieldEffecENUM
{
	EM_MagicShieldEffec_All,	//©Т¦і§рА»
	EM_MagicShieldEffec_Phy,	//Є«Іz§рА»
	EM_MagicShieldEffec_Magic,	//ЄkіN§рА»
	EM_MagicShieldEffec_Earth,	//¦aЁtЄkіN§рА»
	EM_MagicShieldEffec_Water,	//¤фЁtЄkіN§рА»
	EM_MagicShieldEffec_Fire,	//¤хЁtЄkіN§рА»
	EM_MagicShieldEffec_Wind,	//­·ЁtЄkіN§рА»
	EM_MagicShieldEffec_Light,	//ҐъЁtЄkіN§рА»
	EM_MagicShieldEffec_Darkness,	//·tЁtЄkіN§рА»
};

struct MagicShieldStruct
{
	MagicShieldTypeENUM		Type;
	MagicShieldEffecENUM	Effect;
	int						Point;		//Ґi©lҐОВIјЖ
	float					SkillLvArg;
};

//----------------------------------------------------------------------------------------
// coldownёк®Жі]©w
//----------------------------------------------------------------------------------------
enum	MagicColdownClassENUM
{
	EM_MagicColdownClass_None,		//ЁS¦і
	EM_MagicColdownClass_Job,		//Вѕ·~Гю«¬
	EM_MagicColdownClass_EQ,		//ёЛіЖГю«¬
	EM_MagicColdownClass_Item,		//Є««~Гю«¬

};
struct MagicColdownBaseStruct
{
	//¤@ЇлЄєColdown
	MagicColdownClassENUM		Class;
	int							Type;
	int							Time;	//(¬н)
	//¤U¦ёҐiҐўЄkЄє®Й¶Ў
	int							TimeAllMagic;
};
//----------------------------------------------------------------------------------------
//ЇS®н¦ж¬°
//----------------------------------------------------------------------------------------
struct MagicAttackSpecialAction
{
	union
	{
		int _Type;
		struct
		{
			bool	None : 1;	//«OЇd
			bool    StrikeBack : 1;	//ѕ_°h
			bool	SpellInterrupt : 1;	//¬IЄk¤¤В_
			bool	IgnoreDefine : 1;	//µLµшЁѕїmor§Ь©К
			bool	EraseHate : 1;	//ІM°Ј¤і«л( ·|Ва¶щ ¦PҐ[¦е­pєв )
			bool	HateOnePoint : 1;	//¤і«лІM°Ј¬°1
			bool	Bomb : 1;	//¬µ­ё
		};
	};
};
//----------------------------------------------------------------------------------------
//§рА»ЄkіN­pєв¤и¦Ў¦CБ|
//----------------------------------------------------------------------------------------
enum MagicAttackCalBaseENUM
{
	EM_MagicAttackCalBase_Physics,//Є«Іz­pєв
	EM_MagicAttackCalBase_Magic,//ЄkіN­pєв
	EM_MagicAttackCalBase_Throw,//§лВY­pєв
	EM_MagicAttackCalBase_Shoot,//®gА»­pєв
	EM_MagicAttackCalBase_Fix,//©w­И
	EM_MagicAttackCalBase_HolyHeal,//Ї«ёtЄvАш­pєв( ­t­ИЄнҐЬMiss  )
	EM_MagicAttackCalBase_DarkHeal,//¤Ј¦єЁtАш­pєв( ­t­ИЄнҐЬMiss  )
	EM_MagicAttackCalBase_SpecialAction,//ЁS§рА»Ґu¦іЇS®н¦ж¬°
	EM_MagicAttackCalBase_FixPer,//¦К¤А¤с
	EM_MagicAttackCalBase_Physics_Point,//Є«Іz­pєв
	EM_MagicAttackCalBase_MagicEx,//ЄkјЖ­pєв ¤сЁТ*MDmg * MDmg_MagArg 

//	EM_MagicAttackCalBase_ShootEx		,//®gА»­pєв(¦PЄ«Іz­pєв)
};
//----------------------------------------------------------------------------------------
enum	SpecailMagicEventTypeENUM
{
	EM_SpecailMagicEventType_None,	//µL
	EM_SpecailMagicEventType_SelfCritial,	//¦Ы¤v §рА»­P©R¤@А»(ґ¶§р)
	EM_SpecailMagicEventType_TargetCritial,	//ҐШјР §рА»­P©R¤@А»(ґ¶§р)
	EM_SpecailMagicEventType_SelfDodge,	//¦Ы¤v °{БЧ
	EM_SpecailMagicEventType_TargetDodge,	//ҐШјР °{БЧ
	EM_SpecailMagicEventType_SelfMiss,	//¦Ы¤v Miss
	EM_SpecailMagicEventType_TargetMiss,	//ҐШјР Miss
	EM_SpecailMagicEventType_ZoneDamage,	//°П°м¶Л®`ЁЖҐу
	EM_SpecailMagicEventType_Phy_SelfCritial,	//¦Ы¤v §рА»­P©R¤@А»(Є«Іz)
	EM_SpecailMagicEventType_Phy_TargetCritial,	//ҐШјР §рА»­P©R¤@А»(Є«Іz)
	EM_SpecailMagicEventType_Mag_SelfCritial,	//¦Ы¤v §рА»­P©R¤@А»(ЄkіN)
	EM_SpecailMagicEventType_Mag_TargetCritial,	//ҐШјР §рА»­P©R¤@А»(ЄkіN)
	EM_SpecailMagicEventType_Cure_Self,	//¦Ы¤v ЁьЄvАш
	EM_SpecailMagicEventType_Cure_Target,	//ҐШјР ЄvАшҐШјР
	EM_SpecailMagicEventType_SelfParry,	//¦Ы¤v ®жѕЧ
	EM_SpecailMagicEventType_TargetParry,	//ҐШјР ®жѕЧ
	EM_SpecailMagicEventType_SelfSpell,	//¦Ы¤v¬IЄk
};
//----------------------------------------------------------------------------------------
//ДЭ©уЄkіNЄє°тҐ»ёк®Ж
//----------------------------------------------------------------------------------------
#define _MAX_BUFFSKILL_COUNT_	6
struct GameObjectMagicStruct
{
	MagicFuncENUM			MagicFunc;				//ЄkіNєШГю(­nҐОЁє­УЄkіNFunctionҐh©IҐs)
	MagicResistENUM			MagicType;              //Е]ЄkГю«¬
	MagicorPhyENUM			EffectType;

	float	    EffectTime;							//¦і®Д®Й¶Ў(0.1 ¬н¬°іж¦м)
	float       DLV_EffectTime;					    //µҐЇЕ®tјvЕT¦і®Д®Й¶Ў(0.1 ¬н¬°іж¦м)
	float		EffectTime_SkillLVArg;				//§ЮЇаµҐЇЕјvЕT®Й¶Ў°СјЖ
	int		    HateCost;							//¤і«л­И(+ ҐШјРҐ[¦h¤Ц¤і«л­И)(- ҐШјРЄє§рА»ЄєҐШјР ґо¦h¤Ц¤і«л­И)

	int			MaxBuffLv_Base;						//Buf°пЕ|®ДЄGЄєіМ°ЄµҐЇЕ
	float		MaxBuffLv_SkillLVArg;				//¦]§ЮЇа®tјvЕTbufіМ°ЄµҐЇЕЄє°СјЖ

	int			MagicGroupSet;						//¦№Е]Єk©ТДЭЄєГю«¬
	//-----------------------------------------------------------------------
	MagicSpellFlagStruct			Setting;		//ЄkіNі]©wєXјР
	MagicAttackSpecialAction		SpecialAction;	//ЇS®нЄє¦ж¬°

	//-----------------------------------------------------------------------
	//(«щДтјvЕTЄєјЖ­И)»І§UЕ]Єk
	//-----------------------------------------------------------------------
	GameObjectMagicTypeENUM			AssistType;			//ЄkіNГю«¬(ҐD­nҐО©у»І§UЄkіN¤¬ҐёіBІz)
	int		                        EffectLV;			//»І§UЄkіN«В¤OµҐЇЕ
	int								EffectAddonType;
	MagicEffectStruct				Effect;				//јvЕT®ДЄG


	MagicClearTimeStruct            ClearTime;			//®ш°Ј±шҐу
	GameObjectWearEqStruct			Ability;			//јvЕTДЭ©К
	float							Ability_SkillLVArg;	//§ЮЇаµҐЇЕјvЕT®ДЄG

	int                             OnTimeMagic_Magic;  //©w®ЙІЈҐНЄkіN®ДЄG(ЄkіNiD)
	int                             OnTimeMagic_Time;   //©w®ЙІЈҐНЄkіN®ДЄG(¦h¤[¤@¦ё)
	int                             FaceOffID;          //ЕЬЁ­ID( 0 ҐNЄн¤Ј§пЕЬ )
	int								RideID;				//ГM­јID( 0 ҐNЄн¤Ј§пЕЬ )

	int								OnAttackMagicID;		//·ніQ§рА»®ЙІЈҐНЄєЄkіN
	int								OnBuffTimeOutMagicID;	//·н¦№»І§UЄkіNµІ§ф®ЙІЈҐНЄєЄkіN
	int								OnAttackReboundMagicID;	//іQҐґЁм¤ПјuҐXЁУЄєЄkіN
	int								OnMagicAttackReboundMagicID;	//іQҐґЁм¤ПјuҐXЁУЄєЄkіN
	int								OnDeadMagicID;			//¦є¤`®ЙІЈҐНЄєЄkјЖ
	int								TempHateCost;			//јИ®ЙјWҐ[Єє¤і«лЅи
	int								ClearMagicGroupSet;		//ЁПЁєЁЗГюЄєЄkіNµL®Д
	DotMagicStruct                  DotMagic;
	//-----------------------------------------------------------------------
	//§рА»Е]Єk
	//-----------------------------------------------------------------------
	MagicAttackStruct				Attack;		//§рА»Е]Єkёк®Ж
	//-----------------------------------------------------------------------
	//¶З°eЕ]Єk
	//-----------------------------------------------------------------------
	MagicTeleport					Teleport;
	//-----------------------------------------------------------------------
	//ґ_¬ЎіN
	//-----------------------------------------------------------------------
	MagicRaise						Raise;
	//-----------------------------------------------------------------------
	//ҐlікҐНЄ«
	//-----------------------------------------------------------------------
	MagicSummonCreature				SummonCreature;
	//-----------------------------------------------------------------------
	//ҐlікЄ««~
	//-----------------------------------------------------------------------
	MagicSummonItem					SummonItem;
	//-----------------------------------------------------------------------
	//°ЅЕС
	//-----------------------------------------------------------------------
	MagicSteal						Steal;

	//-----------------------------------------------------------------------
	//ІЈҐНЄ««~°х¦жј@±Ў
	//-----------------------------------------------------------------------
	MagicItemRunPlot				ItemRunPlot;
	//-----------------------------------------------------------------------
	//°х¦жј@±Ў
	//-----------------------------------------------------------------------
	MagicRunPlot					RunPlot;
	//-----------------------------------------------------------------------
	//Єk¤O¬Ю
	MagicShieldStruct				MagicShield;

	//§рА»ЄkіN­pєв¤и¦Ў
	MagicAttackCalBaseENUM			AtkCalType;

	//ЄkіNёsІХ¤АГю
	int								MagicGroupID;

	//Buff¤WЕгҐЬЄєјЖ¦r
	int								BuffCount;

	//Є«ІzјЬ®`Гю«¬
	PhyResistTypeENUM				PhyAttackType;

	//////////////////////////////////////////////////////////////////////////
	//¤¶­±іBІz
	bool							IsStandard_Attack;
	float							RefPowerLv_Attack;
	float							RefPowerRate_Attack;
	//////////////////////////////////////////////////////////////////////////
	float							ModelSize;			//јТ«¬БY©с¤сЁТ

	int								Revive_SkillLVArg;	//µҐ·ҐјvЕT¦^ґ_ёgЕз­И¦К¤А¤с

	int								AddBuffTime;		//јWҐ[Buff®Й¶Ў( ¬н )
	int								OnKillMagicID;		//±ю¦є®ЙІЈҐНЄєЄkјЖ
	char							CheckUseLua[_MAX_SIZE64_QUEST_SCRIPT];

	SpecailMagicEventTypeENUM		SpecialMagicEventType;
	int								OnSpecialMagicID;

	int								BuffTimeDesc_Type;
	int								BuffTimeDesc_Time;

	int								BuffSkill[_MAX_BUFFSKILL_COUNT_];

	char							EndUseLua[_MAX_SIZE64_QUEST_SCRIPT];
	int								HitBackDist;

	int								CarryCount;			//Ґiёь¤HјЖ
	int								MusicID;			//BUFFДІµo­Iґє­µјЦ
	int								BuffMaxLv;
	int								OnBuffTimeOutMagicIDEx;	//·н¦№»І§UЄkіNµІ§ф®ЙІЈҐНЄєЄkіN(Ґu¦іҐї±`®Й¶ЎЁм·|ДІµo)
	int								MotionChangeType[4];	//°К§@§пЕЬГю«¬
	int								MotionChangeID[4];		//°К§@§пЕЬID
	MagicSpellFlagExStruct			SettingEx;				//ЄkіNі]©wєXјР

};
//----------------------------------------------------------------------------------------
//Вѕ·~§ЮЇа
//----------------------------------------------------------------------------------------
//Вѕ·~±MДЭ§ЮЇаі]©w
struct  JobSkillBaseStruct
{
	int		MagicID;        //ЄkіN
	int		LearnLV;        //¦Ы°КѕЗ·|µҐЇЕ
	int		KeyItemID;      //ҐІ­nЄє­«­nЄ««~
	int		NeedMagicID[2];	//»Э­nЄkіN


	char	NeedMagicLv[2];	//»Э­nЄkіNµҐЇЕ
	char	SaveLvPos;		//µҐЇЕАx¦s¦мёm
	char	Reserve;		//(«OЇd)

};

struct GameObjectLearnMagicStruct
{
	int		SPSkillCount;
	JobSkillBaseStruct  SPSkill[_MAX_Edit_SPSkill_COUNT_];            //µҐЇЕЁм¦Ы°КѕЗ·|
	int		NormalSkillCount;
	JobSkillBaseStruct  NormalSkill[_MAX_Edit_NormalSkill_COUNT_];
};

//----------------------------------------------------------------------------------------
//°У©±µІєc
//----------------------------------------------------------------------------------------
struct GameObjectShopStruct
{
	int         BuyRate;
	int         SellRate;
	YesNoENUM   WeaponRepair;
	int         SellItemID[30];
	int         SellItemMax[30];
	int         SellItemPorduce[30];

	int				SellCost[30][2];
	PriceTypeENUM	CostType[30][2];
};
//////////////////////////////////////////////////////////////////////////
enum LanguageTypeENUM
{
	EM_LanguageType_RW = -1,
	EM_LanguageType_TW,
	EM_LanguageType_CN, //ВІЕй
	EM_LanguageType_ENUS,
	EM_LanguageType_DE, //јw¤е
	EM_LanguageType_ENEU, //
	EM_LanguageType_JP, //¤й¤е
	EM_LanguageType_KR, //Бъ¤е
	EM_LanguageType_BR, //¤Ъ¦и
	EM_LanguageType_TR, //¤g¦ХЁд
	EM_LanguageType_VN, //¶V«n
	EM_LanguageType_RU, //«XГ№ґµ
//	EM_LanguageType_SG		, //¬PҐ[©Y
EM_LanguageType_TH, //®х°к
EM_LanguageType_FR, //Єk°к
EM_LanguageType_ID, //¦LҐ§
EM_LanguageType_ES, //¦иЇZ¤ъ
EM_LanguageType_PH, //µб«Я»«
EM_LanguageType_AE, //ЁFЇQ¦aЄь©Ф§B
//	EM_LanguageType_NL		, //ІьДх
	//EM_LanguageType_RO		, //Г№°ЁҐ§ЁИ
	EM_LanguageType_PL, //ЄiДх
	EM_LanguageType_PT, //ёІµе¤ъ»y
	EM_LanguageType_SAES, //¤¤«n¬ь¬w(¦иЇZ¤ъ)
	EM_LanguageType_ENAR, //¤¤ЄF­^¤е
	EM_LanguageType_SAPT, //¤¤«n¬ь¬w(ёІµе¤ъ)
	EM_LanguageType_IT, //ёq¤j§Q


	EM_LanguageType_Count,
};

enum CountryTypeENUM
{
	EM_CountryType_RW = -1,
	EM_CountryType_TW,
	EM_CountryType_CN, //ВІЕй
	EM_CountryType_ENUS,
	EM_CountryType_DE, //јw¤е
	EM_CountryType_ENEU, //
	EM_CountryType_JP, //¤й¤е
	EM_CountryType_KR, //Бъ¤е
	EM_CountryType_BR, //¤Ъ¦и
	EM_CountryType_TR, //¤g¦ХЁд
	EM_CountryType_VN, //¶V«n
	EM_CountryType_RU, //«XГ№ґµ
	EM_CountryType_SG, //¬PҐ[©Y
	EM_CountryType_FR, //Єk°к
	EM_CountryType_ID, //¦LҐ§
	EM_CountryType_ES, //¦иЇZ¤ъ
	EM_CountryType_NL, //ІьДх
	EM_CountryType_PH, //µб«Я»«
	EM_CountryType_RO, //Г№°ЁҐ§ЁИ
	EM_CountryType_PT, //ёІµе¤ъ
	EM_CountryType_PL, //ЄiДх
	EM_CountryType_TH, //®х°к
	EM_CountryType_AE, //ЁFЇQ¦aЄь©Ф§B
	EM_CountryType_SA, //«n¬ь
	EM_CountryType_AUS, //їD¦{
	EM_CountryType_IT, //ёq¤j§Q

	EM_CountryType_Count,
};
//----------------------------------------------------------------------------------------
//®MёЛ
//----------------------------------------------------------------------------------------
struct GameObjectSuitStruct
{
	int TotalCount;		//®MёЛБ`јЖ

	struct
	{
		WearEqTypeENUM  EqType[3];
		int             EqTypeValue[3];
	}SuitBase[9];		//ЁCµҐЇЕ®MёЛГBҐ~±aЁУЄєЇа¤O


	struct
	{
		union
		{
			GameObjectWearEqStruct	Ability;
			struct
			{
				WearEqTypeENUM  EqType[10];
				int             EqTypeValue[10];
			};
		};
	}SuitValue[9];		//ЁCµҐЇЕ®MёЛ±aЁУЄєЇа¤O(ІЦ­p­И)

	int	SuitList[10];	//©Т¦і®MёЛ

	//®MёЛ§ЮЇа ¦¬¶°§№ѕг®MҐiїпЄє§ЮЇа
	int	SuitSkillID[4];
	int SuitSkillMoney;

};
//----------------------------------------------------------------------------------------
//°}Азёк°T
//----------------------------------------------------------------------------------------
#define _MAX_CAMP_COUNT_ 15
enum GameCampTypeENUM
{
	EM_GameCampType_Alliance = 0,	//¦P·щ¤ЈҐi§рА» ( ·|А°¦Ј)
	EM_GameCampType_Friend,	//¤НµЅ( ·|А°¦Ј ) ЎA Ґi±jЁо§рА»
	EM_GameCampType_Neutrality,	//¤¤ҐЯ
	EM_GameCampType_Enemy,	//јД¤H
};

struct GameCampStatuteStruct
{
	char	CampName[_MAX_CAMP_COUNT_][_MAX_OBJ_NAME_STR_SIZE_];		//°}Аз¦WєЩ
	char	CampInfo[_MAX_CAMP_COUNT_][_MAX_CAMP_COUNT_ + 1];		//°}Аз¤§¶ЎЄєГц«Y //GameCampTypeENUM
};
//----------------------------------------------------------------------------------------
//¶}Д_µІєc
//----------------------------------------------------------------------------------------
#define _MAX_TREASURE_COUNT_	100
struct TreasureBaseInfo
{
	int		OrgID;
	int		Count;
	int		DropCount;		//ЁC¦ё±јёЁ¦h¤Ц­У
};

struct TreasureGambleInfo
{
	int	Cost;
	int	HoleRate[4];
	//int	AddPower;
	ItemDropStruct DropItemAbility[_MAX_DROP_ABILITY_COUNT_];
};

struct GameTreasureStruct
{
	int		ItemCount;
	TreasureBaseInfo Item[_MAX_TREASURE_COUNT_];		//Є««~Б`јЖ¶qҐ[°_ЁУ¤@©w¬° 10 ёU­У
	int		NeedDLv;									//(¦ЁҐЯ±шҐу)©ЗЄ«µҐЇЕ®t
	TreasureGambleInfo Gamble;
	char	LuaCheckScript[_MAX_SIZE64_QUEST_SCRIPT];
};
//----------------------------------------------------------------------------------------
//ЄkіN¶°¦X
//----------------------------------------------------------------------------------------
struct	MagicCollectFlagStruct
{
	union
	{
		int _Type;
		struct
		{
			bool    Interrupt_OnMove : 1;	//¬IЄk¤¤В_ Іѕ°К
			bool    Interrupt_OnAttack : 1;	//¬IЄk¤¤В_ іQ§рА»
			bool    Spell_Back : 1;	//јД¤H­I«б¤~Ґi¬I®i
			bool    Spell_Front : 1;	//јД¤HҐї­±¤~Ґi¬I®i
			bool    Spell_LookAtTarget : 1;	//­±¦VјД¤H¤~Ґi¬I®i
			bool	ReferenceWeapon_Bow : 1;	//°С·У¤} §ріt §рА»¶ZВч ( §N«o®Й¶Ў¶ZВч)
			bool	ReferenceWeapon_Throw : 1;	//°С·У·tѕ№ §ріt §рА»¶ZВч ( §N«o®Й¶Ў¶ZВч)
			bool	Dash : 1;	//ЅДјІ
			bool	AllColdown_Reference_Weapon : 1;	//¦@ҐО§N«o °С¦ТЄZѕ№§ріt
			bool	NoInterrupt : 1;	//¤Ј·|іQ§ЮЇа¤¤В_
			bool	HideCastingBar : 1;	//¤ЈЕгҐЬ¬IЄk±ш
			bool	Interrupt_SpellOtherMagic : 1;	//¬I®iЁдҐ¦ЄєЄkіN¤¤В_
			bool	Ignore_Obstruct : 1;	//©їµшЄэАЙЄ«(ЅdітЄkіN)
			bool	ReferenceWeapon_Bow_SpellTime : 1;	//°С·У¤} §ріt §рА»¶ZВч ( ¬IЄk®Й¶Ў¶ZВч)
			bool	ReferenceWeapon_Throw_SpellTime : 1;	//°С·У·tѕ№ §ріt §рА»¶ZВч ( ¬IЄk®Й¶Ў¶ZВч)
			bool	IgnoreFightingLog : 1;	//©їІ¤ѕФ°«°Oїэ
			bool	NoObjectInMagicEffectRange : 1;	//Ѕdіт¤є¤ЈҐiҐH¦іЄ«Ґу
			bool	DescDurable : 1;	//·lЇУ­@¤[«Ч
			bool	forceDisplayEffect : 1;	//і]©wЇS®ДҐІ»ЭіQЕгҐЬ
			bool	IgnoreSpellMagicClear : 1;	//©їІ¤¬I®i®шҐў
			bool	SpellStopMove : 1;	//¬IЄk°±¤оІѕ°К
		};
	};
};


//----------------------------------------------------------------------------------------
//ЄkіN¶°¦X
//----------------------------------------------------------------------------------------


enum MagicCheckFunctionENUM
{
	EM_MagicCheck_None,//"µL"											
	EM_MagicCheck_Rand,//"¶ГјЖ"										(r,    ) r¤@«ЯҐNЄнѕчІv
	EM_MagicCheck_Self_Eq,//"¦Ы¤vёЛіЖ( ѕчІv , ёЛіЖё№ЅX )"				­Y¦Ы¤v¦і¦№ёЛіЖ(r,Ѕsё№)
	EM_MagicCheck_Target_Eq,//"ҐШјРёЛіЖ( ѕчІv , ёЛіЖё№ЅX )"				­YҐШјР¦і¦№ёЛіЖ(r,Ѕsё№)
	EM_MagicCheck_Self_EqType,//"¦Ы¤vёЛіЖГю«¬( ѕчІv , ёЛіЖГю«¬ )"			­Y¦Ы¤v¦і¦№ГюЄZѕ№(r,Гю«¬Ѕsё№)
	EM_MagicCheck_Target_EqType,//"ҐШјРёЛіЖГю«¬( ѕчІv , ёЛіЖГю«¬ )"			­YҐШјР¦і¦№ГюЄZѕ№(r,Гю«¬Ѕsё№)
	EM_MagicCheck_Self_Item,//"¦Ы¤vЄ««~( ѕчІv , Є««~ or ­«­nЄ««~ )"		­Y¦Ы¤v¦і(r,Є««~or­«­nЄ««~)
	EM_MagicCheck_Target_Item,//"ҐШјРЄ««~( ѕчІv , Є««~ or ­«­nЄ««~ )"		­YҐШјР¦і(r,Є««~or­«­nЄ««~)
	EM_MagicCheck_Self_Buff,//"¦Ы¤vBuff( ѕчІv , ѕЦ¦і¬YBuff )"				­Y¦Ы¤vѕЦ¦і¬Ybuff(r,Ѕsё№)
	EM_MagicCheck_Target_Buff,//"ҐШјРBuff( ѕчІv , ѕЦ¦і¬YBuff )"				­YҐШјРѕЦ¦і¬Ybuff(r,Ѕsё№)
	EM_MagicCheck_Self_Position,//"¦Ы¤v¦мёm( ѕчІv , ¦мёmїл§OЅX )"				ЁS¦і§@ҐО¤wІѕ°Ј
	EM_MagicCheck_Target_Position,//"ҐШјР¦мёm( ѕчІv , ¦мёmїл§OЅX )"				ЁS¦і§@ҐО¤wІѕ°Ј
	EM_MagicCheck_Self_Fight,//"¦Ы¤vѕФ°«( ѕчІv , 0«DѕФ°«/1ѕФ°«  )"			¦Ы¤vЄ¬єA(r,0«DѕФ°«/1ѕФ°«)
	EM_MagicCheck_Target_Fight,//"ҐШјРѕФ°«( ѕчІv , 0«DѕФ°«/1ѕФ°«  )"			ҐШјРЄ¬єA(r,0«DѕФ°«/1ѕФ°«)
	EM_MagicCheck_Target_Race,//"ҐШјРєШ±Ъ( ѕчІv , єШ±ЪID  )"					ҐШјРЄєєШ±Ъ¬O(r,єШ±ЪЅsё№)
	EM_MagicCheck_Self_HP,//"¦Ы¤vHP§C©у( ѕчІv , HP¦К¤А¤с )"				¦Ы¤vHP§C©у(r, ¦К¤А¤с)
	EM_MagicCheck_Time,//"®Й¶Ў( ѕчІv , ®Й¶Ў )"						ЁS¦і§@ҐО¤wІѕ°Ј
	EM_MagicCheck_Weather,//"®р­Ф( ѕчІv , ®р­Фїл§OЅX )"					ЁS¦і§@ҐО¤wІѕ°Ј
	EM_MagicCheck_Self_BuffGroup,//"¦Ы¤vBuff Group( ѕчІv , ѕЦ¦і¬YBuffёsІХ )"	¦Ы¤v¦і¬YГю«¬Єєbuff(r, Гю«¬Ѕsё№)		
	EM_MagicCheck_Target_BuffGroup,//"ҐШјРBuff Group( ѕчІv , ѕЦ¦і¬YBuffёsІХ )"	¦Ы¤v¦і¬YГю«¬Єєbuff(r, Гю«¬Ѕsё№)		
	EM_MagicCheck_Self_Skill,//"¦Ы¤vѕЗ·|¬Y§ЮЇа( ѕчІv , ЄkіNID )"			¦Ы¤vҐiЁПҐО¬Y§ЮЇа(r,ЄkіN¶°¦X)
	EM_MagicCheck_BaseLv,//"»ЭЁDµҐЇЕ( ѕчІv , LV )"						­YµҐЇЕ¶W№L(r,µҐЇЕ)

	EM_MagicCheck_MaxCount,
};

//¬IЄk±шҐу
struct MagicCollectCondition
{
	MagicCheckFunctionENUM	FuncType;
	int		MagicBaseID;
	int		Arg[2];

	GameObjDbStruct* MagicBaseDB_Backup;
};


enum HitRateFuncENUM
{
	EM_HitRateFunc_Base,				//©w­И
	EM_HitRateFunc_DLV,				//µҐЇЕ®t
	EM_HitRateFunc_DLVxDLV,			//µҐЇЕ®tҐ­¤и
	EM_HitRateFunc_DecPerPersion,	//ЁМ¤H¦ё­°§C
	EM_HitRateFunc_Shoot,	//®gА»­pєв

};

#define _MAX_MAGICCOLLECT_COUNT_			4
#define _MAX_MAGICCOLLECT_GROUP_COUNT_		3

enum MagicSpellGroupTypeENUM
{
	EM_MagicSpellGroupType_Normal,	//ҐюіЎїWҐЯ­pєв
	EM_MagicSpellGroupType_FirstGroupHit,	//ІД¤@­УЄkіN©R¤¤¤~·|іBµoІД¤G»PІД¤TІХЄkіN
	EM_MagicSpellGroupType_PrvGroupHit,	//«e¤@ІХЄkіN©R¤¤ЎA¤U¤@ІХ¤~·|іBІz
	EM_MagicSpellGroupType_Vampire,	//ІД¤@­УёsІХ­n©R¤¤ ¤G¤TёsІХ№п¦Ы¤v¬IЄk
	EM_MagicSpellGroupType_Vampire1,	//ІД¤@­УёsІХ­n©R¤¤ ¤TёsІХ№п¦Ы¤v¬IЄk
};

enum MagicSpellSelectTypeENUM
{
	EM_MagicSpellSelectType_Circle,//¶к§О
	EM_MagicSpellSelectType_Line1,//ЄЅЅu( ҐШјР­±¦V  ¦і®Д¶ZВч)
	EM_MagicSpellSelectType_Line2,//ЄЅЅu( ¬IЄkЄМ -> ҐШјР )
	EM_MagicSpellSelectType_Line3,//ЄЅЅu( ¬IЄkЄМ¦VҐШјР  ¦і®Д¶ZВч )
	EM_MagicSpellSelectType_Fan,//®°§О
	EM_MagicSpellSelectType_Lighting1,//іsВк№q
	EM_MagicSpellSelectType_Lighting2,//іsВк№q( Ґi­«ВРїп )
	EM_MagicSpellSelectType_Horizontal,//ѕоЅu
	EM_MagicSpellSelectType_Box,//¤и«¬
};

struct MagicJobInfoStruct
{
	Voc_ENUM	Job;			//ЄkіNВѕ·~
	bool		IsJobLimit;		//¬O§_Вѕ·~­­©w
};

enum MagicComboTypeENUM
{
	EM_MagicComboType_Null = -1,	//µL®Д
	EM_MagicComboType_Earth,	//¦a
	EM_MagicComboType_Water,   //¤ф
	EM_MagicComboType_Fire,	//¤х
	EM_MagicComboType_Wind,	//­·
	EM_MagicComboType_Light,	//Ґъ
	EM_MagicComboType_Darkness,	//·t
	EM_MagicComboType_Start,	//°_°К§Ю
};

// CLIENT ¬IЄk«e§PВ_ЄэАЙ
enum MagicCollisionTypeENUM
{
	EM_MagicCollisionType_None = 0,				// µLЄk
	EM_MagicCollisionType_Line,							// ¬IЄkЄМЁмҐШјРЄЅЅuґъёХ
	EM_MagicCollisionType_TargetForward,				// ҐШјРЄє­±¦V
};

struct GameObjectMagicCollectStruct
{
	//	int								MagicBase;      //ІД¤@ЇЕЄkіN( )
	int								MagicLV;        //ЄkіNµҐЇЕ
	MagicorPhyENUM					EffectType;		//Є«Іz±HЇа or ЄkіN§ЮЇа or іQ°К§ЮЇа

	//-------------------------------------------------------------------------------------
	//ЅdітїпЁъ
	//-------------------------------------------------------------------------------------
	GameObjectRelationENUM			TargetType;	    //¦і®ДҐШјР
	int								AttackDistance;	//¦і®Д¶ZВч
	int								EffectRange;    //¦і®ДЅdіт
	GameObjectMagicEffectRangeENUM	RangeType;	    //¦і®ДГю«¬
	MagicSpellSelectTypeENUM        RangeSelectType;//ЅdітїпЁъ¤и¦Ў
	int                             EffectCount;    //¦і®Д¤HјЖ
	int								DecreaseArg;	//»јґо°СјЖ( µ№Ѕdіт«В¤OіBІzҐО)
	//-------------------------------------------------------------------------------------
	HitRateFuncENUM					HitRateFunc;	//©R¤¤Іv
	float							HitRateArg[2];	//©R¤¤Іv­pєв°СјЖ
	//-------------------------------------------------------------------------------------
	MagicCostStruct					Cost[2];	    //®шЇУЄ««~
	MagicNeedStruct					Need[2];		//»ЭЁDЄ««~
	MagicColdownBaseStruct			Coldown;		//ЄkіN§N«o
	MagicCollectFlagStruct			Flag;			//
	float							SpellTime;		//¬IЄk®Й¶Ў
	float							MagicAttackDelay;//¬IЄk§рА»©µїр(¬н)	
	int								SpellCount;		//¬IЄk¦ёјЖ
	float							NextSpellTime;	//¤U¦ё­«¬IЄkЄє¶Ў№j

	char							LUAScript[128];	//Аф№Ті]©wЄєScript і]©w §NЅT®Й¶Ў ®шЇУ ЄkіN­PЇа
	union
	{
		MagicCollectCondition			Magic[_MAX_MAGICCOLLECT_GROUP_COUNT_][_MAX_MAGICCOLLECT_COUNT_];		//¦№ЄkіN¤є§t¦іЄє©Т¦і¤ёҐуЄkіN
		MagicCollectCondition			Magic_List[_MAX_MAGICCOLLECT_GROUP_COUNT_ * _MAX_MAGICCOLLECT_COUNT_];		//¦№ЄkіN¤є§t¦іЄє©Т¦і¤ёҐуЄkіN
	};

	MagicSpellGroupTypeENUM			MagicSpellGroupType;
	char							CheckUseScript[128];//ЁПҐОАЛ¬d lua

	int								MaxSkillLv;		//Ґi­ЧЅmµҐЇЕ
	float							ExpTableRate;	//§ЮЇа¤ЙЇЕЄн­їІv
	bool							IsMagicAttackDelay_TargetDist;	//ГBҐ~јWҐ[ҐШјР¶ZВч­ё¦ж©Т»Э®Й¶Ў

	MagicJobInfoStruct				JobInfo;

	MagicComboTypeENUM				ComboType;

	int								NextMagicID;	//¬IЄkµІЄG«бЎAДІµoЄєЄkіN

	union
	{
		int					_Mode;
		struct
		{
			bool							IsAutoAttack : 1;		//¦Ы°К¶i¤JѕФ°«
			bool							IsSwitchType : 1;		//ҐГ¤[Гю«¬(¶}Гц)
			bool							IsRideEnable : 1;		//ГM­јҐiЁПҐО
			bool							IsShowMagicName : 1;		//¬IЄkЕгҐЬ¦WєЩ
			bool							IsStopAttack : 1;		//°±¤о§рА»
			bool							IsLockTarget : 1;		//­±¦VВк©w¬IЄkҐШјР
			bool							IsPetCycleMagic : 1;		//ГdЄ«Ґiґ`АфЄkіN
			bool							IsAutoSpell : 1;		//ГdЄ«ІЈҐН¦Ы°К¬I®i
			bool							IsIgnoreInvincible : 1;		//©їІ¤Є«Іz»PЄkіNµL®Д(ҐШјР)
			bool							IsIgnoreSilence : 1;		//©їІ¤Є«Іz»PЄkіNµL®Д(¦Ы¤v)
			bool							IsWaterDisabled : 1;		//¤ф¤¤¤ЈҐiЁПҐО
			bool							IsWaterEnabled : 1;		//¤ф¤¤ЁПҐО
			bool							IsRidePetSkill : 1;		//ГMГdЄє§ЮЇа(®yГMј·¬IЄk)
			bool							IsIgnoreTargetDead : 1;		//©їІ¤ҐШјР¦є¤`
			bool							IsDiabledPvPRule : 1;		//¤ЈіBІzІ{¦жҐЗіW«h
			bool							IsIgnoreDmgEffect : 1;		//ЄkіN¶Л®`( ¤Ј­pєв ЄZѕ№ )
			bool							IsFixDmgEffect : 1;		//ЄkіN¶Л®`( ЄZѕ№ Їа¤OҐ[¦Ё ©T©w1¬н )
			bool							IsOnWagonEnabled : 1;		//­ј®yёьЁгҐiҐHЁПҐО
			bool							IsDisableSpellBuffEffect : 1;		//BuffєXјРјvЕT¬IЄkµL®Д( Аюµo Іѕ°К¬IЄk µL®шЇУ )
			bool							IsShareDamage : 1;		//Ґ­§Ў¤Аѕб¶Л®`
		};
	};

	int								PetSkillType;			//ГdЄ«§ЮЇаГю«¬ -1ЄнҐЬ¬°Є±®aЄє§ЮЇа
	float							MoveDelayTime;			//¬IЄk§№Іѕ°К©µїр
	MagicCollisionTypeENUM			CliCollisionType;
	int								CliCollisionValue;
	int								LimitLv;				//­­ЁоµҐЇЕ
	char							SelectTargetScript[64];	//ЄkіNГz¬µҐШјРїпѕЬЁг±Ў
	char							ShootTargetScript[64];	//ЄkіNҐШјРїпѕЬ
	int								AddSoulPoint;			//јWҐ[SoulPoint
	int								SuitPassiveSkill;		//®MёЛіQ°К§ЮЇа 
};
//----------------------------------------------------------------------------------------
enum PrivateZoneTypeENUM
{
	EM_PrivateZoneType_None,	//¤@Їл
	EM_PrivateZoneType_Private,
	EM_PrivateZoneType_Party,
	EM_PrivateZoneType_Raid,
	EM_PrivateZoneType_Private_Party,
};
enum PKZoneTypeENUM
{
	EM_PKZoneType_Normal,	//¤@Їл«DPK°П
	EM_PKZoneType_PK,	//¤@ЇлPK°П
};

struct MapInfoStruct
{
	int		MapID;
	char	MapName[64];
	bool	IsEnabledSave;		//¬O§_ҐiҐH¦sВI
};

//----------------------------------------------------------------------------------------
struct GameObjectZoneStruct
{
	char	MapFile[64];					//¦a№ПАЙ
	bool	IsEnabledBlockCtrl;				//ҐО°П¶фі]©w±±ЁоЅdіт
	int		CtrlMaxZ;
	int		CtrlMaxX;
	int		CtrlMinZ;
	int		CtrlMinX;

	bool	IsPrivateZone;
	int		BaseRoomCount;
	int		PlayRoomCount;
	PrivateZoneTypeENUM	PrivateZoneType;	//їWҐЯ°П°мГю«¬	
	int		PrivateZoneLiveTime;			//їWҐЯ°П°мЁS¤HҐiҐHҐН¦s¦h¤[ 


	bool	IsEnabledRevPoint;				//±jЁо­«ҐН¦мёm
	int		RevX, RevY, RevZ;				//­«ҐН¦мёm
	int		RevZoneID;

	int		ViewBlock;						//µшіҐ°П¶ф¤j¤p
//	PKZoneTypeENUM	PKType;	
	int		MapID;
	int		CampStatueObjID;				//°}Азёк®Ж

	int		MapCount;						//¤p°П¶ф­УјЖ
	MapInfoStruct	MapInfoList[40];		//°П§Цёк°T

	char	LuaInitZone[64];
	bool	IsEnabledRide;

	struct
	{
		char	Key[64];
		char	Value[64];
	}ZoneIni[30];

	int		RoomPlayerCount;				//©Р¶ЎіМ¦hЄ±®aјЖ

	bool	IsWorldBattleGround;			// ¬O§_¬°Ґ@¬ЙѕФіх
	int		iWorldBattleGroundWorldID;		// Ґ@¬ЙѕФіхЄA°Иѕ№ҐNё№

	bool	IsCountryBattleGround;			// ¬O§_¬°°кѕФѕФіх

	bool	IsAllowTeamJoin;				// ¬O§_¤№і\№О¶¤Ґ[¤J
	bool	IsEnableBattleGroundQueue;		// ¬O§_°СҐ[ѕФіх±Ж¶¤ЁtІО

	int		IsLimitJoinLevel;				// ¬O§_­­©w¶i¤JµҐЇЕ
	int		iMinJoinLevel;					// іМ§Cµn¤JµҐЇЕ
	int		iMaxJoinLevel;					// іМ°Єµn¤JµҐЇЕ

	bool	IsForceGroup;					// ¬O§_±jЁоІХ¶¤

	char	LuaPreJoinQueueCheck[64];
	char	LuaAssistWorldGroupID[64];
	int		iCountryWarLinkMap[4];

	bool	IsAutoJoinEventZone;			// ¬O§_¦Ы°КҐ[¤JЁЖҐу°ЖҐ»

	int     MirrorInstanceType;				// ГиҐ@¬Й°ЖҐ»ёsІХ
	int     MirrorLimitLv;					// ГиҐ@¬Й°ЖҐ»­­ЁоµҐЇЕ
	int     MirrorLimitTime;				// ГиҐ@¬Й°ЖҐ»­­Ёо®Й¶Ў
	int		Mirror_X, Mirror_Y, Mirror_Z;	// ГиҐ@¬Й­«ҐН¦мёm

};
//----------------------------------------------------------------------------------------
struct CreateRoleEQInfoStruct
{
	union
	{
		int ID[18];
		struct
		{
			int Head;
			int Gloves;
			int Shoes;
			int Clothes;
			int Pants;
			int Back;
			int Belt;
			int Shoulder;
			int Necklace;
			int Ammo;
			int Bow;
			int Ring1;
			int Ring2;
			int Earring1;
			int Earring2;
			int MainHand;
			int SecondHand;
			int Manufacture;
		};
	};
};


struct GameObjectCreateRoleStruct
{
	Voc_ENUM Job;

	//­«ҐН¦мёm	
	struct
	{
		int	ZoneID;
		int X, Y, Z, Dir;
	}BornPos;

	CreateRoleEQInfoStruct EQ;

	int ItemID[10];
	int ItemCount[10];

	RaceFlagStruct	RaceLimit;

	CreateRoleEQInfoStruct EQPreview;
};

struct GameObjectPEStruct
{
	int ParentGUID;
	int Type;

	union
	{
		int _FlagData;

		struct
		{
			bool UIVisible : 1;
			bool ScoreVisible : 1;
		}PE;

		struct
		{
			bool SLogic : 1;
			bool FLogic : 1;
		}PH;

		struct
		{
			bool Visible : 1;
			bool Minor : 1;
		}OB;

	}Flag;

	union
	{
		char _MainData[256];

		struct
		{
			int CoolDown;
			int ZoneID;
			int SubZoneID[10];
		}PE;

		struct
		{
			int PHIndex;
			int NPHTimeGap;
			int Score;
			int BonusScore1;
			int BonusScore2;
		}PH;

		struct
		{
			int OBIndex;
			int OBType;
			int OBCamp;
			int InitValue;
			int Threshold;
			int NPHGUID;
			int NPHTimeGap;
			char VarName[32];
		}OB;

		struct
		{
			char VarName[32];
			char VarNote[128];
		}VAR;

	}Data;

	union
	{
		struct
		{
			char LuaEvent1[64];
			char LuaEvent2[64];
			char LuaEvent3[64];
			char LuaEvent4[64];
			char LuaEvent5[64];
			char LuaEvent6[64];
			char LuaEvent7[64];
			char LuaEvent8[64];
			char LuaEvent9[64];
			char LuaEvent10[64];
		}All;

		struct
		{
			char OnBegin[64];
			char OnSuccess[64];
			char OnFail[64];
			char OnBalance[64];
			char OnPlayerEnter[64];
			char OnPlayerLeave[64];
			char OnPHBegin[64];
			char OnPHSuccess[64];
			char OnPHFail[64];
			char OnOBAchieve[64];
		}PE;

		//struct
		//{
		//}PH;

		//struct
		//{
		//}OB;

	}LuaEvent;
};

//----------------------------------------------------------------------------------------
//№CАё°тҐ»Є«Ґуёк®Ж
//----------------------------------------------------------------------------------------
struct	GameObjDbStruct
{
	int								GUID;
	GameObjectClassificationENUM	Classification;

	union
	{
		int		LanguageSet;
		struct
		{
			bool	Language1 : 1;
			bool	Language2 : 1;
			bool	Language3 : 1;
			bool	Language4 : 1;
			bool	Language5 : 1;
			bool	Language6 : 1;
			bool	Language7 : 1;
			bool	Language8 : 1;
		};
	};


	char* Name;							//Є«Ґу¦WєЩ
	char* Name_Plural;					//ЅЖјЖ¦WєЩ

	int                             ImageID;                        //јv№іёк®Ж
	GameObjectImageStruct* ImageObj_Backup;

	//Ег№іёк®Ж
	int								MaxHeap;			            //іМ¤j°пЕ|¤W­­
	char* Note;							//Є«ҐуµщёСёк®Ж
	char* ShortNote;						//Є«ҐуµщёСёк®Ж
	ObjectModeStruct				Mode;				            //Є«ҐуЄє°тҐ»ДЭ©К
	PriceTypeENUM					PricesType;						//»щ­ИГю«¬
	int								SellType;						//°У«~¤АГю
	int								Cost;				            //Є«ҐуЄє»щ­И ( ҐH№CАё№ф¬°©w»щ )
	int								BuyUnit;						//БК¶Rіж¦м(Client°У©±БК¶RЁПҐО)
	int								ReviveTime;			            //Є«ҐуЄє­«ҐН®Й¶Ў
	int                             Rare;                           //µ}¦і«Ч
	float					        GenerateRate;		            //ІЈҐНіt«Ч(
	float					        MaxWorldCount;		            //іМ¤jЄє°П°мБ`¶q	

	LimitStruct                     Limit;

	ItemLiveTimeTypeENUM			LiveTimeType;					//Є««~ҐН¦s®Й¶ЎГю«¬
	int								LiveTime;						//¬н¬°іж¦м

	int								ACSearchType[3];				//AC ©зЅжЄє¤АГю
	EqWearSkillStruct				ItemEqType;						//AC ·jґM¤АГюҐО

	int								SpecialMoneyCost;				//ЇS®ніf№ф©w»щ, ҐH№CАё№фҐHҐ~Єєіж¦м­p»щ
	int								iTagID;							// TAGID 


	union
	{
		GameObjectNpcStruct				NPC;
		GameObjectItemStruct			Item;

		GameObjectMagicStruct			MagicBase;
		GameObjectMagicCollectStruct	MagicCol;

		GameObjectAttributeStruct		Attirbute;
		GameObjectPlayerStruct	    	Player;
		GameObjectBodyObjStruct	    	BodyObj;
		GameObjectQuestDetail			QuestDetail;
		GameObjectMineStruct        	Mine;
		GameObjectKeyItemStruct     	KeyItem;
		GameObjectRecipeStruct      	Recipe;
		GameObjectTitleStruct       	Title;
		GameObjectLearnMagicStruct  	LearnMagic;
		GameObjectShopStruct        	Shop;
		GameObjectImageStruct       	Image;
		GameObjectSuitStruct        	Suit;
		GameCampStatuteStruct			Camp;		//°}Азёк°T
		GameTreasureStruct				Treasure;
		GameObjectEqRefineAttributeStruct	EqRefineAttribute;
		GameObjectZoneStruct			Zone;
		GameObjectCreateRoleStruct		CreateRole;
		GameObjectPEStruct				PE;
		GameObjectPhantomStruct			Phantom;

		char	Temp[20000];
	};

};


//----------------------------------------------------------------------------------------
//ёьTable ҐО
//----------------------------------------------------------------------------------------
#define _MAX_SKILL_LEVE_	100		//§ЮЇа¤W­­µҐЇЕ

struct		LvExpRelationTableOld			//ёgЕзЅи¬ЫГцЄн
{
	int 	LV;				//µҐЇЕ
	int 	JobExp;			//Вѕ·~ёgЕз­И
	int 	SkillExp;		//§ЮЇајфЅmёgЕз­И
	int 	GatherExp;		//±Д¶°ёgЕз­И
	int 	RefineExp;		//¦X¦ЁёgЕз­И
	int 	PlotExp;		//ј@±ЎёgЕзішДw
	int 	PlotMoney;		//ј@±ЎЄчїъішДw
	float	JobExp_LvGrowRate;	//ёgЕз¦ЁЄшІv
	int		JobMagicExp;	//Вѕ·~
	int		StandardCost;

	float	BaseMagicPower;
	float	MagicPowerGrowRate;
	int		AbilityPoint;
	int		DefPoint;
	int		HPPoint;
	int		HouseExpArg;	//©Р«Оexp №пАі¦ёјЖ
	float	MagicMPGrowRate;

	int		Pet_Exp;
	int		Pet_LifeSkill;
	int		PlantExp;		//єШґУёgЕз­И
	float	PlantEffectPoint;
};
// TODO:: NEW FIELD
struct LvExpRelationTable			//ёgЕзЅи¬ЫГцЄн
{
	int GUID;
	int JobExp; // WORK
	int unknown1;
	int unknown2;
	int unknown3;
	int LV; // WORK
	int SkillExp; // WORK
	int GatherExp; // WORK
	int RefineExp;
	int PlotExp; // WORK
	int PlotMoney; // WORK
	float JobExp_LvGrowRate; // WORK
	int JobMagicExp; // WORK
	int StandardCost;
	float BaseMagicPower; // WORK
	float MagicPowerGrowRate; // WORK
	int AbilityPoint; // WORK
	int DefPoint; // WORK
	int HPPoint; // WORK
	int HouseExpArg; // WORK
	float MagicMPGrowRate; // WORK
	int Pet_Exp; // WORK
	int Pet_LifeSkill; // WORK
	int PlantExp; // WORK
	float PlantEffectPoint; // WORK
};
//////////////////////////////////////////////////////////////////////////
//©оёСЄ««~ёк®ЖTable
struct	LvDissolutionTable
{
	int		Lv;
	int		Weapon_Item;
	float	Weapon_Rate;
	int		Armor_Item;
	float	Armor_Rate;
	int		Ring_Item;
	float	Ring_Rate;
};
//////////////////////////////////////////////////////////////////////////
struct	ComboTableOld
{
	int	Type[5];	//іsДв¤и¦Ў
	int	Bonus;		//°ЖҐ[ЄєЄkіN
	int	Final;		//µІ§ф§Ю¬I®iЄєЄkіN
};
// TODO:: NEW FIELD
struct	ComboTable
{
	int GUID;
	int	Type[5];	//іsДв¤и¦Ў
	int	Bonus;		//°ЖҐ[ЄєЄkіN
	int	Final;		//µІ§ф§Ю¬I®iЄєЄkіN
};

//////////////////////////////////////////////////////////////////////////
//єлЅmЄZѕ№Ґ[­ј
struct EqRefinePowerStruct
{
	float	DmgRate;
	float	DefRate;
	float	MagicAtkRate;
	float	MagicDefRate;
	float	AddDmg;
	float	AddDef;
};
//////////////////////////////////////////////////////////////////////////
//іРЁ¤ёк®ЖµІєc
#define DEF_MAX_COUNT_CreateRoleInfo_Body	100
#define DEF_MAX_COUNT_CreateRoleInfo_Color	60
#define DEF_MAX_COUNT_CreateRoleInfo_Job	20
struct CreateRoleInfoTableStructOld
{
	int	BodyCount;
	struct
	{
		bool	Enabled;
		int		OrgID;
		int		Type;
		int		ID;
	} Body[DEF_MAX_COUNT_CreateRoleInfo_Body];

	int	ColorCount;
	struct
	{
		bool	Enabled;
		int		ID;
		int		Type;
		unsigned char	R, G, B;
		Race_ENUM	Race;
	} Color[DEF_MAX_COUNT_CreateRoleInfo_Color];

	int	JobCount;
	int		JobID[DEF_MAX_COUNT_CreateRoleInfo_Job];
	char	JobKeyStr[DEF_MAX_COUNT_CreateRoleInfo_Job][32];
};
// TODO:: NEW FIELD
struct CreateRoleInfoTableStruct
{
	int GUID;
	int	BodyCount;
	struct
	{
		bool	Enabled;
		int		OrgID;
		int		Type;
		int		ID;
	} Body[DEF_MAX_COUNT_CreateRoleInfo_Body];

	int	ColorCount;
	struct
	{
		bool	Enabled;
		int		ID;
		int		Type;
		unsigned char	R, G, B;
		Race_ENUM	Race;
	} Color[DEF_MAX_COUNT_CreateRoleInfo_Color];

	int	JobCount;
	int		JobID[DEF_MAX_COUNT_CreateRoleInfo_Job];
	char	JobKeyStr[DEF_MAX_COUNT_CreateRoleInfo_Job][32];
};
//////////////////////////////////////////////////////////////////////////
//MotionЄєёк®Ж
struct MotionTableStructOld
{
	int		ID;
	int		MotionID;
	char	IconStr[64];
};
// TODO:: NEW FIELD
struct MotionTableStruct
{
	int GUID;
	int		ID;
	int		MotionID;
	char	IconStr[64];
};
//////////////////////////////////////////////////////////////////////////
//±РѕЗёк®Ж
struct TeachTableStructOld
{
	int		ID;
	int		Height;
	int		Width;
	char	TypeStr[32];
	char	ImageStr[128];
};
// TODO:: NEW FIELD
struct TeachTableStruct
{
	int GUID;
	int		ID;
	int		Height;
	int		Width;
	char	TypeStr[32];
	char	ImageStr[128];
};
//////////////////////////////////////////////////////////////////////////
enum	ColoringShopTypeENUM
{
	EM_ColoringShopType_Main,
	EM_ColoringShopType_Off,
	EM_ColoringShopType_Skin,
	EM_ColoringShopType_Hair,
	EM_ColoringShopType_HairStyle,
	EM_ColoringShopType_FaceStyle,
};
struct ColoringShopStructOld
{
	int							ID;
	ColoringShopTypeENUM		Type;	//0 ҐD¦в 1 °Ж¦в 2 Ѕ§¦в 3 ѕv¦в 4ѕv«¬ 5Бy«¬
	union
	{
		int Value;
		struct
		{
			unsigned char B, G, R, A;
		};

	}Color;
	int		ObjID;				//°С¦ТЄ«Ґу
	int		Cost;
};
// TODO:: NEW FIELD
struct ColoringShopStruct
{
	int GUID;
	int							ID;
	ColoringShopTypeENUM		Type;	//0 ҐD¦в 1 °Ж¦в 2 Ѕ§¦в 3 ѕv¦в 4ѕv«¬ 5Бy«¬
	union
	{
		int Value;
		struct
		{
			unsigned char B, G, R, A;
		};

	}Color;
	int		ObjID;				//°С¦ТЄ«Ґу
	int		Cost;
};
//////////////////////////////////////////////////////////////////////////
//Ґ@¬Й¦a№Пёк®Ж
enum WorldMapITypeENUM
{
	EM_WorldMapIType_None = 0, //¤ЈЁПҐО
	EM_WorldMapIType_World = 1, //¤jі°
	EM_WorldMapIType_Zone = 2, //¦a°П
	EM_WorldMapIType_City = 3, //«°Ґ«
	EM_WorldMapIType_Instance = 4, //°ЖҐ»
	EM_WorldMapIType_WorldInstance = 5, //Ґ@¬Й°ЖҐ»
	EM_WorldMapIType_BusinessInstance = 6, //¬Ў°К°ЖҐ» 
	EM_WorldMapIType_Battleground = 7, //ѕФіх
	EM_WorldMapIType_Other = 8, //ЁдҐL 


};

struct WorldMapInfoStructOld
{
	char    wdbFile[64];	//wdbАЙ¦W ҐNЄн¤@­У®yјРЁtІО
	char	MapName[64];	//Name
	int		MapID;			//ui ЕгҐЬЄєҐ@¬Й¦a№ПЇБ¤Ю
	int 	WorldID;		//±N¤Ј¦PwdbFile ¤АГюЁм¦P¤@Ґ@¬ЙҐО
	int 	Type;			//WorldMapITypeENUM
	int		LX;
	int 	LZ;
	int 	RX;
	int 	RZ;
};
// TODO:: NEW FIELD
struct WorldMapInfoStruct
{
	int GUID;
	char    wdbFile[64];	//wdbАЙ¦W ҐNЄн¤@­У®yјРЁtІО
	char	MapName[64];	//Name
	int		MapID;			//ui ЕгҐЬЄєҐ@¬Й¦a№ПЇБ¤Ю
	int 	WorldID;		//±N¤Ј¦PwdbFile ¤АГюЁм¦P¤@Ґ@¬ЙҐО
	int 	Type;			//WorldMapITypeENUM
	int		LX;
	int 	LZ;
	int 	RX;
	int 	RZ;
};
//////////////////////////////////////////////////////////////////////////
/*
struct GuildShopInfoStruct
{
	int FlagID;
	int MemberCount;
	int RankCount;
	int NeedFlagID;		//¶·­nЁє­УєXјР¶}±Т¤~Їа
	int	Cost;
	bool	EnabledGuildBoard;
};
*/
struct GuildLvInfoStructOld
{
	int	Lv;
	union
	{
		int _UpgradeValue[7];
		struct
		{
			int	UpgradeGold;
			int	UpgradeBonusGold;
			int	UpgradeMine;
			int	UpgradeWood;
			int	UpgradeHerb;
			int	UpgradeGuildRune;
			int	UpgradeGuildStone;
		};
	};
	int	MemberCount;
	int	DeclareWar;
	int	GuildFlag;
	int	GuildHouse;
	int	GuildWar;
	int	CityWar;
	int	Quest;
	int RankCount;
	int	ChangeGuildStyle;
};
// TODO:: NEW FIELD
struct GuildLvInfoStruct
{
	int GUID;
	int	Lv;
	union
	{
		int _UpgradeValue[7];
		struct
		{
			int	UpgradeGold;
			int	UpgradeBonusGold;
			int	UpgradeMine;
			int	UpgradeWood;
			int	UpgradeHerb;
			int	UpgradeGuildRune;
			int	UpgradeGuildStone;
		};
	};
	int	MemberCount;
	int	DeclareWar;
	int	GuildFlag;
	int	GuildHouse;
	int	GuildWar;
	int	CityWar;
	int	Quest;
	int RankCount;
	int	ChangeGuildStyle;
};
//////////////////////////////////////////////////////////////////////////
struct HouseShopInfoStructOld
{
	int FlagID;
	int FunitureCount;
	int	OpenEnergy;
	int	KeppEnergy;
	int Type;			//( 0 ЄнҐЬ¬°іГ­С®ж¤l 1 ЄнҐЬЁдҐ¦Гю )
};
// TODO:: NEW FIELD
struct HouseShopInfoStruct
{
	int GUID;
	int FlagID;
	int FunitureCount;
	int	OpenEnergy;
	int	KeppEnergy;
	int Type;			//( 0 ЄнҐЬ¬°іГ­С®ж¤l 1 ЄнҐЬЁдҐ¦Гю )
};
//////////////////////////////////////////////////////////////////////////
struct HouseEnergyCostStruct
{
	int		HouseEnergy;
	int		AccountMoneyCost;
};

//////////////////////////////////////////////////////////////////////////
struct AccountShopInfoStruct
{
	int		Type[4];		//¤АГю
	int		ItemObjID;	//Є««~ID
	int		Count;		//јЖ¶q
	int		Cost;		//»щїъ
	int		MarkType;	// Hot or OnSell or New
};
//////////////////////////////////////////////////////////////////////////
struct SysColorDefineStruct
{
	int Type;
	int ID;
	union
	{
		int Value;
		struct
		{
			unsigned char B, G, R, A;
		};

	}Color;
};
//////////////////////////////////////////////////////////////////////////
struct SysKeyFunctionStructOld
{
	int		KeyItemID;
	int		MagicID;
};
// TODO:: NEW FIELD
struct SysKeyFunctionStruct
{
	int GUID;
	int		KeyItemID;
	int		MagicID;
};
//////////////////////////////////////////////////////////////////////////
struct LotteryInfoTableStructOld
{
	int		SelectCount;			//їпґX­Уё№ЅX
	int		CombinationCount;		//Б`¦@¦і¦h¤ЦІХ¦X
	int		SelectPrize[6][3];		//¤¤N­УЄє¦U¶µјъ¤¤јъ¦ёјЖ
};
// TODO:: NEW FIELD
struct LotteryInfoTableStruct
{
	int GUID;
	int		SelectCount;			//їпґX­Уё№ЅX
	int		CombinationCount;		//Б`¦@¦і¦h¤ЦІХ¦X
	int		SelectPrize[6][3];		//¤¤N­УЄє¦U¶µјъ¤¤јъ¦ёјЖ
};
//////////////////////////////////////////////////////////////////////////
struct SysKeyValueTableStructOld
{
	char  KeyStr[64];
	int	  Value;
};
// TODO:: NEW FIELD
struct SysKeyValueTableStruct
{
	int GUID;
	char  KeyStr[64];
	int	  Value;
};
//////////////////////////////////////////////////////////////////////////
//Їа¶qґ«євЇа¤OҐ[­јЄн
struct PowerRateTableStructOld
{
	int		Power[5];
	float	PowerRate[5];
};
// TODO:: NEW FIELD
struct PowerRateTableStruct
{
	int GUID;
	int		Power[5];
	float	PowerRate[5];
};
//////////////////////////////////////////////////////////////////////////
//Їа¶qґ«євЇа¤OҐ[­јЄн
struct RentFunctionTableStructOld
{
	int		MemberType;
	int		Cost;
	float	RentDay;
	int		RentFunc[20];
};
// TODO:: NEW FIELD
struct RentFunctionTableStruct
{
	int GUID;
	int		MemberType;
	int		Cost;
	float	RentDay;
	int		RentFunc[20];
};
//////////////////////////////////////////////////////////////////////////
//ЅбҐ[Їа¤OѕчІv
struct InherentRateTableStructOld
{
	int	Lv;
	union
	{
		int AllRateLv[50];
		struct
		{
			int	ItemRateNone;
			int ItemRateSpacial;
			int	ItemRateLv[33];
		};
	};
};
// TODO:: NEW FIELD
struct InherentRateTableStruct
{
	int GUID;
	int	Lv;
	union
	{
		int AllRateLv[50];
		struct
		{
			int	ItemRateNone;
			int ItemRateSpacial;
			int	ItemRateLv[33];
		};
	};
};
//////////////////////////////////////////////////////////////////////////
//¤Ѕ·|єXі]©wёк®Ж
struct	 GuildFlagTableStructOld
{
	int		ID;
	int		Type;
	char	Str[50];
};
// TODO:: NEW FIELD
struct	 GuildFlagTableStruct
{
	int GUID;
	int		ID;
	int		Type;
	char	Str[50];
};
//////////////////////////////////////////////////////////////////////////
//¤Ѕ·|«Ші]
struct GuildBuildingInfoTableStructOld
{
	int		ID;
	char	BuildPoint[64];
	char	TypeStr[32];
	int		Lv;
	int		OrgObjID;				//npc °С¦ТЄ«ҐуID
	int		SrcBuildingID;

	GuildResourceStruct Resource;
	char	NeedTypeStr1[32];
	char	NeedLv1;
	char	NeedTypeStr2[32];
	char	NeedLv2;
	int		BlockSizeX;
	int		BlockSizeY;
	ObjectModeStruct Mode;
	int		MaintenanceCharge;		//єы«щ¶O
	int		NeedGuildLv;
	int		BuildSize;
	char	IconPath[64];
	int		SortID;

	void init()
	{
		memset(this, 0, sizeof(*this));
	}
};
// TODO:: NEW FIELD
struct GuildBuildingInfoTableStruct
{
	int GUID;
	int		ID;
	char	BuildPoint[64];
	char	TypeStr[32];
	int		Lv;
	int		OrgObjID;				//npc °С¦ТЄ«ҐуID
	int		SrcBuildingID;

	GuildResourceStruct Resource;
	char	NeedTypeStr1[32];
	char	NeedLv1;
	char	NeedTypeStr2[32];
	char	NeedLv2;
	int		BlockSizeX;
	int		BlockSizeY;
	ObjectModeStruct Mode;
	int		MaintenanceCharge;		//єы«щ¶O
	int		NeedGuildLv;
	int		BuildSize;
	char	IconPath[64];
	int		SortID;

	void init()
	{
		memset(this, 0, sizeof(*this));
	}
};


//¤Ѕ·|°тҐ»«Шїv
struct GuildBaseBuildingTableStructOld
{
	int		GUID;
	char	BuildPoint[32];
	int		ParentID;
	int		BuildID;
};
// TODO:: NEW FIELD
struct GuildBaseBuildingTableStruct
{
	int GUID_NEW;
	int		GUID;
	char	BuildPoint[32];
	int		ParentID;
	int		BuildID;
};
//////////////////////////////////////////////////////////////////////////
//¤Ѕ·|Є««~­¶БК¶R»щ®ж
struct	GuildHouseItemPageCostTableStructOld
{
	int		GUID;
	int		ItemPageID;
	GuildResourceStruct Resource;

	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};
// TODO:: NEW FIELD
struct	GuildHouseItemPageCostTableStruct
{
	int GUID_NEW;
	int		GUID;
	int		ItemPageID;
	GuildResourceStruct Resource;

	void Init()
	{
		memset(this, 0, sizeof(*this));
	}
};
//////////////////////////////////////////////////////////////////////////
//Ґш¤Жі]©w¦a№ПВI
struct MapPointStructOld
{
	char	KeyStr[64];
	char	wdbFile[64];
	float	X;
	float	Y;
	float	Z;
};
// TODO:: NEW FIELD
struct MapPointStruct
{
	int GUID;
	char	KeyStr[64];
	char	wdbFile[64];
	float	X;
	float	Y;
	float	Z;
};
//Гц«YЄkіN 
struct RelationMagicStructOld
{
	RelationTypeENUM Relation;
	int	Lv;
	int	NormalMagic;
	int Quarter_HP_Magic;

	void Init()
	{
		Relation = EM_RelationType_None;
		Lv = 0;
		NormalMagic = 0;
		Quarter_HP_Magic = 0;
	};
	bool IsEmpty()
	{
		if (Relation == EM_RelationType_None)
			return true;
		return false;
	}
};
// TODO:: NEW FIELD
struct RelationMagicStruct
{
	int GUID;
	RelationTypeENUM Relation;
	int	Lv;
	int	NormalMagic;
	int Quarter_HP_Magic;

	void Init()
	{
		Relation = EM_RelationType_None;
		Lv = 0;
		NormalMagic = 0;
		Quarter_HP_Magic = 0;
	};
	bool IsEmpty()
	{
		if (Relation == EM_RelationType_None)
			return true;
		return false;
	}
};
/*
//ІЕ¤еВаґ«Єн
struct RuneExchangeTableStruct
{
	int	ExchangeTable[50];
};
*/
struct RuneExchangeExTableStructOld
{
	int	Type1;
	int	Type2;
	int ResultType;
};
// TODO:: NEW FIELD
struct RuneExchangeExTableStruct
{
	int GUID;
	int	Type1;
	int	Type2;
	int ResultType;
};

/*
//////////////////////////////////////////////////////////////////////////
//ГdЄ«ёк®ЖЄн
//////////////////////////////////////////////////////////////////////////
//°йѕiГdЄ«°тҐ»°СјЖ
struct CultivatePetBaseValue
{
	float Sweet;		//їЛ»e«Ч
	float Loyal;		//©ѕёЫ«Ч
	float Grow;			//¦ЁЄш«Ч
	float Fitness;		//°·¬ь«Ч
	float Hunger;		//№Ў­№«Ч
	float Tired;		//ЇhіТ«Ч
};
//////////////////////////////////////////////////////////////////////////
//ГdЄ«¶i¤ЖЄн
struct CultivatePetBaseTableStruct
{
	int PetObjID;				//ГdЄ«Є«Ґу
	int BasePetObjID;			//­мЁУЄєГdЄ«Є«Ґу
	int	PetType[2];
	int NewSkill[3];

	int Evolve_RarePoint;		//¶i¤Ж»ЭЁDБэ­№µ}¦іВIјЖ
	int Evolve_GrowPoint;		//¶i¤Ж»ЭЁDѕiЁ|ВIјЖ

	CultivatePetBaseValue Fix;	//јЖ­И·LЅХјvЕT
};
//////////////////////////////////////////////////////////////////////////
//ГdЄ«ДЭ©КІЈҐНЄkіNBuffЄн
struct CultivatePetValueEffectTableStruct
{
	int	MagicBuffID;
	CultivatePetBaseValue LimitLow;
	CultivatePetBaseValue LimitHi;
};
//////////////////////////////////////////////////////////////////////////
//ГdЄ«ЁЖҐу¦p¦уі]©w( Ґґ¤u , Є±­A , ґІЁB , °VЅm , ­ЧІЯ )»PјvЕTЄєјЖ­И(table)
struct CultivatePetEventTableStruct
{
	int		ActionType[2];				//¬Ў°К¤АГю
	char	KeyStr[64];					//ЁЖҐу¤є®e
	char	LuaScript[64];				//ЁЖҐу°х¦жЄєј@±Ў
	struct
	{
		int		PetType[2];			//ГdЄ«Гю«¬
		int		KeyItem;			//­«­nЄ««~
		int		Character;			//­У©К
		CultivatePetBaseValue Low;
		CultivatePetBaseValue Hi;

	}Limit;
	CultivatePetBaseValue Effect;
	int		Money;						//»ЭЁD or Ат±oЄчїъ
};
//////////////////////////////////////////////////////////////////////////
//ГdЄ«»ЎёЬЄн
struct CultivatePetTalkTableStruct
{
	char  KeyStr[64];
	struct
	{
		int		KeyItem;
		int		Character;
		int		PetType[2];
		CultivatePetBaseValue Low;
		CultivatePetBaseValue Hi;
	}Limit;

	int		ActionTimeType;
	int		IntervalTime;
	int		Rate;
};
//////////////////////////////////////////////////////////////////////////
//ГdЄ«¦ЁЄшЄн
struct CultivatePetGrowTableStruct
{
	int Type;
	int Lv;
	int Policy;
	int Str;
	int Sta;
	int Int;
	int Mnd;
	int Agi;
	};
*/
//////////////////////////////////////////////////////////////////////////
/*
//ГdЄ«°тҐ»Єн
struct CultivatePetBaseTableStruct
{
	int PetType;
	int	BaseStr;
	int	BaseSta;
	int	BaseInt;
	int	BaseMnd;
	int	BaseAgi;
	int	MinTalent;
	int	MaxTalent;
};
*/
//ГdЄ«№ПЕІ
struct PetCollectionTableStructOld
{
	int GUID;
	int ObjID;
};
// TODO:: NEW FIELD
struct PetCollectionTableStruct
{
	int GUID_NEW;
	int GUID;
	int ObjID;
};
//ГdЄ«§ЮЇа
struct CultivatePetLearnMagicTableStructOld
{
	struct
	{
		int PetType;
		int PetProperty;
		int Lv;
		int Str;
		int Sta;
		int Int;
		int Mnd;
		int Agi;
		int KeyItem;
		int Lock;			//±MДЭ¬YPetType,¤ЈЇаД~©У
	}Limit;

	int MagicID;
	int	PowerLv;
	int SkillPoint;		//»ЭЁD§ЮЇаВIјЖ

	bool operator > (const CultivatePetLearnMagicTableStructOld& P) const
	{
		if (MagicID == P.MagicID)
			return PowerLv > P.PowerLv;
		return MagicID > P.MagicID;
	};

	bool operator < (const CultivatePetLearnMagicTableStructOld& P) const
	{
		if (MagicID == P.MagicID)
			return PowerLv < P.PowerLv;
		return MagicID < P.MagicID;
	};
};

// TODO:: NEW FILED
struct CultivatePetLearnMagicTableStruct
{
	int GUID;
	struct
	{
		int PetType;
		int PetProperty;
		int Lv;
		int Str;
		int Sta;
		int Int;
		int Mnd;
		int Agi;
		int KeyItem;
		int Lock;			//±MДЭ¬YPetType,¤ЈЇаД~©У
	}Limit;

	int MagicID;
	int	PowerLv;
	int SkillPoint;		//»ЭЁD§ЮЇаВIјЖ

	bool operator > (const CultivatePetLearnMagicTableStruct& P) const
	{
		if (MagicID == P.MagicID)
			return PowerLv > P.PowerLv;
		return MagicID > P.MagicID;
	};

	bool operator < (const CultivatePetLearnMagicTableStruct& P) const
	{
		if (MagicID == P.MagicID)
			return PowerLv < P.PowerLv;
		return MagicID < P.MagicID;
	};
};

//ГdЄ«ҐН¬Ў§ЮЇа
struct CultivatePetLifeSkillTableStructOld
{
	int Lv;
	int SkillType;
	int EventTime;
	struct
	{
		int SkillLv;
		int	ToolID;
		int ItemID;
		int ItemCount;
	}Need;
	struct
	{
		int ItemID;
		//int	ItemCount;
		//int	ItemMaxCount;
	}Product;
};
// TODO:: NEW FIELD
struct CultivatePetLifeSkillTableStruct
{
	int GUID;
	int Lv;
	int SkillType;
	int EventTime;
	struct
	{
		int SkillLv;
		int	ToolID;
		int ItemID;
		int ItemCount;
	}Need;
	struct
	{
		int ItemID;
		//int	ItemCount;
		//int	ItemMaxCount;
	}Product;
};
//////////////////////////////////////////////////////////////////////////
/*
//єШґУ ¬IЄОёк®Ж
struct PlantGrowActionTableStruct
{
	int ID;
	int	Type;
	int GrowRate;
	int Water;			//0§_ 1 Ґi
	int Topdress;
	int MagicWater;
};
*/
//////////////////////////////////////////////////////////////////////////
//єШґУ ¦¬¦Ёёк®Ж
struct PlantProductTableStructOld
{
	int SeedObjID;
	int Env_RarePoint;
	int Env_GrowPoint;
	int DropID;
	int	DropID_DLv5;
	int DropID_DLv10;
	float	ExpRate;
	char	LuaScript[32];
};
// TODO:: NEW FIELD
struct PlantProductTableStruct
{
	int GUID;
	int SeedObjID;
	int Env_RarePoint;
	int Env_GrowPoint;
	int DropID;
	int	DropID_DLv5;
	int DropID_DLv10;
	float	ExpRate;
	char	LuaScript[32];
};
//////////////////////////////////////////////////////////////////////////
//¤u·|Є«npc ґАҐNіBІz
struct GuildHouseNpcReplaceTableStructOld
{
	int Type;
	int OrgNpcID;
	int NpcID;
};
// TODO:: NEW FIELD
struct GuildHouseNpcReplaceTableStruct
{
	int GUID;
	int Type;
	int OrgNpcID;
	int NpcID;
};

//¤u·|Є«npc ґАҐNіBІz»щїъ
struct GuildHouseNpcReplaceCostTableStructOld
{
	int Type;
	int GuildLv;
	GuildResourceStruct	Cost;
};
// TODO:: NEW FIELD
struct GuildHouseNpcReplaceCostTableStruct
{
	int GUID;
	int Type;
	int GuildLv;
	GuildResourceStruct	Cost;
};

//////////////////////////////////////////////////////////////////////////
//ДЭ©КЅдіХЄн
struct GambleItemStateTableStructOld
{
	int	GambleObjID;
	int FirstTimeCost;
	int ContinueCost;
	int MaxLv;
	int MinLv;
};
// TODO:: NEW FIELD
struct GambleItemStateTableStruct
{
	int GUID;
	int	GambleObjID;
	int FirstTimeCost;
	int ContinueCost;
	int MaxLv;
	int MinLv;
};
//////////////////////////////////////////////////////////////////////////
// ¤k№ІЁЖҐу
//////////////////////////////////////////////////////////////////////////
enum ServantCondition_Servant_ENUM
{
	EM_ServantCondition_Servant_Boy,
	EM_ServantCondition_Servant_Girl,
	EM_ServantCondition_Servant_Monster,
	EM_ServantCondition_Servant_All,
};


enum ServantCondition_Master_ENUM
{
	EM_ServantCondition_Master_MasterBoy,	//ЁkҐD¤H
	EM_ServantCondition_Master_MasterGirl,	//¤kҐD¤H
	EM_ServantCondition_Master_Boy,	//Ёk«И¤H
	EM_ServantCondition_Master_Girl,	//¤k«И¤H
	EM_ServantCondition_Master_Master,	//ҐD¤H
	EM_ServantCondition_Master_Visitor,	//«И¤H
	EM_ServantCondition_Master_All,	//ҐюіЎ
};


//¤k№ІЁЖҐуЄн
struct ServantEventTableStructOld
{
	int	GUID;					//ЁЖҐуё№ЅX
	int	EventType[2];			//ЁЖҐуГю«¬	
	union
	{
		int	Condition[3];			//±шҐу
		struct
		{
			ServantCondition_Servant_ENUM	Servant;
			ServantCondition_Master_ENUM	Master;
			int		Character;					//­У©К
		}Check;
	};

	int Rate;					//µoҐНѕчІv
	int MinValue[8];			//іМ§CјЖ­И»ЭЁD
	int	MaxValue[8];			//іМ°ЄјЖ­И»ЭЁD
	int MotionID;				//°К§@
	char TalkStr[128];			//»ЎёЬ
	char Voice[128];			//Бn­µАЙ
	char LuaScript[128];		//Server Lua ј@±Ў
	char LuaCheckScript[128];	//Server Lua АЛ¬d ¬O§_ҐiҐH°х¦ж
};
// TODO:: NEW FIELD
struct ServantEventTableStruct
{
	int GUID_NEW;
	int	GUID;					//ЁЖҐуё№ЅX
	int	EventType[2];			//ЁЖҐуГю«¬	
	union
	{
		int	Condition[3];			//±шҐу
		struct
		{
			ServantCondition_Servant_ENUM	Servant;
			ServantCondition_Master_ENUM	Master;
			int		Character;					//­У©К
		}Check;
	};

	int Rate;					//µoҐНѕчІv
	int MinValue[8];			//іМ§CјЖ­И»ЭЁD
	int	MaxValue[8];			//іМ°ЄјЖ­И»ЭЁD
	int MotionID;				//°К§@
	char TalkStr[128];			//»ЎёЬ
	char Voice[128];			//Бn­µАЙ
	char LuaScript[128];		//Server Lua ј@±Ў
	char LuaCheckScript[128];	//Server Lua АЛ¬d ¬O§_ҐiҐH°х¦ж
};
//////////////////////////////////////////////////////////////////////////
//¶±ҐО¤k№І
struct ServantHireTableStructOld
{
	int NPCObjID;
	int FaceID;
	int HairID;
	int SkinColor;
	int HairColor;
	int	ClothColor1;
	int ClothColor2;
	int Character;
};
// TODO:: NEW FIELD
struct ServantHireTableStruct
{
	int GUID;
	int NPCObjID;
	int FaceID;
	int HairID;
	int SkinColor;
	int HairColor;
	int	ClothColor1;
	int ClothColor2;
	int Character;
};
//////////////////////////////////////////////////////////////////////////
struct ServantStartValueTableStructOld
{
	int NPCObjID;
	int StartValue[8];
	int MaxValue[8];
	int Sex;
	int LookNpcObjID;
	int Cost;
};
// TODO:: NEW FIELD
struct ServantStartValueTableStruct
{
	int GUID;
	int NPCObjID;
	int StartValue[8];
	int MaxValue[8];
	int Sex;
	int LookNpcObjID;
	int Cost;
};
//////////////////////////////////////////////////////////////////////////
struct ItemCombineStructOld
{
	int ProductID;
	int	ProductCount;
	int SourceID[5];
	int	SourceCount[5];

	void Init()
	{
		memset(this, 0, sizeof(*this));
	};
};
// TODO:: NEW FIELD
struct ItemCombineStruct
{
	int GUID;
	int ProductID;
	int	ProductCount;
	int SourceID[5];
	int	SourceCount[5];

	void Init()
	{
		memset(this, 0, sizeof(*this));
	};
};
//////////////////////////////////////////////////////////////////////////
//·Т¤Ж µ}¦і«Ч¤W¤Й§Iґ«Є««~Єн
struct LifeSkillRareExStructOld
{
	int ItemID;
	int	RareItemID[3];
};
// TODO:: NEW FIELD
struct LifeSkillRareExStruct
{
	int GUID;
	int ItemID;
	int	RareItemID[3];
};
//////////////////////////////////////////////////////////////////////////
//АJ¤еЁtІО¦@»пІХі]©wЄн
struct RuneSystemStruct
{
	int GUID;
	int MagicCol;		//¦@»п®ДЄG
	int MagicColEx;		//±j¤Ж¦@»п®ДЄG
	int TriggerCount;
	int RuneColor[6];
};
// TODO:: NEW
struct BaseMagicListTableStruct
{
	int GUID;
	int BaseMagicList[EM_BaseMagic_MaxCount];
};
struct JobWearSkillTableStructOld
{
	EqWearSkillStruct JobWearSkill[_MAX_VOCATION_COUNT_];
};
// TODO:: NEW FIELD
struct JobWearSkillTableStruct
{
	int GUID;
	EqWearSkillStruct JobWearSkill[_MAX_VOCATION_COUNT_];
};
// TODO:: NEW
struct SkillValueBaseTableStruct
{
	int GUID;
	int SkillValueBase[_MAX_SKILLVALUE_BASETYPE];
};
// TODO:: NEW
struct JobSkillValueTableStruct
{
	int GUID;
	int JobSkillValue[_MAX_VOCATION_COUNT_];
};
// TODO:: NEW
struct WeaponBaseTableStruct
{
	int GUID;
	int WeaponBaseDelayTime[_MAX_SKILLVALUE_TYPE];
	float WeaponBaseDmg[_MAX_SKILLVALUE_TYPE];
	int WeaponBaseRand[_MAX_SKILLVALUE_TYPE];
	float WeaponBaseMAtk[_MAX_SKILLVALUE_TYPE];
	float WeaponBaseCostRate[_MAX_SKILLVALUE_TYPE];
};
// TODO:: NEW
struct ArmorArgStruct
{
	int GUID;
	float ArmorArg[3][EM_ArmorPos_MaxCount][EM_Armor_MaxCount];
};
//////////////////////////////////////////////////////////////////////////
struct LoadTableStruct
{
	int GUID;
	union
	{
		char					Data[1];
		FixFightArgumentStruct  FixTB; // +	
		FightArgumentStruct     ArgTB; // +	
		//int     ExpTB[_MAX_VOCATION_COUNT_];
		SkillValueBaseTableStruct SkillValueBase; // +
		JobSkillValueTableStruct JobSkillValue; // +
		JobWearSkillTableStruct JobWearSkill; // +
		WeaponBaseTableStruct WeaponBase; // +
		BaseMagicListTableStruct BaseMagicList; // +
		//ResistArgumentStruct	ResistArg;				
		//int		RefineExp[_MAX_SKILL_LEVE_];					
		LvExpRelationTable	AllExp; // +						
		CreateRoleInfoTableStruct CreateRole; // +
		ComboTable			Combo; // +
		MotionTableStruct	Motion; // +
		TeachTableStruct	Teach; // +
		ColoringShopStruct	ColoringShop; // +
		WorldMapInfoStruct	WorldMapInfo; // +
		GuildLvInfoStruct	GuildLvInfo; // +
		//AccountShopInfoStruct	AccountShopInfo;
		SysKeyFunctionStruct SysKeyFunction; // +
		LotteryInfoTableStruct	LotteryInfoTable; // +
		SysKeyValueTableStruct SysKeyValue; // +
		InherentRateTableStruct	InheretRate; // +
		HouseShopInfoStruct HouseShopInfo; // +
		HouseEnergyCostStruct HouseEnergyCost;
		PowerRateTableStruct PowerRate; // +
		RentFunctionTableStruct	RentFunction; // +
		GuildFlagTableStruct	GuildFlagTable; // +
		MapPointStruct			MapPoint; // +
		RelationMagicStruct		RelationMagic; // +
		RuneExchangeExTableStruct RuneExchangeExTable; // +
		GuildBuildingInfoTableStruct	GuildBuilding; // +
		GuildBaseBuildingTableStruct	GuildBaseBuilding; // +
		GuildHouseItemPageCostTableStruct	GuildHouseItemPageCost; // +
		CultivatePetLearnMagicTableStruct	CultivatePetLearnMagic; // +
		CultivatePetLifeSkillTableStruct	CultivatePetLifeSkill; // +
		PlantProductTableStruct PlantProduct; // +
		GambleItemStateTableStruct GambleItemState; // +
		ServantEventTableStruct	ServantEvent; // +
		ServantHireTableStruct ServantHire; // +
		ServantStartValueTableStruct ServantStartValue; // +
		ItemCombineStruct		ItemCombine; // +
		LifeSkillRareExStruct	LifeSkillRareEx; // +
		GuildHouseNpcReplaceTableStruct	GuildHouseNpcReplace; // +
		GuildHouseNpcReplaceCostTableStruct	GuildHouseNpcReplaceCost; // +
		//RuneSystemStruct			RuneSystem;
		PetCollectionTableStruct	PetCard; // +
		AbiParametersTableStruct	PhantomAbi; // +
		PhantomRankTableStruct		PhantomRank; // +
		PhantomLvTableStruct		PhantomLv; // +
		PhantomSkillTableStruct		PhantomSkill; // +
	};

};
#endif
