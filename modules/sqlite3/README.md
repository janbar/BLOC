# Module SQLITE3 for BLOC

The "sqlite3" module allows SQLite3 database handling.

## Loading the module

Like any other BLOC module, you have to load it with the statement `import`, before create objects from the type "sqlite3".

```
import sqlite3;
```

Once loaded, you can print the object interface.
```
help sqlite3
```

```
CONSTRUCTORS

sqlite3() is default contructor

sqlite3(object)
Clone a database connection.

sqlite3(string)
Opening a new database connection.


METHODS

open(string IN) returns boolean
Opening a new database connection.

close() returns boolean
Closing a database connection.

isopen() returns boolean
Check a database connection is open.

query(string IN) returns [{ }]
One-step query execution.

query(string IN, { } IN) returns [{ }]
One-step query execution with bound parameters.

exec(string IN) returns boolean
One-step statement execution.

exec(string IN, { } IN) returns boolean
One-step statement execution with bound parameters.

errmsg() returns string
Return text that describes the error.

prepare(string IN) returns boolean
Prepare a new statement object.

bind({ } IN) returns boolean
Bind parameters for the prepared statement.

execute() returns boolean
Execute the prepared statement.

header() returns [{ }]
Return the header as table of tuple { name:string, type:string }.

fetch({ } INOUT) returns boolean
Fetch next row.

finalize() returns boolean
Close the prepared statement.
```
