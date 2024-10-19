#pragma once
/*******************************************************************
Author: David Wei Ge (dge893@gmail.com, aka Wei Ge)
Created: 08/30/2024
Allrights reserved by David Ge

********************************************************************/
int formConstDataFilePathName(char *fdata, const char *fid)
{
	char fname[FILENAME_MAX];
	sprintf_1(fname, FILENAME_MAX, "%s_p%u_N%u.dat", fid, EMDIGITS, Nmax0);
	return formFilePath(fdata, FILENAME_MAX, "../_data/consts", fname);
}
void initFactorials()
{
#if defined(VERIFYCONSTARRAY) || defined(GENERATECONSTARRAY)	
	std::cout << "\n cal factorials ... " << std::flush;
	ed_factorials[0] = 1;
	for(unsigned int n=1;n<Nmax23;n++)
	{
		ed_factorials[n] = ed_factorials[n-1] * n;	
	}
	std::cout << "done" << std::flush;
#endif
}
void initSpeedVector(double v)
{
#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(USETIMEARRAYS)		
	ed_V[0] = 1;
	for(unsigned int h=1;h<Nmax22;h++)
	{
		ed_V[h] = ed_V[h-1] * v;
		ed_V[h] = ed_V[h] / h;	
	}
	std::cout << "\n speed vector calculated" << std::flush;
#endif	
}
void initFactors()
{
#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(CREATETIMEARRAYS)	
	for(unsigned int n=0;n<Nmax1;n++)
	{
		unsigned int n2 = n * 2;
		ed_Qq[n] = 1;
		ed_Qq[n] = ed_Qq[n]/(n2+2);
		ed_Qq[n] = ed_Qq[n]/(n2+1);	
	}
#endif	
#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(USETIMEARRAYS)	
	ed_Wp[0] = 1; ed_Wq[0] = 1;
	for(unsigned int n=0;n<Nmax1;n++)
	{
		unsigned int n2 = n * 2;
		ed_Wp[n] = 1; ed_Wp[n] = ed_Wp[n] / n2; ed_Wp[n] = ed_Wp[n] / (n2+1);
		ed_Wq[n] = 1; ed_Wq[n] = ed_Wq[n] / n2; ed_Wq[n] = ed_Wq[n] / (n2-1);
	}
	unsigned int n2 = Nmax1 * 2;
	ed_Wq[Nmax1] = 1; ed_Wq[Nmax1] = ed_Wq[Nmax1] / n2; ed_Wq[Nmax1] = ed_Wq[Nmax1] / (n2-1);
#endif	
}
#if defined(VERIFYCONSTARRAY) || defined(ONEPOINT) || defined(RUNSIMULATION) || defined(CREATETIMEARRAYS)
int loadW()
{
	int ret = 0;
	printf("\r\n loading W Nmax0=%u, DblIdxCount1=%u", Nmax0, DblIdxCount1); std::cout << std::flush;
	char fdata[FILENAME_MAX];
	ret = formConstDataFilePathName(fdata, "w");
	if (ret == 0)
	{
		printf("\n W file: %s", fdata);
		size_t sz = DblIdxCount1 * sizeof(EMFLOAT);
		size_t szf = 0;
		ed_W = (EMFLOAT *)ReadFileIntoMemory(fdata, &szf, &ret);
		if (ret == 0)
		{
			if (szf != sz)
			{
				printf("\r\n W file size mismatch. file size = %lu, expected size = %lu", szf, sz);
				ret = 9;
			}
		}
	}
	if (ret == 0)
	{
		printf("\nOK W loaded");
	}
	return ret;
}
#endif
#if defined(VERIFYCONSTARRAY) || defined(ONEPOINT) || defined(RUNSIMULATION) || defined(USETIMEARRAYS)
int loadWs()
{
	int ret = 0;
	printf("\r\n loading Ws Nmax0=%u, DblIdxCount2=%u", Nmax0, DblIdxCount2); std::cout << std::flush;
	char fdata[FILENAME_MAX];
	ret = formConstDataFilePathName(fdata, "ws");
	if (ret == 0)
	{
		printf("\n Ws file: %s", fdata);
		size_t sz = DblIdxCount2 * sizeof(EMFLOAT);
		size_t szf = 0;
		ed_Ws = (EMFLOAT *)ReadFileIntoMemory(fdata, &szf, &ret);
		if (ret == 0)
		{
			if (szf != sz)
			{
				printf("\r\n Ws file size mismatch. file size = %lu, expected size = %lu", szf, sz);
				ret = 9;
			}
		}
	}
	if (ret == 0)
	{
		printf("\nOK Ws loaded");
	}
	return ret;
}
#endif
int loadConstants()
{
	int ret = 0;
#if defined(VERIFYCONSTARRAY) || defined(ONEPOINT) || defined(RUNSIMULATION) || defined(CREATETIMEARRAYS)
	ret = loadW();
#endif
	if(ret == 0)
	{
#if defined(VERIFYCONSTARRAY) || defined(ONEPOINT) || defined(RUNSIMULATION) || defined(USETIMEARRAYS)		
		ret = loadWs();
#endif
	}
	return ret;
}
