[EDF_DbName.Automatic()]
class TAG_MikeGarage : EDF_DbEntity
{
    string guid;
    
    ResourceName VehiclePrefab;
    int payment;

    //------------------------------------------------------------------------------------------------
    //! Db entities cannot have a constructor with parameters; this is a limitation of the engine.
    //! Consult the docs for more info on this.
    static TAG_MikeGarage Create(string guid2, ResourceName text, int payment1)
    {
        TAG_MikeGarage instance = new TAG_MikeGarage(); // Corrected instantiation
        Print("TAG_MikeGarage instance created.", LogLevel.NORMAL);
        
        instance.guid = guid2;
        Print(string.Format("Set guid: %1", guid2), LogLevel.NORMAL);
        
        instance.VehiclePrefab = text;
        Print(string.Format("Set VehiclePrefab: %1", text), LogLevel.NORMAL);
        
        instance.payment = payment1;
        Print(string.Format("Set payment: %1", payment1), LogLevel.NORMAL);
        
        return instance;
    }
};

class MIKE_GarageComponentClass: ADM_ShopBaseComponentClass {}
class MIKE_GarageComponent: ADM_ShopBaseComponent
{
	//MIKE_GarageComponent currentGarageComp;
	[Attribute()]
	ref PointInfo m_SpawnPosition;
	
	
	override array<ref ADM_ShopMerchandise> GetMerchandiseSell()
	{

		return null;
	}
	ref array<ref ADM_ShopCategory> m_LoadedCategories = {};
    array<ref ADM_ShopCategory> GetCategories()
    {
        Print("GetCategories called.", LogLevel.NORMAL);
        return m_LoadedCategories;
    }

}




class MIKE_GarageAction : ScriptedUserAction
{
	//proto void Rpc(func method, void p0 = NULL, void p1 = NULL, void p2 = NULL, void p3 = NULL, void p4 = NULL, void p5 = NULL, void p6 = NULL, void p7 = NULL);
    protected MIKE_GarageComponent m_Shop;
	[RplProp()]
	MIKE_GarageComponent currentGarage;
    
