class KF_CashWidgetComponent : SCR_ScriptedWidgetComponent
{
	TextWidget m_wCashText;
	
	int m_iPlayerId;
	SCR_ChimeraCharacter m_PlayerChar;
	
	SCR_BaseGameMode m_GameMode;
	
	//------------------------------------------------------------------------------------------------
	override bool OnUpdate(Widget w)
	{
		if (!m_GameMode) m_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!m_wCashText) m_wCashText = TextWidget.Cast(w);
		
		if (!m_GameMode.m_bEnableCash)
		{
			m_wCashText.SetVisible(false);
			return super.OnUpdate(w);
		}
		
		if (!m_PlayerChar)
		{
			m_iPlayerId = SCR_PlayerController.GetLocalPlayerId();
			m_PlayerChar = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(m_iPlayerId));
			
			return super.OnUpdate(w);
		}
		
		m_wCashText.SetText("$" + KF_Utils.FormatNumber(m_PlayerChar.m_iCash.ToString()));
		
		return super.OnUpdate(w);
	}
}