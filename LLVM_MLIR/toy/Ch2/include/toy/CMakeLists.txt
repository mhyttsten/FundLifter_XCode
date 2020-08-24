mlir_tablegen(${SUBPROJECT_SRC_DIR} -gen-op-decls Ops.td Ops.h.inc)
mlir_tablegen(${SUBPROJECT_SRC_DIR} -gen-op-defs  Ops.td Ops.cpp.inc)

mlir_tablegen(${SUBPROJECT_SRC_DIR} -gen-op-decls MHOps.td MHOps.h.inc)
mlir_tablegen(${SUBPROJECT_SRC_DIR} -gen-op-defs  MHOps.td MHOps.cpp.inc)

add_custom_target(generate_tabledefs ALL
   DEPENDS Ops.h.inc Ops.cpp.inc MHOps.h.inc MHOps.cpp.inc
   )
