MYSHELL				General Manual

NAME
	myshell

AUTHOR
	Sean DiGirolamo

SYNOSIS
	myshell [file (optional)]

DESCRIPTION
	myshell is a simple shell that reads from either standard input or from
	a file to execute programs.

USAGE
	If myshell is executed without any arguments, a command line will be
	initiated. From there, the user can enter the name of a program and any
	arguments to be passed to that program separated by whitespace. The
	shell will then execute the command and print its output to the screen

	The shell will search in certain specific places for executable programs
	If the user enters an absolute path (eg. /home/$USER/myshell), the shell
	will search that exact location and nothing else. If the user enters
	simply the name of the program, the shell will search all of the folders
	listed in the PATH environment variable. if the user enters a './' the
	shell will search for the program relative to the current directory,
	where './' is replaced by the current directory.

	After the name of the program, the user can specify any arguments to be
	passed to the program separated by whitespace.

	If myshell is executed, say, from another shell, it can also be passed
	an argument specifying a file. This file, if found, will be interpreted
	as a line seperated list of commands which the shell will execute
	sequentially, then quit.

	myshell also supports input redirection, &, and pipes, and those are
	descibed in "INPUT REDIRECTION" and "PIPELINES", later in this readme.

BUILT IN COMMANDS
	myshell supports a number of builtin commands. Here they are listed with
	their purpose

	cd <directory>
		cd changes the current working directory to the directory
		specified in <directory>. If <directory> starts with a /, it
		will be interpreted as an absolute path. If it starts with a
		'~', then it will be considered as relative to the users home
		directory. If it starts with anything else, it will be
		considered as relative to the current directory.

		If cd is executed without any arguments, it will change to the
		users home directory
	clr
		clears the screen. Will put the prompt at the bottom of the
		screen. If you don't like this, most distributions come with
		a clear command in the bin and you may prefer that, even though
		this is probably much faster.
	dir <directory>
		Briefly lists the contents of the directory specified in
		<directory>. If <directory> is not specified, it will list the
		contents of the present working directory.
	environ
		Lists all environment variables and their values
	echo <string>
		Prints <string> to the screen. Will not print whitespace unless
		it is inside quotes. In addition, any whitespace will be changed
		to just one space
	help
		Prints this manual to more
	pause
		Stops the shell from doing anything until the enter key is
		pressed
	quit
		Exits the shell. You can also hit ctrl-c in most distributions
		or simply close the window

I/O REDIRECTION
	myshell supports a limited amount of input redirection

	>
		If a > is detected in the command line, the standard output of
		the program will be changed from the screen to a file specified
		by the first string after the >. If the file does not exist,
		it will be created, and if it does exist, it will replace it
	>>
		Does the same thing as the '>' operator, but will not replace
		the file. Instead it will append to it.

	<
		If a < is detected in the command, the standard input will be
		changed from the keyboard to a file specified after the '<'

PIPELINES
	Pipelines are a unique kind of I/O redirection. They can be used to
	separate multiple commands and are symbolized by '|'. An example is
		ls | grep l
	When a pipe is used, the first commands output will be redirected to the
	pipe, and then the second commands input will be taken from the pipe.
	In other words, the first commands output will be sent to the second
	commands input and then the second commands input will be printed to the
	screen.
	In the case of the command above, ls's output is routed to grep's input,
	processed by grep, and then printed to the screen. This will print
	all directories in the current folder with an l in the name

BACKGROUND PROGRAM EXECUTION
	When the shell executes a command, it normally creates a new process,
	and then suspends all operation until that process finishes. However,
	using the '&' symbol, this can be changed.

	&
		If this symbol is detected at the end of the line, then the
		shell will not wait for the command to complete, or process to
		exit before it lets the user enter another command. For example,
		if & is not included and the user executes a browser, the user
		will not be able to execute any other commands from that shell
		instance until the browser is closed. if & is included,
		then the user will be able to execute another command without
		first waiting for the browser to close.
