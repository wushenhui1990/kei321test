#ifndef __PANELPOINT_H__
#define __PANELPOINT_H__

#define IMAGE_POINT_MAX_NUM 10

#define PANEL_POINT_MAX_NUM 10

#define IMAGE_POINT_KEEP 5  //ͼ���ϵĵ�Ҫ�������ʷ�������������ܴ��ں궨��IMAGE_POINT_MAX_NUM��


#define PANEL_POINT_MAX_NUM		10
#define MAX_POINT				4 //�����ĵ���


#define abs(a) ((a)>0?(a):(-(a)))

//���ڱ���ͼ���ϵĵ����Ϣ
typedef struct stMyPoint  
{
	unsigned short x;                     //���X����
	unsigned short width;                    //��Ŀ��
	short Velocity;                  //���˲ʱ�ٶ�
	unsigned char ID;             //���ID
	short Vel[IMAGE_POINT_MAX_NUM];           //�����ʷ�ٶ�    
    unsigned char VelNum;
}ImagePoint;

//������һЩ���Ʋ���
typedef struct stControlPara
{
//	unsigned char ImagePointKeepNum; //ͼ���ϵĵ�Ҫ�������ʷ�������������ܴ��ں궨��IMAGE_POINT_MAX_NUM��
	unsigned char PanelPointKeepNum; //�������ϵĵ�Ҫ�������ʷ�������������ܴ��ں궨��PANEL_POINT_MAX_NUM
	unsigned char WaitFrame;         //ͼ��ͬ��ʱ��ͣ��֡��
	unsigned char TwoPointDis;      //�ж�ͼ�����ϲ�ʱ�ľ���
	char PathSmoothMethod; //·��ƽ����������Ϊtrue�����÷���1����PanelPointKeepNumӦ��Ϊ8��10��
	                       //��Ϊfalse�����÷���2����PanelPointKeepNumӦ��Ϊ2��4��
}ControlPara;

//���ڱ��津Ŀ������ָ����Ϣ
typedef struct stMyPanelPoint  
{
#ifdef FLOAT_POINT
	double x;                        //X����
	double y;                        //y����
#else
	short x;                        //X���꣬��λ���أ�pixel��
	short y;                        //y���꣬��λ���أ�pixel��
#endif
    short VectX[PANEL_POINT_MAX_NUM];              //x�����ϵ���ʷ����
	short VectY[PANEL_POINT_MAX_NUM];              //y�����ϵ���ʷ����
	unsigned char VectNum;
	unsigned char ID;                         //��ָ��ID
}PanelPoint;

typedef struct stClbPoint
{
	short x;
	short y;
}ClbPoint;

typedef struct stClbLens
{
	ClbPoint P1_l2;			//�ο���1����
	ClbPoint P2_l2;			//�ο���2����
	unsigned short X1_l1;	//�ο���1 ��Ӧ X
	unsigned short X2_l1;	//�ο���2 ��Ӧ X
	unsigned short invDX_l18;	//	( 1/(X2-X1) 
	ClbPoint Po_l4;			//����ͷ���ĵ�����
}ClbLens;

//�궨�Ĳ���
typedef struct stClbInfo
{
	ClbLens	LCam;
	ClbLens	RCam;
}ClbInfo;

//��ʾ����Ļ����
typedef struct stScreenPara
{
	unsigned short ScreenWidth;            //��Ļ����ֱ��ʣ����أ�
	unsigned short ScreenHight;            //��Ļ����ֱ��ʣ����أ�
}ScreenPara;

typedef struct _CLB_PARA_{
	ClbInfo clbInfo;
	ScreenPara screenM;
	//ImageProcessPara ImageProcessPara_L;
	//ImageProcessPara ImageProcessPara_R;
}ClbPara;


//����������õ�����ʱ�����Ͳ���
typedef struct stGlobalPara
{
	PanelPoint LastMyPoint[2];					//��������ǰһ֡�ĵ�
	unsigned char LastPointNum;
	unsigned char ID;									//�������ϵ��ID

	char bStartFrame;							//����Ƿ���ʼ֡
	char bUpdateBackground;
	char bUpdatePanelPoints;
	char bUpdateImagePoints;
	char bSimul00to22;

	unsigned short MergeFlag;
	char bWaitFlag;
	unsigned char WaitCount;								//������ͼ������ı�ʱ��֡����
	char MatchStyle;							//��Է�ʽ(���ʱ������): 0-Ĭ��ֵ��1-˳�����;2-�������

    //��Ϊstatic
	ImagePoint LastMyPoint1[MAX_POINT];
	ImagePoint LastMyPoint2[MAX_POINT];
	//����ͼ����ǰһ֡�ĵ���
	unsigned char LastPointNum1;
	unsigned char LastPointNum2;

	ClbInfo gClbPara;
	ScreenPara gScreenPara;
	ControlPara gControlPara;

}GlobalPara;



#endif
