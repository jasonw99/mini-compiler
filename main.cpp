//
// Created by 15813 on 2019/12/1.
//

#include <iostream>
#include <cstdio>
#include <cstring>
#include <stdlib.h>

using namespace std;

typedef struct {
    int num;
    char *word;
} WORD;

struct{
    char result[50];
    char ag1[50];
    char op[50];
    char ag2[50];
}quad[20];



char input[1000];
//单词缓冲区
char token[255] = "";
int p_input;//输入缓冲区指针
int p_token;//单词缓冲区指针
char ch;//当前读入字符
int line = 1;
//当前获取的词
WORD *temp;
//上一个词的种别码
int pre;
int syn;
int kk = 0; //0表示没有错误
int overSignal = 0;
int blankRow = 0;
int q = 0;
int k;

//关键字数组
string keywords[20] = {"begin", "if", "else", "while", "do", "int", "main", "for", "end", "return", "void"};

//扫描一个单词
WORD *scaner();

//从缓冲区读入一个字符
char m_getch();

//去掉词之间的空格和换行
void getbc();

//拼接单词，使字符数组变为字符串
void concat();

//判断是否是字母
int letter();

//判断是否是数字
int digit();

//检索关键字表格，如果是关键字返回其种别码，如果只是标识符返回标识符的种别码10
int reserve();

//回退一个字符
void retract();

//数字转换成二进制
char *d2b();

int Irparser();

//语句串
int yucu();

//语句
void statement();

//赋值语句
int ass_statement();

//条件语句
int con_statement();

//循环语句
int loop_statement();

//表达式
char* expression();

//项
char* term();

//因子
char* factor();

//返回一个临时变量名 ti
char* newtemp();

//语句块
int block();

//生成三地址语句到四元式表中
void emit(char *result,char *ag1,char*op,char *ag2);

void printProc();

int main() {
    int over = 1;
    WORD *oneword = new WORD;
    freopen("test1.txt", "r", stdin);
    freopen("result.txt","w",stdout);
    scanf("%[^#]s", input);
    p_input = 0;
    //printf("!");
    Irparser();
    if(kk == 0){
        //printf("q = %d\n", q);
//        for(int j = 0; j < q; j++){
//            //printf("(%d) %s = %s %s %s\n", j+1, quad[j].result,quad[j].ag1,quad[j].op,quad[j].ag2);
//            printf("(%s, %s, %s, %s)\n", quad[j].op, quad[j].ag1, quad[j].ag2, quad[j].result);
//        }
        printProc();
        printf("%d: end", q+1);
    }

    return 0;
}

void printProc(){
    for(int i=0;i<q;i++){
        if(!strcmp(quad[i].op,"jump")||!strcmp(quad[i].result,"jump"))
            cout<<i+1<<": "<<quad[i].result<<" "<<quad[i].ag1<<" "<<quad[i].op<<" "<<quad[i].ag2<<endl;
        else
            cout<<i+1<<": "<<quad[i].result<<" = "<<quad[i].ag1<<quad[i].op<<quad[i].ag2<<endl;
    }
}

void emit(char *result,char *ag1,char*op,char *ag2)
{
    strcpy(quad[q].result,result);
    strcpy(quad[q].ag1,ag1);
    strcpy(quad[q].op,op);
    strcpy(quad[q].ag2,ag2);
    q++;
}

char* newtemp(){
    char* p;
    char m[50];
    p = (char*) malloc(50);
    k++;
    itoa(k, m, 10);
    strcpy(p+1, m);
    p[0] = 't';
    return p;
}

int Irparser() {
    int schain = 0;
    kk = 0;
    do { temp = scaner(); } while (temp == NULL);
    //6 int 36 void  主函数必有其中一个为返回值
    //如果没有取到返回值
    if(!(temp->num == 6 || temp->num == 36)){
        printf("error: main函数返回值错误，或缺少返回值, %d\n", line);
        kk = 1;
        if(temp->num != 7){
            printf("error: 缺少main %d\n", line);
        }
        else
            do { temp = scaner(); } while (temp == NULL);
    }
    else{
        do { temp = scaner(); } while (temp == NULL);
        if(temp->num != 7){
            printf("error: 缺少main %d\n", line);
            kk = 1;
        }
        else {
            do { temp = scaner(); } while (temp == NULL);
            //printf("%d", temp->num);
        }
    }
    if(temp ->num != 27){
        printf("error: main函数缺少左括号 %d\n", line);
    }
    else {
        do { temp = scaner(); } while (temp == NULL);
    }
    if(temp->num != 28){
        printf("error: main函数缺少右括号 %d\n", line);
    }
    else {
        do { temp = scaner(); } while (temp == NULL);
    }
    //此时temp为语句块的第一个字符，进入语句块的分析函数。
    block();
    return schain;
}

