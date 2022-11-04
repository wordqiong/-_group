#include "reader.h"

typedef struct rule_list
{
    struct rule_list* next;
    rule* value;
}rule_list;

rule_list*** derives;


static void print_derives(void)
{
    //���derives ����ǰ�����Ӳ���ʽ
    //��һ�������
}

void derives_compute(void)
{
    symbol_number i;
    rule_number r;
    rule** q;

    rule_list** dset =(rule_list**) calloc(nvars, sizeof * dset);

    rule_list* delts = (rule_list*)calloc(nrules, sizeof * delts);

    for (r = nrules - 1; r >= 0; --r)
    {
        //��ʼ��������
        symbol_number lhs = rules[r].lhs->number;
        rule_list* p = &delts[r];

        p->next = dset[lhs - ntokens];
        p->value = &rules[r];
        dset[lhs - ntokens] = p;
    }

    derives = (rule_list***)calloc(nvars, sizeof * derives);
    q = (rule **)calloc(nvars + nrules, sizeof * q);

    //��ʼתΪ����
    for (i = ntokens; i < nsyms; i++)
    {
        rule_list* p = dset[i - ntokens];
        derives[i - ntokens] = q;
        while (p)
        {
            *q++ = p->value;
            p = p->next;
        }
        *q++ = NULL;//��־�ŵ�ǰ����ʽ�������

    }
    //�����ط���
    if (debug)
    {
        print_derives();
    }
    free(dset);
    free(delts);
}
bool* nullable = NULL;
static void nullable_print()
{
    //����մ��ļ�
}

//����մ� Ҳ������
void nullable_compute(void)
{
    rule_number ruleno;
    symbol_number* s1;
    symbol_number* s2;
    rule_list* p;

    symbol_number* squeue =(symbol_number*) calloc(nvars, sizeof * squeue);

    size_t* rcount = (size_t*)calloc(nrules, sizeof * rcount);
    rule_list** rsets =(rule_list**) calloc(nvars, sizeof * rsets);

    rule_list* relts =(rule_list*) calloc(nritems + nvars + 1, sizeof * relts);

    nullable = (bool*)calloc(nvars, sizeof * nullable);

    s1 = s2 = squeue;
    p = relts;

    for (ruleno = 0; ruleno < nrules; ruleno++)
    {
        //Ĭ����������й����õ���
        rule* rules_ruleno = &rules[ruleno];
        if (rules_ruleno->rhs[0] >= 0)
        {
            //�Ҳ�����Ƿǿ�RHS
            item_number* rp = NULL;
            bool any_tokens = false;
            for (rp = rules_ruleno->rhs; *rp >= 0;rp++)
            {
                if (ISTOKEN(*rp))
                    any_tokens = true;
            }
            if (!any_tokens)
            {
                //˵���ұ߶��Ƿ��ս������ô��Ҫmarkһ��
                for (rp = rules_ruleno->rhs; *rp >= 0; rp++)
                {
                    //�������������ruleno���ֵĴ���
                    rcount[ruleno]++;
                    //����rsets��
                    p->next = rsets[*rp - ntokens];
                    p->value = rules_ruleno;
                    rsets[*rp - ntokens] = p;
                    p++;
                }
            }
        }
        else {
            //�ǹ����� Ϊ���� Ϊ�մ�
            //rhsΪת�ú�ĸ��� ��Ҫ�����
            assert(rules_ruleno->rhs[0] == ruleno);
            if (!nullable[rules_ruleno->lhs->number - ntokens])
            {
                nullable[rules_ruleno->lhs->number - ntokens] = true;
                *s2++ = rules_ruleno->lhs->number;
            }
        }
    }
    while (s1 < s2)
    {
        for (p = rsets[*s1++ - ntokens]; p; p = p->next)
        {
            rule* r = p->value;
            //������Ϊ�㣬˵���ù�����Ҳ��������Ƶ���NULL
            //Ҳ����˵ 
            if (--rcount[r->number] == 0)
            {
                if (!nullable[r->lhs->number - ntokens])
                {
                    nullable[r->lhs->number - ntokens] = true;
                    *s2++ = r->lhs->number;
                }
            }
        }
    }
    free(squeue);
    free(rcount);
    free(rsets);
    free(relts);
}

void
nullable_free(void)
{
    free(nullable);
}
