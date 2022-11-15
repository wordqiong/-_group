#include "grammar.h"
#define FILE_OPEN_ERROR 1
#define DEBUG 0

//���ߺ���
//�ַ����ָ��  
vector<string> split(const string& str, const string& pattern) {
	vector<string> res;
	if (str == "")
		return res;
	//���ַ���ĩβҲ����ָ����������ȡ���һ��
	string strs = str + pattern;
	size_t pos = strs.find(pattern);

	while (pos != strs.npos)
	{
		string temp = strs.substr(0, pos);
		res.push_back(temp);
		//ȥ���ѷָ���ַ���,��ʣ�µ��ַ����н��зָ�
		strs = strs.substr(pos + pattern.length(), strs.size());
		pos = strs.find(pattern);
	}

	return res;
}


//ȥ����β �ո��
void Trimmed(std::string& str) {
	str.erase(0, str.find_first_not_of(" \r\n\t\v\f"));
	str.erase(str.find_last_not_of(" \r\n\t\v\f") + 1);
	if (DEBUG)
		std::cout << "Trimmed [" << str << "]" << std::endl;
}


//first���� follow���Ϻϲ�����ĺ��� 
//��һ�����ս������ؼ��Ϻϲ�����һ�����ս��
// ���ǰ�߲����ڿմ�����ô��ζ�� ����Ҳ��Ӧ�ô��ڿմ�
int mergeSet(set<int>& src, set<int>& dst, int null_index)
{
	set<int>temp;
	set_difference(src.begin(), src.end(), dst.begin(), dst.end(), inserter(temp, temp.begin()));
	//���dst��û�пմ����Ѳ����մ���ɾ��
	bool isNullExist = dst.find(null_index) != dst.end();
	int pre_size = dst.size();
	dst.insert(temp.begin(), temp.end());
	if (!isNullExist)
	{
		dst.erase(null_index);
	}
	return pre_size < dst.size();
}

/************* symbol *****************/
symbol::symbol(symbol_class type, const string tag) {

	this->type = type;
	this->tag = tag;
}

/************* rule *****************/

rule::rule(const int left, const vector<int>& right) {

	this->left_symbol = left;
	this->right_symbol = right;
}

/************* grammar *****************/

grammar::grammar(const string file_path) {

	ReadGrammar(file_path);
	//��ʼ�� �����ս����first
	print();
	//��ʼ�����з��ս����first
	InitFirst();

}


void  grammar::initGrammar()
{
	if (!haveEndToken)
	{
		symbols.push_back(symbol(symbol_class::end, EndToken));
		terminals.insert(symbols.size() - 1);
	}
	if (!haveStartToken)
	{
		//log_error;
	}
	if (!haveAllTerminalToken)
	{
		//logerror;
	}
	if (!haveExtendStartToken)
	{
		//loginfo;
		int left_symbol;
		vector<int>right_symbol_index;

		symbols.push_back(symbol(symbol_class::nterm_sym, ExtendStartToken));
		left_symbol = symbols.size() - 1;
		int right = Find_Symbol_Index_By_Token(StartToken);
		right_symbol_index.push_back(right);
		this->rules.push_back(rule(left_symbol, right_symbol_index));
	}
}

void  grammar::checkGrammar()
{
	int index = 0;
	index = Find_Symbol_Index_By_Token(EndToken);
	if (index == -1) {
		haveEndToken = false;
	}
	else {
		haveEndToken = true;
	}
	index = Find_Symbol_Index_By_Token(StartToken);
	if (index == -1) {
		haveStartToken = false;
	}
	else {
		haveStartToken = true;
	}
	index = Find_Symbol_Index_By_Token(ExtendStartToken);
	if (index == -1) {
		haveExtendStartToken = false;
	}
	else {

		haveExtendStartToken = true;
	}
	index = Find_Symbol_Index_By_Token(AllTerminalToken);
	if (index == -1) {
		haveAllTerminalToken = false;
	}
	else {
		haveAllTerminalToken = true;
	}

}

