
//int     SelfClock();
void    IncDMove( );
int     MoveTime( float X , float Y , float Z );

//取得SampleRoleData
void    GetSampleRoleData( SampleRoleData* Data );

//取得NPCData的資料
void    GetNPCData( NPCData* Data );

//簡查時間  要是 _DEF_BASE_CLOCL_UNIT_ 倍數
bool    ClockMod( int Mod );


//計算面對的方向
static float CalDir( float dx , float dz );

float	CalDir( RoleDataEx* Target );

static void	CalDir( float Dir , float& dx , float& dz );

//與看的方向所夾的角度
float   CalAngle( RoleDataEx* Target );

//攻擊目標(攻擊與Follow)
bool	AttackTarget( int TargetID );

//停止攻擊
bool	StopAttack();

//清除使用中的物品( 使用失敗清除 )
bool    ClsUseItem();

//刪除使用物品
bool    DelUseItem( );

//使用法術耗損
void    DelMagicCost( GameObjDbStructEx* Magic , int MagicLv );
void    DelMagicCostBase( GameObjDbStructEx* Magic , int MagicLv , MagicSpellCostTypeENUM CostType , int CostValue );

//停止移動
void    StopMove( );
void    StopMoveNow( );

//收尋愁恨的角色
RoleDataEx*    SearchHate( float Range );
RoleDataEx*    SearchHate( );

//搜尋新的目標 找不到就停止攻擊
//bool		FindNewHateTarget( );
//bool		FindNewHateTarget( float Range );
//-------------------------------------------------------------------------------------------
//法術技能配點
//-------------------------------------------------------------------------------------------
//初始化主副職法術資料
bool    InitMagicBook( );

//檢察是否可以法術配點
bool    CheckSetMagicPoint( int MagicPos );

//重設職業資料
void    ResetJobSkill( );

//檢查是否法術與位置資料與角色點術相符合
bool    CheckRoleMagic( int MagicID , int MagicPos );
//檢查是否法術與位置資料與角色點術相符合
bool    CheckRoleSPMagic( int MagicID , int MagicPos );

//重設擁有的被動技能
void    ReSetPassiveSkill( );
//-------------------------------------------------------------------------------------------
//取得物件源始的主人
RoleDataEx* GetOrgOwner();
//-------------------------------------------------------------------------------------------
//取得ＨＱ品的額外增加機率
int		GetHQExtraValue( SkillValueTypeENUM SkillType );

//-------------------------------------------------------------------------------------------
//下線有計算的Buff or Coldown處理
void	ProcOffLineTime();

//取得 OnlinePlayerInfoStruct 的資料
void	GetOnlinePlayerInfo( OnlinePlayerInfoStruct& Info );

//-------------------------------------------------------------------------------------------
//魔法盾吸收計算 回傳吸收多少( 會處理封包)
int		MagicShieldAbsorbProc( RoleDataEx* Attack , GameObjDbStruct* AtkMagic , int &Damage );

//-------------------------------------------------------------------------------------------
//取得某技能上限物件
int		GetLifeSkillMax( SkillValueTypeENUM Type );
//-------------------------------------------------------------------------------------------
//清除PK 資料
void	SetPKOff( );
//-------------------------------------------------------------------------------------------
//以目前的等級資料取得頭銜
void	GetTitle( );	
//-------------------------------------------------------------------------------------------
//取得施法時間
float		GetSpellTime( GameObjDbStructEx* MagicDB );
//-------------------------------------------------------------------------------------------
//取得冷?j時間
float		GetColdownTime( GameObjDbStructEx* MagicDB );
//-------------------------------------------------------------------------------------------
//重設裝備的耐久度
void		ResetEqDurable();
//裝備耐久損耗處理
bool		EqDurableProcess( EQWearPosENUM Pos , int DecDurable );
//-------------------------------------------------------------------------------------------
//某些事件增加Buf時間
void		OnEventAddBuffTime( );
//-------------------------------------------------------------------------------------------
//設定攻擊延遲
void		SetDelayAttack( int Time );
//-------------------------------------------------------------------------------------------
//把所有Itme Queue內的東西堆入身上
void		ItemQueueToBody();
//-------------------------------------------------------------------------------------------
//善惡等級
GoodEvilTypeENUM	GetGoodEvilType( );

//-------------------------------------------------------------------------------------------
//刪除身上物品
bool		DelBodyItem_ByPos( int Pos , int Count , ActionTypeENUM Type );

//最高等級
int			MaxJobLv();
//////////////////////////////////////////////////////////////////////////
//裝備符文融鍊系統
//////////////////////////////////////////////////////////////////////////
//裝備容解 ==> 魔石
static bool	SmeltEqtoMagicStone( ItemFieldStruct* EQ , ItemFieldStruct* Item , ItemFieldStruct* outResultItem );
//精煉 魔石(升等)
static bool	RefineMagicStone( ItemFieldStruct* Item[3] , ItemFieldStruct* outResultItem );
//裝備 與魔石熔合
static bool	CombinEQandMagicStone(  ItemFieldStruct* EQ , ItemFieldStruct* MagicStone[3] , ItemFieldStruct* outResultItem  );
//符文轉換
static bool	RuneTransfer( ItemFieldStruct* Rune1 , ItemFieldStruct* Rune2 , ItemFieldStruct* outResultItem );
//符文升等
static bool	RefineRune( ItemFieldStruct* Rune , ItemFieldStruct* outResultItem );
//物品融合兌換
static bool	ItemCombine( vector< ItemFieldStruct >& ItemList , ItemFieldStruct* outResultItem );

//////////////////////////////////////////////////////////////////////////
//魔法盒煉化
bool	MagicBoxSmelt(  ItemFieldStruct* outResultItem );
//////////////////////////////////////////////////////////////////////////
static void	SelectAbility( vector<ItemFieldStruct*>& AbilityList , ItemFieldStruct* outResultItem );
//計算有幾個空位
int		EmptyPacketCount();
//////////////////////////////////////////////////////////////////////////
void	ResetLockAccountMoney( );

FixFightArgumentStruct&   FixArg();     //固定參數表

//取得攻防影響等級差
int CalAddDotMagicLv( RoleDataEx* Target , GameObjDbStructEx* MagicBaseObjDB , int SpellTime , int OrgMagicLv );

//保險處理
void	InsureProc();

//刪除一個物品
bool	DelOneItem( int ItemPos , ItemFieldStruct& Item , ActionTypeENUM ActionType );

void	SwapEQ( int posID );

int		JobCount( );

bool	OldUnPacket( ItemFieldStruct *SrvItem );
bool	NewUnPacket( ItemFieldStruct *SrvItem );
bool	UnPacket_DropList( ItemFieldStruct *SrvItem );

//修正物品資料
static void	FixItem( ItemFieldStruct *Item );

int		HPMPSPSum();

//取得寵物資料
PetStruct*	GetPetInfo( RoleDataEx* Pet );

//取得到此等級的TP
static int	GetLvTotalTP( int Lv );

static void	GetRareEffect( GameObjDbStructEx* OrgDB , int AddRare , float& Dmg , float& MDmg , float& Def , float& MDef );

//計算可以載多少人
int		CalCarryCount();

/*
//增加金錢(所有類型)
bool	AddMoneyBase( PriceTypeENUM Type , int Money );
//檢查金錢(所有類型)
bool	CheckMoneyBase( PriceTypeENUM Type , int Money );
*/

void	PhantomEQChangeProc();

static int KeyItemObjToID( int objID );
static int IDtoKeyItemObj( int id );