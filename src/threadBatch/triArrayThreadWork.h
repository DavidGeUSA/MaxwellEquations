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
	explicit triArrayExecuter(THREADCOUNT threadCount, unsigned int arrayDim);
	~triArrayExecuter();
	virtual void execute(THREADINDEX th, FUNCTIONID functionId);
	void run();
	int returnCode();
};


