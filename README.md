[![build-ubuntu](https://github.com/janbar/BLOC/actions/workflows/build-ubuntu-latest.yml/badge.svg)](https://github.com/janbar/BLOC/actions/workflows/build-ubuntu-latest.yml)
[![build-macos](https://github.com/janbar/BLOC/actions/workflows/build-macos.yml/badge.svg)](https://github.com/janbar/BLOC/actions/workflows/build-macos.yml)
[![build-windows](https://github.com/janbar/BLOC/actions/workflows/build-windows.yml/badge.svg)](https://github.com/janbar/BLOC/actions/workflows/build-windows.yml)

# BLOC
A modular interpreter based on PL syntax.

BLOC can be used as is or embedded in a C/C++ program to perform dynamic processing. The language is of the PASCALIAN type, therefore easy to learn. You can extend functionality by dynamically importing modules (object). Samples of module are "file", "date", "utf8", "sqlite3", or "plplot" that allows to operate others types. See [modules](./modules/).

The syntax of the language is extremely readable, which avoids misunderstandings and design ambiguities.
The declaration and execution of functions are confined in their own context.
Internally, BLOC supports scalar data types like boolean, integer, decimal, string, byte array, and compound types like complex, tuple and table. See the [Reference Manual](./docs/BLOC-Reference-Manual.md) for concepts and details.
For now, it processes twice as fast as Python and Lua, and twenty as fast than Perl.

It is my common scripting language, running secure, fast, and as same on all platforms (MS-Windows, Linux, BSD, MacOS, Illumos, SunOS).
By using the modules SQLite3, MariaDB, or Oracle, it interoperates natively with a database.
And by using the modules sys, file, date, it makes it a powerful scripting language for everyday processing.
Finally, it is my powerful calculator, and my replacement for the farmer *bc* or *expr*.

## Install BLOC
Ubuntu packages are available in the repository `ppa:jlbarriere68/bloc`. You can install BLOC on Ubuntu by the following:
```
sudo add-apt-repository ppa:jlbarriere68/bloc
sudo apt-get update
sudo apt-get install bloc bloc-module-*
```
For other OS, please check if there is an artifact on the release page, otherwise you need to build it yourself.

## Build BLOC
You can build and test BLOC on Unix (Linux, BSD, MacOS, Illumos, SunOS), and MS-Windows. This repository contains the CMake project to configure and build BLOC. To achieve the build you will need to install **cmake** and a C99/C++11 compiler (**GCC** or **CLang**).

The build provides the core library **blocc**, the CLI tool **bloc**, and some modules to extend types and features of the language.

```
cmake -B release -DCMAKE_BUILD_TYPE=Release
cmake --build release
cmake --build release --target test
cmake --build release --target install
```

At least the modules **file**, **date**, **csv**, **utf8** and **sys** don't require external dependencies, and so they are made with the core library.
To enable the build of the others modules **sqlite3**, **mariadb** and **plplot**, prior you have to install and configure the dependencies.

## Run the CLI
Launch **bloc** and type "help".

You could type inline expression as shown below.
```
 = sqrt( sin(3*pi/4) ** 2 + cos(3*pi/4) ** 2 )
```
Or run a sample script as follow.
```
for I in 2 to 100000 loop
    for J in 2 to int(sqrt(I) + 1) loop
        if (I % J) == 0 then
            break;
        end if;
    end loop;
    if J > sqrt(I) then
        print I;
    end if;
end loop;
```
And create your customized function.
```
function split(buf, delim, trimnull:boolean) return table is
begin
  t = tab(0, str()); /* The strongly typed table will be returned */
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

function split(buf, delim) return table is begin
  return split(buf,delim,false);
end;

function split(buf) return table is begin
  return split(buf," ",true);
end;


forall e in split("abcd  1234 efgh   6789 ") loop print e; end loop;
```
Learn more about typing:
`help type`, `help statement`, `help builtin`, `help operator`, or `help` + *keyword*.

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

