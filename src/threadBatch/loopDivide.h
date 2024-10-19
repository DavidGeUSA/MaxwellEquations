#pragma once
/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Created: 02/22/2024
Allrights reserved by David Ge

********************************************************************/

typedef struct ThreadSingleLoop
{
	unsigned int m0;
	unsigned int m1;
}ThreadSingleLoop;

typedef struct ThreadDoubleLoop
{
	unsigned int main0;
	unsigned int main1;
	unsigned int sub0;
	unsigned int sub1;
}ThreadDoubleLoop;

#define DOUBLELOOPCOUNT(n) ((static_cast<size_t>(n)+1)*(static_cast<size_t>(n)+2)/2)
#define TRIPLELOOPCOUNT(n) ( static_cast<size_t>(n)*(static_cast<size_t>(n)+1)*(static_cast<size_t>(n)+5)/6+static_cast<size_t>(n)+1) 


/*
each pair of macros must be used in one thread. do not use more than one pair of macros in one thread.
*/

/*
loop type 1

loop: 0,1,...,dim-1

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
a local variable m is declared to represent the work to be executed by the thread

*/
#define SINGLELOOPBEGIN(th) if(twS[th].m0<=twS[th].m1) {for(unsigned int m=twS[th].m0;m<=twS[th].m1;m++){ 

#define SINGLELOOPEND }}

