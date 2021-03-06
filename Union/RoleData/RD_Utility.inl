
// 取得目前擁有容量
static int GetAttachRuneCount(ItemFieldStruct* item);

// 取得符文總容量
static int	GetRuneTotalVolume(ItemFieldStruct* item);

// 取得物品總名字
static bool	GetItemFieldName(ItemFieldStruct* item, char* outName, int size);

// 取得抗性上限值
int	GetResistanceMaxValue();

// 取得基本數值
void GetAbilityDetailValues(RoleValueName_ENUM name, int* outAbility);

// 取得DPS
void GetPlayerDPS(float* outDPS);

// 取得相同物品在背包內最大數量
int GetBagItemCount(int objectID);

// 取得技能使用冷?j時間
static bool GetMagicCooldown(MagicColdownStruct& coldown, int objectID, float* outTime);

// 取得技能使用冷?j時間
bool GetObjectCooldown(int objectID, float* outTime);

// 檢查物品限制是否可以裝備(有效)
bool CanWearObject(GameObjDbStruct* itemDB, int slot=-1, bool ignoreLevel=false , int ImageObjID = 0  );

// 檢查背包內物品是否可以使用(true 可以用, false 鎖定中不能用)
bool IsActiveBagItem(int index);

// 檢查裝備技能
bool CheckEquipmentWearSkill(GameObjDbStruct* itemDB, int slot=-1);

// 取得技能熟練等級
float GetSkillPoint(int magicID);

//取得某Buff的PowerLv
int GetBuffPowerLv( int magicID );

//取得某欄位的技能配點總值
int	GetTolalSkillPoint( int Pos );

// 判斷是否為黑色單角色
bool CheckIgnoreNameList(const char* name);

//計算修裝假錢 return cost   輸入  修理前  , 修理後
static int	CalRepairWeapon( ItemFieldStruct* InItem , ItemFieldStruct* OutItem = NULL );

bool IsEquipSuitSkill(int skillId);

// 檢查裝備欄位是否正確
static bool	CheckEquipmentPos(int pos, ItemFieldStruct* itemField=NULL);


