# Module ORACLE for BLOC

The "oracle" module allows Oracle database handling.

## Loading the module

Like any other BLOC module, you have to load it with the statement `import`, before create objects from the type "oracle".
An environment variable, i.e ORACLE_HOME, LD_LIBRARY_PATH or PATH, should be configured to locate the Oracle Client Library.
The simplest Oracle Client is the free Oracle Instant Client. Only the "Basic" or "Basic Light" package is required.
Using Oracle Instant Client, you should configure the variable PATH or LD_LIBRARY_PATH.
Else using an Oracle Database installation (>= 11.2), the variable ORACLE_HOME should be configured.

```
import oracle;
```

Once loaded, you can print the object interface.
```
help oracle
```

```
CONSTRUCTORS

oracle() is default contructor

oracle(object)
Clone a database connection.
The current session will be shared with the new clone.

oracle(string, string, string)
Opening a new database connection.
1  : connect string
2  : user
3  : password

oracle(string, string, string, string)
Opening a new database connection with auth mode.
1  : connect string
2  : user
3  : password
4  : auth mode ( SYSDBA | SYSOPER | SYSASM )


METHODS

open(string IN, string IN, string IN) returns boolean
Opening a new database connection.
1  : connect string
2  : user
3  : password

open(string IN, string IN, string IN, string IN) returns boolean
Opening a new database connection with auth mode.
1  : connect string
2  : user
3  : password
4  : auth mode ( SYSDBA | SYSOPER | SYSASM )

close() returns boolean
Closing a database connection.

isopen() returns boolean
Check a database connection is open.

commit() returns boolean
Commits the current active transaction.

rollback() returns boolean
Rolls back the current active transaction.

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
