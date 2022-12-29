#include"grammar.h"
#include"LR1.h"
#define FILE_OPEN_ERROR 1
#define CODE_FILE_OPEN_ERROR 2

int main()
{
	cout << "****************************************" << endl;
	cout << "LR(1)�ʷ����﷨�����������" << endl;
	cout << "****************************************" << endl << endl;
	cout << "ϵͳĬ�϶����ķ��ļ�grammar.txt" << endl << endl;

	string gfile_name = "grammar.txt";
	string b;
	cout << "������Ҫ������ķ��ļ����������ѡ��Ĭ���ļ���������e��\n";
	cin >> b;
	if (b != "e")
	{
		gfile_name = b;
		cout << "�Ѷ����ļ�" << gfile_name << endl;
	}

	else {
		cout << "�Ѷ���Ĭ���ļ�" << gfile_name << endl;
	}
	//TODO ����λ�õĴ������������ô�������
	//TODO ����Դ�ļ��Ĵ������������ô����
	//TODO ����Ĭ������ļ���ΪXXX ��������ģ��������Ӧ���ļ���·������ȷ������
	//TODO ����ĸ���
	try
	{
		cout << "���ڶ��ķ����з��������Ժ�..." << endl;
		LR1_Grammar lr1_grammar(gfile_name);
		lr1_grammar.checkClosure();
		lr1_grammar.getClosureSum();
		lr1_grammar.computeACTION_GOTO();
		lr1_grammar.printTables();

		cout << "������ķ�����������../work_dir�ļ����в鿴\n1. ����ʽ�����ļ�Grammar_Rules.txt\n2. ��ʼ�հ����ļ�Zero_Closure.txt\n3. ACTION���GOTO��Tables.csv��" << endl;
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
		cout << "���ڽ����﷨��������������Ժ�..." << endl;

		int mark = lr1_grammar.analyze(res.analysis_res);

		lr1_grammar.semantic_analysis.PrintQuaternion(quaternion_path);

		if (mark == 0)
		{
			cout << "�﷨����������ѽ���������../work_dir�ļ����в鿴\n1. �ķ����������ļ�Analysis_Process.txt\n2. ���������Ԫʽ�����ļ�Quaternion_List.txt��" << endl;
			cout << "�Ƿ���Ҫ���ɹ�Լ�﷨��ͼƬ����Ҫ��װgraphviz����y/n��" << endl;
			string ans;
			cin >> ans;
			if (ans == "y")
			{
				lr1_grammar.generateTree(res.analysis_res);
				system("dot -Tpng ../work_dir/Parse_Tree.dot -o ../work_dir/Parse_Tree.png");
				cout << "�﷨�����������ɣ�����../work_dir�ļ����в鿴\n1. �﷨��ͼƬParse_Tree.png��" << endl;
			}
		}
		else if (mark == 1)
			cout << "�ķ�����������ACTION����ҳ�������../work_dir�ļ����в鿴�ķ����������ļ�Analysis_Process.txt��" << endl;
		else
			cout << "�ķ�����������GOTO����ҳ�������../work_dir�ļ����в鿴�ķ����������ļ�Analysis_Process.txt��" << endl;

	}
	catch (int a)
	{
		if (a == FILE_OPEN_ERROR)
			cout << "������ķ��ļ���ʧ�ܣ������Ӧ��·��" << endl;
		else if (a == CODE_FILE_OPEN_ERROR)
			cout << "�����code�ļ���ʧ�ܣ������Ӧ��·��" << endl;
		return 0;
	}

	cout << endl;
	cout << "���������" << endl;
	return 0;
}
