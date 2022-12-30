#include"semantic.h"
using namespace std;
Quaternion::Quaternion(int i, string op_type, string a1, string a2, string res)
{
	this->index = i;
	this->operator_type = op_type;
	this->arg1 = a1;
	this->arg2 = a2;
	this->result = res;
}
//=======================���ű�=====================
SemanticSymbolTable::SemanticSymbolTable(const TableType type, const string name)
{
	this->type = type;
	this->name = name;
}
/*
* ��������FindSymbol
* ���ܣ����ط���id_name�ڷ��ű��е�������
* ������ű�û�и÷��ţ�����-1
*
*/
int SemanticSymbolTable::FindSymbol(const string id_name)
{
	for (int i = 0; i < this->content.size(); i++)
	{
		if (id_name == this->content[i].identifier_name)
			return i;
	}
	return -1;
}

/*
* ��������InsertSymbol
* ���ܣ�����ű��в���һ������
*
*
*/
int SemanticSymbolTable::InsertSymbol(const IdentifierInfo ident)
{
	int is_existed = this->FindSymbol(ident.identifier_name);
	if (is_existed == -1)
	{
		this->content.push_back(ident);
		return this->content.size() - 1;
	}
	else
		return -1;
}

//====================������� ===========================
//���캯��
SemanticAnalysis::SemanticAnalysis()
{
	//����ȫ�ֵķ��ű�
	tables.push_back(SemanticSymbolTable(SemanticSymbolTable::GlobalTable, "global table"));
	//��ǰ������Ϊȫ��������
	current_table_stack.push_back(0);
	//������ʱ������
	tables.push_back(SemanticSymbolTable(SemanticSymbolTable::TempTable, "temp variable table"));

	//�ӱ��1��ʼ������Ԫʽ��ţ�0������ (j, -, -, main_address)
	next_quaternion_index = 1;
	//main�����ı������Ϊ-1
	main_index = -1;
	//��ʼ������Ϊ0����ʾ����Ҫ����
	backpatching_level = 0;
	//��ʱ��������������
	tmp_var_count = 0;
}


/*
* ��������AddSymbolToList
* ���ܣ���������Ϣ����symbol_list
*
*
*/
void SemanticAnalysis::AddSymbolToList(const SemanticSymbol symbol)
{
	symbol_list.push_back(symbol);
}
bool SemanticAnalysis::CheckIdDefine(SemanticSymbol identifier, int* tb_index, int* tb_index_index) {
	bool exist = false;
	int index_table = -1;
	int index = -1;
	//�ӵ�ǰ�����ϱ���
	for (int now_layer = current_table_stack.size() - 1; now_layer >= 0; now_layer--)
	{
		SemanticSymbolTable now_table = tables[current_table_stack[now_layer]];
		*tb_index_index = now_table.FindSymbol(identifier.val);
		if (*tb_index_index != -1)
		{
			*tb_index = current_table_stack[now_layer];
			exist = true;
			break;
		}
	}
	return exist;
}
int SemanticAnalysis::CheckParNum(SemanticSymbol check, int* value) {
	int para_num = tables[check.table_index].content[check.symbol_index].function_parameter_num;
	if (para_num > *value)
	{
		return 1;
	}
	else if (para_num < *value)
	{
		return 2;
	}
	return 0;
}
void SemanticAnalysis::ProcessError(SemanticSymbol identifier, int* tb_index, int* tb_index_index, ErrorProcess type) {
	if (type == ID_UNDEFIEND) {
		if (!CheckIdDefine(identifier, tb_index, tb_index_index))
		{
			cout << "��������з�������\"" << identifier.val << "\"δ����" << endl;
			throw(SEMANTIC_ERROR_UNDEFINED);
		}
		else {
			return;
		}
	}
	else if (type == WRONG_PARAMETER_NUM) {
		int res = CheckParNum(identifier, tb_index);
		if (res == 2)
		{
			cout << "��������з������󣺺������ò������࣬��������Ϊ   " <<identifier.val<< endl;
			throw(SEMANTIC_ERROR_PARAMETER_NUM);
		}
		if (res == 1)
		{
			cout << "��������з������󣺺������ò������٣���������Ϊ   " << identifier.val<< endl;
			throw(SEMANTIC_ERROR_PARAMETER_NUM);
		}
		return;
	}
	else if (type == FUN_ID_REDEFINED) {
		//��ȫ�ֵ�table�жϺ������Ƿ��ض���
		if (tables[0].FindSymbol(identifier.val) != -1) {
			cout << "��������з�������" <<
				"����" << identifier.val <<
				"�ض���" << endl;
			throw(SEMANTIC_ERROR_REDEFINED);//�׳������Ƿ�Ҫ����
		}
	}
	else if (type == PARA_ID_REDEFINED) {
		SemanticSymbolTable& function_table = tables[*tb_index];
		//����Ѿ����й�����
		if (function_table.FindSymbol(identifier.val) != -1) {
			cout << "��������з�������" <<
				"��������" << identifier.val << "�ض���" << endl;
			throw(SEMANTIC_ERROR_REDEFINED);
		}
	}
	else if (type == ID_REDEFINED) {
		SemanticSymbolTable& current_table = tables[*tb_index];
		//�ض����򱨴�
		if (current_table.FindSymbol(identifier.val) != -1)
		{
			cout << "��������з�������"
				<< "����" << identifier.val << "�ض���" << endl;
			throw(SEMANTIC_ERROR_REDEFINED);
		}
	}
	else if (type == MAIN_UNDEFINED)
	{
		cout << "��������з�������main����δ����" << endl;
		throw(SEMANTIC_ERROR_MAIN_UNDEFINED);
	}

}
void SemanticAnalysis::PopSymbolList(int count) {
	int cnt = count;
	while (cnt--)
		symbol_list.pop_back();
}

