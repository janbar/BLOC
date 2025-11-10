# BLOC 2.7 Reference Manual

2025-11

## Table of contents

[1 - Introduction](#1---introduction)

[2 - Basic Concepts](#2---basic-concepts)

- [2.1 - Values and Types](#21---values-and-types)

- [2.2 - Contexts and the Global environment](#22---contexts-and-the-global-environment)

- [2.3 - Error Handling](#23---error-handling)

- [2.4 - Type methods](#24---type-methods)

- [2.5 - Garbage Collection](#25---garbage-collection)

[3 - The language](#3---the-language)

- [3.1 - Lexical Conventions](#31---lexical-conventions)

- [3.2 - Variables](#32---variables)

- [3.3 - Statements](#33---statements)

- [3.4 - Expressions](#34---expressions)

- [3.5 - Null value handling and Type Introspection](#35---null-value-handling-and-type-introspection)

- [3.6 - Function Definitions](#36---function-definitions)

- [3.7 - Built-in Functions](#37---built-in-functions)

---

## 1 - Introduction

BLOC is a powerful, efficient, lightweight, embeddable scripting language. It supports procedural programming. BLOC is dynamically typed, runs in virtual context, and has automatic memory management with a cycle garbage collection, making it ideal for scripting, and rapid prototyping.

BLOC is implemented as a library, written in clean C, the common subset of standard C and C++. The BLOC distribution includes a host program called "bloc", which uses the BLOC core library to offer a complete, standalone BLOC interpreter, for interactive or batch use. BLOC is intended to be used both as a powerful, lightweight, embeddable scripting language for any program that needs one, and as a powerful but lightweight and efficient stand-alone language. As an extension language, BLOC has no notion of a "main" program: it works embedded in a host client, called the embedding program or simply the host. (Frequently, this host is the stand-alone "bloc" program.) The host program can invoke functions to execute a piece of BLOC code, can write and read BLOC variables. BLOC is extensible with dynamically imported modules. A module provides a new object-oriented type, with specialized features. Through the use of modules, BLOC can be augmented to cope with a wide range of different domains.

BLOC is free software, and is provided as usual with no guarantees, as stated in its license.

---

## 2 - Basic Concepts

This section describes the basic concepts of the language.

### 2.1 - Values and Types

BLOC is a dynamically typed language. This means that normal variables do not have a predefined type; only values do. Each value has its own type. A particular class of variable ($NAME) has a type immutability constraint, and consequently, the type of the stored value cannot change.

All values in BLOC are first-class values. This means that all values can be stored in variables, passed as arguments to other functions, and returned as results. In addition to the *undefined* type, there are 9 basic types in BLOC:

***undefined*, boolean, integer, decimal, string, bytes, complex, object, tuple, table.**

BLOC implements the Three-valued logic (3VL). The **null** value has a qualified type, else the *undefined* type. Its main property is that it is different from any other value; it represents the absence of a useful value.

The **boolean** type has two values: false and true. The values null and false make a condition false.

| Operation     | Result (boolean) | Conditionnal branch    |
|:-------------:|:----------------:| ---------------------- |
| null == false | null             | the condition is false |
| null == null  | null             | the condition is false |
| null != null  | null             | the condition is false |

See [3.5](#35---null-value-handling-and-type-introspection) for more details on handling null values.

**Integer** and **decimal** types are sub-types of number and can generally be combined in expressions. BLOC uses 64-bit integers and double-precision (64-bit) floats, on all platforms (64 or 32 bits). Unless stated otherwise, any overflow when manipulating integer values wrap around, according to the usual rules of two-complement arithmetic. (In other words, the actual result is the unique representable integer that is equal modulo 2 power of n to the mathematical result, where n is the number of bits of the integer type.) BLOC has explicit rules about when each sub-type is used,  but it also converts between them automatically as needed (see [3.4.3](#343---coercions-and-conversions)). To avoid pitfalls, the programmer should assume complete control over the representation of each number.

The type **string** represents sequences of characters. BLOC is 8-bit clean: strings can contain any 8-bit value, including embedded zeros. BLOC is also encoding-agnostic; it makes no assumptions about the contents of a string. The length of any string in BLOC must fit in a BLOC integer.

The type **bytes** represents sequences of bytes. Unlike the string type, the value is not printable as is. BLOC displays the content in canonical HEX+ASCII. The length of any bytes in BLOC must fit in a BLOC integer.

```
print raw("Hello World!");
00000000:  48 65 6c 6c 6f 20 57 6f 72 6c 64 21              Hello.World!
```

The type **complex** represents imaginary numbers with a real and imaginary parts :

( 2 + 3.i )

In BLOC a complex value is defined by an expression involving the constant **ii**, i.e :

```
2 + 3 * ii
```

The type **object** is handle of initialized structure from imported module. BLOC can manipulate external objects. Under the hood, a module is an external library, loaded on demand *(import)*, that supports BLOC's Plugin API. The module exposes the specifications of the object's constructors, methods, destructor, and implements related features. A module can therefore implement any domains. BLOC initializes the object via a constructor. The initialized object can be stored in a variable, either directly or within a tuple or table, or destroyed immediately after a method call.

```
import date;
print "Now: " date().iso8601utc();
Now: 2025-11-02T13:41:15Z

instant = date("2025-01-01","%Y-%m-%d");
instant.add(12,"month");
print instant.year();
2026
```

BLOC never copies an object, only its reference. The programmer can copy an object using a copy constructor, if available.

```
a = date();
b = a;            // B and A point to the same date instance
c = date(b);      // C point to a new date instance
```

The type **tuple** represents records. Tuples contain elements of any non-composite type, that is *boolean*, *integer*, *decimal*, *string*, *bytes*, *complex* and *object*. Nesting is not allowed. Element can be null, but they must be typed. A not null tuple has a super-type (or structure), which is the aggregate of the types of its elements; a null tuple has no structure. The structure of tuple is immutable. A tuple element is accessed by its rank, starting from **1**.

The type **table** implements n-dimensional uniform arrays. The tables can contain elements of all types, but they cannot be heterogeneous; in other words, they cannot contain values of different types. An element is accessed by index, starting from **0**.

---

### 2.2 - Contexts and the Global environment

As we will see in more detail in sections [3.2](#32---variables) and [3.3.2](#332---assignment), any reference to a free variable is syntactically associated with a context. Furthermore, each code block is compiled within the scope of a context. Therefore, free names in BLOC code refer to entries in the current context and are thus also called context variables.

The code block of function, on the other hand, is compiled in a self-contained child context. Variables are therefore local by nature; and access to values from another context is only possible by the parameters.

Functions can only be declared in the root context and are obviously accessible from any child context. Recursion is limited to 255 nested calls.

BLOC keeps a global environment. Common structures and values are initialized here and shared with all contexts.

- Current version and Compatibility Level
- Environment Country and Language
- The Random Number Generator
- Imported Modules

External modules can be loaded in advance, either programmatically or via a BLOC code.

---

### 2.3 - Error Handling

Several operations in BLOC can raise an error. An error interrupts the normal flow of the program, which can continue by catching the error. BLOC code can explicitly raise an error by calling the *raise* statement. BLOC code can intercept explicit error, zero-divide and out-of-range. For other runtime errors, the program stops and control returns to the host. To catch errors in BLOC, you can do a protected block, using *begin ... exception ... end* (see [3.3.1](#331---blocks)). Any error while running inside the block stops its execution, and control returns immediately to the exception-handling routine. When an otherwise unprotected error occurs during the compilation or execution of a BLOC code, control returns to the host, which can take appropriate measures, such as printing an error message. Whenever there is an error, an error object is propagated with information about the error. BLOC generates errors whose values are a string and an identifier.

The error handler is called only for regular compilation or runtime errors. It is not called for memory-allocation errors nor for errors while running finalizers.

---

### 2.4 - Type Methods

Some types have methods to manipulate values in place. They help improve code readability, or even efficiency.

- **string, bytes, table**
  - **concat**(...)
  - **delete**(...)
  - **put**(...)
  - **insert**(...)
  - **at**(...)
  - **count**()
- **tuple**
  - **set@** {rank} (...)
  - **count**()

These methods can all be called using syntactic sugar, i.e :

**expr ::= expr '.' method '(' [expr { ',' expr }] ')'**

Most methods return the resulting value after manipulation allowing calls to be chained, otherwise a property or an element.

##### Tuple accessor and mutator

To manipulate the elements of a tuple, a specific syntax is used in BLOC to distinguish it from that used for the table type, which could be confused. Accessing an element of the tuple is written as *{tuple}* **@** *{rank}*. Modifying an element of the tuple is done by calling the method *{tuple}*.**set@** *{rank}* ( *{new value}* ). The *set* method returns the tuple itself, allowing calls to be chained within the same statement.

The tuple item accessor:

**expr ::= expr:tuple '@' rank**

The tuple item mutator:

**expr:tuple ::= expr:tuple '.' method:set@ rank '(' expr ')'**

Note that *{rank}* is not an expression, but a coded value. It is evaluated at compile time. This constraint is imposed to the programmer, because conceptually a *tuple* has a fixed structure. 

---

### 2.5 - Garbage Collection

BLOC performs automatic memory management. This means that you do not have to worry about allocating memory for new objects or freeing it when the objects are no longer needed.

BLOC does not use a traditional collector to manage memory release. Memory allocations for values are registered in pools of pointer. One contains pointers of stored values bound to variables; and second contains pointers of temporary values, that is the working area. Assigning a new value to variable, their pointers are swapped between the pools; this operation is very fast. At the end of statement, all memory allocations pointed by the temporary pool are freed. If an error occurs during processing, BLOC retains the temporary pool for analysis. It will be purged at the end of next statement, or programmatically, or in context finalizer.

Memory is therefore freed cyclically, by bulk, and  triggered after each statement.

---

## 3 - The language

This section describes the lexis, the syntax, and the semantics of BLOC. In other words, this section describes which tokens are valid, how they can be combined, and what their combinations mean.

### 3.1 - Lexical Conventions

BLOC is a free-form language. It ignores spaces and comments between lexical elements (tokens), except as delimiters between two tokens. In source code, BLOC recognizes as spaces the standard ASCII white-space characters space, newline, carriage return, horizontal tab. Every executable statement in BLOC must end with a semicolon (`;`).

##### Names

Names (also called identifiers) in BLOC can be any string of Latin letters, Arabic-Indic digits, dollars and underscores, not beginning with a digit and not being a reserved word. Identifiers are used to name variables, and are not case-sensitive. The following keywords are reserved and cannot be used as names:

*Statement keywords*

```
asc       begin     break     continue  desc      do
else      elsif     end       end if    end loop  exception
for       forall    function  if        import    in
include   is        let       loop      nop       print
put       raise     return    step      then      to
trace     when      while
```

*Built-in function and constant keywords*

```
abs       acos      asin      atan      atan2     b64dec
b64enc    bool      ceil      chr       clamp     cos    
cosh      ee        error     exp       false     floor
getenv    getsys    hash      hex       iconj     ii
imag      input     int       iphase    isnull    isnum
log       log10     lower     lsubstr   ltrim     max
min       mod       null      num       off       on
phi       pi        pow       random    raw       read
readln    replace   round     rsubstr   rtrim     sign
sin       sinh      sqrt      str       strlen    strpos
subraw    substr    tab       tan       tanh      tokenize
trim      true      tup       typeof    upper
```

*Literal operator keywords*

```
and       or        xor       not       power     matches
```

BLOC is a case-sensitive language for the reserved keywords, and not case-sensitive for free names: *and* is a reserved word, but *And* alias *AND* is different, valid name.

The following strings denote other tokens:

```
+    -    *    /    **    %    &    |    ^    ~
<<   >>   ==   !=   <=    >=   <    >    =    (
)    :    ;    ,    .     @    #    //   /*   */
```

##### Literal strings

A literal string can be delimited by matching double quotes, and can contain the following limited C-like escape sequences: `\a` (bell), `\b` (backspace), `\f` (form feed), `\n` (newline), `\r` (carriage return), `\t` (horizontal tab), `\\` (backslash). If a literal string requires to embed double quotes as part of a string then it must be duplicated (double-double quotes).

##### Numeral constants

A numeric constant can be written with an fractional part and an decimal exponent, marked by a letter 'e' or 'E'. BLOC also accepts hexadecimal constants, which start with `0x` or `0X`.

A numeric constant with a radix point or an exponent denotes a **decimal**; otherwise, if its value fits in an integer or it is a hexadecimal constant, it denotes an **integer**.

Examples of valid integer constants are

```
3    123    0xff    0xBEBADA
```

Examples of valid decimal constants are

```
3.0    3.1416    314.16e-2    0.31416E1    34e1    .314
```

##### Comments

A line begining with a sharp (`#`) outside a string, is interpreted as comment line. That is to be compatible with UNIX script Sha-Bang.

```
#!/usr/bin/bloc
print "Running BLOC";
# end
```

A short comment at the end of a line begins with `//`, anywhere outside a string.

```
a = 2 * pi; // one cycle
```

A long or enclosed comment starts with `/*` and ends with `*/`.

```
/* About this program
...
*/
print "PI=" /*3.1415*/ pi;
```

##### At sign

The at sign (`@`) is used to point to an element of tuple by its rank:

```
km = tup("A place", 389.0, "N-NW")@2;  // km is decimal 389.0
```

---

### 3.2 - Variables

Variables are places that store values. Before the first assignment to a variable, the name is not a valid syntactic identifier.

**var ::= Name**

A name prefixed with `$` denotes a variable with immutable type constraint. The immutable type constraint is also enabled for iterator variable inside loop block, regardless name prefix.

Any variable name is assumed to be visible in the current context. Outside a function block, the current context is the root context. Inside a function block, the context is private and self-contained.

Assigning new value to a variable, will free the memory allocated by old value; therefore assigning *null* will free the old, without new allocation.

---

### 3.3 - Statements

BLOC supports an almost conventional set of statements, similar to those in other PASCALIAN languages. This set includes blocks, assignments, control structures, function calls, standard IO.

#### 3.3.1 - Blocks

A block is a list of statements, which are executed sequentially. Blocks can be protected by an exception clause; on error control returns to the exception-routine, else to the parent block.

The interceptable errors are limited to the following:

- User Named error 
  - Raised using statement **raise** (See [3.3.12](#3312---raise-statement))
- **divide_by_zero**
  - Attempt to divide by zero
- **out_of_range**
  - Number value does not fit in character or byte range (8 bits)
  - Integer constant or its string representation does not fit in BLOC integer value
  - Decimal constant or its string representation does not fit in BLOC decimal value

Exception clause can only be used in explicit block, starting with **begin**, ending by **end**.

**stat ::= begin {stat} [ exception {when expr then {stat}} ] end;**

Example:

```
d = 0;
begin
    a = 1 / d;
exception
when divide_by_zero then
    print "Invalid divisor";
when others then
    print error@1;
end;
```

#### 3.3.2 - Assignment

Variable assignment is done with the **let** statement. However, the keyword is implicit.

**stat ::= [let] Name = expr ;**

Examples:

```
let a = 123;
b = 0.345;    // keyword let is implicit
```

The null assignment can be written in the simplified form *{name}*`:`*{type}*.

**stat ::= Name : Type ;**

Or using a built-in initializer without argument.

```
a = null;    // null without qualified type
a:undefined; // null without qualified type
b:string;    // null string
c = str();   // null string
d = num();   // null decimal
```

BLOC provides a built-in initializer for each basic types:

- **int**() , **int**( *expr* ) 
  - expr : integer|decimal|complex|string|bytes
- **num**() , **num**( *expr* ) 
  - expr : integer|decimal|complex|string|bytes
- **str**() , **str**( *expr* ) 
  - expr : integer|decimal|string|bytes
- **raw**() , **raw**( *n* ) , **raw**( *n* , *b* ) , **raw**( *expr : string* )
- **tup**() , **tup**( *expr*, ... ) 
  - expr : integer|decimal|complex|string|bytes|object
- **tab**() , **tab**( *n* , *expr* ) 
  - expr : integer|decimal|complex|string|bytes|object|tuple|table

Finally assignments can be chained and followed by statement using the comma (`,`).

**stat ::= let {, let} [, stat] ;**

Examples:

```
a = pi, b = 2 * a, c = (a + b)/4, print "c = " c;
```

### 3.3.3 - Control Structures

The control structures *if*, *while* have the usual meaning and familiar syntax:

**stat ::= if expr then {stat} {elsif expr then {stat}} [else {stat}] end if ;**

**stat ::= while expr loop {stat} end loop ;**

BLOC also has a for statement, in two flavors (see [3.3.4](#334---for-statement), [3.3.5](#335---forall-statement)).

The condition expression of a control structure should return boolean value. Both false and null test false.

BLOC has statement **break** and **continue** to take control inside a loop block. The *break* statement terminates the innermost enclosing *loop* (breaks out of it), while *continue* causes a jump to the next iteration of the innermost enclosing *loop*, skipping all the remaining statements.

```
i = 0;
while true loop
    i = i + 1;
    if i < 10 then continue;    // jump to next iteration
    elsif i > 15 then break;    // break out of loop
    end if;
    print i;
end loop;
```

### 3.3.4 - For Statement

The **for** statement is numerical for loop.

The numerical **for** loop repeats a block of code while a control variable goes through an arithmetic progression. It has the following syntax:

**stat ::= for Name in expr to expr [step expr] [asc|desc] loop {stat} end loop ;**

The given identifier (Name) defines the control variable of type *integer*.

The loop starts by evaluating once the three control expressions. Their values are called respectively the *initial value*, the *limit*, and the *step*. If the step is absent, it defaults to 1. The loop is done with integers; After that initialization, the loop body is repeated with the value of the control variable going through an arithmetic progression, starting at the initial value, with a common absolute difference given by the step. A step less than 1 raises an error. The loop continues for all value is in the range. The order of progression within the range is automatic, unless a constraint has been specified by keyword *asc* or *desc*. If the progression constraint cannot be met, the loop block is not executed.

The control variable can never wraps around, but the initial value or the limit could in case of overflow. Adding a progression constraint help to resolve this borderline case.

You can change the value of the control variable during the loop; Inside the loop the variable has the immutable type constraint. After exiting the loop, the constraint is deactivated.

### 3.3.5 - Forall Statement

The **forall** statement works over tables; It is a control flow statement for traversing items in a table. On each iteration, the iterator variable point to the next item, stopping when no item can be fetched. It has the following syntax:

**stat ::= forall Name in expr [asc|desc] loop {stat} end loop ;**

The *forall* statement never detach (copy) the content of the source table. The table cannot be modified inside the loop, except via the iteration variable. Therefore only the pointed-to element could be modified within the loop; validations related to the read-only constraint are performed at compile time.

The iterator variable points to item value. Therefore you can change the value, which is the value of the table element.

Inside the loop, the iterator variable has the immutable type constraint. After exiting the loop, the constraints are deactivated and the variable is empty (null).

Examples:

```
t = tab(5, "Hello World");
forall e in t.concat("Bye") loop e.concat(" !"); end loop;
forall e in t loop print e; end loop;

Avg = 0.0, Nb = 1000;
forall e in tab(Nb, random()) loop Avg = Avg + e / Nb; end loop;
print Avg;

t=tab(2, tab(5, num()));
forall e in t.at(0) loop
    e = random();
end loop;
forall e in t.at(0) loop print e; end loop;
```

### 3.3.6 - Function Calls as Statements

Function calls can be executed as statements:

**stat ::= functioncall**

In this case, all returned values are thrown away. Function calls are explained in section [3.4.13](#3413---function-calls).

### 3.3.7 - Import Statement

The **import** statement is the directive to load an external module. External module provides new **object** type with features. Loading is performed immediately, so the new type can be used now.

**stat ::= import module ;**

The statement try to load the library *libbloc_{module}.so* on UNIX, or *libbloc_{module}.dylib* on OSX, or *bloc_{module}.dll* on WINDOWS.

Alternatively you can load a module using the library path.

**stat ::= import "path" ;**

### 3.3.8 - Include Statement

The **include** statement is the directive to embed another source. You can have up to three levels of nesting.

**stat ::= include "path" ;**

### 3.3.9 - Nop Statement

The **nop** statement does nothing.

**stat ::= nop ;**

### 3.3.10 - Print Statement

The **print** statement writes out the string representation of expression(s), terminated by a newline. Expressions are chained by spaces.

**stat ::= print [expr {expr}] ;**

The printed values are human readable text, depending of expression type.

### 3.3.11 - Put Statement

The **put** statement writes out the string value of expression(s). Expressions are chained by spaces.

**stat ::= put [expr {expr}] ;**

Only *boolean*, *integer*, *decimal*, and *string* expression can be written out. All other types must be converted first, i.e using built-in function **str**.

### 3.3.12 - Raise Statement

The **raise** statement throws a user named exception, or throwable error.

**stat ::= raise Name ;**

The following exception names are predefined in BLOC:

- DIVIDE_BY_ZERO

- OUT_OF_RANGE

Raised exception can be intercepted (See [3.3.1](#331---blocks)).

### 3.3.13 - Return Statement

The **return** statement is used to return a value from a function or the program. It can be written anywhere; all enclosing loops will be terminated. The syntax for the return statement is:

**stat ::= return [ expr ] ;**

Return from the program, value type should be among boolean, integer, decimal, or string.  Any other value will be ignored.

### 3.3.14 - Trace Statement

The **trace** statement enable or disable trace verbosity.

**stat ::= trace expr:boolean ;**

Trace mode is intended for use only when absolutely necessary, typically for debugging a program. Performance are severely degraded due to the error output being jammed.

---

## 3.4 - Expressions

The basic expressions in BLOC are the following:

**expr ::= Numeral**

**expr ::= Literal**

**expr ::= expr binop expr**

**expr ::= unop expr**

**expr ::= var | builtin | functioncall | '(' expr ')'**

Numeral and literal strings are explained in [3.1](#31---lexical-conventions); variables are explained in [3.2](#32---variables); function calls are explained in [3.4.13](#3413---function-calls).

Binary operators comprise arithmetic operators (see [3.4.1](#341---arithmetic-operators)), bitwise operators (see [3.4.2](#342---bitwise-operators)), relational operators (see [3.4.4](#344---relational-operators)), logical operators (see [3.4.5](#345---logical-operators)), unary operators comprise the unary minus (see [3.4.1](#341---arithmetic-operators)), the unary bitwise not (see [3.4.2](#342---bitwise-operators)), the unary logical not (see [3.4.5](#345---logical-operators)).

### 3.4.1 - Arithmetic Operators

BLOC supports the following arithmetic operators:

| Symbol               | Operation      |
|:--------------------:| -------------- |
| **+**                | addition       |
| **-**                | subtraction    |
| **\***               | multiplication |
| **/**                | division       |
| **\*\*** , **power** | exponentiation |
| **%**                | modulo         |
| **-**                | unary minus    |

The arithmetic operators work as follows: If both operands are integers, the operation is performed over integers and the result is an integer. Otherwise, if one operands is float, then other is converted to float, the operation is performed following the machine's rules for floating-point arithmetic (usually the IEEE 754 standard), and the result is a decimal.

Modulo is defined as the remainder of a division.

In case of overflows in integer arithmetic, all operations wrap around.

### 3.4.2 - Bitwise Operators

BLOC supports the following bitwise operators:

| Symbol | Operation         |
|:------:|:----------------- |
| **&**  | bitwise AND       |
| **\|** | bitwise OR        |
| **^**  | bitwise XOR       |
| **\~** | unary bitwise NOT |
| **<<** | left shift bit    |
| **>>** | right shift bit   |

All bitwise operations are applicable only with integers, operate on all bits of those integers, and result in an integer.
Both right and left shifts fill the vacant bits with zeros. Negative displacements shift to the other direction; displacements with absolute values equal to or higher than the number of bits in an integer result in zero (as all bits are shifted out).

### 3.4.3 - Coercions and Conversions

BLOC provides some automatic conversions between some types and representations at run time.

All arithmetic operations applied to mixed numbers (integers and floats) convert the integer operand to a float. This implicit conversion is always performed without loss of information:

- 5 / 2.0 => the decimal 2.5

- 5 ** 0.5 => the decimal 2.5

Integer division never converts integer operands to floats:

- 5 / 2 => the integer 2

Bitwise operators can only be used with integer operands.

- 5.0 << 2 => will raise an error

- int(5.0) << 2 => the integer 20

In a conversion from integer to float, if the integer value has an exact representation as a float, that is the result. Otherwise, the conversion gets the nearest higher or the nearest lower representable value. This kind of conversion never fails.

- 9223372036854775807 + 0.0 => the decimal 9.223372036854776e+18

- num(9223372036854775807) => the decimal 9.223372036854776e+18

The explicit conversion from float to integer (built-in function *int*) checks the float is in the range of integer representation. If it does, that representation is the result. Otherwise, the conversion fails with error out-of-range.

### 3.4.4 - Relational Operators

BLOC supports the following relational operators:

| Symbol      | Operation                  |
|:-----------:|:-------------------------- |
| **==**      | equality                   |
| **!=**      | inequality                 |
| **<**       | less than                  |
| **<=**      | less or equal              |
| **>**       | greater than               |
| **>=**      | greater or equal           |
| **matches** | matches regular expression |

These operators result in false, true or null if one of the operands is null.

Ordering operators work as follows: if both arguments are numbers, they are compared according to their mathematical values, regardless of their subtype. Otherwise, if both arguments are objects, their references are compared. Otherwise, the values are compared according to their binary content.

The *matches* operator is applicable only with string operands.

### 3.4.5 - Logical Operators

BLOC supports the following logical operators:

| Symbol  | Operation            |
|:-------:| -------------------- |
| **and** | logical AND          |
| **or**  | logical OR           |
| **xor** | logical exclusive OR |
| **not** | logical negation NOT |

**(null OR true)** is always true; **(null AND false)** is always false.

Apart from these cases, these operators result in false, true or null if one of the operands is null.

### 3.4.6 - Concatenation

The type method **concat** (see [2.4](#24---type-methods)) performs concatenation in place. It is usable with all type of sequences: string, bytes and table. The argument can be of the same type or of the sequenced subtype. For string and bytes, the subtype is a 8bits integer.

Furthermore, for convenience, BLOC allows string concatenation with the operator `+`. In this case, both operands must be strings.

### 3.4.7 - Precedence

Operator precedence in BLOC follows the table below, from lower to higher priority:

1. **`&&   and  ||   or     xor`**

2. **`<    >    <=    >=    !=    ==    matches`**

3. **`|    ^    &`**

4. **`<<   >>`**

5. **`+    -`**

6. **`*    /    %`**

7. Unary operators **`not  !    +    -    ~`**

8. Exponentiation **`power **`** 

9. Member **`.`** 

As usual, you can use parentheses to change the precedences of an expression. The exponentiation (`**`) and member (`.`) operators are right associative. All other binary operators are left associative.

### 3.4.8 - Built-in Constants

BLOC defines several useful mathematical and boolean constants.

**null** is the null value, with undefined type.

**true** or **on** is the boolean expression TRUE.

**false** or **off** is the booelan expression FALSE.

**ii** is the imaginary number.

**pi** is the ratio of a circle's circumference to its diameter.

**ee** is the Euler's number.

**phi** is the golden ratio.

### 3.4.9 - Basic Constructors and Converters

BLOC provides some constructors for basic types. They are useful for initializing a typed null value or for performing a conversion from another type.

#### boolean

**bool()** returns the null boolean.

**bool( expr:integer )** converts the integer representation of boolean to boolean; it never fails. Any value other than 0 is converted to *true*.

**bool( expr:decimal )** converts the float representation of boolean to boolean; it never fails. Any value other than 0 is converted to *true*.

#### string

**str()** returns the null string. The character count is null.

**str( expr:integer )** returns the string representation of integer.

**str( expr:decimal )** returns the string representation of float.

**str( expr:bytes )** returns the string representation of bytes.

**chr( expr:integer )** returns the string (one character) of byte, or fails with out-of-range error.

#### integer

**int()** returns the null integer.

**int( expr:decimal )** converts the float to integer, or fails with out-of-range error.

**int( expr:string )** converts the string representation of an integer into an integer, or fails. Use the built-in function *isnum()* to test beforehand that the conversion is possible.

**int( expr:bytes )** converts the string (bytes) representation of an integer into an integer, or fails. Use the built-in function *isnum()* to test beforehand that the conversion is possible.

#### decimal

**num()** returns the null decimal.

**num( expr:integer )** converts the integer to float; it never fails.

**num( expr:string )** converts the string representation of a float to a float, or fails. Use the built-in function *isnum()* to test beforehand that the conversion is possible.

**num( expr:bytes )** converts the string (bytes) representation of a float to a float, or fails. Use the built-in function *isnum()* to test beforehand that the conversion is possible.

### 3.4.10 - Bytes Constructor

Bytes constructor is the built-in function **raw** that create byte array. Every time a constructor is evaluated, a new bytes is created. The constructor can be used to create a null bytes or to create a bytes and initialize a sequence. The general syntax for constructors is the function call syntax.

**raw()** returns the null bytes. The byte count is null.

**raw( expr:integer , expr:integer )** return the byte array filled with n byte(s) of the given value. The value can be null or integer in the range 0 to 255. The returned bytes is not null, therefore the byte count is 0 or more.

**raw( expr:string )** return the byte array filled with string content.

Examples:

```
b=raw();                       // null bytes
b=raw(0, null);                // bytes with size 0
b=raw(16, null);               // bytes size 16, zero filled
b=raw(4, 0xff);                // bytes size 4, filled with 255
b=raw("Hello World!");         // bytes filled with string
```

Finally the bytes can be updated using the type methods (see [2.4](#24---type-methods)).

### 3.4.11 - Tuple Constructor

Tuple constructor is the built-in function **tuple** that create tuples. Every time a constructor is evaluated, a new tuple is created. The constructor can be used to create a null tuple or to create a tuple and initialize its elements. The general syntax for constructors is the function call syntax.

**tup()** returns the null tuple without structure. The element count is null.

**tup( expr {, expr} )** returns the tuple for the given expressions. The expressions can be null, but they must be typed. The built-in function *bool()*, *str()*, *int()*, *num()*, and *raw()* provide a null typed value. The returned tuple is not null, therefore the element count is 1 or more.

Examples:

```
import file;
r=tup();                           // null tuple
r=tup(1, "file 1", file());        // not null tuple
```

Finally the tuple can be updated using the type methods (see [2.4](#24---type-methods)).

### 3.4.12 - Table Constructor

Table constructor is the built-in function **tab** that create tables. Every time a constructor is evaluated, a new table is created. The constructor can be used to create an empty table or to create a table and initialize some of its elements. The general syntax for constructors is the function call syntax.

**tab()** returns the null table with undefined type. The element count is null.

**tab( expr:integer , expr )** returns the table of n element(s) of the given expression. The expression can be null, but it must be typed. The built-in function *bool()*, *str()*, *int()*, *num()*, and *raw()* provide a null typed value. The null tuple *tup()*, and null table *tab()*, cannot be used here, because not null table has a defined type. The returned table is not null, therefore the element count is 0 or more.

Examples:

```
import file;
t=tab();                               // null table
t=tab(0, num());                       // empty table of decimal
t=tab(0, tab(0, str()));               // empty [ [ string ] ]
t=tab(0, tup(str(), file(), bool()));  // empty table of tuple
t=tab(100, random(100));               // table of 100 decimals
t=tab(10, "");                         // table of 10 strings
t=tab(10, 0);                          // table of 10 integers
```

Finally the table can be updated using the type methods (see [2.4](#24---type-methods), [3.4.6](#346---concatenation)).

### 3.4.13 - Function Calls

A function call in BLOC has the following syntax:

**functioncall ::= name args**

Arguments have the following syntax:

**args ::= '(' \[expr { ',' expr }] ')'**

All argument expressions are evaluated before the call.

#### User functions

Argument values are passed by copy. Note that an object value is a reference; therefore, the object itself is not copied.

#### Built-in functions and Object methods

Argument values are passed by reference.

---

## 3.5 - Null value handling and Type Introspection

A null should not be confused with a value of 0. A null indicates a lack of a value, which is not the same as a zero value. In BLOC, **null is a marker, not a value**. This usage is quite different from most programming languages, where a null value of a reference means it is not pointing to any object.

To solve problems related to the concept of null value, BLOC provides the built-in boolean function **isnull**. It allows you to test the state of the null marker in an expression. It is useful in control structures because the null value cannot be handled by standard relational operators.

```
a = num();
if isnull(a) then print "A is null";
else print "A is not null";
end if;
```

A null expression, therefore with the null marker, may or may not have a type. For this case, the type "**undefined**" indicates that the type is not defined.

The built-in **typeof** function allows you to identify the type of an expression at runtime. It is useful within the body of a function when the type of a parameter is opaque, or to disambiguate the result of a function call.

```
a = num();
if typeof(a) == "decimal" then print "A has type decimal";
else raise unexpected_type_for_a;
end if;
```

## 3.6 - Function Definitions

Function declarations are statements used to define functions with a free name, parameters, and body. The function declarations can only appear at the root context.

Once defined, the name of the function is visible for the context in which it is defined (the root context), and for child contexts; therefore it is visible within its private context, to enable recursive functions.

BLOC supports function overloading, based on arity (number of parameters).

The syntax for function definition is:

**functiondef ::= function Name [parms] return type is begin {stat} end ;**

With:

**type ::= undefined | boolean | integer | decimal | complex | string | object | tuple  | table**

**parm ::= Name [':' type]**

**parms ::= '(' [parm {',' parm}] ')'**

A function parameter is a variable name to which the passed value is bound within the function block.

The parameter can be followed by a type declaration. Otherwise, it remains opaque. The type declaration imposes no constraints; it is intended to aid the programmer, and no validation is performed at runtime. Similarly, the return type declared imposes no constraints at runtime.

The **return** statement ends function execution and specifies a value to be returned to the caller (See [3.3.13](#3313---return-statement)).

Example 1: An opaque function

```
function dummy( a , b ) return undefined is
begin
  print "params = " a " , " b;
  return a + b; // works for numbers and strings, else fails at runtime
end;
```

Example 2:

```
function reciproque( a ) return decimal is
begin
  if isnum(a) then
    return 1 / num(a);
  end if;
  raise INVALID_VALUE;
end;

print reciproque("3.14");
print reciproque(pi);
print reciproque("Not a Number");
```

Example 3: Passing object (by reference)

```
import date;

function next_year( Now:date ) return integer is
begin
  return date(Now).add(12, "month").year();
end;

print next_year(date());
```

## 3.7 - Built-in Functions

BLOC provides a set of built-in functions. Built-in functions interact with internal types. They provide basic functionality, such as conversions, string manipulation, mathematical operations, standard input/output, and more advanced features. See section [3.4.13](#3413---function-calls) for the call syntax.
