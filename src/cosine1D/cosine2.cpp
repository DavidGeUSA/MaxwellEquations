
/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Created: 08/28/2024
Allrights reserved by David Ge

********************************************************************/
/*
calculate electromagnetic fields of moving cosine source with Gaussian decay. See cosineV2.doc
use stationary source and moving source, in rest frame and in moving frame
*/

#include <iostream>
#include <cstdio>
#include <ctime>
#include <stdio.h>

#include "cosine2.h"
#include "cosine_th2.h"
#include "timer.hpp"
#include "../FileUtil/fileutil.h"
#include "../FileUtil/taskFile.h"
#include "../MemoryMan/MemoryManager.h"

/*
set parameters and configurations
hard-coded for coding simplicity
*/
void initParam(DataSetting& param)
{
	param.dataFileId = 4;
	param.dataDir = "../_data/gcosine";
	param.outDir = "../_data/";
	param.tmpDir = "../_data/tmp";
	//
	param.v = 0.006; //earth speed
	//param.v = 0.003;
	//param.v = 1.0;
	//-simulation ------------
	param.ls = 8;
	param.lt = 2;
	//
	param.startSpaceStep = 0;
	param.spaceCount = 300;   //startTimeStep * (ls / lt)
	//
	param.startTimeStep = 45;
	param.timeCount = 1;

}
unsigned int DblIdxCount1 = 0; // = (Nmax1 * (Nmax1 + 1)) / 2, triangle array item count for Nmax1
unsigned int DblIdxCount2 = 0; // = (Nmax2 * (Nmax2 + 1)) / 2, triangle array item count for Nmax2

MemoryManager mem("../_data/tmp");
MemoryManager *_mem = &mem;
// threading objects
#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(CREATETIMEARRAYS)
th_gen_U*  thwkU  = nullptr; //Nmax1
EMFLOAT ed_th_var[MAXTHREADCOUNT]; //variable to be used in threads 
#endif
#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(USETIMEARRAYS)	
th_gen_Dp* thwkDp = nullptr; //Nmax1
th_gen_Dq* thwkDq = nullptr; //Nmax2
th_gen_B*  thwkB  = nullptr; //Nmax1
th_gen_A0* thwkA0 = nullptr; //Nmax1
th_gen_A1* thwkA1 = nullptr; //Nmax1
#endif

#include "cosine_indexer.hpp"
const double maxField = 1.0e+3;
unsigned int finishedIndex = 0;
//constants
#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(USETIMEARRAYS)
EMFLOAT ed_V[Nmax22]; //v^h / h!
EMFLOAT ed_Wq[Nmax1]; // 1/ (2n)(2n-1)
EMFLOAT ed_Wp[Nmax1]; // 1/ (2n)(2n+1)
#endif

#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(CREATETIMEARRAYS)
EMFLOAT ed_Qq[Nmax1]; // 1/ (2n+2)(2n+1)
#endif

#if defined(GENERATECONSTARRAY) || defined(VERIFYCONSTARRAY)
EMFLOAT ed_factorials[Nmax23]; //factorials
#endif

#if defined(GENERATECONSTARRAY) || defined(VERIFYCONSTARRAY) || defined(ONEPOINT) || defined(RUNSIMULATION) || defined(CREATETIMEARRAYS)
EMFLOAT *ed_W   = nullptr; //DblIdxCount1
#endif
EMFLOAT *ed_Ws  = nullptr; //DblIdxCount2

#include "cosine_consts2.hpp"
#ifdef GENERATECONSTARRAY
#include "cosine_consts_gen2.hpp"
#endif
#ifdef VERIFYCONSTARRAY
#include "cosine_consts_verify2.hpp"
#endif

#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(CREATETIMEARRAYS)
EMFLOAT ed_Q[Nmax1];
#endif

EMFLOAT* ed_Uh0 = nullptr; //DblIdxCount1
EMFLOAT* ed_Uh1 = nullptr; //DblIdxCount1
EMFLOAT* ed_Ue1 = nullptr; //DblIdxCount1
EMFLOAT* ed_Ue0 = nullptr; //DblIdxCount1
int allocateTimeArrays()
{
	int ret = 0;
	size_t sz = DblIdxCount1 * sizeof(EMFLOAT);
	if(ret == 0)
	{
		ed_Uh0 = (EMFLOAT*)AllocateMemory(sz);
		if(ed_Uh0 == nullptr)
		{
			ret = ERR_OUTOFMEMORY;
		}		
	}
	if(ret == 0)
	{
		ed_Uh1 = (EMFLOAT*)AllocateMemory(sz);
		if(ed_Uh1 == nullptr)
		{
			ret = ERR_OUTOFMEMORY;
		}
	}
	if(ret == 0)
	{
		ed_Ue1 = (EMFLOAT*)AllocateMemory(sz);
		if(ed_Ue1 == nullptr)
		{
			ret = ERR_OUTOFMEMORY;
		}		
	}
	if(ret == 0)
	{
		ed_Ue0 = (EMFLOAT*)AllocateMemory(sz);
		if(ed_Ue0 == nullptr)
		{
			ret = ERR_OUTOFMEMORY;
		}		
	}	
	return ret;
}
void deAllocateTimeArrays()
{
	if(ed_Uh0 != nullptr)
	{
		FreeMemory(ed_Uh0); ed_Uh0 = nullptr;
	}
	if(ed_Uh1 != nullptr)
	{
		FreeMemory(ed_Uh1); ed_Uh1 = nullptr;
	}
	if(ed_Ue1 != nullptr)
	{
		FreeMemory(ed_Ue1); ed_Ue1 = nullptr;
	}
	if(ed_Ue0 != nullptr)
	{
		FreeMemory(ed_Ue0); ed_Ue0 = nullptr;
	}
}

////time step///////////////////////////
EMFLOAT ed_cta;  //q/l0ct
EMFLOAT ed_cta2; //cta^2
EMFLOAT ed_cta3; //cta^3
EMFLOAT ed_q02;  //-cta^2

void onTimeStep()
{
	ed_cta2 = ed_cta * ed_cta;
	ed_cta3 = ed_cta * ed_cta2;
	ed_q02  = -ed_cta2 ;
}
#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(CREATETIMEARRAYS)
void atTimeStep()
{
	onTimeStep();
	//
	ed_Q[0] = ed_Qq[0];
	for(unsigned int n=1;n<Nmax1;n++)
	{
		ed_Q[n] = ed_q02 * ed_Qq[n] * ed_Q[n-1];
	}
	std::cout << " U... " << std::flush;
	finishedIndex = 0;
	thwkU->run();
}

//loop: h=0,...,Nmax0:m=h,...,Nmax0
void th_gen_U::work(THREADINDEX th, unsigned int h, unsigned int m)
{
	if(m == h)
	{
		if( (h % 100) == 0)
		{
			std::cout << th << ":" << h << " " << std::flush;
		}
	}
	size_t w = dblidx3(h,m);
	ed_Uh0[w] = 0; ed_Uh1[w] = 0; ed_Ue1[w] = 0; ed_Ue0[w] = 0;
	for(unsigned int k=0;k<=Nmax0-m;k++)
	{
		size_t w2 = dblidx2(h,k);
		unsigned int r = 2 * (k + m + 1);
		unsigned int rh = 2 * (k + h) + 1;
		ed_th_var[th] = ed_Q[m+k] * ed_W[w2]; //only one high-precision multiplication
		//
		ed_Uh0[w] += ed_th_var[th];
		ed_Ue1[w] += rh * ed_th_var[th];
		ed_Ue0[w] += r * ed_th_var[th];
		//
		ed_th_var[th] = (ed_th_var[th] * rh) / (r+1);
		ed_Uh1[w] += ed_th_var[th];

	}
}
#endif
////////space step///////////////////////////////////////////////
#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(USETIMEARRAYS)
EMFLOAT ed_rho;  //2s / l0s
EMFLOAT ed_rho2; //rho^2
EMFLOAT ed_s02;  //-rho^2
EMFLOAT ed_Sp[Nmax1];
EMFLOAT ed_Sq[Nmax2];

