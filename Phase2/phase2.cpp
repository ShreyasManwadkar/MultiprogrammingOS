#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <algorithm>
using namespace std;

class OS
{
    char M[300][4], IR[4] = {0, 0, 0, 0}, R[4] = {0, 0, 0, 0}, buffer[40];
    vector<int> ranNo;
    int IC, SI, PI, TI, PTR;
    bool C = false, breakFlag = false;
    int IR34;
    string error[9] = {"NO ERROR", "OUT OF DATA", "LINE LIMIT EXCEDDED", "TIME LIMIT EXCEDDED",
                       "OPCODE ERROR", "OPERAND ERROR", "INVALID PAGE FAULT", "TIME LIMIT EXCEDDED WITH OPCODE",
                       "TIME LIMIT EXCEDDED WITH OPERAND"};
    struct pcb
    {
        int job_id, TTL, TLL, TTC, LLC;
        void setJId(int id)
        {
            job_id = id;
        }
        void setTtl(int ttl)
        {
            TTL = ttl;
            TTC = 0;
        }
        void setTll(int tll)
        {
            TLL = tll;
            LLC = 0;
        }
        void reset()
        {
            job_id = 0;
            TTL = 0;
            TLL = 0;
            TTC = 0;
            LLC = 0;
        }
    };
    pcb PCB;

public:
    void INIT();
    void LOAD();
    void STARTEXECUTION();
    void EXECUTEUSERPROGRAM();//INSTRUCTIONS HANDLER
    void MOS();//INTERRUPT HANDLER 
    void READ();
    void WRITE();
    void PRINTMAINMEMORY();
    int ALLOCATE();
    int ADDRESSMAP(int VA, bool valid);
    fstream Input;
    fstream Output;
    void TERMINATE(int e1, int e2 = 0)
    {
        Output << error[e1 + e2] << "\n";
        Output << "JOB ID : " << PCB.job_id << "\n";
        Output << "    IC : " << IC << "\n";
        Output << "    IR : ";
        for (int i = 0; i < 4; i++)
        {
            Output << IR[i];
        }
        Output << "\n     R : ";
        for (int i = 0; i < 4; i++)
        {
            Output << R[i];
        }
        Output << "\n   TTL : " << PCB.TTL << "\n";
        Output << "   TTC : " << PCB.TTC << "\n";
        Output << "   TLL : " << PCB.TLL << "\n";
        Output << "   LLC : " << PCB.LLC << "\n";
        Output << endl;
        Output << endl;
        if (e1 + e2 != 0)
        {
            breakFlag = true;
        }
        SI = 0;
    }
};

void OS::INIT()
{
    for (int i = 0; i < 300; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            M[i][j] = '\0';
        }
    }
    for (int i = 0; i < 4; i++)
    {
        IR[i] = '\0';
        R[i] = '\0';
    }
    SI = 0;
    PI = 0;
    TI = 0;
    C = false;
    breakFlag = false;
    PCB.reset();
    ranNo.clear();
}

void OS::PRINTMAINMEMORY()
{
    for (int i = 0; i < 300; i++)
    {
        cout << "M [" << i << "] \t" << M[i][0] << M[i][1] << M[i][2] << M[i][3] << endl;
    }
    cout << endl
         << endl;
}