/*
* ��������Analysis
* ���ܣ��������̣����ݲ���ʽ��ѡ���Ӧ�ķ��뷽��
*
*
*/
void SemanticAnalysis::Analysis(const string production_left, const vector<string> production_right)
{
	//Program ::= ExtDefList 
	if (production_left == "Program")
		TranslateProgram(production_left, production_right);
	//ExtDef ::= VarSpecifier <ID> ; | FunSpecifier FunDec Block ExitFunTable_m
	else if (production_left == "ExtDef")
		TranslateExtDef(production_left, production_right);
	//VarSpecifier ::= int
	else if (production_left == "VarSpecifier")
		TranslateVarSpecifier(production_left, production_right);
	else if (production_left == "FunSpecifier")
		TranslateFunSpecifier(production_left, production_right);
	else if (production_left == "FunDec")
		TranslateFunDec(production_left, production_right);
	else if (production_left == "CreateFunTable_m")
		TranslateCreateFunTable_m(production_left, production_right);
	else if (production_left == "ParamDec")
		TranslateParamDec(production_left, production_right);
	else if (production_left == "Block")
		TranslateBlock(production_left, production_right);
	else if (production_left == "Def")
		TranslateDef(production_left, production_right);
	else if (production_left == "AssignStmt")
		TranslateAssignStmt(production_left, production_right);
	else if (production_left == "Exp")
		TranslateExp(production_left, production_right);
	else if (production_left == "AddSubExp")
		TranslateAddSubExp(production_left, production_right);
	else if (production_left == "Item")
		TranslateItem(production_left, production_right);
	else if (production_left == "Factor")
		TranslateFactor(production_left, production_right);
	else if (production_left == "CallStmt")
		TranslateCallStmt(production_left, production_right);
	else if (production_left == "CallFunCheck")
		TranslateCallFunCheck(production_left, production_right);
	else if (production_left == "Args")
		TranslateArgs(production_left, production_right);
	else if (production_left == "ReturnStmt")
		TranslateReturnStmt(production_left, production_right);
	else if (production_left == "Relop")
		TranslateRelop(production_left, production_right);
	else if (production_left == "IfStmt")
		TranslateIfStmt(production_left, production_right);
	else if (production_left == "IfStmt_m1")
		TranslateIfStmt_m1(production_left, production_right);
	else if (production_left == "IfStmt_m2")
		TranslateIfStmt_m2(production_left, production_right);
	else if (production_left == "IfNext")
		TranslateIfNext(production_left, production_right);
	else if (production_left == "IfStmt_next")
		TranslateIfStmt_next(production_left, production_right);
	else if (production_left == "WhileStmt")
		TranslateWhileStmt(production_left, production_right);
	else if (production_left == "WhileStmt_m1")
		TranslateWhileStmt_m1(production_left, production_right);
	else if (production_left == "WhileStmt_m2")
		TranslateWhileStmt_m2(production_left, production_right);
	else {
		if (production_right[0] != "@") {
			int count = production_right.size();
			while (count--)
				symbol_list.pop_back();
		}
		symbol_list.push_back({ production_left,"",-1,-1 });
	}

}

