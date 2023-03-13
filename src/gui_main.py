from tkinter import *
from tkinter import filedialog
import functions
root=Tk()
root.geometry('200x300')
root.title('Risc-v Simulator')

f1=Frame(root)
f2=Frame(root)

for frame in (f1, f2):
    frame.grid(row=0, column=0, sticky='news')

greeting=Label(f1, text="Upload a file to continue").pack()
upload=Button(f1, text='Upload Dump File', command=lambda:functions.func_upload(f1,f2)).pack()

functions.raise_frame(f1)
root.mainloop()