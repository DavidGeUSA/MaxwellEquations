/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Last modified: 11/16/2020
Allrights reserved by David Ge

divide a for(...) loop into multiple parts to be executed by multiple threads
********************************************************************/
#include "LoopWorkDivider.h"
#include "ErrorThreadBatch.h"
#include <malloc.h>

LoopWorkDivider::LoopWorkDivider()
{
	works = NULL;
}


LoopWorkDivider::~LoopWorkDivider()
{
	if (works != NULL)
	{
		free(works); works = NULL;
	}
}

int LoopWorkDivider::initializeDivider(unsigned int i0, unsigned int i1, unsigned int n)
{
	int ret = ERR_OK;
	N = n;
	if (i0 > i1)
	{
		return ERR_TB_INV_DIV;
	}
	if (N < 2) N = 1;
	works = (LoopWorkStruct *)malloc(N*sizeof(LoopWorkStruct));
	if (works == NULL)
	{
		return ERR_OUTOFMEMORY;
	}
	unsigned int cycleCount = i1 - i0 + 1;
	if (cycleCount < N)
	{
		unsigned int idx = i0;
		for (unsigned int i = 0; i < cycleCount; i++)
		{
			works[i].StartIndex = idx;
			works[i].EndIndex = idx;
			idx++;
		}
		for (unsigned int i = cycleCount; i < N; i++)
		{
			works[i].StartIndex = idx;
			works[i].EndIndex = 0;
		}
	}
	else
	{
		unsigned int dnx = (unsigned int)(cycleCount / N);
		for (unsigned int i = 0; i < N; i++)
		{
			works[i].StartIndex = i0 + i*dnx;
			if (i < N - 1)
				works[i].EndIndex = i0 + (i + 1) * dnx - 1;
			else
				works[i].EndIndex = i1;
		}
	}
	return ret;
}

LoopWorkDivider3D::LoopWorkDivider3D()
{
	works = NULL;
}
LoopWorkDivider3D::~LoopWorkDivider3D()
{
	if (works != NULL) free(works);
}

int LoopWorkDivider3D::initializeDivider(unsigned int i0, unsigned int i1, unsigned int j0, unsigned int j1, unsigned int k0, unsigned int k1, Axis_Component loopAxis, unsigned int n)
{
	int ret = ERR_OK;
	N = n;
	if (i0 > i1 || j0 > j1 || k0 > k1)
	{
		return ERR_TB_INV_DIV;
	}
	iStart = i0; iEnd = i1;
	jStart = j0; jEnd = j1;
	kStart = k0; kEnd = k1;
	axis = loopAxis;
	if (N < 2) N = 1;
	works = (LoopWorkStruct *)malloc(N*sizeof(LoopWorkStruct));
	if (works == NULL)
	{
		return ERR_OUTOFMEMORY;
	}
	unsigned int cycleCount;
	unsigned int idx0;
	unsigned int idx1;
	if (axis == AXIS_X)
	{
		idx0 = i0; idx1 = i1;
	}
	else if (axis == AXIS_Y)
	{
		idx0 = j0; idx1 = j1;
	}
	else
	{
		idx0 = k0; idx1 = k1;
	}
	cycleCount = idx1 - idx0 + 1;
	if (cycleCount < N)
	{
		unsigned int idx = idx0;
		for (unsigned int th = 0; th < cycleCount; th++)
		{
			works[th].StartIndex = idx;
			works[th].EndIndex = idx;
			idx++;
		}
		for (unsigned int th = cycleCount; th < N; th++)
		{
			works[th].StartIndex = 1;
			works[th].EndIndex = 0;
		}
	}
	else if (N == 1)
	{
		works[0].StartIndex = idx0;
		works[0].EndIndex = idx1;
	}
	else
	{
		unsigned int dnx = (unsigned int)(cycleCount / N);
		for (unsigned int th = 0; th < N; th++)
		{
			works[th].StartIndex = idx0 + th * dnx;
			if (th < N - 1)
				works[th].EndIndex = idx0 + (th + 1) * dnx - 1;
			else
				works[th].EndIndex = idx1;
		}
	}
	return ret;
}
