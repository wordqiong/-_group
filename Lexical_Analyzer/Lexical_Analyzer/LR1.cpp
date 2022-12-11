#include "LR1.h"

LR1_item::LR1_item(int l, vector<int>& r, int ds, int fw, int gi)
{
	this->left = l;
	this->right = r;
	this->dot_site = ds;
	this->forward = fw;
	this->grammar_index = gi;
}
void LR1_item::LR1_itemInit(int l, vector<int>& r, int ds, int fw, int gi)
{
	this->left = l;
	this->right = r;
	this->dot_site = ds;
	this->forward = fw;
	this->grammar_index = gi;
}
bool LR1_item::operator==(const LR1_item& item)
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
	//����д�Ĳ��ԣ�ֻ�ж���clos����Ŀ�ǲ����ڱ�closure�ڣ�û�з����жϣ��������ĵȺ������ǲ���ȷ�ģ���������size����ȵĲŻᵽ����
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
		if (MAP.find(present_symbol) != MAP.end())
			MAP[present_symbol].push_back(i);//������Ŀ�����
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
	//this->start_closure = computeClosure(vector<LR1_item>(1, this->start_item));��
	//��ʼ�հ����뵽�հ�������
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
			if (next_closure_site == -1)//������
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
void LR1_Grammar::printTables()
{
	const int width = 10;
	const int interval = 10;
	const int start_state = 5;
	const int state_action = 20;
	const int action_goto = 230;

	ofstream ofs(out_Table_path, ios::out);
	ofs.setf(std::ios::left);
	ofs << "STATE" << "," << "ACTION" << "," << "GOTO" << endl;

	int rowNum = this->closure_sum.size();
	int columnNum = this->symbols.size();
	int actionNum = this->terminals.size();
	int gotoNum = this->non_terminals.size();
	vector<int> terminal_site;//��¼�ս����symbols��������
	vector<int> non_terminal_site;//��¼���ս����symbols��������

	ofs << ",";

	/*************
	Ҫȷ��terminal��non_terminal��������symbols���ܺ�
	***************/
	for (int i = 0; i < columnNum; i++)
	{
		if (terminals.find(i) != terminals.end())
		{
			if (symbols[i].tag == ",")
				ofs << "\"" << symbols[i].tag << "\"" << ",";
			else
				ofs << symbols[i].tag << ",";
			terminal_site.push_back(i);
		}
	}

	for (int i = 0; i < columnNum; i++)
	{
		if (non_terminals.find(i) != non_terminals.end())
		{

			ofs << symbols[i].tag << ",";
			non_terminal_site.push_back(i);
		}
	}

	ofs << endl;

	for (int i = 0; i < rowNum; i++)
	{
		ofs << i << ",";
		for (int j = 0; j < actionNum; j++)
		{
			auto it = ACTION.find(pair<int, int>(i, terminal_site[j]));
			if (it != ACTION.end())
			{
				if (it->second.op == ACTION_Option::SHIFT_IN)
					ofs << "s" << it->second.serial << ",";
				else if (it->second.op == ACTION_Option::REDUCE)
					ofs << "r" << it->second.serial << ",";
				else if (it->second.op == ACTION_Option::ACCEPT)
					ofs << "acc" << ",";
			}
			else
				ofs << ",";
		}
		for (int j = 0; j < gotoNum; j++)
		{
			auto it = GOTO.find(pair<int, int>(i, non_terminal_site[j]));
			if (it != GOTO.end())
				ofs << it->second.serial << ",";
			else
				ofs << ",";
		}

		ofs << endl;
	}
	ofs.close();
}

