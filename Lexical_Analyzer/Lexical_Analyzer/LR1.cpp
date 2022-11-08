#include "LR1.h"

LR1_item::LR1_item(int l, vector<int>& r, int ds, int fw, int gi)
{
	this->left = l;
	this->right = r;
	this->dot_site = ds;
	this->forward = fw;
	this->grammar_index = gi;
}

bool LR1_item::operator==(LR1_item& item)
{
	if (this->grammar_index == item.grammar_index && this->dot_site == item.dot_site && this->forward == item.forward)
		return true;
	return false;
}

bool LR1_closure::isIn(LR1_item item)
{
	for (int i = 0; i < this->closure.size(); i++)
	{
		if (item == this->closure[i])
			return true;
	}
	return false;
}


bool LR1_closure::operator==(LR1_closure& clos)
{
	if (this->closure.size() != clos.closure.size())
		return false;

	//TODO:�������ѭ���ˣ�������ô�ܸ���һЩ
	for (int i = 0; i < clos.closure.size(); i++)
	{
		if (!this->isIn(clos.closure[i]))
			return false;
	}
	return true;
}

map<int, vector<int>> LR1_closure::getShiftinSymbol()
{
	map<int, vector<int>> MAP;
	for (int i = 0; i < this->closure.size(); i++)
	{
		LR1_item temp = this->closure[i];
		if (temp.dot_site >= temp.right.size())//�㵽ͷ�ˣ��ǹ�Լ��Ŀ
			continue;
		int present_symbol = temp.right[temp.dot_site];//��ǰ�������ַ�
		//���ڸ��ַ�
		if (MAP.find(present_symbol)!=MAP.end())
		{
			vector<int> target = MAP[present_symbol];
			target.push_back(i);//������Ŀ�����
		}
		else
		{
			vector<int> target;
			target.push_back(i);
			MAP[present_symbol] = target;//����
		}
	}
	return MAP;
}

vector<pair<int, int>> LR1_closure::getReduceSymbol()
{
	vector<pair<int, int>> VEC;
	for (int i = 0; i < this->closure.size(); i++)
	{
		LR1_item temp = this->closure[i];
		if (temp.dot_site >= temp.right.size())//�㵽ͷ�ˣ��ǹ�Լ��Ŀ
		{
			VEC.push_back(pair<int, int>(temp.forward, temp.grammar_index));
		}
		else
			continue;		
	}
	return VEC;
}

int LR1_Grammar::getClosureIndex(LR1_closure& clos)
{
	for (int i = 0; i < this->closure_sum.size(); i++)
	{
		if (clos == this->closure_sum[i])
			return i;
	}
	return -1;
}

ACTION_item::ACTION_item(ACTION_Option ac_op, int num)
{
	this->op = ac_op;
	this->serial = num;
}
ACTION_item::ACTION_item()
{
}
GOTO_item::GOTO_item(GOTO_Option goto_op, int num)
{
	this->op = goto_op;
	this->serial = num;
}
GOTO_item::GOTO_item()
{
}
void LR1_Grammar::getClosureSum()
{
	//�õ���ʼ�հ�
	this->start_closure = computeClosure(vector<LR1_item>(1, this->start_item));
	this->closure_sum.push_back(this->start_closure);

	//Ӧ�����ö���ѭ������������
	queue<LR1_closure> q;
	q.push(this->start_closure);

	while (!q.empty())
	{
		LR1_closure source = q.front();
		q.pop();

		map<int, vector<int>> shift_symbol = source.getShiftinSymbol();
		//set<int> reduce_symbol = source.getReduceSymbol();

		for (map<int, vector<int>>::iterator it = shift_symbol.begin(); it != shift_symbol.end(); it++)
		{
			//���˴�ʱ�ƽ����ַ�
			//��Ҫ��õ�����ƶ�һλ�����Ŀ��Ȼ�����vector����հ����ټ������q������հ����ϣ��������DFA��
			vector<LR1_item> next_closure_key;
			for (int j = 0; j < it->second.size(); j++)
			{
				LR1_item temp = source.closure[it->second[j]];
				temp.dot_site++;//������ƶ�һλ������������
				/* ************************************/
				//TODO:����Ҫ��Ҫ�ĳ���ţ�������Ŀ��Ҫ��Ҫ���
				/* *************************************/
				next_closure_key.push_back(temp);//�����ڸ��ַ��¿����ƽ���LR1��Ŀ
			}

			LR1_closure next_closure = computeClosure(next_closure_key);
			int next_closure_site = getClosureIndex(next_closure);
			if (next_closure_site == -1)
			{
				q.push(next_closure);
				this->closure_sum.push_back(next_closure);
			}
			//�����ڲ��ڣ�DFA��Ҫ���ϱ�
			this->DFA[pair<int, int>(getClosureIndex(source), it->first)] = getClosureIndex(next_closure);

		}

	}

}

void LR1_Grammar::computeACTION_GOTO()
{
	//�������DFA�ͱհ����󣬹���ACTION��<closure�ı�ţ����ŵı��>,����
	//��¼�������е��ս��������ֻ��¼���ڵ�״̬
	//DFA�м�¼�Ķ���ת�ƣ�����Ҫ��¼��Լ״̬�������ǲ���ʽ�����
	//���е�REJECT״̬��û�м�¼

	//����DFA�õ��ƽ�����
	for (auto it = this->DFA.begin(); it != this->DFA.end(); it++)
	{
		//ת�Ʒ������ս������¼��ACTION����
		if (terminals.find(it->first.second) != terminals.end())
		{
			ACTION_item act_item(ACTION_Option::SHIFT_IN, it->second);
			this->ACTION[pair<int, int>(it->first.first, it->first.second)] = act_item;
		}
		else
		{
			//���ս����¼��GOTO����
			GOTO_item goto_item(GOTO_Option::GO, it->second);
			this->GOTO[pair<int, int>(it->first.first, it->first.second)] = goto_item;
		}
	}

	//���ݱհ��Ĺ�Լ��õ���Լ/���ܶ���
	for (int i = 0; i < this->closure_sum.size(); i++)
	{
		vector<pair<int, int>> reduce_line = this->closure_sum[i].getReduceSymbol();
		for (int j = 0; j < reduce_line.size(); j++)
		{
			//�ڵ�i���հ�״̬�������ս��reduce_line[j].first��Ҫʹ��reduce_line[j].second����ʽ��Լ
			//**********************************************
			// ��TODO:�費��Ҫ��ǰ�������#����
			//����ù�Լ��Ŀ�ǳ�ʼ��Լ��Ŀ����Ӧ����Ϊ����״̬
			
			if (reduce_line[j].second == start_location)
			{
				ACTION_item act_item(ACTION_Option::ACCEPT, reduce_line[j].second);
				this->ACTION[pair<int, int>(i, reduce_line[j].first)] = act_item;
			}
			else
			{
				ACTION_item act_item(ACTION_Option::REDUCE, reduce_line[j].second);
				this->ACTION[pair<int, int>(i, reduce_line[j].first)] = act_item;
			}
		}

	}
}

void LR1_Grammar::printTable()
{

}

void LR1_Grammar::analyze(string file_path)
{

}
