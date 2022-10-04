#include "analysis.h"


//多行注释的情况 没有添加 即 多行注释的情况下 不应该
void analysis::getStrBuffer() {
    //自己读到东西 就不断的往缓冲区里添加
    //读到/n 或者 缓冲区满  进行deleNotes 
    //需要设置两个缓冲池 ，因为缓冲池满了，可能字母还没有读完
    //这时候就需要另外一个缓冲池，利用 界符 把缓冲池给分割开来，实现完整符号的读入
    //满了，处理完毕后，缓冲池交换，应用其对应的下符
    //然后将dele处理后的缓冲池 送到状态机进行分析 
    char c='\0';
    int buffer_flag=0;//缓冲区是否需要轮转

    while (1)
    {

        c = fgetc(fin);
        if (c == EOF)
        {
            break;
        }
       
        //缓冲池满了
        if (buffer_read[buffer_choose].count == BUFFER_SIZE - 2)
        {
            buffer_read[buffer_choose].buffer[buffer_read[buffer_choose].count] = c;
            int i;
            for (i = 0; i < buffer_read[buffer_choose].count; i++)
            {
                if (isDelimiter(buffer_read[buffer_choose].buffer[i]))
                {
                    int j;//分界点
                    int k;
                    //把buffer_choose的转移到1-buffer_choose中，
                    for (j = 0, k = i + 1; k <= buffer_read[buffer_choose].count; k++, j++)
                    {
                        buffer_read[1 - buffer_choose].buffer[j] = buffer_read[buffer_choose].buffer[k];
                    }
                    //count大小重新设置
                    buffer_read[1 - buffer_choose].count = j;
                    buffer_read[buffer_choose].count = i;

                    //设置终结点
                    buffer_read[1 - buffer_choose].buffer[j] = '\0';
                    buffer_read[buffer_choose].buffer[i+1] = '\0';

                    //缓冲区轮转
                    buffer_flag = 1;

                    break;
                }
            }

        }
        else if (c == '\n'&&!note_flag)
        {
            buffer_read[buffer_choose].buffer[buffer_read[buffer_choose].count] = '\0';
        }

        else {
            buffer_read[buffer_choose].buffer[buffer_read[buffer_choose].count++] = c;
            continue;//继续吧
        }
        //继续处理换行后/缓冲池满后的处理
        deleNotes();
        deleSpaces();

        if (buffer_read[buffer_choose].count > 0)
        {
            strcpy(buffer_end.buffer, buffer_read[buffer_choose].buffer);
            //进入状态机处理 
            //注：给的缓冲区 有可能是不完整的字串 如果传入的太长了 
            //eg: "111*n"超过300个了，就会分割开，
            buffer_read[buffer_choose].count = 0;
            //fprintf(fout, "  [ %s ] \n", buffer_read[buffer_choose].buffer);
            spearateStates();
        }

        if (buffer_flag == 1)
        {
            //下一次 缓冲区轮转
            buffer_read[buffer_choose].count = 0;
            buffer_choose = 1 - buffer_choose;
            buffer_flag = 0;
        }

    }

}//循环得到一串新的strbuffer  并经过deleNotes后 送到状态机函数中
void analysis::deleNotes() {
    //删除注释
    char note[BUFFER_SIZE];
    char note_count = 0;
    bool flag_qoute = 0;
    //状态机 读到非“”包含的/进入循环
    for (int i = 0; buffer_read[buffer_choose].buffer[i] != '\0'; i++)
    {
        if (buffer_read[buffer_choose].buffer[i] == '"')
        {
            flag_qoute = 1 - flag_qoute;
            continue;
        }
        if (flag_qoute == 1)
            continue;
        if (buffer_read[buffer_choose].buffer[i] == '/'||note_flag==1)
        {
            if (buffer_read[buffer_choose].buffer[i + 1] == '\0')
            {
                continue;
            }
            if (buffer_read[buffer_choose].buffer[i + 1] == '/')
            {
                //进入 //状态 直到\0停止
                int j;

                for (j = i; buffer_read[buffer_choose].buffer[j] != '\0'; j++)
                {
                    note[note_count++] = buffer_read[buffer_choose].buffer[j];
                    buffer_read[buffer_choose].buffer[j] = '\0';
                }
                note[note_count] = '\0';
                //fprintf(fout, "  [ %s ] --注释\n", note);
                buffer_read[buffer_choose].count -= note_count;
                note_count = 0;

                break;

            }
            if (buffer_read[buffer_choose].buffer[i + 1] == '*' || note_flag == 1)
            {
                //进入/* 状态 
                note_flag = 1;
                int j;
                for (j = i + 2*(1-note_flag); buffer_read[buffer_choose].buffer[j] != '\0'; j++)
                {

                    note[note_count++] = buffer_read[buffer_choose].buffer[j];
                    if (buffer_read[buffer_choose].buffer[j] == '*' && buffer_read[buffer_choose].buffer[j + 1] == '/')
                    {
                        note_flag = 0;
                        note[note_count++] = '/';
                        note[note_count] = '\0';
                        //fprintf(fout, "  [ %s ]--注释 \n", note);
                        
                        buffer_read[buffer_choose].count -= note_count;
                        note_count = 0;
                        break;
                    }
                }
                
                if (note_flag == 0)
                    j = j + 2;
                
                    //开始前移
                    
                    for (; buffer_read[buffer_choose].buffer[j] != '\0'; j++, i++)
                    {
                        if (buffer_read[buffer_choose].buffer[j] == '\n')
                        {
                            i--;
                            continue;
                        }
                        buffer_read[buffer_choose].buffer[i] = buffer_read[buffer_choose].buffer[j];
                    }
                
                if(note_flag) {
                    //意味着多行注释，直接printf
                    note[note_count] = '\0';
                    //fprintf(fout, " [ %s ]--注释 \n", note);
                    
                    buffer_read[buffer_choose].buffer[i] = '\0';
                    buffer_read[buffer_choose].count -= note_count;
                    break;
                    //注意的是 \n被我直接读进来了，需要进行处理



                }
                buffer_read[buffer_choose].buffer[i] = '\0';
            }
           
        }
    }
}
void analysis::deleSpaces() {
    //界符 的空格可以删去
    //但需要判断这个是不是界符的范围内 因为 ";"肯定不算是界符

    bool flag1 = true, flag2 = true;
    for (int i = 0; buffer_read[buffer_choose].buffer[i] != '\0'; i++)
    {
        //不能删除字符串内的空格
        if (buffer_read[buffer_choose].buffer[i] == '"')
            flag1 = !flag1;
        //不能删除空格字符
        if (buffer_read[buffer_choose].buffer[i] == '\'')
            flag2 = !flag2;
        if (buffer_read[buffer_choose].buffer[i] == ' ' && flag1 && flag2)
        {
            //找到空格的最后，末尾或是第一个不是空格的位置
            int j = i + 1;
            for(; buffer_read[buffer_choose].buffer[j]!='\0'&& buffer_read[buffer_choose].buffer[j] == ' ';j++)
            { }
            //如果是到末尾了，直接修改尾零位置即可
            if (buffer_read[buffer_choose].buffer[j] == '\0')
            {
                buffer_read[buffer_choose].buffer[i] = '\0';
                break;
            }
            //判断空格可不可以删除，只要左右两边有一个是 周围可以没有空格就能与其他区分 的即可
            //但是例如 a > = b 这种错误写法就无法辨别，会将>与=之间的空格给吃掉
            //bool b = 1 > = 2;
            
            // TODO:这个要修改，可能需要修改spaceCanDelete判断函数以解决上述问题
            if (buffer_read[buffer_choose].buffer[j] != '\0' && ((spaceCanDelete(buffer_read[buffer_choose].buffer[j]) || (i > 0 && spaceCanDelete(buffer_read[buffer_choose].buffer[i - 1])))))
            {
                //把后面的移动到前面
                int k = i;
                for (; buffer_read[buffer_choose].buffer[j] != '\0'; j++, k++)
                    buffer_read[buffer_choose].buffer[k] = buffer_read[buffer_choose].buffer[j];
                buffer_read[buffer_choose].buffer[k] = '\0';
                
                // TODO:这里i--有啥用？没太明白
                i--;
            }
        }

    }
    
}

