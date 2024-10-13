class MIKE_GarageShopUI_Item : SCR_ModularButtonComponent
{
	[Attribute(defvalue: "MenuNavLeft")]
	protected string m_sQuantityActionLess;
	
	[Attribute(defvalue: "MenuNavRight")]
	protected string m_sQuantityActionMore;
	
	[Attribute(defvalue: "MenuLeft")]
	protected string m_sScrollActionLeft;
	
	[Attribute(defvalue: "MenuRight")]
	protected string m_sScrollActionRight;
	
	[Attribute(defvalue: "1")]
	protected int m_quantity = 1;
	
	protected bool m_bHasActionListeners;
	protected ref ADM_ShopMerchandise m_Merchandise;
	protected MIKE_GarageShopUI m_ShopUI;
	protected SCR_PagingButtonComponent m_ButtonLeft;
	protected SCR_PagingButtonComponent m_ButtonRight;
	protected ScrollLayoutWidget m_wPriceScrollLayout;
	protected bool m_bBuyOrSell; 
	protected int m_iMaxQuantity = 0;
	protected bool m_bIsCartItem = false;
override bool OnClick(Widget w, int x, int y, int button)
{
    Print("MIKE_GarageShopUI_Item OnClick Invoked!", LogLevel.NORMAL);
    return super.OnClick(w, x, y, button);
}

	int GetQuantity()
	{
		Print("GetQuantity Invoked!", LogLevel.NORMAL);
		return m_quantity;
	}
	
	void SetQuantity(int quantity)
	{
		Print("SetQuantity Invoked!", LogLevel.NORMAL);
		m_quantity = quantity;
		UpdateItem(this.m_wRoot);
	}
	
	void UpdateQuantity(int amount)
	{
		Print("UpdateQuantity Invoked!", LogLevel.NORMAL);
		m_quantity = m_quantity + amount;
		if (m_quantity < 1)
			m_quantity = 1;
		
		UpdateItem(this.m_wRoot);
	}
	
	void SetMaxQuantity(int quantity)
	{
		m_iMaxQuantity = quantity;
	}
	
	ADM_ShopMerchandise GetMerchandise()
	{
		Print("GetMerchandise Invoked!", LogLevel.NORMAL);
		return m_Merchandise;
	}
	
	void SetMerchandise(ADM_ShopMerchandise merch)
	{
		Print("SetMerchandise Invoked!", LogLevel.NORMAL);
		m_Merchandise = merch;
	}
	
	void SetShopUI(MIKE_GarageShopUI shopUI)
	{
		Print("SetShopUI Invoked!", LogLevel.NORMAL);
		m_ShopUI = shopUI;
	}
	
	protected void OnQuantityLess()
	{
		if (GetGame().GetWorkspace().GetFocusedWidget() != m_wRoot)
			return;

		if (m_bIsCartItem)
			return;
		
		UpdateQuantity(-1);
	}

	protected void OnQuantityMore()
	{
		if (GetGame().GetWorkspace().GetFocusedWidget() != m_wRoot)
			return;

		if (m_bIsCartItem)
			return;

		UpdateQuantity(1);
	}
	
	protected void OnScrollLeft()
	{
		if (GetGame().GetWorkspace().GetFocusedWidget() != m_wRoot || !m_wPriceScrollLayout)
			return;
		
		float x,y;
		m_wPriceScrollLayout.GetSliderPos(x,y);
		m_wPriceScrollLayout.SetSliderPos(x-0.1,y);
	}
	
	protected void OnScrollRight()
	{
		if (GetGame().GetWorkspace().GetFocusedWidget() != m_wRoot || !m_wPriceScrollLayout)
			return;
		
		float x,y;
		m_wPriceScrollLayout.GetSliderPos(x,y);
		m_wPriceScrollLayout.SetSliderPos(x+0.1,y);
	}
	
	void SetBuyOrSell(bool b)
	{
		m_bBuyOrSell = b;
	}
	
	bool GetBuyOrSell()
	{
		return m_bBuyOrSell;
	}
	
	override void HandlerAttached(Widget w)
	{
		Print("HandlerAttached Invoked!", LogLevel.NORMAL);
		super.HandlerAttached(w);
		
		Widget left = w.FindAnyWidget("ButtonLeft");
		Widget right = w.FindAnyWidget("ButtonRight");
		m_wPriceScrollLayout = ScrollLayoutWidget.Cast(w.FindAnyWidget("ScrollLayout0"));

		if (left)
		{
			left.SetVisible(false);
			m_ButtonLeft = SCR_PagingButtonComponent.Cast(left.FindHandler(SCR_PagingButtonComponent));
			if (m_ButtonLeft)
				m_ButtonLeft.m_OnActivated.Insert(OnQuantityLess);
		}

		if (right)
		{
			right.SetVisible(false);
			m_ButtonRight = SCR_PagingButtonComponent.Cast(right.FindHandler(SCR_PagingButtonComponent));
			if (m_ButtonRight)
				m_ButtonRight.m_OnActivated.Insert(OnQuantityMore);
		}
		
		UpdateItem(m_wRoot);
	}
	
	void AddActionListeners()
	{
		if (m_bHasActionListeners)
			return;
		
		GetGame().GetInputManager().AddActionListener(m_sQuantityActionLess, EActionTrigger.DOWN, OnQuantityLess);
		GetGame().GetInputManager().AddActionListener(m_sQuantityActionMore, EActionTrigger.DOWN, OnQuantityMore);
		
		GetGame().GetInputManager().AddActionListener(m_sScrollActionLeft, EActionTrigger.DOWN, OnScrollLeft);
		GetGame().GetInputManager().AddActionListener(m_sScrollActionRight, EActionTrigger.DOWN, OnScrollRight);
		
		m_bHasActionListeners = true;
	}
	
	void RemoveActionListeners()
	{
		if (!m_bHasActionListeners)
			return;
		
		GetGame().GetInputManager().RemoveActionListener(m_sQuantityActionLess, EActionTrigger.DOWN, OnQuantityLess);
		GetGame().GetInputManager().RemoveActionListener(m_sQuantityActionMore, EActionTrigger.DOWN, OnQuantityMore);
		
		GetGame().GetInputManager().RemoveActionListener(m_sScrollActionLeft, EActionTrigger.DOWN, OnScrollLeft);
		GetGame().GetInputManager().RemoveActionListener(m_sScrollActionRight, EActionTrigger.DOWN, OnScrollRight);
		
		m_bHasActionListeners = false;
	}
	
	override bool OnFocus(Widget w, int x, int y)
	{
		Print("OnFocus Invoked!", LogLevel.NORMAL);
		super.OnFocus(w, x, y);

		AddActionListeners();

		Widget left = w.FindAnyWidget("ButtonLeft");
		if (left)
		{
			left.SetVisible(true);
		}

		Widget right = w.FindAnyWidget("ButtonRight");
		if (right)
		{
			right.SetVisible(true);
		}
		
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		Print("OnFocusLost Invoked!", LogLevel.NORMAL);
		super.OnFocusLost(w, x, y);

		RemoveActionListeners();

		Widget left = w.FindAnyWidget("ButtonLeft");
		if (left)
		{
			left.SetVisible(false);
		}

		Widget right = w.FindAnyWidget("ButtonRight");
		if (right)
		{
			right.SetVisible(false);
		}

		return false;
	}
	
	void SetCartItem(bool b)
	{
		Print("SetCartItem Invoked!", LogLevel.NORMAL);
		m_bIsCartItem = b;
	}
	
	bool CanPurchase()
	{
		Print("CanPurchase Invoked!", LogLevel.NORMAL);
		if (GetBuyOrSell())
		{
			int additionalQuantity = m_quantity;
			if (m_bIsCartItem)
			{
				additionalQuantity = 0;
			}
			
			return m_ShopUI.CanPurchase(m_Merchandise, additionalQuantity);
		} else {
			Print("CanPurchase Invoked2!", LogLevel.NORMAL);
			return true;
		}
	}
	
	bool UpdateItem(Widget w)
	{
		Print("UpdateItem Invoked!", LogLevel.NORMAL);
		if (m_quantity < 1) m_quantity = 1;

		if (m_iMaxQuantity > 0 && m_quantity > m_iMaxQuantity)
		{
			m_quantity = m_iMaxQuantity;
		}
		
		TextWidget quantity = TextWidget.Cast(w.FindAnyWidget("Quantity"));
		if (quantity) 
		{
			quantity.SetTextFormat("x%1", m_quantity);
			
			if (m_Merchandise && !CanPurchase())
			{
				quantity.SetColor(Color.Red);
			} else {
				quantity.SetColor(Color.White);
			}
			
			quantity.Update();
		}
		
		return true;
	}
}





