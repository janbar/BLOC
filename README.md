# BLOC
A modular interpreter based on PL syntax.

BLOC can be used as is or embedded in a C++ program to perform dynamic processing. The language is of the PASCALIAN type, therefore easy to learn. You can extend functionality by dynamically importing modules (object). A sample of module is "file" that allows to operate files.

Internally BLOC supports classical data types, among boolean, integer, decimal, string, byte array, tuple, and table. For now it processes twice faster than Python.

## Build BLOC
You can build and test BLOC on Unix (Linux, BSD, MacOS, Illumos, SunOS), and MS-Windows. This repository contains the CMake project to configure and build BLOC. To achieve the build you will need to install **cmake** and a C99/C++11 compiler (**GCC** or **CLang**).

The build provides the core library **blocc**, the CLI tool **bloc**, and some modules to extend features of the language.

```
cmake -B release -DCMAKE_BUILD_TYPE=Release
cmake --build release
cmake --build release --target test
cmake --build release --target install
```

## Run the CLI
Launch **bloc** and type "help", or try a sample program as follow.
```
for i in 2 to 1000 loop
    b=true;
    for j in 2 to i/2 loop
        if i%j == 0 then b=false; break; end if;
    end loop;
    if b then print i; end if;
end loop;
```
