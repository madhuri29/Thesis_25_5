#ifndef _DNODE_H
#define _DNODE_H
#include<iostream>
#include "node.h"
using namespace std;
 class DNode: public Node
 {
	 int dValue;
	 DNode();
 public:
	 //constructors/destructors
	 DNode(int num, ResourceType rType, int loc, int d_val): Node(num,rType,loc)
 	{
		 dValue = d_val;
 	}
	 ~DNode();
	 //getters/setters
	 void setDVal(int val) {dValue = val;}
	 int getDVal() {return dValue;}

 };
#endif