class MIKE_GarageShopUI: ChimeraMenuBase
{
	static const ResourceName m_BarterIconPrefab = "{BBEF0EBB9F35B19F}UI/Layouts/Menus/BasicShopMenu/MarketBarterItemIcon.layout";
	
	protected MIKE_GarageComponent m_Shop;
	protected ItemPreviewManagerEntity m_ItemPreviewManager;
	
	protected bool m_bFocused = false;
	protected int m_iCategoryBuy = -1;
	protected int m_iCategorySell = -1;
	protected string m_sSearchBuy = "";
	protected string m_sSearchSell = "";
	
	protected Widget m_wRoot;
	protected TextWidget m_wHeaderText;
	protected TextWidget m_wMoneyText;
	
	protected Widget m_wBuySellTabWidget;
	protected SCR_TabViewComponent m_BuySellTabComponent;
	protected SCR_TabViewContent m_wBuyTabView;
	protected SCR_TabViewContent m_wSellTabView;
	
	protected Widget m_wCartTabWidget;
	protected SCR_TabViewComponent m_CartTabComponent;
	protected SCR_TabViewContent m_wCartTabView;
	
	protected ref map<ADM_ShopMerchandise, int> m_BuyShoppingCart = new map<ADM_ShopMerchandise, int>;
	protected ref map<ADM_ShopMerchandise, int> m_SellShoppingCart = new map<ADM_ShopMerchandise, int>;
	
	void SetShop(MIKE_GarageComponent shop)
	{
		Print("SetShop Invoked!", LogLevel.NORMAL);
		m_Shop = shop;
		
		if (m_wHeaderText)
		{
			m_wHeaderText.SetText(m_Shop.GetName());
		}
		
		ConfigureTabs();
	}

	
	bool CanPurchase(ADM_ShopMerchandise additionalMerchandise = null, int additionalQuantity = 0)
	{
		Print("CanPurchase Invoked!", LogLevel.NORMAL);
		// this method isn't perfect because it doesn't keep track of money spent as it iterates all merchandise in the cart
		// so at best it only returns false when an item of the same type exceeds the amount possible
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return false;
		
		IEntity player = playerController.GetMainEntity();
		if (!player)
			return false;
		
		int sameMerchandiseQuantity = 0;
		foreach(ADM_ShopMerchandise merch, int quantity : m_BuyShoppingCart)
		{
			if (additionalMerchandise && additionalMerchandise == merch)
				sameMerchandiseQuantity = quantity;
			
			if (!m_Shop.CanPurchase(player, merch.GetBuyPayment(), quantity))
			{
				return false;
			}
		}
		
		if (additionalMerchandise && !m_Shop.CanPurchase(player, additionalMerchandise.GetBuyPayment(), additionalQuantity+sameMerchandiseQuantity))
			return false;
		
		return true;
	}
	
