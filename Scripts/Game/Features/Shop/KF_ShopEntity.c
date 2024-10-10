class KF_ShopEntityClass : GenericEntityClass
{}

class KF_ShopEntity : GenericEntity
{
	[Attribute("", UIWidgets.CheckBox, desc: "Is this entity a shop item?")]
	bool m_bShopEntity;
	
	[Attribute("", UIWidgets.EditBox, desc: "Item price")]
	int m_iPrice;
	
	[Attribute("", UIWidgets.CheckBox, desc: "Players can increase amouns")]
	bool m_bAllowAmountChange;
	
	int m_iCurrentAmount = 1;
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		if (!m_bShopEntity) return;
		
		InventoryItemComponent itemComp = InventoryItemComponent.Cast(owner.FindComponent(InventoryItemComponent));
		if (!itemComp)
		{
			Print("DEBUG: itemComp missing on type: " + owner.Type());
			return;
		}
		
		// Disable pickup
		itemComp.RequestUserLock(owner, true);
		
		// Hide in vicinity
		SCR_ItemAttributeCollection itemAttributes = SCR_ItemAttributeCollection.Cast(itemComp.GetAttributes());
		if (!itemAttributes) return;
		
		itemAttributes.SetVisibility(false);
	}
}


class KF_BuyAction : ScriptedUserAction
{
	SCR_BaseGameMode m_GameMode;
	KF_ShopEntity m_ShopEntity;
	int m_iAmount, m_iPrice;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		m_ShopEntity = KF_ShopEntity.Cast(pOwnerEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		m_iAmount = m_ShopEntity.m_iCurrentAmount;
		m_iPrice = m_iAmount * m_ShopEntity.m_iPrice;
		
		outName = "Buy " + m_iAmount + "x for $" + m_iPrice;
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		KF_ShopEntity shopEnt = KF_ShopEntity.Cast(GetOwner());
		if (!shopEnt) return false;
		
		return shopEnt.m_bShopEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user) 
	{
		SetCannotPerformReason("Insufficent balance");
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user);
		
		return (m_GameMode.GetBalance(playerId, "$") >= m_iPrice);
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
		m_GameMode.AdjustBalance(playerId, "$", -m_iPrice);
		
		InventoryStorageManagerComponent invComp = InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(InventoryStorageManagerComponent));
		if (!invComp) return;
		
		ResourceName prefab = pOwnerEntity.GetPrefabData().GetPrefabName();
		invComp.TrySpawnPrefabToStorage(prefab);
	}
}


class KF_ChangeAmountAction : SCR_AdjustSignalAction
{
	KF_ShopEntity m_ShopEntity;
	int m_iAmount = 1;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_ShopEntity = KF_ShopEntity.Cast(pOwnerEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Adjust amount: " + m_iAmount.ToString();
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		KF_ShopEntity shopEnt = KF_ShopEntity.Cast(GetOwner());
		if (!shopEnt) return false;
		
		return (shopEnt.m_bShopEntity && shopEnt.m_bAllowAmountChange);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandleAction(float value)
	{
		super.HandleAction(value);
		
		if (value > 0 && m_iAmount < 100) m_iAmount++;
		else if (value < 0 && m_iAmount > 1) m_iAmount--;
		
		m_ShopEntity.m_iCurrentAmount = m_iAmount;
	}
}