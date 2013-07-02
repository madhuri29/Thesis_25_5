#include "../Headers/partition.h"

Partition::Partition()
{
	for(int i = 0; i <= numNodes ; i++)
	{
		intCon.push_back(0);
		extCon.push_back(0);
		lockStatus.push_back(false);
		basket.push_back(UNKNOWN_SIDE);
	}
}

Partition::~Partition()
{
	delete &intCon;
	delete &extCon;
	delete &lockStatus;
}

void Partition::init_part_random()
{
	for(int i = 1; i <= numNodes ; i++){
		if (i % 2 == 0 ) {
			partA.push_back(i);
			basket[i] = SIDE_A;
		}
		else {
			partB.push_back(i);
			basket[i] = SIDE_B;
		}
	}

}

void Partition::print_partition_data()
{
	cout<<"Basket A:"<<endl;
	for(unsigned int i = 0; i < partA.size(); i++) {
		cout<<partA[i]<<" ";
	}
	cout<<endl<<"Basket B:"<<endl;
	for(unsigned int i = 0; i < partB.size(); i++) {
		cout<<partB[i]<<" ";
	}
/*	cout<<endl<<"int Connections"<<endl;
	for(unsigned int i = 1; i <= intCon.size(); i++) {
		cout<<i<<":"<<intCon[i]<<" ";
	}
	cout<<endl<<"ext Connections"<<endl;
	for(unsigned int i = 1; i <= extCon.size(); i++) {
		cout<<i<<":"<<extCon[i]<<" ";
	}
*/
}
void Partition::compute_ext_int_con(Matrix *ConMat)
{
	int total_int = 0;
	int total_ext = 0;
	int k;
	vector<int> Neighbors;
	for (int i = 1; i <= numNodes; i++){
		Neighbors = ConMat->getNeighbors(i);
		if (basket[i] == SIDE_A){
			for(int j = 0; j < Neighbors.size(); j++) {
				k = Neighbors[j];
				if(basket[k] == SIDE_A) {
					total_int += ConMat->getWeight(i,k);
				} else {
					total_ext += ConMat->getWeight(i,k);
				}
			}
		} else { // i is part of B
			for(int j = 0; j < Neighbors.size(); j++) {
				k = Neighbors[j];
				if(basket[k] == SIDE_B) {
					total_int += ConMat->getWeight(i,k);
				} else {
					total_ext += ConMat->getWeight(i,k);
				}
			}
		}
		intCon[i]=total_int;
		extCon[i]=total_ext;
	}
}

