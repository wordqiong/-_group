#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <string.h>

#include <vector>
#include <set>
#include <map>

using namespace std;

#define SEMANTIC_ERROR_UNDEFINED 1
#define SEMANTIC_ERROR_PARAMETER_NUM 2
#define SEMANTIC_ERROR_NO_RETURN 3
enum ErrorProcess {
	ID_UNDEFIEND,
	WRONG_PARAMETER_NUM
};

//��������еķ���
struct SemanticSymbol
{
	string type;//����
	string val;//ֵ
	int table_index;//��������table��index
	int symbol_index;//������table�ڲ���index
};


//��ʶ����Ϣ������������������ʱ�����������ľ�����Ϣ
struct IdentifierInfo
{
	//�����ʶ�����ͣ��ֱ�Ϊ��������������ʱ����������������ֵ
	enum IdentifierType { Function, Variable, TempVar, ConstVar, ReturnVar };
	IdentifierType identifier_type;//��ʶ��������
	string specifier_type;//��(��)������/������������
	string identifier_name;//��ʶ������/����ֵ
	int function_parameter_num;//������������
	int function_entry;//������ڵ�ַ(��Ԫʽ�ı��)
	int function_table_index;//�����ĺ������ű�����������ķ��ű��б��е�����
};

//��������еķ��ű�
struct SemanticSymbolTable {
	//���ֱ�����ͣ��ֱ�Ϊȫ�ֱ��������鼶����ʱ��
	enum TableType { GlobalTable, FunctionTable, BlockTable, TempTable };

	TableType type;//������
	vector<IdentifierInfo> content;//���ű������
	string name;//����

	//���캯��
	SemanticSymbolTable(const TableType type, const string name);

	//Ѱ��ָ����������λ��
	int FindSymbol(const string symbol_name);

	//����һ�����ţ����ؼ����λ��
	int InsertSymbol(const IdentifierInfo ident);
};

//��Ԫʽ
struct Quaternion
{
	int index;//��Ԫʽ���
	string operator_type;//��������
	string arg1;//������1
	string arg2;//������2
	string result;//���

	Quaternion(int i, string op_type, string a1, string a2, string res);
};


class SemanticAnalysis {
public:
	vector<Quaternion> quaternion_list;//��Ԫʽ��
	int main_index;//main������Ӧ����Ԫʽ���
	int backpatching_level;//������

	vector<int> backpatching_list;//���������Ԫʽ����Ԫʽ��quaternion_list�е����
	int next_quaternion_index;//��ǰλ����һ����Ԫʽ���
	int tmp_var_count;//��ʱ��������

	vector<SemanticSymbol> symbol_list;//����������̵ķ��������������Ϊ����������еķ���ջ�������﷨�����е�Ҫ����
	vector<SemanticSymbolTable> tables;//�������з��ű�
	vector<int> current_table_stack;//��ǰ�������Ӧ�ķ��ű�����ջ

	//���캯��
	SemanticAnalysis();
	//�����еķ�����Ϣ����symbol_list
	void AddSymbolToList(const SemanticSymbol symbol);
	//��������
	void Analysis(const string production_left, const vector<string> production_right);
	//��ӡ��Ԫʽ��
	void PrintQuaternion(const string file_path);

private:
	//ÿ�η��붼��Ӧ���﷨�����е�һ����Լʽ
	void TranslateProgram(const string production_left, const vector<string> production_right);
	void TranslateExtDef(const string production_left, const vector<string> production_right);
	void TranslateVarSpecifier(const string production_left, const vector<string> production_right);
	void TranslateFunSpecifier(const string production_left, const vector<string> production_right);
	void TranslateIfStmt(const string production_left, const vector<string> production_right);
	void TranslateIfStmt_m1(const string production_left, const vector<string> production_right);
	void TranslateIfStmt_m2(const string production_left, const vector<string> production_right);
	void TranslateIfNext(const string production_left, const vector<string> production_right);
	void TranslateIfStmt_next(const string production_left, const vector<string> production_right);

	//===============================================================
	void TranslateFunDec(const string production_left, const vector<string> production_right);
	void TranslateCreateFunTable_m(const string production_left, const vector<string> production_right);
	void TranslateParamDec(const string production_left, const vector<string> production_right);
	void TranslateBlock(const string production_left, const vector<string> production_right);
	void TranslateDef(const string production_left, const vector<string> production_right);
	void TranslateWhileStmt(const string production_left, const vector<string> production_right);
	void TranslateWhileStmt_m1(const string production_left, const vector<string> production_right);
	void TranslateWhileStmt_m2(const string production_left, const vector<string> production_right);

	//==============================================================
	void TranslateAssignStmt(const string production_left, const vector<string> production_right);
	void TranslateExp(const string production_left, const vector<string> production_right);
	void TranslateAddSubExp(const string production_left, const vector<string> production_right);
	void TranslateItem(const string production_left, const vector<string> production_right);
	void TranslateFactor(const string production_left, const vector<string> production_right);
	void TranslateCallStmt(const string production_left, const vector<string> production_right);
	void TranslateCallFunCheck(const string production_left, const vector<string> production_right);
	void TranslateArgs(const string production_left, const vector<string> production_right);
	void TranslateReturnStmt(const string production_left, const vector<string> production_right);
	void TranslateRelop(const string production_left, const vector<string> production_right);

	//=====================tool======================================
	bool CheckIdDefine(SemanticSymbol identifier, int* tb_index, int* tb_index_index);
	int CheckParNum(SemanticSymbol check, int* value);
	void ProcessError(SemanticSymbol identifier, int* tb_index, int* tb_index_index, ErrorProcess type);
	void PopSymbolList(int count);
};