[EDF_DbName.Automatic()]
class TAG_LicenseInfo : EDF_DbEntity
{
    string guid1;
    string licensename;

    //------------------------------------------------------------------------------------------------
    //! Db entities cannot have a constructor with parameters; this is a limitation of the engine.
    //! Consult the docs for more info on this.
    static TAG_LicenseInfo Create(string guid2, string text)
    {
        TAG_LicenseInfo instance();
        instance.guid1 = guid2;
        instance.licensename = text;
        return instance;
    }
};

[BaseContainerProps()]
class MIKE_License : ADM_MerchandisePrefab
{
    [Attribute(defvalue: "civhandgun", desc: "This is the variable name used in the database to store the license.", uiwidget: UIWidgets.EditBox, category: "Physical Shop")]
    protected string LicenseName;
	
    bool licenseExists = false;

    override bool CanRespawn(ADM_ShopBaseComponent shop, int quantity = 1, array<IEntity> ignoreEntities = null)
    {
        // This will always be true; for items with large geometry, consider checking for space to avoid collisions.
        return true;
    }

    override bool CanSell(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1)
    {
		//Print("1", LogLevel.NORMAL);
        return false;
    }

    override bool CanCollectMerchandise(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1)
    {
		//Print("2", LogLevel.NORMAL);
        // Same as CanSell for now. In the future, check for limited quantity or pulling from specific storage.
        return CanSell(player, shop, merchandise, quantity);
    }

    override bool CollectMerchandise(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1)
    {
		//Print("3", LogLevel.NORMAL);
        return false;
    }

    override bool CanDeliver(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1, array<IEntity> ignoreEntities = null)
    {
        if (!Replication.IsServer()) return false;

        // Get the BackendApi and the player's guid1
        BackendApi api = GetGame().GetBackendApi();
        int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player);
        string playerguid1 = api.GetPlayerIdentityId(playerID);

        // Get the connection info
        EDF_JsonFileDbConnectionInfo connectInfo();
        connectInfo.m_sDatabaseName = "LicenseInfo";

        // Get a db context instance
        EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);

        // Interact with the DB context through a repository
        EDF_DbRepository<TAG_LicenseInfo> repository = EDF_DbEntityHelper<TAG_LicenseInfo>.GetRepository(dbContext);

        // Now find the record
        EDF_DbFindCondition condition = EDF_DbFind.And({
            EDF_DbFind.Field("guid1").Contains(playerguid1),
            EDF_DbFind.Field("licensename").Contains(LicenseName)
        });
        EDF_DbFindCallbackSingle<TAG_LicenseInfo> findRecordHandler(this, "FindRecord");
        repository.FindFirstAsync(condition, callback: findRecordHandler);
		SCR_HintManagerComponent hintComponent = SCR_HintManagerComponent.GetInstance();
        // Check if the player already has the license
        if (licenseExists == true)
        {
			hintComponent.ShowCustomHint("License Not Purchased!", "You already have a "+LicenseName+". Maximum 1 License per Citizen.", 10);
			
            //Print("License Found! Returning false to CanDeliver.", LogLevel.NORMAL);
            return false;
        }
        else
        {
            //Print("License Not Found! Returning true to CanDeliver.", LogLevel.NORMAL);
            return true;
        }
    }

    override bool Deliver(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1)
    {
		//Print("101", LogLevel.NORMAL);
        if (!Replication.IsServer()) return false;
		//Print("102", LogLevel.NORMAL);
        // Get the BackendApi and the player's guid
        BackendApi api = GetGame().GetBackendApi();
        int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player);
        string playerguid1 = api.GetPlayerIdentityId(playerID);

        // Get the connection info
        EDF_JsonFileDbConnectionInfo connectInfo();
        connectInfo.m_sDatabaseName = "LicenseInfo";

        // Get a db context instance
        EDF_DbContext dbContext = EDF_DbContext.Create(connectInfo);

        // Interact with the DB context through a repository
        EDF_DbRepository<TAG_LicenseInfo> repository = EDF_DbEntityHelper<TAG_LicenseInfo>.GetRepository(dbContext);
		TAG_LicenseInfo newEntry = TAG_LicenseInfo.Create(playerguid1, LicenseName);
		newEntry.SetId(playerguid1);
        // Add or update the license entry
        repository.AddOrUpdateAsync(newEntry);

        // Confirm the license was added
        EDF_DbFindCondition condition = EDF_DbFind.And({
            EDF_DbFind.Field("guid1").Contains(playerguid1),
            EDF_DbFind.Field("licensename").Contains(LicenseName)
        });
        EDF_DbFindCallbackSingle<TAG_LicenseInfo> findRecordHandler(this, "FindRecord");
        repository.FindFirstAsync(condition, callback: findRecordHandler);
		SCR_HintManagerComponent hintComponent = SCR_HintManagerComponent.GetInstance();
        if (licenseExists == true)
        {
			
			hintComponent.ShowCustomHint("License Purchased!", "You have purchased a "+LicenseName+". You are now able to use the shops locked to this license.", 10);
            //Print("License Found! Returning true to Deliver.", LogLevel.NORMAL);
			SCR_ChimeraCharacter playerChar = SCR_ChimeraCharacter.Cast(player);
			
			
			
			if (LicenseName == "drivers"){
		
			playerChar.driversLicenseExists = true;
							
			}
			else if (LicenseName == "civhandgun"){
				playerChar.basicGunLicenseExists = true;
			
			}
			else if (LicenseName == "tier2civhandgun"){
				playerChar.tier2GunLicenseExists = true;
			
			}
	        return true;
        }
        else
        {
			hintComponent.ShowCustomHint("License Purchase Failure!", "There was an error in purchasing your license. Please try again later.", 10);
            //Print("License Not Found! Returning false to Deliver.", LogLevel.NORMAL);
            return false;
        }
    }

    // Changed access modifier from 'protected' to 'public'
    void FindRecord(EDF_EDbOperationStatusCode statusCode, TAG_LicenseInfo result)
    {

        if (result)
        {
            //Print(string.Format("License Found! Got License Name and guid1 as: "+ result.licensename+ result.guid1), LogLevel.NORMAL);
            licenseExists = true;
        }
        else
        {
            //Print("License Not Found!", LogLevel.NORMAL);
            licenseExists = false;
        }
    }
}