/*
* ��������PrintQuaternion
* ���ܣ���ӡ��Ԫʽ���ļ�file_path��
*
*
*/
void SemanticAnalysis::PrintQuaternion(const string file_path)
{
	ofstream ofs(file_path, ios::out);

	for (int i = 0; i < this->quaternion_list.size(); i++)
	{
		Quaternion temp = quaternion_list[i];
		ofs << temp.index << "(";
		ofs << temp.operator_type << "," << temp.arg1 << "," << temp.arg2 << "," << temp.result << ")" << endl;

	}
	ofs.close();
}

/*
* ��������PrintQuaternion
* ���ܣ�����Program ::= ExtDefList
*
*
*/
void SemanticAnalysis::TranslateProgram(const string production_left,
	const vector<string> production_right)
{
	if (main_index == -1)
	{
		//cout << "Semantic Analysis Error:main����δ����" << endl;
		ProcessError({ "","",-1,-1 }, NULL, NULL, MAIN_UNDEFINED);
	}

	//�����ʼ��Ԫʽ
	quaternion_list.insert(quaternion_list.begin(), { 0, "j","-" , "-", to_string(main_index) });
	//�޸ķ�����
	PopSymbolList(production_right.size());
	symbol_list.push_back({ production_left,"",-1,-1 });//����һ���գ���ʾmain
}


//ExtDef ::= VarSpecifier <ID> ; | FunSpecifier FunDec Block
//���������뺯������
void SemanticAnalysis::TranslateExtDef(const string production_left,
	const vector<string> production_right)
{
	//TODO:���������ô�жϵģ�����ʽ�ұ߶���������ô���֣��޸ĳ����ж����һ��string�ǲ��Ƿֺ�

	//��������
	//if (production_right.size() == 3)
	if (production_right.back() == ";")
	{
		//������
		SemanticSymbol name = symbol_list[symbol_list.size() - 2];
		//��������
		SemanticSymbol type = symbol_list[symbol_list.size() - 3];

		//�ж��Ƿ��Ѿ��ж���
		//��ǰ���ű�
		ProcessError(name, &(current_table_stack.back()), NULL, ID_REDEFINED);


		//δ�ض��壬������ű�
		IdentifierInfo variable;
		variable.identifier_type = IdentifierInfo::Variable;//�Ǹ�����
		variable.specifier_type = type.val;
		variable.identifier_name = name.val;

		//���±������뵽��ǰ�ķ��ű���
		tables[current_table_stack.back()].InsertSymbol(variable);

		//�޸ķ�����
		PopSymbolList(production_right.size());
		//���浱ǰ��ʶ��������
		symbol_list.push_back({ production_left,name.val,-1,-1 });
	}
	else //��������
	{
		//TODO���������������Ҫ���뵽���ű�(���ű��Ǹ�ÿ��������һ�����ű�)���Ǳ�ʶ����Function�Ǹ�ʲô��
		//TODO:���������ұ����õ���ʶ��Ӧ����FunDec��value����Ǻ�����
		SemanticSymbol name = symbol_list[symbol_list.size() - 2];

		//���������﷨��Լ����ʾ�Ӻ����ڲ��Ե����Ϲ�Լ�����ˣ�����Ҫ������ʱ��������
		current_table_stack.pop_back();

		//�޸ķ�����
		PopSymbolList(production_right.size());
		//���浱ǰ������ʶ��������
		symbol_list.push_back({ production_left,name.val,-1,-1 });
	}
}

//VarSpecifier ::= int
void  SemanticAnalysis::TranslateVarSpecifier(const string production_left, const vector<string> production_right)
{
	//��Լ�޸ķ���������
	SemanticSymbol type = symbol_list[symbol_list.size() - 1];
	PopSymbolList(production_right.size());
	//��������ֵ
	symbol_list.push_back({ production_left,type.val,-1,-1 });//�����滻���Ȳ���ֵ������λ�ã�ֻ�ǽ��Ҳ��Լ�����
}

