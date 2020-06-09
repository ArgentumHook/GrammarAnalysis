#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "grammar.h"
#include <QDebug>
#include <QDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QLabel>
#include <QStandardItemModel>
#include <QWidget>
#include <QStandardItem>
#define MAX 507
using namespace std;

QString rawInput;//获取输入文本
QStringList exprSet;//输入的产生式集合

void make_item()
{
    memset(to, -1, sizeof(-1));
    for (int i = 0; i < wf.size(); i++)
        VN_set[wf[i].left].push_back(i);
    for (int i = 0; i < wf.size(); i++)
        for (int j = 0; j <= wf[i].right.length(); j++)
        {
            string temp = wf[i].right;
            temp.insert(temp.begin() + j, CH);
            dic[wf[i].left].push_back(items.size());
            if (j)
                to[items.size() - 1] = items.size();
            items.push_back(WF(wf[i].left, temp, i, items.size()));
        }
}

void dfs(const string &x)
{
    if (vis[x])
        return;
    vis[x] = 1;
    vector<int> &id = VN_set[x];
    for (int i = 0; i < id.size(); i++)
    {
        string &left = wf[id[i]].left;
        string &right = wf[id[i]].right;
        for (int j = 0; j < right.length(); j++)
            if (isupper(right[j]))
            {
                dfs(right.substr(j, 1));
                set<char> &temp = first[right.substr(j, 1)];
                set<char>::iterator it = temp.begin();
                bool flag = true;
                for (; it != temp.end(); it++)
                {
                    if (*it == '~')
                        flag = false;
                    first[left].insert(*it);
                }
                if (flag)
                    break;
            }
            else
            {
                first[left].insert(right[j]);
                break;
            }
    }
}

void make_first()
{
    vis.clear();
    map<string, vector<int>>::iterator it2 = dic.begin();
    for (; it2 != dic.end(); it2++)
        if (vis[it2->first])
            continue;
        else
            dfs(it2->first);
    //新建first集对话框并设置属性
    QDialog *firstSetOutput=new QDialog();
    firstSetOutput->setWindowTitle("first集");
    firstSetOutput->resize(400,300);
    //输出first集
    QLabel *label=new QLabel(firstSetOutput);
    QString output="";
    map<string, set<char>>::iterator it = first.begin();
    for (; it != first.end(); it++)
    {
        output+="FIRST(";
        output+=it->first.c_str();
        output+=")={";
        set<char> &temp = it->second;
        set<char>::iterator it1 = temp.begin();
        bool flag = false;
        for (; it1 != temp.end(); it1++)
        {
            if (flag)
                output+=",";
            output+=*it1;
            flag = true;
        }
        output+="}\n";
    }
    //设置输出文字样式
    QFont ft;
    ft.setPointSize(16);
    label->setFont(ft);
    label->setText(output);
    firstSetOutput->show();
}

void append(const string &str1, const string &str2)
{
    set<char> &from = follow[str1];
    set<char> &to = follow[str2];
    set<char>::iterator it = from.begin();
    for (; it != from.end(); it++)
        to.insert(*it);
}

bool _check(const vector<int> &id, const string str)
{
    for (int i = 0; i < id.size(); i++)
    {
        int x = id[i];
        if (wf[x].right == str)
            return true;
    }
    return false;
}

