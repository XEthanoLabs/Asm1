#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <filesystem>
#include <Windows.h> // import "sleep"

using namespace std;
using ubyte = unsigned char;

// this is the execution code we load in from the file. Consider this the ROM code for our program
vector<ubyte> binCode;

// these are the registers that belong to the CPU. They are implemented in hardware. We simply emulate them.
// PC is 64 bits because we assume we have a 64 bit address bus. Which is different from the data bus width.
uint64_t PC = 0;
// we assume the data bus width for this example is just 8 bits! That's why it's an unsigned byte.
ubyte ACC = 0;

// same opcodes that are in Asm1, belong in here. Keep this list synced.
map<string, int> opcodes =
{
    { "NOP", 0 },
    { "SLEEP", 1 },
    { "PRINT_ACC", 2 },
    { "LOAD_ACC", 3 },
    { "ACC++", 4 },
    { "ACC--", 5 },
    { "RET", 100 }
};

enum
{
    OP_NOP = 0,
    OP_SLEEP = 1,
    OP_PRINT_ACC = 2,
    OP_LOAD_ACC = 3,
    OP_ACC_INC = 4,
    OP_ACC_DEC = 5,
    OP_RET = 100
};

void Opcode_Nop()
{
}

void Opcode_LoadAcc()
{
    // geez this is dead simple. Keep in mind we're just emulating what the hardware does.
    // the hardware would be stuffing a value into the ACC transistors, when the clock signal goes high.
    // the clock would go low, the values on the data bus would be flowing into the ACC transistors
    // and then when the clock goes high, the ACC transistors would "latch" (retain, store) the data line's 
    // values permanently. This is how we load a hardware "register"

    ubyte val = binCode[PC++];
    ACC = val;
}

void Opcode_AccInc()
{
    // in this case, when the clock goes low, there are hardware transistors that calculate the value of ACC+1.
    // These calculated values take a while for the electrons to get through the transistors. Eventually the 
    // correct value flows to the data lines. When the clock goes high, the ACC's contents are now set with the
    // new value of ACC+1!

    ACC++;
}

void Opcode_AccDec()
{
    ACC--;
}

void Opcode_Ret()
{
    // we bail out of the VM when we see any RET. This is normally not true, this will normally
    // affect the SP register, which we don't have right now.
}

void Opcode_Sleep()
{
    // this wouldn't normally be a real opcode.
    ubyte val = binCode[PC++];
    Sleep(val * 1000);
}

void Opcode_PrintAcc()
{
    // this wouldn't normally be a real opcode.
    cout << "ACC=" << (int) ACC << endl;
}

bool VirtualMachine()
{
    PC = 0;

    bool bDone = false;

    while (!bDone)
    {
        // this simulates the CPU prefetching the next instruction that is at the current program counter, PC.
        // that opcode tells the CPU what to do next.

        int opCode = (int) binCode[PC++]; // always increment the PC at least by 1. The opcode determines how many more bytes PC is advanced.


        switch (opCode)
        {
        case OP_NOP:
            Opcode_Nop();
            break;
        case OP_LOAD_ACC:
            // has an extra arg in it
            Opcode_LoadAcc();
            break;
        case OP_SLEEP:
            Opcode_Sleep();
            break;
        case OP_PRINT_ACC:
            Opcode_PrintAcc();
            break;
        case OP_ACC_INC:
            Opcode_AccInc();
            break;
        case OP_ACC_DEC:
            Opcode_AccDec();
            break;
        case OP_RET:
            Opcode_Ret();
            bDone = true;
            break;
        }
    }

    return true;
}

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        cout << "Usage: vm1 filename" << endl;
        return 0;
    }

    if (!filesystem::exists(argv[1]))
    {
        cout << "File " << argv[1] << " doesn't exist!" << endl;
        return -1;
    }

    uintmax_t size = filesystem::file_size(argv[1]);
    binCode.resize(size);

    ifstream s(argv[1]);
    if (!s.is_open())
    {
        cout << "could not open " << argv[1] << endl;
        return -1;
    }

    s.read((char*) binCode.data(), size);

    // start the VM
    VirtualMachine();

    cout << "finished executing." << endl;
}

    