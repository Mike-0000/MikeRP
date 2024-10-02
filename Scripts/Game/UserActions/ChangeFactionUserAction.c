// ChangeFactionUserAction.c

class ChangeFactionUserAction : ScriptedUserAction
{
    // Define the target faction key
    private const string TARGET_FACTION_KEY = "US"; // Change to your desired faction key

    //------------------------------------------------------------------------------------------------
    override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent) 
    {
        // Initialization if needed
    }
    
    //------------------------------------------------------------------------------------------------
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
    {   
        // Ensure the user entity is valid
        if (!pUserEntity)
        {
            Print("Error: User entity is null.", LogLevel.ERROR);
            return;
        }

        // Get the BackendApi and the player's GUID
        BackendApi api = GetGame().GetBackendApi();
        int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
        string playerGUID = api.GetPlayerIdentityId(playerID);

        // Print the player's GUID to the log
        Print("Player GUID: " + playerGUID, LogLevel.NORMAL);

        // Check if the player is allowed to change faction
        if (!IsPlayerAllowed(pUserEntity, playerGUID))
        {
            Print("Access Denied: You cannot change your faction.", LogLevel.NORMAL);
            return;
        }

        // Get the FactionManager
        FactionManager factionManager = GetGame().GetFactionManager();
        if (!factionManager)
        {
            Print("Error: FactionManager not found.", LogLevel.ERROR);
            return;
        }

        // Get the target faction
        Faction targetFaction = factionManager.GetFactionByKey(TARGET_FACTION_KEY);
        if (!targetFaction)
        {
            Print("Error: Target faction not found: " + TARGET_FACTION_KEY, LogLevel.ERROR);
            return;
        }

        // Get the player's FactionAffiliationComponent
        FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(pUserEntity.FindComponent(FactionAffiliationComponent));
        if (!factionAffiliation)
        {
            Print("Error: Player has no FactionAffiliationComponent.", LogLevel.ERROR);
            return;
        }

        // Change the player's faction
        factionAffiliation.SetAffiliatedFaction(targetFaction);
        Print("Faction changed to: " + TARGET_FACTION_KEY, LogLevel.NORMAL);
    }
    
    //------------------------------------------------------------------------------------------------
    override bool CanBeShownScript(IEntity user)
    {
        // Determine if the action should be shown to the player
        FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(user.FindComponent(FactionAffiliationComponent));
        if (!factionAffiliation)
            return false;

        Faction userFaction = factionAffiliation.GetAffiliatedFaction();
        if (!userFaction)
            return false;

        return userFaction.GetFactionKey() != TARGET_FACTION_KEY;
    }
    
    //------------------------------------------------------------------------------------------------
    override bool CanBePerformedScript(IEntity user)
    {
        // Determine if the action can be performed by the player
        return CanBeShownScript(user);
    }
    
    //------------------------------------------------------------------------------------------------
    override bool GetActionNameScript(out string outName)
    {
        outName = "Change Faction to " + TARGET_FACTION_KEY;
        return true;
    }
    
    //------------------------------------------------------------------------------------------------
    override bool HasLocalEffectOnlyScript()
    {
        return false;
    }
    
    //------------------------------------------------------------------------------------------------
    override bool CanBroadcastScript() 
    { 
        return false; 
    }

    //------------------------------------------------------------------------------------------------
    /**
     * Helper function to determine if the player is allowed to perform the faction change action
     */
    private bool IsPlayerAllowed(IEntity pUserEntity, string playerGUID)
    {
        // Check if the player's GUID is in the whitelist
        if (!IsGUIDWhitelisted(playerGUID))
        {
            Print("Access Denied: Your GUID is not whitelisted.", LogLevel.NORMAL);
            return false;
        }

        // Existing condition: Only allow faction change if the player is not already in the target faction
        FactionAffiliationComponent userFactionAffiliation = FactionAffiliationComponent.Cast(pUserEntity.FindComponent(FactionAffiliationComponent));
        if (!userFactionAffiliation)
        {
            Print("Error: Player has no FactionAffiliationComponent.", LogLevel.NORMAL);
            return false; // Player has no faction affiliation component, deny access
        }
        
        Faction userFaction = userFactionAffiliation.GetAffiliatedFaction();
        if (!userFaction)
        {
            Print("Error: Player has no affiliated faction.", LogLevel.NORMAL);
            return false; // Player has no faction, deny access
        }

        string userFactionKey = userFaction.GetFactionKey();
        if (userFactionKey == TARGET_FACTION_KEY)
        {
            Print("Player already belongs to the target faction '" + TARGET_FACTION_KEY + "'.", LogLevel.NORMAL);
            return false; // Player already in target faction
        }

        // Additional conditions can be added here (e.g., permissions, cooldowns)

        return true; // Player is allowed to change faction
    }

    //------------------------------------------------------------------------------------------------
    /**
     * Reads the whitelist file and checks if the player's GUID is listed
     */
    private bool IsGUIDWhitelisted(string guid)
    {
        string filePath = "$profile:whitelist.txt";
        FileHandle file = FileIO.OpenFile(filePath, FileMode.READ);
        
        if (!file) 
        {
            Print("ERROR: Could not open file " + filePath, LogLevel.ERROR);
            return false;
        }

        // Go through the file and look for the GUID
        file.Seek(0);
        string line;
        while (file.ReadLine(line) != -1) 
        {
            if (line == guid) 
            {
                file.Close();
                return true;
            }
        }
        
        // GUID not found
        Print("GUID not found in whitelist.", LogLevel.NORMAL);
        file.Close();
        return false;
    }
}
