#include <stdio.h>
#include "ErrorFileUtil.h"

bool showErrorFileUtil(int err)
{
	if (err >= ERR_FILEUTIL_START && err <= ERR_FILEUTIL_END)
	{
		switch (err)
		{
		case ERR_TASKFIILE_INVALID://       380
			printf("Invalid task parameter formatting. Each parameter value should be expressed as 'name=value' in one line in a task file. (error=%d)", err);
			break;
		case ERR_TASKFIILE_DUPLICATE://     381
			printf("Duplicated task parameter value in a task file (error=%d)", err);
			break;
		case ERR_TASK_PARAMETER_NAME://     382
			printf("Task parameter name not in the task file (error=%d)", err);
			break;
		case ERR_TASKFIILE_CANNOT_OPEN://      383
			printf("Error opening task file (error=%d)", err);
			break;
		case ERR_TASK_INVALID_VALUE://      384
			printf("Invalid task parameter value (error=%d)", err);
			break;
		case ERR_TASK_MISSING_NAME://385
			printf("Missing task parameter name (error=%d)", err);
			break;

		case ERR_FILE_OPEN_READ_EACCES://   1001
			printf("Error open file for reading. file's sharing mode does not allow the specified operations, or the given path is a directory. (error=%d)", err);
			break;
		case ERR_FILE_OPEN_READ_EEXIST://   1002
			printf("Error open file for reading. _O_CREAT and _O_EXCL flags specified, but filename already exists. (error=%d)", err);
			break;
		case ERR_FILE_OPEN_READ_EINVAL://   1003
			printf("Error open file for reading. Invalid oflag or pmode argument. (error=%d)", err);
			break;
		case ERR_FILE_OPEN_READ_EMFILE://   1004
			printf("Error open file for reading. No more file descriptors are available (too many files are open). (error=%d)", err);
			break;
		case ERR_FILE_OPEN_READ_ENOENT://   1005
			printf("Error open file for reading. File does not exist. (error=%d)", err);
			break;
		case ERR_FILE_OPEN_READ_UNDEFI://   1006
			printf("Error open file for reading. File does not exist (UNDEFI). (error=%d)", err);
			break;
		case ERR_FILE_OPEN_WRIT_EACCES://   1011
			printf("Error open file for writing. file's sharing mode does not allow the specified operations, or the given path is a directory. (error=%d)", err);
			break;
		case ERR_FILE_OPEN_WRIT_EEXIST://   1012
			printf("Error open file for writing. _O_CREAT and _O_EXCL flags specified, but filename already exists. (error=%d)", err);
			break;
		case ERR_FILE_OPEN_WRIT_EINVAL://   1013
			printf("Error open file for writing. Invalid oflag or pmode argument. (error=%d)", err);
			break;
		case ERR_FILE_OPEN_WRIT_EMFILE://   1014
			printf("Error open file for writing. No more file descriptors are available (too many files are open). (error=%d)", err);
			break;
		case ERR_FILE_OPEN_WRIT_ENOENT://   1015
			printf("Error open file for writing. File does not exist. (error=%d)", err);
			break;
		case ERR_FILE_OPEN_WRIT_UNDEFI://   1016
			printf("Error open file for writing. File does not exist (UNDEFI). (error=%d)", err);
			break;
		case ERR_FILE_WRITE_FAIL://    1020
			printf("Error writing file. (error=%d)", err);
			break;
		case ERR_FILE_WRITE_LESS://    1021
			printf("Error writing file. Not all data written. (error=%d)", err);
			break;
		case ERR_FILE_CANNOT_DELETE:// 1022
			printf("Error deleting file. (error=%d)", err);
			break;
		case ERR_FILE_READ_FAIL://     1030
			printf("Error reading file. (error=%d)", err);
			break;
		case ERR_FILE_READ_LESS://     1031
			printf("Error reading file. Not all data read. (error=%d)", err);
			break;
		case ERR_FILE_READ_EOF://      1032
			printf("Error reading file. Already at end of file. (error=%d)", err);
			break;
		case ERR_GET_FILESIZE://    1050
			printf("Error geting file size. Not all data read. (error=%d)", err);
			break;
		case ERR_FILE_NOTEXIST://   1051
			printf("File does not exist. (error=%d)", err);
			break;
		case ERR_FILE_NAMING://   1052
			printf("Invalid file name. (error=%d)", err);
			break;
		case ERR_FILENAME_LONG:// 1053
			printf("File name is too long. (error=%d)", err);
			break;
		case ERR_FILESIZE_MISMATCH:// 1054
			printf("File size mismatch. (error=%d)", err);
			break;
		case ERR_FILENAME_MISSING://  1055
			printf("File name missing. (error=%d)", err);
			break;

		case ERR_STR_CPY://  1056
			printf("Error copying string. (error=%d)", err);
			break;
		case ERR_STR_CAT://  1057
			printf("Error appending string. (error=%d)", err);
			break;
		case ERR_STR_PRF://  1058
			printf("Error formating string. (error=%d)", err);
			break;
		case ERR_STR_SIZE_TOO_SMALL:// 1059
			printf("String size too small. (error=%d)", err);
			break;
		case ERR_DIR_WORK://  1070
			printf("Error getting current folder. (error=%d)", err);
			break;
		case ERR_FILE_MAP://  1071
			printf("Error mapping file with mmap. (error=%d)", err);
			break;
		case ERR_DIR_CREATE:
			printf("Error creating directory. (error=%d)", err);
			break;
		default:
			printf("Unknown error. (error=%d)", err);
			break;
		}
		return true;
	}
	return false;
}
