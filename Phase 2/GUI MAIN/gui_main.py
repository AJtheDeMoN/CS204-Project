from tkinter import *
from tkinter import filedialog
from ttkthemes import ThemedTk
import functions
root=Tk()
root.title('Risc-v Simulator')
root.eval('tk::PlaceWindow . top')
root['bg']='#282828'

root.geometry('200x420')

# Run the tkinter event loop
# root.mainloop()



f1=Frame(root,width=1000,bg='#282828')
f2=Frame(root)

for frame in (f1, f2):
    frame.grid(row=0, column=0, sticky='news')

greeting=Label(f1, text="Upload a file to continue\n____________________",anchor="center",pady=10,width=21,border=5,bg='#282828',fg='white',font=16)
greeting.grid(row=20,column=0,columnspan=2)

upload=Button(f1, text='Upload Dump File', command=lambda:functions.func_upload(f1,f2),anchor="center",width=15,border=5,bg='#202020',fg='white',relief=RAISED,font=10)
upload.grid(row=21,column=0,columnspan=2)

# root.after(5000,root.destroy)

functions.raise_frame(f1)
root.mainloop()
