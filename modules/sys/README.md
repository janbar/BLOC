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

Below, I expose the use case.
```
import sys;

ret = sys().exec("touch dummy.tmp");
```
