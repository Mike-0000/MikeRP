//! User action that ought to be attached to an entity with door component.
//! When performed either opens or closes the door based on the previous state of the door.
class SCR_PoliceDoorUserAction : DoorUserAction
{
    // Define the required faction key
    const string REQUIRED_FACTION_KEY = "US";

    //------------------------------------------------------------------------------------------------
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
    {		
        // First, check if the player is allowed to perform the action
        if (!IsPlayerAllowed(pUserEntity))
        {
            // Optionally, provide feedback to the player
            // Example: ShowNotification(pUserEntity, "You do not have the required faction to perform this action.");
			

			//hintComponent.ShowCustomHint("Access Denied", "You do not have keys for this door.", 5);
            //Print("Access Denied: Player does not belong to the 'US' faction.");
            return; // Exit the method without performing the action
        }
			
        // Existing door action logic
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
            
            //Print(controlValue);
            doorComponent.SetActionInstigator(pUserEntity);
            doorComponent.SetControlValue(controlValue);
        }
        
        super.PerformAction(pOwnerEntity, pUserEntity);
    }

    //------------------------------------------------------------------------------------------------
    override bool CanBePerformedScript(IEntity user)
    {
        DoorComponent doorComponent = GetDoorComponent();
        if (!doorComponent)
            return false;

        // Check if the player is allowed to perform the action
        return IsPlayerAllowed(user);
    }
    
    //------------------------------------------------------------------------------------------------
    override bool GetActionNameScript(out string outName)
    {
        auto doorComponent = GetDoorComponent();
        // Logic here is flipped since method returns the opposite of what we expect
        if (doorComponent && Math.AbsFloat(doorComponent.GetControlValue()) >= 0.5)
            outName = "#AR-UserAction_Close";
        else
            outName = "#AR-UserAction_Open";
        
        return true;
    }

    //------------------------------------------------------------------------------------------------
    /**
     * Helper function to determine if the player is allowed to perform the door action based on faction
     */
    private bool IsPlayerAllowed(IEntity pUserEntity)
    {
        // Get the FactionAffiliationComponent of the player
        FactionAffiliationComponent userFactionAffiliation = FactionAffiliationComponent.Cast(pUserEntity.FindComponent(FactionAffiliationComponent));
        if (!userFactionAffiliation)
            return false; // Player has no faction affiliation component, deny access
        
        // Get the player's faction
        Faction userFaction = userFactionAffiliation.GetAffiliatedFaction();
        if (!userFaction)
            return false; // Player has no faction, deny access

        // Get the faction key
        string userFactionKey = userFaction.GetFactionKey(); // Now we can use GetFactionKey()

        // Compare the faction key
        if (userFactionKey == REQUIRED_FACTION_KEY)
            return true; // Player belongs to the required faction
        
        // Faction key does not match
        return false;
    }
};
