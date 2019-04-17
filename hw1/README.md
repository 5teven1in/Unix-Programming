# Implement a 'netstat -nap'-like program
In this homework, you have to implement a 'netstat -nap' tool by yourself. You have to list all the existing TCP and UDP connections. For each identified connection (socket descriptor), find the corresponding process name and its command lines that creates the connection (socket descriptor). **You have to implement all the features by yourself and cannot make calls to the system built-in netstat program nor parse output from 'netstat -nap'.** Your codes must be implemented in C and/or C++.

To provide more flexibilities, your program have to accept several predefined options, including

- -t or --tcp: list only TCP connections.
- -u or --udp: list only UDP connections.
- An optional string to list only command lines that containing the string.

You have to handle the additional options using getopt_long function. In short, the synopsis of homework #1 would be:

```$ ./hw1 [-t|--tcp] [-u|--udp] [filter-string]```

When no argument is passed, your program should output all identified connections. You may test your program with a root account so that your program would be able to access /proc files owned by other users.
