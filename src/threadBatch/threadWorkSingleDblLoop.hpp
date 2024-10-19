#pragma once
/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Created: 05/19/2024
Allrights reserved by David Ge

********************************************************************/
#include "loopwork.hpp"

/*
loop type 2

loop: m=0,1,...,dim-1; n=m,m+1,...,dim-1
only outer loop in parallel

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
local variable m is declared to represent the work to be executed by the thread
m=m0,...,m1: n=m,...,Nmax

*/
class singleDoubleLoopWork: public loopWork
{
protected:
	ThreadSingleLoop* twSd;
	inline virtual void divideWork() {
		twSd = nullptr;
		if(ret == 0)
		{
			unsigned int thCount = threadCount();
			if(thCount > 0)
			{
				twSd = (ThreadSingleLoop*)malloc(sizeof(ThreadSingleLoop) * thCount);
				divideSingleDoubleLoop_mN(twSd, dim-1, thCount);
			}
		}
	}
public:
	inline singleDoubleLoopWork(unsigned int workCount):loopWork(workCount) {
		divideWork();	
	}
	inline ~singleDoubleLoopWork() {
		if(twSd != nullptr) free(twSd);
	}
};
/*
to make coding simple for just running a single task.
the coding is reduced to implementing virtual void work(unsigned int m)
*/
class singleDoubleLoopWorkOne:public singleDoubleLoopWork
{
protected:
	const FUNCTIONID fid = 0;
	TASKID tid;	
	/*
	inside this code there should be a structure of for(unsigned int n=m;n<=(Nmax);n++) {...}	
	*/
	virtual void work(unsigned int m) = 0;
public:
	inline singleDoubleLoopWorkOne(unsigned int workCount) : singleDoubleLoopWork(workCount) {
		tid = registerTask(fid);
	}
	inline ~singleDoubleLoopWorkOne() {}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		if(functionId == fid) {
			SINGLEDOUBLELOOPSINGLEBEGIN(th)
				work(m);
			SINGLEDOUBLELOOPSINGLEEND
		}
	}
	inline void run()  {
		runTask(tid);
	}	
};

/*
loop type 3

loop: m=0,1,...,dim-1; n=0,1,...,m
only outer loop in parallel

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
local variables m is declared to represent the work to be executed by the thread
m=m0,...,m1: n=0,...,m
*/
class singleDbl0mLoopWork: public loopWork
{
protected:
	ThreadSingleLoop* twS0;
	inline virtual void divideWork() {	
		twS0 = nullptr;
		if(ret == 0)
		{
			unsigned int thCount = threadCount();
			if(thCount > 0)
			{
				twS0 = (ThreadSingleLoop*)malloc(sizeof(ThreadSingleLoop) * thCount);
				divideSingleDoubleLoop_0m(twS0, dim-1, thCount);
			}
		}
	}
	
public:
	inline singleDbl0mLoopWork(unsigned int workCount):loopWork(workCount) {
		divideWork();	
	}
	inline ~singleDbl0mLoopWork() {
		if(twS0 != nullptr) free(twS0);
	}
};
/*
to make coding simple for just running a single task.
the coding is reduced to implementing virtual void work(unsigned int m)
*/
class singleDbl0mLoopWorkOne:public singleDbl0mLoopWork
{
protected:
	const FUNCTIONID fid = 0;
	TASKID tid;	
	/*
		implementation should include for(unsigned int n=0;n<=m;n++) {...}	
	*/
	virtual void work(unsigned int m) = 0;
public:
	inline singleDbl0mLoopWorkOne(unsigned int workCount):singleDbl0mLoopWork(workCount) {
		tid = registerTask(fid);
	}
	inline ~singleDbl0mLoopWorkOne(){}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		if(functionId == fid) {
			SINGLEDOUBLELOOP_0mSINGLE_BEGIN(th)
				work(m);
			SINGLEDOUBLELOOP_0mSINGLE_END
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
class singleDbl0mLoopWorkOneTH:public singleDbl0mLoopWork
{
protected:
	const FUNCTIONID fid = 0;
	TASKID tid;	
	/*
		implementation should include for(unsigned int n=0;n<=m;n++) {...}	
	*/
	virtual void work(THREADINDEX th, unsigned int m) = 0;
public:
	inline singleDbl0mLoopWorkOneTH(unsigned int workCount):singleDbl0mLoopWork(workCount) {
		tid = registerTask(fid);
	}
	inline ~singleDbl0mLoopWorkOneTH(){}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		if(functionId == fid) {
			SINGLEDOUBLELOOP_0mSINGLE_BEGIN(th)
				work(th, m);
			SINGLEDOUBLELOOP_0mSINGLE_END
		}
	}
	inline void run()  {
		runTask(tid);
	}		
};

/*
loop type 4

loop: m=0,1,...,dim-1; n=m,m-1,...,0
only outer loop in parallel

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
local variables m and n are declared to represent the work to be executed by the thread
m=m0,...,m1: n=m,...,0

*/
class singleDblm0LoopWork: public loopWork
{
protected:
	ThreadSingleLoop* twS0;
	inline virtual void divideWork() {		
		twS0 = nullptr;
		if(ret == 0)
		{
			unsigned int thCount = threadCount();
			if(thCount > 0)
			{
				twS0 = (ThreadSingleLoop*)malloc(sizeof(ThreadSingleLoop) * thCount);
				divideSingleDoubleLoop_0m(twS0, dim-1, thCount);
			}
		}
	}
	
public:
	inline singleDblm0LoopWork(unsigned int workCount):loopWork(workCount) {
		divideWork();	
	}
	inline ~singleDblm0LoopWork() {
		if(twS0 != nullptr) free(twS0);
	}
};
/*
to make coding simple for just running a single task.
the coding is reduced to implementing virtual void work(unsigned int m)
*/
class singleDblm0LoopWorkOne:public singleDblm0LoopWork
{
protected:
	const FUNCTIONID fid = 0;
	TASKID tid;	
	/*
		implementation should include for(int n=(int)m;n>=0;n--) { {...}	
	*/
	virtual void work(unsigned int m) = 0;
public:
	inline singleDblm0LoopWorkOne(unsigned int workCount):singleDblm0LoopWork(workCount){
		tid = registerTask(fid);
	}
	inline ~singleDblm0LoopWorkOne(){}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		if(functionId == fid) {
			SINGLEDOUBLELOOP_m0SINGLE_BEGIN(th)
				work(m);
			SINGLEDOUBLELOOP_m0SINGLE_END
		}
	}
	inline void run()  {
		runTask(tid);
	}	
};