EMFLOAT ed_exp_a_x2; //exp(-rho^2/4)
EMFLOAT ed_Dp[Nmax1];
EMFLOAT ed_Dq[Nmax2];
///factors to calculate fields
EMFLOAT ed_imp; //sqrt(mu/eps)
EMFLOAT ed_fh0; //cta2 * rho * exp
EMFLOAT ed_fh1; //-cta3 * exp
EMFLOAT ed_fe1; //-imp*cta2 * rho * exp
EMFLOAT ed_fe0; //-imp*cta * exp

void onSpaceStep()
{
	ed_rho2 = ed_rho * ed_rho;
	ed_s02  = -ed_rho2;
	ed_exp_a_x2 = exp(ed_s02 / 4);
	//
	ed_fh0 =  ed_cta2 * ed_rho * ed_exp_a_x2;
	ed_fh1 = -ed_cta3 * ed_exp_a_x2;
	ed_fe1 = -ed_imp * ed_cta2 * ed_rho * ed_exp_a_x2;
	ed_fe0 = -ed_imp * ed_cta * ed_exp_a_x2;
	//
	ed_Sq[0] = 1; ed_Sp[0] = 1;
	for(unsigned int k=1;k<Nmax1;k++)
	{
		ed_Sp[k] = ed_s02 * ed_Wp[k] * ed_Sp[k-1];
	}
	for(unsigned int k=1;k<Nmax2;k++)
	{
		ed_Sq[k] = ed_s02 * ed_Wq[k] * ed_Sq[k-1];
	}
}

EMFLOAT ed_var;
EMFLOAT ed_Bh0[Nmax1];
EMFLOAT ed_Bh1[Nmax1];
EMFLOAT ed_Be1[Nmax1];
EMFLOAT ed_Be0[Nmax1];

EMFLOAT ed_Ah0;
EMFLOAT ed_Ah1;
EMFLOAT ed_Ae1;
EMFLOAT ed_Ae0;

void calculateDpDq()
{
	onSpaceStep();
	//
	thwkDp->run();
	//
	thwkDq->run();
}
void th_gen_Dp::work(unsigned int n)
{
	ed_Dp[n] = 0;
	for(unsigned int k=0;k<=n;k++)
	{
		size_t w = dblidx1(n,k);
		ed_Dp[n] += ed_Sp[k] * ed_Ws[w];
	}
	ed_Dp[n] = ed_Dp[n] * (2*n + 1);
}
void th_gen_Dq::work(unsigned int n)
{
	ed_Dq[n] = 0;
	for(unsigned int k=0;k<=n;k++)
	{
		size_t w = dblidx1(n,k);
		ed_Dq[n] += ed_Sq[k] * ed_Ws[w];
	}
}

void calculateB()
{
	calculateDpDq();
	thwkB->run();
}
void th_gen_B::work(unsigned int h)
{
	ed_Bh0[h] = 0; ed_Bh1[h] = 0; ed_Be1[h] = 0; ed_Be0[h] = 0;
	for(unsigned int m=h;m<Nmax1;m++)
	{
		size_t w = dblidx3(h,m);
		//
		ed_Bh0[h] += ed_Dp[m] * ed_Uh0[w];
		ed_Be0[h] += ed_Dq[m] * ed_Ue0[w];
		//
		ed_Bh1[h] += ed_Dq[m+1] * ed_Uh1[w];
		ed_Be1[h] += ed_Dp[m] * ed_Ue1[w];
	}
}
void atSpaceStepStationary()
{
	calculateDpDq();
	ed_Bh0[0] = 0; ed_Be0[0] = 0;
	for(unsigned int m=0;m<Nmax1;m++)
	{
		size_t w = dblidx3(0,m);
		ed_Bh0[0] += ed_Dp[m] * ed_Uh0[w];
		ed_Be0[0] += ed_Dq[m] * ed_Ue0[w];
	}
}
void atSpaceStepMoving()
{
	calculateB();
	//Dp and Dq no longer needed, reuse them
	thwkA0->run();
	ed_Ah0 = 0; ed_Ae0 = 0;
	for(unsigned int h=0;h<Nmax1;h++)
	{
		ed_Ah0 += ed_Dp[h];
		ed_Ae0 += ed_Dq[h];
	}
	thwkA1->run();
	ed_Ah1 = 0; ed_Ae1 = 0;
	for(unsigned int h=0;h<Nmax1;h++)
	{
		ed_Ah1 += ed_Dp[h];
		ed_Ae1 += ed_Dq[h];
	}
}
void th_gen_A0::work(unsigned int h)
{
	unsigned int h2 = 2 * h;
	ed_Dp[h] = ed_V[h2] * ed_Bh0[h];
	ed_Dq[h] = ed_V[h2] * ed_Be0[h];
}
void th_gen_A1::work(unsigned int h)
{
	unsigned int h2 = 2 * h + 1;
	ed_Dp[h] = ed_V[h2] * ed_Bh1[h];
	ed_Dq[h] = ed_V[h2] * ed_Be1[h];
}
#endif
///////////////////////////////////////////////////
#ifdef RUNSIMULATION
int formDataFilePathName(char *fdata, const char *fid, DataSetting &param, bool includeSpeed)
{
	char ltname[NUMSTRSIZE];
	char lsname[NUMSTRSIZE];
	char fname[FILENAME_MAX];
	//
	doubleToIntegersString(param.lt, ltname, NUMSTRSIZE);
	doubleToIntegersString(param.ls, lsname, NUMSTRSIZE);
	if(includeSpeed)
	{
		char vname[NUMSTRSIZE];
		doubleToIntegersString(param.v, vname, NUMSTRSIZE);
		sprintf_1(fname, FILENAME_MAX, "cos%s_v%s_lt%s_ls%s_t0%u_s0%u_p%u_N%u_rows%u_cols%u_hSpace_%d.dat", 
			fid, vname, ltname, lsname, param.startTimeStep, param.startSpaceStep, EMDIGITS, Nmax0, param.timeCount, param.spaceCount, param.dataFileId);
	}
	else
	{
		sprintf_1(fname, FILENAME_MAX, "cos%s_lt%s_ls%s_t0%u_s0%u_p%u_N%u_rows%u_cols%u_hSpace_%d.dat", 
			fid, ltname, lsname, param.startTimeStep, param.startSpaceStep, EMDIGITS, Nmax0, param.timeCount, param.spaceCount, param.dataFileId);
	}
	return formFilePath(fdata, FILENAME_MAX, param.dataDir, fname);
}

