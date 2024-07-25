# Module DATE for BLOC

The "date" module allows calendar handling.

## Loading the module

Like any other BLOC module, you have to load it with the statement `import`, before create objects from the type "date".

```
import date;
```

Once loaded, you can print the object interface.
```
help date
```

```
CONSTRUCTORS

date() is default contructor

date(string)
Build a new calendar from the given ISO date or time string.
Passing an empty string or null, it provides POSIX epoch.

date(object)
Build a new calendar as a copy of the given one.

date(string, string)
Build a new calendar from the given string and format.


METHODS

format(string IN) returns string
Formats calendar time according to the directives in the given format string.

unixtime() returns integer
Converts calendar time to time since epoch.

iso8601() returns string
Formats calendar time according to ISO8601 format string.

iso8601utc() returns string
Formats calendar time UTC according to ISO8601 format string.

isodate() returns string
Formats calendar according to ISO date format string.

difftime(object IN) returns decimal
Computes the difference between times in seconds.

add(decimal IN) returns object
Add or remove day(s) to the calendar.

add(integer IN, string IN) returns object
Add or remove unit(s) to the calendar.
Unit could by among second, minute, hour, day, month, or year.

second() returns integer
Returns second of calendar.

minute() returns integer
Returns minute of calendar.

hour() returns integer
Returns hour of calendar.

day() returns integer
Returns day of calendar.

month() returns integer
Returns month of calendar.

year() returns integer
Returns year of calendar.

weekday() returns integer
Returns week day of calendar. Monday=1 ... Sunday=7.

yearday() returns integer
Returns year day of calendar.

trunc() returns object
Truncate time to the calendar.

trunc(string IN) returns object
Truncate the calendar to a specific unit.
Unit could by among minute, hour, day, month, or year.
```
