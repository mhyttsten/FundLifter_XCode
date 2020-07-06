message(STATUS "Ch2, src: ${CMAKE_CURRENT_SOURCE_DIR}, bin: ${CMAKE_CURRENT_BINARY_DIR}")

set(SUBPROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(include)

add_executable(toyc-ch2
  toyc.cpp
  parser/AST.cpp
  mlir/MLIRGen.cpp
  mlir/Dialect.cpp
  )

add_dependencies(toyc-ch2 generate_tabledefs)

target_include_directories(toyc-ch2
 PRIVATE
   include/  # This includes ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_BINARY_DIR}/include)
   
target_link_libraries(toyc-ch2
  PRIVATE
    ${MLIR_ALLLIBS})
