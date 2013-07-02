#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits.h>
#include <time.h>
#include <search.h>
#define MAX1 50000
#define MAX2 25000
using namespace std;
#include<string.h>
int nodes;
int Ia[MAX1];
int Ea[MAX1];
int Da[MAX1];
struct nodelement{
	int cellid;
	int netwt;
};

struct tempset
{
	int setele;
	int lock;
}tempA[MAX2],tempB[MAX2],DgainA[MAX2],DgainB[MAX2],DgainA1[MAX2],DgainB1[MAX2];

struct gainv
{
	int aele;
	int bele;
	int gain;
	int indA;
	int indB;
};

vector<vector<nodelement> > conmat;

void initmat()
{	
	for (int i=0;i<=nodes;i++)
	{
		conmat.push_back(vector<nodelement>());
	}	
}

int binsearchvectrec(const vector<nodelement> &thisvector, int cellid, int low, int high)
{
	int mid = (high + low)/2;
	int rtv;
	int midcellid;
	
	rtv = -1;

	if (high < low) {
	  return rtv;
	}

	midcellid = thisvector[mid].cellid;
	if (midcellid > cellid) {
	  rtv = binsearchvectrec(thisvector, cellid, low, (mid - 1));
	} else if (midcellid < cellid) {
	  rtv = binsearchvectrec(thisvector, cellid, mid + 1, high);
	} else {
	  rtv = mid;
	}
	return rtv;
}

int searchnode(int x, int y)
{
	int j;
	
	j = -1;
	if (1) {
		j = binsearchvectrec(conmat[x], y, 0, (conmat[x].size() - 1));
	} else {	
	for(int i=0;i<conmat[x].size();i++)
	{
		if (conmat[x][i].cellid == y)
		{
			j=i;
			break;
		}
			else		
		{
			j=-1;
		}
	}
	}
	return j;
}

bool cmpfunction(nodelement i, nodelement j) 
{ 
	return (i.cellid<j.cellid); 
}

void addelement(int x,int y)
{	
	int found,found1;
	found = searchnode(x,y);
	
	if (found >= 0)
		
		{
		  found1 = searchnode(y,x);
		  conmat[x][found].netwt = conmat[x][found].netwt + 1;
		  conmat[y][found1].netwt = conmat[y][found1].netwt + 1;
		}
		else
		{
		nodelement newnode;
		newnode.cellid=y;
		newnode.netwt=1;
		conmat[x].push_back(newnode);	
		nodelement newnode1;	
		newnode1.cellid=x;
		newnode1.netwt=1;
		conmat[y].push_back(newnode1);
		std::sort(conmat[x].begin(), conmat[x].end(), cmpfunction);
		std::sort(conmat[y].begin(), conmat[y].end(), cmpfunction);
		}
}

void randsplit()
{
	int split=1;
	for (int i=0;i<nodes/2;i++)
	{
	tempA[i].setele=split;
	tempA[i].lock=0;
	split=split+1;
	tempB[i].setele=split;
	tempB[i].lock=0;
	split=split+1;
				
}
	
}

void calEaIa(tempset temp1[MAX2], tempset temp2[MAX2])
{
	int seta_x=0,seta_y=0,seta_z=0;	
	int setb_x=0,setb_y=0,setb_z=0;
	int suminta=0,sumexta=0;
	int sumintb=0,sumextb=0;
	int find1=0,find2=0,find3=0,find4=0;
	for (int i=0;i<nodes/2;i++)
	{
	seta_x=temp1[i].setele;
	setb_x=temp2[i].setele;
	suminta=0;
	sumintb=0;
	sumexta=0;
	sumextb=0;
	find1=0;
	find2=0;
	find3=0;
	find4=0;
		for(int j=0;j<nodes/2;j++)
		{
  			seta_y=temp1[j].setele;
  			setb_y=temp2[j].setele;
  			find1=searchnode(seta_x,seta_y);
  			find2=searchnode(setb_x,setb_y);
  			find3=searchnode(seta_x,setb_y);
  			find4=searchnode(setb_x,seta_y);
  			if(find1 >= 0)
			{
		 		suminta=suminta+conmat[seta_x][find1].netwt;	 	
			}
			if(find2 >= 0)
			{
				sumintb=sumintb+conmat[setb_x][find2].netwt;
			}
			if(find3 >= 0)
  			{
		 		sumexta=sumexta+conmat[seta_x][find3].netwt; 	
  			}
  			if(find4 >= 0)
  			{
		 		sumextb=sumextb+conmat[setb_x][find4].netwt;	
  			}
  			
		}
	Ia[seta_x-1]=suminta;
	Ia[setb_x-1]=sumintb;
	Ea[seta_x-1]=sumexta;
	Ea[setb_x-1]=sumextb;
	}
}

