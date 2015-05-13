# osProject
## The Final Project for CSSE 332 - Operating Systems
Authors: [Austin Fahsl](https://github.com/fahslaj),
[Alex Memering](https://github.com/memeriaj) and
[Joel Shapiro](https://github.com/jshap70).

The goal of the overall project is to create a very small, but working, operating system.

## To Build
Run 'make' from the top level directory (aka just inside the m5 folder in the SVN
repo).  This will compile everything and start 'bochs' bringing you right to the
console.

## Testing it
Type 'help' into the console.  Then type into the console and run the commands
that it lists with valid parameters as it instructs.

## Bugs
* Parameter checking
	We do not check any of the parameters that are given to any of the shell
	commands; so make sure that they are valid.
* Shell indicator
	Because commands are running on different processes then they may appear to
	print their output in the inccorect order (or spliced together).
* Same name files
	Multiple files with the same name can be created but only the first file
	with that name will ever be accessed.
* Process switching
	Apperently processes seem to only switch over when they use interrupts.

## Additional features
* top
	We have implemented the "top" command, which functions just as the "top" command in linux. When run, it displays a list of all top-level processes that are currently running.
* dir
	The "dir" command displays the files in the filesystem, as per requirement. However, it also shows the size of each file in the system.
* Backspace printing space
	By default, a backspace character will only move the cursor back and not change the character displayed on the screen at the location of the cursor. However, we have changed this to print a space at the previous location of the cursor, which makes the backspace key actually delete the character on the screen as well as in the buffer.
* help
	The "help" command shows a list of all of the other commands, what each of their purposes is, and calling procedures and parameters of each command. 

## Notes
Please note that the excuteProgram method in kernel.c still takes two
parameters, however this is because the second parameter is purely used by the
kernel itself.  It is used since the blocking execute is very similar so the
methods were just combined and the second parameter is the differentiator.
