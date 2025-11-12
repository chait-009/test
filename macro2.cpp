#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

struct MNTEntry
{
    string name;
    int mdtIndex;
    int alaIndex;
};

struct MDTEntry {
    string line;
};

vector<MNTEntry> MNT;
vector<MDTEntry> MDT;
vector<vector<string>> ALA;


vector<vector<string>> macroPass_2(vector<vector<string>> IC)
{
    vector<vector<string>> expandedCode;

    for(auto line : IC)
    {
        if (line.empty()) continue;

        string opcode = line[0];
        bool isMacro = false;
        int mntIndex = -1;

        // check if macro call
        for (int i = 0; i < MNT.size(); i++) {
            if (MNT[i].name == opcode) {
                isMacro = true;
                mntIndex = i;
                break;
            }
        }

        if (!isMacro) expandedCode.push_back(line);
        else
        {
            int mdtIndex = MNT[mntIndex].mdtIndex;
            int alaIndex = MNT[mntIndex].alaIndex;

            // 1. Build local ALA with actual arguments
            vector<string> actualArgs(line.begin() + 1, line.end());
            vector<string> localALA = ALA[alaIndex];

            for (int i = 0; i < localALA.size(); i++) {
                string arg = localALA[i];
                string def = "";
                if (arg.find('=') != string::npos) {
                    def = arg.substr(arg.find('=') + 1);
                    arg = arg.substr(0, arg.find('='));
                }

                if (i < actualArgs.size())
                    localALA[i] = actualArgs[i];
                else if (!def.empty())
                    localALA[i] = def;
                else
                    localALA[i] = "???";
            }

            // 2. expand from MDT
            for (int i = mdtIndex; i < MDT.size(); i++) {
                if (MDT[i].line == "MEND") break;

                string expandedLine = MDT[i].line;
                for (int j = 0; j < localALA.size(); j++)
                {
                    string key = "(#" + to_string(j + 1) + ")";
                    size_t pos;
                    while ((pos = expandedLine.find(key)) != string::npos)
                        expandedLine.replace(pos, key.size(), localALA[j]);
                }

                // Split expanded line into tokens and add to output
                stringstream ss(expandedLine);
                string token;
                vector<string> tokens;
                while (ss >> token) tokens.push_back(token);
                expandedCode.push_back(tokens);
            }
        }
    }
    return expandedCode;
}

int main()
{
    // MNT from Pass-I
    MNT = {
        {"INCR", 0, 0},
        {"DECR", 4, 1},
        {"SQUARE", 8, 2}
    };

    // MDT from Pass-I
    MDT = {
        {"MOVER (#3) (#1)"},
        {"ADD (#3) (#2)"},
        {"MOVEM (#3) (#1)"},
        {"MEND"},
        {"MOVER (#3) (#1)"},
        {"SUB (#3) (#2)"},
        {"MOVEM (#3) (#1)"},
        {"MEND"},
        {"MOVER (#2) (#1)"},
        {"MULT (#2) (#1)"},
        {"MOVEM (#2) (#1)"},
        {"MEND"}
    };

    // ALA from Pass-I
    ALA = {
        {"&A", "&B", "&REG=AREG"},
        {"&X", "&Y", "&R=DREG"},
        {"&NUM", "&TEMP=CREG"}
    };

    // Intermediate code (no macro definitions)
    vector<vector<string>> intermediate = {
        {"START", "100"},
        {"READ", "A"},
        {"INCR", "A", "B"},
        {"DECR", "B", "C"},
        {"SQUARE", "X"},
        {"STOP"},
        {"END"}
    };

    vector<vector<string>> result =  macroPass_2(intermediate);

    for(auto line : result)
    {
        for (auto word : line)
            cout << word << " ";
        cout << '\n';
    }

    return 0;
}