int block(){

    //29 {
    if(temp->num != 29){
        printf("error: 缺少左大括号, %d\n", line);
        kk = 1;
    }
        //缺少大括号就不用取了，因为已经取到函数体。
    else{
        do { temp = scaner(); } while (temp == NULL);
        //printf("%s", temp->word);
    }
    yucu();
    //30 }
    if (temp->num != 30) {
        printf("error: 缺少右大括号 行数: %d %s \n", line, temp->word);
        kk = 1;
    }
    else
        do { temp = scaner(); } while (temp == NULL);
    //printf("block=%s\n", temp->word);
}

//语句串分析
int yucu(){//处理语句串=<语句>{;<语句>}
    int schain=0;
    statement();
    while(true){
        //; 26
        //从赋值语句中出来 正常情况
        if(temp->num == 26){
            { temp = scaner(); } while (temp == NULL);
            //30 }
            //;  } 说明本语句串已经结束了
            if(temp->num == 30){
                //printf("break from yucu\n");
                break;
            }

            statement();
        }
        //如果本字符不是分号，且上一个字符是右大括号
        //从条件或循环语句中出来
        else if(pre == 30){
        //如果这个字符是end或者右大括号，也代表了语句串的结束 正常情况
            if(temp->num==9||temp->num==30){
                //printf("break from yucu\n");
                break;
            }

            //如果不是三种语句的开头，
            if(temp->num!=10&&temp->num!=2&&temp->num!=4)
                { temp = scaner(); } while (temp == NULL);
            statement();
        }
        else if((temp->num >= 2 && temp->num <= 11 )|| temp-> num == 35 || temp->num == 36){//缺少“;”同时下一行的首部为关键字
            kk=1;
            printf("error: 语句之间缺少分号 行数: %d\n", line);
            statement();
        }
        else
            break;
    }
    return(schain);
}

//三种语句 赋值 循环 条件
void statement(){
    int type = 0;
    //10 字符串 赋值语句
    if(temp->num == 10){
        type = 1;
        //printf("%s\n", temp->word);
    }
    //2 if  条件语句
    if(temp->num == 2){
        type = 2;
    }
    //4 while 循环语句
    if(temp->num == 4){
        type = 3;
    }
    //遇到结束标志
    if(temp->num == 9 || temp->num == 1000){
        return;
    }
    switch(type){
        case 1:
            ass_statement();
            //printf("赋值语句完成  %d %s\n", line, temp->word);
            break;
        case 2:
            con_statement();
            //printf("条件语句完成  %d %s\n", line, temp->word);
            break;
        case 3:
            loop_statement();
            //printf("循环语句完成  %d %s\n",line, temp->word);
            break;
        default:
            printf("error: 错误的语句格式 行数: %d\n", line);
            kk = 1;
            //因为没有语句种类的标识，只能随意认为是其中一种语句进行恢复
            ass_statement();
            break;
    }
}

int loop_statement(){
    char eplace1[10],eplace2[10],op[10],dest[10],condition[20];
    do { temp = scaner(); } while (temp == NULL);
    if(temp->num != 27){
        printf("error: 缺少条件判断语句的左括号 行数: %d  %s\n", line, temp->word);
        kk = 1;
    }
    else
        do { temp = scaner(); } while (temp == NULL);
    strcpy(eplace1,expression());
    if(temp->num == 23)
        strcpy(op,">");
    else if(temp->num == 20)
        strcpy(op,"<");
    else if(temp->num==24)
        strcpy(op,">=");
    else if(temp->num==22)
        strcpy(op,"<=");
    else if(temp->num==25)
        strcpy(op,"==");
    else if(temp->num==34)
        strcpy(op,"!=");
    else{
        kk = 1;
        printf("error: 关系运算符错误，需要为> < <= >= == != 中的一个 行数: %d \n", line);
    }
    do { temp = scaner(); } while (temp == NULL);
    strcpy(eplace2,expression());
    sprintf(condition,"%s%s%s",eplace1,op,eplace2);
    itoa(q+3,dest,10);
    emit("if",condition,"jump",dest);
    //记录while循环开始的地方，循环完要返回这里
    int backpoint=q;
    //保存当前指令的位置，确定while语句结束位置的时候，再将这里的jump语句补全
    int flag=q;
    emit("jump","","","");
    if(temp->num!=28) {
        kk = 1;
        printf("error: 缺少条件循环语句的右括号 行数: %d\n", line);
    }
    else
        do { temp = scaner(); } while (temp == NULL);
    block();
    //将backpoint的值放入dest
    itoa(backpoint,dest,10);
    emit("jump",dest,"","");
    //此时q+1为跳出while的地址
    itoa(q+1,dest,10);
    //printf("跳 %s\n", dest);
    sprintf(quad[flag].ag1,dest);
    return 1;
}

