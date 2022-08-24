#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>

using namespace std;

uintptr_t followChain(vector<uintptr_t> chain, HANDLE hProcess) {
    uintptr_t currPtr = chain[0];

    for (int i = 1; i < chain.size(); ++i) {
        // get content of first ptr which should be a pointer
        uintptr_t next = 0;

        cout << "Reading at: 0x" << hex << currPtr << endl;
        bool succ = ReadProcessMemory(hProcess, (LPCVOID)currPtr, &next, sizeof(uintptr_t), NULL);
        if (!succ) {
            cout << "Something went wrong at: " << hex << currPtr << endl;
            return 0;
        }
        cout << "Read value: 0x" << hex << next << dec << " (" << next << ")" << endl;

        currPtr = next + chain[i];
    }

    return currPtr;
}

void readPointer(HANDLE hProcess) {
    int intRead = 0;
    uintptr_t memAddr = 0;

    cout << "Memory address: ";
    cin >> hex >> memAddr;

    ReadProcessMemory(hProcess, (LPCVOID)memAddr, &intRead, sizeof(int), NULL);

    cout << "\nintRead = " << intRead << endl;
}

void readPointerToVal(HANDLE hProcess) {
    uint64_t ptrAddr = 0;
    uint64_t ptrValue = 0;

    cout << "Pointer location: ";
    cin >> hex >> ptrAddr;

    ReadProcessMemory(hProcess, (LPCVOID)ptrAddr, &ptrValue, sizeof(uint64_t), NULL);
    cout << "\nPointer pointing at: 0x" << hex << uppercase << ptrValue << endl;

    int ptrRef = 0;
    ReadProcessMemory(hProcess, (LPCVOID)ptrValue, &ptrRef, sizeof(int), NULL);
    cout << "Value referenced by pointer: " << dec << ptrRef << endl;
}

void readPointerChain(HANDLE hProcess) {
    uint64_t ptrAddr = 0;

    cout << "Pointer chain base location: ";
    cin >> hex >> ptrAddr;

    vector<uintptr_t>* chain = new vector<uintptr_t>(4);

    chain->push_back(ptrAddr);
    chain->push_back(0);
    chain->push_back(0);
    chain->push_back(0);

    followChain(*chain, hProcess);

    delete chain;
}

void readStr(HANDLE hProcess) {
    uint64_t ptrAddr = 0;

    const int buff_size = 256;
    char buff[buff_size];

    memset(buff, 0, buff_size * sizeof(char));

    cout << "String var location: ";
    cin >> hex >> ptrAddr;

    ptrAddr += 8;

    ReadProcessMemory(hProcess, (LPCVOID) ptrAddr, buff, buff_size * sizeof(char), NULL);
    cout << "buff content: `" << buff << "`" << endl;
}

void writeInt(HANDLE hProcess) {
    int intToWrite = 987654;
    uintptr_t memAddr = 0;

    cout << "Memory address to write to: ";
    cin >> hex >> memAddr;

    BOOL res = WriteProcessMemory(hProcess, (LPVOID) memAddr, (LPCVOID) &intToWrite, sizeof(int), NULL);
    if (!res) {
        cout << "Something went wrong writing: " << dec << GetLastError() << endl;
        return;
    }

    cout << "Overridden successfully! c:" << endl;
}

int main()
{
    DWORD pid = 0;

    cout << "PID: ";
    cin >> dec >> pid;

    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid);

    if (hProcess == NULL) {
        // Failed
        std::cout << "OpenProcess failed. GetLastError = " << std::dec << GetLastError() << std::endl;
        system("pause");
        return EXIT_FAILURE;
    }

    // readPointer(hProcess);

    // readPointerToVal(hProcess);
    
    // readPointerChain(hProcess);

    // readStrAndBuff(hProcess);

    writeInt(hProcess);
    
    CloseHandle(hProcess);
    cout << "Press ENTER to quit." << endl;
    getchar();

    return EXIT_SUCCESS;
}