//状态机，从buffer_end中读取语句并划分成单词  
//不需要考虑注释，因此/只是除法
//输入是一行预处理后的代码，将其分割为单词进行类型判定
void analysis::spearateStates()
{
    char word[BUFFER_SIZE];
    int count = 0;//当前word中的字符个数
    bool finish = false;
    int state = 0;//初态，state为0就表示了在初态

    for (int i = 0; i < buffer_end.count; i++)
    {
        switch (state)
        {
        //初态
        case 0:
            switch (charKind(buffer_end.buffer[i]))
            {
            case 1://字母
                word[count++] = buffer_end.buffer[i];
                state = 1;
                break;
            case 2://数字
                word[count++] = buffer_end.buffer[i];
                state = 2;
                break;
            case 3://$,_
                word[count++] = buffer_end.buffer[i];
                state = 3;
                break;
            case 4://转义符只会在字符串内部使用，否则作为一个字符单独出来
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
                state = 9;//结束状态
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
                    state = 9;//结束状态
                }
                break;
            default:
                word[count] = '\0';
                i--;
                finish = 1;
                state = 9;//结束状态
                break;
            }
            break;
        case 3://好像$_和字母是一样的操作
            switch (charKind(buffer_end.buffer[i]))
            {
            case 1:case 2:case 3:
                word[count++] = buffer_end.buffer[i];
                break;
            default:
                word[count] = '\0';
                i--;
                finish = 1;
                state = 9;//结束状态
                break;
            }
            break;
        case 4:
            //字符串内转义符的情况在5态内部处理，这里处理单独的'\'
            word[count] = '\0';
            i--;
            finish = 1;
            state = 9;//结束状态
            break;
        case 5:
            word[count++] = buffer_end.buffer[i];
            if (charKind(buffer_end.buffer[i]) == '"')
            {
                //此时一定不是初态，所以不需要判断i与1的关系
                if (buffer_end.buffer[i - 1] == '\\')
                { }
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
            //要结束的字符，直接结束
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
        case 9://结束态
            //此时word已经得到，并且最后以\0结尾，故状态换成初始状态
            state = 0;
            count = 0;
            finish = 0;
            i--;

            kindJudge(word);
            break;
        default:
            break;
        }
        if (buffer_end.buffer[i + 1] == '\0')
        {
            word[count] = '\0';
            kindJudge(word);
        }
    }
    
}