void make_follow()
{
    while (true)
    {
        bool goon = false;
        map<string, vector<int>>::iterator it2 = VN_set.begin();
        for (; it2 != VN_set.end(); it2++)
        {
            vector<int> &id = it2->second;
            for (int i = 0; i < id.size(); i++)
            {
                bool flag = true;
                WF &tt = wf[id[i]];
                string &left = tt.left;
                const string &right = tt.right;
                for (int j = right.length() - 1; j >= 0; j--)
                    if (isupper(right[j]))
                    {
                        if (flag)
                        {
                            int tx = follow[right.substr(j, 1)].size();
                            append(left, right.substr(j, 1));
                            int tx1 = follow[right.substr(j, 1)].size();
                            if (tx1 > tx)
                                goon = true;
                            if (_check(id, "~"))
                                flag = false;
                        }
                        for (int k = j + 1; k < right.length(); k++)
                            if (isupper(right[k]))
                            {
                                string idd = right.substr(k, 1);
                                set<char> &from = first[idd];
                                set<char> &to = follow[right.substr(j, 1)];
                                set<char>::iterator it1 = from.begin();
                                int tx = follow[right.substr(j, 1)].size();
                                for (; it1 != from.end(); it1++)
                                    if (*it1 != '~')
                                        to.insert(*it1);
                                int tx1 = follow[right.substr(j, 1)].size();
                                if (tx1 > tx)
                                    goon = true;
                                if (_check(id, "~"))
                                    break;
                            }
                            else
                            {
                                int tx = follow[right.substr(j, 1)].size();
                                follow[right.substr(j, 1)].insert(right[k]);
                                int tx1 = follow[right.substr(j, 1)].size();
                                if (tx1 > tx)
                                    goon = true;
                                break;
                            }
                    }
                    else
                        flag = false;
            }
        }
        if (!goon)
            break;
    }
    //新建follow集对话框并设置属性
    QDialog *followSetOutput=new QDialog();
    followSetOutput->setWindowTitle("follow集");
    followSetOutput->resize(400,300);
    //输出follow集
    QLabel *label=new QLabel(followSetOutput);
    QString output="";
    map<string, set<char>>::iterator it = follow.begin();
    for (; it != follow.end(); it++)
    {
        output+="FOLLOW(";
        output+=it->first.c_str();
        output+=")={";
        set<char> &temp = it->second;
        temp.insert('#');
        set<char>::iterator it1 = temp.begin();
        bool flag = false;
        for (; it1 != temp.end(); it1++)
        {
            if (flag)
                output+=",";
            output+=*it1;
            flag = true;
        }
        output+="}\n";
    }
    //设置输出文字样式
    QFont ft;
    ft.setPointSize(16);
    label->setFont(ft);
    label->setText(output);
    followSetOutput->show();
}

void make_set()
{
    bool has[MAX];
    for (int i = 0; i < items.size(); i++)
        if (items[i].left[0] == 'S' && items[i].right[0] == CH)
        {
            Closure temp;
            string &str = items[i].right;
            vector<WF> &element = temp.element;
            element.push_back(items[i]);
            int x = 0;
            for (x = 0; x < str.length(); x++)
                if (str[x] == CH)
                    break;
            memset(has, 0, sizeof(has));
            has[i] = 1;
            if (x != str.length() - 1)
            {
                queue<string> q;
                q.push(str.substr(x + 1, 1));
                while (!q.empty())
                {
                    string u = q.front();
                    q.pop();
                    vector<int> &id = dic[u];
                    for (int j = 0; j < id.size(); j++)
                    {
                        int tx = id[j];
                        if (items[tx].right[0] == CH)
                        {
                            if (has[tx])
                                continue;
                            has[tx] = 1;
                            if (isupper(items[tx].right[1]))
                                q.push(items[tx].right.substr(1, 1));
                            element.push_back(items[tx]);
                        }
                    }
                }
            }
            collection.push_back(temp);
        }
    for (int i = 0; i < collection.size(); i++)
    {
        map<int, Closure> temp;
        for (int j = 0; j < collection[i].element.size(); j++)
        {
            string str = collection[i].element[j].right;
            int x = 0;
            for (; x < str.length(); x++)
                if (str[x] == CH)
                    break;
            if (x == str.length() - 1)
                continue;
            int y = str[x + 1];
            int ii;
            //cout << i << "previous: " << str << endl;
            str.erase(str.begin() + x);
            str.insert(str.begin() + x + 1, CH);
            //cout << i <<"after: " << str << endl;
            WF cmp = WF(collection[i].element[j].left, str, -1, -1);
            for (int k = 0; k < items.size(); k++)
                if (items[k] == cmp)
                {
                    ii = k;
                    break;
                }
            //string& str1 = items[ii].right;
            memset(has, 0, sizeof(has));
            vector<WF> &element = temp[y].element;
            element.push_back(items[ii]);
            has[ii] = 1;
            x++;
            if (x != str.length() - 1)
            {
                queue<string> q;
                q.push(str.substr(x + 1, 1));
                while (!q.empty())
                {
                    string u = q.front();
                    q.pop();
                    vector<int> &id = dic[u];
                    for (int j = 0; j < id.size(); j++)
                    {
                        int tx = id[j];
                        if (items[tx].right[0] == CH)
                        {
                            if (has[tx])
                                continue;
                            has[tx] = 1;
                            if (isupper(items[tx].right[1]))
                                q.push(items[tx].right.substr(1, 1));
                            element.push_back(items[tx]);
                        }
                    }
                }
            }
        }
        map<int, Closure>::iterator it = temp.begin();
        for (; it != temp.end(); it++)
            collection.push_back(it->second);
        for (int i = 0; i < collection.size(); i++)
            sort(collection[i].element.begin(), collection[i].element.end());
        for (int i = 0; i < collection.size(); i++)
            for (int j = i + 1; j < collection.size(); j++)
                if (collection[i] == collection[j])
                    collection.erase(collection.begin() + j);
    }
}

