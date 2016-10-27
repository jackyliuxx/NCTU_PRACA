#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cmath>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

int encode(int a, int b, int c, int base) {
    return (((a * base) + b) * base) + c + 1;
}

void decode(int x, int &a, int &b, int &c, int base) {
    x--;
    c = x % base;
    x /= base;
    b = x % base;
    x /= base;
    a = x;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cout << "usage: ./solver <input_file> <output_file> <minisatexe>" << endl;
        return 0;
    }
    string in_fname = argv[1], out_fname = argv[2], minisatexe = argv[3];
    fstream fin(in_fname, ios::in);
    vector<vector<int>> board;
    string s;
    while (getline(fin, s)) {
        stringstream ss(s);
        board.push_back(vector<int>());
        int x;
        while (ss >> x) {
            board.back().push_back(x);
        }
    }
    fin.close();
    fstream cnf("cnf.tmp", ios::out);
    int n = board.size();
    int sqn = sqrt(n);
    for (int i = 0; i < n; i++) { // tile
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                cnf << encode(i, j, k, n) << ' ';
            }
            cnf << 0 << endl;
            for (int k1 = 0; k1 < n; k1++) {
                for (int k2 = k1 + 1; k2 < n; k2++) {
                    cnf << -encode(i, j, k1, n) << ' ' << -encode(i, j, k2, n) << ' ' << 0 << endl;
                }
            }
        }
    }
    for (int i = 0; i < n; i++) { // row
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                cnf << encode(i, k, j, n) << ' ';
            }
            cnf << 0 << endl;
            for (int k1 = 0; k1 < n; k1++) {
                for (int k2 = k1 + 1; k2 < n; k2++) {
                    cnf << -encode(i, k1, j, n) << ' ' << -encode(i, k2, j, n) << ' ' << 0 << endl;
                }
            }
        }
    }
    for (int i = 0; i < n; i++) { // column
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                cnf << encode(k, i, j, n) << ' ';
            }
            cnf << 0 << endl;
            for (int k1 = 0; k1 < n; k1++) {
                for (int k2 = k1 + 1; k2 < n; k2++) {
                    cnf << -encode(k1, i, j, n) << ' ' << -encode(k2, i, j, n) << ' ' << 0 << endl;
                }
            }
        }
    }
    for (int i = 0; i < n; i++) { // block
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                cnf << encode(i / sqn * sqn + k / sqn, i % sqn * sqn + k % sqn, j, n) << ' ';
            }
            cnf << 0 << endl;
            for (int k1 = 0; k1 < n; k1++) {
                for (int k2 = k1 + 1; k2 < n; k2++) {
                    cnf << -encode(i / sqn * sqn + k1 / sqn, i % sqn * sqn + k1 % sqn, j, n) << ' ';
                    cnf << -encode(i / sqn * sqn + k2 / sqn, i % sqn * sqn + k2 % sqn, j, n) << ' ';
                    cnf << 0 << endl;
                }
            }
        }
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j]) {
                cnf << encode(i, j, board[i][j] - 1, n) << ' ' << 0 << endl;
            }
        }
    }
    cnf.close();
    int pid = fork();
    if (pid == 0) {
        if (minisatexe.find('/') == string::npos) {
            minisatexe = "./" + minisatexe;
        }
        execl(minisatexe.c_str(), minisatexe.c_str(), "cnf.tmp", "sat.tmp", NULL);
    } else {
        int status;
        wait(&status);
    }
    fstream sat("sat.tmp", ios::in);
    fstream fout(out_fname, ios::out);
    sat >> s;
    if (s == "UNSAT") {
        fout << "NO" << endl;
    } else {
        int x;
        while (sat >> x) {
            if (x > 0) {
                int i, j, k;
                decode(x, i, j, k, n);
                board[i][j] = k + 1;
            }
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (j) fout << ' ';
                fout << board[i][j];
            }
            fout << endl;
        }
    }
    sat.close();
    fout.close();
    remove("cnf.tmp");
    remove("sat.tmp");
}
