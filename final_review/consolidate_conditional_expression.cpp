// CONSOLIDATE CONDITIONAL EXPRESSION
// before
bool isSpecialDeal(int subtotal)
{
	if (subtotal > 100)
	{
		return true;
	}
	if (subtotal < 10)
	{
		return true;
	}
	return false;
}

// after
bool isSpecialDeal(int subtotal)
{
	return subtotal > 100 || subtotal < 10;
}
