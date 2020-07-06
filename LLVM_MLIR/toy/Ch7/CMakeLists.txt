message(STATUS "Ch7, src: ${CMAKE_CURRENT_SOURCE_DIR}, bin: ${CMAKE_CURRENT_BINARY_DIR}")

set(SUBPROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(include)

mlir_tablegen(${SUBPROJECT_SRC_DIR} -gen-rewriters mlir/ToyCombine.td ToyCombine.inc)

add_custom_target(ch7_generate_tabledefs ALL DEPENDS ToyCombine.inc)

add_executable(toyc-ch7
toyc.cpp
parser/AST.cpp
mlir/MLIRGen.cpp
mlir/Dialect.cpp
mlir/DeadFunctionEliminationPass.cpp
mlir/LowerToAffineLoops.cpp
mlir/LowerToLLVM.cpp
mlir/ShapeInferencePass.cpp
mlir/ToyCombine.cpp
)

add_dependencies(toyc-ch7 ch7_generate_include_tabledefs ch7_generate_tabledefs)

target_include_directories(toyc-ch7
 PRIVATE
   include/  # This includes ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_BINARY_DIR}/include
   ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(toyc-ch7 PRIVATE ${MLIR_ALLLIBS})
