/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Created: 02/22/2024
Allrights reserved by David Ge

********************************************************************/
#include <stdio.h>

#include "loopDivide.h"

/*
m=0,1,...,Nmax
*/
void divideSingleLoop(ThreadSingleLoop *twS, unsigned int Nmax, unsigned int threadCount)
{
	unsigned int itemCount = 1;
	if (Nmax + 1 > threadCount)
	{
		itemCount = (Nmax + 1) / threadCount;
	}
	twS[0].m0 = 0; 
	twS[0].m1 = itemCount - 1;
	for (unsigned int th = 1; th < threadCount; th++)
	{
		twS[th].m0 = twS[th - 1].m1 + 1;
		twS[th].m1 = twS[th].m0 + itemCount - 1;
	}
	twS[threadCount - 1].m1 = Nmax;
	if (itemCount == 1)
	{
		for (unsigned int th = Nmax + 1; th < threadCount; th++)
		{
			//make m1<m0 to disable the thread
			twS[th].m0 = 1; 
			twS[th].m1 = 0;
		}
	}
}
/*
m=0,1,...,Nmax 
	n=m,m+1,...,Nmax
	
only divide the outer loop	
*/
void divideSingleDoubleLoop_mN(ThreadSingleLoop *twSd, unsigned int Nmax, unsigned int threadCount)
{
	size_t workCount0 = DOUBLELOOPCOUNT(Nmax);
	size_t workCount = workCount0;
	size_t itemCount = 1;
	if (workCount > threadCount)
	{
		itemCount = workCount / threadCount;
	}
	//
	unsigned int th = 0;
	twSd[0].m0 = 0;
	twSd[0].m1 = 0;
	size_t currentCount = Nmax + 1;
	size_t sum = currentCount;
	for (unsigned int m = 1; m <= Nmax; m++)
	{
		unsigned int nCount = Nmax - m + 1;
		if (nCount + currentCount <= itemCount || th == threadCount - 1)
		{
			//let ths thread do it
			twSd[th].m1 = m;
			currentCount += nCount;
		}
		else
		{
			//let the next thread do it
			th++;
			twSd[th].m0 = m;
			twSd[th].m1 = m;
			currentCount = nCount;
			//re-calculate itemCount
			workCount = workCount0 - sum;
			if (workCount > threadCount)
			{
				itemCount = workCount / (threadCount - th);
			}
			else
			{
				itemCount = 1;
			}
		}
		sum += nCount;
	}
	//disable unused threads
	th++;
	while (th < threadCount)
	{
		twSd[th].m0 = 1;
		twSd[th].m1 = 0;
		th++;
	}
}
/*
m=0,1,...,Nmax
n=0,1,...,m

or
n=m,m-1,...,0

only divide the outer loop
*/
void divideSingleDoubleLoop_0m(ThreadSingleLoop *twS0, unsigned int Nmax, unsigned int threadCount)
{
	size_t workCount0 = DOUBLELOOPCOUNT(Nmax);
	size_t workCount = workCount0;
	size_t itemCount = 1;
	if (workCount > threadCount)
	{
		itemCount = workCount / threadCount;
	}
	//
	unsigned int th = 0;
	twS0[0].m0 = 0;
	twS0[0].m1 = 0;
	size_t currentCount = 1;
	size_t sum = currentCount;
	for (unsigned int m = 1; m <= Nmax; m++)
	{
		unsigned int nCount = m + 1;
		if (nCount + currentCount <= itemCount || th == threadCount - 1)
		{
			//let ths thread do it
			twS0[th].m1 = m;
			currentCount += nCount;
		}
		else
		{
			//let the next thread do it
			th++;
			twS0[th].m0 = m;
			twS0[th].m1 = m;
			currentCount = nCount;
			//re-calculate itemCount
			workCount = workCount0 - sum;
			if (workCount > threadCount)
			{
				itemCount = workCount / (threadCount - th);
			}
			else
			{
				itemCount = 1;
			}
		}
		sum += nCount;
	}
	//disable unused threads
	th++;
	while (th < threadCount)
	{
		twS0[th].m0 = 1;
		twS0[th].m1 = 0;
		th++;
	}
}
/*
m=0,1,...,dim1-1
	s=0,1,...,dim2-1
*/
void divide2DLoop(ThreadDoubleLoop *twD, unsigned int dim1, unsigned int dim2, unsigned int threadCount)
{
	size_t workCount = static_cast<size_t>(dim1) * static_cast<size_t>(dim2);
	size_t itemCount = 1;
	if (workCount > threadCount)
	{
		itemCount = workCount / threadCount;
	}
	size_t sum = 0;
	unsigned int th = 0;
	twD[0].main0 = 0; twD[0].main1 = 0; twD[0].sub0 = 0; twD[0].sub1 = 0;
	size_t currentCount = 0;
	for (unsigned int m = 0; m < dim1; m++)
	{
		for (unsigned int s = 0; s < dim2; s++)
		{
			if (currentCount == 0)
			{
				//the very first one
				currentCount = 1;
				sum = 1;
			}
			else if (currentCount + 1 <= itemCount || th == threadCount - 1)
			{
				//let this thread do it
				twD[th].main1 = m;
				twD[th].sub1 = s;
				currentCount++;
				sum++;
			}
			else
			{
				//let the next thread do it
				th++;
				currentCount = 1;
				sum++;
				twD[th].main0 = m; twD[th].main1 = m; twD[th].sub0 = s; twD[th].sub1 = s;
			}
		}
	}
	if (th < threadCount - 1)
	{
		th++;
		while (th < threadCount)
		{
			twD[th].main0 = 1;
			twD[th].main1 = 0;
			th++;
		}
	}
}
/*
m=0,1,...,Nmax
	s=0,1,...,m
	
divide both loops
*/
void divideDoubleLoop(ThreadDoubleLoop *twD, unsigned int Nmax, unsigned int threadCount)
{
	size_t workCount = DOUBLELOOPCOUNT(Nmax);
	size_t itemCount = 1;
	if (workCount > threadCount)
	{
		itemCount = workCount / threadCount;
	}
	size_t sum = 0;
	unsigned int th = 0;
	twD[0].main0 = 0; twD[0].main1 = 0; twD[0].sub0 = 0; twD[0].sub1 = 0;
	size_t currentCount = 0;
	for (unsigned int m = 0; m <= Nmax; m++)
	{
		for (unsigned int s = 0; s <= m; s++)
		{
			if (currentCount == 0)
			{
				//the very first one
				currentCount = 1;
				sum = 1;
			}
			else if (currentCount + 1 <= itemCount || th == threadCount - 1)
			{
				//let this thread do it
				twD[th].main1 = m;
				twD[th].sub1 = s;
				currentCount++;
				sum++;
			}
			else
			{
				//let the next thread do it
				th++;
				currentCount = 1;
				sum++;
				twD[th].main0 = m; twD[th].main1 = m; twD[th].sub0 = s; twD[th].sub1 = s;
			}
		}
	}
	if (th < threadCount - 1)
	{
		th++;
		while (th < threadCount)
		{
			twD[th].main0 = 1;
			twD[th].main1 = 0;
			th++;
		}
	}
}

