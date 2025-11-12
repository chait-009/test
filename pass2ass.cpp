#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
using namespace std;

struct Symbol { string name; int address; };
struct Literal { string name; int address; };

// Register codes
map<string, int> REGCODE = {
    {"RG,1", 1},
    {"RG,2", 2},
    {"RG,3", 3},
    {"RG,4", 4}
};

vector<string> assemblerPass2(vector<vector<string>> IC, vector<Symbol> SYMTAB, vector<Literal> LITTAB)
{
    vector<string> machineCode;
    int LC = 0;

    for (auto line : IC)
    {
        if (line.empty()) continue;

        string first = line[0];

        // ----- Assembler Directives -----
        if (first.find("(AD") != string::npos)
        {
            if (first.find("01") != string::npos)  // START
            {
                string val = line[1].substr(3, line[1].size() - 4);
                LC = stoi(val);
            }
            else if (first.find("03") != string::npos) // ORIGIN
            {
                string val = line[1].substr(3, line[1].size() - 4);
                LC = stoi(val);
            }
            else if (first.find("04") != string::npos) // EQU
                continue;
            else if (first.find("05") != string::npos) // LTORG or END
                continue;

            continue;
        }

        // ----- Declarative Statements -----
        else if (first.find("(DL") != string::npos)
        {
            string code = first.substr(4, 2);
            string val = line[1].substr(3, line[1].size() - 4);

            if (code == "01") // DC
            {
                machineCode.push_back(to_string(LC++) + "\t" + "00 0 " + val);
            }
            else if (code == "02") // DS
            {
                LC++;
            }
        }

        // ----- Imperative Statements -----
        else if (first.find("(IS") != string::npos)
        {
            string opcode = first.substr(4, 2);
            string reg = "0";
            string mem = "000";

            // Register
            if (line.size() > 1 && line[1].find("(RG") != string::npos)
            {
                string regStr = line[1].substr(1, line[1].size() - 2);
                regStr.erase(remove_if(regStr.begin(), regStr.end(), ::isspace), regStr.end());
                if (REGCODE.count(regStr)) reg = to_string(REGCODE[regStr]);
            }

            // Operand (symbol or literal)
            if (line.size() > 2)
            {
                string op = line[2];
                if (op.find("(S") != string::npos)
                {
                    string idx = op.substr(3, op.size() - 4);
                    int index = stoi(idx);
                    if (index > 0 && index <= SYMTAB.size())
                        mem = to_string(SYMTAB[index - 1].address);
                }
                else if (op.find("(L") != string::npos)
                {
                    string idx = op.substr(3, op.size() - 4);
                    int index = stoi(idx);
                    if (index > 0 && index <= LITTAB.size())
                        mem = to_string(LITTAB[index - 1].address);
                }
            }

            machineCode.push_back(to_string(LC++) + "\t" + opcode + " " + reg + " " + mem);
        }
    }

    return machineCode;
}

void printMachineCode(vector<string> MC)
{
    cout << "\n---- MACHINE CODE ----\n";
    for (auto l : MC) cout << l << "\n";
}

int main()
{
    // ---------- Intermediate Code ----------
    vector<vector<string>> IC = {
        {"(AD,01)", "(C,201)"},
        {"201", "(IS,04)", "(RG, 1)", "(L,1)"},
        {"202", "(IS,05)", "(RG, 1)", "(S,1)"},
        {"203", "(IS,04)", "(RG, 2)", "(L,1)"},
        {"(AD,03)", "(C,206)"},
        {"(AD,05)"},
        {"206", "(DL,02)", "(C,='2')"},
        {"207", "(DL,02)", "(C,='5')"},
        {"208", "(IS,01)", "(RG, 1)", "(L,1)"},
        {"209", "(IS,02)", "(RG, 2)", "(L,2)"},
        {"210", "(IS,07)", "(S,1)"},
        {"(AD,05)"},
        {"211", "(DL,02)", "(C,='1')"},
        {"(AD,04)", "BACK", "=", "203"},
        {"(AD,03)", "(C,213)"},
        {"213", "(IS,03)", "(RG, 3)", "(L,3)"},
        {"214", "(IS,00)"},
        {"(AD,02)"}
    };

    // ---------- Symbol Table ----------
    vector<Symbol> SYMTAB = {
        {"BACK", 203},
        {"DS", 215},
        {"L1", 203},
        {"NEXT", 208},
        {"X", -1}
    };

    // ---------- Literal Table ----------
    vector<Literal> LITTAB = {
        {"='1'", 211},
        {"='2'", 206},
        {"='4'", 215},
        {"='5'", 207}
    };

    // ---------- Run Pass-II ----------
    vector<string> MC = assemblerPass2(IC, SYMTAB, LITTAB);
    printMachineCode(MC);

    return 0;
}
