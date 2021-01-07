# linux-shell

This is a simple implementation of a shell in C. This project uses parts of some code taken from [this project.](https://github.com/brenns10/lsh) The main structure of the project is taken from here. I have just made some fancy improvements. To run it you need only two things :-
1. A Linux environment (an emulator or WSL)
2. GCC to run the C file.

## Structure 

This is a very simple linux shell and supports no fancy features such as autocomplete using TAB, or globbing. However taking a look at the source code, you get a fairly clear idea of how a linux shell works. Many commands (excluding very basic ones) may not even work in this shell.  
This uses execvp() function to execute commands. And since it replaces the current process with the one passed to it, fork() is used.  
Additionally, it also supports a history feature, which allows you to view a list of previously executed commands; and makes retrieving and executing commands from this list very easy. 

## Pre- Usage modifications

In ***shell.h***, there is a global variable string name ***hd***. I have kept it empty, simply initialize it with your **complete** home directory.  
Also in the function ***newLine()***, there is an empty printf statement. Linux terminal prints a new line after executing every command which normally consists of your name followed by a ***$*** symbol. So simply put your username (or whatever you wish) in that printf() statement.


## Usage

Simply use gcc to run the file ***history.c***. Simply type in ***gcc history.c*** and then type ***./a.out***.  
Using the history feature is also easy.  
The history buffer has a size of 10, so users can access 10 most recent commands using this feature. The commands will be consecutively numbered starting at 1, and the numbering will continue past 10.  
For example, if the user has entered 35 commands, the 10 most recent commands will be numbered 26 to 35. The user will be able to list the command history by entering the command ***history*** at the command prompt.  
1. When the user enters !!, the most recent command in the history is executed.
2. When the user enters a single ! followed by an integer N, the Nth command in the history is executed.  
  
After you quit the shell by pressing Control + D, the most recent commands are saved in a file named ***history.txt***. This file does not exist the first time you start this shell, so it is automatically created. The next time you start up the shell, the history buffer initializes with the contents of ***history.txt***.


