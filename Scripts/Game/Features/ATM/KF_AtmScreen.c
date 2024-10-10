// KF_AtmScreen.c

class KF_AtmScreenClass : GenericEntityClass  // Changed from GenericEntityClass to RplEntityClass
{
};

class KF_AtmScreen : GenericEntity  // Changed from GenericEntity to RplEntity
{
    protected Widget m_wRoot;
    protected RTTextureWidget m_wRenderTargetTextureWidget;

    [Attribute("{640C6064DB7E7352}UI/layouts/ATM/ATM_Screen.layout", UIWidgets.ResourcePickerThumbnail, "The layout used for the PIP component", params: "layout")]
    protected ResourceName m_Layout;

    [Attribute("RTTexture0", UIWidgets.EditBox, "Name of RTTexture widget within provided layout")]
    protected string m_sRTTextureWidgetName;

    TextWidget m_UserName;
    TextWidget m_AccountBalance;
    TextWidget m_ChangeAmount;
    TextWidget m_StartText;

    ImageWidget m_IdleScreen;
    ImageWidget m_InterfaceScreen;

    SCR_BaseGameMode m_GameMode;

    [RplProp()]
    bool m_bBusy;

    [RplProp()]
    int m_iCurrentUser;

    [RplProp()]
    int m_iInputAmount;

    IEntity m_UserEntity;

    //------------------------------------------------------------------------------------------------
    override void EOnInit(IEntity owner)
    {
        m_GameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());

        if (!m_wRoot || !m_wRenderTargetTextureWidget)
            m_wRoot = CreateUI(m_Layout, m_sRTTextureWidgetName, m_wRenderTargetTextureWidget);

        if (m_wRoot)
        {
            m_UserName = TextWidget.Cast(m_wRoot.FindAnyWidget("UserName"));
            m_AccountBalance = TextWidget.Cast(m_wRoot.FindAnyWidget("AccountBalance"));
            m_ChangeAmount = TextWidget.Cast(m_wRoot.FindAnyWidget("ChangeAmount"));
            m_StartText = TextWidget.Cast(m_wRoot.FindAnyWidget("StartText"));

            m_IdleScreen = ImageWidget.Cast(m_wRoot.FindAnyWidget("IdleScreen"));
            m_InterfaceScreen = ImageWidget.Cast(m_wRoot.FindAnyWidget("InterfaceScreen"));

            m_wRenderTargetTextureWidget.SetGUIWidget(this, 0);

            IdleScreen();
        }
    }

    //------------------------------------------------------------------------------------------------
    protected Widget CreateUI(string layout, string rtTextureName, out RTTextureWidget RTTexture)
    {
        // Empty layout, cannot create any widget
        if (layout == string.Empty)
            return null;

        // Create layout
        Widget root = GetGame().GetWorkspace().CreateWidgets(layout);

        // Layout was not created successfully
        if (!root)
            return null;

        // We don't have required RT widgets, delete layout and terminate
        RTTexture = RTTextureWidget.Cast(root.FindAnyWidget(rtTextureName));
        if (!RTTexture)
        {
            root.RemoveFromHierarchy();
            return null;
        }

        return root;
    }

    //------------------------------------------------------------------------------------------------
    void SetUser(int playerId)
    {
        if (Replication.IsServer())
        {
            m_bBusy = true;
            m_iCurrentUser = playerId;
            Replication.BumpMe();
            GetGame().GetCallqueue().CallLater(RefreshScreen, 1000, false);
        }
        else
        {
            Rpc(RpcAsk_SetUser, playerId);
        }
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcAsk_SetUser(int playerId)
    {
        SetUser(playerId);
    }

    //------------------------------------------------------------------------------------------------
    void RefreshScreen()
    {
        if (!m_bBusy) return;

        IEntity userEntity = GetGame().GetPlayerManager().GetPlayerControlledEntity(m_iCurrentUser);
        if (!userEntity)
        {
            ResetATM();
            return;
        }

        vector userPos = userEntity.GetOrigin();
        vector atmPos = GetOrigin();
        float distance = vector.Distance(userPos, atmPos);

        if (distance > 2)
        {
            ResetATM();
            return;
        }

        Interface();
    }

    void ResetATM()
    {
        m_bBusy = false;
        m_iCurrentUser = -1;
        Replication.BumpMe();
        m_UserName.SetText("N/A");
        m_AccountBalance.SetText("N/A");
        m_ChangeAmount.SetText("$0"); // Reset the input amount display
        IdleScreen();
    }

    //------------------------------------------------------------------------------------------------
    void Interface()
    {
        if (!m_bBusy) return;

        string playerName = GetGame().GetPlayerManager().GetPlayerName(m_iCurrentUser);
        SCR_ChimeraCharacter playerChar = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(m_iCurrentUser));
        int accountBalance = playerChar.m_iBalance;

        m_UserName.SetText(playerName);
        m_AccountBalance.SetText("$" + accountBalance.ToString());
        m_ChangeAmount.SetText("$" + m_iInputAmount.ToString()); // Update the displayed input amount

        GetGame().GetCallqueue().CallLater(RefreshScreen, 1000, false);
    }

    //------------------------------------------------------------------------------------------------
    void IdleScreen()
    {
        m_IdleScreen.SetOpacity(1);

        // Toggle StartText opacity without using the ternary operator
        float startTextOpacity = m_StartText.GetOpacity();
        if (startTextOpacity == 0)
            m_StartText.SetOpacity(1);
        else
            m_StartText.SetOpacity(0);

        if (m_bBusy)
        {
            m_IdleScreen.SetOpacity(0);
            m_StartText.SetOpacity(0);
            Interface();
            return;
        }

        GetGame().GetCallqueue().CallLater(IdleScreen, 1000, false);
    }

    // Server-side method to update m_iInputAmount
	void SetInputAmount(int amount)
	{
	    if (Replication.IsServer())
	    {
	        // Clamp the amount
	        m_iInputAmount = Math.Clamp(amount, 0, 20000000);
	
	        // Notify the replication system
	        Replication.BumpMe();
	
	        // Update the UI (if needed)
	        if (m_ChangeAmount)
	        {
	            m_ChangeAmount.SetText("$" + m_iInputAmount.ToString());
	        }
	    }
	}
	
	// RPC from client to server to request updating m_iInputAmount
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcSetInputAmount(int amount)
	{
	    SetInputAmount(amount);
	}
}