from tkinter import *
from tkinter import filedialog
import subprocess

def raise_frame(frame):
    frame.tkraise()

def reset_txt():
    reg=open('register.txt', "w")
    for i in range(32):
        reg.write(f"0x{i} 0x0\n")
    reg.close()
    mem=open('memory.txt', "w")
    for i in range(0,1024,4):
        mem.write(f"0x{i} 0x0\n")
    mem.close()

def nxt_func(nxt, line, text, registers, mem):
    nxt.configure(text="Next")
    idx=float(line[0])
    idx2=float(line[1])
    print(idx, idx2)
    if idx2!=0.0:
        text.tag_remove("here", idx2, idx2+1)
        text.tag_add("there", idx2, idx2+1)
        text.tag_config("there", background="white")
        
    text.tag_add("here", idx, idx+1)
    text.tag_remove("there", idx, idx+1)
    text.tag_config("here", background="yellow")
    
    args=text.get(idx,idx+1).split(' ')
    print(args)
    terminal=""
    if(idx==1.0):
        terminal=subprocess.run(['gcc', 'myRISCVdebug.c', '-o', 'debug', '&&', 'debug', args[0], args[1]],stdout=subprocess.PIPE, shell=True)
        reset_txt()
    else:
        terminal=subprocess.run(['debug', args[0], args[1]],stdout=subprocess.PIPE, shell=True)

    pc=int(terminal.stdout.decode(), 16)
    print(pc)
    if(pc!=(0xffffffff)):
        line[1]=line[0]
        line[0]=pc/4+1
    reg=open("register.txt", "r")
    regVals=reg.read()
    reg.close()
    registers.insert(1.0, regVals)
    memory=open("memory.txt", "r")
    memVals=memory.read()
    memory.close()
    mem.insert(1.0, memVals)
    

def run_func(location):
    subprocess.run(['gcc', 'myRISCVsim.c', '-o', 'run', '&&', 'run', str(location)], shell=True)
    reg=open("register.txt", "r")
    regVals=reg.read()
    reg.close()
    regWindow=Toplevel()
    regWindow.geometry('300x400')
    regWindow.title('Registers')
    registers=Text(regWindow, height=10, width=18, pady=5)
    registers.insert(1.0, regVals)
    registers.pack()

    mem=open("memory.txt", "r")
    memVals=mem.read()
    mem.close()
    memWindow=Toplevel()
    memWindow.geometry('300x400')
    memWindow.title('Memory')
    mem=Text(memWindow, height=10, width=18, pady=5)
    mem.insert(1.0, memVals)
    mem.pack()

def debug_func(content, root):
    line=[1, 0]
    raise_frame(root)
    above=Frame(root)
    above.pack()
    below=Frame(root)
    below.pack()
    below.configure()
    reg=open("register.txt", "r")
    regVals=reg.read()
    reg.close()
    mem=open("memory.txt", "r")
    memVals=mem.read()
    mem.close()
    regWindow=Toplevel()
    regWindow.geometry('300x400')
    regWindow.title('Registers')
    registers=Text(regWindow, height=10, width=18, pady=5)
    registers.insert(1.0, regVals)
    registers.pack()
    memWindow=Toplevel()
    memWindow.geometry('300x400')
    memWindow.title('Memory')
    mem=Text(memWindow, height=10, width=18, pady=5)
    mem.insert(1.0, memVals)
    mem.pack()
    code=Text(above, height=10, width=18, pady=5)
    code.insert(1.0, content)
    btn_nxt=Button(below, text="Start", padx=10, command=lambda: nxt_func(btn_nxt, line, code, registers, mem))
    code.pack()
    btn_nxt.pack()
    print('debugging')

def func_upload(root, f2):
    root.filename=filedialog.askopenfilename(initialdir="c:/Desktop/", title="select a dumpfile (.mc)", filetypes=[("dump files", "*.mc")])
    location=root.filename
    f=open(location, "r")
    content=f.read()
    f.close()
    above=Frame(root)
    above.pack()
    below=Frame(root)
    below.pack()
    below.configure()
    code=Text(above, height=10, width=18, pady=5)
    btn_run=Button(below, text="Run", padx=10, command=lambda: run_func(location))
    btn_debug=Button(below, text="Debug", padx=10, command=lambda: debug_func(content,f2))
    code.insert(1.0, content)
    code.pack()
    btn_run.grid(row=0, column=0)
    btn_debug.grid(row=0, column=1)
