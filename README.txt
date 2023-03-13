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
  |- readme.txt
  |- src
      |
      |- Makefile
      |- myRISCVsim.c
  |- test
      |
      |- bubble_sort.mem
      |- fibonacci.mem
      |- simple_add.mem
      |- sum_of_array.mem

How to build
------------
For building:
	$cd src
	$make

For cleaning the project:
	$cd src
	$make clean


How to execute
--------------
./myARMSim test/simple_add.mem
