/*
 *** Chapter 2: https://mlir.llvm.org/docs/Tutorials/Toy/Ch-2/
 LLVM: Fixed set of predefined types, and (low-level) instructions
    This is a big lowering, causing different front-ends to reimplement significant infrastructure
 MLIR is designed for flexibility, having few pre-defined instructions or types
    I supportes extensibility through Dialects, grouping concepts under a unique namespace
 
 Operations: Core unit of abstraction and computation (similar to LLVM instructions)
    Can represent all core IR structures of LLVM (instructions, globals, modules)
    %t_tensor = "toy.transpose"(%arg0)  // return dialect.name(args), >1 return values are ok too
       {inplace = true}  // constant data available in operation
       : (tensor<2x3xf64>) -> tensor<3x2xf64>  // argument & return types
       loc("example/file/path":12:1)  // mandatory
       list_of_successor_blocks  // mostly for branches
       list_or_regions  // for structural operations like functions

 $ mlir-opt  // Tests compiler passes
    --help
    --print-debuginfo  // Tells it to include source locations
 
 mlir/include/IR/OpDefinition.h  // Op, OpTrait namespace
 
 Creating a Dialect
   class ToyDialect : public mlir::Dialect {
    public:
     explicit ToyDialect(mlir::MLIRContext *ctx);
     static llvm::StringRef getDialectNamespace() { return "toy"; }
   };
 
   // Registers Toy dialect
   // Any MLIRContext created from now on knows about Toy dialect
   mlir::registerDialect<ToyDialect>();
 
 CRTP: Curiously Recurring Template Pattern (or F-bound polymorphism)
    A class X derives from a class template instantiation using X itself as template argument
   1. Static polymorphism: Allows base class to use derived functionality before it is instantiated
      (same as dynamic polymorphism / virtual functions without dynamic overhead)
   2. Other: Object counting, Polymorphic chaining
   3. interface Comparable<T> { public int compareTo(T other); }
   class Integer implements Comparable<Integer> {
     @Override
     public int compareTo(Integer other) { }
   }
   public static <S extends Comparable> S min(S a, S b) { return a.compareTo(b) <= 0 ? a: b; }
   public static <T extends Comparable<T>> T fmin(T a, T b) { return a.compareTo(b) <= 0 ? a: b; }
   Comparable<Integer> r1 = min(new Integer(10), new Integer(3));
   Integer i = fmin(new Integer(10), new Integer(3));
 
 Creating Operations
 - Alternative #1 (not preferred): Create and register an operation
      class ConstOp: public mlir::Op<ConstOp>,
                            mlir::OpTrait::ZeroOperands,
                            mlir::OpTrait::OneResult {
         using Op::Op;  // Inherit constructors
         // Which are virtual of these?
         static llvm::StringRef getOperationName() { return "toy.constant"; }
         mlir::DenseElementsAttr getValue();
         LogicalResult verify();
 
         static void build(... various versions ...);
     }
     // Register op
     ToyDialect::ToyDialect(mlir::MLIRContext *ctx): mlir::Dialect(getDialectNamespace(), ctx) {
       addOperations<ConstantOp>();
     }
   Op is a smart pointer wrapper around Operation (so we pass it around by value)
     Operation is opaque (generic operations API, does not describe specifics of operations)
     mlir::Operation *operation = ... from somewhere
     ConstantOp op = llvm::dyn_cast<ConstantOp>(operation);  // Return nil cast failed
     // Get the internal operation instance wrapped by the smart pointer.
     mlir::Operation *internalOperation = op.getOperation();  // Same pointer as operation
   OpTraits are defined in: https://github.com/llvm/llvm-project/blob/master/mlir/include/mlir/IR/OpDefinition.h
     
 - Alternative #2 (preferred):
   Use Operation Definition Specification (ODS): https://mlir.llvm.org/docs/OpDefinitions/
   Achieved via TableGen: https://llvm.org/docs/TableGen/index.html
   Create operation via TableGen record that gets compiled to mlir::Op equivalents
   def Toy_Dialect : Dialect {
     let name = "toy";  // == ToyDialect::getDialectNamespace
     let cppNamespace = "toy";
   }
   // Create base class for Toy dilect operations
   class Toy_Op<string opNameWODialectPrefix, list<OpTrait> traits = []> :
     Op<Toy_Dialect, opNameWODialectPrefix, traits>;
   // Op definition: ZeroOperands and OneResult traits will be inferred later
   def ConstantOp : Toy_Op<"constant"> { }
   
 
 
 MLIR APIs
 - mlir::MLIRContext:
 - mlir::Dialect
 
      Dialect(String, mlir::MLIRContext *)
      addOperations(T1, ..., Tn)  // Call from your constructor listing your operations
      getDialectNameSpace
- mlir::registerDialect<DialectType>();
- mlir::OwningModuleRef
    void dump()
  Parse an .mlir source file and generate its mlir again
    llvm::SourceMgr sourceMgr;
    sourceMgr.AddNewSourceBuffer(fileContent, llvm::SMLoc())
    mlir::OwningModuleRef m = mlir::parseSourceFile(srcMgr, mlir::MLIRContext*);
    m->dump()
  Convert Toy AST to mlir
    mlir::registerDialect<mlir::toy::ToyDialect>();
    mlir::MLIRContext context;
    mlir::Builder builder(&context);
    mlir::ModuleOp theModule = mlir::ModuleOp::create(builder.getUnknownLoc());
    // Now use theModule.push_back(mlir::FuncOp) to add all functions and their instructions
    mlir::OwningModuleRef m = theModule;
    m->dump()

 - mlir::ModuleOp theModule;  // List of functions in e.g. a file
 - mlir::OpBuilder builder(MLIRContext*);  // Create IR within a function
     Location getUnknownLoc()  // Used at mlir::ModuleOp m = mlir::ModuleOp::create(builder.getUnknownLoc());
     Location getFileLineColLoc(Identifier fname, unsigned line, unsigned column);
     Identifier getIdentifier(StringRef s);  // Used in getFileLineColLoc, has StringRef and MLIRContext*: https://github.com/llvm/llvm-project/blob/master/mlir/include/mlir/IR/Identifier.h
 - llvm::ScopedHashTable<StringRef, mlir::Value> symbolTable;  // Variable name -> value in current scope
 - mlir::LogicalResult declare(llvm::StringRef var, mlir::Value value)  // Declare variable in symbolTable
 - mlir::Location loc(builder.getFileLineColLoc(builder.getIdentifier(std::string fname), int line, int col);
 - mlir::LogicalResult: { mlir::success(), mlir::failure() }


 - Builder
    Location
    Types
      FloatType: getBF16Type(), getF16Type(), getF32Type(), getF64Type()
      IndexType: getIndexType()
      IntegerType: getI1Type(), getI32Type(), getI64Type()
      FunctionType: getFunctionType(ArrayRef<Type> inputs, ArrayRef<Type> results)
      TupleType
      NoneType
    Attributes
      Attribute, UnitAttr, BoolAttr, DictionaryAttr, IntegerAttr, FloatAttr, StringAttr, ArrayAttr
      FlatSymbolRefAttr, SymbolRefAttr, DenseIntElementsAttr
    Affine expressions and maps

 - OperationState
    addTypes(<type>)
    addOperands({lhs, rhs})
 - Value
 
 DenseElementsAttr
 RankedTensorType: Has a shape
 TensorType
 UnrankedTensorType: Does not have a shape
 
 - llvm::ArrayRef and llvm::StringRef: Has a reference to an array (startpointer+length)
     Does not own underlying array so not generally safe to store it
     Is trivially copyable so should be passed by value
     Can be constructed with (NoneType)
 - llvm::None: Null object (of NoneType which is an enum just containing None)
 - llvm::SmallVector<Type, Size> sm(4, Obj{}): Superoptimized if #elems < Size
 
 - std::move, 
   std::unique_ptr<T> o;
   std::vector<T> o;  // T could be primitives, eg double
      o.reserve(int);
   std::multiplies<int>()  // Operator function multiplying two values
   std::accumulate(T0 first, T0 last, T1 init, op): Computes the sum in range [first, last)
      std::vector<int64_t> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
      int m = std::accumulate(v.begin(), v.end(), 1, std::multiplies<int>());
      int s = std::accumulate(v.begin(), v.end(), 0);
   std::next(v.begin());
   std::to_string(v[0]);
   std::cout << ... <<;
 
   std::move(T)
   #include <numeric>
 
   std::string a;
 */




