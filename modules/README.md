# Modules for BLOC

Modules extend the functionality of the language. For more details about a particular module, see the README file in the module folder.

## Loading a module

Required module has to be loaded with the statement `import`, before to use it.

## Examples

Below, I expose a sample usage with the modules `file`, `csv`, and `sqlite3`.
```
import file;
import csv;
import sqlite3;

/* a CSV parser with format ';"' */
CSV = csv(";");

/***********************************/
/* create a sample CSV file        */
/***********************************/
F = file("data.csv","w");
F.write( CSV.serialize( tup("SNAP_ID","SNAP_TIME","STAT") ));
F.write("\n");
F.write( CSV.serialize( tup(8364,"23/03/2020 02:00:02",40) ));
F.write("\n");
F.write( CSV.serialize( tup(8365,"23/03/2020 03:00:02",220) ));
F.write("\n");
F.write( CSV.serialize( tup(8366,"23/03/2020 04:00:00",115930) ));
F.write("\n");
F.write( CSV.serialize( tup(8367,"23/03/2020 05:00:01",3219) ));
F.write("\n");
F.write( CSV.serialize( tup(8368,"23/03/2020 06:00:03",292) ));
F.flush();
F.close();

/***********************************/
/* open or create a new database   */
/***********************************/
DB = sqlite3("data.db");
DB.exec("drop table stats");
DB.exec("create table stats (id integer, timestr varchar2(20), stat integer)");

/***********************************/
/* MAIN                            */
/***********************************/
print "\nLoad data into database...";
/* open CSV file for read */
F = file("data.csv","r");

LINE = null; /* a line of data */
T = null; /* parsed values from CSV row */
NEEDMORE = null; /* status of CSV row */

while F.readln(LINE) loop
    if NEEDMORE then
        /* add more data to finalize CSV row */
        NEEDMORE = CSV.deserialize_next(LINE, T);
    else
        /* push data of new CSV row */
        NEEDMORE = CSV.deserialize(LINE, T);
    end if;
    /* discard the CSV header */
    if !isnum(T.at(0)) then continue; end if;
    /* */
    VALUES = tup(int(T.at(0)),str(T.at(1)),int(T.at(2)));
    if !DB.exec("insert into stats values (:1,:2,:3)", VALUES) then
      print "!!! Error inserting new row !!!";
    else
      print "INSERT: " VALUES;
    end if;
end loop;
F.close();

/***********************************/
/* query stored data               */
/***********************************/
print "\nRead the database...";
R = DB.query("select * from stats");
forall E in R loop
  print "QUERY: " E;
end loop;

/* finally close the database */
DB.close();
```
