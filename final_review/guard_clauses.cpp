// GUARD CLAUSES
// before
void mystery(bool a, bool b)
{
	if (a)
	{
		if (b)
		{
			cout << "do something" << endl;
		}
	}
}

// after
void mystery(bool a, bool b)
{
	if (!a || !b)
		return;

	cout << "do something" << endl;
}