	int GetCartQuantity(ADM_ShopMerchandise merchandise, bool buyOrSell = true)
	{
		Print("GetCartQuantity Invoked!", LogLevel.NORMAL);
		map<ADM_ShopMerchandise, int> shoppingCart = m_BuyShoppingCart;
		if (!buyOrSell)
		{
			shoppingCart = m_SellShoppingCart;
		}
			
		int quantity = shoppingCart.Get(merchandise);
		return quantity;
	}
	
	void AddToCart(MIKE_GarageShopUI_Item item)
	{
	    // Log the start of the AddToCart function
	    Print("SCRIPT       : AddToCart started.", LogLevel.NORMAL);
	    
	    // Retrieve the merchandise from the item
	    ADM_ShopMerchandise merch = item.GetMerchandise();
	    if (!merch)
	    {
	        Print("SCRIPT       : Error! ADM_ShopMerchandise is null.", LogLevel.ERROR);
	        return;
	    }
	    Print("SCRIPT       : Retrieved ADM_ShopMerchandise: " + merch, LogLevel.NORMAL);
	    
	    // Check if the item can be purchased
	    if (!item.CanPurchase())
	    {
	        Print("SCRIPT       : Cannot purchase item: " + merch.GetType().GetDisplayName(), LogLevel.WARNING);
	        return;
	    }
	    Print("SCRIPT       : Item can be purchased: " + merch.GetType().GetDisplayName(), LogLevel.NORMAL);
	    
	    // Determine which shopping cart to use (Buy or Sell)
	    map<ADM_ShopMerchandise, int> shoppingCart = m_BuyShoppingCart;
	    if (!item.GetBuyOrSell())
	    {
	        shoppingCart = m_SellShoppingCart;
	        Print("SCRIPT       : Using Sell Shopping Cart for merchandise: " + merch.GetType().GetDisplayName(), LogLevel.NORMAL);
	    }
	    else
	    {
	        Print("SCRIPT       : Using Buy Shopping Cart for merchandise: " + merch.GetType().GetDisplayName(), LogLevel.NORMAL);
	    }
	    
	    // Check if the merchandise is already in the shopping cart
	    bool inShoppingCart = shoppingCart.Contains(merch);
	    if (inShoppingCart)
	    {
	        int curQuantity = shoppingCart.Get(merch);
	        int newQuantity = curQuantity + item.GetQuantity();
	        shoppingCart.Set(merch, newQuantity);
	        Print("SCRIPT       : Updated quantity for " + merch.GetType().GetDisplayName() + ": " + curQuantity + " -> " + newQuantity, LogLevel.NORMAL);
	    }
	    else
	    {
	        shoppingCart.Insert(merch, item.GetQuantity());
	        Print("SCRIPT       : Inserted new merchandise into cart: " + merch.GetType().GetDisplayName() + " with quantity: " + item.GetQuantity(), LogLevel.NORMAL);
	    }
	    
	    // Populate the cart tab to reflect changes
	    Print("SCRIPT       : Populating Cart Tab.", LogLevel.NORMAL);
	    PopulateCartTab(m_wCartTabView);
	    Print("SCRIPT       : PopulateCartTab completed.", LogLevel.NORMAL);
	    
	    // Log the successful completion of AddToCart
	    Print("SCRIPT       : AddToCart completed successfully for merchandise: " + merch.GetType().GetDisplayName(), LogLevel.NORMAL);
	}

	
	void RemoveFromCart(MIKE_GarageShopUI_Item item)
	{
		Print("RemoveFromCart Invoked!", LogLevel.NORMAL);
		if (!item)
			return;
		
		ADM_ShopMerchandise merch = item.GetMerchandise();
		if (!merch)
			return;
		
		if (!item.GetBuyOrSell())
		{
			m_SellShoppingCart.Remove(merch);
		} else {
			m_BuyShoppingCart.Remove(merch);
		}
		
		PopulateCartTab(m_wCartTabView);
	}
	
