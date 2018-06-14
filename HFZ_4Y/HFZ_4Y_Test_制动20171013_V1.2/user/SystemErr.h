/****************************************************************************
 * 文件名: SystemErr.h
 * 文件历史:
 * 版本号    V1.0
 * 日期      2017.10.31
 * 作者      qgk 
*****************************************************************************/
/*=============================================================================
*******修改日志*******
1、日    期：
地    点：
修 改 人：
添加内容：
版本号:
2、日    期：
地    点：
修 改 人：
添加内容：
版本号:
================================================================================*/
#ifndef	_qSystemErr_H_
#define	_qSystemErr_H_
#include "stm32f10x.h"
/**********************************************************
故障类型选择，请注释掉不用的类型。
***********************************************************/
#define USE_11      //开入故障选择
// #define USE_12      //开出故障选择
#define USE_13      //A/D故障选择
// #define USE_14      //D/A故障选择
// #define USE_15      //脉冲量故障选择
// #define USE_16      //串口故障
#define USE_17      //网络通讯故障
#define USE_18      //流程控制错误
// #define USE_19      //数据处理错误
// #define USE_20      //数据存储错误
// #define USE_21      //参数错误
// #define USE_22      //故障预留1
// #define USE_23      //故障预留2
/**********************************************************
故障通道号个数设置，只需修改所用故障通道号个数，所有的
通道号个数不能设置为0。
***********************************************************/
#define Pass_11_Number 4      //开入故障通道号数量设置     0制动到位  1举升到位  2左停机  3右停机
#define Pass_12_Number 1      //开出故障通道号数量设置     
#define Pass_13_Number 3      //A/D故障通道号数量设置       0左制动    1右制动    2复合称重
#define Pass_14_Number 1      //D/A故障通道号数量设置
#define Pass_15_Number 1      //脉冲量故障通道号数量设置
#define Pass_16_Number 1      //串口故障通道号数量设置
#define Pass_17_Number 1      //网络通讯故障通道号数量设置
#define Pass_18_Number 1      //流程控制故障通道号数量设置
#define Pass_19_Number 1      //数据处理故障通道号数量设置
#define Pass_20_Number 1      //数据存储故障通道号数量设置
#define Pass_21_Number 1      //参数错误故障通道号数量设置
/**********************************************************
引用外部变量
***********************************************************/
// extern unsigned char   Pass_11_Number_Set[Pass_11_Number];      //开入故障通道索引数组
// extern unsigned char   Pass_12_Number_Set[Pass_12_Number];      //开出故障通道索引数组
// extern unsigned char   Pass_13_Number_Set[Pass_13_Number];      //A/D故障通道索引数组
// extern unsigned char   Pass_14_Number_Set[Pass_14_Number];      //D/A故障通道索引数组
// extern unsigned char   Pass_15_Number_Set[Pass_15_Number];      //脉冲量故障通道索引数组
// extern unsigned char   Pass_16_Number_Set[Pass_16_Number];      //串口故障通道索引数组
// extern unsigned char   Pass_17_Number_Set[Pass_17_Number];      //网络通讯故障索引数组
// extern unsigned char   Pass_18_Number_Set[Pass_18_Number];      //流程控制故障索引数组
// extern unsigned char   Pass_19_Number_Set[Pass_19_Number];      //数据处理故障索引数组
// extern unsigned char   Pass_20_Number_Set[Pass_20_Number];      //数据存储故障索引数组
// extern unsigned char   Pass_21_Number_Set[Pass_21_Number];      //参数错误故障索引数组


/**********************************************************
故障码类型定义
***********************************************************/

typedef union  
{
    unsigned char 	ucSystemErr[3];
    struct 
    {
        unsigned char ucErr_Code;     //故障代码
        unsigned char ucPass;         //故障通道
        unsigned char ucErr_State;    //故障状态
    }
    stSystemErr;
}
unSystemErr_TypeDef;	


/**********************************************************
故障标志位定义
***********************************************************/
/********************开入故障标志位定义********************/
typedef enum 
{
      es11_Err_0,      //故障状态：断
      es11_Err_1,      //故障状态：通
			es11_Err_N       //无故障
}
st11_ErrFlag_TypeDef;
extern st11_ErrFlag_TypeDef st11_ErrFlag;

/********************开出故障标志位定义********************/		
typedef struct 
{
    ErrorStatus  es12_Err_0;      //故障状态：断
    ErrorStatus  es12_Err_1;      //故障状态：通
		
}
st12_ErrFlag_TypeDef;
/********************A/D故障标志位定义********************/		
typedef enum 
{
      es13_Err_0,      //故障状态：零漂
      es13_Err_1,      //故障状态：最大溢出
      es13_Err_2,      //故障状态：最小溢出
      es13_Err_3,      //故障状态：信号跳动
      es13_Err_4,      //故障状态：标定系数
			es13_Err_N       //无故障
}
st13_ErrFlag_TypeDef;
extern st13_ErrFlag_TypeDef st13_ErrFlag;
/********************D/A故障标志位定义********************/		
typedef struct 
{
    ErrorStatus  es14_Err_0;      //故障状态：零漂
    ErrorStatus  es14_Err_1;      //故障状态：最大溢出
    ErrorStatus  es14_Err_2;      //故障状态：最小溢出
    ErrorStatus  es14_Err_3;      //故障状态：信号跳动
    ErrorStatus  es14_Err_4;      //故障状态：标定系数
}
st14_ErrFlag_TypeDef;
/******************脉冲量故障标志位类型定义********************/		
typedef struct 
{
    ErrorStatus  es15_Err_0;      //无脉冲
    ErrorStatus  es15_Err_1;      //丢脉冲
}
st15_ErrFlag_TypeDef;

