[BaseContainerProps()]
class MIKE_GaragedMerchandiseVehicle: ADM_MerchandisePrefab
{
	
	ref PointInfo m_SpawnPosition;
	
	[Attribute(desc: "Ignore player when checking if spawn area is clear")]
	bool m_bIgnorePlayer;
	
	override int GetMaxPurchaseQuantity() 
	{
		return 1;
	}
	
	EntitySpawnParams GetVehicleSpawnTransform(ADM_ShopBaseComponent shop)
	{
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		IEntity shopEntity = shop.GetOwner();
		MIKE_GarageComponent getComp;
		getComp = MIKE_GarageComponent.Cast(shopEntity.FindComponent(MIKE_GarageComponent));
		m_SpawnPosition = getComp.m_SpawnPosition;
		if (m_SpawnPosition && !shop.IsInherited(ADM_PhysicalShopComponent)) {
			m_SpawnPosition.Init(shop.GetOwner());
			m_SpawnPosition.GetModelTransform(params.Transform);
			params.Transform[3] = shop.GetOwner().CoordToParent(params.Transform[3]);
		} else {
			shop.GetOwner().GetTransform(params.Transform);
		}		
		
		return params;
	}
	
	// This causes a noticable studder in workbench, if many vehicle locations getting checked this could be an issue for large servers
	int lastCheckTime = -1;
	bool canRespawnCache = false;
	override bool CanRespawn(ADM_ShopBaseComponent shop, int quantity = 1, array<IEntity> ignoreEntities = null)
	{		
		int curTick = System.GetTickCount();
		if (curTick - lastCheckTime >= 1000)
		{
			IEntity shopEntity = shop.GetOwner();
			array<IEntity> excludeEntities = {shopEntity};
			
			if (ignoreEntities)
			{
				excludeEntities.InsertAll(ignoreEntities);
			}
				
			canRespawnCache = ADM_Utils.IsSpawnPositionClean(m_sPrefab, GetVehicleSpawnTransform(shop), excludeEntities);
			lastCheckTime = curTick;
		}
		
		return canRespawnCache;
	}
	
	override bool CanDeliver(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1, array<IEntity> ignoreEntities = null)
	{
		if (quantity > m_iMaxPurchaseQuantity) quantity = m_iMaxPurchaseQuantity;
		
		array<IEntity> excludeEntities = {};
		if (m_bIgnorePlayer)
		{
			ADM_Utils.GetChildrenRecursive(player, excludeEntities);
			excludeEntities.Insert(player);
		}
			
		if (ignoreEntities)
			excludeEntities.InsertAll(ignoreEntities);
		
		return CanRespawn(shop, quantity, excludeEntities); 
	}
	
	override bool Deliver(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1)
	{
		if (!Replication.IsServer()) return false;
		if (quantity > m_iMaxPurchaseQuantity) quantity = m_iMaxPurchaseQuantity;
		
		bool canDeliver = this.CanDeliver(player, shop, merchandise, quantity, null);
		if (!canDeliver) 
			return false;
		
		EntitySpawnParams params = GetVehicleSpawnTransform(shop);
		IEntity entity = GetGame().SpawnEntityPrefab(Resource.Load(m_sPrefab), shop.GetOwner().GetWorld(), params);
		if (entity.GetPhysics())
			entity.GetPhysics().SetActive(true);
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player);
		WCS_LockableVehicleComponent lockcomp = WCS_LockableVehicleComponent.Cast(entity.FindComponent(WCS_LockableVehicleComponent));
		lockcomp.SetOwnerPlayer(playerID);
		return true;
	}
}