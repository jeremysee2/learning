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

Statically initialising arrays:

```C
int array[6] = {1, 2, 3, 4, 5, 6};
```

Variable Scope

* Local variables: confined to the function it is in
* Global variables: declared outside of all functions, can be changed by any function
* Most variables are passed by value in C, copied

## Strings / Array of Characters

Compare the string pointed to by their respective pointers. Returns 0 if successful, 1 if unsuccessful.

```C
#include <string.h>

// Initialise strings
char c[] = "abcd"; // You can't do this normally, due to uncertainity with malloc()
char c[10] = "abcd";
char c[] = {'a', 'b', 'c', 'd'};

// Compare two strings
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t n)

// Copy a string into another
void *memcpy(void *dest, const void *src, size_t n); // Copy part of memory to another
void *memmove(void *dest, const void *src, size_t n); // same as memcpy
void *memset(void *str, int c, size_t n); // Set first n characters as c

char *strncpy(char *dest, const char *src, size_t n); // Copy part of string
char *strcpy(char *dest, const char *src); // Copy entire string

// Searching functions
void *memchr(const void *str, int c, size_t n); // Search for first c in first n bytes of string
char *strchr(const char *str, int c); // same as memchr
char *strstr(const char *haystack, const char *needle); // First occurence of the entire string needle in the string haystack
char *strpbrk(const char *str1, const char *str2); // First character in str1 that matches any character in str2

// Other functions
char *strcat(char *dest, const char *src); // Append src to the end of the string
char *strncat(char *dest, const char *src, size_t n); // same as strcat, n times

size_t strcspn(const char *str1, const char *str2); // Length of initial segment of str1 which consists entirely of characters not in str2
size_t strspn(const char *str1, const char *str2); // Opposite of strcspn

size_t strlen(const char *str); // Length of string, not including NULL character

char *strtok(char *str, const char *delim); // Breaks str into a series of tokens separated by delim

size_t strxfm(char *dest, const char *src, size_t n); // Transforms the first n characters of src into current locale, and places them in dest

```

## Typedef

Creating a struct (person) with fields (name,number).

```C
typedef struct {
  char name[10];
  char number[10];
} person;

person people[4];
strcpy(people[0].name,"EMMA");
```

## Command Line Arguments

* Command is argv[0], first argument is argv[1] and so on...

```C
int main(int argc, char *argv[])
{
    // argc is the Argument Count, in int
    // argv is the Argument Vector, in char array
}
```

## Inputs

`scanf()` reads the sequence of characters until it encounters whitespace (space, newline, tab)

```C
scanf("%s",name);
```

## Functions

* Basic structure:
  * `return-type name(argument-list)`
  * return-type can be any datatype
  * argument-list requiress the datatype and argument name
* Function definition/prototype is just the first line declaring the function at the top of the file, to allow it to be called even if the full declaration is at the bottom

## Algorithms

Big O notation is for worst case complexity, Omega is for best case complexity.

* Linear search (unsorted list)
  * Search from beginning to end
  * O(n), omega(1)
* Binary search (sorted list)
  * Start in the middle, then search in the middle of the left or right depending on whether its larger or smaller than the middle value
  * O(log(n)), omega(1)

Sorting algorithms

* Bubble sort
  * Compare adjacent values and sort 2 at a time. Repeat for each pair until all sorted.
  * O(n^2), omega(n)
* Selection sort
  * Iterate through the list and store the smallest value, then put it at the front once you go through the whole list. Repeat for each element.
  * O(n^2), omega(n^2)
* Merge sort
  * If one item, just return. Sort left half, sort right half, merge sorted halves.
  * 

Recursion

```C
void draw(int h)
{
  if (h == 0)
  {
    return;
  }
  else
  {
    draw(h-1);
    for (int i = 0; i < h; i++) 
    {
      printf("#");
    }
    printf("\n");
  }
}
```
