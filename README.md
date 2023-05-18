# OneChar
is a simple stack-based programming language. 

Every operation takes exactly one printable ASCII-character

## Example Programs

Simple Arithmetic:
OneChar can evaluate simple arithmetic expressions consisting of numbers, brackets and the operators `+` `-` `*` `/` `%` `^`
```
1+1; \ prints 2
```

```
3^4*5%6; \ computes ((3^4)*5)%6 prints 3
```

Hello World:

```
{::[(:+2)#,-1:].:[-1 (0-1)#].}$4
"Hello World!\n" 4@?
```

Fibonacci-Numbers:

```
1$0 1$1 1[1@(0@+1@)$1$0 1@:;<1000]
```

more examples can be found in the examples folder
## Syntax
Each operation is exactly one printable ASCII-character,
all characters are executed from left to right.

### Integers
- The first digit in and integer-literal pushes that digit onto the value-stack.
- Each further digit `k` takes the top value `N` from the stack and pushes `10*N+k` onto the stack.
- A non-digit character terminates and integer-literal.

This rule-set allows writing integer-literals directly into the code.

Example:

```
1
1024
123456789
```
results in the 3 numbers `123456789` `1024` and `1` being on the stack.

### Strings
String literals start with a `"` and end with the next non-escaped `"`
Within a string literal, every character (excluding `\`) directly pushes its corresponding char-code onto the stack.
Within strings `\` can be used to escape `"` and `\` as well as insert the space-characters `\n` `\t` and `\r`.

After the end of the string literal the total number of characters is pushed on the stack.

Examples:

```
"Hello World\n"
```
pushes the char-codes 
`72` `101` `108` `108` `111` `32` `87` `111` `114` `108` `100` `10`
with the length `12` being on top of the stack

```
"\""
```
pushes the char-code of `"` (`34`) and the length (`1`).

### Comments

`\\` comments out all code until the end of the current line


### Operations
Unary operators are evaluated in postfix-notation, 
unlike in most stack-based languages binary operators 
are evaluated in infix notation.
#### Unary operators
- `!` takes the top element on the stack and pushes `1` if it is `0`
 and `0` otherwise
- `~` flips all bits in the integer on top of the stack.
- `@` loads the value at the memory address given by the value on top of the stack.

Unary operations have precedence over binary operations 

#### Binary operators
All binary operations take two values from the top of the stack and push 
the result of the given operation.
Binary operations are stored on the operator-stack until a 2nd operand and 
an operator with lower or equal precedence (or a white-space) is pushed.

The supported binary operations in order of increasing precedence are:

- `&` logical and
- `|` logical or
&nbsp;
- `>` pushes `1` if the left value is greater that the right value, `0`otherwise
- `<` pushes `1` if the left value is less that the right value, `0`otherwise
- `=` pushes `1` if the left value is equal to the right value, `0`otherwise	
&nbsp;
- `+` adds the top two values on the stack 
- `-` subtracts the top value on the stack from the value below it
&nbsp;
- `*` multiplies the top two values on the stack
- `/` divides the 2nd value on the stack be the top value on the stack.
- `%` remainder of the division of the 2nd-value by the top-value 
&nbsp;
- `^` pushes the 2nd value on the stack to the power of the top value on the stack.
 unlike the other operators this operation is weaker than itself,
  and therefor is evaluated right to left.
&nbsp;
- `$` saves the 2nd value on the stack, 
at the address given be the top value on the stack. 
This operation does not push a value on the stack.

#### Brackets
The brackets `(` and `)` an be used to group operations,
all operations between `(` and `)` will be evaluated before operations left of `(`
are evaluated.

#### Examples

```
1+2*3^4
```
evaluates to `163` (which is `1+(2*(3⁴))`)

```
1*2+3
1+2*3
(1+2)*3
1*(2+3)
```
evaluate to `5` `7` `9` and `5` respectively.

```
42$0
16$1
0@/1@ 
0@%1@ 
```
results in `2` and `10` being on top of the stack, 
as well as `42` being memory cell `0` and `16` in memory cell `1`

```
1<0
1=0
1>0
```
results in the values `1` `0` `0` being on the stack.

### Stack manipulation 
- `:` duplicates the top element on the stack
- `.` discards the top element on the stack
- `#` takes the top element on the stack.
     - if it is positive the element at that index in the stack, 
     (counting up from the top element which is labeled with 1)
     will be pushed on top of the stack.
     - if it is zero or negative the element at that index in the stack
     (counting down from the top element which is labeled with 0),
     will be set to the value of the top element in the stack.
     
Example:

```
1 2 : 3 4 5 .
2# 0~#
```
the first line results in `4` `3` `2` `2` `1` being on the stack.
`2#` copies the 2nd element to the top giving `3` `4` `3` `2` `2` `1`.
`0~#` (`0~` is -1) replaces element 1 counting from 0 which is `4` resulting in `3` `3` `3` `2` `2` `1`.

### IO

- `;` prints the top value on the stack as (signed base 10) integer,
 and starts a new line.
- `,` prints the lowest byte of top-value on the stack as a character.
- `'` reads one character from standard input.

Examples:

```
1+1;
```
prints 2

```
"olleH".,,,,,
```
prints `Hello`

```
"💻".4#,3#,2#,1#,
```
prints the 4-byte UTF8 character `💻`. 
Due to the stack-oriented way of saving strings,
the bytes have to be printed in reverse order.


```
',
```
reads one character from the standard input, and prints it to the console

### Loops
`[` and `]` can be used to define loops and conditions.
`[` will jump to the matching `]` if the value on top of the stack is `0`,
`]` will jump to the matching `[` if the value on top of the stack is not `0`.

Examples:

```
10:[:;-1:];
```
prints the numbers from `10` to `0`


``` 
1$0 1$1 1[1@(0@+1@)$1$0 1@:;<1000]
```
prints all Fibonacci-numbers up to `1000` 

### Subroutines
`{` and `}` can be used to define subroutines,
when the execution hits a `{` it pushes the current instruction pointer 
on the value-stack and jumps to the matching `}`.
When the execution hits a `?` it jumps to the address given by 
the value on top of the stack, and executes the code until it reaches a `}`,
then it jumps back to the address after the `?`.

Examples:

```
{(2#|2#&(3#&3#)~) (0-2)#..}$3
```
defines a routine for calculating the bit-wise exclusive or of two values, 
and saves it to memory address `3`.
```
42 37 3@?
```
will then calculate the bit-wise exclusive or of `42` and `37` which evaluates 
to `15`.

The Hello World example defines the routine 

```
{::[(:+2)#,-1:].:[-1 0~#].}$4
```
which prints all characters of a string in reversed order,
 and the removes them from the stack.

Using that procedure the Hello World program is

```
"Hello World!\n" 4@?
```
for an arbitrary string
