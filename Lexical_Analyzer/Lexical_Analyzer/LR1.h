#pragma once
#include "grammar.h"
#include <vector>
#include <map>

class LR1_item
{
private:
	int left;//��������ű��
	vector<int> right;//�Ҳ������ű��
	int dot_site;//���ĵ��λ��
	int forward;//��ǰ���ķ��ű�� 
	int grammar_index;//���LR1�����ĸ�����ʽ������
public:
	LR1_item(int l, vector<int>& r, int ds, int fw, int gi);
	bool operator==(LR1_item& item);

};

////������Ŀ��һ������
//class Item_Sum
//{
//private:
//	set<LR1_item> item_sum;
//public:
//	int getItemIndex(LR1_item& item);
//};

class LR1_closure
{
private:
	vector<LR1_item> key_item;//�ñհ��Ĺؼ���Ŀ����û���ò�֪��
	vector<LR1_item> closure;//��Ŀ�հ�
public:
	//TODO:�����Ҫ����Ҫ��Ҫ����
	bool isIn(LR1_item item);//���ظ���Ŀ�Ƿ��ڸñհ���
	bool operator==(LR1_closure& closure);
};

//class Closure_Sum
//{
//private:
//	set<LR1_closure> closure_sum;
//public:
//	int getClosureIndex(LR1_closure&);
//};

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
	REJECT
};

class ACTION_item
{
public:
	ACTION_Option op;
	int serial;//��ţ�����op���ƽ�����Լ����ܶ��в�ͬ�ĺ���
};

class LR1_Grammar:grammar
{
private:
	set<LR1_item> item_sum;//�����е���Ŀ��Ҳ���б����
	set<LR1_closure> closure_sum;//���п��ܳ��ֵıհ����൱�ڱ����
	map<pair<int, int>, int> DFA;//ǰ���pair��<closure�ı�ţ����ŵı��>����Ӧ���������ӵ�Ŀ��closure���
								//�൱�ھ��Ǳ�ʾ���ӹ�ϵ    
	//ACTION���DFA���������ڹ�Լ���ε��ù�Լʱ��ʾ��Լ����ʽ���
	map<pair<int, int>, ACTION_item> ACTION;//ACTION��
	//GOTO����Ƿ��ս����״̬֮�䣬ֻ��״̬ת�ƻ��
	map<pair<int, int>, GOTO_Option> GOTO;
	
public:
	//�õ�������Ŀ
	int getItemSum(); //��grammar�̳е�rules���ӿ�ʼ����ʽ��ʼ��ʹ����Ŀ���е�һ���Ǳհ�

	LR1_closure computeClosure(vector<LR1_item>);//������Ŀ����հ�

	//�õ����бհ�����ʼ�հ���0�űհ����ڹ����оͰ�DFAȷ���ˣ���������û����ţ��������������е�
	int getClosureSum();

	//����ACTION��
	int computeACTION();

	//����GOTO��
	int computeGOTO();

};