void make_V()
{
    memset(used, 0, sizeof(used));
    for (int i = 0; i < wf.size(); i++)
    {
        string &str = wf[i].left;
        for (int j = 0; j < str.length(); j++)
        {
            if (used[str[j]])
                continue;
            used[str[j]] = 1;
            V.push_back(str[j]);
        }
        string &str1 = wf[i].right;
        for (int j = 0; j < str1.length(); j++)
        {
            if (used[str1[j]])
                continue;
            used[str1[j]] = 1;
            V.push_back(str1[j]);
        }
    }
    sort(V.begin(), V.end());
    V.push_back('#');
}

void make_cmp(vector<WF> &cmp1, int i, char ch)
{
    for (int j = 0; j < collection[i].element.size(); j++)
    {
        string str = collection[i].element[j].right;
        int k;
        for (k = 0; k < str.length(); k++)
            if (str[k] == CH)
                break;
        if (k != str.length() - 1 && str[k + 1] == ch)
        {
            str.erase(str.begin() + k);
            str.insert(str.begin() + k + 1, CH);
            cmp1.push_back(WF(collection[i].element[j].left, str, -1, -1));
        }
    }
    sort(cmp1.begin(), cmp1.end());
}

void make_go()
{
    memset(go, -1, sizeof(go));
    int m = collection.size();
    for (int t = 0; t < V.size(); t++)
    {
        char ch = V[t];
        for (int i = 0; i < m; i++)
        {
            vector<WF> cmp1;
            make_cmp(cmp1, i, ch);
            //cout << cmp1.size() << endl;
            if (cmp1.size() == 0)
                continue;
            for (int j = 0; j < m; j++)
            {
                vector<WF> cmp2;
                for (int k = 0; k < collection[j].element.size(); k++)
                {
                    string &str = collection[j].element[k].right;
                    int x;
                    for (x = 0; x < str.length(); x++)
                        if (str[x] == CH)
                            break;
                    if (x && str[x - 1] == ch)
                        cmp2.push_back(WF(collection[j].element[k].left, str, -1, -1));
                }
                sort(cmp2.begin(), cmp2.end());
                //cout << cmp2.size() << endl;
                bool flag = true;
                if (cmp2.size() != cmp1.size())
                    continue;
                //cout << cmp1.size() << endl;
                for (int k = 0; k < cmp1.size(); k++)
                    if (cmp1[k] == cmp2[k])
                        continue;
                    else
                        flag = false;
                //cout << "out " << endl;
                if (flag)
                    go[i][ch] = j;
            }
            //cout << "YES" << endl;
        }
    }
}