/*
loop type 2

loop: m=0,1,...,dim-1; n=m,m+1,...,dim-1
only outer loop in parallel

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
local variables m and n are declared to represent the work to be executed by the thread
m=m0,...,m1: n=m,...,Nmax

*/
#define SINGLEDOUBLELOOPBEGIN(th, Nmax) if(twSd[th].m0<=twSd[th].m1) {for(unsigned int m=twSd[th].m0;m<=twSd[th].m1;m++){\
	for(unsigned int n=m;n<=(Nmax);n++) {

#define SINGLEDOUBLELOOPEND }}}
/*
same as above but remove 	for(unsigned int n=m;n<=(Nmax);n++) {
	custom code is 	for(unsigned int n=m;n<=(Nmax);n++) {...}
*/
#define SINGLEDOUBLELOOPSINGLEBEGIN(th) if(twSd[th].m0<=twSd[th].m1) {for(unsigned int m=twSd[th].m0;m<=twSd[th].m1;m++){

#define SINGLEDOUBLELOOPSINGLEEND }}

/*
loop type 3

loop: m=0,1,...,dim-1; n=0,1,...,m
only outer loop in parallel

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
local variables m and n are declared to represent the work to be executed by the thread
m=m0,...,m1: n=0,...,m

*/
#define SINGLEDOUBLELOOP_0m_BEGIN(th) if(twS0[th].m0<=twS0[th].m1) {for(unsigned int m=twS0[th].m0;m<=twS0[th].m1;m++){\
	for(unsigned int n=0;n<=m;n++) {

#define SINGLEDOUBLELOOP_0m_END }}

/*
same as above but remove for(unsigned int n=0;n<=m;n++) {
	custom code include for(unsigned int n=0;n<=m;n++) {...}
*/
#define SINGLEDOUBLELOOP_0mSINGLE_BEGIN(th) if(twS0[th].m0<=twS0[th].m1) {for(unsigned int m=twS0[th].m0;m<=twS0[th].m1;m++){
	

#define SINGLEDOUBLELOOP_0mSINGLE_END }}

/*
loop type 4

loop: m=0,1,...,dim-1; n=m,m-1,...,0
only outer loop in parallel

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
local variables m and n are declared to represent the work to be executed by the thread
m=m0,...,m1: n=m,...,0

*/
#define SINGLEDOUBLELOOP_m0_BEGIN(th) if(twS0[th].m0<=twS0[th].m1) {for(unsigned int m=twS0[th].m0;m<=twS0[th].m1;m++){\
	for(int n=(int)m;n>=0;n--) {

#define SINGLEDOUBLELOOP_m0_END }}}

/*
same as above but remove for(int n=(int)m;n>=0;n--) {
*/
#define SINGLEDOUBLELOOP_m0SINGLE_BEGIN(th) if(twS0[th].m0<=twS0[th].m1) {for(unsigned int m=twS0[th].m0;m<=twS0[th].m1;m++){
	

#define SINGLEDOUBLELOOP_m0SINGLE_END }}


/*
loop type 5_a

loop: m=0,1,...,dim1-1; s=0,1,...,dim2-1
both loops in parallel

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
local variables m and s are declared to represent the work to be executed by the thread

*/
#define DOUBLE2DLOOPBEGIN(th, twD2d, Nmax2) if(twD2d[th].main0<=twD2d[th].main1) for(unsigned int m=twD2d[th].main0;m<=twD2d[th].main1;m++){\
	unsigned int s0,s1;\
	if(m==twD2d[th].main0){s0=twD2d[th].sub0;}else{s0=0;}\
	if(m==twD2d[th].main1){s1=twD2d[th].sub1;}else{s1=Nmax2;}\
	for(unsigned int s=s0;s<=s1;s++){

#define DOUBLE2DLOOPEND }}


/*
loop type 5

loop: m=0,1,...,dim-1; s=0,1,...,m
both loops in parallel

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
local variables m and s are declared to represent the work to be executed by the thread

*/
#define DOUBLELOOPBEGIN(th) if(twD[th].main0<=twD[th].main1) for(unsigned int m=twD[th].main0;m<=twD[th].main1;m++){\
	unsigned int s0,s1;\
	if(m==twD[th].main0){s0=twD[th].sub0;}else{s0=0;}\
	if(m==twD[th].main1){s1=twD[th].sub1;}else{s1=m;}\
	for(unsigned int s=s0;s<=s1;s++){

#define DOUBLELOOPEND }}

/*
loop type 5b

loop: m=0,1,...,dim-1; s=0,1,...,dim-1-m
both loops in parallel

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
local variables m and s are declared to represent the work to be executed by the thread

*/
#define DOUBLELOOPBEGINb(N0,th) if(twD[th].main0<=twD[th].main1) for(unsigned int m=twD[th].main0;m<=twD[th].main1;m++){\
	unsigned int s0,s1;\
	if(m==twD[th].main0){s0=twD[th].sub0;}else{s0=0;}\
	if(m==twD[th].main1){s1=twD[th].sub1;}else{s1=N0-m;}\
	for(unsigned int s=s0;s<=s1;s++){

#define DOUBLELOOPENDb }}

/*
loop type 6

loop: s=0,1,...,dim-1; m=s,s+1,...,dim-1
both loops in parallel

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
local variables m and s are declared to represent the work to be executed by the thread
*/
#define DOUBLELOOPINVERSEBEGIN(th, Nmax) if(twDs[th].sub0<=twDs[th].sub1) for(unsigned int s=twDs[th].sub0;s<=twDs[th].sub1;s++){\
	unsigned int m0,m1;\
	if(s==twDs[th].sub0){m0=twDs[th].main0;}else{m0=s;}\
	if(s==twDs[th].sub1){m1=twDs[th].main1;}else{m1=(Nmax);}\
	for(unsigned int m=m0;m<=m1;m++){

#define DOUBLELOOPINVERSEEND }}

/*
loop type 7

loop: m=0,...,dim-1:s=0,...,m;n=0,...,s
the first 2 loops in parallel

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
local variables m, s and n are declared to represent the work to be executed by the thread
m=m0,...,m1:s=s0,...,s1
n=0,...,s
*/
#define TRIPLELOOPBEGIN(th) if(twT[th].main0<=twT[th].main1) for(unsigned int m=twT[th].main0;m<=twT[th].main1;m++){\
	unsigned int s0,s1;\
	if(m==twT[th].main0){s0=twT[th].sub0;}else{s0=0;}\
	if(m==twT[th].main1){s1=twT[th].sub1;}else{s1=m;}\
	for(unsigned int s=s0;s<=s1;s++){\
	for(unsigned int n=0;n<=s;n++){

#define TRIPLELOOPEND }}}

/*
same as above but remove for(unsigned int n=0;n<=s;n++){
*/
#define TRIPLELOOP2BEGIN(th) if(twT[th].main0<=twT[th].main1) for(unsigned int m=twT[th].main0;m<=twT[th].main1;m++){\
	unsigned int s0,s1;\
	if(m==twT[th].main0){s0=twT[th].sub0;}else{s0=0;}\
	if(m==twT[th].main1){s1=twT[th].sub1;}else{s1=m;}\
	for(unsigned int s=s0;s<=s1;s++){

#define TRIPLELOOP2END }}

/*
loop type 8

loop: s=0,...,dim-1:m=s,...,dim-1;n=m,m+1,...,dim-1
the first 2 loops in parallel

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
local variables m, s and n are declared to represent the work to be executed by the thread
s=s0,...,s1:m=m0,...,m1
n=m,m+1,...,dim-1
*/
#define TRIPLELOOPINVERSEBEGIN(th, Nmax) if(twTs[th].sub0<=twTs[th].sub1) for(unsigned int s=twTs[th].sub0;s<=twTs[th].sub1;s++){\
	unsigned int m0,m1;\
	if(s==twTs[th].sub0){m0=twTs[th].main0;}else{m0=s;}\
	if(s==twTs[th].sub1){m1=twTs[th].main1;}else{m1=(Nmax);}\
	for(unsigned int m=m0;m<=m1;m++){\
	for(unsigned int n=m;n<=(Nmax);n++){

#define TRIPLELOOPINVERSEEND }}}

/*
same as above but remove for(unsigned int n=m;n<=(Nmax);n++){
*/
#define TRIPLELOOPINVERSE2BEGIN(th, Nmax) if(twTs[th].sub0<=twTs[th].sub1) for(unsigned int s=twTs[th].sub0;s<=twTs[th].sub1;s++){\
	unsigned int m0,m1;\
	if(s==twTs[th].sub0){m0=twTs[th].main0;}else{m0=s;}\
	if(s==twTs[th].sub1){m1=twTs[th].main1;}else{m1=(Nmax);}\
	for(unsigned int m=m0;m<=m1;m++){

#define TRIPLELOOPINVERSE2END }}

/*
loop type 8a

loop: s=0,...,dim-1:m=s,...,dim-1;n=0,1,...,dim-1-m
the first 2 loops in parallel

th is the thread index. for simplicity, th should be a single variable to avoid compilation errors
local variables m, s and n are declared to represent the work to be executed by the thread
s=s0,...,s1:m=m0,...,m1
n=0,1,...,dim-1-m
*/
#define TRIPLELOOPINVERSEBEGIN2(th, Nmax) if(twTs[th].sub0<=twTs[th].sub1) for(unsigned int s=twTs[th].sub0;s<=twTs[th].sub1;s++){\
	unsigned int m0,m1;\
	if(s==twTs[th].sub0){m0=twTs[th].main0;}else{m0=s;}\
	if(s==twTs[th].sub1){m1=twTs[th].main1;}else{m1=(Nmax);}\
	for(unsigned int m=m0;m<=m1;m++){\
	for(unsigned int n=0;n<=(Nmax)-m;n++){

#define TRIPLELOOPINVERSEEND2 }}}

/*
same as above but remove for(unsigned int n=0;n<=(Nmax)-m;n++){
*/
#define TRIPLELOOPINVERSE2BEGIN2(th, Nmax) if(twTs[th].sub0<=twTs[th].sub1) for(unsigned int s=twTs[th].sub0;s<=twTs[th].sub1;s++){\
	unsigned int m0,m1;\
	if(s==twTs[th].sub0){m0=twTs[th].main0;}else{m0=s;}\
	if(s==twTs[th].sub1){m1=twTs[th].main1;}else{m1=(Nmax);}\
	for(unsigned int m=m0;m<=m1;m++){

#define TRIPLELOOPINVERSE2END2 }}

void divideSingleLoop(ThreadSingleLoop *twS, unsigned int Nmax, unsigned int threadCount);
void divideSingleDoubleLoop_mN(ThreadSingleLoop *twSd, unsigned int Nmax, unsigned int threadCount);
void divideSingleDoubleLoop_0m(ThreadSingleLoop *twSf, unsigned int Nmax, unsigned int threadCount);
void divide2DLoop(ThreadDoubleLoop *twD, unsigned int dim1, unsigned int dim2, unsigned int threadCount);
void divideDoubleLoop(ThreadDoubleLoop *twD, unsigned int Nmax, unsigned int threadCount);
void divideDoubleLoop2(ThreadDoubleLoop *twD, unsigned int Nmax, unsigned int threadCount);
void divideDoubleLoopSubFirst(ThreadDoubleLoop *twDs, unsigned int Nmax, unsigned int threadCount);
void divideTripleLoop(ThreadDoubleLoop *twT, unsigned int Nmax, unsigned int threadCount);
void divideTripleLoopSubFirst(ThreadDoubleLoop *twTs, unsigned int Nmax, unsigned int threadCount);

//#define _DEBUGLOOPDIVIDER
#ifdef _DEBUGLOOPDIVIDER
int testCountSingleLoop(ThreadSingleLoop *twS, unsigned int Nmax, unsigned int threadCount);
int testCountSingleDoubleLoop(ThreadSingleLoop *twSd, unsigned int Nmax, unsigned int threadCount);
int testCountSingleDoubleLoop_0m(ThreadSingleLoop *twS0, unsigned int Nmax, unsigned int threadCount);
int testCountSingleDoubleLoop_m0(ThreadSingleLoop *twS0, unsigned int Nmax, unsigned int threadCount);
int testCountDoubleLoop(ThreadDoubleLoop *twD, unsigned int Nmax, unsigned int threadCount);
int testCountDoubleLoopInverse(ThreadDoubleLoop *twDs, unsigned int Nmax, unsigned int threadCount);
int testCountTripleLoop(ThreadDoubleLoop *twT, unsigned int Nmax, unsigned int threadCount);
int testCountTripleLoopInverse(ThreadDoubleLoop *twTs, unsigned int Nmax, unsigned int threadCount);
#endif



