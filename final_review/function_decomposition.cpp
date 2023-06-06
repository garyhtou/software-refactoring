// FUNCTION DECOMPOSITION
// before
const int TAX_RATE = 1.1;
int pay(Order order)
{
	// calculate total
	int subtotal = 0;
	for (int i = 0; i < order.count; i++)
	{
		subtotal += order.items[i].price;
	}

	// calculate shipping
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

	// calculate discount
	int discount;
	if (order.count > 5)
	{
		discount = 10;
	}
	else
	{
		discount = 0;
	}

	// calculate grand total
	return (subtotal * TAX_RATE) + shipping - discount;
}

// after
const int TAX_RATE = 1.1;
int calculateSubtotal(Order order)
{
	int subtotal = 0;
	for (int i = 0; i < order.count; i++)
	{
		subtotal += order.items[i].price;
	}
	return subtotal;
}

int calculateShipping(int subtotal)
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

int calculateDiscount(Order order)
{
	int discount;
	if (order.count > 5)
	{
		discount = 10;
	}
	else
	{
		discount = 0;
	}
	return discount;
}

int pay(Order order)
{
	int subtotal = calculateSubtotal(order);
	int shipping = calculateShipping(subtotal);
	int discount = calculateDiscount(order);
	return (subtotal * TAX_RATE) + shipping - discount;
}