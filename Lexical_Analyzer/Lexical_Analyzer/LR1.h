#pragma once
#include "grammar.h"
#include "analysis.h"
#include <vector>
#include <map>
#include <queue>
#include <fstream>
#include <iomanip>

const string out_Table_path = "../work_dir/Tables.csv";//������ַ
const string analysis_process_path = "../work_dir/Analysis_Process.txt";//�����Լ��ַ
const string closure_test_path = "../work_dir/Zero_Closure.txt";//����հ����Ե�ַ

class LR1_item
{
public:
	int left;//��������ű��
	vector<int> right;//�Ҳ������ű��
	int dot_site;//���ĵ��λ��
	int forward;//��ǰ���ķ��ű�� 
	int grammar_index;//���LR1�����ĸ�����ʽ������,��ʵ�������࣬�����index���Ѿ�����left��right
public:
	void print();
	LR1_item() { left = 0; dot_site = 0; forward = 0; grammar_index = 0; right.push_back(0); };
	LR1_item(int l, vector<int>& r, int ds, int fw, int gi);
	bool operator==(const LR1_item& item);
	void LR1_itemInit(int l, vector<int>& r, int ds, int fw, int gi);
};


class LR1_closure
{
public:
	vector<LR1_item> key_item;//�ñհ��Ĺؼ���Ŀ����û���ò�֪��
	vector<LR1_item> closure;//��Ŀ�հ�
public:
	//TODO:�����Ҫ����Ҫ��Ҫ����
	bool isIn(LR1_item item);//���ظ���Ŀ�Ƿ��ڸñհ���
	bool operator==(LR1_closure& clos);
	map<int, vector<int>> getShiftinSymbol();//�õ����ƽ����ַ��Լ���Ŀ�ڱհ��е�λ��
	vector<pair<int, int>> getReduceSymbol();//�õ����Թ�Լ�ķ��źͶ�Ӧ�Ĳ���ʽ�����
	void print(const vector<symbol>symbols);
};

//�����DFA��ÿһ��DFA�еıհ�����һ��״̬��Ҫ��ACTION��GOTO������ʹ�õ� 

//ACTION���п��Դ��ڵĶ���
enum ACTION_Option
{
	SHIFT_IN,//�ƽ�
	REDUCE,//��Լ
	ACCEPT,//����
	REJECT//�ܾ�
};
//GOTO���п��Դ��ڵĶ���
enum GOTO_Option
{
	GO,
	REJECT_
};

class ACTION_item
{
public:
	ACTION_Option op;
	int serial;//��ţ�����op���ƽ�����Լ����ܶ��в�ͬ�ĺ���

	ACTION_item(ACTION_Option ac_op, int num);
	ACTION_item();
};

class GOTO_item
{
public:
	GOTO_Option op;
	int serial;//��ţ���GOTO����ֻ��ת�Ʊ�ţ����Ǳհ����еı��

	GOTO_item(GOTO_Option goto_op, int num);
	GOTO_item();
};

class LR1_Grammar :public grammar
{
public:
	vector<LR1_item> item_sum;//�����е���Ŀ��setû�б��
	vector<LR1_closure> closure_sum;//���п��ܳ��ֵıհ����൱�ڱ����
	map<pair<int, int>, int> DFA;//ǰ���pair��<closure�ı�ţ����ŵı��>����Ӧ���������ӵ�Ŀ��closure���
	//�൱�ھ��Ǳ�ʾ���ӹ�ϵ    
//ACTION���DFA���������ڹ�Լ���ε��ù�Լʱ��ʾ��Լ����ʽ���
	map<pair<int, int>, ACTION_item> ACTION;//ACTION��
	//GOTO����Ƿ��ս����״̬֮�䣬ֻ��״̬ת�ƻ��
	map<pair<int, int>, GOTO_item> GOTO;

	LR1_item start_item; //��ʼ��Ŀ
	LR1_closure start_closure; //��ʼ��Ŀ�հ�
	LR1_Grammar(){};
	LR1_Grammar(const string file_path);

public:
	//��ʼ��start_item��start_closure
	int checkClosure(); //��grammar�̳е�rules���ӿ�ʼ����ʽ��ʼ��ʹ����Ŀ���е�һ���Ǳհ�

	LR1_closure computeClosure(vector<LR1_item>);//������Ŀ����հ�

	//�жϱհ��������Ƿ��иñհ������з�����ţ���û�з���-1
	int getClosureIndex(LR1_closure& clos);

	//�õ����бհ�����ʼ�հ���0�űհ����ڹ����оͰ�DFAȷ���ˣ���������û����ţ��������������е�
	void getClosureSum();

	//����ACTION���GOTO��
	void computeACTION_GOTO();

	//��ӡACTION��GOTO��Ϊcsv�ļ�
	void printTables();

	//���й�Լ���ڹ����н��д�ӡ
	void analyze(vector<unit>& lexical_res);
}; 