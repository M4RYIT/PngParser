clang -I core\inc -o bin\png_parser.lib core\src\*.c -L bin -lzlibwapi -fuse-ld=llvm-lib