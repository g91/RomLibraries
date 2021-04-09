#include "ObjectStructEx.h"
#include "ReaderClass/CreateDBCmdClass.h"
#ifdef _CLIENT
#include "RoleData/ObjectDataClass.h"
#endif
string GameObjDbStructEx::m_tempString;



char* WearEqTypeENUMString[] = {
	"no effect",
	"Durability",
	"Attribute STR Power",
	"Attribute STA Stamina",
	"Attribute INT Intelligence",
	"Attribute MND Spiritual Power",
	"Attribute AGI Agility",
	"Attributes all (basic five attributes)",
	"Max HP",
	"Maximum MP",
	"Response Rate HP",
	"Response Rate MP",
	"ATK Attack Power",
	"DEF Defense",
	"MDEF Magic Defense",
	"MATK Magic Attack",

	"Physical Hit",
	"Physical Avoidance",
	"Critical Strike Rating",
	"Critical Strike Power Increase (%)",
	"Spell Crit Level",
	"Spell crit power increase (%)",
	"Block level",
	"attack speed(%)",
	"Moving speed(%)",
	"Weapon Power All Melee or Long Range (DMG)",
	"Resistance to all",
	"Resistant Land",
	"Resistant to Water",
	"Resistance to Fire",
	"Resistant Wind",
	"Resistance to Light",
	"Resistance to Darkness",
	"MP consumption reduction (%)",
	"Experience",
	"Drop rate",
	"Deputy Hand Hit Rate",
	"Deputy Hand Damage (DMG%)",
	"Defensive Strengthening Armor (%)",
	"Defensive Strengthened Mail (%)",
	"Defensive Strengthening Leather Armor (%)",
	"Defensive Strengthening Clothes (%)",
	"Defense Strengthening Robe (%)",
	"Defensive Strengthening Shield (%)",
	"Magic Power All (%)",
	"Magic Power (%)",
	"Magic Power Water (%)",
	"Magic Power Fire (%)",
	"Magic Power Wind (%)",
	"Magic Power Light (%)",
	"Magic Power Dark (%)",
	"Cast Speed All (%)",
	"Ranged weapon power (%) all",
	"Ranged Weapon Power (%) Bow",
	"Ranged Weapon Power (%) Crossbow",
	"Ranged weapon power (%) gun",
	"Melee weapon power (%) all",
	"Melee Weapon Power (%) Empty Hand",
	"Melee weapon power (%) sword",
	"Melee Weapon Power (%) Dagger",
	"Melee Weapon Power (%) Scepter",
	"Melee Weapon Power (%) Axe",
	"Melee Weapon Power (%) Hammer and Stick",
	"Melee weapon power (%) two-handed sword",
	"Melee Weapon Power (%) Rod",
	"Melee weapon power (%) two-handed axe",
	"Melee weapon power (%) two-handed hammer",
	"Melee weapon power (%) gun (spear)",
	"Ranged weapon speed (%) all",
	"Ranged weapon speed (%) bow",
	"Ranged weapon speed (%) crossbow",
	"Ranged weapon speed (%) gun",
	"Melee weapon speed (%) all",
	"Melee weapon speed (%) empty hand",
	"Melee weapon speed (%) sword",
	"Melee weapon speed (%) dagger",
	"Melee Weapon Speed (%) Scepter",
	"Melee weapon speed (%) axe",
	"Melee weapon speed (%) hammer and stick",
	"Melee weapon speed (%) two-handed sword",
	"Melee weapon speed (%) rod",
	"Melee weapon speed (%) two-handed axe",
	"Melee weapon speed (%) Two-handed hammer",
	"Melee weapon speed (%) gun (spear)",
	"Equipment Ability Empty-handed",
	"Equipment Ability Sword",
	"Equipment Ability Dagger",
	"Equipment Ability Scepter",
	"Equipment Ability Axe",
	"Equipment Ability Hammer and Stick",
	"Equipment Ability Two-handed Sword",
	"Equipment Ability Rod",
	"Equipment Ability Two-handed Axe",
	"Equipment Ability Two-handed Hammer",
	"Equipment ability gun (spear)",
	"Equipment Ability Bow",
	"Equipment Ability Crossbow",
	"Equipment Ability Gun",
	"Equipment Ability Throwing",
	"Equipment Ability Armor",
	"Equipment Ability Lock Armor",
	"Equipment Ability Leather Armor",
	"Equipment Capability Clothes",
	"Equipment Ability Robe",
	"Equipment Ability Shield",
	"Equipment Ability Handheld Magical Equipment",
	"Equipment Ability Dual Handheld",
	"Skill Up Unarmed Empty Hand",
	"Skill Up Sword",
	"Skill Up Dagger",
	"Skill Up Scepter",
	"Skill Up Axe",
	"Skills Up Hammer and Stick",
	"Skills up two-handed sword",
	"Skill Up Rod",
	"Skills up two-handed axe",
	"Skills up two-handed hammer",

	"Skills up gun (spear)",
	"Skill Up, Ranged Weapon, Bow",
	"Skill Up, Ranged Weapon, Crossbow",
	"Skills up, long-range weapons, guns",
	"Skills up defense",
	"Skills rise, strike iron",
	"Skills up woodworking",
	"Skills up, armor making",
	"Skills Up Tailor",
	"Skills up cooking",
	"Skill Up Alchemy",
	"The probability of HQ products rises to strike iron (%)",
	"The probability of HQ products rises and rises. Woodworking (%)",
	"The chance of HQ products rises and armor (%)",
	"The probability of HQ products rises and rises tailoring (%)",
	"The probability of HQ products rises and rises cooking (%)",
	"The chance of HQ products rises and rises Alchemy (%)",
	"ATK Attack Power (%)",
	"DEF Defense (%)",
	"Theft success rate (%)",
	"Money drop rate (%)",
	"Hate degree increased (%)",
	"Warrior SP reply plus percentage",
	"Ranger SP reply plus percentage",
	"Rogue SP response plus percentage",
	"Magic Absorption Rate",
	"Defense Absorption Rate",
	"Therapeutic Absorption Rate",
	"Magic Absorption Points",
	"Defensive Absorption Points",
	"Healing Absorption Points",
	"Magic damage plus points",
	"Healing Power Increase Rate",
	"Healing power increases points",
	"Long-range weapon attack range increased",

	"Skills Up Raft",
	"Skills up, picking herbs",
	"Skills Up Mining",
	"Skills Up Fishing",
	"The chance of HQ goods increases. Raft wood",
	"The probability of HQ products increases, picking herbs",
	"The probability of HQ products increases mining",
	"The chance of HQ products increases fishing",
	"Collection Success Rate",

	"Attribute STR Strength (%)",
	"Attribute STA Stamina (%)",
	"Attribute INT Intelligence (%)",
	"Attribute MND Spirit Power (%)",
	"Attribute AGI Agility (%)",
	"Attribute all (%) (basic five attributes)",
	"Upper Limit HP(%)",
	"Maximum MP(%)",
	"Riding Speed(%)",
	"MDEF(%) Magic Defense",
	"MATK(%) Magic Attack",
	"Shield Block Level",
	"Weapon Power All melee or ranged (DMG%)",
	"Deputy position experience value",
	"TP Experience Points",

	"Raft success rate",
	"Pharmaceutical collection success rate",
	"Mining Success Rate",
	"Fishing success rate",
	"Gravity Correction",

	"Two Attacks",
	"Anti-riot and other attacks",
	"Spell Riot Level",
	"Land Attack",
	"Water attribute attack",
	"Fire Attack",
	"Wind attribute attack",
	"Light attribute attack",
	"Dark Attribute Attack",

	"Jump ratio",
	"Magic Death",
	"Magic Death Ratio",

	"Piercing Shield File",
	"Piercing Block",

	"Magic Hit",
	"Magic Dodge",

	"Physical Hit Rate",
	"Physical Evasion Rate",
	"Magic Hit Rate",
	"Magic Dodge Rate",

	"Increase rate of collection output",
	"Collection Speed Increase Rate",
	"Gathering experience value increase rate",

	"(Attack) Attacking player (or player pet) power addition and subtraction",
	"(Attack) Attack NPC power addition and subtraction",
	"(Shou) is attacked by the player (or player pet) plus or minus",
	"(Shou) is increased or decreased by NPC attack power",
	"(Attack) AE spell attack power addition and subtraction",
	"(Shou) is increased or decreased by AC spell attack power",

	"Production speed increase rate",
	"Production experience value increase rate",

	"Increase the rate of experience given by the plot",
	"Increase the TP rate given by the plot",

	"Battle intent consumption reduction (%)",
	"Energy consumption reduction (%)",
	"Concentration consumption reduction (%)",
	"NPC experience value correction (%)",
	"NPC TP experience value correction (%)",
	"Increase the attack range of short-range weapons",
	"Resistance all%",
	"Increased damage from earth spells by %",
	"Increased damage from water spells by %",
	"Increased damage from fire spells by %",
	"Increased damage from wind spells by %",
	"Increased damage from light spells by %",
	"Increased damage from dark spells by %",
	"Block %",
	"Daily mission experience value multiplication",
	"Daily Mission TP Multiplication"
};

char* WeaponTypeENUMString[] = {
	"claw",
	"Sword",
	"Dagger",
	"Scepter",
	"Axe",
	"Hammer and Stick",
	"Two-handed sword",
	"Stick",
	"Two-handed axe",
	"Two-handed hammer",
	"Lance",
	"Bow",
	"crossbow",
	"Gun",
	"Arrow",
	//"Arrow",
	"Bullet",
	"Throw",
	"Logging",
	"Collecting Medicine",
	"Mining",
	"Fishing"
};

char* WeaponPosENUMString[] = {
	"Master hand",
	"Deputy",
	"one-hand",
	"Hands",
	"Bullet",
	"Bow",
	"Collect"
};

char* ArmorTypeENUMString[] = {
	"armor",
	"Lock Armor",
	"Leather Armor",
	"Commercial",
	"Robe",
	"shield",
	"Dharma Device",
};

char* ArmorPosENUMString[] = {
	"Head Load",
	"gloves",
	"Foot",
	"Upper Body",
	"Lower Body",
	"Back",
	"Belt",
	"Pauldrons",
	"Necklace",
	"Ring",
	"Earrings",
	"Deputy",
	"Magic Weapons",
	"Decorations"
};

char* ItemTypeENUMString[] = {
	"general",
	"Story Items",
	"Food",
	"Sweets",
	"Potion",
	"Ore",
	"Wood",
	"Herb",
	"Raw Materials",
	"Seed",
	"flower pot",
	"Crop",
	"Fish",
	"Collection Tool",
	"Processed Goods",
	"Recipe for Strike Iron",
	"Recipe Woodworking",
	"Formula Armor",
	"Recipe Tailor",
	"Recipe Alchemy",
	"Recipe Cooking",
	"Chong waiting for gems",
	"Effect Light Ball",
	"Money",
	"Runes",
	"Location History",
};

char* MagicCheckFunctionENUMString[] = {
	"None",
	"(r,) r always represents probability",
	"If you have this armor (r, number)",
	"If the target has this armor (r, number)",
	"If you have such weapons (r, type number)",
	"If the target has such weapons (r, type number)",
	"If you have (r, items or important items)",
	"If the target has (r, item or important item)",
	"If you own a buff(r, number)",
	"If the target has a certain buff(r, number)",
	"No effect has been removed",
	"No effect has been removed",
	"Own state (r, 0 non-combat/1 combat)",
	"Target status (r, 0 non-combat/1 combat)",
	"The target race is (r, race number)",
	"Own HP is lower than (r, percentage)",
	"No effect has been removed",
	"No effect has been removed",
	"I have the buff of a certain group (r, group number)",
	"The target has a group's buff (r, group number)",
	"I have learned a certain skill (r, spell set)",
	"If the level exceeds (r, level)"
};



