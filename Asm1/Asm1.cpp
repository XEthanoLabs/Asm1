#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <filesystem>

using namespace std;

vector<byte> binCode;
uint64_t PC = 0;

// be able to look up the unique ID for each opcode string
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

// define this just to make it obvious what is going on inside switch statements.
// but, keep this in sync with the above list
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

// duh
bool iswhitespace(char c)
{
    if (c == ' ' || c == '\t' )
    {
        return true;
    }
    return false;
}

// read in all lines from the assembly code and turn them into a vector of strings. 
// while we're all it, remove all comments, and leading and trailing whitespace
//
vector<string> ReadInLines(ifstream& s)
{
    // read all the lines into a vector
    vector<string> szLines;
    while (!s.eof())
    {
        // read the line
        char buf[256] = { 0 };
        s.getline(buf, 256);

        // if it's a blank line, skip it
        if (buf[0] == 0)
            continue;

        // all upper case. This wouldn't be true in C++/etc
        int l = strlen(buf);
        for (int i = 0; i < l; i++)
        {
            buf[i] = toupper(buf[i]);
        }

        // make a string out of it. dunno why.
        string sBuf(buf);

        // strip after first ; we find. Them's just comments
        int index = sBuf.find(';');
        if (index >= 0)
        {
            sBuf = sBuf.substr(0, index);
        }

        // strip any trailing whitespace
        while (sBuf.size() > 0 && iswhitespace(sBuf[sBuf.size() - 1]))
        {
            sBuf = sBuf.substr(0, sBuf.size() - 1);
        }

        // if it ended up nothing, skip it
        if (sBuf.size() == 0)
        {
            continue;
        }

        // add it
        szLines.push_back(sBuf);
    }

    s.close();

    // return the vector of code lines
    return szLines;
}

// split an input string into separate words. Takes into account any character in szDelims will cause a break
vector<string> splitString(string s, const string& szDelims)
{
    vector<string> szSplits;
    size_t pos = 0;
    while (
        (pos = s.find_first_of(szDelims)) != std::string::npos
        ) 
    {
        string token = s.substr(0, pos);

        if (token.size() > 0)
        {
            szSplits.push_back(token);
        }

        s.erase(0, pos + 1);
    }
    if (s.size() > 0)
    {
        szSplits.push_back(s);
    }
    return szSplits;
}

//=================== individual opcodes we can assemble =============================

bool Opcode_Nop()
{
    // push a NOP opcode
    binCode.push_back((byte) OP_NOP);
    PC++;
    return true;
}

bool Opcode_LoadAcc( const vector<string>& szWords )
{
    // this opcode requires an additional byte, the value to load into the ACC
    byte val = (byte) atoi(szWords[1].c_str());
    binCode.push_back((byte) OP_LOAD_ACC);
    binCode.push_back(val);
    PC += 2; // the value to load is only a byte, so PC gets added 2
    return true;
}

bool Opcode_AccInc()
{
    binCode.push_back((byte)OP_ACC_INC);
    PC++;
    return true;
}

bool Opcode_AccDec()
{
    binCode.push_back((byte)OP_ACC_DEC);
    PC++;
    return true;
}

bool Opcode_Ret()
{
    binCode.push_back((byte)OP_RET);
    PC++;
    return true;
}

bool Opcode_Sleep(const vector<string>& szWords)
{
    byte val = (byte)atoi(szWords[1].c_str());
    binCode.push_back((byte)OP_SLEEP);
    binCode.push_back(val);
    PC += 2;
    return true;
}

bool Opcode_PrintAcc()
{
    binCode.push_back((byte)OP_PRINT_ACC);
    PC++;
    return true;
}

//=================== here's the main parser ===================================

// go through and parse all lines, keeping track of where we are in terms of the PC, because
// if we were more advanced, we might want to know how to JMP (jump) to a certain point in the code.
// (we don't do that right now)

bool ParseLines(vector<string>& szLines)
{
    // no 'sections' (data, code, rom) for now, just straight opcodes

    for (const string& szLine : szLines)
    {
        // split this line of assembly into words
        vector<string> szWords = splitString(szLine, " ,");

        // the first word is always the opcode
        string szOpcode = szWords[0];

        // for this supposed opcode, go find the unique ID for it.
        // if it's not found, we have a syntax error!
        map<string, int>::iterator i = opcodes.find(szOpcode);
        if (i == opcodes.end())
        {
            cout << "Opcode not found: " << szOpcode << endl;
            return false;
        }

        // the map returns the unique ID in the "second" data member of i
        int nOpcode = i->second; // map holds the opcode value.

        // go see what to do based on the opcode. it determines how many bytes we need to add to the binCode vector

        switch (nOpcode)
        {
        case OP_NOP:
            Opcode_Nop();
            break;
        case OP_LOAD_ACC:
            // has an extra arg in it
            Opcode_LoadAcc(szWords);
            break;
        case OP_SLEEP:
            Opcode_Sleep(szWords);
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
            break;
        }
    }

    // by this time, binCode is filled and we need to go write it out

    return true;
}

//=================== MAIN ===================================

int main( int argc, const char* argv[] )
{
    if (argc != 2)
    {
        cout << "Usage: asm1 filename" << endl;
        return 0;
    }

    ifstream s(argv[1]);
    if (!s.is_open())
    {
        cout << "could not open " << argv[1] << endl;
        return -1;
    }

    vector<string> szLines = ReadInLines(s);
    if (szLines.size() == 0)
    {
        cout << "file doesn't contain compilable code!" << endl;
        return 0;
    }

    bool bParsed = ParseLines(szLines);
    if (!bParsed)
    {
        cout << "Got a syntax error. Not writing out .bin file" << endl;
        return -1;
    }

    // figure out output name
    filesystem::path p(argv[1]);
    filesystem::path ext(".bin");

    // replace the .txt extension with .bin
    p.replace_extension(ext);

    // this is the output file we want to write to
    string szBinOut = p.string();

    // write out all the binary opcodes and data
    ofstream pBinFile(szBinOut);
    for (byte b : binCode)
    {
        pBinFile << (char)b;
    }
    pBinFile.close();

    cout << "compiled: 0 errors" << endl;

    int stop = 0;
}

