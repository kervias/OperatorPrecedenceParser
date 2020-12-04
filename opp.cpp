#include "opp.h"
#include <QDebug>
#include <string.h>
#include <QPair>
#include <QVector>

OPP::OPP()
{
    this->success = false; //当前是否输入了文法，并生成了算符优先分析表
}

bool OPP::start(QStringList qsl)
{
    // 1. 预处理
    this->clear_all();
    this->grammer = qsl;
    if(!this->_isValid())
        return false;
    if(!this->_produceGrammer())
        return false;

//    qDebug()<<this->handleGrammer;

    this->_produceFirstVt();
    this->_produceLastVt();
    if(!this->_producePrecedenceTable())
        return false;
    this->success = true;
    return true;
}

void OPP::clear_all()
{
    this->success = false;
    mapVN.clear();
    mapVT.clear();
    grammer.clear();
    handleGrammer.clear();
    memset(precedenceTable, 0, sizeof (QChar)*(MAX_LEN+1)*(MAX_LEN+1));
    memset(FIRSTVT, 0, sizeof(bool)*MAX_LEN*MAX_LEN);
    memset(LASTVT, 0, sizeof(bool)*MAX_LEN*MAX_LEN);
}

bool OPP::_isVN(QChar ch) //0是VT, 1是VN
{
    if(ch.isUpper())
        return 1;
    return 0;
}

bool OPP::_isVT(QChar ch)
{
    if(ch.isLower() || valid_vt.contains(ch))
        return true;
    return false;
}


bool OPP::_isValid()
{
    QStringList strL;
    int i,j,k;
    bool b;
    for(i = 0; i < this->grammer.size(); i++)
    {
       strL = this->grammer[i].split("->");
       if(strL.size() != 2 || strL[0].length() != 1 || !strL[0][0].isUpper()) return false;
       strL = strL[1].split("|");
       if(strL.size() == 0) return false;
       for(j = 0; j < strL.size(); j++)
       {
           if(strL[j].size() == 0)
           {
               this->err_msg = this->grammer[i];
               return false;
           }
           b = false; //上一个字符是大写还是小写字符，判断是否是算符文法
           for(k = 0; k < strL[j].length(); k++)
           {
               if(!(this->_isVN(strL[j][k]) || this->_isVT(strL[j][k])))
               {
                   this->err_msg = this->grammer[i];
                   return false;
               }
               if(this->_isVN(strL[j][k]))
               {
                   if(b)
                   {
                       this->err_msg = this->grammer[i] + "\n" + "该文法不是算符优先文法";
                       return false;
                   }
                   b = true;
               }else{
                   b = false;
               }
           }
       }
    }
    return true;
}

bool OPP::_produceGrammer() //将原先的文法进行处理并保存
{
    int i,j;
    QStringList strL1,strL2;

    this->beginChar = this->grammer[0][0];
    // 1. 将grammer处理并保存到handleGrammer中
    for(i = 0; i < this->grammer.size(); i++)
    {
        strL1 = this->grammer[i].split("->");
        strL2 = strL1[1].split("|");
        if(!this->handleGrammer.contains(strL1[0][0]))
        {
            this->handleGrammer.insert(strL1[0][0], QStringList());
        }
        for(j = 0; j < strL2.size(); j++)
        {
            this->handleGrammer[strL1[0][0]].append(strL2[j]);
        }
    }

    // 2. 保存所有非终结符到VN中,并设置mapVN
    this->vnNum = this->handleGrammer.keys().size();
    if(this->vnNum > MAX_LEN)
    {
        this->err_msg = "非终结符数量超出限制";
        return false;
    }
    QList<QChar> tmp_l = this->handleGrammer.keys();
    for(i = 0; i < tmp_l.size(); i++)
    {
        this->VN[i] = tmp_l[i];
        this->mapVN[this->VN[i]] = i;
    }

    // 3. 保存所有终结符到VT数组中,并设置mapVT
    QStringList tmp_str_l;
    this->vtNum = 0; //VT索引，最后值代表VT的个数
    for(auto it = this->handleGrammer.begin(); it != this->handleGrammer.end(); it++)
    {
        tmp_str_l = it.value();
        for(i = 0; i < tmp_str_l.size(); i++)
        {
            for(j = 0; j < tmp_str_l[i].length(); j++)
            {
                if(this->_isVN(tmp_str_l[i][j]))
                {
                    if(!this->mapVN.contains(tmp_str_l[i][j])) //如果非终结符不在之前的统计范围内，即该非终结符没有产生式
                    {
                        this->err_msg = QString("非终结符 %1 没有对应的产生式").arg(tmp_str_l[i][j]);
                        return false;
                    }
                }else
                {
                    this->VT[this->vtNum] = tmp_str_l[i][j]; //这里还需要去重
                    this->mapVT[tmp_str_l[i][j]] = this->vtNum ++;
                }
            }
        }
    }
    return true;
}


