#include <memory>
//#include "node.h"
#include "codegen.h"

extern NBlock* programBlock;
extern int yyparse();

int main(int argc, char* argv[]) {
  yyparse();

  CodeGenContext context;
  context.generateCode(*programBlock);
  
  return 0;
}