/*
m=0,1,...,Nmax
	s=0,1,...,Nmax-m
	
divide both loops
*/
void divideDoubleLoop2(ThreadDoubleLoop *twD, unsigned int Nmax, unsigned int threadCount)
{
	size_t workCount = DOUBLELOOPCOUNT(Nmax);
	size_t itemCount = 1;
	if (workCount > threadCount)
	{
		itemCount = workCount / threadCount;
	}
	size_t sum = 0;
	unsigned int th = 0;
	twD[0].main0 = 0; twD[0].main1 = 0; twD[0].sub0 = 0; twD[0].sub1 = 0;
	size_t currentCount = 0;
	for (unsigned int m = 0; m <= Nmax; m++)
	{
		for (unsigned int s = 0; s <= Nmax-m; s++)
		{
			if (currentCount == 0)
			{
				//the very first one
				currentCount = 1;
				sum = 1;
			}
			else if (currentCount + 1 <= itemCount || th == threadCount - 1)
			{
				//let this thread do it
				twD[th].main1 = m;
				twD[th].sub1 = s;
				currentCount++;
				sum++;
			}
			else
			{
				//let the next thread do it
				th++;
				currentCount = 1;
				sum++;
				twD[th].main0 = m; twD[th].main1 = m; twD[th].sub0 = s; twD[th].sub1 = s;
			}
		}
	}
	if (th < threadCount - 1)
	{
		th++;
		while (th < threadCount)
		{
			twD[th].main0 = 1;
			twD[th].main1 = 0;
			th++;
		}
	}
}


