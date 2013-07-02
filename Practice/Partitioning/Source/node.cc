#include "../Headers/node.h"
///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
Node::Node(int num, ResourceType rType, int loc)
{
	id = num;
	resType = rType;
	location = loc;
}
///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
Node::~Node()
{
	delete &id;
	delete &resType;
	delete &location;
}


