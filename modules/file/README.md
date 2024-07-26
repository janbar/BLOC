# Module FILE for BLOC

The "file" module allows file handling.

## Loading the module

Like any other BLOC module, you have to load it with the statement `import`, before create objects from the type "file".

```
import file;
```

Once loaded, you can print the object interface.
```
help file
```

```
CONSTRUCTORS

file() is default contructor

file(string, string)
Build and open file for the given filename, and flags.


METHODS

close() returns boolean
Close the file handle.

open(string IN, string IN) returns integer
Opens a file indicated by filename and flags, and returns the system error
number or 0 for success.

write(string IN) returns integer
Writes string, and returns the number of character written successfully.

read(string INOUT, integer IN) returns integer
Reads up to count characters into the variable, and returns the number of
character read successfully.

readln(string INOUT) returns boolean
Reads a chunk of characters into the variable, ended by LF or limited by
the size of the internal buffer. At EOF it returns FALSE else TRUE.

seekset(integer IN) returns integer
Moves the file position indicator to an absolute location in a file.

seekcur(integer IN) returns integer
Moves the file position indicator from the current file position.

seekend(integer IN) returns integer
Moves the file position indicator from the end of the file.

position() returns integer
Returns the file position indicator.

flush() returns boolean
synchronizes output stream with the actual file.

filename() returns string
Returns filename of the actual file.

isopen() returns boolean
Checks whether the file is open.

write(bytes IN) returns integer
Writes bytes, and returns the number of byte written successfully.

read(bytes INOUT, integer IN) returns integer
Reads up to count bytes into the variable, and returns the number of byte
read successfully.

mode() returns string
Returns open flags of the actual file.

stat(string IN) returns integer
Returns information about a file: Regular=1, Directory=2, Other=3.

dir(string IN) returns [{ integer, string }]
Returns the entry list of a directory.
First field contains the entry type, second contains the entry name.
```