	protected void CreateCartListboxItem(SCR_ListBoxComponent listbox, ADM_ShopMerchandise merch, bool buyOrSell = true)
	{
		Print("CreateCartListboxItem Invoked!", LogLevel.NORMAL);
		ADM_MerchandiseType merchType = merch.GetType();
		if (!merchType)
			return;
		
		// Item Name
		string itemName = merchType.GetDisplayName();
		if (!itemName)
			itemName = "Item";
			
		int itemIdx = listbox.AddItem(itemName);
		SCR_ListBoxElementComponent lbItem = listbox.GetElementComponent(itemIdx);
		if (!lbItem)
			return;
			
		// Item Preview
		ItemPreviewWidget wItemPreview = ItemPreviewWidget.Cast(lbItem.GetRootWidget().FindAnyWidget("ItemPreview0"));
		if (wItemPreview)
		{
			m_ItemPreviewManager.SetPreviewItemFromPrefab(wItemPreview, merchType.GetDisplayEntity(), null, false);
		}
			
		// Cost
		TextWidget wItemPrice = TextWidget.Cast(lbItem.GetRootWidget().FindAnyWidget("Cost"));
		array<ref ADM_PaymentMethodBase> paymentRequirement = merch.GetBuyPayment();
		bool isCurrencyOnly = MIKE_GarageComponent.IsBuyPaymentOnlyCurrency(merch);
		if (!buyOrSell)
		{
			paymentRequirement = merch.GetSellPayment();
			isCurrencyOnly = MIKE_GarageComponent.IsSellPaymentOnlyCurrency(merch);
		}
		
		if (isCurrencyOnly || paymentRequirement.Count() == 0)
		{
			if (wItemPrice) {
				string posOrMinus = "+";
				Color textCol = Color.Green;
				if (buyOrSell)
				{
					textCol = Color.Red;
					posOrMinus = "-";
				}
				wItemPrice.SetColor(textCol);
				
				wItemPrice.SetVisible(true);
				if (paymentRequirement.Count() == 0)
				{
					wItemPrice.SetColor(Color.Green);
					wItemPrice.SetText("Free");
				} else {
					ADM_PaymentMethodCurrency paymentMethod = ADM_PaymentMethodCurrency.Cast(paymentRequirement[0]);
					int quantity = paymentMethod.GetQuantity();
					if (quantity > 0)
					{
						wItemPrice.SetTextFormat("%1$%2", posOrMinus, quantity);
					} else {
						wItemPrice.SetColor(Color.White);
						wItemPrice.SetText("Free");
					}
				}
			}
		} else {
			Widget priceParent = lbItem.GetRootWidget().FindAnyWidget("CostOverlay");
			if (wItemPrice)
			{ 
				wItemPrice.SetVisible(false);
			}
			
			for (int i = 0; i < paymentRequirement.Count(); i++)
			{
				Widget icon = GetGame().GetWorkspace().CreateWidgets(m_BarterIconPrefab, priceParent);
				ADM_IconBarterTooltip barterItemIcon = ADM_IconBarterTooltip.Cast(icon.FindHandler(ADM_IconBarterTooltip));
				if (barterItemIcon)
				{
					Color texColor = Color.White;
					if (!buyOrSell)
					{
						texColor = Color.Green;
					}
					
					barterItemIcon.SetPayment(paymentRequirement[i], texColor);
				}
			}
		}
		
		// Quantity
		Print("Quantity Invoked!", LogLevel.NORMAL);
		MIKE_GarageShopUI_Item item = MIKE_GarageShopUI_Item.Cast(lbItem.GetRootWidget().FindHandler(MIKE_GarageShopUI_Item));
		if (item) {
			int quantity = GetCartQuantity(merch, buyOrSell);
			item.SetShopUI(this);
			item.SetCartItem(true);
			item.SetMerchandise(merch);
			item.SetBuyOrSell(buyOrSell);
			item.SetQuantity(quantity);
			item.SetMaxQuantity(merch.GetType().GetMaxPurchaseQuantity());
			
			item.m_OnClicked.Insert(RemoveFromCart);
		}
	}
	
