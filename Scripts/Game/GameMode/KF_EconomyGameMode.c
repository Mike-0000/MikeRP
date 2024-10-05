[EDF_DbName.Automatic()]
class MIKE_BankDB : EDF_DbEntity
{
    string guid5;
    int amount5;

    //------------------------------------------------------------------------------------------------
    //! Db entities cannot have a constructor with parameters; this is a limitation of the engine.
    //! Consult the docs for more info on this.
    static MIKE_BankDB Create(string guid2, int amount1)
    {
        MIKE_BankDB instance();
        instance.guid5 = guid2;
        instance.amount5 = amount1;
        return instance;
    }
};
class MIKE_CashDB : EDF_DbEntity
{
    string guid3;
    int amount3;

    //------------------------------------------------------------------------------------------------
    //! Db entities cannot have a constructor with parameters; this is a limitation of the engine.
    //! Consult the docs for more info on this.
    static MIKE_CashDB Create(string guid4, int amount2)
    {
        MIKE_CashDB instance();
        instance.guid3 = guid4;
        instance.amount3 = amount2;
        return instance;
    }
};

modded enum ENotification
{
	PLAYER_REWARD = 75,
}

modded class SCR_BaseGameMode
{
	// ###############################################################################################
	//						VVV		GAMEMODE	VVV
	// ###############################################################################################
	
	//[Attribute("{14790B5F3B7B0935}Configs/Currency/CurrencyConfig.conf", UIWidgets.Auto, desc: "Currency config, holding information about all currencies", category: "Economy - System")]
	//ref KF_CurrencyConfig m_CurrencyConfig;
	
	[Attribute("1000", UIWidgets.EditBox, desc: "Bank starting amount", category: "Economy - System")]
	int m_iStartingAmount;
	
	[Attribute("1", UIWidgets.CheckBox, desc: "Enables cash system, required for ATMs and cash payment", category: "Economy - System")]
	bool m_bEnableCash;
	
	[Attribute("0", UIWidgets.CheckBox, desc: "Kill rewards will be paid in cash", category: "Economy - Rewards")]
	bool m_bCashReward;
	
	[Attribute("100", UIWidgets.EditBox, desc: "Hostile kill reward", category: "Economy - Rewards")]
	int m_iKillReward;
	
	[Attribute("-150", UIWidgets.EditBox, desc: "Friendly fire penalty", category: "Economy - Rewards")]
	int m_iFriendlyFire;
	
	//------------------------------------------------------------------------------------------------
	override void OnPlayerSpawned(int playerId, IEntity controlledEntity)
	{
		super.OnPlayerSpawned(playerId, controlledEntity);
		
		if (Replication.IsClient()) return;
		
		// Assign PlayerUid to ChimeraCharacter
		string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
		SCR_ChimeraCharacter playerChar = SCR_ChimeraCharacter.Cast(controlledEntity);
		playerChar.SetUid(uid);
		
		// Check for exisiting cash file
		if (HasAccount(playerId, true)) playerChar.LoadCash();
		
		// Save balance to files
		playerChar.SaveCash(-1, true);
		
		// Check for account existence and create one if none is found
		if (!HasAccount(playerId)) CreateAccount(uid);
		
		// Initialize balance values and request balances from server
		//playerChar.InitBalanceArray(m_CurrencyConfig.m_aCurrencies.Count());
		playerChar.m_iBalance = 0;
		/*
		foreach (KF_Currency currency : m_CurrencyConfig.m_aCurrencies)
		{
			GetGame().GetCallqueue().CallLater(RefreshBalance, 1000, true, playerId, currency.m_CurrencyProperties.m_sCurrencyName);
		}
		*/
		
		GetGame().GetCallqueue().CallLater(RefreshBalance, 1000, false, playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	// PvP-Kill reward behaviour
	protected override void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		super.OnPlayerKilled(playerId, playerEntity, killerEntity, killer);
		
		// Delete cash
		SCR_ChimeraCharacter playerChar = SCR_ChimeraCharacter.Cast(playerEntity);
		if (playerChar)
		{
			playerChar.SaveCash(0);
			playerChar.m_bRecentDeath = true;
		}
		
		// Abort when killer is non-player entity
		if (!Replication.IsServer() || playerEntity == killerEntity || !EntityUtils.IsPlayer(killerEntity)) return;
		
		PlayerManager pm = GetGame().GetPlayerManager();
		SCR_FactionManager fm = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		int killerId = pm.GetPlayerIdFromControlledEntity(killerEntity);
		
		FactionAffiliationComponent factionCompVictim = FactionAffiliationComponent.Cast(playerEntity.FindComponent(FactionAffiliationComponent));
		FactionAffiliationComponent factionCompKiller = FactionAffiliationComponent.Cast(killerEntity.FindComponent(FactionAffiliationComponent));
		if (!factionCompVictim || !factionCompKiller) return;
		
		Faction victimFaction = fm.GetFactionByKey(factionCompVictim.GetAffiliatedFactionKey());
		Faction killerFaction = fm.GetFactionByKey(factionCompKiller.GetAffiliatedFactionKey());
		
		// Hostile kill reward
		if (victimFaction != killerFaction) AdjustBalanceAndNotify(killerId, "$", m_iKillReward, "EKIA");
		// Friendly fire penalty
		else if (victimFaction == killerFaction) AdjustBalanceAndNotify(killerId, "$", m_iFriendlyFire, "FF");
	}
	
	//------------------------------------------------------------------------------------------------
	// PvE-Kill reward behaviour
	protected override void OnControllableDestroyed(IEntity entity, IEntity killerEntity, notnull Instigator instigator)
	{
		super.OnControllableDestroyed(entity, killerEntity, instigator);
		
		// Abort when killer is non-player entity
		if (!Replication.IsServer() || entity == killerEntity || !EntityUtils.IsPlayer(killerEntity)) return;
		
		PlayerManager pm = GetGame().GetPlayerManager();
		SCR_FactionManager fm = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		int killerId = pm.GetPlayerIdFromControlledEntity(killerEntity);
		
		FactionAffiliationComponent factionCompBot = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
		FactionAffiliationComponent factionCompKiller = FactionAffiliationComponent.Cast(killerEntity.FindComponent(FactionAffiliationComponent));
		if (!factionCompBot || !factionCompKiller) return;
		
		Faction botFaction = fm.GetFactionByKey(factionCompBot.GetAffiliatedFactionKey());
		Faction killerFaction = fm.GetFactionByKey(factionCompKiller.GetAffiliatedFactionKey());
		
		// Hostile kill reward
		if (botFaction != killerFaction) AdjustBalanceAndNotify(killerId, "$", m_iKillReward, "EKIA");
		// Friendly fire penalty
		else if (botFaction == killerFaction) AdjustBalanceAndNotify(killerId, "$", m_iFriendlyFire, "FF");
	}
	
	// ###############################################################################################
	//						VVV		ECONOMY		VVV
	// ###############################################################################################
	
	//------------------------------------------------------------------------------------------------
	//! Check if server has balance file stored for specific player
	//! \param[in] string uid Unique player identity id
    bool HasAccount(int playerId, bool cash = false)
    {
		string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
		
		if (Replication.IsServer())
		{
			string filePath = "$profile:Bank/" + uid + ".txt";
			if (cash) filePath = "$profile:Cash/" + uid + ".txt";
        	return FileIO.FileExists(filePath);
		}
		
		Rpc(RpcAsk_HasAccount, playerId);
		
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId));
		return char.m_bHasAccount;
    }
	
	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_HasAccount(int playerId)
    {
		string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
		string filePath = "$profile:Bank/" + uid + ".txt";
        bool hasAccount = FileIO.FileExists(filePath);
		
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId));
		char.SetHasAccount(hasAccount);
    }
    
    //------------------------------------------------------------------------------------------------
	//! No balance file was found, so server needs to create one and fill it with starting values
	//! \param[in] string uid Unique player identity id
    void CreateAccount(string uid)
    {
		Rpc(RpcDo_CreateCashAccount, uid);
		Rpc(RpcDo_CreateBankAccount, uid);
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcDo_CreateBankAccount(string uid)
    {

		
		// Create Bank database entry and write balance
		EDF_JsonFileDbConnectionInfo connectInfo();
		connectInfo.m_sDatabaseName = "Bank";
		EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
		EDF_DbRepository<MIKE_BankDB> repository = EDF_DbEntityHelper<MIKE_BankDB>.GetRepository(dbContext);
		MIKE_BankDB newEntry = MIKE_BankDB.Create(uid, m_iStartingAmount);
		repository.AddOrUpdateAsync(newEntry);
	}
	
		//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcDo_CreateCashAccount(string uid)
    {

		EDF_JsonFileDbConnectionInfo connectInfo();
		// Create Cash database entry and write balance
		connectInfo.m_sDatabaseName = "Cash";
		EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
		EDF_DbRepository<MIKE_CashDB> repository = EDF_DbEntityHelper<MIKE_CashDB>.GetRepository(dbContext);
		MIKE_CashDB newEntry = MIKE_CashDB.Create(uid, m_iStartingAmount);
		repository.AddOrUpdateAsync(newEntry);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Compare balance stored on player to balance stored on server. When server balance changes, the
	//! new balance is assigned to players ChimeraCharacter.
	//! \param[in] int playerId Unique player identifier for current session
	//! \param[in] string currencySymbol Currency identifier assigned in KF_CurrencyConfig
	//void RefreshBalance(int playerId, string currencySymbol)
	void RefreshBalance(int playerId)
	{	
		// Get chimera character
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId));
		if (!char) return;
		
		GetGame().GetCallqueue().CallLater(RefreshBalance, 1000, false, playerId);
		

		// Get current balance
		//int currentBalance = char.m_aBalances.Get(index);
		int currentBalance = char.m_iBalance;
		
		//Rpc(RpcAsk_RefreshBalance, playerId, currencySymbol, currentBalance);
		Rpc(RpcAsk_RefreshBalance, playerId, currentBalance);
	}
	
	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    //void RpcAsk_RefreshBalance(int playerId, string currencySymbol, int charBalance)
	void RpcAsk_RefreshBalance(int playerId, int charBalance)
    {
		string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
		//int fileBalance = ReadBalanceBySymbol(uid, currencySymbol);
		int fileBalance = ReadBalance(uid);
		
		// Client balance doesn't match file balance
		if (charBalance != fileBalance)
		{

			
			// Get chimera character
			SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId));
			//char.SetBalance(index, fileBalance);
			char.SetBalance(fileBalance);
		}
    }
	
	//------------------------------------------------------------------------------------------------
	//! \param[in] int playerId Unique player identifier for current session
	//! \param[in] string currencySymbol Currency identifier assigned in KF_CurrencyConfig
	//! \return balance corresponding to currency, stored on ChimeraCharacter
	int GetBalance(int playerId, string currencySymbol)
	{
		// Get chimera character
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId));
		

		return char.m_iBalance;
	}
	
    //------------------------------------------------------------------------------------------------
	//! \param[in] int playerId Unique player identifier for current session
	//! \param[in] string currencySymbol Currency identifier assigned in KF_CurrencyConfig
	//! \param[in] int amount Amount to be added to current balance (positive value for addition, negative for removal)
    void AdjustBalance(int playerId, string currencySymbol, int amount)
    {
		Rpc(RpcAsk_AdjustBalance, playerId, currencySymbol, amount);
    }
	
	//------------------------------------------------------------------------------------------------
	//! \param[in] string message Notification message
	void AdjustBalanceAndNotify(int playerId, string currencySymbol, int amount, string message)
    {
		NotifyPlayer(playerId, amount, message);
		
		Rpc(RpcAsk_AdjustBalance, playerId, currencySymbol, amount);
    }
	
	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_AdjustBalance(int playerId, string currencySymbol, int amount)
    {
		string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
		//int currentBalance = ReadBalanceBySymbol(uid, currencySymbol);
		int currentBalance = ReadBalance(uid);
		int newBalance = currentBalance + amount;
		
		//SetBalanceBySymbol(uid, currencySymbol, newBalance);
		SetBalance(uid, newBalance);
	}
	
	//------------------------------------------------------------------------------------------------
	// TODO: SERVER-SIDE
	//! \param[in] FileHandle file Balance file of specific player
	//! \param[in] array<int> balances Array of all balance values in order corresponding to KF_CurrencyConfig
	//void WriteBalances(FileHandle file, array<int> balances)
	void WriteBalance(FileHandle file, int balance)
	{
		if (!file) return;
		

		
		file.WriteLine(balance.ToString());
		file.Close();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Go through balance file stored on server and look for specific currency balance
	//! \param[in] string uid Unique player identity id
	//! \param[in] string symbol Currency identifier assigned in KF_CurrencyConfig
	//! \return balance corresponding to symbol
	//int ReadBalanceBySymbol(string uid, string symbol)
	int ReadBalance(string uid)
	{
		string filePath = "$profile:Bank/" + uid + ".txt";
		FileHandle file = FileIO.OpenFile(filePath, FileMode.READ);
		
		if (!file)
		{
			Print("ERROR: Could not open file " + filePath, LogLevel.ERROR);
			return -1;
		}


		
		string fileBalance;
		file.ReadLine(fileBalance);
		file.Close();
		return fileBalance.ToInt();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Change specific balance value in balance file on server by overriding current value
	//! \param[in] string uid Unique player identity id
	//! \param[in] string symbol Currency identifier assigned in KF_CurrencyConfig
	//! \param[in] int newBalance Balance value overriding current balance
	//void SetBalanceBySymbol(string uid, string symbol, int newBalance)
	void SetBalance(string uid, int newBalance)
	{
    	//Rpc(RpcAsk_SetBalanceBySymbol, uid, symbol, newBalance);
		Rpc(RpcAsk_SetBalance, uid, newBalance);
	}
	
	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    //void RpcAsk_SetBalanceBySymbol(string uid, string symbol, int newBalance)
	void RpcAsk_SetBalance(string uid, int newBalance)
	{
		// Read all balances and temp store them
		array<string> balances = {};
		balances.Insert(newBalance.ToString());
		/*
		foreach (KF_Currency currency : m_CurrencyConfig.m_aCurrencies)
		{
			string currencySymbol = currency.m_CurrencyProperties.m_sCurrencyName;
			
			if (currencySymbol == symbol) balances.Insert(currencySymbol + newBalance);
			else if (currencySymbol != symbol) balances.Insert(currencySymbol + ReadBalanceBySymbol(uid, currencySymbol));
		}
		*/
		
		// Delete files content and rewrite it with new values
		string filePath = "$profile:Bank/" + uid + ".txt";
		SCR_FileIOHelper.WriteFileContent(filePath, balances);
	}
	
	//------------------------------------------------------------------------------------------------
	void NotifyPlayer(int playerId, int amount, string message)
	{
		Rpc(RpcDo_NotifyPlayer, playerId, amount, message);
	}
	
	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    void RpcDo_NotifyPlayer(int playerId, int amount, string message)
	{
		int localId = SCR_PlayerController.GetLocalPlayerId();
		if (localId != playerId) return;
		
		IEntity playerEnt = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		if (!playerEnt) return;
		
		SCR_BaseHUDComponent hudComp = SCR_BaseHUDComponent.Cast(playerEnt.FindComponent(SCR_BaseHUDComponent));
		if (!hudComp) return;
		
		array<BaseInfoDisplay> outInfoDisplays = {};
		hudComp.GetInfoDisplays(outInfoDisplays);
		
		KF_EconomyInfoDisplay economyDisplay;
		foreach (BaseInfoDisplay display : outInfoDisplays)
		{
			if (display.Type() == KF_EconomyInfoDisplay) economyDisplay = KF_EconomyInfoDisplay.Cast(display);
		}
		
		if (economyDisplay) economyDisplay.NotifyPlayer(amount, message);
	}
}