void OPP::_produceFirstVt()
{
    QChar vn;
    QStringList strL;
    int i;
    QStack<QPair<QChar,QChar>> stack;
    for(auto it = this->handleGrammer.begin(); it != this->handleGrammer.end(); it++)
    {
        vn = it.key();
        strL = it.value();
        for(i = 0; i < strL.size(); i++)
        {
            if(this->_isVT(strL[i][0])) //匹配 P->a... 的产生式
            {
                if(!this->FIRSTVT[mapVN[vn]][mapVT[strL[i][0]]])
                {
                    //printf("%c %c\n", vn, strL[i][0].toLatin1());
                    //printf("%d %d\n",mapVN[vn],mapVT[strL[i][0]]);
                    //qDebug()<<vn<<strL[i][0];
                    stack.push(qMakePair(vn, strL[i][0])); //入栈
                    this->FIRSTVT[mapVN[vn]][mapVT[strL[i][0]]] = 1;
                }
            }
            else if(strL[i].size() > 1 && this->_isVN(strL[i][0]) && this->_isVT(strL[i][1])) //匹配 P->Qa... 的产生式
            {
                if(!this->FIRSTVT[mapVN[vn]][mapVT[strL[i][1]]])
                {
                    //printf("%c %c\n", vn, strL[i][1].toLatin1());
                    //printf("%d %d\n",mapVN[vn],mapVT[strL[i][1]]);
                     //qDebug()<<vn<<strL[i][1];
                    stack.push(qMakePair(vn, strL[i][1])); //入栈
                    this->FIRSTVT[mapVN[vn]][mapVT[strL[i][1]]] = 1;
                }
            }
        }
    }

    while(!stack.empty())
    {
        QPair<QChar, QChar> pair = stack.pop();
        for(auto it = this->handleGrammer.begin(); it != this->handleGrammer.end(); it++)
        {
            vn = it.key();
            strL = it.value();
            for(i = 0; i < strL.size(); i++)
            {
                if(strL[i][0] == pair.first) //匹配 P->Q...
                {
                    if(!this->FIRSTVT[mapVN[vn]][mapVT[pair.second]])
                    {
                        stack.push(qMakePair(vn, pair.second));
                        this->FIRSTVT[mapVN[vn]][mapVT[pair.second]] = 1;
                    }
                }
            }
        }
    }
}

void OPP::_produceLastVt()
{
    QChar vn;
    QStringList strL;
    int i;
    QStack<QPair<QChar,QChar>> stack;
    for(auto it = this->handleGrammer.begin(); it != this->handleGrammer.end(); it++)
    {
        vn = it.key();
        strL = it.value();
        for(i = 0; i < strL.size(); i++)
        {
            if(this->_isVT(strL[i][strL[i].length()-1])) //匹配 P->...a 的产生式
            {
                if(!this->LASTVT[mapVN[vn]][mapVT[strL[i][strL[i].length()-1]]])
                {
                    stack.push(qMakePair(vn, strL[i][strL[i].length()-1])); //入栈
                    this->LASTVT[mapVN[vn]][mapVT[strL[i][strL[i].length()-1]]] = 1;
                }
            }
            else if(strL[i].size() > 1 && this->_isVN(strL[i][strL[i].length()-1]) && this->_isVT(strL[i][strL[i].length()-2])) //匹配 P->...Qa 的产生式
            {
                if(!this->LASTVT[mapVN[vn]][mapVT[strL[i][strL[i].length()-2]]])
                {
                    stack.push(qMakePair(vn, strL[i][strL[i].length()-2])); //入栈
                    this->LASTVT[mapVN[vn]][mapVT[strL[i][strL[i].length()-2]]] = 1;
                }
            }
        }
    }

    while(!stack.empty())
    {
        QPair<QChar, QChar> pair = stack.pop();
        for(auto it = this->handleGrammer.begin(); it != this->handleGrammer.end(); it++)
        {
            vn = it.key();
            strL = it.value();
            for(i = 0; i < strL.size(); i++)
            {
                if(strL[i][strL[i].length()-1] == pair.first) //匹配 P->...Q
                {
                    if(!this->LASTVT[mapVN[vn]][mapVT[pair.second]])
                    {
                        stack.push(qMakePair(vn, pair.second));
                        this->LASTVT[mapVN[vn]][mapVT[pair.second]] = 1;
                    }
                }
            }
        }
    }

//    for(int i = 0; i < vnNum; i++)
//    {
//      printf("%c ", this->VN[i].toLatin1());
//    }
//    printf("\n");
//    for(int i = 0; i < vtNum; i++)
//    {
//        printf("%c ", this->VT[i].toLatin1());
//    }
//    printf("\n");

//    for(i = 0; i < vnNum; i++)
//    {
//        for(int j = 0; j < vtNum; j++)
//        {
//            printf("%d ", this->FIRSTVT[i][j]);
//        }
//        printf("\n");
//    }
//    printf("\n");
//    for(i = 0; i < vnNum; i++)
//    {
//        for(int j = 0; j < vtNum; j++)
//        {
//            printf("%d ", this->LASTVT[i][j]);
//        }
//        printf("\n");
    //    }
}

