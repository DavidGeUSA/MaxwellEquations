#include <stdio.h>
#include "ErrorThreadBatch.h"

bool showErrorThreadBatch(int err)
{
	if (err >= ERR_THREADBATCH_START && err <= ERR_THREADBATCH_END)
	{
		switch (err)
		{
		case ERR_TB_INV_DIV:       //110
			printf("invalid thread dividing. Loop range cannot be 0. (error=%d)", err);
			break;
		case ERR_TB_INV_SIZE:     //111
			printf("invalid size. (error=%d)", err);
			break;
		case ERR_TB_CREATE_POOL:  //112
			printf("cannot create ThreadBatch instance. (error=%d)", err);
			break;
		default:
			printf("Unknown error. (error=%d)", err);
			break;
		}
		return true;
	}
	return false;
}