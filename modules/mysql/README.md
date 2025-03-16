# Module MYSQL for BLOC

The "mysql" module allows MySQL database handling.

## Loading the module

Like any other BLOC module, you have to load it with the statement `import`, before create objects from the type "mysql".

```
import mysql;
```

Once loaded, you can print the object interface.
```
help mysql
```

```
CONSTRUCTORS

mysql() is default contructor

mysql(object)
Clone a database connection.

mysql(string, string, string, string, integer)
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

autocommit(boolean IN) returns boolean
Sets autocommit mode on or off.

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
