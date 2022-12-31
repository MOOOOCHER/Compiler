# Compiler
This project is a Just-In-Time Compiler for the educational programming language PL/O with some simplifications.  
This is a C++ library where the user can pass his source code by using the interface which the library exposes. 
The source code passed will then be compiled and a function handle, which can be copied to multiple threads, will be returned. 
Calling the function handle with parameter, which are specified in the passed source code, will return a the value, that is computed by the source code.

Because the handle can be copied to multiple threads before the function was compiled, the implementation also makes sure that it handles multiple threads calling 
and compiling the function correctly.

## The Programming Language
A program consists of the following components.
1. Parameter declarations (optional): PARAM P0, …, PN;
2. Variable declarations (optional): VAR V0, …, VN;
3. Constant declarations (optional): CONST C0 = c0, …, CN = cN;
4. Body begin: BEGIN
5. Program body: statement0; … ; statementN
6. Body end: END
7. Program terminator: .

A statement can either be an assignment statement or a return statement.
• Assignment statement: variable := arithmetic-expression
• Return statement: RETURN arithmetic-expression

Finally, arithmetic expressions can contain the usual unary operators (+ and -), binary operators
(+, -, *, and /), as well as parentheses. They can refer to parameters, variables, constants and
integer literals.

##Example
For example, the following program computes the weight of a block of concrete in kilograms
given its width, height, and depth in meters.

PARAM width, height, depth;
VAR volume;
CONST density = 2400;

BEGIN
  volume := width * height * depth;
  RETURN density * volume
END.