bool OPP::_producePrecedenceTable()
{
    int i,j,k;
    // 1. 设置#号优先级
    mapVT['#'] = vtNum;
    for(i = 0; i < vtNum; i++)
    {
        if(this->FIRSTVT[mapVN[this->beginChar]][i]) //最后一行
        {
            this->precedenceTable[mapVT['#']][i] = '<';
        }
    }
    for(i = 0; i < vtNum; i++)
    {
        if(this->LASTVT[mapVN[this->beginChar]][i]) //最后一列
        {
            this->precedenceTable[i][mapVT['#']] = '>';
        }
    }
    this->precedenceTable[mapVT['#']][mapVT['#']] = '=';

    // 2. 找到 = > < 的关系
    QStringList tmp_str_l;
    QChar vn;
    for(auto it = this->handleGrammer.begin(); it != this->handleGrammer.end(); it++)
    {
        vn = it.key();
        tmp_str_l = it.value();
        for(i = 0; i < tmp_str_l.size(); i++)
        {
            for(j = 1; j < tmp_str_l[i].length(); j++)
            {
                // 寻找 = 的关系
                if(this->_isVT(tmp_str_l[i][j-1]) && this->_isVT(tmp_str_l[i][j])) // P->...ab...
                {
                    if(this->precedenceTable[mapVT[tmp_str_l[i][j-1]]][mapVT[tmp_str_l[i][j]]] == '\0'
                            || this->precedenceTable[mapVT[tmp_str_l[i][j-1]]][mapVT[tmp_str_l[i][j]]] == '=')
                        this->precedenceTable[mapVT[tmp_str_l[i][j-1]]][mapVT[tmp_str_l[i][j]]] = '=';
                    else{
                        qDebug() << this->precedenceTable[mapVT[tmp_str_l[i][j-1]]][mapVT[tmp_str_l[i][j]]] << " 1";
                        this->err_msg = "该文法不是算符优先文法，生成算符优先分析表时，存在两个终结符之间有多种优先关系";
                        return false;
                    }
                }

                if(j >= 2 &&  this->_isVT(tmp_str_l[i][j-2]) && this->_isVN(tmp_str_l[i][j-1]) && this->_isVT(tmp_str_l[i][j])) // P->...aQb...
                {
                    if(this->precedenceTable[mapVT[tmp_str_l[i][j-2]]][mapVT[tmp_str_l[i][j]]] == '\0'
                            || this->precedenceTable[mapVT[tmp_str_l[i][j-2]]][mapVT[tmp_str_l[i][j]]] == '=')
                        this->precedenceTable[mapVT[tmp_str_l[i][j-2]]][mapVT[tmp_str_l[i][j]]] = '=';
                    else{
                        qDebug() << this->precedenceTable[mapVT[tmp_str_l[i][j-2]]][mapVT[tmp_str_l[i][j]]] << " 2";
                        this->err_msg = "该文法不是算符优先文法，生成算符优先分析表时，存在两个终结符之间有多种优先关系";
                        return false;
                    }
                }

                // 寻找 < 的关系
                if(this->_isVT(tmp_str_l[i][j-1]) && this->_isVN(tmp_str_l[i][j])) // P->...aR...
                {
                    for(k = 0; k < vtNum; k++)
                    {
                        if(this->FIRSTVT[mapVN[tmp_str_l[i][j]]][k])
                        {
                            if(this->precedenceTable[mapVT[tmp_str_l[i][j-1]]][k] == '\0'
                                    || this->precedenceTable[mapVT[tmp_str_l[i][j-1]]][k] == '<')
                                this->precedenceTable[mapVT[tmp_str_l[i][j-1]]][k] = '<';
                            else{
                                qDebug() <<  this->precedenceTable[mapVT[tmp_str_l[i][j-1]]][k] << " 3";
                                this->err_msg = "该文法不是算符优先文法，生成算符优先分析表时，存在两个终结符之间有多种优先关系";
                                return false;
                            }
                        }
                    }

                }

                // 寻找 > 的关系
                if(this->_isVN(tmp_str_l[i][j-1]) && this->_isVT(tmp_str_l[i][j])) // P->...Rb...
                {
                    for(k = 0; k < vtNum; k++)
                    {
                        if(this->LASTVT[mapVN[tmp_str_l[i][j-1]]][k])
                        {
                            if(this->precedenceTable[k][mapVT[tmp_str_l[i][j]]] == '\0'
                                    || this->precedenceTable[k][mapVT[tmp_str_l[i][j]]] == '>')
                                this->precedenceTable[k][mapVT[tmp_str_l[i][j]]] = '>';
                            else{
                                qDebug() << this->precedenceTable[k][mapVT[tmp_str_l[i][j]]] << " 4";
                                this->err_msg = "该文法不是算符优先文法，生成算符优先分析表时，存在两个终结符之间有多种优先关系";
                                return false;
                            }
                        }
                    }

                }
            }
        }
    }
    return true;
}

