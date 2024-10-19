#pragma once
/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Created: 05/26/2024
Allrights reserved by David Ge

********************************************************************/
#include "loopwork.hpp"

/*
loop type 5

loop: m=0,1,...,dim-1; s=0,1,...,m
both loops in parallel

local variables m and s are declared to represent the work to be executed by the thread
*/
class doubleLoopWork: public loopWork
{
protected:
	ThreadDoubleLoop* twD;
	inline virtual void divideWork() {
		twD = nullptr;
		if(ret == 0)
		{
			unsigned int thCount = threadCount();
			if(thCount > 0)
			{
				twD = (ThreadDoubleLoop*)malloc(sizeof(ThreadDoubleLoop) * thCount);
				divideDoubleLoop(twD, dim-1, thCount);
			}
		}	
	}
public:
	inline doubleLoopWork(unsigned int workCount) : loopWork{workCount} {
		divideWork();	
	}
	inline ~doubleLoopWork() {
		if(twD != nullptr) free(twD);
	}
};
/*
to make coding simple for just running a single task.
the coding is reduced to implementing virtual void work(unsigned int m)
*/
class doubleLoopWorkOne : public doubleLoopWork
{
protected:
	const FUNCTIONID fid = 0;
	TASKID tid;	
	inline virtual void work(unsigned int m, unsigned int s) = 0;

public:
	inline doubleLoopWorkOne(unsigned int workCount) : doubleLoopWork(workCount)  {
		tid = registerTask(fid);		
	}
	inline ~doubleLoopWorkOne() {}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		if(functionId == fid) {
			DOUBLELOOPBEGIN(th)
				work(m,s);
			DOUBLELOOPEND
		}
	}
	inline void run() {
		runTask(tid);
	}	
};

/*
loop type 5b

loop: m=0,1,...,dim-1; s=0,1,...,dim-1-m
both loops in parallel

local variables m and s are declared to represent the work to be executed by the thread
*/
class doubleLoopWork2: public loopWork
{
protected:
	ThreadDoubleLoop* twD;
	unsigned int Nmax;
	inline virtual void divideWork() {
		twD = nullptr;
		if(ret == 0)
		{
			unsigned int thCount = threadCount();
			if(thCount > 0)
			{
				Nmax = dim - 1;
				twD = (ThreadDoubleLoop*)malloc(sizeof(ThreadDoubleLoop) * thCount);
				divideDoubleLoop2(twD, dim-1, thCount);
			}
		}	
	}
public:
	inline doubleLoopWork2(unsigned int workCount) : loopWork{workCount} {
		divideWork();	
	}
	inline ~doubleLoopWork2() {
		if(twD != nullptr) free(twD);
	}
};
/*
to make coding simple for just running a single task.
the coding is reduced to implementing virtual void work(unsigned int m)
*/
class doubleLoopWork2One : public doubleLoopWork2
{
protected:
	const FUNCTIONID fid = 0;
	TASKID tid;	
	inline virtual void work(unsigned int m, unsigned int s) = 0;

public:
	inline doubleLoopWork2One(unsigned int workCount) : doubleLoopWork2(workCount)  {
		tid = registerTask(fid);		
	}
	inline ~doubleLoopWork2One() {}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		if(functionId == fid) {
			DOUBLELOOPBEGINb(Nmax, th)
				work(m,s);
			DOUBLELOOPENDb
		}
	}
	inline void run() {
		runTask(tid);
	}	
};


/*
loop type 6

loop: s=0,1,...,dim-1; m=s,s+1,...,dim-1
both loops in parallel

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
local variables m and s are declared to represent the work to be executed by the thread
*/
class doubleLoopWorkSubFirst: public loopWork
{
protected:
	ThreadDoubleLoop* twDs;
	unsigned int Nmax;
	inline virtual void divideWork() {
		twDs = nullptr;
		if(ret == 0)
		{
			unsigned int thCount = threadCount();
			if(thCount > 0)
			{
				Nmax = dim - 1;
				twDs = (ThreadDoubleLoop*)malloc(sizeof(ThreadDoubleLoop) * thCount);
				divideDoubleLoopSubFirst(twDs, Nmax, thCount);
			}
		}
	}
public:
	inline doubleLoopWorkSubFirst(unsigned int workCount) : loopWork(workCount) {
		divideWork();	
	}
	inline ~doubleLoopWorkSubFirst() {
		if(twDs != nullptr) free(twDs);
	}
};
/*
to make coding simple for just running a single task.
the coding is reduced to implementing virtual void work(unsigned int m)
*/
class doubleLoopWorkSubFirstOne : public doubleLoopWorkSubFirst
{
protected:
	const FUNCTIONID fid = 0;
	TASKID tid;	
	virtual void work(unsigned int s, unsigned int m) = 0;

public:
	inline doubleLoopWorkSubFirstOne(unsigned int workCount) : doubleLoopWorkSubFirst(workCount) {
		tid = registerTask(fid);		
	}
	inline ~doubleLoopWorkSubFirstOne(){}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		if(functionId == fid) {
			DOUBLELOOPINVERSEBEGIN(th, Nmax)
				work(s,m);
			DOUBLELOOPINVERSEEND
		}
	}	
	inline void run() {
		runTask(tid);
	}
};


