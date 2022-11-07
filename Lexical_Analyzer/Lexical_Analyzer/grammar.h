#pragma once

#include<iostream>
#include<vector>
#include<set>
#include<string>
#include<fstream>
#include<algorithm>
using namespace std;

typedef enum
{
    unknown_sym,          /* Undefined.  */
    token_sym,		/* Terminal. */
    nterm_sym,		/* Non-terminal */
    epsilon, /* null */
    end /* end terminal*/
}symbol_class;
typedef const char* uniqstr;

//���������������
class symbol {
public:
    symbol_class type;//�ķ���������
    set<int> first_set;//��¼��symbol��first���Ŷ�Ӧ��symbol���±�
    set<int>follow_set;//��¼��symbol��follow���Ŷ�Ӧ��symbol���±�
    uniqstr tag;//������

    symbol(symbol_class type, const string tag);
};

//�����ķ��Ľṹ
class rule {
public:
    int left_symbol;
    vector<int> right_symbol;

    rule(const int left, const vector<int>& right);
    void print();
};

const string EpsilonToken = "@";
const string SplitToken = " | ";// ����ʽ�Ҳ��ָ���
const string ProToken = "::=";// ����ʽ���Ҳ��ָ���
const string EndToken = "#";// βtoken ��ֹ����
const string StartToken = "Program";// �ķ���ʼ����
const string ExtendStartToken = "S";// ��չ�ķ���ʼ����
const string AllTerminalToken = "%token";//���е��ս��

class grammar {
public:
    vector<symbol>symbols;
    set<int>terminals;
    set<int>non_terminals;
    vector<rule>rules;
    int start_location;//��ʼ����ʽ��rules�е�λ��

    grammar(const string file_path);
    void ReadGrammar(const string file_path);
    void print();

    int Find_Symbol_Index_By_Token(const string token);

    void InitFirst();
    
    set<int>GetFirst(const vector<int>& str);
    //
protected:
    bool haveStartToken;
    bool haveAllTerminalToken;
    bool haveExtendStartToken;
    bool haveEndToken;
    void initGrammar();
    void checkGrammar();

};