//FunSpecifier ::= void | int
void  SemanticAnalysis::TranslateFunSpecifier(const string production_left, const vector<string> production_right)
{
	//ͬ�����޸ķ�����
	SemanticSymbol type = symbol_list[symbol_list.size() - 1];
	PopSymbolList(production_right.size());
	//���溯������
	symbol_list.push_back({ production_left,type.val,-1,-1 });//�����滻���Ȳ���ֵ������λ�ã�ֻ�ǽ��Ҳ��Լ�����
}
void  SemanticAnalysis::TranslateFunDec(const string production_left, const vector<string> production_right)
{
	//symbol_list�����һ����int��void
	//ͬ�����޸ķ�����
	SemanticSymbol type = symbol_list[symbol_list.size() - 1];
	PopSymbolList(production_right.size());
	//���溯������
	symbol_list.push_back({ production_left,type.val,-1,-1 });//�����滻���Ȳ���ֵ������λ�ã�ֻ�ǽ��Ҳ��Լ�����
}
void  SemanticAnalysis::TranslateCreateFunTable_m(const string production_left, const vector<string> production_right)
{
	//����������
	//func_name��������func_para��������
	SemanticSymbol func_name = symbol_list.back();
	SemanticSymbol func_para = symbol_list[symbol_list.size() - 2];

	/*
	//��ȫ�ֵ�table�жϺ������Ƿ��ض���
	if (tables[0].FindSymbol(func_name.val) != -1) {
		cout << "��������з�������" <<
			"����" << func_name.val <<
			"�ض���" << endl;
		throw(SEMANTIC_ERROR_REDEFINED);//�׳������Ƿ�Ҫ����
	}*/
	ProcessError(func_name, NULL, NULL, FUN_ID_REDEFINED);

	//����������
	tables.push_back(SemanticSymbolTable(SemanticSymbolTable::FunctionTable, func_name.val));
	//��ȫ�ַ��ű�����ǰ�����ķ�������������������ڵ�ַ����л��
	tables[0].InsertSymbol({ IdentifierInfo::Function,func_para.val,
		func_name.val,0,0,int(tables.size() - 1) });
	//��������3�����Ͳ����Ǹ����

	//������ѹ��ջ
	current_table_stack.push_back(tables.size() - 1);

	//����ֵ
	IdentifierInfo return_value;
	return_value.identifier_type = IdentifierInfo::ReturnVar;
	return_value.identifier_name = tables.back().name + "_return_value";
	return_value.specifier_type = func_para.val;
	//���Ϊmain����������м�¼
	if (func_name.val == "main")
		main_index = next_quaternion_index;
	//������Ԫʽ
	quaternion_list.push_back({ next_quaternion_index++ , func_name.val,"-","-" ,"-" });
	//�������м��뷵�ر���
	tables[current_table_stack.back()].InsertSymbol(return_value);
	//�մ�����Ҫpop
	//����pushback
	symbol_list.push_back({ production_left,func_name.val,
		0,int(tables[0].content.size() - 1) });
}
void  SemanticAnalysis::TranslateParamDec(const string production_left, const vector<string> production_right)
{
	//symbol_list���һ��Ϊ�������������ڶ���Ϊ����
	SemanticSymbol name = symbol_list.back();
	SemanticSymbol type = symbol_list[symbol_list.size() - 2];


	//��ǰ������
	SemanticSymbolTable& function_table = tables[current_table_stack.back()];
	/*
	//����Ѿ����й�����
	if (function_table.FindSymbol(func_name.val) != -1) {
		cout << "��������з�������" <<
			"��������" << func_name.val << "�ض���" << endl;
		//throw(SEMANTIC_ERROR_REDEFINED);
	}*/

	ProcessError(name, &(current_table_stack.back()), NULL, PARA_ID_REDEFINED);

	//����������βα���
	int new_position = function_table.InsertSymbol(
		{ IdentifierInfo::Variable,type.val,name.val,-1,-1,-1 });
	//��ǰ������ȫ�ַ����е�����
	int table_position = tables[0].FindSymbol(function_table.name);
	//�βθ���++
	tables[0].content[table_position].function_parameter_num++;

	//������Ԫʽ
	quaternion_list.push_back({ next_quaternion_index++, "defpar","-" , "-", name.val });

	//symbol_list����
	PopSymbolList(production_right.size());
	symbol_list.push_back({ production_left,name.val,
		current_table_stack.back(),new_position });
}
void  SemanticAnalysis::TranslateBlock(const string production_left, const vector<string> production_right)
{
	//����symbol_list
	SemanticSymbol type = symbol_list[symbol_list.size() - 1];
	PopSymbolList(production_right.size());
	symbol_list.push_back({ production_left,
		to_string(next_quaternion_index),-1,-1 });
}
void  SemanticAnalysis::TranslateDef(const string production_left, const vector<string> production_right)
{
	//symbol_list�ĵ����ڶ����������������Ǳ�����������
	SemanticSymbol name = symbol_list[symbol_list.size() - 2];
	SemanticSymbol type = symbol_list[symbol_list.size() - 3];
	SemanticSymbolTable& current_table = tables[current_table_stack.back()];
	/*
	//�ض����򱨴�
	if (current_table.FindSymbol(name.val) != -1)
	{
		cout << "��������з�������"
			<< "����" << name.val << "�ض���" << endl;
		//throw(SEMANTIC_ERROR_REDEFINED);
	}*/
	ProcessError(name, &(current_table_stack.back()), NULL, ID_REDEFINED);

	current_table.InsertSymbol({ IdentifierInfo::Variable,type.val,name.val ,-1,-1,-1 });

	for (int i = 0; i < production_right.size(); i++)
		symbol_list.pop_back();
	symbol_list.push_back({ production_left, name.val,
		current_table_stack.back(),int(tables[current_table_stack.back()].content.size() - 1) });
}
void  SemanticAnalysis::TranslateAssignStmt(const string production_left, const vector<string> production_right)
{
	//AssignStmt ::= <ID> = Exp
	//1.���symbol�Ѵ���symbol_list�ĺ�
	//2.�����ƣ�ID�Ƿ񱻶����
	int s_size = symbol_list.size();
	SemanticSymbol exp = symbol_list[s_size - 1];
	SemanticSymbol idtf = symbol_list[s_size - 3];
	int tb_index = -1, tb_index_index = -1;
	//���IDδ����
	ProcessError(idtf, &tb_index, &tb_index_index, ID_UNDEFIEND);
	quaternion_list.push_back({ next_quaternion_index++,"=",exp.val,"-",idtf.val });
	//process symbol_list
	int cnt = production_right.size();
	PopSymbolList(cnt);

	symbol_list.push_back({ production_left,idtf.val,tb_index,tb_index_index });

}
void SemanticAnalysis::TranslateExp(const string production_left, const vector<string> production_right)
{
	//Exp ::= AddSubExp | Exp Relop AddSubExp
	//1.�����жϷ�ʽ
	//2.�����ƣ��޷����в��
	int r_size = production_right.size();
	int s_size = symbol_list.size();
	if (r_size == 1) {
		SemanticSymbol add_sub_exp = symbol_list[s_size - 1];
		PopSymbolList(r_size);

		symbol_list.push_back({ production_left,add_sub_exp.val,add_sub_exp.table_index,add_sub_exp.symbol_index });
	}
	else {
		SemanticSymbol exp1 = symbol_list[s_size - 3];
		SemanticSymbol op = symbol_list[s_size - 2];
		SemanticSymbol exp2 = symbol_list[s_size - 1];
		string var = "T" + to_string(tmp_var_count++);
		int lable_num_next = next_quaternion_index++;

		quaternion_list.push_back({ lable_num_next,"j" + op.val,exp1.val,exp2.val,to_string(lable_num_next+3) });
		quaternion_list.push_back({ next_quaternion_index++,"=" ,"0","-",var });
		quaternion_list.push_back({ next_quaternion_index++,"j" ,"-","-",to_string(lable_num_next + 4) });
		quaternion_list.push_back({ next_quaternion_index++,"=" ,"1","-",var });
		PopSymbolList(r_size);
		symbol_list.push_back({ production_left,var,-1,-1 });
	}
}
void SemanticAnalysis::TranslateAddSubExp(const string production_left, const vector<string> production_right)
{

	//AddSubExp ::= Item | Item + Item | Item - Item
	//1.�����жϷ�ʽ
	//2.�����ƣ��޷����в��
	int r_size = production_right.size();
	int s_size = symbol_list.size();
	if (r_size == 1) {
		SemanticSymbol add_sub_exp = symbol_list[s_size - 1];
		PopSymbolList(r_size);
		symbol_list.push_back({ production_left,add_sub_exp.val,add_sub_exp.table_index,add_sub_exp.symbol_index });
	}
	else {
		SemanticSymbol exp1 = symbol_list[s_size - 3];
		SemanticSymbol op = symbol_list[s_size - 2];
		SemanticSymbol exp2 = symbol_list[s_size - 1];
		string var = "T" + to_string(tmp_var_count++);

		quaternion_list.push_back({ next_quaternion_index++,op.val, exp1.val, exp2.val,var });
		PopSymbolList(r_size);
		symbol_list.push_back({ production_left,var,-1,-1 });
	}
}
void SemanticAnalysis::TranslateItem(const string production_left, const vector<string> production_right)
{
	//Item ::= Factor | Factor * Factor | Factor / Factor
	//1.�����жϷ�ʽ
	//2.�����ƣ��޷����в��
	int r_size = production_right.size();
	int s_size = symbol_list.size();

	if (r_size == 1) {
		SemanticSymbol item_exp = symbol_list[s_size - 1];
		PopSymbolList(r_size);
		symbol_list.push_back({ production_left,item_exp.val,item_exp.table_index,item_exp.symbol_index });
	}
	else {
		SemanticSymbol exp1 = symbol_list[s_size - 3];
		SemanticSymbol op = symbol_list[s_size - 2];
		SemanticSymbol exp2 = symbol_list[s_size - 1];
		string var = "T" + to_string(tmp_var_count++);

		quaternion_list.push_back({ next_quaternion_index++,op.val, exp1.val, exp2.val,var });
		PopSymbolList(r_size);
		symbol_list.push_back({ production_left,var,-1,-1 });
	}

}
void SemanticAnalysis::TranslateFactor(const string production_left, const vector<string> production_right)
{
	//Factor ::= <INT> | ( Exp ) | <ID> | CallStmt
	//1.�����жϷ�ʽ
	//2.�����ƣ�ID�Ƿ���
	int r_size = production_right.size();
	int s_size = symbol_list.size();
	if (r_size == 1) {
		SemanticSymbol exp = symbol_list[s_size - 1];
		if ("<ID>" == production_right[0]) {
			int tb_index = -1, tb_index_index = -1;
			ProcessError(exp, &tb_index, &tb_index_index, ID_UNDEFIEND);

		}
		PopSymbolList(r_size);
		symbol_list.push_back({ production_left,exp.val,exp.table_index,exp.symbol_index });
	}
	else {
		SemanticSymbol exp = symbol_list[s_size - 2];
		PopSymbolList(r_size);
		symbol_list.push_back({ production_left,exp.val,exp.table_index,exp.symbol_index });
	}
}
void SemanticAnalysis::TranslateCallStmt(const string production_left, const vector<string> production_right)
{
	//CallStmt ::= <ID> ( CallFunCheck Args )
	//1.һ���жϷ�ʽ
	//2.�����ƣ�ID�Ƿ��� CallFunCheck���
	//			  ���functiontalbe�����������Ƿ�����Ҫ��
	int r_size = production_right.size();
	int s_size = symbol_list.size();
	SemanticSymbol idtf = symbol_list[s_size - 5];
	SemanticSymbol chk = symbol_list[s_size - 3];
	SemanticSymbol args = symbol_list[s_size - 2];

	int value = stoi(args.val);
	int temp = -1;
	ProcessError(chk, &value, &temp, WRONG_PARAMETER_NUM);
	string var = "T" + to_string(tmp_var_count++);
	quaternion_list.push_back({ next_quaternion_index++,"call",idtf.val,"-",var });
	PopSymbolList(r_size);
	symbol_list.push_back({ production_left,var,-1,-1 });


}
void SemanticAnalysis::TranslateCallFunCheck(const string production_left, const vector<string> production_right)
{
	//CallFunCheck ::= @
	//1.һ���жϷ�ʽ
	//2.�����ƣ�func�Ƿ����(ID�Ƿ��壬�������Ƿ�Ϊfunc)
	int s_size = symbol_list.size();
	bool check_ID = false;
	bool check_ID_type = false;
	SemanticSymbol func_id = symbol_list[s_size - 2];
	int func_pos = tables[0].FindSymbol(func_id.val);
	if (func_pos == -1)
		check_ID = false;
	else
		check_ID = true;
	if (!check_ID)
	{
		cout << "δ����ĺ�����" << endl;
		throw(SEMANTIC_ERROR_UNDEFINED);
	}
	if (tables[0].content[func_pos].identifier_type != IdentifierInfo::Function)
		check_ID_type = false;
	else
		check_ID_type = true;
	if (!check_ID_type) {
		cout << "ID���壬���Ͳ���" << endl;
		throw(SEMANTIC_ERROR_UNDEFINED);
	}
	symbol_list.push_back({ production_left,func_id.val,0,func_pos });
}
void SemanticAnalysis::TranslateArgs(const string production_left, const vector<string> production_right)
{
	//Args ::= Exp , Args | Exp | @
	//1.�����жϷ�ʽ
	//2.�����ƣ��޷����в��
	int r_size = production_right.size();
	int s_size = symbol_list.size();
	if (r_size == 3)
	{
		SemanticSymbol exp = symbol_list[s_size - 3];
		int args_num = stoi(symbol_list[s_size - 1].val) + 1;
		PopSymbolList(r_size);
		quaternion_list.push_back({ next_quaternion_index++,"param",exp.val,"-","-" });
		symbol_list.push_back({ production_left,to_string(args_num),-1,-1 });
	}
	else {
		if ("Exp" == production_right[0]) {
			SemanticSymbol exp = symbol_list[s_size - 1];
			PopSymbolList(r_size);
			quaternion_list.push_back({ next_quaternion_index++,"param",exp.val,"-","-" });
			symbol_list.push_back({ production_left,"1",-1,-1 });
		}
		if ("@" == production_right[0]) {
			symbol_list.push_back({ production_left,"0",-1,-1 });
		}
	}
}
void SemanticAnalysis::TranslateReturnStmt(const string production_left, const vector<string> production_right)
{
	//ReturnStmt ::= return Exp | return
	//1.�����жϷ�ʽ
	//2.�����ƣ��޷����в��
	int r_size = production_right.size();
	int s_size = symbol_list.size();
	SemanticSymbolTable tb = tables[current_table_stack.back()];

	if (r_size == 1) {
		auto type = tables[0].content[tables[0].FindSymbol(tb.name)].specifier_type;
		if (type != "void")
		{
			cout << "��Ҫ�з���ֵ" << endl;
			throw(SEMANTIC_ERROR_NO_RETURN);
		}
		PopSymbolList(r_size);
		quaternion_list.push_back({ next_quaternion_index++,"return","-","-",tb.name });
		symbol_list.push_back({ production_left,"" - 1,-1 });
	}
	else {
		SemanticSymbol exp = symbol_list[s_size - 1];

		quaternion_list.push_back({ next_quaternion_index++,"=",exp.val,"-",tb.content[0].identifier_name });
		quaternion_list.push_back({ next_quaternion_index++,"return",tb.content[0].identifier_name,"-",tb.name });
		PopSymbolList(r_size);
		symbol_list.push_back({ production_left,exp.val,-1,-1 });
	}
}
void SemanticAnalysis::TranslateRelop(const string production_left, const vector<string> production_right)
{
	//Relop ::= > | < | >= | <= | == | !=
	//1.�����жϷ�ʽ
	//2.�����ƣ��޷����в��
	int r_size = production_right.size();
	int s_size = symbol_list.size();
	SemanticSymbol exp = symbol_list[s_size - 1];
	PopSymbolList(r_size);
	symbol_list.push_back({ production_left,exp.val,-1,-1 });

}