//条件语句
int con_statement(){
    //分别为条件语句的左表达式，条件语句的右表达式，条件语句的符号，条件语句的跳转地址，条件语句
    char eplace1[10],eplace2[10],op[10],dest[10],condition[20];
    do { temp = scaner(); } while (temp == NULL);
    //29 {
    if(temp->num != 27){
        printf("error: 缺少条件判断语句的左括号 行数: %d\n", line);
        kk = 1;
    }
    else
        do { temp = scaner(); } while (temp == NULL);
    strcpy(eplace1,expression());
    if(temp->num == 23)
        strcpy(op,">");
    else if(temp->num == 20)
        strcpy(op,"<");
    else if(temp->num==24)
        strcpy(op,">=");
    else if(temp->num==22)
        strcpy(op,"<=");
    else if(temp->num==25)
        strcpy(op,"==");
    else if(temp->num==34)
        strcpy(op,"!=");
    else{
        kk = 1;
        printf("error: 关系运算符错误，需要为> < <= >= == != 中的一个 行数: %d \n", line);
    }
    do { temp = scaner(); } while (temp == NULL);
    strcpy(eplace2,expression());
    sprintf(condition,"%s%s%s",eplace1,op,eplace2);
    //jump自己这一句（没有emit，q没有加），jump else的一句，本身往前一句，一共跳3步，+3
    itoa(q+3,dest,10);
    emit("if",condition,"jump",dest);
    //先空着else跳转的地址，并将指令编号保存到cur
    int cur = q;
    emit("jump","","","");
    if(temp->num!=28) {
        kk = 1;
        printf("error: 缺少条件判断语句的右括号 行数: %d\n", line);
    }
    else
        do { temp = scaner(); } while (temp == NULL);
    block();
    //到了条件语句末尾，确定else的跳转位置并赋值
    itoa(q+1,dest,10);
    sprintf(quad[cur].ag1,dest);
    return 1;


}

//赋值语句
int ass_statement() {
    int schain = 0;
    char tt[50], eplace[50];
    strcpy(tt, temp->word);
    do { temp = scaner(); } while (temp == NULL);
    //18 =
    if (temp->num != 18) {
        printf("error: 表达式中没有赋值符号 行数: %d\n", line);
        kk = 1;
    }
    else{
        do { temp = scaner(); } while (temp == NULL);
        strcpy(eplace, expression());
        //变量名 = 操作数1
        emit(tt, eplace, "", "");
        schain = 0;
    }

    return schain;

}

//表达式 加减连接
char* expression() {
    char *result,*eplace2,*eplace1,*op;
    result=(char*)malloc(12);
    eplace1=(char*)malloc(12);
    eplace2=(char*)malloc(12);
    op=(char*)malloc(12);
    strcpy(eplace1,term());
    while(temp->num==13||temp->num==14) {
        if (temp->num == 13)
            sprintf(op, "%c", '+');//op[0]='+';
        if (temp->num == 14)
            sprintf(op, "%c", '-');//op[0]='-';
        do { temp = scaner(); } while (temp == NULL);
        strcpy(eplace2, term());
        strcpy(result, newtemp());
        emit(result, eplace1, op, eplace2);
        strcpy(eplace1, result);
    }
    return eplace1;
}

//项 乘除连接
char* term() {
    char *result,*eplace2,*eplace1,*op;
    result=(char*)malloc(12);
    eplace1=(char*)malloc(12);
    eplace2=(char*)malloc(12);
    op=(char*)malloc(12);
    strcpy(eplace1,factor());
    while(temp->num==15||temp->num==16) {
        if (temp->num == 15)
            sprintf(op, "%c", '*');//op[0]='*';
        if (temp->num == 16)
            sprintf(op, "%c", '/');//op[0]='/';
        do { temp = scaner(); } while (temp == NULL);
        strcpy(eplace2, factor());
        strcpy(result, newtemp());
        emit(result, eplace1, op, eplace2);
        strcpy(eplace1, result);
    }
    return eplace1;
}


char* factor(){
    char *fplace;
    fplace=(char *)malloc(12);
    strcpy(fplace,"");
    // 10 字符串
    if(temp->num == 10){
        strcpy(fplace,temp->word);
        do { temp = scaner(); } while (temp == NULL);
    }
        // 11 数字
    else if(temp->num == 11){
        strcpy(fplace, temp->word);
        do { temp = scaner(); } while (temp == NULL);
    }
        // 27 （
    else if(temp->num==27){
        do { temp = scaner(); } while (temp == NULL);
        fplace=expression();
        //28 ）
        if(temp->num==28) do { temp = scaner(); } while (temp == NULL);
        else {
            printf("error: 缺少右括号错误\n");
            kk=1;
        }
    }
    else {
        printf("error: 缺少左括号错误\n");
        kk=1;
    }
    return (fplace);
}

