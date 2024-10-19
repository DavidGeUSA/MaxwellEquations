#pragma once
/*******************************************************************
Author: David Wei Ge (dge893@gmail.com, aka Wei Ge)
Created: 08/28/2024
Allrights reserved by David Ge

********************************************************************/
#include "EMFloat.h"

//defined in the makefile, i.e.
//-D MAXFIELDTERMS2000

/*
static vector sizes
N: last index of the summation terms
Nmax0  = N
Nmax1  = N + 1: count of the summation terms; it is the size of most vectors
Nmax2  = N + 2: size of vector Dq and s2k
Nmax22 = 2N + 2: size of vector V
Nmax23 = 2N + 3: size of vector f  
*/

#ifdef MAXFIELDTERMS100
#define Nmax0   100
#define Nmax1   101
#define Nmax2   102
#define Nmax22  202
#define Nmax23  203 

#else
#ifdef MAXFIELDTERMS200
#define Nmax0   200
#define Nmax1   201
#define Nmax2   202
#define Nmax22  402
#define Nmax23  403

#else
#ifdef MAXFIELDTERMS500
#define Nmax0   500
#define Nmax1   501
#define Nmax2   502
#define Nmax22  1002
#define Nmax23  1003 

#else
#ifdef MAXFIELDTERMS1000
#define Nmax0   1000
#define Nmax1   1001
#define Nmax2   1002
#define Nmax22  2002
#define Nmax23  2003

#else
#ifdef MAXFIELDTERMS2000
#define Nmax0   2000
#define Nmax1   2001
#define Nmax2   2002
#define Nmax22  4002
#define Nmax23  4003

#else
#ifdef MAXFIELDTERMS3000
#define Nmax0   3000
#define Nmax1   3001
#define Nmax2   3002
#define Nmax22  6002
#define Nmax23  6003

#else
#ifdef MAXFIELDTERMS4000
#define Nmax0   4000
#define Nmax1   4001
#define Nmax2   4002
#define Nmax22  8002
#define Nmax23  8003

#else
#define Nmax0   5000
#define Nmax1   5001
#define Nmax2   5002
#define Nmax22  10002
#define Nmax23  10003

#endif
#endif
#endif
#endif
#endif
#endif
#endif

/////////////////////////////////////

#define NUMSTRSIZE 10

#define FLOATEMTODBL(d) (d.convert_to<double>())

#define FLOATEMTOSTR(c, dec) (c).str((dec), std::ios::fixed).c_str()

/*
field source = sqrt(a) * exp(-a (x-v*ct)^2) * cos(omega * ct)
for simplifying calculations, choose omega = sqrt(a), see cosineV2.docx
l0: 2pi * l0 = number of field values in one cycle
ds = dt = 1 / (omega * l0) = (wave length) / (2 pi * l0)
*/
typedef struct DataSetting{
	unsigned int dataFileId; // to distinguish different simulations of the same parameters. i.e. by different algorithms
	double v;  // source moving speed
	//simulation configurations
	double lt; // 2pi * lt = number of field values in one cycle of time. it is a frequency independent time interval
	double ls; // 2pi * ls = number of field values in one cycle of space. it is a frequency independent space interval
	unsigned int startSpaceStep; // starting space location = startSpaceStep * ds
	unsigned int startTimeStep;  // starting time = startTimeStep * dt
	unsigned int spaceCount;     // number of the space steps in the result data files
	unsigned int timeCount;      // number of the time steps in the result data files
	// folders
	const char *tmpDir;  // for temporary files
	const char *outDir;  // for output files
	const char *dataDir; // for result data files
	//
	void copyFrom(DataSetting& src)
	{
		dataFileId = src.dataFileId;
		v  = src.v;
		lt = src.lt;
		ls = src.ls;
		startSpaceStep = src.startSpaceStep;
		startTimeStep  = src.startTimeStep;
		spaceCount = src.spaceCount;
		timeCount  = src.timeCount;
		tmpDir = src.tmpDir;
		outDir = src.outDir;
		dataDir = src.dataDir;
	}
	//
	/*
		startSpaceStep <= s < endStepExclude_space
	*/
	unsigned int endStepExclude_space() {
		return startSpaceStep + spaceCount;
	}
	/*
		startTimeStep <= q < endStepExclude_time
	*/
	unsigned int endStepExclude_time() {
		return startTimeStep + timeCount;
	}
	//
	///interpret the simulation range by specifying the source angular frequency omega: a = omega^2 ///////////
	/*
		dct
	*/
	double step_size_time(double a) {
		return 1.0 / (sqrt(a) * lt);
	}
	/*
		ds
	*/
	double step_size_space(double a) {
		return 1.0 / (sqrt(a) * ls);
	}
	/*
		start space in meters	
	*/
	double start_space(double a) {
		return (double)startSpaceStep * step_size_space(a);
	}
	/*
		end space in meters	
	*/
	double end_space(double a) {
		return (double)(startSpaceStep + spaceCount) * step_size_space(a);	
	}
	/*
		start time (ct) in meters	
	*/
	double start_time(double a) {
		return (double)startTimeStep * step_size_time(a);
	}
	/*
		end time (ct) in meters	
	*/
	double end_time(double a) {
		return (double)(startTimeStep + timeCount) * step_size_time(a);
	}	
}DataSetting;

