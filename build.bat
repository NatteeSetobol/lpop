@echo off
REM remember to set the path to the include and lib directory
set Libs=User32.lib comdlg32.lib gdi32.lib -I../lib/include /link /libpath:../lib/lib
set CPPFiles=queue.cpp htmlparser.cpp required/memory.cpp stringz.cpp bucket.cpp httpprotocol.cpp marray.cpp token.cpp win_socket.cpp socket.cpp required/nix.cpp  
set flags=-DCOMPILER_MSVC  -wd4067
cl main.cpp %CPPFiles% %Flags% %Libs%
