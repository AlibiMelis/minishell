#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream> 
#include <cstring> 
#include <string.h>
#include <readline/readline.h> 
#include <readline/history.h> 
#include <signal.h> 
#include <map>

using namespace std;

//Global Variables
char* args[10]; 
int status = 0;
pid_t fgpid = 0; // pid of the Process in a foreground

map<pid_t, map<string, string> > processes; // map to store info about Processes
// Format
// {pid : {"state": state, "path": filepath, "args": args}}

void execute(char* ground, int count);
void ctrlZHandler(int sig);
void ctrlCHandler(int sig);
void reap();
void complete(pid_t pid);
void stop(pid_t pid);
void terminate(pid_t pid);
void insert(pid_t pid, int count, char** args);
void info();
void childChanged(int sig);


int main(int argc, char **argv) {
	// Adding Signal handlers for Ctrl+C and Ctrl+Z
	signal(SIGINT, ctrlCHandler);
    signal(SIGTSTP, ctrlZHandler); 

	while (true) {
		
		char *input = NULL;
		char *command = NULL;
		int count = 0;
		for (int i = 0; i < 10; i++) { // cleaning args from previous loop
			args[i] = NULL;
		}

    	input = readline("sh >"); 
    	char *p = strtok(input," \t");

    	if (!p) {
    		cout << "NULL command\n";
    	} else {
    		command = p;

    		if (strcmp(command, "exit") == 0) {
    			// EXIT
    			for (auto const& pair: processes) { // terminate all existing Processes
					pid_t pid = pair.first;
					terminate(pid);
				}
    			break;
    		}

	    	while ((p = strtok(NULL," \t"))) {
	    		args[count++] = p;
	    	}

	    	if (strcmp(command, "fg") == 0) {
	    		if (fgpid != 0) { // Check if any fg Process existing
	    			cout << "There is already a foreground process.\n";
	    			continue;
	    		}
	    		// FG (foreground call)
			    execute(command, count);
    		} else if (strcmp(command, "bg") == 0) {
    			// BG (backround call)
    			execute(command, count);
			} else if (strcmp(command, "list") == 0) {
				// LIST
				info();
			} else { // any other unknown case
    			cout << "Unknown command " << command << "\n";
    		}
    	}
	}
}

void execute(char* ground, int count) {
	pid_t pid = fork(); // creating Child Process
	setpgid(0, 0); // Putting this shell in the foreground of Linux shell
	if (pid > 0) {
		insert(pid, count, args);
		signal(SIGCHLD, childChanged); // adding handler
		if (strcmp(ground, "fg") == 0) { // if fg Process, need to wait till the end
			fgpid = pid;
			pid = waitpid(-1, &status, WUNTRACED);
			if (pid > 0 && processes[pid]["state"] != "stopped") { // chech the reason of stop
				complete(pid);
			}
		}
	} else if (pid == 0) {
		if (execvp(args[0], args) < 0) { // Execute the file in a child process
			cout << "Couldn't run the program\n"; 
			exit(EXIT_FAILURE);
		}
	} else {
		cout << "fork failed\n";
		exit(EXIT_FAILURE);
	}
}

// Termination handler
void ctrlCHandler(int sig) {
	cin.clear();
	if (fgpid != 0) {
		terminate(fgpid);
		fgpid = 0;
	}
}

// Stop handler
void ctrlZHandler(int sig) {
	cin.clear();
	if (fgpid != 0) {
		stop(fgpid);
	}
}

// Reaping The Walking Dead
void reap() { 
	pid_t pid = waitpid(-1, &status, WNOHANG); // Check for any Proccess that is terminated then reap
	if (pid > 0) {
		complete(pid);
	}
}

// Completing Process
void complete(pid_t pid) {
	if (WEXITSTATUS(status) == 0) { // if completed noramally
		if (WIFEXITED(status) == 1) {
			cout << "Process " << pid << " completed\n";
		}
	}
	if (pid == fgpid) { // if fg Process is completed then change the variable
		fgpid = 0;      //  value to 0 to indicate that there is no fg Process
	}                  
	processes.erase(pid); // remove from map as no longer needed
}

// Stopping Process
void stop(pid_t pid) { 
	kill(pid, SIGSTOP);
	cout << "Process " << pid << " stopped\n";
	processes[pid]["state"] = "stopped";
}

// Terminating Process
void terminate(pid_t pid) {
	kill(pid, SIGTERM);
	cout << "Process " << pid << " terminated\n";
	processes.erase(pid); // remove from map as no longer needed
	reap();
}

// Inserting data about Process into map
void insert(pid_t pid, int count, char** args) {
	processes[pid]["state"] = "running";
	processes[pid]["path"] = args[0];

	string arguments = "";
	for (int i = 1; i < count; i++) {
		arguments += args[i];
		if (i != count - 1) {
			arguments += " ";
		}
	}
	processes[pid]["args"] = arguments;
}

// Displaying data about running and stopped Processes
void info() {
	for (auto const& pair: processes) {
		pid_t pid = pair.first;
		cout << pid << ": ";
		cout << processes[pid]["state"] << " ";
		cout << processes[pid]["path"] << " ";
		cout << processes[pid]["args"] << "\n";
	}
}

// Handler for any state change of Child Process
void childChanged(int sig) {
	reap();
}









