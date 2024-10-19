#pragma once
/*******************************************************************
Author: David Wei Ge (dge893@gmail.com, aka Wei Ge)
Created: 08/28/2024
Allrights reserved by David Ge

********************************************************************/
#include "../threadBatch/threadWorkDoubleLoop.hpp"
#include "../threadBatch/threadWorkTripleLoop.hpp"
#include "../threadBatch/threadWorkSingleDblLoop.hpp"
#include "../threadBatch/threadWorkSingleLoop.hpp"

//for define static variables
#define MAXTHREADCOUNT 32

#ifdef USEWORKARRAY
//Nmax0=2000, DblIdxCount=2003001, Time used: 0.218812 seconds, average time: 1.09242e-07 seconds
template <typename T> class th_copyMem : public virtual doubleLoopWorkOne
{
protected:
	size_t* idxHeader;
	T* src;
	T* tgt;
	inline virtual void work(unsigned int m, unsigned int s) {
		size_t w = idxHeader[m] + s;
		tgt[w] = src[w];
	}
	
public:
	inline th_copyMem(unsigned int workCount): doubleLoopWorkOne(workCount) {
		idxHeader = nullptr;
		src = nullptr;
		tgt = nullptr;	
	}
	inline virtual ~th_copyMem() {}
	inline void copy(T* copyFrom, T* copyTo, size_t* indexHeader) {
		src = copyFrom;
		tgt = copyTo;
		idxHeader = indexHeader;
		run();
	}
};
#endif

#ifdef GENERATECONSTARRAY

class th_gen_w : public virtual doubleLoopWork2One
{
protected:
	virtual void work(unsigned int m, unsigned int s);
	
public:
	inline th_gen_w(unsigned int workCount): doubleLoopWork2One(workCount) {
			
	}
	inline ~th_gen_w() {}
};

class th_gen_ws : public virtual doubleLoopWorkOne
{
protected:
	virtual void work(unsigned int m, unsigned int s);
	
public:
	inline th_gen_ws(unsigned int workCount): doubleLoopWorkOne(workCount) {
			
	}
	inline ~th_gen_ws() {}
};


#endif

#ifdef VERIFYCONSTARRAY

class th_verify_w : public virtual doubleLoopWork2One
{
protected:
	virtual void work(unsigned int m, unsigned int s);
	
public:
	inline th_verify_w(unsigned int workCount): doubleLoopWork2One(workCount) {
			
	}
	inline ~th_verify_w() {}
};

class th_verify_ws : public virtual doubleLoopWorkOne
{
protected:
	virtual void work(unsigned int m, unsigned int s);
	
public:
	inline th_verify_ws(unsigned int workCount): doubleLoopWorkOne(workCount) {
			
	}
	inline ~th_verify_ws() {}
};

#endif
#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(CREATETIMEARRAYS)
/*
class th_gen_Q : public singleLoopWorkOne
{
protected:
	virtual void work(unsigned int n);
	
public:
	inline th_gen_Q(unsigned int workCount): singleLoopWorkOne(workCount) {
			
	}
	inline ~th_gen_Q() {}
};
*/
/*
	loop type 8
	
	loop: s=0,...,dim-1:m=s,...,dim-1;n=m,m+1,...,dim-1
	the first 2 loops in parallel
	
	th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
	local variables m, s and n are declared to represent the work to be executed by the thread
	s=s0,...,s1:m=m0,...,m1
	n=m,m+1,...,dim-1
	can also be
	n=0,1,...,dim-1-m
*/
class th_gen_U : public virtual tripleLoopWorkSubFirstOne
{
protected:
	/*
		implement should include for(unsigned int n=m;n<=(Nmax);n++){...}
		or	for(unsigned int n=0;n<=(Nmax)-m;n++){...}
	*/
	virtual void work(THREADINDEX th, unsigned int s, unsigned int m);
	
public:
	inline th_gen_U(unsigned int workCount): tripleLoopWorkSubFirstOne(workCount) {
			
	}
	inline virtual ~th_gen_U() {}
};
#endif

#if defined(ONEPOINT) || defined(RUNSIMULATION) || defined(USETIMEARRAYS)	

class th_gen_Dp : public virtual singleDbl0mLoopWorkOne
{
protected:
	/*
		implementation should include for(unsigned int n=0;n<=m;n++) {...}	
	*/
	virtual void work(unsigned int m);
public:
	inline th_gen_Dp(unsigned int workCount): singleDbl0mLoopWorkOne(workCount) {
			
	}
	inline virtual ~th_gen_Dp() {}
};

class th_gen_Dq : public virtual singleDbl0mLoopWorkOne
{
protected:
	/*
		implementation should include for(unsigned int n=0;n<=m;n++) {...}	
	*/
	virtual void work(unsigned int m);
public:
	inline th_gen_Dq(unsigned int workCount): singleDbl0mLoopWorkOne(workCount) {
			
	}
	inline virtual ~th_gen_Dq() {}
};

class th_gen_B : public virtual singleDoubleLoopWorkOne
{
protected:
	/*
		inside this code there should be a structure of for(unsigned int n=m;n<=(Nmax);n++) {...}		
	*/
	virtual void work(unsigned int m);
public:
	inline th_gen_B(unsigned int workCount): singleDoubleLoopWorkOne(workCount) {
			
	}
	inline virtual ~th_gen_B() {}
};

class th_gen_A0 : public virtual singleLoopWorkOne
{
protected:
	virtual void work(unsigned int m);
public:
	inline th_gen_A0(unsigned int workCount): singleLoopWorkOne(workCount) {
			
	}
	inline virtual ~th_gen_A0() {}
};

class th_gen_A1 : public virtual singleLoopWorkOne
{
protected:
	virtual void work(unsigned int m);
public:
	inline th_gen_A1(unsigned int workCount): singleLoopWorkOne(workCount) {
			
	}
	inline virtual ~th_gen_A1() {}
};
#endif







