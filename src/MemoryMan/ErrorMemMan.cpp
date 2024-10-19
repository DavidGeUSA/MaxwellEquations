
#include <stdio.h>
#include "ErrorMemoryMan.h"

bool showErrorMemoryMan(int err)
{
	if (err >= ERR_MEMORYMAN_START && err <= ERR_MEMORYMAN_END)
	{
		switch (err)
		{
		case ERR_MEM_CREATE_FILE: //    6001
			printf("Error creating/opening file. (error=%d)", err);
			break;
		case ERR_MEM_CREATE_MAP: //     6002
			printf("Error creating memory map file. (error=%d)", err);
			break;
		case ERR_MEM_CREATE_VIEW: //    6003
			printf("Error creating memory map view. (error=%d)", err);
			break;
		case ERR_MEM_ADDR_NOT_FOUND: // 6004
			printf("Error locating allocated address for memory map. (error=%d)", err);
			break;
		case ERR_MEM_EINVAL: //         6005
			printf("Error formatning file name. (error=%d)", err);
			break;
		case ERR_MEM_WRITE_FILE: //     6006
			printf("Error writing file. (error=%d)", err);
			break;
		case ERR_MEM_OPEN_FILE: //      6007
			printf("Error opening file for memory mapping. (error=%d)", err);
			break;
		case ERR_INVALID_DLLPARAM: //   6008
			printf("Error loading DLL file: invalid parameter. (error=%d)", err);
			break;
		case ERR_INVALID_DLLPATH: //    6009
			printf("Error loading DLL file: invalid path. (error=%d)", err);
			break;
		case ERR_DLL_FUNC_NOTFOUND: //  6010
			printf("Error loading DLL file: function not found. (error=%d)", err);
			break;
		case ERR_DLL_FUNC_ERROR: //     6011
			printf("Error executing DLL file. Is the class constructor listed in Export.cpp? (error=%d)", err);
			break;
		case ERR_INVALID_PARAMS: //     6012
			printf("Invalid parameters. (error=%d)", err);
			break;
		case ERR_MEM_GET_FILE_SIZE: //  6013
			printf("Error getting file size. (error=%d)", err);
			break;

		case ERR_FOLDER_NOTEXIST://    6014
			printf("Folder does not exist. (error=%d)", err);
			break;
		case ERR_MEM_MAN_NULL://       6015
			printf("Memory manager does not exist. (error=%d)", err);
			break;
		case ERR_MEM_MAN_APP_PATH://   6016
			printf("Error getting app folder. (error=%d)", err);
			break;
		case ERR_MEM_OUTOFMEMORY://    6017
			printf("Out of memory. (error=%d)", err);
			break;
		case ERR_MEM_DIR_NOTEXIST://   6018
			printf("Folder does not exist. (error=%d)", err);
			break;
		case ERR_MEM_UNKNOWN: //       6030
			printf("Unknown memory management error. (error=%d)", err);
			break;

		default:
			printf("Unknown error. (error=%d)", err);
			break;
		}
		return true;
	}
	return false;
}
