#include"gram.h"
#include<sys/types.h>
#include <assert.h>

# define ISTOKEN(i)	((i) < ntokens)
# define ISVAR(i)	((i) >= ntokens)



//�Ҳ�����ʽ�ķ��� ����ż��϶�Ӧ����
//eg rsymbol[2]=A; rsymbol[1]=a;
//B->Aa (1);
// C->Ba (2);
//ritem[0]=2;ritem[1]=1;ritem[2]=-1-(1)=-2;
extern int* ritem;
extern int nritems;//�Ҳ�����ʽ�����˶��ٸ�����
extern int nrules;//���� B->Aa|bC;��ֺ��ж��ٸ�����
extern int ntokens;//�ж��ٸ��ս��
extern int nvars;//���ٸ����ս��
extern int nsyms;//һ�����ٸ�����


//һ��symbol_list��ʲô��
//A->ABaCd
// ��ֺ� ÿ��list����һ��symbol�������NULL��β����־�����
// ����ʽ����
//A A B a C d NULL
struct symbol_list
{
    union
    {
        symbol* sym;
        uniqstr type_name;
    }content;
    location location;
    /* The list.  */
    struct symbol_list* next;
};

//rule ��Ŷ�Ӧ�Ĺ��� �ֳ��󲿷��ź��Ҳ����Ŵ洢
struct rule
{
    rule_number user_number;//�ķ��е�index
    rule_number number;//rule�е�index
    symbol* lhs;
    item_number* rhs;
};

extern rule* rules;