	protected void PopulateCartTab(SCR_TabViewContent wTabView)
	{
		Print("PopulateCartTab Invoked!", LogLevel.NORMAL);
		Widget wListboxBuy = wTabView.m_wTab.FindAnyWidget("ListBox0");
		if (!wListboxBuy)
			return;
		
		SCR_ListBoxComponent listboxBuy = SCR_ListBoxComponent.Cast(wListboxBuy.FindHandler(SCR_ListBoxComponent));
		if (!listboxBuy)
			return;
		
		Widget wListboxSell = wTabView.m_wTab.FindAnyWidget("ListBox1");
		if (!wListboxSell)
			return;
		
		SCR_ListBoxComponent listboxSell = SCR_ListBoxComponent.Cast(wListboxSell.FindHandler(SCR_ListBoxComponent));
		if (!listboxSell)
			return;
		
		ClearTab(wTabView);
		
		TextWidget buyText = TextWidget.Cast(wTabView.m_wTab.FindAnyWidget("Text0"));
		if (m_BuyShoppingCart.Count() > 0)
		{
			buyText.SetVisible(true);
		} else {
			buyText.SetVisible(false);
		}
		
		TextWidget sellText = TextWidget.Cast(wTabView.m_wTab.FindAnyWidget("Text1"));
		if (m_SellShoppingCart.Count() > 0)
		{
			sellText.SetVisible(true);
		} else {
			sellText.SetVisible(false);
		}
		
		array<ref ADM_PaymentMethodBase> totalCost = {};
		foreach (ADM_ShopMerchandise merch, int quantity : m_BuyShoppingCart)
		{
			if (!merch)
				continue;
				
			ADM_MerchandiseType merchType = merch.GetType();
			if (!merchType)
				continue;
			
			CreateCartListboxItem(listboxBuy, merch, true);
			
			foreach(ADM_PaymentMethodBase paymentMethod : merch.GetBuyPayment())
			{
				bool didAddToTotal = false;
				foreach(ADM_PaymentMethodBase existing : totalCost)
				{
					if (didAddToTotal)
					{
						continue;
					}
					
					bool isSame = existing.Equals(paymentMethod);
					didAddToTotal = existing.Add(paymentMethod, quantity);
				}
				
				if (!didAddToTotal)
				{
					ADM_PaymentMethodBase clone = ADM_PaymentMethodBase.Cast(paymentMethod.Clone());
					int idx = totalCost.Insert(clone);
					clone.Add(paymentMethod, quantity-1);
				}
			}
		}
		
		foreach (ADM_ShopMerchandise merch, int quantity : m_SellShoppingCart)
		{
			if (!merch)
				continue;
			
			ADM_MerchandiseType merchType = merch.GetType();
			if (!merchType)
				continue;
			
			CreateCartListboxItem(listboxSell, merch, false);
			
			foreach(ADM_PaymentMethodBase paymentMethod : merch.GetSellPayment())
			{
				bool didAddToTotal = false;
				foreach(ADM_PaymentMethodBase existing : totalCost)
				{
					if (didAddToTotal)
					{
						continue;
					}
					
					bool isSame = existing.Equals(paymentMethod);
					didAddToTotal = existing.Add(paymentMethod, quantity*-1);
				}
				
				if (!didAddToTotal)
				{
					ADM_PaymentMethodBase clone = ADM_PaymentMethodBase.Cast(paymentMethod.Clone());
					int idx = totalCost.Insert(clone);
					clone.Add(paymentMethod, (quantity+1)*-1);
				}
			}
		}
		
		Widget totalCostOverlay = wTabView.m_wTab.FindAnyWidget("TotalCostOverlay");
		Widget barterParent = totalCostOverlay.FindAnyWidget("BarterItemContainer");
		TextWidget wItemPrice = TextWidget.Cast(totalCostOverlay.FindAnyWidget("TotalCostCurrencyText"));
		if ((totalCost.Count() == 1 && totalCost[0].Type().IsInherited(ADM_PaymentMethodCurrency)) || totalCost.Count() == 0)
		{
			if (barterParent)
			{
				barterParent.GetParent().SetVisible(false);
			}
			
			if (wItemPrice) {
				wItemPrice.SetColor(Color.White);
				wItemPrice.SetVisible(true);
				if (totalCost.Count() == 0)
				{
					wItemPrice.SetTextFormat("Free");
				} else {
					ADM_PaymentMethodCurrency paymentMethod = ADM_PaymentMethodCurrency.Cast(totalCost[0]);
					int quantity = paymentMethod.GetQuantity();
					if (Math.AbsInt(quantity) > 0)
					{
						wItemPrice.SetTextFormat("$%1", Math.AbsInt(quantity));
						
						if (quantity < 0)
						{
							wItemPrice.SetColor(Color.Green);
						}
					} else {
						wItemPrice.SetTextFormat("Free");
					}
				}
			}
		} else {
			if (wItemPrice)
			{ 
				wItemPrice.SetVisible(false);
			}
			
			if (barterParent)
			{
				barterParent.GetParent().SetVisible(true);
				SCR_WidgetHelper.RemoveAllChildren(barterParent);
			}
			
			for (int i = 0; i < totalCost.Count(); i++)
			{
				Widget icon = GetGame().GetWorkspace().CreateWidgets(m_BarterIconPrefab, barterParent);
				ADM_IconBarterTooltip barterItemIcon = ADM_IconBarterTooltip.Cast(icon.FindHandler(ADM_IconBarterTooltip));
				if (barterItemIcon)
				{
					barterItemIcon.SetPayment(totalCost[i]);
				}
			}
		}
		
		TextWidget emptyText = TextWidget.Cast(wTabView.m_wTab.FindAnyWidget("Empty Text"));
		if (m_BuyShoppingCart.Count() == 0 && m_SellShoppingCart.Count() == 0)
		{
			emptyText.SetVisible(true);
			emptyText.SetText("No items in shopping cart.");
			emptyText.Update();
		} else {
			emptyText.SetVisible(false);
			emptyText.Update();
		}
	}
	
	protected void Checkout(SCR_InputButtonComponent btn)
	{
		Print("Checkout Invoked!", LogLevel.NORMAL);
		PlayerController playerController = GetGame().GetPlayerController();
		if (!playerController)
			return;
		
		ADM_PlayerShopManagerComponent playerShopManager = ADM_PlayerShopManagerComponent.Cast(playerController.FindComponent(ADM_PlayerShopManagerComponent));
		if (!playerShopManager) 
			return;
		
		playerShopManager.AskProcessCart(m_Shop, m_BuyShoppingCart, m_SellShoppingCart);
		
		Close();
	}
	
	protected void ConfigureCategory(SCR_SpinBoxComponent component, array<ADM_ShopCategory> categories)
	{
		Print("ConfigureCategory Invoked!", LogLevel.NORMAL);
		if (!component || !categories)
			return;
		
		component.ClearAll();	
		
		component.AddItem("All");
		foreach (int i, ADM_ShopCategory category : categories)
		{
			component.AddItem(category.m_DisplayName);
		}
	}
	
	protected void SelectCategory(SCR_TabViewContent wTabView, int index, array<ref ADM_ShopMerchandise> items, string search = "", bool buyOrSell = true)
	{
		Print("SelectCategory Invoked!", LogLevel.NORMAL);
		if (!m_Shop || !items || items.Count() < 1) 
			return;
		
		array<ref ADM_ShopCategory> categories = m_Shop.GetCategories();
		array<ref ADM_ShopMerchandise> sortedItems = {};
		
		// All category
		if (index == -1) 
		{
			sortedItems = items;
		} else {
			array<ResourceName> validItems = {};
			if (categories[index])
				validItems = categories[index].GetItems();
			
			foreach (ADM_ShopMerchandise merch : items)
			{
				ADM_MerchandisePrefab prefabMerch = ADM_MerchandisePrefab.Cast(merch.GetType());
				if (prefabMerch && validItems.Contains(prefabMerch.GetPrefab())) {
					sortedItems.Insert(merch);
				}
			}
		}
		
		if (search != string.Empty)
		{
			sortedItems = ProcessSearch(search, sortedItems);
		}
			
		PopulateTab(wTabView, sortedItems, buyOrSell);
	}
	
