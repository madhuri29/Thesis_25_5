/*------------------Class Details------------------------
 * Name: Matrix
 *
 * Description: Defines the connectivity matrix
 * 				A vector of vectors that stores the connectivity information
 * 				between nodes
 *--------------------------------------------------------*/

#ifndef _MATRIX_H
#define _MATRIX_H
#include <iostream>
#include <vector>
#include <string>
#include "edge.h"
#include "data.h"
#include "claim.h"
#include <algorithm>
using namespace std;

class Matrix : public Data
{
	vector<vector<Edge> > mat;
//methods
	int binarySearchMat(vector<Edge> &thisvector, int searchId, int low, int high);

public:
	//constructors/destructors
	Matrix();
	Matrix(int nodes, int edges);
	~Matrix();

	//methods
	int edgeExists(int node1Id,int node2Id );
	void addEdge (int node1Id,int node2Id );
	int getWeight (int node1Id,int node2Id);
	vector<int> getNeighbors(int nodeId);
	void printMatrix();
	void checkConnectivityMatrix();

};
#endif