vector<map<char, string>> predict_table;

bool check_first(const string &text, char ch)
{
    for (int i = 0; i < text.length(); i++)
    {
        bool hasEmpty = false;
        if (!isupper(text[i]) && text[i] != '\'')
        {
            if (text[i] != ch)
                return false;
            else
                return true;
        }
        else if (isupper(text[i]))
        {
            string temp;
            if (i != text.length() - 1 && text[i + 1] == '\'')
                temp = text.substr(i, 2);
            else
                temp = text.substr(i, 1);
            set<char> &dic = first[temp];
            set<char>::iterator it = dic.begin();
            for (; it != dic.end(); it++)
            {
                if (*it == '~')
                    hasEmpty = true;
                if (*it == ch)
                    return true;
            }
            if (!hasEmpty)
                break;
        }
        else
            continue;
    }
    return false;
}

bool check_follow(const string &text, char ch)
{
    set<char> &dic = follow[text];
    set<char>::iterator it = dic.begin();
    for (; it != dic.end(); it++)
        if (*it == ch)
            return true;
    return false;
}

void make_LL_table()
{
    map<char, string> temp;
    vector<char> letter;
    bool vis[500];
    memset(vis, 0, sizeof(vis));
    for (int i = 0; i < Vn_set.size(); i++)
    {
        set<string> &Right = Vn_set[i].Right;
        set<string>::iterator it = Right.begin();
        for (; it != Right.end(); it++)
            for (int j = 0; j < it->length(); j++)
                if (!isupper(it->at(j)) && it->at(j) != '\'')
                {
                    if (vis[it->at(j)])
                        continue;
                    vis[it->at(j)] = true;
                    letter.push_back(it->at(j));
                }
    }
    for (int i = 0; i < Vn_set.size(); i++)
    {
        temp.clear();
        string &left = Vn_set[i].left;
        set<string> &Right = Vn_set[i].Right;
        set<string>::iterator it = Right.begin();
        for (; it != Right.end(); it++)
            for (int j = 0; j < letter.size(); j++)
            {
                //cout << *it << " " <<  letter[j] << endl;
                if (check_first(*it, letter[j]))
                {
                    //cout << "YES" << endl;
                    temp[letter[j]] = *it;
                }
                if (it->at(0) == '~' && check_follow(left, letter[j]))
                    temp[letter[j]] = *it;
            }
        predict_table.push_back(temp);
    }
    //新建LL(1)表对话框并设置属性
    QDialog *LLTableOutput=new QDialog();
    LLTableOutput->setWindowTitle("LL(1)分析表");
    LLTableOutput->resize(800,600);
    //建立LL(1)表
    QTableView *LLTable=new QTableView(LLTableOutput);
    //建立LL(1)表骨架
    QStandardItemModel *LLItem=new QStandardItemModel(LLTable);
    //建立表头内容
    LLItem->setColumnCount(letter.size()+1);
    LLItem->setHeaderData(0,Qt::Horizontal,"");
    for (int i = 0; i < letter.size(); i++)
    {
        LLItem->setHeaderData(i+1,Qt::Horizontal,QChar(letter[i]));
    }
    for (int i = 0; i < Vn_set.size(); i++)
    {
        QStandardItem *standItem = new QStandardItem(Vn_set[i].left.c_str());
        LLItem->setItem(i,0,standItem);
        for (int j = 0; j < letter.size(); j++)
        {
            QStandardItem *predictItem = new QStandardItem(predict_table[i][letter[j]].c_str());
            LLItem->setItem(i,j+1,predictItem);
        }
    }
    LLTable->setModel(LLItem);
    LLTable->resize(800,600);
    LLTable->show();
    LLTableOutput->show();
}

