#include <bits/stdc++.h>
#include "parser.h"
using namespace std;
typedef unsigned int UI;
typedef vector<int> VI;
typedef vector<vector<int> > VVI;
typedef map<int, int> MII;

inline int var(int x) {
    return x >> 1;
}

inline int isneg(int x) {
    return x & 1;
}

void renumber(VVI &clauses, VI &orinum) {
    MII newnum;
    orinum.clear();
    for (UI i = 0; i < clauses.size(); i++) {
        for (UI j = 0; j < clauses[i].size(); j++) {
            int x = clauses[i][j];
            int absx = abs(x);
            if (newnum.find(absx) == newnum.end()) {
                newnum[absx] = orinum.size();
                orinum.push_back(absx);
            }
            clauses[i][j] = (newnum[absx] << 1) + (x < 0 ? 1 : 0);
        }
    }
}

bool bf(VVI &clauses, VI &value, UI n) {
    if (n < value.size()) {
        value[n] = 1;
        if (bf(clauses, value, n + 1)) {
            return true;
        }
        value[n] = 0;
        return bf(clauses, value, n + 1);
    } else {
        for (UI i = 0; i < clauses.size(); i++) {
            bool ok = false;
            for (UI j = 0; j < clauses[i].size(); j++) {
                int &x = clauses[i][j];
                if (isneg(x) ^ value[var(x)]) {
                    ok = true;
                    break;
                }
            }
            if (!ok) {
                return false;
            }
        }
        return true;
    }
}

bool solve(VVI &clauses, VI &value) {
    return bf(clauses, value, 0);
}

bool sort_by_abs(int a, int b) {
    return abs(a) < abs(b);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage : ./yasat [input.cnf]" << endl;
        return 0;
    }

    VVI clauses;
    int maxVarIndex;
    parse_DIMACS_CNF(clauses, maxVarIndex, argv[1]);

    VI orinum;
    renumber(clauses, orinum);

    string outfname = argv[1];
    for (int i = outfname.size() - 1; i >= 0 && outfname[i] != '/'; i--) {
        if (outfname[i] == '.') {
            outfname = outfname.substr(0, i) + ".sat";
            break;
        }
    }
    fstream fout(outfname.c_str(), ios_base::out);

    VI value(orinum.size());
    if (solve(clauses, value)) {
        fout << "s SATISFIABLE" << endl;
        VI ans;
        for (UI i = 0; i < orinum.size(); i++) {
            ans.push_back((value[i] ? 1 : -1) * orinum[i]);
        }
        sort(ans.begin(), ans.end(), sort_by_abs);
        fout << "v ";
        for (UI i = 0; i < ans.size(); i++) {
            fout << ans[i] << ' ';
        }
        fout << 0 << endl;
    } else {
        fout << "s UNSATISFIABLE" << endl;
    }
}
