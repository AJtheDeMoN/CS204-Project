from tkinter import *
from tkinter import filedialog
import subprocess

root=Tk()
root.geometry('300x100')
root.title('Test App')

greeting=Label(root, text="Upload a file to continue")

def nxt_func(nxt, line, text):
    nxt.configure(text="Next")
    idx=float(line[0])
    if idx!=1.0:
        text.tag_add("there", idx-1, idx)
        text.tag_config("there", background="white")
        
    text.tag_add("here", idx, idx+1)
    text.tag_config("here", background="yellow")
    
    line[0]+=1

def run_func():
    subprocess.run(['gcc', 'run.c', '-o', 'run', '&&', 'run'], shell=True)

def func_upload():
    root.filename=filedialog.askopenfilename(initialdir="c:/Desktop/Computer", title="select a dumpfile (.mc)", filetypes=[("dump files", "*.mc")])
    f=open(root.filename, "r")
    temp=f.read()
    # print(temp)
    # root.withdraw()
    new=Toplevel()
    new.geometry('500x300')
    above=Frame(new)
    above.pack()
    below=Frame(new)
    below.pack(pady=10)
    executor=Text(below, height=10, width=20, pady=5)
    curr_line=[1]
    nxt=Button(above, text='Start', padx=15, command=lambda: nxt_func(nxt, curr_line,executor))
    run=Button(above, text='Run', padx=15, command=run_func)
    nxt.pack(side=LEFT)
    run.pack(side=RIGHT)
    executor.insert(1.0, temp)
    executor.pack(side=LEFT)
    # print(executor.get(2.0, 3.0))

upload=Button(root, text='Upload Dump File', command=func_upload)
greeting.pack()
upload.pack(pady=10)
root.mainloop()