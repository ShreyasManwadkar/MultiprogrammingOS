#include <bits/stdc++.h>
using namespace std;

class VirtualMachine
{
private:
    char Memory[100][4]; // Memory
    unsigned char R[4];  // Register
    unsigned char IR[4]; // Instruction Registers
    unsigned int IC;
    bool C;
    int SI;
    char buffer[200];

public:
    void INIT();
    void LOAD();
    void ExecuteUserProgram();
    void MOS();
    void StartExecution();
    int OperandAddress();
    void Read();
    void Write();
    void Terminate();

    fstream inputfile;
    fstream outputfile;
};

int main()
{
    unsigned x;
    VirtualMachine vm;

    vm.inputfile.open("Input.txt", ios::in);
    vm.outputfile.open("Output.txt", ios::out);

    if (!vm.inputfile)
        cout << "File doesn't exist" << endl;
    else
        cout << "File Exist" << endl;

    vm.LOAD();
    vm.inputfile.close();
    vm.outputfile.close();
    return 0;
}

void VirtualMachine ::INIT()
{

    for (int i = 0; i < 100; i++)
        for (int j = 0; j < 4; j++)
            Memory[i][j] = ' ';

    IR[3] = ' ';
    R[3] = ' ';
    C = false;
    IC = 0;
}

void VirtualMachine ::LOAD()
{
    cout << "Reading Data..." << endl;
    int x = 0;
    for (int i = 0; i < 100; i++)
        for (int j = 0; j < 4; j++) // clear buffer
            Memory[i][j] = ' ';
    do
    {

        for (int i = 0; i <= 100; i++) // clear buffer
            buffer[i] = ' ';

        inputfile.getline(buffer, 42);

        if (buffer[0] == '$' && buffer[1] == 'A' && buffer[2] == 'M' && buffer[3] == 'J')
        {
            INIT();
        }
        else if (buffer[0] == '$' && buffer[1] == 'D' && buffer[2] == 'T' && buffer[3] == 'A')
        {
            StartExecution();
        }
        else if (buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D')
        {
            x = 0;
            for (int i = 0; i < 100; i++)
            {
                cout << "Memory[" << i << "]\t";
                for (int j = 0; j < 4; j++)
                {
                    cout << Memory[i][j];
                }
                cout << endl;
                if (i % 10 == 9)
                    cout << endl;
            }
            continue;
        }
        else
        {
            int k = 0;
            // if(x==100) abort();
            int limit = x + 10;
            for (; x < limit; ++x)
            {
                for (int j = 0; j < 4; ++j)
                {
                    Memory[x][j] = buffer[k];
                    k++;
                }
                if (buffer[k] == ' ' || buffer[k] == '\n')
                {
                    break;
                }
            }
        }
    } while (!inputfile.eof());
}

int VirtualMachine::OperandAddress()
{
    int add = IR[2] - '0';
    add = add * 10 + (IR[3] - '0');
    return add;
}
void VirtualMachine::StartExecution()
{
    IC = 00;
    ExecuteUserProgram();
}

void VirtualMachine::ExecuteUserProgram()
{
    while (true)
    {
        for (int i = 0; i < 4; i++) // Load in register
        {
            IR[i] = Memory[IC][i];
        }
        IC++;

        int loc = OperandAddress();

        if (IR[0] == 'G' && IR[1] == 'D') // GD
        {
            SI = 1;
            MOS();
        }
        else if (IR[0] == 'P' && IR[1] == 'D') // PD
        {
            SI = 2;
            MOS();
        }
        else if (IR[0] == 'H') // H
        {
            SI = 3;
            MOS();
            break;
        }
        else if (IR[0] == 'L' && IR[1] == 'R') // LR
        {

            for (int j = 0; j < 4; j++)
                R[j] = Memory[loc][j];
        }
        else if (IR[0] == 'S' && IR[1] == 'R') // SR
        {

            for (int j = 0; j < 4; j++)
                Memory[loc][j] = R[j];
        }
        else if (IR[0] == 'C' && IR[1] == 'R') // CR
        {
            int count = 0;

            for (int j = 0; j <= 3; j++)
                if (Memory[loc][j] == R[j])
                    count++;

            if (count == 4)
                C = true;
            else
                C = false;
        }
        else if (IR[0] == 'B' && IR[1] == 'T') // BT
        {
            if (C == true)
            {
                IC = loc;
                C = false;
            }
            // else
            //     IC++;
        }
    }
}

void VirtualMachine::MOS()
{
    switch (SI)
    {
    case 1: // Read Mode
        for (int i = 0; i <= 40; i++)
            buffer[i] = '\0';
        Read();
        break;

    case 2:
        Write(); // write
        break;

    case 3: // Terminate

        Terminate();
        break;
    default:
        break;
    }
}

void VirtualMachine::Read()
{
    inputfile.getline(buffer, 42);
    IR[3] = '0';
    int k = 0;
    int loc = OperandAddress();

    for (int l = 0; l < 10; ++l)
    {
        for (int j = 0; j < 4; ++j)
        {
            Memory[loc][j] = buffer[k];
            k++;
        }
        if (Memory[loc][0] == '$' && Memory[loc][1] == 'E' && Memory[loc][2] == 'N' && Memory[loc][3] == 'D')
            return;

        loc++;
    }
}

void VirtualMachine::Write()
{
    int k = 0;
    IR[3] = '0';
    int loc = OperandAddress();
    int i = 0;

    for (int l = 0; l < 10; ++l)
    {
        for (int j = 0; j < 4; ++j)
        {
            outputfile << Memory[loc][j];
            k++;
        }
        loc++;
    }

    outputfile << "\n";
}
void VirtualMachine::Terminate()
{
    outputfile << "\n\n";
}