char* GameObjDbStructEx::GetName()
{
#ifdef _CLIENT
	if (IsRecipe() != false)
	{
		char    szReplaceString[512];
		char    szProductKey[512];
		sprintf(szProductKey, "Sys%d_name", Recipe.Item1_Normal.OrgObjID);
		sprintf(szReplaceString, "%s%s", g_ObjectData->GetString("SYS_RECIPE_TITLE"), g_ObjectData->GetString(szProductKey));
		m_tempString = szReplaceString;
		return (char*)m_tempString.c_str();
	}
	else
		if (IsCard() != false)
		{
			char    szReplaceString[512];
			char    szProductKey[512];
			sprintf(szProductKey, "Sys%d_name", Item.CardOrgObjID);
			sprintf(szReplaceString, "%s%s", g_ObjectData->GetString("SYS_CARD_TITLE"), g_ObjectData->GetString(szProductKey));
			m_tempString = szReplaceString;
			return (char*)m_tempString.c_str();
		}
#endif
	return Name;
}
char* GameObjDbStructEx::GetPluralName()
{
	return Name_Plural;
}
bool GameObjDbStructEx::IsEmpty()
{
	return (this == NULL);
}
bool GameObjDbStructEx::IsShop()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_Shop);
}
bool GameObjDbStructEx::IsQuestNPC()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_QuestNPC);
}
bool GameObjDbStructEx::IsQuestDetail()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_QuestDetail);
}
bool GameObjDbStructEx::IsNPC()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_NPC || Classification == EM_ObjectClass_QuestNPC);
}
bool GameObjDbStructEx::IsItem()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_Item);
}
bool GameObjDbStructEx::IsItem_Pure()
{
	if (IsEmpty())
		return false;
	if (IsItem() && Item.Class == EM_Item_Normal)
		return true;
	return false;
}
bool GameObjDbStructEx::IsWeapon()
{
	if (IsEmpty())
		return false;
	if (IsItem() && Item.Class == EM_Item_Weapon)
		return true;
	return false;
}
bool GameObjDbStructEx::IsArmor()
{
	if (IsEmpty())
		return false;
	if (IsItem() && Item.Class == EM_Item_Armor)
		return true;
	return false;
}
bool GameObjDbStructEx::IsMagicBase()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_MagicBase);
}
bool GameObjDbStructEx::IsMagicCollect()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_MagicCollect);
}
bool GameObjDbStructEx::IsSuit()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_Suit);
}
bool GameObjDbStructEx::IsKeyItem()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_KeyItem);
}
bool GameObjDbStructEx::IsMine()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_Mine);
}
bool GameObjDbStructEx::IsAttribute()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_Attribute);
}
bool GameObjDbStructEx::IsRune()
{
	if (IsEmpty())
		return false;
	return (IsItem() && Item.Class == EM_Item_Rune);
}
bool GameObjDbStructEx::IsQuest()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_QuestDetail);
}
bool GameObjDbStructEx::IsFlagObj()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_Flag);
}
bool GameObjDbStructEx::IsEquip()
{
	if (IsArmor() || IsWeapon())
		return true;
	return false;
}
bool GameObjDbStructEx::IsBody()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_BodyObj);
}
bool GameObjDbStructEx::IsImage()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_Image);
}
bool GameObjDbStructEx::IsLearnMagic()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_LearnMagic);
}
bool GameObjDbStructEx::IsPlayer()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_Player);
}
bool GameObjDbStructEx::IsTitle()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_Title);
}
bool GameObjDbStructEx::IsRecipe()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_Recipe);
}
bool GameObjDbStructEx::IsCamp()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_Camp);
}
bool GameObjDbStructEx::IsEqRefineAbility()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_EqRefineAbility);
}
bool GameObjDbStructEx::IsTreasure()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_Treasure);
}
bool GameObjDbStructEx::IsZone()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_Zone);
}
bool GameObjDbStructEx::IsCreateRole()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_CreateRole);
}
bool GameObjDbStructEx::IsCard()
{
	if (IsEmpty())
		return false;
	if (IsItem() && Item.Class == EM_Item_Card)
		return true;
	return false;
}
bool GameObjDbStructEx::IsPhantom()
{
	if (IsEmpty())
		return false;
	return (Classification == EM_ObjectClass_Phantom);
}
bool GameObjDbStructEx::IsRestore()
{
	return false;
}
bool GameObjDbStructEx::IsSpecial()
{
	return false;
}
bool GameObjDbStructEx::IsSeed()
{
	if (IsEmpty() || !IsItem_Pure() || Item.Plant.Seed.Type == EM_SeedType_None)
		return false;
	return true;
}
bool GameObjDbStructEx::IsPot()
{
	if (IsEmpty() || !IsItem_Pure() || Item.Plant.Pot.Mode == 0)
		return false;
	return true;
}
bool GameObjDbStructEx::IsManure()
{
	if (IsEmpty() || !IsItem_Pure() || Item.Plant.Manure.Type == EM_ManureType_None)
		return false;
	return true;
}
bool GameObjDbStructEx::IsAssistMagic()
{
	if (IsMagicBase() == false)
		return false;
	if (MagicBase.MagicFunc != EM_MAGICFUN_Assist)
		return false;
	return true;
}
ObjectModeStruct  GameObjDbStructEx::GetModeFlag()
{
	ObjectModeStruct Mode;
	if (IsEmpty())
		return Mode;
	Mode._Mode = 0;
	if (IsNPC())
	{
		Mode.IsAllZoneVisible = NPC.IsAllZoneVisible;
	}
	switch (Classification)
	{
	case EM_ObjectClass_QuestNPC:
	{
		Mode.Show = true;
		Mode.Mark = true;
		Mode.Obstruct = false;
		Mode.Gravity = true;
		Mode.Save = false;
		Mode.Drag = true;
		Mode.Strikback = false;
		Mode.Move = true;
		Mode.Searchenemy = false;
		Mode.Revive = true;
		Mode.Fight = false;
		if (NPC.iQuestMode == 1)
		{
			Mode.HideName = true;
			Mode.ShowRoleHead = false;
		}
		else
		{
			Mode.HideName = false;
			Mode.ShowRoleHead = true;
		}
		Mode.IsAllZoneVisible = NPC.IsAllZoneVisible;
		Mode.DisableRotate = (NPC.AutoRotate == false);
	} break;
	case EM_ObjectClass_NPC:
	{
		Mode.Show = true;
		Mode.Mark = true;
		Mode.Obstruct = false;
		Mode.Gravity = true;
		Mode.Save = false;
		Mode.Drag = true;
		Mode.Strikback = true;
		Mode.Move = true;
		Mode.Searchenemy = true;
		Mode.Revive = true;
		Mode.Fight = true;
		Mode.ShowRoleHead = true;
		Mode.IsAllZoneVisible = NPC.IsAllZoneVisible;
		Mode.DisableRotate = (NPC.AutoRotate == false);
	} break;
	case EM_ObjectClass_Item:
	{
		Mode.Show = true;
		Mode.Mark = true;
		Mode.Obstruct = false;
		Mode.Gravity = true;
		Mode.Save = false;
		Mode.Drag = true;
		Mode.Strikback = false;
		Mode.Move = false;
		Mode.Searchenemy = false;
		Mode.Revive = false;
		Mode.Fight = false;
		Mode.ShowRoleHead = false;
	} break;
	default:
	{
		Mode.Show = true;
		Mode.Mark = true;
		Mode.Obstruct = false;
		Mode.Gravity = true;
		Mode.Save = false;
		Mode.Drag = true;
		Mode.Strikback = false;
		Mode.Move = false;
		Mode.Searchenemy = false;
		Mode.Revive = false;
		Mode.Fight = false;
		Mode.ShowRoleHead = false;
	} break;
	};
	return Mode;
}
char* GameObjDbStructEx::GetWearEqTypeENUMString(WearEqTypeENUM ID)
{
	if (ID >= EM_WearEqType_MaxCount || ID < 0)
		return "";
	return WearEqTypeENUMString[ID];
}
char* GameObjDbStructEx::GetMagicCheckFunctionENUMString(MagicCheckFunctionENUM ID)
{
	if (ID >= EM_MagicCheck_MaxCount || ID < 0)
		return "";
	return MagicCheckFunctionENUMString[ID];
}
char* GameObjDbStructEx::GetWeaponTypeENUMString(GameObjectWeaponENUM Type)
{
	if (Type >= EM_Weapon_Unarmed && Type <= EM_Weapon_Fishing)
		return WeaponTypeENUMString[Type];
	return "";
}
char* GameObjDbStructEx::GetWeaponPosENUMString(GameObjectWeaponPosENUM Pos)
{
	if (Pos >= EM_WeaponPos_MainHand && Pos <= EM_WeaponPos_Manufacture)
		return WeaponPosENUMString[Pos];
	return "";
}
char* GameObjDbStructEx::GetArmorTypeENUMString(GameObjectArmorENUM Type)
{
	if (Type >= EM_Armor_HeavyArmed && Type <= EM_Armor_Implement)
		return ArmorTypeENUMString[Type];
	return "";
}
char* GameObjDbStructEx::GetArmorPosENUMString(GameObjectArmorPosENUM Pos)
{
	if (Pos >= EM_ArmorPos_Head && Pos <= EM_ArmorPos_Ornament)
		return ArmorPosENUMString[Pos];
	return "";
}
char* GameObjDbStructEx::GetItemTypeENUMString(ItemType_ENUM Type)
{
	if (Type >= EM_ItemType_Normal && Type <= EM_ItemType_Money)
		return ItemTypeENUMString[Type];
	return "";
}
int GameObjDbStructEx::GetWearEqValue(GameObjectWearEqStruct& Ability, WearEqTypeENUM Type, int begin)
{
	int value = 0;
	for (int i = begin; i < Max_WearEqType_Count; i++)
	{
		if (Ability.EqType[i] == Type)
			value += Ability.EqTypeValue[i];
	}
	return value;
}
int GameObjDbStructEx::GetandClearWearEqValue(GameObjectWearEqStruct& Ability, WearEqTypeENUM Type)
{
	int value = 0;
	for (int i = 0; i < Max_WearEqType_Count; i++)
	{
		if (Ability.EqType[i] == Type)
		{
			value += Ability.EqTypeValue[i];
			Ability.EqType[i] = EM_WearEqType_None;
			Ability.EqTypeValue[i] = 0;
		}
	}
	return value;
}
int GameObjDbStructEx::GetWeaponDMG(GameObjectWearEqStruct& Ability)
{
	return GetWearEqValue(Ability, EM_WearEqType_DMG);
}
int GameObjDbStructEx::GetArmorDEF(GameObjectWearEqStruct& Ability)
{
	return GetWearEqValue(Ability, EM_WearEqType_DEF);
}
int GameObjDbStructEx::GetWeaponAtkSpeed(GameObjectWearEqStruct& Ability)
{
	return GetWearEqValue(Ability, EM_WearEqType_AtkSpeed);
}
int GameObjDbStructEx::GetWearEqDurable(GameObjectWearEqStruct& Ability)
{
	return GetWearEqValue(Ability, EM_WearEqType_Durable);
}
int GameObjDbStructEx::GetWeaponMATK(GameObjectWearEqStruct& Ability)
{
	return GetWearEqValue(Ability, EM_WearEqType_MATK);
}
int GameObjDbStructEx::GetArmorMDEF(GameObjectWearEqStruct& Ability)
{
	return GetWearEqValue(Ability, EM_WearEqType_MDEF);
}
bool    GameObjDbStructEx::CheckItemAndEqSkill(EqWearSkillStruct EqSkill)
{
	if (IsWeapon())
	{
		switch (Item.WeaponType)
		{
		case EM_Weapon_None:
			break;
		case EM_Weapon_Unarmed:
			if (EqSkill.Unarmed == false)
				return false;
			break;
		case EM_Weapon_Blade:
			if (EqSkill.Blade == false)
				return false;
			break;
		case EM_Weapon_Dagger:
			if (EqSkill.Dagger == false)
				return false;
			break;
		case EM_Weapon_Wand:
			if (EqSkill.Wand == false)
				return false;
			break;
		case EM_Weapon_Axe:
			if (EqSkill.Axe == false)
				return false;
			break;
		case EM_Weapon_Bludgeon:
			if (EqSkill.Bludgeon == false)
				return false;
			break;
		case EM_Weapon_Claymore:
			if (EqSkill.Claymore == false)
				return false;
			break;
		case EM_Weapon_Staff:
			if (EqSkill.Staff == false)
				return false;
			break;
		case EM_Weapon_2H_Axe:
			if (EqSkill.Axe_2H == false)
				return false;
			break;
		case EM_Weapon_2H_Hammer:
			if (EqSkill.Hammer_2H == false)
				return false;
			break;
		case EM_Weapon_Polearm:
			if (EqSkill.Polearm == false)
				return false;
			break;
		case EM_Weapon_Bow:
		case EM_Weapon_Ammo_Bow:
			if (EqSkill.Bow == false)
				return false;
			break;
		case EM_Weapon_CossBow:
			if (EqSkill.CossBow == false)
				return false;
			break;
		case EM_Weapon_Gun:
		case EM_Weapon_Ammo_Gun:
			if (EqSkill.Gun == false)
				return false;
			break;
		case EM_Weapon_Throw:
			if (EqSkill.Throw == false)
				return false;
			break;
		case EM_Weapon_Lumbering:
		case EM_Weapon_Herbalism:
		case EM_Weapon_Mining:
		case EM_Weapon_Fishing:
			return true;
		default:
			return false;
		}
		return true;
	}
	else if (IsArmor())
	{
		if (Item.ArmorPos == EM_ArmorPos_Necklace
			|| Item.ArmorPos == EM_ArmorPos_Ring
			|| Item.ArmorPos == EM_ArmorPos_Earring
			|| Item.ArmorPos == EM_ArmorPos_Back
			|| Item.ArmorPos == EM_ArmorPos_MagicTool
			|| Item.ArmorPos == EM_ArmorPos_Ornament)
			return true;
		switch (Item.ArmorType)
		{
		case EM_Armor_HeavyArmed:
			if (EqSkill.HeavyArmed == false)
				return false;
			break;
		case EM_Armor_LightArmed:
			if (EqSkill.LightArmed == false)
				return false;
			break;
		case EM_Armor_Leather:
			if (EqSkill.Leather == false)
				return false;
			break;
		case EM_Armor_Clothes:
			if (EqSkill.Clothes == false)
				return false;
			break;
		case EM_Armor_Robe:
			if (EqSkill.Robe == false)
				return false;
			break;
		case EM_Armor_Shield:
			if (EqSkill.Shield == false)
				return false;
			break;
		case EM_Armor_Implement:
			if (EqSkill.Implement == false)
				return false;
			break;
		default:
			return false;
		}
		return true;
	}
	return false;
}
void GameObjDbStructEx::ObjReader_InitBaseCommonObj(DB_GameObjDbStruct*& Pt, ReaderClass< DB_GameObjDbStruct >& Reader)
{
	Reader.SetData("DirZoneID", &Pt->ZoneID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ZoneID));
	Reader.SetData("DirVersionID", &Pt->VersionID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->VersionID));
	Reader.SetData("DirTagID", &Pt->TagID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->TagID));
	Reader.SetData("Translate", &Pt->Translate, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Translate));
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Zone()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char    Buf[256];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("MapFile", &Pt->GameData.Zone.MapFile, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.MapFile));
		Reader.SetData("IsEnabledBlockCtrl", &Pt->GameData.Zone.IsEnabledBlockCtrl, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.IsEnabledBlockCtrl));
		Reader.SetData("CtrlMaxZ", &Pt->GameData.Zone.CtrlMaxZ, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.CtrlMaxZ));
		Reader.SetData("CtrlMaxX", &Pt->GameData.Zone.CtrlMaxX, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.CtrlMaxX));
		Reader.SetData("CtrlMinZ", &Pt->GameData.Zone.CtrlMinZ, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.CtrlMinZ));
		Reader.SetData("CtrlMinX", &Pt->GameData.Zone.CtrlMinX, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.CtrlMinX));
		Reader.SetData("IsPrivateZone", &Pt->GameData.Zone.IsPrivateZone, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.IsPrivateZone));
		Reader.SetData("BaseRoomCount", &Pt->GameData.Zone.BaseRoomCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.BaseRoomCount));
		Reader.SetData("PlayRoomCount", &Pt->GameData.Zone.PlayRoomCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.PlayRoomCount));
		Reader.SetData("PrivateZoneType", &Pt->GameData.Zone.PrivateZoneType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.PrivateZoneType));
		Reader.SetData("PrivateZoneLiveTime", &Pt->GameData.Zone.PrivateZoneLiveTime, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.PrivateZoneLiveTime));
		Reader.SetData("MapID", &Pt->GameData.Zone.MapID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.MapID));
		Reader.SetData("CampStatueObjID", &Pt->GameData.Zone.CampStatueObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.CampStatueObjID));
		Reader.SetData("IsEnabledRevPoint", &Pt->GameData.Zone.IsEnabledRevPoint, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.IsEnabledRevPoint));
		Reader.SetData("RevX", &Pt->GameData.Zone.RevX, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.RevX));
		Reader.SetData("RevY", &Pt->GameData.Zone.RevY, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.RevY));
		Reader.SetData("RevZ", &Pt->GameData.Zone.RevZ, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.RevZ));
		Reader.SetData("RevZoneID", &Pt->GameData.Zone.RevZoneID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.RevZoneID));
		Reader.SetData("ViewBlock", &Pt->GameData.Zone.ViewBlock, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.ViewBlock));
		Reader.SetData("MapCount", &Pt->GameData.Zone.MapCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.MapCount));
		Reader.SetData("LuaInitZone", &Pt->GameData.Zone.LuaInitZone, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.LuaInitZone));
		Reader.SetData("IsEnabledRide", &Pt->GameData.Zone.IsEnabledRide, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.IsEnabledRide));
		Reader.SetData("IsWorldBattleGround", &Pt->GameData.Zone.IsWorldBattleGround, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.IsWorldBattleGround));
		Reader.SetData("iWorldBattleGroundWorldID", &Pt->GameData.Zone.iWorldBattleGroundWorldID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.iWorldBattleGroundWorldID));
		Reader.SetData("IsCountryBattleGround", &Pt->GameData.Zone.IsCountryBattleGround, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.IsCountryBattleGround));
		Reader.SetData("IsAllowTeamJoin", &Pt->GameData.Zone.IsAllowTeamJoin, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.IsAllowTeamJoin));
		Reader.SetData("IsEnableBattleGroundQueue", &Pt->GameData.Zone.IsEnableBattleGroundQueue, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.IsEnableBattleGroundQueue));
		Reader.SetData("IsLimitJoinLevel", &Pt->GameData.Zone.IsLimitJoinLevel, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.IsLimitJoinLevel));
		Reader.SetData("IsForceGroup", &Pt->GameData.Zone.IsForceGroup, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.IsForceGroup));
		Reader.SetData("iMinJoinLevel", &Pt->GameData.Zone.iMinJoinLevel, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.iMinJoinLevel));
		Reader.SetData("iMaxJoinLevel", &Pt->GameData.Zone.iMaxJoinLevel, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.iMaxJoinLevel));
		Reader.SetData("LuaPreJoinQueueCheck", &Pt->GameData.Zone.LuaPreJoinQueueCheck, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.LuaPreJoinQueueCheck));
		Reader.SetData("LuaAssistWorldGroupID", &Pt->GameData.Zone.LuaAssistWorldGroupID, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.LuaAssistWorldGroupID));
		for (int i = 0; i < 40; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "MapID%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Zone.MapInfoList[i].MapID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.MapInfoList[i].MapID));
			sprintf_s(Buf, sizeof(Buf), "MapName%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Zone.MapInfoList[i].MapName, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.MapInfoList[i].MapName));
			sprintf_s(Buf, sizeof(Buf), "IsEabledSave%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Zone.MapInfoList[i].IsEnabledSave, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.MapInfoList[i].IsEnabledSave));
		}
		for (int i = 0; i < 30; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "ZoneIniKey%d", i);
			Reader.SetData(Buf, &Pt->GameData.Zone.ZoneIni[i].Key, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.ZoneIni[i].Key));
			sprintf_s(Buf, sizeof(Buf), "ZoneIniValue%d", i);
			Reader.SetData(Buf, &Pt->GameData.Zone.ZoneIni[i].Value, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.ZoneIni[i].Value));
		}
		for (int i = 0; i < 4; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "iCountryWarLinkMap%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Zone.iCountryWarLinkMap[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.iCountryWarLinkMap[i]));
		}
		Reader.SetData("RoomPlayerCount", &Pt->GameData.Zone.RoomPlayerCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.RoomPlayerCount));
		Reader.SetData("IsAutoJoinEventZone", &Pt->GameData.Zone.IsAutoJoinEventZone, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.IsAutoJoinEventZone));
		Reader.SetData("MirrorInstanceType", &Pt->GameData.Zone.MirrorInstanceType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.MirrorInstanceType));
		Reader.SetData("MirrorLimitLv", &Pt->GameData.Zone.MirrorLimitLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.MirrorLimitLv));
		Reader.SetData("MirrorLimitTime", &Pt->GameData.Zone.MirrorLimitTime, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.MirrorLimitTime));
		Reader.SetData("Mirror_X", &Pt->GameData.Zone.Mirror_X, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.Mirror_X));
		Reader.SetData("Mirror_Y", &Pt->GameData.Zone.Mirror_Y, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.Mirror_Y));
		Reader.SetData("Mirror_Z", &Pt->GameData.Zone.Mirror_Z, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Zone.Mirror_Z));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_EqRefineAbility()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char    Buf[256];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		for (int i = 0; i < Max_WearEqType_Count; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "EqType%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.EqRefineAttribute.Ability.EqType[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.EqRefineAttribute.Ability.EqType[i]));
			sprintf_s(Buf, sizeof(Buf), "EqTypeValue%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.EqRefineAttribute.Ability.EqTypeValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.EqRefineAttribute.Ability.EqTypeValue[i]));
		}
		Reader.SetData("ExEqPowerRate", &Pt->GameData.EqRefineAttribute.ExEqPowerRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.EqRefineAttribute.ExEqPowerRate));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_AddPower()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char    Buf[256];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("Inc_Magic", &Pt->GameData.Attirbute.Inc_Magic, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.Inc_Magic));
		Reader.SetData("Inc_Magic_ActionRate", &Pt->GameData.Attirbute.Inc_Magic_ActionRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.Inc_Magic_ActionRate));
		Reader.SetData("Inc_MagicType", &Pt->GameData.Attirbute.Inc_MagicType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.Inc_MagicType));
		Reader.SetData("AddOnPos_ArmorPos", &Pt->GameData.Attirbute.AddPos._ArmorPos, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.AddPos._ArmorPos));
		Reader.SetData("AddOnPos_ArmorType", &Pt->GameData.Attirbute.AddPos._ArmorType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.AddPos._ArmorType));
		Reader.SetData("AddOnPos_WeaponType", &Pt->GameData.Attirbute.AddPos._WeaponType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.AddPos._WeaponType));
		Reader.SetData("AddOnPos_WeaponPos", &Pt->GameData.Attirbute.AddPos.WeaponPosEnum, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.AddPos.WeaponPosEnum));
		Reader.SetData("AddOnPos_Other", &Pt->GameData.Attirbute.AddPos._Other, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.AddPos._Other));
		Reader.SetData("Type", &Pt->GameData.Attirbute.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.Type));
		for (int i = 0; i < Max_WearEqType_Count; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "EqType%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Attirbute.Ability.EqType[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.Ability.EqType[i]));
			sprintf_s(Buf, sizeof(Buf), "EqTypeValue%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Attirbute.Ability.EqTypeValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.Ability.EqTypeValue[i]));
		}
		Reader.SetData("EqTypeOnAttackMagicID", &Pt->GameData.Attirbute.Ability.OnAttackMagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.Ability.OnAttackMagicID));
		Reader.SetData("EqTypeOnAttackMagicRank", &Pt->GameData.Attirbute.Ability.OnAttackMagicRank, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.Ability.OnAttackMagicRank));
		Reader.SetData("EqTypeOnAttackMagicRate", &Pt->GameData.Attirbute.Ability.OnAttackMagicRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.Ability.OnAttackMagicRate));
		Reader.SetData("IsStandardAbility", &Pt->GameData.Attirbute.IsStandardAbility, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.IsStandardAbility));
		Reader.SetData("StandardAbility_Lv", &Pt->GameData.Attirbute.StandardAbility_Lv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.StandardAbility_Lv));
		for (int i = 0; i < 5; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "StandardAbility_Arg%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Attirbute.StandardAbility_Arg[i], ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.StandardAbility_Arg[i]));
			sprintf_s(Buf, sizeof(Buf), "StandardAbility_ArgType%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Attirbute.StandardAbility_ArgType[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.StandardAbility_ArgType[i]));
		}
		Reader.SetData("StarValue", &Pt->GameData.Attirbute.StarValue, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.StarValue));
		Reader.SetData("InherentValue", &Pt->GameData.Attirbute.InherentValue, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.InherentValue));
		Reader.SetData("IsBirthAbility", &Pt->GameData.Attirbute.IsBirthAbility, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.IsBirthAbility));
		Reader.SetData("Rarity", &Pt->GameData.Attirbute.Rarity, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.Rarity));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_CreateRole()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char    Buf[256];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("Job", &Pt->GameData.CreateRole.Job, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.CreateRole.Job));
		Reader.SetData("BornPosZoneID", &Pt->GameData.CreateRole.BornPos.ZoneID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.CreateRole.BornPos.ZoneID));
		Reader.SetData("BornPosX", &Pt->GameData.CreateRole.BornPos.X, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.CreateRole.BornPos.X));
		Reader.SetData("BornPosY", &Pt->GameData.CreateRole.BornPos.Y, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.CreateRole.BornPos.Y));
		Reader.SetData("BornPosZ", &Pt->GameData.CreateRole.BornPos.Z, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.CreateRole.BornPos.Z));
		Reader.SetData("BornPosDir", &Pt->GameData.CreateRole.BornPos.Dir, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.CreateRole.BornPos.Dir));
		for (int i = 0; i < 18; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "EQ_ID%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.CreateRole.EQ.ID[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.CreateRole.EQ.ID[i]));
		}
		for (int i = 0; i < 18; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "EQPreview_ID%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.CreateRole.EQPreview.ID[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.CreateRole.EQPreview.ID[i]));
		}
		for (int i = 0; i < 10; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "Item_ID%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.CreateRole.ItemID[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.CreateRole.ItemID[i]));
		}
		for (int i = 0; i < 10; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "Item_Count%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.CreateRole.ItemCount[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.CreateRole.ItemCount[i]));
		}
		Reader.SetData("RaceLimit", &Pt->GameData.CreateRole.RaceLimit._Race, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.CreateRole.RaceLimit._Race));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Armor()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char    Buf[256];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("LimitVoc", &Pt->GameData.Limit.Voc._Voc, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.Voc._Voc));
		Reader.SetData("LimitRace", &Pt->GameData.Limit.Race._Race, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.Race._Race));
		Reader.SetData("LimitSex", &Pt->GameData.Limit.Sex._Sex, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.Sex._Sex));
		Reader.SetData("LimitLv", &Pt->GameData.Limit.Level, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.Level));
		Reader.SetData("LimitStr", &Pt->GameData.Limit.STR, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.STR));
		Reader.SetData("LimitSta", &Pt->GameData.Limit.STA, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.STA));
		Reader.SetData("LimitInt", &Pt->GameData.Limit.INT, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.INT));
		Reader.SetData("LimitMnd", &Pt->GameData.Limit.MND, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.MND));
		Reader.SetData("LimitAgi", &Pt->GameData.Limit.AGI, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.AGI));
		Reader.SetData("Mode", &Pt->GameData.Mode._Mode, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mode._Mode));
		Reader.SetData("ItemType", &Pt->GameData.Item.ItemType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ItemType));
		Reader.SetData("GenerateRate", &Pt->GameData.GenerateRate, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.GenerateRate));
		Reader.SetData("MaxWorldCount", &Pt->GameData.MaxWorldCount, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MaxWorldCount));
		Reader.SetData("Rare", &Pt->GameData.Rare, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Rare));
		Reader.SetData("Cost", &Pt->GameData.Cost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Cost));
		Reader.SetData("SpecialMoneyCost", &Pt->GameData.SpecialMoneyCost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.SpecialMoneyCost));
		Reader.SetData("PricesType", &Pt->GameData.PricesType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PricesType));
		Reader.SetData("SellType", &Pt->GameData.SellType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.SellType));
		Reader.SetData("IncMagic_Onuse", &Pt->GameData.Item.IncMagic_Onuse, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.IncMagic_Onuse));
		Reader.SetData("DisappearRate", &Pt->GameData.Item.DisappearRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DisappearRate));
		Reader.SetData("DissolutionRate", &Pt->GameData.Item.DissolutionRate, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DissolutionRate));
		Reader.SetData("SrcRecipe", &Pt->GameData.Item.SrcRecipe, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.SrcRecipe));
		Reader.SetData("Durable", &Pt->GameData.Item.Durable, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Durable));
		Reader.SetData("SuitID", &Pt->GameData.Item.SuitID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.SuitID));
		for (int i = 2; i < Max_WearEqType_Count; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "EqType%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Item.Ability.EqType[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Ability.EqType[i]));
			sprintf_s(Buf, sizeof(Buf), "EqTypeValue%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Item.Ability.EqTypeValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Ability.EqTypeValue[i]));
		}
		Reader.SetData("EqTypeOnAttackMagicID", &Pt->GameData.Item.Ability.OnAttackMagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Ability.OnAttackMagicID));
		Reader.SetData("EqTypeOnAttackMagicRank", &Pt->GameData.Item.Ability.OnAttackMagicRank, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Ability.OnAttackMagicRank));
		Reader.SetData("EqTypeOnAttackMagicRate", &Pt->GameData.Item.Ability.OnAttackMagicRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Ability.OnAttackMagicRate));
		Reader.SetData("ArmorType", &Pt->GameData.Item.ArmorType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ArmorType));
		Reader.SetData("ArmorPos", &Pt->GameData.Item.ArmorPos, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ArmorPos));
		Reader.SetData("HoleBase", &Pt->GameData.Item.HoleBase, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.HoleBase));
		Reader.SetData("HoleRange", &Pt->GameData.Item.HoleRange, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.HoleRange));
		Reader.SetData("MaxAttrCount", &Pt->GameData.Item.MaxAttrCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.MaxAttrCount));
		Reader.SetData("BaseAttrCount", &Pt->GameData.Item.BaseAttrCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.BaseAttrCount));
		Reader.SetData("SelectBaseAttrCount", &Pt->GameData.Item.SelectBaseAttrCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.SelectBaseAttrCount));
		Reader.SetData("UseType", &Pt->GameData.Item.UseType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.UseType));
		Reader.SetData("CliScript", &Pt->GameData.Item.CliScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CliScript));
		Reader.SetData("SrvScript", &Pt->GameData.Item.SrvScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.SrvScript));
		Reader.SetData("CheckUseScript", &Pt->GameData.Item.CheckUseScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CheckUseScript));
		Reader.SetData("MagicToolsScript", &Pt->GameData.Item.MagicToolsScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.MagicToolsScript));
		Reader.SetData("LiveTimeType", &Pt->GameData.LiveTimeType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LiveTimeType));
		Reader.SetData("LiveTime", &Pt->GameData.LiveTime, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LiveTime));
		Reader.SetData("IsStandardCost", &Pt->GameData.Item.IsStandardCost, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.IsStandardCost));
		Reader.SetData("CardID", &Pt->GameData.Item.CardID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CardID));
		Reader.SetData("MagicToolsTime", &Pt->GameData.Item.MagicToolsTime, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.MagicToolsTime));
		Reader.SetData("MagicToolsColdown", &Pt->GameData.Item.MagicToolsColdown, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.MagicToolsColdown));
		Reader.SetData("IsFixDurable", &Pt->GameData.Item.IsFixDurable, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.IsFixDurable));
		Reader.SetData("StarValue", &Pt->GameData.Item.StarValue, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.StarValue));
		Reader.SetData("RuneLimetLv", &Pt->GameData.Item.RuneLimetLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.RuneLimetLv));
		Reader.SetData("MagicToolsTpExpScript", &Pt->GameData.Item.MagicToolsTpExpScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.MagicToolsTpExpScript));
		Reader.SetData("ItemUniqueType", &Pt->GameData.Item.ItemUniqueType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ItemUniqueType));
		for (int i = 0; i < _MAX_DROP_ABILITY_COUNT_; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "DropAbility%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Item.DropItemAbility[i].OrgID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DropItemAbility[i].OrgID));
			sprintf_s(Buf, sizeof(Buf), "DropAbilityRate%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Item.DropItemAbility[i].Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DropItemAbility[i].Rate));
		}
		Reader.SetData("RefineTableID", &Pt->GameData.Item.RefineTableID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.RefineTableID));
		Reader.SetData("RefineTableID1", &Pt->GameData.Item.RefineTableID1, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.RefineTableID1));
		Reader.SetData("DDef", &Pt->GameData.Item.DDef, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DDef));
		Reader.SetData("DMDef", &Pt->GameData.Item.DMDef, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DMDef));
		Reader.SetData("BodyMaxCount", &Pt->GameData.Item.BodyMaxCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.BodyMaxCount));
		Reader.SetData("RefineLv", &Pt->GameData.Item.RefineLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.RefineLv));
		Reader.SetData("AddPowerQuality", &Pt->GameData.Item.AddPowerQuality, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.AddPowerQuality));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Body()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("LimitSetting", &Pt->GameData.BodyObj.Limit.Setting, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.BodyObj.Limit.Setting));
		Reader.SetData("LimitRace", &Pt->GameData.BodyObj.Limit.Race._Race, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.BodyObj.Limit.Race._Race));
		Reader.SetData("LimitSex", &Pt->GameData.BodyObj.Limit.Sex._Sex, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.BodyObj.Limit.Sex._Sex));
		Reader.SetData("ObjType", &Pt->GameData.BodyObj.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.BodyObj.Type));
		Reader.SetData("Priority", &Pt->GameData.BodyObj.Priority, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.BodyObj.Priority));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Flag()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Image()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("ActField", &Pt->GameData.Image.ACTField, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ACTField));
		Reader.SetData("ActWorld", &Pt->GameData.Image.ACTWorld, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ACTWorld));
		Reader.SetData("Sound", &Pt->GameData.Image.Sound, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.Sound));
		Reader.SetData("ModelSize", &Pt->GameData.Image.ModelSize, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ModelSize));
		Reader.SetData("ImageType", &Pt->GameData.Image.ImageType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ImageType));
		Reader.SetData("BoundRadiusX", &Pt->GameData.Image.BoundRadiusX, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.BoundRadiusX));
		Reader.SetData("BoundRadiusY", &Pt->GameData.Image.BoundRadiusY, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.BoundRadiusY));
		Reader.SetData("GatherMotion", &Pt->GameData.Image.GatherMotion, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.GatherMotion));
		Reader.SetData("GatherParticalPoint1", &Pt->GameData.Image.GatherPartical[0].Point, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.GatherPartical[0].Point));
		Reader.SetData("GatherParticalPoint2", &Pt->GameData.Image.GatherPartical[1].Point, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.GatherPartical[1].Point));
		Reader.SetData("GatherParticalPoint3", &Pt->GameData.Image.GatherPartical[2].Point, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.GatherPartical[2].Point));
		Reader.SetData("GatherParticalName1", &Pt->GameData.Image.GatherPartical[0].Name, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.GatherPartical[0].Name));
		Reader.SetData("GatherParticalName2", &Pt->GameData.Image.GatherPartical[1].Name, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.GatherPartical[1].Name));
		Reader.SetData("GatherParticalName3", &Pt->GameData.Image.GatherPartical[2].Name, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.GatherPartical[2].Name));
		Reader.SetData("GatherParticalSound1", &Pt->GameData.Image.GatherPartical[0].IsSoundEnabled, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.GatherPartical[0].IsSoundEnabled));
		Reader.SetData("GatherParticalSound2", &Pt->GameData.Image.GatherPartical[1].IsSoundEnabled, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.GatherPartical[1].IsSoundEnabled));
		Reader.SetData("GatherParticalSound3", &Pt->GameData.Image.GatherPartical[2].IsSoundEnabled, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.GatherPartical[2].IsSoundEnabled));
		Reader.SetData("ShootMotion", &Pt->GameData.Image.ShootMotion, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ShootMotion));
		Reader.SetData("ShootMotionScale", &Pt->GameData.Image.ShootMotionScale, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ShootMotionScale));
		Reader.SetData("ShootACT", &Pt->GameData.Image.ShootACT, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ShootACT));
		Reader.SetData("FlyPoint", &Pt->GameData.Image.FlyBeginPoint, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.FlyBeginPoint));
		Reader.SetData("ShootPoint", &Pt->GameData.Image.ShootPoint, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ShootPoint));
		Reader.SetData("FlyACT", &Pt->GameData.Image.FlyACT, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.FlyACT));
		Reader.SetData("TargetPoint", &Pt->GameData.Image.TargetPoint, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.TargetPoint));
		Reader.SetData("ExplodeACT", &Pt->GameData.Image.ExplodeACT, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ExplodeACT));
		Reader.SetData("ArmorPos", &Pt->GameData.Image.ArmorPos, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ArmorPos));
		Reader.SetData("ObjectType", &Pt->GameData.Image.ObjectType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ObjectType));
		Reader.SetData("ExtenPoint", &Pt->GameData.Image.ExtenPoint, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ExtenPoint));
		Reader.SetData("ExtendACT", &Pt->GameData.Image.ExtendACT, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ExtendACT));
		Reader.SetData("BuffPoint", &Pt->GameData.Image.BuffPoint, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.BuffPoint));
		Reader.SetData("BuffACT", &Pt->GameData.Image.BuffACT, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.BuffACT));
		Reader.SetData("BuffACTEx", &Pt->GameData.Image.BuffACTEx, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.BuffACTEx));
		Reader.SetData("PaperdollHairName", &Pt->GameData.Image.PaperdollHair.Name, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollHair.Name));
		Reader.SetData("PaperdollFaceName", &Pt->GameData.Image.PaperdollFace.Name, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollFace.Name));
		Reader.SetData("PaperdollHeadName", &Pt->GameData.Image.PaperdollHead.Name, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollHead.Name));
		Reader.SetData("PaperdollGlovesName", &Pt->GameData.Image.PaperdollGloves.Name, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollGloves.Name));
		Reader.SetData("PaperdollShoesName", &Pt->GameData.Image.PaperdollShoes.Name, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollShoes.Name));
		Reader.SetData("PaperdollClothesName", &Pt->GameData.Image.PaperdollClothes.Name, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollClothes.Name));
		Reader.SetData("PaperdollPantsName", &Pt->GameData.Image.PaperdollPants.Name, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollPants.Name));
		Reader.SetData("PaperdollBackName", &Pt->GameData.Image.PaperdollBack.Name, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollBack.Name));
		Reader.SetData("PaperdollBeltName", &Pt->GameData.Image.PaperdollBelt.Name, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollBelt.Name));
		Reader.SetData("PaperdollShouderName", &Pt->GameData.Image.PaperdollShouder.Name, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollShouder.Name));
		Reader.SetData("PaperdollHairMainColor", &Pt->GameData.Image.PaperdollHair.MainColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollHair.MainColor.Color));
		Reader.SetData("PaperdollFaceMainColor", &Pt->GameData.Image.PaperdollFace.MainColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollFace.MainColor.Color));
		Reader.SetData("PaperdollHeadMainColor", &Pt->GameData.Image.PaperdollHead.MainColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollHead.MainColor.Color));
		Reader.SetData("PaperdollGlovesMainColor", &Pt->GameData.Image.PaperdollGloves.MainColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollGloves.MainColor.Color));
		Reader.SetData("PaperdollShoesMainColor", &Pt->GameData.Image.PaperdollShoes.MainColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollShoes.MainColor.Color));
		Reader.SetData("PaperdollClothesMainColor", &Pt->GameData.Image.PaperdollClothes.MainColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollClothes.MainColor.Color));
		Reader.SetData("PaperdollPantsMainColor", &Pt->GameData.Image.PaperdollPants.MainColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollPants.MainColor.Color));
		Reader.SetData("PaperdollBackMainColor", &Pt->GameData.Image.PaperdollBack.MainColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollBack.MainColor.Color));
		Reader.SetData("PaperdollBeltMainColor", &Pt->GameData.Image.PaperdollBelt.MainColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollBelt.MainColor.Color));
		Reader.SetData("PaperdollShouderMainColor", &Pt->GameData.Image.PaperdollShouder.MainColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollShouder.MainColor.Color));
		Reader.SetData("PaperdollHairOffColor", &Pt->GameData.Image.PaperdollHair.OffColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollHair.OffColor.Color));
		Reader.SetData("PaperdollFaceOffColor", &Pt->GameData.Image.PaperdollFace.OffColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollFace.OffColor.Color));
		Reader.SetData("PaperdollHeadOffColor", &Pt->GameData.Image.PaperdollHead.OffColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollHead.OffColor.Color));
		Reader.SetData("PaperdollGlovesOffColor", &Pt->GameData.Image.PaperdollGloves.OffColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollGloves.OffColor.Color));
		Reader.SetData("PaperdollShoesOffColor", &Pt->GameData.Image.PaperdollShoes.OffColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollShoes.OffColor.Color));
		Reader.SetData("PaperdollClothesOffColor", &Pt->GameData.Image.PaperdollClothes.OffColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollClothes.OffColor.Color));
		Reader.SetData("PaperdollPantsOffColor", &Pt->GameData.Image.PaperdollPants.OffColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollPants.OffColor.Color));
		Reader.SetData("PaperdollBackOffColor", &Pt->GameData.Image.PaperdollBack.OffColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollBack.OffColor.Color));
		Reader.SetData("PaperdollBeltOffColor", &Pt->GameData.Image.PaperdollBelt.OffColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollBelt.OffColor.Color));
		Reader.SetData("PaperdollShouderOffColor", &Pt->GameData.Image.PaperdollShouder.OffColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PaperdollShouder.OffColor.Color));
		Reader.SetData("ArmorMainColor", &Pt->GameData.Image.ArmorMainColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ArmorMainColor.Color));
		Reader.SetData("ArmorOffColor", &Pt->GameData.Image.ArmorOffColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ArmorOffColor.Color));
		Reader.SetData("SkinColor", &Pt->GameData.Image.SkinColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.SkinColor.Color));
		Reader.SetData("HairColor", &Pt->GameData.Image.HairColor.Color, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.HairColor.Color));
		Reader.SetData("BoneScaleChest", &Pt->GameData.Image.BoneScaleChest, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.BoneScaleChest));
		Reader.SetData("BoneScaleButtock", &Pt->GameData.Image.BoneScaleButtock, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.BoneScaleButtock));
		Reader.SetData("BoneScaleHand", &Pt->GameData.Image.BoneScaleHand, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.BoneScaleHand));
		Reader.SetData("BoneScaleLeg", &Pt->GameData.Image.BoneScaleLeg, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.BoneScaleLeg));
		Reader.SetData("BoneScaleBra", &Pt->GameData.Image.BoneScaleBra, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.BoneScaleBra));
		Reader.SetData("Alpha", &Pt->GameData.Image.Alpha, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.Alpha));
		Reader.SetData("TipType", &Pt->GameData.Image.TipType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.TipType));
		Reader.SetData("IsShowShadow", &Pt->GameData.Image.IsShowShadow, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.IsShowShadow));
		Reader.SetData("IsThrowType", &Pt->GameData.Image.IsThrowType, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.IsThrowType));
		Reader.SetData("ThrowTypePreviewID", &Pt->GameData.Image.ThrowTypePreviewID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ThrowTypePreviewID));
		Reader.SetData("FlagType", &Pt->GameData.Image.Flag.Type, ENUM_Type_CharString, sizeof(Pt->GameData.Image.Flag.Type), ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.Flag.Type));
		Reader.SetData("FlagBanner", &Pt->GameData.Image.Flag.Banner, ENUM_Type_CharString, sizeof(Pt->GameData.Image.Flag.Banner), ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.Flag.Banner));
		Reader.SetData("FlagInsignia", &Pt->GameData.Image.Flag.Insignia, ENUM_Type_CharString, sizeof(Pt->GameData.Image.Flag.Insignia), ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.Flag.Insignia));
		Reader.SetData("FlagInsigniaColor1", &Pt->GameData.Image.Flag.InsigniaColor[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.Flag.InsigniaColor[0]));
		Reader.SetData("FlagInsigniaColor2", &Pt->GameData.Image.Flag.InsigniaColor[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.Flag.InsigniaColor[1]));
		Reader.SetData("FlagBannerColor1", &Pt->GameData.Image.Flag.BannerColor[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.Flag.BannerColor[0]));
		Reader.SetData("FlagBannerColor2", &Pt->GameData.Image.Flag.BannerColor[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.Flag.BannerColor[1]));
		Reader.SetData("LimitSex", &Pt->GameData.Image.LimitSex._Sex, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.LimitSex._Sex));
		Reader.SetData("IsPolyLineEffect", &Pt->GameData.Image.IsPolyLineEffect, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.IsPolyLineEffect));
		Reader.SetData("MagicShowType", &Pt->GameData.Image.MagicShowType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.MagicShowType));
		Reader.SetData("SmallIconType", &Pt->GameData.Image.SmallIconType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.SmallIconType));
		Reader.SetData("RideMotion", &Pt->GameData.Image.RideMotion, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.RideMotion));
		Reader.SetData("PhantomImg", &Pt->GameData.Image.PhantomImg, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.PhantomImg));
		for (int i = 0; i < _MAX_DROP_ABILITY_COUNT_; i++)
		{
			char Buf[256];
			sprintf_s(Buf, sizeof(Buf), "ActRate%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Image.ActRate[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Image.ActRate[i]));
		}
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Item()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char    Buf[512];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, 1, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 1, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 1, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("LimitVoc", &Pt->GameData.Limit.Voc._Voc, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.Voc._Voc));
		Reader.SetData("LimitRace", &Pt->GameData.Limit.Race._Race, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.Race._Race));
		Reader.SetData("LimitSex", &Pt->GameData.Limit.Sex._Sex, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.Sex._Sex));
		Reader.SetData("LimitLv", &Pt->GameData.Limit.Level, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.Level));
		Reader.SetData("LimitStr", &Pt->GameData.Limit.STR, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.STR));
		Reader.SetData("LimitSta", &Pt->GameData.Limit.STA, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.STA));
		Reader.SetData("LimitInt", &Pt->GameData.Limit.INT, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.INT));
		Reader.SetData("LimitMnd", &Pt->GameData.Limit.MND, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.MND));
		Reader.SetData("LimitAgi", &Pt->GameData.Limit.AGI, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.AGI));
		Reader.SetData("Mode", &Pt->GameData.Mode._Mode, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mode._Mode));
		Reader.SetData("ItemType", &Pt->GameData.Item.ItemType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ItemType));
		Reader.SetData("MaxHeap", &Pt->GameData.MaxHeap, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MaxHeap));
		Reader.SetData("GenerateRate", &Pt->GameData.GenerateRate, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.GenerateRate));
		Reader.SetData("MaxWorldCount", &Pt->GameData.MaxWorldCount, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MaxWorldCount));
		Reader.SetData("Rare", &Pt->GameData.Rare, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Rare));
		Reader.SetData("Cost", &Pt->GameData.Cost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Cost));
		Reader.SetData("PricesType", &Pt->GameData.PricesType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PricesType));
		Reader.SetData("SpecialMoneyCost", &Pt->GameData.SpecialMoneyCost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.SpecialMoneyCost));
		Reader.SetData("SellType", &Pt->GameData.SellType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.SellType));
		Reader.SetData("IncMagic_Onuse", &Pt->GameData.Item.IncMagic_Onuse, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.IncMagic_Onuse));
		Reader.SetData("DisappearRate", &Pt->GameData.Item.DisappearRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DisappearRate));
		Reader.SetData("DissolutionRate", &Pt->GameData.Item.DissolutionRate, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DissolutionRate));
		Reader.SetData("SrcRecipe", &Pt->GameData.Item.SrcRecipe, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.SrcRecipe));
		Reader.SetData("BuyUnit", &Pt->GameData.BuyUnit, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.BuyUnit));
		Reader.SetData("BodyMaxCount", &Pt->GameData.Item.BodyMaxCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.BodyMaxCount));
		Reader.SetData("UseType", &Pt->GameData.Item.UseType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.UseType));
		Reader.SetData("CliScript", &Pt->GameData.Item.CliScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CliScript));
		Reader.SetData("SrvScript", &Pt->GameData.Item.SrvScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.SrvScript));
		Reader.SetData("CheckUseScript", &Pt->GameData.Item.CheckUseScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CheckUseScript));
		Reader.SetData("MaxAttrCount", &Pt->GameData.Item.MaxAttrCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.MaxAttrCount));
		Reader.SetData("BaseAttrCount", &Pt->GameData.Item.BaseAttrCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.BaseAttrCount));
		Reader.SetData("SelectBaseAttrCount", &Pt->GameData.Item.SelectBaseAttrCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.SelectBaseAttrCount));
		Reader.SetData("Coloring_OffColor", &Pt->GameData.Item.Coloring_OffColor, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Coloring_OffColor));
		Reader.SetData("Coloring_MainColor", &Pt->GameData.Item.Coloring_MainColor, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Coloring_MainColor));
		Reader.SetData("FixQuality", &Pt->GameData.Item.FixQuality, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.FixQuality));
		Reader.SetData("EqRefineType", &Pt->GameData.Item.EqRefineType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.EqRefineType));
		Reader.SetData("EqRefineTableId", &Pt->GameData.Item.EqRefineTableId, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.EqRefineTableId));
		Reader.SetData("EqRefineRate_Base", &Pt->GameData.Item.EqRefineRate_Base, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.EqRefineRate_Base));
		Reader.SetData("EqRefineRate_Fix", &Pt->GameData.Item.EqRefineRate_Fix, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.EqRefineRate_Fix));
		Reader.SetData("EqRefineRate_Per", &Pt->GameData.Item.EqRefineRate_Per, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.EqRefineRate_Per));
		Reader.SetData("EqRefineMinLv", &Pt->GameData.Item.EqRefineMinLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.EqRefineMinLv));
		Reader.SetData("EqRefineMaxLv", &Pt->GameData.Item.EqRefineMaxLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.EqRefineMaxLv));
		Reader.SetData("EqRefineEqMinLv", &Pt->GameData.Item.EqRefineEqMinLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.EqRefineEqMinLv));
		Reader.SetData("EqRefineEqMaxLv", &Pt->GameData.Item.EqRefineEqMaxLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.EqRefineEqMaxLv));
		Reader.SetData("EqRefineFailed_LevelDownRate_Base", &Pt->GameData.Item.EqRefineFailed_LevelDownRate_Base, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.EqRefineFailed_LevelDownRate_Base));
		Reader.SetData("EqRefineFailed_LevelDownRate_Fix", &Pt->GameData.Item.EqRefineFailed_LevelDownRate_Fix, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.EqRefineFailed_LevelDownRate_Fix));
		Reader.SetData("EqRefineFailed_LevelDownRate_Per", &Pt->GameData.Item.EqRefineFailed_LevelDownRate_Per, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.EqRefineFailed_LevelDownRate_Per));
		Reader.SetData("EqRefineFailed_ExplodeRate_Base", &Pt->GameData.Item.EqRefineFailed_ExplodeRate_Base, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.EqRefineFailed_ExplodeRate_Base));
		Reader.SetData("EqRefineFailed_ExplodeRate_Fix", &Pt->GameData.Item.EqRefineFailed_ExplodeRate_Fix, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.EqRefineFailed_ExplodeRate_Fix));
		Reader.SetData("EqRefineFailed_ExplodeRate_Per", &Pt->GameData.Item.EqRefineFailed_ExplodeRate_Per, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.EqRefineFailed_ExplodeRate_Per));
		Reader.SetData("LiveTimeType", &Pt->GameData.LiveTimeType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LiveTimeType));
		Reader.SetData("LiveTime", &Pt->GameData.LiveTime, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LiveTime));
		Reader.SetData("ReferenceLv", &Pt->GameData.Item.ReferenceLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ReferenceLv));
		Reader.SetData("IsStandardCost", &Pt->GameData.Item.IsStandardCost, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.IsStandardCost));
		Reader.SetData("CardID", &Pt->GameData.Item.CardID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CardID));
		Reader.SetData("CastingTime", &Pt->GameData.Item.CastingTime, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CastingTime));
		Reader.SetData("GuildContributionValue", &Pt->GameData.Item.GuildContributionValue, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.GuildContributionValue));
		Reader.SetData("emFurnitureType", &Pt->GameData.Item.emFurnitureType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.emFurnitureType));
		Reader.SetData("emFurnitureEffect", &Pt->GameData.Item.emFurnitureEffect, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.emFurnitureEffect));
		Reader.SetData("iFurnitureStorageSize", &Pt->GameData.Item.iFurnitureStorageSize, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.iFurnitureStorageSize));
		Reader.SetData("emFurniturePos", &Pt->GameData.Item.emFurniturePos, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.emFurniturePos));
		Reader.SetData("iCraftObjType", &Pt->GameData.Item.iCraftObjType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.iCraftObjType));
		Reader.SetData("iFurnitureSuitID", &Pt->GameData.Item.iFurnitureSuitID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.iFurnitureSuitID));
		Reader.SetData("FurnitureNPCObjID", &Pt->GameData.Item.FurnitureNPCObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.FurnitureNPCObjID));
		Reader.SetData("BlackSmithHQPoint", &Pt->GameData.Item.BlackSmithHQPoint, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.BlackSmithHQPoint));
		Reader.SetData("CarpenterHQPoint", &Pt->GameData.Item.CarpenterHQPoint, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CarpenterHQPoint));
		Reader.SetData("MakeArmorHQPoint", &Pt->GameData.Item.MakeArmorHQPoint, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.MakeArmorHQPoint));
		Reader.SetData("TailorHQPoint", &Pt->GameData.Item.TailorHQPoint, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.TailorHQPoint));
		Reader.SetData("CookHQPoint", &Pt->GameData.Item.CookHQPoint, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CookHQPoint));
		Reader.SetData("AlchemyHQPoint", &Pt->GameData.Item.AlchemyHQPoint, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.AlchemyHQPoint));
		Reader.SetData("ExpPoint", &Pt->GameData.Item.ExpPoint, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ExpPoint));
		Reader.SetData("TpPoint", &Pt->GameData.Item.TpPoint, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.TpPoint));
		Reader.SetData("DigHoleMaxHoleCount", &Pt->GameData.Item.DigHole.MaxHoleCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DigHole.MaxHoleCount));
		Reader.SetData("DigHoleType", &Pt->GameData.Item.DigHole.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DigHole.Type));
		Reader.SetData("DigHoleRate", &Pt->GameData.Item.DigHole.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DigHole.Rate));
		Reader.SetData("IsUseNeedPassword", &Pt->GameData.Item.IsUseNeedPassword, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.IsUseNeedPassword));
		Reader.SetData("SenderRelation", &Pt->GameData.Item.SenderRelation, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.SenderRelation));
		Reader.SetData("TargetRelation", &Pt->GameData.Item.TargetRelation, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.TargetRelation));
		Reader.SetData("StarValue", &Pt->GameData.Item.StarValue, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.StarValue));
		Reader.SetData("GuildResourceType", &Pt->GameData.Item.GuildResourceType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.GuildResourceType));
		Reader.SetData("GuildResourceValue", &Pt->GameData.Item.GuildResourceValue, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.GuildResourceValue));
		for (int i = 0; i < _MAX_DROP_ABILITY_COUNT_; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "DropAbility%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Item.DropItemAbility[i].OrgID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DropItemAbility[i].OrgID));
			sprintf_s(Buf, sizeof(Buf), "DropAbilityRate%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Item.DropItemAbility[i].Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DropItemAbility[i].Rate));
		}
		for (int i = 0; i < 8; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "AttrValue%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Item.AttrValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.AttrValue[i]));
		}
		Reader.SetData("OnDestroyScript", &Pt->GameData.Item.OnDestroyScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.OnDestroyScript));
		Reader.SetData("ItemUseOKScript", &Pt->GameData.Item.ItemUseOKScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ItemUseOKScript));
		Reader.SetData("Plant_Seed_Type", &Pt->GameData.Item.Plant.Seed.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Seed.Type));
		Reader.SetData("Plant_Seed_GrowSpeed", &Pt->GameData.Item.Plant.Seed.GrowSpeed, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Seed.GrowSpeed));
		Reader.SetData("Plant_Seed_ModelType", &Pt->GameData.Item.Plant.Seed.ModelType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Seed.ModelType));
		Reader.SetData("Plant_Seed_BestWater", &Pt->GameData.Item.Plant.Seed.BestWater, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Seed.BestWater));
		Reader.SetData("Plant_Seed_BestTopdress", &Pt->GameData.Item.Plant.Seed.BestTopdress, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Seed.BestTopdress));
		Reader.SetData("Plant_Seed_RandSeed", &Pt->GameData.Item.Plant.Seed.RandSeed, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Seed.RandSeed));
		Reader.SetData("Plant_DecWater", &Pt->GameData.Item.Plant.DecWater, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.DecWater));
		Reader.SetData("Plant_DecTopdress", &Pt->GameData.Item.Plant.DecTopdress, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.DecTopdress));
		Reader.SetData("Plant_Pot_Mode", &Pt->GameData.Item.Plant.Pot.Mode, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Pot.Mode));
		Reader.SetData("Plant_Pot_Type1_Evn1", &Pt->GameData.Item.Plant.Pot.EvnPoint[0][0], ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Pot.EvnPoint[0][0]));
		Reader.SetData("Plant_Pot_Type1_Evn2", &Pt->GameData.Item.Plant.Pot.EvnPoint[0][1], ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Pot.EvnPoint[0][1]));
		Reader.SetData("Plant_Pot_Type2_Evn1", &Pt->GameData.Item.Plant.Pot.EvnPoint[1][0], ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Pot.EvnPoint[1][0]));
		Reader.SetData("Plant_Pot_Type2_Evn2", &Pt->GameData.Item.Plant.Pot.EvnPoint[1][1], ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Pot.EvnPoint[1][1]));
		Reader.SetData("Plant_Pot_Type3_Evn1", &Pt->GameData.Item.Plant.Pot.EvnPoint[2][0], ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Pot.EvnPoint[2][0]));
		Reader.SetData("Plant_Pot_Type3_Evn2", &Pt->GameData.Item.Plant.Pot.EvnPoint[2][1], ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Pot.EvnPoint[2][1]));
		Reader.SetData("Plant_Manure_Type", &Pt->GameData.Item.Plant.Manure.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Manure.Type));
		Reader.SetData("Plant_Manure_AddRare", &Pt->GameData.Item.Plant.Manure.AddRare, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Manure.AddRare));
		Reader.SetData("Plant_Manure_AddWater", &Pt->GameData.Item.Plant.Manure.AddWater, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Manure.AddWater));
		Reader.SetData("Plant_Manure_AddTopdress", &Pt->GameData.Item.Plant.Manure.AddTopdress, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Manure.AddTopdress));
		Reader.SetData("Plant_Manure_AddGrow", &Pt->GameData.Item.Plant.Manure.AddGrow, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Manure.AddGrow));
		Reader.SetData("Plant_Manure_AddHealth", &Pt->GameData.Item.Plant.Manure.AddHealth, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Manure.AddHealth));
		Reader.SetData("Plant_Manure_FeedPoint", &Pt->GameData.Item.Plant.Manure.FeedPoint, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Manure.FeedPoint));
		Reader.SetData("Plant_Level", &Pt->GameData.Item.Plant.Level, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Plant.Level));
		Reader.SetData("Furniture_Type", &Pt->GameData.Item.Furniture_Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Furniture_Type));
		Reader.SetData("Pet_PetObjID", &Pt->GameData.Item.Pet.PetObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Pet.PetObjID));
		Reader.SetData("Pet_Lv", &Pt->GameData.Item.Pet.Lv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Pet.Lv));
		Reader.SetData("Pet_Property", &Pt->GameData.Item.Pet.Property, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Pet.Property));
		Reader.SetData("Pet_SkillPoint", &Pt->GameData.Item.Pet.SkillPoint, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Pet.SkillPoint));
		Reader.SetData("Pet_Talent", &Pt->GameData.Item.Pet.Talent, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Pet.Talent));
		Reader.SetData("Pet_PetType", &Pt->GameData.Item.Pet.PetType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Pet.PetType));
		Reader.SetData("Pet_Talent", &Pt->GameData.Item.Pet.Talent, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Pet.Talent));
		Reader.SetData("Pet_Str", &Pt->GameData.Item.Pet.STR, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Pet.STR));
		Reader.SetData("Pet_Sta", &Pt->GameData.Item.Pet.STA, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Pet.STA));
		Reader.SetData("Pet_Int", &Pt->GameData.Item.Pet.INT, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Pet.INT));
		Reader.SetData("Pet_Mnd", &Pt->GameData.Item.Pet.MND, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Pet.MND));
		Reader.SetData("Pet_Agi", &Pt->GameData.Item.Pet.AGI, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Pet.AGI));
		Reader.SetData("Pet_Spirit", &Pt->GameData.Item.Pet.Spirit, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Pet.Spirit));
		Reader.SetData("Pet_LuaEvent", &Pt->GameData.Item.Pet.LuaEvent, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Pet.LuaEvent));
		Reader.SetData("PetFood_SkillPoint", &Pt->GameData.Item.PetFood.SkillPoint, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.PetFood.SkillPoint));
		Reader.SetData("PetFood_Spirit", &Pt->GameData.Item.PetFood.Spirit, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.PetFood.Spirit));
		Reader.SetData("PetFood_Loyal", &Pt->GameData.Item.PetFood.Loyal, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.PetFood.Loyal));
		Reader.SetData("PetFood_Talent", &Pt->GameData.Item.PetFood.Talent, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.PetFood.Talent));
		Reader.SetData("PetFood_Hunger", &Pt->GameData.Item.PetFood.Hunger, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.PetFood.Hunger));
		Reader.SetData("PetFood_Exp", &Pt->GameData.Item.PetFood.Exp, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.PetFood.Exp));
		Reader.SetData("PetFood_Talent", &Pt->GameData.Item.PetFood.Talent, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.PetFood.Talent));
		Reader.SetData("PetFood_EffectType", &Pt->GameData.Item.PetFood.EffectType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.PetFood.EffectType));
		Reader.SetData("PetFood_EffectValue", &Pt->GameData.Item.PetFood.EffectValue, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.PetFood.EffectValue));
		Reader.SetData("DropListPacket", &Pt->GameData.Item.DropListPacket, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DropListPacket));
		Reader.SetData("TargetLimit_Level", &Pt->GameData.Item.ItemToItemLimit.Level, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ItemToItemLimit.Level));
		Reader.SetData("TargetLimit_ArmorPos", &Pt->GameData.Item.ItemToItemLimit._ArmorPos, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ItemToItemLimit._ArmorPos));
		Reader.SetData("TargetLimit_ArmorType", &Pt->GameData.Item.ItemToItemLimit._ArmorType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ItemToItemLimit._ArmorType));
		Reader.SetData("TargetLimit_WeaponType", &Pt->GameData.Item.ItemToItemLimit._WeaponType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ItemToItemLimit._WeaponType));
		Reader.SetData("TargetLimit_Other", &Pt->GameData.Item.ItemToItemLimit._Other, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ItemToItemLimit._Other));
		Reader.SetData("TargetLimit_WeaponPos", &Pt->GameData.Item.ItemToItemLimit.WeaponPosEnum, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ItemToItemLimit.WeaponPosEnum));
		Reader.SetData("FurnitureSoundID", &Pt->GameData.Item.FurnitureSoundID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.FurnitureSoundID));
		Reader.SetData("ExBagType", &Pt->GameData.Item.ExBagType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ExBagType));
		Reader.SetData("ExBagTypeSub", &Pt->GameData.Item.ExBagTypeSub, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ExBagTypeSub));
		Reader.SetData("TitleSysFamous", &Pt->GameData.Item.TitleSys.Famous, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.TitleSys.Famous));
		Reader.SetData("TitleSysExp", &Pt->GameData.Item.TitleSys.Exp, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.TitleSys.Exp));
		Reader.SetData("CollectionNPC", &Pt->GameData.Item.CollectionNPC, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CollectionNPC));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_KeyItem()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, 1, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 1, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 1, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("Type", &Pt->GameData.KeyItem.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.KeyItem.Type));
		Reader.SetData("QuestID", &Pt->GameData.KeyItem.QuestID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.KeyItem.QuestID));
		Reader.SetData("LifeSkillType", &Pt->GameData.KeyItem.LifeSkillType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.KeyItem.LifeSkillType));
		Reader.SetData("LifeSkillLearnLv", &Pt->GameData.KeyItem.LifeSkillLearnLV, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.KeyItem.LifeSkillLearnLV));
		Reader.SetData("LifeSkillMaxLv", &Pt->GameData.KeyItem.LifeSkillMaxLV, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.KeyItem.LifeSkillMaxLV));
		Reader.SetData("Cost", &Pt->GameData.Cost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Cost));
		Reader.SetData("SellType", &Pt->GameData.SellType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.SellType));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_LearnMagic()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("SPMagicCount", &Pt->GameData.LearnMagic.SPSkillCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LearnMagic.SPSkillCount));
		Reader.SetData("NormalMagicCount", &Pt->GameData.LearnMagic.NormalSkillCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LearnMagic.NormalSkillCount));
		Reader.SetData("SPMagicInfo", Pt->GameData.LearnMagic.SPSkill, ENUM_Type_BinaryData, sizeof(Pt->GameData.LearnMagic.SPSkill), ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LearnMagic.SPSkill));
		Reader.SetData("NormalMagicInfo", Pt->GameData.LearnMagic.NormalSkill, ENUM_Type_BinaryData, sizeof(Pt->GameData.LearnMagic.NormalSkill), ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LearnMagic.NormalSkill));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_MagicBase()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char Buf[256];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, 1, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 1, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 1, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("EffectType", &Pt->GameData.MagicBase.EffectType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.EffectType));
		Reader.SetData("MagicType", &Pt->GameData.MagicBase.MagicType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.MagicType));
		Reader.SetData("AtkCalType", &Pt->GameData.MagicBase.AtkCalType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.AtkCalType));
		Reader.SetData("MagicFunc", &Pt->GameData.MagicBase.MagicFunc, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.MagicFunc));
		Reader.SetData("EffectTime", &Pt->GameData.MagicBase.EffectTime, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.EffectTime));
		Reader.SetData("DLV_EffectTime", &Pt->GameData.MagicBase.DLV_EffectTime, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.DLV_EffectTime));
		Reader.SetData("HateCost", &Pt->GameData.MagicBase.HateCost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.HateCost));
		Reader.SetData("EffectLV", &Pt->GameData.MagicBase.EffectLV, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.EffectLV));
		Reader.SetData("AssistType", &Pt->GameData.MagicBase.AssistType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.AssistType));
		Reader.SetData("Effect", &Pt->GameData.MagicBase.Effect._Value[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Effect._Value[0]));
		Reader.SetData("Effect1", &Pt->GameData.MagicBase.Effect._Value[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Effect._Value[1]));
		Reader.SetData("OnTimeMagic_Time", &Pt->GameData.MagicBase.OnTimeMagic_Time, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.OnTimeMagic_Time));
		Reader.SetData("OnTimeMagic_Magic", &Pt->GameData.MagicBase.OnTimeMagic_Magic, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.OnTimeMagic_Magic));
		Reader.SetData("ChangeID", &Pt->GameData.MagicBase.FaceOffID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.FaceOffID));
		Reader.SetData("RideID", &Pt->GameData.MagicBase.RideID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.RideID));
		Reader.SetData("EffectAddonType", &Pt->GameData.MagicBase.EffectAddonType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.EffectAddonType));
		Reader.SetData("OnAttackMagicID", &Pt->GameData.MagicBase.OnAttackMagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.OnAttackMagicID));
		Reader.SetData("OnAttackReboundMagicID", &Pt->GameData.MagicBase.OnAttackReboundMagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.OnAttackReboundMagicID));
		Reader.SetData("OnMagicAttackReboundMagicID", &Pt->GameData.MagicBase.OnMagicAttackReboundMagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.OnMagicAttackReboundMagicID));
		Reader.SetData("OnBuffTimeOutMagicID", &Pt->GameData.MagicBase.OnBuffTimeOutMagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.OnBuffTimeOutMagicID));
		Reader.SetData("OnBuffTimeOutMagicIDEx", &Pt->GameData.MagicBase.OnBuffTimeOutMagicIDEx, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.OnBuffTimeOutMagicIDEx));
		Reader.SetData("OnDeadMagicID", &Pt->GameData.MagicBase.OnDeadMagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.OnDeadMagicID));
		Reader.SetData("OnKillMagicID", &Pt->GameData.MagicBase.OnKillMagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.OnKillMagicID));
		Reader.SetData("AddBuffTime", &Pt->GameData.MagicBase.AddBuffTime, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.AddBuffTime));
		Reader.SetData("BuffMaxLv", &Pt->GameData.MagicBase.BuffMaxLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.BuffMaxLv));
		Reader.SetData("TempHateCost", &Pt->GameData.MagicBase.TempHateCost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.TempHateCost));
		Reader.SetData("Atk_AtkType", &Pt->GameData.MagicBase.Attack.AtkType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Attack.AtkType));
		Reader.SetData("Atk_HateRate", &Pt->GameData.MagicBase.Attack.HateRate, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Attack.HateRate));
		Reader.SetData("Atk_DmgPower", &Pt->GameData.MagicBase.Attack.DmgPower, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Attack.DmgPower));
		Reader.SetData("Atk_FixValue", &Pt->GameData.MagicBase.Attack.FixValue, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Attack.FixValue));
		Reader.SetData("Atk_FixType", &Pt->GameData.MagicBase.Attack.FixType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Attack.FixType));
		Reader.SetData("Atk_Rand", &Pt->GameData.MagicBase.Attack.Rand, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Attack.Rand));
		Reader.SetData("Atk_CritialRate", &Pt->GameData.MagicBase.Attack.CritialRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Attack.CritialRate));
		Reader.SetData("TeleportZoneID", &Pt->GameData.MagicBase.Teleport.ZoneID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Teleport.ZoneID));
		Reader.SetData("TeleportX", &Pt->GameData.MagicBase.Teleport.X, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Teleport.X));
		Reader.SetData("TeleportY", &Pt->GameData.MagicBase.Teleport.Y, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Teleport.Y));
		Reader.SetData("TeleportZ", &Pt->GameData.MagicBase.Teleport.Z, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Teleport.Z));
		Reader.SetData("TeleportDir", &Pt->GameData.MagicBase.Teleport.Dir, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Teleport.Dir));
		Reader.SetData("RaiseExpPrecnt", &Pt->GameData.MagicBase.Raise.ExpPrecnt, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Raise.ExpPrecnt));
		Reader.SetData("SCObjID", &Pt->GameData.MagicBase.SummonCreature.ObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.SummonCreature.ObjID));
		Reader.SetData("SCLevel", &Pt->GameData.MagicBase.SummonCreature.Level, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.SummonCreature.Level));
		Reader.SetData("SCRangeLevel", &Pt->GameData.MagicBase.SummonCreature.RangeLevel, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.SummonCreature.RangeLevel));
		Reader.SetData("SCLiveTime", &Pt->GameData.MagicBase.SummonCreature.LiveTime, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.SummonCreature.LiveTime));
		Reader.SetData("SCSkillLvArg", &Pt->GameData.MagicBase.SummonCreature.SkillLvArg, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.SummonCreature.SkillLvArg));
		Reader.SetData("SCOwnerPowerRate", &Pt->GameData.MagicBase.SummonCreature.OwnerPowerRate, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.SummonCreature.OwnerPowerRate));
		Reader.SetData("SCType", &Pt->GameData.MagicBase.SummonCreature.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.SummonCreature.Type));
		Reader.SetData("SCGroupID", &Pt->GameData.MagicBase.SummonCreature.GroupID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.SummonCreature.GroupID));
		Reader.SetData("SCSkillType0", &Pt->GameData.MagicBase.SummonCreature.SkillType[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.SummonCreature.SkillType[0]));
		Reader.SetData("SCSkillType1", &Pt->GameData.MagicBase.SummonCreature.SkillType[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.SummonCreature.SkillType[1]));
		Reader.SetData("SCSkillType2", &Pt->GameData.MagicBase.SummonCreature.SkillType[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.SummonCreature.SkillType[2]));
		Reader.SetData("SCInitLua", &Pt->GameData.MagicBase.SummonCreature.InitLua, ENUM_Type_CharString, sizeof(Pt->GameData.MagicBase.SummonCreature.InitLua), ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.SummonCreature.InitLua));
		Reader.SetData("SIObjID", &Pt->GameData.MagicBase.SummonItem.ObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.SummonItem.ObjID));
		Reader.SetData("StealSuccessRate", &Pt->GameData.MagicBase.Steal.SuccessRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Steal.SuccessRate));
		Reader.SetData("MIObjID", &Pt->GameData.MagicBase.ItemRunPlot.ObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.ItemRunPlot.ObjID));
		Reader.SetData("MIMode", &Pt->GameData.MagicBase.ItemRunPlot.Mode, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.ItemRunPlot.Mode));
		Reader.SetData("MIPlotName", &Pt->GameData.MagicBase.ItemRunPlot.PlotName, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.ItemRunPlot.PlotName));
		Reader.SetData("MILiveTime", &Pt->GameData.MagicBase.ItemRunPlot.LiveTime, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.ItemRunPlot.LiveTime));
		Reader.SetData("MIUseMagic", &Pt->GameData.MagicBase.ItemRunPlot.UseMagic, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.ItemRunPlot.UseMagic));
		Reader.SetData("MIOnDeadMagic", &Pt->GameData.MagicBase.ItemRunPlot.OnDeadMagic, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.ItemRunPlot.OnDeadMagic));
		Reader.SetData("MIType", &Pt->GameData.MagicBase.ItemRunPlot.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.ItemRunPlot.Type));
		Reader.SetData("MIGroupID", &Pt->GameData.MagicBase.ItemRunPlot.GroupID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.ItemRunPlot.GroupID));
		Reader.SetData("MITargetType", &Pt->GameData.MagicBase.ItemRunPlot.TargetType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.ItemRunPlot.TargetType));
		Reader.SetData("SettingFlag", &Pt->GameData.MagicBase.Setting._Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Setting._Type));
		Reader.SetData("SettingFlagEx", &Pt->GameData.MagicBase.SettingEx._Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.SettingEx._Type));
		Reader.SetData("SpecialAction", &Pt->GameData.MagicBase.SpecialAction._Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.SpecialAction._Type));
		Reader.SetData("MRPlotName", &Pt->GameData.MagicBase.RunPlot.PlotName, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.RunPlot.PlotName));
		for (int i = 0; i < Max_WearEqType_Count; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "EqType%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.MagicBase.Ability.EqType[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Ability.EqType[i]));
			sprintf_s(Buf, sizeof(Buf), "EqTypeValue%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.MagicBase.Ability.EqTypeValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Ability.EqTypeValue[i]));
		}
		Reader.SetData("EqTypeOnAttackMagicID", &Pt->GameData.MagicBase.Ability.OnAttackMagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Ability.OnAttackMagicID));
		Reader.SetData("EqTypeOnAttackMagicRank", &Pt->GameData.MagicBase.Ability.OnAttackMagicRank, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Ability.OnAttackMagicRank));
		Reader.SetData("EqTypeOnAttackMagicRate", &Pt->GameData.MagicBase.Ability.OnAttackMagicRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Ability.OnAttackMagicRate));
		Reader.SetData("DotTime", &Pt->GameData.MagicBase.DotMagic.Time, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.DotMagic.Time));
		Reader.SetData("DotType", &Pt->GameData.MagicBase.DotMagic.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.DotMagic.Type));
		Reader.SetData("DotBase", &Pt->GameData.MagicBase.DotMagic.Base, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.DotMagic.Base));
		Reader.SetData("MagicClear", &Pt->GameData.MagicBase.ClearTime, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.ClearTime));
		Reader.SetData("MagicShield_Type", &Pt->GameData.MagicBase.MagicShield.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.MagicShield.Type));
		Reader.SetData("MagicShield_Effect", &Pt->GameData.MagicBase.MagicShield.Effect, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.MagicShield.Effect));
		Reader.SetData("MagicShield_Point", &Pt->GameData.MagicBase.MagicShield.Point, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.MagicShield.Point));
		Reader.SetData("MagicGroupID", &Pt->GameData.MagicBase.MagicGroupID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.MagicGroupID));
		Reader.SetData("BuffCount", &Pt->GameData.MagicBase.BuffCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.BuffCount));
		Reader.SetData("MaxBuffLv", &Pt->GameData.MagicBase.MaxBuffLv_Base, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.MaxBuffLv_Base));
		Reader.SetData("BuffTimeDesc_Time", &Pt->GameData.MagicBase.BuffTimeDesc_Time, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.BuffTimeDesc_Time));
		Reader.SetData("BuffTimeDesc_Type", &Pt->GameData.MagicBase.BuffTimeDesc_Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.BuffTimeDesc_Type));
		for (int i = 0; i < _MAX_BUFFSKILL_COUNT_; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "BuffSkill%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.MagicBase.BuffSkill[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.BuffSkill[i]));
		}
		Reader.SetData("DotSkillLVArg", &Pt->GameData.MagicBase.DotMagic.DotSkillLVArg, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.DotMagic.DotSkillLVArg));
		Reader.SetData("EffectTime_SkillLVArg", &Pt->GameData.MagicBase.EffectTime_SkillLVArg, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.EffectTime_SkillLVArg));
		Reader.SetData("MaxBuffLv_SkillLVArg", &Pt->GameData.MagicBase.MaxBuffLv_SkillLVArg, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.MaxBuffLv_SkillLVArg));
		Reader.SetData("Ability_SkillLVArg", &Pt->GameData.MagicBase.Ability_SkillLVArg, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Ability_SkillLVArg));
		Reader.SetData("DmgPower_SkillLVArg", &Pt->GameData.MagicBase.Attack.DmgPower_SkillLVArg, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Attack.DmgPower_SkillLVArg));
		Reader.SetData("FixDmg_SkillLVArg", &Pt->GameData.MagicBase.Attack.FixDmg_SkillLVArg, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Attack.FixDmg_SkillLVArg));
		Reader.SetData("MagicShield_SkillLVArg", &Pt->GameData.MagicBase.MagicShield.SkillLvArg, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.MagicShield.SkillLvArg));
		Reader.SetData("PhyAttackType", &Pt->GameData.MagicBase.PhyAttackType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.PhyAttackType));
		Reader.SetData("IsStandard_Attack", &Pt->GameData.MagicBase.IsStandard_Attack, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.IsStandard_Attack));
		Reader.SetData("RefPowerLv_Attack", &Pt->GameData.MagicBase.RefPowerLv_Attack, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.RefPowerLv_Attack));
		Reader.SetData("RefPowerRate_Attack", &Pt->GameData.MagicBase.RefPowerRate_Attack, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.RefPowerRate_Attack));
		Reader.SetData("ModelSize", &Pt->GameData.MagicBase.ModelSize, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.ModelSize));
		Reader.SetData("Revive_SkillLVArg", &Pt->GameData.MagicBase.Revive_SkillLVArg, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.Revive_SkillLVArg));
		Reader.SetData("MagicGroupSet", &Pt->GameData.MagicBase.MagicGroupSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.MagicGroupSet));
		Reader.SetData("ClearMagicGroupSet", &Pt->GameData.MagicBase.ClearMagicGroupSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.ClearMagicGroupSet));
		Reader.SetData("CheckUseLua", &Pt->GameData.MagicBase.CheckUseLua, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.CheckUseLua));
		Reader.SetData("SpecialMagicEventType", &Pt->GameData.MagicBase.SpecialMagicEventType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.SpecialMagicEventType));
		Reader.SetData("OnSpecialMagicID", &Pt->GameData.MagicBase.OnSpecialMagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.OnSpecialMagicID));
		Reader.SetData("EndUseLua", &Pt->GameData.MagicBase.EndUseLua, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.EndUseLua));
		Reader.SetData("HitBackDist", &Pt->GameData.MagicBase.HitBackDist, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.HitBackDist));
		Reader.SetData("CarryCount", &Pt->GameData.MagicBase.CarryCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.CarryCount));
		for (int i = 0; i < 4; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "MotionChangeType%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.MagicBase.MotionChangeType[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.MotionChangeType[i]));
			sprintf_s(Buf, sizeof(Buf), "MotionChangeID%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.MagicBase.MotionChangeID[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicBase.MotionChangeID[i]));
		}
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Mine()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("GenerateRate", &Pt->GameData.GenerateRate, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.GenerateRate));
		Reader.SetData("MaxWorldCount", &Pt->GameData.MaxWorldCount, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MaxWorldCount));
		Reader.SetData("Rare", &Pt->GameData.Rare, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Rare));
		Reader.SetData("BaseCount", &Pt->GameData.Mine.BaseCount, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mine.BaseCount));
		Reader.SetData("Zone1", &Pt->Zone[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Zone[0]));
		Reader.SetData("Zone2", &Pt->Zone[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Zone[1]));
		Reader.SetData("Zone3", &Pt->Zone[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Zone[2]));
		Reader.SetData("ItemType", &Pt->GameData.Mine.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mine.Type));
		Reader.SetData("CreateType", &Pt->GameData.Mine.CreateType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mine.CreateType));
		Reader.SetData("LVLimit", &Pt->GameData.Mine.LvLimit, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mine.LvLimit));
		Reader.SetData("FlagID", &Pt->GameData.Mine.iFlagID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mine.iFlagID));
		Reader.SetData("ReduceVal", &Pt->GameData.Mine.iReduceVal, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mine.iReduceVal));
		Reader.SetData("szLuaGetItemScript", &Pt->GameData.Mine.szLuaGetItemScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mine.szLuaGetItemScript));
		char Buf[256];
		for (int i = 0; i < 7; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "GetItem%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Mine.GetItem[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mine.GetItem[i]));
			sprintf_s(Buf, sizeof(Buf), "GetItemRate%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Mine.GetItemRate[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mine.GetItemRate[i]));
			sprintf_s(Buf, sizeof(Buf), "GetItemCount%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Mine.GetItemCount[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mine.GetItemCount[i]));
		}
		for (int i = 0; i < 5; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "GenerateObj%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Mine.iGenerateObj[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mine.iGenerateObj[i]));
			sprintf_s(Buf, sizeof(Buf), "GenerateObjRate%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Mine.iGenerateObjRate[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mine.iGenerateObjRate[i]));
		}
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_NPC()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("Zone1", &Pt->Zone[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Zone[0]));
		Reader.SetData("Zone2", &Pt->Zone[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Zone[1]));
		Reader.SetData("Zone3", &Pt->Zone[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Zone[2]));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("ReviveTime", &Pt->GameData.ReviveTime, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ReviveTime));
		Reader.SetData("Race", &Pt->GameData.NPC.Race, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Race));
		Reader.SetData("Vocation", &Pt->GameData.NPC.Vocation, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Vocation));
		Reader.SetData("Vocation_Sub", &Pt->GameData.NPC.Vocation_Sub, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Vocation_Sub));
		Reader.SetData("Sex", &Pt->GameData.NPC.Sex, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Sex));
		Reader.SetData("ExpRate", &Pt->GameData.NPC.ExpRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.ExpRate));
		Reader.SetData("TPRate", &Pt->GameData.NPC.TpRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.TpRate));
		Reader.SetData("Level", &Pt->GameData.NPC.Level, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Level));
		Reader.SetData("LevelRange", &Pt->GameData.NPC.LevelRange, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.LevelRange));
		Reader.SetData("STR", &Pt->GameData.NPC.STR, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.STR));
		Reader.SetData("STA", &Pt->GameData.NPC.STA, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.STA));
		Reader.SetData("INT", &Pt->GameData.NPC.INT, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.INT));
		Reader.SetData("MND", &Pt->GameData.NPC.MND, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.MND));
		Reader.SetData("AGI", &Pt->GameData.NPC.AGI, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.AGI));
		Reader.SetData("MoveSpeed", &Pt->GameData.NPC.MoveSpeed, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.MoveSpeed));
		Reader.SetData("SearchRange", &Pt->GameData.NPC.SearchRange, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.SearchRange));
		Reader.SetData("DropMoney_Base", &Pt->GameData.NPC.DropMoney_Base, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.DropMoney_Base));
		Reader.SetData("DropMoney_Rand", &Pt->GameData.NPC.DropMoney_Rand, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.DropMoney_Rand));
		Reader.SetData("Brave", &Pt->GameData.NPC.Brave, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Brave));
		Reader.SetData("LinkType", &Pt->GameData.NPC.LinkType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.LinkType));
		Reader.SetData("EscapeType", &Pt->GameData.NPC.EscapeType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.EscapeType));
		char Buf[256];
		for (int i = 0; i < _MAX_DROPITEM_COUNT_; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "DropID%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.DropItem[i].OrgID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.DropItem[i].OrgID));
			sprintf_s(Buf, sizeof(Buf), "DropRate%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.DropItem[i].Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.DropItem[i].Rate));
		}
		for (int i = 0; i < _MAX_DROP_ABILITY_COUNT_; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "DropAbility%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.DropItemAbility[i].OrgID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.DropItemAbility[i].OrgID));
			sprintf_s(Buf, sizeof(Buf), "DropAbilityRate%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.DropItemAbility[i].Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.DropItemAbility[i].Rate));
		}
		for (int i = 0; i < _MAX_NPC_EQSEL_COUNT_; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "MainHandEq%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.MainHandEQ[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.MainHandEQ[i]));
			sprintf_s(Buf, sizeof(Buf), "SecHandEq%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.SecHandEQ[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.SecHandEQ[i]));
		}
		Reader.SetData("BowEQ", &Pt->GameData.NPC.BowEQ, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.BowEQ));
		Reader.SetData("OrnamentEQ", &Pt->GameData.NPC.OrnamentEQ, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.OrnamentEQ));
		Reader.SetData("CampID", &Pt->GameData.NPC.CampID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.CampID));
		for (int i = 0; i < _MAX_NPC_SPELL_MAGIC_COUNT_; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "SpellRightTime%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.Spell[i].RigthTime, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Spell[i].RigthTime));
			sprintf_s(Buf, sizeof(Buf), "SpellTarget%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.Spell[i].TargetType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Spell[i].TargetType));
			sprintf_s(Buf, sizeof(Buf), "SpellRate%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.Spell[i].Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Spell[i].Rate));
			sprintf_s(Buf, sizeof(Buf), "SpellMagic%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.Spell[i].MagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Spell[i].MagicID));
			sprintf_s(Buf, sizeof(Buf), "SpellMagicLv%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.Spell[i].MagicLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Spell[i].MagicLv));
			sprintf_s(Buf, sizeof(Buf), "SpellString%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.Spell[i].SpellString, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Spell[i].SpellString));
		}
		Reader.SetData("TitleType", &Pt->GameData.NPC.TitleType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.TitleType));
		Reader.SetData("DB_ResistID", &Pt->GameData.NPC.DB_ResistID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.DB_ResistID));
		for (int i = 0; i < _MAX_NPC_STEAL_ITEM_COUNT_; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "StealItemOrgID%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.StealItem[i].OrgID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.StealItem[i].OrgID));
			sprintf_s(Buf, sizeof(Buf), "StealItemRate%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.StealItem[i].Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.StealItem[i].Rate));
		}
		Reader.SetData("StealMoney_Base", &Pt->GameData.NPC.StealMoney_Base, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.StealMoney_Base));
		Reader.SetData("StealMoney_Rand", &Pt->GameData.NPC.StealMoney_Rand, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.StealMoney_Rand));
		for (int i = 0; i < _MAX_NPC_AVOID_MAGIC_TYPE_COUNT_; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "AvoidMagicType%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.AvoidMagicType[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.AvoidMagicType[i]));
		}
		Reader.SetData("Effect", &Pt->GameData.NPC.Effect._Value, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Effect._Value));
		Reader.SetData("RaceGroupType", &Pt->GameData.NPC.RaceGroupType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.RaceGroupType));
		Reader.SetData("KeepDistance", &Pt->GameData.NPC.KeepDistance, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.KeepDistance));
		Reader.SetData("OrgWeaponType", &Pt->GameData.NPC.OrgWeaponType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.OrgWeaponType));
		Reader.SetData("OrgArmorType", &Pt->GameData.NPC.OrgArmorType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.OrgArmorType));
		Reader.SetData("FollowRange", &Pt->GameData.NPC.FollowRange, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.FollowRange));
		Reader.SetData("RaiseChangeNPCID1", &Pt->GameData.NPC.RaiseChangeNPCID[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.RaiseChangeNPCID[0]));
		Reader.SetData("RaiseChangeNPCID2", &Pt->GameData.NPC.RaiseChangeNPCID[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.RaiseChangeNPCID[1]));
		Reader.SetData("RaiseChangeRate1", &Pt->GameData.NPC.RaiseChangeRate[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.RaiseChangeRate[0]));
		Reader.SetData("RaiseChangeRate2", &Pt->GameData.NPC.RaiseChangeRate[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.RaiseChangeRate[1]));
		Reader.SetData("OtherFlag", &Pt->GameData.NPC.OtherFlag, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.OtherFlag));
		for (int i = 0; i < Max_WearEqType_Count; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "EqType%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.Ability.EqType[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Ability.EqType[i]));
			sprintf_s(Buf, sizeof(Buf), "EqTypeValue%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.Ability.EqTypeValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Ability.EqTypeValue[i]));
			sprintf_s(Buf, sizeof(Buf), "DEqTypeValue%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.DEqTypeValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.DEqTypeValue[i]));
		}
		for (int i = 0; i < Max_WearEqType_Count; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "EqExType%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.AbilityEx.EqType[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.AbilityEx.EqType[i]));
			sprintf_s(Buf, sizeof(Buf), "EqExTypeValue%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.NPC.AbilityEx.EqTypeValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.AbilityEx.EqTypeValue[i]));
		}
		Reader.SetData("IsAbilityStandard", &Pt->GameData.NPC.IsAbilityStandard, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.IsAbilityStandard));
		Reader.SetData("AbilityValue1", &Pt->GameData.NPC.AbilityValue[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.AbilityValue[0]));
		Reader.SetData("AbilityValue2", &Pt->GameData.NPC.AbilityValue[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.AbilityValue[1]));
		Reader.SetData("EqTypeOnAttackMagicID", &Pt->GameData.NPC.Ability.OnAttackMagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Ability.OnAttackMagicID));
		Reader.SetData("EqTypeOnAttackMagicRank", &Pt->GameData.NPC.Ability.OnAttackMagicRank, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Ability.OnAttackMagicRank));
		Reader.SetData("EqTypeOnAttackMagicRate", &Pt->GameData.NPC.Ability.OnAttackMagicRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.Ability.OnAttackMagicRate));
		Reader.SetData("TitleName", &Pt->GameData.NPC.TitleName, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.TitleName));
		Reader.SetData("PhyResistRate1", &Pt->GameData.NPC.PhyResistRate[0], ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.PhyResistRate[0]));
		Reader.SetData("PhyResistRate2", &Pt->GameData.NPC.PhyResistRate[1], ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.PhyResistRate[1]));
		Reader.SetData("PhyResistRate3", &Pt->GameData.NPC.PhyResistRate[2], ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.PhyResistRate[2]));
		Reader.SetData("ZoneID", &Pt->GameData.NPC.ZoneID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.NPC.ZoneID));
		Reader.SetData("X", &Pt->GameData.NPC.X, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.NPC.X));
		Reader.SetData("Y", &Pt->GameData.NPC.Y, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.NPC.Y));
		Reader.SetData("Z", &Pt->GameData.NPC.Z, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.NPC.Z));
		Reader.SetData("ZoneID_Bk", &Pt->GameData.NPC.ZoneID_Bk, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.ZoneID_Bk));
		Reader.SetData("KillGroupID", &Pt->GameData.NPC.KillGroupID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.KillGroupID));
		Reader.SetData("szLuaInitScript", &Pt->GameData.NPC.szLuaInitScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.szLuaInitScript));
		Reader.SetData("AddPowerLV", &Pt->GameData.NPC.AddPowerLV, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.AddPowerLV));
		Reader.SetData("LuaEvent_OnAttackMagic", &Pt->GameData.NPC.LuaEvent_OnAttackMagic, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.LuaEvent_OnAttackMagic));
		Reader.SetData("LuaEvent_OnAssistMagic", &Pt->GameData.NPC.LuaEvent_OnAssistMagic, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.LuaEvent_OnAssistMagic));
		Reader.SetData("LuaEvent_OnDead", &Pt->GameData.NPC.LuaEvent_OnDead, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.LuaEvent_OnDead));
		Reader.SetData("LuaEvent_OnKill", &Pt->GameData.NPC.LuaEvent_OnKill, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.LuaEvent_OnKill));
		Reader.SetData("LuaEvent_BeginAttack", &Pt->GameData.NPC.LuaEvent_BeginAttack, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.LuaEvent_BeginAttack));
		Reader.SetData("LuaEvent_EndAttack", &Pt->GameData.NPC.LuaEvent_EndAttack, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.LuaEvent_EndAttack));
		Reader.SetData("SpecialDropItemType", &Pt->GameData.NPC.SpecialDropItemType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.SpecialDropItemType));
		Reader.SetData("MinAbilityLv", &Pt->GameData.NPC.MinAbilityLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.MinAbilityLv));
		Reader.SetData("NpcMode", &Pt->GameData.NPC._NpcMode, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC._NpcMode));
		Reader.SetData("szLuaDisplayScript", &Pt->GameData.NPC.szLuaDisplayScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.szLuaDisplayScript));
		Reader.SetData("CarryType", &Pt->GameData.NPC.CarryType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.CarryType));
		Reader.SetData("CarryCount", &Pt->GameData.NPC.CarryCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.CarryCount));
		Reader.SetData("SearchGroupID", &Pt->GameData.NPC.SearchGroupID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.SearchGroupID));
		Reader.SetData("CarryLuaScript", &Pt->GameData.NPC.CarryLuaScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.CarryLuaScript));
		Reader.SetData("CarryShootRange", &Pt->GameData.NPC.CarryShootRange, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.CarryShootRange));
		Reader.SetData("CarryShootAngle", &Pt->GameData.NPC.CarryShootAngle, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.CarryShootAngle));
		Reader.SetData("CheckCarryLuaScript", &Pt->GameData.NPC.CheckCarryLuaScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.CheckCarryLuaScript));
		Reader.SetData("CheckCarryLuaPoint", &Pt->GameData.NPC.CheckCarryLuaPoint, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.CheckCarryLuaPoint));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Player()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, sizeof(Pt->LastModifyDate), ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, sizeof(Pt->LastModifyAccount), ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, sizeof(Pt->DataGroup), ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, sizeof(Pt->LockAccount), ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, sizeof(Pt->GameData.Name), ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, sizeof(Pt->GameData.Note), ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, sizeof(Pt->GameData.ShortNote), ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_QuestDetail()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char    Buf[256];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("iQuest_Catalog", &Pt->GameData.QuestDetail.iQuest_Catalog, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iQuest_Catalog));
		Reader.SetData("iQuest_Type", &Pt->GameData.QuestDetail.iQuest_Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iQuest_Type));
		Reader.SetData("iCheck_Class_Type", &Pt->GameData.QuestDetail.iCheck_Class_Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_Class_Type));
		Reader.SetData("iCheck_Class1", &Pt->GameData.QuestDetail.iCheck_Class1, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_Class1));
		Reader.SetData("iCheck_Class2", &Pt->GameData.QuestDetail.iCheck_Class2, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_Class2));
		Reader.SetData("iCheck_LV", &Pt->GameData.QuestDetail.iCheck_LV, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_LV));
		Reader.SetData("iCheck_LowerLV", &Pt->GameData.QuestDetail.iCheck_LowerLV, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_LowerLV));
		Reader.SetData("iCheck_HigherLV", &Pt->GameData.QuestDetail.iCheck_HigherLV, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_HigherLV));
		Reader.SetData("iCheck_Title", &Pt->GameData.QuestDetail.iCheck_Title, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_Title));
		Reader.SetData("iCheck_Group", &Pt->GameData.QuestDetail.iCheck_Group, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_Group));
		Reader.SetData("iCheck_Sex", &Pt->GameData.QuestDetail.iCheck_Sex, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_Sex));
		Reader.SetData("iCheck_RepGroup", &Pt->GameData.QuestDetail.iCheck_RepGroup, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_RepGroup));
		Reader.SetData("iCheck_Rep", &Pt->GameData.QuestDetail.iCheck_Rep, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_Rep));
		Reader.SetData("iCheck_Flag1", &Pt->GameData.QuestDetail.iCheck_Flag[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_Flag[0]));
		Reader.SetData("iCheck_Flag2", &Pt->GameData.QuestDetail.iCheck_Flag[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_Flag[1]));
		Reader.SetData("iCheck_Flag3", &Pt->GameData.QuestDetail.iCheck_Flag[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_Flag[2]));
		Reader.SetData("iCheck_ObjID1", &Pt->GameData.QuestDetail.iCheck_ObjID[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_ObjID[0]));
		Reader.SetData("iCheck_ObjID2", &Pt->GameData.QuestDetail.iCheck_ObjID[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_ObjID[1]));
		Reader.SetData("iCheck_ObjID3", &Pt->GameData.QuestDetail.iCheck_ObjID[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_ObjID[2]));
		Reader.SetData("iCheck_ObjID4", &Pt->GameData.QuestDetail.iCheck_ObjID[3], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_ObjID[3]));
		Reader.SetData("iCheck_ObjID5", &Pt->GameData.QuestDetail.iCheck_ObjID[4], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_ObjID[4]));
		Reader.SetData("iCheck_ObjVal1", &Pt->GameData.QuestDetail.iCheck_ObjVal[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_ObjVal[0]));
		Reader.SetData("iCheck_ObjVal2", &Pt->GameData.QuestDetail.iCheck_ObjVal[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_ObjVal[1]));
		Reader.SetData("iCheck_ObjVal3", &Pt->GameData.QuestDetail.iCheck_ObjVal[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_ObjVal[2]));
		Reader.SetData("iCheck_ObjVal4", &Pt->GameData.QuestDetail.iCheck_ObjVal[3], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_ObjVal[3]));
		Reader.SetData("iCheck_ObjVal5", &Pt->GameData.QuestDetail.iCheck_ObjVal[4], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_ObjVal[4]));
		Reader.SetData("iCheck_Loop", &Pt->GameData.QuestDetail.iCheck_Loop, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_Loop));
		Reader.SetData("bDisableTeam", &Pt->GameData.QuestDetail.bDisableTeam, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.bDisableTeam));
		Reader.SetData("iAccept_FlagOn1", &Pt->GameData.QuestDetail.iAccept_FlagOn[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iAccept_FlagOn[0]));
		Reader.SetData("iAccept_FlagOn2", &Pt->GameData.QuestDetail.iAccept_FlagOn[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iAccept_FlagOn[1]));
		Reader.SetData("iAccept_FlagOn3", &Pt->GameData.QuestDetail.iAccept_FlagOn[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iAccept_FlagOn[2]));
		Reader.SetData("iAccept_FlagOff1", &Pt->GameData.QuestDetail.iAccept_FlagOff[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iAccept_FlagOff[0]));
		Reader.SetData("iAccept_FlagOff2", &Pt->GameData.QuestDetail.iAccept_FlagOff[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iAccept_FlagOff[1]));
		Reader.SetData("iAccept_FlagOff3", &Pt->GameData.QuestDetail.iAccept_FlagOff[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iAccept_FlagOff[2]));
		Reader.SetData("iAccept_ItemID1", &Pt->GameData.QuestDetail.iAccept_ItemID[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iAccept_ItemID[0]));
		Reader.SetData("iAccept_ItemID2", &Pt->GameData.QuestDetail.iAccept_ItemID[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iAccept_ItemID[1]));
		Reader.SetData("iAccept_ItemID3", &Pt->GameData.QuestDetail.iAccept_ItemID[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iAccept_ItemID[2]));
		Reader.SetData("iAccept_ItemID4", &Pt->GameData.QuestDetail.iAccept_ItemID[3], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iAccept_ItemID[3]));
		Reader.SetData("iAccept_ItemID5", &Pt->GameData.QuestDetail.iAccept_ItemID[4], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iAccept_ItemID[4]));
		Reader.SetData("iAccept_ItemVal1", &Pt->GameData.QuestDetail.iAccept_ItemVal[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iAccept_ItemVal[0]));
		Reader.SetData("iAccept_ItemVal2", &Pt->GameData.QuestDetail.iAccept_ItemVal[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iAccept_ItemVal[1]));
		Reader.SetData("iAccept_ItemVal3", &Pt->GameData.QuestDetail.iAccept_ItemVal[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iAccept_ItemVal[2]));
		Reader.SetData("iAccept_ItemVal4", &Pt->GameData.QuestDetail.iAccept_ItemVal[3], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iAccept_ItemVal[3]));
		Reader.SetData("iAccept_ItemVal5", &Pt->GameData.QuestDetail.iAccept_ItemVal[4], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iAccept_ItemVal[4]));
		Reader.SetData("iRequest_CoinType", &Pt->GameData.QuestDetail.iRequest_CoinType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iRequest_CoinType));
		Reader.SetData("iRequest_SpecialCoin", &Pt->GameData.QuestDetail.iRequest_SpecialCoin, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iRequest_SpecialCoin));
		Reader.SetData("iRequest_Timer", &Pt->GameData.QuestDetail.iRequest_Timer, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iRequest_Timer));
		Reader.SetData("iRequest_Money", &Pt->GameData.QuestDetail.iRequest_Money, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iRequest_Money));
		Reader.SetData("iRequest_QuestID", &Pt->GameData.QuestDetail.iRequest_QuestID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iRequest_QuestID));
		Reader.SetData("iReward_Money", &Pt->GameData.QuestDetail.iReward_Money, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_Money));
		Reader.SetData("iReward_Exp", &Pt->GameData.QuestDetail.iReward_Exp, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_Exp));
		Reader.SetData("iReward_Honor", &Pt->GameData.QuestDetail.iReward_Honor, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_Honor));
		Reader.SetData("iReward_TP", &Pt->GameData.QuestDetail.iReward_TP, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_TP));
		Reader.SetData("iReward_CoinType", &Pt->GameData.QuestDetail.iReward_CoinType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_CoinType));
		Reader.SetData("iReward_SpecialCoin", &Pt->GameData.QuestDetail.iReward_SpecialCoin, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_SpecialCoin));
		Reader.SetData("iReward_Rep1", &Pt->GameData.QuestDetail.iReward_Rep[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_Rep[0]));
		Reader.SetData("iReward_Rep2", &Pt->GameData.QuestDetail.iReward_Rep[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_Rep[1]));
		Reader.SetData("iReward_RepGroup1", &Pt->GameData.QuestDetail.iReward_RepGroup[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_RepGroup[0]));
		Reader.SetData("iReward_RepGroup2", &Pt->GameData.QuestDetail.iReward_RepGroup[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_RepGroup[1]));
		Reader.SetData("iReward_ItemID1", &Pt->GameData.QuestDetail.iReward_ItemID[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ItemID[0]));
		Reader.SetData("iReward_ItemID2", &Pt->GameData.QuestDetail.iReward_ItemID[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ItemID[1]));
		Reader.SetData("iReward_ItemID3", &Pt->GameData.QuestDetail.iReward_ItemID[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ItemID[2]));
		Reader.SetData("iReward_ItemID4", &Pt->GameData.QuestDetail.iReward_ItemID[3], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ItemID[3]));
		Reader.SetData("iReward_ItemID5", &Pt->GameData.QuestDetail.iReward_ItemID[4], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ItemID[4]));
		Reader.SetData("iReward_ItemVal1", &Pt->GameData.QuestDetail.iReward_ItemVal[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ItemVal[0]));
		Reader.SetData("iReward_ItemVal2", &Pt->GameData.QuestDetail.iReward_ItemVal[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ItemVal[1]));
		Reader.SetData("iReward_ItemVal3", &Pt->GameData.QuestDetail.iReward_ItemVal[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ItemVal[2]));
		Reader.SetData("iReward_ItemVal4", &Pt->GameData.QuestDetail.iReward_ItemVal[3], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ItemVal[3]));
		Reader.SetData("iReward_ItemVal5", &Pt->GameData.QuestDetail.iReward_ItemVal[4], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ItemVal[4]));
		Reader.SetData("iReward_ChoiceID1", &Pt->GameData.QuestDetail.iReward_ChoiceID[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ChoiceID[0]));
		Reader.SetData("iReward_ChoiceID2", &Pt->GameData.QuestDetail.iReward_ChoiceID[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ChoiceID[1]));
		Reader.SetData("iReward_ChoiceID3", &Pt->GameData.QuestDetail.iReward_ChoiceID[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ChoiceID[2]));
		Reader.SetData("iReward_ChoiceID4", &Pt->GameData.QuestDetail.iReward_ChoiceID[3], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ChoiceID[3]));
		Reader.SetData("iReward_ChoiceID5", &Pt->GameData.QuestDetail.iReward_ChoiceID[4], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ChoiceID[4]));
		Reader.SetData("iReward_ChoiceVal1", &Pt->GameData.QuestDetail.iReward_ChoiceVal[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ChoiceVal[0]));
		Reader.SetData("iReward_ChoiceVal2", &Pt->GameData.QuestDetail.iReward_ChoiceVal[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ChoiceVal[1]));
		Reader.SetData("iReward_ChoiceVal3", &Pt->GameData.QuestDetail.iReward_ChoiceVal[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ChoiceVal[2]));
		Reader.SetData("iReward_ChoiceVal4", &Pt->GameData.QuestDetail.iReward_ChoiceVal[3], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ChoiceVal[3]));
		Reader.SetData("iReward_ChoiceVal5", &Pt->GameData.QuestDetail.iReward_ChoiceVal[4], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_ChoiceVal[4]));
		Reader.SetData("iReward_Spell1", &Pt->GameData.QuestDetail.iReward_Spell[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_Spell[0]));
		Reader.SetData("iReward_Spell2", &Pt->GameData.QuestDetail.iReward_Spell[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_Spell[1]));
		Reader.SetData("iReward_Spell3", &Pt->GameData.QuestDetail.iReward_Spell[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_Spell[2]));
		Reader.SetData("iReward_Spell4", &Pt->GameData.QuestDetail.iReward_Spell[3], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_Spell[3]));
		Reader.SetData("iReward_Spell5", &Pt->GameData.QuestDetail.iReward_Spell[4], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iReward_Spell[4]));
		Reader.SetData("iComplete_ShowQuest", &Pt->GameData.QuestDetail.iComplete_ShowQuest, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iComplete_ShowQuest));
		Reader.SetData("iComplete_FlagOn1", &Pt->GameData.QuestDetail.iComplete_FlagOn[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iComplete_FlagOn[0]));
		Reader.SetData("iComplete_FlagOn2", &Pt->GameData.QuestDetail.iComplete_FlagOn[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iComplete_FlagOn[1]));
		Reader.SetData("iComplete_FlagOn3", &Pt->GameData.QuestDetail.iComplete_FlagOn[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iComplete_FlagOn[2]));
		Reader.SetData("iComplete_FlagOff1", &Pt->GameData.QuestDetail.iComplete_FlagOff[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iComplete_FlagOff[0]));
		Reader.SetData("iComplete_FlagOff2", &Pt->GameData.QuestDetail.iComplete_FlagOff[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iComplete_FlagOff[1]));
		Reader.SetData("iComplete_FlagOff3", &Pt->GameData.QuestDetail.iComplete_FlagOff[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iComplete_FlagOff[2]));
		Reader.SetData("szQuest_Desc", &Pt->GameData.QuestDetail.szQuest_Desc, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.szQuest_Desc));
		Reader.SetData("szQuest_Accept_Detail", &Pt->GameData.QuestDetail.szQuest_Accept_Detail, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.szQuest_Accept_Detail));
		Reader.SetData("szQuest_UnComplete_Detail", &Pt->GameData.QuestDetail.szQuest_UnComplete_Detail, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.szQuest_UnComplete_Detail));
		Reader.SetData("szQuest_Complete_Detail", &Pt->GameData.QuestDetail.szQuest_Complete_Detail, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.szQuest_Complete_Detail));
		Reader.SetData("szScript_OnBegin", &Pt->GameData.QuestDetail.szScript_OnBegin, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.szScript_OnBegin));
		Reader.SetData("szScript_OnAccept", &Pt->GameData.QuestDetail.szScript_OnAccept, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.szScript_OnAccept));
		Reader.SetData("szScript_OnComplete", &Pt->GameData.QuestDetail.szScript_OnComplete, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.szScript_OnComplete));
		Reader.SetData("szScript_OnDelete", &Pt->GameData.QuestDetail.szScript_OnDelete, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.szScript_OnDelete));
		Reader.SetData("szScript_OnClickObjBegin", &Pt->GameData.QuestDetail.szScript_OnClickObjBegin, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.szScript_OnClickObjBegin));
		Reader.SetData("szScript_OnClickObjEnd", &Pt->GameData.QuestDetail.szScript_OnClickObjEnd, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.szScript_OnClickObjEnd));
		Reader.SetData("szScript_RequestString", &Pt->GameData.QuestDetail.szScript_RequestString, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.szScript_RequestString));
		Reader.SetData("iQuestGroup", &Pt->GameData.QuestDetail.iQuestGroup, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iQuestGroup));
		Reader.SetData("iControlType", &Pt->GameData.QuestDetail.iControlType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iControlType));
		Reader.SetData("iQuestGroupLV", &Pt->GameData.QuestDetail.iQuestGroupLV, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iQuestGroupLV));
		for (int i = 0; i < _MAX_SIZE_QUEST_CONDITION; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "iRequest_ItemID%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iRequest_ItemID[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iRequest_ItemID[i]));
			sprintf_s(Buf, sizeof(Buf), "iRequest_ItemVal%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iRequest_ItemVal[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iRequest_ItemVal[i]));
			sprintf_s(Buf, sizeof(Buf), "iRequest_ItemStr%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iRequest_ItemStr[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iRequest_ItemStr[i]));
		}
		for (int i = 0; i < 10; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "iProcess_ClickToGetItem_ObjID%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iProcess_ClickToGetItem_ObjID[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iProcess_ClickToGetItem_ObjID[i]));
			sprintf_s(Buf, sizeof(Buf), "iProcess_ClickToGetItem_ProcPercent%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iProcess_ClickToGetItem_ProcPercent[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iProcess_ClickToGetItem_ProcPercent[i]));
			sprintf_s(Buf, sizeof(Buf), "iProcess_ClickToGetItem_GetItem%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iProcess_ClickToGetItem_GetItem[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iProcess_ClickToGetItem_GetItem[i]));
			sprintf_s(Buf, sizeof(Buf), "iProcess_ClickToGetItem_GetItemVal%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iProcess_ClickToGetItem_GetItemVal[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iProcess_ClickToGetItem_GetItemVal[i]));
		}
		for (int i = 0; i < 10; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "iProcess_KilltoGetItem_ObjID%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iProcess_KilltoGetItem_ObjID[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iProcess_KilltoGetItem_ObjID[i]));
			sprintf_s(Buf, sizeof(Buf), "iProcess_KilltoGetItem_ProcPercent%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iProcess_KilltoGetItem_ProcPercent[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iProcess_KilltoGetItem_ProcPercent[i]));
			sprintf_s(Buf, sizeof(Buf), "iProcess_KilltoGetItem_GetItem%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iProcess_KilltoGetItem_GetItem[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iProcess_KilltoGetItem_GetItem[i]));
			sprintf_s(Buf, sizeof(Buf), "iProcess_KilltoGetItem_GetItemVal%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iProcess_KilltoGetItem_GetItemVal[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iProcess_KilltoGetItem_GetItemVal[i]));
		}
		for (int i = 0; i < 5; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "iRequest_KillID%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iRequest_KillID[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iRequest_KillID[i]));
			sprintf_s(Buf, sizeof(Buf), "iRequest_KillVal%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iRequest_KillVal[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iRequest_KillVal[i]));
			sprintf_s(Buf, sizeof(Buf), "iRequest_KillStr%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iRequest_KillStr[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iRequest_KillStr[i]));
		}
		for (int i = 0; i < _MAX_SIZE_QUEST_ITEM; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "iDeleteQuestItem%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iDeleteQuestItem[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iDeleteQuestItem[i]));
			sprintf_s(Buf, sizeof(Buf), "iDeleteQuestItemCount%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iDeleteQuestItemCount[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iDeleteQuestItemCount[i]));
		}
		for (int i = 0; i < _MAX_SIZE_QUEST_ITEM; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "iQuestLinkNPC%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iQuestLinkNPC[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iQuestLinkNPC[i]));
		}
		for (int i = 0; i < _MAX_SIZE_QUEST_ITEM; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "iCheck_ObjStatus%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.QuestDetail.iCheck_ObjStatus[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.QuestDetail.iCheck_ObjStatus[i]));
		}
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_QuestNPC()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("Zone1", &Pt->Zone[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Zone[0]));
		Reader.SetData("Zone2", &Pt->Zone[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Zone[1]));
		Reader.SetData("Zone3", &Pt->Zone[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Zone[2]));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("szQuestNpcText", &Pt->GameData.NPC.szQuestNpcText, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.szQuestNpcText));
		Reader.SetData("szQuestDoingText", &Pt->GameData.NPC.szQuestDoingText, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.szQuestDoingText));
		Reader.SetData("szLuaScript", &Pt->GameData.NPC.szLuaScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.szLuaScript));
		Reader.SetData("iQuestID1", &Pt->GameData.NPC.iQuestID[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[0]));
		Reader.SetData("iQuestID2", &Pt->GameData.NPC.iQuestID[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[1]));
		Reader.SetData("iQuestID3", &Pt->GameData.NPC.iQuestID[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[2]));
		Reader.SetData("iQuestID4", &Pt->GameData.NPC.iQuestID[3], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[3]));
		Reader.SetData("iQuestID5", &Pt->GameData.NPC.iQuestID[4], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[4]));
		Reader.SetData("iQuestID6", &Pt->GameData.NPC.iQuestID[5], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[5]));
		Reader.SetData("iQuestID7", &Pt->GameData.NPC.iQuestID[6], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[6]));
		Reader.SetData("iQuestID8", &Pt->GameData.NPC.iQuestID[7], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[7]));
		Reader.SetData("iQuestID9", &Pt->GameData.NPC.iQuestID[8], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[8]));
		Reader.SetData("iQuestID10", &Pt->GameData.NPC.iQuestID[9], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[9]));
		Reader.SetData("iQuestID11", &Pt->GameData.NPC.iQuestID[10], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[10]));
		Reader.SetData("iQuestID12", &Pt->GameData.NPC.iQuestID[11], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[11]));
		Reader.SetData("iQuestID13", &Pt->GameData.NPC.iQuestID[12], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[12]));
		Reader.SetData("iQuestID14", &Pt->GameData.NPC.iQuestID[13], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[13]));
		Reader.SetData("iQuestID15", &Pt->GameData.NPC.iQuestID[14], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[14]));
		Reader.SetData("iQuestID16", &Pt->GameData.NPC.iQuestID[15], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[15]));
		Reader.SetData("iQuestID17", &Pt->GameData.NPC.iQuestID[16], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[16]));
		Reader.SetData("iQuestID18", &Pt->GameData.NPC.iQuestID[17], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[17]));
		Reader.SetData("iQuestID19", &Pt->GameData.NPC.iQuestID[18], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[18]));
		Reader.SetData("iQuestID20", &Pt->GameData.NPC.iQuestID[19], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestID[19]));
		Reader.SetData("iQuestMode", &Pt->GameData.NPC.iQuestMode, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestMode));
		Reader.SetData("ReviveTime", &Pt->GameData.ReviveTime, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ReviveTime));
		Reader.SetData("MainHandEq1", &Pt->GameData.NPC.MainHandEQ[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.MainHandEQ[0]));
		Reader.SetData("SecHandEq1", &Pt->GameData.NPC.SecHandEQ[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.SecHandEQ[0]));
		Reader.SetData("iQuestNPCRefID", &Pt->GameData.NPC.iQuestNPCRefID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestNPCRefID));
		Reader.SetData("iQuestObjType", &Pt->GameData.NPC.iQuestObjType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.iQuestObjType));
		Reader.SetData("TitleName", &Pt->GameData.NPC.TitleName, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.TitleName));
		Reader.SetData("QuestObjCraftMode", &Pt->GameData.NPC.QuestObjCraftMode, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.QuestObjCraftMode));
		Reader.SetData("ZoneID", &Pt->GameData.NPC.ZoneID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.NPC.ZoneID));
		Reader.SetData("X", &Pt->GameData.NPC.X, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.NPC.X));
		Reader.SetData("Y", &Pt->GameData.NPC.Y, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.NPC.Y));
		Reader.SetData("Z", &Pt->GameData.NPC.Z, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.NPC.Z));
		Reader.SetData("ZoneID_Bk", &Pt->GameData.NPC.ZoneID_Bk, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.ZoneID_Bk));
		Reader.SetData("CampID", &Pt->GameData.NPC.CampID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.CampID));
		Reader.SetData("szLuaInitScript", &Pt->GameData.NPC.szLuaInitScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.szLuaInitScript));
		Reader.SetData("OtherFlag", &Pt->GameData.NPC.OtherFlag, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.OtherFlag));
		Reader.SetData("NpcMode", &Pt->GameData.NPC._NpcMode, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC._NpcMode));
		Reader.SetData("szLuaDisplayScript", &Pt->GameData.NPC.szLuaDisplayScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.szLuaDisplayScript));
		Reader.SetData("OrnamentEQ", &Pt->GameData.NPC.OrnamentEQ, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.OrnamentEQ));
		Reader.SetData("SearchGroupID", &Pt->GameData.NPC.SearchGroupID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.NPC.SearchGroupID));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Recipe()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char    Buf[256];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("Rare", &Pt->GameData.Rare, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Rare));
		for (int i = 0; i < 8; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "Source%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Recipe.Source[i].OrgObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Source[i].OrgObjID));
			sprintf_s(Buf, sizeof(Buf), "SourceCount%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Recipe.Source[i].Count, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Source[i].Count));
			sprintf_s(Buf, sizeof(Buf), "SourceReduce%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Recipe.Source[i].Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Source[i].Rate));
		}
		Reader.SetData("Item1_Normal", &Pt->GameData.Recipe.Item1_Normal.OrgObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_Normal.OrgObjID));
		Reader.SetData("Item1_NormalCount", &Pt->GameData.Recipe.Item1_Normal.Count, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_Normal.Count));
		Reader.SetData("Item1_NormalRate", &Pt->GameData.Recipe.Item1_Normal.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_Normal.Rate));
		Reader.SetData("Item1_HQ", &Pt->GameData.Recipe.Item1_HQ.OrgObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_HQ.OrgObjID));
		Reader.SetData("Item1_HQCount", &Pt->GameData.Recipe.Item1_HQ.Count, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_HQ.Count));
		Reader.SetData("Item1_HQRate", &Pt->GameData.Recipe.Item1_HQ.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_HQ.Rate));
		Reader.SetData("Item1_Slot1", &Pt->GameData.Recipe.Item1_Slot1.OrgObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_Slot1.OrgObjID));
		Reader.SetData("Item1_Slot1Count", &Pt->GameData.Recipe.Item1_Slot1.Count, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_Slot1.Count));
		Reader.SetData("Item1_Slot1Rate", &Pt->GameData.Recipe.Item1_Slot1.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_Slot1.Rate));
		Reader.SetData("Item1_Slot2", &Pt->GameData.Recipe.Item1_Slot2.OrgObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_Slot2.OrgObjID));
		Reader.SetData("Item1_Slot2Count", &Pt->GameData.Recipe.Item1_Slot2.Count, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_Slot2.Count));
		Reader.SetData("Item1_Slot2Rate", &Pt->GameData.Recipe.Item1_Slot2.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_Slot2.Rate));
		Reader.SetData("Item1_Slot3", &Pt->GameData.Recipe.Item1_Slot3.OrgObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_Slot3.OrgObjID));
		Reader.SetData("Item1_Slot3Count", &Pt->GameData.Recipe.Item1_Slot3.Count, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_Slot3.Count));
		Reader.SetData("Item1_Slot3Rate", &Pt->GameData.Recipe.Item1_Slot3.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_Slot3.Rate));
		Reader.SetData("Item1_Slot4", &Pt->GameData.Recipe.Item1_Slot4.OrgObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_Slot4.OrgObjID));
		Reader.SetData("Item1_Slot4Count", &Pt->GameData.Recipe.Item1_Slot4.Count, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_Slot4.Count));
		Reader.SetData("Item1_Slot4Rate", &Pt->GameData.Recipe.Item1_Slot4.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item1_Slot4.Rate));
		Reader.SetData("Item2", &Pt->GameData.Recipe.Item2.OrgObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item2.OrgObjID));
		Reader.SetData("Item2_Count", &Pt->GameData.Recipe.Item2.Count, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item2.Count));
		Reader.SetData("Item2_Rate", &Pt->GameData.Recipe.Item2.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item2.Rate));
		Reader.SetData("Item3", &Pt->GameData.Recipe.Item3.OrgObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item3.OrgObjID));
		Reader.SetData("Item3_Count", &Pt->GameData.Recipe.Item3.Count, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item3.Count));
		Reader.SetData("Item3_Rate", &Pt->GameData.Recipe.Item3.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item3.Rate));
		Reader.SetData("Item4", &Pt->GameData.Recipe.Item4.OrgObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item4.OrgObjID));
		Reader.SetData("Item4_Count", &Pt->GameData.Recipe.Item4.Count, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item4.Count));
		Reader.SetData("Item4_Rate", &Pt->GameData.Recipe.Item4.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Item4.Rate));
		Reader.SetData("RequestSkill", &Pt->GameData.Recipe.Request.Skill, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Request.Skill));
		Reader.SetData("RequestSkillLV", &Pt->GameData.Recipe.Request.SkillLV, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Request.SkillLV));
		Reader.SetData("RequestKeyItemID", &Pt->GameData.Recipe.Request.KeyItemID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Request.KeyItemID));
		Reader.SetData("Coldown", &Pt->GameData.Recipe.Coldown, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Coldown));
		Reader.SetData("Type", &Pt->GameData.Recipe.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.Type));
		Reader.SetData("RequestCraftObjType", &Pt->GameData.Recipe.RequestCraftObjType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.RequestCraftObjType));
		Reader.SetData("ExpRate", &Pt->GameData.Recipe.ExpRate, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.ExpRate));
		Reader.SetData("Cost", &Pt->GameData.Cost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Cost));
		Reader.SetData("SellType", &Pt->GameData.SellType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.SellType));
		Reader.SetData("Mode", &Pt->GameData.Mode._Mode, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mode._Mode));
		Reader.SetData("LearnFee", &Pt->GameData.Recipe.LearnFee, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.LearnFee));
		Reader.SetData("CraftFee", &Pt->GameData.Recipe.CraftFee, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Recipe.CraftFee));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Rune()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char    Buf[256];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("Rare", &Pt->GameData.Rare, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Rare));
		Reader.SetData("MaxHeap", &Pt->GameData.MaxHeap, ENUM_Type_IntPoint, ENUM_Indirect_Address, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MaxHeap));
		Reader.SetData("DisappearRate", &Pt->GameData.Item.DisappearRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DisappearRate));
		Reader.SetData("DissolutionRate", &Pt->GameData.Item.DissolutionRate, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DissolutionRate));
		Reader.SetData("Type", &Pt->GameData.Item.RuneType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.RuneType));
		Reader.SetData("Level", &Pt->GameData.Item.Level, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Level));
		Reader.SetData("Volume", &Pt->GameData.Item.Volume, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Volume));
		Reader.SetData("RuneGroup", &Pt->GameData.Item.RuneGroup, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.RuneGroup));
		Reader.SetData("AddOnPos_ArmorPos", &Pt->GameData.Item.AddPos._ArmorPos, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.AddPos._ArmorPos));
		Reader.SetData("AddOnPos_ArmorType", &Pt->GameData.Item.AddPos._ArmorType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.AddPos._ArmorType));
		Reader.SetData("AddOnPos_WeaponType", &Pt->GameData.Item.AddPos._WeaponType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.AddPos._WeaponType));
		Reader.SetData("AddOnPos_WeaponPos", &Pt->GameData.Item.AddPos.WeaponPosEnum, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.AddPos.WeaponPosEnum));
		Reader.SetData("AddOnPos_Other", &Pt->GameData.Item.AddPos._Other, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.AddPos._Other));
		for (int i = 0; i < Max_WearEqType_Count; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "EqType%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Item.Ability.EqType[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Ability.EqType[i]));
			sprintf_s(Buf, sizeof(Buf), "EqTypeValue%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Item.Ability.EqTypeValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Ability.EqTypeValue[i]));
		}
		Reader.SetData("EqTypeOnAttackMagicID", &Pt->GameData.Item.Ability.OnAttackMagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Ability.OnAttackMagicID));
		Reader.SetData("EqTypeOnAttackMagicRank", &Pt->GameData.Item.Ability.OnAttackMagicRank, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Ability.OnAttackMagicRank));
		Reader.SetData("EqTypeOnAttackMagicRate", &Pt->GameData.Item.Ability.OnAttackMagicRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Ability.OnAttackMagicRate));
		Reader.SetData("ReferenceLv", &Pt->GameData.Item.ReferenceLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ReferenceLv));
		Reader.SetData("IsStandardCost", &Pt->GameData.Item.IsStandardCost, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.IsStandardCost));
		Reader.SetData("Cost", &Pt->GameData.Cost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Cost));
		Reader.SetData("PricesType", &Pt->GameData.PricesType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PricesType));
		Reader.SetData("SpecialMoneyCost", &Pt->GameData.SpecialMoneyCost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.SpecialMoneyCost));
		Reader.SetData("CardID", &Pt->GameData.Item.CardID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CardID));
		Reader.SetData("StarValue", &Pt->GameData.Item.StarValue, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.StarValue));
		Reader.SetData("DrawOutCost", &Pt->GameData.Item.DrawOutCost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DrawOutCost));
		Reader.SetData("IsStandardAbility", &Pt->GameData.Item.IsStandardAbility, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.IsStandardAbility));
		Reader.SetData("StandardAbility_Lv", &Pt->GameData.Item.StandardAbility_Lv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.StandardAbility_Lv));
		for (int i = 0; i < 5; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "StandardAbility_Arg%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Item.StandardAbility_Arg[i], ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.StandardAbility_Arg[i]));
			sprintf_s(Buf, sizeof(Buf), "StandardAbility_ArgType%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Item.StandardAbility_ArgType[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.StandardAbility_ArgType[i]));
		}
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Card()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("Rare", &Pt->GameData.Rare, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Rare));
		Reader.SetData("CardOrgObjID", &Pt->GameData.Item.CardOrgObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CardOrgObjID));
		Reader.SetData("Cost", &Pt->GameData.Cost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Cost));
		Reader.SetData("MaxHeap", &Pt->GameData.MaxHeap, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MaxHeap));
		Reader.SetData("ReferenceLv", &Pt->GameData.Item.ReferenceLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ReferenceLv));
		Reader.SetData("IsStandardCost", &Pt->GameData.Item.IsStandardCost, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.IsStandardCost));
		Reader.SetData("CardPriorityID", &Pt->GameData.Item.CardPriorityID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CardPriorityID));
		Reader.SetData("StarValue", &Pt->GameData.Attirbute.StarValue, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Attirbute.StarValue));
		Reader.SetData("CardAddpower", &Pt->GameData.Item.CardAddpower, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CardAddpower));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_PE()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("ParentGUID", &Pt->GameData.PE.ParentGUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PE.ParentGUID));
		Reader.SetData("Type", &Pt->GameData.PE.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PE.Type));
		Reader.SetData("Flag", &Pt->GameData.PE.Flag, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PE.Flag));
		Reader.SetData("Data", &Pt->GameData.PE.Data, ENUM_Type_BinaryData, sizeof(Pt->GameData.PE.Data), ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PE.Data));
		Reader.SetData("LuaEvent1", &Pt->GameData.PE.LuaEvent.All.LuaEvent1, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PE.LuaEvent.All.LuaEvent1));
		Reader.SetData("LuaEvent2", &Pt->GameData.PE.LuaEvent.All.LuaEvent2, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PE.LuaEvent.All.LuaEvent2));
		Reader.SetData("LuaEvent3", &Pt->GameData.PE.LuaEvent.All.LuaEvent3, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PE.LuaEvent.All.LuaEvent3));
		Reader.SetData("LuaEvent4", &Pt->GameData.PE.LuaEvent.All.LuaEvent4, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PE.LuaEvent.All.LuaEvent4));
		Reader.SetData("LuaEvent5", &Pt->GameData.PE.LuaEvent.All.LuaEvent5, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PE.LuaEvent.All.LuaEvent5));
		Reader.SetData("LuaEvent6", &Pt->GameData.PE.LuaEvent.All.LuaEvent6, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PE.LuaEvent.All.LuaEvent6));
		Reader.SetData("LuaEvent7", &Pt->GameData.PE.LuaEvent.All.LuaEvent7, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PE.LuaEvent.All.LuaEvent7));
		Reader.SetData("LuaEvent8", &Pt->GameData.PE.LuaEvent.All.LuaEvent8, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PE.LuaEvent.All.LuaEvent8));
		Reader.SetData("LuaEvent9", &Pt->GameData.PE.LuaEvent.All.LuaEvent9, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PE.LuaEvent.All.LuaEvent9));
		Reader.SetData("LuaEvent10", &Pt->GameData.PE.LuaEvent.All.LuaEvent10, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PE.LuaEvent.All.LuaEvent10));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Phantom()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char    Buf[512];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, 1, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 1, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 1, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("RankUpMaterial", &Pt->GameData.Phantom.RankUpMaterial, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Phantom.RankUpMaterial));
		Reader.SetData("LevelUpType", &Pt->GameData.Phantom.LevelUpType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Phantom.LevelUpType));
		Reader.SetData("Release", &Pt->GameData.Phantom.Release, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Phantom.Release));
		for (int i = 0; i < 5; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "PhantomEqType%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Phantom.abi_Phantom.EqType[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Phantom.abi_Phantom.EqType[i]));
			sprintf_s(Buf, sizeof(Buf), "PhantomEqValue%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Phantom.abi_Phantom.EqTypeValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Phantom.abi_Phantom.EqTypeValue[i]));
		}
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Script()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, 1, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 1, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 1, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("Type", &Pt->ScriptInfo.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ScriptInfo.Type));
		Reader.SetData("FuncName", &Pt->ScriptInfo.Name, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ScriptInfo.Name));
		Reader.SetData("FuncArg", &Pt->ScriptInfo.Arg, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ScriptInfo.Arg));
		Reader.SetData("Text", &Pt->ScriptInfo.Script, ENUM_Type_TextData, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ScriptInfo.Script));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Shop()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char    Buf[256];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("BuyRate", &Pt->GameData.Shop.BuyRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Shop.BuyRate));
		Reader.SetData("SellRate", &Pt->GameData.Shop.SellRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Shop.SellRate));
		Reader.SetData("WeaponRepair", &Pt->GameData.Shop.WeaponRepair, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Shop.WeaponRepair));
		for (int i = 0; i < 30; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "SellItemID%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Shop.SellItemID[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Shop.SellItemID[i]));
			sprintf_s(Buf, sizeof(Buf), "SellItemMax%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Shop.SellItemMax[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Shop.SellItemMax[i]));
			sprintf_s(Buf, sizeof(Buf), "SellItemPorduce%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Shop.SellItemPorduce[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Shop.SellItemPorduce[i]));
			sprintf_s(Buf, sizeof(Buf), "SellCost1_%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Shop.SellCost[i][0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Shop.SellCost[i][0]));
			sprintf_s(Buf, sizeof(Buf), "SellCost2_%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Shop.SellCost[i][1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Shop.SellCost[i][1]));
			sprintf_s(Buf, sizeof(Buf), "CostType1_%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Shop.CostType[i][0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Shop.CostType[i][0]));
			sprintf_s(Buf, sizeof(Buf), "CostType2_%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Shop.CostType[i][1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Shop.CostType[i][1]));
		}
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Suit()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char    Buf[256];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("TotalCount", &Pt->GameData.Suit.TotalCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Suit.TotalCount));
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 3; j++)
			{
				sprintf_s(Buf, sizeof(Buf), "BaseType%d_%d", i + 1, j + 1);
				Reader.SetData(Buf, &Pt->GameData.Suit.SuitBase[i].EqType[j], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Suit.SuitBase[i].EqType[j]));
				sprintf_s(Buf, sizeof(Buf), "BaseTypeValue%d_%d", i + 1, j + 1);
				Reader.SetData(Buf, &Pt->GameData.Suit.SuitBase[i].EqTypeValue[j], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Suit.SuitBase[i].EqTypeValue[j]));
			}
		for (int i = 0; i < 10; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "SuitList%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Suit.SuitList[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Suit.SuitList[i]));
		}
		for (int i = 0; i < 4; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "SuitISkillD%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Suit.SuitSkillID[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Suit.SuitSkillID[i]));
		}
		Reader.SetData("SuitSkillMoney", &Pt->GameData.Suit.SuitSkillMoney, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Suit.SuitSkillMoney));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Title()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char Buf[256];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("Mode", &Pt->GameData.Mode._Mode, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mode._Mode));
		Reader.SetData("Rare", &Pt->GameData.Rare, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Rare));
		Reader.SetData("Type", &Pt->GameData.Title.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.Type));
		Reader.SetData("Famous", &Pt->GameData.Title.Famous, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.Famous));
		Reader.SetData("MainJob", &Pt->GameData.Title.MainJob, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.MainJob));
		Reader.SetData("SubJob", &Pt->GameData.Title.SubJob, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.SubJob));
		Reader.SetData("MainJobLV", &Pt->GameData.Title.MainJobLV, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.MainJobLV));
		Reader.SetData("SubJobLV", &Pt->GameData.Title.SubJobLV, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.SubJobLV));
		Reader.SetData("KillCount_Type", &Pt->GameData.Title.KillCount.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.KillCount.Type));
		Reader.SetData("KillCount_KillGroup", &Pt->GameData.Title.KillCount.KillGroupID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.KillCount.KillGroupID));
		Reader.SetData("KillCount_Count", &Pt->GameData.Title.KillCount.Count, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.KillCount.Count));
		for (int i = 0; i < 5; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "EqType%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Title.Ability.EqType[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.Ability.EqType[i]));
			sprintf_s(Buf, sizeof(Buf), "EqTypeValue%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Title.Ability.EqTypeValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.Ability.EqTypeValue[i]));
		}
		Reader.SetData("TitleSysNote", &Pt->GameData.Title.TitleSysNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.TitleSysNote));
		for (int i = 0; i < 5; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "ExEqType%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Title.AbilityEx.EqType[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.AbilityEx.EqType[i]));
			sprintf_s(Buf, sizeof(Buf), "ExEqTypeValue%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Title.AbilityEx.EqTypeValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.AbilityEx.EqTypeValue[i]));
		}
		Reader.SetData("EqTypeOnAttackMagicID", &Pt->GameData.Title.Ability.OnAttackMagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.Ability.OnAttackMagicID));
		Reader.SetData("EqTypeOnAttackMagicRank", &Pt->GameData.Title.Ability.OnAttackMagicRank, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.Ability.OnAttackMagicRank));
		Reader.SetData("EqTypeOnAttackMagicRate", &Pt->GameData.Title.Ability.OnAttackMagicRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.Ability.OnAttackMagicRate));
		Reader.SetData("SkillID", &Pt->GameData.Title.SkillID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.SkillID));
		Reader.SetData("SkillLv", &Pt->GameData.Title.SkillLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.SkillLv));
		Reader.SetData("Classify1", &Pt->GameData.Title.Classify[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.Classify[0]));
		Reader.SetData("Classify2", &Pt->GameData.Title.Classify[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.Classify[1]));
		Reader.SetData("LuaEvent", &Pt->GameData.Title.LuaEvent, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.LuaEvent));
		Reader.SetData("TitleSysLuaEvent", &Pt->GameData.Title.TitleSysLuaEvent, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Title.TitleSysLuaEvent));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Weapon()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char    Buf[256];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, 1, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 1, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 1, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("MaxHeap", &Pt->GameData.MaxHeap, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MaxHeap));
		Reader.SetData("LimitVoc", &Pt->GameData.Limit.Voc._Voc, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.Voc._Voc));
		Reader.SetData("LimitRace", &Pt->GameData.Limit.Race._Race, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.Race._Race));
		Reader.SetData("LimitSex", &Pt->GameData.Limit.Sex._Sex, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.Sex._Sex));
		Reader.SetData("LimitLv", &Pt->GameData.Limit.Level, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.Level));
		Reader.SetData("LimitStr", &Pt->GameData.Limit.STR, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.STR));
		Reader.SetData("LimitSta", &Pt->GameData.Limit.STA, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.STA));
		Reader.SetData("LimitInt", &Pt->GameData.Limit.INT, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.INT));
		Reader.SetData("LimitMnd", &Pt->GameData.Limit.MND, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.MND));
		Reader.SetData("LimitAgi", &Pt->GameData.Limit.AGI, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Limit.AGI));
		Reader.SetData("Mode", &Pt->GameData.Mode._Mode, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Mode._Mode));
		Reader.SetData("ItemType", &Pt->GameData.Item.ItemType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ItemType));
		Reader.SetData("GenerateRate", &Pt->GameData.GenerateRate, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.GenerateRate));
		Reader.SetData("MaxWorldCount", &Pt->GameData.MaxWorldCount, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MaxWorldCount));
		Reader.SetData("Rare", &Pt->GameData.Rare, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Rare));
		Reader.SetData("Cost", &Pt->GameData.Cost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Cost));
		Reader.SetData("SpecialMoneyCost", &Pt->GameData.SpecialMoneyCost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.SpecialMoneyCost));
		Reader.SetData("PricesType", &Pt->GameData.PricesType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.PricesType));
		Reader.SetData("SellType", &Pt->GameData.SellType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.SellType));
		Reader.SetData("IncMagic_Onuse", &Pt->GameData.Item.IncMagic_Onuse, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.IncMagic_Onuse));
		Reader.SetData("DisappearRate", &Pt->GameData.Item.DisappearRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DisappearRate));
		Reader.SetData("DissolutionRate", &Pt->GameData.Item.DissolutionRate, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DissolutionRate));
		Reader.SetData("SrcRecipe", &Pt->GameData.Item.SrcRecipe, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.SrcRecipe));
		Reader.SetData("Durable", &Pt->GameData.Item.Durable, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Durable));
		Reader.SetData("ManuFactureType", &Pt->GameData.Item.ManuFactureType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ManuFactureType));
		Reader.SetData("ManuFactureLV", &Pt->GameData.Item.ManuFactureLV, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ManuFactureType));
		Reader.SetData("BuyUnit", &Pt->GameData.BuyUnit, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.BuyUnit));
		Reader.SetData("BodyMaxCount", &Pt->GameData.Item.BodyMaxCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.BodyMaxCount));
		Reader.SetData("SuitID", &Pt->GameData.Item.SuitID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.SuitID));
		Reader.SetData("AttackRange", &Pt->GameData.Item.AttackRange, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.AttackRange));
		Reader.SetData("AttackSpeed", &Pt->GameData.Item.AttackSpeed, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.AttackSpeed));
		for (int i = 2; i < Max_WearEqType_Count; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "EqType%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Item.Ability.EqType[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Ability.EqType[i]));
			sprintf_s(Buf, sizeof(Buf), "EqTypeValue%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Item.Ability.EqTypeValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Ability.EqTypeValue[i]));
		}
		for (int i = 0; i < _MAX_DROP_ABILITY_COUNT_; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "DropAbility%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Item.DropItemAbility[i].OrgID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DropItemAbility[i].OrgID));
			sprintf_s(Buf, sizeof(Buf), "DropAbilityRate%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Item.DropItemAbility[i].Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DropItemAbility[i].Rate));
		}
		Reader.SetData("EqTypeOnAttackMagicID", &Pt->GameData.Item.Ability.OnAttackMagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Ability.OnAttackMagicID));
		Reader.SetData("EqTypeOnAttackMagicRank", &Pt->GameData.Item.Ability.OnAttackMagicRank, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Ability.OnAttackMagicRank));
		Reader.SetData("EqTypeOnAttackMagicRate", &Pt->GameData.Item.Ability.OnAttackMagicRate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.Ability.OnAttackMagicRate));
		Reader.SetData("WeaponType", &Pt->GameData.Item.WeaponType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.WeaponType));
		Reader.SetData("weaponPos", &Pt->GameData.Item.WeaponPos, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.WeaponPos));
		Reader.SetData("HoleBase", &Pt->GameData.Item.HoleBase, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.HoleBase));
		Reader.SetData("HoleRange", &Pt->GameData.Item.HoleRange, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.HoleRange));
		Reader.SetData("MaxAttrCount", &Pt->GameData.Item.MaxAttrCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.MaxAttrCount));
		Reader.SetData("BaseAttrCount", &Pt->GameData.Item.BaseAttrCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.BaseAttrCount));
		Reader.SetData("SelectBaseAttrCount", &Pt->GameData.Item.SelectBaseAttrCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.SelectBaseAttrCount));
		Reader.SetData("UseType", &Pt->GameData.Item.UseType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.UseType));
		Reader.SetData("CliScript", &Pt->GameData.Item.CliScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CliScript));
		Reader.SetData("SrvScript", &Pt->GameData.Item.SrvScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.SrvScript));
		Reader.SetData("CheckUseScript", &Pt->GameData.Item.CheckUseScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CheckUseScript));
		Reader.SetData("LiveTimeType", &Pt->GameData.LiveTimeType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LiveTimeType));
		Reader.SetData("LiveTime", &Pt->GameData.LiveTime, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LiveTime));
		Reader.SetData("IsStandardCost", &Pt->GameData.Item.IsStandardCost, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.IsStandardCost));
		Reader.SetData("ImageEfficacyType", &Pt->GameData.Item.ImageEfficacyType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ImageEfficacyType));
		Reader.SetData("CardID", &Pt->GameData.Item.CardID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.CardID));
		Reader.SetData("IsFixDurable", &Pt->GameData.Item.IsFixDurable, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.IsFixDurable));
		Reader.SetData("StarValue", &Pt->GameData.Item.StarValue, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.StarValue));
		Reader.SetData("RuneLimetLv", &Pt->GameData.Item.RuneLimetLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.RuneLimetLv));
		Reader.SetData("ItemUniqueType", &Pt->GameData.Item.ItemUniqueType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.ItemUniqueType));
		Reader.SetData("RefineTableID", &Pt->GameData.Item.RefineTableID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.RefineTableID));
		Reader.SetData("RefineTableID1", &Pt->GameData.Item.RefineTableID1, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.RefineTableID1));
		Reader.SetData("DDmg", &Pt->GameData.Item.DDmg, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DDmg));
		Reader.SetData("DMDmg", &Pt->GameData.Item.DMDmg, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.DMDmg));
		Reader.SetData("BodyMaxCount", &Pt->GameData.Item.BodyMaxCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.BodyMaxCount));
		Reader.SetData("RefineLv", &Pt->GameData.Item.RefineLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.RefineLv));
		Reader.SetData("AddPowerQuality", &Pt->GameData.Item.AddPowerQuality, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Item.AddPowerQuality));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Camp()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char Buf[256];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		for (int i = 0; i < 15; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "CampName%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Camp.CampName[i], ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Camp.CampName[i]));
			sprintf_s(Buf, sizeof(Buf), "CampRelation%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Camp.CampInfo[i], ENUM_Type_CharString, sizeof(Pt->GameData.Camp.CampInfo[i]) - 1, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Camp.CampInfo[i]));
		}
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_Treasure()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char Buf[256];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("Cost", &Pt->GameData.Cost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Cost));
		Reader.SetData("NeedDLv", &Pt->GameData.Treasure.NeedDLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Treasure.NeedDLv));
		Reader.SetData("ItemCount", &Pt->GameData.Treasure.ItemCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Treasure.ItemCount));
		for (int i = 0; i < _MAX_TREASURE_COUNT_; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "TreasureItem%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Treasure.Item[i].OrgID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Treasure.Item[i].OrgID));
			sprintf_s(Buf, sizeof(Buf), "TreasureCount%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Treasure.Item[i].Count, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Treasure.Item[i].Count));
			sprintf_s(Buf, sizeof(Buf), "TreasureDropCount%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Treasure.Item[i].DropCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Treasure.Item[i].DropCount));
		}
		Reader.SetData("GambleCost", &Pt->GameData.Treasure.Gamble.Cost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Treasure.Gamble.Cost));
		Reader.SetData("GambleHole1Rate", &Pt->GameData.Treasure.Gamble.HoleRate[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Treasure.Gamble.HoleRate[0]));
		Reader.SetData("GambleHole2Rate", &Pt->GameData.Treasure.Gamble.HoleRate[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Treasure.Gamble.HoleRate[1]));
		Reader.SetData("GambleHole3Rate", &Pt->GameData.Treasure.Gamble.HoleRate[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Treasure.Gamble.HoleRate[2]));
		Reader.SetData("GambleHole4Rate", &Pt->GameData.Treasure.Gamble.HoleRate[3], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Treasure.Gamble.HoleRate[3]));
		for (int i = 0; i < _MAX_DROP_ABILITY_COUNT_; i++)
		{
			sprintf_s(Buf, sizeof(Buf), "GambleDropAbility%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Treasure.Gamble.DropItemAbility[i].OrgID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Treasure.Gamble.DropItemAbility[i].OrgID));
			sprintf_s(Buf, sizeof(Buf), "GambleDropAbilityRate%d", i + 1);
			Reader.SetData(Buf, &Pt->GameData.Treasure.Gamble.DropItemAbility[i].Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Treasure.Gamble.DropItemAbility[i].Rate));
		}
		Reader.SetData("LuaCheckScript", &Pt->GameData.Treasure.LuaCheckScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Treasure.LuaCheckScript));
	}
	return &Reader;
}
ReaderClass< DB_GameObjDbStruct >* GameObjDbStructEx::ObjReader_MagicCollect()
{
	static ReaderClass< DB_GameObjDbStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char Buf[256];
		DB_GameObjDbStruct* Pt = 0;
		IsInit = true;
		ObjReader_InitBaseCommonObj(Pt, Reader);
		Reader.SetData("IsLock", &Pt->IsLock, ENUM_Type_ShortPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->IsLock));
		Reader.SetData("LastModifyDate", &Pt->LastModifyDate, ENUM_Type_TimeCharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyDate));
		Reader.SetData("LastModifyAccount", &Pt->LastModifyAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LastModifyAccount));
		Reader.SetData("DataGroup", &Pt->DataGroup, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DataGroup));
		Reader.SetData("LockAccount", &Pt->LockAccount, ENUM_Type_CharString, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->LockAccount));
		Reader.SetData("ProcRate", &Pt->ProcRate.Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProcRate.Rate));
		Reader.SetData("OutputLanguage", &Pt->GameData.LanguageSet, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.LanguageSet));
		Reader.SetData("GUID", &Pt->GameData.GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, Def_Disabled_Update_Flag, sizeof(Pt->GameData.GUID));
		Reader.SetData("Name", &Pt->GameData.Name, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name));
		Reader.SetData("Name_Plural", &Pt->GameData.Name_Plural, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Name_Plural));
		Reader.SetData("ImageID", &Pt->GameData.ImageID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ImageID));
		Reader.SetData("Note", &Pt->GameData.Note, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.Note));
		Reader.SetData("ShortNote", &Pt->GameData.ShortNote, ENUM_Type_CharStringPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.ShortNote));
		Reader.SetData("MagicLV", &Pt->GameData.MagicCol.MagicLV, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.MagicLV));
		Reader.SetData("EffectType", &Pt->GameData.MagicCol.EffectType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.EffectType));
		Reader.SetData("CostType", &Pt->GameData.MagicCol.Cost[0].Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Cost[0].Type));
		Reader.SetData("CostValue", &Pt->GameData.MagicCol.Cost[0].Value, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Cost[0].Value));
		Reader.SetData("CostType1", &Pt->GameData.MagicCol.Cost[1].Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Cost[1].Type));
		Reader.SetData("CostValue1", &Pt->GameData.MagicCol.Cost[1].Value, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Cost[1].Value));
		Reader.SetData("NeedType1", &Pt->GameData.MagicCol.Need[0].Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Need[0].Type));
		Reader.SetData("NeedValue1", &Pt->GameData.MagicCol.Need[0].Value, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Need[0].Value));
		Reader.SetData("NeedType2", &Pt->GameData.MagicCol.Need[1].Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Need[1].Type));
		Reader.SetData("NeedValue2", &Pt->GameData.MagicCol.Need[1].Value, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Need[1].Value));
		Reader.SetData("TargetType", &Pt->GameData.MagicCol.TargetType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.TargetType));
		Reader.SetData("AttackDistance", &Pt->GameData.MagicCol.AttackDistance, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.AttackDistance));
		Reader.SetData("RangeType", &Pt->GameData.MagicCol.RangeType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.RangeType));
		Reader.SetData("EffectRange", &Pt->GameData.MagicCol.EffectRange, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.EffectRange));
		Reader.SetData("RangeSelectType", &Pt->GameData.MagicCol.RangeSelectType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.RangeSelectType));
		Reader.SetData("EffectCount", &Pt->GameData.MagicCol.EffectCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.EffectCount));
		Reader.SetData("DecreaseArg", &Pt->GameData.MagicCol.DecreaseArg, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.DecreaseArg));
		Reader.SetData("SpellTime", &Pt->GameData.MagicCol.SpellTime, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.SpellTime));
		Reader.SetData("MagicAttackDelay", &Pt->GameData.MagicCol.MagicAttackDelay, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.MagicAttackDelay));
		Reader.SetData("SpellCount", &Pt->GameData.MagicCol.SpellCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.SpellCount));
		Reader.SetData("NextSpellTime", &Pt->GameData.MagicCol.NextSpellTime, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.NextSpellTime));
		Reader.SetData("HitRateFunc", &Pt->GameData.MagicCol.HitRateFunc, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.HitRateFunc));
		Reader.SetData("HitRateArg1", &Pt->GameData.MagicCol.HitRateArg[0], ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.HitRateArg[0]));
		Reader.SetData("HitRateArg2", &Pt->GameData.MagicCol.HitRateArg[1], ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.HitRateArg[1]));
		Reader.SetData("ColdownClass", &Pt->GameData.MagicCol.Coldown.Class, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Coldown.Class));
		Reader.SetData("ColdownType", &Pt->GameData.MagicCol.Coldown.Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Coldown.Type));
		Reader.SetData("ColdownTime", &Pt->GameData.MagicCol.Coldown.Time, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Coldown.Time));
		Reader.SetData("ColdownTimeAllMagic", &Pt->GameData.MagicCol.Coldown.TimeAllMagic, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Coldown.TimeAllMagic));
		Reader.SetData("Flag", &Pt->GameData.MagicCol.Flag._Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Flag._Type));
		Reader.SetData("LuaScript", &Pt->GameData.MagicCol.LUAScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.LUAScript));
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 4; j++)
			{
				int ID = i * 4 + j + 1;
				sprintf_s(Buf, sizeof(Buf), "MagicBaseID%d", ID);
				Reader.SetData(Buf, &Pt->GameData.MagicCol.Magic[i][j].MagicBaseID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Magic[i][j].MagicBaseID));
				sprintf_s(Buf, sizeof(Buf), "MagicCheckFuncType%d", ID);
				Reader.SetData(Buf, &Pt->GameData.MagicCol.Magic[i][j].FuncType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Magic[i][j].FuncType));
				sprintf_s(Buf, sizeof(Buf), "MagicCheckArg1%d", ID);
				Reader.SetData(Buf, &Pt->GameData.MagicCol.Magic[i][j].Arg[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Magic[i][j].Arg[0]));
				sprintf_s(Buf, sizeof(Buf), "MagicCheckArg2%d", ID);
				Reader.SetData(Buf, &Pt->GameData.MagicCol.Magic[i][j].Arg[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.Magic[i][j].Arg[1]));
			}
		Reader.SetData("MagicSpellGroupType", &Pt->GameData.MagicCol.MagicSpellGroupType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.MagicSpellGroupType));
		Reader.SetData("CheckUseScript", &Pt->GameData.MagicCol.CheckUseScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.CheckUseScript));
		Reader.SetData("MaxSkillLv", &Pt->GameData.MagicCol.MaxSkillLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.MaxSkillLv));
		Reader.SetData("ExpTableRate", &Pt->GameData.MagicCol.ExpTableRate, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.ExpTableRate));
		Reader.SetData("IsMagicAttackDelay_TargetDist", &Pt->GameData.MagicCol.IsMagicAttackDelay_TargetDist, ENUM_Type_BitPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.IsMagicAttackDelay_TargetDist));
		Reader.SetData("ComboType", &Pt->GameData.MagicCol.ComboType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.ComboType));
		Reader.SetData("Mode", &Pt->GameData.MagicCol._Mode, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol._Mode));
		Reader.SetData("NextMagicID", &Pt->GameData.MagicCol.NextMagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.NextMagicID));
		Reader.SetData("PetSkillType", &Pt->GameData.MagicCol.PetSkillType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.PetSkillType));
		Reader.SetData("MoveDelayTime", &Pt->GameData.MagicCol.MoveDelayTime, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.MoveDelayTime));
		Reader.SetData("CliCollisionType", &Pt->GameData.MagicCol.CliCollisionType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.CliCollisionType));
		Reader.SetData("CliCollisionValue", &Pt->GameData.MagicCol.CliCollisionValue, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.CliCollisionValue));
		Reader.SetData("LimitLv", &Pt->GameData.MagicCol.LimitLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.LimitLv));
		Reader.SetData("SelectTargetScript", &Pt->GameData.MagicCol.SelectTargetScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.SelectTargetScript));
		Reader.SetData("ShootTargetScript", &Pt->GameData.MagicCol.ShootTargetScript, ENUM_Type_CharString, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.ShootTargetScript));
		Reader.SetData("AddSoulPoint", &Pt->GameData.MagicCol.AddSoulPoint, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.AddSoulPoint));
		Reader.SetData("SuitPassiveSkill", &Pt->GameData.MagicCol.SuitPassiveSkill, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GameData.MagicCol.SuitPassiveSkill));
	}
	return &Reader;
}
bool    GameObjDbStructEx::CheckInherentAbility(AddonPosTypeStruct& Pos, bool CheckLv)
{
	if (CheckLv != false && Limit.Level >= Pos.Level)
		return false;
	if (IsArmor())
	{
		bool IsNeedCheckType = false;
		switch (Item.ArmorPos)
		{
		case EM_ArmorPos_Head:
			if (Pos.ArmorPos.Head == false)
				return false;
			IsNeedCheckType = true;
			break;
		case EM_ArmorPos_Gloves:
			if (Pos.ArmorPos.Gloves == false)
				return false;
			IsNeedCheckType = true;
			break;
		case EM_ArmorPos_Shoes:
			if (Pos.ArmorPos.Shoes == false)
				return false;
			IsNeedCheckType = true;
			break;
		case EM_ArmorPos_Clothes:
			if (Pos.ArmorPos.Clothes == false)
				return false;
			IsNeedCheckType = true;
			break;
		case EM_ArmorPos_Pants:
			if (Pos.ArmorPos.Pants == false)
				return false;
			IsNeedCheckType = true;
			break;
		case EM_ArmorPos_Back:
			if (Pos.Other.Back == false)
				return false;
			break;
		case EM_ArmorPos_Belt:
			if (Pos.ArmorPos.Belt == false)
				return false;
			IsNeedCheckType = true;
			break;
		case EM_ArmorPos_Shoulder:
			if (Pos.ArmorPos.Shoulder == false)
				return false;
			IsNeedCheckType = true;
			break;
		case EM_ArmorPos_Necklace:
			if (Pos.Other.Necklace == false)
				return false;
			break;
		case EM_ArmorPos_Ring:
			if (Pos.Other.Ring == false)
				return false;
			break;
		case EM_ArmorPos_Earring:
			if (Pos.Other.Earring == false)
				return false;
			break;
		case EM_ArmorPos_SecondHand:
			if (Item.ArmorType == EM_Armor_Shield && Pos.Other.Shield == false)
				return false;
			if (Item.ArmorType == EM_Armor_Implement && Pos.Other.Implement == false)
				return false;
			break;
		case EM_ArmorPos_MagicTool:
			return false;
		case EM_ArmorPos_Ornament:
			return true;
		}
		if (IsNeedCheckType)
		{
			switch (Item.ArmorType)
			{
			case EM_Armor_HeavyArmed:
				if (Pos.ArmorType.HeavyArmed == false)
					return false;
				break;
			case EM_Armor_LightArmed:
				if (Pos.ArmorType.LightArmed == false)
					return false;
				break;
			case EM_Armor_Leather:
				if (Pos.ArmorType.Leather == false)
					return false;
				break;
			case EM_Armor_Clothes:
				if (Pos.ArmorType.Clothes == false)
					return false;
				break;
			case EM_Armor_Robe:
				if (Pos.ArmorType.Robe == false)
					return false;
				break;
			}
		}
	}
	else if (IsWeapon())
	{
		switch (Pos.WeaponPosEnum)
		{
		case EM_AddonPos_WeaponLimitPos_None:
			break;
		case EM_AddonPos_WeaponLimitPos_MainHand:
		{
			switch (Item.WeaponPos)
			{
			case EM_WeaponPos_MainHand:
			case EM_WeaponPos_OneHand:
			case EM_WeaponPos_TwoHand:
				break;
			default:
				return false;
			}
		}
		break;
		case EM_AddonPos_WeaponLimitPos_OffHand:
		{
			switch (Item.WeaponPos)
			{
			case EM_WeaponPos_SecondHand:
			case EM_WeaponPos_OneHand:
				break;
			default:
				return false;
			}
		}
		break;
		}
		switch (Item.WeaponType)
		{
		case EM_Weapon_Unarmed:
			if (Pos.WeaponType.Unarmed == false)
				return false;
			break;
		case EM_Weapon_Blade:
			if (Pos.WeaponType.Blade == false)
				return false;
			break;
		case EM_Weapon_Dagger:
			if (Pos.WeaponType.Dagger == false)
				return false;
			break;
		case EM_Weapon_Wand:
			if (Pos.WeaponType.Wand == false)
				return false;
			break;
		case EM_Weapon_Axe:
			if (Pos.WeaponType.Axe == false)
				return false;
			break;
		case EM_Weapon_Bludgeon:
			if (Pos.WeaponType.Bludgeon == false)
				return false;
			break;
		case EM_Weapon_Claymore:
			if (Pos.WeaponType.Claymore == false)
				return false;
			break;
		case EM_Weapon_Staff:
			if (Pos.WeaponType.Staff == false)
				return false;
			break;
		case EM_Weapon_2H_Axe:
			if (Pos.WeaponType.Axe_2H == false)
				return false;
			break;
		case EM_Weapon_2H_Hammer:
			if (Pos.WeaponType.Hammer_2H == false)
				return false;
			break;
		case EM_Weapon_Polearm:
			if (Pos.WeaponType.Polearm == false)
				return false;
			break;
		case EM_Weapon_Bow:
			if (Pos.WeaponType.Bow == false)
				return false;
			break;
		case EM_Weapon_CossBow:
			if (Pos.WeaponType.CossBow == false)
				return false;
			break;
		case EM_Weapon_Gun:
			if (Pos.WeaponType.Gun == false)
				return false;
			break;
		}
	}
	return true;
}
bool GameObjDbStructEx::CheckInherentAbility(GameObjDbStructEx* AbilityDB, bool CheckLv)
{
	if (AbilityDB->IsAttribute() == false)
		return false;
	if (Item.MaxAttrCount == 0)
		return false;
	return CheckInherentAbility(AbilityDB->Attirbute.AddPos, CheckLv);
}
ReaderClass< CultivatePetLearnMagicTableStruct >* GameObjDbStructEx::ObjReader_CultivatePetLearnMagic()
{
	static ReaderClass< CultivatePetLearnMagicTableStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		CultivatePetLearnMagicTableStruct* Pt = 0;
		IsInit = true;
		Reader.SetData("GUID", &Pt->GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID));
		Reader.SetData("MagicID", &Pt->MagicID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->MagicID));
		Reader.SetData("Limit_PetProperty", &Pt->Limit.PetProperty, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Limit.PetProperty));
		Reader.SetData("Limit_PetType", &Pt->Limit.PetType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Limit.PetType));
		Reader.SetData("Limit_Lv", &Pt->Limit.Lv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Limit.Lv));
		Reader.SetData("Limit_Str", &Pt->Limit.Str, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Limit.Str));
		Reader.SetData("Limit_Sta", &Pt->Limit.Sta, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Limit.Sta));
		Reader.SetData("Limit_Int", &Pt->Limit.Int, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Limit.Int));
		Reader.SetData("Limit_Mnd", &Pt->Limit.Mnd, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Limit.Mnd));
		Reader.SetData("Limit_Agi", &Pt->Limit.Agi, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Limit.Agi));
		Reader.SetData("KeyItem", &Pt->Limit.KeyItem, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Limit.KeyItem));
		Reader.SetData("PowerLv", &Pt->PowerLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->PowerLv));
		Reader.SetData("SkillPoint", &Pt->SkillPoint, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->SkillPoint));
		Reader.SetData("Lock", &Pt->Limit.Lock, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Limit.Lock));
	}
	return &Reader;
}
ReaderClass< CultivatePetLifeSkillTableStruct >* GameObjDbStructEx::ObjReader_CultivatePetLifeSkill()
{
	static ReaderClass< CultivatePetLifeSkillTableStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		CultivatePetLifeSkillTableStruct* Pt = 0;
		Reader.SetData("GUID", &Pt->GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID));
		Reader.SetData("Lv", &Pt->Lv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Lv));
		Reader.SetData("SkillType", &Pt->SkillType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->SkillType));
		Reader.SetData("NeedSkillLv", &Pt->Need.SkillLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Need.SkillLv));
		Reader.SetData("NeedToolID", &Pt->Need.ToolID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Need.ToolID));
		Reader.SetData("NeedItemID", &Pt->Need.ItemID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Need.ItemID));
		Reader.SetData("NeedItemCount", &Pt->Need.ItemCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Need.ItemCount));
		Reader.SetData("ProductItemID", &Pt->Product.ItemID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Product.ItemID));
		Reader.SetData("EventTime", &Pt->EventTime, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->EventTime));
		IsInit = true;
	}
	return &Reader;
}
ReaderClass< GuildHouseNpcReplaceTableStruct >* GameObjDbStructEx::ObjReader_GuildHouseNpcReplace()
{
	static ReaderClass< GuildHouseNpcReplaceTableStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		GuildHouseNpcReplaceTableStruct* Pt = 0;
		Reader.SetData("GUID", &Pt->GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID));
		Reader.SetData("Type", &Pt->Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Type));
		Reader.SetData("OrgNpcID", &Pt->OrgNpcID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->OrgNpcID));
		Reader.SetData("NpcID", &Pt->NpcID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->NpcID));
		IsInit = true;
	}
	return &Reader;
}
ReaderClass< GuildHouseNpcReplaceCostTableStruct >* GameObjDbStructEx::ObjReader_GuildHouseNpcReplaceCost()
{
	static ReaderClass< GuildHouseNpcReplaceCostTableStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		GuildHouseNpcReplaceCostTableStruct* Pt = 0;
		Reader.SetData("GUID", &Pt->GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID));
		Reader.SetData("Type", &Pt->Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Type));
		Reader.SetData("GuildLv", &Pt->GuildLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GuildLv));
		Reader.SetData("Resource_Gold", &Pt->Cost.Gold, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Cost.Gold));
		Reader.SetData("Resource_BonusGold", &Pt->Cost.BonusGold, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Cost.BonusGold));
		Reader.SetData("Resource_Mine", &Pt->Cost.Mine, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Cost.Mine));
		Reader.SetData("Resource_Wood", &Pt->Cost.Wood, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Cost.Wood));
		Reader.SetData("Resource_Herb", &Pt->Cost.Herb, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Cost.Herb));
		Reader.SetData("Resource_GuildRune", &Pt->Cost.GuildRune, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Cost.GuildRune));
		Reader.SetData("Resource_GuildStone", &Pt->Cost.GuildStone, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Cost.GuildStone));
		IsInit = true;
	}
	return &Reader;
}
ReaderClass< PlantProductTableStruct >* GameObjDbStructEx::ObjReader_PlantProduct()
{
	static ReaderClass< PlantProductTableStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		PlantProductTableStruct* Pt = 0;
		Reader.SetData("GUID", &Pt->GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID));
		Reader.SetData("SeedObjID", &Pt->SeedObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->SeedObjID));
		Reader.SetData("Env_RarePoint", &Pt->Env_RarePoint, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Env_RarePoint));
		Reader.SetData("Env_GrowPoint", &Pt->Env_GrowPoint, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Env_GrowPoint));
		Reader.SetData("DropID", &Pt->DropID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DropID));
		Reader.SetData("DropID_DLv5", &Pt->DropID_DLv5, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DropID_DLv5));
		Reader.SetData("DropID_DLv10", &Pt->DropID_DLv10, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->DropID_DLv10));
		Reader.SetData("ExpRate", &Pt->ExpRate, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ExpRate));
		Reader.SetData("LuaScript", &Pt->LuaScript, ENUM_Type_CharString, sizeof(Pt->LuaScript), ENUM_Indirect_Address, 0, sizeof(Pt->LuaScript));
		IsInit = true;
	}
	return &Reader;
}
ReaderClass< GambleItemStateTableStruct >* GameObjDbStructEx::ObjReader_GambleItemState()
{
	static ReaderClass< GambleItemStateTableStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		GambleItemStateTableStruct* Pt = 0;
		Reader.SetData("GambleObjID", &Pt->GambleObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GambleObjID));
		Reader.SetData("FirstTimeCost", &Pt->FirstTimeCost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->FirstTimeCost));
		Reader.SetData("ContinueCost", &Pt->ContinueCost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ContinueCost));
		Reader.SetData("MaxLv", &Pt->MaxLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->MaxLv));
		Reader.SetData("MinLv", &Pt->MinLv, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->MinLv));
		IsInit = true;
	}
	return &Reader;
}
ReaderClass< RuneSystemStruct >* GameObjDbStructEx::ObjReader_RuneSystem()
{
	static ReaderClass< RuneSystemStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		RuneSystemStruct* Pt = 0;
		Reader.SetData("GUID", &Pt->GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID));
		Reader.SetData("MagicCol", &Pt->MagicCol, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->MagicCol));
		Reader.SetData("MagicColEx", &Pt->MagicColEx, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->MagicColEx));
		Reader.SetData("TriggerCount", &Pt->TriggerCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->TriggerCount));
		Reader.SetData("RuneColor1", &Pt->RuneColor[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->RuneColor[0]));
		Reader.SetData("RuneColor2", &Pt->RuneColor[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->RuneColor[1]));
		Reader.SetData("RuneColor3", &Pt->RuneColor[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->RuneColor[2]));
		Reader.SetData("RuneColor4", &Pt->RuneColor[3], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->RuneColor[3]));
		Reader.SetData("RuneColor5", &Pt->RuneColor[4], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->RuneColor[4]));
		Reader.SetData("RuneColor6", &Pt->RuneColor[5], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->RuneColor[5]));
		IsInit = true;
	}
	return &Reader;
}
ReaderClass< PetCollectionTableStruct >* GameObjDbStructEx::ObjReader_PetCollection()
{
	static ReaderClass< PetCollectionTableStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		PetCollectionTableStruct* Pt = 0;
		Reader.SetData("GUID_NEW", &Pt->GUID_NEW, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID_NEW));
		Reader.SetData("GUID", &Pt->GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID));
		Reader.SetData("ObjID", &Pt->ObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ObjID));
		IsInit = true;
	}
	return &Reader;
}
ReaderClass< AbiParametersTableStruct >* GameObjDbStructEx::ObjReader_AbiParameters()
{
	static ReaderClass< AbiParametersTableStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		AbiParametersTableStruct* Pt = 0;
		Reader.SetData("GUID", &Pt->GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID));
		Reader.SetData("Type", &Pt->Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Type));
		Reader.SetData("Value", &Pt->Value, ENUM_Type_FloatPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Value));
		IsInit = true;
	}
	return &Reader;
}
ReaderClass< PhantomRankTableStruct >* GameObjDbStructEx::ObjReader_PhantomRank()
{
	static ReaderClass< PhantomRankTableStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		PhantomRankTableStruct* Pt = 0;
		Reader.SetData("GUID", &Pt->GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID));
		Reader.SetData("Rank", &Pt->Rank, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Rank));
		Reader.SetData("MaxLevel", &Pt->MaxLevel, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->MaxLevel));
		Reader.SetData("NeedFragment", &Pt->NeedFragment, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->NeedFragment));
		IsInit = true;
	}
	return &Reader;
}
ReaderClass< PhantomLvTableStruct >* GameObjDbStructEx::ObjReader_PhantomLv()
{
	static ReaderClass< PhantomLvTableStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char Buf[256];
		PhantomLvTableStruct* Pt = 0;
		Reader.SetData("GUID", &Pt->GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID));
		Reader.SetData("LV", &Pt->LV, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LV));
		Reader.SetData("Type", &Pt->Type, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Type));
		for (int i = 0; i < 4; i++)
		{
			sprintf(Buf, "Material%d", i + 1);
			Reader.SetData(Buf, &Pt->Material[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Material[i]));
		}
		IsInit = true;
	}
	return &Reader;
}
ReaderClass< PhantomSkillTableStruct >* GameObjDbStructEx::ObjReader_PhantomSkill()
{
	static ReaderClass< PhantomSkillTableStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char Buf[256];
		PhantomSkillTableStruct* Pt = 0;
		Reader.SetData("GUID", &Pt->GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID));
		for (int i = 0; i < 2; i++)
		{
			sprintf(Buf, "Phantom%d", i + 1);
			Reader.SetData(Buf, &Pt->Phantom[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Phantom[i]));
		}
		Reader.SetData("SkillID", &Pt->SkillID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->SkillID));
		IsInit = true;
	}
	return &Reader;
}
ReaderClass< ServantEventTableStruct >* GameObjDbStructEx::ObjReader_ServantEvent()
{
	static ReaderClass< ServantEventTableStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char Buf[256];
		ServantEventTableStruct* Pt = 0;
		Reader.SetData("GUID_NEW", &Pt->GUID_NEW, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID_NEW));
		Reader.SetData("GUID", &Pt->GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID));
		Reader.SetData("EventType1", &Pt->EventType[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->EventType[0]));
		Reader.SetData("EventType2", &Pt->EventType[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->EventType[1]));
		Reader.SetData("Condition1", &Pt->Condition[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Condition[0]));
		Reader.SetData("Condition2", &Pt->Condition[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Condition[1]));
		Reader.SetData("Condition3", &Pt->Condition[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Condition[2]));
		Reader.SetData("Rate", &Pt->Rate, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Rate));
		for (int i = 0; i < 8; i++)
		{
			sprintf(Buf, "MinValue%d", i + 1);
			Reader.SetData(Buf, &Pt->MinValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->MinValue[i]));
			sprintf(Buf, "MaxValue%d", i + 1);
			Reader.SetData(Buf, &Pt->MaxValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->MaxValue[i]));
		}
		Reader.SetData("Motion", &Pt->MotionID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->MotionID));
		Reader.SetData("TalkStr", &Pt->TalkStr, ENUM_Type_CharString, sizeof(Pt->TalkStr), ENUM_Indirect_Address, 0, sizeof(Pt->TalkStr));
		Reader.SetData("Voice", &Pt->Voice, ENUM_Type_CharString, sizeof(Pt->Voice), ENUM_Indirect_Address, 0, sizeof(Pt->Voice));
		Reader.SetData("SrvLuaScript", &Pt->LuaScript, ENUM_Type_CharString, sizeof(Pt->LuaScript), ENUM_Indirect_Address, 0, sizeof(Pt->LuaScript));
		Reader.SetData("SrvLuaCheckScript", &Pt->LuaCheckScript, ENUM_Type_CharString, sizeof(Pt->LuaScript), ENUM_Indirect_Address, 0, sizeof(Pt->LuaCheckScript));
		IsInit = true;
	}
	return &Reader;
}
ReaderClass< ServantHireTableStruct >* GameObjDbStructEx::ObjReader_ServantHire()
{
	static ReaderClass< ServantHireTableStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		ServantHireTableStruct* Pt = 0;
		Reader.SetData("GUID", &Pt->GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID));
		Reader.SetData("NPCObjID", &Pt->NPCObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->NPCObjID));
		Reader.SetData("FaceID", &Pt->FaceID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->FaceID));
		Reader.SetData("HairID", &Pt->HairID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->HairID));
		Reader.SetData("SkinColor", &Pt->SkinColor, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->SkinColor));
		Reader.SetData("HairColor", &Pt->HairColor, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->HairColor));
		Reader.SetData("ClothColor1", &Pt->ClothColor1, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ClothColor1));
		Reader.SetData("ClothColor2", &Pt->ClothColor2, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ClothColor2));
		Reader.SetData("Character", &Pt->Character, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Character));
		IsInit = true;
	}
	return &Reader;
}
ReaderClass< ServantStartValueTableStruct >* GameObjDbStructEx::ObjReader_ServantStartValue()
{
	static ReaderClass< ServantStartValueTableStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char Buf[256];
		ServantStartValueTableStruct* Pt = 0;
		Reader.SetData("NPCObjID", &Pt->NPCObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->NPCObjID));
		Reader.SetData("LookNpcObjID", &Pt->LookNpcObjID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->LookNpcObjID));
		for (int i = 0; i < 8; i++)
		{
			sprintf(Buf, "StartValue%d", i + 1);
			Reader.SetData(Buf, &Pt->StartValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->StartValue[i]));
			sprintf(Buf, "MaxValue%d", i + 1);
			Reader.SetData(Buf, &Pt->MaxValue[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->MaxValue[i]));
		}
		Reader.SetData("Sex", &Pt->Sex, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Sex));
		Reader.SetData("Cost", &Pt->Cost, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Cost));
		IsInit = true;
	}
	return &Reader;
}
ReaderClass< ItemCombineStruct >* GameObjDbStructEx::ObjReader_ItemCombine()
{
	static ReaderClass< ItemCombineStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		char Buf[256];
		ItemCombineStruct* Pt = 0;
		Reader.SetData("GUID", &Pt->GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID));
		Reader.SetData("ProductID", &Pt->ProductID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProductID));
		Reader.SetData("ProductCount", &Pt->ProductCount, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ProductCount));
		for (int i = 0; i < 5; i++)
		{
			sprintf(Buf, "SourceID%d", i + 1);
			Reader.SetData(Buf, &Pt->SourceID[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->SourceID[i]));
			sprintf(Buf, "SourceCount%d", i + 1);
			Reader.SetData(Buf, &Pt->SourceCount[i], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->SourceCount[i]));
		}
		IsInit = true;
	}
	return &Reader;
}
ReaderClass< RuneExchangeExTableStruct >* GameObjDbStructEx::ObjReader_RuneExchangeEx()
{
	static ReaderClass< RuneExchangeExTableStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		RuneExchangeExTableStruct* Pt = 0;
		Reader.SetData("GUID", &Pt->GUID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->GUID));
		Reader.SetData("Type1", &Pt->Type1, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Type1));
		Reader.SetData("Type2", &Pt->Type2, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->Type2));
		Reader.SetData("ResultType", &Pt->ResultType, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ResultType));
		IsInit = true;
	}
	return &Reader;
}
ReaderClass< LifeSkillRareExStruct >* GameObjDbStructEx::ObjReader_LifeSkillRareEx()
{
	static ReaderClass< LifeSkillRareExStruct > Reader;
	static bool IsInit = false;
	if (IsInit == false)
	{
		LifeSkillRareExStruct* Pt = 0;
		Reader.SetData("ItemID", &Pt->ItemID, ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->ItemID));
		Reader.SetData("Rare1ItemID", &Pt->RareItemID[0], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->RareItemID[0]));
		Reader.SetData("Rare2ItemID", &Pt->RareItemID[1], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->RareItemID[1]));
		Reader.SetData("Rare3ItemID", &Pt->RareItemID[2], ENUM_Type_IntPoint, 0, ENUM_Indirect_Address, 0, sizeof(Pt->RareItemID[2]));
		IsInit = true;
	}
	return &Reader;
}