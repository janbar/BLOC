BLOC 2.7 Reference Manual
=========================

2026-02

# Table of contents

* [Introduction](#introduction)

* [Basic Concepts](#basic-concepts)
  
  + [Values and Types](#values-and-types)
  + [Contexts and the Global environment](#contexts-and-the-global-environment)
  + [Error Handling](#error-handling)
  + [Type methods](#type-methods)
  + [Garbage Collection](#garbage-collection)

* [The language](#the-language)
  
  + [Lexical Conventions](#lexical-conventions)
  + [Variables](#variables)
  + [Statements](#statements)
  + [Expressions](#expressions)
  + [Null value handling and Type Introspection](#null-value-handling-and-type-introspection)
  + [Function Definitions](#function-definitions)
  + [Built-in Functions](#built-in-functions)

* [Index](#index)
  
  + [Statement index](#statement-index)
  + [Function index](#function-index)
  + [Constant index](#constant-index)

---

# Introduction

BLOC is a powerful, efficient, lightweight, embeddable scripting language. It supports procedural programming. BLOC is dynamically typed, runs in virtual context, and has automatic memory management, making it ideal for scripting, and rapid prototyping.

BLOC is implemented as a library, written in clean C, the common subset of standard C and C++. The BLOC distribution includes a host program called "bloc", which uses the BLOC core library to offer a complete, standalone BLOC interpreter, for interactive or batch use. BLOC is intended to be used both as a powerful, lightweight, embeddable scripting language for any program that needs one, and as a powerful but lightweight and efficient stand-alone language. As an extension language, BLOC has no notion of a "main" program: it works embedded in a host client, called the embedding program or simply the host. (Frequently, this host is the stand-alone "bloc" program.) The host program can invoke functions to execute a piece of BLOC code, can write and read BLOC variables. BLOC is extensible with dynamically imported modules. A module provides a new object-oriented type, with specialized features. Through the use of modules, BLOC can be augmented to cope with a wide range of different domains.

BLOC is free software, and is provided as usual with no guarantees, as stated in its license.

---

# Basic Concepts

This section describes the basic concepts of the language.

## Values and Types

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

See [Null Handling](#null-value-handling-and-type-introspection) for more details on handling null values.

**Integer** and **decimal** types are sub-types of number and can generally be combined in expressions. BLOC uses 64-bit integers and double-precision (64-bit) floats, on all platforms (64 or 32 bits). Unless stated otherwise, any overflow when manipulating integer values wrap around, according to the usual rules of two-complement arithmetic. (In other words, the actual result is the unique representable integer that is equal modulo 2 power of n to the mathematical result, where n is the number of bits of the integer type. BLOC has explicit rules about when each sub-type is used,  but it also converts between them automatically as needed (see [Conversions](#coercions-and-conversions)). To avoid pitfalls, the programmer should assume complete control over the representation of each number.

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

## Contexts and the Global environment

As we will see in more detail in sections [Variables](#variables) and [Assignment](#assignment), any reference to a free variable is syntactically associated with a context. Furthermore, each code block is compiled within the scope of a context. Therefore, free names in BLOC code refer to entries in the current context and are thus also called context variables.

The code block of function, on the other hand, is compiled in a self-contained child context. Variables are therefore local by nature; and access to values from another context is only possible by the parameters.

Functions can only be declared in the root context and are obviously accessible from any child context. Recursion is limited to 255 nested calls.

BLOC keeps a global environment. Common structures and values are initialized here and shared with all contexts.

- Current version and Compatibility Level
- Environment Country and Language
- The Random Number Generator
- Imported Modules

External modules can be loaded in advance, either programmatically or via a BLOC code.

---

## Error Handling

Several operations in BLOC can raise an error. An error interrupts the normal flow of the program, which can continue by catching the error. BLOC code can explicitly raise an error by calling the *raise* statement. BLOC code can intercept explicit error, zero-divide and out-of-range. For other runtime errors, the program stops and control returns to the host. To catch errors in BLOC, you can do a protected block, using *begin ... exception ... end* (see [Blocks](#blocks)). Any error while running inside the block stops its execution, and control returns immediately to the exception-handling routine. When an otherwise unprotected error occurs during the compilation or execution of a BLOC code, control returns to the host, which can take appropriate measures, such as printing an error message. Whenever there is an error, an error object is propagated with information about the error. BLOC generates errors whose values are a string and an identifier.

The error handler is called only for regular compilation or runtime errors. It is not called for memory-allocation errors nor for errors while running finalizers.

---

## Type Methods

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

**\* Tuple Accessor and Mutator**

To manipulate the elements of a tuple, a specific syntax is used in BLOC to distinguish it from that used for the table type, which could be confused. Accessing an element of the tuple is written as *{tuple}*  **\@** *{rank}*. Modifying an element of the tuple is done by calling the method *{tuple}* . **set@** *{rank}* ( *{new value}* ). The *set* method returns the tuple itself, allowing calls to be chained within the same statement.

Syntax of the tuple item accessor:

**expr ::= expr:tuple '@' rank**

Syntax of the tuple item mutator:

**expr:tuple ::= expr:tuple '.' method:set @ rank '(' expr ')'**

Note that *rank* is not an expression, but a coded value. It is evaluated at compile time. This constraint is imposed to the programmer, because conceptually a *tuple* has a fixed structure. 

---

## Garbage Collection

BLOC manages memory automatically, so you don't need to worry about manually allocating or freeing memory for objects.

It uses two pre-allocated stacks of values:

- **The persistent stack**: contains values that are used throughout the execution.

- **The temporary stack (or sandbox)**: handles temporary values created during execution.

When a new value is created, it is allocated on the temporary stack. During assignment, a pointer swap occurs between the temporary value and the stored value. The temporary stack contains only releasable data, meaning data that can be freed when no longer needed.

At the end of each statement or declaration, the temporary stack counter is reset to zero, which automatically releases all temporary values. The memory associated with these values is then freed during a new allocation on the temporary stack: the value pointed to by the counter is purged and then re-initialized with a new value.

Values of types number, boolean, or null do not have a specific destructor, so purging these values is inexpensive, since it doesn't require complex memory release.

In case of an error during execution, the state of the temporary stack is retained for analysis. The counter will be reset at the next instruction. It is also possible to manually purge the temporary stack programmatically or during context finalization.

---

# The Language

This section describes the lexis, the syntax, and the semantics of BLOC. In other words, this section describes which tokens are valid, how they can be combined, and what their combinations mean.

## Lexical Conventions

BLOC is a free-form language. It ignores spaces and comments between lexical elements (tokens), except as delimiters between two tokens. In source code, BLOC recognizes as spaces the standard ASCII white-space characters space, newline, carriage return, horizontal tab. Every executable statement in BLOC must end with a semicolon (`;`).

**\* Names**

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

**\* Literal strings**

A literal string can be delimited by matching double quotes, and can contain the following limited C-like escape sequences: `\a` (bell), `\b` (backspace), `\f` (form feed), `\n` (newline), `\r` (carriage return), `\t` (horizontal tab), `\\` (backslash). If a literal string requires to embed double quotes as part of a string then it must be escaped (\") or duplicated (double-double quotes).

**\* Numeral constants**

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

**\* Comments**

A line beginning with a sharp (`#`) outside a string, is interpreted as comment line. That is to be compatible with UNIX script Sha-Bang.

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

**\* At sign**

The at sign (`@`) is used to point to an element of tuple by its rank:

```
km = tup("A place", 389.0, "N-NW")@2;  // km is decimal 389.0
```

---

## Variables

Variables are places that store values. Before the first assignment to a variable, the name is not a valid syntactic identifier.

**var ::= Name**

A name prefixed with `$` denotes a variable with immutable type constraint. The immutable type constraint is also enabled for iterator variable inside loop block, regardless name prefix.

Any variable name is assumed to be visible in the current context. Outside a function block, the current context is the root context. Inside a function block, the context is private and self-contained.

Assigning new value to a variable, will free the memory allocated by old value; therefore assigning *null* will free the old, without new allocation.

---

## Statements

BLOC supports an almost conventional set of statements, similar to those in other PASCALIAN languages. This set includes blocks, assignments, control structures, function calls, standard output.

## Blocks

A block is a list of statements, which are executed sequentially. Blocks can be protected by an exception clause; on error control returns to the exception-routine, else to the parent block.

The interceptable errors are limited to the following:

- User Named error 
  - Raised using statement **raise** (See [Raise Statement](#raise-statement))
- **DIVIDE_BY_ZERO**
  - Attempt to divide by zero
- **OUT_OF_RANGE**
  - Number value does not fit in character or byte range (8 bits)
  - Integer constant or its string representation does not fit in BLOC integer value
  - Decimal constant or its string representation does not fit in BLOC decimal value

Exception clause can only be used in explicit block, starting with **begin**, ending by **end**.

**stat ::= begin {stat} [ exception {when Name then {stat}} ] end;**

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

The keyword **others** will match to any interceptable error. Logically, it should only be used in the last **when** clause.

## Assignment

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

## Control Structures

The control structures *if*, *while* have the usual meaning and familiar syntax:

**stat ::= if expr then {stat} {elsif expr then {stat}} [else {stat}] end if ;**

**stat ::= while expr loop {stat} end loop ;**

BLOC also has a for statement, in two flavors (see [For Statement](#for-statement), [Forall Statement](#forall-statement)).

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

## For Statement

The **for** statement is numerical for loop.

The numerical **for** loop repeats a block of code while a control variable goes through an arithmetic progression. It has the following syntax:

**stat ::= for Name in expr to expr [step expr] [asc|desc] loop {stat} end loop ;**

The given identifier (Name) defines the control variable of type *integer*.

The loop starts by evaluating once the three control expressions. Their values are called respectively the *initial value*, the *limit*, and the *step*. If the step is absent, it defaults to 1. The loop is done with integers; After that initialization, the loop body is repeated with the value of the control variable going through an arithmetic progression, starting at the initial value, with a common absolute difference given by the step. A step less than 1 raises an error. The loop continues for all value is in the range. The order of progression within the range is automatic, unless a constraint has been specified by keyword *asc* or *desc*. If the progression constraint cannot be met, the loop block is not executed.

The control variable can never wraps around, but the initial value or the limit could in case of overflow. Adding a progression constraint help to resolve this borderline case.

You can change the value of the control variable during the loop; Inside the loop the variable has the immutable type constraint. After exiting the loop, the constraint is deactivated.

## Forall Statement

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

## Function Calls as Statements

Function calls can be executed as statements.

**stat ::= [do] functioncall**

The keyword *do* is implicit, and therefore optional. 

In this case, all returned values are thrown away. Function calls are explained in section [Function Calls](#function-calls).

## Import Statement

The **import** statement is the directive to load an external module. External module provides new **object** type with features. Loading is performed immediately, so the new type can be used now.

**stat ::= import module ;**

The statement try to load the library *libbloc_{module}.so* on UNIX, or *libbloc_{module}.dylib* on OSX, or *bloc_{module}.dll* on WINDOWS.

Alternatively you can load a module using the library path.

**stat ::= import "path" ;**

## Include Statement

The **include** statement is the directive to embed another source. You can have up to three levels of nesting.

**stat ::= include "path" ;**

## Nop Statement

The **nop** statement does nothing.

**stat ::= nop ;**

## Print Statement

The **print** statement writes out the string representation of expression(s), terminated by a newline. Expressions are chained by spaces.

**stat ::= print [expr {expr}] ;**

The printed values are human readable text, depending of expression type.

## Put Statement

The **put** statement writes out the string value of expression(s). Expressions are chained by spaces.

**stat ::= put [expr {expr}] ;**

Only *boolean*, *integer*, *decimal*, and *string* expression can be written out. All other types must be converted first, i.e using built-in function **str**.

## Raise Statement

The **raise** statement throws a user named exception, or throwable error.

**stat ::= raise Name ;**

The following exception names are predefined in BLOC:

- DIVIDE_BY_ZERO

- OUT_OF_RANGE

Raised exception can be intercepted (See [Blocks](#blocks)).

## Return Statement

The **return** statement is used to return a value from a function or the program. It can be written anywhere; all enclosing loops will be terminated. The syntax for the return statement is:

**stat ::= return [ expr ] ;**

Return from the program, value type should be among boolean, integer, decimal, or string.  Any other value will be ignored.

## Trace Statement

The **trace** statement enable or disable trace verbosity.

**stat ::= trace expr:boolean ;**

Trace mode is intended for use only when absolutely necessary, typically for debugging a program. Performance are severely degraded due to the error output being jammed.

---

# Expressions

The basic expressions in BLOC are the following:

**expr ::= Numeral**

**expr ::= Literal**

**expr ::= expr binop expr**

**expr ::= unop expr**

**expr ::= var | builtin | functioncall | '(' expr ')'**

Numeral and literal strings are explained in [Lexical Conventions](#lexical-conventions); variables are explained in [Variables](#variables); function calls are explained in [Function Calls](#function-calls).

Binary operators comprise [Arithmetic Operators](#arithmetic-operators), [Bitwise Operators](#bitwise-operators), [Relational Operators](#relational-operators), [Logical Operators](#logical-operators), unary operators comprise the unary minus (see [Arithmetic Operator](#arithmetic-operators)), the unary bitwise not (see [Bitwise Operators](#bitwise-operators)), the unary logical not (see [Logical Operators](#logical-operators)).

## Arithmetic Operators

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

## Bitwise Operators

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

## Coercions and Conversions

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

## Relational Operators

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

## Logical Operators

BLOC supports the following logical operators:

| Symbol  | Operation            |
|:-------:| -------------------- |
| **and** | logical AND          |
| **or**  | logical OR           |
| **xor** | logical exclusive OR |
| **not** | logical negation NOT |

**(null OR true)** is always true; **(null AND false)** is always false.

Apart from these cases, these operators result in false, true or null if one of the operands is null.

## Concatenation

The type method **concat** (see [Type Methods](#type-methods)) performs concatenation in place. It is usable with all type of sequences: string, bytes and table. The argument can be of the same type or of the sequenced subtype. For string and bytes, the subtype is a 8bits integer.

Furthermore, for convenience, BLOC allows string concatenation with the operator `+`. In this case, both operands must be strings.

## Precedence

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

## Built-in Constants

BLOC defines several useful mathematical and boolean constants.

| Keyword   | Description                                           |
|:--------- |:----------------------------------------------------- |
| **null**  | The null value, with undefined type                   |
| **true**  | The boolean expression TRUE                           |
| **on**    | The boolean expression TRUE                           |
| **false** | The boolean expression FALSE                          |
| **off**   | The boolean expression FALSE                          |
| **ii**    | The imaginary number                                  |
| **pi**    | The ratio of a circle's circumference to its diameter |
| **ee**    | The Euler's number                                    |
| **phi**   | The golden ratio                                      |

The following constants are useful to intercept a functional error at runtime.

| Keyword            | Description                          |
|:------------------ |:------------------------------------ |
| **DIVIDE_BY_ZERO** | Attempting to divide by zero         |
| **OUT_OF_RANGE**   | Number is out of the range of values |

## Basic Constructors and Converters

BLOC provides some constructors for basic types. They are useful for initializing a typed null value or for performing a conversion from another type.

**\* boolean**

**bool()** returns the null boolean.

**bool( expr:integer )** converts the integer representation of boolean to boolean; it never fails. Any value other than 0 is converted to *true*.

**bool( expr:decimal )** converts the float representation of boolean to boolean; it never fails. Any value other than 0 is converted to *true*.

**\* string**

**str()** returns the null string. The character count is null.

**str( expr:integer )** returns the string representation of integer.

**str( expr:decimal )** returns the string representation of float.

**str( expr:bytes )** returns the string representation of bytes.

**chr( expr:integer )** returns the string (one character) of byte, or fails with out-of-range error.

**\* integer**

**int()** returns the null integer.

**int( expr:decimal )** converts the float to integer, or fails with out-of-range error.

**int( expr:string )** converts the string representation of an integer into an integer, or fails. Use the built-in function *isnum()* to test beforehand that the conversion is possible.

**int( expr:bytes )** converts the string (bytes) representation of an integer into an integer, or fails. Use the built-in function *isnum()* to test beforehand that the conversion is possible.

**\* decimal**

**num()** returns the null decimal.

**num( expr:integer )** converts the integer to float; it never fails.

**num( expr:string )** converts the string representation of a float to a float, or fails. Use the built-in function *isnum()* to test beforehand that the conversion is possible.

**num( expr:bytes )** converts the string (bytes) representation of a float to a float, or fails. Use the built-in function *isnum()* to test beforehand that the conversion is possible.

## Bytes Constructor

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

Finally the bytes can be updated using the type methods (see [Type Methods](#type-methods)).

## Tuple Constructor

Tuple constructor is the built-in function **tuple** that create tuples. Every time a constructor is evaluated, a new tuple is created. The constructor can be used to create a null tuple or to create a tuple and initialize its elements. The general syntax for constructors is the function call syntax.

**tup()** returns the null tuple without structure. The element count is null.

**tup( expr {, expr} )** returns the tuple for the given expressions. The expressions can be null, but they must be typed. The built-in function *bool()*, *str()*, *int()*, *num()*, and *raw()* provide a null typed value. The returned tuple is not null, therefore the element count is 1 or more.

Examples:

```
import file;
r=tup();                           // null tuple
r=tup(1, "file 1", file());        // not null tuple
```

Finally the tuple can be updated using the type methods (see [Type Methods](#type-methods)).

## Table Constructor

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

Finally the table can be updated using the type methods (see [Type Methods](#type-methods), [Concatenation](#concatenation)).

## Function Calls

A function call in BLOC has the following syntax:

**functioncall ::= name args**

Arguments have the following syntax:

**args ::= '(' \[expr { ',' expr }] ')'**

All argument expressions are evaluated before the call.

**\* User functions**

Argument values are passed by copy. Note that an object value is a reference; therefore, the object itself is not copied.

**\* Built-in functions and Object methods**

Argument values are passed by reference.

---

# Null value handling and Type Introspection

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

---

# Function Definitions

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

The **return** statement ends function execution and specifies a value to be returned to the caller (See [Return Statement](#return-statement)).

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

---

# Built-in Functions

BLOC provides a set of built-in functions. Built-in functions interact with internal types. They provide basic functionality, such as conversions, string manipulation, mathematical operations, standard input, and more advanced features. See section [Function Calls](#function-calls) for the call syntax.

## Abs Function

'**abs**' returns the absolute value of x.

abs( x )

## Acos Function

'**acos**' returns the arc cosine of x in radians.

acos( x )

## Asin Function

'**asin**' returns the arc sine of x in radians.

asin( x )

## Atan2 Function

'**atan2**' returns the angle in radians for the quadrant y , x.

atan2( y , x )

## Atan Function

'**atan**' returns the arc tangent of x in radians.

atan( x )

## B64dec Function

'**b64dec**' returns a bytes array that contains the base64-decoded version of the source x as a string or bytes array type.

b64dec( x )

## B64enc Function

'**b64enc**' returns a string that contains the base64-encoded version of source x as a string or bytes array type.

b64enc( x )

## Bool Function

'**bool**' returns the boolean representation of numeric x, or the null boolean.

bool( [ x ] )

When x is 0 the boolean false is returned, else true.

## Ceil Function

'**ceil**' returns the smallest integer value greater than or equal to x.

ceil( x )

## Chr Function

'**chr**' returns a character from the specified ASCII value x.

chr( x )

## Clamp Function

'**clamp**' restricts the given value x between the lower bound y and upper bound z. In this way, it acts like a combination of the min() and max() functions.

clamp( x , y , z )

## Cos Function

'**cos**' returns the cosine of a radian angle x.

cos( x )

## Cosh Function

'**cosh**' returns the hyperbolic cos of x.

cosh( x )

## Exp Function

'**exp**' returns the value of e raised to the x^th^ power.

exp( x )

## Floor Function

'**floor**' returns the largest integer value less than or equal to x.

floor( x )

## Getenv Function

'**getenv**' returns the corresponding value string of the environment variable x.

getenv( x )

## Getsys Function

'**getsys**' returns the corresponding value of context environment variable x.

getsys( x )

Where x could be any string of the following values.

| String        | Description                               |
|:------------- |:----------------------------------------- |
| `compatible`  | Language compatibility level              |
| `language`    | Lang code from locale                     |
| `country`     | Country code from locale                  |
| `integer_max` | Maximum value of integer                  |
| `integer_min` | Minimum value of integer                  |
| `system`      | The system type `UNIX` or `MSWIN`         |
| `endianess`   | The system bit order `BE` or `LE`         |
| `clock`       | Seconds elapsed since context incarnation |

The type of returned value depends of the selected variable.

## Hash Function

'**hash**' returns the DJB Hash value (32 bits) of string or bytes array. Optionally the number of buckets is specified by y [1..n].

hash( x [, y] )

## Hex Function

'**hex**' returns a hexadecimal-encoded string based on the number x, and optionally with the number y of leading zeros.

hex( x [, y] )

## Iconj Function

'**iconj**' returns the complex as the conjugate of the given complex.

iconj( x )

## Imag Function

'**imag**' returns the modulus or magnitude of the given complex.

imag( x )

## Input Function

'**input**' returns true when new data has been read from the input.
The read value without newline terminator, is stored into var. The *string* variable var must be initialized first. Optionally y contains the desired prompt.

input( var [, y] )

## Int Function

'**int**' returns the integer value of x else throw error, or the null integer.

int( [ x ] )

## Iphase Function

'**iphase**' returns theta or the phase of the given complex.

iphase( x )

## Isnull Function

'**isnull**' returns true if x is null.

isnull( x )

## Isnum Function

'**isnum**' returns true if x can be converted to number.

isnum( x )

## Log10 Function

'**log10**' returns the common logarithm (base-10 logarithm) of x.

log10( x )

## Log Function

'**log**' returns the natural logarithm (base-e logarithm) of x.

log( x )

## Lower Function

'**lower**' convert any uppercase letters in string x to lowercase letters.

lower( x )

## Lsubstr Function

'**lsubstr**' returns left part of string x.

lsubstr( x , count )

## Ltrim Function

'**ltrim**' removes whitespace from the left side of string x.

ltrim( x )

## Max Function

'**max**' returns the largest value among numbers x and y.

max( x , y )

## Min Function

'**min**' returns the smallest value among numbers x and y.

min( x , y )

## Mod Function

'**mod**' returns the remainder of x divided by y.

mod( x , y )

See [Arithmetic Operators](#arithmetic-operators).

## Num Function

'**num**' returns the decimal value of x else throw error, or the null decimal.

num( [ x ] )

## Pow Function

'**pow**' returns x raised to the power of y.

pow( x , y )

See [Arithmetic Operators](#arithmetic-operators).

## Random Function

'**random**' returns pseudo-random number that's greater than or equal to 0 and less than 1 or optionally x.

random( [ x ] )

## Raw Function

'**raw**' returns a new bytes array filled with string x, or filled with x byte y, or the null bytes array.

raw( [ x [, y] ] )

Bytes array has the method *at*, *put*, *insert*, *delete*, *count*, and *concat( bytes | integer )*.

## Readln Function

'**readln**' returns true when new line has been read from the input. The read data ended with LF or limited to the size of the internal buffer, is stored into var. The *string* or *bytes* variable var must be initialized first.

readln( var )

## Read Function

'**read**' returns the count of characters or bytes read from the input. The read data are stored into var with a maximum of y bytes. The *string* or *bytes* variable var must be initialized first.

read( var [, y] )

## Replace Function

'**replace**' returns a string from x with all occurrences of y replaced by z.

replace( x , y , z)

## Round Function

'**round**' returns a value to the nearest integer, or round to the decimal place y.

round( x [, y] )

## Rsubstr Function

'**rsubstr**' returns right part of string x.

rsubstr( x , count )

## Rtrim Function

'**rtrim**' Remove whitespace from the right side of string x.

rtrim( x )

## Sign Function

'**sign**' returns the signed unity of x, i.e the number +1 or -1.

sign( x )

## Sin Function

'**sin**' returns the sine of a radian angle x.

sin( x )

## Sinh Function

'**sinh**' returns the hyperbolic sine of x.

sinh( x )

## Sqrt Function

'**sqrt**' returns the square root of x.

sqrt( x )

## Strlen Function

'**strlen**' returns the size of string x.

strlen( x )

See *count* in section [Type Methods](#type-methods) for type *string*.

## Strpos Function

'**strpos**' finds the position of the first occurrence of the string y starting at position z, inside the string x. If none then null is returned.

strpos( x , y [, z] )

## Str Function

'**str**' returns the string representation of x, or the null string.

str( [ x ] )

String has the method *at*, *put*, *insert*, *delete*, *count*,
and *concat( string | integer )*.

## Subraw Function

'**subraw**' returns a part of bytes array x.

subraw( x , begin [, count] )

## Substr Function

'**substr**' returns a part of string x.

substr( x , begin [, count] )

## Tab Function

'**tab**' returns a new uniform table filled with x element(s) y, or the null table.

tab( [ x , y ] )

Nested element can be any type, or tuple. Nesting level is supported up
to 254 dimensions. Table has the methods *at*, *put*, *insert*, *delete*, *count*, and *concat( table | element )*.

## Tan Function

'**tan**' returns the tangent of a radian angle x.

tan( x )

## Tanh Function

'**tanh**' returns the hyperbolic tangent of x.

tanh( x )

## Tokenize Function

'**tokenize**' splits the values in the string x, using the string y as separator. If the boolean z is true, then null values will be trimmed. The result is returned as table.

tokenize( x , y [, z] )

## Trim Function

'**trim**' Remove whitespace from the left and right side of string x.

trim( x )

## Tup Function

'**tup**' returns a new tuple of items, or the null tuple.

tup( [ x [, ...] ] )

Item can be boolean, integer, decimal, complex, string, object, or bytes. Nesting and table are not allowed. Tuple is unchangeable, meaning that we cannot change type, add or remove items after the tuple has been created.
The operator '@'{rank} accesses the item at position [1..n], as shown below. Expression {tuple}@1 returns the value of item at position 1.
Tuple has the method *set@{rank}(x)* to assign the value x to the item of rank #. Item type must be qualified. Therefore to assign null you have to use one of built-in function providing the null type qualified, i.e *bool()*, *int()*, *num()*, *str()* or *raw()*.

## Typeof Function

'**typeof**' returns the value type in literal form:
`undefined`, `boolean`, `integer`, `decimal`, `string`, `bytes`, `complex`, `tuple`, `table`.

typeof( x )

## Upper Function

'**upper**' converts any lowercase letters in string x to uppercase letters.

upper( x )

---

# Index

## Statement Index

[begin](#blocks)

[break](#control-structures)

[continue](#control-structures)

[do](#function-calls-as-statements)

[exception](#blocks)

[for](#for-statement) 

[forall](#forall-statement)

[function](#function-definitions)

[if](#control-structures)

[import](#import-statement)

[include](#include-statement)

[let](#assignment)

[nop](#nop-statement)

[print](#print-statement)

[put](#put-statement)

[raise](#raise-statement)

[return](#return-statement)

[trace](#trace-statement)

[while](#control-structures)

## Function Index

[abs](#abs-function)

[acos](#acos-function)

[asin](#asin-function)

[atan](#atan-function)

[atan2](#atan2-function)

[b64dec](#b64dec-function)

[b64enc](#b64enc-function)

[bool](#bool-function)

[ceil](#ceil-function)

[chr](#chr-function)

[clamp](#clamp-function)

[cos](#cos-function)

[cosh](#cosh-function)

[exp](#exp-function)

[floor](#floor-function)

[getenv](#getenv-function)

[getsys](#getsys-function)

[hash](#hash-function)

[hex](#hex-function)

[iconj](#iconj-function)

[imag](#imag-function)

[input](#input-function)

[int](#int-function)

[iphase](#iphase-function)

[isnull](#isnull-function)

[isnum](#isnum-function)

[log](#log-function)

[log10](#log10-function)

[lower](#lower-function)

[lsubstr](#lsubstr-function)

[ltrim](#ltrim-function)

[max](#max-function)

[min](#min-function)

[mod](#mod-function)

[num](#num-function)

[pow](#pow-function)

[random](#random-function)

[raw](#raw-function)

[read](#read-function)

[readln](#readln-function)

[replace](#replace-function)

[round](#round-function)

[rsubstr](#rsubstr-function)

[rtrim](#rtrim-function)

[sign](#sign-function)

[sin](#sin-function)

[sinh](#sinh-function)

[sqrt](#sqrt-function)

[str](#str-function)

[strlen](#strlen-function)

[strpos](#strpos-function)

[subraw](#subraw-function)

[substr](#substr-function)

[tab](#tab-function)

[tan](#tan-function)

[tanh](#tanh-function)

[tokenize](#tokenize-function)

[trim](#trim-function)

[tup](#tup-function)

[typeof](#typeof-function)

[upper](#upper-function)

## Constant Index

[ee](#built-in-constants)

[false](#built-in-constants)

[ii](#built-in-constants)

[null](#built-in-constants)

[off](#built-in-constants)

[on](#built-in-constants)

[phi](#built-in-constants)

[pi](#built-in-constants)

[true](#built-in-constants)

[DIVIDE_BY_ZERO](#built-in-constants)

[OUT_OF_RANGE](#built-in-constants)
