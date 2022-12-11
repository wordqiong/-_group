#include"semantic.h"
using namespace std;
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
		symbol_list.push_back({ production_left,"",-1,-1,-1,-1 });
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
* ���ܣ��������ʽ��ΪProgram��ʽ��
*
*
*/
void SemanticAnalysis::TranslateProgram(const string production_left,
	const vector<string> production_right)
{


}


void SemanticAnalysis::TranslateExtDef(const string production_left,
	const vector<string> production_right)
{


}

void  SemanticAnalysis::TranslateVarSpecifier(const string production_left, const vector<string> production_right)
{

}
void  SemanticAnalysis::TranslateFunSpecifier(const string production_left, const vector<string> production_right)
{

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
void SemanticAnalysis::TranslateIfStmt(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateIfStmt_m1(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateIfStmt_m2(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateIfNext(const string production_left, const vector<string> production_right)
{

}
void SemanticAnalysis::TranslateIfStmt_next(const string production_left, const vector<string> production_right)
{

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