int bygain(const void *elem1,const void *elem2)
{
return((tempset*)elem1)->lock > ((tempset*)elem2) -> lock? 1 : -1;
}

void sortdata(int count ,tempset tempset[])
{
qsort(tempset,count,sizeof(tempset),bygain);
}

int calDa()
{
	for (int i=0;i<nodes;i++)
	{
		Da[i]=Ea[i]-Ia[i];
	}
}



gainv calgain()
{	
	int x,y,Ga;
	gainv old;
	gainv highest;
	int flag=0;
	old.aele=0;
	old.bele=0;
	old.gain=0;
	old.indA=0;
	old.indB=0;
	highest=old;

	int Eaind,cutwt,ind;
	for (int i=0;i<nodes/2;i++)
		{
		x=tempA[i].setele;
		for(int j=0;j<nodes/2;j++)	
			{
				y=tempB[j].setele;
				if(tempA[i].lock == 0 && tempB[j].lock == 0)
				{	
					Eaind=searchnode(x,y);
					if(Eaind >= 0)
					{
					ind=Eaind;	
					cutwt=conmat[x][ind].netwt;
					Ga=Da[x-1]+Da[y-1]-2*cutwt;
					}
					else
					{
					Ga=Da[x-1]+Da[y-1];
					}
					old.aele=x;
					old.bele=y;
					old.gain=Ga;
					old.indA=i;
					old.indB=j;
						if(flag == 0)
						{
							highest.aele=x;
							highest.bele=y;
							highest.gain=Ga;
							highest.indA=i;
							highest.indB=j;
							flag=1;
						}
						else	
						{
							if(Ga>highest.gain)
								{
								highest.aele=x;
								highest.bele=y;
								highest.gain=Ga;
								highest.indA=i;
								highest.indB=j;
								}
						}						
					}							
				}
		}
	return highest;
}

void copysortda()
{
int j=0;
//cout<<"int copysortda loop";
for (int i=0;i<nodes/2;i++)
{
//cout<<"int copysortda for 1 loop";
DgainA1[i].setele=i;
DgainA1[i].lock=Da[tempA[i].setele-1];
//cout<<i<<" ele of A is "<<tempA[i].setele<<"  whose Da is "<<Da[tempA[i].setele-1]<<"\n";
DgainB1[i].setele=i;
DgainB1[i].lock=Da[tempB[i].setele-1];
//cout<<i<<" ele of B is "<<tempB[i].setele<<"  whose Da is "<<Da[tempB[i].setele-1]<<"\n";
}
/*for (int i=0;i<nodes/2;i++)
{
cout<<"before sort "<<DgainA[i].setele<<"a ele "<<"\t";
cout<<"before sort "<<DgainA[i].lock<<" a da "<<"\n";
cout<<"before sort "<<DgainB[i].setele<<" b ele "<<"\t";
cout<<"before sort "<<DgainB[i].lock<<" b da  "<<"\n";
}*/
sortdata(nodes/2,DgainA1);
sortdata(nodes/2,DgainB1);
for (int i=(nodes/2)-1;i>=0;i--)
{
//cout<<"int copysortda for 2 loop";
DgainA[i].setele=DgainA1[j].setele;
DgainA[i].lock=DgainA1[j].lock;
DgainB[i].setele=DgainB1[j].setele;
DgainB[i].lock=DgainB1[j].lock;
j=j+1;

//cout<<"after sort "<<DgainA[i].setele<<" a ele "<<"\t";
//cout<<"after sort "<<DgainA[i].lock<<" a da "<<"\n";
//cout<<"after sort "<<DgainB[i].setele<<"b ele "<<"\t";
//cout<<"after sort "<<DgainB[i].lock<<"b da "<<"\n";
}
}


