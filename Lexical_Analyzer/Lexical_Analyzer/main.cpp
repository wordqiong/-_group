#include"grammar.h"
#include"LR1.h"
int main()
{
	cout << "****************************************" << endl;
	cout << "LR(1)�ʷ��﷨������" << endl;
	cout << "****************************************" << endl << endl;
	cout << "ϵͳĬ�϶����ķ��ļ�grammar.txt" << endl << endl;

	LR1_Grammar lr1_grammar("grammar.txt");
	lr1_grammar.checkClosure();
	lr1_grammar.getClosureSum();
	lr1_grammar.computeACTION_GOTO();
	lr1_grammar.printTables();

	cout << "�Ѷ����ķ��ļ�������../work_dir�ļ����в鿴\n1. ����ʽ�����ļ�Grammar_Rules.txt\n2. ��ʼ�հ����ļ�Zero_Closure.txt\n3. ACTION���GOTO��Tables.csv��" << endl;
	cout << endl;
	cout << "ϵͳĬ�϶���Դ������ļ�·��code_in.txt" << endl;
	cout << "���ڽ��дʷ����������Ժ�..." << endl;
	//cout << "��鿴�ļ�·���Ƿ���ȷ(y/n)" << endl;
	//string pin;
	//cin >> pin;
	//if (pin != "y")
	//{
	//	cout << "���������ļ�" << endl;
	//	return 0;
	//}
	analysis res;
	res.getStrBuffer();

	cout << "�ʷ������ѽ���������../work_dir�ļ����в鿴\n1. Ԥ��������ļ�Pre-Processed_Code.txt\n2. �ʷ���������ļ�Lexical_Result.txt\n3. �ʷ��������Ŷ�Ӧ��Word_Lable.txt" << endl;
	cout << endl;
	cout << "���ڽ����﷨���������Ժ�..." << endl;

	int mark = lr1_grammar.analyze(res.analysis_res);
	if (mark == 0)
	{
		lr1_grammar.generateTree(res.analysis_res);
		system("dot -Tpng ../work_dir/Parse_Tree.dot -o ../work_dir/Parse_Tree.png");
		cout << "�﷨�����ѽ���������../work_dir�ļ����в鿴\n1. �ķ����������ļ�Analysis_Process.txt\n2. �﷨��ͼƬParse_Tree.png��" << endl;
	}
	else if (mark == 1)
		cout << "�﷨����������ACTION����ҳ�������../work_dir�ļ����в鿴�ķ����������ļ�Analysis_Process.txt��" << endl;
	else
		cout << "�﷨����������GOTO����ҳ�������../work_dir�ļ����в鿴�ķ����������ļ�Analysis_Process.txt��" << endl;

	return 0;
}