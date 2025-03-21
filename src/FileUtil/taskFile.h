#ifndef __TASKFILE_H__
#define __TASKFILE_H__

/*******************************************************************
	Author: David Ge (dge893@gmail.com, aka Wei Ge)
	Last modified: 03/31/2018
	Allrights reserved by David Ge

********************************************************************/

/*
	a task file contains lines of strings
	each line is formed by "name"="value"
	"name" is a string. All names in a task file are unique.
	"value" is a value of a primary type, i.e. double, integer, unsigned integer, long integer, Boolean, and string.
	some types of array and structures are also supported by "value"
	
	a line can be a comment if it starts with "//"
	
	a part of the task file is disabled or is treated as a comment if it starts with a line starting with two letters of "/" and "*", 
	and ends with a line starting with two letters of "*" and "/".
	using a line starting with two letters ("/" and "*", and "*" and "/") is to make the markers clear for human reading. 
	the purpose of this feature is for the user to easily disable and enable parts of the file.
	using this feature, one em.env file may contain many parts, each part for one execution of the EM program.

	each module, i.e. simulation console, boundary condition, TFSF, FDTD, field source, and initial condition, defines names and expected values

	a task file defines a task by the values assigned to names

	this module provides functions, get???(name, optional), for searching and parsing values in a task file,
	where "name" identifies a task parameter.

	if a given "name" does not exist in a task file then a function get???(name, optional) returns a default value;
	the default value for a string is NULL; the default value for a number is 0; the default value for a Boolean is false;
	function getErrorCode returns ERR_OK if "optonal" is true; 
	function getErrorCode returns ERR_TASK_PARAMETER_NAME if "optonal" is false, and subsequent calling of get???(name, optional) will do nothing.

	the designed usage of this class is that when get???(name, false) is called (that is, optional is false) then getErrorCode should be called to
	check whether the name exists; if the name does not exist then the program should bail out and return the error code. But you do not
	have to call getErrorCode after each call of get???(name, optional); you may make calls of get???(name, optional) for all the task parameters
	and then make one call of getErrorCode to do checking.

	if you want to know whether a "name" exists but also want to treat the task parameter as optional then use false for the "optional" when 
	calling get???(name, optional), and call getErrorCode to examine whether the "name" exist, then call resetErrorCode before calling get???(name, optional)
	for other task parameters.

	Future modifications of this class should keep the above code logic so that the modifications do not break existing code using this class.

*/

#include <iostream>
#include <fstream>
#include <vector>

#include "ErrorFileUtil.h"

using namespace std;


/*
Grid node
*/
typedef struct GridNode3Dstruct
{
	unsigned int i;
	unsigned int j;
	unsigned int k;
	size_t w;
}GridNode3Dstruct;

/*
field elements
*/
typedef enum {
	None = 0,
	Ex = 1,
	Ey = 2,
	Ez = 4,
	Hx = 8,
	Hy = 16,
	Hz = 32,
}FieldElements;

/*
specify what to include in an output file
*/
typedef struct FieldOutputStruct{
	FieldElements elements;
	GridNode3Dstruct point;
}FieldOutputStruct;


/*
	task parameter name and value
*/
typedef struct NameValue
{
	char *name;
	char *value;
	void *data;
	size_t dataSize;
	void reset(){ name = NULL; value = NULL; data = NULL; dataSize = 0; }
}NameValue;

/*
	convert a double number to two integers. It is for forming a file name.
	i1 is formed by removing digits after the decimal point
	i2 is formed using digits after the decimal point
	en is the number of 0's after the decimal point
*/
void doubleToIntegers(double d, int *i1, int *i2, int *en);

/*
	form a string from d for the purpose of forming a file name
*/
int doubleToIntegersString(double d, char *str, unsigned int size);

/*
	get task parameters from a task file
*/
class TaskFile
{
private:
	int ret;           //error code
	NameValue *_lines; //lines in the task file
	int lineArraySize; //items in _lines
	int count;         //count of (name, value) pairs read from the file
	char *unfoundName; //the name passed to getIndexByName but not found in _lines
	char *nameBadVal;  //the name with an invalid value
	char filepath[FILENAME_MAX];    //task file full path
	//
	void allocateLines();
	
public:
	TaskFile(const char *taskfile);
	~TaskFile();
	int getIndexByName(const char *name);
	int saveToFile(char *file);
	int getErrorCode(){return ret;}
	void resetErrorCode(void);
	char *missingParameter(){return unfoundName;}
	char *invalidParameter(){return nameBadVal;}
	void setNameOfInvalidValue(const char *name);
	void showInvalidName();
	bool hasInvalidName();
	bool hasMissingName();
	void showMissingName();
	//
	double getDouble(const char *name, bool optional);
	int getInt(const char *name, bool optional);
	unsigned int getUInt(const char *name, bool optional);
	long int getLong(const char *name, bool optional);
	char *getString(const char *name, bool optional);
	int setString(const char *name, const char *value);
	bool getBoolean(const char *name, bool optional);
	GridNode3Dstruct *getGridNodes(const char *name, bool optional, size_t *size);
	FieldOutputStruct *getOutputFormats(const char *name, bool optional, unsigned int *count);
	unsigned *getUIntArray(const char *name, bool optional, size_t *size);
	int getIntVector(const char *name, bool optional, vector<int> &v);
	int getStringVector(const char *name, bool optional, vector<string> &v);
};

//common task parameter names for FDTD

//task parameters used by the simulation console
//#define TP_SIMTASK      "SIM.TASK"
//#define TP_SIMFDTD_DLL  "SIM.FDTD_DLL"
//#define TP_SIMFDTD_NAME "SIM.FDTD_NAME"
//#define TP_SIMIV_DLL    "SIM.IV_DLL"
//#define TP_SIMIV_NAME   "SIM.IV_NAME"
//#define TP_SIMBC_DLL    "SIM.BC_DLL"
//#define TP_SIMBC_NAME   "SIM.BC_NAME"
//#define TP_SIMFS_DLL    "SIM.FS_DLL"
//#define TP_SIMFS_NAME   "SIM.FS_NAME"
//#define TP_SIMTFSF_DLL  "SIM.TFSF_DLL"
//#define TP_SIMTFSF_NAME "SIM.TFSF_NAME"
//#define TP_SIMBASENAME  "SIM.BASENAME"
//#define TP_SIMDATAFOLDER "TSS.DATAFOLDER"

//use RAM instead of hard disk for field memories
//#define TP_USE_RAM       "SIM.USE_RAM"

//task parameters used by a FDTD module
//#define TP_FDTDN            "FDTD.N"
//#define TP_FDTDR            "FDTD.R"
//#define TP_MAX_TIMESTEP     "FDTD.MAXTIMESTEP"
//enable recording speeds
//#define TP_REC_TIMESTEP     "FDTD.RECTIMESTEP"
//task parameters used by a FDTD module which may use different estimation orders
//#define TP_HALF_ORDER_SPACE "FDTD.HALF_ORDER_SPACE"
//#define TP_HALF_ORDER_TIME  "FDTD.HALF_ORDER_TIME"

//task parameters needed by some tasks
//#define TP_SIMFILE1     "SIM.FILE1"
//#define TP_SIMFILE2     "SIM.FILE2"
//#define TP_SIMTHICKNESS "SIM.THICKNESS"
//#define TP_POINTM       "POINT.m"
//#define TP_POINTN       "POINT.n"
//#define TP_POINTP       "POINT.p"

#endif