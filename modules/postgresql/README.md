# Module POSTGRESQL for BLOC

The "postgresql" module allows PostgreSQL database handling.

### Build the module on Unix

Install the development files for postgresql client.

### Build the module on Windows

You should configure the library and header locations for the CMake project.

```
-DPostgreSQL_INCLUDE_DIR=C:/INSTALL_PATH/include/postgresql
-DPostgreSQL_LIBRARY=C:/INSTALL_PATH/lib/libpq.lib
```

Finally you should copy the DLL `libpq.dll` into the PATH of the BLOC binaries.

## Loading the module

Like any other BLOC module, you have to load it with the statement `import`, before create objects from the type "postgresql".

```
import postgresql;
```

Once loaded, you can print the object interface.
```
help postgresql
```

```
CONSTRUCTORS

postgresql() is default contructor

postgresql(object)
Clone a database connection.

postgresql(string, string, string, string, integer)
Opening a new database connection.
1  : host
2  : user
3  : password
4  : database
5  : port


METHODS

open(string IN, string IN, string IN, string IN, integer IN) returns boolean
Opening a new database connection.
1  : host
2  : user
3  : password
4  : database
5  : port

close() returns boolean
Closing a database connection.

isopen() returns boolean
Check a database connection is open.

commit() returns boolean
Commits the current transaction, making its changes permanent.

rollback() returns boolean
Rolls back the current transaction, canceling its changes.

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

execute({ } IN) returns boolean
Bind and execute the prepared statement.

execute() returns boolean
Execute the prepared statement.

header() returns [{ }]
Return the header as table of tuple { name:string, type:string }.

fetch({ } INOUT) returns boolean
Fetch next row.

finalize() returns boolean
Close the prepared statement.
```
