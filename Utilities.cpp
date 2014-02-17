#include "Utilities.h"

bool Utilities::compareStringVectors(vector<string> vec1, vector<string> vec2) // GOOD TO GO
{
	bool equalVectors = true;
	if (vec1.size() != vec2.size())
	{
		equalVectors = false;
	}
	else
	{
		for (int i = 0; i < vec1.size(); i++)
		{
			if (vec1[i] != vec2[i])
			{
				equalVectors = false;
			}
		}
	}

	return equalVectors;
}

vector<string> Utilities::appendStringVectors(vector<string> vec1, vector<string> vec2) // GOOD TO GO
{
	for (int i = 0; i< vec2.size(); i++)
	{
		vec1.push_back(vec2[i]);
	}
	return vec1;
}