#include <stdio.h>
#define assertm(exp, msg) assert(((void)msg, exp))

int main(int argc, char* argv[]) {
  printf("Hello from MLIR_MagnusSummary\n");
    
  mlir::registerDialect<mlir::toy::ToyDialect>();
  mlir::MLIRContext c;
  mlir::OpBuilder b(&c);
  // llvm::ScopedHashTable<StringRef, mlir::Value> symbolTable;

  // Module
  mlir::ModuleOp m1 = mlir::ModuleOp::create(builder.getUnknownLoc());
  
  // f1 with 1 scalar argument [alt: mlir::RankedTensorType::get(ArrayRef<int64_t> shape, builder.getF64Type())]
  // FuncOp: https://github.com/llvm/llvm-project/blob/master/mlir/include/mlir/IR/Function.h
  mlir::Type f1arg1T = mlir::UnrankedTensorType::get(builder.getF64Type());
  llvm::SmallVector<llvm::Type, 1> f1args(1, f1arg1T);
  // FuncOp is like any custom MLIR op, inheriting from Op<FuncOp, ..., OpTrait::FunctionLike>
  mlir::FuncOp f1type = builder.getFunctionType(f1args, llvm::None);  // No return value
  mlir::Location f1loc = Location(identifierStr, lineInt, colInt);
  mlir::Identifier mname = "mname";
  mlir::FuncOp f1 = mlir::FuncOp::create(builder.getFileLineColLoc(mname, 1, 1), "f1", f1type);
  assertm(!function, "Error for function");
  
  // Block: https://github.com/llvm/llvm-project/blob/master/mlir/include/mlir/IR/Block.h
  // addEntryBlock comes from OpTrait::FunctionLike: https://github.com/llvm/llvm-project/blob/master/mlir/include/mlir/IR/FunctionSupport.h
  mlir::Block* f1EntryBlock = f1.addEntryBlock();

  // Value: https://github.com/llvm/llvm-project/blob/master/mlir/include/mlir/IR/Value.h
  //   An SSA which is result from either a BlockArgument or operation
  //   Value is a value-semantics wrapper around ValueImpl
  //   It is owned either by a Block (BlockArgument) or an Operation (OpResult)
  // BlockArgument inherits Value
  // std::vector<BlockArgument> f1EntryBlockArgs = f1EntryBlock->getArguments();
  // In Toy case, add f1EntryBlockArgs to symbol table (with names from function proto)

  builder.setInsertionPointToStart(f1EntryBlock);

  // Here emit the body of f1
  
  
  
  
  m1.push_back(f1);
  if (failed(mlir::verify(m1))) {
    m1.emitError("module verification error");
    return -1;
  }
  
  mlir::OwningModuleRef m2 = m1;
  m2->dump();
}

