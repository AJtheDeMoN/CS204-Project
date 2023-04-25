#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// function to write a block of data to memory
void writeBlockToMemory(int* memory, int startAddress, int blockSize, const vector<int>& data) {
    for (int i = 0; i < blockSize; i++) {
        memory[startAddress + i] = data[i];
    }
}

int main() {
    // declare variables
    int memory[1024];
    string filename;
    ifstream file;
    int startAddress, blockSize, value;

    // initialize memory
    for (int i = 0; i < 1024; i++) {
        memory[i] = 0;
    }

    // get filename from user
    cout << "Enter filename: ";
    cin >> filename;

    // open file
    file.open(filename);

    // read data from file and write to memory
    while (file >> startAddress >> blockSize) {
        vector<int> data;
        for (int i = 0; i < blockSize; i++) {
            file >> value;
            data.push_back(value);
        }
        writeBlockToMemory(memory, startAddress, blockSize, data);
    }

    // close file
    file.close();

    // print contents of memory
    for (int i = 0; i < 1024; i++) {
        if (memory[i] != 0) {
            cout << i << ": " << memory[i] << endl;
        }
    }

    return 0;
}
