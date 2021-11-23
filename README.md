C++ minishell

--------------------------------------------------------
USER GUIDE

1) Open terminal
2) Enter "g++ file_name -lreadline -o shell" to compile cpp file (Note: file_name in our case is 55850833_55819497.cpp)
3) Enter "./shell" to run the program
4) When program prompts "sh>", you can enter your commands
5) Enter fg [ path of executable file ] [ a list of arguments ] to execute program in foreground process
Example: fg ./test hello 2 5
6) Enter bg [ path of executable file ] [ a list of arguments ] to execute program in background process
Example: bg ./test hello 2 5
7) If you want to stop foreground process, press CTRL+Z
8) If you want to terminate foreground process, press CTRL+C
9) If you want to see all running or stopped processes, use "list" command 
10) If you want to exit the shell, use "exit" command, all processes will be terminated.
11) If you want to start new program in foreground process, you will first need to terminate current foreground process by pressing CTRL+C (if there is any), and then start new foreground process by following step 5) of this user guide.

--------------------------------------------------------
ALL POSSIBLE OUTPUTS

1) NULL command - empty line entered
2) Unknown command - entered command does not exist
3) Couldn't run the program - entered path is incorrect
4) fork failed - creating child process failed
4) Process [pid] stopped - process with this pid is stopped by pressing CTRL+Z
5) Process [pid] terminated - process with this pid is terminated
6) Process [pid] completed - process with this pid is 
7) There is already a foreground process - it means you tried to start new foreground process while there is another stopped foreground process. To start new program in foreground process follow step 11) of this user guide
