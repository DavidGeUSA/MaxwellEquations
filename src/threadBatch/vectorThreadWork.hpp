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
assume operations on vector elements can be done in parallel	
*/
class vectorExecuter: public virtual TaskExecuter
{
protected:
	ThreadBatch* threadpool;
	ThreadSingleLoop* twS;
	const FUNCTIONID fid = 0;
	TASKID tid;	
	int ret;
	virtual void work(unsigned int m) = 0;
public:
	inline explicit vectorExecuter(THREADCOUNT threadCount, unsigned int vectorDim) {
		twS = (ThreadSingleLoop*)malloc(sizeof(ThreadSingleLoop)*threadCount);
		divideSingleLoop(twS, vectorDim-1, threadCount);
		threadpool = new ThreadBatch(threadCount);
		tid = threadpool->registerTask(this, fid);
		ret = 0;	
	}
	inline ~vectorExecuter() {
		if(threadpool != nullptr) delete threadpool;
		if(twS != nullptr) free(twS);
	}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		SINGLELOOPBEGIN(th)
			work(m);
		SINGLELOOPEND
	}
	inline void run()  {
		threadpool->executeTaskBatch(tid);
	}
	inline int returnCode() {return ret;}
};


