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

// ---------------- Tables (shared between passes) ----------------
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
void processSource(const vector<vector<string>> &source) {
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
                        // store defaults in KPTAB keyed by the parameter name (keep leading & if present)
                        KPTAB[key] = val;
                        KP_idx++;
                        kp_count++;
                        // push only the parameter name (without the '=default' part)
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
                        string arg = ALA[currentALA][i]; // argument name as stored (maybe starts with &)
                        string key = arg;
                        if (arg.find('=') != string::npos)
                            key = arg.substr(0, arg.find('='));
                        // if token equals the parameter name, replace with placeholder
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

// ---------------- Macro Pass 2 (updated) ----------------
vector<vector<string>> macroPass_2(const vector<vector<string>> &IC) {
    vector<vector<string>> expandedCode;

    for (auto line : IC) {
        if (line.empty()) continue;

        string opcode = line[0];
        bool isMacro = false;
        int mntIndex = -1;

        // find macro in MNT
        for (int i = 0; i < (int)MNT.size(); i++) {
            if (MNT[i].name == opcode) {
                isMacro = true;
                mntIndex = i;
                break;
            }
        }

        if (!isMacro) {
            // not a macro call -> copy as is
            expandedCode.push_back(line);
        } else {
            const MNTEntry &mnt = MNT[mntIndex];
            int mdtIndex = mnt.MDT_index;
            int alaIndex = mnt.ALA_index;
            int pp = mnt.pp_count;
            int kp = mnt.kp_count;
            int totalParams = pp + kp;

            // collect actual args from call (tokens after macro name)
            vector<string> actualTokens(line.begin() + 1, line.end());

            // Build local ALA values (strings) initialized to empty
            vector<string> localValues(totalParams, "");

            // 1) Separate positional and keyword actuals.
            //    Positional: tokens without '=' consumed left-to-right up to pp.
            //    Keywords: tokens with '=' (form key=value)
            vector<pair<string,string>> kwActuals; // (key, value)
            int posFilled = 0;

            for (auto &tok : actualTokens) {
                size_t eqpos = tok.find('=');
                if (eqpos != string::npos) {
                    string key = tok.substr(0, eqpos);
                    string val = tok.substr(eqpos + 1);
                    kwActuals.push_back({key, val});
                } else {
                    if (posFilled < pp) {
                        localValues[posFilled] = tok;
                        posFilled++;
                    } else {
                        // If more non-keyword tokens appear after pp are exhausted,
                        // treat them as additional positional (fall-back).
                        if (posFilled < totalParams) localValues[posFilled++] = tok;
                    }
                }
            }

            // 2) Apply keyword actuals: match key with ALA param names.
            //    ALA stores parameter names as they were in header (e.g., "&REG" or "&R").
            //    Keyword in call may be given without '&' (e.g., REG=AREG) or with '&'.
            auto stripAmp = [&](const string &s)->string {
                if (!s.empty() && s[0] == '&') return s.substr(1);
                return s;
            };

            for (auto &kv : kwActuals) {
                string key = kv.first;
                string val = kv.second;
                // try to match against ALA params
                bool matched = false;
                for (int i = 0; i < totalParams; ++i) {
                    string paramName = ALA[alaIndex][i]; // e.g. "&REG" or "&A"
                    if (stripAmp(paramName) == stripAmp(key) || paramName == key || ("&"+key) == paramName) {
                        localValues[i] = val;
                        matched = true;
                        break;
                    }
                }
                if (!matched) {
                    // unknown keyword - still store by key (no parameter matched)
                    // (could warn; for now ignore)
                }
            }

            // 3) Fill remaining empty params from KPTAB defaults (if keyword param)
            for (int i = 0; i < totalParams; ++i) {
                if (!localValues[i].empty()) continue; // already filled by positional or keyword

                string paramName = ALA[alaIndex][i]; // may have &

                // look for a default in KPTAB (key stored in pass1 with the exact paramName)
                auto it = KPTAB.find(paramName);
                if (it != KPTAB.end()) {
                    localValues[i] = it->second; // default value from KPTAB
                } else {
                    // no default; leave placeholder or empty (choose a visible marker)
                    localValues[i] = "??";
                }
            }

            // 4) Expand MDT lines starting at mdtIndex until MEND
            for (int mi = mdtIndex; mi < (int)MDT.size(); ++mi) {
                if (MDT[mi].line == "MEND") break;

                string expandedLine = MDT[mi].line;

                // replace placeholders (#1..#n)
                for (int j = 0; j < totalParams; ++j) {
                    string key = "(#" + to_string(j + 1) + ")";
                    size_t pos;
                    while ((pos = expandedLine.find(key)) != string::npos) {
                        expandedLine.replace(pos, key.size(), localValues[j]);
                    }
                }

                // tokenize expandedLine and push into expandedCode
                stringstream ss(expandedLine);
                string token;
                vector<string> tokens;
                while (ss >> token) tokens.push_back(token);
                if (!tokens.empty()) expandedCode.push_back(tokens);
            }
        }
    }

    return expandedCode;
}

// ---------------- Printing Tables ----------------
void printTables() {
    cout << "\n========== MACRO NAME TABLE (MNT) ==========\n";
    cout << "Idx\tName\tPP_Count\tKP_Count\tMDT_Idx\tKP_Idx\tALA_Idx\n";
    for (int i = 0; i < (int)MNT.size(); i++) {
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
    for (int j = 0; j < (int)MDT.size(); j++)
        cout << j + 1 << "\t" << MDT[j].line << "\n";

    cout << "\n========== ARGUMENT LIST ARRAY (ALA) ==========\n";
    for (int k = 0; k < (int)ALA.size(); k++) {
        cout << "#" << k + 1 << ": ";
        for (int j = 0; j < (int)ALA[k].size(); j++) {
            cout << ALA[k][j];
            if (j + 1 < (int)ALA[k].size()) cout << ", ";
        }
        cout << "\n";
    }
}

// ---------------- Main (build tables with pass1 and run pass2) ----------------
int main() {
    // Source (Pass-1 input) - same as your example
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

    // Build MNT, MDT, ALA, KPTAB using Pass-1 logic
    processSource(source);
    printTables();

    // Intermediate code (no macro definitions)
    vector<vector<string>> intermediate = {
        {"START", "100"},
        {"READ", "A"},
        {"INCR", "A", "B"},
        {"INCR", "A", "B", "REG=AREG"}, // example with explicit keyword
        {"DECR", "B", "C"},
        {"SQUARE", "X"},
        {"STOP"},
        {"END"}
    };

    cout << "\n========== PASS-2 EXPANDED CODE ==========\n";
    vector<vector<string>> result = macroPass_2(intermediate);

    for (auto &line : result) {
        for (auto &word : line)
            cout << word << " ";
        cout << '\n';
    }

    return 0;
}