	protected void SelectCategoryBuy(string search = "")
	{		
		Print("SelectCategoryBuy Invoked!", LogLevel.NORMAL);
		SelectCategory(m_wBuyTabView, m_iCategoryBuy, m_Shop.GetMerchandiseBuy(), search, true);
	}
	
	protected void SelectCategorySell(string search = "")
	{		
		Print("SelectCategorySell Invoked!", LogLevel.NORMAL);
		SelectCategory(m_wSellTabView, m_iCategorySell, m_Shop.GetMerchandiseSell(), search, false);
	}
	
	protected void ChangeCategoryBuy(SCR_SpinBoxComponent comp, int itemIndex)
	{
		Print("ChangeCategoryBuy Invoked!", LogLevel.NORMAL);
		m_iCategoryBuy = itemIndex - 1;
		SelectCategoryBuy(m_sSearchBuy);
	}
	
	protected void ChangeCategorySell(SCR_SpinBoxComponent comp, int itemIndex)
	{
		Print("ChangeCategorySell Invoked!", LogLevel.NORMAL);
		m_iCategorySell = itemIndex - 1;
		SelectCategorySell(m_sSearchSell);
	}
	
	protected void ConfigureTabs()
	{
		Print("ConfigureTabs Invoked!", LogLevel.NORMAL);
		if (!m_wBuyTabView || !m_wSellTabView || !m_Shop)
			return;
		
		array<ADM_ShopCategory> categories = {};
		foreach (ADM_ShopCategory category : m_Shop.GetCategories())
		{
			categories.Insert(category);
		}
		
		Widget wCategoryBuy = m_wBuyTabView.m_wTab.FindAnyWidget("CategoriesBox");
		SCR_SpinBoxComponent spinBoxComponentBuy = SCR_SpinBoxComponent.Cast(wCategoryBuy.FindHandler(SCR_SpinBoxComponent));
		ConfigureCategory(spinBoxComponentBuy, categories);
		spinBoxComponentBuy.m_OnChanged.Insert(ChangeCategoryBuy);
			
		Widget wCategorySell = m_wSellTabView.m_wTab.FindAnyWidget("CategoriesBox");
		SCR_SpinBoxComponent spinBoxComponentSell = SCR_SpinBoxComponent.Cast(wCategorySell.FindHandler(SCR_SpinBoxComponent));
		ConfigureCategory(spinBoxComponentSell, categories);
		spinBoxComponentSell.m_OnChanged.Insert(ChangeCategorySell);
		
		Widget wSearchBuy = m_wBuyTabView.m_wTab.FindAnyWidget("EditBoxSearch0");
		SCR_EditBoxSearchComponent searchBuy = SCR_EditBoxSearchComponent.Cast(wSearchBuy.FindHandler(SCR_EditBoxSearchComponent));
		if (searchBuy && searchBuy.m_OnConfirm)
		{
			searchBuy.m_OnConfirm.Insert(ProcessSearchBuy);
		}
		
		Widget wSearchSell = m_wSellTabView.m_wTab.FindAnyWidget("EditBoxSearch0");
		SCR_EditBoxSearchComponent searchSell = SCR_EditBoxSearchComponent.Cast(wSearchSell.FindHandler(SCR_EditBoxSearchComponent));
		if (searchSell && searchSell.m_OnConfirm)
		{
			searchSell.m_OnConfirm.Insert(ProcessSearchSell);
		}
		
		SelectCategoryBuy(m_sSearchBuy);
		SelectCategorySell(m_sSearchSell);
	}
	
	protected void ClearTab(SCR_TabViewContent wTabView)
	{
		Print("ClearTab Invoked!", LogLevel.NORMAL);
		if (!wTabView)
			return;
		
		Widget wListbox = wTabView.m_wTab.FindAnyWidget("ListBox0");
		if (wListbox)
		{
			SCR_ListBoxComponent listbox = SCR_ListBoxComponent.Cast(wListbox.FindHandler(SCR_ListBoxComponent));
			if (listbox)
				listbox.Clear();
		}
		
		Widget wListbox1 = wTabView.m_wTab.FindAnyWidget("ListBox1");
		if (wListbox1)
		{
			SCR_ListBoxComponent listbox1 = SCR_ListBoxComponent.Cast(wListbox1.FindHandler(SCR_ListBoxComponent));
			if (listbox1)
				listbox1.Clear();
		}
	}
	
