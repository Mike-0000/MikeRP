modded enum ENotification
{
	PLAYER_REWARD = 75,
}

modded class SCR_BaseGameMode
{
	// ###############################################################################################
	//									VVV		GAMEMODE	VVV
	// ###############################################################################################
	
	[Attribute("{14790B5F3B7B0935}Configs/Currency/CurrencyConfig.conf", UIWidgets.Auto, desc: "Currency config, holding information about all currencies", category: "Economy - System")]
	ref KF_CurrencyConfig m_CurrencyConfig;
	
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

    if (!playerChar)
    {
        Print("Error: Player character is null in OnPlayerSpawned for playerId " + playerId, LogLevel.ERROR);
        return;
    }

    playerChar.SetUid(uid);

    // Check for existing cash file
    if (HasAccount(playerId, true))
        playerChar.LoadCash();

    // Save balance to files
    playerChar.SaveCash(-1, true);

    // Check for account existence and create one if none is found
    if (!HasAccount(playerId))
        CreateAccount(uid);

    // Initialize balance values and request balances from server
    playerChar.m_iBalance = 0;

    foreach (KF_Currency currency : m_CurrencyConfig.m_aCurrencies)
    {
        // Schedule RefreshBalance with unique parameters
        GetGame().GetCallqueue().CallLater(RefreshBalance, 1000, true, playerId, currency.m_CurrencyProperties.m_sCurrencyName);
    }
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
	
	/*
	//------------------------------------------------------------------------------------------------
	//~ Consume supplies for spawning
	override protected void ConsumeSuppliesOnPlayerSpawn_S(int playerID, IEntity spawnPoint, SCR_PlayerLoadoutComponent loadoutComp)
	{
		SCR_CampaignMilitaryBaseComponent base;
		SCR_ResourceComponent resourceComp;
		
		int spawnSupplyCost = 0;
		if (loadoutComp)
			spawnSupplyCost = SCR_ArsenalManagerComponent.GetLoadoutCalculatedSupplyCost(loadoutComp.GetLoadout(), false, playerID, null, spawnPoint, base, resourceComp);
 		
		if (spawnSupplyCost > 0) AdjustBalanceAndNotify(playerID, "$", -spawnSupplyCost, "Loadout");
	}
	*/
	
	// ###############################################################################################
	//									VVV		ECONOMY		VVV
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
		Rpc(RpcDo_CreateAccount, uid);
    }

	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcDo_CreateAccount(string uid)
    {
		// Init starting values
		array<int> currencies = {};
		
		foreach (KF_Currency currency : m_CurrencyConfig.m_aCurrencies)
		{
			currencies.Insert(currency.m_iStartingAmount);
		}
		
		// Create file and write balance
		string filePath = "$profile:Bank/" + uid + ".txt";
		FileHandle balanceFile = FileIO.OpenFile(filePath, FileMode.WRITE);
		if (!balanceFile) 
		{
			FileIO.MakeDirectory("$profile:Bank/");
			balanceFile = FileIO.OpenFile(filePath, FileMode.WRITE);
		}
		
		WriteBalances(balanceFile, currencies);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Compare balance stored on player to balance stored on server. When server balance changes, the
	//! new balance is assigned to players ChimeraCharacter.
	//! \param[in] int playerId Unique player identifier for current session
	//! \param[in] string currencySymbol Currency identifier assigned in KF_CurrencyConfig
	void RefreshBalance(int playerId, string currencySymbol)
{
    // Get chimera character 
    SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId));
    
    // Check if the character is valid
    if (!char)
    {
        Print("Error: SCR_ChimeraCharacter is null for playerId " + playerId, LogLevel.ERROR);
        return;
    }

    // Find currency index
    int index = -1;
    foreach (KF_Currency currency : m_CurrencyConfig.m_aCurrencies)
    {
        if (currency.m_CurrencyProperties.m_sCurrencyName == currencySymbol)
        {
            index = m_CurrencyConfig.m_aCurrencies.Find(currency);
            break;
        }
    }

    // Check if currency was found
    if (index == -1)
    {
        Print("Error: Currency symbol '" + currencySymbol + "' not found in m_CurrencyConfig for playerId " + playerId, LogLevel.ERROR);
        return;
    }

    // Get current balance
    int currentBalance = char.m_iBalance;

    // Proceed with RPC call
    Rpc(RpcAsk_RefreshBalance, playerId, currencySymbol, currentBalance);
}

	
	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_RefreshBalance(int playerId, string currencySymbol, int charBalance)
    {
		string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
		int fileBalance = ReadBalanceBySymbol(uid, currencySymbol);
		
		// Client balance doesn't match file balance
		if (charBalance != fileBalance)
		{
			// Find currency index
			int index;
			foreach (KF_Currency currency : m_CurrencyConfig.m_aCurrencies)
			{
				if (currency.m_CurrencyProperties.m_sCurrencyName == currencySymbol) index = m_CurrencyConfig.m_aCurrencies.Find(currency);
			}
			
			// Get chimera character 
			SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId));
			char.SetBalance(index, fileBalance);
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
		
		// Find currency index
		int index;
		foreach (KF_Currency currency : m_CurrencyConfig.m_aCurrencies)
		{
			if (currency.m_CurrencyProperties.m_sCurrencyName == currencySymbol) index = m_CurrencyConfig.m_aCurrencies.Find(currency);
		}
		
		// Get balance corresponding to currenySymbol
		//return char.m_aBalances.Get(index);
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
		int currentBalance = ReadBalanceBySymbol(uid, currencySymbol);
		int newBalance = currentBalance + amount;
		
		SetBalanceBySymbol(uid, currencySymbol, newBalance);
	}
	
	//------------------------------------------------------------------------------------------------
	// TODO: SERVER-SIDE
	//! \param[in] FileHandle file Balance file of specific player
	//! \param[in] array<int> balances Array of all balance values in order corresponding to KF_CurrencyConfig
	void WriteBalances(FileHandle file, array<int> balances)
	{
		if (!file) return;
		
		for (int i = 0; i < balances.Count(); i++) 
		{
			string currencyName = m_CurrencyConfig.m_aCurrencies.Get(i).m_CurrencyProperties.m_sCurrencyName;
			string line = currencyName + balances.Get(i).ToString();
			file.WriteLine(line);
		}
		
		file.Close();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Go through balance file stored on server and look for specific currency balance
	//! \param[in] string uid Unique player identity id
	//! \param[in] string symbol Currency identifier assigned in KF_CurrencyConfig
	//! \return balance corresponding to symbol
	int ReadBalanceBySymbol(string uid, string symbol)
	{
		string filePath = "$profile:Bank/" + uid + ".txt";
		FileHandle file = FileIO.OpenFile(filePath, FileMode.READ);
		
		if (!file) 
		{
			Print("ERROR: Could not open file " + filePath, LogLevel.ERROR);
			return -1;
		}

		// Go through file and look for currencySymbol
		file.Seek(0);
		string line;
		while (file.ReadLine(line) != -1) 
		{
			if (line.StartsWith(symbol)) 
			{
				string valueStr = line.Substring(symbol.Length(), line.Length() - symbol.Length());
				file.Close();
				return valueStr.ToInt();
			}
		}
		
		// Handle error: symbol not found
		Print("ERROR: Symbol not found in file.", LogLevel.ERROR);
		file.Close();
		return -1;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Change specific balance value in balance file on server by overriding current value
	//! \param[in] string uid Unique player identity id
	//! \param[in] string symbol Currency identifier assigned in KF_CurrencyConfig
	//! \param[in] int newBalance Balance value overriding current balance
	void SetBalanceBySymbol(string uid, string symbol, int newBalance)
	{
    	Rpc(RpcAsk_SetBalanceBySymbol, uid, symbol, newBalance);
	}
	
	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_SetBalanceBySymbol(string uid, string symbol, int newBalance)
	{
		// Read all balances and temp store them 
		array<string> balances = {};
		foreach (KF_Currency currency : m_CurrencyConfig.m_aCurrencies)
		{
			string currencySymbol = currency.m_CurrencyProperties.m_sCurrencyName;
			
			if (currencySymbol == symbol) balances.Insert(currencySymbol + newBalance);
			else if (currencySymbol != symbol) balances.Insert(currencySymbol + ReadBalanceBySymbol(uid, currencySymbol));
		}
		
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
