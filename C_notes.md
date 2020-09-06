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

Using strings with pointers, the pointer only points to the first character.

```C
// Defining a string, contains the address of a character
char *s = "EMMA";
printf("%s\n",s);

// Print address of the first character
printf("%p\n",s);
printf("%p\n",s[0]);

// Print first character
printf("%c\n",*s);

// Print second character
printf("%c\n",*(s+1));
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
  * O(n) worst, omega(1) best
* Binary search (sorted list)
  * Start in the middle, then search in the middle of the left or right depending on whether its larger or smaller than the middle value
  * If element not in array, terminates when start point is greater tha end point, value not in array
  * O(log(n)) worst, omega(1) best

Sorting algorithms

* Bubble sort
  * Compare adjacent values and sort 2 at a time, by swapping. Repeat for each pair until a pass where no swaps are made -> all sorted.
  * O(n^2), omega(n)
* Selection sort
  * Iterate through the list and store the smallest value, then put it at the front once you go through the whole list. Repeat for each element.
  * O(n^2), omega(n^2)
* Insertion sort
  * Build sorted array in place, shifting elements out of the way. Call the first element in array "sorted". Insert elements into "sorted" portion by shifting others up/down.
  * O(n^2), omega(n)
* Merge sort
  * If one item, just return. Sort left half, sort right half, merge sorted halves. Done recursively, base case of one element array (sorted). Sorting process is done during merging.
  * O(nlog(n)), because run-time is n wide and log(n) deep, and omega(nlog(n))

Recursion

Function that calls itself, with a base case. Recursive case is where recursion occurs. Possible to have multiple base cases if you depend on the previous two results. Multiple recursive cases: e.g. Collatz conjecture, differs base on even or odd.

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

## Pointers

Pointers are just the address of a variable, usually to the first byte.

& is the addressOf/referencing operator

\* is the goToAddress/dereferencing operator, gets a value there

\* and & cancel each other out

If `arr` is an array of doubles, then `&arr[i]` is a pointer-to-double whose value is the address of the i-th element of `arr`. An array `arr` is the pointer to its first element.

NULL pointer is a pointer that points to nothing, good to initialise something you know will be updated. If you dereference a NULL pointer, get a Segmentation fault.

```C
int n = 50;

// Describe a pointer, p is a pointer, *p is the value at the pointer
int *p = &n;

// Print value or pointer address
printf("%i\n", *p);
printf("$p\n", &n);

// Initialise multiple pointers at once
int* pa, *pb, *pc;
```

## Memory Allocation

Dynamically allocated memoery comes from the heap. Environment variables / functions use the stack.

`malloc()` allocates memory `free()` frees up memory. `malloc()` returns the pointer to the first byte of the allocated memory. If the function fails, returns `NULL`.

To actually change the value of a variable, you need to change the value using the pointer. C normally passes by value, copies.

```C
void swap(int *a, int *b)
{
  // tmp is value, a is pointer
  int tmp = *a;
  *a = *b;
  *b = tmp;
}
```

## Input and Files

Input can be done using `scanf()`.

```C
// Input an int
int x;
prinf("x: ");
scanf("%i",&x);
printf("x is %i\n",x);

// Input a string
char s[10];
prinf("s: ");
scanf("%s",s);
printf("s is %s\n",s);

```

Files can be used to store persistent data, usually handled by pointers. Common functions are `fopen(), fclose(), fgetc(), fputc(), fread(), fwrite()`.

```C
// Open a file, 'a' 'r' 'w'
FILE *file = fopen("phonebook.csv", "a");
fprintf(file, "%s,%s\n",name,number);

// Read the file, one character at a time
char ch;
while((ch = fgetc(file)) != EOF)
  printf("%c",ch);

// Write one character
fputc(ch, file);

// Read the file
int arr[10]; // Buffer address
fread(arr, sizeof(int), 10, file);

// Close the file
fclose(file);

// Other file types
int main(int argc, char *argv[])
{
  // Check for inputs
  if (argc != 2)
  {
    return 1;
  }

  // Open file
  FILE *file = file(argv[1], "r");
  if (file == NULL)
  {
    return 1;
  }

  // Read 3 bytes
  unsigned char bytes[3];
  fread(bytes, 3, 1, file);

  // Check if bytes are 0xff 0xd8 0xff
  if (bytes[0] == 0xff && bytes[1] == 0xd8 && bytes[2] == 0xff)
  {
    printf("Maybe JPEG\n").
  }
  else
  {
    printf("Not JPEG");
  }
}

