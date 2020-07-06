message(STATUS "Ch1, src: ${CMAKE_CURRENT_SOURCE_DIR}, bin: ${CMAKE_CURRENT_BINARY_DIR}")

set(SUBPROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(include/)

add_executable(toyc-ch1
  toyc.cpp
  parser/AST.cpp
  )

target_link_libraries(toyc-ch1 PRIVATE
  MLIRSupport
  LLVMSupport
  LLVMDemangle
  curses)
