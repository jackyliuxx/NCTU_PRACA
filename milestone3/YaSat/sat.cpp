#define PRINT_RESULT
// #define DEBUG

#include <bits/stdc++.h>
#include "parser.h"
using namespace std;

typedef unsigned int UI;
typedef vector<int> VI;
typedef vector<vector<int> > VVI;
typedef map<int, int> MII;
typedef set<int> SI;
typedef queue<int> QI;


class SatSolver {
    private:
        VI orinum;
        VVI clauses;
        VI value;
        VI assign_level;
        VI related_clause;
        VVI watch;
        VVI watched_by;

        inline int var(int x) {
            return x >> 1;
        }
        inline int sign(int x) {
            return x & 1;
        }
        inline int value_v(int x) {
            return value[var(x)];
        }
        inline bool is_true(int x) {
            return x == value_v(x);
        }
        inline bool is_false(int x) {
            return value_v(x) != -1 && value_v(x) != x;
        }
        inline bool is_unseted(int x) {
            return value_v(x) == -1;
        }

        void renumber_clauses() {
            MII newnum;
            orinum.assign(1, 0);
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

        void reorganize_clauses() {
            VVI new_clauses;
            for (UI i = 0; i < clauses.size(); i++) {
                SI s;
                for (UI j = 0; j < clauses[i].size(); j++) {
                    s.insert(clauses[i][j]);
                }
                VI cls;
                for (int j : s) {
                    if (!s.count(-j)) {
                        cls.push_back(j);
                    }
                }
                if (cls.size()) {
                    new_clauses.push_back(move(cls));
                }
            }
            clauses = move(new_clauses);
        }
        
        VI firstUIP(const VI &cls, int level) {
            QI q;
            VI newcls;
            SI occured;
            for (UI j = 0; j < cls.size(); j++) {
                q.push(cls[j]);
                occured.insert(cls[j]);
            }
            while (q.size() > 1) {
                int tmpv = q.front();
                int tmpvv = var(tmpv);
                q.pop();
                if (assign_level[tmpvv] != level) {
                    newcls.push_back(tmpv);
                } else if (related_clause[tmpvv] == -1) {
                    q.push(tmpv);
                } else {
                    VI &tmpcls = clauses[related_clause[tmpvv]];
                    for (UI j = 0; j < tmpcls.size(); j++) {
                        if (!occured.count(tmpcls[j]) && tmpcls[j] != (tmpv ^ 1)) {
                            occured.insert(tmpcls[j]);
                            q.push(tmpcls[j]);
                        }
                    }
                }
            }
            newcls.push_back(q.front());
            return newcls;
        }

        void push_clause(VI &&cls) {
            watch.push_back({-1, -1});
            for (UI j = 0; j < cls.size(); j++) {
                if (is_unseted(cls[j])) {
                    watched_by[var(cls[j])].push_back(clauses.size());
                    if (watch.back()[0] == -1) {
                        watch.back()[0] = cls[j];
                    } else {
                        watch.back()[1] = cls[j];
                        break;
                    }
                }
            }
            clauses.push_back(move(cls));
        }

        VI getorder() {
            vector<pair<double, int>> vc(value.size());
            for (UI i = 0; i < vc.size(); i++) {
                vc[i] = { 0, i };
            }
            for (UI i = 0; i < clauses.size(); i++) {
                for (UI j = 0; j < clauses[i].size(); j++) {
                    vc[var(clauses[i][j])].first -= pow(0.5, clauses[i].size());
                }
            }
            sort(vc.begin(), vc.end());
            VI vr(value.size());
            for (UI i = 0; i < vc.size(); i++) {
                vr[i] = vc[i].second;
            }
            return vr;
        }

#ifdef DEBUG
        void printstatus() {
            for (UI i = 0; i < value.size(); i++) {
                cout << i << " : " << (value[i] == -1 ? -1 : value[i] & 1);
                cout << " watched by ";
                for (UI j = 0; j < watched_by[i].size(); j++) {
                    cout << watched_by[i][j] << ' ';
                }
                cout << endl;
            }
            cout << "*************" << endl;
            for (UI i = 0; i < clauses.size(); i++) {
                for (UI j = 0; j < clauses[i].size(); j++) {
                    cout << ((clauses[i][j] & 1) ? -1 : 1) * (var(clauses[i][j])) << ' ';
                }
                cout << "watching " << watch[i][0] << ' ' << watch[i][1];
                cout << endl;
            }
            cout << "-----------------" << endl;
        }
#endif

    public:
        VI solve(const VVI &_clauses) {
            clauses = _clauses;
            reorganize_clauses();
            renumber_clauses();
            value.assign(orinum.size(), -1);
            assign_level.assign(value.size(), -1);
            related_clause.assign(value.size(), -1);
            int exv = 1; // the level 0 assign var
            for (UI i = 0; i < clauses.size(); i++) {
                clauses[i].push_back(exv);
            }
            watch.assign(clauses.size(), VI(2));
            watched_by.assign(value.size(), VI());
            for (UI i = 0; i < clauses.size(); i++) {
                watch[i][0] = exv;
                watch[i][1] = clauses[i][0];
                watched_by[var(exv)].push_back(i);
                watched_by[var(clauses[i][0])].push_back(i);
            }
            VI order = getorder();
            VI r_order(order.size());
            for (UI i = 0; i < order.size(); i++) {
                r_order[order[i]] = i;
            }
            VI to_be_assigned_v(value.size());
            for (UI i = 0; i < to_be_assigned_v.size(); i++) {
                to_be_assigned_v[i] = (i << 1) + rand() % 2;
            }
            int restart_cnt = 0, restart_bound = 10 * value.size();
            int level = 0;
            UI to_be_assigned_i = 0;
            VI assigned;

            while (to_be_assigned_i < order.size()) {
#ifdef DEBUG
                printstatus();
#endif
                UI to_be_assigned = order[to_be_assigned_i];
                related_clause[to_be_assigned] = -1;
                VI asv(1);
                if (to_be_assigned == 0) {
                    asv[0] = 0;
                } else {
                    asv[0] = to_be_assigned_v[to_be_assigned];
                    to_be_assigned_v[to_be_assigned] ^= 1;
                }
                while (asv.size()) {
                    int v = asv.back();
                    int vv = var(v);
                    asv.pop_back();
                    if (is_true(v)) {
                        continue;
                    }
                    assigned.push_back(vv);
                    value[vv] = v;
                    assign_level[vv] = level;
                    for (UI i = 0; i < watched_by[vv].size(); i++) {
                        int c = watched_by[vv][i];
                        VI &cls = clauses[c];
                        VI &w = watch[c];
                        bool keep = true;
                        if (!is_true(w[0]) && !is_true(w[1])) { // no watched var is true
                            for (UI j = 0; j < cls.size(); j++) {
                                if (w[0] != cls[j] && w[1] != cls[j] && !is_false(cls[j])) { //is not watched and not false
                                    (w[0] == (v ^ 1) ? w[0] : w[1]) = cls[j];
                                    watched_by[var(cls[j])].push_back(c);
                                    keep = false;
                                    break;
                                }
                            }
                            if (keep && (is_unseted(w[0]) || is_unseted(w[1]))) { // only one var left
                                int newasv = is_unseted(w[0]) ? w[0] : w[1];
                                asv.push_back(newasv);
                                related_clause[var(newasv)] = c;
                            }
                        }
                        if (!keep) {
                            swap(watched_by[vv][i], watched_by[vv].back());
                            watched_by[vv].pop_back();
                            i--;
                        } else if (is_false(w[0]) && is_false(w[1])) { // conflict
                            if (level == 0) {
                                return {};
                            }
                            // find 1UIP
                            VI newcls = firstUIP(cls, level);
                            // restart
                            int restart_level = 0;
                            for (UI j = 0; j < newcls.size() - 1; j++) {
                                restart_level = max(restart_level, assign_level[var(newcls[j])]);
                            }
                            while (assigned.size() && assign_level[assigned.back()] >= restart_level) {
                                value[assigned.back()] = -1;
                                to_be_assigned_i = r_order[assigned.back()];
                                assigned.pop_back();
                            }
                            level = restart_level - 1;

                            restart_cnt++;
                            if (newcls.size() > 5 * log(value.size())) {
                                restart_cnt += 1;
                            }
                            // add new clause
                            push_clause(move(newcls));

                            asv.clear();
                            break;
                        }
                    }
                }
                while (to_be_assigned_i < order.size() && ~value[order[to_be_assigned_i]]) {
                    to_be_assigned_i++;
                }
                level++;
                if (to_be_assigned_i < order.size() && restart_cnt >= restart_bound) {
                    restart_cnt = 0;
                    value.assign(value.size(), -1);
                    assigned.clear();
                    level = 0;
                    to_be_assigned_i = 0;

                    order = getorder();
                    for (UI i = 0; i < order.size(); i++) {
                        r_order[order[i]] = i;
                    }
                }
            }
            VI ans;
            for (UI i = 1; i < orinum.size(); i++) {
                ans.push_back(((value[i] & 1) ? -1 : 1) * orinum[i]);
            }
            sort(ans.begin(), ans.end(), [](int a, int b){return abs(a) < abs(b);});
            return ans;
        }
};

int main(int argc, char *argv[]) {
    srand(time(NULL));
    if (argc < 2) {
        cout << "Usage : ./yasat [input.cnf]" << endl;
        return 0;
    }

    VVI clauses;
    int maxVarIndex;
    parse_DIMACS_CNF(clauses, maxVarIndex, argv[1]);

    SatSolver solver;
    VI ans = solver.solve(clauses);
    
#ifdef PRINT_RESULT
    if (ans.size()) {
        cout << "SAT" << endl;
        SI st;
        for (UI i = 0; i < ans.size(); i++) {
            st.insert(ans[i]);
        }
        for (UI i = 0; i < clauses.size(); i++) {
            bool wrong = true;
            for (UI j = 0; j < clauses[i].size(); j++) {
                if (st.count(clauses[i][j])) {
                    wrong = false;
                    break;
                }
            }
            if (wrong) {
                cout << "WRONG!!!!! " << i << endl;
            }
        }
    } else {
        cout << "UNSAT" << endl;
    }
#endif

    string outfname = argv[1];
    for (int i = outfname.size() - 1; i >= 0 && outfname[i] != '/'; i--) {
        if (outfname[i] == '.') {
            outfname = outfname.substr(0, i) + ".sat";
            break;
        }
    }
    fstream fout(outfname.c_str(), ios_base::out);
    if (ans.size()) {
        fout << "s SATISFIABLE" << endl;
        fout << "v ";
        for (UI i = 0; i < ans.size(); i++) {
            fout << ans[i] << ' ';
        }
        fout << 0 << endl;
    } else {
        fout << "s UNSATISFIABLE" << endl;
    }
}
