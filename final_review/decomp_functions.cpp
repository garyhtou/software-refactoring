// DECOMPOSITION FUNCTIONS
// before
int orderTotal(int subtotal, bool isPreferredCustomer)
{
	if (isPreferredCustomer)
	{
		if (subtotal > 1000)
		{
			return subtotal * 0.8;
		}
		else if (subtotal > 500)
		{
			return subtotal * 0.85;
		}
		else
		{
			return subtotal * 0.9;
		}
	}
	else
	{
		if (subtotal > 1000)
		{
			return subtotal * 0.9;
		}
		else if (subtotal > 500)
		{
			return subtotal * 0.95;
		}
		else
		{
			return subtotal;
		}
	}
}

// after
int preferredCustomerOrderTotal(int subtotal)
{
	if (subtotal > 1000)
	{
		return subtotal * 0.8;
	}
	else if (subtotal > 500)
	{
		return subtotal * 0.85;
	}
	else
	{
		return subtotal * 0.9;
	}
}

int regularCustomerOrderTotal(int subtotal)
{
	if (subtotal > 1000)
	{
		return subtotal * 0.9;
	}
	else if (subtotal > 500)
	{
		return subtotal * 0.95;
	}
	else
	{
		return subtotal;
	}
}

int orderTotal(int subtotal, bool isPreferredCustomer)
{
	if (isPreferredCustomer)
	{
		return preferredCustomerOrderTotal(subtotal);
	}
	else
	{
		return regularCustomerOrderTotal(subtotal);
	}
}