#include "analysis.h"
//����ע�͵���� û����� �� ����ע�͵������ ��Ӧ��
void analysis::getStrBuffer() {
    //�Լ��������� �Ͳ��ϵ��������������
    //����/n ���� ��������  ����deleNotes 
    //��Ҫ������������� ����Ϊ��������ˣ�������ĸ��û�ж���
    //��ʱ�����Ҫ����һ������أ����� ��� �ѻ���ظ��ָ����ʵ���������ŵĶ���
    //���ˣ�������Ϻ󣬻���ؽ�����Ӧ�����Ӧ���·�
    //Ȼ��dele�����Ļ���� �͵�״̬�����з��� 
    char c = '\0';
    int buffer_flag = 0;//�������Ƿ���Ҫ��ת

    while (1)
    {

        c = fgetc(fin);
        if (c == EOF)
        {
            break;
        }

        //���������
        if (buffer_read[buffer_choose].count == BUFFER_SIZE - 2)
        {
            buffer_read[buffer_choose].buffer[buffer_read[buffer_choose].count] = c;
            int i;
            for (i = 0; i < buffer_read[buffer_choose].count; i++)
            {
                if (isDelimiter(buffer_read[buffer_choose].buffer[i]))
                {
                    int j;//�ֽ��
                    int k;
                    //��buffer_choose��ת�Ƶ�1-buffer_choose�У�
                    for (j = 0, k = i + 1; k <= buffer_read[buffer_choose].count; k++, j++)
                    {
                        buffer_read[1 - buffer_choose].buffer[j] = buffer_read[buffer_choose].buffer[k];
                    }
                    //count��С��������
                    buffer_read[1 - buffer_choose].count = j;
                    buffer_read[buffer_choose].count = i;

                    //�����ս��
                    buffer_read[1 - buffer_choose].buffer[j] = '\0';
                    buffer_read[buffer_choose].buffer[i + 1] = '\0';

                    //��������ת
                    buffer_flag = 1;

                    break;
                }
            }

        }
        else if (c == '\n' && !note_flag)
        {
            buffer_read[buffer_choose].buffer[buffer_read[buffer_choose].count] = '\0';
        }

        else {
            buffer_read[buffer_choose].buffer[buffer_read[buffer_choose].count++] = c;
            continue;//������
        }
        //���������к�/���������Ĵ���
        deleNotes();
        deleSpaces();

        if (buffer_read[buffer_choose].count > 0)
        {
            strcpy(buffer_end.buffer, buffer_read[buffer_choose].buffer);
            buffer_end.count = buffer_read[buffer_choose].count;
            //����״̬������ 
            //ע�����Ļ����� �п����ǲ��������ִ� ��������̫���� 
            //eg: "111*n"����300���ˣ��ͻ�ָ��
            buffer_read[buffer_choose].count = 0;
            //fprintf(fout, "  [ %s ] \n", buffer_read[buffer_choose].buffer);
            spearateStates();
        }

        if (buffer_flag == 1)
        {
            //��һ�� ��������ת
            buffer_read[buffer_choose].count = 0;
            buffer_choose = 1 - buffer_choose;
            buffer_flag = 0;
        }

    }
    cout << "The result of lexical analysis has been saved in the res_out.txt file." << endl;

}
//ѭ���õ�һ���µ�strbuffer  ������deleNotes�� �͵�״̬��������
void analysis::deleNotes() {
    //ɾ��ע��
    char note[BUFFER_SIZE];
    char note_count = 0;
    bool flag_qoute = 0;
    //״̬�� �����ǡ���������/����ѭ��
    for (int i = 0; buffer_read[buffer_choose].buffer[i] != '\0'; i++)
    {
        if (buffer_read[buffer_choose].buffer[i] == '"')
        {
            flag_qoute = 1 - flag_qoute;
            continue;
        }
        if (flag_qoute == 1)
            continue;
        if (buffer_read[buffer_choose].buffer[i] == '/' || note_flag == 1)
        {
            if (buffer_read[buffer_choose].buffer[i + 1] == '\0')
            {
                continue;
            }
            if (buffer_read[buffer_choose].buffer[i + 1] == '/')
            {
                //���� //״̬ ֱ��\0ֹͣ
                int j;

                for (j = i; buffer_read[buffer_choose].buffer[j] != '\0'; j++)
                {
                    note[note_count++] = buffer_read[buffer_choose].buffer[j];
                    buffer_read[buffer_choose].buffer[j] = '\0';
                }
                note[note_count] = '\0';
                fprintf(fout, "  [ %s ] --ע��\n", note);
                buffer_read[buffer_choose].count -= note_count;
                note_count = 0;

                break;

            }
            if (buffer_read[buffer_choose].buffer[i + 1] == '*' || note_flag == 1)
            {
                //����/* ״̬ 
                note_flag = 1;
                int j;
                for (j = i + 2 * (1 - note_flag); buffer_read[buffer_choose].buffer[j] != '\0'; j++)
                {

                    note[note_count++] = buffer_read[buffer_choose].buffer[j];
                    if (buffer_read[buffer_choose].buffer[j] == '*' && buffer_read[buffer_choose].buffer[j + 1] == '/')
                    {
                        note_flag = 0;
                        note[note_count++] = '/';
                        note[note_count] = '\0';
                        fprintf(fout, "  [ %s ]--ע�� \n", note);

                        buffer_read[buffer_choose].count -= note_count;
                        note_count = 0;
                        break;
                    }
                }

                if (note_flag == 0)
                    j = j + 2;

                //��ʼǰ��

                for (; buffer_read[buffer_choose].buffer[j] != '\0'; j++, i++)
                {
                    if (buffer_read[buffer_choose].buffer[j] == '\n')
                    {
                        i--;
                        continue;
                    }
                    buffer_read[buffer_choose].buffer[i] = buffer_read[buffer_choose].buffer[j];
                }

                if (note_flag) {
                    //��ζ�Ŷ���ע�ͣ�ֱ��printf
                    note[note_count] = '\0';
                    fprintf(fout, " [ %s ]--ע�� \n", note);

                    buffer_read[buffer_choose].buffer[i] = '\0';
                    buffer_read[buffer_choose].count -= note_count;
                    break;
                    



                }
                buffer_read[buffer_choose].buffer[i] = '\0';
            }

        }
    }
}
void analysis::deleSpaces() {
    //��� �Ŀո����ɾȥ
    //����Ҫ�ж�����ǲ��ǽ���ķ�Χ�� ��Ϊ ";"�϶������ǽ��

    bool flag1 = true, flag2 = true;
    for (int i = 0; buffer_read[buffer_choose].buffer[i] != '\0'; i++)
    {
        //����ɾ���ַ����ڵĿո�
        if (buffer_read[buffer_choose].buffer[i] == '"')
            flag1 = !flag1;
        //����ɾ���ո��ַ�
        if (buffer_read[buffer_choose].buffer[i] == '\'')
            flag2 = !flag2;
        if ((buffer_read[buffer_choose].buffer[i] == ' '|| buffer_read[buffer_choose].buffer[i] == '\t') && flag1 && flag2)
        {
            //�ҵ��ո�����ĩβ���ǵ�һ�����ǿո��λ��
            int j = i + 1;
            for (; buffer_read[buffer_choose].buffer[j] != '\0' && (buffer_read[buffer_choose].buffer[j] == ' '|| buffer_read[buffer_choose].buffer[j] == '\t'); j++)
            {
            }
            //����ǵ�ĩβ�ˣ�ֱ���޸�β��λ�ü���
            if (buffer_read[buffer_choose].buffer[j] == '\0')
            {
                buffer_read[buffer_choose].buffer[i] = '\0';
                break;
            }
            //����ǿ�ͷ,ֱ��ȫɾ����
            if (i == 0)
            {
                int k = i;
                for (; buffer_read[buffer_choose].buffer[j] != '\0'; j++, k++)
                    buffer_read[buffer_choose].buffer[k] = buffer_read[buffer_choose].buffer[j];
                buffer_read[buffer_choose].buffer[k] = '\0';

                // i--����Ϊԭ��i��λ���ǿո����ڱ�ɾ�����ˣ����Ի���һ����λ
                i--;
            }
            else
            {
                //���֮���ж���ո���ɾ��ֻ��һ��
                if (j - i >= 2)
                {
                    int k = i + 1;
                    for (; buffer_read[buffer_choose].buffer[j] != '\0'; j++, k++)
                        buffer_read[buffer_choose].buffer[k] = buffer_read[buffer_choose].buffer[j];
                    buffer_read[buffer_choose].buffer[k] = '\0';
                    //�Ƚ�j�ƶ���i+1��λ��
                    j = i + 1;
                    //���ﲻ��Ҫi--����
                }

                //�жϿո�ɲ�����ɾ����ֻҪ����������һ���� ��Χ����û�пո�������������� �ļ���
                //�������� a > = b ���ִ���д�����޷���𣬻Ὣ>��=֮��Ŀո���Ե�
                //bool b = 1 > = 2;

                // TODO:���Ҫ�޸ģ�������Ҫ�޸�spaceCanDelete�жϺ����Խ����������
                if (buffer_read[buffer_choose].buffer[j] != '\0' && ((spaceCanDelete(buffer_read[buffer_choose].buffer[j]) || (i > 0 && spaceCanDelete(buffer_read[buffer_choose].buffer[i - 1])))))
                    //if (buffer_read[buffer_choose].buffer[j] != '\0' && ((isDelimiter(buffer_read[buffer_choose].buffer[j]) || (i > 0 && isDelimiter(buffer_read[buffer_choose].buffer[i - 1])))))
                {
                    //�Ѻ�����ƶ���ǰ��
                    int k = i;
                    for (; buffer_read[buffer_choose].buffer[j] != '\0'; j++, k++)
                        buffer_read[buffer_choose].buffer[k] = buffer_read[buffer_choose].buffer[j];
                    buffer_read[buffer_choose].buffer[k] = '\0';

                    // i--����Ϊԭ��i��λ���ǿո����ڱ�ɾ�����ˣ����Ի���һ����λ
                    i--;
                }

            }
        }

    }

}

