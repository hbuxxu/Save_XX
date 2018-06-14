/***********************************************************************************************************
*�ļ�����Cabrate_Computer.c
*�ļ���ʷ����
*�汾�ţ�VER1.0
*���ڣ�2017.6.23
*���ߣ��۴���
*˵������������㷨������λ��ͬ��C�汾
***********************************************************************************************************/
#include "Cabrate_Computer.h"
#include  <math.h>

//=�궨������ϱ���
uint8_t  CabItemIndex=0;//�궨ͨ����
float    lij[10][20],liy[20],GauAA[20];//���Ӷ�����ⲿ����
_ADCab*  CabBuf[16];//����궨���ݽṹ��
static   void Get_Valid_Point(void);//��ȡ��Ч�궨����
static   uint8_t Compute_Coefficient(int NumbVar,int NbofSam,float Val[][20],float KeyVal[],float Cof[]);//��ϴ������궨��������ѹֵ�����棬����ֵ��ϵ������
static   int Rregress_Anaylsis(int NumbVar,int NbofSam,float x[10][20],float y[20],float cof[],float *s,float *r);//�ع��������
static   int gauss(int NumbVar,float cof[]);//��˹�ⷨ


//=============================��������㷨������===============================================================================
//===����궨ϵ������  
uint8_t  Cabrate_Computer(void)  //calibrate   
{
    float ADV[10][20];//��ѹֵ���ݴ�����
    float Cof[MaxCabPoint];//�궨ϵ������
	  int j,i,k;
	  Get_Valid_Point();//��ԭʼ�궨�����л�ȡ��Ч�궨����
	  
	
    for(j=0;j<CabBuf[CabItemIndex]->CabPointNum;j++)
    {
        ADV[0][j]=CabBuf[CabItemIndex]->Vlt[j];
        for(k=1;k<=CabBuf[CabItemIndex]->NbofVar;k++)
				{
						ADV[k][j]=ADV[k-1][j]*CabBuf[CabItemIndex]->Vlt[j];
				}
    }
    if(Compute_Coefficient(CabBuf[CabItemIndex]->NbofVar,CabBuf[CabItemIndex]->CabPointNum,ADV,CabBuf[CabItemIndex]->Val,Cof))
    {
       for(i=0;i<MaxCabPoint;i++)
       {
           CabBuf[CabItemIndex]->Cof[i]=Cof[i];//��ֵ��Ӧ�궨ϵ��
       }
       return 1;//������ɹ�����1
    }
    else 
			 return 0;
}

//=��ȡ�궨��Ч��
static  void Get_Valid_Point(void)
{
    float  Vlt[MaxCabPoint],Val[MaxCabPoint];
    int    Num=0,ZeroNum=0;
    int    vmin=-10000,vmax=10000;
	  int    i;
    for(i=0;i<MaxCabPoint;i++)
    {
        if(CabBuf[CabItemIndex]->Vlt[i]==0)//���ֻ����һ����Ч
        {
           if(ZeroNum==0)
           {
              ZeroNum++;
              Num++;
           }
           else
						 continue;
        }
        else 
					if(CabBuf[CabItemIndex]->Vlt[i]>=vmin&&CabBuf[CabItemIndex]->Vlt[i]<=vmax)//�ڷ�Χ����Ч
          {
             Num++;
          }
          else
						continue;
        Vlt[Num-1]=CabBuf[CabItemIndex]->Vlt[i];
        Val[Num-1]=CabBuf[CabItemIndex]->Val[i];
    }
    for(i=0;i<Num;i++)
    {
        CabBuf[CabItemIndex]->Vlt[i]=Vlt[i];//��Чֵ����
        CabBuf[CabItemIndex]->Val[i]=Val[i];
    }
		for(i=Num;i<MaxCabPoint;i++)
    {
        CabBuf[CabItemIndex]->Vlt[i]=-20000.0;
        CabBuf[CabItemIndex]->Val[i]=0;
    }
    CabBuf[CabItemIndex]->CabPointNum=Num;//��Ч����
}

