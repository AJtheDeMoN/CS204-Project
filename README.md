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
|&ensp;├── GUI MAIN<br>
|&ensp;|&ensp;├── <br>
|&ensp;|&ensp;└── <br>
|&ensp;├── Src <br>
|&ensp;|&ensp;├── Backend<br>
|&ensp;|&ensp;|&ensp;├── Headers<br>
|&ensp;|&ensp;|&ensp;|&ensp;├── Common.h<br>
|&ensp;|&ensp;|&ensp;|&ensp;├── Decode.h<br>
|&ensp;|&ensp;|&ensp;|&ensp;├── Execute.h<br>
|&ensp;|&ensp;|&ensp;|&ensp;├── Fetch.h<br>
|&ensp;|&ensp;|&ensp;|&ensp;├── MeMAccess.h<br>
|&ensp;|&ensp;|&ensp;|&ensp;├── PC.h<br>
|&ensp;|&ensp;|&ensp;|&ensp;├── Pipeline.h<br>
|&ensp;|&ensp;|&ensp;|&ensp;├── Predictor.h<br>
|&ensp;|&ensp;|&ensp;|&ensp;├── test.h<br>
|&ensp;|&ensp;|&ensp;|&ensp;└── Writeback.h<br>
|&ensp;|&ensp;|&ensp;├── inst.txt<br>
|&ensp;|&ensp;|&ensp;├── main.cpp<br>
|&ensp;|&ensp;|&ensp;├── memory.txt<br>
|&ensp;|&ensp;|&ensp;├── output.txt<br>
|&ensp;|&ensp;|&ensp;├── temp.txt<br>
|&ensp;|&ensp;├── plan.png<br>
|&ensp;|&ensp;├── plan2.txt<br>
|&ensp;├── test<br>
|&ensp;|&ensp;├── bubblesort.mc<br>
|&ensp;|&ensp;├── fibonacci.mc<br>
|&ensp;|&ensp;├── simple_add.mc<br>
|&ensp;|&ensp;└── sum_of_array.mc<br>
|&ensp;└── Readme.md<br>
└─ .gitignore \

----------

### How to Execute 

For running programme: \
	First, make sure tkinter library is installed for python GUI. \
	Then, open terminal and run following instructions: 
	
	$cd src
	$python3 gui_main.py
	
You will get your desired output on the GUI.