void grammar::ReadGrammar(const string file_path) {

	ifstream file_open;
	file_open.open(file_path, ios::in);

	symbols.push_back(symbol(symbol_class::epsilon, EpsilonToken));

	if (!file_open.is_open())
	{
		cout << "file open fail" << endl;
		throw FILE_OPEN_ERROR;
		return;
	}
	int rule_index = 0;
	int row_num = 0;
	string rule_str;
	while (getline(file_open, rule_str, '\n'))
	{
		row_num++;
		if (rule_str.empty())
			continue;
		Trimmed(rule_str);
		if (rule_str.find("$") != rule_str.npos)
			//��$��һ�γ��ֵ�λ�� ɾ����β
			rule_str.erase(rule_str.find_first_of("$"));
		if (rule_str.empty())
			continue;

		//ע�ʹ������

		//��ʼ����rule
		vector<string> process = split(rule_str, ProToken);
		if (process.size() != 2) {
			cout << "��" << row_num << "�еĲ���ʽ��ʽ����ÿ��Ӧ����ֻ��һ��\"" << ProToken << "\"���ţ�" << endl;
			cout << process[0] << " " << process[1] << endl;
			cout << process.size() << rule_str;
			//throw(GRAMMAR_ERROR);
			return;
		}
		string left = process[0];
		string right = process[1];
		Trimmed(left);
		int left_symbol = -1;
		if (left != AllTerminalToken)
		{
			left_symbol = Find_Symbol_Index_By_Token(left);
			if (left_symbol == -1)//û�ҵ�
			{
				Trimmed(left);
				symbols.push_back(symbol(symbol_class::nterm_sym, left));
				left_symbol = symbols.size() - 1;
				non_terminals.insert(left_symbol);
			}
		}
		//�󲿷��ս��������� 
		//����Ҳ��ķָ�
		vector<string>right_part = split(right, SplitToken);
		if (right_part.size() == 0)
		{
			cout << "��" << row_num << "�еĲ���ʽ��ʽ����û���ķ��ţ�" << endl;
			//throw(GRAMMAR_ERROR);
			return;
		}
		for (auto it = right_part.begin(); it != right_part.end(); it++)
		{
			if (left_symbol == -1)
			{
				Trimmed(*it);
				symbols.push_back(symbol(symbol_class::token_sym, *it));
				terminals.insert(symbols.size() - 1);
			}
			else {
				vector<int>right_symbol_index;
				//�ָ��Ҳ�
				vector<string>right_symbol_tag = split(*it, " ");
				for (auto right_it = right_symbol_tag.begin(); right_it != right_symbol_tag.end(); right_it++)
				{
					int right_index_now = Find_Symbol_Index_By_Token(*right_it);
					if (right_index_now == -1) {
						Trimmed(*right_it);
						if ((*right_it).length() == 0)
							continue;
						symbols.push_back(symbol(symbol_class::nterm_sym, *right_it));
						right_index_now = symbols.size() - 1;
						non_terminals.insert(right_index_now);
					}

					right_symbol_index.push_back(right_index_now);
				}
				this->rules.push_back(rule(left_symbol, right_symbol_index));
				//��չ�ķ�����ʽ��rule�е�λ��
				if (symbols[left_symbol].tag == ExtendStartToken)
				{
					start_location = rules.size() - 1;
					//cout << "��չ����ʽ��rule�е�λ��" << start_location << endl;
				}

			}
		}
	}

	file_open.close();
	//init symbols
	checkGrammar();
	initGrammar();


}

void grammar::print() {
	//�����ǰ���ս������
	// �����ǰ�ķ��ս������

	fstream file_open;
	file_open.open("../work_dir/Grammar_Rules.txt", ios::out);
	file_open << symbols[0].tag << " ";
	file_open << "�ս��" << endl;
	for (set<int>::iterator i = terminals.begin(); i != terminals.end(); i++)
	{
		int index = *i;
		file_open << symbols[index].tag << " ";
	}
	file_open << endl;
	file_open << "���ս��" << endl;
	for (set<int>::iterator i = non_terminals.begin(); i != non_terminals.end(); i++)
	{
		int index = *i;
		file_open << symbols[index].tag << "  ";
	}
	file_open << endl;

	//������е�rules
	for (int i = 0; i < rules.size(); i++)
	{
		file_open << "rule" << i << "  ";
		file_open << "left: " << symbols[rules[i].left_symbol].tag;
		file_open << "  right: ";
		for (int j = 0; j < rules[i].right_symbol.size(); j++)
			file_open << symbols[rules[i].right_symbol[j]].tag << " ";
		file_open << endl;
	}

}

//���ض�Ӧ������symbol�е��±꣬��������ڣ�����-1
int grammar::Find_Symbol_Index_By_Token(const string token) {

	//�������ű� �ҵ���Ӧ��λ��
	//symbol���ű�
	bool have_find = 0;
	int index = 0;
	for (int i = 0; i < symbols.size(); i++)
	{
		if (token == symbols[i].tag)
		{
			have_find = 1;
		}
		else {
			have_find = 0;
		}
		if (have_find)
		{
			index = i;
			break;
		}
		index = i;
	}
	if (have_find)
		return index;
	else {
		return -1;
	}

	return 0;
}

void grammar::InitFirst() {

	InitFirstTerm();
	InitFirstNonTerm();
	//ProcessFirst();
	PrintFirst();
}
void grammar::ProcessFirst()
{
	//������ս����first���ϰ������ս�������
	for (set<int>::iterator it = non_terminals.begin(); it != non_terminals.end(); it++)
	{
		bool is_no_term_appear = 0;
		while (1)
		{
			is_no_term_appear = 0;
			for (set<int>::iterator i = symbols[*it].first_set.begin(); i != symbols[*it].first_set.end(); i++)
			{
				if (symbols[*i].type == symbol_class::nterm_sym)
				{
					symbols[*it].first_set.erase(*i);
					is_no_term_appear = 1;
					break;
				}
			}
			if (!is_no_term_appear)
				break;
		}

	}
}
void grammar::InitFirstTerm() {
	//������ս����first���Ͼ������Ǳ���
	for (set<int>::iterator i = terminals.begin(); i != terminals.end(); i++)
	{
		symbols[*i].first_set.insert(*i);
	}
}

