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
Open file for the given filename, and flags:
r or rb    Open file for reading.
w or wb    Truncate to zero length or create file for writing
a or ab    Append, open or create file for writing at end-of-file
r+ or rb+  Open file for update (reading and writing)
w+ or wb+  Truncate to zero length or create file for update
a+ or ab+  Append, open or create file for update, writing at end-of-file


METHODS

close() returns boolean
Close the file handle.

open(string IN, string IN) returns integer
Opens the file indicated by path and flags, and returns the system error
number or 0 for success.

write(string IN) returns integer
Writes string, and returns the number of character written successfully.

write(bytes IN) returns integer
Writes bytes, and returns the number of byte written successfully.

flush() returns boolean
Synchronizes output stream with the file.

readln(string INOUT) returns boolean
Reads a chunk of characters into the variable, ended by LF or limited by
the size of the internal buffer. At EOF it returns FALSE else TRUE.

read(string INOUT, integer IN) returns integer
Reads up to count characters into the variable, and returns the number of
character read successfully.

read(bytes INOUT, integer IN) returns integer
Reads up to count bytes into the variable, and returns the number of byte
read successfully.

seekset(integer IN) returns integer
Moves the file position indicator to an absolute location in a file.

seekcur(integer IN) returns integer
Moves the file position indicator from the current file position.

seekend(integer IN) returns integer
Moves the file position indicator from the end of the file.

position() returns integer
Returns the file position indicator.

isopen() returns boolean
Checks whether the file is open.

mode() returns string
Returns open flags of the file.

filename() returns string
Returns filename of the file.

dirname() returns string
Returns the dirname of the file.

basename() returns string
Returns the basename of the file.

stat() returns { integer, integer, string, integer }
Returns a tuple containing basic informations about the file:
{ Type, Size, Absolute Path, CTime }
Type: 0=Invalid, 1=Regular, 2=Directory, 3=Other
Size: file size in byte

stat(string IN) returns { integer, integer, string, integer }
Returns a tuple containing basic informations about the given path:
{ Type, Size, Absolute Path, CTime }
Type: 0=Invalid, 1=Regular, 2=Directory, 3=Other
Size: file size in byte

dir(string IN) returns [{ integer, integer, string, integer }]
Returns the entry list of a directory: [{ Type, Size, Name, CTime }]
Type: 1=Regular, 2=Directory, 3=Other
Size: file size in byte

separator() returns string
Returns the file separator character.

dirname(string IN) returns string
Returns the dirname of the given path.

basename(string IN) returns string
Returns the basename of the given path.
```
