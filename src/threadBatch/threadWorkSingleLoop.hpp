#pragma once
/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Created: 05/19/2024
Allrights reserved by David Ge

********************************************************************/
#include "loopwork.hpp"

/*
loop type 1

vector:
m=0,1,...,Nmax
assume operations on vector elements can be done in parallel	
*/
class singleLoopWork: public loopWork
{
protected:
	ThreadSingleLoop* twS;
	inline virtual void divideWork() {			
		twS = nullptr;
		if(ret == 0)
		{
			unsigned int thCount = threadCount();
			if(thCount > 0)
			{
				twS = (ThreadSingleLoop*)malloc(sizeof(ThreadSingleLoop) * thCount);
				divideSingleLoop(twS, dim-1, thCount);
			};
		}	
	}
	
public:
	inline singleLoopWork(unsigned int workCount) : loopWork(workCount) {
		divideWork();	
	}
	inline virtual ~singleLoopWork() {
		if(twS != nullptr) free(twS);
	}
};

/*
to make coding simple for just running a single task.
the coding is reduced to implementing virtual void work(unsigned int m)
*/
class singleLoopWorkOne : public singleLoopWork
{
protected:
	const FUNCTIONID fid = 0;
	TASKID tid;
	inline virtual void work(unsigned int m) = 0;	
public:
	inline singleLoopWorkOne(unsigned int workCount) : singleLoopWork(workCount) {
		tid = registerTask(fid);
	}
	inline virtual ~singleLoopWorkOne() {}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		if(functionId == fid) {
			SINGLELOOPBEGIN(th)
				work(m);
			SINGLELOOPEND
		}
	}	
	inline void run()  {
		runTask(tid);
	}
};

/*
to make coding simple for just running a single task.
the coding is reduced to implementing virtual void work(THREADINDEX th, unsigned int m)
*/
class singleLoopWorkOneTH : public singleLoopWork
{
protected:
	const FUNCTIONID fid = 0;
	TASKID tid;
	inline virtual void work(THREADINDEX th, unsigned int m) = 0;	
public:
	inline singleLoopWorkOneTH(unsigned int workCount) : singleLoopWork(workCount) {
		tid = registerTask(fid);
	}
	inline virtual ~singleLoopWorkOneTH() {}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		if(functionId == fid) {
			SINGLELOOPBEGIN(th)
				work(th, m);
			SINGLELOOPEND
		}
	}	
	inline void run()  {
		runTask(tid);
	}
};



