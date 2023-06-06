// INLINE METHOD
// before
int shippingCost(int subtotal)
{
	return subtotal > 1000 ? 0 : 10;
}

int total(int subtotal)
{
	return subtotal + shippingCost(subtotal);
}

// after
int total(int subtotal)
{
	return subtotal + (subtotal > 1000 ? 0 : 10);
}