int simulate(DataSetting &param)
{
	int ret = loadConstants();
	if(ret != 0) return ret;
	FILE *fhHyS_R = NULL; //stationary source. in resting frame, or in moving frame as an inertial field
	FILE *fhEzS_R = NULL; //stationary source. in resting frame, or in moving frame as an inertial field
	FILE *fhHyS_M = NULL; //stationary source in moving-frame
	FILE *fhEzS_M = NULL; //stationary source in moving-frame	
	FILE *fhHyM_M = NULL; //moving source in moving-frame
	FILE *fhEzM_M = NULL; //moving source in moving-frame	
	{
		char fdata[FILENAME_MAX];
		if (ret == 0)
		{
			ret = formDataFilePathName(fdata, "MinM_Ez", param, true);
			if (ret == 0)
			{
				printf("\r\n Moving source in Moving Frame, Ez Data file:%s", fdata);
				ret = openfileWrite(fdata, &fhEzM_M);
			}
		}
		if (ret == 0)
		{
			ret = formDataFilePathName(fdata, "MinM_Hy", param, true);
			if (ret == 0)
			{
				printf("\r\n Moving source in Moving Frame, Hy Data file:%s", fdata);
				ret = openfileWrite(fdata, &fhHyM_M);
			}
		}
		if (ret == 0)
		{
			ret = formDataFilePathName(fdata, "SinM_Ez", param, true);
			if (ret == 0)
			{
				printf("\r\n Stationary source in Moving Frame, Ez Data file:%s", fdata);
				ret = openfileWrite(fdata, &fhEzS_M);
			}
		}
		if (ret == 0)
		{
			ret = formDataFilePathName(fdata, "SinM_Hy", param, true);
			if (ret == 0)
			{
				printf("\r\n Stationary source in Moving Frame, Hy Data file:%s", fdata);
				ret = openfileWrite(fdata, &fhHyS_M);
			}
		}
		if (ret == 0)
		{
			ret = formDataFilePathName(fdata, "SinR_Hy", param, false);
			if (ret == 0)
			{
				printf("\r\n Stationary source in Rest Frame, Hy Data file:%s", fdata);
				ret = openfileWrite(fdata, &fhHyS_R);
			}
		}
		if (ret == 0)
		{
			ret = formDataFilePathName(fdata, "SinR_Ez", param, false);
			if (ret == 0)
			{
				printf("\r\n Stationary source in Rest Frame, Ez Data file:%s", fdata);
				ret = openfileWrite(fdata, &fhEzS_R);
			}
		}
	}
	if(ret == 0)
	{
		std::cout << "\nStart simulation. time step range: ["<<param.startTimeStep<<", "<<param.endStepExclude_time()<< ") space step range: ["<<param.startSpaceStep<<", "<<param.endStepExclude_space() << ")";
		std::cout << " lt=" << param.lt << " ls=" << param.ls << " v=" << param.v;		
		Timer_wall timer;
		double secs;
		double totalTime = 0;
		double var;
		for(unsigned int q = param.startTimeStep; q < param.endStepExclude_time(); q++)
		{
			ed_cta = q; ed_cta = ed_cta / param.lt;
			std::cout << "\nq:" << q << " cta:" << FLOATEMTODBL(ed_cta) << " Q..." << std::flush;
			atTimeStep();
			secs = timer.time_secs(); timer.reset();
			totalTime += secs;
			std::cout << secs << " s: \n";
			for(unsigned int s = param.startSpaceStep; s < param.endStepExclude_space(); s++)
			{
				std::cout << s << " " << std::flush;
				ed_rho = s;
				ed_rho = 2 * ed_rho; ed_rho = ed_rho / param.ls;
				atSpaceStepStationary();
				//save to S_R				
				ed_var = ed_fh0 * ed_Bh0[0];
				var = FLOATEMTODBL(ed_var);
				fwrite(&var, sizeof(double), 1, fhHyS_R);
				ed_var = ed_fe0 * ed_Be0[0];
				var = FLOATEMTODBL(ed_var);
				fwrite(&var, sizeof(double), 1, fhEzS_R);
				if(abs(var) > maxField)
				{
					std::cout << "\n field overflow:" << var << " at q=" << q << " s=" << s << std::endl;
					ret = 5;
					break;
				}
				//				
				ed_rho = param.v;
				ed_rho = ed_rho * q; ed_rho = ed_rho + s;
				ed_rho = 2 * ed_rho; ed_rho = ed_rho / param.ls;			
				atSpaceStepMoving();
				//
				//save to S_M
				ed_var = ed_fh0 * ed_Bh0[0];
				var = FLOATEMTODBL(ed_var);
				fwrite(&var, sizeof(double), 1, fhHyS_M);
				ed_var = ed_fe0 * ed_Be0[0];
				var = FLOATEMTODBL(ed_var);
				fwrite(&var, sizeof(double), 1, fhEzS_M);
				//
				//save to M_M
				ed_var = ed_fh0 * ed_Ah0 + ed_fh1 * ed_Ah1;
				var = FLOATEMTODBL(ed_var);
				fwrite(&var, sizeof(double), 1, fhHyM_M);
				ed_var = ed_fe0 * ed_Ae0 + ed_fe1 * ed_Ae1;
				var = FLOATEMTODBL(ed_var);
				fwrite(&var, sizeof(double), 1, fhEzM_M);
				//
				if(ret != 0)
				{
					break;
				}
			}
			secs = timer.time_secs(); timer.reset();
			totalTime += secs;
			std::cout << "\n space time: " << secs << "  average one step time: " << secs / (double)(param.spaceCount);
		}
		std::cout << "\n Time used: " << totalTime << " seconds, " << totalTime / 60.0 << " minutes, " << totalTime / 3600.0 << " hours"; 
	}
	if (fhHyS_R != NULL)
	{
		closefile(fhHyS_R);
	}
	if (fhEzS_R != NULL)
	{
		closefile(fhEzS_R);
	}
	if (fhHyM_M != NULL)
	{
		closefile(fhHyM_M);
	}
	if (fhEzM_M != NULL)
	{
		closefile(fhEzM_M);
	}
	if (fhHyS_M != NULL)
	{
		closefile(fhHyS_M);
	}
	if (fhEzS_M != NULL)
	{
		closefile(fhEzS_M);
	}
	return ret;
}
#endif
#ifdef ONEPOINT
int OnePoint(unsigned int q, unsigned int s, DataSetting &param)
{
	int ret = loadConstants();
	if(ret == 0)
	{
		Timer_wall timer;
		double secs;
		double totalTime = 0;
		double var;
		ed_cta = q; ed_cta = ed_cta / param.lt;
		std::cout << "\nq:" << q << " cta:" << FLOATEMTODBL(ed_cta) << " Q..." << std::flush;
		atTimeStep();
		secs = timer.time_secs(); timer.reset();
		totalTime += secs;
		std::cout << secs << " seconds \ns: " << s << std::flush;
		//
		ed_rho = s;
		ed_rho = 2 * ed_rho; ed_rho = ed_rho / param.ls;
		atSpaceStepStationary();
		//save to S_R				
		ed_var = ed_fh0 * ed_Bh0[0];
		var = FLOATEMTODBL(ed_var);
		std::cout << "\n HySR: " <<	var;
		ed_var = ed_fe0 * ed_Be0[0];
		var = FLOATEMTODBL(ed_var);
		std::cout << " EzSR: " << var << std::flush;
		//				
		ed_rho = param.v;
		ed_rho = ed_rho * q; ed_rho = ed_rho + s;
		ed_rho = 2 * ed_rho; ed_rho = ed_rho / param.ls;			
		atSpaceStepMoving();
		//
		//save to S_M
		ed_var = ed_fh0 * ed_Bh0[0];
		var = FLOATEMTODBL(ed_var);
		std::cout << "\n HySM: " << var;
		ed_var = ed_fe0 * ed_Be0[0];
		var = FLOATEMTODBL(ed_var);
		std::cout << " EzSM: " << var;
		//
		//save to M_M
		ed_var = ed_fh0 * ed_Ah0 + ed_fh1 * ed_Ah1;
		var = FLOATEMTODBL(ed_var);
		std::cout << "\n HyMM: " << var;
		ed_var = ed_fe0 * ed_Ae0 + ed_fe1 * ed_Ae1;
		var = FLOATEMTODBL(ed_var);
		std::cout << " EzMM: " << var;
		//
		secs = timer.time_secs(); timer.reset();
		totalTime += secs;
		std::cout << "\n space time: " << secs;
		//
		std::cout << "\n Time used: " << totalTime << " seconds, " << totalTime / 60.0 << " minutes, " << totalTime / 3600.0 << " hours";
	}
	return ret;
}
#endif
#if defined(CREATETIMEARRAYS) || defined(USETIMEARRAYS)
int formTimeArrayFilePathName(char *fdata, const char *fid, DataSetting& param)
{
	char ltname[NUMSTRSIZE];
	char fname[FILENAME_MAX];
	//
	doubleToIntegersString(param.lt, ltname, NUMSTRSIZE);
	//
	sprintf_1(fname, FILENAME_MAX, "%s_lt%s_q%u_p%u_N%u.dat", 
			fid, ltname, param.startTimeStep, EMDIGITS, Nmax0);
	return formFilePath(fdata, FILENAME_MAX, param.dataDir, fname);
}
#endif
#ifdef CREATETIMEARRAYS
int createTimeArrayMemory(DataSetting& param)
{
	int ret = 0;
	deAllocateTimeArrays();
	size_t sz = DblIdxCount1 * sizeof(EMFLOAT);
	char fdata[FILENAME_MAX];
	if (ret == 0)
	{
		ret = formTimeArrayFilePathName(fdata, "Uh0", param);
		if (ret == 0)
		{
			printf("\r\n Data file for Uh0 : %s", fdata);
			ed_Uh0 = (EMFLOAT*)CreateFileIntoMemory(fdata, sz, &ret);
			if(ret == 0)
			{
				if(ed_Uh0 == nullptr)
				{
					ret = ERR_OUTOFMEMORY;
				}
			}	
		}
	}
	if (ret == 0)
	{
		ret = formTimeArrayFilePathName(fdata, "Uh1", param);
		if (ret == 0)
		{
			printf("\r\n Data file for Uh1 : %s", fdata);
			ed_Uh1 = (EMFLOAT*)CreateFileIntoMemory(fdata, sz, &ret);
			if(ret == 0)
			{
				if(ed_Uh1 == nullptr)
				{
					ret = ERR_OUTOFMEMORY;
				}
			}	
		}
	}
	if (ret == 0)
	{
		ret = formTimeArrayFilePathName(fdata, "Ue1", param);
		if (ret == 0)
		{
			printf("\r\n Data file for Ue1 : %s", fdata);
			ed_Ue1 = (EMFLOAT*)CreateFileIntoMemory(fdata, sz, &ret);
			if(ret == 0)
			{
				if(ed_Ue1 == nullptr)
				{
					ret = ERR_OUTOFMEMORY;
				}
			}	
		}
	}
	if (ret == 0)
	{
		ret = formTimeArrayFilePathName(fdata, "Ue0", param);
		if (ret == 0)
		{
			printf("\r\n Data file for Ue0 : %s", fdata);
			ed_Ue0 = (EMFLOAT*)CreateFileIntoMemory(fdata, sz, &ret);
			if(ret == 0)
			{
				if(ed_Ue0 == nullptr)
				{
					ret = ERR_OUTOFMEMORY;
				}
			}	
		}
	}

	return ret;
}
/*
	use param.startTimeStep to calculate Uh0, Uh1, Ue1, Ue0
*/
int createTimeArrays(DataSetting& param)
{
	int ret = loadConstants();
	if(ret == 0)
	{
		ret = createTimeArrayMemory(param);
	}
	if(ret == 0)
	{
		unsigned int q = param.startTimeStep;
		Timer_wall timer;
		ed_cta = q; ed_cta = ed_cta / param.lt;
		std::cout << "\nq:" << q << " cta:" << FLOATEMTODBL(ed_cta) << " Q..." << std::flush;
		atTimeStep();
		double totalTime = timer.time_secs();
		std::cout << "\n Time used: " << totalTime << " seconds, " << totalTime / 60.0 << " minutes, " << totalTime / 3600.0 << " hours";
	}
	return ret;
}
#endif
#if defined(DATAEMFLOATTODOUBLE) || defined(USETIMEARRAYS)
EMFLOAT* ed_dataHy = nullptr;
EMFLOAT* ed_dataEz = nullptr;
int formFieldFilePathName(char *fdata, const char *fid, DataSetting &param)
{
	char ltname[NUMSTRSIZE];
	char lsname[NUMSTRSIZE];
	char fname[FILENAME_MAX];
	//
	doubleToIntegersString(param.lt, ltname, NUMSTRSIZE);
	doubleToIntegersString(param.ls, lsname, NUMSTRSIZE);
	char vname[NUMSTRSIZE];
	doubleToIntegersString(param.v, vname, NUMSTRSIZE);
	sprintf_1(fname, FILENAME_MAX, "cos%s_v%s_lt%s_ls%s_q%u_s0%u_p%u_N%u_rows3_cols%u_hSpace_%d.dat", 
			fid, vname, ltname, lsname, param.startTimeStep, param.startSpaceStep, EMDIGITS, Nmax0, param.spaceCount, param.dataFileId);
	return formFilePath(fdata, FILENAME_MAX, param.dataDir, fname);
}
void closeFieldFiles()
{
	if(ed_dataHy != nullptr)
	{
		FreeMemory(ed_dataHy); ed_dataHy = nullptr;
	}
	if(ed_dataEz != nullptr)
	{
		FreeMemory(ed_dataEz); ed_dataEz = nullptr;
	}
}
#endif
#ifdef USETIMEARRAYS
int loadTimeArrays(DataSetting& param)
{
	int ret = 0;
	deAllocateTimeArrays();
	size_t fz = 0;
	size_t sz = DblIdxCount1 * sizeof(EMFLOAT);
	char fdata[FILENAME_MAX];
	if (ret == 0)
	{
		ret = formTimeArrayFilePathName(fdata, "Uh0", param);
		if (ret == 0)
		{
			printf("\nUh0: %s", fdata);
			ed_Uh0 = (EMFLOAT*)ReadFileIntoMemory(fdata, &fz, &ret);
			if(ret == 0)
			{
				if(fz != sz)
				{
					ret = ERR_FILESIZE_MISMATCH;
				}
			}
		}
	}
	if (ret == 0)
	{
		ret = formTimeArrayFilePathName(fdata, "Uh1", param);
		if (ret == 0)
		{
			printf("\nUh1: %s", fdata);
			ed_Uh1 = (EMFLOAT*)ReadFileIntoMemory(fdata, &fz, &ret);
			if(ret == 0)
			{
				if(fz != sz)
				{
					ret = ERR_FILESIZE_MISMATCH;
				}
			}
		}
	}
	if (ret == 0)
	{
		ret = formTimeArrayFilePathName(fdata, "Ue1", param);
		if (ret == 0)
		{
			printf("\nUe1: %s", fdata);
			ed_Ue1 = (EMFLOAT*)ReadFileIntoMemory(fdata, &fz, &ret);
			if(ret == 0)
			{
				if(fz != sz)
				{
					ret = ERR_FILESIZE_MISMATCH;
				}
			}
		}
	}
	if (ret == 0)
	{
		ret = formTimeArrayFilePathName(fdata, "Ue0", param);
		if (ret == 0)
		{
			printf("\nUe0: %s", fdata);
			ed_Ue0 = (EMFLOAT*)ReadFileIntoMemory(fdata, &fz, &ret);
			if(ret == 0)
			{
				if(fz != sz)
				{
					ret = ERR_FILESIZE_MISMATCH;
				}
			}
		}
	}
	return ret;
}

