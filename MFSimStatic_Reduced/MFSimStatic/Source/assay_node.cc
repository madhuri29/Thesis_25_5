/*------------------------------------------------------------------------------*
 *                       (c)2013, All Rights Reserved.     						*
 *       ___           ___           ___     									*
 *      /__/\         /  /\         /  /\    									*
 *      \  \:\       /  /:/        /  /::\   									*
 *       \  \:\     /  /:/        /  /:/\:\  									*
 *   ___  \  \:\   /  /:/  ___   /  /:/~/:/        								*
 *  /__/\  \__\:\ /__/:/  /  /\ /__/:/ /:/___     UCR DMFB Synthesis Framework  *
 *  \  \:\ /  /:/ \  \:\ /  /:/ \  \:\/:::::/     www.microfluidics.cs.ucr.edu	*
 *   \  \:\  /:/   \  \:\  /:/   \  \::/~~~~ 									*
 *    \  \:\/:/     \  \:\/:/     \  \:\     									*
 *     \  \::/       \  \::/       \  \:\    									*
 *      \__\/         \__\/         \__\/    									*
 *-----------------------------------------------------------------------------*/
/*---------------------------Implementation Details-----------------------------*
 * Source: assay_node.cc														*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: October 7, 2012							*
 *																				*
 * Details: N/A																	*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
#include "../Headers/assay_node.h"

int AssayNode::next_id = 1;

///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
AssayNode::AssayNode(OperationType opType)
{
	id = next_id++;
	type = opType;
	order = 0;
	cycles = 0;
	volume = 0;
	reading = 0;
	seconds = 0;
	portName = "EMPTY";
	status = UNBOUND_UNSCHED;
	boundedResType = UNKNOWN_RES;
	startCycle = 1000000000;
	priority = 0;
	startTimeStep = 0;
	endTimeStep = 0;
	numDrops = 0;
	transfer = NULL;
	reconfigMod = NULL;
	ioPort = NULL;
}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
AssayNode::~AssayNode()
{
	children.clear();
	parents.clear();
	droplets.clear();
}

//////////////////////////////////////////////////////////////////
// Tells whether the AssayNode is still "in control" of the droplet
// with global id "gid", or if it has given control to another node
//////////////////////////////////////////////////////////////////
bool AssayNode::isMaintiningDroplet(Droplet *drop)
{
	vector<Droplet *>::iterator it;
	for(it = droplets.begin(); it != droplets.end(); it++)
	{
		if (*it == drop)
			return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////
// Removes the given droplet from the node's droplet list
//////////////////////////////////////////////////////////////////
void AssayNode::eraseFromDropList(Droplet* drop)
{
	vector<Droplet *>::iterator it;
	for(it = droplets.begin(); it != droplets.end(); it++)
	{
		if (*it == drop)
		{
			droplets.erase(it);
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////
// Prints basic scheduling, name and resource info
//////////////////////////////////////////////////////////////////
void AssayNode::Print()
{
	string t = "UNKNOWN";
	if (type == MIX)
		t = "mix";
	else if (type == SPLIT)
		t = "split";
	else if (type == HEAT)
		t = "heat";
	else if (type == DETECT)
		t = "detect";
	else if (type == DISPENSE)
		t = "in";
	else if (type == OUTPUT)
		t = "out";
	else if (type == STORAGE_HOLDER)
		t = "storage";
	else if (type == STORAGE)
		t = "individual storage";

	//cout << "P" << priority << " "; // DTG Debug

	cout << name << "(" << t << "): [" << startTimeStep << ", " << endTimeStep << "),";
	//cout << "\t";
	if (type == DISPENSE)
		cout << " \"" << portName << "\" Input Well" << endl;
	else if (type == OUTPUT)
		cout << " \"" << portName << "\" Output Sink" << endl;
	else
	{
		switch(boundedResType)
		{
			case BASIC_RES:
				cout << " Basic Chamber" << endl;
				break;
			case D_RES:
				cout << " Detecting Chamber" << endl;
				break;
			case H_RES:
				cout << " Heating Chamber" << endl;
				break;
			case DH_RES:
				cout << " Heating/Detecting Chamber" << endl;
				break;
			default:
				//cerr << endl << "UNKNOWN RESOURCE TYPE" << endl;
				//exit(1);
				cout << " Unknown Res Type" << endl;
				break;
		}
	}
	list<AssayNode*>::iterator it = storageOps.begin();
	for (; it != storageOps.end(); it++)
	{
		cout << "\t";
		(*it)->Print();
	}
}
