# Module CSV for BLOC

The "csv" module allows you to serialize/deserialize records formatted in the well-known CSV format.

## Loading the module

Like any other BLOC module, you have to load it with the statement `import`, before create objects from the type "csv".

```
import csv;
```

Once loaded, you can print the object interface.
```
help csv
```

Below, I expose some use cases.
```
import csv;

/*********************************/
/* Use case 1: Serialize a tuple */
/*********************************/
R = tup(1.23456, "Hello world!", true, -10);

TMP = csv(",").serialize(R);

print TMP;
/* will print:
1.23456,Hello world!,TRUE,-10
*/

/**********************************************/
/* Use case 2: Deserialise a mono-line record */
/**********************************************/
T = tab();
need_more_data = csv(",").deserialize(TMP, T);

print need_more_data; /* will print FALSE */

for i in 0 to T.count()-1 loop
  print "col " i " = " T.at(i);
end loop;

/* output will be
col 0 = 1.23456
col 1 = Hello world!
col 2 = TRUE
col 3 = -10
*/

/************************************************/
/* Use case 3: Serialize a table with format .| */
/************************************************/
T = tab(0, num());
for i in 1 to 10 loop T.concat(round(i * phi,3)); end loop;

TMP = csv(".|").serialize(T);

print TMP;
/* will print:
|1.618|.|3.236|.|4.854|.|6.472|.|8.09|.|9.708...
*/

/************************************************/
/* Use case 4: Deserialise a multi-lines record */
/************************************************/
data = tab(2, str());
data.put(0, "1.23456,""Hello world!\n"); /* 1st line of CSV record */
data.put(1, "HELLO WOLD!"",TRUE,-10"); /* Rest of CSV record */

T = tab();
i = 0;
p = csv(",");
need_more_data = p.deserialize(data.at(i), T);
while need_more_data loop
  i = i + 1;
  need_more_data = p.deserialize_next(data.at(i), T);
end loop;

for i in 0 to T.count()-1 loop
  print "col " i " = " T.at(i);
end loop;

/* output will be
col 0 = 1.23456
col 1 = Hello world!
HELLO WOLD!
col 2 = TRUE
col 3 = -10
*/
```