int openFieldFiles(DataSetting& param)
{
	int ret = 0;
	size_t sz = sizeof(EMFLOAT) * 3 * param.spaceCount;
	char fdata[FILENAME_MAX];
	if (ret == 0)
	{
		ret = formFieldFilePathName(fdata, "gmpHy", param);
		if (ret == 0)
		{
			printf("\r\n Field file for Hy : %s", fdata);
			ed_dataHy = (EMFLOAT*)CreateFileIntoMemory(fdata, sz, &ret);
			if(ret == 0)
			{
				if(ed_dataHy == nullptr)
				{
					ret = ERR_OUTOFMEMORY;
				}
			}	
		}
	}
	if (ret == 0)
	{
		ret = formFieldFilePathName(fdata, "gmpEz", param);
		if (ret == 0)
		{
			printf("\r\n Field file for Ez : %s", fdata);
			ed_dataEz = (EMFLOAT*)CreateFileIntoMemory(fdata, sz, &ret);
			if(ret == 0)
			{
				if(ed_dataEz == nullptr)
				{
					ret = ERR_OUTOFMEMORY;
				}
			}	
		}
	}
	return ret;
}
#ifdef EXAMINETIMEARRAYS
int examineTimeArrays(DataSetting& param)
{
	int ret = loadConstants();
	if(ret == 0)
	{
		ret = loadTimeArrays(param);
	}
	if(ret == 0)
	{
		
	}
	return ret;
}
#endif
int useTimeArrays(DataSetting& param)
{
	double totalTime = 0;
	int ret = loadConstants();
	if(ret == 0)
	{
		ret = loadTimeArrays(param);
	}
	if(ret == 0)
	{
		ret = openFieldFiles(param);
	}
	if(ret == 0)
	{
		size_t wSR = 0;
		size_t wSM = param.spaceCount;
		size_t wMM = wSM + param.spaceCount; 
		double secs = 0;
		unsigned int q = param.startTimeStep;
		Timer_wall timer;
		ed_cta = q; ed_cta = ed_cta / param.lt;
		std::cout << "\nq:" << q << " cta:" << FLOATEMTODBL(ed_cta) << " Q..." << std::flush;
		std::cout << secs << " s: \n";
		onTimeStep();
		for(unsigned int s = param.startSpaceStep; s < param.endStepExclude_space(); s++)
		{
			std::cout << s << " " << std::flush;
			ed_rho = s;
			ed_rho = 2 * ed_rho; ed_rho = ed_rho / param.ls;
			atSpaceStepStationary();
			//save to S_R				
			ed_dataHy[wSR] = ed_fh0 * ed_Bh0[0];
			ed_dataEz[wSR] = ed_fe0 * ed_Be0[0];
			if(wSR == 0)
			{
				if(abs(FLOATEMTODBL(ed_dataEz[wSR])) > maxField)
				{
					std::cout << "\n field overflow:" << FLOATEMTODBL(ed_dataEz[wSR]) << " at q=" << q << " s=" << s << std::endl;
					ret = 5;
					break;
				}
			}
			//				
			ed_rho = param.v;
			ed_rho = ed_rho * q; ed_rho = ed_rho + s;
			ed_rho = 2 * ed_rho; ed_rho = ed_rho / param.ls;			
			atSpaceStepMoving();
			//
			//save to S_M
			ed_dataHy[wSM] = ed_fh0 * ed_Bh0[0];
			ed_dataEz[wSM] = ed_fe0 * ed_Be0[0];
			//
			//save to M_M
			ed_dataHy[wMM] = ed_fh0 * ed_Ah0 + ed_fh1 * ed_Ah1;
			ed_dataEz[wMM] = ed_fe0 * ed_Ae0 + ed_fe1 * ed_Ae1;
			//
			if(ret != 0)
			{
				break;
			}
			wSR++;
			wSM++;
			wMM++;
		}
		secs = timer.time_secs(); timer.reset();
		totalTime += secs;
		std::cout << "\n space time: " << secs << "  average one step time: " << secs / (double)(param.spaceCount);
	}
	closeFieldFiles();
	std::cout << "\n Time used: " << totalTime << " seconds, " << totalTime / 60.0 << " minutes, " << totalTime / 3600.0 << " hours"; 
	return ret;
}
#endif
#ifdef DATAEMFLOATTODOUBLE
#ifdef GENERATECOMPAREFILES
EMFLOAT * ed_data = nullptr;
/*
compare = "SRSM" or "SRMM"
*/
int formCompareFilePathName(char *fdata, const char *fid, const char *compare, DataSetting &param)
{
	char ltname[NUMSTRSIZE];
	char lsname[NUMSTRSIZE];
	char fname[FILENAME_MAX];
	//
	doubleToIntegersString(param.lt, ltname, NUMSTRSIZE);
	doubleToIntegersString(param.ls, lsname, NUMSTRSIZE);
	char vname[NUMSTRSIZE];
	doubleToIntegersString(param.v, vname, NUMSTRSIZE);
	sprintf_1(fname, FILENAME_MAX, "cos%s_%s_v%s_lt%s_ls%s_q%u_s0%u_p%u_N%u_rows2_cols%u_hSpace_%d.dat", 
			fid, compare, vname, ltname, lsname, param.startTimeStep, param.startSpaceStep, EMDIGITS, Nmax0, param.spaceCount, param.dataFileId);
	return formFilePath(fdata, FILENAME_MAX, param.dataDir, fname);
}
/*
	EHsrc = "gmpHy" or "gmpEz"
	EHtgt = "Hy" or "Ez"
*/
int separateDataFilesEH(DataSetting& param, const char* EHsrc, const char* EHtgt)
{
	int ret = 0;
	size_t fz = 0;
	size_t sz = sizeof(EMFLOAT) * 3 * param.spaceCount;
	char fdata[FILENAME_MAX];
	if (ret == 0)
	{
		ret = formFieldFilePathName(fdata, EHsrc, param);
		if (ret == 0)
		{
			printf("\r\n Load field file for %s : %s", EHsrc, fdata);
			ed_data = (EMFLOAT*)ReadFileIntoMemory(fdata, &fz, &ret);
			if(ret == 0)
			{
				if(fz != sz)
				{
					ret = ERR_FILESIZE_MISMATCH;
				}
			}	
		}
	}
	if(ret == 0)
	{
		double* dataSRSM = nullptr;
		double* dataSRMM = nullptr;
		size_t szDbl = sizeof(double) * 2 * param.spaceCount;
		ret = formCompareFilePathName(fdata, EHtgt, "SRSM", param);
		if (ret == 0)
		{
			printf("\r\n generate compare file SRSM for %s : %s", EHtgt, fdata);
			dataSRSM = (double*)CreateFileIntoMemory(fdata, szDbl, &ret);
		}
		if (ret == 0)
		{
			ret = formCompareFilePathName(fdata, EHtgt, "SRMM", param);
			printf("\r\n generate compare file SRMM for %s : %s", EHtgt, fdata);
			dataSRMM = (double*)CreateFileIntoMemory(fdata, szDbl, &ret);
		}
		if (ret == 0)
		{
			for(unsigned int c=0;c<param.spaceCount;c++)
			{
				double var = FLOATEMTODBL(ed_data[c]);
				dataSRSM[c] = var;
				dataSRMM[c] = var;
			}
			size_t w1 = param.spaceCount;
			size_t w2 = param.spaceCount * 2;
			for(unsigned int c=0;c<param.spaceCount;c++)
			{
				double var = FLOATEMTODBL(ed_data[w1 + c]);
				dataSRSM[w1 + c] = var;
				var = FLOATEMTODBL(ed_data[w2 + c]);
				dataSRMM[w1 + c] = var;
			}
		}
		if(dataSRSM != nullptr)
		{
			FreeMemory(dataSRSM); dataSRSM = nullptr;
		}
		if(dataSRMM != nullptr)
		{
			FreeMemory(dataSRMM); dataSRMM = nullptr;
		}
	}
	if(ed_data != nullptr)
	{
		FreeMemory(ed_data); ed_data = nullptr;
	}
	return ret;
}
int generateCompareFiles(DataSetting& param)
{
	int ret = 0;
	ret = separateDataFilesEH(param, "gmpHy", "Hy");
	if(ret == 0)
	{
		ret = separateDataFilesEH(param, "gmpEz", "Ez");
	}
	return ret;
}
#else
#ifdef GENERATEDIFFFILES
EMFLOAT * ed_data = nullptr;
/*
for  "Hy" or "Ez"
*/
int formDiffFilePathName(char *fdata, const char *fid, DataSetting &param)
{
	char ltname[NUMSTRSIZE];
	char lsname[NUMSTRSIZE];
	char fname[FILENAME_MAX];
	//
	doubleToIntegersString(param.lt, ltname, NUMSTRSIZE);
	doubleToIntegersString(param.ls, lsname, NUMSTRSIZE);
	char vname[NUMSTRSIZE];
	doubleToIntegersString(param.v, vname, NUMSTRSIZE);
	sprintf_1(fname, FILENAME_MAX, "cos%s_DIFF_v%s_lt%s_ls%s_q%u_s0%u_p%u_N%u_rows2_cols%u_hSpace_%d.dat", 
			fid, vname, ltname, lsname, param.startTimeStep, param.startSpaceStep, EMDIGITS, Nmax0, param.spaceCount, param.dataFileId);
	return formFilePath(fdata, FILENAME_MAX, param.dataDir, fname);
}
/*
	EHsrc = "gmpHy" or "gmpEz"
*/
int makeDiffFile(DataSetting& param, const char* EHsrc)
{
	int ret = 0;
	size_t fz = 0;
	size_t sz = sizeof(EMFLOAT) * 3 * param.spaceCount;
	char fdata[FILENAME_MAX];
	if (ret == 0)
	{
		ret = formFieldFilePathName(fdata, EHsrc, param);
		if (ret == 0)
		{
			printf("\r\n Load field file for %s : %s", EHsrc, fdata);
			ed_data = (EMFLOAT*)ReadFileIntoMemory(fdata, &fz, &ret);
			if(ret == 0)
			{
				if(fz != sz)
				{
					ret = ERR_FILESIZE_MISMATCH;
				}
			}	
		}
	}
	if(ret == 0)
	{
		double* data = nullptr;
		size_t szDbl = sizeof(double) * 2 * param.spaceCount;
		ret = formDiffFilePathName(fdata, EHsrc, param);
		if (ret == 0)
		{
			printf("\r\n generate diff file for %s : %s", EHsrc, fdata);
			data = (double*)CreateFileIntoMemory(fdata, szDbl, &ret);
		}
		if (ret == 0)
		{
			size_t w1 = param.spaceCount;
			size_t w2 = param.spaceCount * 2;
			for(unsigned int c=0;c<param.spaceCount;c++)
			{
				double varSR = FLOATEMTODBL(ed_data[c]);
				double varSM = FLOATEMTODBL(ed_data[w1 + c]);
				double varMM = FLOATEMTODBL(ed_data[w2 + c]);
				data[c]      = varSM - varSR;
				data[w1 + c] = varMM - varSR;
			}
		}
		if(data != nullptr)
		{
			FreeMemory(data); data = nullptr;
		}
	}
	if(ed_data != nullptr)
	{
		FreeMemory(ed_data); ed_data = nullptr;
	}
	return ret;
}

