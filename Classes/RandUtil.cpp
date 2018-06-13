#include "RandUtil.h"

#include <stdlib.h>
#include <time.h>

RandUtil::RandUtil()
{
}


RandUtil::~RandUtil()
{
}

int * RandUtil::rand_N(int n)
{
	int *pn1 = new int[n + 1];
	int *pn2 = new int[n];
	int i, tmp;
	srand(time(0));
	for (i = 0; i <= n; i++)
	{
		pn1[i] = 0;
	}
	for (i = 0; i < n; i++)
	{
		tmp = rand() % n + 1;
		if (pn1[tmp] == 0)
		{
			pn2[i] = tmp;
			pn1[tmp] = -1;
		}
		else
			--i;
	}
	delete[]pn1;
	return pn2;
}
