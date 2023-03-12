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
  |- bin
      |
      |- myARMSim
  |- doc
      |
      |- design-doc.docx
  |- include
      |
      |- myARMSim.h
  |- src
      |- main.c
      |- Makefile
      |- myARMSim.h
  |- test
      |- simple_add.mem
      |- fib.mem
      |- array_add.mem

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
