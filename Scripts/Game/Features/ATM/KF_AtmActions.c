class KF_ATM_StartAction : ScriptedUserAction
{
	[RplProp()]
    KF_AtmScreen m_AtmScreen;

    //------------------------------------------------------------------------------------------------
    override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
    {
        m_AtmScreen = KF_AtmScreen.Cast(pOwnerEntity);
    }

    //------------------------------------------------------------------------------------------------
    override bool CanBeShownScript(IEntity user)
    {
        return (!m_AtmScreen.m_bBusy);
    }

    //------------------------------------------------------------------------------------------------
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
    {
        int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
        m_AtmScreen.SetUser(playerId);
    }
}
class KF_ATM_DepositAction100 : ScriptedUserAction
{
	int moneyAmount = 100;
	[RplProp()]
    KF_AtmScreen m_AtmScreen;
    SCR_BaseGameMode m_GameMode;

    [Attribute("0", UIWidgets.CheckBox, "")]
    bool m_bDepositAll;

    //------------------------------------------------------------------------------------------------
    override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
    {
        m_AtmScreen = KF_AtmScreen.Cast(pOwnerEntity);
        m_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
    }

    //------------------------------------------------------------------------------------------------
    override bool CanBeShownScript(IEntity user)
    {
        return (m_AtmScreen.m_bBusy);
    }

    //------------------------------------------------------------------------------------------------
    override bool CanBePerformedScript(IEntity user)
    {
        SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
        if (!userChar) return false;

        int cashAmount = userChar.m_iCash;

        SetCannotPerformReason("Insufficient cash");
        if (m_bDepositAll)
        {
            return (cashAmount > 0);
        }
        else
        {
            return (moneyAmount <= cashAmount);
        }
    }

    //------------------------------------------------------------------------------------------------
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
    {
        int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);

        SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(pUserEntity);
        if (!userChar) return;

        int amount;
        if (m_bDepositAll)
        {
            amount = userChar.m_iCash;
        }
        else
        {
            amount = moneyAmount;
        }

        if (moneyAmount > 0)
        {
            userChar.SetCash(-moneyAmount, true);
            m_GameMode.AdjustBalanceAndNotify(playerId, "$", moneyAmount, "Deposit $" + moneyAmount);
        }
    }
}
class KF_ATM_DepositAction1000 : ScriptedUserAction
{
	int moneyAmount = 1000;
	[RplProp()]
    KF_AtmScreen m_AtmScreen;
    SCR_BaseGameMode m_GameMode;

    [Attribute("0", UIWidgets.CheckBox, "")]
    bool m_bDepositAll;

    //------------------------------------------------------------------------------------------------
    override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
    {
        m_AtmScreen = KF_AtmScreen.Cast(pOwnerEntity);
        m_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
    }

    //------------------------------------------------------------------------------------------------
    override bool CanBeShownScript(IEntity user)
    {
        return (m_AtmScreen.m_bBusy);
    }

    //------------------------------------------------------------------------------------------------
    override bool CanBePerformedScript(IEntity user)
    {
        SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
        if (!userChar) return false;

        int cashAmount = userChar.m_iCash;

        SetCannotPerformReason("Insufficient cash");
        if (m_bDepositAll)
        {
            return (cashAmount > 0);
        }
        else
        {
            return (moneyAmount <= cashAmount);
        }
    }

    //------------------------------------------------------------------------------------------------
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
    {
        int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);

        SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(pUserEntity);
        if (!userChar) return;

        int amount;
        if (m_bDepositAll)
        {
            amount = userChar.m_iCash;
        }
        else
        {
            amount = moneyAmount;
        }

        if (moneyAmount > 0)
        {
            userChar.SetCash(-moneyAmount, true);
            m_GameMode.AdjustBalanceAndNotify(playerId, "$", moneyAmount, "Deposit $" + moneyAmount);
        }
    }
}
class KF_ATM_DepositAction10000 : ScriptedUserAction
{
	int moneyAmount = 100;
	[RplProp()]
    KF_AtmScreen m_AtmScreen;
    SCR_BaseGameMode m_GameMode;

    [Attribute("0", UIWidgets.CheckBox, "")]
    bool m_bDepositAll;

    //------------------------------------------------------------------------------------------------
    override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
    {
        m_AtmScreen = KF_AtmScreen.Cast(pOwnerEntity);
        m_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
    }

