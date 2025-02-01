add_executable(calc calc.cpp lexer.cpp parser.cpp)
target_link_libraries(calc PRIVATE ${llvm_libs})

