class MIKE_LicensedShopAction : ScriptedUserAction
{
	[Attribute(defvalue: "civhandgun", desc: "This is the variable name used in the database to store the license.", uiwidget: UIWidgets.EditBox, category: "Physical Shop")]
    protected string LicenseName;
    protected ADM_ShopComponent m_Shop;
	bool licenseExists = false;

    //------------------------------------------------------------------------------------------------
    override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent) 
    {
        m_Shop = ADM_ShopComponent.Cast(pOwnerEntity.FindComponent(ADM_ShopComponent));
        if (!m_Shop) 
        {
            Print("Error: Could not find shop component for this action.", LogLevel.NORMAL);
            return;
        }
    }
    
    //------------------------------------------------------------------------------------------------
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
    {   
        // First, check if the player is allowed to perform the action
        if (!IsPlayerAllowed(pUserEntity))
        {
            // Optionally, provide feedback to the player
            // Example: ShowNotification(pUserEntity, "You do not have the required faction to access the shop.");
          //  Print("Access Denied: Player does not belong to the '" + REQUIRED_FACTION_KEY + "' faction.", LogLevel.NORMAL);
            return; // Exit the method without performing the action
        }

        if (!m_Shop || m_Shop.GetMerchandiseBuy().Count() <= 0) return;
        
        SCR_PlayerController scrPlayerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
        if (!scrPlayerController || pUserEntity != scrPlayerController.GetMainEntity()) 
            return;
        
        MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.ADM_ShopMenu); 
        ADM_ShopUI menu = ADM_ShopUI.Cast(menuBase);
        menu.SetShop(m_Shop);
    }
    
    //------------------------------------------------------------------------------------------------
    override bool CanBeShownScript(IEntity user)
    {
        if (!m_Shop || !m_Shop.GetMerchandiseAll() || m_Shop.GetMerchandiseAll().Count() <= 0) 
            return false;
            
        // Check if the player is allowed to perform the action
        bool isAllowed = IsPlayerAllowed(user);
        return isAllowed;
    }
    
    //------------------------------------------------------------------------------------------------
    override bool CanBePerformedScript(IEntity user)
    {
        if (!m_Shop || m_Shop.GetMerchandiseAll().Count() <= 0) 
            return false;
        
        // Check if the player is allowed to perform the action
        bool isAllowed = IsPlayerAllowed(user);
        return isAllowed;
    }
    
    //------------------------------------------------------------------------------------------------
    override bool GetActionNameScript(out string outName)
    {
        outName = "View Shop";
        
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    override bool HasLocalEffectOnlyScript()
    {
        return true;
    }
    
    //----------------------------------------------------------------------------------
    override bool CanBroadcastScript() 
    { 
        return false; 
    }

    //------------------------------------------------------------------------------------------------
    /**
     * Helper function to determine if the player is allowed to perform the shop action based on faction
     */
    private bool IsPlayerAllowed(IEntity pUserEntity)
    {
        if (!Replication.IsServer()) return false;

        // Get the BackendApi and the player's GUID
        BackendApi api = GetGame().GetBackendApi();
        int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
        string playerGUID = api.GetPlayerIdentityId(playerID);

        // Get the connection info
        EDF_JsonFileDbConnectionInfo connectInfo();
        connectInfo.m_sDatabaseName = "LicenseInfo";

        // Get a db context instance
        EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);

        // Interact with the DB context through a repository
        EDF_DbRepository<MIKE_LicenseInfo> repository = EDF_DbEntityHelper<MIKE_LicenseInfo>.GetRepository(dbContext);

        // Now find the record
        EDF_DbFindCondition condition = EDF_DbFind.And({
            EDF_DbFind.Field("guid").Contains(playerGUID),
            EDF_DbFind.Field("licensename").Contains(LicenseName)
        });
        EDF_DbFindCallbackSingle<MIKE_LicenseInfo> findRecordHandler(this, "FindRecord");
        repository.FindFirstAsync(condition, callback: findRecordHandler);

        // Check if the player already has the license
        if (licenseExists == true)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
	void FindRecord(EDF_EDbOperationStatusCode statusCode, MIKE_LicenseInfo result)
    {

        if (result)
        {
            licenseExists = true;
        }
        else
        {
            licenseExists = false;
        }
    }
};
