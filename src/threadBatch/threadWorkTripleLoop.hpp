#pragma once
/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Created: 05/26/2024
Allrights reserved by David Ge

********************************************************************/
#include "loopwork.hpp"

/*
loop type 7

loop: m=0,...,dim-1:s=0,...,m;n=0,...,s
the first 2 loops in parallel

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
local variables m, s and n are declared to represent the work to be executed by the thread
m=m0,...,m1:s=s0,...,s1
n=0,...,s
*/
class tripleLoopWork: public loopWork
{
protected:
	ThreadDoubleLoop* twT;
	inline virtual void divideWork() {		
		twT = nullptr;
		if(ret == 0)
		{
			unsigned int thCount = threadCount();
			if(thCount > 0)
			{
				twT = (ThreadDoubleLoop*)malloc(sizeof(ThreadDoubleLoop) * thCount);
				divideTripleLoop(twT, dim-1, thCount);
			}
		}
	}
	
public:
	inline tripleLoopWork(unsigned int workCount) : loopWork(workCount) {
		divideWork();	
	}
	inline ~tripleLoopWork() {
		if(twT != nullptr) free(twT);
	}
};
/*
to make coding simple for just running a single task.
the coding is reduced to implementing virtual void work(unsigned int m)
*/
class tripleLoopWorkOne:public tripleLoopWork
{
protected:
	const FUNCTIONID fid = 0;
	TASKID tid;	
	/*
		implement should include for(unsigned int n=0;n<=s;n++){...}	
	*/
	virtual void work(unsigned int m, unsigned int s) = 0;
public:
	inline tripleLoopWorkOne(unsigned int workCount) : tripleLoopWork(workCount) {
		tid = registerTask(fid);	
	}
	inline ~tripleLoopWorkOne(){}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		if(functionId == fid) {
			TRIPLELOOP2BEGIN(th)
				work(m,s);
			TRIPLELOOP2END
		}
	}
	inline void run() {
		runTask(tid);
	}
	
};

/*
loop type 8

loop: s=0,...,dim-1:m=s,...,dim-1;n=m,m+1,...,dim-1
the first 2 loops in parallel

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
local variables m, s and n are declared to represent the work to be executed by the thread
s=s0,...,s1:m=m0,...,m1
n=m,m+1,...,dim-1
can also be
n=0,1,...,dim-1-m
*/
class tripleLoopWorkSubFirst: public loopWork
{
protected:
	ThreadDoubleLoop* twTs;
	unsigned int Nmax;
	inline virtual void divideWork() {			
		twTs = nullptr;
		if(ret == 0)
		{
			unsigned int thCount = threadCount();
			if(thCount > 0)
			{
				Nmax = dim - 1;
				twTs = (ThreadDoubleLoop*)malloc(sizeof(ThreadDoubleLoop) * thCount);
				divideTripleLoopSubFirst(twTs, Nmax, thCount);
			}
		}
	}
	
public:
	inline tripleLoopWorkSubFirst(unsigned int workCount) : loopWork(workCount) {
		divideWork();	
	}
	inline ~tripleLoopWorkSubFirst() {
		if(twTs != nullptr) free(twTs);
	}
};
/*
to make coding simple for just running a single task.
the coding is reduced to implementing virtual void work(unsigned int m)
*/
class tripleLoopWorkSubFirstOne:public tripleLoopWorkSubFirst
{
protected:
	const FUNCTIONID fid = 0;
	TASKID tid;	
	/*
		implement should include 
			for(unsigned int n=m;n<=(Nmax);n++){...}
		or	
			for(unsigned int n=0;n<=(Nmax)-m;n++){...}
	*/
	virtual void work(THREADINDEX th, unsigned int s, unsigned int m) = 0;
public:
	inline tripleLoopWorkSubFirstOne(unsigned int workCount) : tripleLoopWorkSubFirst(workCount) {
		tid = registerTask(fid);	
	}
	inline ~tripleLoopWorkSubFirstOne(){}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		if(functionId == fid) {
			TRIPLELOOPINVERSE2BEGIN2(th, Nmax)
				work(th, s,m);
			TRIPLELOOPINVERSE2END2
		}	
	}
	inline void run()  {
		runTask(tid);
	}
};




