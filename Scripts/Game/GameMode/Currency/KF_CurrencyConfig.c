[BaseContainerProps(configRoot: true)]
class KF_CurrencyConfig : ScriptAndConfig
{
	[Attribute("", UIWidgets.Object, "")]
	ref array<ref KF_Currency> m_aCurrencies;
	
	//------------------------------------------------------------------------------------------------
	array<ref KF_Currency> GetCurrencies()
	{
		return m_aCurrencies;
	}
}

[BaseContainerProps(), KF_CurrencyTitle()]
class KF_Currency
{
	[Attribute("", UIWidgets.Auto, desc: "Visual properties for currency")]
	ref KF_CurrencyProperties m_CurrencyProperties;
	
	[Attribute("1", UIWidgets.CheckBox, "")]
	bool m_bVirtualCurrency;
	
	[Attribute("", UIWidgets.ResourceNamePicker, "")]
	ResourceName m_sCurrencyPrefab;
	
	[Attribute("", UIWidgets.EditBox, desc: "Starting amount will be assigned when no bank account is found")]
	int m_iStartingAmount;
}

[BaseContainerProps()]
class KF_CurrencyProperties
{
	[Attribute("", UIWidgets.EditBox, desc: "Name of currency. Will be used for buy/sell action")]
	string m_sCurrencyName;
	
	[Attribute("1", UIWidgets.CheckBox, desc: "Display currency name in front of amount (E.g.: [$420] or [420$]")]
	bool m_bPrefix;
	
	[Attribute("1", UIWidgets.CheckBox, desc: "Show balance in players HUD")]
	bool m_bDisplay;
	
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Icon for HUD")]
	ResourceName m_sCurrencyIcon;
}

class KF_CurrencyTitle : BaseContainerCustomTitle
{
	//------------------------------------------------------------------------------------------------
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{
		KF_CurrencyProperties currencyProperties;
		string currencyTitle;
		bool virtual;
		
		source.Get("m_CurrencyProperties", currencyProperties);
		currencyTitle = currencyProperties.m_sCurrencyName;
		source.Get("m_bVirtualCurrency", virtual);
		
		switch (virtual)
		{
			case true:
				title = currencyTitle + " : virtual";
				break;
			
			case false:
				title = currencyTitle + " : cash";
		}
		
		return true;
	}
}