void OS::LOAD()
{
    cout << "Fetching Jobs ...." << endl;
    do
    {
        for (int i = 0; i <= 39; i++)
            buffer[i] = '\0';

        Input.getline(buffer, 41);

        string buffer4 = "";
        for (int i = 0; i < 4; i++)
            buffer4 += buffer[i];

        if (buffer4 == "$AMJ")
        {
            INIT();
            string jid = "", ttl = "", tll = "";
            for (int i = 4; i < 8; i++)
            {
                jid += buffer[i];
                ttl += buffer[i + 4];
                tll += buffer[i + 8];
            }
            PCB.setJId(stoi(jid));
            PCB.setTtl(stoi(ttl));
            PCB.setTll(stoi(tll));
            PTR = ALLOCATE() * 10;
            for (int i = PTR; i < PTR + 10; i++)
            {
                M[i][0] = '0';
                M[i][1] = ' ';
                M[i][2] = '*';
                M[i][3] = '*';
            }
        }
        else if (buffer4 == "$DTA")
        {
            STARTEXECUTION();
        }
        else if (buffer4 == "$END")
        {
            PRINTMAINMEMORY();
            continue;
        }
        else
        {
            if (breakFlag)
            {
                continue;
            }

            int k = 0;
            int x = ALLOCATE() * 10;
            for (int i = PTR; i < PTR + 10; i++)
            {
                if (M[i][0] == '0')
                {
                    M[i][0] = '1';
                    M[i][2] = char((x / 100) + 48);
                    M[i][3] = char(((x / 10) % 10) + 48);
                    break;
                }
            }
            for (int i = x; i < x + 10; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    M[i][j] = buffer[k];
                    k++;
                }
                if (k == 40 || buffer[k] == ' ' || buffer[k] == '\n')
                {
                    break;
                }
            }
        }

    } while (!Input.eof());
}

void OS::STARTEXECUTION()
{
    IC = 00;
    EXECUTEUSERPROGRAM();
}

void OS::EXECUTEUSERPROGRAM()
{
    while (true)
    {
        if (PCB.TTC >= PCB.TTL)
        {
            TI = 2;
        }

        if (breakFlag)
        {
            break;
        }

        int ra = ADDRESSMAP(IC, false);
        for (int i = 0; i < 4; i++)
        {
            IR[i] = M[ra][i];
        }
        IC++;

        string IR2 = "";
        IR2 += IR[0];
        IR2 += IR[1];

        IR34 = (IR[2] - 48) * 10 + IR[3] - 48;
        if (!(isdigit(IR[2]) && isdigit(IR[3])))
        {
            if (!IR2.find('H') == 0)
            {
                PI = 2;
                MOS();
                return;
            }
        }

        if (IR2 == "GD")
        {
            SI = 1;
            MOS();
        }
        else if (IR2 == "PD")
        {
            SI = 2;
            MOS();
        }
        else if (IR2.find('H') == 0)
        {
            PCB.TTC++;
            SI = 3;
            MOS();
            break;
        }
        else if (IR2 == "LR")
        {
            if (TI == 2)
            {
                MOS();
                return;
            }

            int ir3 = (IR[2] - 48) * 10;
            int ir34 = ADDRESSMAP(ir3, false);
            if (ir34 == -1)
            {
                PI = 3;
                MOS();
                return;
            }
            ir34 += (IR[3] - 48);

            for (int j = 0; j <= 3; j++)
                R[j] = M[ir34][j];
        }
        else if (IR2 == "SR")
        {
            if (TI == 2)
            {
                MOS();
                return;
            }

            int ir3 = (IR[2] - 48) * 10;
            int ir34 = ADDRESSMAP(ir3, true);
            ir34 += (IR[3] - 48);
            for (int j = 0; j <= 3; j++)
                M[ir34][j] = R[j];
        }
        else if (IR2 == "CR")
        {
            if (TI == 2)
            {
                MOS();
                return;
            }

            int ir3 = (IR[2] - 48) * 10;
            int ir34 = ADDRESSMAP(ir3, false);
            if (ir34 == -1)
            {
                PI = 3;
                MOS();
                return;
            }
            ir34 += (IR[3] - 48);

            int count = 0;
            for (int j = 0; j <= 3; j++)
                if (M[ir34][j] == R[j])
                    count++;
            if (count == 4)
                C = true;
        }
        else if (IR2 == "BT")
        {
            if (TI == 2)
            {
                MOS();
                return;
            }

            if (C == true)
            {
                IC = IR34;
            }
        }
        else
        {
            PI = 1;
            MOS();
        }
        PCB.TTC++;
    }
}

