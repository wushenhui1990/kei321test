#include "PanelPoint.h"

#if 0

#define DEBUG_PM	0


//往数组后面添加元素
static void ArrayPushBack( short *Array, unsigned char *ArrayNum, unsigned short Element)
{
	if ((*ArrayNum) < 0 )
		return;
	
    Array[(*ArrayNum)]=Element;
	(*ArrayNum)++;
}

//删除int数组的元素
static void ArrayErase( short *Array, unsigned char *ArrayNum, unsigned char Index)
{
	unsigned char i=0;

	if ((*ArrayNum)<=0 || Index<0 || Index > (*ArrayNum)-1)
	{
		return;
	}

	if(Index == (*ArrayNum)-1)
	{
		(*ArrayNum)--;
		return;
	}

	for (i=Index; i<(*ArrayNum)-1; i++)
	{
		Array[i]= Array[i+1];
	}
	(*ArrayNum)--;
}

//-----------------------------------------------------------------------------------------------------
// smooth function
//
//-----------------------------------------------------------------------------------------------------
static void TrackSmooth(GlobalPara *globalPara, PanelPoint *MyPoint, unsigned char PointNum)
{
	unsigned char i=0,j=0;
	short XSum=0;
	short YSum=0;
	short SwitchX = 0;
	short SwitchY = 0;
	unsigned char SwitchID = 0;

	if(globalPara->LastPointNum==0)
	{
		MyPoint[0].VectNum = 0;
		MyPoint[1].VectNum = 0;
	}
	else
	{
		if(1==PointNum && 1==globalPara->LastPointNum)
		{
			MyPoint[0].VectNum = globalPara->LastMyPoint[0].VectNum ;
			for (i=0; i<globalPara->LastMyPoint[0].VectNum; i++)
			{
				MyPoint[0].VectX[i]=globalPara->LastMyPoint[0].VectX[i];
				MyPoint[0].VectY[i]=globalPara->LastMyPoint[0].VectY[i];
			}
		} /*
		else if(1==PointNum && 2==globalPara->LastPointNum)
		{
			if(MyPoint[0].ID == globalPara->LastMyPoint[0].ID)
			{
				MyPoint[0].VectNum = globalPara->LastMyPoint[0].VectNum ;
				for (i=0; i<globalPara->LastMyPoint[0].VectNum; i++)
				{
					MyPoint[0].VectX[i]=globalPara->LastMyPoint[0].VectX[i];
					MyPoint[0].VectY[i]=globalPara->LastMyPoint[0].VectY[i];
				}
			}
			else
			{
				MyPoint[0].VectNum = globalPara->LastMyPoint[1].VectNum ;
				for (i=0; i<globalPara->LastMyPoint[1].VectNum; i++)
				{
					MyPoint[0].VectX[i]=globalPara->LastMyPoint[1].VectX[i];
					MyPoint[0].VectY[i]=globalPara->LastMyPoint[1].VectY[i];
				}
			}
		}
		else if(2==PointNum && 1==globalPara->LastPointNum)
		{
			if(MyPoint[1].ID==globalPara->LastMyPoint[0].ID)
			{
				SwitchX = MyPoint[1].x;
				SwitchY = MyPoint[1].y;
				SwitchID = MyPoint[1].ID;
				MyPoint[1].x = MyPoint[0].x;
				MyPoint[1].y = MyPoint[0].y;
				MyPoint[1].ID = MyPoint[0].ID;
				MyPoint[0].x = SwitchX;
				MyPoint[0].y = SwitchY;
				MyPoint[0].ID = SwitchID;
			}
			MyPoint[0].VectNum = globalPara->LastMyPoint[0].VectNum ;
			for (i=0; i<globalPara->LastMyPoint[0].VectNum; i++)
			{
				MyPoint[0].VectX[i]=globalPara->LastMyPoint[0].VectX[i];
				MyPoint[0].VectY[i]=globalPara->LastMyPoint[0].VectY[i];
			}
			MyPoint[1].VectNum = 0;
		}
		else if(2==PointNum && 2==globalPara->LastPointNum)
		{
			if(MyPoint[1].ID==globalPara->LastMyPoint[0].ID)
			{
				SwitchX = MyPoint[1].x;
				SwitchY = MyPoint[1].y;
				SwitchID = MyPoint[1].ID;
				MyPoint[1].x = MyPoint[0].x;
				MyPoint[1].y = MyPoint[0].y;
				MyPoint[1].ID = MyPoint[0].ID;
				MyPoint[0].x = SwitchX;
				MyPoint[0].y = SwitchY;
				MyPoint[0].ID = SwitchID;
			}
			MyPoint[0].VectNum = globalPara->LastMyPoint[0].VectNum ;
			for (i=0; i<globalPara->LastMyPoint[0].VectNum; i++)
			{
				MyPoint[0].VectX[i]=globalPara->LastMyPoint[0].VectX[i];
				MyPoint[0].VectY[i]=globalPara->LastMyPoint[0].VectY[i];
			}
			MyPoint[1].VectNum = globalPara->LastMyPoint[1].VectNum ;
			for (i=0; i<globalPara->LastMyPoint[1].VectNum; i++)
			{
				MyPoint[1].VectX[i]=globalPara->LastMyPoint[1].VectX[i];
				MyPoint[1].VectY[i]=globalPara->LastMyPoint[1].VectY[i];
			}
		}  */
	}

	//路径平滑
	for (i=0;i<PointNum;i++)
	{
		XSum=0;
		YSum=0;

		if (globalPara->gControlPara.PathSmoothMethod)//平滑方法1
		{
			if(MyPoint[i].VectNum==globalPara->gControlPara.PanelPointKeepNum)
			{
				ArrayErase(MyPoint[i].VectX, &MyPoint[i].VectNum,0);
				MyPoint[i].VectNum += 1;
				ArrayErase(MyPoint[i].VectY, &MyPoint[i].VectNum,0);
			}

			ArrayPushBack(MyPoint[i].VectX, &MyPoint[i].VectNum ,MyPoint[i].x);
			MyPoint[i].VectNum -= 1;
			ArrayPushBack(MyPoint[i].VectY, &MyPoint[i].VectNum ,MyPoint[i].y);

			if(MyPoint[i].VectNum>0)
			{
				for (j=0;j<MyPoint[i].VectNum;j++)
				{
					XSum +=MyPoint[i].VectX[j];
					YSum +=MyPoint[i].VectY[j];
				}

				MyPoint[i].x = XSum / MyPoint[i].VectNum;
				MyPoint[i].y = YSum / MyPoint[i].VectNum;
			}
		}
		else//平滑方法2
		{
			for (j=0;j<MyPoint[i].VectNum;j++)
			{
				XSum += MyPoint[i].VectX[j];
				YSum += MyPoint[i].VectY[j];
			}

			MyPoint[i].x= (XSum + MyPoint[i].x) / (MyPoint[i].VectNum+1);
			MyPoint[i].y= (YSum + MyPoint[i].y) / (MyPoint[i].VectNum+1);

			if(MyPoint[i].VectNum==globalPara->gControlPara.PanelPointKeepNum)
			{
				ArrayErase(MyPoint[i].VectX, &MyPoint[i].VectNum,0);
				MyPoint[i].VectNum += 1;
				ArrayErase(MyPoint[i].VectY, &MyPoint[i].VectNum,0);
			}

			ArrayPushBack(MyPoint[i].VectX, &MyPoint[i].VectNum ,MyPoint[i].x);
			MyPoint[i].VectNum -= 1;
			ArrayPushBack(MyPoint[i].VectY, &MyPoint[i].VectNum ,MyPoint[i].y);

		}
	}
}
/*
static void SetPointsIDOnDetachWithStillPoint(GlobalPara *globalPara,PanelPoint *MyPoint)
{
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned short dis = 0;
	unsigned short MinDis = 10000;
	unsigned char NowIndex = 0;
	unsigned char LastIndex = 0;

	for(j=0;j<2;j++)
	{
		for(i=0;i<2;i++)
		{
			dis = abs(MyPoint[j].x - globalPara->LastMyPoint[i].x) + abs(MyPoint[j].y - globalPara->LastMyPoint[i].y);
			if(dis < MinDis)
			{
				MinDis = dis;
				LastIndex = i;
				NowIndex = j;
			}
		}
	}
	MyPoint[NowIndex].ID = globalPara->LastMyPoint[LastIndex].ID;
	MyPoint[1-NowIndex].ID = globalPara->LastMyPoint[1-LastIndex].ID;
}
*/
//---------------------------------------------------------------------------------------------------------------------------
//setting panel points ID referring to last frame points info
//
//
//---------------------------------------------------------------------------------------------------------------------------
static void SetPointsID(GlobalPara *globalPara,PanelPoint *MyPoint,unsigned char PointNum)
{
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned short dis = 0;
	unsigned short MinDis = 10000;
	unsigned char NowIndex = 0;
	unsigned char LastIndex = 0;
	short VelX = 0;
	short VelY = 0;
	unsigned short VelMaxAbs = 0;
	unsigned short DeltX = 0;
	unsigned short DeltY = 0;

	
	if (1==PointNum && 1==globalPara->LastPointNum)
	{
		MyPoint[0].ID = globalPara->LastMyPoint[0].ID;
	}
	else if(0!=PointNum && 0!=globalPara->LastPointNum)
	{
		if ( (globalPara->LastMyPoint[0].VectNum>=2) && (globalPara->LastMyPoint[globalPara->LastPointNum].VectNum>=2) )
		{
			for(j=0;j<PointNum;j++)
			{
				for(i=0;i<globalPara->LastPointNum;i++)
				{
					VelX = globalPara->LastMyPoint[i].VectX[globalPara->LastMyPoint[i].VectNum-1] - globalPara->LastMyPoint[i].VectX[globalPara->LastMyPoint[i].VectNum-2];
					VelY = globalPara->LastMyPoint[i].VectY[globalPara->LastMyPoint[i].VectNum-1] - globalPara->LastMyPoint[i].VectY[globalPara->LastMyPoint[i].VectNum-2];
					VelMaxAbs = (abs(VelX) >= abs(VelY)) ? abs(VelX) : abs(VelY);
					DeltX = abs(MyPoint[j].x - globalPara->LastMyPoint[i].VectX[globalPara->LastMyPoint[i].VectNum-1] - VelX);
					DeltY = abs(MyPoint[j].y - globalPara->LastMyPoint[i].VectY[globalPara->LastMyPoint[i].VectNum-1] - VelY);
					if(DeltX > VelMaxAbs)
						DeltX += (DeltX - VelMaxAbs);
					if(DeltY > VelMaxAbs)
						DeltY += (DeltY - VelMaxAbs);
					dis = DeltX + DeltY;
					if (dis<MinDis)
					{
						MinDis = dis;
						LastIndex = i;
						NowIndex = j;
					}
				}
			}
		}
		else
		{
			for(j=0;j<PointNum;j++)
			{
				for(i=0;i<globalPara->LastPointNum;i++)
				{
					dis = abs(MyPoint[j].x - globalPara->LastMyPoint[i].x) + abs(MyPoint[j].y - globalPara->LastMyPoint[i].y);
					if(dis < MinDis)
					{
						MinDis = dis;
						LastIndex = i;
						NowIndex = j;
					}
				}
			}
		}
		MyPoint[NowIndex].ID = globalPara->LastMyPoint[LastIndex].ID;
		if(2==PointNum && 2==globalPara->LastPointNum)
		{
			MyPoint[1-NowIndex].ID = globalPara->LastMyPoint[1-LastIndex].ID;
		}
		else if(2==PointNum && 1==globalPara->LastPointNum)
		{
			MyPoint[1-NowIndex].ID = globalPara->ID;
			globalPara->ID++;
		}
	}
}