WORD *scaner() {
    //更新pre
    if(temp != NULL)
        pre = temp-> num;
    WORD *myword = new WORD;
    myword->num = 10;//标识符
    myword->word = "";
    //重置token指针
    p_token = 0;
    m_getch();
    getbc();
    //保证第一个字符是字母或下划线，后续字符可以为数字
    if (letter()) {
        while (letter() || digit()) {
            concat();
            //printf("%s", token);
            m_getch();
            //printf("%c ", ch);
        }
        retract();//最后一个m_getch()检测出不是属于这个单词，但是指针往后移了一位，所以回退
        myword->num = reserve();
        //printf("%s ", token);
        myword->word = token;
        //printf("%s\n", myword->word);
        return myword;
    }
        //是数字
    else if (digit()) {
        while (digit()) {
            concat();
            m_getch();
        }
        retract();
        myword->num = 11;
        myword->word = token;
        return myword;
    }
        //是运算符或界符
    else
        switch (ch) {
            case '=': {
                m_getch();
                if (ch == '=') {
                    myword->num = 25;
                    myword->word = "==";
                    return myword;
                }
                //发现不是=就回退一个
                retract();
                myword->num = 18;
                myword->word = "=";
                return myword;
                break;
            }

            case '+': {
                myword->num = 13;
                myword->word = "+";
                return myword;
                break;
            }

            case '-': {
                myword->num = 14;
                myword->word = "-";
                return myword;
                break;
            }
            case '*': {
                myword->num = 15;
                myword->word = "*";
                return myword;
                break;
            }
            case '/': {
                m_getch();
                if (ch == '/') {
                    int skip = 0;
                    while (ch != 10) {
                        m_getch();
                        skip++;
                    }
                    retract();
                    //printf("%d", skip);
                    return NULL;
                    break;
                }
                retract();
                myword->num = 16;
                myword->word = "/";
                return myword;
                break;
            }
            case '(': {
                myword->num = 27;
                myword->word = "(";
                return myword;
                break;
            }
            case ')': {
                myword->num = 28;
                myword->word = ")";
                return myword;
                break;
            }

            case '[': {
                myword->num = 32;
                myword->word = "[";
                return myword;
                break;
            }
            case ']': {
                myword->num = 33;
                myword->word = "]";
                return myword;
                break;
            }
            case '{': {
                myword->num = 29;
                myword->word = "{";
                return myword;
                break;
            }
            case '}': {
                myword->num = 30;
                myword->word = "}";
                return myword;
                break;
            }
            case ',': {
                myword->num = 31;
                myword->word = ".";
                return myword;
                break;
            }
            case ':': {
                myword->num = 17;
                myword->word = ":";
                return myword;
                break;
            }
            case ';': {
                myword->num = 26;
                myword->word = ";";
                return myword;
                break;
            }
            case '>': {
                m_getch();
                if (ch == '=') {
                    myword->num = 24;
                    myword->word = ">=";
                    return myword;
                }
                retract();
                myword->num = 23;
                myword->word = ">";
                return myword;
                break;
            }
            case '<': {
                m_getch();
                if (ch == '=') {
                    myword->num = 22;
                    myword->word = "<=";
                    return myword;
                }
                retract();
                myword->num = 20;
                myword->word = "<";
                return myword;
                break;
            }
            case '!': {
                m_getch();
                if (ch == '=') {
                    myword->num = 24;
                    myword->word = "!=";
                    return myword;
                }
                retract();
                myword->num = -1;
                myword->word = "ERROR";
                return myword;
                break;
            }
                //输入的字符串的结尾
            case '\0': {
                myword->num = 1000;
                myword->word = "OVER";
                break;
            }
            case '#': {
                myword->num = 0;
                myword->word = "#";
                return myword;
                break;
            }
                //其他无效字符
            default: {
                myword->num = -1;
                myword->word = "ERROR";
                return myword;
            }
        }
}

char m_getch() {
    ch = input[p_input++];
    return ch;
}

void getbc() {
    //ASCII=10为换行符
    while (ch == ' ' || ch == 10) {
        if (ch == 10) {
            line++;
        }
        ch = input[p_input++];
    }
}

void concat() {
    token[p_token] = ch;
    token[++p_token] = '\0';
}

int letter() {
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
        return 1;
    else
        return 0;
}

int digit() {
    if (ch >= '0' && ch <= '9')
        return 1;
    else
        return 0;
}

int reserve() {
    int i = 0;
    //11个关键字
    for (; i < 11; ++i) {
        if (!strcmp(keywords[i].c_str(), token)) {
            //关键字 对应种别码表
            if(i < 9)
                return i + 1;
            else
                return i + 1 + 25;
        }
    }
    return 10;
}

void retract() {
    p_input--;
}


