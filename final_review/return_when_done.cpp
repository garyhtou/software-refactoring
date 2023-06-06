// RETURN WHEN DONE
// before
int shippingCost(int subtotal)
{
	int shipping;
	if (subtotal > 1000)
	{
		shipping = 0;
	}
	else if (subtotal > 500)
	{
		shipping = 5;
	}
	else
	{
		shipping = 10;
	}
	return shipping;
}

// after
int shippingCost(int subtotal)
{
	if (subtotal > 1000)
	{
		return 0;
	}
	else if (subtotal > 500)
	{
		return 5;
	}
	else
	{
		return 10;
	}
}