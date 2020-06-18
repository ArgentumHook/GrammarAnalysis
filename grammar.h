#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <set>
#include <sstream>
#define MAX 507
using namespace std;

class WF
{
public:
    string left, right;//候选式左部，右部
    set<string> Right;//候选式右部
    int back;
    int id;
    WF(char s[])
    {
        left = s;
    }
    WF(char s1[], char s2[], int x, int y)
    {
        left = s1;
        right = s2;
        back = x;
        id = y;
    }
    WF(const string &s1, const string &s2, int x, int y)
    {
        left = s1;
        right = s2;
        back = x;
        id = y;
    }
    bool operator<(const WF &a) const
    {
        if (left == a.left)
            return right < a.right;
        return left < a.left;
    }
    bool operator==(const WF &a) const
    {
        return (left == a.left) && (right == a.right);
    }
    void print()
    {
        printf("%s->%s\n", left.c_str(), right.c_str());
    }
    void insert(char s[])
    {
        Right.insert(s);
    }
};

class Closure
{
public:
    vector<WF> element;
    void print(string str)
    {
        printf("%-15s%-15s\n", "", str.c_str());
        for (int i = 0; i < element.size(); i++)
            element[i].print();
    }
    bool operator==(const Closure &a) const
    {
        if (a.element.size() != element.size())
            return false;
        for (int i = 0; i < a.element.size(); i++)
            if (element[i] == a.element[i])
                continue;
            else
                return false;
        return true;
    }
};

struct Content
{
    int type;
    int num;
    string out;
    Content() { type = -1; }
    Content(int a, int b)
        : type(a), num(b) {}
};

vector<WF> wf;
map<string, vector<int>> dic;
map<string, vector<int>> VN_set;
map<string, bool> vis;
string start = "S";
vector<Closure> collection;
vector<WF> items;
char CH = '$';
int go[MAX][MAX];
int to[MAX];
vector<char> V;
bool used[MAX];
Content action[MAX][MAX];
int Goto[MAX][MAX];
map<string, set<char>> first;
map<string, set<char>> follow;
map<string, int> VN_dic;
vector<WF> Vn_set;
bool isClickedFirst;
bool isClickedFollow;
bool isClickedLL;
bool isClickedLR;
bool isClickedSLR;

#endif // GRAMMAR_H