static void GetInterSection(ClbPoint PL1_l4, ClbPoint PLo_l4, ClbPoint PR1_l4, ClbPoint PRo_l4, short* X, short* Y)
{
	int XL, YL, XR, YR, Dx, Dy;
	int t = 0;

	XL = PL1_l4.x - PLo_l4.x;
	YL = PL1_l4.y - PLo_l4.y;
	XR = PR1_l4.x - PRo_l4.x;
	YR = PR1_l4.y - PRo_l4.y;
	Dx = PRo_l4.x - PLo_l4.x;
	Dy = PRo_l4.y - PLo_l4.y;
	
	if (0 != ((XL*YR - YL*XR)>>13))
	{
		t = (Dx*YR - Dy*XR)/((XL*YR - YL*XR) >> 13);
		*X = (short)((PLo_l4.x + ((XL * t + 0x1000) >> 13) + 0x8) >> 4);
		*Y = (short)((PLo_l4.y + ((YL * t + 0x1000) >> 13) + 0x8) >> 4);
	}
	else		//divided by 0 error!!!
	{
		*X = -1;
		*Y = -1;
	}
}
  
static void GetMyXY(ClbInfo clbInfo, int X1_l1, int X2_l1, short *X, short *Y)	// X1,X2 = X_left + X_right
{
	int	delt_x, delt_y, U32_Rat;
	ClbPoint Pleft_l4, Pright_l4;

	U32_Rat = (X1_l1 - clbInfo.LCam.X1_l1) * clbInfo.LCam.invDX_l18;		// 2^19, invDX_l18 is actually l17 (1/del(X_l1) << 18)
	delt_x = U32_Rat * (clbInfo.LCam.P2_l2.x - clbInfo.LCam.P1_l2.x);		// 2^19 * 2^11
	delt_y = U32_Rat * (clbInfo.LCam.P2_l2.y - clbInfo.LCam.P1_l2.y);		// 2^19 * 2^11
	Pleft_l4.x = (((clbInfo.LCam.P1_l2.x << 8) + ((delt_x + 0x200) >> 10) + 0x20) >> 6);
	Pleft_l4.y = (((clbInfo.LCam.P1_l2.y << 8) + ((delt_y + 0x200) >> 10) + 0x20) >> 6);

	U32_Rat = (X2_l1 - clbInfo.RCam.X1_l1) * clbInfo.RCam.invDX_l18;
	delt_x = U32_Rat * (clbInfo.RCam.P2_l2.x - clbInfo.RCam.P1_l2.x);
	delt_y = U32_Rat * (clbInfo.RCam.P2_l2.y - clbInfo.RCam.P1_l2.y);
	Pright_l4.x = (((clbInfo.RCam.P1_l2.x << 8) + ((delt_x + 0x200) >> 10) + 0x20) >> 6);
	Pright_l4.y = (((clbInfo.RCam.P1_l2.y << 8) + ((delt_y + 0x200) >> 10) + 0x20) >> 6);

	GetInterSection(Pleft_l4, clbInfo.LCam.Po_l4, Pright_l4, clbInfo.RCam.Po_l4, X, Y);
}

//----------------------------------------------------------------------
//wait to check
//
//
//----------------------------------------------------------------------
static void WaitToCheck(GlobalPara *globalPara)
{
	if(0==globalPara->bWaitFlag)
	{
		globalPara->bWaitFlag = 1;
		globalPara->WaitCount++;
	}
	else
	{
		if(globalPara->WaitCount < globalPara->gControlPara.WaitFrame)
		{
			globalPara->WaitCount++;
		}
		else		//error, background should be updated
		{
			//update background------------------------------
			globalPara->bUpdateBackground = 1;
			globalPara->bWaitFlag = 0;
			globalPara->WaitCount = 0;
		}
	}
}

//---------------------------------------------------------------------------------------------
//	reset parameters when no points on screen
//
//---------------------------------------------------------------------------------------------
static void ResetParam(GlobalPara *globalPara)
{
	globalPara->ID=0;
	globalPara->WaitCount=0;
	globalPara->MergeFlag=0;
	globalPara->bWaitFlag=0;
	globalPara->LastPointNum1=0;
	globalPara->LastPointNum2=0;
    globalPara->LastPointNum=0;
	globalPara->MatchStyle = 0;
	globalPara->bUpdateBackground = 0;
	globalPara->bUpdateImagePoints = 0;
	globalPara->bUpdatePanelPoints = 0;
	globalPara->bSimul00to22 = 0;
}

