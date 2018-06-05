#include "node.h"
#include "codegen.h"
#include "parser.hpp"

using namespace std;

/* Compile the AST into a module */
void CodeGenContext::generateCode(NBlock& root) {
  std::cout << "Generating code...\n";
  std::vector<Type*> sysArgs;
  FunctionType* mainFuncType = FunctionType::get(Type::getVoidTy(MyContext), makeArrayRef(sysArgs), false);
  Function* mainFunc = Function::Create(mainFuncType, GlobalValue::ExternalLinkage, "main");
  /* top level block for detect global variables */
  BasicBlock *bblock = BasicBlock::Create(MyContext, "entry");
  this->topBlock = bblock;
	
  /* Push a new variable/block context */
  pushBlock(bblock);
  root.codeGen(*this); /* emit bytecode for the toplevel block */
  popBlock();
	
  /* Print the bytecode in a human-readable format 
     to see if our program compiled properly
  */
  std::cout << "Code is generated.\n";
  module->dump();
  /*
    legacy::PassManager pm;
    pm.add(createPrintModulePass(outs()));
    pm.run(*module);
  */
}

/* Executes the AST by running the main function */
GenericValue CodeGenContext::runCode() {
  std::cout << "Running code...\n";
  ExecutionEngine *ee = EngineBuilder( unique_ptr<Module>(module) ).create();
  ee->finalizeObject();
  vector<GenericValue> noargs;
  GenericValue v = ee->runFunction(mainFunction, noargs);
  std::cout << "Code was run.\n";
  return v;
}

/* Returns an LLVM type based on the identifier */
static Type *typeOf(const NIdentifier& type) 
{
  if (type.name.compare("int") == 0) {
    return Type::getInt32Ty(MyContext);
  }
  else if (type.name.compare("double") == 0) {
    return Type::getDoubleTy(MyContext);
  }
  return Type::getVoidTy(MyContext);
}

static Value* CastToBoolean(CodeGenContext& context, Value* condValue) {
  return context.builder.CreateIntCast(condValue, Type::getInt1Ty(MyContext), true);
  /*
    if( ISTYPE(condValue, Type::IntegerTyID) ){
    condValue = context.builder.CreateIntCast(condValue, Type::getInt1Ty(context.llvmContext), true);
    return context.builder.CreateICmpNE(condValue, ConstantInt::get(Type::getInt1Ty(context.llvmContext), 0, true));
    }else if( ISTYPE(condValue, Type::DoubleTyID) ){
    return context.builder.CreateFCmpONE(condValue, ConstantFP::get(context.llvmContext, APFloat(0.0)));
    }else{
    return condValue;
    }
  */
}

/* -- Code Generation -- */

Value* NInteger::codeGen(CodeGenContext& context)
{
  std::cout << "Creating integer: " << value << endl;
  return ConstantInt::get(Type::getInt32Ty(MyContext), value, true);
}
/*
  Value* NDouble::codeGen(CodeGenContext& context)
  {
  std::cout << "Creating double: " << value << endl;
  return ConstantFP::get(Type::getDoubleTy(MyContext), value);
  }
*/
Value* NIdentifier::codeGen(CodeGenContext& context)
{
  std::cout << "Creating identifier reference: " << name << endl;
  if (context.locals().find(name) == context.locals().end()) {
    std::cerr << "undeclared variable " << name << endl;
    return NULL;
  }
  
  return context.builder.CreateLoad(context.locals()[name], false, "");
}
/*
  Value* NMethodCall::codeGen(CodeGenContext& context)
  {
  Function *function = context.module->getFunction(id.name.c_str());
  if (function == NULL) {
  std::cerr << "no such function " << id.name << endl;
  }
  std::vector<Value*> args;
  ExpressionList::const_iterator it;
  for (it = arguments.begin(); it != arguments.end(); it++) {
  args.push_back((**it).codeGen(context));
  }
  CallInst *call = CallInst::Create(function, makeArrayRef(args), "", context.currentBlock());
  std::cout << "Creating method call: " << id.name << endl;
  return call;
  }
*/
Value* NBinaryOperator::codeGen(CodeGenContext& context)
{
  std::cout << "Creating binary operation " << op << endl;

  Value* L = lhs.codeGen(context);
  Value* R = rhs.codeGen(context);
  
  //Instruction::BinaryOps instr;
  outs() << op << " " << TCEQ;
  switch (op) {
    /* comparitions */
    case TCEQ:
      return context.builder.CreateICmpEQ(L, R);
    case TCNE:
      return context.builder.CreateICmpNE(L, R);
    case TCGT:
      return context.builder.CreateICmpUGT(L, R);
    case TCGE:
      return context.builder.CreateICmpUGE(L, R);
    case TCLT:
      return context.builder.CreateICmpULT(L, R);
    case TCLE:
      return context.builder.CreateICmpULE(L, R);

      /* math */
    case TPLUS:
      return context.builder.CreateAdd(L, R);
    case TMINUS:
      return context.builder.CreateSub(L, R);
    case TMUL:
      return context.builder.CreateMul(L, R);
    case TDIV:
      return context.builder.CreateUDiv(L, R);

      /* bitops */
    case TAND:
      return context.builder.CreateAdd(L, R);
    case TOR:
      return context.builder.CreateOr(L, R);
    case TXOR:
      return context.builder.CreateXor(L, R);
    case TSHIFTL:
      return context.builder.CreateShl(L, R);
    case TSHIFTR:
      return context.builder.CreateAShr(L, R);
    default:
      return NULL;
  }
}

