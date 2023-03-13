================================
Functional Simulator for RISC-V
================================

README

Table of contents
1. Directory Structure
2. How to build
3. How to execute



Directory Structure:
--------------------
CS204-Project
  |
  |- doc
      |
      |- design-doc.docx
  |- src
      |
      |- functions.py
      |- gui_main.py
      |- myRISCVsim.c
  |- test
      |
      |- bubble_sort.mem
      |- fibonacci.mem
      |- sum_of_array.mem
  |- readme.txt
  
How to build
------------
For running programme:
	First, make sure tkinter library is installed.
	$cd src
	$python3 gui_main.py

For cleaning the project:
	$cd src
	$make clean


How to execute
--------------
./myARMSim test/simple_add.mem
