#ifndef __TEST_H__
#define __TEST_H__
 //���ڱ��津Ŀ������ָ����Ϣ
typedef struct stMyPanelPoint  
{
	short x;                        //X����
	short y;                        //y����
	u8		id;
}PanelPoint;

char FillHidPacket(PanelPoint *MyPoint,u8 PointNum);
#endif