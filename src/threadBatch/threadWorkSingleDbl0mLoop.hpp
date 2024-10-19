#pragma once
/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Created: 05/19/2024
Allrights reserved by David Ge

********************************************************************/
#include "ThreadBatch.h"
#include "loopDivide.h"

/*
vector:
m=0,1,...,Nmax
		n=0,1,...,m
assume operations on vector elements can be done in parallel for outer loop	
*/
class singleDbl0mLoopWork: public virtual TaskExecuter
{
protected:
	ThreadBatch* threadpool;
	ThreadSingleLoop* twSf;
	const FUNCTIONID fid = 0;
	TASKID tid;	
	int ret;
	unsigned int dim;
	virtual void work(unsigned int m, unsigned int n) = 0;
public:
	inline explicit singleDbl0mLoopWork(ThreadBatch* thread_pool, unsigned int vectorDim) {
		dim = vectorDim;
		threadpool = thread_pool;
		twSf = (ThreadSingleLoop*)malloc(sizeof(ThreadSingleLoop)*threadpool->threadCount());
		divideSingleDoubleLoop_0m(twSf, vectorDim-1, threadpool->threadCount());
		tid = threadpool->registerTask(this, fid);
		ret = 0;	
	}
	inline ~singleDbl0mLoopWork() {
		if(twS != nullptr) free(twS);
	}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		SINGLEDOUBLELOOP_0m_BEGIN(th)
			work(m, n);
		SINGLEDOUBLELOOP_0m_END
	}
	inline void run()  {
		threadpool->executeTaskBatch(tid);
	}
	inline int returnCode() {return ret;}
};

/*
m=m0,...,m1

n=0,...,m: {...} is the custom code for each m
*/
class singleDbl0mSingleLoopWork: public virtual TaskExecuter
{
protected:
	ThreadBatch* threadpool;
	ThreadSingleLoop* twSf;
	const FUNCTIONID fid = 0;
	TASKID tid;	
	int ret;
	unsigned int dim;
	virtual void work(unsigned int m) = 0;
public:
	inline explicit singleDbl0mSingleLoopWork(ThreadBatch* thread_pool, unsigned int vectorDim) {
		dim = vectorDim;
		threadpool = thread_pool;
		twSf = (ThreadSingleLoop*)malloc(sizeof(ThreadSingleLoop)*threadpool->threadCount());
		divideSingleDoubleLoop_0m(twSf, vectorDim-1, threadpool->threadCount());
		tid = threadpool->registerTask(this, fid);
		ret = 0;	
	}
	inline ~singleDbl0mSingleLoopWork() {
		if(twS != nullptr) free(twS);
	}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		SINGLEDOUBLELOOP_0mSINGLE_BEGIN(th)
			work(m);
		SINGLEDOUBLELOOP_0mSINGLE_END
	}
	inline void run()  {
		threadpool->executeTaskBatch(tid);
	}
	inline int returnCode() {return ret;}
};

/*
vector:
m=0,1,...,Nmax
		n=0,1,...,m
assume operations on vector elements can be done in parallel for outer loop	
*/
class th_singleDbl0mLoopWork: public virtual TaskExecuter
{
protected:
	ThreadBatch* threadpool;
	ThreadSingleLoop* twSf;
	const FUNCTIONID fid = 0;
	TASKID tid;	
	int ret;
	unsigned int dim;
	virtual void work(unsigned int m, unsigned int n) = 0;
public:
	inline explicit th_singleDbl0mLoopWork(THREADCOUNT threadCount, unsigned int vectorDim) {
		dim = vectorDim;
		threadpool = new ThreadBatch(threadCount);
		twSf = (ThreadSingleLoop*)malloc(sizeof(ThreadSingleLoop)*threadpool->threadCount());
		divideSingleDoubleLoop_0m(twSf, vectorDim-1, threadpool->threadCount());
		tid = threadpool->registerTask(this, fid);
		ret = 0;	
	}
	inline ~th_singleDbl0mLoopWork() {
		if(twS != nullptr) free(twS);
		if(threadpool != nullptr) delete threadpool;
	}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		SINGLEDOUBLELOOP_0m_BEGIN(th)
			work(m, n);
		SINGLEDOUBLELOOP_0m_END
	}
	inline void run()  {
		threadpool->executeTaskBatch(tid);
	}
	inline int returnCode() {return ret;}
};


/*
m=m0,...,m1

n=0,...,m: {...} is the custom code for each m
*/
class th_singleDbl0mSingleLoopWork: public virtual TaskExecuter
{
protected:
	ThreadBatch* threadpool;
	ThreadSingleLoop* twSf;
	const FUNCTIONID fid = 0;
	TASKID tid;	
	int ret;
	unsigned int dim;
	virtual void work(unsigned int m) = 0;
public:
	inline explicit th_singleDbl0mSingleLoopWork(THREADCOUNT threadCount, unsigned int vectorDim) {
		dim = vectorDim;
		threadpool = new ThreadBatch(threadCount);
		twSf = (ThreadSingleLoop*)malloc(sizeof(ThreadSingleLoop)*threadpool->threadCount());
		divideSingleDoubleLoop_0m(twSf, vectorDim-1, threadpool->threadCount());
		tid = threadpool->registerTask(this, fid);
		ret = 0;	
	}
	inline ~th_singleDbl0mSingleLoopWork() {
		if(twS != nullptr) free(twS);
		if(threadpool != nullptr) delete threadpool;
	}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		SINGLEDOUBLELOOP_0mSINGLE_BEGIN(th)
			work(m);
		SINGLEDOUBLELOOP_0mSINGLE_END
	}
	inline void run()  {
		threadpool->executeTaskBatch(tid);
	}
	inline int returnCode() {return ret;}
};

