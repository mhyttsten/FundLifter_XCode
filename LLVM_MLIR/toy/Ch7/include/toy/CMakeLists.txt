mlir_tablegen(${SUBPROJECT_SRC_DIR} -gen-op-decls Ops.td Ops.h.inc)
mlir_tablegen(${SUBPROJECT_SRC_DIR} -gen-op-defs  Ops.td Ops.cpp.inc)
mlir_tablegen(${SUBPROJECT_SRC_DIR} -gen-op-interface-decls ShapeInferenceInterface.td ShapeInferenceOpInterfaces.h.inc)
mlir_tablegen(${SUBPROJECT_SRC_DIR} -gen-op-interface-defs ShapeInferenceInterface.td ShapeInferenceOpInterfaces.cpp.inc)

add_custom_target(ch7_generate_include_tabledefs ALL
   DEPENDS Ops.h.inc Ops.cpp.inc ShapeInferenceOpInterfaces.h.inc ShapeInferenceOpInterfaces.cpp.inc
   )