void make_LR_table()
{
    memset(Goto, -1, sizeof(Goto));
    //write s to the table
    for (int i = 0; i < collection.size(); i++)
        for (int j = 0; j < V.size(); j++)
        {
            char ch = V[j];
            int x = go[i][ch];
            if (x == -1)
                continue;
            if (!isupper(ch))
                action[i][ch] = Content(0, x);
            else
                Goto[i][ch] = x;
        }
    //write r and acc to the table
    for (int i = 0; i < collection.size(); i++)
        for (int j = 0; j < collection[i].element.size(); j++)
        {
            WF &tt = collection[i].element[j];
            if (tt.right[tt.right.length() - 1] == CH)
            {
                if (tt.left[0] == 'S')
                    action[i]['#'] = Content(2, -1);
                else
                    for (int k = 0; k < V.size(); k++)
                    {
                        int y = V[k];
                        //cout << "YES " << endl;
                        action[i][y] = Content(1, tt.back);
                    }
            }
        }
    //新建LR(0)表对话框并设置属性
    QDialog *LRTableOutput=new QDialog();
    LRTableOutput->setWindowTitle("LR(0)分析表");
    LRTableOutput->resize(800,600);
    //建立LR(0)表
    QTableView *LRTable=new QTableView(LRTableOutput);
    //建立LR(0)表骨架
    QStandardItemModel *LRItem=new QStandardItemModel(LRTable);
    //建立表头内容
    LRItem->setColumnCount(V.size());
    for (int i = 0; i < V.size(); i++)
    {
        LRItem->setHeaderData(i,Qt::Horizontal,QChar(V[i]));
    }
    stringstream sin;
    for (int i = 0; i < collection.size(); i++)
    {
        QStandardItem *standItem = new QStandardItem(i);
        LRItem->setItem(i,0,standItem);
        for (int j = 0; j < V.size(); j++)
        {
            //QStandardItem *predictItem = new QStandardItem();
            //LRItem->setItem(i,j+1,predictItem);
            char ch = V[j];
            if (isupper(ch))
            {
                if (Goto[i][ch] != -1)
                {
                    string res;
                    stringstream ss;
                    ss << Goto[i][ch];
                    ss >> res;
                    QStandardItem *predictItem = new QStandardItem(res.c_str());
                    LRItem->setItem(i,j,predictItem);
                }
            }
            else
            {
                sin.clear();
                if (action[i][ch].type != -1)
                {
                    Content &temp = action[i][ch];
                    if (temp.type == 0)
                        sin << "S";
                    if (temp.type == 1)
                        sin << "R";
                    if (temp.type == 2)
                        sin << "acc";
                    if (temp.num != -1)
                        sin << temp.num;
                    sin >> temp.out;
                    QStandardItem *predictItem = new QStandardItem(temp.out.c_str());
                    LRItem->setItem(i,j,predictItem);
                }
            }
        }
    }
    LRTable->setModel(LRItem);
    LRTable->resize(800,600);
    LRTable->show();
    LRTableOutput->show();
}

