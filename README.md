


## Compilation
For now, Galactous can juniquely compile on Mingw64
### Makefile
Pour compiler avec makefile sur mingw64 : 
`
make
`

On execute ensuite le programme avec `./target/main.exe`


### CMake
Pour compiler avec cmake sur mingw64 : 
`
cd build
cmake -G "MinGW Makefiles" .
mingw32-make
cd ..
`

On execute ensuite le programme avec `./build/Galactous.exe`.