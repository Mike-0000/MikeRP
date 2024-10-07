// KF_EconomyGameMode.c

[EDF_DbName.Automatic()]
class MIKE_Bank : EDF_DbEntity
{
    string mikeguid;
    int mikebank;

    //------------------------------------------------------------------------------------------------
    //! Db entities cannot have a constructor with parameters; this is a limitation of the engine.
    //! Consult the docs for more info on this.
    static MIKE_Bank Create(string guid2, int mikebankReturn)
    {
        MIKE_Bank instance();
        instance.mikeguid = guid2;
        instance.mikebank = mikebankReturn;

        return instance;
    }
}

modded enum ENotification
{
    PLAYER_REWARD = 75,
}

class AccountContext
{
    int playerId;
    bool cash;
    int amount; // Added for AdjustBalance context
}

modded class SCR_BaseGameMode
{
    // Replicated Configuration Variables

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

    // Map to store context based on uid
    protected ref map<string, ref AccountContext> m_AccountContexts = new map<string, ref AccountContext>();

    //------------------------------------------------------------------------------------------------
    // Removed OnInit method since we cannot override it

    //------------------------------------------------------------------------------------------------
    override void OnPlayerSpawned(int playerId, IEntity controlledEntity)
    {
		
			
        super.OnPlayerSpawned(playerId, controlledEntity);

        if (Replication.IsClient())
            return;

		
        string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
        SCR_ChimeraCharacter playerChar = SCR_ChimeraCharacter.Cast(controlledEntity);
        playerChar.SetUid(uid);

        // Check for existing cash record and load cash
        HasAccount(playerId, true);

        // Initialize balance values and request balances from server
        playerChar.m_iBalance = 0;
		
		
		
		
		

        string playerGUID = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);

        // Get the connection info
        EDF_JsonFileDbConnectionInfo connectInfo();
        connectInfo.m_sDatabaseName = "LicenseInfo";

        // Get a db context instance
        EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);

        // Interact with the DB context through a repository
        EDF_DbRepository<TAG_LicenseInfo> repository = EDF_DbEntityHelper<TAG_LicenseInfo>.GetRepository(dbContext);
		
        
		
		
		
		
		
		// Now find the drivers record
	
		
        EDF_DbFindCondition condition = EDF_DbFind.And({
            EDF_DbFind.Field("guid1").Equals(playerGUID),
            EDF_DbFind.Field("licensename").Equals("drivers")
        });
		

        // Set up the callback handler
		Tuple1<SCR_ChimeraCharacter> context(SCR_ChimeraCharacter.Cast(controlledEntity));
        EDF_DbFindCallbackSingle<TAG_LicenseInfo> findRecordHandler(this, "FindRecord", context);
		Print("FindFirstAsync Invoked", LogLevel.NORMAL); // DEBUG
        repository.FindFirstAsync(condition, callback: findRecordHandler);
		
		
		
		
		
		
		
		

        GetGame().GetCallqueue().CallLater(RefreshBalance, 1000, false, playerId);
    }
	
	
	 void FindRecord(EDF_EDbOperationStatusCode statusCode, TAG_LicenseInfo result, Managed playerEntity)
    {
		Tuple1<SCR_ChimeraCharacter> typedContext = Tuple1<SCR_ChimeraCharacter>.Cast(playerEntity);
		SCR_ChimeraCharacter currentUserChar = typedContext.param1; // We get our DoFind parameter back yay!
		if (result.licensename == "drivers"){
		
			currentUserChar.driversLicenseExists = true;
							
		}
		else if (result.licensename == "civhandgun"){
			currentUserChar.basicGunLicenseExists = true;
		
		}
		else if (result.licensename == "tier2civhandgun"){
			currentUserChar.tier2GunLicenseExists = true;
		
		}
		
		Print("Record received. recentLicense = Result GUID = " + result.guid1, LogLevel.NORMAL); // DEBUG
        if (statusCode == EDF_EDbOperationStatusCode.SUCCESS && result != null)
        {
			currentUserChar.recentLicense = result.licensename;	
            //licenseExists = true;
            Print("License found for user: " + currentUserChar.GetName(), LogLevel.NORMAL);
        }
        else
        {
			
            //licenseExists = false;
            Print("License not found for user: " + currentUserChar.GetName(), LogLevel.NORMAL);
        }

    }
	

    //------------------------------------------------------------------------------------------------
    //! Check if server has balance record stored for specific player
    //! \param[in] int playerId Unique player identifier for current session
    //! \param[in] bool cash Indicates if cash account should be checked/created
    void HasAccount(int playerId, bool cash = false)
    {
        string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);

        if (Replication.IsServer())
        {
            // Create and store context
            ref AccountContext context = new AccountContext();
	        //Print("AccountContext Created: PlayerId=" + playerId + ", Cash=" + cash, LogLevel.NORMAL); // DEBUG
            context.playerId = playerId;
            context.cash = cash;
            m_AccountContexts.Insert(uid, context);

            EDF_JsonFileDbConnectionInfo connectInfo();
            connectInfo.m_sDatabaseName = "BankDB";

            // Get a db context instance
            EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
            // Interact with the DB context through a repository
            EDF_DbRepository<MIKE_Bank> repository = EDF_DbEntityHelper<MIKE_Bank>.GetRepository(dbContext);
            // Now find the record
            EDF_DbFindCondition condition = EDF_DbFind.Field("mikeguid").Equals(uid);
            repository.FindFirstAsync(condition, callback: new EDF_DbFindCallbackSingle<MIKE_Bank>(this, "OnHasAccountFound"));
        }
        else
        {
            Rpc(RpcAsk_HasAccount, playerId, cash);
        }
    }

    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_HasAccount(int playerId, bool cash)
    {
        HasAccount(playerId, cash);
    }

    //------------------------------------------------------------------------------------------------
    //! Callback function after checking if account exists
    //! \param[in] EDF_EDbOperationStatusCode statusCode Operation status
    //! \param[in] MIKE_Bank result The result from the database query
    void OnHasAccountFound(EDF_EDbOperationStatusCode statusCode, MIKE_Bank result) {
        string resultStatus;
        if (result != null)
        {
            resultStatus = "Exists";
        }
        else
        {
            resultStatus = "Not Found";
        }
        //Print("OnHasAccountFound: StatusCode=" + statusCode + ", Result=" + resultStatus, LogLevel.NORMAL); // DEBUG
        string uid;
        if (result != null)
        {
            uid = result.mikeguid;
        }
        else
        {
            uid = string.Empty;
        }

        if (uid == string.Empty)
        {
            // Unable to retrieve UID from result, handle accordingly
            //Print("OnHasAccountFound: UID is empty.", LogLevel.WARNING);
            return;
        }

        // Find the corresponding context
        AccountContext context;
        if (m_AccountContexts.Find(uid, context))
        {
            bool accountExists = (result != null);
            SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(context.playerId));
            if (char)
            {
                char.SetHasAccount(accountExists);
            }

            if (!accountExists)
            {
                // Account not found, create new account
                CreateAccount(uid);
            }
            else
            {
                // Account exists
                if (context.cash && char)
                {
                    char.LoadCash();
                }

                // Save balance to record
                if (char)
                {
                    char.SaveCash(-1, true);
                }
            }

            // Remove context from the map
            m_AccountContexts.Remove(uid);
        }
        else
        {
            // Handle cases where uid is empty or context not found
            //Print("OnHasAccountFound: Context not found for UID: " + uid, LogLevel.WARNING);
        }
    }

    //------------------------------------------------------------------------------------------------
    //! No balance file was found, so server needs to create one and fill it with starting values
    //! \param[in] string uid Unique player identity id
    void CreateAccount(string uid)
    {
        if (Replication.IsServer())
        {
            EDF_JsonFileDbConnectionInfo connectInfo();
            connectInfo.m_sDatabaseName = "BankDB";
            EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
            EDF_DbRepository<MIKE_Bank> repository = EDF_DbEntityHelper<MIKE_Bank>.GetRepository(dbContext);
            MIKE_Bank newEntry = MIKE_Bank.Create(uid, m_iStartingAmount);
            newEntry.SetId(uid);
            repository.AddOrUpdateAsync(newEntry);
        }
        else
        {
            Rpc(RpcDo_CreateAccount, uid);
        }
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcDo_CreateAccount(string uid)
    {
        CreateAccount(uid);
    }

    //------------------------------------------------------------------------------------------------
    //! Compare balance stored on player to balance stored on server. When server balance changes, the
    //! new balance is assigned to player's ChimeraCharacter.
    //! \param[in] int playerId Unique player identifier for current session
    void RefreshBalance(int playerId) {
        //Print("RefreshBalance: PlayerId=" + playerId, LogLevel.NORMAL); // DEBUG
        // Get chimera character
        SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId));
        if (!char)
            return;

        // Get current balance
        int currentBalance = char.m_iBalance;

        // Request server to check balance
        if (Replication.IsServer())
        {
            string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
            ReadBalance(uid, playerId, currentBalance);
        }
        else
        {
            Rpc(RpcAsk_RefreshBalance, playerId, currentBalance);
        }

        GetGame().GetCallqueue().CallLater(RefreshBalance, 1000, false, playerId);
    }

    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_RefreshBalance(int playerId, int charBalance)
    {
        string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
        ReadBalance(uid, playerId, charBalance);
		//Print("RpcAsk_RefreshBalance: PlayerId=" + playerId + ", Amount=" + charBalance, LogLevel.NORMAL);
    }

    //------------------------------------------------------------------------------------------------
    void ReadBalance(string uid, int playerId, int charBalance)
    {
        // Create and store context
        ref AccountContext context = new AccountContext();
        //Print("ReadBalance AccountContext Created: PlayerId=" + playerId + ", Amount=" + charBalance, LogLevel.NORMAL); // DEBUG
        context.playerId = playerId;
        context.cash = false; // Not needed for balance refresh
        context.amount = 0;   // Not needed here
        m_AccountContexts.Insert(uid, context);

        EDF_JsonFileDbConnectionInfo connectInfo();
        connectInfo.m_sDatabaseName = "BankDB";

        // Get a db context instance
        EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
        // Interact with the DB context through a repository
        EDF_DbRepository<MIKE_Bank> repository = EDF_DbEntityHelper<MIKE_Bank>.GetRepository(dbContext);
        // Now find the record
        EDF_DbFindCondition condition = EDF_DbFind.Field("mikeguid").Equals(uid);
        repository.FindFirstAsync(condition, callback: new EDF_DbFindCallbackSingle<MIKE_Bank>(this, "OnBalanceRead"));
    }

    //------------------------------------------------------------------------------------------------
    //! Callback function after reading balance
    //! \param[in] EDF_EDbOperationStatusCode statusCode Operation status
    //! \param[in] MIKE_Bank result The result from the database query
    void OnBalanceRead(EDF_EDbOperationStatusCode statusCode, MIKE_Bank result)
    {
        string uid;
        if (result != null)
        {
            uid = result.mikeguid;
        }
        else
        {
            uid = string.Empty;
        }

        if (uid == string.Empty)
        {
            // Unable to retrieve UID from result, handle accordingly
            //Print("OnBalanceRead: UID is empty.", LogLevel.WARNING);
            return;
        }

        // Find the corresponding context
        AccountContext context;
        if (m_AccountContexts.Find(uid, context))
        {
            SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(context.playerId));
            if (char)
            {
                int fileBalance;
                if (result != null)
                {
                    fileBalance = result.mikebank;
                }
                else
                {
                    fileBalance = 0;
                }

                if (char.m_iBalance != fileBalance)
                {
                    char.SetBalance(fileBalance);
                }
            }

            // Remove context from the map
            m_AccountContexts.Remove(uid);
        }
        else
        {
            // Handle cases where context is not found
            //Print("OnBalanceRead: Context not found for UID: " + uid, LogLevel.WARNING);
        }
    }

    //------------------------------------------------------------------------------------------------
    //! Adjusts the player's balance
    //! \param[in] int playerId Unique player identifier for current session
    //! \param[in] string currencySymbol Currency identifier (not used here but kept for compatibility)
    //! \param[in] int amount Amount to be added to current balance (positive value for addition, negative for removal)
    void AdjustBalance(int playerId, string currencySymbol, int amount) {
        //Print("AdjustBalance (Client/Server Initial Call): PlayerId=" + playerId + ", Amount=" + amount, LogLevel.NORMAL); // DEBUG
        //Print("AdjustBalance (Initial Client/Server Call): PlayerId=" + playerId + ", Amount=" + amount, LogLevel.NORMAL); // DEBUG
        //Print("AdjustBalance (Client): PlayerId=" + playerId + ", Amount=" + amount, LogLevel.NORMAL); // DEBUG
        if (Replication.IsServer())
        {
            AdjustBalanceInternal(playerId, amount);
        }
        else
        {
            //Print("RpcAsk_AdjustBalance (Client-Side Call): PlayerId=" + playerId + ", Amount=" + amount, LogLevel.NORMAL); // DEBUG
        //Print("RpcAsk_AdjustBalance (Before Client-Side Call): PlayerId=" + playerId + ", Amount=" + amount, LogLevel.NORMAL); // DEBUG
        Rpc(RpcAsk_AdjustBalance, playerId, currencySymbol, amount);
        }
    }

    //------------------------------------------------------------------------------------------------
    //! Adjusts the player's balance and notifies them
    //! \param[in] int playerId Unique player identifier for current session
    //! \param[in] string currencySymbol Currency identifier (not used here but kept for compatibility)
    //! \param[in] int amount Amount to be added to current balance
    //! \param[in] string message Notification message
    void AdjustBalanceAndNotify(int playerId, string currencySymbol, int amount, string message) {
        //Print("AdjustBalanceAndNotify (Client/Server): PlayerId=" + playerId + ", Amount=" + amount + ", Message=" + message, LogLevel.NORMAL); // DEBUG
        if (Replication.IsServer())
        {
            AdjustBalanceInternal(playerId, amount);
            NotifyPlayer(playerId, amount, message);
        }
        else
        {
            //Print("RpcAsk_AdjustBalanceAndNotify (Client-Side Call): PlayerId=" + playerId + ", Amount=" + amount + ", Message=" + message, LogLevel.NORMAL); // DEBUG
        Rpc(RpcAsk_AdjustBalanceAndNotify, playerId, currencySymbol, amount, message);
        }
    }

    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_AdjustBalance(int playerId, string currencySymbol, int amount) {
        //Print("RpcAsk_AdjustBalance (Server Received): PlayerId=" + playerId + ", Amount=" + amount, LogLevel.NORMAL); // DEBUG
        //Print("RpcAsk_AdjustBalance (Server-Side Received): PlayerId=" + playerId + ", Amount=" + amount, LogLevel.NORMAL); // DEBUG
        //Print("RpcAsk_AdjustBalance (Server): PlayerId=" + playerId + ", Amount=" + amount, LogLevel.NORMAL); // DEBUG
        AdjustBalanceInternal(playerId, amount);
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_AdjustBalanceAndNotify(int playerId, string currencySymbol, int amount, string message) {
        //Print("RpcAsk_AdjustBalanceAndNotify (Server): PlayerId=" + playerId + ", Amount=" + amount + ", Message=" + message, LogLevel.NORMAL); // DEBUG
        AdjustBalanceInternal(playerId, amount);
        NotifyPlayer(playerId, amount, message);
    }

    //------------------------------------------------------------------------------------------------
    void AdjustBalanceInternal(int playerId, int amount) {
        //Print("AdjustBalanceInternal (Server): PlayerId=" + playerId + ", Amount=" + amount, LogLevel.NORMAL); // DEBUG
        //Print("AdjustBalanceInternal: PlayerId=" + playerId + ", Amount=" + amount, LogLevel.NORMAL); // DEBUG
        string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);

        // Create and store context
        ref AccountContext context = new AccountContext();
        //Print("AdjustBalanceInternal AccountContext Created: PlayerId=" + playerId + ", Amount=" + amount, LogLevel.NORMAL); // DEBUG
        context.playerId = playerId;
        context.cash = false; // Not needed for balance adjustment
        context.amount = amount;
        //Print("AdjustBalanceInternal AccountContext Updated: PlayerId=" + playerId + ", Amount=" + amount, LogLevel.NORMAL); // DEBUG
        m_AccountContexts.Insert(uid, context);

        EDF_JsonFileDbConnectionInfo connectInfo();
        connectInfo.m_sDatabaseName = "BankDB";

        EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
        EDF_DbRepository<MIKE_Bank> repository = EDF_DbEntityHelper<MIKE_Bank>.GetRepository(dbContext);

        EDF_DbFindCondition condition = EDF_DbFind.Field("mikeguid").Equals(uid);
        repository.FindFirstAsync(condition, callback: new EDF_DbFindCallbackSingle<MIKE_Bank>(this, "OnAdjustBalanceRead"));
    }

    //------------------------------------------------------------------------------------------------
    //! Callback function after adjusting balance
    //! \param[in] EDF_EDbOperationStatusCode statusCode Operation status
    //! \param[in] MIKE_Bank result The result from the database query
    void OnAdjustBalanceRead(EDF_EDbOperationStatusCode statusCode, MIKE_Bank result)
    {
		EDF_JsonFileDbConnectionInfo connectInfo();
        connectInfo.m_sDatabaseName = "BankDB";
        string uid;
        if (result != null)
        {
            uid = result.mikeguid;
        }
        else
        {
            uid = string.Empty;
        }

        if (uid == string.Empty)
        {
            // Unable to retrieve UID from result, handle accordingly
            //Print("OnAdjustBalanceRead: UID is empty.", LogLevel.WARNING);
            return;
        }

        // Find the corresponding context
        AccountContext context;
        if (m_AccountContexts.Find(uid, context))
        {
            int currentBalance;
            if (result != null)
            {
                currentBalance = result.mikebank;
            }
            else
            {
                currentBalance = 0;
            }

            // Server-side validation
            if (context.amount < 0 && Math.AbsInt(context.amount) > currentBalance)
            {
                //Print("Insufficient balance for playerId: " + context.playerId, LogLevel.WARNING);
                NotifyPlayer(context.playerId, 0, "Insufficient funds for withdrawal.");
                m_AccountContexts.Remove(uid);
                return;
            }

            int newBalance = currentBalance + context.amount;

            // Update database
            MIKE_Bank newEntry = MIKE_Bank.Create(uid, newBalance);
            newEntry.SetId(uid);
			
			
			EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
           	EDF_DbRepository<MIKE_Bank> repository = EDF_DbEntityHelper<MIKE_Bank>.GetRepository(dbContext);


            repository.AddOrUpdateAsync(newEntry);

            // Update player's character
            SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(context.playerId));
            if (char)
            {
                char.SetBalance(newBalance);
            }

            // Remove context from the map
            m_AccountContexts.Remove(uid);
        }
        else
        {
            // Handle cases where context is not found
            //Print("OnAdjustBalanceRead: Context not found for UID: " + uid, LogLevel.WARNING);
        }
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
        if (localId != playerId)
            return;

        IEntity playerEnt = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
        if (!playerEnt)
            return;

        SCR_BaseHUDComponent hudComp = SCR_BaseHUDComponent.Cast(playerEnt.FindComponent(SCR_BaseHUDComponent));
        if (!hudComp)
            return;

        array<BaseInfoDisplay> outInfoDisplays = {};
        hudComp.GetInfoDisplays(outInfoDisplays);

        KF_EconomyInfoDisplay economyDisplay;
        foreach (BaseInfoDisplay display : outInfoDisplays)
        {
            if (display.Type() == KF_EconomyInfoDisplay)
                economyDisplay = KF_EconomyInfoDisplay.Cast(display);
        }

        if (economyDisplay)
            economyDisplay.NotifyPlayer(amount, message);
    }

    //------------------------------------------------------------------------------------------------
    //! Retrieves the player's current balance
    //! \param[in] int playerId Unique player identifier for current session
    //! \return int The current balance of the player
    int GetBalance(int playerId, string symbol)
    {
        SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId));
        if (char)
        {
            return char.m_iBalance;
        }
        return 0; // Default balance if character not found
    }

}
