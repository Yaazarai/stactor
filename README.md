# stactor
Stack memory-implemented vector. The purpose of this is to simplify vector handling for small data-sets by stack allocating memory saving on dynamic allocation performance and cleanup. The C programming language and most compilers (gcc, msvc, etc.) all support type-aliasing, which this library uses heavily. You typedef your own struct that matches the layout of `struct stactor` with your own array size and type and pass a pointer to the library of all of your operations on that struct. The new aliased struct MUST match the exact layout of `stactor` with the array at the end--but the array can have any type/size.

Base stactor implementation definition includes the `size` (struct size), `type` (array type size), `length` (array length in bytes), `iterator` (vector read/write position) and finally `stack` (array).
```C
// size (sizeof struct), type (sizeof stack type), length (length of stack), iterator (position of stack).
typedef struct stactor {
  size_t size, type, length, iterator;
  int08_t stack[256];
} stactor;
```
You can then typedef your own struct and fill/init the struct using the stactor interface:
```C
typedef struct string_stactor {
  size_t size, type, length, iterator;
  int08_t stack[16][256];
} string_stactor;

int32_t main() {
  string_stactor mystrings;
  stactor_fill(&mystrings, sizeof(string_stactor), sizeof(mystrings.stack[0]));
}
```
The example specifically creates a stactor that is an array of 256-byte strings, so we can write/read those out. The replace function will replace N bytes, but a string with N bytes includes the null-terminating character, so we avoid copying the null character by using N-1 bytes to replace part of the original string. Then prtinf the final 3 strings stored in the stactor:
```C
...
  
int32_t main() {
  string_stactor stactor;
  stactor_fill(&stactor, sizeof(string_stactor), sizeof(stactor.stack[0]));
  const char_t string1[256] = "Hello World...";
  const char_t string2[256] = "Goodbye World...";
  const char_t string3[256] = "Freedom is death.";
  const char_t string4[] = "Whoop";
  
  stactor_pushext(&stactor, string1, sizeof(string1));
  stactor_pushext(&stactor, string2, sizeof(string2));
  stactor_insertext(&stactor, 1, string3, sizeof(string3));
  stactor_replaceext(&stactor, 0, string4, sizeof(string4)-1);
  
  const char_t* mystr1 = stactor_find(&stactor, 0);
  const char_t* mystr2 = stactor_find(&stactor, 2);
  const char_t* mystr3 = stactor_find(&stactor, 1);
  printf("%s\n", mystr1);
  printf("%s\n", mystr2);
  printf("%s\n", mystr3);
}z
```
