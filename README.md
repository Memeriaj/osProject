# osProject
## The Final Project for CSSE 332 - Operating Systems
Authors: [Austin Fahsl](https://github.com/fahslaj),
[Alex Memering](https://github.com/memeriaj) and
[Joel Shapiro](https://github.com/jshap70).

The goal of the overall project is to create a very small, but working, operating system.

## Milestone notes
### Milestone 4
We implemented all of the commands required.  The writeFile function cannot be
directly called from the shell, but is implemented in both copy and create.
Additionally the dir command is entirely written in the shell file needing no
extra interrupts defined in the kernel, it also displays the number of sectors
per file for extra credit.

As another note we've partially fixed backspacing in the shell command-line as
now one cannot backspace over the shell prompt, but it does not move or
overwrite the existing characters.