    //------------------------------------------------------------------------------------------------
    override bool CanBeShownScript(IEntity user)
    {
        return (m_AtmScreen.m_bBusy);
    }

    //------------------------------------------------------------------------------------------------
    override bool CanBePerformedScript(IEntity user)
    {
        SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
        if (!userChar) return false;

        int cashAmount = userChar.m_iCash;

        SetCannotPerformReason("Insufficient cash");
        if (m_bDepositAll)
        {
            return (cashAmount > 0);
        }
        else
        {
            return (moneyAmount <= cashAmount);
        }
    }

    //------------------------------------------------------------------------------------------------
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
    {
        int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);

        SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(pUserEntity);
        if (!userChar) return;

        int amount;
        if (m_bDepositAll)
        {
            amount = userChar.m_iCash;
        }
        else
        {
            amount = moneyAmount;
        }

        if (moneyAmount > 0)
        {
            userChar.SetCash(-moneyAmount, true);
            m_GameMode.AdjustBalanceAndNotify(playerId, "$", moneyAmount, "Deposit $" + moneyAmount);
        }
    }
}
//class KF_ATM_WithdrawAction : ScriptedUserAction
//{
//    KF_AtmScreen m_AtmScreen;
//    SCR_BaseGameMode m_GameMode;
//
//    //------------------------------------------------------------------------------------------------
//    override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
//    {
//        m_AtmScreen = KF_AtmScreen.Cast(pOwnerEntity);
//        m_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
//    }
//
//    //------------------------------------------------------------------------------------------------
//    override bool CanBeShownScript(IEntity user)
//    {
//        return (m_AtmScreen.m_bBusy);
//    }
//
//    //------------------------------------------------------------------------------------------------
//    override bool CanBePerformedScript(IEntity user)
//    {
//        SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
//        if (!userChar) return false;
//
//        Print("KF_ATM_WithdrawAction CanBePerformedScript m_iInputAmount = " + m_AtmScreen.m_iInputAmount.ToString(), LogLevel.ERROR); // DEBUG
//
//        int accountBalance = userChar.m_iBalance;
//
//        SetCannotPerformReason("Insufficient balance");
//        return (m_AtmScreen.m_iInputAmount <= accountBalance);
//    }
//
//    //------------------------------------------------------------------------------------------------
//    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
//    {
//        int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
//
//        SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(pUserEntity);
//        if (!userChar) return;
//
//        int amount = m_AtmScreen.m_iInputAmount;
//        Print("KF_ATM_WithdrawAction PerformAction m_iInputAmount = " + m_AtmScreen.m_iInputAmount.ToString() + " Amount: " + amount.ToString(), LogLevel.ERROR);  // DEBUG
//
//        if (amount > 0 && amount <= userChar.m_iBalance)
//        {
//            userChar.SetCash(amount, true);
//            m_GameMode.AdjustBalanceAndNotify(playerId, "$", -amount, "Withdrawal");
//        }
//    }
//}

class KF_ATM_WithdrawAction100 : ScriptedUserAction
{
	[RplProp()]
	KF_AtmScreen m_AtmScreen;
	SCR_BaseGameMode m_GameMode;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_AtmScreen = KF_AtmScreen.Cast(pOwnerEntity);
		m_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		return (m_AtmScreen.m_bBusy);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_GameMode) m_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user);
		int accountBalance = m_GameMode.GetBalance(playerId, "$");
		
		SetCannotPerformReason("Insufficient balance");
		return (100 <= accountBalance);
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
		
		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!userChar) return;
		
		userChar.SetCash(m_AtmScreen.m_iInputAmount, true);
		m_GameMode.AdjustBalanceAndNotify(playerId, "$", -100, "Withdrawal");
	}
}

class KF_ATM_WithdrawAction1000 : ScriptedUserAction
{
	[RplProp()]
	KF_AtmScreen m_AtmScreen;
	SCR_BaseGameMode m_GameMode;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_AtmScreen = KF_AtmScreen.Cast(pOwnerEntity);
		m_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		return (m_AtmScreen.m_bBusy);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_GameMode) m_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user);
		int accountBalance = m_GameMode.GetBalance(playerId, "$");
		
		SetCannotPerformReason("Insufficient balance");
		return (1000 <= accountBalance);
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
		
		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!userChar) return;
		
		userChar.SetCash(m_AtmScreen.m_iInputAmount, true);
		m_GameMode.AdjustBalanceAndNotify(playerId, "$", -1000, "Withdrawal");
	}
}
class KF_ATM_WithdrawAction10000 : ScriptedUserAction
{
	[RplProp()]
	KF_AtmScreen m_AtmScreen;
	SCR_BaseGameMode m_GameMode;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_AtmScreen = KF_AtmScreen.Cast(pOwnerEntity);
		m_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		return (m_AtmScreen.m_bBusy);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_GameMode) m_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user);
		int accountBalance = m_GameMode.GetBalance(playerId, "$");
		
		SetCannotPerformReason("Insufficient balance");
		return (10000 <= accountBalance);
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
		
		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!userChar) return;
		
		userChar.SetCash(m_AtmScreen.m_iInputAmount, true);
		m_GameMode.AdjustBalanceAndNotify(playerId, "$", -10000, "Withdrawal");
	}
}



