#pragma once

#define ERR_FILEUTIL_START   300
#define ERR_FILEUTIL_END    1200

////////////////////////////////////////////////////
#define ERR_OK                  0
#define ERR_OUTOFMEMORY         1

#define ERR_FILE_FORMAT_ERROR       300

#define ERR_TASKFIILE_INVALID       380
#define ERR_TASKFIILE_DUPLICATE     381
#define ERR_TASK_PARAMETER_NAME     382
#define ERR_TASKFIILE_CANNOT_OPEN   383
//invalid task parameter value
#define ERR_TASK_INVALID_VALUE      384 
#define ERR_TASK_MISSING_NAME       385

#define ERR_FILE_OPEN_READ_EACCES   1001
#define ERR_FILE_OPEN_READ_EEXIST   1002
#define ERR_FILE_OPEN_READ_EINVAL   1003
#define ERR_FILE_OPEN_READ_EMFILE   1004
#define ERR_FILE_OPEN_READ_ENOENT   1005
#define ERR_FILE_OPEN_READ_UNDEFI   1006

#define ERR_FILE_OPEN_WRIT_EACCES   1011
#define ERR_FILE_OPEN_WRIT_EEXIST   1012
#define ERR_FILE_OPEN_WRIT_EINVAL   1013
#define ERR_FILE_OPEN_WRIT_EMFILE   1014
#define ERR_FILE_OPEN_WRIT_ENOENT   1015
#define ERR_FILE_OPEN_WRIT_UNDEFI   1016

#define ERR_FILE_WRITE_FAIL    1020
#define ERR_FILE_WRITE_LESS    1021
#define ERR_FILE_CANNOT_DELETE 1022

#define ERR_FILE_READ_FAIL     1030
#define ERR_FILE_READ_LESS     1031
#define ERR_FILE_READ_EOF      1032        

#define ERR_FILE_OPEN          1040

#define ERR_GET_FILESIZE       1050
#define ERR_FILE_NOTEXIST      1051
#define ERR_FILE_NAMING        1052
#define ERR_FILENAME_LONG      1053
#define ERR_FILESIZE_MISMATCH  1054
#define ERR_FILENAME_MISSING   1055

#define ERR_STR_CPY            1056
#define ERR_STR_CAT            1057
#define ERR_STR_PRF            1058
#define ERR_STR_SIZE_TOO_SMALL 1059

#define ERR_DIR_WORK           1070
#define ERR_FILE_MAP           1071
#define ERR_DIR_CREATE         1072

