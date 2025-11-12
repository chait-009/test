#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <iomanip>
using namespace std;

map<string, int> symtab;
map<string, int> littab;
vector<int> pooltab;
int LC = 0;

// ----------------- Utility Functions -----------------
bool isAD(const string &s) {
    vector<string> AD = {"START", "END", "ORIGIN", "EQU", "LTORG"};
    return find(AD.begin(), AD.end(), s) != AD.end();
}

bool isIS(const string &s) {
    vector<string> IS = {"STOP", "ADD", "SUB", "MULT", "MOVER", "MOVEM", "COMP", "BC", "DIV", "READ", "PRINT"};
    return find(IS.begin(), IS.end(), s) != IS.end();
}

bool isReg(const string &s) {
    vector<string> R = {"AREG", "BREG", "CREG", "DREG"};
    return find(R.begin(), R.end(), s) != R.end();
}

int opCode(const string &s) {
    map<string, int> m = {{"STOP", 0}, {"ADD", 1}, {"SUB", 2}, {"MULT", 3}, {"MOVER", 4}, {"MOVEM", 5},
                          {"COMP", 6}, {"BC", 7}, {"DIV", 8}, {"READ", 9}, {"PRINT", 10}};
    return m.count(s) ? m[s] : -1;
}

int regCode(const string &s) {
    map<string, int> m = {{"AREG", 1}, {"BREG", 2}, {"CREG", 3}, {"DREG", 4}};
    return m.count(s) ? m[s] : -1;
}

int getlitIdx(const string &token) {
    int i = 1;
    for (auto &p : littab) {
        if (p.first == token) return i;
        i++;
    }
    return -1;
}

int getsymIdx(const string &token) {
    int i = 1;
    for (auto &p : symtab) {
        if (p.first == token) return i;
        i++;
    }
    return -1;
}

void assignLiterals() {
    for (auto &p : littab)
        if (p.second == -1)
            p.second = LC++;
}

void assignLiterals1() {
    for (auto &p : littab) {
        if (p.second == -1) {
            int temp = LC;
            p.second = LC++;
            cout << temp << " (DL,02) (C," << p.first << ")\n";
        }
    }
}

// ----------------- Core Function -----------------
void processLine(const string &line) {
    if (line.empty()) return;

    vector<string> tokens;
    stringstream ss(line);
    string token;
    while (ss >> token) tokens.push_back(token);

    int i = 0;

    if (!isAD(tokens[i]) && !isIS(tokens[i])) {
        symtab[tokens[i]] = LC;
        i++;
    }

    if (i >= tokens.size()) return;

    string word = tokens[i];

    if (word == "START") {
        cout << "(AD,01) (C," << tokens[i + 1] << ")\n";
        LC = stoi(tokens[i + 1]);
        return;
    }

    if (word == "END") {
        cout << "(AD,02)\n";
        assignLiterals();
        pooltab.push_back(littab.size());
        return;
    }

    if (word == "LTORG") {
        cout << "(AD,05)\n";
        assignLiterals1();
        pooltab.push_back(littab.size());
        return;
    }

    if (word == "EQU") {
        cout << "(AD,04) ";
        string sym1 = tokens[i - 1];   // The symbol before EQU
        string expr = tokens[i + 1];
        int value = 0;

        size_t pos;
        if ((pos = expr.find('+')) != string::npos) {
            string sym = expr.substr(0, pos);
            int offset = stoi(expr.substr(pos + 1));
            value = symtab[sym] + offset;
        } else if ((pos = expr.find('-')) != string::npos) {
            string sym = expr.substr(0, pos);
            int offset = stoi(expr.substr(pos + 1));
            value = symtab[sym] - offset;
        } else if (isdigit(expr[0])) {
            value = stoi(expr);
        } else {
            value = symtab[expr];
        }

        symtab[sym1] = value;
        cout << sym1 << " = " << value << endl;
        return;
    }

    if (word == "ORIGIN") {
        cout << "(AD,03) ";
        string expr = tokens[i + 1];
        int newLC = 0;

        // Check for expression like "LABEL+5" or "LABEL-3"
        size_t pos;
        if ((pos = expr.find('+')) != string::npos) {
            string sym = expr.substr(0, pos);
            int offset = stoi(expr.substr(pos + 1));
            newLC = symtab[sym] + offset;
        } else if ((pos = expr.find('-')) != string::npos) {
            string sym = expr.substr(0, pos);
            int offset = stoi(expr.substr(pos + 1));
            newLC = symtab[sym] - offset;
        } else if(isdigit(expr[0])) {
            newLC = stoi(expr);
        } else {
            newLC = symtab[expr];
        }
        LC = newLC;
        cout << "(C," << LC << ")\n";
        return;
    }

    if (isIS(word)) {
        cout << LC << " (IS," << setw(2) << setfill('0') << opCode(word) << ") ";
        LC++;
        for (int j = i + 1; j < tokens.size(); j++) {
            string cleanToken = tokens[j];
            if (!cleanToken.empty() && cleanToken.back() == ',') {
                cleanToken.pop_back();
            }
            
            if (isReg(cleanToken)) {
                cout << "(RG, " << regCode(cleanToken) << ")";
            } else {
                if (cleanToken == "LT") continue;
                if (cleanToken.find("=") != string::npos) {
                    if (!littab.count(cleanToken)) littab[cleanToken] = -1;
                    cout << "(L," << getlitIdx(cleanToken) << ")";
                } else {
                    if (!symtab.count(cleanToken)) symtab[cleanToken] = -1;
                    cout << "(S," << getsymIdx(cleanToken) << ")";
                }
            }
        }
        cout << endl;
        return;
    }

    if (word == "DS") {
        cout << LC << " (DL,01) (C," << tokens[i + 1] << ")\n";
        LC += stoi(tokens[i + 1]);
        return;
    }

    if (word == "DC") {
        cout << LC << " (DL,02) (C," << tokens[i + 1] << ")\n";
        LC++;
        return;
    }
}

// ----------------- Main Function -----------------
int main() {
    ifstream fin("input.txt");
    if (!fin) {
        cerr << "Error: input.txt not found!" << endl;
        return 1;
    }

    pooltab.push_back(0);

    string line;
    while (getline(fin, line)) {
        processLine(line);
    }

    cout << "\n----SYMTAB----\n";
    int i = 1;
    for (auto &p : symtab)
        cout << i++ << " " << p.first << " : " << p.second << endl;

    cout << "\n----LITTAB----\n";
    i = 1;
    for (auto &p : littab)
        cout << i++ << " " << p.first << " : " << p.second << endl;

    cout << "\n----POOLTAB----\n";
    for (int j = 0; j < pooltab.size(); j++)
        cout << j << " " << pooltab[j] << endl;

    return 0;
}
// START 100
// L1 MOVER AREG, ='6'
// MOVEM AREG, Y
// ORIGIN 300
// NEXT ADD AREG, X
// SUB BREG, Y
// BC LT, L1
// ORIGIN NEXT+5
// MULT CREG, Z
// STOP
// X DS 2
// Y DS 1
// Z DC '9'
// END
