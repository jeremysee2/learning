# C Programming (CS50)

## Data Types

* `int`: 4 bytes
  * Unsigned int: double the range, but no access to negative values
* `char`: 1 byte, mapped to integers through ASCII
* `float`: 4 bytes, limited precision
* `double`: 8 bytes, higher precision
* `bool`: `true` or `false`
* `string`: collection of characters (not in C)
* `struct`/`typdef`: create your own datatype
* `void`: doesn't return a value, a type and not a datatype

Qualifiers

* `Unsigned`: double the range, but no access to negative values
* `Short`: 2 bytes
* `Long`: 8 bytes
* `Const`: cannot be changed in the rest of the program

Operators

* add: +
* subtract: -
* multiply: *
* divide: /
* modulus: %
* logical AND: &&
* logical OR: ||
* logical NOT: !
* comparison: > < <= >= ==

Conditionals

* `if`

```C
if ()
{
    // Do something
}
if ()
{
    // Do something if #2 is true
}
else ()
{
   // Else do something
}
```

* `switch`

```C
switch(x)
{
    case 5:
        // Do something
    case 1:
        // Do something else
}
```

* Conditional expression replace if-else
  * `int x = (true) ? 5 : 6`
  * if `true`, then x = 5, else x = 6

* `while`: repeat unknown number of times
* `do-while`: repeat unknown number of times, at least once
* `for` : repeat known number of times

```C
// While loop

while()
{

}

// Do-while loop

do
{

}while()

// For loop

for (int i = 0; i < 10; i++)
{
    // Do something a set number of times
}
```

Arrays

* Block of contiguous space in memory, with each element of same datatype
* Zero-indexed
* Declaration
  * `type name[size]`
  * Size can be left as blank for memory to be allocated by the compiler
* Assigning elements
  * Has to be done elementwise, not the whole array at once (a=b doesn't work)
* Passed by reference, not value

Variable Scope

* Local variables: confined to the function it is in
* Global variables: declared outside of all functions, can be changed by any function
* Most variables are passed by value in C, copied

## Command Line Arguments

* Command is argv[0], first argument is argv[1] and so on...

```C
int main(int argc, char *argv[])
{
    // argc is the Argument Count, in int
    // argv is the Argument Vector, in char array
}
```

## Functions

* Basic structure:
  * `return-type name(argument-list)`
  * return-type can be any datatype
  * argument-list requiress the datatype and argument name
* Function definition/prototype is just the first line declaring the function at the top of the file, to allow it to be called even if the full declaration is at the bottom

## Algorithms

* Linear search (unsorted list)
  * Search from beginning to end
* Binary search (sorted list)
  * Start in the middle, then search in the middle of the left or right depending on whether its larger or smaller than the middle value
