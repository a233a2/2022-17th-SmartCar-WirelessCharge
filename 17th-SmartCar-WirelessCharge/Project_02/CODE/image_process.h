
#ifndef  _search_line_h
#define  _search_line_h



//找边线
void seek_line(void);

//简化边线显示用调试用
void Pixle_Show(void);

//OTSU
uint8 otsuThreshold_Pro(uint8 *image, uint16 width, uint16 height);

//二值化
void Binaryzation(uint8 *image,uint16 width, uint16 height,uint8 image_threshold);

//裁切
void Get_use_image(void);

void fix_image(void);

//腐蚀滤波
uint8 Corrode_Filter(uint8 i, uint8 *image, uint8 Left_Min, uint8 Right_Max);

int32 range_protect(uint8 duty, uint8 min, uint8 max);

//整合
void image_process1(void);

//去除单一亮/黑点
void OnePoint_fix(void);

//十字判断
void Element_Cross(void);

//P回环处理
void Element_BackCross(void);
void Element_BackCross2(void);

void Backcross_process(void);

//判断线段是否单调
void Analyse_line_straight(uint8 dir);  //返回0则不单调 存在突变 返回1则边线单调

//环岛处理
void Round_Processer(void);  //使用完毕 Round_Flag 置为0

void Round_Processer_Charge1(void); //1号速度使用
void Round_Processer_Charge2(void); //2号速度使用
void Round_Processer_Charge3(void); //3号速度使用
void Round_Processer_Charge4(void); //4号速度使用


void Round_Process_Select(void);
//环岛判断
void Element_Round_Pro(int DIR);


//三叉判断
void Element_Three_Pro(void);
//三叉处理
void Three_Processor(void);

//斑马线检测
void Element_Zebra(void);

//斑马线元素处理
void Element_Zebra_Process(void);



//整体元素处理打包
void Element_Judge(void);
void Element_Do(void);
void Element(void);

//中线修复  补线完成后 调用 修改中线
void Mid_Line_Fix(uint8 DATA);

//元素展示 调试用
void Element_show(void);

//出库程序 入库???
void Out_Garage(void);
void Out_Garage2(void);


extern uint8 camear_foward;
extern uint8 up_line;  //上线
extern uint8 zuo_line; // 前瞻 左线
extern uint8 you_line; // 前瞻 右线
extern uint8 middle_line; // 前瞻 中线
extern uint8 middle_line1; // 前瞻 中线
//边线数组
extern uint8 ImageSide[MT9V03X_H2][2];  //1左边线 2右边线
extern uint8 UpdowmSide[2][MT9V03X_W2];  //上下边线 ?
extern uint8 ImageMid[MT9V03X_H2];  //中线

extern uint8 Road_width[MT9V03X_H2];  //赛道宽度

extern uint8 low_line;  //上丢线行数 重要参数

//环岛标志位
extern uint8 Round_Road_Flag;
//1遇到 2进入 3出环岛 4离开环岛
extern uint8 Round_Statue;
//环岛 延时
extern uint8 Round_Cnt_Boring;
extern uint8 Round_Cnt;
//十字回环标志位
extern uint8 Cross_Road_Flag;

//三叉标志位
extern uint8 Three_Fork_Flag;
extern uint8 Three_Fork_Cnt;


//斑马线标志位
extern uint8 Zebra_Flag;
extern uint8 Zebra_cnt;


//Loss_line 丢线标志位
extern uint8 Loss_line_Flag;
extern uint8 Loss_line;

//补线 选择器
extern uint8 Fix_Line_Flag;

//元素正在处理标志位 元素处理时关元素判断
extern uint8 Element_Processing_Flag;

//目标角度改变标志位  达到目标角度进行下一步操作或者退出操作  三岔 60关    环岛 60关进线  260 开出线  360 关出线
extern uint8 Angle_CNT_Flag;

//回环标志位
extern uint8 Back_Cross_Flag;

//P回环帧数检测统计数  无元素且连续10帧数为偏移时候判断为回环 之后切线循迹 究其根源还是赛道太简单了
extern sint32 Back_Road_FPS_CNT1; //左
extern sint32 Back_Road_FPS_CNT2; //右

//出回环标志位
extern uint8 Back_Out_Flag;
//出回环计数器
extern uint8 Back_Out_Boring;
extern uint8 Back_Cnt;  //回环计数器

//左右边线正常与否标志位 1正常 0不正常
extern uint8 Zuo_Correct_Flag;
extern uint8 You_Correct_Flag;

//左右数组丢线时的值
extern uint8 Zuo_Hind;
extern uint8 You_Hind;

//腐蚀滤波标志位 1为开 0为关
extern uint8 Eat_Line_Flag;

//出库标志位
extern uint8 Out_Garage_Flag;

//extern uint8 Finish_Flag;
extern uint8 Judge_Stright_Amount;//直线判断量 以25/50符合要求为判断条件  速度1 25   速度2 20 速度3 20
#endif
