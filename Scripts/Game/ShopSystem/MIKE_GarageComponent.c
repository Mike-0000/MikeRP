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
	[Attribute()]
	ref PointInfo m_SpawnPosition;
	
	
	override array<ref ADM_ShopMerchandise> GetMerchandiseSell()
	{

		return null;
	}
	
    void FindRecord(EDF_EDbOperationStatusCode statusCode, array<ref TAG_MikeGarage> result, Managed garageCompTupled)
    {
        Print("FindRecord called.", LogLevel.NORMAL);
        
        if (!result)
        {
            Print("Error! Result array is null.", LogLevel.NORMAL);
            return;
        }
        Print("Result array is not null. Number of records: " + result.Count(), LogLevel.NORMAL);
        
		Tuple1<MIKE_GarageComponent> garageCompTyped = Tuple1<MIKE_GarageComponent>.Cast(garageCompTupled);
        MIKE_GarageComponent garageComp = garageCompTyped.param1;
		
		garageComp.m_Merchandise.Clear();
		
        foreach (TAG_MikeGarage car : result) 
        {
            if (!car)
            {
                Print("Error! TAG_MikeGarage instance is null.", LogLevel.NORMAL);
                continue;
            }
            Print("Processing TAG_MikeGarage with GUID: " + car.guid, LogLevel.NORMAL);
            
            ResourceName prefabName = car.VehiclePrefab;
            Print("VehiclePrefab: " + prefabName, LogLevel.NORMAL);
            
            MIKE_GaragedMerchandiseVehicle newVic = new MIKE_GaragedMerchandiseVehicle();
            if (!newVic)
            {
                Print("Error! MIKE_GaragedMerchandiseVehicle instantiation failed.", LogLevel.NORMAL);
                continue;
            }
            Print("MIKE_GaragedMerchandiseVehicle instance created.", LogLevel.NORMAL);
            
            newVic.m_sPrefab = car.VehiclePrefab;
            Print("Set newVic.m_sPrefab: " + car.VehiclePrefab, LogLevel.NORMAL);
            
            newVic.m_SpawnPosition = new PointInfo();
            if (!newVic.m_SpawnPosition)
            {
                Print("Error! PointInfo instantiation failed.", LogLevel.NORMAL);
                continue;
            }
            Print("PointInfo instance created for m_SpawnPosition.", LogLevel.NORMAL);
            
            vector spawnMatrix[4];
            spawnMatrix[0] = "1 0 0"; // Right direction
            spawnMatrix[1] = "0 1 0"; // Up direction
            spawnMatrix[2] = "0 0 1"; // Forward direction
            spawnMatrix[3] = "-10 1 0"; // Spawn position coordinates (x, y, z)
            newVic.m_SpawnPosition.Set(null, "", spawnMatrix);
            Print("Set spawnMatrix for m_SpawnPosition.", LogLevel.NORMAL);
            
            newVic.m_bIgnorePlayer = true;
            Print("Set m_bIgnorePlayer to true.", LogLevel.NORMAL);
            
            ADM_ShopMerchandise insertVic = new ADM_ShopMerchandise();
            if (!insertVic)
            {
                Print("Error! ADM_ShopMerchandise instantiation failed.", LogLevel.NORMAL);
                continue;
            }
            Print("ADM_ShopMerchandise instance created.", LogLevel.NORMAL);
            
            insertVic.m_Merchandise = newVic;
            Print("Set insertVic.m_Merchandise to newVic.", LogLevel.NORMAL);
            
            ADM_PaymentMethodCurrency payment = new ADM_PaymentMethodCurrency();
            if (!payment)
            {
                Print("Error! ADM_PaymentMethodCurrency instantiation failed.", LogLevel.NORMAL);
                continue;
            }
            Print("ADM_PaymentMethodCurrency instance created.", LogLevel.NORMAL);
            
            payment.m_Quantity = car.payment;
            Print(string.Format("Set payment.m_Quantity to: %1", car.payment), LogLevel.NORMAL);
            
            ref array<ref ADM_PaymentMethodBase> finalPayment = new array<ref ADM_PaymentMethodBase>();
            if (!finalPayment)
            {
                Print("Error! finalPayment array instantiation failed.", LogLevel.NORMAL);
                continue;
            }
            Print("finalPayment array created.", LogLevel.NORMAL);
            
            finalPayment.Insert(payment);
            Print("Inserted payment into finalPayment array.", LogLevel.NORMAL);
            
            insertVic.m_BuyPayment = finalPayment;
            Print("Set insertVic.m_BuyPayment to finalPayment.", LogLevel.NORMAL);
            
            garageComp.m_Merchandise.Insert(insertVic);
            Print("Inserted insertVic into m_Merchandise.", LogLevel.NORMAL);
        }
        MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.MIKE_GarageShopMenu);
        if (!menuBase) 
	        {
	            Print("Error! menuBase is null after OpenMenu.", LogLevel.NORMAL);
	            return;
	        }
        Print("menuBase obtained.", LogLevel.NORMAL);
		MIKE_GarageShopUI menu = MIKE_GarageShopUI.Cast(menuBase);
        if (!menu) 
	        {
	            Print("Error! menu is null after casting to MIKE_GarageShopUI.", LogLevel.NORMAL);
	            //return;
	        }
        Print("menu casted to MIKE_GarageShopUI.", LogLevel.NORMAL);
        
        menu.SetShop(garageComp);
        Print("SetShop called on menu with garageComp.", LogLevel.NORMAL);
        
        Print("PerformAction completed successfully.", LogLevel.NORMAL);
        Print("FindRecord completed processing records.", LogLevel.NORMAL);
    }
    
    ref array<ref ADM_ShopCategory> m_LoadedCategories = {};
    array<ref ADM_ShopCategory> GetCategories()
    {
        Print("GetCategories called.", LogLevel.NORMAL);
        return m_LoadedCategories;
    }
    
    void MikeBuildGarageShop(IEntity pUserEntity, MIKE_GarageComponent garageComp)
    {
		
        Print("MikeBuildGarageShop started.", LogLevel.NORMAL);
        
        BackendApi api = GetGame().GetBackendApi();
        if (!api)
        {
            Print("Error! BackendApi is null.", LogLevel.NORMAL);
            return;
        }
        Print("BackendApi obtained.", LogLevel.NORMAL);
        
        int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
        Print(string.Format("Obtained playerID: %1", playerID), LogLevel.NORMAL);
        
        string playerguid1 = api.GetPlayerIdentityId(playerID);
        Print(string.Format("Obtained playerguid1: %1", playerguid1), LogLevel.NORMAL);
        
        // Get the connection info
        EDF_JsonFileDbConnectionInfo connectInfo = new EDF_JsonFileDbConnectionInfo();
        if (!connectInfo)
        {
            Print("Error! EDF_JsonFileDbConnectionInfo instantiation failed.", LogLevel.NORMAL);
            return;
        }
        Print("EDF_JsonFileDbConnectionInfo instance created.", LogLevel.NORMAL);
        
        connectInfo.m_sDatabaseName = "PlayerGarages";
        Print("Set connectInfo.m_sDatabaseName to 'PlayerGarages'.", LogLevel.NORMAL);
        
        // Get a db context instance
        EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);
        if (!dbContext)
        {
            Print("Error! EDF_DbContext creation failed.", LogLevel.NORMAL);
            return;
        }
        Print("EDF_DbContext instance created.", LogLevel.NORMAL);
        
        // Interact with the DB context through a repository
        EDF_DbRepository<TAG_MikeGarage> repository = EDF_DbEntityHelper<TAG_MikeGarage>.GetRepository(dbContext);
        if (!repository)
        {
            Print("Error! EDF_DbRepository<TAG_MikeGarage> retrieval failed.", LogLevel.NORMAL);
            return;
        }
        Print("EDF_DbRepository<TAG_MikeGarage> obtained.", LogLevel.NORMAL);
        
        // Now find the record
        EDF_DbFindCondition condition = EDF_DbFind.Field("guid").Contains(playerguid1);
        if (!condition)
        {
            Print("Error! EDF_DbFindCondition creation failed.", LogLevel.NORMAL);
            return;
        }
        Print("EDF_DbFindCondition created with guid: " + playerguid1, LogLevel.NORMAL);
        Tuple1<MIKE_GarageComponent> context = new Tuple1<MIKE_GarageComponent>(MIKE_GarageComponent.Cast(garageComp));

        EDF_DbFindCallbackMultiple<TAG_MikeGarage> findRecordHandler = new EDF_DbFindCallbackMultiple<TAG_MikeGarage>(this, "FindRecord", context);
        if (!findRecordHandler)
        {
            Print("Error! EDF_DbFindCallbackMultiple instantiation failed.", LogLevel.NORMAL);
            return;
        }
        Print("EDF_DbFindCallbackMultiple instance created.", LogLevel.NORMAL);
        
        repository.FindAllAsync(condition, callback: findRecordHandler);
        Print("FindAllAsync called on repository.", LogLevel.NORMAL);
        
        Print("MikeBuildGarageShop completed.", LogLevel.NORMAL);
    }
    
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		//MIKE_GarageComponent.Cast(owner.FindComponent(MIKE_GarageComponent));
//		foreach (ADM_ShopMerchandise merch : m_AdditionalMerchandise) 
//		{
//			m_Merchandise.Insert(merch);
//		}
		
