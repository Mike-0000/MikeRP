// ADM_CurrencyComponent.c

class ADM_CurrencyPredicate: InventorySearchPredicate
{
    void ADM_CurrencyPredicate()
    {
        Print("ADM_CurrencyPredicate constructor called.", LogLevel.NORMAL);
        QueryComponentTypes.Insert(ADM_CurrencyComponent);
    }

    override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
    {		
        Print("ADM_CurrencyPredicate.IsMatch() called.", LogLevel.NORMAL);
        bool isMatch = ADM_CurrencyComponent.Cast(queriedComponents[0]) != null;
        Print("IsMatch: " + isMatch, LogLevel.NORMAL);
        return isMatch;
    }
}

class ADM_CurrencyComponentClass: ScriptComponentClass {}

class ADM_CurrencyComponent: ScriptComponent
{
    // Existing static methods for SCR_ChimeraCharacter
    static int GetValue(SCR_ChimeraCharacter player)
    {
        if (!player)
        {
            Print("ADM_CurrencyComponent.GetValue(): player is null.", LogLevel.NORMAL);
            return 0;
        }
        Print("ADM_CurrencyComponent.GetValue(): Player cash: " + player.m_iCash, LogLevel.NORMAL);
        return player.m_iCash;
    }

    static void SetValue(SCR_ChimeraCharacter player, int value)
    {
        Print("ADM_CurrencyComponent.SetValue() called. New Value: " + value, LogLevel.NORMAL);
        if (!Replication.IsServer() || !player)
        {
            Print("SetValue: Not on server or player is null. Exiting.", LogLevel.NORMAL);
            return;
        }
        
        player.SetCash(value, false);
        Print("SetValue: Player cash set to " + value, LogLevel.NORMAL);
    }

    static bool ModifyValue(SCR_ChimeraCharacter player, int amount, bool increase)
    {
        Print("ADM_CurrencyComponent.ModifyValue() called. Amount: " + amount + ", Increase: " + increase, LogLevel.NORMAL);
        if (!Replication.IsServer() || !player)
        {
            Print("ModifyValue: Not on server or player is null. Exiting.", LogLevel.NORMAL);
            return false;
        }
        
        if (amount < 0)
        {
            Print("ModifyValue: Amount is negative. Exiting.", LogLevel.NORMAL);
            return false;
        }
        
        int currentCash = player.m_iCash;
        Print("ModifyValue: CurrentCash: " + currentCash, LogLevel.NORMAL);
        if (increase)
            currentCash += amount;
        else
            currentCash -= amount;
        
        // Ensure cash doesn't go negative
        if (currentCash < 0)
        {
            Print("ModifyValue: CurrentCash after modification is negative. Exiting.", LogLevel.NORMAL);
            return false;
        }
        
        player.SetCash(currentCash, false);
        Print("ModifyValue: Player cash updated to " + currentCash, LogLevel.NORMAL);
        
        return true;
    }

    static int FindTotalCurrencyInInventory(SCR_ChimeraCharacter player)
    {
        Print("ADM_CurrencyComponent.FindTotalCurrencyInInventory(SCR_ChimeraCharacter) called.", LogLevel.NORMAL);
        if (!player)
        {
            Print("FindTotalCurrencyInInventory: player is null.", LogLevel.NORMAL);
            return 0;
        }
        int total = player.m_iCash;
        Print("FindTotalCurrencyInInventory: TotalCurrency: " + total, LogLevel.NORMAL);
        return total;
    }

    static bool RemoveCurrencyFromInventory(SCR_ChimeraCharacter player, int amount)
    {
        Print("ADM_CurrencyComponent.RemoveCurrencyFromInventory(SCR_ChimeraCharacter) called. Amount: " + amount, LogLevel.NORMAL);
        if (!player)
        {
            Print("RemoveCurrencyFromInventory: player is null.", LogLevel.NORMAL);
            return false;
        }
        if (amount < 1)
        {
            Print("RemoveCurrencyFromInventory: Amount less than 1. No action taken.", LogLevel.NORMAL);
            return true;
        }
        int totalCurrency = FindTotalCurrencyInInventory(player);
        if (totalCurrency < amount)
        {
            Print("RemoveCurrencyFromInventory: Insufficient funds. TotalCurrency: " + totalCurrency + ", Requested: " + amount, LogLevel.NORMAL);
            return false;
        }
        
        bool modified = ModifyValue(player, amount, false);
        Print("RemoveCurrencyFromInventory: ModifyValue returned: " + modified, LogLevel.NORMAL);
        return modified;
    }

