#ifndef C_CONVERTER_H
#define C_CONVERTER_H

#include "ast.h"

static const char* PREAMBLE_BOILERPLATE = 
"#include <stdio.h>\n"
"#include <stdint.h>\n\n";
"typedef uint32_t u32;\n"
"typedef int32_t i32;\n"
"typedef float f32;\n"
"typedef double f64;\n"
"typedef uint64_t u64;\n"
"typedef int64_t i64;\n\n";

static const char* MAIN_BOILERPLATE = 
"\n"
"int main(int argc, char *argv[]) {\n";

static const char* POSTAMBLE_BOILERPLATE = 
"\n}"

void convert_translation_unit(Ast_TranslationUnit* unit);

#endif //!C_CONVERTER_H