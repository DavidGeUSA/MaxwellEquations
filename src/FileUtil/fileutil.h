#ifndef __FILEUTIL_H__
#define __FILEUTIL_H__
/*******************************************************************
	Author: David Ge (dge893@gmail.com, aka Wei Ge)
	Last modified: 03/31/2018
	Allrights reserved by David Ge

	Modifications
	Date            Author      Description
	---------------------------------------------
	2021-03-06      David Ge    Linux compatibility

********************************************************************/
#include <cstdio>
//#include "../include/env.h"

#include "ErrorFileUtil.h"

//#define _DEBUG_DETAIL_

int formFilePath(char *filepath, size_t size, const char *path, const char *name);
int getFilename(const char *filepath, char * filename, size_t targetSize);
int getFileFolder(const char *filepath, char * fileFolder, size_t targetSize);
int WriteToFile(char *filename, void* data, size_t itemSize, size_t count, int *errCode);

int openfileRead(const char *file, FILE **handle);
int openfileWrite(const char *file, FILE **handle);
int openTextfileWrite(const char *file, FILE **handle);
int openTextfileAppend(const char *file, FILE **handle);
int openTextfileRead(const char *file, FILE **handle);
int writefile(FILE *handle, const void *data, unsigned int size);
int readfile(FILE *handle, void *data, unsigned int size);
void closefile(FILE *handle);

int deleteFile(const char * file);

bool directoryExists(const char *dir);
bool createdirectory(const char *dir);
bool fileexists(const char *fileName);
bool FileExists(const wchar_t *fileName);
long long int fileSize(const char* name);

int GetCurrentFolder(char *appfolder, unsigned long size);

int formDataFileName(char *filename, size_t size, const char *basename, size_t timeIndex);
int formFieldFileName(char *filename, size_t size, const char *basename, size_t timeIndex);

double *parseDecimals(char *name_argv, int *argc, int *ret);

int copyC2W(wchar_t *dest, size_t size, const char *src);
int catC2W(wchar_t *dest, size_t size, const char *src);
int copyW2C(char *dest, size_t size, const wchar_t *src);

int strcpy_0(char *dest, size_t size, const char *src);
int strcat_0(char *dest, size_t size, const char *src);
int strcmpi_0(const char * str1, const char * str2);
//int sprintf_0(char *buff, size_t size, const char *format, ...);
size_t strnlen_0(char *buff, size_t size);

size_t getTimeTick();
bool setProcessHighPtiority();
bool setThreadHighPtiority();

//
//#ifdef __linux__
//#define sprintf_1(buff, size, format, ...) (snprintf(buff, size, format, ...)<=0?0:ERR_STR_PRF)
//#elif _WIN32
//#define sprintf_1(buff, size, format, ...) (sprintf_s(buff, size, format, ...)<=0?0:ERR_STR_PRF)
//#else
//#endif

#ifdef __linux__
#define sprintf_1 snprintf
#elif _WIN32
#define sprintf_1 sprintf_s
#else
#endif
#endif
/*
#ifdef __linux__

#elif _WIN32

#else
#endif
<=0?ERR_STR_PRF:ERR_OK
*/