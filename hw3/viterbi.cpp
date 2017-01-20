#include <bits/stdc++.h>
using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Usage : ./viterbi [input file] [output file]" << endl;
        return 1;
    }
    fstream fin(argv[1], ios::in);
    fstream fout(argv[2], ios::out);
    if (!fin.is_open()) {
        cout << "Can not open file " << argv[1] << endl;
        return 1;
    }
    if (!fout.is_open()) {
        cout << "Can not open file " << argv[2] << endl;
        return 1;
    }
    int n;
    fin >> n >> ws;
    vector<int> coat(n);
    vector<int> weather(n); // 0 : sunny, 1 : foggy, 2 : rainy
    for (int i = 0; i < n; i++) {
        string sw, sc;
        getline(fin, sw, ',');
        getline(fin, sc);
        if (sw == "sunny") {
            weather[i] = 0;
        } else if (sw == "foggy") {
            weather[i] = 1;
        } else {
            weather[i] = 2;
        }
        if (sc == "yes") {
            coat[i] = 1;
        } else {
            coat[i] = 0;
        }
    }
    double trans_prob[3][3] = {
        { 0.8, 0.15, 0.05 },
        { 0.2, 0.5, 0.3 },
        { 0.2, 0.2, 0.6 }
    };
    double emis_prob[2][3] = {
        { 0.9, 0.7, 0.2 },
        { 0.1, 0.3, 0.8 }
    };
    vector<double> pred[3] = { vector<double>(n, 0), vector<double>(n, 0), vector<double>(n, 0) };
    vector<int> from[3] = { vector<int>(n), vector<int>(n), vector<int>(n) };
    pred[0][0] = 0.5 * emis_prob[coat[0]][0];
    pred[1][0] = 0.25 * emis_prob[coat[0]][1];
    pred[2][0] = 0.25 * emis_prob[coat[0]][2];
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                double r = pred[k][i - 1] * trans_prob[k][j] * emis_prob[coat[i]][j];
                if ( r >= pred[j][i]) {
                    pred[j][i] = r;
                    from[j][i] = k;
                }
            }
        }
    }
    vector<int> ans(n);
    double maxr = 0;
    int w = 0;
    for (int i = 0; i < 3; i++) {
        if (pred[i][n - 1] > maxr) {
            maxr = pred[i][n - 1];
            w = i;
        }
    }
    for (int i = n - 1; i >= 0; i--) {
        ans[i] = w;
        w = from[w][i];
    }
    int correct = 0;
    for (int i = 0; i < n; i++) {
        if (ans[i] == weather[i]) {
            correct++;
        }
    }
    fout << (double) correct / n << endl;
    string weather_name[3] = { "sunny", "foggy", "rainy" };
    for (int i = 0; i < n; i++) {
        fout << weather_name[ans[i]] << endl;
        //cout << i << "\t: " << pred[0][i] << "\t" << pred[1][i] << "\t" << pred[2][i] << endl;
    }
}
