class SCR_OpenVehicleDoorUserAction : VehicleDoorUserAction
{
	[Attribute("1", desc: "Should the door open on their own, or should they be opened by the character?")]
	protected bool m_bAnimateCharacter;
	WCS_LockableVehicleComponent m_LockableVehicleComponent;
	
	override event void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		GetGame().GetCallqueue().CallLater(LoadVehicleComponentDelayed1, 1000, false, pOwnerEntity);
	}
	
	private void LoadVehicleComponentDelayed1(IEntity pOwnerEntity)
	{
		const IEntity vehicle = SCR_EntityHelper.GetMainParent(pOwnerEntity, true);
		if (!vehicle)
			return;

		m_LockableVehicleComponent = WCS_LockableVehicleComponent.Cast(vehicle.FindComponent(WCS_LockableVehicleComponent));
	}
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = GetUIInfo().GetName();
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!pOwnerEntity || !pUserEntity)
			return;
		
		ChimeraCharacter character = ChimeraCharacter.Cast(pUserEntity);
		if (!character)
			return;
		
		CompartmentAccessComponent compartmentAccess = character.GetCompartmentAccessComponent();
		if (!compartmentAccess)
			return;
		
		ECharacterDoorAnimType animType = ECharacterDoorAnimType.INVALID;
		if (compartmentAccess.IsInCompartment())
			animType = ECharacterDoorAnimType.FROM_INSIDE;
		else
			animType = ECharacterDoorAnimType.FROM_OUTSIDE;
		
		if (!compartmentAccess.OpenDoor(pOwnerEntity, animType, GetDoorIndex()))
			return;
		
		super.PerformAction(pOwnerEntity, pUserEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(GetOwner().FindComponent(SCR_VehicleDamageManagerComponent));
		if (damageManager && damageManager.GetState() == EDamageState.DESTROYED)
			return false;
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
			return false;

		CompartmentAccessComponent compartmentAccess = character.GetCompartmentAccessComponent();
		if (!compartmentAccess)
			return false;
		
		if (compartmentAccess.AreDoorsFromDifferentVehicle(GetOwner(), GetDoorIndex()))
			return false;
		
		BaseCompartmentManagerComponent managerComponent = BaseCompartmentManagerComponent.Cast(GetOwner().FindComponent(BaseCompartmentManagerComponent));
		if (!managerComponent)
			return false;
		
		Vehicle vehicle = Vehicle.Cast(SCR_EntityHelper.GetMainParent(GetOwner(), true));
		if (vehicle)
		{
			Faction characterFaction = character.GetFaction();
			Faction vehicleFaction = vehicle.GetFaction();
			if (characterFaction && vehicleFaction && characterFaction.IsFactionEnemy(vehicleFaction))
			{
				SetCannotPerformReason("#AR-UserAction_SeatHostile");
				return false;
			}
		}
		
		if (managerComponent.GetDoorUser(GetDoorIndex()) && managerComponent.GetDoorUser(GetDoorIndex()) != user || managerComponent.AreDoorOpen(GetDoorIndex()))
		{
			SetCannotPerformReason("#AR-UserAction_SeatOccupied");
			return false;
		}
		
		if (!compartmentAccess.CanAccessDoor(vehicle, managerComponent, GetDoorIndex()))
		{
			SetCannotPerformReason("#AR-UserAction_SeatObstructed");
			return false;
		}
		
		
		//MIKE ADDED FOR WCS_LockableVehicleComponent
		
		if (!super.CanBePerformedScript(user))
			return false;
		
		if (m_LockableVehicleComponent && m_LockableVehicleComponent.IsLockedForPlayer(user))
		{
			SetCannotPerformReason("Vehicle is locked");
			return false;
		}
		
		
		
		
		return true;
	}	
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(GetOwner().FindComponent(SCR_VehicleDamageManagerComponent));
		if (damageManager && damageManager.GetState() == EDamageState.DESTROYED)
			return false;
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(user);
		if (!character)
			return false;
		
		BaseCompartmentManagerComponent managerComponent = BaseCompartmentManagerComponent.Cast(GetOwner().FindComponent(BaseCompartmentManagerComponent));
		if (!managerComponent)
			return false;
		
		CompartmentAccessComponent compartmentAccess = character.GetCompartmentAccessComponent();
		if (!compartmentAccess)
			return false;
		
		if (compartmentAccess.AreDoorsFromDifferentVehicle(GetOwner(), GetDoorIndex()))
			return false;

		if (compartmentAccess.IsGettingIn() || compartmentAccess.IsGettingOut())
			return false;
		
		if (managerComponent.AreDoorOpen(GetDoorIndex()))
		{
			return false;
		}
		
		return true;
	}	
};
