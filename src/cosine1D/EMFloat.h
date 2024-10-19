#pragma once
/*******************************************************************
Author: David Ge (dge893@gmail.com, aka Wei Ge)
Created: 11/12/2023
Allrights reserved by David Ge

********************************************************************/
#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/detail/default_ops.hpp>
#include <boost/multiprecision/number.hpp>
//#include <boost/multiprecision/detail/functions/constants.hpp>
#include <boost/rational.hpp>
#include <iostream>
#include <vector>
using namespace boost::multiprecision;
using boost::rational;

/*for cosine*/
#define EMDIGITS 5000
//

//*** cpp_bin_float is 1154 times faster than cpp_dec_float converting with cpp_int ********

//typedef number<gmp_float<EMDIGITS>> EMFLOAT;
typedef number<cpp_bin_float<EMDIGITS>> EMFLOAT;
//typedef number<cpp_dec_float<EMDIGITS>> EMFLOAT;

#define FLOATEMTODBL(d) (d.convert_to<double>())

#define FLOATEMTOSTR(c, dec) (c).str((dec), std::ios::fixed).c_str()

typedef cpp_int EMINT;
typedef rational<cpp_int> EMRATIONAL;
typedef unsigned short int EMINT16;
typedef unsigned int EMINT32;

//if it is enabled then EMFLOAT.cpp should be included in the makefile
//#define TESTEMTYPEIMPEXP
#ifdef TESTEMTYPEIMPEXP
void testEMFloatVectorExpImp();
void testEMFloatExpImp();

void emfloatexport(EMFLOAT &vf, FILE *fh);
void emfloatimport(EMFLOAT &vf, FILE *fh);

int emintexport(EMINT &vi, FILE *fh);
int emintimport(EMINT &vi, FILE *fh);
#endif

