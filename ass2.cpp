#include <bits/stdc++.h>
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

    for (auto line : IC)
    {
        // Skip empty lines or lines with less than 2 elements
        if (line.empty() || line.size() < 2 || line[0].find("(AD")!=string::npos) continue;
        
        string ans="";
        if(isdigit(line[0][0])) ans+=line[0];
        
        // Check if it's a DL statement
        if(line.size() >= 3 && line[1]==("(DL,02)")) {
            ans+=(" 00 00 ");
            string str= line[2].substr(3,4);
            for(auto p:LITTAB){
                if(p.name==str){
                    str=to_string(p.address);
                    break;
                }
            }
            ans+=str;
            machineCode.push_back(ans);
            continue;
        }
        
        // Check if line has enough elements for IS statement
        if(line.size() < 4) continue;
        
        // Process IS statement
        ans+=" ";
        string str=line[1].substr(4,2);  // Get opcode (2 digits)
        ans+=str+" ";
        ans+=line[2]+" ";  // Register code
        
        if(line[3].find("(L")!=string::npos){
            string str=line[3].substr(3,1);
            int idx=stoi(str);
            int addr=LITTAB[idx-1].address;
            ans+=to_string(addr);
        }
        else if(line[3].find("(S")!=string::npos){
            string str=line[3].substr(3,1);
            int idx=stoi(str);
            int addr=SYMTAB[idx-1].address;
            ans+=to_string(addr);
        }
        else {
            ans+="0";  // No operand
        }
        
        machineCode.push_back(ans);
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
        {"201", "(IS,04)", "01", "(L,1)"},
        {"202", "(IS,05)", "01", "(S,1)"},
        {"203", "(IS,04)", "02", "(L,1)"},
        {"(AD,03)", "(C,206)"},
        {"(AD,05)"},
        {"206", "(DL,02)", "(C,='2')"},
        {"207", "(DL,02)", "(C,='5')"},
        {"208", "(IS,01)", "01", "(L,1)"},
        {"209", "(IS,02)", "02", "(L,2)"},
        {"210", "(IS,07)", "(S,1)"},
        {"(AD,05)"},
        {"211", "(DL,02)", "(C,='1')"},
        {"(AD,04)", "BACK", "=", "203"},
        {"(AD,03)", "(C,213)"},
        {"213", "(IS,03)", "03", "(L,3)"},
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
