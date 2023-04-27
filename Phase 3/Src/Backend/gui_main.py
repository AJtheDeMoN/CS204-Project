import os
import streamlit as st
import subprocess

# st.set_option('font.size', 20)
def reset_txt():
    reg=open('register.txt', "w")
    for i in range(32):
        reg.write(f"{hex(i)} 0x0\n")
    reg.close()
    mem=open('memory.txt', "w")
    for i in range(0,1024,4):
        mem.write(f"{hex(i)} 0x0\n")
    mem.close()

def run_simulator(file_path, pipeline, forwarding, reg, pipe_all, pipe_x):
    cmd = ['g++', '-o', 'simulator', 'main.cpp']
    subprocess.run(cmd, check=True)
    args = ['./simulator', file_path]
    if pipeline:
        args.append('-p')
    if forwarding:
        args.append('-f')
    if reg:
        args.append('-v')
        args.append('r')
    if pipe_all:
        args.append('-v')
        args.append('p')
    if pipe_x:
        args.append('-v')
        args.append(str(pipe_x))
        

    result = subprocess.run(args, text=True)
    return result.stdout, result.stderr

st.title('RISC-V Simulator')
file_path = st.file_uploader('Select a file')

if file_path:
    pipeline = st.checkbox('Pipeline mode')
    forwarding = st.checkbox('Enable forwarding/stalls')
    reg = st.checkbox('Print register values')
    pipe_all = st.checkbox('Print pipeline status for all instructions')
    debug=st.checkbox('Debug mode')
    
    if(debug):
        if 'reset' not in st.session_state:
            st.session_state.reset = False
            reset_txt()
        if 'pipe_x' not in st.session_state:
            st.session_state.pipe_x = 0
        if st.button('Next'):
            st.session_state.pipe_x += 1
            print(st.session_state.pipe_x)
            output, error = run_simulator(file_path.name, pipeline, forwarding, reg, pipe_all, st.session_state.pipe_x)
            st.code(output)
            if os.path.isfile('register.txt') and os.path.isfile('memory.txt'):
                with st.beta_container():
                    reg_output = ''
                    with open('register.txt', 'r') as f:
                        reg_output = f.read()
                    mem_output = ''
                    with open('memory.txt', 'r') as f:
                        mem_output = f.read()

                    col1, col2 = st.beta_columns(2)
                    with col1:
                        st.title('Registers')
                        st.code(reg_output, language='text')
                    with col2:
                        st.title('Memory')
                        st.code(mem_output, language='text')
            elif os.path.isfile('register.txt'):
                with open('register.txt', 'r') as f:
                    reg_output = f.read()
                    st.title('Registers')
                    st.code(reg_output, language='text')
            elif os.path.isfile('memory.txt'):
                with open('memory.txt', 'r') as f:
                    mem_output = f.read()
                    st.title('Memory')
                    st.code(mem_output, language='text')
            if error:
                st.error(error)



    
    if st.button('Run'):
        output, error = run_simulator(file_path.name, pipeline, forwarding, reg, pipe_all, 0)
        st.code(output)
        if os.path.isfile('register.txt') and os.path.isfile('memory.txt'):
            with st.beta_container():
                reg_output = ''
                with open('register.txt', 'r') as f:
                    reg_output = f.read()
                mem_output = ''
                with open('memory.txt', 'r') as f:
                    mem_output = f.read()

                col1, col2 = st.beta_columns(2)
                with col1:
                    st.title('Registers')
                    st.code(reg_output, language='text')
                with col2:
                    st.title('Memory')
                    st.code(mem_output, language='text')
        elif os.path.isfile('register.txt'):
            with open('register.txt', 'r') as f:
                reg_output = f.read()
                st.title('Registers')
                st.code(reg_output, language='text')
        elif os.path.isfile('memory.txt'):
            with open('memory.txt', 'r') as f:
                mem_output = f.read()
                st.title('Memory')
                st.code(mem_output, language='text')
        if error:
            st.error(error)


