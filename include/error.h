#ifndef ERROR_H
#define ERROR_H

extern void fatal_error(const char* fmt, ...);

extern void report_warning(const char* fmt, ...);

extern void report_error(const char* fmt, ...);

#endif // !ERROR_H