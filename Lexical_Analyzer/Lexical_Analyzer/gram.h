#include<iostream>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)
typedef int item_number;
typedef int rule_number;
typedef char const* uniqstr;
typedef int location;
typedef int symbol_number;
typedef char const* symbol_name_list;

//symbol�� �ս�� ���ս������ ����undefined Ĭ���ķ��������򻯴���
typedef enum
{
    unknown_sym,          /* Undefined.  */
    token_sym,		/* Terminal. */
    nterm_sym		/* Non-terminal */
} symbol_class;


//һ��symbol ��Ҫ��ʲô
//���� ���� ��Ӧ��constchar*�±� 
//���ǽ����ű��д洢��һ���ط���ͨ��number���ʶ�Ӧ��λ��
struct symbol
{
    uniqstr tag;//the symbol of name

    //���ǲ���һ���򵥵�char*����ɣ���¼�±�
    //location number�ȼۣ��򻯴���
    location location;
    symbol_number number;
    symbol_class class_;
    int user_token_number;
};

//����rule_number�ı�ţ����� ���������������������
item_number rule_number_as_item_number(rule_number ruleno) {
    return -1 - ruleno;
};
//����symbol_number ����itemnumber
item_number symbol_number_as_item_number(symbol_number number) {
    return number;
};

//��item_numberת��rule_number
rule_number item_number_as_rule_number(item_number item)
{
    return -1 - item;
}