//IfStmt ::= if IfStmt_m1 ( Exp ) IfStmt_m2 Block IfNext
void SemanticAnalysis::TranslateIfStmt(const string production_left, const vector<string> production_right)
{
	//�ӷ������ж�ȡ��ɲ���
	SemanticSymbol IfStmt_m2 = symbol_list[symbol_list.size() - 3];
	SemanticSymbol IfNext = symbol_list[symbol_list.size() - 1];

	//���û��else��ֻ��Ҫ����IfStmt_m2��һ����Ԫʽ
	if (IfNext.val.empty())
	{
		//����ٳ��ڣ�����if�������next_quaternary_index���������һ��
		quaternion_list[backpatching_list.back()].result = to_string(next_quaternion_index);
		backpatching_list.pop_back();
	}
	else
		//��else����Ҫ����������Ԫʽ
	{
		//����if����ڣ������������ڣ���ӦBlockִ�����������jump����תĿ��
		quaternion_list[backpatching_list.back()].result = to_string(next_quaternion_index);
		backpatching_list.pop_back();

		//����if�ٳ���
		quaternion_list[backpatching_list.back()].result = IfNext.val;
		backpatching_list.pop_back();
	}

	//�޸�symbol list
	PopSymbolList(production_right.size());
	symbol_list.push_back({ production_left,"",-1,-1 });
}

