class ADM_PoliceShopAction : ScriptedUserAction
{
    protected ADM_ShopComponent m_Shop;

    // Define the required faction key
    private const string REQUIRED_FACTION_KEY = "US";

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
            Print("Access Denied: Player does not belong to the '" + REQUIRED_FACTION_KEY + "' faction.", LogLevel.NORMAL);
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
        if (!isAllowed)
        {
            Print("Access Denied in CanBeShownScript: Player does not belong to the '" + REQUIRED_FACTION_KEY + "' faction.", LogLevel.NORMAL);
        }
        return isAllowed;
    }
    
    //------------------------------------------------------------------------------------------------
    override bool CanBePerformedScript(IEntity user)
    {
        if (!m_Shop || m_Shop.GetMerchandiseAll().Count() <= 0) 
            return false;
        
        // Check if the player is allowed to perform the action
        bool isAllowed = IsPlayerAllowed(user);
        if (!isAllowed)
        {
            Print("Access Denied in CanBePerformedScript: Player does not belong to the '" + REQUIRED_FACTION_KEY + "' faction.", LogLevel.NORMAL);
        }
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
        // Get the FactionAffiliationComponent of the player
        FactionAffiliationComponent userFactionAffiliation = FactionAffiliationComponent.Cast(pUserEntity.FindComponent(FactionAffiliationComponent));
        if (!userFactionAffiliation)
        {
            Print("Error: Player has no FactionAffiliationComponent.", LogLevel.NORMAL);
            return false; // Player has no faction affiliation component, deny access
        }
        
        // Get the player's faction
        Faction userFaction = userFactionAffiliation.GetAffiliatedFaction();
        if (!userFaction)
        {
            Print("Error: Player has no affiliated faction.", LogLevel.NORMAL);
            return false; // Player has no faction, deny access
        }

        // Get the faction key
        string userFactionKey = userFaction.GetFactionKey(); // Assumes GetFactionKey() is available
        if (userFactionKey == "")
        {
            Print("Error: Player's faction key is empty.", LogLevel.NORMAL);
            return false;
        }

        // Compare the faction key
        if (userFactionKey == REQUIRED_FACTION_KEY)
        {
            Print("Player belongs to the required faction '" + REQUIRED_FACTION_KEY + "'.", LogLevel.NORMAL);
            return true; // Player belongs to the required faction
        }
        
        // Faction key does not match
        Print("Access Denied: Player's faction '" + userFactionKey + "' does not match the required faction '" + REQUIRED_FACTION_KEY + "'.", LogLevel.NORMAL);
        return false;
    }
};
