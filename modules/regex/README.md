# Module regex for BLOC

The "regex" module provides a type that represents regular expressions, which are a kind of mini-language used to perform pattern matching within strings.

## Loading the module

Like any other BLOC module, you have to load it with the statement `import`, before create objects from the type "regex".

```
import regex;
```

Once loaded, you can print the object interface.
```
help regex
```

```
CONSTRUCTORS

regex() is default contructor

regex(string)
Create a new regex for the given expression.

regex(object)
Build a new regex as a copy of the given one.


METHODS

search(string IN) returns [{ integer, integer, string }]
Attempts to match the regular expression to any part of a string,
and returns a table of tuple containing the matched sequences:
{ Position, Length, Sequence that matched }

replace(string IN, string IN) returns string
Replaces occurrences of the regular expression with formatted
replacement string.
1: target string
2: the regex replacement format string
```

Below, I expose some use cases.
```
import regex;

re = regex("^(.+)-(.+)$");

forall e in re.search("prefix-suffix")
loop
  print "# " e;
end loop;

# 0, 13, "prefix-suffix"
# 0, 6, "prefix"
# 7, 6, "suffix"

re=regex("a|e|i|o|u");

print "# " re.replace("Quick brown fox","*");

# Q**ck br*wn f*x

re=regex("\\b(sub)([^ ]*)");

print "# " re.replace("There is a subsequence in the string\n", "sub-$2");

# There is a sub-sequence in the string
```
