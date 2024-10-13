//! User action that ought to be attached to an entity with door component.
//! When performed either opens or closes the door based on the previous state of the door.
class MIKE_BankDoorUserAction : DoorUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{		
	    DoorComponent doorComponent = GetDoorComponent();
	    if (doorComponent)
	    {
	        vector doorOpeningVecWS = pOwnerEntity.VectorToParent(vector.Forward);
	        if (doorComponent.GetAngleRange() < 0.0)
	            doorOpeningVecWS = -1.0 * doorOpeningVecWS;
	        
	        vector userMat[4];
	        pUserEntity.GetWorldTransform(userMat);
	        float dotP = vector.Dot(doorOpeningVecWS, userMat[3] - doorComponent.GetDoorPivotPointWS());
	        
	        // Flip the control value if it is above a threshold
	        float controlValue = 1.0;
	        float currentState = doorComponent.GetDoorState();
	        if ((dotP < 0.0 && currentState <= 0.0)  || (dotP > 0.0 && currentState < 0.0))
	            controlValue = -1.0;
	        if (Math.AbsFloat(doorComponent.GetControlValue()) > 0.5)
	            controlValue = 0.0;
	        
	        doorComponent.SetActionInstigator(pUserEntity);
	        doorComponent.SetControlValue(controlValue);
	
	        // Play sound here
	        PlayDoorSound(pOwnerEntity, controlValue);
	    }
	    
	    super.PerformAction(pOwnerEntity, pUserEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	void PlayDoorSound(IEntity pOwnerEntity, float controlValue)
	{
		Print("PlayDoorSound Enacted", LogLevel.NORMAL);
	    // Get the sound component on the door entity (ensure the entity has a sound component)
	    SoundComponent soundComp = SoundComponent.Cast(pOwnerEntity.FindComponent(SoundComponent));
	
	    if (soundComp)
	    {
			Print("soundComp Exists", LogLevel.NORMAL);
	        if (controlValue != 0.0)
	        {
				Print("ControlValue != 0.0, playing sound!", LogLevel.NORMAL);
				SCR_HintManagerComponent hintComponent = SCR_HintManagerComponent.GetInstance();
				hintComponent.ShowCustomHint("The Bank at St. Phillipe is being robbed!", "All Units Respond! Civilians stay clear.", 20);
	            // Play door opening or closing soundComp
				soundComp.PlayStr("MIKE_SOUND_BANK_BELL");
	        }
	    }
	}
	
	//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
override bool CanBePerformedScript(IEntity user)
{
    DoorComponent doorComponent = GetDoorComponent();
    if (!doorComponent)
        return false;

    // Get the player manager
    PlayerManager playerManager = GetGame().GetPlayerManager();
    if (!playerManager)
        return false; // No player manager found

    // Get all connected player IDs
    array<int> playerIds = new array<int>();
    playerManager.GetPlayers(playerIds);

    // Iterate over all connected players
    foreach (int playerId : playerIds)
    {
        // Get the controlled entity for each player
        IEntity playerEntity = playerManager.GetPlayerControlledEntity(playerId);
        if (!playerEntity)
            continue;

        // Get the faction affiliation component
        FactionAffiliationComponent factionComponent = FactionAffiliationComponent.Cast(playerEntity.FindComponent(FactionAffiliationComponent));
        if (!factionComponent)
            continue;

        // Get the affiliated faction
        Faction faction = factionComponent.GetAffiliatedFaction();
        if (!faction)
            continue;

        // Get the faction key
        string factionKey = faction.GetFactionKey();
        if (factionKey == "US")
        {
            // At least one player with faction key "US" is connected
            return true;
        }
    }
		
	//Print("No player with faction key US found", LogLevel.NORMAL);
	SetCannotPerformReason("1 Police on Duty Required to Rob Bank");
    // No player with faction key "US" found
    return false;
}

	
	//------------------------------------------------------------------------------------------------
//	override bool CanBePerformedScript(IEntity user)
//	{
//		DoorComponent doorComponent = GetDoorComponent();
//		if (doorComponent)
//			return true;
//		
//		return false;
//	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "Lockpick Bank Door (90 seconds)";
		return true;
	}
	
};
