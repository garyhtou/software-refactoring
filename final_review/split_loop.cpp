// SPLIT LOOP
// before
int neg = 0;
int pos = 0;
for (int i = 0; i < n; i++)
{
	if (nums[i] < 0)
	{
		neg++;
	}
	if (nums[i] > 0)
	{
		pos++;
	}
}

// after
int neg = 0;
for (int i = 0; i < n; i++)
{
	if (nums[i] < 0)
	{
		neg++;
	}
}

int pos = 0;
for (int i = 0; i < n; i++)
{
	if (nums[i] > 0)
	{
		pos++;
	}
}