gainv newgain()
{

	int x,y,Ga,x1,y1;
	gainv old;
	gainv highest;
	int test=0;
	int flag=0;
	old.aele=0;
	old.bele=0;
	old.gain=0;
	old.indA=0;
	old.indB=0;
	highest=old;


	int Eaind,cutwt,ind;
	copysortda();
	for (int i=0;i<nodes/2;i++)
		{
		//cout<<"in new gain loops 1st for";
		x=DgainA[i].setele;
		x1=tempA[x].setele;
		for(int j=0;j<nodes/2;j++)	
			{
				//cout<<"in new gain loops 2st for";
				y=DgainB[j].setele;
				y1=tempB[y].setele;
				if(tempA[x].lock == 0 && tempB[y].lock == 0 && test==0)
				{
				Eaind=searchnode(x1,y1);
					if(Eaind >= 0 && test==0)
					{
					ind=Eaind;	
					cutwt=conmat[x1][ind].netwt;
					Ga=DgainA[i].lock+DgainB[j].lock-2*cutwt;
					}
					else
					{
					Ga=DgainA[i].lock+DgainB[j].lock;
					}
					old.aele=x1;
					old.bele=y1;
					old.gain=Ga;
					old.indA=x;
					old.indB=y;
					if(flag == 0 && test==0)
						{
							highest.aele=x1;
							highest.bele=y1;
							highest.gain=Ga;
							highest.indA=x;
							highest.indB=y;
							flag=1;
						}
						else	
						{
							if(Ga>highest.gain && test ==0)
								{
								highest.aele=x1;
								highest.bele=y1;
								highest.gain=Ga;
								highest.indA=x;
								highest.indB=y;
								}
							else
								{
								test =1;
								}	
						}						
					}							
				}
		}
	return highest;
}
			
			
	



void lockele(gainv gswap)
{	
	int x,y;
	int Apos,Bpos;
	x=gswap.aele;
	y=gswap.bele;
	Apos=gswap.indA;
	Bpos=gswap.indB;
	tempA[Apos].lock=1;
	tempB[Bpos].lock=1;	
}

int maxgain(gainv G1[MAX1])
{
	int sum=0;
	int flag=0;
	int gqueue[nodes/2];
	int maxpos=0;
	int maxsum;
	for (int i=0;i<nodes/2;i++)
	{
		sum=sum+G1[i].gain;
		gqueue[i]=sum;
		if(flag==0)
		{
			maxsum=sum;
			flag=1;
		}
		else
			{
			if(sum > maxsum)
			{
				maxsum=sum;
				maxpos=i;
			}	
			}		
	}
	return maxpos;
}



void permswap(gainv G2[MAX1],int pos)
{
	for (int i=0;i<=pos;i++)
	{
		tempA[G2[i].indA].setele=G2[i].bele;
		tempB[G2[i].indB].setele=G2[i].aele;
	}
}

int calcutset()

{
	int cutset=0;
	int x;
	for(int i=0;i<nodes/2;i++)
	{
		x=tempA[i].setele;
		cutset=cutset+Ea[x-1];
	}
	return cutset;
}

void printVect(vector<nodelement> &thisvector) {
  for (int i=0;i< thisvector.size(); i++) {
    cout << thisvector[i].cellid << endl;
  }
}


void updateDa(gainv G3)
{
int x,y,f,f1,cax,cax1,cbx,cbx1,found,found1;
x=G3.aele;
y=G3.bele;
	for(int i=0;i<nodes/2;i++)
	{
	cax=0;
	cbx=0;
	f=tempA[i].setele;
	cax1=0;
	cbx1=0;
	f1=tempB[i].setele;	
	if(tempA[i].lock == 0)
		{
			found=searchnode(x,f);
			found1=searchnode(y,f);
			if(found >=0)
				{
					cax=conmat[x][found].netwt;
				}	
			if(found1 >= 0)
				{	
					cbx=conmat[y][found1].netwt;
				}
			if(found >=0 || found1>=0)
				{
				Da[f-1]=Da[f-1]+2*cax-2*cbx;
				}
		}
	if(tempB[i].lock == 0)
		{
			found=searchnode(x,f1);
			found1=searchnode(y,f1);
			if(found >=0)
				{
					cax1=conmat[x][found].netwt;
								
				}	
			if(found1 >= 0)
				{	
					cbx1=conmat[y][found1].netwt;
				}
			if(found >=0 || found1>=0)
				{
				Da[f1-1]=Da[f1-1]+2*cbx1-2*cax1;
				}
		}
	}
}
		
				
void unlock()
{
for(int i=0;i<nodes/2;i++)
{
tempA[i].lock=0;
tempB[i].lock=0;
}
}

