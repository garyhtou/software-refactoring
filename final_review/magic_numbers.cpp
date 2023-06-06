// MAGIC NUMBERS
// before
int orderTotal(float subtotal)
{
	return subtotal * 1.1; // 1.1 is a magic number. It has no meaning
}

// after: replace magic numbers with constants
const int TAX_RATE = 1.1;
int orderTotal(float subtotal)
{
	return subtotal * TAX_RATE;
}
