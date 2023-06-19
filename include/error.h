/**
 * Copyright (C) 2023 Strahinja Marinkovic - All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License as
 * published by the Free Software Foundation.
 *
 * You should have received a copy of the MIT License along with
 * this program. If not, see https://opensource.org/license/mit/
 */

#ifndef ERROR_H
#define ERROR_H

extern void fatal_error(const char* fmt, ...);

extern void report_warning(const char* fmt, ...);

extern void report_error(const char* fmt, ...);

#endif //!ERROR_H