//״̬������buffer_end�ж�ȡ��䲢���ֳɵ���  
//����Ҫ����ע�ͣ����/ֻ�ǳ���
//������һ��Ԥ�����Ĵ��룬����ָ�Ϊ���ʽ��������ж�

void analysis::spearateStates()
{
    char word[BUFFER_SIZE];
    int count = 0;//��ǰword�е��ַ�����
    bool finish = false;
    int state = 0;//��̬��stateΪ0�ͱ�ʾ���ڳ�̬

    for (int i = 0; i <= buffer_end.count; i++)
    {
        switch (state)
        {
            //��̬
        case 0:
            switch (charKind(buffer_end.buffer[i]))
            {
            case 1://��ĸ
                word[count++] = buffer_end.buffer[i];
                state = 1;
                break;
            case 2://����
                word[count++] = buffer_end.buffer[i];
                state = 2;
                break;
            case 3://$,_
                word[count++] = buffer_end.buffer[i];
                state = 3;
                break;
            case 4://ת���ֻ�����ַ����ڲ�ʹ�ã�������Ϊһ���ַ���������
                word[count++] = buffer_end.buffer[i];
                state = 4;
                break;
            case 5:
                word[count++] = buffer_end.buffer[i];
                state = 5;
                break;
            case 6:
                word[count++] = buffer_end.buffer[i];
                state = 6;
                break;
            case 7:
                word[count++] = buffer_end.buffer[i];
                state = 7;
                break;
            case 8:
                word[count++] = buffer_end.buffer[i];
                state = 8;
                break;
            case 10:
                word[count++] = buffer_end.buffer[i];
                state = 10;
                break;
            default:
                word[count++] = buffer_end.buffer[i];
                break;
            }
            break;
        case 1:
            switch (charKind(buffer_end.buffer[i]))
            {
            case 1:case 2:case 3:
                word[count++] = buffer_end.buffer[i];
                break;
            default:
                word[count] = '\0';
                i--;
                finish = 1;
                state = 9;//����״̬
            }
            break;
        case 2:
            switch (charKind(buffer_end.buffer[i]))
            {
            case 1:
            case 2:
                word[count++] = buffer_end.buffer[i];
                break;
            case 7:
                if (buffer_end.buffer[i] == '.')
                {
                    word[count++] = buffer_end.buffer[i];
                    break;
                }
                else
                {
                    word[count] = '\0';
                    i--;
                    finish = 1;
                    state = 9;//����״̬
                }
                break;
            default:
                word[count] = '\0';
                i--;
                finish = 1;
                state = 9;//����״̬
                break;
            }
            break;
        case 3://����$_����ĸ��һ���Ĳ���
            switch (charKind(buffer_end.buffer[i]))
            {
            case 1:case 2:case 3:
                word[count++] = buffer_end.buffer[i];
                break;
            default:
                word[count] = '\0';
                i--;
                finish = 1;
                state = 9;//����״̬
                break;
            }
            break;
        case 4:
            //�ַ�����ת����������5̬�ڲ��������ﴦ������'\'
            word[count] = '\0';
            i--;
            finish = 1;
            state = 9;//����״̬
            break;
        case 5:
            word[count++] = buffer_end.buffer[i];
            if (charKind(buffer_end.buffer[i]) == '"')
            {
                //��ʱһ�����ǳ�̬�����Բ���Ҫ�ж�i��1�Ĺ�ϵ
                if (buffer_end.buffer[i - 1] == '\\')
                {
                }
                else
                {
                    word[count] = '\0';
                    finish = 1;
                    state = 9;
                }
            }
            break;
        case 6:
            word[count++] = buffer_end.buffer[i];
            if (buffer_end.buffer[i] == '\'')
            {
                word[count] = '\0';
                finish = 1;
                state = 9;
            }
            break;
        case 7:
            //Ҫ�������ַ���ֱ�ӽ���
            word[count] = '\0';
            i--;
            finish = 1;
            state = 9;
            break;
        case 8:
            switch (charKind(buffer_end.buffer[i]))
            {
            case 8:
                word[count++] = buffer_end.buffer[i];
                break;
            default:
                word[count] = '\0';
                i--;
                finish = 1;
                state = 9;
                break;
            }
            break;
        case 9://����̬
            //��ʱword�Ѿ��õ������������\0��β����״̬���ɳ�ʼ״̬
            state = 0;
            count = 0;
            finish = 0;
            i--;
            kindJudge(word);
            break;
        case 10://�ո����
            switch (charKind(buffer_end.buffer[i]))
            {
            case 10:
                word[count++] = buffer_end.buffer[i];
                break;
            default:
                word[count] = '\0';
                i--;
                finish = 1;
                state = 9;
                break;
            }
            break;
        default:
            break;
        }
        if (buffer_end.buffer[i + 1] == '\0')
        {
            word[count] = '\0';
            kindJudge(word);
            break;
        }
    }

}

