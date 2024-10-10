class KF_Utils
{
	//------------------------------------------------------------------------------------------------
	static string GetTimestamp()
    {
        // Time of day
        int hour;
        int minute;
        int second;
        System.GetHourMinuteSecondUTC(hour, minute, second);

        string sHour = FormatTime(hour);
        string sMinute = FormatTime(minute);
        string sSecond = FormatTime(second);

        string time = "[" + sHour + ":" + sMinute + ":" + sSecond + "] ";

        // Date
        int year;
        int month;
        int day;
        System.GetYearMonthDayUTC(year, month, day);

        string sMonth = FormatTime(month);
        string sDay = FormatTime(day);

        string date = "[" + sDay + "." + sMonth + "." + year + "]";

        return date + time;
    }
	
	//------------------------------------------------------------------------------------------------
	static string FormatTime(int number)
    {
        if (number < 10) return "0" + number.ToString();

        return number.ToString();
    }
	
	//------------------------------------------------------------------------------------------------
	static string FormatNumber(string inputNumber)
    {
        // Check if the input is empty or null
        if (inputNumber.IsEmpty())
        {
            return inputNumber;
        }

        // Remove any leading/trailing whitespace
        inputNumber = inputNumber.Trim();

        // Reverse the integer part to process from the least significant digit
        string reversedIntegerPart = "";
        for (int i = inputNumber.Length() - 1; i >= 0; i--)
        {
            reversedIntegerPart += inputNumber.Get(i);
        }

        // Add periods after every 3 digits
        string formattedReversed = "";
        for (int i = 0; i < reversedIntegerPart.Length(); i++)
        {
            if (i > 0 && i % 3 == 0)
            {
                formattedReversed += ".";
            }
            formattedReversed += reversedIntegerPart.Get(i);
        }

        // Reverse the string back to the correct order
        string formattedIntegerPart = "";
        for (int i = formattedReversed.Length() - 1; i >= 0; i--)
        {
            formattedIntegerPart += formattedReversed.Get(i);
        }

        return formattedIntegerPart;
    }
	
	//------------------------------------------------------------------------------------------------
	static string RemoveFormatting(string formattedNumber)
    {
        // Check if the input is empty or null
        if (formattedNumber.IsEmpty())
        {
            return formattedNumber;
        }

        // Remove any leading/trailing whitespace
        formattedNumber = formattedNumber.Trim();

        // Create a new string without periods
        string cleanNumber = "";
        for (int i = 0; i < formattedNumber.Length(); i++)
        {
            // Only add characters that are not periods
            if (formattedNumber.Get(i) != ".")
            {
                cleanNumber += formattedNumber.Get(i);
            }
        }

        return cleanNumber;
    }
	
	//------------------------------------------------------------------------------------------------
	static float EaseInOutSine(float x)
	{
		return -(Math.Cos(Math.PI * x) - 1) / 2;
	}
	
	//------------------------------------------------------------------------------------------------
	static float EaseInOutCirc(float x)
	{
    	if (x < 0.5)
    	{
        	return (1 - (float)Math.Sqrt(1 - Math.Pow(2 * x, 2))) / 2;
    	}
    	else
    	{
        	return ((float)Math.Sqrt(1 - Math.Pow(-2 * x + 2, 2)) + 1) / 2;
    	}
	}
	
	//------------------------------------------------------------------------------------------------
	static int GetResourceCost(ResourceName resource, IEntity user)
	{
		int cost;
		if (!user) return cost;
		
		SCR_EntityCatalogManagerComponent entityCatalogManager = SCR_EntityCatalogManagerComponent.GetInstance();
		if (!entityCatalogManager) return cost;
		
		FactionAffiliationComponent factionComp = FactionAffiliationComponent.Cast(user.FindComponent(FactionAffiliationComponent));
		if (!factionComp) return cost;
		
		SCR_Faction userFaction = SCR_Faction.Cast(factionComp.GetAffiliatedFaction());
		if (!userFaction) return cost;
		
		SCR_EntityCatalogEntry entry = entityCatalogManager.GetEntryWithPrefabFromFactionCatalog(EEntityCatalogType.ITEM, resource, userFaction);
		if (!entry) return cost;
		
		SCR_ArsenalItem data = SCR_ArsenalItem.Cast(entry.GetEntityDataOfType(SCR_ArsenalItem));
		if (!data) return cost;
		
		cost = data.GetSupplyCost(SCR_EArsenalSupplyCostType.DEFAULT);
		
		return cost;
	}

}