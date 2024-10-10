// KF_ChimeraCharacter.c

[EDF_DbName.Automatic()]
class MIKE_Cash : EDF_DbEntity
{
    string mikeguid;
    int mikecash;

    //------------------------------------------------------------------------------------------------
    //! Db entities cannot have a constructor with parameters; this is a limitation of the engine.
    //! Consult the docs for more info on this.
    static MIKE_Cash Create(string guid2, int mikecashReturn)
    {
        MIKE_Cash instance();
        instance.mikeguid = guid2;
        instance.mikecash = mikecashReturn;

        return instance;
    }
}

modded class SCR_ChimeraCharacter
{
	[RplProp()]
    bool basicGunLicenseExists = false;
	[RplProp()]
    bool driversLicenseExists = false;
	[RplProp()]
    bool tier2GunLicenseExists = false;
	[RplProp()]
    string recentLicense = "";
    [RplProp()]
    bool m_bIsBusy = false;
    [RplProp()]
    string m_sPlayerUid;

    [RplProp()]
    bool m_bHasAccount;

    [RplProp()]
    int m_iBalance = 0;

    [RplProp()]
    int m_iCash;

    [RplProp()]
    bool m_bRecentDeath;

	void SetBusyState(bool isBusy)
    {
        m_bIsBusy = isBusy;
		Replication.BumpMe(); // Ensure the state is synchronized across clients and server
        //Print("[SCR_ChimeraCharacter] SetBusyState: " + isBusy.ToString(), LogLevel.DEBUG);
    }
	bool IsBusy()
    {
        return m_bIsBusy;
    }
    //------------------------------------------------------------------------------------------------
    //! Assigns replicated player uid
    //! \param[in] string uid Unique player identity id
    void SetUid(string uid)
    {
        if (Replication.IsServer())
        {
            m_sPlayerUid = uid;
            Replication.BumpMe();
        }
        else
        {
            Rpc(RpcDo_SetUid, uid);
        }
    }

    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    void RpcDo_SetUid(string uid)
    {
        m_sPlayerUid = uid;
        Replication.BumpMe();
    }

    //------------------------------------------------------------------------------------------------
    //! Assigns replicated currency balance
    //! \param[in] int newBalance To override existing balance value
    void SetBalance(int newBalance)
    {
        if (Replication.IsServer())
        {
            m_iBalance = newBalance;
            Replication.BumpMe();
        }
        else
        {
            Rpc(RpcAsk_SetBalance, newBalance);
        }
    }

    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_SetBalance(int newBalance)
    {
        m_iBalance = newBalance;
        Replication.BumpMe();
    }

    //------------------------------------------------------------------------------------------------
    //! Tell ChimeraCharacter whether server holds account info
    //! \param[in] bool hasAccount State of account ownership
    void SetHasAccount(bool hasAccount)
    {
        if (Replication.IsServer())
        {
            m_bHasAccount = hasAccount;
            Replication.BumpMe();
        }
        else
        {
            Rpc(RpcDo_SetHasAccount, hasAccount);
        }
    }

    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    void RpcDo_SetHasAccount(bool hasAccount)
    {
        m_bHasAccount = hasAccount;
        Replication.BumpMe();
    }

    //------------------------------------------------------------------------------------------------
    //! Sets/Adds replicated cash amount
    //! \param[in] int cashAmount
    //! \param[in] bool addition True to add cashAmount to current cash, false to set cashAmount as current cash
    void SetCash(int cashAmount, bool addition)
    {
        if (Replication.IsServer())
        {
            if (!addition)
                m_iCash = cashAmount;
            else
                m_iCash += cashAmount;

            Replication.BumpMe();
        }
        else
        {
            Rpc(RpcAsk_SetCash, cashAmount, addition);
        }
    }

    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_SetCash(int cashAmount, bool addition)
    {
        if (!addition)
            m_iCash = cashAmount;
        else
            m_iCash += cashAmount;

        Replication.BumpMe();
    }

    //------------------------------------------------------------------------------------------------
    void SaveCash(int cashOverride = -1, bool repeat = false)
    {
        if (Replication.IsServer())
        {
            int cashToSave = m_iCash;
            if (cashOverride != -1)
                cashToSave = cashOverride;

            int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(this);
            string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);

            EDF_JsonFileDbConnectionInfo connectInfo();
            connectInfo.m_sDatabaseName = "CashDB";
            EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
            EDF_DbRepository<MIKE_Cash> repository = EDF_DbEntityHelper<MIKE_Cash>.GetRepository(dbContext);

            MIKE_Cash newEntry = MIKE_Cash.Create(uid, cashToSave);
            newEntry.SetId(uid);
            repository.AddOrUpdateAsync(newEntry);

            if (repeat && !m_bRecentDeath)
               GetGame().GetCallqueue().CallLater(SaveCash, 20000, true, -1, false);
            m_bRecentDeath = false;
        }
        else
        {
            Rpc(RpcAsk_SaveCash, cashOverride, repeat);
        }
    }

    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_SaveCash(int cashOverride, bool repeat)
    {
        SaveCash(cashOverride, repeat);
    }

    //------------------------------------------------------------------------------------------------
    void LoadCash()
    {
        if (Replication.IsServer())
        {
            int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(this);
            string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);

            EDF_JsonFileDbConnectionInfo connectInfo();
            connectInfo.m_sDatabaseName = "CashDB";

            // Get a db context instance
            EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
            // Interact with the DB context through a repository
            EDF_DbRepository<MIKE_Cash> repository = EDF_DbEntityHelper<MIKE_Cash>.GetRepository(dbContext);
            // Now find the record
            EDF_DbFindCondition condition = EDF_DbFind.Field("mikeguid").Equals(uid);
            repository.FindFirstAsync(condition, callback: new EDF_DbFindCallbackSingle<MIKE_Cash>(this, "OnCashLoaded"));
        }
        else
        {
            Rpc(RpcAsk_LoadCash);
        }
    }

    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_LoadCash()
    {
        LoadCash();
    }

    void OnCashLoaded(EDF_EDbOperationStatusCode statusCode, MIKE_Cash result)
    {
        int loadedCash = 0;
        if (result != null)
        {
            loadedCash = result.mikecash;
            //Print("Loaded cash: " + loadedCash.ToString(), LogLevel.ERROR);
        }
        else
        {
            //Print("No cash record found.", LogLevel.ERROR);
        }

        SetCash(loadedCash, false);
    }
}
