/*
s=0,1,...,Nmax
	m=s,s+1,...,Nmax
	
divide both loops
*/
void divideDoubleLoopSubFirst(ThreadDoubleLoop *twDs, unsigned int Nmax, unsigned int threadCount)
{
	size_t workCount = DOUBLELOOPCOUNT(Nmax);
	size_t itemCount = 1;
	if (workCount > threadCount)
	{
		itemCount = workCount / threadCount;
	}
	size_t sum = 0;
	unsigned int th = 0;
	twDs[0].main0 = 0; twDs[0].main1 = 0; twDs[0].sub0 = 0; twDs[0].sub1 = 0;
	size_t currentCount = 0;
	for (unsigned int s = 0; s <= Nmax; s++)
	{
		for (unsigned int m = s; m <= Nmax; m++)
		{
			if (currentCount == 0)
			{
				//the very first one
				currentCount = 1;
				sum = 1;
			}
			else if (currentCount + 1 <= itemCount || th == threadCount - 1)
			{
				//let this thread do it
				twDs[th].main1 = m;
				twDs[th].sub1 = s;
				currentCount++;
				sum++;
			}
			else
			{
				//let the next thread do it
				th++;
				currentCount = 1;
				sum++;
				twDs[th].main0 = m; twDs[th].main1 = m; twDs[th].sub0 = s; twDs[th].sub1 = s;
			}
		}
	}
	if (th < threadCount - 1)
	{
		th++;
		while (th < threadCount)
		{
			twDs[th].sub0 = 1;
			twDs[th].sub1 = 0;
			th++;
		}
	}

}
/*
m=0,...,Nmax:s=0,...,m
n=0,...,s

divide outer two loops
*/
void divideTripleLoop(ThreadDoubleLoop *twT, unsigned int Nmax, unsigned int threadCount)
{
	size_t workCount = TRIPLELOOPCOUNT(Nmax);
	size_t itemCount = 1;
	if (workCount > threadCount)
	{
		itemCount = workCount / threadCount;
	}
	size_t sum = 0;
	unsigned int th = 0;
	twT[0].main0 = 0; twT[0].main1 = 0; twT[0].sub0 = 0; twT[0].sub1 = 0;
	size_t currentCount = 0;
	for (unsigned int m = 0; m <= Nmax; m++)
	{
		for (unsigned int s = 0; s <= m; s++)
		{
			if (currentCount == 0)
			{
				//the very first one
				currentCount = 1;
				sum = 1;
			}
			else
			{
				unsigned int nCount = s + 1;
				if (currentCount + nCount <= itemCount || th == threadCount - 1)
				{
					//let this thread do it
					twT[th].main1 = m;
					twT[th].sub1 = s;
					currentCount += nCount;
					sum += nCount;
				}
				else
				{
					//let the next thread do it
					th++;
					currentCount = nCount;
					sum += nCount;
					twT[th].main0 = m; twT[th].main1 = m; twT[th].sub0 = s; twT[th].sub1 = s;
				}
			}
		}
	}
	if (th < threadCount - 1)
	{
		th++;
		while (th < threadCount)
		{
			twT[th].main0 = 1;
			twT[th].main1 = 0;
			th++;
		}
	}
}

/*
s=0..N: m=s..N
n=m,...,N

divide outer two loops
*/
void divideTripleLoopSubFirst(ThreadDoubleLoop *twTs, unsigned int Nmax, unsigned int threadCount)
{
	size_t workCount = TRIPLELOOPCOUNT(Nmax);
	size_t itemCount = 1;
	if (workCount > threadCount)
	{
		itemCount = workCount / threadCount;
	}
	size_t sum = 0;
	unsigned int th = 0;
	twTs[0].main0 = 0; twTs[0].main1 = 0; twTs[0].sub0 = 0; twTs[0].sub1 = 0;
	size_t currentCount = 0;
	for (unsigned int s = 0; s <= Nmax; s++)
	{
		for (unsigned int m = s; m <= Nmax; m++)
		{
			unsigned int nCount = Nmax - m + 1;
			if (currentCount == 0)
			{
				//the very first one
				currentCount = nCount;
				sum = nCount;
			}
			else if (currentCount + nCount <= itemCount || th == threadCount - 1)
			{
				//let this thread do it
				twTs[th].main1 = m;
				twTs[th].sub1 = s;
				currentCount += nCount;
				sum += nCount;
			}
			else
			{
				//let the next thread do it
				th++;
				currentCount = nCount;
				sum += nCount;
				twTs[th].main0 = m; twTs[th].main1 = m; twTs[th].sub0 = s; twTs[th].sub1 = s;
			}
		}
	}
	if (th < threadCount - 1)
	{
		th++;
		while (th < threadCount)
		{
			twTs[th].sub0 = 1;
			twTs[th].sub1 = 0;
			th++;
		}
	}

}


#ifdef _DEBUGLOOPDIVIDER
int testCountSingleLoop(ThreadSingleLoop *twS, unsigned int Nmax, unsigned int threadCount)
{
	unsigned int count = 0;
	for (unsigned int th = 0; th < threadCount; th++)
	{
		SINGLELOOPBEGIN(th)
			//execute work(m)
			count++;
		SINGLELOOPEND
	}
	if (count == Nmax + 1)
	{
		printf("\r\nOK. single loop count verified. Nmax=%u, threads: %u", Nmax, threadCount);
		return 0;
	}
	else
	{
		printf("\r\nERROR. single loop count : Nmax+1:%u != count:%u. threads: %u ", Nmax + 1, count, threadCount);
		return 10;
	}
}
int testCountSingleDoubleLoop(ThreadSingleLoop *twSd, unsigned int Nmax, unsigned int threadCount)
{
	size_t count = 0;
	for (unsigned int th = 0; th < threadCount; th++)
	{
		SINGLEDOUBLELOOPBEGIN(th, Nmax)
			//execute work(m, n)
			count++;
		SINGLEDOUBLELOOPEND
	}
	size_t wk = DOUBLELOOPCOUNT(Nmax);
	if (count == wk)
	{
		printf("\r\nOK. single double loop count verified. Nmax=%u, count = %lu, threads: %u", Nmax, wk, threadCount);
		return 0;
	}
	else
	{
		printf("\r\nThreads:%u", threadCount);
		for (unsigned int th = 0; th < threadCount; th++)
		{
			printf("\n%u, (%u, %u)",th, twSd[th].m0, twSd[th].m1);
		}
		printf("\r\nERROR. single double loop count, Nmax=%u, work:%lu != count:%lu. threads: %u ", Nmax, wk, count, threadCount);
		return 10;
	}
}
int testCountSingleDoubleLoop_0m(ThreadSingleLoop *twS0, unsigned int Nmax, unsigned int threadCount)
{
	size_t count = 0;
	for (unsigned int th = 0; th < threadCount; th++)
	{
		SINGLEDOUBLELOOP_0m_BEGIN(th)
			//execute work(m, n)
			count++;
		SINGLEDOUBLELOOP_0m_END
	}
	size_t wk = DOUBLELOOPCOUNT(Nmax);
	if (count == wk)
	{
		printf("\r\nOK. single double 0..m loop count verified. Nmax=%u, count = %lu, threads: %u", Nmax, wk, threadCount);
		return 0;
	}
	else
	{
		printf("\r\nThreads:%u", threadCount);
		for (unsigned int th = 0; th < threadCount; th++)
		{
			printf("\n%u, (%u, %u)", th, twS0[th].m0, twS0[th].m1);
		}
		printf("\r\nERROR. single double 0..m loop count, Nmax=%u, work:%lu != count:%lu. threads: %u ", Nmax, wk, count, threadCount);
		return 10;
	}
}
int testCountSingleDoubleLoop_m0(ThreadSingleLoop *twS0, unsigned int Nmax, unsigned int threadCount)
{
	size_t count = 0;
	for (unsigned int th = 0; th < threadCount; th++)
	{
		SINGLEDOUBLELOOP_m0_BEGIN(th)
			//execute work(m, n)
			count++;
		SINGLEDOUBLELOOP_m0_END
	}
	size_t wk = DOUBLELOOPCOUNT(Nmax);
	if (count == wk)
	{
		printf("\r\nOK. single double m..0 loop count verified. Nmax=%u, count = %lu, threads: %u", Nmax, wk, threadCount);
		return 0;
	}
	else
	{
		printf("\r\nThreads:%u", threadCount);
		for (unsigned int th = 0; th < threadCount; th++)
		{
			printf("\n%u, (%u, %u)", th, twS0[th].m0, twS0[th].m1);
		}
		printf("\r\nERROR. single double m..0 loop count, Nmax=%u, work:%lu != count:%lu. threads: %u ", Nmax, wk, count, threadCount);
		return 10;
	}
}
int testCountDoubleLoop(ThreadDoubleLoop *twD, unsigned int Nmax, unsigned int threadCount)
{
	size_t count = 0;
	for (unsigned int th = 0; th < threadCount; th++)
	{
		DOUBLELOOPBEGIN(th)
			//execute (m,s)
			count++;
		DOUBLELOOPEND
	}
	size_t wk = DOUBLELOOPCOUNT(Nmax);
	if (count == wk)
	{
		printf("\r\nOK. double loop count verified. Nmax=%u. count=%lu. threads: %u", Nmax, wk, threadCount);
		return 0;
	}
	else
	{
		printf("\r\nThreads: %u", threadCount);
		for (unsigned int th = 0; th < threadCount; th++)
		{
			printf("\n%u (m0:%u, m1:%u) - (s0:%u, s1:%u)", th, twD[th].main0, twD[th].main1, twD[th].sub0, twD[th].sub1);
		}
		printf("\r\nERROR. double loop count mismatch. Nmax=%u, expected: %lu != count:%lu ", Nmax, wk, count);
		return 10;
	}
}
int testCountDoubleLoopInverse(ThreadDoubleLoop *twDs, unsigned int Nmax, unsigned int threadCount)
{
	size_t count = 0;
	for (unsigned int th = 0; th < threadCount; th++)
	{
		DOUBLELOOPINVERSEBEGIN(th, Nmax)
			//execute (m,s)
			count++;
		DOUBLELOOPEND
	}
	size_t wk = DOUBLELOOPCOUNT(Nmax);
	if (count == wk)
	{
		printf("\r\nOK. inverse double loop count verified. Nmax=%u. count=%lu. threads: %u", Nmax, wk, threadCount);
		return 0;
	}
	else
	{
		printf("\r\nThreads: %u", threadCount);
		for (unsigned int th = 0; th < threadCount; th++)
		{
			printf("\n%u (s0:%u, s1:%u) - (m0:%u, m1:%u)", th, twDs[th].sub0, twDs[th].sub1, twDs[th].main0, twDs[th].main1);
		}
		printf("\r\nERROR. inverse double loop count mismatch. Nmax=%u, expected: %lu != count:%lu ", Nmax, wk, count);
		return 10;
	}
}
int testCountTripleLoop(ThreadDoubleLoop *twT, unsigned int Nmax, unsigned int threadCount)
{
	size_t count = 0;
	for (unsigned int th = 0; th < threadCount; th++)
	{
		TRIPLELOOPBEGIN(th)
			//execute (m,s, n)
			count++;
		TRIPLELOOPEND
	}
	size_t wk = TRIPLELOOPCOUNT((size_t)Nmax);
	if (count == wk)
	{
		printf("\r\nOK. triple loop count verified. Nmax=%u. count=%lu. threads: %u", Nmax, wk, threadCount);
		return 0;
	}
	else
	{
		printf("\r\nThreads: %u", threadCount);
		for (unsigned int th = 0; th < threadCount; th++)
		{
			printf("\n%u (m0:%u, m1:%u) - (s0:%u, s1:%u)", th, twT[th].main0, twT[th].main1, twT[th].sub0, twT[th].sub1);
		}
		printf("\r\nERROR. triple loop count mismatch. Nmax=%u, expected: %lu != count:%lu ", Nmax, wk, count);
		return 10;
	}
}
int testCountTripleLoopInverse(ThreadDoubleLoop *twTs, unsigned int Nmax, unsigned int threadCount)
{
	size_t count = 0;
	for (unsigned int th = 0; th < threadCount; th++)
	{
		TRIPLELOOPINVERSEBEGIN(th, Nmax)
			//execute (m,s, n)
			count++;
		TRIPLELOOPEND
	}
	size_t wk = TRIPLELOOPCOUNT((size_t)Nmax);
	if (count == wk)
	{
		printf("\r\nOK. inverse triple loop count verified. Nmax=%u. count=%lu. threads = %u", Nmax, wk, threadCount);
		return 0;
	}
	else
	{
		printf("\r\nThreads: %u", threadCount);
		for (unsigned int th = 0; th < threadCount; th++)
		{
			printf("\n%u (s0:%u, s1:%u) - (m0:%u, m1:%u)", th, twTs[th].sub0, twTs[th].sub1, twTs[th].main0, twTs[th].main1);
		}
		printf("\r\nERROR. inverse triple loop count mismatch. Nmax=%u, expected: %lu != count:%lu ", Nmax, wk, count);
		return 10;
	}
}
#endif