/******************串口故障标志位类型定义********************/		
typedef struct 
{
    ErrorStatus  es16_Err_0;      //连接失败
    ErrorStatus  es16_Err_1;      //传输错误
}
st16_ErrFlag_TypeDef;
/******************网络通讯故障标志位类型定义********************/		
#define ERR_Net_ID 0
typedef enum 
{
    es17_Err_0,     //连接失败
    es17_Err_1,      //传输错误
		es17_Err_N       //无故障
}
st17_ErrFlag_TypeDef;
extern st17_ErrFlag_TypeDef st17_ErrFlag;

/******************流程控制故障标志位类型定义********************/		
#define ERR_Test_ID 0
typedef enum 
{
    es18_Err_0,      //控制中断
		es18_Err_1,      //第三滚筒异常  第三滚连续异常次数超过10次
		es18_Err_N,      //无故障
}
st18_ErrFlag_TypeDef;
extern st18_ErrFlag_TypeDef st18_ErrFlag;

/******************数据处理故障标志位类型定义********************/		
typedef struct 
{
    ErrorStatus  es19_Err_0;      //数据类型
    ErrorStatus  es19_Err_1;      //数据越界
}
st19_ErrFlag_TypeDef;

/******************数据存储故障标志位类型定义********************/		
typedef struct 
{
    ErrorStatus  es20_Err_0;      //数据类型
    ErrorStatus  es20_Err_1;      //数据越界
    ErrorStatus  es20_Err_2;      //连接失败
    ErrorStatus  es20_Err_3;      //空间溢出
}
st20_ErrFlag_TypeDef;

/******************参数故障标志位类型定义********************/		
typedef struct 
{
    ErrorStatus  es21_Err_0;      //参数错误
}
st21_ErrFlag_TypeDef;


extern    unSystemErr_TypeDef  un11_Err[Pass_11_Number];        // 开入通道故障码定义
extern    unSystemErr_TypeDef  un12_Err[Pass_12_Number];        // 开出通道故障码定义
extern    unSystemErr_TypeDef  un13_Err[Pass_13_Number];        // A/D故障码定义
extern    unSystemErr_TypeDef  un14_Err[Pass_14_Number];        // D/A故障码定义
extern    unSystemErr_TypeDef  un15_Err[Pass_15_Number];        // 脉冲量故障码定义
extern    unSystemErr_TypeDef  un16_Err[Pass_16_Number];        // 串口故障码定义
extern    unSystemErr_TypeDef  un17_Err[Pass_17_Number];        // 网络通讯故障码定义
extern    unSystemErr_TypeDef  un18_Err[Pass_18_Number];        // 流程控制故障码定义
extern    unSystemErr_TypeDef  un19_Err[Pass_19_Number];        // 数据处理故障码定义
extern    unSystemErr_TypeDef  un20_Err[Pass_20_Number];        // 数据存储故障码定义
extern    unSystemErr_TypeDef  un21_Err[Pass_21_Number];        // 参数错误故障码定义

// /*******************************************************************************
// 故障码标志位定义
// *******************************************************************************/

// extern st11_ErrFlag_TypeDef   st11_ErrFlag[Pass_11_Number];   // 开入通道故障标志位定义
// extern st12_ErrFlag_TypeDef   st12_ErrFlag[Pass_12_Number];   // 开出通道故障标志位定义
// extern st13_ErrFlag_TypeDef   st13_ErrFlag[Pass_13_Number];   // A/D通道故障标志位定义
// extern st14_ErrFlag_TypeDef   st14_ErrFlag[Pass_14_Number];   // D/A通道故障标志位定义
// extern st15_ErrFlag_TypeDef   st15_ErrFlag[Pass_15_Number];   // 脉冲量通道故障标志位定义
// extern st16_ErrFlag_TypeDef   st16_ErrFlag[Pass_16_Number];   // 串口故障标志位定义
// extern st17_ErrFlag_TypeDef   st17_ErrFlag[Pass_17_Number];   // 网络通讯故障标志位定义
// extern st18_ErrFlag_TypeDef   st18_ErrFlag[Pass_18_Number];   // 流程控制故障标志位定义
// extern st19_ErrFlag_TypeDef   st19_ErrFlag[Pass_19_Number];   // 数据处理故障标志位定义
// extern st20_ErrFlag_TypeDef   st20_ErrFlag[Pass_20_Number];   // 数据存储故障标志位定义
// extern st21_ErrFlag_TypeDef   st21_ErrFlag[Pass_21_Number];   // 参数错误故障标志位定义

// extern    void SystemErrTest(unsigned char *ucErrCode,unsigned short  *size);
extern    void SystemErr_Init(void);
extern    void SystemPass_Init(void);
extern void SystemErr_W5500Send(void);
extern void SystemErr_Test(void);
#endif