void OS::MOS()
{
    switch (SI)
    {
    case 1:
        if (TI == 2)
        {
            TERMINATE(3);
        }
        else
        {
            READ();
        }
        break;
    case 2:
        WRITE();
        if (TI == 2)
        {
            PCB.TTC++;
            TERMINATE(3);
            return;
        }
        break;
    case 3:
        TERMINATE(0);
        break;
    }

    switch (PI)
    {
    case 1:
        if (TI == 2)
        {
            TERMINATE(3, 4);
        }
        else
        {
            TERMINATE(4);
        }
        break;
    case 2:
        if (TI == 2)
        {
            TERMINATE(3, 5);
        }
        else
        {
            TERMINATE(5);
        }
        break;
    case 3:
        if (TI == 2)
        {
            TERMINATE(3);
        }
        else
        {
            TERMINATE(6);
        }
        break;
    }

    if (SI == 0 && PI == 0 && TI == 2)
    {
        TERMINATE(3);
    }
}

void OS::READ()
{
    for (int i = 0; i <= 39; i++)
        buffer[i] = '\0';

    Input.getline(buffer, 40);

    if (buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D')
    {
        TERMINATE(1);
    }

    int ir34 = ADDRESSMAP(IR34, true);

    for (int l = 0; l < 10; l++)
    {
        for (int j = 0; j < 4; j++)
        {
            M[ir34][j] = buffer[(l * 4) + j];
        }
        ir34++;
    }

    SI = 0;
}

void OS::WRITE()
{
    if (PCB.LLC >= PCB.TLL)
    {
        PCB.TTC++;
        TERMINATE(2);
    }
    else
    {
        for (int i = 0; i <= 39; i++)
        {
            buffer[i] = '\0';
        }

        int ir34 = ADDRESSMAP(IR34, false);
        if (ir34 == -1)
        {
            PI = 3;
            SI = 0;
            return;
        }

        for (int l = 0; l < 10; l++)
        {
            for (int j = 0; j < 4; j++)
            {
                buffer[(l * 4) + j] = M[ir34][j];
                Output << buffer[(l * 4) + j];
            }
            ir34++;
        }
        Output << "\n";
        PCB.LLC++;
    }
    SI = 0;
}

int OS::ALLOCATE()
{
    // get frame for page table
start:
    int a = ((rand() % 10) % 3) * 10 + (rand() % 10);
    if (!count(ranNo.begin(), ranNo.end(), a))
    {
        ranNo.push_back(a);
    }
    else
    {
        goto start;
    }
    // cout << "random : " << a << endl;
    return a;
}

int OS::ADDRESSMAP(int VA, bool valid)
{
    if (VA >= 0 && VA < 100)
    {
        if (VA / 10 >= 2)
        {
            for (int i = PTR; i < PTR + 10; i++)
            {
                if (M[i][0] == char((VA / 10) + 48))
                {
                    string a = "";
                    a += M[i][2];
                    a += M[i][3];
                    int frame = stoi(a);
                    frame *= 10;
                    return (frame + (VA % 10));
                }
            }
        }
        if (valid)
        {
            int x = ALLOCATE() * 10;
            for (int i = PTR; i < PTR + 10; i++)
            {
                if (M[i][0] == '0')
                {
                    M[i][0] = char((VA / 10) + 48);
                    M[i][2] = char((x / 100) + 48);
                    M[i][3] = char(((x / 10) % 10) + 48);
                    return x;
                }
            }
        }
        else
        {
            string a = "";
            a += M[PTR + (VA / 10)][2];
            a += M[PTR + (VA / 10)][3];
            if (a == "**")
            {
                return -1;
            }
            int frame = stoi(a);
            frame *= 10;
            return (frame + (VA % 10));
        }
    }
}

int main()
{
    OS os;

    os.Input.open("input2.txt", ios::in);
    os.Output.open("output2.txt", ios::out);

    if (!os.Input)
    {
        cout << "Fail To Open File !! " << endl;
    }
    else
    {
        cout << "File Exist" << endl;
    }
    os.LOAD();
    cout << "\nExecution Success !!" << endl;

    return 0;
}
