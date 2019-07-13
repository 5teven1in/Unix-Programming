# Simple Instruction Level Debugger

In this homework, we are going to implement a simple instruction-level debugger that allows a user to debug a program interactively at the assembly instruction level. You can implement the debugger by using the ptrace interface. The commands you have to implement are summarized as follows:

- break {instruction-address}: add a break point
- cont: continue execution
- delete {break-point-id}: remove a break point
- disasm addr: disassemble instructions in a file or a memory region
- dump addr [length]: dump memory content
- exit: terminate the debugger
- get reg: get a single value from a register
- getregs: show registers
- help: show this message
- list: list break points
- load {path/to/a/program}: load a program
- run: run the program
- vmmap: show memory layout
- set reg val: get a single value to a register
- si: step into instruction
- start: start the program and stop at the first instruction

The details of each command are explained below. In a debugging process, you have to load a program first, configure the debugger, and start debugging by running the program. A debugger command may be only used in certain "states." The states include **any**, **loaded**, and **running**. **any** means that a command can be used at any time. **loaded** means that a command can be only used when a program is loaded. **running** means that a command can be only used when the program is running. We will use brackets right after a command to enclose the list of the state(s) that should be supported by the command.



- **break** or **b** [**loaded** and **running**]: Setup a break point. If a program is loaded but is not running, the address should be within the range specified by the text segment in the ELF file. When a break point is hit, you have to output a message and indicate the corresponding address and instruction.
- **cont** or **c** [**running**]: continue the execution when a running program is stopped (suspended).
- **delete** [**any**]: remove a break point.
- **disasm** or **d** [**loaded** and **running**]: Disassemble instructions in a file or a memory region. The address should be within the range specified by the text segment in the ELF file. You only have to dump 10 instructions for each command. If **disasm** command is executed without an address, it should disassemble the codes right after the previously disassembled codes. See the demonstration section for the sample output format.
- **dump** or **x** [**running**]: Dump memory content. You only have to dump 80 bytes from a given address. The output contains the addresses, the hex values, and printable ascii characters. If **dump** command is executed without an address, it should dump the region right after the previous dump.
- **exit** or **q** [**any**]: Quit from the debugger. The program being debugged should be killed as well.
- **get** or **g** [**running**]: Get the value of a register. Register names are all in lowercase.
- **getregs** [**running**]: Get the value of all registers.
- **help** or **h** [**any**]: Show the help message.
- **list** or **l** [**any**]: List break points, which contains index numbers (for deletion) and addresses.
- **load** [not **loaded**]: Load a program into the debugger. When a program is loaded, you have to print out the entry point, the address, the offset, and the size for the text segment.
- **run** or **r** [**loaded** and **running**]: Run the program. If the program is already running, show a warning message and continue the execution.
- **vmmap** or **m** [**loaded** and **running**]: Show memory layout for a running program. If a program is loaded but is not running, it should display the text segment address of the loaded program.
- **set** or **s** [**running**]: Set the value of a register
- **si** [**running**]: Run a single instruction, and step into function calls.
- **start** [**loaded**]: Start the program and stop at the first instruction.



For more details about the implementation, please check the demonstration section for the sample input and the corresponding output.
