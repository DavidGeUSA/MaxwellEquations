#pragma once

/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Last modified: 01/19/2022
Allrights reserved by David Ge

a specialized thread pool for executing repeated tasks in batches parallely.
the purpose is to avoid dynamic memory allocations, as required by a generic purpose thread pool.

1. create a ThreadBatch object with desired number of threads
2. derive subclasses from TaskExecuter, implementing TaskExecuter.execute(threadIndex, functionId) to execute functions in parallel
	note that, unlike most other pool implementations, the code to be executed can be determined by threadIndex and functionId.
	therefore, it is not to hand jobs to a thread pool; it is to invoke a set of predefined jobs. 
	it is predefined to run how many threads, thus the name of thread batch.
3. call registerTask to register a function identified by an integer ID, functionId, local to an executer, 
	it returns a globally unique task ID. 
4. call executeTaskBatch, passing a task ID returned from calling of registerTask. 
	function TaskExecuter.execute(threadIndex, functionId) is called as many times as the desired number of threads, parallely,
	threadIndex = 0, 1, 2, ..., number of threads - 1
	on returning, all threads finish executing the corresponding Executer.execute(threadIndex, functionId)

********************************************************************/
/*******************************************************************
05/26/2024 David Ge
The above description outlines a "low-level" usage.
To make a "high-level" usage, 4 files are added for 8 types of for-loops.
8 classes are provided, each for one type of for-loop. This "high-level" usage hides all the threading details.

threadWorkSingleLoop.hpp
	loop type 1: m=0,1,...,N
threadWorkSingleDblLoop.hpp
	only outer loop runs in parallel
	loop type 2: m=0,1,...,N; n=m,m+1,...,N
	loop type 3: m=0,1,...,N; n=0,1,...,m
	loop type 4: m=0,1,...,N; n=m,m-1,...,0
threadWorkDoubleLoop.hpp
	outer and inner loops run in parallel	
	loop type 5: m=0,1,...,N; s=0,1,...,m
	loop type 6: s=0,1,...,N; m=s,s+1,...,N
threadWorkTripleLoop.hpp
	the outer two loops run in parallel
	loop type 7: m=0,...,N:s=0,...,m;n=0,...,s
	loop type 8: s=0,...,N:m=s,...,N;n=m,m+1,...,N
	
The purpose of these classes are making balanced work load on each thread.
Except for the single for-loop, I am not sure whether Open MP can do the same work load balance.
Comparing with Open MP, for the single for-loop the performance is the same; for other types of loops the performance is increased by 30%.
The reason for the performance increase is that the dimensions of the inner for-loops are not fixed as assumed by Open MP (I assume); 
work loads for the outer for-loop are not evenly divided among the dimension.

Usage:
	1. derive a new class for a loop work from a base class; the base class is chosen based on the type of the for-loop
	2. implement a virtual function to perform the work. in case of an execution error, set an error code to a member variable named ret.
	3. create a function to pass needed variables into the class, to prepare for the work
	4. call a member function run() to start multi-thread execution of the work. it will wait for all threads to finish.
	5. call a member function returnCode() to get an integer representing the error code. 0 is success. non-0 is an error code.
	
Because it is one class for one type of loop work, it makes sense to use inline functions	 	
********************************************************************/

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#define ERR_INVALID_TASKID 100

/*
function identifier
*/
typedef unsigned int TASKID; //global ID
typedef int FUNCTIONID;      //local ID 

/*
thread identifier
*/
typedef unsigned int THREADCOUNT; //number of threads
typedef unsigned int THREADINDEX; //number of threads

/*
interface of task executers
*/
class TaskExecuter
{
public:
	TaskExecuter(){}
	~TaskExecuter(){}
	virtual void execute(THREADINDEX threadIndex, FUNCTIONID functionId) = 0;
};

/*
holder of task executers
*/
class TaskHolder
{
private:
	THREADINDEX  threadIndex;
	FUNCTIONID   functionId;
	TaskExecuter *task;
public:
	TaskHolder(TaskExecuter *exe, THREADINDEX th, FUNCTIONID fid){ task = exe; threadIndex = th; functionId = fid; }
	~TaskHolder(){}
	void execute()
	{
		task->execute(threadIndex, functionId);
	}
};

/*
thread pool for executing tasks parallely
*/
class ThreadBatch
{
private:
	std::thread **threadList;  //thread pool
	THREADCOUNT numOfthreads;  //count of threadList
	bool *threadActive;        //wakeup each thread
	//
	int batchIndexToExecute; //0,...,taskList.size-1
	std::vector<TaskHolder **> batchList; //batchList[batchIndexToExecute][threadIndex]->execute();
	//
	std::mutex queue_mutex;
	std::condition_variable condition;
	std::condition_variable conditionWait;
	int runningTasks;
	bool stop;
	TASKID addTaskBatch(TaskHolder **taskBatch);
	void run(THREADINDEX index);
public:
	ThreadBatch(THREADCOUNT threadCount);
	virtual ~ThreadBatch();
	THREADCOUNT threadCount(){ return numOfthreads; }
	TASKID registerTask(TaskExecuter *exe, FUNCTIONID functionID);
	void deregisterTask(TASKID taskId);
	int executeTaskBatch(TASKID taskId);
};

ThreadBatch* getThreadPool(int *ret);                                                                 



