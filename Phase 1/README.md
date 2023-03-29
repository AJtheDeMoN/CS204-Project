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
 &ensp;&ensp; | \
 &ensp;&ensp; |- doc \
 &ensp;&ensp;&ensp;&ensp;   | \
 &ensp;&ensp;&ensp;&ensp;   |- design-doc.docx \
 &ensp;&ensp; |- src \
 &ensp;&ensp;&ensp;&ensp;     | \
 &ensp;&ensp;&ensp;&ensp;    |- functions.py \
 &ensp;&ensp;&ensp;&ensp;    |- gui_main.py \
 &ensp;&ensp;&ensp;&ensp;    |- myRISCVsim.c \
 &ensp;&ensp;&ensp;&ensp;    |- memory.txt \
 &ensp;&ensp;&ensp;&ensp;    |- myRISCVdebug.c \
 &ensp;&ensp;&ensp;&ensp;    |- register.txt \
 &ensp;&ensp;|- test \
 &ensp;&ensp;&ensp;&ensp;  | \
 &ensp;&ensp;&ensp;&ensp;  |- bubble_sort.mc  \
 &ensp;&ensp;&ensp;&ensp;  |- fibonacci.mc  \
 &ensp;&ensp;&ensp;&ensp;  |- sum_of_array.mc  \
 &ensp;&ensp; |- readme.txt 
 
----------

### How to Execute 

For running programme: \
	First, make sure tkinter library is installed for python GUI. \
	Then, open terminal and run following instructions: 
	
	$cd src
	$python3 gui_main.py
	
You will get your desired output on the GUI.
