#ifndef _DATA_H
#define _DATA_H
#include <iostream>
using namespace std;
class Data
{
protected:
	int numNodes;
	int numEdges;

public:
	//getters/setters
	int getNumNodes() {return numNodes;}
	void setNumNodes(int num) {numNodes = num;}
	int getNumEdges() {return numEdges;}
	void setNumEdges(int num) {numEdges = num;}
};
#endif