//		foreach (ResourceName category : m_Categories) 
//		{
//			m_LoadedCategories.Insert(ADM_ShopCategory.GetConfig(category));
//		}
	}
}

class MIKE_GarageAction : ScriptedUserAction
{
    //protected MIKE_GarageComponent m_Shop;
    
    //------------------------------------------------------------------------------------------------
//    override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent) 
//    {
//        Print("Init called for MIKE_GarageAction.", LogLevel.NORMAL);
//        
//        m_Shop = MIKE_GarageComponent.Cast(pOwnerEntity.FindComponent(MIKE_GarageComponent));
//        if (!m_Shop) 
//        {
//            Print("Error! Could not find shop component for this action.", LogLevel.NORMAL);
//            return;
//        }
//        Print("m_Shop component found and assigned.", LogLevel.NORMAL);
//    }
    
    //------------------------------------------------------------------------------------------------
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
    {	
        Print("PerformAction started.", LogLevel.NORMAL);
        
//        if (!m_Shop) 
//        {
//            Print("Error! m_Shop is null.", LogLevel.NORMAL);
//            return;
//        }
        Print("m_Shop is valid.", LogLevel.NORMAL);
        
        SCR_PlayerController scrPlayerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
        if (!scrPlayerController) 
        {
            Print("Error! scrPlayerController is null.", LogLevel.NORMAL);
            return;
        }
        Print("scrPlayerController obtained.", LogLevel.NORMAL);
        
        IEntity mainEntity = scrPlayerController.GetMainEntity();
        if (!mainEntity) 
        {
            Print("Error! mainEntity is null.", LogLevel.NORMAL);
            return;
        }
        Print("mainEntity obtained.", LogLevel.NORMAL);
        
        if (pUserEntity != mainEntity) 
        {
            Print("pUserEntity does not match mainEntity.", LogLevel.NORMAL);
            return;
        }
        Print("pUserEntity matches mainEntity.", LogLevel.NORMAL);
        
       
        
        // Instantiate garageComp before using it
        MIKE_GarageComponent garageComp;
		garageComp = MIKE_GarageComponent.Cast(pOwnerEntity.FindComponent(MIKE_GarageComponent));
        if (!garageComp)
        {
            Print("Error! garageComp instantiation failed.", LogLevel.NORMAL);
            return;
        }
        Print("garageComp instantiated successfully.", LogLevel.NORMAL);
        
        garageComp.MikeBuildGarageShop(pUserEntity, garageComp);
        Print("MikeBuildGarageShop called on garageComp.", LogLevel.NORMAL);
        

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