    //------------------------------------------------------------------------------------------------
    override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent) 
    {
        Print("Init called for MIKE_GarageAction.", LogLevel.NORMAL);
        
        m_Shop = MIKE_GarageComponent.Cast(pOwnerEntity.FindComponent(MIKE_GarageComponent));
        if (!m_Shop) 
        {
            Print("Error! Could not find shop component for this action.", LogLevel.NORMAL);
            return;
        }
        Print("m_Shop component found and assigned.", LogLevel.NORMAL);
    }

	
	void LoadMerchandiseServer(string playerguid, int playerId){
		Print("Log Stage 0", LogLevel.NORMAL);

		if (Replication.IsServer())
        {
            Print("Log Stage 1", LogLevel.NORMAL);
		
			IEntity pUserEntity = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
	        SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(pUserEntity);
	            Print("Log Stage 2", LogLevel.NORMAL);

			MIKE_GarageComponent garageComp = MIKE_GarageComponent.Cast(pUserEntity.FindComponent(MIKE_GarageComponent));
	        if (!garageComp)
	        {
				Print("Log Stage 22", LogLevel.NORMAL);
	            return;
	        }
	        Print("PerformAction Passed Initial Checks.", LogLevel.NORMAL);
			garageComp.m_Merchandise.Clear();
			
			
				
				//m_Shop = MIKE_GarageComponent.Cast(garageComp);
			
			
			
			
					Print("Going for foreach", LogLevel.NORMAL);
	        foreach (ResourceName car : char.arrayOfVehiclesOwned) 
	        {
	            if (!car)
	            {
	                continue;
	            }
	            Print("Prefab Loaded: " + car, LogLevel.NORMAL);
			    ResourceName prefabName = car;
	            
	            MIKE_GaragedMerchandiseVehicle newVic = new MIKE_GaragedMerchandiseVehicle();
	            if (!newVic)
	            {
	                continue;
	            }
	            
	            newVic.m_sPrefab = car;
	            
	            newVic.m_SpawnPosition = new PointInfo();
	            if (!newVic.m_SpawnPosition)
	            {
	                continue;
	            }
	            
	            vector spawnMatrix[4];
	            spawnMatrix[0] = "1 0 0"; // Right direction
	            spawnMatrix[1] = "0 1 0"; // Up direction
	            spawnMatrix[2] = "0 0 1"; // Forward direction
	            spawnMatrix[3] = "-10 1 0"; // Spawn position coordinates (x, y, z)
	            newVic.m_SpawnPosition.Set(null, "", spawnMatrix);
	            
	            newVic.m_bIgnorePlayer = true;
	            
	            ADM_ShopMerchandise insertVic = new ADM_ShopMerchandise();
	            if (!insertVic)
	            {
	                continue;
	            }
	            
	            insertVic.m_Merchandise = newVic;
	            
	            ADM_PaymentMethodCurrency payment = new ADM_PaymentMethodCurrency();
	            if (!payment)
	            {
	                continue;
	            }
	            
	            payment.m_Quantity = 500;
	            
	            ref array<ref ADM_PaymentMethodBase> finalPayment = new array<ref ADM_PaymentMethodBase>();
	            if (!finalPayment)
	            {
	                continue;
	            }
	            
	            finalPayment.Insert(payment);
	            
	            insertVic.m_BuyPayment = finalPayment;
	            
	            garageComp.m_Merchandise.Insert(insertVic);
	        
	
	
				
		
		}
		
			RplComponent rpl = RplComponent.Cast(pUserEntity.FindComponent(RplComponent));
			if (rpl) 
			{
				rpl.InsertToReplication();
				
			}
		 }
        else
        {
			
            m_Shop.Rpc(RpcAsk_LoadMerchandise, playerguid, playerId);
        }
	}

	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_LoadMerchandise(string playerguid, int playerid)
    {
        Print("Log Stage 00", LogLevel.NORMAL);
		LoadMerchandiseServer(playerguid, playerid);
		Print("Log Stage 000", LogLevel.NORMAL);
    }
	
	    
    //------------------------------------------------------------------------------------------------
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
    {	
		
        Print("PerformAction Initiated.", LogLevel.NORMAL);
//        if (!m_Shop) 
//        {
//            Print("Error! m_Shop is null.", LogLevel.NORMAL);
//            return;
//        }

        SCR_PlayerController scrPlayerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
        if (!scrPlayerController) 
        {
            return;
        }
        Print("PerformAction Initiated.1", LogLevel.NORMAL);
        IEntity mainEntity = scrPlayerController.GetMainEntity();
        if (!mainEntity) 
        {
            return;
        }
        Print("PerformAction Initiated.2", LogLevel.NORMAL);
        if (pUserEntity != mainEntity) 
        {
            return;
        }
        Print("PerformAction Initiated.3", LogLevel.NORMAL);
       
        
        // Instantiate garageComp before using it
        MIKE_GarageComponent garageComp;
		garageComp = MIKE_GarageComponent.Cast(pUserEntity.FindComponent(MIKE_GarageComponent));
//        if (!garageComp)
//        {
//            return;
//        }
//        Print("PerformAction Passed Initial Checks.", LogLevel.NORMAL);
//		garageComp.m_Merchandise.Clear();
//		foreach (ADM_ShopMerchandise merch : shopConfig.m_Merchandise) 
//				{
//					m_Merchandise.Insert(merch);
//				}
//			}
//		}
//		
//		RplComponent rpl = RplComponent.Cast(owner.FindComponent(RplComponent));
//		if (rpl) 
//		{
//			rpl.InsertToReplication();
//		}
//	}		
		
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
        string playerGUID = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
		
		
				
//		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(pUserEntity);
		
		//Set Garage
		currentGarage = garageComp;

		LoadMerchandiseServer(playerGUID, playerId);

		
//		Print("Going for foreach", LogLevel.NORMAL);
//        foreach (ResourceName car : char.arrayOfVehiclesOwned) 
//        {
//            if (!car)
//            {
//                continue;
//            }
//            Print("Prefab Loaded: " + car, LogLevel.NORMAL);
//
//			
//		
//		}
		
//		Print("Foreach Done", LogLevel.NORMAL);
		
		
		
		
		
        MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.MIKE_GarageShopMenu);
        if (!menuBase) 
	        {
	            return;
	        }
		MIKE_GarageShopUI menu = MIKE_GarageShopUI.Cast(menuBase);
        if (!menu) 
	        {
	            //return;
	        }

        
        menu.SetShop(currentGarage);

    
		
		
		
		
		
        //garageComp.MikeBuildGarageShop(pUserEntity, garageComp);
        

    }
		
		
    
    //------------------------------------------------------------------------------------------------
    override bool CanBeShownScript(IEntity user)
    {			
        //Print("CanBeShownScript called.", LogLevel.NORMAL);
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    override bool CanBePerformedScript(IEntity user)
    {
        //Print("CanBePerformedScript called.", LogLevel.NORMAL);
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    override bool GetActionNameScript(out string outName)
    {
        outName = "View Owned Cars";
        //Print("GetActionNameScript called. Action name set to 'View Owned Cars'.", LogLevel.NORMAL);
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    override bool HasLocalEffectOnlyScript()
    {
        //Print("HasLocalEffectOnlyScript called.", LogLevel.NORMAL);
        return true;
    }
    
    //----------------------------------------------------------------------------------
    override bool CanBroadcastScript() 
    { 
        //Print("CanBroadcastScript called.", LogLevel.NORMAL);
        return false; 
    }
}

