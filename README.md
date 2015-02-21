#Spow
It's a programming language made by people like you. Spow is an experimental mini-language based on the Lisp family of programming languages.

## About Spow
Spow starts as a try to create and understand how programming languages are built and how someone could create (or try to create) one. This project has not the vision of be the next big programming languaje in the world and it will not have that goal in the future, but it's a good start to learn about programming languages, interpreters and compilers. The unique profit in this project is more about interpreter design and programming in C.

This is project is intended to be a platform for everybody who wants to try The language programming challenge. Everything borns in base of [Build your own Lisp book]() and the fantastic [mpc library](), it provides a good start but is not a complete guide, this project will try to expand the limits of the book and let people like you and me start a programming language with all the rules and great things of a professional language.

### mpc library (Micro Parser Combinators)
_mpc_ is a lightweight and powerful Parser Combinator library for C.

Using _mpc_ might be of interest to you if you are...

* Building a new programming language
* Building a new data format
* Parsing an existing programming language
* Parsing an existing data format
* Embedding a Domain Specific Language
* Implementing [Greenspun's Tenth Rule](http://en.wikipedia.org/wiki/Greenspun%27s_tenth_rule)

## Features
Spow is a mini-language that is inspired by the Lisp family of languages. Thus, it shares most of its features with Lisp and Scheme. These include:

- Dynamic typing
- First-class functions, including [anonymous (lambda) functions](http://en.wikipedia.org/wiki/Anonymous_function)
- Function  [closures](http://en.wikipedia.org/wiki/Closure_(computer_programming))
- [Partial application](http://en.wikipedia.org/wiki/Partial_application)
- [Tail-call optimization](http://en.wikipedia.org/wiki/Tail_recursion)
- Data immutability
- Lists as the primary data structure
- [Homoiconicity](http://en.wikipedia.org/wiki/Homoiconicity) - that is, similar representations of code and data
- Metaprogramming in the form of simple macros

Currently, Spow data definition and manipulation capabilities are lacking, but this will hopefully be changed in the future.

### Basic features in action
Spow supports inline comments using hash (`#`):

    # Comment here
    (print 'hello') # More comments here

Printing to standard output can be done using `print` and `println`:

    spow> (println 'Hello world')
    Hello world!

Variables are created with `define` (which affects the local environment) and `global` (which, as the name suggests, affects the global environment):

    spow> (global foo 'bar')
    spow> (println foo)
    bar

## Requeriments:
- `gcc` or `clang`
- You'll need `make`
- `git` to clone the source

## Build and run
Most of Spow dependencies are included in the repository, so you shouldn't need to install anything other than the build tools, so you will need a fairly recent C compiler. First, clone the repository, and then compile using `make`:

    $ git clone https://github.com/Gioyik/spow
    $ cd spow
    $ make

This will create an `spow` binary under `out/bin/` directory.

Clean up if you want to start over:

    $ make clean

## Usage
The `spow` binary can take a single argument - a path to a file to execute.

    $ ./out/bin/spow [file].zl

If no argument is given, then it will drop into the REPL (interpreter):

    $ ./out/bin/spow
	 ______                         
	 |__  / | __ _ _ __   __ _  ___  
	   / /| |/ _` | '_ \ / _` |/ _ \ 
	  / /_| | (_| | | | | (_| | (_) |
	 /____|_|\__,_|_| |_|\__, |\___/ 
	                     |___/       

	Spow repl - vX.X.X (Press Ctrl+c or type exit to finish)
	spow> 

## Docs
TBD

### Primitive Data Types

<table>

<colgroup>
<col style="width: 15%;">
<col style="width: 35%;">
<col style="width: 50%;">
</colgroup>

<thead>
<th>Type</th>
<th>Example</th>
<th>Description</th>
</thead>

<tbody>

<tr>
<td>Integer</td>
<td><code>5</code>, <code>-9</code></td>
<td>A standard integer (<code>long</code>)</td>
</tr>

<tr>
<td>Floating point</td>
<td><code>-5.</code>, <code>3.14</code></td>
<td>A standard floating point (<code>double</code>)</td>
</tr>

<tr>
<td>Boolean</td>
<td><code>true</code>, <code>false</code></td>
<td>A standard... boolean</td>
</tr>

<tr>
<td>String</td>
<td><code>"\"escapes\" OK"</code>, <code>'foobar'</code></td>
<td>A string type - either single or double quotes</td>
</tr>

<tr>
<td>Q-Symbol</td>
<td><code>:like-in-ruby</code>, <code>:'foo'</code></td>
<td>A quoted symbol (identifier), can also be written similar to strings</td>
</tr>

<tr>
<td>Q-Expr</td>
<td><code>{1 'b' (+ 1 2) x y}</code></td>
<td>A quoted expression. The basic data structure - acts like a list</td>
</tr>

<tr>
<td>Dictionary</td>
<td><code>[:x 23 :y 'hello' :z {a b c}]</code></td>
<td>A key-value store. Keys are Q-Symbols, values can be anything</td>
</tr>

<tr>
<td>Function</td>
<td><code>(fn (x) (/ 1 x))</code></td>
<td>An anonymous function. The basic mechanism of function definition</td>
</tr>

<tr>
<td>Error</td>
<td><code>(error 'somebody set up us the bomb')</code></td>
<td>An error. Stops evaluation</td>
</tr>

</tbody>

</table>

### Expressions

Function calls in Awl are defined as [S-Expressions](http://en.wikipedia.org/wiki/S_expression) (symbolic expressions). They are syntactically enclosed in parentheses `()`. The first argument of the expression must be a callable, and is evaluated in the current environment with any following arguments as parameters (this is the iconic "[Polish notation](http://en.wikipedia.org/wiki/Polish_notation)" of Lisp).

    awl> (+ 5 6)
    11
    awl> (println 'foo')
    foo

When evaluating user-defined functions, partial application is done automatically for any unfilled arguments (this is currently not done for builtins).  This makes it easy to use higher-order functions quickly:

    awl> (define xs {1 2 3 4})
    awl> (define square (map (fn (x) (* x x))))
    awl> (square xs)
    {1 4 9 16}

Variable and function identifiers, called "symbols," are evaluated to the values that they map to, except in certain special forms (e.g. when they are being defined):

    awl> (define x 5)
    awl> (+ x 6)
    11

The primitive types evaluate to themselves.

Q-Expressions (quoted expressions, often referred to simply as 'lists') are particularly important. They are enclosed inside curly braces `{}`. They are a collection type and behave similar to lists in other languages. They can store any number and mixture of primitive types. And they have one more important ability: expressions that they contain which would normally be evaluated, such as symbols and S-Expressions, are left unevaluated (i.e. they are "quoted"). This allows them to contain arbitrary code, and then be converted and evaluated as S-Expressions:

    awl> (head {1 2 3})
    1
    awl> (tail {1 2 3})
    {2 3}
    awl> (define x {* 3 (+ 2 2)})
    awl> x
    {* 3 (+ 2 2)}
    awl> (eval x)
    12

There are a few more expression types that are useful in special cases.

E-Expressions (escaped expressions) are denoted with a preceding backslash `\`, and can be used to specifically evaluate a section within a Q-Expression literal:

    awl> {1 2 (+ 2 1)}
    {1 2 (+ 2 1)}
    awl> {1 2 \(+ 2 1)}
    {1 2 3}

C-Expressions (concatenating expressions) are denoted with a preceding at-sign `@`.  They behave similarly to E-Expressions, with the exception that, when given a list (Q-Expression), they "extract" the contents and include it directly in the outer list:

    awl> {1 2 \{3 4}}
    {1 2 {3 4}}
    awl> {1 2 @{3 4}}
    {1 2 3 4}

Finally, there is another collection type that is slightly more mundane than Q-Expressions and their ilk: Dictionaries. Dictionaries act as simple key-value stores, and are similar to the dictionaries in other languages. They are delimited with square brackets `[]`, use Q-Symbols as their keys, and can store any normal value:

    awl> (dict-get [:foo 12 :bar 43] :foo)
    12
    awl> (dict-set [:x 1 :y 2] :z 3)
    [:'x' 1 :'y' 2 :'z' 3]

### Builtins

Builtins usually behave like normal functions, but they also have the special role of enabling some of Awl's basic features, since they are written in C (for example, the `fn` builtin creates a new anonymous function).

Awl makes no distinction between "operators" (`+`, `-`, `*`) and other kinds of builtins - they are simply named differently.

<table>

<colgroup>
<col style="width: 15%;">
<col style="width: 35%;">
<col style="width: 50%;">
</colgroup>

<thead>
<th>Builtin</th>
<th>Signature</th>
<th>Description</th>
</thead>

<tbody>

<tr>
<td><code>+</code></td>
<td><code>(+ [args...])</code></td>
<td>Addition. Takes 2 or more arguments</td>
</tr>

<tr>
<td><code>-</code></td>
<td><code>(- [args...])</code></td>
<td>Subtraction. Takes 2 or more arguments</td>
</tr>

<tr>
<td><code>*</code></td>
<td><code>(* [args...])</code></td>
<td>Multiplication. Takes 2 or more arguments</td>
</tr>

<tr>
<td><code>/</code></td>
<td><code>(/ [args...])</code></td>
<td>Division. Promotes integers to floats if necessary. Takes 2 or more arguments</td>
</tr>

<tr>
<td><code>//</code></td>
<td><code>(// [args...])</code></td>
<td>Truncating division. Removes decimal remainder. Takes 2 or more arguments</td>
</tr>

<tr>
<td><code>%</code></td>
<td><code>(% [args...])</code></td>
<td>Modulo. Takes 2 or more arguments</td>
</tr>

<tr>
<td><code>^</code></td>
<td><code>(^ [args...])</code></td>
<td>Power operator. Takes 2 or more arguments</td>
</tr>

<tr>
<td><code>&gt;</code></td>
<td><code>(&gt; [arg1] [arg2])</code></td>
<td>Greater than. Takes 2 arguments</td>
</tr>

<tr>
<td><code>&gt;=</code></td>
<td><code>(&gt;= [arg1] [arg2])</code></td>
<td>Greater than or equal to. Takes 2 arguments</td>
</tr>

<tr>
<td><code>&lt;</code></td>
<td><code>(&lt; [arg1] [arg2])</code></td>
<td>Less than. Takes 2 arguments</td>
</tr>

<tr>
<td><code>&lt;=</code></td>
<td><code>(&lt;= [arg1] [arg2])</code></td>
<td>Less than or equal to. Takes 2 arguments</td>
</tr>

<tr>
<td><code>==</code></td>
<td><code>(== [arg1] [arg2])</code></td>
<td>Equal to. Tests deep equality. Takes 2 arguments</td>
</tr>

<tr>
<td><code>!=</code></td>
<td><code>(!= [arg1] [arg2])</code></td>
<td>Unequal to. Tests deep equality. Takes 2 arguments</td>
</tr>

<tr>
<td><code>and</code></td>
<td><code>(and [arg1] [arg2])</code></td>
<td>Logical 'and'. Short circuiting. Takes 2 arguments</td>
</tr>

<tr>
<td><code>or</code></td>
<td><code>(or [arg1] [arg2])</code></td>
<td>Logical 'or'. Short circuiting. Takes 2 arguments</td>
</tr>

<tr>
<td><code>not</code></td>
<td><code>(not [arg1])</code></td>
<td>Logical 'not'. Takes 1 argument</td>
</tr>

<tr>
<td><code>head</code></td>
<td><code>(head [arg1])</code></td>
<td>Returns the extracted first element (head) of a list</td>
</tr>

<tr>
<td><code>qhead</code></td>
<td><code>(qhead [arg1])</code></td>
<td>Like <code>head</code>, except quotes symbols and S-Exprs</td>
</tr>

<tr>
<td><code>tail</code></td>
<td><code>(tail [arg1])</code></td>
<td>Returns the tail of a list, excluding the first element</td>
</tr>

<tr>
<td><code>first</code></td>
<td><code>(first [arg1])</code></td>
<td>Similar to <code>head</code>, but doesn't extract</td>
</tr>

<tr>
<td><code>last</code></td>
<td><code>(last [arg1])</code></td>
<td>Returns the last element of a list, unextracted</td>
</tr>

<tr>
<td><code>except-last</code></td>
<td><code>(except-last [arg1])</code></td>
<td>Returns the first section of a list, excluding the last element</td>
</tr>

<tr>
<td><code>list</code></td>
<td><code>(list [args...])</code></td>
<td>Returns a list containing the evaluated arguments</td>
</tr>

<tr>
<td><code>eval</code></td>
<td><code>(eval [arg1])</code></td>
<td>Evaluates a list as if it were an S-Expression</td>
</tr>

<tr>
<td><code>append</code></td>
<td><code>(append [args...])</code></td>
<td>Concatenates two or more lists</td>
</tr>

<tr>
<td><code>cons</code></td>
<td><code>(cons [arg1] [arg2])</code></td>
<td>Attaches a primitive type to the head of a list</td>
</tr>

<tr>
<td><code>dict-get</code></td>
<td><code>(dict-get [dict] [key])</code></td>
<td>Retrieves a value from a dict using a key</td>
</tr>

<tr>
<td><code>dict-set</code></td>
<td><code>(dict-set [dict] [key] [val])</code></td>
<td>Returns a new dict with a value set</td>
</tr>

<tr>
<td><code>dict-del</code></td>
<td><code>(dict-del [dict] [key])</code></td>
<td>Returns a new dict with a value deleted</td>
</tr>

<tr>
<td><code>dict-haskey?</code></td>
<td><code>(dict-haskey? [dict] [key])</code></td>
<td>Checks if a dict has a key set</td>
</tr>

<tr>
<td><code>dict-keys</code></td>
<td><code>(dict-keys [dict])</code></td>
<td>Returns a list of keys in the dictionary</td>
</tr>

<tr>
<td><code>dict-vals</code></td>
<td><code>(dict-vals [dict])</code></td>
<td>Returns a list of values in the dictionary</td>
</tr>

<tr>
<td><code>len</code></td>
<td><code>(len [arg1])</code></td>
<td>Returns the length of a collection</td>
</tr>

<tr>
<td><code>reverse</code></td>
<td><code>(reverse [arg1])</code></td>
<td>Reverses a collection</td>
</tr>

<tr>
<td><code>slice</code></td>
<td><code>(slice [c] [start] [end] [step])</code></td>
<td>Returns a slice of a collection based on start, stop, and step numbers</td>
</tr>

<tr>
<td><code>if</code></td>
<td><code>(if [pred] [then-branch] [else-branch])</code></td>
<td>If expression. Evaluates a predicate, and one of two branches based on the result</td>
</tr>

<tr>
<td><code>define</code></td>
<td><code>(define [sym] [value])</code></td>
<td>Defines a variable in the local environment</td>
</tr>

<tr>
<td><code>global</code></td>
<td><code>(global [sym] [value])</code></td>
<td>Defines a variable in the global environment</td>
</tr>

<tr>
<td><code>let</code></td>
<td><code>(let (([sym1] [val1])...) [expr])</code></td>
<td>Creates a local environment and defines variables within</td>
</tr>

<tr>
<td><code>fn</code></td>
<td><code>(fn ([args...]) [body])</code></td>
<td>Defines an anonymous function with the specified arguments and body. The
function also retains the current environment as a closure</td>
</tr>

<tr>
<td><code>macro</code></td>
<td><code>(macro [name] ([args...]) [body])</code></td>
<td>Defines a macro that can operate on code before it is evaluated</td>
</tr>

<tr>
<td><code>typeof</code></td>
<td><code>(typeof [arg1])</code></td>
<td>Returns a string representing the type of the argument</td>
</tr>

<tr>
<td><code>convert</code></td>
<td><code>(convert [type] [value])</code></td>
<td>Converts a value to <code>type</code>, which is represented by a qsym,
as returned by <code>typeof</code></td>
</tr>

<tr>
<td><code>import</code></td>
<td><code>(import [path])</code></td>
<td>Attempts to import the <code>awl</code> file at the given path</td>
</tr>

<tr>
<td><code>print</code></td>
<td><code>(print [arg1])</code></td>
<td>Prints to standard output</td>
</tr>

<tr>
<td><code>println</code></td>
<td><code>(println [arg1])</code></td>
<td>Prints to standard output, adding a newline</td>
</tr>

<tr>
<td><code>random</code></td>
<td><code>(random)</code></td>
<td>Returns a floating point random number between 0 and 1</td>
</tr>

<tr>
<td><code>error</code></td>
<td><code>(error [arg1])</code></td>
<td>  </td>
</tr>

<tr>
<td><code>exit</code></td>
<td><code>(exit [arg1])</code></td>
<td>Exits the interactive REPL</td>
</tr>

</tbody>

</table>

### Core Library

In addition to builtins, there exists a core library that Awl imports on startup. Among other things, this library aims to exercise some of Awl's features, as well as provide some basic functional tools.

<table>

<colgroup>
<col style="width: 15%;">
<col style="width: 35%;">
<col style="width: 50%;">
</colgroup>

<thead>
<th>Symbol</th>
<th>Signature</th>
<th>Description</th>
</thead>

<tbody>

<tr>
<td><code>nil</code></td>
<td></td>
<td>Alias for <code>{}</code></td>
</tr>

<tr>
<td><code>func</code></td>
<td><code>(func ([name] [args]) [body])</code></td>
<td>Macro that defines a named function</td>
</tr>

<tr>
<td><code>int?</code></td>
<td><code>(int? [arg1])</code></td>
<td>Checks that argument is an integer</td>
</tr>

<tr>
<td><code>float?</code></td>
<td><code>(float? [arg1])</code></td>
<td>Checks that argument is a floating point</td>
</tr>

<tr>
<td><code>str?</code></td>
<td><code>(str? [arg1])</code></td>
<td>Checks that argument is a string</td>
</tr>

<tr>
<td><code>builtin?</code></td>
<td><code>(builtin? [arg1])</code></td>
<td>Checks that argument is a builtin</td>
</tr>

<tr>
<td><code>fn?</code></td>
<td><code>(fn? [arg1])</code></td>
<td>Checks that argument is a user-defined function</td>
</tr>

<tr>
<td><code>macro?</code></td>
<td><code>(macro? [arg1])</code></td>
<td>Checks that argument is a macro</td>
</tr>

<tr>
<td><code>bool?</code></td>
<td><code>(bool? [arg1])</code></td>
<td>Checks that argument is a boolean</td>
</tr>

<tr>
<td><code>qexpr?</code></td>
<td><code>(qexpr? [arg1])</code></td>
<td>Checks that argument is a Q-Expression</td>
</tr>

<tr>
<td><code>dict?</code></td>
<td><code>(dict? [arg1])</code></td>
<td>Checks that argument is a Dictionary</td>
</tr>

<tr>
<td><code>list?</code></td>
<td><code>(list? [arg1])</code></td>
<td>Alias for <code>qexpr?</code></td>
</tr>

<tr>
<td><code>nil?</code></td>
<td><code>(nil? [arg1])</code></td>
<td>Checks that argument is <code>nil</code></td>
</tr>

<tr>
<td><code>to-str</code></td>
<td><code>(to-str [arg1])</code></td>
<td>Converts argument to a string</td>
</tr>

<tr>
<td><code>do</code></td>
<td><code>(do [expr1] [expr2] ... [exprn])</code></td>
<td>Evaluates its arguments one by one, and returns the result of the last
argument</td>
</tr>

<tr>
<td><code>compose</code></td>
<td><code>(compose [f] [g] [xs...])</code></td>
<td>Composes two functions</td>
</tr>

<tr>
<td><code>flip</code></td>
<td><code>(flip [f] [x] [y])</code></td>
<td>Takes a function and two argument, and flip the ordering of the arguments</td>
</tr>

<tr>
<td><code>id</code></td>
<td><code>(id [x])</code></td>
<td>The identity function, returns whatever is passed</td>
</tr>

<tr>
<td><code>reduce</code></td>
<td><code>(reduce [f] [l] [acc])</code></td>
<td>Reduces a list to a single value using a reducer function</td>
</tr>

<tr>
<td><code>reduce-left</code></td>
<td><code>(reduce-left [f] [l] [acc])</code></td>
<td>Like <code>reduce</code>, but traverses the list in the opposite direction</td>
</tr>

<tr>
<td><code>map</code></td>
<td><code>(map [f] [l])</code></td>
<td>Applies a function to each element of a list</td>
</tr>

<tr>
<td><code>filter</code></td>
<td><code>(filter [f] [l])</code></td>
<td>Uses a predicate function to filter out elements from a list</td>
</tr>

<tr>
<td><code>any</code></td>
<td><code>(any [f] [l])</code></td>
<td>Checks whether any value in list <code>l</code> satisfies <code>f</code></td>
</tr>

<tr>
<td><code>all</code></td>
<td><code>(all [f] [l])</code></td>
<td>Checks whether all values in list <code>l</code> satisfy <code>f</code></td>
</tr>

<tr>
<td><code>sum</code></td>
<td><code>(sum [l])</code></td>
<td>Sums elements of a list</td>
</tr>

<tr>
<td><code>product</code></td>
<td><code>(product [l])</code></td>
<td>Multiplies together elements of a list</td>
</tr>

<tr>
<td><code>pack</code></td>
<td><code>(pack [f] [args...])</code></td>
<td>Takes multiple argument and feeds it to a function as a single list
argument</td>
</tr>

<tr>
<td><code>unpack</code></td>
<td><code>(unpack [f] [l])</code></td>
<td>Evaluates a function using a list of arguments</td>
</tr>

<tr>
<td><code>nth</code></td>
<td><code>(nth [n] [l])</code></td>
<td>Returns the <code>nth</code> element of a list</td>
</tr>

<tr>
<td><code>zip</code></td>
<td><code>(zip [lists...])</code></td>
<td>Returns a list of lists, each containing the i-th element of the argument lists</td>
</tr>

<tr>
<td><code>take</code></td>
<td><code>(take [n] [l])</code></td>
<td>Takes the first <code>n</code> elements of a list</td>
</tr>

<tr>
<td><code>drop</code></td>
<td><code>(drop [n] [l])</code></td>
<td>Drops the first <code>n</code> elements of a list, returning what's
left</td>
</tr>

<tr>
<td><code>member?</code></td>
<td><code>(member? [x] [l])</code></td>
<td>Checks if an element is a member of a list</td>
</tr>

<tr>
<td><code>range</code></td>
<td><code>(range [s] [e])</code></td>
<td>Returns a list of integers starting with <code>s</code> and going up to
<code>e</code></td>
</tr>

<tr>
<td><code>dict-items</code></td>
<td><code>(dict-items [dict])</code></td>
<td>Returns a list of key-value pairs from the given dict</td>
</tr>

<tr>
<td><code>random-between</code></td>
<td><code>(random-between [s] [e])</code></td>
<td>Returns a random floating point between <code>s</code> and
<code>e</code></td>
</tr>

</tbody>

</table>

## TODO
This a list of a non-detailed ToDo list:

### Sintax
- Work on a custom sintax different to lisp

### Language features
- string manipulation functions
    - index-of
    - concat
- proper error handling? Propagation and try/catch
- precision decimal (and fraction) types?
- complex number type?
- bignum integers?
- modules
- user defined types (algebraic data types?)
- pattern matching on user defined types?
- memory pool allocation
- closures that only add free variables?

### Interpreter features
- coloring, completion, etc
- test cases

### Low level APIs
- file system interface

### Standard library
- mathematical functions

## Contribute

## Credits
Many thanks goes to the following awesome libraries and open source projects, and their creators:

- mpc.c
- linenoise
- clang / LLVM

Also, thanks goes to the creator of the free "Build Your Own Lisp" online book, which is what Awl was inspired from.


## License
Legally, nobody can copyright a programming language (it's like try to copyright spanish, english or any other spoken language). So, **Spow** as a _programming language_ has a BDFL (Benevolent Dictator for Life) who determines what goes into the language and what doesn't. The BDFL is [Giovanny Andres Gongora Granada](http://gioyik.com) who was the person who started spow as programming language. However, the code of the interpreter, compiler and files in this repository are licesed under [MIT License](). 