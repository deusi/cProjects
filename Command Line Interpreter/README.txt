/*test machines: csel-kh4240-[01], csel-kh4250-[38], Personal Computer: Ubuntu 19.04 x86_64 - 5.1.15-surface-linux-surface kernel
* name: Denis Rybkin
* x500: rybki001 */

PURPOSE:
  This project implements a simplified version of a command line interpreter, also known as a shell
program. The shell program will prompt the user to type in a command. The shell creates a
child process to execute the command and prompts the user for more once it has finished. The
shell program is similar but simpler than the ones offered in Unix systems. Our own versions of cd
ls and wc were also implemented. This allowed us to add more functionality to our programs and
not restrict ourselves to the version in `/bin/'. This project is meant to get the basic understanding
yof the Linux programming environment.

HOW TO COMPILE:
  Typing "make" (alternatively "make all" or "make build") in shellProgram directory of the project will
compile all required files for the project and the executable ./shell  along with ./cd, ./ls and ./wc
will be created.

HOW TO EXECUTE:
shell:
  The project can be executed by typing ./shell in terminal, where the executable file is located (in this
case, directory shellProgram). After that, the shell can accept any input.
cd:
  Cd part of the project can be executed separately by typing ./cd <path>
ls:
  Ls can be executed by ./ls <path> or ./ls -R <path> where -R indicated that the insides of every directory
should be printed as well
wc:
  Wc can be executed by using one of the following commands: ./wc [filename] ./wc -l [filename] ./wc -w [filename]
./wc -c [filename]. In this case ./wc [filename] would print every line word and character of a file [filename]
while -l, -w or -c would print either line count, word count or character count respectively.

WHAT THE PROGRAM DOES:
  The shell always displays the current working directory along with a dollar while prompting the user to enter
a command, as the original unix shell does. To differentiate from the original shell, we added the tag [4061-shell] at
the start. For example, if `home/csci4061/project' is the current working directory, it should display:
[4061-shell]/home/csci4061/project $
We use STDIN to read the user input using the read system calls. The shell is able
to parse any of the commands in the "how to execute" section and then execute the correct command along
with user provided arguments. In the shell, we used fork and exec to execute the command. If a
specified command is not one of the above 3 commands implemented by us, the shell calls the
system's original command executable directly using the exec function. If any error occurs during
the execution of the given command, the shell outputs `Command error'.
Additionally, we implemented exit command to terminate the shell by performing all the necessary steps to
quit the shell when handling the exit command. The shell also handles file redirection, pipes and combination
of both by using forks, dups, execs, open and other functions useful in implementation of mentioned features
(covered in lectures and labs).

ASSUMPTIONS:
  - ./cd works the way original cd is supposed to work only inside the ./shell. Otherwise, it prints the message that original cd
would have produced without changing the directory (since Ubuntu shell doesn't allow our version of cd to interact with it directly)
  - ./wc [filename] can't handle binary files the same way original wc does (number of lines and characters is the same, but number of
words is different). TA mentioned that it's not a problem and binary/exec files wouldn't be tested.
  - due to its simplicity, ./shell is much more primitive than its unix counterpart, hence it wasn't designed to perform operations outside
the ones which were described in the instructions to the project.
