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

## Sample usage

First I downloaded the basic instant client, and copied the libraries in the
folder `/Users/foo/instantclient_19_8/lib`. So the environ ORACLE_HOME should
be set with the path `/Users/foo/instantclient_19_8`.

```
import sys;
import oracle;

/* set environ ORACLE_HOME */
sys().setenv("ORACLE_HOME", "/Users/foo/instantclient_19_8");

db = oracle("192.168.2.3:1521/orcl", "system", "manager");

/* is the db connection valid ? */
if not db.isopen() then
  raise FAILED_TO_CONNECT;
end if;

/* loop resultset of one-step query */
r = db.query("select count(*),max(num_rows),max(last_analyzed)
  from dba_tables");

forall e in r loop print "# " e; end loop;

# 6241, 100111553, "2026-02-25T21:02:00Z"

/* loop resultset of one-step query with parameters */
r = db.query("select table_name, tablespace_name from dba_tables
  where owner = :1 and num_rows > :2" , tup("FOO", 1e6) );

forall e in r loop print "# " e; end loop;

# "MYTAB1", "DATA"
# "MYTAB2", "DATA"

/* fetch loop in resultset */
db.prepare("select table_name, tablespace_name from dba_tables
  where owner = :1 and num_rows > :2");

db.execute(tup("FOO", 1e6));

forall e in db.header() loop
  print "# type of column " e@1 " is " e@2;
end loop;

# type of column TABLE_NAME is string
# type of column TABLESPACE_NAME is string

row = tup();

while db.fetch(row) loop print "# " row; end loop;

# "MYTAB1", "DATA"
# "MYTAB2", "DATA"

/* close and execute a new one */
db.execute(tup("SYS", 1e3));

while db.fetch(row) loop print "# " row; end loop;

# "TAB$", "SYSTEM"
# "IND$", "SYSTEM"
# "ICOL$", "SYSTEM"
# "COL$", "SYSTEM"
# "LOB$", "SYSTEM"
# ...

/* close prepared statement */
db.finalize();

/* share the session for concurrent calls */
db2 = oracle(db);

/* close db connection... not really as long as a shared session exists */
db.close();

print "# " db2.isopen();

# TRUE

db2.close();
```
