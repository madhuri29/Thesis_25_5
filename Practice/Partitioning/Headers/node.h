/*--------------------------------Class Details---------------------------------*
 * Name: Node
 *
 * Details: Represents the Node in the connected undirected graph used for
 * representing the work module.
 *-----------------------------------------------------------------------------*/

#ifndef _NODE_H
#define _NODE_H
using namespace std;
#include "enum.h"

class Node
{
	int id;
	ResourceType resType;
	int location;
	//leftX, TopY, rightX, bottomY can be added of required
	Node(); // makes default constructor illegal
public:
	//constructors/destructors
	Node(int num, ResourceType rType, int loc);
	~Node();
	//getters/setters
	int getId () {return id;}
	//void setId (int num) {id = num;}
	ResourceType getResType () {return resType;}
	//void setResType (ResourceType rType) {resType = rType;}
	int getLocation () {return location;}
	void setLocation (int loc) {location = loc;}
	//methods
};
#endif
