# Python Notes (Lecture 6)

## Regular Expressions

* . represents any character
* .* 0 or more characters
* .+ 1 or more characters
* ? optional
* ^ start of input
* $ end of input

```py
import re

s = str(input("Do you agree?\n"))

if re.search("yes",s):
    print("Agreed.")
elif re.search("no",s):
    print("Not agreed.")
```

Sorting items can be done with `sorted()`, with an optional parameter `key` to use a function to select which value to sort by.

Lambda functions are anonymous, single-use functions. The format is:

```py
print(lambda input: output)
```

## CSV / TSV Files

Python can handle csv or tsv files using the csv library.

```py
import csv

with open("file.tsv", "r") as titles:
    reader = csv.DictReader(titles, delimiter="\t")

    with open("shows0.csv", "w") as shows:
        writer = csv.writer(shows)
        writer.writerow(["tconst", "primaryTitle", "year"])
        for row in reader:
            if row["startYear"] != "\\N":
                if row["titleType"] == "tvSeries":
                    writer.writerow([row["tconst"], row["primaryTitle"], row["year"]])
```

## SQL Notes

Basic operations of data manipulation:

* Create
* Read
* Update
* Delete

Run using `sqlite3`.

```bash
> sqlite3 favourites.db
> .mode csv
> .import "filename"
> ls
```

Some basic functions:

```sql
INSERT
SELECT
UPDATE
DELETE

WHERE
LIKE
LIMIT
GROUP BY
ORDER BY
JOIN

CREATE TABLE table (column type, ...);
SELECT DISTINCT TITLE FROM DATABASE

SELECT columns FROM table WHERE condition;
DELETE FROM table WHERE condition;
DROP TABLE; // delete everything

// Data structures
INTEGER
    smallint
    bigint
    integer
REAL
    real
    double precision
NUMERIC
    boolean
    date
    datetime
    numeric(scale,precision)
    time
    timestamp
TEXT
    char(n)    // number of characters in each cell
    varchar(n) // max characters in each cell
    text
BLOB // binary format
```

## SQL in Python

```py
db = 
```