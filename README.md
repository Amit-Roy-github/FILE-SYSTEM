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

## Implementation Details 

### Data structures Used 

The file system is implemented using the following data structures : 

1. **Node Structure:**
  - Represents a directory or file .
  - Contains imformation such as name , type( directory or file ) , and a map of children .

2. **FileSystem Class:**
  - Manages the entire file system .
  - Utilizes a hierarchical tree structure , with a root node representing the root directory .
  - Tracks the current working directory using a pointer .


## Design Decisions 

1. ##File System Structures**
   - Hierarchical tree structures to represent directories and files.
   - Root node serves as the starting point for the entire file system.

2. **Directory Navigation (cd):**
   - Supports various path types for navigation, including relative (`..`, `../`), absolute (`/`), and specified paths.

3. **Directory Operations:**
	- **mkdir**: Create a new directory. 
	-   **cd**: Change the current directory, supporting relative paths (`..`, `../`), absolute paths (`/`), and navigating to a specified path. 
  -  **ls**: List the contents of the current or specified directory.

4. **File Operations:**
   -  **touch**: Create a new empty file. 
   - **echo**: Write text to a file.

5. **File and Directory Management:**
   - **mv**: Move a file or directory to another location.
   -  **cp**: Copy a file or directory to another location. 
   -  **rm**: Remove a file or directory.

6. **Save and Load State (Bonus):**
   - Implemented functionality to save and load the file system state.
   - Recursive approach to traverse the file system hierarchy during save and load operations.
