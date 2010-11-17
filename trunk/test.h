#ifndef __TEST_H__
#define __TEST_H__
 //用于保存触目板上手指的信息
typedef struct stMyPanelPoint  
{
	short x;                        //X坐标
	short y;                        //y坐标
	u8		id;
}PanelPoint;

char FillHidPacket(PanelPoint *MyPoint,u8 PointNum);
#endif