#pragma once
#include"derives.h"

//first goto closure����
//derives��������������߼���ܾͺ������
//ʣ�µ��� derives����˸��ַ��ս�������Ĳ���ʽ
//nullable����˸��ֿ��Բ����մ���ʽ��
//�˿� ��Ҫ������� ���first ���� closure�ļ���

//�˴����ɷ��Ӱ�
typedef int* firstSet;
typedef int first_number;
struct First
{
	symbol_number number;//��ǰ���ս����symbol���е�λ��
	symbol name;//symbol���� ����������ս�����Ƿ��ս��
	//���ǵ� rules[]�� ����ruleno ���Եõ����Ҳ����ս��
	//���ǵ� derives[]�� ����һ�����ս�����Բ���������ʽ��  Ҳ���Ҳ��ı��ʽ 
	//���ǵ�nullable�� ������ ��Ӧ��ruleno �ܷ�����մ���
	//����first����ʵ���Ѷ� Ӧ�ò���
	//���first���� ��ʲô����ţ�

	firstSet firstsSet;
	first_number fisrt_numbers;//��ǰ���ϴ��˶��ٸ�
};

//derives[]��һ������ derives[0] A, derives[1]B  
//derives��һ��rule_list ���� ����
//derives[1]��һ������
//derives[2]��һ������
//....
//Relop(symbol)  relop > ���� relop < ����relop >= ����relop <= ����relop == ����relop !=
//nullable  ruleno  
//rules[] ���е�rule
First* set;//������symbol�� ÿ���±� ��symbol����ķ���ֹ����Ӧ 

//��first����
//������ô��
//���ȿ��ҵ��ĵ�һ������ ���ս�� ��������
//�Ƿ��ս��������nullable�� ����ܲ����Ƶ��մ� 
//�Ʋ������Ǿ� ��������ս�� �ӵ�first������   ���Ҫprint �Ǿ�һ����ҾͿ���  //�п��ܳ��ֻ�������������������Ϊ�ķ������
//���Ƶ����ٿ���һ������ ֱ������


