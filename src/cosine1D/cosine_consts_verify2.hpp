#pragma once
/*******************************************************************
Author: David Wei Ge (dge893@gmail.com, aka Wei Ge)
Created: 09/01/2024
Allrights reserved by David Ge

********************************************************************/

int verifyW()
{
	int ret = 0;
	printf("\r\n verifying W Nmax0=%u, DblIdxCount1=%u\n", Nmax0, DblIdxCount1); std::cout << std::flush;
	finishedIndex = 0;
	th_verify_w thwk(Nmax1);
	Timer_wall timer;
	thwk.run();
	ret = thwk.returnCode();
	double duration = timer.time_secs();
	printf("\n Time used: %g seconds, average time: %g seconds", duration, duration / (double)DblIdxCount1);
	if (ret == 0)
	{
		printf("\nOK W verified.");
	}
	return ret;
}
void th_verify_w::work(unsigned int n, unsigned int k)
{
	if(ret != 0) return;
	if(k==0)
	{
		if(n > finishedIndex) 
		{
			finishedIndex = n;
			printf(" %u", n); fflush(stdout);
		}	
	}
	EMFLOAT v;
	size_t w = dblidx2(n, k);
	unsigned int n2 = 2 * n;
	unsigned int k2 = 2 * k;
	v = ed_factorials[n2+k2];
	v = v / ed_factorials[k2];
	//
	if (v != ed_W[w])
	{
		printf("\r\n W error at h=%u, k=%u", n, k);
		ret = 9;
	}
}

int verifyWs()
{
	int ret = 0;
	printf("\r\n verifying Ws Nmax2=%u, DblIdxCount2=%u\n", Nmax2, DblIdxCount2); std::cout << std::flush;
	finishedIndex = 0;
	Timer_wall timer;
	th_verify_ws thwk(Nmax2);
	//
	thwk.run();
	ret = thwk.returnCode();
	double duration = timer.time_secs();
	printf("\n Time used: %g seconds, average time: %g seconds", duration, duration / (double)DblIdxCount2);
	if (ret == 0)
	{
		printf("\nOK Ws verified.");
	}
	return ret;
}

void th_verify_ws::work(unsigned int n, unsigned int k)
{
	if(ret != 0) return;
	if(k==0)
	{
		if(n > finishedIndex) 
		{
			finishedIndex = n;
			printf(" %u", n); fflush(stdout);
		}	
	}
	//printf("(%u,%u)", n, k); fflush(stdout);
	EMFLOAT v;
	//
	size_t w = dblidx1(n, k);
	v = ed_factorials[n*2];
	v = v / ed_factorials[n-k];
	//
	if (v != ed_Ws[w])
	{
		printf("\r\n Ws error at h=%u, k=%u", n, k);
		ret = 9;
	}
}


////////////////////////////////////////////////////////////////////////////////

int verifyConstants()
{
	int ret = 0;
	ret = loadConstants();
	if(ret == 0)
	{
		ret = verifyW();
	}
	if(ret == 0)
	{
		ret = verifyWs();
	}

	return ret;
}
