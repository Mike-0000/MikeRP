class MIKE_LicensedShopAction : ScriptedUserAction
{
    [Attribute(defvalue: "civhandgun", desc: "This is the variable name used in the database to store the license.", uiwidget: UIWidgets.EditBox, category: "Physical Shop")]
    protected string LicenseName;
    protected ADM_ShopComponent m_Shop;
    //bool licenseExists = false;

    // **New Class-Level Field**
//  private SCR_ChimeraCharacter currentUserChar; // To store the userChar reference

    //------------------------------------------------------------------------------------------------
    override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent) 
    {
        m_Shop = ADM_ShopComponent.Cast(pOwnerEntity.FindComponent(ADM_ShopComponent));
        if (!m_Shop) 
        {
            //Print("Error: Could not find shop component for this action.", LogLevel.NORMAL);
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
            //Print("Access Denied", LogLevel.NORMAL);
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
            
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    override bool CanBePerformedScript(IEntity user)
    {
        if (!m_Shop || m_Shop.GetMerchandiseAll().Count() <= 0) 
            return false;
        
        // Check if the player is allowed to perform the action
        bool isAllowed = IsPlayerAllowed(user);
        if (!isAllowed)
            SetCannotPerformReason("No License Found");
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
		//Print("IsPlayerAllowed Invoked!", LogLevel.NORMAL); // DEBUG
        if (!Replication.IsServer()){
            //Print("Replication on Client Exiting", LogLevel.NORMAL); // DEBUG
            //return false;
      		} 

        
		
		
		
		
		

        // **Note**: Since the DB call is asynchronous, the licenseExists flag will be set in the callback.
        // Therefore, the following synchronous check might not reflect the DB result immediately.
        // Consider handling the flow accordingly.
        
        // Check if the player already has the license locally
		////Print("if (currentUserChar.recentLicense == LicenseName) Invoked. Current License = " + currentUserChar.recentLicense + "LicenseName =" + LicenseName, LogLevel.NORMAL); // DEBUG
        SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (LicenseName == "drivers"){
		
			if(char.driversLicenseExists)
				return true;
			
		}
		else if (LicenseName == "civhandgun"){
			if(char.basicGunLicenseExists)
				return true;
		
		}
		else if (LicenseName == "tier2civhandgun"){
			if(char.tier2GunLicenseExists)
				return true;
		}
		return false;
    }

};
