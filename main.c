#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_EXP_LEN 100 // 输入的表达式最大长度
#define ISOPERATER 1 // 操作符
#define ISNUMBER 2   // 数字
#define ISALPHA 3    // 函数

typedef struct{
	int type; // 1-->操作符, 2-->数字, 3-->函数
	double number;
	char op[10];
}ElemType; // 数据类型

typedef struct node{
	ElemType data;
	struct node *next;
}sNode, *Stack; // 链栈

Stack opStack, numStack;

sNode *creatNode()
{
	sNode *re = (sNode*)calloc(1, sizeof(sNode));
	return re;
}

// 入栈-->插入新结点(头插)
void push(Stack s, ElemType *data)
{
	sNode *newNode = creatNode();
	strcpy(newNode->data.op, data->op);
	newNode->data.number = data->number;
	newNode->data.type = data->type;
	newNode->next = s->next;
	s->next = newNode;
}

// 出栈
void pop(Stack s, sNode *re)
{
	sNode *p = s->next;
	if(NULL == p)
	{
		return;
	}
	s->next = p->next;
	strcpy(re->data.op, p->data.op);
	re->data.number = p->data.number;
	re->data.type = p->data.type;
	free(p);
}

/*void printStack(Stack s)
{
	sNode *p = s->next;
	while(p)
	{
		if(ISNUMBER == p->data.type)
			printf("%lf", p->data.number);
		else
			printf("%s", p->data.op);
		p = p->next;
	}
	printf("\n");
}*/

// 销毁栈
void destroy(Stack s)
{
	sNode *pre = s, *p = s->next;
	s->next = NULL;
	while(p)
	{
		pre = p;
		p = p->next;
		free(pre);
	}
	free(s);
}

// 判断字符
int judgeChar(char ch)
{
	if(ch >= '0' && ch <= '9')
		return ISNUMBER;
	else if(ch >= 'a' && ch <= 'z')
		return ISALPHA;
	else if(ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')')
		return ISOPERATER;
	return -1;
}

// 链栈翻转
void reverseStack(Stack s)
{
	sNode *p = s->next, *q;
	s->next = NULL;
	while(p)
	{
		q = p->next;
		p->next = s->next;
		s->next = p;
		p = q;
	}
}

// 将字符串拆开存进结构体
Stack storeExp(char *exp)
{
	int i = 0, negative, k;
	ElemType *data;
	Stack re = creatNode();
	while(exp[i])
	{
		k = 0, negative = 1;
		data = (ElemType*)calloc(1, sizeof(ElemType));
		// 如果 - 号前面没有字符或者为（, 为负数
		if(exp[i] == '-' && (i - 1 < 0 || exp[i-1] == '('))
		{
			negative = -1;
			++i;
		}
		if(ISNUMBER == judgeChar(exp[i])) // 存数字
		{
			while(exp[i] && ISNUMBER == judgeChar(exp[i]))
			{
				data->number = data->number * 10 + exp[i++] - '0';
			}
			data->number *= negative;
			data->type = ISNUMBER;
		}
		else if(ISALPHA == judgeChar(exp[i])) // 存函数
		{
			while(exp[i] && ISALPHA == judgeChar(exp[i]))
			{
				data->op[k++] = exp[i++];
			}
			if(!strcmp(data->op, "sin") || !strcmp(data->op, "cos") || !strcmp(data->op, "tan") || !strcmp(data->op, "lg"))
			{
				data->type = ISALPHA;
			}
			else
			{
				printf("函数输入错误，请重新输入！\n");
				return NULL;
			}
		}
		// 存运算符
		else if(ISOPERATER == judgeChar(exp[i]))
		{
			data->op[k++] = exp[i++];
			data->type = ISOPERATER;
		}
		else if(exp[i] == ' ') // 空格跳过
		{
			++i;
			continue;
		}
		else
		{
			printf("表达式不合法，请重新输入！\n");
			return NULL;
		}
		push(re, data);
	}
	reverseStack(re);
	return re;
}

