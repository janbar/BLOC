[![build-ubuntu](https://github.com/janbar/BLOC/actions/workflows/build-ubuntu-latest.yml/badge.svg)](https://github.com/janbar/BLOC/actions/workflows/build-ubuntu-latest.yml)
[![build-macos](https://github.com/janbar/BLOC/actions/workflows/build-macos.yml/badge.svg)](https://github.com/janbar/BLOC/actions/workflows/build-macos.yml)
[![build-windows](https://github.com/janbar/BLOC/actions/workflows/build-windows.yml/badge.svg)](https://github.com/janbar/BLOC/actions/workflows/build-windows.yml)

# BLOC
A modular interpreter based on PL syntax.

BLOC can be used as is or embedded in a C/C++ program to perform dynamic processing. The language is of the PASCALIAN type, therefore easy to learn. You can extend functionality by dynamically importing modules (object). Samples of module are "file", "date", "utf8", "sqlite3", or "plplot" that allows to operate others types. See [modules](./modules/).

Internally, BLOC supports scalar data types like boolean, integer, decimal, string, byte array, and compound types like complex, tuple and table. For now, it processes twice as fast as Python.

## Install BLOC
Ubuntu packages are available in the repository `ppa:jlbarriere68/bloc`. You can install BLOC on Ubuntu by the following:
```
sudo add-apt-repository ppa:jlbarriere68/bloc
sudo apt-get update
sudo apt-get install bloc
```
For other OS, you have to build it yourself.

## Build BLOC
You can build and test BLOC on Unix (Linux, BSD, MacOS, Illumos, SunOS), and MS-Windows. This repository contains the CMake project to configure and build BLOC. To achieve the build you will need to install **cmake** and a C99/C++11 compiler (**GCC** or **CLang**).

The build provides the core library **blocc**, the CLI tool **bloc**, and some modules to extend types and features of the language.

```
cmake -B release -DCMAKE_BUILD_TYPE=Release
cmake --build release
cmake --build release --target test
cmake --build release --target install
```

To enable the build of the modules SQLite3 and PLplot, prior you have to install their dependencies.

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
And create your customized function.
```
function tokenize(buf, delim, trimnull:boolean) return table is
begin
  t = tab(0, str()); /* The table will be returned */
  p = 0; /* The current position in buf */
  s = delim.count();
  while true loop /* Or use limit: for i in 1 to 256 loop */
    n = strpos(buf,delim,p);
    if isnull(n) then
      break;
    end if;
    t.concat(substr(buf,p,n-p));
    p = n+s;
    while (trimnull and strpos(buf,delim,p) == p) loop
      p=p+s;
    end loop;
  end loop;
  if not trimnull or p < buf.count() then
    t.concat(substr(buf,p));
  end if;
  return t;
end;

function tokenize(buf, delim) return table is begin
  return tokenize(buf,delim,false);
end;

function tokenize(buf) return table is begin
  return tokenize(buf," ",true);
end;


forall e in tokenize("abcd  1234 efgh   6789 ") loop print e; end loop;
```
Learn more about typing:
`help type`, `help function`, `help table`, `help string`, `help while`,
`help strpos`, `help substr` ...

The example to follow uses the complex type (ii) to calculate Z of a Low Pass Filter order 2 (LC) loaded by a resistor R.
```
L = 1.8e-3; /* 1.8mH */
C = 14e-6;  /* 14uF */
R = 8;      /* 8R */

F = 31.5;   /* from band 0 i.e 31.5 hz */
while F < 20000 loop /* to band 9 */
  ZC = 1 / ( 2 * pi * F * C * ii );
  ZL = 2 * pi * F * L * ii;

  Z = ZL + ( R * ZC / ( R + ZC ) );

  print "Freq: " round(F,1) "\tZ: " imag(Z) "\tPhase: " 180 * iphase(Z) / pi;
  f = f * 2; /* loop next band */
end loop;
```
Note that the constant `ii` allows us to naturally construct a complex number.