void grammar::InitFirstNonTerm() {
	//����Ƿ��ս��
	//�������з��ս���Ĳ���ʽ
	//�Ҳ����ս����մ�����ô����first����
	//����Ƿ��ս������������ս����first���Ϻϲ��� ��ô�ж�������ս���ܲ����Ƴ��մ����ܵû�����
	//���ܵĻ������䵽�ڽ���
	//��һ�ε�ʱ�� ������ս�����ӽ�ȥ��Ҳ�����˴ε����
	bool useful_action;
	while (1) {
		//�������ս��
		bool useful_action = 0;;
		for (set<int>::iterator i = non_terminals.begin(); i != non_terminals.end(); i++)
		{
			//����rule
			for (int j = 0; j < rules.size(); j++)
			{
				//�����һ�����ս�� �մ�
				//����� Ѱ����һ��
				if (rules[j].left_symbol != *i)
					continue;
				if (symbols[rules[j].right_symbol[0]].type == symbol_class::token_sym || symbols[rules[j].right_symbol[0]].type == symbol_class::epsilon)
				{
					useful_action = symbols[*i].first_set.insert(rules[j].right_symbol[0]).second || useful_action;
					continue;
				}
				//���ս��
				bool isAllNull = 1;
				int k = 0;
				for (k = 0; k < rules[j].right_symbol.size(); k++)
				{
					//����Ҳ����ս��
					if (symbols[rules[j].right_symbol[k]].type == symbol_class::token_sym)
					{
						useful_action = symbols[*i].first_set.insert(rules[j].right_symbol[k]).second || useful_action;

						isAllNull = false;
						break;
						//����һ���ķ�
					}
					else {
						//����Ҳ��Ƿ��ս��
						useful_action = mergeSet(symbols[rules[j].right_symbol[k]].first_set, symbols[*i].first_set, Find_Symbol_Index_By_Token(EpsilonToken)) || useful_action;
						isAllNull = symbols[rules[j].right_symbol[k]].first_set.count(Find_Symbol_Index_By_Token(EpsilonToken)) && isAllNull;

						if (!isAllNull)
							break;
					}
				}
				if (k == rules[j].right_symbol.size() && isAllNull)
					useful_action = symbols[*i].first_set.insert(Find_Symbol_Index_By_Token(EpsilonToken)).second || useful_action;
				//����Ҳ��Ƿ��ս��
				//�����Ҳ�symbol��
				//�����޿մ� �ǲ����ս��

			}

		}
		if (useful_action == 0)
			break;
	}
}
void grammar::PrintFirst()
{
	fstream file_open;
	file_open.open("../work_dir/First_Set_Test.txt", ios::out);
	//���ÿ��symbol��first����
	for (set<int>::iterator it = terminals.begin(); it != terminals.end(); it++)
	{
		file_open << "symbol name: ";
		file_open << symbols[*it].tag;
		file_open << " first set: ";

		for (set<int>::iterator i = symbols[*it].first_set.begin(); i != symbols[*it].first_set.end(); i++)
		{
			file_open << symbols[*i].tag << " ";
		}
		file_open << endl;
	}
	for (set<int>::iterator it = non_terminals.begin(); it != non_terminals.end(); it++)
	{
		file_open << "symbol name: ";
		file_open << symbols[*it].tag;
		file_open << " first set: ";

		for (set<int>::iterator i = symbols[*it].first_set.begin(); i != symbols[*it].first_set.end(); i++)
		{
			file_open << symbols[*i].tag << " ";
		}
		file_open << endl;
	}
}

//����һ���ִ����������ǵ�first����
set<int>grammar::GetFirst(const vector<int>& str) {
	set<int>first_set;
	// above all �ǲ��ǿմ�
	if (str.empty()) {
		return first_set;
	}
	//1.�жϿմ��Ƿ���Ҫ����
	//2.�ж����ս�����Ƿ��ս��
	// 3.�жϷ��ս���ܷ��Ƶ����մ�
	bool is_epsilon = true;
	int empty_location = Find_Symbol_Index_By_Token(EpsilonToken);
	for (auto i = str.begin(); i != str.end(); i++)
	{
		if (symbols[*i].type == symbol_class::token_sym)
		{
			is_epsilon = false;
			mergeSet(symbols[*i].first_set, first_set, empty_location);

			break;
		}
		if (symbols[*i].type == symbol_class::epsilon)
		{
			is_epsilon = false;
			first_set.insert(*i);
			break;
		}
		mergeSet(symbols[*i].first_set, first_set, empty_location);
		is_epsilon = symbols[*i].first_set.count(empty_location) && is_epsilon;
		if (!is_epsilon)
			break;

	}
	if (is_epsilon)
		first_set.insert(empty_location);
	return first_set;
}