int generateDiffFiles(DataSetting& param)
{
	int ret = 0;
	ret = makeDiffFile(param, "gmpHy");
	if(ret == 0)
	{
		ret = makeDiffFile(param, "gmpEz");
	}
	return ret;
}
#else
#ifdef GENERATEDIFFFILEVS
EMFLOAT * ed_data_v1 = nullptr;
EMFLOAT * ed_data_v2 = nullptr;
EMFLOAT ed_diffSM;
EMFLOAT ed_diffMM;
/*
for  "Hy" or "Ez"
*/
int formDiffFilePathNameVS(char *fdata, const char *fid, DataSetting &param, double v1, double v2)
{
	char ltname[NUMSTRSIZE];
	char lsname[NUMSTRSIZE];
	char fname[FILENAME_MAX];
	char v1name[NUMSTRSIZE];
	char v2name[NUMSTRSIZE];
	//
	doubleToIntegersString(param.lt, ltname, NUMSTRSIZE);
	doubleToIntegersString(param.ls, lsname, NUMSTRSIZE);
	doubleToIntegersString(v1, v1name, NUMSTRSIZE);
	doubleToIntegersString(v2, v2name, NUMSTRSIZE);
	sprintf_1(fname, FILENAME_MAX, "cos%s_DIFF_v1_%s_v2_%s_lt%s_ls%s_q%u_s0%u_p%u_N%u_rows2_cols%u_hSpace_%d.dat", 
			fid, v1name, v2name, ltname, lsname, param.startTimeStep, param.startSpaceStep, EMDIGITS, Nmax0, param.spaceCount, param.dataFileId);
	return formFilePath(fdata, FILENAME_MAX, param.dataDir, fname);
}
/*
	EHsrc = "gmpHy" or "gmpEz"
*/
int makeDiffFileVelocities(DataSetting& param, double v1, double v2, const char* EHsrc)
{
	int ret = 0;
	size_t fz = 0;
	size_t sz = sizeof(EMFLOAT) * 3 * param.spaceCount;
	char fdata[FILENAME_MAX];
	if (ret == 0)
	{
		param.v = v1;
		ret = formFieldFilePathName(fdata, EHsrc, param);
		if (ret == 0)
		{
			printf("\r\n Load v1 field file for %s : %s", EHsrc, fdata);
			ed_data_v1 = (EMFLOAT*)ReadFileIntoMemory(fdata, &fz, &ret);
			if(ret == 0)
			{
				if(fz != sz)
				{
					ret = ERR_FILESIZE_MISMATCH;
				}
			}	
		}
	}
	if (ret == 0)
	{
		param.v = v2;
		ret = formFieldFilePathName(fdata, EHsrc, param);
		if (ret == 0)
		{
			printf("\r\n Load v2 field file for %s : %s", EHsrc, fdata);
			ed_data_v2 = (EMFLOAT*)ReadFileIntoMemory(fdata, &fz, &ret);
			if(ret == 0)
			{
				if(fz != sz)
				{
					ret = ERR_FILESIZE_MISMATCH;
				}
			}	
		}
	}
	if(ret == 0)
	{
		double* data = nullptr;
		size_t szDbl = sizeof(double) * 2 * param.spaceCount;
		ret = formDiffFilePathNameVS(fdata, EHsrc, param, v1, v2);
		if (ret == 0)
		{
			printf("\r\n generate velocity diff file for %s : %s", EHsrc, fdata);
			data = (double*)CreateFileIntoMemory(fdata, szDbl, &ret);
		}
		if (ret == 0)
		{
			size_t w1 = param.spaceCount;
			size_t w2 = param.spaceCount * 2;
			for(unsigned int c=0;c<param.spaceCount;c++)
			{
				ed_diffSM = ed_data_v1[w1 + c] - ed_data_v2[w1 + c];
				ed_diffMM = ed_data_v1[w2 + c] - ed_data_v2[w2 + c];
				data[c]      = FLOATEMTODBL(ed_diffSM);
				data[w1 + c] = FLOATEMTODBL(ed_diffMM);
			}
		}
		if(data != nullptr)
		{
			FreeMemory(data); data = nullptr;
		}
	}
	if(ed_data_v1 != nullptr)
	{
		FreeMemory(ed_data_v1); ed_data_v1 = nullptr;
	}
	if(ed_data_v2 != nullptr)
	{
		FreeMemory(ed_data_v2); ed_data_v2 = nullptr;
	}
	return ret;
}
int generateDiffFileVelocities(DataSetting& param, double v1, double v2)
{
	int ret = 0;
	ret = makeDiffFileVelocities(param, v1, v2, "gmpHy");
	if(ret == 0)
	{
		ret = makeDiffFileVelocities(param, v1, v2, "gmpEz");
	}
	return ret;
}
#else
#ifdef MERGESPACE
EMFLOAT * ed_data_s0 = nullptr;
EMFLOAT * ed_data_s1 = nullptr;
EMFLOAT * ed_data_s2 = nullptr;
int mergeFilesBySpace(DataSetting& param1, const char* EHsrc)
{
	int ret = 0;
	DataSetting param0;
	DataSetting param2;
	size_t fz = 0;
	size_t sz = 0;
	char fdata[FILENAME_MAX];
	param0.copyFrom(param1);
	param2.copyFrom(param1);
	if (ret == 0)
	{
		param0.startSpaceStep = 0;
		param0.spaceCount = param1.startSpaceStep;
		ret = formFieldFilePathName(fdata, EHsrc, param0);
		if (ret == 0)
		{
			printf("\r\n Load field file 0 for %s : %s", EHsrc, fdata);
			sz = sizeof(EMFLOAT) * 3 * param0.spaceCount;
			ed_data_s0 = (EMFLOAT*)ReadFileIntoMemory(fdata, &fz, &ret);
			if(ret == 0)
			{
				if(fz != sz)
				{
					ret = ERR_FILESIZE_MISMATCH;
				}
			}	
		}
	}
	if (ret == 0)
	{
		ret = formFieldFilePathName(fdata, EHsrc, param1);
		if (ret == 0)
		{
			printf("\r\n Load field file 1 for %s : %s", EHsrc, fdata);
			sz = sizeof(EMFLOAT) * 3 * param1.spaceCount;
			ed_data_s1 = (EMFLOAT*)ReadFileIntoMemory(fdata, &fz, &ret);
			if(ret == 0)
			{
				if(fz != sz)
				{
					ret = ERR_FILESIZE_MISMATCH;
				}
			}	
		}
	}
	if(ret == 0)
	{
		param2.startSpaceStep = 0;
		param2.spaceCount = param1.startSpaceStep + param1.spaceCount;
		ret = formFieldFilePathName(fdata, EHsrc, param2);
		if (ret == 0)
		{
			printf("\r\n generate field file 2 for %s : %s", EHsrc, fdata);
			sz = sizeof(EMFLOAT) * 3 * param2.spaceCount;
			ed_data_s2 = (EMFLOAT*)CreateFileIntoMemory(fdata, sz, &ret);
		}
		if (ret == 0)
		{
			size_t w12 = param2.spaceCount;
			size_t w22 = param2.spaceCount * 2;
			size_t w1 = param0.spaceCount;
			size_t w2 = param0.spaceCount * 2;
			for(unsigned int c=0;c<param0.spaceCount;c++)
			{
				ed_data_s2[c]       = ed_data_s0[c];
				ed_data_s2[w12 + c] = ed_data_s0[w1 + c];
				ed_data_s2[w22 + c] = ed_data_s0[w2 + c];
			}
			w1 = param1.spaceCount;
			w2 = param1.spaceCount * 2;
			for(unsigned int c=0;c<param1.spaceCount;c++)
			{
				ed_data_s2[c + param0.spaceCount]       = ed_data_s1[c];
				ed_data_s2[w12 + c + param0.spaceCount] = ed_data_s1[w1 + c];
				ed_data_s2[w22 + c + param0.spaceCount] = ed_data_s1[w2 + c];
			}
		}
	}
	
	if(ed_data_s0 != nullptr)
	{
		FreeMemory(ed_data_s0); ed_data_s0 = nullptr;
	}
	if(ed_data_s1 != nullptr)
	{
		FreeMemory(ed_data_s1); ed_data_s1 = nullptr;
	}
	if(ed_data_s2 != nullptr)
	{
		FreeMemory(ed_data_s2); ed_data_s2 = nullptr;
	}
	return ret;
}
int mergeSpace(DataSetting& param)
{
	int ret = 0;
	ret = mergeFilesBySpace(param, "gmpHy");
	if(ret == 0)
	{
		ret = mergeFilesBySpace(param, "gmpEz");
	}
	return ret;
}
#else
#ifdef CONVERTTODOUBLE
int convertDataFileToDouble(DataSetting& param)
{
	int ret = 0;
	FILE *fhDbl = NULL;
	size_t fz = 0;
	size_t sz = sizeof(EMFLOAT) * 3 * param.spaceCount;
	char fdata[FILENAME_MAX];
	if (ret == 0)
	{
		ret = formFieldFilePathName(fdata, "gmpHy", param);
		if (ret == 0)
		{
			printf("\r\n Load field file for Hy : %s", fdata);
			ed_dataHy = (EMFLOAT*)ReadFileIntoMemory(fdata, &fz, &ret);
			if(ret == 0)
			{
				if(fz != sz)
				{
					ret = ERR_FILESIZE_MISMATCH;
				}
			}	
		}
	}
	if(ret == 0)
	{
		ret = formFieldFilePathName(fdata, "dblHy", param);
		if (ret == 0)
		{
			printf("\r\n convert field file for Hy : %s", fdata);
			ret = openfileWrite(fdata, &fhDbl);
			if(ret == 0)
			{
				size_t w = 0;
				for(int r=0;r<3;r++)
				{
					for(unsigned int c=0;c<param.spaceCount;c++)
					{
						double var = FLOATEMTODBL(ed_dataHy[w]);
						fwrite(&var, sizeof(double), 1, fhDbl);
						w++;
					}
				}
				closefile(fhDbl);
			}
		}
	}
	if(ed_dataHy != nullptr)
	{
		FreeMemory(ed_dataHy); ed_dataHy = nullptr;
	}
	if (ret == 0)
	{
		ret = formFieldFilePathName(fdata, "gmpEz", param);
		if (ret == 0)
		{
			printf("\r\n Load field file for Ez : %s", fdata);
			ed_dataEz = (EMFLOAT*)ReadFileIntoMemory(fdata, &fz, &ret);
			if(ret == 0)
			{
				if(fz != sz)
				{
					ret = ERR_FILESIZE_MISMATCH;
				}
			}	
		}
	}
	if(ret == 0)
	{
		ret = formFieldFilePathName(fdata, "dblEz", param);
		if (ret == 0)
		{
			printf("\r\n convert field file for Ez : %s", fdata);
			ret = openfileWrite(fdata, &fhDbl);
			if(ret == 0)
			{
				size_t w = 0;
				for(int r=0;r<3;r++)
				{
					for(unsigned int c=0;c<param.spaceCount;c++)
					{
						double var = FLOATEMTODBL(ed_dataEz[w]);
						fwrite(&var, sizeof(double), 1, fhDbl);
						w++;
					}
				}
				closefile(fhDbl);
			}
		}
	}
	if(ed_dataEz != nullptr)
	{
		FreeMemory(ed_dataEz); ed_dataEz = nullptr;
	}
	return ret;
}
#endif
#endif
#endif
#endif
#endif
#endif

