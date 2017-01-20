#include <bits/stdc++.h>
using namespace std;

struct Node {
    int type;
    Node *l, *r;
    Node(int _type, Node *_l, Node *_r) : type(_type), l(_l), r(_r) {}
};

Node *build(char *s, int size) {
    if (s[0] == '0') {
        return new Node(0, NULL, NULL);
    } else if (s[0] == '1') {
        return new Node(1, NULL, NULL);
    } else {
        char *s1, *s2;
        int size1, size2;
        s1 = s + 3;
        int bcnt = 0;
        for (size1 = 0; bcnt != 0 || s1[size1] != ','; size1++) {
            if (s1[size1] == '(') bcnt++;
            if (s1[size1] == ')') bcnt--;
        }
        s2 = s1 + size1 + 1;
        size2 = s + size - 1 - s2;
        return new Node(2, build(s1, size1), build(s2, size2));
    }
}

void del(Node *t) {
    if (t->type == 2) {
        del(t->l);
        del(t->r);
    }
    delete t;
}

double solve(double p, Node *t) {
    if (t->type < 2) {
        return t->type;
    } else {
        return (1 - p) * solve(p, t->l) + p * solve(p, t->r);
    }
}

int main() {
    double p;
    string s;
    while (cin >> p >> s && (p != 0 || s != "0")) {
        Node *t = build(&s[0], s.size());
        cout << fixed << setprecision(6) << solve(p, t) << endl;
        del(t);
    }
}