//IfStmt_m1 ::= @
void SemanticAnalysis::TranslateIfStmt_m1(const string production_left, const vector<string> production_right)
{
	//IfStmt_m1ֻ��Ҫ��Ϊһ���﷨���ռλ���ɣ�����Ϊ�Ҳ��ǿգ����Բ���pop
	//���浱ǰ��һ����Ԫʽ��λ��,TODO:��ʵû��
	symbol_list.push_back({ production_left,to_string(next_quaternion_index),-1,-1 });
}

//IfStmt_m2 ::= @
//�����жϲ������ʽ����ٳ��ڣ��Խӵ���Exp���ʽ����
void SemanticAnalysis::TranslateIfStmt_m2(const string production_left, const vector<string> production_right)
{
	//if������䲼�����ʽ
	SemanticSymbol expression = symbol_list[symbol_list.size() - 2];

	//�������ʽ�ٳ���,������
	quaternion_list.push_back({ next_quaternion_index++ ,"j=",expression.val,"0","" });
	backpatching_list.push_back(quaternion_list.size() - 1);

	//�������ʽ����ڣ�����ֱ��д����
	//TODO����ʵ�о����Բ�д���j=����ֱ��д��j����д����ΪҪִ�еľ�����һ����䣬�ڻ����ʱ��Ҳ���ֳ�����
	quaternion_list.push_back({ next_quaternion_index++ ,"j=","-","-",to_string(next_quaternion_index)});

	//�޸�symbol list
	symbol_list.push_back({ production_left,to_string(next_quaternion_index),-1,-1 });

}

