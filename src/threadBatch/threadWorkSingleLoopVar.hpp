#pragma once
/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Created: 06/02/2024
Allrights reserved by David Ge

********************************************************************/
#include "loopwork.hpp"

//debug
//#include <iostream>

/*
loop type 1, variable dimension

vector:
m=0,1,...,Nmax
assume operations on vector elements can be done in parallel	

three local variables are created: m0, m1, m to construct work: for(unsigned int m=m0;m<m1;m++) {...}
*/
#define SINGLELOOPVARBEGIN(th) if(th > lastTH) return;\
{unsigned int m0 = start[th], m1; if(th < lastTH) m1 = m0 + itemCount; else m1 = dim; for(unsigned int m=m0;m<m1;m++) {
	
#define SINGLELOOPVAREND }}
 
class singleLoopWorkVar: public loopWork
{
protected:
	unsigned int* start;    //start index
	unsigned int itemCount; //interval
	unsigned int thCount;   //thread count
	unsigned int lastTH;    //last thread index	
	inline virtual void divideWork() {			
		start = nullptr;
		if(ret == 0)
		{
			thCount = threadCount();
			if(thCount > 0)
			{
				start = (unsigned int*)malloc(sizeof(unsigned int) * thCount);
				start[0] = 0;
				lastTH = thCount - 1;
			};
		}	
		//debug
		//std::cout << "\n ret=" << ret << " thCount=" << thCount << " " << std::flush;
	}
	
public:
	inline explicit singleLoopWorkVar(unsigned int workCount) : loopWork(workCount) {
		divideWork();
		if(ret == 0)
		{
			setWorkCount(workCount);
		}	
	}
	inline ~singleLoopWorkVar() {
		if(start != nullptr) free(start);
	}
	inline void setWorkCount(unsigned int workCount) {
		dim = workCount;
		if (dim > thCount)
		{
			lastTH    = thCount - 1;
			itemCount = dim / thCount;			
		}
		else
		{
			lastTH    = dim - 1;
			itemCount = 1;
		}
		for(unsigned int th=1;th<thCount;th++)
		{
			start[th] = start[th-1] + itemCount;
		}
	}
};

/*
to make coding simple for just running a single task.
the coding is reduced to implementing virtual void work(unsigned int m)
*/
class singleLoopWorkOneVar : public singleLoopWorkVar
{
protected:
	const FUNCTIONID fid = 0;
	TASKID tid;
	inline virtual void work(unsigned int m) = 0;	
public:
	inline singleLoopWorkOneVar(unsigned int workCount) : singleLoopWorkVar(workCount) {
		tid = registerTask(fid);
	}
	inline ~singleLoopWorkOneVar() {}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		if(functionId == fid) {
			SINGLELOOPVARBEGIN(th)
				work(m);
			SINGLELOOPVAREND
		}
	}	
	inline void run()  {
		runTask(tid);
	}
	inline void runDim(unsigned int workCount)  {
		setWorkCount(workCount);
		runTask(tid);
	}	
};



