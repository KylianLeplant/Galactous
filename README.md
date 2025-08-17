**Compilation**

Pour compiler avec makefile sur mingw64 : 
`
make
`

On execute ensuite le programme avec `./target/main.exe`



Pour compiler avec cmake sur mingw64 dans le répertoire build: 
`
cmake -G "MinGW Makefiles" .
mingw32-make
`

On execute ensuite le programme avec `./Galactous.exe` toujours dans le répertoire build.