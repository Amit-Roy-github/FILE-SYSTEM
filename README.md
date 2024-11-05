# In-Memory File System Implementation

This is an in-memory file system designed to support various functionalities commonly found in a traditional file system . The implementaion is in C++ , providing a commad-line interface for interacting with the file system.

## Features 

The file system includes the following operations:

1. `mkdir` : Create a new directory .
2. `cd` : Change the current directory , supporting relative paths (`..` , `../` ) , absolute paths (`/`) , and navigating to specified path .
3. `ls`: List the content of current and specified directory .
4. `touch` : Create a new empty file .
5. `mv` : Move a file or directory to another location .
6. `cp` : Copy a file or directory to another location .
7. `rm` : Remove a file or directory .
8. `save` and `load` : Save and reload the file system state . 