int LR1_Grammar::analyze(vector<unit>& lexical_res)
{
	vector<int> status_stack;//״̬ջ
	vector<int> symbol_stack;//����ջ
	int step = 0;

	unit end(EndToken, EndToken);
	lexical_res.push_back(end);//�����봮�������Ͻ�������
	status_stack.push_back(0);//״̬ջ��ѹ��״̬0
	symbol_stack.push_back(Find_Symbol_Index_By_Token(EndToken));//�ڷ���ջ���ȷ����������

	//���������ʼ��
	semantic_analysis = SemanticAnalysis();
	semantic_analysis.AddSymbolToList({ StartToken,"",-1,-1 });

	ofstream ofs(analysis_process_path, ios::out);

	const int width = 5;
	const int interval = 10;
	const int start_step = 10;
	const int step_status = 20;
	const int status_symbol = 30;
	const int symbol_lex = 150;

	ofs << setw(start_step) << "STEP" << setw(step_status) << "STATUS_STACK" << setw(status_symbol) << "SYMBOL_STACK" << setw(symbol_lex) << "INPUT" << endl;

	//�����һ��
	ofs << setw(start_step) << step;
	ofs << setw(start_step);
	for (int t = 0; t < status_stack.size(); t++)
		ofs << " " << status_stack[t];
	ofs << setw(status_symbol);
	for (int t = 0; t < symbol_stack.size(); t++)
		ofs << symbols[symbol_stack[t]].tag;
	ofs << setw(symbol_lex);
	for (int t = 0; t < lexical_res.size(); t++)
		ofs << lexical_res[t].value;
	ofs << endl;
	step++;

	int err_code = 0;
	int reduce_error_status = -1;
	int reduce_error_symbol = -1;
	//��ʼ�����﷨����
	for (int i = 0; i < lexical_res.size(); i++)
	{
		//ÿ�δ����봮����һ���ս�����������ջ�У�����ǰ״̬�������÷��ź�ACTION���еĲ���
		//�����ת�ƣ���״̬ջ�м���ת�ƺ��״̬
		//����ǹ�Լ����ʹ�ù�Լ���򣬽�����ջ���漰��Լ������Ҳ���ʽ��״̬ջ��ɾȥ��ͬ������״̬������GOTO���в��ʱ״̬�����÷��ս��Ӧת�Ƶ�����
		//����ת�ƺ��״̬ѹ��״̬ջ
		//������ACTION��Ϊaccʱ����������reject����ACTION�����Ҳ���ת�ƣ����������GOTO���Ҳ���Ҳ�Ǵ���
		string present_terminal = lexical_res[i].type;
		int present_terminal_serial = Find_Symbol_Index_By_Token(present_terminal);
		int present_status = status_stack.back();
		auto it = ACTION.find(pair<int, int>(present_status, present_terminal_serial));

		//�����ڣ���REJECT�������˳�
		if (it == ACTION.end())
		{
			err_code = 1;
		}
		else
		{
			switch (it->second.op)
			{
			case ACTION_Option::SHIFT_IN:
			{
				//�ƽ�
				status_stack.push_back(it->second.serial);//��״̬��ջ
				symbol_stack.push_back(present_terminal_serial);//������ս��ѹջ
				
				//SHIFT_IN �ƽ���Ҳ��Ҫ�ƽ���������ķ�����
				semantic_analysis.AddSymbolToList({ lexical_res[i].type ,lexical_res[i].value ,-1,-1 });
				
				break;
			}
			case ACTION_Option::REDUCE:
			{
				//��Լ��Ҫ��Լ��ǰ���봮����һ����
				i--;
				rule rule_need = rules[it->second.serial];//Ҫʹ�õĲ���ʽ
				int right_length = rule_need.right_symbol.size();//Ҫ��Լ���ĳ���
				if (right_length == 1)
				{
					//����epsilon����Ϊ���size��1����ʵ��length��0
					if (rule_need.right_symbol[0] == Find_Symbol_Index_By_Token("@"))
						right_length = 0;
				}
				for (int k = 0; k < right_length; k++)
				{
					status_stack.pop_back();//״̬ջ�Ƴ�
					symbol_stack.pop_back();//����ջ�Ƴ�
				}
				symbol_stack.push_back(rule_need.left_symbol);//����ջѹ����ս��

				int temp_status = status_stack.back();

				//��Լ֮��鿴GOTO��
				auto goto_it = GOTO.find(pair<int, int>(temp_status, rule_need.left_symbol));
				if (goto_it == GOTO.end())//������ת�ƣ���Ӧ�˳�GOTO���������
				{
					err_code = 2;
					reduce_error_status = temp_status;
					reduce_error_symbol = rule_need.left_symbol;
					break;
				}
				else
				{
					if (goto_it->second.op == GOTO_Option::GO)
					{
						status_stack.push_back(goto_it->second.serial);//����״̬ѹջ
					
						//�����������Լ��ʹ�ù�Լʱ�Ĳ���ʽ�����﷨������Լ��ȫ�ɹ�ʱ����
						vector<string> production_right;
						for (int i = 0; i < rule_need.right_symbol.size(); i++)
							production_right.push_back(symbols[rule_need.right_symbol[i]].tag);

						semantic_analysis.Analysis(symbols[rule_need.left_symbol].tag, production_right);

					}
					else//�������
					{
						err_code = 2;
						break;
					}
				}
				break;
			}
			case ACTION_Option::ACCEPT:
			{
				//����״̬��ֱ�ӷ���
				ofs << endl << "Parse successfully!" << endl;
				ofs.close();
				return err_code;
				break;
			}
			case ACTION_Option::REJECT:
			{
				err_code = 1;
				break;
			}
			default:
				break;
			}
		}
		//��error��ֱ���˳�
		if (err_code == 1)
		{
			ofs << endl << "Parse Error:Non-existed action!" << endl;
			ofs << "Present Status: " << present_status << endl;
			ofs << "Present Terminal Type: " << present_terminal << endl;
			break;
		}
		else if (err_code == 2)
		{
			ofs << endl << "Parse Error:Non-existed goto!" << endl;
			ofs << "Present Status: " << reduce_error_status << endl;
			ofs << "Present NonTerminal: " << symbols[reduce_error_symbol].tag << endl;
			break;
		}

		//�����һ��
		ofs << setw(start_step) << step;
		ofs << setw(start_step);
		for (int t = 0; t < status_stack.size(); t++)
		{
			ofs << " " << status_stack[t];
		}
		ofs << setw(status_symbol);
		for (int t = 0; t < symbol_stack.size(); t++)
		{
			ofs << symbols[symbol_stack[t]].tag;
		}
		ofs << setw(symbol_lex);
		for (int t = i + 1; t < lexical_res.size(); t++)
		{
			ofs << lexical_res[t].type;
		}
		ofs << endl;
		step++;
	}
	ofs.close();
	return err_code;
}
void LR1_Grammar::generateTree(vector<unit>& lexical_res)
{
	int node_serial = 0;
	vector<int> status_stack;//״̬ջ
	vector<int> symbol_stack;//����ջ
	vector<int> serial_stack;//���ڼ�¼����ջ�е�tmpֵ
	int step = 0;

	unit end(EndToken, EndToken);
	lexical_res.push_back(end);//�����봮�������Ͻ�������
	status_stack.push_back(0);//״̬ջ��ѹ��״̬0
	symbol_stack.push_back(Find_Symbol_Index_By_Token(EndToken));//�ڷ���ջ���ȷ����������

	ofstream ofs(tree_dot_path, ios::out);
	ofs << "digraph mygraph {\n";
	step++;
	int err_code = 0;
	for (int i = 0; i < lexical_res.size(); i++)
	{
		string present_terminal = lexical_res[i].type;
		int present_terminal_serial = Find_Symbol_Index_By_Token(present_terminal);
		int present_status = status_stack.back();
		auto it = ACTION.find(pair<int, int>(present_status, present_terminal_serial));

		//�����ڣ���REJECT�������˳�
		if (it == ACTION.end())
		{
			err_code = 1;
		}
		else
		{
			switch (it->second.op)
			{
			case ACTION_Option::SHIFT_IN:
			{
				//�ƽ�
				status_stack.push_back(it->second.serial);//��״̬��ջ
				symbol_stack.push_back(present_terminal_serial);//������ս��ѹջ
				serial_stack.push_back(node_serial);
				ofs << "n" << node_serial++ << "[label=\"" << lexical_res[i].value << "\",color=red];" << endl;
				break;
			}
			case ACTION_Option::REDUCE:
			{
				//��Լ��Ҫ��Լ��ǰ���봮����һ����
				i--;
				rule rule_need = rules[it->second.serial];//Ҫʹ�õĲ���ʽ
				int right_length = rule_need.right_symbol.size();//Ҫ��Լ���ĳ���
				if (right_length == 1)
				{
					//����epsilon����Ϊ���size��1����ʵ��length��0
					if (rule_need.right_symbol[0] == Find_Symbol_Index_By_Token("@"))
						right_length = 0;
				}
				vector<int> drop;
				for (int k = 0; k < right_length; k++)
				{
					status_stack.pop_back();//״̬ջ�Ƴ�
					symbol_stack.pop_back();//����ջ�Ƴ�
					drop.push_back(serial_stack.back());
					serial_stack.pop_back();
				}
				symbol_stack.push_back(rule_need.left_symbol);//����ջѹ����ս��
				int temp_status = status_stack.back();

				//��Լ֮��鿴GOTO��
				auto goto_it = GOTO.find(pair<int, int>(temp_status, rule_need.left_symbol));
				if (goto_it == GOTO.end())//������ת�ƣ���Ӧ�˳�GOTO���������
				{
					err_code = 2;
					break;
				}
				else
				{
					if (goto_it->second.op == GOTO_Option::GO)
					{
						status_stack.push_back(goto_it->second.serial);//����״̬ѹջ
						serial_stack.push_back(node_serial);
					}
					else//�������
					{
						err_code = 2;
						break;
					}
				}

				ofs << "n" << node_serial++ << "[label=\"" << symbols[rule_need.left_symbol].tag << "\"];\n";
				if (right_length == 0)
				{
					ofs << "e" << node_serial << "[label=\"@\"];\n";
					ofs << "n" << node_serial - 1 << " -> " << "e" << node_serial << ";\n";
				}
				else
				{
					for (auto t = drop.begin(); t != drop.end(); t++)
						ofs << "n" << node_serial - 1 << " -> " << "n" << *t << ";\n";
				}

				break;
			}
			case ACTION_Option::ACCEPT:
			{
				//����״̬��ֱ�ӷ���
				ofs << "}";
				ofs.close();
				return;
				break;
			}
			case ACTION_Option::REJECT:
			{
				err_code = 1;
				break;
			}
			default:
				break;
			}
		}
		//��error��ֱ���˳�
		if (err_code != 0)
			break;
		step++;
	}
	ofs.close();

}
void LR1_item::print() {

}
void LR1_closure::print(const vector<symbol>symbols)
{
	//������е�key
	fstream file_open;
	file_open.open(closure_test_path, ios::out);
	for (int i = 0; i < key_item.size(); i++)
	{
		file_open << "rule" << i << "  ";
		file_open << "left: " << symbols[key_item[i].left].tag;
		file_open << "  right: ";
		for (int j = 0; j < key_item[i].right.size(); j++)
		{
			if (j == key_item[i].dot_site)
			{
				file_open << " * ";
			}
			file_open << symbols[key_item[i].right[j]].tag << " ";
		}
		if (key_item[i].dot_site == key_item[i].right.size())
			file_open << " * ";
		file_open << "     terim:  " << symbols[closure[i].forward].tag;
		file_open << endl;
	}
	//������е�key
	for (int i = 0; i < closure.size(); i++)
	{
		file_open << "rule" << i << "  ";
		file_open << "left: " << symbols[closure[i].left].tag;
		file_open << "  right: ";
		for (int j = 0; j < closure[i].right.size(); j++)
		{
			if (j == closure[i].dot_site)
			{
				file_open << " * ";
			}
			file_open << symbols[closure[i].right[j]].tag << " ";
		}
		if (closure[i].dot_site == closure[i].right.size())
			file_open << " * ";
		file_open << "     terim:  " << symbols[closure[i].forward].tag;
		file_open << endl;
	}
}
LR1_closure LR1_Grammar::computeClosure(vector<LR1_item> lr1)
{
	//���������
	//������հ�
	LR1_closure closure_now;
	closure_now.key_item = lr1;
	closure_now.closure = lr1;
	//����������
	for (int i = 0; i < closure_now.closure.size(); i++) {
		//����ǰrule
		LR1_item item_now = closure_now.closure[i];
		//���*�����һ��λ��
		if (item_now.dot_site >= item_now.right.size())
		{
			continue;
		}
		//��ǰrule�£�dot��ķ��Ŷ�Ӧ���±�
		int dot_next_symbol_index = item_now.right[item_now.dot_site];
		symbol dot_next_symbol = symbols[dot_next_symbol_index];
		//��ʼ�����ж�
		// �������������Ǹ��մ�����ô����Ϊ���
		if (dot_next_symbol.type == symbol_class::epsilon)
		{
			closure_now.closure[i].dot_site++;
			continue;
		}
		//�������������Ǹ��ս�� �ǾͲ��ü�
		if (dot_next_symbol.type == symbol_class::token_sym)
		{
			continue;
		}
		//�������������Ǹ����ս������������ս����first�ӽ���
		//��dot����ӵڶ�����ʼ���е��ַ��ͼ�����ս���ϲ�����һ��first����
		vector<int>BetaA(item_now.right.begin() + item_now.dot_site + 1, item_now.right.end());
		BetaA.push_back(item_now.forward);
		//��ʼ�����
		set<int> BetaAset = GetFirst(BetaA);
		//A->����B��,a 
		//B->XX ,first(��,a)
		//��ɴ˲������
		//��������rule���ҵ���Ӧ��rule����
		for (int j = 0; j < rules.size(); j++)
		{
			rule rule_now = rules[j];
			if (dot_next_symbol_index != rule_now.left_symbol)
				continue;
			//��ʼ���뵽closure�� 
			//�˴�����Ҫ�ж��Ҳ�����ʽ�ǲ��ǿմ�
			//�մ� dotλ����ĩ��
			//����first
			for (auto it = BetaAset.begin(); it != BetaAset.end(); it++) {
				//closure���Ƿ������
				bool have_exist = false;
				bool is_epsilon = false;
				is_epsilon = (symbols[rule_now.right_symbol[0]].type == symbol_class::epsilon);
				for (auto temp = closure_now.closure.begin(); temp != closure_now.closure.end(); temp++)
				{
					if (*temp == LR1_item(rule_now.left_symbol, rule_now.right_symbol, have_exist, *it, j))
					{
						have_exist = true;
						break;
					}
				}
				//���û��
				if (!have_exist)
				{
					closure_now.closure.push_back(LR1_item(rule_now.left_symbol, rule_now.right_symbol, is_epsilon, *it, j));
				}

			}
		}

	}
	return closure_now;
}
//check
int LR1_Grammar::checkClosure()
{
	//related var 
	// vector<LR1_item> item_sum
	// LR1_item start_item
	// vector<rule>rules;
	// start_location

	start_item.LR1_itemInit(rules[0].left_symbol, rules[0].right_symbol, 0, Find_Symbol_Index_By_Token(EndToken), start_location);
	vector<LR1_item>lr1;
	lr1.push_back(start_item);
	start_closure = computeClosure(lr1);
	start_closure.print(this->symbols);
	return 0;

}
LR1_Grammar::LR1_Grammar(const string file_path)
{
	ReadGrammar(file_path);
	//��ʼ�� �����ս����first
	print();
	//��ʼ�����з��ս����first
	InitFirst();
}