int initThreadObjects()
{
	int ret = 0;
#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(CREATETIMEARRAYS)	
	thwkU  = new th_gen_U(Nmax1);  ret = thwkU->returnCode();  if(ret != 0) return ret;
	int thc = thwkU->threadCount();
	if(MAXTHREADCOUNT < thc)
	{
		printf("\r\n MAXTHREADCOUNT(%u) is smaller than %u \r\n",MAXTHREADCOUNT, thc);
		ret = 8;
	}
#endif
#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(USETIMEARRAYS)
	thwkDp = new th_gen_Dp(Nmax1); ret = thwkDp->returnCode(); if(ret != 0) return ret;
	thwkDq = new th_gen_Dq(Nmax2); ret = thwkDq->returnCode(); if(ret != 0) return ret; //Nmax2
	thwkB  = new th_gen_B(Nmax1);  ret = thwkB->returnCode();  if(ret != 0) return ret;
	thwkA0 = new th_gen_A0(Nmax1); ret = thwkA0->returnCode(); if(ret != 0) return ret;
	thwkA1 = new th_gen_A1(Nmax1); ret = thwkA1->returnCode(); if(ret != 0) return ret;
	
#endif	
	return ret;
}
void deinitThreadObjects()
{
#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(CREATETIMEARRAYS)	
	if(thwkU  != nullptr) {delete thwkU;  thwkU  = nullptr;}	
#endif
#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(USETIMEARRAYS)
	if(thwkDp != nullptr) {delete thwkDp; thwkDp = nullptr;}
	if(thwkDq != nullptr) {delete thwkDq; thwkDq = nullptr;}
	if(thwkB  != nullptr) {delete thwkB;  thwkB  = nullptr;}
	if(thwkA0 != nullptr) {delete thwkA0; thwkA0 = nullptr;}
	if(thwkA1 != nullptr) {delete thwkA1; thwkA1 = nullptr;}
#endif
}
int initSim(DataSetting& param)
{
	int ret = 0;
	initParam(param);
	DblIdxCount1 = (Nmax1 * (Nmax1 + 1)) / 2;
	DblIdxCount2 = (Nmax2 * (Nmax2 + 1)) / 2;
	//
	size_t trb = DblIdxCount2 * sizeof(EMFLOAT);
	double kb = 1024.0;
	double trmb = ((double)trb/kb) / kb;
	//
	printf("\n sizeof(EMFLOAT)=%lu, triangle array size: %u items, %lu Bytes, %g MB, %g GB", sizeof(EMFLOAT), DblIdxCount2, trb, trmb, trmb / kb);
	//
	ret = initThreadObjects();
	if(ret == 0)
	{
		ret = initIndexHeaders();
	}
	if(ret == 0)
	{
		initFactorials();
		initSpeedVector(param.v);
		initFactors();
		ret = allocateTimeArrays();
	}
#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(USETIMEARRAYS)	
	if(ret == 0)
	{
		ed_imp = M_PI * 4.0e-7; //mu
		ed_imp = ed_imp * 299792458.0;
	}
#endif	
	if(ret == 0)
	{
		puts("\n OK. initialized simulation"); fflush(stdout);
	}
	return ret;
}
void deinit()
{
#if defined(GENERATECONSTARRAY) || defined(VERIFYCONSTARRAY) || defined(ONEPOINT) || defined(RUNSIMULATION) || defined(CREATETIMEARRAYS)
	if(ed_W != nullptr)
	{
		FreeMemory(ed_W); ed_W = nullptr;
	}
#endif
	if(ed_Ws != nullptr)
	{
		FreeMemory(ed_Ws); ed_Ws = nullptr;
	}
	//
	deAllocateTimeArrays();
	//
	deinitThreadObjects();
}