//���Զ����е��ã��жϴ��Զ�������ĵ������Ͳ�������ļ�������<���ͣ�ԭֵ>
void analysis::kindJudge(char* str)
{
    int kind = 0;
    if (isKeyWord(str) == 1) //�ж��Ƿ�Ϊ�ؼ���
    {
        kind = KeyWord;
        //fprintf(fout, "  [ %s ]  ----  [ �ؼ��� ]\n", str);
    }
    else if (isSignWord(str) == 1)  //�ж��Ƿ�Ϊ��ʶ��
        kind = SignWord;
    else if (isInt(str) == 1)//�ж��Ƿ�Ϊ����
        kind = Integer;
    else if (isFloat(str) == 1)//�ж��Ƿ�Ϊ������
        kind = FloatPoint;
    else if (isMonocularOperator(str) == 1)//�ж��Ƿ�Ϊ��Ŀ�����
        kind = MonocularOperator;
    else if (isBinocularOperator(str) == 1)//�ж��Ƿ�Ϊ˫Ŀ�����
        kind = BinocularOperator;
    else if (isDelimiter(str) == 1)//�ж��Ƿ�Ϊ���        
        kind = Delimiter;
    else if (isBlank(str) == 1)//�ж��Ƿ��ǿո�
        kind = Blank;
    else if (isSeparator(str[0]) == 1&&strlen(str)==1)
        kind = Separator;
    else if (isBracketsLeft(str[0]) == 1 && strlen(str) == 1)
        kind = BracketsLeft;
    else if (isBracketsRight(str[0]) == 1 && strlen(str) == 1)
        kind = BracketsRight;
    else if (isBracketsLeftBig(str[0]) == 1 && strlen(str) == 1)
        kind = BracketsLeftBig;
    else if (isBracketsRightBig(str[0]) == 1 && strlen(str) == 1)
        kind = BracketsRightBig;
    else if (isEnd(str[0]) == 1 && strlen(str) == 1)
        kind = End;
    else
        kind = WrongWord;
    printResult(kind, str, 0);
}