class KF_ATM_ChangeAmountAction : SCR_AdjustSignalAction
{
	[RplProp()]
	KF_AtmScreen m_AtmScreen;
	[RplProp()]
	int m_iAmount = 1000;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_AtmScreen = KF_AtmScreen.Cast(pOwnerEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!m_AtmScreen) return false;
		
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user);
		
		return (playerId == m_AtmScreen.m_iCurrentUser);
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandleAction(float value)
{
    super.HandleAction(value);

    // Adjust m_iAmount based on input
    if (value > 0 && m_iAmount < 1000000)
        m_iAmount += 1000;
    else if (value < 0 && m_iAmount > 1000)
        m_iAmount -= 1000;

    // Update local UI
    m_AtmScreen.m_ChangeAmount.SetText("$" + m_iAmount);

    // If not on the server, send an RPC to the server to update m_iInputAmount
    if (!Replication.IsServer())
    {
        m_AtmScreen.RpcSetInputAmount(m_iAmount);
    }
    else
    {
        m_AtmScreen.SetInputAmount(m_iAmount);
    }
}

}

//class KF_ATM_ChangeAmountAction : SCR_AdjustSignalAction
//{
//    KF_AtmScreen m_AtmScreen;
//    [RplProp()]
//    int m_iAmount = 1000;
//
//    //------------------------------------------------------------------------------------------------
//    override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
//    {
//        m_AtmScreen = KF_AtmScreen.Cast(pOwnerEntity);
//    }
//
//    //------------------------------------------------------------------------------------------------
//    override bool CanBeShownScript(IEntity user)
//    {
//        if (!m_AtmScreen) return false;
//
//        int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user);
//
//        return (playerId == m_AtmScreen.m_iCurrentUser);
//    }
//  
	    //------------------------------------------------------------------------------------------------
//    override void HandleAction(float value)
//    {
//        super.HandleAction(value);
//    
//        if (!Replication.IsServer())
//        {
//            Print("[CLIENT] Attempting to call server-side HandleAction", LogLevel.ERROR);
//            RpcDoHandleActionId(value);  // Call RPC to execute server-side logic
//            return;
//        }
//    
//        Print("[SERVER/CLIENT] Executing HandleAction m_iAmount = " + m_iAmount.ToString() + " value: " + value.ToString(), LogLevel.ERROR);
//    
//        AdjustAmount(value);
//    
//        m_iAmount = Math.Clamp(m_iAmount, 0, 20000000);
//    
//        // Update local UI
//        if (m_AtmScreen.m_ChangeAmount)
//        {
//            m_AtmScreen.m_ChangeAmount.SetText("$" + m_iAmount.ToString());
//        }
//    }
//    
//    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
//    void RpcDoHandleActionId(float value)
//    {
//        if (Replication.IsServer())
//        {
//            HandleAction(value);  // Ensure server executes the same logic
//        }
//    }
//    
//    void AdjustAmount(float value)
//    {
//        if (value > 0)
//        {
//            if (m_iAmount <= 1000)
//                m_iAmount += 50;
//            else if (m_iAmount <= 20000)
//                m_iAmount += 500;
//            else if (m_iAmount <= 200000)
//                m_iAmount += 5000;
//            else if (m_iAmount < 200000000)
//                m_iAmount += 25000;
//        }
//        else if (value < 0)
//        {
//            if (m_iAmount > 200000000)
//                m_iAmount -= 25000;
//            else if (m_iAmount > 200000)
//                m_iAmount -= 5000;
//            else if (m_iAmount > 2000)
//                m_iAmount -= 500;
//            else if (m_iAmount > 100)
//                m_iAmount -= 50;
//        }
//    }
//}