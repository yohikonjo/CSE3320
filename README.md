# MavShell
Mav shell(msh) is a shell program amde in C similar to bourne shell(bash), c-shell(csh), or korn shell(ksh). It accepts and executes common unix commands, and forks a child process. The msh like csh and ksh will keeping running and accepting commands until the user exits the shell.
# Features
•	Input string tokenization
•	 Forking of different processes 
•	can support up to 10 command line parameters in addition to the command
•	an execute UNIX commands in /bin, /usr/bin/, /usr/local/bin/, and the current working directory
•	supports the 'showpids' command that lists the PIDs of up to the last 15 processes spawned by the shell
•	supports the 'history' which will list up to the last 15 commands entered by the user
•	 supports '!n', where n is a number between 1 and 15 which will result in the shell re-running the nth command
