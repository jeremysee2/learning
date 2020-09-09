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

Using the `sqlite3` module in Python to create tables, is similar to using raw SQL where the query is stored in a string variable that is passed to `cursor.execute()`.

```py
import sqlite3
import csv
from sqlite3 import Error

def create_connection(path):
    connection = None
    try:
        connection = sqlite3.connect(path)
        print("Connection to SQLite DB successful")
    except Error as e:
        print(f"Error {e} occurred")

def execute_query(connection, query):
    cursor = connection.cursor()
    try:
        cursor.execute(query)
        connection.commit()
        print("Query executed successfully")
    except Error as e:
        print(f"The error '{e}' occurred")

# Create table
create_users_table = """
CREATE TABLE IF NOT EXISTS users (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL,
  age INTEGER,
  gender TEXT,
  nationality TEXT
);
"""

execute_query(connection, create_users_table)

# To insert data. Because id column is AUTOINCREMENT, don't need to provide values.
create_users = """
INSERT INTO
  users (name, age, gender, nationality)
VALUES
  ('James', 25, 'male', 'USA'),
  ('Leila', 32, 'female', 'France'),
  ('Brigitte', 35, 'female', 'England'),
  ('Mike', 40, 'male', 'Denmark'),
  ('Elizabeth', 21, 'female', 'Canada');
"""

execute_query(connection, create_users)

# SELECTing records. Not recommended to use SELECT * on large tables because of large number of I/O operations
def execute_read_query(connection, query):
    cursor = connection.cursor()
    result = None
    try:
        cursor.execute(query)
        result = cursor.fetchall()
        return result
    except Error as e:
        print(f"The error '{e}' occurred")
select_users = "SELECT * from users"
users = execute_read_query(connection, select_users)

for user in users:
    print(user)

# JOIN operations to retrieve data from related tables. Retrieve user id and names, with their posts.
select_users_posts = """
SELECT
  users.id,
  users.name,
  posts.description
FROM
  posts
  INNER JOIN users ON users.id = posts.user_id
"""

users_posts = execute_read_query(connection, select_users_posts)

for users_post in users_posts:
    print(users_post)

# Alternatively...
select_posts_comments_users = """
SELECT
  posts.description as post,
  text as comment,
  name
FROM
  posts
  INNER JOIN comments ON posts.id = comments.post_id
  INNER JOIN users ON users.id = comments.user_id
"""

posts_comments_users = execute_read_query(
    connection, select_posts_comments_users
)

for posts_comments_user in posts_comments_users:
    print(posts_comments_user)

# WHERE query as a condition
select_post_likes = """
SELECT
  description as Post,
  COUNT(likes.id) as Likes
FROM
  likes,
  posts
WHERE
  posts.id = likes.post_id
GROUP BY
  likes.post_id
"""

post_likes = execute_read_query(connection, select_post_likes)

for post_like in post_likes:
    print(post_like)

# Updating Table Records
update_post_description = """
UPDATE
  posts
SET
  description = "The weather has become pleasant now"
WHERE
  id = 2
"""

execute_query(connection, update_post_description)

# Using nested queries to search in one table using properties from another table
"SELECT * FROM shows WHERE id IN (SELECT show_id FROM genres WHERE genre = 'Comedy') AND year = 2019;"
```

Steps:

1. Create a table with appropriate columns and their datatypes (e.g. INT, FLOAT, DATE, CHAR, TEXT, BLOB, ENUM, GEOMETRY...) CHAR would be a fixed-length string, VARCHAR is a variable-length string.
2. Primary Key for one column, ensure that each rows are unique and quickly identified. Joint primary key is also possible. You can set the Primary Key to AUTOINCREMENT.
3. Common to use two different tables together to hold the identity and property of that item separately.

Alternatives to SQLite are MySQL and PostgreSQL. Can use a GUI like phpMyAdmin for setting up databases.