int main(int argc, char *argv[])
{
	int size=0;
	int val=0;
	int val1=0;
	int count = 0;
	int netsize;
	int x;
	int y;
	gainv G[MAX1];
	clock_t start, end;
	start = clock();
	///declare the input and the output files
	fstream benchfile,outmatrix;
	//open the input file
	benchfile.open(argv[1],ios::in);
	//open the output file
	outmatrix.open("matrix.txt",ios::out);
	//check if the input file has opened properly
		if (benchfile.is_open())
		{
		cout<< "bench file opened successfully"<<"\n";
		}
		else 
		{
		
		cout<< "bench file open error";
		}
	//point to the first element of th einput file
	benchfile.seekg(0,ios::beg);
	while(benchfile>>val)
	{
		if (count==0)
		{
	//gets the first line of the bechmark file which is the number of nodes
		nodes=val;
		cout<<"number of nodes"<<val<<"\n";	
		}
		if (count==1)
		{
		//gets the second line which is the netsize
			netsize=val;
			break;
		}
		count ++;
	}
	initmat();
	// create the connnectivity matrix vector
	while (benchfile>>val)
		{
		benchfile>>val1;
		x=val;
		y=val1;
		addelement(x,y);
		}

	   	//randomnly creates 2 sets
		randsplit();
		int prevCutset = INT_MAX;
		int fincutset=0;
		int iterationcount = 1;
		int reset=1;
		calEaIa(tempA,tempB);
		while(iterationcount < 3) 
		{
			if (reset == 1)
			{
			//	cout << "Iteration count: " << iterationcount << " Cutset before iteration: " << prevCutset << endl;
				
				calDa();
				G[0]=newgain();
				/*cout<<"a element"<<G[0].aele<<"\t";
		      		cout<<"b element"<<G[0].bele<<"\t";
		      		cout<<"gain"<<G[0].gain<<"\t";
		      		cout<<"indA"<<G[0].indA<<"\t";
		      		cout<<"indB"<<G[0].indB<<"\n";*/
				lockele(G[0]);
				reset++;
			}
			else
			{
			 for (int i=1;i<nodes/2;i++)
			    {
			    updateDa(G[i-1]);
			      G[i]=newgain();
			      	lockele(G[i]);
				/*cout<<"a element"<<G[i].aele<<"\t";
		      		cout<<"b element"<<G[i].bele<<"\t";
		      		cout<<"gain"<<G[i].gain<<"\t";
		      		cout<<"indA"<<G[i].indA<<"\t";
		      		cout<<"indB"<<G[i].indB<<"\n";*/
			    }
			 int gainpos=0;
			  gainpos=maxgain(G);
			  permswap(G,gainpos);
			  unlock();
			
		 	 calEaIa(tempA,tempB);
		 	 fincutset=calcutset();
		 	 if (fincutset >= prevCutset) {
		 	   fincutset = prevCutset;
		 	   break;
		 	 }
		 	 prevCutset = fincutset;
		 	// cout << "Iteration count: " << iterationcount++ << " Cutset after iteration: " << fincutset << endl;
			reset=1;
			}
		}		
		//cout << "Final cutset is : " << fincutset << endl;
	cout<<" Final Cutset value is: " << fincutset<<endl;
	cout<<" Set A"<<"\t"<<"SetB"<<"\n";
	for(int i=0;i<nodes/2;i++)
	{
	cout<<tempA[i].setele<<"\t"<<tempB[i].setele<<"\n";
	}

		
    //outmatrix.seekg(0, ios::end);    
    //size=outmatrix.tellg();
    //cout<<size;
	outmatrix.close();
	end = clock();
	cout << "Time required for execution: "<< (double)(end-start)/CLOCKS_PER_SEC<< " seconds." << "\n\n";
	return 0;
}
	
