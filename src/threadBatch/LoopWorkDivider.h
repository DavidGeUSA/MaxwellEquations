#pragma once
/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Last modified: 07/15/2020
Allrights reserved by David Ge

divide a for(...) loop into multiple parts to be executed by multiple threads parallely

Modifications
Date            Author      Description
---------------------------------------------
2021-03-06      David Ge    Linux compatibility
2022-12-17      David Ge    Add 3D support
********************************************************************/

#ifdef __linux__ 
#include <cstddef>
#endif
#ifdef _WIN32
#define NULL 0
#endif

typedef struct LoopWorkStruct{
	unsigned int StartIndex;
	unsigned int EndIndex;
}LoopWorkStruct;

/*
	single thread 3D loop:
	for(i=i0;i<=i1;i++)
	{
		for(j=j0;j<=j1;j++)
		{
			for(k=k0;k<=k1;k++)
			{
				work();
			}
		}
	}
	divide x-loop into N parts for multi-threads.
	loop from i0 to i1
	dnx = int((i1-i0+1)/N) 
	1st loop work:      i0,      i0+1,..., i0+ dnx-1
	2nd loop work: i0+ dnx, i0+ dnx+1,..., i0+2dnx-1
	3rd loop work: i0+2dnx, i0+2dnx+1,..., i0+3dnx-1
	...
	Nth loop work: i0+(N-1)dnx,i0+(N-1)dnx+1,...,i1

	n-th loop work, n=1,2,...,N:
		starting index = i0+(n-1)*dnx
		ending index   = if(n<N) i0+n*dnx-1; if(n==N) i1
		starting one-Dimension index = (nz+1)*(ny+1)*(starting index)
*/
class LoopWorkDivider
{
protected:
	unsigned int N;
	LoopWorkStruct *works;
public:
	LoopWorkDivider();
	virtual ~LoopWorkDivider();
	virtual int initializeDivider(unsigned int i0,unsigned int i1, unsigned int n);
	unsigned int Count(){ return N; }
	bool empty(unsigned int th){ return works[th].StartIndex > works[th].EndIndex; }
	unsigned int StartIndex(unsigned int th){ return works[th].StartIndex; }
	unsigned int EndIndex(unsigned int th){ return works[th].EndIndex; }
};

typedef enum {
	AXIS_X = 1,
	AXIS_Y = 2,
	AXIS_Z = 3,
}Axis_Component;

/*
support divide specified axis
*/
class LoopWorkDivider3D
{
protected:
	unsigned int N;
	Axis_Component axis;
	LoopWorkStruct *works;
	unsigned int iStart;
	unsigned int jStart;
	unsigned int kStart;
	unsigned int iEnd;
	unsigned int jEnd;
	unsigned int kEnd;
public:
	LoopWorkDivider3D();
	virtual ~LoopWorkDivider3D();
	virtual int initializeDivider(unsigned int i0, unsigned int i1, unsigned int j0, unsigned int j1, unsigned int k0, unsigned int k1, Axis_Component loopAxis, unsigned int n);
	unsigned int Count(){ return N; }
	bool empty(unsigned int th){ return works[th].StartIndex > works[th].EndIndex; }
	//
	unsigned int StartIndexX(unsigned int th){ if (axis == AXIS_X) return works[th].StartIndex; return iStart; }
	unsigned int StartIndexY(unsigned int th){ if (axis == AXIS_Y) return works[th].StartIndex; return jStart; }
	unsigned int StartIndexZ(unsigned int th){ if (axis == AXIS_Z) return works[th].StartIndex; return kStart; }
	//
	unsigned int EndIndexX(unsigned int th){ if (axis == AXIS_X) return works[th].EndIndex; return iEnd; }
	unsigned int EndIndexY(unsigned int th){ if (axis == AXIS_Y) return works[th].EndIndex; return jEnd; }
	unsigned int EndIndexZ(unsigned int th){ if (axis == AXIS_Z) return works[th].EndIndex; return kEnd; }
};

