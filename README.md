#Zlango
It's a programming language made by people like you.

## About Zlango
Zlango starts as a try to create and understand how programming languages are built and how someone could create (or try to create) one. This project has not the vision of be the next big programming languaje in the world and it will not have that goal in the future, but it's a good start to learn about programming languages, interpreters and compilers. This is project is intended to be a platform for everybody who wants to try The language programming challenge. Everything borns in base of [Build your own Lisp book](), it provides a good start but is not a complete guide, this project will try to expand the limits of the book and let people like you start your programming language with all the rules and great things of a professional language.

### About Micro Parser Combinators (mpc)
_mpc_ is a lightweight and powerful Parser Combinator library for C.

Using _mpc_ might be of interest to you if you are...

* Building a new programming language
* Building a new data format
* Parsing an existing programming language
* Parsing an existing data format
* Embedding a Domain Specific Language
* Implementing [Greenspun's Tenth Rule](http://en.wikipedia.org/wiki/Greenspun%27s_tenth_rule)

### Basic Features
Zlango supports inline comments using hash (`#`):

    # Comment here
    (put "hello") # More comments here

Printing to standard output can be done using `put` and `putln`:

    zlango> (putln "Hello world")
    Hello world!

Variables are created with `lvar` (which affects the local environment) and
`var` (which, as the name suggests, affects the global environment):

    zlango> (var foo 'bar')
    zlango> (putln foo)
    bar

## Requeriments:
- `gcc` (tested with version 4.9.1)
- You'll need `make`

## Build and run
Most of Zlango dependencies are included in the repository, so you shouldn't need to install anything other than the build tools, so you will need a fairly recent C compiler. First, clone the repository, and then compile using `make`:

    $ git clone https://github.com/Zlango/zlango
    $ cd zlango
    $ make

This will create an `zlango` binary under `out/bin/` directory.

## Usage
The `zlango` binary can take a single argument - a path to a file to execute.

    $ ./out/bin/zlango [file].zl

If no argument is given, then it will drop into the REPL (interpreter):

    $ ./out/bin/zlango
	 ______                         
	 |__  / | __ _ _ __   __ _  ___  
	   / /| |/ _` | '_ \ / _` |/ _ \ 
	  / /_| | (_| | | | | (_| | (_) |
	 /____|_|\__,_|_| |_|\__, |\___/ 
	                     |___/       

	Zlango repl - vX.X.X (Press Ctrl+c or type exit to finish)
	zlango> 

## Docs

## TODO

## Contribute

## Credits

## License
Legally, nobody can copyright a programming language (it's like try to copyright spanish, english or any other spoken language). So, **Zlango** as a _programming language_ has a BDFL (Benevolent Dictator for Life) who determines what goes into the language and what doesn't. The BDFL is [Giovanny Andres Gongora Granada](http://gioyik.com) who was the person who invented and started zlango as programming language. However, the code of the interpreter, compiler and files in this repository are licesed under [MIT License](). 