int main(int argc, char* argv[])
{
	int ret = 0;
	DataSetting param;
	ret = initSim(param);
	if(ret == 0)
	{
		if(argc > 1)
		{
			double a = atof(argv[1]);
			if(a > 0)
			{
				printf("\n a=%g, omega=%g, ds:%g, dt:%g, space range: (%g, %g), time range: (%g, %g)\n", 
					a, sqrt(a),param.step_size_space(a), param.step_size_time(a), param.start_space(a), param.end_space(a), param.start_time(a), param.end_time(a));
			}
		}
		#ifdef GENERATECONSTARRAY
			ret = generateConstants();
		#else 
		#ifdef VERIFYCONSTARRAY
			ret = verifyConstants();
		#else
		#ifdef ONEPOINT
			unsigned int q = 40;
			unsigned int s = 2;
			ret = OnePoint(q, s, param);
		#else
		#ifdef RUNSIMULATION
			ret = simulate(param);
		#else
		#ifdef CREATETIMEARRAYS
			ret = createTimeArrays(param);
		#else
		#ifdef USETIMEARRAYS
			#ifdef EXAMINETIMEARRAYS
				ret = examineTimeArrays(param);
			#else
				ret = useTimeArrays(param);
			#endif
		#else
		#ifdef DATAEMFLOATTODOUBLE
			#ifdef GENERATECOMPAREFILES
				ret = generateCompareFiles(param);
			#else
			#ifdef GENERATEDIFFFILES
				ret = generateDiffFiles(param);
			#else
			#ifdef GENERATEDIFFFILEVS
				ret = generateDiffFileVelocities(param, 0.006, 0.003);
			#else
			#ifdef MERGESPACE
				ret = mergeSpace(param);
			#else
			#ifdef CONVERTTODOUBLE
				ret = convertDataFileToDouble(param);
			#endif	
			#endif	
			#endif
			#endif
			#endif
		#endif
		#endif
		#endif
		#endif
		#endif
		#endif
		#endif
	}
	//
	if (ret != 0)
	{
		printf("\r\n!!! Error code = %d !!!", ret);
	}
	deinit();
	//----------------------------------------
	puts("\r\n Finished.");	
	return ret;
}