//KF_ChimeraCharacter.c
//
//[EDF_DbName.Automatic()]
//class MIKE_Cash : EDF_DbEntity
//{
//    string mikeguid;
//    int mikecash;
//
//
//    //------------------------------------------------------------------------------------------------
//    //! Db entities cannot have a constructor with parameters; this is a limitation of the engine.
//    //! Consult the docs for more info on this.
//    static MIKE_Cash Create(string guid2, int mikecashReturn)
//    {
//        MIKE_Cash instance();
//        instance.mikeguid = guid2;
//        instance.mikecash = mikecashReturn;
//
//        return instance;
//    }
//};






//modded class SCR_ChimeraCharacter
//{
//	bool accountExists = false;
//	ref MIKE_Cash cashResults = new MIKE_Cash();
//	[RplProp()]
//	string m_sPlayerUid;
//	
//	[RplProp()]
//	bool m_bHasAccount;
//	
//	[RplProp()]
//	int m_iBalance = 0;
//	//ref array<int> m_aBalances = {};
//	
//	[RplProp()]
//	int m_iCash;
//	
//	[RplProp()]
//	bool m_bRecentDeath;
//	
//	//------------------------------------------------------------------------------------------------
//	//! Assigns replicated player uid
//	//! \param[in] string uid Unique player identity id
//	void SetUid(string uid)
//	{
//		Rpc(RpcDo_SetUid, uid);
//	}
//	
//	//------------------------------------------------------------------------------------------------
//    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
//    void RpcDo_SetUid(string uid)
//    {
//		m_sPlayerUid = uid;
//		Replication.BumpMe();
//	}
//	
//	
//	void FindRecord(EDF_EDbOperationStatusCode statusCode, MIKE_Cash result)
//    {
//		cashResults = result;
//        if (result)
//        {
//            //Print(string.Format("Account found!"), LogLevel.ERROR);
//			
//            accountExists = true;
//        }
//        else
//        {
//            //Print("Account not found!", LogLevel.ERROR);
//            accountExists = false;
//        }
//    }
//	
//	
//	//------------------------------------------------------------------------------------------------
//	//! Assigns replicated currency balance 
//	//! \param[in] int index Index of currency in currency-array of currency-config
//	//! \param[in] int newBalance To override existing balance value in balance-array
//	//void SetBalance(int index, int newBalance)
//	void SetBalance(int newBalance)
//	{
//		//if (Replication.IsServer()) m_aBalances.Set(index, newBalance);
//		if (Replication.IsServer()) m_iBalance = newBalance;
//		
//		//Rpc(RpcDo_SetBalance, index, newBalance);
//		Rpc(RpcDo_SetBalance, newBalance);
//	}
//	
//	//------------------------------------------------------------------------------------------------
//    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
//    //void RpcDo_SetBalance(int index, int newBalance)
//	void RpcDo_SetBalance(int newBalance)
//    {
//		//m_aBalances.Set(index, newBalance);
//		m_iBalance = newBalance;
//		Replication.BumpMe();
//	}
//	
//	//------------------------------------------------------------------------------------------------
//	//! Tell ChimeraCharacter whether server holds account info
//	//! \param[in] bool hasAccount State of account ownership
//	void SetHasAccount(bool hasAccount)
//	{
//		if (Replication.IsServer()) m_bHasAccount = hasAccount;
//		
//		Rpc(RpcDo_SetHasAccount, hasAccount);
//	}
//	
//	//------------------------------------------------------------------------------------------------
//    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
//    void RpcDo_SetHasAccount(bool hasAccount)
//    {
//		m_bHasAccount = hasAccount;
//		Replication.BumpMe();
//	}
//	
//	/*
//	//------------------------------------------------------------------------------------------------
//	//! Initialize balance array in desired size. Necessary for SetBalance(...) to work
//	//! \param[in] int size Desired amount of currencies stored
//	void InitBalanceArray(int size)
//	{
//		if (Replication.IsServer())
//		{
//			for (int i = 0; i < size; i++)
//			{
//				m_aBalances.Insert(-1);
//			}
//		}
//		
//		Rpc(RpcDo_InitBalanceArray, size);
//	}
//	
//	//------------------------------------------------------------------------------------------------
//    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
//    void RpcDo_InitBalanceArray(int size)
//    {
//		for (int i = 0; i < size; i++)
//		{
//			m_aBalances.Insert(-1);
//		}
//		
//		Replication.BumpMe();
//	}
//	*/
//	
//	//------------------------------------------------------------------------------------------------
//	//! Sets/Adds replicated cash amount
//	//! \param[in] int cashAmount
//	void SetCash(int cashAmount, bool addition)
//	{
//		if (Replication.IsServer())
//		{
//			if (!addition) m_iCash = cashAmount;
//			else m_iCash += cashAmount;
//		}
//		
//		Rpc(RpcDo_SetCash, cashAmount, addition);
//	}
//	
//	//------------------------------------------------------------------------------------------------
//    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
//    void RpcDo_SetCash(int cashAmount, bool addition)
//    {
//		if (!addition) m_iCash = cashAmount;
//		else m_iCash += cashAmount;
//		
//		Replication.BumpMe();
//	}
//	
//	//------------------------------------------------------------------------------------------------
//	void SaveCash(int cashOverride = -1, bool repeat = false)
//	{
//		Rpc(RpcAsk_SaveCash, cashOverride);
//		
//		if (repeat && !m_bRecentDeath) GetGame().GetCallqueue().CallLater(SaveCash, 1000, true, -1, false);
//		m_bRecentDeath = false;
//	}
//	
//	//------------------------------------------------------------------------------------------------
//	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
//	void RpcAsk_SaveCash(int cashOverride)
//	{
//		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(this);
//		string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
//		
//		// Create Money database entry and write initial balance
//		EDF_JsonFileDbConnectionInfo connectInfo();
//		connectInfo.m_sDatabaseName = "CashDB";
//		EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
//		EDF_DbRepository<MIKE_Cash> repository = EDF_DbEntityHelper<MIKE_Cash>.GetRepository(dbContext);
//		if (cashOverride == -1){
//			MIKE_Cash newEntry = MIKE_Cash.Create(uid, m_iCash);
//			newEntry.SetId(uid);
//			repository.AddOrUpdateAsync(newEntry);
//		}
//		else{
//			MIKE_Cash newEntry = MIKE_Cash.Create(uid, cashOverride);
//			newEntry.SetId(uid);
//			repository.AddOrUpdateAsync(newEntry);
//		}
//				
////		if (cashOverride == -1) balanceFile.Write(m_iCash.ToString());
////		else balanceFile.Write(cashOverride.ToString());
//	}
//	
//	//------------------------------------------------------------------------------------------------
//	void LoadCash()
//	{
//	    if (!Replication.IsServer())
//	    {
//	        Rpc(RpcAsk_LoadCash);
//	        return;
//	    }
//	
//	    int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(this);
//	    string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
//	
//	    EDF_JsonFileDbConnectionInfo connectInfo();
//	    connectInfo.m_sDatabaseName = "CashDB";
//	
//	    EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
//	    EDF_DbRepository<MIKE_Cash> repository = EDF_DbEntityHelper<MIKE_Cash>.GetRepository(dbContext);
//	    EDF_DbFindCondition condition = EDF_DbFind.Field("mikeguid").Equals(uid);
//	    repository.FindFirstAsync(condition, callback: new EDF_DbFindCallbackSingle<MIKE_Cash>(this, "OnCashLoaded"));
//	}
//	
//	void OnCashLoaded(EDF_EDbOperationStatusCode statusCode, MIKE_Cash result)
//	{
//	    if (result)
//	    {
//	        SetCash(result.mikecash, false);
//	    }
//	    else
//	    {
//	        // Handle case where cash data does not exist
//	        SetCash(0, false);
//	    }
//	}
//
//	
//	//------------------------------------------------------------------------------------------------
//	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
//	void RpcAsk_LoadCash()
//	{
//		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(this);
//		string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
//		
//		EDF_JsonFileDbConnectionInfo connectInfo();
//		connectInfo.m_sDatabaseName = "CashDB";
//
//		// Get a db context instance
//		EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
//		// Interact with the DB context through a repository
//		EDF_DbRepository<MIKE_Bank> repository = EDF_DbEntityHelper<MIKE_Bank>.GetRepository(dbContext);
//		// Now find the record
//		EDF_DbFindCondition condition = EDF_DbFind.Field("mikeguid").Equals(uid);
//    	EDF_DbFindCallbackSingle<MIKE_Bank> findRecordHandler(this, "FindRecord");
//		repository.FindFirstAsync(condition, callback: findRecordHandler);
//		//return cashResults.mikecash;
//		//Print(cashResults.mikecash.ToString(), LogLevel.ERROR);
//		SetCash(cashResults.mikecash, false);
//	}
//}