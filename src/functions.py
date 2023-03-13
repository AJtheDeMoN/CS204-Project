from tkinter import *
from tkinter import filedialog
import subprocess
# copy=gui_main.root
def raise_frame(frame):
    frame.tkraise()

def nxt_func(nxt, line, text):
    nxt.configure(text="Next")
    idx=float(line[0])
    if idx!=1.0:
        text.tag_add("there", idx-1, idx)
        text.tag_config("there", background="white")
        
    text.tag_add("here", idx, idx+1)
    text.tag_config("here", background="yellow")
    
    args=text.get(idx,idx+1)
    print(text.get(idx,idx+1).split(" ")[1][:-1])
    # terminal=subprocess.run(['gcc', 'debug.c', '-o', 'debug', '&&', 'debug', 'args'],stdout=subprocess.PIPE, shell=True)
    # pc=int(terminal)
    # line[0]+=pc
    line[0]+=1

def run_func():
    subprocess.run(['gcc', 'myRISCVsim.c', '-o', 'myRISCVsim', '&&', 'myRISCVsim'], shell=True)
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
    line=[1]
    raise_frame(root)
    above=Frame(root)
    above.pack()
    below=Frame(root)
    below.pack()
    below.configure()
    code=Text(above, height=10, width=18, pady=5)
    code.insert(1.0, content)
    btn_nxt=Button(below, text="Start", padx=10, command=lambda: nxt_func(btn_nxt, line, code))
    code.pack()
    btn_nxt.pack()
    print('debugging')

def func_upload(root, f2):
    root.filename=filedialog.askopenfilename(initialdir="c:/Desktop/", title="select a dumpfile (.mc)", filetypes=[("dump files", "*.mc")])
    f=open(root.filename, "r")
    content=f.read()
    f.close()
    # print('hi', content)
    above=Frame(root)
    above.pack()
    below=Frame(root)
    below.pack()
    below.configure()
    code=Text(above, height=10, width=18, pady=5)
    btn_run=Button(below, text="Run", padx=10, command=lambda: run_func())
    btn_debug=Button(below, text="Debug", padx=10, command=lambda: debug_func(content,f2))
    code.insert(1.0, content)
    code.pack()
    btn_run.grid(row=0, column=0)
    btn_debug.grid(row=0, column=1)
