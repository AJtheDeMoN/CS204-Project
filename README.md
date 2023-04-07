================================================
# Functional Simulator for RISC-V
================================================

## README

### Table of contents
- [Directory Structure](#directory-structure)
- [How to Execute](#how-to-execute)



### Directory Structure:
----------
CS204-Project \
├── Phase 2 \
| ├── GUI MAIN<br>
| |   ├── <br>
| |   └── <br>
| ├── Src <br>
| |   ├── Backend<br>
| |   |   ├── Headers<br>
| |   |   |   ├── Common.h<br>
| |   |   |   ├── Decode.h<br>
| |   |   |   ├── Execute.h<br>
| |   |   |   ├── Fetch.h<br>
| |   |   |   ├── MeMAccess.h<br>
| |   |   |   ├── PC.h<br>
| |   |   |   ├── Pipeline.h<br>
| |   |   |   ├── Predictor.h<br>
| |   |   |   ├── test.h<br>
| |   |   |   └── Writeback.h<br>
| |   |   ├── inst.txt<br>
| |   |   ├── main.cpp<br>
| |   |   ├── memory.txt<br>
| |   |   ├── output.txt<br>
| |   |   ├── temp.txt<br>
| |   ├── plan.png<br>
| |   ├── plan2.txt<br>
| ├── test<br>
| |   ├── bubblesort.mc<br>
| |   ├── fibonacci.mc<br>
| |   ├── simple_add.mc<br>
| |   └── sum_of_array.mc<br>
| └── Readme.md<br>
└─ .gitignore \

----------

### How to Execute 

For running programme: \
	First, make sure tkinter library is installed for python GUI. \
	Then, open terminal and run following instructions: 
	
	$cd src
	$python3 gui_main.py
	
You will get your desired output on the GUI.