// 中缀转后缀
void infixToPostfix(Stack s)
{
	sNode *tmp = creatNode(), *p = s->next;
	opStack = creatNode();
	s->next = NULL;
	while(p)
	{
		if(ISNUMBER == p->data.type)
		{
			push(s, &p->data);
		}
		else if(ISALPHA == p->data.type)
		{
			push(opStack, &p->data);
		}
		else if(ISOPERATER == p->data.type)
		{
			if(p->data.op[0] == ')')
			{
				while(opStack->next && opStack->next->data.op[0] != '(')
				{
					pop(opStack, tmp);
					push(s, &tmp->data);
				}
				if(opStack->next && opStack->next->data.op[0] == '(')
					pop(opStack, tmp);
			}
			else if(p->data.op[0] == '+' || p->data.op[0] == '-')
			{
				if(opStack->next != NULL)
				{
					while(opStack->next && opStack->next->data.op[0] != '(')
					{
						pop(opStack, tmp);
						push(s, &tmp->data);
					}
				}
				push(opStack, &p->data);
			}
			else if(p->data.op[0] == '*' || p->data.op[0] == '/' || p->data.op[0] == '(')
			{
				push(opStack, &p->data);
			}
		}
		p = p->next;
	}
	while(opStack->next)
	{
		pop(opStack, tmp);
		push(s, &tmp->data);
	}
	reverseStack(s);
}

// 后缀表达式求值
double calculate(Stack s)
{
	sNode *num1 = creatNode(), *num2 = creatNode(), *p = s->next;
	opStack = creatNode();
	numStack = creatNode();
	while(p)
	{
		if(ISNUMBER == p->data.type)
		{
			push(numStack, &p->data);
		}
		else if(ISALPHA == p->data.type)
		{
			if(numStack->next)
			{
				pop(numStack, num1);
				if(strcmp(p->data.op, "sin") == 0)
				{
					num1->data.number = sin(num1->data.number);
				}
				if(strcmp(p->data.op, "cos") == 0)
				{
					num1->data.number = cos(num1->data.number);
				}
				if(strcmp(p->data.op, "tan") == 0)
				{
					num1->data.number = tan(num1->data.number);
				}
				if(strcmp(p->data.op, "lg") == 0)
				{
					num1->data.number = log10(num1->data.number);
				}
				push(numStack, &num1->data);
			}
			else
			{
				printf("表达式不合法，请重新输入！\n");
				return 0;
			}
		}
		else if(ISOPERATER == p->data.type)
		{
			if(numStack->next && numStack->next->next)
			{
				pop(numStack, num1);
				pop(numStack, num2);
				if(strcmp(p->data.op, "+") == 0)
				{
					num2->data.number += num1->data.number;
				}
				if(strcmp(p->data.op, "-") == 0)
				{
					num2->data.number -= num1->data.number;
				}
				if(strcmp(p->data.op, "*") == 0)
				{
					num2->data.number *= num1->data.number;
				}
				if(strcmp(p->data.op, "/") == 0)
				{
					if(num1->data.number == 0)
					{
						printf("整数除0！\n");
						return 0;
					}
					num2->data.number /= num1->data.number;
				}
				push(numStack, &num2->data);
			}
			else
			{
				printf("表达式不合法，请重新输入！\n");
				return 0;
			}
		}
		p = p->next;
	}
	if(numStack->next && !numStack->next->next)
		return numStack->next->data.number;
	else
	{
		printf("表达式不合法，请重新输入！\n");
		return 0;
	}
}

// 主界面
void menu()
{
	printf("--------------------------------\n");
	printf("\t简易科学计算器\n");
	printf("--------------------------------\n");
	printf("1）支持的运算符：\n\n");
	printf("1.+\t2.-\t3.*\t4./\n\n5.sin\t6.cos\t7.tan\t8.lg\n\n");
	printf("2）特别说明：\n\n");
	printf("请使用半角符号！\n");
	printf("--------------------------------\n");
}

// 用户交互
void keyDown()
{
	int op;
	double ans;
	char *exp = (char*)calloc(MAX_EXP_LEN, sizeof(char));
	Stack express = creatNode();
	printf("请输入（1.计算 2.退出）：");
	fflush(stdin);
	scanf("%d", &op);
	printf("--------------------------------\n");
	switch(op)
	{
		case 1:
			printf("请输入表达式：");
			// 读取表达式
			fflush(stdin);
			scanf("%s", exp);
			// 表达式拆开存进栈
			exp = strlwr(exp);
			express = storeExp(exp);
			// 若正确存入
			if(express != NULL)
			{
				infixToPostfix(express);
				ans = calculate(express);
				printf("result = %lf\n", ans);
			}
			break;
		case 2:
			exit(0);
			break;
		default:
			printf("输入不合法，请重新输入！\n");
			break;
	}
}

void main()
{
	while(1)
	{
		menu();
		keyDown();
		system("pause");
		system("cls");
	}
	system("pause");
}