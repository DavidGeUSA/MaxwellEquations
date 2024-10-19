#pragma once
/*******************************************************************
Author: David Wei Ge (dge893@gmail.com, aka Wei Ge)
Created: 08/30/2024
Allrights reserved by David Ge

********************************************************************/

/*
	(2n+2k)!/(2k)!
*/
int generateW()
{
	int ret = 0;
	printf("\r\n generate W Nmax0=%u, DblIdxCount=%u\n", Nmax0, DblIdxCount1); std::cout << std::flush;
	char fdata[FILENAME_MAX];
	ret = formConstDataFilePathName(fdata, "w");
	if (ret == 0)
	{
		if(fileexists(fdata))
		{
			printf("\n\n warning: file already exists: %s\n\n",fdata);
			//return ERR_FILE_OPEN_WRIT_EEXIST;
			return 0;
		}
		size_t sz = DblIdxCount1 * sizeof(EMFLOAT);
		ed_W = (EMFLOAT *)CreateFileIntoMemory(fdata, sz, &ret);
	}
	if (ret == 0)
	{
		finishedIndex = 0;
		th_gen_w thwk(Nmax1);
		Timer_wall timer;
		thwk.run();
		ret = thwk.returnCode();
		FreeMemory(ed_W); ed_W = nullptr;
		double duration = timer.time_secs();
		printf("\n Time used: %g seconds, average time: %g seconds", duration, duration / (double)DblIdxCount1);

	}
	if (ret == 0)
	{
		printf("\nOK W generated");
	}
	return ret;
}
void th_gen_w::work(unsigned int n, unsigned int k)
{
	if(k == 0 && n > 400)
	{
		if(n > finishedIndex) 
		{
			finishedIndex = n;
			printf(" %u ", n); fflush(stdout);
		}	
	}
	size_t w = dblidx2(n, k);
	unsigned int n2 = 2 * n;
	unsigned int k2 = 2 * k;
	ed_W[w] = ed_factorials[n2+k2] / ed_factorials[k2];
}

/*
	(2n)!/(n-k)!
*/
int generateWs()
{
	int ret = 0;
	printf("\r\n generate Ws Nmax0=%u, DblIdxCount2=%u\n", Nmax0, DblIdxCount2); std::cout << std::flush;
	char fdata[FILENAME_MAX];
	ret = formConstDataFilePathName(fdata, "ws");
	if (ret == 0)
	{
		if(fileexists(fdata))
		{
			printf("\n\n warning: file already exists: %s\n\n",fdata);
			//return ERR_FILE_OPEN_WRIT_EEXIST;
			return 0;
		}
		size_t sz = DblIdxCount2 * sizeof(EMFLOAT);
		ed_Ws = (EMFLOAT *)CreateFileIntoMemory(fdata, sz, &ret);
	}
	if (ret == 0)
	{
		finishedIndex = 0;
		th_gen_ws thwk(Nmax2);
		Timer_wall timer;
		thwk.run();
		ret = thwk.returnCode();
		FreeMemory(ed_Ws); ed_Ws = nullptr;
		double duration = timer.time_secs();
		printf("\n Time used: %g seconds, average time: %g seconds", duration, duration / (double)DblIdxCount2);

	}
	if (ret == 0)
	{
		printf("\nOK Ws generated");
	}
	return ret;
}
void th_gen_ws::work(unsigned int n, unsigned int k)
{
	if(k == 0 && n > 400)
	{
		if(n > finishedIndex) 
		{
			finishedIndex = n;
			printf(" %u ", n); fflush(stdout);
		}	
	}
	size_t w = dblidx1(n, k);
	ed_Ws[w] = ed_factorials[n * 2] / ed_factorials[n-k];
}

////////////////////////////////////////////////////////////////////////////////
int generateConstants()
{
	int ret = 0;
	if(ret == 0)
	{
		ret = generateW();
	}
	if(ret == 0)
	{
		ret = generateWs();
	}
	if(ret == 0)
	{
		puts("\n OK. constants generated"); fflush(stdout);
	}
	return ret;
}