	protected void CreateListboxItem(SCR_ListBoxComponent listbox, ADM_ShopMerchandise merch, bool buyOrSell = true)
	{	
		Print("CreateListboxItem Invoked!", LogLevel.NORMAL);		
		ADM_MerchandiseType merchType = merch.GetType();
		if (!merchType)
			return;
		
		// Item Name
		string itemName = merchType.GetDisplayName();
		if (!itemName)
			itemName = "Item";
			
		int itemIdx = listbox.AddItem(itemName);
		SCR_ListBoxElementComponent lbItem = listbox.GetElementComponent(itemIdx);
		if (!lbItem)
			return;
			
		// Item Preview
		ItemPreviewWidget wItemPreview = ItemPreviewWidget.Cast(lbItem.GetRootWidget().FindAnyWidget("ItemPreview0"));
		if (wItemPreview)
		{
			m_ItemPreviewManager.SetPreviewItemFromPrefab(wItemPreview, merchType.GetDisplayEntity(), null, false);
		}
		
		// On click
		MIKE_GarageShopUI_Item mainBtn = MIKE_GarageShopUI_Item.Cast(lbItem.GetRootWidget().FindHandler(MIKE_GarageShopUI_Item));
		if (mainBtn)
		{
			mainBtn.m_OnClicked.Insert(AddToCart);
		}
			
		// Cost
		TextWidget wItemPrice = TextWidget.Cast(lbItem.GetRootWidget().FindAnyWidget("Cost"));
		array<ref ADM_PaymentMethodBase> paymentRequirement = merch.GetBuyPayment();
		bool isOnlyCurrency = MIKE_GarageComponent.IsBuyPaymentOnlyCurrency(merch);
		if (!buyOrSell)
		{
			paymentRequirement = merch.GetSellPayment(); 
			isOnlyCurrency = MIKE_GarageComponent.IsSellPaymentOnlyCurrency(merch);
		}
		
		if (isOnlyCurrency || paymentRequirement.Count() == 0)
		{
			if (wItemPrice) {
				wItemPrice.SetVisible(true);
				if (paymentRequirement.Count() == 0)
				{
					wItemPrice.SetTextFormat("Free");
				} else {
					ADM_PaymentMethodCurrency paymentMethod = ADM_PaymentMethodCurrency.Cast(paymentRequirement[0]);
					int quantity = paymentMethod.GetQuantity();
					if (quantity > 0)
					{
						wItemPrice.SetTextFormat("$%1", quantity);
					} else {
						wItemPrice.SetTextFormat("Free");
					}
				}
			}
		} else {
			Widget priceParent = lbItem.GetRootWidget().FindAnyWidget("CostOverlay");
			if (wItemPrice)
			{ 
				wItemPrice.SetVisible(false);
			}
			
			for (int i = 0; i < paymentRequirement.Count(); i++)
			{
				Widget icon = GetGame().GetWorkspace().CreateWidgets(m_BarterIconPrefab, priceParent);
				ADM_IconBarterTooltip barterItemIcon = ADM_IconBarterTooltip.Cast(icon.FindHandler(ADM_IconBarterTooltip));
				if (barterItemIcon)
				{
					barterItemIcon.SetPayment(paymentRequirement[i]);
				}
			}
		}
		
		// Quantity
		MIKE_GarageShopUI_Item item = MIKE_GarageShopUI_Item.Cast(lbItem.GetRootWidget().FindHandler(MIKE_GarageShopUI_Item));
		if (item) {
			item.SetShopUI(this);
			item.SetMerchandise(merch);
			item.SetBuyOrSell(buyOrSell);
			item.SetMaxQuantity(merch.GetType().GetMaxPurchaseQuantity());
			item.SetQuantity(1);
		}
	}
	
	protected void PopulateTab(SCR_TabViewContent wTabView, array<ref ADM_ShopMerchandise> merchandise, bool buyOrSell = true)
	{
		Print("PopulateTab Invoked!", LogLevel.NORMAL);
		Widget wListbox = wTabView.m_wTab.FindAnyWidget("ListBox0");
		if (!wListbox)
			return;
		
		SCR_ListBoxComponent listbox = SCR_ListBoxComponent.Cast(wListbox.FindHandler(SCR_ListBoxComponent));
		if (!listbox)
			return;
		
		ClearTab(wTabView);
		foreach (ADM_ShopMerchandise merch : merchandise)
		{
			if (!merch)
				continue;
				
			ADM_MerchandiseType merchType = merch.GetType();
			if (!merchType)
				continue;
			
			CreateListboxItem(listbox, merch, buyOrSell);
		}
		
		TextWidget emptyText = TextWidget.Cast(wTabView.m_wTab.FindAnyWidget("Empty Text"));
		if (merchandise.Count() == 0)
		{
			emptyText.SetVisible(true);
			emptyText.SetText("No items available.");
			emptyText.Update();
		} else {
			emptyText.SetVisible(false);
			emptyText.Update();
		}
	}
	
	protected array<ref ADM_ShopMerchandise> ProcessSearch(string search, array<ref ADM_ShopMerchandise> merchandise)
	{
		Print("ProcessSearch Invoked!", LogLevel.NORMAL);
		search = search.Trim();
		search.ToLower();
		
		if (search.IsEmpty())
		{
			return merchandise;
		}
		
		array<ref ADM_ShopMerchandise> matchedMerchandise = {};
		foreach (ADM_ShopMerchandise merch : merchandise)
		{
			string itemName = merch.GetType().GetDisplayName();
			itemName.ToLower();
			
			if (itemName.Contains(search))
			{
				matchedMerchandise.Insert(merch);
			}
		}
		
		return matchedMerchandise;
	}
	
	protected void ProcessSearchBuy(SCR_EditBoxSearchComponent searchbox, string search)
	{
		Print("ProcessSearchBuy Invoked!", LogLevel.NORMAL);
		m_sSearchBuy = search;
		SelectCategoryBuy(m_sSearchBuy);
	}
	