//---------------------------------------------------------------------------------------------------------------------------
//根据一个或两个手指两图像上的信息来计算手指在触摸板上的坐标，此函数用GetMyXY函数。
//
//
//---------------------------------------------------------------------------------------------------------------------------
static void GetPanelPoints(GlobalPara *globalPara,ImagePoint *LeftPoints,unsigned char LeftNum,ImagePoint *RightPoints,unsigned char RightNum,PanelPoint *MyPoint,unsigned char *PanelPointNum)
{
	unsigned char i = 0;
	unsigned char j = 0;
	unsigned short dis = 0;
	unsigned short MinDis = 10000;
	unsigned char NowIndex = 0;
	unsigned char LastIndex = 0;
	unsigned char LeftIndex = 0;
	unsigned char RightIndex = 0;
	unsigned char MatchFinish = 0;
	short VelX = 0;
	short VelY = 0;
	unsigned short VelMaxAbs = 0;
	unsigned short DeltX = 0;
	unsigned short DeltY = 0;
//	short MergeX[2];

	*PanelPointNum = 0;				//init
	globalPara->bUpdatePanelPoints = 0;
	globalPara->bUpdateBackground = 0;
	globalPara->bUpdateImagePoints = 0;

	if(0==LeftNum && 0==RightNum)
	{
#if(1==DEBUG_PM)
		//---------------------------------------------------------------------------------------------------------------
		fprintf(g_pf,"00\n");
		//-------------------------------------------------------------------------------------------------------------------
#endif
		//no points - reset all parameters
		ResetParam(globalPara);
		*PanelPointNum = 0;
		globalPara->bUpdateBackground = 1;
	}
	else if(0==LeftNum && 1==RightNum)
	{
#if(1==DEBUG_PM)
		//---------------------------------------------------------------------------------------------------------------
		fprintf(g_pf,"01\n");
		//-------------------------------------------------------------------------------------------------------------------
#endif

		//wait
		WaitToCheck(globalPara);
		*PanelPointNum = 0;
		globalPara->bUpdateImagePoints = 0;
		globalPara->bUpdatePanelPoints = 0;
		globalPara->MatchStyle = 0;
		globalPara->MergeFlag = 0;
	}
	else if(1==LeftNum && 0==RightNum)
	{
#if(1==DEBUG_PM)
		//---------------------------------------------------------------------------------------------------------------
		fprintf(g_pf,"10\n");
		//-------------------------------------------------------------------------------------------------------------------
#endif

		//wait
		WaitToCheck(globalPara);
		*PanelPointNum = 0;
		globalPara->bUpdateImagePoints = 0;
		globalPara->bUpdatePanelPoints = 0;
		globalPara->MatchStyle = 0;
		globalPara->MergeFlag = 0;
	}
	else if(1==LeftNum && 1==RightNum)
	{
#if(1==DEBUG_PM)
		//---------------------------------------------------------------------------------------------------------------
		fprintf(g_pf,"11\n");
		//-------------------------------------------------------------------------------------------------------------------
#endif

		//1 point
		GetMyXY(globalPara->gClbPara, LeftPoints[0].x,RightPoints[0].x, &MyPoint[0].x , &MyPoint[0].y);
		if (1==globalPara->LastPointNum || 2==globalPara->LastPointNum)
		{
			SetPointsID(globalPara,MyPoint,1);
		}
		else
		{
			MyPoint[0].ID = globalPara->ID;
			globalPara->ID++;
		}
		if (globalPara->bWaitFlag)
		{
			globalPara->bWaitFlag = 0;
			globalPara->WaitCount = 0;
		}
		*PanelPointNum = 1;
		globalPara->bUpdatePanelPoints = 1;
		globalPara->bUpdateImagePoints = 1;
		globalPara->MatchStyle = 0;
		globalPara->MergeFlag = 0;
#if(1==DEBUG_PM)
		//---------------------------------------------------------------------------------------------------------------
		fprintf(g_pf,"11:ID=%d,(%d,%d)\n",MyPoint[0].ID,MyPoint[0].x,MyPoint[0].y);
		//-------------------------------------------------------------------------------------------------------------------
#endif
	}/*
	else if(0==LeftNum && 2==RightNum)
	{
#if(1==DEBUG_PM)
		//---------------------------------------------------------------------------------------------------------------
		fprintf(g_pf,"02\n");
		//-------------------------------------------------------------------------------------------------------------------
#endif

		//wait
		WaitToCheck(globalPara);
		*PanelPointNum = 0;
		globalPara->bUpdateImagePoints = 0;
		globalPara->bUpdatePanelPoints = 0;
		globalPara->MatchStyle = 0;
		globalPara->MergeFlag = 0;
	}
	else if(2==LeftNum && 0==RightNum)
	{
#if(1==DEBUG_PM)
		//---------------------------------------------------------------------------------------------------------------
		fprintf(g_pf,"20\n");
		//-------------------------------------------------------------------------------------------------------------------
#endif

		//wait
		WaitToCheck(globalPara);
		*PanelPointNum = 0;
		globalPara->bUpdateImagePoints = 0;
		globalPara->bUpdatePanelPoints = 0;
		globalPara->MatchStyle = 0;
		globalPara->MergeFlag = 0;
	}
	else if(1==LeftNum && 2==RightNum)
	{
#if(1==DEBUG_PM)
		//-------------------------------------------------------------------------------------------------------------------
		fprintf(g_pf,"12\n");
		//-------------------------------------------------------------------------------------------------------------------
#endif
		//depend on last frame
		if (1==globalPara->MergeFlag)
		{
			if(1==globalPara->bWaitFlag && globalPara->WaitCount<globalPara->gControlPara.WaitFrame)
			{
				globalPara->WaitCount++;
				globalPara->bUpdateImagePoints = 1;
			}
			else
			{
				globalPara->bWaitFlag = 0;
				globalPara->WaitCount = 0;
				globalPara->MergeFlag = 2;
				globalPara->bUpdateImagePoints = 1;
			}
		}
		else if (globalPara->MergeFlag>=2)			//still merge
		{
	#if(1==DEBUG_PM)
			//-------------------------------------------------------------------------------------------------------------------
			fprintf(g_pf,"2toMerge:MergeFlag=%d,globalPara->LastPointNum1=%d,globalPara->LastPointNum2=%d\n",globalPara->MergeFlag,globalPara->LastPointNum1,globalPara->LastPointNum2);
			//-------------------------------------------------------------------------------------------------------------------
	#endif
			if ((2==globalPara->LastPointNum1) && (2==globalPara->LastPointNum2))
			{
				MergeX[0] = globalPara->LastMyPoint1[0].x + globalPara->MergeFlag * globalPara->LastMyPoint1[0].Velocity;
				if ( (MergeX[0]>LeftPoints[0].x+LeftPoints[0].width) )
				{
					MergeX[0] = LeftPoints[0].x + LeftPoints[0].width - globalPara->LastMyPoint1[0].width;
				}
				else if(MergeX[0]<LeftPoints[0].x-LeftPoints[0].width)
				{
					MergeX[0] = LeftPoints[0].x - LeftPoints[0].width + globalPara->LastMyPoint1[0].width;
				}
				MergeX[1] = globalPara->LastMyPoint1[1].x + globalPara->MergeFlag * globalPara->LastMyPoint1[1].Velocity;
				if ( (MergeX[1]>LeftPoints[0].x+LeftPoints[0].width) )
				{
					MergeX[1] = LeftPoints[0].x + LeftPoints[0].width - globalPara->LastMyPoint1[1].width;
				}
				else if(MergeX[1]<LeftPoints[0].x-LeftPoints[0].width)
				{
					MergeX[1] = LeftPoints[0].x - LeftPoints[0].width + globalPara->LastMyPoint1[1].width;
				}
		#if(1==DEBUG_PM)
				//---------------------------------------------------------------------------------------------------------------
				fprintf(g_pf,"2toMerge:LastLeftPoints[0].x=%d,LastLeftPoints[0].Velocity=%d,LastLeftPoints[1].x=%d,LastLeftPoints[1].Velocity=%d,LeftPoints[0].x=%d,LeftPoints[0].width=%d\n",globalPara->LastMyPoint1[0].x,globalPara->LastMyPoint1[0].Velocity,globalPara->LastMyPoint1[1].x,globalPara->LastMyPoint1[1].Velocity,LeftPoints[0].x,LeftPoints[0].width);
				//-------------------------------------------------------------------------------------------------------------------
		#endif
			}
			//else if ((1==globalPara->LastPointNum1) && (1==globalPara->LastPointNum2))
			//{
			//	MergeX[0] = globalPara->LastMyPoint1[0].x + globalPara->LastMyPoint1[0].Velocity;
			//	if ( (MergeX[0]>LeftPoints[0].x+LeftPoints[0].width) )
			//	{
			//		MergeX[0] = LeftPoints[0].x + LeftPoints[0].width - globalPara->LastMyPoint1[0].width;
			//	}
			//	else if(MergeX[0]<LeftPoints[0].x-LeftPoints[0].width)
			//	{
			//		MergeX[0] = LeftPoints[0].x - LeftPoints[0].width + globalPara->LastMyPoint1[0].width;
			//	}
			//	MergeX[1] = LeftPoints[0].x;
			//}
			else
			{
				MergeX[0] = MergeX[1] = LeftPoints[0].x;
				//globalPara->bUpdateImagePoints = 1;					//only update image points on the no merge side;
			}
			if(2==globalPara->MatchStyle)
			{
				GetMyXY(globalPara->gClbPara,MergeX[0],RightPoints[1].x,&MyPoint[0].x,&MyPoint[0].y);
				GetMyXY(globalPara->gClbPara,MergeX[1],RightPoints[0].x,&MyPoint[1].x,&MyPoint[1].y);
			}
			else
			{
				GetMyXY(globalPara->gClbPara,MergeX[0],RightPoints[0].x,&MyPoint[0].x,&MyPoint[0].y);
				GetMyXY(globalPara->gClbPara,MergeX[1],RightPoints[1].x,&MyPoint[1].x,&MyPoint[1].y);
			}

			//set points ID
			if (2==globalPara->LastPointNum || 1==globalPara->LastPointNum)
			{
				SetPointsID(globalPara,MyPoint,2);
			}
			else
			{
				MyPoint[0].ID = globalPara->ID;
				globalPara->ID++;
				MyPoint[1].ID = globalPara->ID;
				globalPara->ID++;
		#if(1==DEBUG_PM)
				//---------------------------------------------------------------------------------------------------------------
				fprintf(g_pf,"0toMerge:ID=(%d,%d)\n",MyPoint[0].ID,MyPoint[1].ID);
				//-------------------------------------------------------------------------------------------------------------------
		#endif
			}
			*PanelPointNum = 2;
			globalPara->bUpdatePanelPoints = 1;
			globalPara->MergeFlag++;
		}
		else		//first time
		{
			if(2==globalPara->LastPointNum1 && 2==globalPara->LastPointNum2)
			{
				globalPara->MergeFlag = 2;		//wait one frame
				globalPara->bWaitFlag = 0;
				globalPara->WaitCount = 0;
			}
			else
			{
				globalPara->MergeFlag = 1;
				globalPara->bWaitFlag = 1;
				globalPara->WaitCount = 1;
				globalPara->bUpdateImagePoints = 1;
			}
			*PanelPointNum = globalPara->LastPointNum;
			MyPoint[0] = globalPara->LastMyPoint[0];
			MyPoint[1] = globalPara->LastMyPoint[1];
		}

	}
	else if(2==LeftNum && 1==RightNum)
	{
#if(1==DEBUG_PM)
		//---------------------------------------------------------------------------------------------------------------
		fprintf(g_pf,"21\n");
		//-------------------------------------------------------------------------------------------------------------------
#endif

		//depend on last frame
		if (1==globalPara->MergeFlag)
		{
			if(1==globalPara->bWaitFlag && globalPara->WaitCount<globalPara->gControlPara.WaitFrame)
			{
				globalPara->WaitCount++;
				globalPara->bUpdateImagePoints = 1;
			}
			else
			{
				globalPara->bWaitFlag = 0;
				globalPara->WaitCount = 0;
				globalPara->MergeFlag = 2;
				globalPara->bUpdateImagePoints = 1;
			}
		}
		else if (globalPara->MergeFlag>=2)			//still merge
		{
	#if(1==DEBUG_PM)
			//---------------------------------------------------------------------------------------------------------------
			fprintf(g_pf,"2toMerge:MergeFlag=%d,globalPara->LastPointNum1=%d,globalPara->LastPointNum2=%d\n",globalPara->MergeFlag,globalPara->LastPointNum1,globalPara->LastPointNum2);
			//-------------------------------------------------------------------------------------------------------------------
	#endif
			if ((2==globalPara->LastPointNum1) && (2==globalPara->LastPointNum2))
			{
				MergeX[0] = globalPara->LastMyPoint2[0].x + globalPara->MergeFlag * globalPara->LastMyPoint2[0].Velocity;
				if ( (MergeX[0]>RightPoints[0].x+RightPoints[0].width) )
				{
					MergeX[0] = RightPoints[0].x + RightPoints[0].width - globalPara->LastMyPoint2[0].width;
				}
				else if(MergeX[0]<RightPoints[0].x-RightPoints[0].width)
				{
					MergeX[0] = RightPoints[0].x - RightPoints[0].width + globalPara->LastMyPoint2[0].width;
				}
				MergeX[1] = globalPara->LastMyPoint2[1].x + globalPara->MergeFlag * globalPara->LastMyPoint2[1].Velocity;
				if ( (MergeX[1]>RightPoints[0].x+RightPoints[0].width) )
				{
					MergeX[1] = RightPoints[0].x + RightPoints[0].width - globalPara->LastMyPoint2[1].width;
				}
				else if(MergeX[1]<RightPoints[0].x-RightPoints[0].width)
				{
					MergeX[1] = RightPoints[0].x - RightPoints[0].width + globalPara->LastMyPoint2[1].width;
				}
		#if(1==DEBUG_PM)
				//---------------------------------------------------------------------------------------------------------------
				fprintf(g_pf,"2toMerge:LastRightPoints[0].x=%d,LastRightPoints[0].Velocity=%d,LastRightPoints[1].x=%d,LastRightPoints[1].Velocity=%d,RightPoints[0].x=%d,RightPoints[0].width=%d\n",globalPara->LastMyPoint2[0].x,globalPara->LastMyPoint2[0].Velocity,globalPara->LastMyPoint2[1].x,globalPara->LastMyPoint2[1].Velocity,RightPoints[0].x,RightPoints[0].width);
				//-------------------------------------------------------------------------------------------------------------------
		#endif
			}
			//else if ((1==globalPara->LastPointNum1) && (1==globalPara->LastPointNum2))
			//{
			//	MergeX[0] = globalPara->LastMyPoint2[0].x + globalPara->LastMyPoint2[0].Velocity;
			//	if ( (MergeX[0]>RightPoints[0].x+RightPoints[0].width) )
			//	{
			//		MergeX[0] = RightPoints[0].x + RightPoints[0].width - globalPara->LastMyPoint2[0].width;
			//	}
			//	else if(MergeX[0]<RightPoints[0].x-RightPoints[0].width)
			//	{
			//		MergeX[0] = RightPoints[0].x - RightPoints[0].width + globalPara->LastMyPoint2[0].width;
			//	}
			//	MergeX[1] = RightPoints[0].x;
			//}
			else
			{
				MergeX[0] = MergeX[1] = RightPoints[0].x;
			}
			if(2==globalPara->MatchStyle)
			{
				GetMyXY(globalPara->gClbPara,LeftPoints[1].x,MergeX[0],&MyPoint[0].x,&MyPoint[0].y);
				GetMyXY(globalPara->gClbPara,LeftPoints[0].x,MergeX[1],&MyPoint[1].x,&MyPoint[1].y);
			}
			else
			{
				GetMyXY(globalPara->gClbPara,LeftPoints[0].x,MergeX[0],&MyPoint[0].x,&MyPoint[0].y);
				GetMyXY(globalPara->gClbPara,LeftPoints[1].x,MergeX[1],&MyPoint[1].x,&MyPoint[1].y);
			}

			//set points ID
			if (2==globalPara->LastPointNum || 1==globalPara->LastPointNum)
			{
				SetPointsID(globalPara,MyPoint,2);
			}
			else
			{
				MyPoint[0].ID = globalPara->ID;
				globalPara->ID++;
				MyPoint[1].ID = globalPara->ID;
				globalPara->ID++;
		#if(1==DEBUG_PM)
				//---------------------------------------------------------------------------------------------------------------
				fprintf(g_pf,"0toMerge:ID=(%d,%d)\n",MyPoint[0].ID,MyPoint[1].ID);
				//-------------------------------------------------------------------------------------------------------------------
		#endif
			}
			*PanelPointNum = 2;
			globalPara->bUpdatePanelPoints = 1;
			globalPara->MergeFlag++;
		}
		else		//first time
		{
			if(2==globalPara->LastPointNum1 && 2==globalPara->LastPointNum2)
			{
				globalPara->MergeFlag = 2;		//wait one frame
				globalPara->bWaitFlag = 0;
				globalPara->WaitCount = 0;
			}
			else
			{
				globalPara->MergeFlag = 1;
				globalPara->bWaitFlag = 1;
				globalPara->WaitCount = 1;
				globalPara->bUpdateImagePoints = 1;
			}
			*PanelPointNum = globalPara->LastPointNum;
			MyPoint[0] = globalPara->LastMyPoint[0];
			MyPoint[1] = globalPara->LastMyPoint[1];
		}
	}
	else if(2==LeftNum && 2==RightNum)
	{
#if(1==DEBUG_PM)
		//-------------------------------------------------------------------------------------------------------------------
		fprintf(g_pf,"22\n");
		//-------------------------------------------------------------------------------------------------------------------
#endif
		//2 points
		//if (globalPara->bSimul00to22)
		//{
		//	GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[0].x,&globalPara->LastMyPoint[0].VectX[2],&globalPara->LastMyPoint[0].VectY[2]);
		//	GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[1].x,&globalPara->LastMyPoint[1].VectX[2],&globalPara->LastMyPoint[1].VectY[2]);
		//	GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[1].x,&globalPara->LastMyPoint[0].VectX[3],&globalPara->LastMyPoint[0].VectY[3]);
		//	GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[0].x,&globalPara->LastMyPoint[1].VectX[3],&globalPara->LastMyPoint[1].VectY[3]);
		//	if(abs(globalPara->LastMyPoint[0].VectX[2] - globalPara->LastMyPoint[0].VectX[0]) + abs(globalPara->LastMyPoint[0].VectX[2]-globalPara->LastMyPoint[0].VectX[0]) 
		//		+ abs(globalPara->LastMyPoint[1].VectX[2] - globalPara->LastMyPoint[1].VectX[0]) + abs(globalPara->LastMyPoint[1].VectY[2] - globalPara->LastMyPoint[1].VectY[0]) >= 40)
		//	{
		//		if ()
		//	}
		//}
		//else 
		if(2==globalPara->LastPointNum)
		{
			NowIndex = 0;
			LastIndex = 0;
			LeftIndex = 0;
			RightIndex = 0;
			MinDis = 10000;
			if (globalPara->MergeFlag>=2)			//merged in last frame
			{
				if(1==globalPara->LastPointNum1 && 2==globalPara->LastPointNum2)
				{
					if(abs(RightPoints[0].x - globalPara->LastMyPoint2[0].x) < 3)
					{
						if (abs(LeftPoints[0].x - globalPara->LastMyPoint1[0].x) <= abs(LeftPoints[1].x-globalPara->LastMyPoint1[0].x))
						{
							GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[0].x,&MyPoint[0].x,&MyPoint[0].y);
							GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[1].x,&MyPoint[1].x,&MyPoint[1].y);
							SetPointsIDOnDetachWithStillPoint(globalPara,MyPoint);
							*PanelPointNum = 2;
							globalPara->MatchStyle = 1;
							globalPara->MergeFlag = 0;
							MatchFinish = 1;
						}
						else
						{
							GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[0].x,&MyPoint[0].x,&MyPoint[0].y);
							GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[1].x,&MyPoint[1].x,&MyPoint[1].y);
							SetPointsIDOnDetachWithStillPoint(globalPara,MyPoint);
							*PanelPointNum = 2;
							globalPara->MatchStyle = 2;
							globalPara->MergeFlag = 0;
							MatchFinish = 1;
						}
					}
					if(abs(RightPoints[1].x - globalPara->LastMyPoint2[1].x) < 3)
					{
						if (abs(RightPoints[1].x - globalPara->LastMyPoint2[1].x) < abs(RightPoints[0].x - globalPara->LastMyPoint2[0].x))
						{
						if (abs(LeftPoints[0].x - globalPara->LastMyPoint1[0].x) <= abs(LeftPoints[1].x-globalPara->LastMyPoint1[0].x))
						{
							GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[1].x,&MyPoint[0].x,&MyPoint[0].y);
							GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[0].x,&MyPoint[1].x,&MyPoint[1].y);
							SetPointsIDOnDetachWithStillPoint(globalPara,MyPoint);
							*PanelPointNum = 2;
							globalPara->MatchStyle = 2;
							globalPara->MergeFlag = 0;
							MatchFinish = 1;
						}
						else
						{
							GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[1].x,&MyPoint[0].x,&MyPoint[0].y);
							GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[0].x,&MyPoint[1].x,&MyPoint[1].y);
							SetPointsIDOnDetachWithStillPoint(globalPara,MyPoint);
							*PanelPointNum = 2;
							globalPara->MatchStyle = 1;
							globalPara->MergeFlag = 0;
							MatchFinish = 1;
						}
					}
				}
				}
				else if(2==globalPara->LastPointNum1 && 1==globalPara->LastPointNum2)
				{
					if(abs(LeftPoints[0].x - globalPara->LastMyPoint1[0].x) < 3)
					{
						if (abs(RightPoints[0].x - globalPara->LastMyPoint2[0].x) <= abs(RightPoints[1].x-globalPara->LastMyPoint2[0].x))
						{
							GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[0].x,&MyPoint[0].x,&MyPoint[0].y);
							GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[1].x,&MyPoint[1].x,&MyPoint[1].y);
							SetPointsIDOnDetachWithStillPoint(globalPara,MyPoint);
							*PanelPointNum = 2;
							globalPara->MatchStyle = 1;
							globalPara->MergeFlag = 0;
							MatchFinish = 1;
						}
						else
						{
							GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[1].x,&MyPoint[0].x,&MyPoint[0].y);
							GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[0].x,&MyPoint[1].x,&MyPoint[1].y);
							SetPointsIDOnDetachWithStillPoint(globalPara,MyPoint);
							*PanelPointNum = 2;
							globalPara->MatchStyle = 2;
							globalPara->MergeFlag = 0;
							MatchFinish = 1;
						}
					}
					if(abs(LeftPoints[1].x - globalPara->LastMyPoint1[1].x) < 3)
					{
						if (abs(LeftPoints[1].x - globalPara->LastMyPoint1[1].x) < abs(LeftPoints[0].x - globalPara->LastMyPoint1[0].x))
					{
						if (abs(RightPoints[0].x - globalPara->LastMyPoint2[0].x) <= abs(RightPoints[1].x-globalPara->LastMyPoint2[0].x))
						{
							GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[0].x,&MyPoint[0].x,&MyPoint[0].y);
							GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[1].x,&MyPoint[1].x,&MyPoint[1].y);
							SetPointsIDOnDetachWithStillPoint(globalPara,MyPoint);
							*PanelPointNum = 2;
							globalPara->MatchStyle = 2;
							globalPara->MergeFlag = 0;
							MatchFinish = 1;
						}
						else
						{
							GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[1].x,&MyPoint[0].x,&MyPoint[0].y);
							GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[0].x,&MyPoint[1].x,&MyPoint[1].y);
							SetPointsIDOnDetachWithStillPoint(globalPara,MyPoint);
							*PanelPointNum = 2;
							globalPara->MatchStyle = 1;
							globalPara->MergeFlag = 0;
							MatchFinish = 1;
						}
					}
				}
				}
				else if(2==globalPara->LastPointNum1 && 2==globalPara->LastPointNum2)
				{
					if ( (abs(LeftPoints[0].x - globalPara->LastMyPoint1[0].x)<3 && abs(LeftPoints[1].x - globalPara->LastMyPoint[1].x)<3)
						&& (abs(RightPoints[0].x - globalPara->LastMyPoint2[0].x)<3 && abs(RightPoints[1].x - globalPara->LastMyPoint2[1].x)<3) )
					{
						if(2==globalPara->MatchStyle)
						{
							GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[1].x,&MyPoint[0].x,&MyPoint[0].y);
							GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[0].x,&MyPoint[1].x,&MyPoint[1].y);
						}
						else
						{
							GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[0].x,&MyPoint[0].x,&MyPoint[0].y);
							GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[1].x,&MyPoint[1].x,&MyPoint[1].y);
						}
						SetPointsIDOnDetachWithStillPoint(globalPara,MyPoint);
						*PanelPointNum = 2;
						MatchFinish = 1;
				#if(1==DEBUG_PM)
						//---------------------------------------------------------------------------------------------------------------
						fprintf(g_pf,"22MergeWithStillPoint:MatchStyle=%d\n",globalPara->MatchStyle);
						//-------------------------------------------------------------------------------------------------------------------
				#endif
					}
					else
					{
						MinDis = 10000;
					for(i=0;i<2;i++)
					{
						for(j=0;j<2;j++)
						{
								if((abs(LeftPoints[i].x - globalPara->LastMyPoint1[j].x)<3) && (abs(LeftPoints[i].x - globalPara->LastMyPoint1[j].x)<MinDis))
								{
									LeftIndex = i;
									MatchFinish = 1;
									MinDis = abs(LeftPoints[i].x - globalPara->LastMyPoint1[j].x);
								}
							}
					}
					if(1==MatchFinish)
					{
						MatchFinish = 0;
							MinDis = 10000;
						for(i=0;i<2;i++)
						{
							for(j=0;j<2;j++)
							{
									if((abs(RightPoints[i].x - globalPara->LastMyPoint2[j].x)<3) && (abs(RightPoints[i].x - globalPara->LastMyPoint2[j].x)<MinDis))
									{
										RightIndex = i;
										MatchFinish = 1;
										MinDis = abs(RightPoints[i].x - globalPara->LastMyPoint2[j].x);
									}
								}
						}
					}
					if(1==MatchFinish)
					{
						GetMyXY(globalPara->gClbPara,LeftPoints[LeftIndex].x,RightPoints[RightIndex].x,&MyPoint[0].x,&MyPoint[0].y);
						GetMyXY(globalPara->gClbPara,LeftPoints[1-LeftIndex].x,RightPoints[1-RightIndex].x,&MyPoint[1].x,&MyPoint[1].y);
						SetPointsIDOnDetachWithStillPoint(globalPara,MyPoint);
						*PanelPointNum = 2;
						if(LeftIndex == RightIndex)
							globalPara->MatchStyle = 1;
						else
							globalPara->MatchStyle = 2;
						globalPara->MergeFlag = 0;
					}
					#if(1==DEBUG_PM)
						//---------------------------------------------------------------------------------------------------------------
						fprintf(g_pf,"Check22MergeWithStillPoint:LastLeft(%d,%d),LastRight(%d,%d),NowLeft(%d,%d),NowRight(%d,%d),MatchFinish=%d,MatchStyle=%d\n",globalPara->LastMyPoint1[0].x,globalPara->LastMyPoint1[1].x,globalPara->LastMyPoint2[0].x,globalPara->LastMyPoint2[1].x,LeftPoints[0].x,LeftPoints[1].x,RightPoints[0].x,RightPoints[1].x,MatchFinish,globalPara->MatchStyle);
						//-------------------------------------------------------------------------------------------------------------------
					#endif
					}
				}
				if (1 != MatchFinish)
				{
					for(i=0;i<2;i++)
					{
						for(j=0;j<2;j++)
						{
							GetMyXY(globalPara->gClbPara,LeftPoints[i].x,RightPoints[j].x,&MyPoint[0].x,&MyPoint[0].y);
							GetMyXY(globalPara->gClbPara,LeftPoints[1-i].x,RightPoints[1-j].x,&MyPoint[1].x,&MyPoint[1].y);
							if ( (globalPara->LastMyPoint[0].VectNum>=2) && (globalPara->LastMyPoint[1].VectNum>=2) )
							{
								VelX = globalPara->LastMyPoint[0].VectX[globalPara->LastMyPoint[0].VectNum-1] - globalPara->LastMyPoint[0].VectX[globalPara->LastMyPoint[0].VectNum-2];
								VelY = globalPara->LastMyPoint[0].VectY[globalPara->LastMyPoint[0].VectNum-1] - globalPara->LastMyPoint[0].VectY[globalPara->LastMyPoint[0].VectNum-2];
								VelMaxAbs = (abs(VelX) >= abs(VelY)) ? abs(VelX) : abs(VelY);
								DeltX = abs(MyPoint[0].x - globalPara->LastMyPoint[0].VectX[globalPara->LastMyPoint[0].VectNum-1] - VelX);
								DeltY = abs(MyPoint[0].y - globalPara->LastMyPoint[0].VectY[globalPara->LastMyPoint[0].VectNum-1] - VelY);
								if(DeltX > VelMaxAbs)
									DeltX = 4*(DeltX);
								if(DeltY > VelMaxAbs)
									DeltY = 4*(DeltY);
								dis = DeltX + DeltY;
								if (0==dis)
								{
									MinDis = dis;
									LeftIndex = i;
									RightIndex = j;
									MatchFinish = 1;
									break;
								}
			#if(1==DEBUG_PM)
					//---------------------------------------------------------------------------------------------------------------
					fprintf(g_pf,"i=%d,j=%d,L0(%d,%d),N0(%d,%d),VelX=%d,VelY=%d,DeltX=%d,DeltY=%d,dis=%d\n",i,j,globalPara->LastMyPoint[0].VectX[globalPara->LastMyPoint[0].VectNum-1],globalPara->LastMyPoint[0].VectY[globalPara->LastMyPoint[0].VectNum-1],MyPoint[0].x,MyPoint[0].y,VelX,VelY,DeltX,DeltY,dis);
					//-------------------------------------------------------------------------------------------------------------------
			#endif
								VelX = globalPara->LastMyPoint[1].VectX[globalPara->LastMyPoint[1].VectNum-1] - globalPara->LastMyPoint[1].VectX[globalPara->LastMyPoint[1].VectNum-2];
								VelY = globalPara->LastMyPoint[1].VectY[globalPara->LastMyPoint[1].VectNum-1] - globalPara->LastMyPoint[1].VectY[globalPara->LastMyPoint[1].VectNum-2];
								VelMaxAbs = (abs(VelX) >= abs(VelY)) ? abs(VelX) : abs(VelY);
								DeltX = abs(MyPoint[1].x - globalPara->LastMyPoint[1].VectX[globalPara->LastMyPoint[1].VectNum-1] - VelX);
								DeltY = abs(MyPoint[1].y - globalPara->LastMyPoint[1].VectY[globalPara->LastMyPoint[1].VectNum-1] - VelY);
								if(DeltX > VelMaxAbs)
									DeltX = 4*(DeltX);
								if(DeltY > VelMaxAbs)
									DeltY = 4*(DeltY);
								if (DeltX+DeltY == 0)
								{
									MinDis = DeltX + DeltY;
									LeftIndex = i;
									RightIndex = j;
									MatchFinish = 1;
									break;
								}
								dis += DeltX + DeltY;
								if (dis<MinDis)
								{
									MinDis = dis;
									LeftIndex = i;
									RightIndex = j;
									//LastIndex = 1;
								}
			#if(1==DEBUG_PM)
					//---------------------------------------------------------------------------------------------------------------
					fprintf(g_pf,"i=%d,j=%d,L1(%d,%d),N1(%d,%d),VelX=%d,VelY=%d,DeltX=%d,DeltY=%d,dis=%d\n",i,j,globalPara->LastMyPoint[1].VectX[globalPara->LastMyPoint[1].VectNum-1],globalPara->LastMyPoint[1].VectY[globalPara->LastMyPoint[1].VectNum-1],MyPoint[1].x,MyPoint[1].y,VelX,VelY,DeltX,DeltY,dis);
					//-------------------------------------------------------------------------------------------------------------------
			#endif
							}
							else
							{
								dis = abs(MyPoint[0].x - globalPara->LastMyPoint[0].x) + abs(MyPoint[0].y - globalPara->LastMyPoint[0].y);
								//if(dis < MinDis)
								//{
								//	MinDis = dis;
								//	LeftIndex = i;
								//	RightIndex = j;
								//	LastIndex = 0;
								//}
								dis += abs(MyPoint[1].x - globalPara->LastMyPoint[1].x) + abs(MyPoint[1].y - globalPara->LastMyPoint[1].y);
								if(dis < MinDis)
								{
									MinDis = dis;
									LeftIndex = i;
									RightIndex = j;
									//LastIndex = 1;
								}
							}
						}
						if (1==MatchFinish)
							break;
					}
					GetMyXY(globalPara->gClbPara,LeftPoints[LeftIndex].x,RightPoints[RightIndex].x,&MyPoint[0].x,&MyPoint[0].y);
					GetMyXY(globalPara->gClbPara,LeftPoints[1-LeftIndex].x,RightPoints[1-RightIndex].x,&MyPoint[1].x,&MyPoint[1].y);
					MyPoint[0].ID = globalPara->LastMyPoint[0].ID;
					MyPoint[1].ID = globalPara->LastMyPoint[1].ID;
					*PanelPointNum = 2;
					if(LeftIndex==RightIndex)
					{
						globalPara->MatchStyle = 1;
					}
					else
					{
						globalPara->MatchStyle = 2;
					}
			#if(1==DEBUG_PM)
					//---------------------------------------------------------------------------------------------------------------
					fprintf(g_pf,"Merge:MatchFinish=%d,MinDis=%d,LeftIndex=%d,RightIndex=%d,LastIndex=%d,ID=%d vs %d\n",MatchFinish,MinDis,LeftIndex,RightIndex,LastIndex,MyPoint[LastIndex].ID,MyPoint[1-LastIndex].ID);
					//-------------------------------------------------------------------------------------------------------------------
			#endif
				}
			}
			else			//two points and no merge
			{
				if (1==globalPara->MatchStyle)
				{
					GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[0].x,&MyPoint[0].x,&MyPoint[0].y);
					GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[1].x,&MyPoint[1].x,&MyPoint[1].y);
		#if(1==DEBUG_PM)
				//---------------------------------------------------------------------------------------------------------------
				fprintf(g_pf,"NoMerge:X1X2(%d,%d),X1X2(%d,%d)\n",LeftPoints[0].x,RightPoints[0].x,LeftPoints[1].x,RightPoints[1].x);
				//-------------------------------------------------------------------------------------------------------------------
		#endif
				}
				else if (2==globalPara->MatchStyle)
				{
					GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[1].x,&MyPoint[0].x,&MyPoint[0].y);
					GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[0].x,&MyPoint[1].x,&MyPoint[1].y);
		#if(1==DEBUG_PM)
				//---------------------------------------------------------------------------------------------------------------
				fprintf(g_pf,"NoMerge:X1X2(%d,%d),X1X2(%d,%d)\n",LeftPoints[0].x,RightPoints[1].x,LeftPoints[1].x,RightPoints[0].x);
				//-------------------------------------------------------------------------------------------------------------------
		#endif
				}

				SetPointsID(globalPara,MyPoint,2);
				*PanelPointNum = 2;
		#if(1==DEBUG_PM)
				//---------------------------------------------------------------------------------------------------------------
				fprintf(g_pf,"NoMerge:MinDis=%d,LastIndex=%d,NowIndex=%d,MatchStyle=%d,ID=(%d,%d),(%d,%d),(%d,%d)\n",MinDis,LastIndex,NowIndex,globalPara->MatchStyle,MyPoint[LastIndex].ID,MyPoint[1-LastIndex].ID,MyPoint[LastIndex].x,MyPoint[LastIndex].y,MyPoint[1-LastIndex].x,MyPoint[1-LastIndex].y);
				//-------------------------------------------------------------------------------------------------------------------
		#endif
			}
		}
		else if(1==globalPara->LastPointNum)		//1 point in last frame
		{
			NowIndex = 0;
			LastIndex = 0;
			LeftIndex = 0;
			RightIndex = 0;
			MinDis = 10000;
			if ( (globalPara->LastMyPoint[0].VectNum>=2) && (globalPara->LastMyPoint[1].VectNum>=2) )
			{
				for(i=0;i<2;i++)
				{
					for(j=0;j<2;j++)
					{
						GetMyXY(globalPara->gClbPara,LeftPoints[i].x,RightPoints[j].x,&MyPoint[0].x,&MyPoint[0].y);
						VelX = globalPara->LastMyPoint[0].VectX[globalPara->LastMyPoint[0].VectNum-1] - globalPara->LastMyPoint[0].VectX[globalPara->LastMyPoint[0].VectNum-2];
						VelY = globalPara->LastMyPoint[0].VectY[globalPara->LastMyPoint[0].VectNum-1] - globalPara->LastMyPoint[0].VectY[globalPara->LastMyPoint[0].VectNum-2];
						VelMaxAbs = (abs(VelX) >= abs(VelY)) ? abs(VelX) : abs(VelY);
						DeltX = abs(MyPoint[0].x - globalPara->LastMyPoint[0].VectX[globalPara->LastMyPoint[0].VectNum-1] - VelX);
						DeltY = abs(MyPoint[0].y - globalPara->LastMyPoint[0].VectY[globalPara->LastMyPoint[0].VectNum-1] - VelY);
						if(DeltX > VelMaxAbs)
							DeltX += (DeltX - VelMaxAbs);
						if(DeltY > VelMaxAbs)
							DeltY += (DeltY - VelMaxAbs);
						dis = DeltX + DeltY;
						if (dis<MinDis)
						{
							MinDis = dis;
							LeftIndex = i;
							RightIndex = j;
						}
					}
				}
				
			}
			else
			{
				for(i=0;i<2;i++)
				{
					for(j=0;j<2;j++)
					{
						GetMyXY(globalPara->gClbPara,LeftPoints[i].x,RightPoints[j].x,&MyPoint[0].x,&MyPoint[0].y);
						dis = abs(MyPoint[0].x - globalPara->LastMyPoint[0].x) + abs(MyPoint[0].y - globalPara->LastMyPoint[0].y);
						if(dis < MinDis)
						{
							MinDis = dis;
							LeftIndex = i;
							RightIndex = j;
						}
					}
				}
			}
			GetMyXY(globalPara->gClbPara,LeftPoints[LeftIndex].x,RightPoints[RightIndex].x,&MyPoint[0].x,&MyPoint[0].y);
			GetMyXY(globalPara->gClbPara,LeftPoints[1-LeftIndex].x,RightPoints[1-RightIndex].x,&MyPoint[1].x,&MyPoint[1].y);
			MyPoint[0].ID = globalPara->LastMyPoint[0].ID;
			MyPoint[1].ID = globalPara->ID;
			globalPara->ID++;
			if (LeftIndex==RightIndex)
			{
				globalPara->MatchStyle = 1;
			}
			else
			{
				globalPara->MatchStyle = 2;
			}
			*PanelPointNum = 2;
		#if(1==DEBUG_PM)
			//---------------------------------------------------------------------------------------------------------------
			fprintf(g_pf,"1to2:MinDis=%d,LeftIndex=%d,RightIndex=%d,(%d,%d),(%d,%d),ID=(%d,%d),(%d,%d),(%d,%d)\n",MinDis,LeftIndex,RightIndex,LeftPoints[LeftIndex].x,RightPoints[RightIndex].x,LeftPoints[1-LeftIndex].x,RightPoints[1-RightIndex].x,MyPoint[0].ID,MyPoint[1].ID,MyPoint[0].x,MyPoint[0].y,MyPoint[1].x,MyPoint[1].y);
			//-------------------------------------------------------------------------------------------------------------------
		#endif	
		}
		else	//two points appear together while no points show in last frame,how to match?
		{
			//----------------------------------------------------------------------------------------------------
			//the algorithm may be updated later
			//if (0==globalPara->bSimul00to22)
			{
				GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[0].x,&globalPara->LastMyPoint[0].VectX[0],&globalPara->LastMyPoint[0].VectY[0]);
				GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[1].x,&globalPara->LastMyPoint[1].VectX[0],&globalPara->LastMyPoint[1].VectY[0]);
				GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[1].x,&globalPara->LastMyPoint[0].VectX[1],&globalPara->LastMyPoint[0].VectY[1]);
				GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[0].x,&globalPara->LastMyPoint[1].VectX[1],&globalPara->LastMyPoint[1].VectY[1]);
				if (globalPara->LastMyPoint[0].VectX[0]<0 || globalPara->LastMyPoint[0].VectX[0]>(globalPara->gScreenPara.ScreenWidth-1) || globalPara->LastMyPoint[0].VectY[0]<0 || globalPara->LastMyPoint[0].VectY[0]>(globalPara->gScreenPara.ScreenHight-1))
 				{
 					MyPoint[0].x = globalPara->LastMyPoint[0].VectX[1];
					MyPoint[0].y = globalPara->LastMyPoint[0].VectY[1];
					MyPoint[1].x = globalPara->LastMyPoint[1].VectX[1];
					MyPoint[1].y = globalPara->LastMyPoint[1].VectY[1];
					MyPoint[0].ID = globalPara->ID;
					globalPara->ID++;
					MyPoint[1].ID = globalPara->ID;
					globalPara->ID++;
					globalPara->MatchStyle = 2;
					*PanelPointNum = 2;
 				}
				else if(globalPara->LastMyPoint[1].VectX[0]<0 || globalPara->LastMyPoint[1].VectX[0]>(globalPara->gScreenPara.ScreenWidth-1) || globalPara->LastMyPoint[1].VectY[0]<0 || globalPara->LastMyPoint[1].VectY[0]>(globalPara->gScreenPara.ScreenHight-1))
				{
					MyPoint[0].x = globalPara->LastMyPoint[0].VectX[1];
					MyPoint[0].y = globalPara->LastMyPoint[0].VectY[1];
					MyPoint[1].x = globalPara->LastMyPoint[1].VectX[1];
					MyPoint[1].y = globalPara->LastMyPoint[1].VectY[1];
					MyPoint[0].ID = globalPara->ID;
					globalPara->ID++;
					MyPoint[1].ID = globalPara->ID;
					globalPara->ID++;
					globalPara->MatchStyle = 2;
					*PanelPointNum = 2;
				}
				else if(globalPara->LastMyPoint[0].VectX[1]<0 || globalPara->LastMyPoint[0].VectX[1]>(globalPara->gScreenPara.ScreenWidth-1) || globalPara->LastMyPoint[0].VectY[1]<0 || globalPara->LastMyPoint[0].VectY[1]>(globalPara->gScreenPara.ScreenHight-1))
				{
					MyPoint[0].x = globalPara->LastMyPoint[0].VectX[0];
					MyPoint[0].y = globalPara->LastMyPoint[0].VectY[0];
					MyPoint[1].x = globalPara->LastMyPoint[1].VectX[0];
					MyPoint[1].y = globalPara->LastMyPoint[1].VectY[0];
					MyPoint[0].ID = globalPara->ID;
					globalPara->ID++;
					MyPoint[1].ID = globalPara->ID;
					globalPara->ID++;
					globalPara->MatchStyle = 1;
					*PanelPointNum = 2;
				}
				else if(globalPara->LastMyPoint[1].VectX[1]<0 || globalPara->LastMyPoint[1].VectX[1]>(globalPara->gScreenPara.ScreenWidth-1) || globalPara->LastMyPoint[1].VectY[1]<0 || globalPara->LastMyPoint[1].VectY[1]>(globalPara->gScreenPara.ScreenHight-1))
				{
					MyPoint[0].x = globalPara->LastMyPoint[0].VectX[0];
					MyPoint[0].y = globalPara->LastMyPoint[0].VectY[0];
					MyPoint[1].x = globalPara->LastMyPoint[1].VectX[0];
					MyPoint[1].y = globalPara->LastMyPoint[1].VectY[0];
					MyPoint[0].ID = globalPara->ID;
					globalPara->ID++;
					MyPoint[1].ID = globalPara->ID;
					globalPara->ID++;
					globalPara->MatchStyle = 1;
					*PanelPointNum = 2;
				}
				else
				{
					//globalPara->bSimul00to22 = 1;
					GetMyXY(globalPara->gClbPara,LeftPoints[0].x,RightPoints[0].x,&MyPoint[0].x,&MyPoint[0].y);
					GetMyXY(globalPara->gClbPara,LeftPoints[1].x,RightPoints[1].x,&MyPoint[1].x,&MyPoint[1].y);
					MyPoint[0].ID = globalPara->ID;
					globalPara->ID++;
					MyPoint[1].ID = globalPara->ID;
					globalPara->ID++;
					globalPara->MatchStyle = 1;
					*PanelPointNum = 2;
				}
			}
			//----------------------------------------------------------------------------------------------------------------
			
		#if(1==DEBUG_PM)
			//---------------------------------------------------------------------------------------------------------------
			//fprintf(g_pf,"0to2:MinDis=%d,LeftIndex=%d,RightIndex=%d,ID=(%d,%d)\n",MinDis,LeftIndex,RightIndex,MyPoint[0].ID,MyPoint[1].ID);
			fprintf(g_pf,"0to2\n");
			//-------------------------------------------------------------------------------------------------------------------
		#endif	
		}
		globalPara->bWaitFlag = 0;
		globalPara->WaitCount = 0;
		globalPara->MergeFlag = 0;
		globalPara->bUpdateImagePoints = 1;
		globalPara->bUpdatePanelPoints = 1;
	} */
	else
	{
#if(1==DEBUG_PM)
		//---------------------------------------------------------------------------------------------------------------
		fprintf(g_pf,"leftnum=%d,rightnum=%d,xx=%d\n",LeftNum,RightNum,globalPara->LastPointNum);
		//-------------------------------------------------------------------------------------------------------------------
#endif

		*PanelPointNum = globalPara->LastPointNum;
		if (globalPara->LastPointNum>0)
		{
			for(i=0;i<globalPara->LastPointNum;i++)
			{
				if(globalPara->LastMyPoint[i].VectNum>=2)
				{
					VelX = globalPara->LastMyPoint[0].VectX[globalPara->LastMyPoint[0].VectNum-1] - globalPara->LastMyPoint[0].VectX[globalPara->LastMyPoint[0].VectNum-2];
					VelY = globalPara->LastMyPoint[0].VectY[globalPara->LastMyPoint[0].VectNum-1] - globalPara->LastMyPoint[0].VectY[globalPara->LastMyPoint[0].VectNum-2];
					MyPoint[i].x = globalPara->LastMyPoint[i].x + VelX;
					MyPoint[i].y = globalPara->LastMyPoint[i].y + VelY;
				}
			}
			globalPara->bUpdatePanelPoints = 1;
		}

		globalPara->bWaitFlag = 0;
		globalPara->WaitCount = 0;
	}

	//去除超出屏幕的点
 	if ( 1==(*PanelPointNum) )
 	{
 		if (MyPoint[0].x<0 || MyPoint[0].x>(globalPara->gScreenPara.ScreenWidth-1) || MyPoint[0].y<0 || MyPoint[0].y>(globalPara->gScreenPara.ScreenHight-1))
 		{
 			(*PanelPointNum) = 0;
 		}
 	}
 	else if( 2==(*PanelPointNum) )
 	{
 		if (MyPoint[0].x<0 || MyPoint[0].x>(globalPara->gScreenPara.ScreenWidth-1) || MyPoint[0].y<0 || MyPoint[0].y>(globalPara->gScreenPara.ScreenHight-1))
 		{
 			MyPoint[0] = MyPoint[1];
 			(*PanelPointNum) =1;
 			if (MyPoint[0].x<0 || MyPoint[0].x>(globalPara->gScreenPara.ScreenWidth-1) || MyPoint[0].y<0 || MyPoint[0].y>(globalPara->gScreenPara.ScreenHight-1))
 			{
 				(*PanelPointNum) =0;
 			}
 		}
 		else if (MyPoint[1].x<0 || MyPoint[1].x>(globalPara->gScreenPara.ScreenWidth-1) || MyPoint[1].y<0 || MyPoint[1].y>(globalPara->gScreenPara.ScreenHight-1))
 		{
 			(*PanelPointNum) = 1;
 		}
 	}
	
	if (*PanelPointNum > 0)
		TrackSmooth(globalPara,MyPoint,*PanelPointNum);
}
#endif
