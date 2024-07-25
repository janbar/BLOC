# Module UTF8 for BLOC

The "utf8" module allows UTF8 string handling.

## Loading the module

Like any other BLOC module, you have to load it with the statement `import`, before create objects from the type "utf8".

```
import utf8;
```

Once loaded, you can print the object interface.
```
help utf8
```

```
CONSTRUCTORS

utf8() is default contructor

utf8(string)
Build a new unicode string from the standard string.

utf8(object)
Build a new unicode string as a copy of the given one.


METHODS

empty() returns boolean
Checks whether the unicode string is empty.

count() returns integer
Returns the count of code points contained in the unicode string.

rawsize() returns integer
Returns the size in bytes of the unicode string.

reserve(integer IN) returns boolean
Reserves storage for code point capacity.

clear() returns boolean
Clears the contents.

append(integer IN) returns object
Appends a code point to the end.

append(string IN) returns object
Appends a standard string to the end.

concat(object IN) returns object
Appends an unicode string to the end.

string() returns string
Returns a standard string from the content.

at(integer IN) returns integer
Returns the code point at the position in content.

remove(integer IN, integer IN) returns boolean
Removes code points from the position, and up to the given count.

insert(integer IN, integer IN) returns boolean
Inserts at the position, the given code point.

insert(integer IN, object IN) returns integer
Inserts at the position, the given unicode string.

substr(integer IN) returns string
Returns a standard string from sub content starting at the position.

substr(integer IN, integer IN) returns string
Returns a standard string from sub content starting at the position, and up
to count of code point.

toupper() returns object
Transform the content to upper case.

tolower() returns object
Transform the content to lower case.

normalize() returns object
Normalize the content, i.e removing extra spaces or control characters.

capitalize() returns object
Capitalize all words of the content.

translit() returns object
Transliterates the content to an ASCII string.
```