	protected void ProcessSearchSell(SCR_EditBoxSearchComponent searchbox, string search)
	{
		Print("ProcessSearchSell Invoked!", LogLevel.NORMAL);
		m_sSearchSell = search;
		SelectCategorySell(m_sSearchSell);
	}
	
	private int m_iCachedMoneyCount = 0;
	private float m_fCachedMoneyTime = -5000000;
	protected void UpdateMoneyText()
	{
		Print("UpdateMoneyText Invoked!", LogLevel.NORMAL);
		if (!m_wMoneyText)
			return;
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return;
		
		IEntity player = playerController.GetMainEntity();
		if (!player)
			return;
		
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager)
			return;
		
		float time = System.GetTickCount();
		if (time - m_fCachedMoneyTime > 1000)
		{
			m_iCachedMoneyCount = ADM_CurrencyComponent.FindTotalCurrencyInInventory(inventoryManager);
			m_fCachedMoneyTime = time;
		}
			
		string money = string.Format("$%1", m_iCachedMoneyCount);
		m_wMoneyText.SetText(money);
	}
	
	static MIKE_GarageShopUI_Item FindShopUIItem(Widget w)
	{
		Print("FindShopUIItem Invoked!", LogLevel.NORMAL);
		Widget parent = w.GetParent();
		while (parent)
		{
			MIKE_GarageShopUI_Item uiItem = MIKE_GarageShopUI_Item.Cast(parent.FindHandler(MIKE_GarageShopUI_Item));
			if (uiItem)
				return uiItem;
			else
				parent = parent.GetParent();
		}
		
		return null;
	}
	
	override void OnMenuFocusLost()
	{
		Print("OnMenuFocusLost Invoked!", LogLevel.NORMAL);
		m_bFocused = false;
		
		InputManager inputManager = GetGame().GetInputManager();
		if (inputManager)
		{
			inputManager.RemoveActionListener("MenuOpen", EActionTrigger.DOWN, Close);
			inputManager.RemoveActionListener("MenuBack", EActionTrigger.DOWN, Close);
			#ifdef WORKBENCH
				inputManager.RemoveActionListener("MenuOpenWB", EActionTrigger.DOWN, Close);
				inputManager.RemoveActionListener("MenuBackWB", EActionTrigger.DOWN, Close);
			#endif
		}
	}

	override void OnMenuFocusGained()
	{
		Print("OnMenuFocusGained Invoked!", LogLevel.NORMAL);
		m_bFocused = true;
		
		InputManager inputManager = GetGame().GetInputManager();
		if (inputManager)
		{
			inputManager.AddActionListener("MenuOpen", EActionTrigger.DOWN, Close);
			inputManager.AddActionListener("MenuBack", EActionTrigger.DOWN, Close);
			#ifdef WORKBENCH
				inputManager.AddActionListener("MenuOpenWB", EActionTrigger.DOWN, Close);
				inputManager.AddActionListener("MenuBackWB", EActionTrigger.DOWN, Close);
			#endif
		}
	}
	
	override void HandlerAttached(Widget w)
	{
		Print("HandlerAttached Invoked!", LogLevel.NORMAL);
		super.HandlerAttached(w);
		m_wRoot = w;
		
		m_wBuySellTabWidget = GetRootWidget().FindAnyWidget("BuySellTabView");
		if (!m_wBuySellTabWidget) 
			return;
		
		m_BuySellTabComponent = SCR_TabViewComponent.Cast(m_wBuySellTabWidget.FindHandler(SCR_TabViewComponent));
		if (!m_BuySellTabComponent) 
			return;
		
		if (m_BuySellTabComponent.GetTabCount() == 2)
		{
			m_wBuyTabView = m_BuySellTabComponent.GetEntryContent(0);
			m_wSellTabView = m_BuySellTabComponent.GetEntryContent(1);
		}
		
		m_wCartTabWidget = GetRootWidget().FindAnyWidget("CartTabView");
		if (!m_wCartTabWidget) 
			return;
		
		m_CartTabComponent = SCR_TabViewComponent.Cast(m_wCartTabWidget.FindHandler(SCR_TabViewComponent));
		if (!m_CartTabComponent) 
			return;
		
		if (m_CartTabComponent.GetTabCount() == 1)
		{
			m_wCartTabView = m_CartTabComponent.GetEntryContent(0);
		}	
		
		m_wHeaderText = TextWidget.Cast(m_wRoot.FindAnyWidget("HeaderText"));
		m_wMoneyText = TextWidget.Cast(m_wRoot.FindAnyWidget("TotalMoneyText"));
		
		Widget closeWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("Back"));
		if (closeWidget)
		{
			SCR_InputButtonComponent closeButton = SCR_InputButtonComponent.Cast(closeWidget.FindHandler(SCR_InputButtonComponent));
			if (closeButton)
			{
				closeButton.m_OnActivated.Insert(Close);
			}
		}
		
		Widget checkoutWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("Checkout"));
		if (checkoutWidget)
		{
			SCR_InputButtonComponent checkoutButton = SCR_InputButtonComponent.Cast(checkoutWidget.FindHandler(SCR_InputButtonComponent));
			if (checkoutButton)
			{
				checkoutButton.m_OnActivated.Insert(Checkout);
			}
		}
		
		ConfigureTabs();
		UpdateMoneyText();
		PopulateCartTab(m_wCartTabView);
		
		ChimeraWorld world = ChimeraWorld.CastFrom(GetGame().GetWorld());
		if (world)
		{
			m_ItemPreviewManager = world.GetItemPreviewManager();
		}
	}
}