Value* NAssignment::codeGen(CodeGenContext& context)
{
  std::cout << "Creating assignment for " << lhs.name << endl;

  Value* exp = this->rhs.codeGen(context);

  context.builder.CreateStore(exp, context.locals()[lhs.name]);
  return context.locals()[lhs.name];
}

Value* NBlock::codeGen(CodeGenContext& context)
{
  StatementList::const_iterator it;
  Value *last = NULL;
  for (it = statements.begin(); it != statements.end(); it++) {
    std::cout << "Generating code for " << typeid(**it).name() << endl;
    last = (**it).codeGen(context);
  }
  std::cout << "Creating block" << endl;
  return last;
}
/*
  Value* NExpressionStatement::codeGen(CodeGenContext& context)
  {
  std::cout << "Generating code for " << typeid(expression).name() << endl;
  return expression.codeGen(context);
  }
*/

Value* NVariableDeclaration::codeGen(CodeGenContext& context)
{
  std::cout << "Creating variable declaration " << type.name << " " << id.name << endl;
  /*
    if (context.currentBlock() == context.topBlock) {
    GlobalVariable* g = new GlobalVariable(*context.module, Type::getInt32Ty(MyContext), false, GlobalValue::CommonLinkage, 0);

    context.globals()[id.name] = g;
    
    if (this->assignmentExpr != NULL) {
    context.builder.SetInsertPoint(context.topBlock);
    NAssignment assn(id, *assignmentExpr);
    assn.codeGen(context);
    }
    return g;
    }
  */
  Value* inst = context.builder.CreateAlloca(typeOf(type));
  context.locals()[id.name] = inst;

  if (this->assignmentExpr != NULL) {
    NAssignment assn(id, *assignmentExpr);
    assn.codeGen(context);
  }

  return inst;
}

Value* NFunctionDeclaration::codeGen(CodeGenContext& context) {
  std::cout << "Creating function: " << id.name << endl;
  
  vector<Type*> argTypes;
  VariableList::const_iterator it;
  for (it = arguments.begin(); it != arguments.end(); it++) {
    argTypes.push_back(typeOf((**it).type));
  }
  FunctionType* ftype = FunctionType::get(typeOf(type), makeArrayRef(argTypes), false);
  Function* function = Function::Create(ftype, GlobalValue::ExternalLinkage, id.name.c_str(), context.module);
  BasicBlock* bblock = BasicBlock::Create(MyContext, "entry", function, nullptr);

  context.builder.SetInsertPoint(bblock);
  //context.pushBlock(bblock);

  Function::arg_iterator argsValues = function->arg_begin();
  Value* argumentValue;

  for (it = arguments.begin(); it != arguments.end(); it++) {
    Value* argAlloc = (**it).codeGen(context);
		
    argumentValue = &*argsValues;
    argumentValue->setName((*it)->id.name.c_str());
    context.builder.CreateStore(argumentValue, argAlloc, false);
    argsValues++;
  }
	
  block.codeGen(context);

  context.builder.CreateRet(context.getCurrentReturnValue());

  //context.popBlock();

  return function;
}

Value* NIfStatement::codeGen(CodeGenContext& context) {
  outs() << "Creating if statement" << '\n';
  Value* condValue = this->condition.codeGen(context);

  condValue = CastToBoolean(context, condValue);

  Function* theFunction = context.builder.GetInsertBlock()->getParent();

  BasicBlock* thenBB = BasicBlock::Create(MyContext, "then", theFunction);
  BasicBlock* falseBB = BasicBlock::Create(MyContext, "else", theFunction);
  BasicBlock* mergeBB = BasicBlock::Create(MyContext, "ifcont", theFunction);

  if (this->hasElse) {
    context.builder.CreateCondBr(condValue, thenBB, falseBB);
  } else {
    context.builder.CreateCondBr(condValue, thenBB, mergeBB);
  }

  context.builder.SetInsertPoint(thenBB);
  this->trueBlock.codeGen(context);
  if (thenBB->getTerminator() == nullptr) {
    context.builder.CreateBr(mergeBB);
  }

  if (this->hasElse) {
    context.builder.SetInsertPoint(falseBB);
    this->falseBlock.codeGen(context);
    context.builder.CreateBr(mergeBB);
  }
  
  context.builder.SetInsertPoint(mergeBB);
  
  return NULL;
}

Value* NReturnStatement::codeGen(CodeGenContext& context)
{
  std::cout << "Generating return code for " << typeid(expression).name() << endl;
  Value *returnValue = expression.codeGen(context);
  context.setCurrentReturnValue(returnValue);
  return returnValue;
}
