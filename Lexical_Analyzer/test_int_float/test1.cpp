#include <iostream>
using namespace std;
/********************************************
 * �ж������ַ����� �� ����
 * *********************************************/
int isNum(char c) {
    if (c <= '9' && c >= '0')
        return 1;

    return 0;
}


/********************************************
 * �ж������Сд��ĸ���� ��
 * *********************************************/
int isLetter(char c) {
    if ((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A'))
        return 1;
    return 0;
}
/********************************************
 * ���������char str[]����'\0'��β
 * �ж������ַ����� �� int��������
 * ����ֵΪ0 ��ζ�� ѹ������
 * ����ֵΪ1 ��ζ�� �ǣ�����4531ʮ����������0456�˽���������0x46\0X46ʮ����������
 * ����ֵΪ2 ��ζ�� �ǵ�int�������� �������� 0x0G
 * *********************************************/
int isInt(char str[]) {
    const int OCT = 8;
    const int DEC = 10;
    const int HEX = 16;
    int intType = DEC;
    int len = 0;
    len = strlen(str);

    //�ж����Ľ���
    if (isNum(str[0]))
    {
        if (str[0] == '0' && str[1] != '\0')//�ж��ǰ˽��ƻ���ʮ�����ƣ�����λ��0�����ֲ�ֹһλʱ������һλ��
        {
            if (isNum(str[1]))
                intType = OCT;
            else if (str[1] == 'x' || str[1] == 'X')
                intType = HEX;
            else
                return 0;
        }
        else
            intType = DEC;
    }
    //������ĸ�ʽ�Ƿ���ȷ
    switch (intType)
    {
    case OCT:
        for (int i = 1; i < len; i++)//�˽�������λΪ0����������Ϊ2.
        {
            if (isNum(str[i]))
            {
                if (str[i] >= '0' && str[i] <= '7')
                    ;
                else
                    return 2;
            }
            else
                return 0;
        }
        return 1;
        break;
    case DEC:
        for (int i = 0; i < len; i++)
        {
            if (isNum(str[i]))
                ;
            else
                return 0;
        }
        return 1;
        break;
    case HEX:
        for (int i = 2; i < len; i++)//ʮ��������ǰ��λΪ0x\0X����������Ϊ3.
        {
            if (isNum(str[i]) || (str[i] >= 'a' && str[i] <= 'f') || (str[i] >= 'A' && str[i] <= 'F'))
                ;
            else
            {
                if (!isLetter(str[i]))//ʮ���������г��ַ���ĸ
                    return 0;
                else
                    return 2;
            }
        }
        return 1;
        break;
    }
}

/********************************************
 * ���������char str[]����'\0'��β
 * �ж������ַ����� �� float��double�͸�������
 * ����ֵΪ0 ��ζ�� ѹ�����ǣ����������һλ���ַ����֣����һλ���ַ�f\F��d\D�����ֵ������
 * ����ֵΪ1 ��ζ�� ��
 * ����ֵΪ2 ��ζ�� �ǵ�float�������� �������� 0.0.1��.1(���ֶ��С�������λ����С����)
 * *********************************************/
int isFloat(char str[]) {

    int len = 0;
    int dotAppearNum = 0;
    len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        if (len > 0)//ȷ������λ�����һλ
        {
            if (i == 0)//�ж���λ
            {
                if (!isNum(str[i]))//��������
                {
                    if (str[i] == '.')
                        return 2;
                    else
                        return 0;
                }
            }
            if (i < len - 1)//�жϷ����һλ�Ҳ�����λ
            {
                if (!isNum(str[i]) && str[i] != '.')//�Ȳ�������Ҳ����С����
                    return 0;
                if (str[i] == '.')//�����С����
                {
                    dotAppearNum++;
                    if (dotAppearNum > 1)//���ֶ��С��������
                        return 2;
                }
            }
            if (i == len - 1)//�ж�ĩλ
            {
                //�Ȳ�������Ҳ����f\F��d\D
                if (!isNum(str[i]) && str[i] != 'f' && str[i] != 'F' && str[i] != 'd' && str[i] != 'D')
                    return 0;
            }
        }
    }
    return 1;
}

int main()
{
	char numInt[9][10] = {
		//������ȷ����
		"0123",
		"0",
		"0x456",
		//��������ȫ�������
		"0891",
		"0Xt9",
		//������ȫ�������
		"056,",
		"456htj",
		"0x*9",
		NULL
	};
	char numFloat[9][10] = {
		//������ȷ����
		"0.123",
		"6.4567f",
		"9.452D",
		//��������ȫ�������
		".12",
		"0.0.1",
		//������ȫ�������
		"0.56t",
		"456htj",
		"ss.456",
		NULL
	};
    for (int i = 0; i<=7; i++)
        cout << isInt(numInt[i]) << endl;
    cout << "==================================" << endl;
    for (int i = 0; i <= 7; i++)
        cout << isFloat(numFloat[i]) << endl;
	return 0;
}