#ifndef ITOA__H__
#define ITOA__H__

#ifdef __cplusplus
extern "C" const char* itoa(int, int = 0);
#else
extern const char* itoa(int, int);
#endif

#endif
