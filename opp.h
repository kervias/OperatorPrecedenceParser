#ifndef OPP_H
#define OPP_H

/*
 *  算符优先文法
 *  OperatorPrecedenceParser(OPP)
 */

#include <QMap>
#include <QChar>
#include <QStringList>
#include <QSet>
#include <QStack>
#define MAX_LEN 30 // 默认的非终结符最大个数和终结符最大个数


typedef struct _info_block
{
    int vtNum, vnNum;
    QChar *VT, *VN;
    bool (*firstvt)[MAX_LEN], (*lastvt)[MAX_LEN];
    QMap<QChar, int> *mapVT,*mapVN;
    QChar (*precedenceTable)[MAX_LEN+1];
    _info_block()
    {
        vtNum = 0;
        vnNum = 0;
        VT = NULL;
        VN = NULL;
        firstvt = NULL;
        lastvt = NULL;
        mapVN = NULL;
        mapVT = NULL;
        precedenceTable = NULL;
    }
}INFO_BLOCK;


class OPP
{
    public:
        OPP();

        // 输入数据处理
        bool start(QStringList qsl); //接收数据并处理
        void clear_all();

        void _init_all_data(); //初始化所有数据
        bool _isVN(QChar ch); //判断是否是非终结符
        bool _isVT(QChar ch); //判断是否是终结符
        bool _isValid(); //根据输入文法判断是否是算符文法
        bool _produceGrammer(); //将文法处理保存

        // 生成FIRSTVT集
        void _produceFirstVt();

        // 生成LASTVT集
        void _produceLastVt();

        // 生成算符优先关系表
        bool _producePrecedenceTable();


        bool __is_inputStr_valid(QString input_str); //检测输入串的合法性

        QStringList __log(QString &stack, int stack_len, QString input_str, int str_p, int ind);
        QString __log_vt_help(QList<QChar> charL);
        QString __log_merge_help(QList<QChar> charL);

        // GET函数集
        QString get_error_msg();
        bool get_result_info(INFO_BLOCK *info_block);
        // 算符优先分析处理
        bool get_process_info(QList<QStringList> *process_info, QString input_str); //分析处理的过程信息

    private:
        QChar precedenceTable[MAX_LEN+1][MAX_LEN+1];  //算符优先关系表, 存储 < 、> 和 = 符号，索引对应非终结符数组索引
        QChar VN[MAX_LEN]; //非终结符数组
        QChar VT[MAX_LEN]; //终结符数组
        int vtNum, vnNum; //终结符和非终结符的个数
        QMap<QChar, int> mapVN; //由非终结符找到其在非终结符数组中的索引，用于加速查找
        QMap<QChar, int> mapVT; //由终结符找到其在终结符数组中的索引，用于加速查找
        bool FIRSTVT[MAX_LEN][MAX_LEN]; //FIRSTVT集，第一个下标对应非终结符数组索引，第二个下标对应终结符数组索引
        bool LASTVT[MAX_LEN][MAX_LEN]; //LASTVT集，第一个下标对应非终结符数组索引，第二个下标对应终结符数组索引

        QStringList grammer; //原始的文法内容
        QMap<QChar, QStringList> handleGrammer; //处理后的文法内容

        QString err_msg;
        QSet<QChar> valid_vt = {'+', '-', '*', '/', '(', ')', '^'}; //文法中，除了大写字母和小写字母中额外合法字符

        QChar beginChar; //文法开始符号
        bool success;

};

#endif // OPP_H