void analysis::printResult(int kind, char* str, int opt)
{
    if (opt == 1)
    {
        switch (kind)
        {
        case KeyWord:
            fprintf(fout, "[�ؼ���]----[%s]\n", str);
            break;
        case SignWord:
            fprintf(fout, "[��ʶ��]----[%s]\n", str);
            break;
        case Integer:
            fprintf(fout, "[����]----[%s]\n", str);
            break;
        case FloatPoint:
            fprintf(fout, "[������]----[%s]\n", str);
            break;
        case MonocularOperator:
            fprintf(fout, "[��Ŀ�����]----[%s]\n", str);
            break;
        case BinocularOperator:
            fprintf(fout, "[˫Ŀ�����]----[%s]\n", str);
            break;
        case Delimiter:
            fprintf(fout, "[���]----[%s]\n", str);
            break;
        case WrongWord:
            fprintf(fout, "[�����]----[%s]\n", str);
            break;
        case Blank:
            fprintf(fout, "[�ո�]----[%s]\n", str);
            break;
        case Separator:
            fprintf(fout, "[�ָ���]----[%s]\n", str);
            break;
        case BracketsLeft:
            fprintf(fout, "[������]----[%s]\n", str);
            break;
        case BracketsRight:
            fprintf(fout, "[������]----[%s]\n", str);
            break;
        case BracketsLeftBig:
            fprintf(fout, "[�������]----[%s]\n", str);
            break;
        case BracketsRightBig:
            fprintf(fout, "[�Ҵ�����]----[%s]\n", str);
            break;
        case End:
            fprintf(fout, "[������]----[%s]\n", str);
            break;

        default:
            fprintf(fout, "[����]----[%s]\n", str);
            break;
        }
    }
    else
    {
        int WordKindCode = 0;
        WordKindCode = getWordKindCode(kind, str);
        switch (kind)
        {
        case KeyWord:
            fprintf(fout, "[�ؼ���]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case SignWord:
            fprintf(fout, "[��ʶ��]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Integer:
            fprintf(fout, "[����]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case FloatPoint:
            fprintf(fout, "[������]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case MonocularOperator:
            fprintf(fout, "[��Ŀ�����]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case BinocularOperator:
            fprintf(fout, "[˫Ŀ�����]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Delimiter:
            fprintf(fout, "[���]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case WrongWord:
            fprintf(fout, "[�����]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Blank:
            fprintf(fout, "[�ո�]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case Separator:
            fprintf(fout, "[�ָ���]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case BracketsLeft:
            fprintf(fout, "[������]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case BracketsRight:
            fprintf(fout, "[������]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case BracketsLeftBig:
            fprintf(fout, "[�������]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case BracketsRightBig:
            fprintf(fout, "[�Ҵ�����]----[%d]----[%s]\n", WordKindCode, str);
            break;
        case End:
            fprintf(fout, "[������]----[%d]----[%s]\n", WordKindCode, str);
            break;
        default:
            fprintf(fout, "[����]----[%s]\n", str);
        }
    }
}

int analysis::getWordKindCode(int kind, char* str)
{
    int ret = 0;
    switch (kind)
    {
    case KeyWord:
        ret = WordCode["keyword"];
        break;
    case SignWord:
        ret = WordCode["signword"];
        break;
    case Integer:
        ret = WordCode["integer"];
        break;
    case FloatPoint:
        ret = WordCode["float"];
        break;
    case MonocularOperator:
        ret = WordCode["str"];
        break;
    case BinocularOperator:
        ret = WordCode["str"];
        break;
    case Delimiter:
        ret = WordCode[";"];
        break;
    case WrongWord:
        ret = WordCode["wrongword"];
        break;
    case Blank:
        ret = WordCode["blank"];
        break;
    case Separator:
        ret = WordCode[","];
        break;
    case BracketsLeft:
        ret = WordCode["("];
        break;
    case BracketsRight:
        ret = WordCode[")"];
        break;
    case BracketsLeftBig:
        ret = WordCode["{"];
        break;
    case BracketsRightBig:
        ret = WordCode["}"];
        break;
    case End:
        ret = WordCode["#"];
        break;
    default:
        ret = -100;
        break;
    }
    //TODO:�����޸�
    return ret;
}

analysis::analysis()
{
    buffer_choose = 0;
    note_flag = 0;
    //map����ֵ
    const int keyword_size = 24;
    const int monocular_operator_size = 11;
    const int binocular_operator_size = 12;
    int cnt = 0;
    //�ؼ���
    for (int i = 0; i < keyword_size; i++)
        WordCode[keyword[i]] = ++cnt;
    //���
    for (int i = 0; i < monocular_operator_size; i++)
        WordCode[monocular_operator[i]] = ++cnt;
    for (int i = 0; i < binocular_operator_size; i++)
        WordCode[binocular_operator[i]] = ++cnt;
    WordCode["signword"] = ++cnt;
    WordCode["wrongword"] = ++cnt;
    WordCode["blank"] = ++cnt;
    WordCode[";"] = ++cnt;
    WordCode[","] = ++cnt;
    WordCode["("] = ++cnt;
    WordCode[")"] = ++cnt;
    WordCode["{"] = ++cnt;
    WordCode["}"] = ++cnt;
    WordCode["#"] = ++cnt;
    //����
    WordCode["integer"] = ++cnt;
    WordCode["float"] = ++cnt;

    //��ӡ�����ֱ���ֵ
    map<string, int>::iterator iter;
    iter = WordCode.begin();
    while (iter != WordCode.end()) {
        cout << iter->first << " : " << iter->second << endl;
        iter++;
    }
    fin = fopen("code_in.txt", "r");
    fout = fopen("res_out.txt", "w");
}
analysis::~analysis()
{
    fclose(fin);
    fclose(fout);
}
