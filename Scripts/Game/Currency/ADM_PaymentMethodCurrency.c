class ADM_PaymentMethodCurrency: ADM_PaymentMethodBase
{
    [Attribute()]
    int m_Quantity;
    
    int GetQuantity()
    {
        return m_Quantity;
    }
    
    override bool CheckPayment(IEntity player, int quantity = 1)
    {
        int totalCurrency = 0;

        // Attempt to cast to SCR_ChimeraCharacter
        SCR_ChimeraCharacter playerCharacter = SCR_ChimeraCharacter.Cast(player);
        if (playerCharacter)
        {
            totalCurrency = ADM_CurrencyComponent.FindTotalCurrencyInInventory(playerCharacter);
        }
        else
        {
            // Retrieve SCR_InventoryStorageManagerComponent from the entity
            SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
            if (inventoryManager)
            {
                totalCurrency = ADM_CurrencyComponent.FindTotalCurrencyInInventory(inventoryManager);
            }
            else
            {
                ////Print("Error: Unable to cast player to SCR_ChimeraCharacter or find SCR_InventoryStorageManagerComponent", LogLevel.ERROR);
                return false;
            }
        }

        if (totalCurrency < m_Quantity * quantity) 
            return false;
        
        return true;
    }
    
    override bool CollectPayment(IEntity player, int quantity = 1)
    {
        if (!Replication.IsServer() || !CheckPayment(player, quantity)) 
            return false;
        
        bool didRemoveCurrency = false;

        // Attempt to cast to SCR_ChimeraCharacter
        SCR_ChimeraCharacter playerCharacter = SCR_ChimeraCharacter.Cast(player);
        if (playerCharacter)
        {
            didRemoveCurrency = ADM_CurrencyComponent.RemoveCurrencyFromInventory(playerCharacter, m_Quantity * quantity);
        }
        else
        {
            // Retrieve SCR_InventoryStorageManagerComponent from the entity
            SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
            if (inventoryManager)
            {
                didRemoveCurrency = ADM_CurrencyComponent.RemoveCurrencyFromInventory(inventoryManager, m_Quantity * quantity);
            }
            else
            {
                ////Print("Error: Unable to cast player to SCR_ChimeraCharacter or find SCR_InventoryStorageManagerComponent", LogLevel.ERROR);
                return false;
            }
        }

        return didRemoveCurrency;
    }
    
    override bool DistributePayment(IEntity player, int quantity = 1)
    {
        if (!Replication.IsServer()) 
            return false;
        
        bool didAddCurrency = false;

        // Attempt to cast to SCR_ChimeraCharacter
        SCR_ChimeraCharacter playerCharacter = SCR_ChimeraCharacter.Cast(player);
        if (playerCharacter)
        {
            didAddCurrency = ADM_CurrencyComponent.AddCurrencyToInventory(playerCharacter, m_Quantity * quantity);
        }
        else
        {
            // Retrieve SCR_InventoryStorageManagerComponent from the entity
            SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
            if (inventoryManager)
            {
                didAddCurrency = ADM_CurrencyComponent.AddCurrencyToInventory(inventoryManager, m_Quantity * quantity);
            }
            else
            {
                ////Print("Error: Unable to cast player to SCR_ChimeraCharacter or find SCR_InventoryStorageManagerComponent", LogLevel.ERROR);
                return false;
            }
        }

        return didAddCurrency;
    }
    
    override string GetDisplayName(int quantity = 1)
    {
        return string.Format("$%1", Math.AbsInt(m_Quantity * quantity));
    }
    
    override ResourceName GetDisplayEntity()
    {
        return "{B0E67230AEEE2DF3}Prefabs/Items/Wallet.et";
    }
    
    override bool Equals(ADM_PaymentMethodBase other)
    {
        ADM_PaymentMethodCurrency otherCurrency = ADM_PaymentMethodCurrency.Cast(other);
        if (!otherCurrency)
            return false;
        
        return true;
    }
    
    override bool Add(ADM_PaymentMethodBase other, int quantity = 1)
    {
        if (!this.Equals(other))
            return false;
        
        ADM_PaymentMethodCurrency otherCurrency = ADM_PaymentMethodCurrency.Cast(other);
        if (!otherCurrency)
            return false;
                
        this.m_Quantity += otherCurrency.m_Quantity * quantity;
        
        return true;
    }
}
