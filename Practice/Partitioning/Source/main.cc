#include <iostream>
#include <string>
#include <fstream>
#include "../Headers/matrix.h"
#include "../Headers/partition.h"
#include"../Headers/DNode.h"
using namespace std;

void test ()
{
	DNode D1(1,BASIC_RES,1,2);
	cout<<D1.getDVal()<<endl;
	cout<<D1.getId()<<endl;
}
int main(int argc, char *argv[])
{
	//ifstream is (argv[1]);
	string fileName = "C:/Users/madhuri/Desktop/Practice/Partitioning/Benchmarks/B1";
	ifstream is;
	is.open(fileName.c_str());
	stringstream str;
	str << "Failed to properly read benchmark: " << fileName << endl;
	claim (is.good(), &str);

	//Create Connectivity Matrix and Check for correctness
	int numNodes = 0;
	int numEdges = 0;
	is>>numNodes;
	is>>numEdges;

	int u = 0;
	int v = 0;

	Matrix* KLConMat = new Matrix(numNodes,numEdges);
	cout<<numNodes<<" "<<numEdges<<endl;

	while(is>>u)
	{
		//is>>u;
		is>>v;
		cout<<"u,v:"<<u<<","<<v<<endl;
		KLConMat->addEdge(u,v);
	}

	KLConMat->printMatrix();
	KLConMat->checkConnectivityMatrix();

	//Do KL partitioning
    Partition* KL_Partition = new Partition();
    KL_Partition->init_part_random();
   // KL_Partition->compute_ext_int_con(KLConMat);
    KL_Partition->print_partition_data();
	return 0;
}



