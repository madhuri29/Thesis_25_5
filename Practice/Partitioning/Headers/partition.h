#include <iostream>
#include <vector>
#include"enum.h"
#include "data.h"
#include "dnode.h"
#include "qval.h"
#include "claim.h"
#include "matrix.h"
using namespace std;

class Partition:public Data
{
	vector<int> partA;
	vector<int> partB;
	vector<int> extCon;
	vector<int> intCon;
	vector<bool> lockStatus;
	vector<Sides> basket;
	vector<DNode> DA;
	vector<DNode> DB;
	vector<QVal> Queue;

public:
	//constructor/destructor
	Partition();
	~Partition();
	//getters/setters

	//methods
	void print_partition_data();
	void init_part_random();
	void compute_ext_int_con(Matrix* mat);

};