//在自动机中调用，判断从自动机输出的单词类型并输出到文件，类似<类型，原值>
void analysis::kindJudge(char* str)
{
    int kind = 0;
    if (isKeyWord(str) == 1) //判断是否为关键字
    {
        kind = KeyWord;
        //fprintf(fout, "  [ %s ]  ----  [ 关键字 ]\n", str);
    }
    else if (isSignWord(str) == 1)  //判断是否为标识符
        kind = SignWord;
    else if (isInt(str) == 1)//判断是否为整数
        kind = Integer;
    else if (isFloat(str) == 1)//判断是否为浮点数
        kind = FloatPoint;
    else if (isMonocularOperator(str) == 1)//判断是否为单目运算符
        kind = MonocularOperator;
    else if (isBinocularOperator(str) == 1)//判断是否为双目运算符
        kind = BinocularOperator;
    else if (isDelimiter(str) == 1)//判断是否为界符        
        kind = Delimiter;
    else if (isBlank)
        kind = Blank;
    else
        kind = WrongWord;
    printResult(kind, str, 1);
}

void analysis::printResult(int kind, char* str, int opt)
{
    if (opt == 1)
    {
        switch (kind)
        {
        case KeyWord:
            fprintf(fout, "[关键字]----[%s]\n", str);
        case SignWord:
            fprintf(fout, "[标识符]----[%s]\n", str);
        case Integer:
            fprintf(fout, "[整数]----[%s]\n", str);
        case FloatPoint:
            fprintf(fout, "[浮点数]----[%s]\n", str);
        case MonocularOperator:
            fprintf(fout, "[单目运算符]----[%s]\n", str);
        case BinocularOperator:
            fprintf(fout, "[双目运算符]----[%s]\n", str);
        case Delimiter:
            fprintf(fout, "[界符]----[%s]\n", str);
        case WrongWord:
            fprintf(fout, "[错误词]----[%s]\n", str);
        case Blank:
            fprintf(fout, "[空格]----[%s]\n", str);
        default:
            fprintf(fout, "[其他]----[%s]\n", str);
        }
    }
    else
    {
        int WordKindCode = 0;
        WordKindCode = getWordKindCode(kind, str);
        switch (kind)
        {
        case KeyWord:
            fprintf(fout, "[关键字]----[%d]----[%s]\n", WordKindCode, str);
        case SignWord:
            fprintf(fout, "[标识符]----[%d]----[%s]\n", WordKindCode, str);
        case Integer:
            fprintf(fout, "[整数]----[%d]----[%s]\n", WordKindCode, str);
        case FloatPoint:
            fprintf(fout, "[浮点数]----[%d]----[%s]\n", WordKindCode, str);
        case MonocularOperator:
            fprintf(fout, "[单目运算符]----[%d]----[%s]\n", WordKindCode, str);
        case BinocularOperator:
            fprintf(fout, "[双目运算符]----[%d]----[%s]\n", WordKindCode, str);
        case Delimiter:
            fprintf(fout, "[界符]----[%d]----[%s]\n", WordKindCode, str);
        case WrongWord:
            fprintf(fout, "[错误词]----[%d]----[%s]\n", WordKindCode, str);
        case Blank:
            fprintf(fout, "[空格]----[%d]----[%s]\n", WordKindCode, str);
        default:
            fprintf(fout, "[其他]----[%s]\n", str);
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
    case SignWord:
        ret = WordCode["signword"];
    case Integer:
        ret = WordCode["integer"];
    case FloatPoint:
        ret = WordCode["float"];
    case MonocularOperator:
        ret = WordCode["str"];
    case BinocularOperator:
        ret = WordCode["str"];
    case Delimiter:
        ret = WordCode["str"];
    case WrongWord:
        ret = WordCode["wrongword"];
    case Blank:
        ret = WordCode["blank"];
    default:
        ret =-100;
    }
}
analysis::~analysis()
{
    fclose(fin);
    fclose(fout);
}
