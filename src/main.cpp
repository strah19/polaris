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

#include "compiler.h"
#include "error.h"

int main(int argc, char* argv[]) {
    if (argc >= 2)
        compile_source(argv[1]);
    else
        fatal_error("No input file.\n");
    return 0;
}

