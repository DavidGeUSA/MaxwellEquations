#pragma once
/*******************************************************************
Author: David Wei Ge (dge893@gmail.com, aka Wei Ge)
Created: 08/30/2024
Allrights reserved by David Ge

********************************************************************/

/*
indexers for triangle arrays
*/

/*
	for pnk and qnk
*/
size_t dblIndexHeader1[Nmax2];
//pnk: n=0,1,...,Nmax0; k=0,1,...,n
//qnk: n=0,1,...,Nmax1; k=0,1,...,n
#define dblidx1(n, k)  (dblIndexHeader1[(n)]+(k))

/*
	for W
*/
size_t dblIndexHeader2[Nmax1];
//n=0,1,...,Nmax0; k=0,1,...,Nmax0-n
#define dblidx2(n, k)  (dblIndexHeader2[(n)]+(k))

/*
	for U
*/
size_t dblIndexHeader3[Nmax1];
//n=0,1,...,Nmax0; k=n,n+1,...,Nmax0
#define dblidx3(n, k)  (dblIndexHeader3[(n)]+(k)-(n))

int initIndexHeaders()
{
	size_t dblCount1 = (Nmax1 * (Nmax1 + 1)) / 2;
	size_t dblCount2 = (Nmax2 * (Nmax2 + 1)) / 2;
	if(DblIdxCount1 != dblCount1)
	{
		std::cout << "\n error: DblIdxCount1 != (Nmax1 * (Nmax1 + 1)) / 2 \n Nmax1=" << Nmax1 << " DblIdxCount1=" << DblIdxCount1 << std::flush;
		return 9;
	}
	if(DblIdxCount2 != dblCount2)
	{
		std::cout << "\n error: DblIdxCount2 != (Nmax2 * (Nmax2 + 1)) / 2 \n Nmax2=" << Nmax2 << " DblIdxCount2=" << DblIdxCount2 << std::flush;
		return 9;
	}
	std::cout << "\n Nmax0=" << Nmax0 << " DblIdxCount1=" << DblIdxCount1 << " DblIdxCount2=" << DblIdxCount2 << std::flush;
	size_t w = 0;
	for(unsigned int n=0;n<Nmax2;n++)
	{
		dblIndexHeader1[n] = w;
		for(unsigned int k=0;k<=n;k++)
		{
			w++;
		}
	}
	w = 0;
	for(unsigned int n=0;n<Nmax1;n++)
	{
		dblIndexHeader2[n] = w;
		for(unsigned int k=0;k<Nmax1 - n;k++)
		{
			w++;
		}
	}
	w = 0;
	for(unsigned int n=0;n<Nmax1;n++)
	{
		dblIndexHeader3[n] = w;
		for(unsigned int k=n;k<Nmax1;k++)
		{
			w++;
		}
	}
	//verify
	int ret = 0;
	w = 0;
	for(unsigned int n=0;n<Nmax2;n++)
	{
		for(unsigned int k=0;k<=n;k++)
		{
			if(dblidx1(n,k) != w)
			{
				ret = 9;
				std::cout << "\n type 1 error at n=" << n << " k=" << k << " dblidx1=" << dblidx1(n,k) << " w = " << w;
				break;
			}
			w++;
		}
		if(ret != 0) break;
	}
	if(ret == 0)
	{
		if(w != DblIdxCount2)
		{
			std::cout << " type 1: count != DblIdxCount2 : count=" << w << ", DblIdxCount2=" << DblIdxCount2;
			ret = 9;  
		}
	}
	if(ret == 0)
	{
		w = 0;
		for(unsigned int n=0;n<Nmax1;n++)
		{
			for(unsigned int k=0;k<Nmax1 - n;k++)
			{
				if(dblidx2(n,k) != w)
				{
					ret = 9;
					std::cout << "\n type 2 error at n=" << n << " k=" << k << " dblidx2=" << dblidx2(n,k) << " w = " << w;
					break;
				}

				w++;
			}
			if(ret != 0) break;
		}
		if(ret == 0)
		{
			if(w != DblIdxCount1)
			{
				std::cout << " type 2: count != DblIdxCount1 : count=" << w << ", DblIdxCount1=" << DblIdxCount1;
				ret = 9;  
			}
		}
	}
	if(ret == 0)
	{
		w = 0;
		for(unsigned int n=0;n<Nmax1;n++)
		{
			for(unsigned int k=n;k<Nmax1;k++)
			{
				if(dblidx3(n,k) != w)
				{
					ret = 9;
					std::cout << "\n type 3 error at n=" << n << " k=" << k << " dblidx3=" << dblidx3(n,k) << " w = " << w;
					break;
				}

				w++;
			}
			if(ret != 0) break;
		}
		if(ret == 0)
		{
			if(w != DblIdxCount1)
			{
				std::cout << " type 3: count != DblIdxCount1 : count=" << w << ", DblIdxCount1=" << DblIdxCount1;
				ret = 9;  
			}
		}
	}
	if(ret == 0)
	{
		std::cout << "\n OK initialized index headers" << std::flush;
	}
	return ret;
}
