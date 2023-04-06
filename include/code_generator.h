#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "parser.h"

class CodeGenerator {
public:
    CodeGenerator(Parser* parser);
    void generate_bytecode();
private:

};

#endif //!CODE_GENERATOR_H