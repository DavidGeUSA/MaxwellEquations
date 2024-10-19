#pragma once
/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Created: 05/31/2024
Allrights reserved by David Ge

********************************************************************/
#include "ThreadBatch.h"
#include "loopDivide.h"
#include "ErrorThreadBatch.h"

/*
base class for doing loop work
it uses registerTask and runTask to hide the thread pool

implement an abstract function, divideWork, to construct the work loads based on dim and threadCount
*/
class loopWork: public TaskExecuter
{
protected:
	int ret;
	unsigned int dim;
	inline TASKID registerTask(FUNCTIONID functionID) {
		if(ret == 0) {
			ThreadBatch* threadpool = getThreadPool(&ret);
			if(ret == 0)
			{
				return threadpool->registerTask(this, functionID);
			};		
		}
		return 0;		
	}
	inline void runTask(TASKID tid) {
		if(ret == 0) {
			ThreadBatch* threadpool = getThreadPool(&ret);
			if(ret == 0)
			{
				threadpool->executeTaskBatch(tid);
			};		
		}
	}
	inline virtual void divideWork() = 0;
public:
	inline loopWork(unsigned int workCount):dim(workCount) {
		if(workCount == 0)
		{
			ret = ERR_TB_INV_SIZE;		
		}
		else
		{
			ret = ERR_OK;		
		}
	}
	inline ~loopWork(){}
	inline unsigned int threadCount() {
		if(ret == 0) {
			ThreadBatch* threadpool = getThreadPool(&ret);
			if(ret == 0)
			{
				return threadpool->threadCount();
			};		
		}
		return 0;	
	}
	inline int returnCode() {return ret;}
};

