class KF_EconomyInfoDisplay : SCR_InfoDisplay
{
	SCR_BaseGameMode m_GameMode;
	PlayerManager m_PlayerManager;
	VerticalLayoutWidget m_wBalanceDisplay;
	VerticalLayoutWidget m_wTransactionFeed;
	Widget m_wCurrencyWidget;
	IEntity m_PlayerEnt;
	
	ref array<TextWidget> m_aBalances = {};
	
	//------------------------------------------------------------------------------------------------
	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		
		m_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		m_PlayerManager = GetGame().GetPlayerManager();
		m_PlayerEnt = owner;
		
		if (!m_wRoot) return;
		m_wBalanceDisplay = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("m_wBalanceDisplay"));
		m_wTransactionFeed = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("m_wTransactionFeed"));
		m_wCurrencyWidget = m_wRoot.FindAnyWidget("m_wCurrencyWidget");
	}
	
	//------------------------------------------------------------------------------------------------
	// Todo: Enable multiple currencies HUD
	override event void OnStartDraw(IEntity owner)
    {
		super.OnStartDraw(owner);
		
		if (!m_wCurrencyWidget) m_wCurrencyWidget = m_wRoot.FindAnyWidget("m_wCurrencyWidget");
		
		GetGame().GetCallqueue().CallLater(RefreshHUD, 1000, true, owner, m_wCurrencyWidget);
	}
	
	//------------------------------------------------------------------------------------------------
	void RefreshHUD(IEntity owner, Widget currencyWidget)
	{
		if (!m_GameMode || !m_PlayerManager || !currencyWidget) return;
		
		TextWidget balanceText = TextWidget.Cast(currencyWidget.FindAnyWidget("m_wAmount"));
		
		DisplayBalanceChange(balanceText);
	}
	
	//------------------------------------------------------------------------------------------------
	void DisplayBalanceChange(TextWidget balanceText)
	{
    	SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(m_PlayerEnt);
    	if (!char) return;

    	int currentBalance = KF_Utils.RemoveFormatting(balanceText.GetText()).ToInt();
    	//int targetBalance = char.m_aBalances.Get(0);
		int targetBalance = char.m_iBalance;

    	if (currentBalance == targetBalance) 
    	{
        	balanceText.SetColor(Color.White);
        	return;
    	}

    	int balanceDifference = targetBalance - currentBalance;
    	int sign;
		if (currentBalance < targetBalance) sign = 1;
		else sign = -1;

    	int changeAmount = Math.Max(1, Math.AbsInt(balanceDifference) / 10);
    	currentBalance += changeAmount * sign;

    	// Set color based on direction
    	if (sign > 0) balanceText.SetColor(Color.DarkGreen);
    	else balanceText.SetColor(Color.DarkRed);

    	balanceText.SetText(KF_Utils.FormatNumber(currentBalance.ToString()));

    	GetGame().GetCallqueue().CallLater(DisplayBalanceChange, 50, false, balanceText);
	}
	
	//------------------------------------------------------------------------------------------------
	void NotifyPlayer(int amount, string message)
	{
		if (!m_wTransactionFeed) m_wTransactionFeed = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("m_wTransactionFeed"));
		
		// Create notification
		Widget notification = GetGame().GetWorkspace().CreateWidgets("{426E0A8C08947912}UI/layouts/HUD/Balance/TransactionEntry.layout", m_wTransactionFeed);
		TextWidget messageWidget = TextWidget.Cast(notification.FindAnyWidget("m_wTitle"));
		
		if (amount > 0)
		{
			messageWidget.SetColor(Color.DarkGreen);
			message += "  +$";
			messageWidget.SetText(message + KF_Utils.FormatNumber(amount.ToString()));
		}
		else if (amount < 0)
		{
			messageWidget.SetColor(Color.DarkRed);
			message += "  -$";
			amount *= -1;
			messageWidget.SetText(message + KF_Utils.FormatNumber(amount.ToString()));
		}
		else if (amount == 0)
		{
			messageWidget.SetColor(Color.DarkYellow);
			message += "  FREE";
			messageWidget.SetText(message);
		}
		
		GetGame().GetCallqueue().CallLater(FadeOutWidget, 3000, false, notification, 0.01);
	}
	
	//------------------------------------------------------------------------------------------------
	void FadeOutWidget(Widget w, float progress)
	{
		float fade = 1 - progress;
		progress = SCR_Easing.EaseOutCubic(progress);
		
		if (fade > 0)
		{
			w.SetOpacity(fade);
			GetGame().GetCallqueue().CallLater(FadeOutWidget, 75, false, w, progress);
		}
		else if (fade <= 0)
		{
			w.RemoveFromHierarchy();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void AlertPlayer(bool enable)
	{
		FrameWidget arsenalFrame = FrameWidget.Cast(m_wRoot.FindAnyWidget("m_wArsenalFrame"));
		if (!arsenalFrame) return;
		
		arsenalFrame.SetVisible(enable);
	}
}