# OneChar
is a simple stack-based programming language. 

Every operation is exactly one printable ascii-character

## Examples

Hello World:

```
{::[(:+2)#,-1:].:[-1 (0-1)#].}$4
"Hello World!\n" 4@?
```

Fibonacci-Numbers:

```
1$0 1$1 1[1@(0@+1@)$1$0 1@:;<1000]
```

Cat Program:

```
':("\n".=)![,':("\n".=)!].
```

## Syntax
Each operation is exactly one printable ascii-character,
all characters are executed from left to right.
The program state consist of 3-stacks (value,operator and ip) 
an array for random-access memory two counters and 4 boolean flags.
### Integers
- The first digit in and integer-literal pushes that digit onto the value-stack.
- Each further digit takes the top values on the stack,
 multiplies it by 10 and pushes it bach onto the stack.
- A non-digit character terminates and integer-literal.

This ruleset allows writing integer-litarals directly into the code.

Example:

```
1
1024
123456789
```
results in the 3 numbers `123456789` `1024` and `1` beeing on the stack.

### Strings
String literals start with `"` within a string literal, every character 
(excluding `"` and `\` directly pushes its corresponding char-code onto the stack.
String literals are termminated with `"`,
 which pushes the total length of the string literal.
 
Examples:

```
"Hello World\n"
```
pushes the charcodes 
`72` `101` `108` `108` `111` `32` `87` `111` `114` `108` `100` `10`
with the length `12` beeing on top of the stack

```
"\""
```
pushes the charcode of `"` (`34`) and the length (`1`).

### Operations
Unary operators are evaluated in postfix-notation, 
unlike in most stack-based languages binary operators 
are evaluated in infix notation.
#### Unary operators
- `!` takes the top element on the stack and pushes `1` if it is `0`
 and `0` otherwise
- `~` flips all bits in the integer on top of the stack.
- `@` loads the value at the memory address given by the value on top of the stack.

!!! Unary operations allways have precedenc over binary operations 

results in the values `0` `-43` 
as well as whatever was stored at address `0`beeing on the stack.

#### Binary operators
All binary operations take two values from the top of the stack and push 
the result of the given operation.
grouped by thier precedence, with the lowest precednce at the top.
Binary operations are stored on the operator-stack until a 2nd operand and 
an operator with lower or equal precedence (or a whitespace) is pushed.

- `&` logical and
- `|` logical or


- `>` pushes `1` if the left value is greather that the right value, `0`otherwise
- `<` pushes `1` if the left value is less that the right value, `0`otherwise
- `=` pushes `1` if the left value is equal to the right value, `0`otherwise	
	
		
- `+` adds the top two values on the stack 
- `-` subtracts the top value on the stack from the value below it


- `*` multiplies the top two values on the stack
- `/` divides the 2nd value on the stack be the top value on the stack.
- `%` remainder of the division of the 2nd-value by the top-value 


- `^` pushes the 2nd value on the stack to the power of the top value on the stack.
 unlike the other operators this operation is wweaker than itself,
  and therefor is evaluated right to left.
  

- `$` saves the 2nd value on the stack, 
at the address given be the top value on the stack. 
This operation does not push a value on the stack.

#### Brackets
The brackets `(` and `)` an be used to group operations,
all operatios between `(` and `)` will be evaluated before operations left of `(`
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
results in `2` and `10` beeing on top of the stack, 
as well as `42` being memory cell `0` and `16` in memory cell `1`

```
1<0
1=0
1>0
```
resultes in the values `1` `0` `0` beeing on the stack.

### Stack manipulation 
- `:` duplicates the top element on the stack
- `.` discards the top element on the stack
- `#` takes the top element on the stack.
     - if it is positive the element at that index in the stack, 
     (counting up from the top elemennt which is labled with 1)
     will be pushed un top of the stack.
     - if it is negative the element at that index in the stack
     (counting down from the top element which is labled with 0),
     will be set to the value of the top element in the stack.
     
Example:

```
1 2 : 3 4 5 .
2# (0-1)#
```
the first line results in `4` `3` `2` `2` `1` being on the stack.
After `2#` there is `3` `4` `3` `2` `2` `1` on the stack
and `(0-1)#` replaces the `4` resulting in `3` `3` `3` `2` `2` `1`.

### IO

- `;` prints the top value on the stack as (signed base 10) integer,
 and starts a new line.
- `,` prints the lowerest byte of top-value on the stack as a character.
- `'` reads one character from standard input.

Examples:

```
1+1;
```
prints 2

```
"Hello".,,,,,
```
prints `Hello`

```
"💻".4#,3#,2#,1#,
```
prints the 4-byte UTF8-character `💻`. 
Due to the stack-oriented way of saving strings,
the bytes have to be printed in reverse order.

```
',
```
reads one charater from the standrd input, and prints it to the console

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
prints all fibonacci-numbers up to `1000` 

### Subroutines
`{` and `}` can be used to define subroutines,
when the execution hits a `{` it pushes the current instruction pointer 
on the value-stack and jumps to the matching `}`.
When the execution hits a `?` it jumps to the address given by 
the value on top of the stack, and executes the code until it reaches a `}`,
the it jumps back to the address after the `?`.

Examples:

```
{(2#|2#&(3#&3#)~) (0-2)#..}$3
```
defines a routine for calculating the bitwise exclusive or of two values, 
and saves it to memory address `3`.
```
42 37 3@?
```
will then calulate the  bitwise exclusive or of `42` and `37` which evaluates 
to `15`.

The Hello World example defines the routine 

```
{::[(:+2)#,-1:].:[-1 (0-1)#].}$4
```
which prints all characters of a string in reversed order,
 and the removes them from the stack.

Using that procedure the Hello World program is

```
"Hello World!\n" 4@?
```
for an arbitrary string