```

## Call Stack

When calling a function, system sets aside space in memoery for that function, called a stack frame. These are arranged in the **Stack**.

Once a new function is called, a new frame is **pushed** onto the top of the stack and becomes the active frame. When the function finishes, the frame is **popped** off the stack and the new frame immediately below it becomes the new active function, continues immediately.

## Data Structures

## Array / List

Insertion has a complexity of O(n), to copy the entire array to a new area of memory, and add in the new element.

Search has a complexity of O(log(n)) for a sorted list, O(n) for unsorted list.

You can use `realloc()` instead of `malloc()` to extend a list. Handles copying of data from old addresses to new.

* Insertion and deletion is O(n)
* Lookup using random access is O(1)
* Easy to sort, low overhead, fixed size

## Linked List

A list that does not use a contiguous portion of memory, each element contains a pointer that points to the next element.

* Insertion and deletion is O(1)
* Lookup is bad, linear search
* Difficult to sort, unless you sort as you construct
* Relatively small

Operations

* Create a linked list
  * Dynamically allocate space for a new node
  * Check enough memory
  * Initialise val and next field
  * Return a pointer to the new node
* Search through a linked list to find an element
  * Call the first pointer to the head of the list
  * Check value, if not then use that pointer to point to the next node and repeat
  * End at NULL pointer, end of list
* Insert a new node
  * Dynamically allocate space for a new node
  * Populate and insert node at beginning of linked list
  * Return a pointer to the new head of the linked list
* Delete a single element
* Deleted an entire linked list
  * Point to next, free previous node, then repeat until reached NULL

```C
typdef struct node
{
  int number;
  struct node *next;
} node;

int main(void)
{
  // List of size 0
  node *list = NULL;

  // Add number to list, 1
  node *n = malloc(sizeof(node)); // temp
  if (n == NULL)
  {
    return 1;
  }
  n->number = 1;
  n->next = NULL;
  list = n; // list points to start of Linked List

  // Add number to list, 2
  n = malloc(sizeof(node));
  if (n == NULL)
  {
    return 1;
  }
  n->number = 2;
  n->next = NULL;
  list->next = NULL;

  // Add number to list, 3
  n = malloc(sizeof(node));
  if (n == NULL)
  {
    return 1;
  }
  n->number = 3;
  n->next = NULL;
  list->next->next = NULL; // usually done in a loop

  // Print list, until last element where pointer is NULL
  for (node *tmp = list; tmp != NULL, tmp = tmp->next)
  {
    printf("%i\n", tmp->number);
  }

  // Free list
  while (list != NULL)
  {
    node *tmp = list->next; // after free(), don't call the pointer anymore
    free(list);
    list = tmp;
  }
}
```

## Binary Search Trees

Two pointers per data, left is smaller, right is larger, for all child elements. Implemented with recursion. Search is O(log(n)).

```C
typdef struct node
{
  int number;
  struct node *left;
  struct node *right;
} node;

bool search(node *tree)
{
  if (tree == NULL)
  {
    return false;
  }
  else if (50 < tree->number)
  {
    return search(tree->left);
  }
  else if (50 > tree->number)
  {
    return search(tree->right);
  }
  else
  {
    return true;
  }
}
```

## Hash Tables

Gives O(n) for search. An array of linked lists. Index into the array using a hash function.

Ideal hash function ensures uniqueness of data in the array, gives O(1).

* Inertion is two-step, hash then add
* Deletion is same time as lookup
* Lookup is on average better than linked lists, because of real-world distributions, unlikely to have all elements under the same hash
* Not ideal for ordering or sorting
* Large range of sizes

Functionality

* Hash function: returns an integer value *hash code*
  * Use only data being hashed
  * Use all of the data being hashed
  * Deterministic
  * Uniformly distribute data
  * Generate very different hash codes
* Array: stores data to place into a data structure
* Run data through the hash function, then store the data in the element of the array represented by the hash code
* In the event of same hash code (collision), place data in the next consecutive element in the array, until we find a vacancy. When searching, if don't find it at that location, it'll be nearby. This creates a problem called clustering, when multiple data misses the hash code due to it being occupied.
* Chaining: solves clustering by putting a linked list at a hash code location

## Trie (Retrieval)

Uses a lot more memory, gives O(1) for elements inserted into the structure. Tree whose nodes are arrays.

Example: storing names. Stores one array for every letter in a name. Each layer is the n-th letter in the name. Search and insert is equal to the length of the name, aka constant time O(1).

* Insertion is complex, needs a lot of dynamic memory allocation
* Deletion is easy, free a node
* Lookup is fast
* Already sorted
* Rapidly takes up a lot of memory with deeper tries (e.g. using 10 digits, each layer grows by x10 in size, each node will have 10 pointers)

Functionality

* Uses the data as a roadmap to an end
* Unlike hash table, no collisions, no data have the same path
* Start from a central root node to a leaf node

## Queue

FIFO data structure. Enqueue/push means get in line, dequeue/pop means get out of line.

## Stacks

LIFO data structure.

## Dictionary

Keys and values. Can be implemented with a hash table to increase search speed.