void make_SLR_table()
{
    memset(Goto, -1, sizeof(Goto));
    //write s to the table
    for (int i = 0; i < collection.size(); i++)
        for (int j = 0; j < V.size(); j++)
        {
            char ch = V[j];
            int x = go[i][ch];
            if (x == -1)
                continue;
            if (!isupper(ch))
                action[i][ch] = Content(0, x);
            else
                Goto[i][ch] = x;
        }
    //write r and acc to the table
    for (int i = 0; i < collection.size(); i++)
        for (int j = 0; j < collection[i].element.size(); j++)
        {
            WF &tt = collection[i].element[j];
            if (tt.right[tt.right.length() - 1] == CH)
            {
                if (tt.left[0] == 'S')
                    action[i]['#'] = Content(2, -1);
                else
                    for (int k = 0; k < V.size(); k++)
                    {
                        int y = V[k];
                        //cout << "YES " << endl;
                        //cout << tt.left << "->" << tt.right << " " << tt.back << endl;
                        if (!follow[tt.left].count(V[k]))
                            continue;
                        //cout <<tt.left << "->" << tt.right << " " <<  i << " " << V[k] << " " << tt.back << endl;
                        action[i][y] = Content(1, tt.back);
                    }
            }
        }
    //新建SLR(1)表对话框并设置属性
    QDialog *SLRTableOutput=new QDialog();
    SLRTableOutput->setWindowTitle("SLR(1)分析表");
    SLRTableOutput->resize(800,600);
    //建立SLR(1)表
    QTableView *SLRTable=new QTableView(SLRTableOutput);
    //建立SLR(1)表骨架
    QStandardItemModel *SLRItem=new QStandardItemModel(SLRTable);
    //建立表头内容
    SLRItem->setColumnCount(V.size());
    for (int i = 0; i < V.size(); i++)
    {
        SLRItem->setHeaderData(i,Qt::Horizontal,QChar(V[i]));
    }
    stringstream sin;
    for (int i = 0; i < collection.size(); i++)
    {
        QStandardItem *standItem = new QStandardItem(i);
        SLRItem->setItem(i,0,standItem);
        for (int j = 0; j < V.size(); j++)
        {
            //QStandardItem *predictItem = new QStandardItem();
            //LRItem->setItem(i,j+1,predictItem);
            char ch = V[j];
            if (isupper(ch))
            {
                if (Goto[i][ch] != -1)
                {
                    string res;
                    stringstream ss;
                    ss << Goto[i][ch];
                    ss >> res;
                    QStandardItem *predictItem = new QStandardItem(res.c_str());
                    SLRItem->setItem(i,j,predictItem);
                }
            }
            else
            {
                sin.clear();
                if (action[i][ch].type != -1)
                {
                    Content &temp = action[i][ch];
                    if (temp.type == 0)
                        sin << "S";
                    if (temp.type == 1)
                        sin << "R";
                    if (temp.type == 2)
                        sin << "acc";
                    if (temp.num != -1)
                        sin << temp.num;
                    sin >> temp.out;
                    QStandardItem *predictItem = new QStandardItem(temp.out.c_str());
                    SLRItem->setItem(i,j,predictItem);
                }
            }
        }
    }
    SLRTable->setModel(SLRItem);
    SLRTable->resize(800,600);
    SLRTable->show();
    SLRTableOutput->show();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void init()
{
    //分隔产生式左部与右部
    int n=exprSet.length();
    char *s;
    for (int i = 0; i < n; i++)
    {
        QByteArray ba = exprSet[i].toLatin1();
        s=ba.data();
        int len = strlen(s), j;
        for (j = 0; j < len; j++)
            if (s[j] == '-')
                break;
        s[j] = 0;
        wf.push_back(WF(s, s + j + 2, -1, -1));
        if (!VN_dic[s])
        {
            Vn_set.push_back(s);
            VN_dic[s] = Vn_set.size();
        }
        int x = VN_dic[s] - 1;
        Vn_set[x].insert(s + j + 2);
    }
}


void MainWindow::on_enterInput_clicked()
{
    rawInput=ui->inputText->toPlainText();
    exprSet=rawInput.split('\n');
    init();
    make_item();
    make_set();
    make_V();
    make_go();
}

void MainWindow::on_getFirstSet_clicked()
{
    make_first();
}

void MainWindow::on_getFollowSet_clicked()
{
    make_follow();
}

void MainWindow::on_getLLTable_clicked()
{
    make_LL_table();
}

void MainWindow::on_getLRTable_clicked()
{
    make_LR_table();
    //新建LR(0)表对话框
}

void MainWindow::on_getSLRTable_clicked()
{
    make_SLR_table();
    //新建SLR(1)表对话框
}