//IfNext ::= @ | IfStmt_next else Block
//else���п���
void SemanticAnalysis::TranslateIfNext(const string production_left, const vector<string> production_right)
{
	//TODO:���Ӧ����Ҫ�жϵģ���ȻIfStmt_next�����õ�����ȷ�Ķ���
	if (production_right.size() == 3)
	{
		SemanticSymbol IfStmt_next = symbol_list[symbol_list.size() - 3];
		//�޸�symbol list
		PopSymbolList(production_right.size());
		//��ĺ�IfStmt_nextһ�����ǵ�һ�ν�������������ת������
		symbol_list.push_back({ production_left,IfStmt_next.val,-1,-1 });
	}
	else//�ǿյ����
	{
		//ֻ��Ҫռλ�����Ҹ����մ���Ϊvalue������if��������ж�
		symbol_list.push_back({ production_left,"",-1,-1 });
	}

}

//IfStmt_next ::= @
//if��һ������֮��Ҫȥ�����һ����������ת
void SemanticAnalysis::TranslateIfStmt_next(const string production_left, const vector<string> production_right)
{
	//���һ����������ת��䣬��תλ�ô�����
	quaternion_list.push_back({ next_quaternion_index++ ,"j","-","-","" });
	backpatching_list.push_back(quaternion_list.size() - 1);

	//�޸�symbol list,�浱ǰ�������һ����Ԫʽλ��
	symbol_list.push_back({ production_left,to_string(next_quaternion_index),-1,-1 });
}
void SemanticAnalysis::TranslateWhileStmt(const string production_left, const vector<string> production_right)
{
	SemanticSymbol whilestmt_m1 = symbol_list[symbol_list.size() - 6];
	SemanticSymbol whilestmt_m2 = symbol_list[symbol_list.size() - 2];

	// ��������ת�� while �������ж���䴦
	quaternion_list.push_back({ next_quaternion_index++,"j","-","-" ,whilestmt_m1.val });

	//���������
	quaternion_list[backpatching_list.back()].result = whilestmt_m2.val;
	backpatching_list.pop_back();

	//����ٳ���
	quaternion_list[backpatching_list.back()].result = to_string(next_quaternion_index);
	backpatching_list.pop_back();

	backpatching_level--;

	PopSymbolList(production_right.size());

	symbol_list.push_back({ production_left,"",-1,-1 });
}
void SemanticAnalysis::TranslateWhileStmt_m1(const string production_left, const vector<string> production_right)
{
	backpatching_level++;
	symbol_list.push_back({ production_left,to_string(next_quaternion_index),-1,-1 });
}
void SemanticAnalysis::TranslateWhileStmt_m2(const string production_left, const vector<string> production_right)
{
	//�ٳ���
	SemanticSymbol while_exp = symbol_list[symbol_list.size() - 2];

	//�ٳ���
	quaternion_list.push_back({ next_quaternion_index++,"j=",while_exp.val,"0","" });
	backpatching_list.push_back(quaternion_list.size() - 1);
	//�����
	quaternion_list.push_back({ next_quaternion_index++ ,"j","-","-" ,"" });
	backpatching_list.push_back(quaternion_list.size() - 1);

	symbol_list.push_back({ production_left,to_string(next_quaternion_index),-1,-1 });
}

