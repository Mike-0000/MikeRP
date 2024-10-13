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
        TAG_MikeGarage instance();
		
        instance.guid = guid2;
        instance.VehiclePrefab = text;
		instance.payment = payment1;
		
        return instance;
    }
};


class MIKE_GarageComponentClass: ADM_ShopBaseComponentClass {}
class MIKE_GarageComponent: ADM_ShopBaseComponent
{
//	[Attribute("", UIWidgets.ResourceNamePicker, "Config", "conf", category: "Shop")]
//	ref array<ResourceName> m_Categories;
//	
//	[Attribute(defvalue: "", desc: "Additional merchandise not defined in config", uiwidget: UIWidgets.Object, category: "Shop")]
//	protected ref array<ref ADM_ShopMerchandise> m_AdditionalMerchandise;
//	
//	ref array<ref ADM_ShopCategory> m_LoadedCategories = {};
//	
//	array<ref ADM_ShopCategory> GetCategories()
//	{
//		return m_LoadedCategories;
//	}
	void FindRecord(EDF_EDbOperationStatusCode statusCode, array<ref TAG_MikeGarage> result)
    {
		if (!result)
			return;
		
		foreach (TAG_MikeGarage car : result) 
		{
			
			ResourceName prefabName = car.VehiclePrefab;
			MIKE_GaragedMerchandiseVehicle newVic = new MIKE_GaragedMerchandiseVehicle;
			newVic.m_sPrefab = car.VehiclePrefab;
			newVic.m_SpawnPosition = new PointInfo();
			vector spawnMatrix[4];
				spawnMatrix[0] = "1 0 0"; // Right direction
				spawnMatrix[1] = "0 1 0"; // Up direction
				spawnMatrix[2] = "0 0 1"; // Forward direction
				spawnMatrix[3] = "-10 1 0"; // Spawn position coordinates (x, y, z)
			newVic.m_SpawnPosition.Set(null, "", spawnMatrix);
			newVic.m_bIgnorePlayer = true;
			ADM_ShopMerchandise insertVic;
			insertVic.m_Merchandise = newVic;
			ADM_PaymentMethodCurrency payment = new ADM_PaymentMethodCurrency;
			payment.m_Quantity = car.payment;
			
			ref array<ref ADM_PaymentMethodBase> finalPayment;
			finalPayment.Insert(payment);
			
			insertVic.m_BuyPayment = finalPayment;
			m_Merchandise.Insert(insertVic);
		}
		
		
		
		
		
		
		
		
//        if (result)
//        {
//            //Print(string.Format("License Found! Got License Name and guid1 as: "+ result.licensename+ result.guid1), LogLevel.NORMAL);
//            licenseExists = true;
//        }
//        else
//        {
//            //Print("License Not Found!", LogLevel.NORMAL);
//            licenseExists = false;
//        }
    }
	
	ref array<ref ADM_ShopCategory> m_LoadedCategories = {};
	array<ref ADM_ShopCategory> GetCategories()
	{
		return m_LoadedCategories;
	}
	void MikeBuildGarageShop(IEntity pUserEntity)
	{
		BackendApi api = GetGame().GetBackendApi();
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
        string playerguid1 = api.GetPlayerIdentityId(playerID);

        // Get the connection info
        EDF_JsonFileDbConnectionInfo connectInfo();
        connectInfo.m_sDatabaseName = "PlayerGarages";

        // Get a db context instance
        EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);

        // Interact with the DB context through a repository
        EDF_DbRepository<TAG_MikeGarage> repository = EDF_DbEntityHelper<TAG_MikeGarage>.GetRepository(dbContext);

        // Now find the record
        EDF_DbFindCondition condition = EDF_DbFind.Field("guid").Contains(playerguid1);
        EDF_DbFindCallbackMultiple<TAG_MikeGarage> findRecordHandler(this, "FindRecord");
        repository.FindAllAsync(condition, callback: findRecordHandler);

	
		//super.OnPostInit(owner);
//		ADM_ShopMerchandise testAsset;
//		testAsset.
		
		
		

		
		
//		foreach (ADM_ShopMerchandise merch : m_AdditionalMerchandise) 
//		{
//			m_Merchandise.Insert(merch);
//		}
		

	}
	override void OnPostInit(IEntity owner)
	{
		
	}
}








class MIKE_GarageAction : ScriptedUserAction
{
	protected MIKE_GarageComponent m_Shop;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent) 
	{
//		m_Shop = MIKE_GarageComponent.Cast(pOwnerEntity.FindComponent(MIKE_GarageComponent));
//		if (!m_Shop) 
//		{
//			Print("Error! Could not find shop component for this action.", LogLevel.ERROR);
//			return;
//		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{	
		//if (!m_Shop) return;
		
		
		
//		ADM_ShopMerchandise newVic = ADM_ShopMerchandise();
//		newVic.
		//MIKE_GarageComponent newVic = MIKE_GarageComponent();
		
		SCR_PlayerController scrPlayerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!scrPlayerController || pUserEntity != scrPlayerController.GetMainEntity()) 
			return;

		MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.ADM_ShopMenu); 
		MIKE_GarageComponent garageComp;
		garageComp.MikeBuildGarageShop(pUserEntity);
		MIKE_GarageShopUI menu = MIKE_GarageShopUI.Cast(menuBase);
		menu.SetShop(garageComp);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{			
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "View Owned Cars";
		
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
}