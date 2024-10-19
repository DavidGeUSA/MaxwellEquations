#pragma once
/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Created: 05/19/2024
Allrights reserved by David Ge

********************************************************************/
#include "ThreadBatch.h"
#include "loopDivide.h"

/*
triangle array:
m=0,1,...,Nmax
	s=0,1,...,m
assume operations on array elements can be done in parallel	
*/
class triArrayExecuter: public virtual TaskExecuter
{
protected:
	ThreadBatch* threadpool;
	ThreadDoubleLoop* twD;
	const FUNCTIONID fid = 0;
	TASKID tid;	
	int ret;
	virtual void work(unsigned int m, unsigned int s) = 0;
public:
	inline explicit triArrayExecuter(THREADCOUNT threadCount, unsigned int arrayDim) {
		twD = (ThreadDoubleLoop*)malloc(sizeof(ThreadDoubleLoop)*threadCount);
		divideDoubleLoop(twD, arrayDim-1, threadCount);
		threadpool = new ThreadBatch(threadCount);
		tid = threadpool->registerTask(this, fid);
		ret = 0;
	}
	inline ~triArrayExecuter() {
		if(threadpool != nullptr) delete threadpool;
		if(twD != nullptr) free(twD);
	}
	inline virtual void execute(THREADINDEX th, FUNCTIONID functionId) {
		DOUBLELOOPBEGIN(th)
			work(m,s);
		DOUBLELOOPEND
	}
	inline void run() {
		threadpool->executeTaskBatch(tid);
	}
	int returnCode() {
		return ret;
	}	
};


