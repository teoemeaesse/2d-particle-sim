#ifndef __COMMON_H__
#define __COMMON_H__

#define ERR_FCLOSE(F) "[error]: Error closing file (%s).\n", F
#define ERR_FREAD(F) "[error]: Error reading from file (%s).\n", F
#define ERR_FTELL(F) "[error]: Could not find current cursor position of file (%s).\n", F
#define ERR_FSEEK(A, F) "[error]: Could not %s of file (%s).\n", A == SEEK_SET ? "seek start" : (A == SEEK_CUR ? "seek current cursor position" : "seek end"), F
#define ERR_FOPEN(F) "[error]: Could not open file (%s).\n", F

#define ERROR(M, R) printf(M); return R

char * read_file_as_string(const char * file);

#endif