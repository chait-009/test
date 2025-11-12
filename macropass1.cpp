#include <iostream>
#include <vector>
#include <string>
using namespace std;

struct MNTEntry{
    string name;
    int mdtIndex;
    int alaIndex;
};

struct MDTEntry{
    string line;
};

vector<MNTEntry> MNT;
vector<MDTEntry> MDT;
vector<vector<string>> ALA;

void printTables() {
    cout << "\n--- MACRO NAME TABLE (MNT) ---\n";
    cout << "Idx\tName\tMDT_Index\tALA_Index\n";
    for (int i = 0; i < MNT.size(); i++)
        cout << i + 1 << "\t" << MNT[i].name << "\t" << MNT[i].mdtIndex + 1
             << "\t\t" << MNT[i].alaIndex + 1 << "\n";

    cout << "\n--- MACRO DEFINITION TABLE (MDT) ---\n";
    cout << "Idx\tDefinition\n";
    for (int i = 0; i < MDT.size(); i++)
        cout << i + 1 << "\t" << MDT[i].line << "\n";

    cout << "\n--- ARGUMENT LIST ARRAY (ALA) ---\n";
    for (int i = 0; i < ALA.size(); i++) {
        cout << "#" << i + 1 << ": ";
        for (int j = 0; j < ALA[i].size(); j++) {
            cout << ALA[i][j];
            if (j + 1 < ALA[i].size()) cout << ", ";
        }
        cout << "\n";
    }
}

void macroPass_1(vector<vector<string>> source)
{
    bool inMacro = false;
    int currentALA = -1;

    for (auto line : source)
    {
        if (line.empty()) continue;

        string opcode = line[0];

        if (opcode == "MACRO")
        {
            inMacro = true;
            continue;
        }

        if (opcode == "MEND")
        {
            MDT.push_back({"MEND"});
            inMacro = false;
            currentALA = -1;
            continue;
        }

        if (inMacro)
        {
            // first line after macro -- macro header
            if (currentALA == -1)
            {
                string macroName = line[0];
                vector<string> args(line.begin()+1, line.end());
                ALA.push_back(args);
                MNT.push_back({macroName, (int)MDT.size(), (int)ALA.size() -1});
                currentALA = (int)ALA.size() - 1;
            }
            else
            {
                // macro body: replace &args with (#index)
                string processed = "";
                for (auto tok: line)
                {
                    string replaced = tok;
                    for(int i=0; i<ALA[currentALA].size(); i++)
                    {
                        string arg = ALA[currentALA][i];
                        if (tok == arg || (arg.find('=') != string::npos && tok == arg.substr(0, arg.find('='))))
                        {
                            replaced = "(#" + to_string(i + 1) + ")";
                            break;
                        }
                    }
                    processed += replaced + " ";
                }
                MDT.push_back({processed});
            }
        }
    }
}

int main() {
    vector<vector<string>> source = {
        {"MACRO"},
        {"INCR", "&A", "&B", "&REG=AREG"},
        {"MOVER", "&REG", "&A"},
        {"ADD", "&REG", "&B"},
        {"MOVEM", "&REG", "&A"},
        {"MEND"},
        {"MACRO"},
        {"DECR", "&X", "&Y", "&R=DREG"},
        {"MOVER", "&R", "&X"},
        {"SUB", "&R", "&Y"},
        {"MOVEM", "&R", "&X"},
        {"MEND"},
        {"MACRO"},
        {"SQUARE", "&NUM", "&TEMP=CREG"},
        {"MOVER", "&TEMP", "&NUM"},
        {"MULT", "&TEMP", "&NUM"},
        {"MOVEM", "&TEMP", "&NUM"},
        {"MEND"}
    };

    macroPass_1(source);
    printTables();

    return 0;
}