    static bool AddCurrencyToInventory(SCR_ChimeraCharacter player, int amount)
    {
        Print("ADM_CurrencyComponent.AddCurrencyToInventory(SCR_ChimeraCharacter) called. Amount: " + amount, LogLevel.NORMAL);
        if (!player)
        {
            Print("AddCurrencyToInventory: player is null.", LogLevel.NORMAL);
            return false;
        }
        if (amount < 1)
        {
            Print("AddCurrencyToInventory: Amount less than 1. No action taken.", LogLevel.NORMAL);
            return false;
        }
        
        bool modified = ModifyValue(player, amount, true);
        Print("AddCurrencyToInventory: ModifyValue returned: " + modified, LogLevel.NORMAL);
        return modified;
    }

    // **New Overloaded Methods to Handle SCR_InventoryStorageManagerComponent**

    /// <summary>
    /// Retrieves the SCR_ChimeraCharacter from the Inventory Storage Manager and returns total currency.
    /// </summary>
    static int FindTotalCurrencyInInventory(SCR_InventoryStorageManagerComponent inventoryManager)
    {
        Print("ADM_CurrencyComponent.FindTotalCurrencyInInventory(SCR_InventoryStorageManagerComponent) called.", LogLevel.NORMAL);
        if (!inventoryManager)
        {
            Print("FindTotalCurrencyInInventory: inventoryManager is null.", LogLevel.NORMAL);
            return 0;
        }

        // Retrieve the owner entity from InventoryStorageManagerComponent
        IEntity playerEntity = inventoryManager.GetOwner(); // Use GetOwner() instead of GetOwnerEntity()
        if (!playerEntity)
        {
            Print("FindTotalCurrencyInInventory: Unable to retrieve player entity from InventoryStorageManagerComponent", LogLevel.ERROR);
            return 0;
        }

        SCR_ChimeraCharacter playerCharacter = SCR_ChimeraCharacter.Cast(playerEntity);
        if (!playerCharacter)
        {
            Print("FindTotalCurrencyInInventory: Unable to cast playerEntity to SCR_ChimeraCharacter", LogLevel.ERROR);
            return 0;
        }

        int total = FindTotalCurrencyInInventory(playerCharacter);
        Print("FindTotalCurrencyInInventory: TotalCurrency from playerCharacter: " + total, LogLevel.NORMAL);
        return total;
    }

    /// <summary>
    /// Removes currency using the Inventory Storage Manager.
    /// </summary>
    static bool RemoveCurrencyFromInventory(SCR_InventoryStorageManagerComponent inventoryManager, int amount)
    {
        Print("ADM_CurrencyComponent.RemoveCurrencyFromInventory(SCR_InventoryStorageManagerComponent) called. Amount: " + amount, LogLevel.NORMAL);
        if (!inventoryManager)
        {
            Print("RemoveCurrencyFromInventory: inventoryManager is null.", LogLevel.NORMAL);
            return false;
        }

        IEntity playerEntity = inventoryManager.GetOwner(); // Use GetOwner() instead of GetOwnerEntity()
        if (!playerEntity)
        {
            Print("RemoveCurrencyFromInventory: Unable to retrieve player entity from InventoryStorageManagerComponent", LogLevel.ERROR);
            return false;
        }

        SCR_ChimeraCharacter playerCharacter = SCR_ChimeraCharacter.Cast(playerEntity);
        if (!playerCharacter)
        {
            Print("RemoveCurrencyFromInventory: Unable to cast playerEntity to SCR_ChimeraCharacter", LogLevel.ERROR);
            return false;
        }

        bool removed = RemoveCurrencyFromInventory(playerCharacter, amount);
        Print("RemoveCurrencyFromInventory: RemoveCurrencyFromInventory(SCR_ChimeraCharacter) returned: " + removed, LogLevel.NORMAL);
        return removed;
    }

    /// <summary>
    /// Adds currency using the Inventory Storage Manager.
    /// </summary>
    static bool AddCurrencyToInventory(SCR_InventoryStorageManagerComponent inventoryManager, int amount)
    {
        Print("ADM_CurrencyComponent.AddCurrencyToInventory(SCR_InventoryStorageManagerComponent) called. Amount: " + amount, LogLevel.NORMAL);
        if (!inventoryManager)
        {
            Print("AddCurrencyToInventory: inventoryManager is null.", LogLevel.NORMAL);
            return false;
        }

        IEntity playerEntity = inventoryManager.GetOwner(); // Use GetOwner() instead of GetOwnerEntity()
        if (!playerEntity)
        {
            Print("AddCurrencyToInventory: Unable to retrieve player entity from InventoryStorageManagerComponent", LogLevel.ERROR);
            return false;
        }

        SCR_ChimeraCharacter playerCharacter = SCR_ChimeraCharacter.Cast(playerEntity);
        if (!playerCharacter)
        {
            Print("AddCurrencyToInventory: Unable to cast playerEntity to SCR_ChimeraCharacter", LogLevel.ERROR);
            return false;
        }

        bool added = AddCurrencyToInventory(playerCharacter, amount);
        Print("AddCurrencyToInventory: AddCurrencyToInventory(SCR_ChimeraCharacter) returned: " + added, LogLevel.NORMAL);
        return added;
    }
}
