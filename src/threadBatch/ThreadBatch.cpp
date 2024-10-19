/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Last modified: 01/19/2022
Allrights reserved by David Ge

a specialized thread pool for executing repeated small tasks in batches parallely
********************************************************************/
#include "ThreadBatch.h"
#include "ErrorThreadBatch.h"
#include "../FileUtil/fileutil.h"

/*
allocate memories for desired thread count
start threads in waiting status
*/
ThreadBatch::ThreadBatch(THREADCOUNT threadCount)
{
	stop = false;
	runningTasks = 0;
	batchIndexToExecute = -1;
	numOfthreads = threadCount;
	threadActive = (bool *)malloc(sizeof(bool)*threadCount);
	threadList = (std::thread **)malloc(sizeof(std::thread *)*threadCount);
	for (THREADINDEX th = 0; th < numOfthreads; th++)
	{
		threadActive[th] = false;
		threadList[th] = new std::thread(&ThreadBatch::run, this, (th));
	}
	//_fpreset();
}

/*
finish all threads and free all memories
*/
ThreadBatch::~ThreadBatch()
{
	if (threadList != NULL)
	{
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			stop = true;
		}
		condition.notify_all();
		for (THREADINDEX th = 0; th < numOfthreads; th++)
		{
			threadList[th]->join();
		}
		for (THREADINDEX th = 0; th < numOfthreads; th++)
		{
			delete threadList[th];
		}
		free(threadList);
		free(threadActive);
	}
	for (size_t m = 0; m < batchList.size(); m++)
	{
		if (batchList[m] != NULL)
		{
			for (THREADINDEX th = 0; th < numOfthreads; th++)
			{
				if (batchList[m][th] != NULL)
				{
					delete batchList[m][th];
				}
			}
			delete[] batchList[m];
			batchList[m] = NULL;
		}
	}
}

/*
add a batch of tasks. use the return value to execute this batch of tasks
*/
TASKID ThreadBatch::addTaskBatch(TaskHolder **taskBatch)
{
	unsigned int currentCount = (unsigned int) batchList.size();
	batchList.push_back(taskBatch);
	return currentCount;
}

/*
thread function
index is an unique thread index in a range of [0, 1, 2,...,numOfthreads-1]
*/
void ThreadBatch::run(THREADINDEX index)
{
//	if (!setThreadHighPtiority())
//	{
//#ifdef _DEBUG
//		puts("\n Failed setting thread priority");
//#endif
//	}
	for (;;)
	{
		TaskHolder *task = NULL;
		{
			//create a lock to wait for
			std::unique_lock<std::mutex> lock(this->queue_mutex);
			//wait for notification to arrive
			this->condition.wait(lock,
				[this, index]{ return this->stop || (this->batchIndexToExecute >= 0 && threadActive[index]); });
			if (this->stop && this->batchIndexToExecute < 0)
				return; //the whole pool stops
			if (threadActive[index])
			{
				//the task is asked to be executed
				task = batchList[batchIndexToExecute][index];
			}
		} //release the lock

		//execute the task
		task->execute();

		{
			//update running task count
			std::unique_lock<std::mutex> lock(this->queue_mutex);
			threadActive[index] = false;
			runningTasks--;
		}
		//notify the waiting thread
		this->conditionWait.notify_one();
	}//go back into waiting state
}

/*
execute a batch of tasks identified by taskId.
the tasks are executed in parallel.
the function waits for all tasks to finish.
*/
int ThreadBatch::executeTaskBatch(TASKID taskId )
{
	int batchId = taskId;
	//verify the batchId
	if (batchId < 0 || (size_t)batchId >= batchList.size())
	{
		puts("\nINVALID TASKID\n");
		throw;
		//return ERR_INVALID_TASKID;
	}
	else
	{
		//prepare to wakeup the threads
		std::unique_lock<std::mutex> lock(this->queue_mutex);
		runningTasks = numOfthreads;
		batchIndexToExecute = batchId;
		for (THREADINDEX th = 0; th < numOfthreads; th++)
		{
			threadActive[th] = true;
		}
	}
	this->condition.notify_all(); //wakeup all threads to do the job
	{
		//wait for all tasks to finish
		std::unique_lock<std::mutex> lock(this->queue_mutex);
		this->conditionWait.wait(lock,
			[this]{ return this->stop || runningTasks == 0; });
		batchIndexToExecute = -1;
	}
	return 0;
}

/*
translate a FUNCTIONID to a TASKID
the FUNCTIONID is local to the TaskExecuter
the returned TASKID is global
use the TASKID to execute the function in parallel threads by the TaskExecuter
*/
TASKID ThreadBatch::registerTask(TaskExecuter *exe, FUNCTIONID functionID)
{
	TaskHolder **batch = new TaskHolder*[numOfthreads];
	for (THREADINDEX th = 0; th < numOfthreads; th++)
	{
		batch[th] = new TaskHolder(exe, th, functionID);//create a task holder
	}
	TASKID id = addTaskBatch(batch);
	return id;
}

/*
remove the task from the pool
calling of this function is not neccessary, at least for now
*/
void ThreadBatch::deregisterTask(TASKID taskId)
{
	size_t m = (size_t)taskId;
	if (m < batchList.size())
	{
		TaskHolder **batch = batchList[m];
		if (batch != NULL)
		{
			for (THREADINDEX th = 0; th < numOfthreads; th++)
			{
				if (batch[th] != NULL)
				{
					delete batch[th];
					batch[th] = NULL;
				}
			}
			delete[] batch;
			batchList[m] = NULL;
		}
	}
}

ThreadBatch* pool = nullptr;

unsigned int getAvailableThreadCount()
{
	unsigned int c = std::thread::hardware_concurrency();
	if(c == 0)
		return 30;
	return c;
}

int checkCreateThreadBatch()
{
	if(pool == nullptr)
	{
		pool = new ThreadBatch(getAvailableThreadCount());
	}
	if(pool != nullptr)
	{
		return ERR_OK;	
	}
	return ERR_TB_CREATE_POOL;
}

void removeThreadPool()
{
	if(pool != nullptr)
	{
		delete pool;
		pool = nullptr;	
	}
}

ThreadBatch* getThreadPool(int *ret)
{
	*ret = checkCreateThreadBatch();
	return pool;
}

unsigned int getThreadCount()
{
	if(pool != nullptr)
	{
		return pool->threadCount();	
	}
	return 0;
}	
	
