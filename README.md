[![build-ubuntu](https://github.com/janbar/BLOC/actions/workflows/build-ubuntu-latest.yml/badge.svg)](https://github.com/janbar/BLOC/actions/workflows/build-ubuntu-latest.yml)
[![build-macos](https://github.com/janbar/BLOC/actions/workflows/build-macos.yml/badge.svg)](https://github.com/janbar/BLOC/actions/workflows/build-macos.yml)

# BLOC
A modular interpreter based on PL syntax.

BLOC can be used as is or embedded in a C/C++ program to perform dynamic processing. The language is of the PASCALIAN type, therefore easy to learn. You can extend functionality by dynamically importing modules (object). Samples of module are "file", "date", "utf8" or "sqlite3" that allows to operate complex types.

Internally, BLOC supports scalar data types like boolean, integer, decimal, string, byte array, and compound types like tuple and table. For now, it processes twice as fast as Python.

## Build BLOC
You can build and test BLOC on Unix (Linux, BSD, MacOS, Illumos, SunOS), and MS-Windows. This repository contains the CMake project to configure and build BLOC. To achieve the build you will need to install **cmake** and a C99/C++11 compiler (**GCC** or **CLang**).

The build provides the core library **blocc**, the CLI tool **bloc**, and some modules to extend types and features of the language.

```
cmake -B release -DCMAKE_BUILD_TYPE=Release
cmake --build release
cmake --build release --target test
cmake --build release --target install
```

## Run the CLI
Launch **bloc** and type "help".

You could type inline expression as shown below.
```
 = sqrt( sin(3*pi/4) ** 2 + cos(3*pi/4) ** 2 )
```
Or run a sample script as follow.
```
for i in 1 to 1000 loop
    b=true;
    for j in 2 to i/2 asc loop
        if i%j == 0 then b=false; break; end if;
    end loop;
    if b then print i; end if;
end loop;
```
