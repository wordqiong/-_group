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

}

//====================������� ===========================
//���캯��
SemanticAnalysis::SemanticAnalysis()
{
	//����ȫ�ֵķ��ű�
	tables.push_back(SemanticSymbolTable(GlobalTable, "global table"));
	//��ǰ������Ϊȫ��������
	current_table_stack.push_back(0);
	//������ʱ������
	tables.push_back(SemanticSymbolTable(TempTable, "temp variable table"));

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
		cout << "Semantic Analysis Error:main����δ����" << endl;
		//TODO:��������throw�˳���
		exit(-1);
	}
	for (int i = 0; i < production_right.size(); i++)
		symbol_list.pop_back();

	quaternion_list.insert(quaternion_list.begin(), { 0, "j","-" , "-", to_string(main_index) });
	//�޸ķ�����
	symbol_list.push_back({ production_left,"",-1,-1 });//����һ���գ���ʾmain
}


//ExtDef ::= VarSpecifier <ID> ; | FunSpecifier FunDec Block
//���������뺯������
void SemanticAnalysis::TranslateExtDef(const string production_left,
	const vector<string> production_right)
{
	//TODO:���������ô�жϵģ�Ϊɶ���������ұ����õ���ʶ����������
	
	//��������
	if (production_right.size() == 3)
	{
		//��������
		SemanticSymbol type = symbol_list[symbol_list.size() - 3];
		//������
		SemanticSymbol name = symbol_list[symbol_list.size() - 2];

		//�ж��Ƿ��Ѿ��ж���
		//��ǰ���ű�
		SemanticSymbolTable table = tables[current_table_stack.back()];
		if (table.FindSymbol(name.val) != -1)
		{
			cout << "Semantic Analysis Error:����" << name.val <<"�ض���" << endl;
			//TODO;�˳�
			exit(-1);
		}

		//δ�ض��壬������ű�
		IdentifierInfo variable;
		variable.identifier_type = IdentifierType::Variable;//�Ǹ�����
		variable.specifier_type = type.val;
		variable.identifier_name = name.val;

		//���±������뵽��ǰ�ķ��ű���
		tables[current_table_stack.back()].InsertSymbol(variable);

		//�޸ķ�����
		for (int i = 0; i < production_right.size(); i++)
			symbol_list.pop_back();
		//���浱ǰ��ʶ��������
		symbol_list.push_back({ production_left,name.val,-1,-1 });
	}
	else //��������
	{
		//TODO���������������Ҫ���뵽���ű�(���ű��Ǹ�ÿ��������һ�����ű�)���Ǳ�ʶ����Function�Ǹ�ʲô��
		SemanticSymbol name = symbol_list[symbol_list.size() - 2];

		//���������﷨��Լ����ʾ�Ӻ����ڲ������ˣ�����Ҫ������ʱ��������
		current_table_stack.pop_back();

		//�޸ķ�����
		for (int i = 0; i < production_right.size(); i++)
			symbol_list.pop_back();
		//���浱ǰ������ʶ��������
		symbol_list.push_back({ production_left,name.val,-1,-1 });
	}
}

//VarSpecifier ::= int
void  SemanticAnalysis::TranslateVarSpecifier(const string production_left, const vector<string> production_right)
{
	//��Լ�޸ķ���������
	SemanticSymbol type = symbol_list.back();
	for (int i = 0; i < production_right.size(); i++)
		symbol_list.pop_back();
	//��������ֵ
	symbol_list.push_back({ production_left,type.val,-1,-1 });//�����滻���Ȳ���ֵ������λ�ã�ֻ�ǽ��Ҳ��Լ�����
}

//FunSpecifier ::= void | int
void  SemanticAnalysis::TranslateFunSpecifier(const string production_left, const vector<string> production_right)
{
	//ͬ�����޸ķ�����
	SemanticSymbol type = symbol_list.back();
	for (int i = 0; i < production_right.size(); i++)
		symbol_list.pop_back();
	//���溯������
	symbol_list.push_back({ production_left,type.val,-1,-1 });//�����滻���Ȳ���ֵ������λ�ã�ֻ�ǽ��Ҳ��Լ�����
}
void  SemanticAnalysis::TranslateFunDec(const string production_left, const vector<string> production_right)
{


}
void  SemanticAnalysis::TranslateCreateFunTable_m(const string production_left, const vector<string> production_right)
{

}
void  SemanticAnalysis::TranslateParamDec(const string production_left, const vector<string> production_right)
{

}
void  SemanticAnalysis::TranslateBlock(const string production_left, const vector<string> production_right)
{

}
void  SemanticAnalysis::TranslateDef(const string production_left, const vector<string> production_right)
{

}
void  SemanticAnalysis::TranslateAssignStmt(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateExp(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateAddSubExp(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateItem(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateFactor(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateCallStmt(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateCallFunCheck(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateArgs(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateReturnStmt(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateRelop(const string production_left, const vector<string> production_right)
{

}

//IfStmt ::= if IfStmt_m1 ( Exp ) IfStmt_m2 Block IfNext
void SemanticAnalysis::TranslateIfStmt(const string production_left, const vector<string> production_right)
{
	SemanticSymbol IfStmt_m2 = symbol_list[symbol_list.size() - 3];
	SemanticSymbol IfNext = symbol_list[symbol_list.size() - 1];

	//���û��else��ֻ��Ҫ����IfStmt_m2��������Ԫʽ
	if (IfNext.val.empty())
	{
		//����ڣ����뵽block��
		quaternion_list[backpatching_list.back()].result = IfStmt_m2.val;
		backpatching_list.pop_back();

		//�ٳ��ڣ�����if�������next_quaternary_index���������һ��
		quaternion_list[backpatching_list.back()].result = to_string(next_quaternion_index);
		backpatching_list.pop_back();
	}
	else
		//��else����Ҫ����������Ԫʽ
	{
		//if����ڣ������������ڣ���ӦBlockִ�����������jump����תĿ��
		quaternion_list[backpatching_list.back()].result = to_string(next_quaternion_index);
		backpatching_list.pop_back();

		//if����ڣ���IfStmt_m2װ��Ĵ�������Ԫʽ����ʾexpΪ��ʱ�ĳ���
		quaternion_list[backpatching_list.back()].result = IfStmt_m2.val;
		backpatching_list.pop_back();

		//if�ٳ���
		quaternion_list[backpatching_list.back()].result = IfNext.val;
		backpatching_list.pop_back();
	}

	//�޸�symbol list
	for (int i = 0; i < production_right.size(); i++)
		symbol_list.pop_back();
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

	//�������ʽ����ڣ�������,TODO
	quaternion_list.push_back({ next_quaternion_index++ ,"j=","-","-","" });
	backpatching_list.push_back(quaternion_list.size() - 1);

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
		for (int i = 0; i < production_right.size(); i++)
			symbol_list.pop_back();
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

}
void SemanticAnalysis::TranslateWhileStmt_m1(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateWhileStmt_m2(const string production_left, const vector<string> production_right)
{

}