bool OPP::__is_inputStr_valid(QString input_str)
{
    if(input_str.size() == 0)
    {
        this->err_msg = "输入串不能为空";
        return false;
    }
    if(input_str.contains('#'))
    {
        this->err_msg = "输入串中请不要包含#号";
        return false;
    }
    for(int i = 0; i < input_str.length(); i++)
    {
        if(this->mapVT.contains(input_str[i]))
            continue;
        else{
            this->err_msg = QString("输入串中包含非法字符：%1\n本程序要求输入串每个字符均属于文法中的终结符集合").arg(input_str[i]);
            return false;
        }
    }
    return true;
}


QString OPP::get_error_msg()
{
    return this->err_msg;
}

bool OPP::get_result_info(INFO_BLOCK *info_block)
{
    if(!this->success)
    {
        this->err_msg = "尚未确认文法";
        return false;
    }
    info_block->vtNum = this->vtNum;
    info_block->vnNum = this->vnNum;
    info_block->VT = this->VT;
    info_block->VN = this->VN;
    info_block->firstvt = this->FIRSTVT;
    info_block->lastvt = this->LASTVT;
    info_block->mapVN = &this->mapVN;
    info_block->mapVT = &this->mapVT;
    info_block->precedenceTable = this->precedenceTable;
    return true;
}

