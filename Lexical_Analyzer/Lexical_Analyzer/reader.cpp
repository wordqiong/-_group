#include "reader.h"


#define ITEM_NUMBER_MAX INT_MAX


//����ս���ͷ��ս���ķ��ż���
symbol_name_list rsymbol[];


static symbol_list* grammar;

static symbol_list* grammar_end = NULL;

//�������symbol ����symbol list��
symbol_list* symbol_list_new_sym(symbol* sym, location loc)
{
    symbol_list* res = (symbol_list*)malloc(sizeof * res);
    res->content.sym = sym;
    res->location = loc;
    res->next = NULL;

    return res;
}

static void
grammar_symbol_append(symbol* sym, location loc)
{
    symbol_list* p = symbol_list_new_sym(sym, loc);

    //���grammar���ˣ���ô��end�󲿼��Ͼ���
    if (grammar_end)
        grammar_end->next = p;
    else
        //û�еĻ���p�������ڵ�grammar
        grammar = p;
    //p�����µ�grammar_end
    grammar_end = p;

    //ǰ���Ѿ�˵�� NULL��һ���ָ�������Ҫ��������
    //A->ABaCd
    //A A B a C d NULL
    if (sym)
        //���sym����null���Ҳ�����ʽ���ż�һ
        ++nritems;
}

//TODO �����﷨���� ����grammar_symbol_append������grammar����
void gram_parse(void)
{
    //�Զ����grammar���д��� 
    //ͳ�����ж��ٸ��ս��
    //ͳ�����ж��ٸ����ս��
    //��������ķ���ӵ�grammar list��
    //���ս���ͷ��ս���ķ��� ����rsymbols�� ���Կ���д��hash
    //ע��symbol ��symbol_list��Ĵ洢
}
symbol* endtoken;//
static bool start_flag = false;


//���rule �Ķ���
static void packgram(void)
{

    unsigned int itemno = 0;
    rule_number ruleno = 0;
    symbol_list* p = grammar;
    ritem = (int *)calloc(nritems + 1, sizeof * ritem);
    rules = (rule*)calloc(nrules, sizeof * rules);
    while (p)
    {
        int rule_length = 0;
        rules[ruleno].user_number = ruleno;
        rules[ruleno].number = ruleno;
        rules[ruleno].lhs = p->content.sym;
        rules[ruleno].rhs = ritem+itemno;
        for (p = p->next; p && p->content.sym; p = p->next)
        {
            rule_length++;
            if (rule_length == INT_MAX)
            {
                assert(rule_length == INT_MAX, "rule too long");
            }
            ritem[itemno++] = symbol_number_as_item_number(p->content.sym->number);

        }
        ritem[itemno++] = rule_number_as_item_number(ruleno);
        //ExtDefList ::= ExtDef ExtDefList
        // ExtDefList ::= ExtDef
        // map<int,char*> symbol;
        //ritem  0 2 [-1-rule_index] 2 1 3 -3(-1-rule_index)
        assert(itemno < ITEM_NUMBER_MAX);
        ruleno++;
        assert(ruleno < ITEM_NUMBER_MAX);
        if (p)//p������NULL��ֻ��p.symbolΪnull
            p = p->next;
    }
}


static void check_and_convert_grammer(void)
{
    if (nrules == 0)
        assert("no rules in the input grammar");

    /* If the user did not define her ENDTOKEN, do it now. */
    if (!endtoken)
    {
        //�˴�������� ��������Լ���ĸ�ʽ end token����Ҫ�������
        /*endtoken = symbol_get("$end", empty_location);
        endtoken->class_ = token_sym;
        endtoken->number = 0;

        endtoken->user_token_number = 0;*/
    }
    if (!start_flag)
    {
        //���û�ж���start���� ����ʼ����
        symbol_list* node;
        for (node = grammar;
            node != NULL ;
            node = node->next)
        {
            for (node = node->next;
                node != NULL && node->content.sym != NULL;
                node = node->next)
                ;
        }
        assert(node != NULL);
        grammar_start_symbol_set(node->content.sym,
            node->content.sym->location);
    }

    {
        //accept 
        //��չ�ķ� �ӽ�ȥ 
        //S->S'
        symbol_list* p = symbol_list_new_sym(accept, empty_location);
        p->location = grammar->location;
        p->next = symbol_list_new_sym(startsymbol, empty_location);
        //p->next->next = symbol_list_new_sym(endtoken, empty_location);
        p->next->next->next = symbol_list_new_sym(NULL, empty_location);
        p->next->next->next->next = grammar;
        nrules += 1;
        nritems += 3;
        grammar = p;
    }
    packgram();
}

symbol* errtoken = NULL;
symbol* undeftoken = NULL;
symbol* endtoken = NULL;
symbol* accept = NULL;
symbol* startsymbol = NULL;
location startsymbol_location;

void reader(void)
{
    symbols_new();

    //Լ�� ������ķ� ��Ҫ����
    //��չ�ķ��õ�
    /* Construct the accept symbol. */
    accept = symbol_get("$accept", empty_location);
    accept->class_ = nterm_sym;
    accept->number = nvars++;

    /* Construct the error token */
    errtoken = symbol_get("error", empty_location);
    errtoken->class_ = token_sym;
    errtoken->number = ntokens++;



    //openfile
    FILE* file = fopen(filename, "r");
    //gram_parse()
    gram_parse();
    check_and_convert_grammer();

    fclose(file);

}