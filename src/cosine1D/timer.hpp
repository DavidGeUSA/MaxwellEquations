#pragma once
/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge, David Wei Ge)
Created: 05/07/2024
Allrights reserved by David Ge

********************************************************************/
#include <ctime>
#include <time.h>

/*
usage:
	Timer_cpu t;
	......
	double duration = t.time_secs();
	......
	t.reset();
	......
	duration = t.time_secs();
*/
class Timer_cpu
{
protected:
	std::clock_t start;
public:
	inline Timer_cpu() {
		start = std::clock();
	}
	inline ~Timer_cpu(){}
	inline void reset() {
		start = std::clock();	
	}
	inline double time_secs() {
		return (std::clock() - start) / (double)CLOCKS_PER_SEC;	
	}
};

class Timer_wall
{
protected:
	struct timespec start;
	struct timespec finish;
public:
	inline Timer_wall() {
		clock_gettime(CLOCK_MONOTONIC, &start);
	}
	inline ~Timer_wall(){}
	inline void reset() {
		clock_gettime(CLOCK_MONOTONIC, &start);	
	}
	inline double time_secs() {
		clock_gettime(CLOCK_MONOTONIC, &finish);
		return (finish.tv_sec - start.tv_sec) +  (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	}
};
#ifdef USE_OPENMP
#include <omp.h>
class Timer_omp
{
protected:
	double start;
public:
	inline Timer_omp() {
		start = omp_get_wtime();
	}
	~Timer_omp(){}
	inline void reset() {
		start = omp_get_wtime();	
	}
	inline double time_secs() {
		return (omp_get_wtime() - start);	
	}	
};
#endif