bool OPP::get_process_info(QList<QStringList> *process_info, QString input_str)
{
    // input_str: 输入的串
    // process_info: 分析处理的信息，通过参数返回


    if(!this->success)
    {
        this->err_msg = "尚未确认文法";
        return false;
    }
    // 检测合法性
    if(!this->__is_inputStr_valid(input_str)) return false;

    QStringList str_l = {"步骤", "符号栈", "输入串", "动作", "说明"};
    process_info->append(str_l); //日志记录
    int ind = 1; //序号
    input_str += '#';
    QString stack(input_str.length()+10); //将string作为栈, 并初始化大小
    int k = 0,j; //k: 符号栈的大小, j: 栈中的移动指针
    int point=0; //输入串位置指针
    QChar ch; //作为输入的字符
    QChar tmp_ch; //作为临时存储变量

    stack[k] = '#'; // 首先 # 入栈

    do{
        ch = input_str[point]; //将下一个输入符号读取到ch中
        if(this->_isVT(stack[k]) || stack[k] == '#') j = k; //取得栈中最高位置的终结符
        else j = k-1;

        while(this->precedenceTable[mapVT[stack[j]]][mapVT[ch]] == '>') //stack[j] > ch
        {
            QList<QChar> vt_all; //扫描过的终结符记录
            vt_all.append(ch);
            vt_all.append(stack[j]);
            do{
                tmp_ch = stack[j];
                if(this->_isVT(stack[j-1]) || stack[j-1] == '#') j -= 1;
                else j -= 2;
                vt_all.append(stack[j]);
                if(this->precedenceTable[mapVT[stack[j]]][mapVT[tmp_ch]] == '\0')
                {
                    this->err_msg = QString("分析过程中出现错误：\n栈顶终结符 %1 和 输入串符号 %2 不存在任何优先关系").arg(stack[j]).arg(tmp_ch);
                    return false;
                }
            }while(this->precedenceTable[mapVT[stack[j]]][mapVT[tmp_ch]] == '=');

            // 归约
            // 从栈中查找出归约串
            QString merge_str = stack.mid(j+1, k-j);


            // 查找产生式
            QList<QChar> mergeL;
            for(auto it = this->handleGrammer.begin(); it != this->handleGrammer.end(); it++)
            {
                tmp_ch = it.key();
                str_l = it.value();
                for(int i = 0; i < str_l.size(); i++)
                {
                    if(str_l[i].compare(merge_str) == 0)
                    {
                        mergeL.append(tmp_ch);
                    }
                }
            }


            if(mergeL.size() == 1) //归约串产生式唯一
            {

                str_l = this->__log(stack, k , input_str, point, ind);
                str_l.append(QString("%1, 归约").arg(__log_vt_help(vt_all)));
                str_l.append(QString("最左素短语：%1").arg(merge_str));
                process_info->append(str_l);
                k = j+1;
                stack[k] = mergeL[0];

            }else if(mergeL.size() > 1) //规约串产生式有多个，任选一个归约
            {
                str_l = this->__log(stack, k , input_str, point, ind);
                str_l.append(QString("%1, 归约").arg(__log_vt_help(vt_all)));
                str_l.append(QString("最左素短语：%1\n可归约成：%2\n此时选择 %3").arg(merge_str).arg(__log_merge_help(mergeL)).arg(mergeL[0]));
                process_info->append(str_l);
                k = j+1;
                stack[k] = mergeL[0];
            }
            else{ // 没有对应产生式

                str_l = this->__log(stack, k , input_str, point, ind);
                str_l.append(QString("%1, 归约").arg(__log_vt_help(vt_all)));
                str_l.append(QString("最左素短语：%1\n无直接对应产生式\n默认选择@作为非终结符").arg(merge_str));
                process_info->append(str_l);
                k = j+1;
                stack[k] = '@'; //选择@作为通用非终结符
            }


            //qDebug() << str_l;
            ind++;
        }

        if(this->precedenceTable[mapVT[stack[j]]][mapVT[ch]] == '<' || this->precedenceTable[mapVT[stack[j]]][mapVT[ch]] == '=')
        { // stack[j] < ch 或 s[j] = ch

            // 记录日志
            QString info = "移进";
            if(ch == '#') info = "结束";
            str_l = this->__log(stack, k , input_str, point, ind);
            str_l.append(QString("%1%2%3, %4").arg(stack[j]).arg(this->precedenceTable[mapVT[stack[j]]][mapVT[ch]]).arg(ch).arg(info));
            str_l.append(" ");
            process_info->append(str_l);
            //qDebug() << str_l;

            // 将当前输入符号放入栈中
            k++;
            stack[k] = ch;
            ind++;
        }else{
            this->err_msg = QString("分析过程中出现错误：\n栈顶终结符 %1 和 输入串符号 %2 不存在任何优先关系").arg(stack[j]).arg(ch);
            return false;
        }
        point++;
//        if(ind > 23) break;

    }while(ch != '#');

    return true;
}


QStringList OPP::__log(QString &stack, int stack_len, QString input_str, int str_p, int ind)
{
    QStringList str_l;
    str_l.append(QString("%1").arg(ind));

    str_l.append(stack.mid(0, stack_len+1));

    str_l.append(input_str.mid(str_p, -1));

    return str_l;
}

QString OPP::__log_vt_help(QList<QChar> charL)
{
    QString ret_str;
   // qDebug() << charL;

    for(int i = charL.size()-1; i > 0; i--)
    {
        ret_str += charL[i];
        ret_str += this->precedenceTable[mapVT[charL[i]]][mapVT[charL[i-1]]];
    }
    ret_str += charL[0];
    return ret_str;
}

QString OPP::__log_merge_help(QList<QChar> charL)
{
    QString ret_str;
    for(int i = 0; i < charL.size(); i++)
    {
        ret_str += charL[i];
        ret_str += "或";
    }
    ret_str = ret_str.mid(0, ret_str.size()-1);
    return ret_str;
}



