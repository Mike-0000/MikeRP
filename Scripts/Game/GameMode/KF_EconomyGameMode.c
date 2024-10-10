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
        MIKE_Bank instance = new MIKE_Bank();
        instance.mikeguid = guid2;
        instance.mikebank = mikebankReturn;

        return instance;
    }
}

modded enum ENotification
{
    PLAYER_REWARD = 75,
}

class MikesMethods
{
    //------------------------------------------------------------------------------------------------
    //! Asynchronously sets local license records for a player
    void SetLocalLicenseRecordsAsync(IEntity player)
    {
        if (Replication.IsClient())
        {
            // Print("SetLocalLicenseRecordsAsync running on client, exiting..", LogLevel.NORMAL); // DEBUG
            return;
        }

        int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player);
        string playerGUID = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);

        // Get the connection info
        EDF_JsonFileDbConnectionInfo connectInfo = new EDF_JsonFileDbConnectionInfo();
        connectInfo.m_sDatabaseName = "LicenseInfo";

        // Get a db context instance
        EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);

        // Interact with the DB context through a repository
        EDF_DbRepository<TAG_LicenseInfo> repository = EDF_DbEntityHelper<TAG_LicenseInfo>.GetRepository(dbContext);

        // Create context to pass the player entity
        Tuple1<SCR_ChimeraCharacter> context = new Tuple1<SCR_ChimeraCharacter>(SCR_ChimeraCharacter.Cast(player));

        // Define license types to check
        array<string> licenseTypes = { "drivers", "civhandgun", "tier2civhandgun" };

        foreach (string license : licenseTypes)
        {
            // Define the condition for each license
            array<ref EDF_DbFindCondition> conditions = new array<ref EDF_DbFindCondition>();
            conditions.Insert(EDF_DbFind.Field("guid1").Equals(playerGUID));
            conditions.Insert(EDF_DbFind.Field("licensename").Equals(license));

            EDF_DbFindCondition condition = EDF_DbFind.And(conditions);

            // Set up the callback handler
            EDF_DbFindCallbackSingle<TAG_LicenseInfo> findRecordHandler = new EDF_DbFindCallbackSingle<TAG_LicenseInfo>(this, "FindLicenseRecord", context);
            repository.FindFirstAsync(condition, callback: findRecordHandler);
        }
    }

    //------------------------------------------------------------------------------------------------
    //! Callback function after finding a license record
    void FindLicenseRecord(EDF_EDbOperationStatusCode statusCode, TAG_LicenseInfo result, Managed userData)
    {
        Tuple1<SCR_ChimeraCharacter> typedContext = Tuple1<SCR_ChimeraCharacter>.Cast(userData);
        SCR_ChimeraCharacter currentUserChar = typedContext.param1; // Retrieve the player character

        if (statusCode != EDF_EDbOperationStatusCode.SUCCESS || result == null)
        {
            // Print("License not found for user: " + currentUserChar.GetName(), LogLevel.NORMAL); // DEBUG
            return;
        }

        if (result.licensename == "drivers")
        {
            currentUserChar.driversLicenseExists = true;
            // Print("Drivers license found for user: " + currentUserChar.GetName(), LogLevel.NORMAL); // DEBUG
        }
        else if (result.licensename == "civhandgun")
        {
            currentUserChar.basicGunLicenseExists = true;
            // Print("Basic Gun license found for user: " + currentUserChar.GetName(), LogLevel.NORMAL); // DEBUG
        }
        else if (result.licensename == "tier2civhandgun")
        {
            currentUserChar.tier2GunLicenseExists = true;
            // Print("Tier 2 Gun license found for user: " + currentUserChar.GetName(), LogLevel.NORMAL); // DEBUG
        }

        if (statusCode == EDF_EDbOperationStatusCode.SUCCESS && result != null)
        {
            currentUserChar.recentLicense = result.licensename;
            // licenseExists = true;
            // Print("License found for user: " + currentUserChar.GetName(), LogLevel.NORMAL); // DEBUG
        }
        else
        {
            // licenseExists = false;
            // Print("License not found for user: " + currentUserChar.GetName(), LogLevel.NORMAL); // DEBUG
        }
    }
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

    //------------------------------------------------------------------------------------------------
    override void OnPlayerSpawned(int playerId, IEntity controlledEntity)
    {
        super.OnPlayerSpawned(playerId, controlledEntity);

        if (Replication.IsClient())
            return;

//        int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(controlledEntity);
        string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
        SCR_ChimeraCharacter playerChar = SCR_ChimeraCharacter.Cast(controlledEntity);
        playerChar.SetUid(uid);

        // Check for existing cash record and load cash
        HasAccount(playerId, true);

        // Initialize balance values and request balances from server
        playerChar.m_iBalance = 0;

        MikesMethods Mikes = new MikesMethods();
        Mikes.SetLocalLicenseRecordsAsync(controlledEntity);

        GetGame().GetCallqueue().CallLater(RefreshBalance, 1000, false, playerId);
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
            EDF_JsonFileDbConnectionInfo connectInfo = new EDF_JsonFileDbConnectionInfo();
            connectInfo.m_sDatabaseName = "BankDB";

            // Create context to pass into callback
            Tuple1<int> context = new Tuple1<int>(playerId);

            // Get a db context instance
            EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
            // Interact with the DB context through a repository
            EDF_DbRepository<MIKE_Bank> repository = EDF_DbEntityHelper<MIKE_Bank>.GetRepository(dbContext);
            // Now find the record
            EDF_DbFindCondition condition = EDF_DbFind.Field("mikeguid").Equals(uid);
            repository.FindFirstAsync(condition, callback: new EDF_DbFindCallbackSingle<MIKE_Bank>(this, "OnHasAccountFound", context));
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
    //! \param[in] Managed userData The context data passed from the callback
    void OnHasAccountFound(EDF_EDbOperationStatusCode statusCode, MIKE_Bank result, Managed userData)
    {
        string uid;
        if (result != null)
        {
            uid = result.mikeguid;
        }
        else
        {
            // Retrieve context
            Tuple1<int> typedContext = Tuple1<int>.Cast(userData);
            int playerID = typedContext.param1;

            BackendApi api = GetGame().GetBackendApi();
            uid = api.GetPlayerIdentityId(playerID);

            // Account not found, create new account
            CreateAccount(uid);

            return; // Exit the callback early since account creation is handled
        }

        // Retrieve context
        Tuple1<int> typedContextSuccess = Tuple1<int>.Cast(userData);
        int successPlayerID = typedContextSuccess.param1;

        SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(successPlayerID));
        if (char)
        {
            char.SetHasAccount(true);
        }

        if (char && m_bEnableCash)
        {
            char.LoadCash();
        }

        // Save balance to record
        if (char)
        {
            char.SaveCash(-1, true);
        }
    }

    //------------------------------------------------------------------------------------------------
    //! No balance file was found, so server needs to create one and fill it with starting values
    //! \param[in] string uid Unique player identity id
    void CreateAccount(string uid)
    {
        if (Replication.IsServer())
        {
            EDF_JsonFileDbConnectionInfo connectInfo = new EDF_JsonFileDbConnectionInfo();
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
    void RefreshBalance(int playerId)
    {
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
            ReadBalance(uid, playerId);
        }
        else
        {
            Rpc(RpcAsk_RefreshBalance, playerId);
        }

        GetGame().GetCallqueue().CallLater(RefreshBalance, 60000, false, playerId);
    }

    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_RefreshBalance(int playerId)
    {
        string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
        ReadBalance(uid, playerId);
    }

    //------------------------------------------------------------------------------------------------
    void ReadBalance(string uid, int playerId)
    {
        // Create context to pass into callback
        Tuple1<int> context = new Tuple1<int>(playerId);

        EDF_JsonFileDbConnectionInfo connectInfo = new EDF_JsonFileDbConnectionInfo();
        connectInfo.m_sDatabaseName = "BankDB";

        // Get a db context instance
        EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
        // Interact with the DB context through a repository
        EDF_DbRepository<MIKE_Bank> repository = EDF_DbEntityHelper<MIKE_Bank>.GetRepository(dbContext);
        // Now find the record
        EDF_DbFindCondition condition = EDF_DbFind.Field("mikeguid").Equals(uid);
        repository.FindFirstAsync(condition, callback: new EDF_DbFindCallbackSingle<MIKE_Bank>(this, "OnBalanceRead", context));
    }

    //------------------------------------------------------------------------------------------------
    //! Callback function after reading balance
    //! \param[in] EDF_EDbOperationStatusCode statusCode Operation status
    //! \param[in] MIKE_Bank result The result from the database query
    //! \param[in] Managed userData The context data passed from the callback
    void OnBalanceRead(EDF_EDbOperationStatusCode statusCode, MIKE_Bank result, Managed userData)
    {
        if (result == null)
            return;

        // Retrieve context
        Tuple1<int> typedContext = Tuple1<int>.Cast(userData);
        int playerId = typedContext.param1;

        SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId));
        if (char)
        {
            int fileBalance = result.mikebank;

            if (char.m_iBalance != fileBalance)
            {
                char.SetBalance(fileBalance);
            }
        }
    }

    //------------------------------------------------------------------------------------------------
    //! Adjusts the player's balance
    //! \param[in] int playerId Unique player identifier for current session
    //! \param[in] string currencySymbol Currency identifier (not used here but kept for compatibility)
    //! \param[in] int amount Amount to be added to current balance (positive value for addition, negative for removal)
    void AdjustBalance(int playerId, string currencySymbol, int amount)
    {
        if (Replication.IsServer())
        {
            AdjustBalanceInternal(playerId, amount);
        }
        else
        {
            Rpc(RpcAsk_AdjustBalance, playerId, currencySymbol, amount);
        }
    }

    //------------------------------------------------------------------------------------------------
    //! Adjusts the player's balance and notifies them
    //! \param[in] int playerId Unique player identifier for current session
    //! \param[in] string currencySymbol Currency identifier (not used here but kept for compatibility)
    //! \param[in] int amount Amount to be added to current balance
    //! \param[in] string message Notification message
    void AdjustBalanceAndNotify(int playerId, string currencySymbol, int amount, string message)
    {
        if (Replication.IsServer())
        {
            AdjustBalanceInternal(playerId, amount);
            NotifyPlayer(playerId, amount, message);
        }
        else
        {
            Rpc(RpcAsk_AdjustBalanceAndNotify, playerId, currencySymbol, amount, message);
        }
    }

    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_AdjustBalance(int playerId, string currencySymbol, int amount)
    {
        AdjustBalanceInternal(playerId, amount);
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_AdjustBalanceAndNotify(int playerId, string currencySymbol, int amount, string message)
    {
        AdjustBalanceInternal(playerId, amount);
        NotifyPlayer(playerId, amount, message);
    }

    //------------------------------------------------------------------------------------------------
    void AdjustBalanceInternal(int playerId, int amount)
    {
        string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);

        // Create context to pass into callback
        Tuple2<int, int> context = new Tuple2<int, int>(playerId, amount);

        EDF_JsonFileDbConnectionInfo connectInfo = new EDF_JsonFileDbConnectionInfo();
        connectInfo.m_sDatabaseName = "BankDB";

        EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
        EDF_DbRepository<MIKE_Bank> repository = EDF_DbEntityHelper<MIKE_Bank>.GetRepository(dbContext);

        EDF_DbFindCondition condition = EDF_DbFind.Field("mikeguid").Equals(uid);
        repository.FindFirstAsync(condition, callback: new EDF_DbFindCallbackSingle<MIKE_Bank>(this, "OnAdjustBalanceRead", context));
    }

    //------------------------------------------------------------------------------------------------
    //! Callback function after adjusting balance
    //! \param[in] EDF_EDbOperationStatusCode statusCode Operation status
    //! \param[in] MIKE_Bank result The result from the database query
    //! \param[in] Managed userData The context data passed from the callback
    void OnAdjustBalanceRead(EDF_EDbOperationStatusCode statusCode, MIKE_Bank result, Managed userData)
    {
        if (result == null)
            return;

        // Retrieve context
        Tuple2<int, int> typedContext = Tuple2<int, int>.Cast(userData);
        int playerId = typedContext.param1;
        int amount = typedContext.param2;

        int currentBalance = result.mikebank;

        // Server-side validation
        if (amount < 0 && Math.AbsInt(amount) > currentBalance)
        {
            NotifyPlayer(playerId, 0, "Insufficient funds for withdrawal.");
            return;
        }

        int newBalance = currentBalance + amount;

        // Update database
        string uid = result.mikeguid;
        MIKE_Bank newEntry = MIKE_Bank.Create(uid, newBalance);
        newEntry.SetId(uid);

        EDF_JsonFileDbConnectionInfo connectInfo = new EDF_JsonFileDbConnectionInfo();
        connectInfo.m_sDatabaseName = "BankDB";
        EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
        EDF_DbRepository<MIKE_Bank> repository = EDF_DbEntityHelper<MIKE_Bank>.GetRepository(dbContext);

        repository.AddOrUpdateAsync(newEntry);

        // Update player's character
        SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId));
        if (char)
        {
            char.SetBalance(newBalance);
        }
    }

    //------------------------------------------------------------------------------------------------
    //! Notifies the player with a message and amount
    //! \param[in] int playerId Unique player identifier for current session
    //! \param[in] int amount Amount involved in the notification
    //! \param[in] string message Notification message
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
    //! \param[in] string symbol Currency identifier
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