//===ϵ�����㺯��
static  uint8_t Compute_Coefficient(int NumbVar,int NbofSam,float Val[][20],float KeyVal[],float Cof[])//��ϴ������궨��������ѹֵ�����棬����ֵ��ϵ������
{
        float x[10][20],y[20],s,r,scalor;
        int i,Var;  /* NumbVar--No of Var; NbofSam--No of Samp */
        for(i=0;i<NumbVar;i++) Cof[i]=0;//��ʼ��
        scalor=0;
        if(NbofSam<NumbVar+1)
        {
          // errProc(hDlg,"?????,????..");
          return 0;
        }
        if(NumbVar<1 || NumbVar>5)//��ϴ�����Ч��Χ
        {
          // errProc(hDlg,"????????...");
          return 0;
        }
        for(i=0;i<NbofSam;i++)
        {
            if(fabs(KeyVal[i])>scalor)
               scalor=fabs(KeyVal[i]);
        }
        if(scalor==0)
        {
           //      errProc(hDlg," ????...   ");
           return 0;
        }
        scalor/=10.0;//???
        for(i=0;i<NbofSam;i++)
        {
            y[i]=KeyVal[i]/scalor;//����������ֵ��
        }
        for(Var=0;Var<NumbVar;Var++)//�����ĵ�ѹֵ��ֵ
        {
            for(i=0;i<NbofSam;i++)
                x[Var][i]=Val[Var][i];
        }
        if(Rregress_Anaylsis(NumbVar,NbofSam,x,y,Cof,&s,&r)==-1)//s,r?    //����Ч����лع����
        {
           //	errProc(hDlg,"????????...");
           return 0;
        }
        for(Var=0;Var<=NumbVar;Var++)
        {
            Cof[Var]*=scalor;
        }
        for(Var=NumbVar+1;Var<7;Var++)
            Cof[Var]=0;
        return 1;
}

//=�ع��������  
static int Rregress_Anaylsis(int NumbVar,int NbofSam,float x[10][20],float y[20],float cof[],float *s,float *r)    
{
    float mx[20];//
    float q,lyy,mf;
    int   i,j,k;
    for(k=0;k<NumbVar;k++)
    {
        mx[k]=0.0;
        for(i=0;i<NbofSam;i++)  mx[k]+=x[k][i];
        mx[k]/=(float)NbofSam;
    }
    mf=0.0;
    for(j=0;j<NbofSam;j++) mf+=y[j];
    mf/=(float)NbofSam;
    for(i=0;i<NumbVar;i++)
    {
        for(j=0;j<NumbVar;j++)
        {
            lij[i][j]=0.0;
            for(k=0;k<NbofSam;k++)
            {
                lij[i][j]+=(x[i][k]-mx[i])*(x[j][k]-mx[j]);
            }
        }
    }
    for(i=0;i<NumbVar;i++)
    {
        liy[i]=0.0;
        for(k=0;k<NbofSam;k++)
            liy[i]+=(x[i][k]-mx[i])*(y[k]-mf);
    }
    if(gauss(NumbVar,cof)==-1) return -1;//��˹�㷨
    for(i=0;i<NumbVar;i++)
        cof[NumbVar-i]=cof[NumbVar-i-1];
    cof[0]=mf;
    for(i=0;i<NumbVar;i++)
        cof[0]-=cof[i+1]*mx[i];
    //     ***the regression__coefficient__Check***
    if(NbofSam-NumbVar>1)
    {
       lyy=0.0;
       for(i=0;i<NbofSam;i++)
           lyy+=(y[i]-mf)*(y[i]-mf);
       q=lyy;
       for(i=0;i<NumbVar;i++)
           q-=cof[i+1]*liy[i];
        //       *s=sqrt(q/(NbofSam-NumbVar-1));
        //       *r=sqrt(1.0-q/lyy);
    }
    else
    {
        *s=0;
        *r=1;
    }
    return 1;
}

//==��˹�ⷨ
static int gauss(int NumbVar,float cof[])
{
    int n,i,j,k,ik;
    float mmax,p,aik;
    n=NumbVar+1;
    for(i=0;i<NumbVar;i++)
    lij[i][NumbVar]=liy[i];
    for(k=0;k<NumbVar;k++)
    {
        mmax=0.0;
        for(i=k;i<NumbVar;i++)
        {
            if(mmax<fabs(lij[i][k]))
            {
               mmax=fabs(lij[i][k]);ik=i;
            }
        }
        if(mmax==0)
        {
            //	show_message("????, ????????...");
            //	getch();clear_message();
            return -1;
        }
        if(ik!=k)
        {
           for(j=0;j<n;j++)
           {
               GauAA[j]=lij[ik][j];
               lij[ik][j]=lij[k][j];
               lij[k][j]=GauAA[j];
           }
        }
        p=lij[k][k];
        if(p==0)
        {
            //	show_message("????...   ");  getch();
           return -1;
        }
        for(j=k;j<=NumbVar;j++)
            lij[k][j]/=p;
        for(i=0;i<NumbVar;i++)
        {
            if(i!=k)
            {
               aik=lij[i][k];
               for(j=k;j<=NumbVar;j++)
                   lij[i][j]-=aik*lij[k][j];
            }
        }
    }
    for(i=0;i<NumbVar;i++)
    {
        cof[i]=lij[i][NumbVar];
    }
    return 1;
}




