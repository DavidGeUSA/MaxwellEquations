/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Created: 05/20/2024
Allrights reserved by David Ge

********************************************************************/
#include "triArrayThreadWork.h"

triArrayExecuter::triArrayExecuter(THREADINDEX threadCount, unsigned int arrayDim){
	twD = (ThreadDoubleLoop*)malloc(sizeof(ThreadDoubleLoop)*threadCount);
	divideDoubleLoop(twD, arrayDim-1, threadCount);
	threadpool = new ThreadBatch(threadCount);
	tid = threadpool->registerTask(this, fid);
	ret = 0;
}
triArrayExecuter::~triArrayExecuter(){
	if(threadpool != nullptr) delete threadpool;
	if(twD != nullptr) free(twD);
}
void triArrayExecuter::execute(THREADINDEX th, FUNCTIONID functionId) {
	DOUBLELOOPBEGIN(th)
		work(m,s);
	DOUBLELOOPEND
}
void triArrayExecuter::run() {
	threadpool->executeTaskBatch(tid);
}
int triArrayExecuter::returnCode() {
	return ret;
}	