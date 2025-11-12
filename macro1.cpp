#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

// ---------------- Structures ----------------
struct MNTEntry {
    string name;
    int pp_count;
    int kp_count;
    int MDT_index;
    int KP_index;
    int ALA_index;
};

struct MDTEntry {
    string line;
};

// ---------------- Tables ----------------
vector<MNTEntry> MNT;
vector<MDTEntry> MDT;
vector<vector<string>> ALA;
unordered_map<string, string> KPTAB;

int MDT_idx = 0;
int KP_idx = 0;
bool inMacro = false;

// ---------------- Utility Functions ----------------
vector<string> split(const string &line) {
    stringstream ss(line);
    string token;
    vector<string> tokens;
    while (ss >> token) tokens.push_back(token);
    return tokens;
}

// ---------------- Macro Pass 1 ----------------
void processSource(vector<vector<string>> source) {
    int currentALA = -1;

    for (auto line : source) {
        if (line.empty()) continue;
        string opcode = line[0];

        if (opcode == "MACRO") {
            inMacro = true;
            continue;
        }

        if (opcode == "MEND") {
            MDT.push_back({"MEND"});
            MDT_idx++;
            inMacro = false;
            currentALA = -1;
            continue;
        }

        if (inMacro) {
            // --- Macro Header ---
            if (currentALA == -1) {
                string macroName = line[0];
                vector<string> params(line.begin() + 1, line.end());
                vector<string> ala_entry;

                int pp_count = 0, kp_count = 0;
                int currentKPIndex = KP_idx;

                // process parameters
                for (auto &p : params) {
                    if (p.find('=') != string::npos) {
                        size_t pos = p.find('=');
                        string key = p.substr(0, pos);
                        string val = (pos + 1 < p.size()) ? p.substr(pos + 1) : "-1";
                        KPTAB[key] = val;
                        KP_idx++;
                        kp_count++;
                        ala_entry.push_back(key);
                    } else {
                        pp_count++;
                        ala_entry.push_back(p);
                    }
                }

                ALA.push_back(ala_entry);
                currentALA = (int)ALA.size() - 1;

                MNT.push_back({macroName, pp_count, kp_count, MDT_idx, currentKPIndex, currentALA});
            }
            // --- Macro Body ---
            else {
                string processed = "";
                for (auto &tok : line) {
                    string replaced = tok;
                    for (int i = 0; i < (int)ALA[currentALA].size(); i++) {
                        string arg = ALA[currentALA][i];
                        string key = arg;
                        if (arg.find('=') != string::npos)
                            key = arg.substr(0, arg.find('='));
                        if (tok == key) {
                            replaced = "(#" + to_string(i + 1) + ")";
                            break;
                        }
                    }
                    processed += replaced + " ";
                }
                MDT.push_back({processed});
                MDT_idx++;
            }
        }
    }
}

// ---------------- Printing Tables ----------------
void printTables() {
    cout << "\n========== MACRO NAME TABLE (MNT) ==========\n";
    cout << "Idx\tName\tPP_Count\tKP_Count\tMDT_Idx\tKP_Idx\tALA_Idx\n";
    for (int i = 0; i < MNT.size(); i++) {
        cout << i + 1 << "\t" << MNT[i].name << "\t"
             << MNT[i].pp_count << "\t\t" << MNT[i].kp_count << "\t\t"
             << MNT[i].MDT_index + 1 << "\t" << MNT[i].KP_index + 1 << "\t"
             << MNT[i].ALA_index + 1 << "\n";
    }

    cout << "\n========== KEYWORD PARAMETER TABLE (KPTAB) ==========\n";
    int i = 1;
    for (auto &e : KPTAB) {
        cout << i++ << "\t" << e.first << " = " << e.second << "\n";
    }

    cout << "\n========== MACRO DEFINITION TABLE (MDT) ==========\n";
    cout << "Idx\tDefinition\n";
    for (int i = 0; i < MDT.size(); i++)
        cout << i + 1 << "\t" << MDT[i].line << "\n";

    cout << "\n========== ARGUMENT LIST ARRAY (ALA) ==========\n";
    for (int i = 0; i < ALA.size(); i++) {
        cout << "#" << i + 1 << ": ";
        for (int j = 0; j < ALA[i].size(); j++) {
            cout << ALA[i][j];
            if (j + 1 < ALA[i].size()) cout << ", ";
        }
        cout << "\n";
    }
}

// ---------------- Main ----------------
int main() {
    // Example source code (can also be read from a file)
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

    processSource(source);
    printTables();

    return 0;
}