# Module SYS for BLOC

The "sys" module allows you to run system command or a block of commands.

## Loading the module

Like any other BLOC module, you have to load it with the statement `import`, before create objects from the type "sys".

```
import sys;
```

Once loaded, you can print the object interface.
```
help sys
```

```
CONSTRUCTORS

sys() is default contructor


METHODS

exec(string IN) returns boolean
Execute a system command or a block of commands.

exec(string IN, bytes INOUT, integer IN) returns boolean
Execute a system command or a block of commands, and tail the output into
the specified variable as a byte array with the given max size.

setenv(string IN, string IN) returns string
Sets the environment variable to the given value and returns the old value.
Passing a null value will unset the variable.

sleep(integer IN) returns integer
Delay for a specified amount of time in seconds.
```
