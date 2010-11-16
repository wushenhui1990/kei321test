#ifndef __PANELPOINT_H__
#define __PANELPOINT_H__

#define IMAGE_POINT_MAX_NUM 10

#define PANEL_POINT_MAX_NUM 10

#define IMAGE_POINT_KEEP 5  //图像上的点要保存的历史点数，此数不能大于宏定义IMAGE_POINT_MAX_NUM，


#define PANEL_POINT_MAX_NUM		10
#define MAX_POINT				4 //最多检测的点数


#define abs(a) ((a)>0?(a):(-(a)))

//用于保存图像上的点的信息
typedef struct stMyPoint  
{
	unsigned short x;                     //点的X坐标
	unsigned short width;                    //点的宽度
	short Velocity;                  //点的瞬时速度
	unsigned char ID;             //点的ID
	short Vel[IMAGE_POINT_MAX_NUM];           //点的历史速度    
    unsigned char VelNum;
}ImagePoint;

//其他的一些控制参数
typedef struct stControlPara
{
//	unsigned char ImagePointKeepNum; //图像上的点要保存的历史点数，此数不能大于宏定义IMAGE_POINT_MAX_NUM，
	unsigned char PanelPointKeepNum; //触摸板上的点要保存的历史点数，此数不能大于宏定义PANEL_POINT_MAX_NUM
	unsigned char WaitFrame;         //图像不同步时的停留帧数
	unsigned char TwoPointDis;      //判断图像分离合并时的距离
	char PathSmoothMethod; //路径平滑方法，若为true，采用方法1，则PanelPointKeepNum应设为8到10，
	                       //若为false，采用方法2，则PanelPointKeepNum应设为2到4，
}ControlPara;

//用于保存触目板上手指的信息
typedef struct stMyPanelPoint  
{
#ifdef FLOAT_POINT
	double x;                        //X坐标
	double y;                        //y坐标
#else
	short x;                        //X坐标，单位像素（pixel）
	short y;                        //y坐标，单位像素（pixel）
#endif
    short VectX[PANEL_POINT_MAX_NUM];              //x方向上的历史坐标
	short VectY[PANEL_POINT_MAX_NUM];              //y方向上的历史坐标
	unsigned char VectNum;
	unsigned char ID;                         //手指的ID
}PanelPoint;

typedef struct stClbPoint
{
	short x;
	short y;
}ClbPoint;

typedef struct stClbLens
{
	ClbPoint P1_l2;			//参考点1坐标
	ClbPoint P2_l2;			//参考点2坐标
	unsigned short X1_l1;	//参考点1 对应 X
	unsigned short X2_l1;	//参考点2 对应 X
	unsigned short invDX_l18;	//	( 1/(X2-X1) 
	ClbPoint Po_l4;			//摄像头中心点坐标
}ClbLens;

//标定的参数
typedef struct stClbInfo
{
	ClbLens	LCam;
	ClbLens	RCam;
}ClbInfo;

//显示器屏幕参数
typedef struct stScreenPara
{
	unsigned short ScreenWidth;            //屏幕横向分辨率（像素）
	unsigned short ScreenHight;            //屏幕纵向分辨率（像素）
}ScreenPara;

typedef struct _CLB_PARA_{
	ClbInfo clbInfo;
	ScreenPara screenM;
	//ImageProcessPara ImageProcessPara_L;
	//ImageProcessPara ImageProcessPara_R;
}ClbPara;


//计算过程中用到的临时变量和参数
typedef struct stGlobalPara
{
	PanelPoint LastMyPoint[2];					//触摸板上前一帧的点
	unsigned char LastPointNum;
	unsigned char ID;									//触摸板上点的ID

	char bStartFrame;							//标记是否起始帧
	char bUpdateBackground;
	char bUpdatePanelPoints;
	char bUpdateImagePoints;
	char bSimul00to22;

	unsigned short MergeFlag;
	char bWaitFlag;
	unsigned char WaitCount;								//当两幅图像点数改变时跳帧计数
	char MatchStyle;							//配对方式(多点时有意义): 0-默认值；1-顺序配对;2-交叉配对

    //改为static
	ImagePoint LastMyPoint1[MAX_POINT];
	ImagePoint LastMyPoint2[MAX_POINT];
	//左右图像上前一帧的点数
	unsigned char LastPointNum1;
	unsigned char LastPointNum2;

	ClbInfo gClbPara;
	ScreenPara gScreenPara;
	ControlPara gControlPara;

}GlobalPara;



#endif
