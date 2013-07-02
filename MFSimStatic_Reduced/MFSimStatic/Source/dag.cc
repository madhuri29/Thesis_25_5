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
 * Source: dag.cc																*
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
#include "../Headers/dag.h"
#include <string>
#include <fstream>

int DAG::orderNum = 1;
int DAG::next_id = 1;

///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
DAG::DAG()
{
	id = next_id++;
}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
DAG::~DAG()
{
	heads.clear();
	tails.clear();
	mixes.clear();
	splits.clear();
	heats.clear();
	detects.clear();
	storage.clear();
	others.clear();

	// Delete the actual AssayNodes
	while (allNodes.size() > 0)
	{
		AssayNode *n = allNodes.front();
		allNodes.erase(allNodes.begin());
		delete n;
	}
	allNodes.clear();

	// Storage Holder nodes are separate from rest of nodes (allNodes)
	while (storageHolders.size() > 0)
	{
		AssayNode *n = storageHolders.front();
		storageHolders.erase(storageHolders.begin());
		delete n;
	}
	storageHolders.clear();
}

///////////////////////////////////////////////////////////////
// Add the node to the DAG and to the appropriate vectors
///////////////////////////////////////////////////////////////
AssayNode * DAG::AddDispenseNode(string inputWell, double volume)
{
	return AddDispenseNode(inputWell, volume, "");
}
AssayNode * DAG::AddMixNode(int numDropsBefore, double seconds)
{
	return AddMixNode(numDropsBefore, seconds, "");
}
AssayNode * DAG::AddSplitNode(bool isFaultTolerant, int numDropsAfter, double seconds)
{
	return AddSplitNode(isFaultTolerant, numDropsAfter, seconds, "");
}
AssayNode * DAG::AddHeatNode(double seconds)
{
	return AddHeatNode(seconds, "");
}
AssayNode * DAG::AddDetectNode(int numDropsIO, double seconds)
{
	return AddDetectNode(numDropsIO, seconds, "");
}
AssayNode * DAG::AddOutputNode(string outputSink)
{
	return AddOutputNode(outputSink, "");
}
AssayNode * DAG::AddStorageNode()
{
	return AddStorageNode("");
}
AssayNode * DAG::AddGeneralNode()
{
	return AddGeneralNode("");
}

///////////////////////////////////////////////////////////////
// Add the node to the DAG and to the appropriate vectors;
// also lets you specify a name
///////////////////////////////////////////////////////////////
AssayNode * DAG::AddDispenseNode(string inputWell, double volume, string nodeName)
{
	AssayNode *node = new AssayNode(DISPENSE);
	node->portName = inputWell;
	node->volume = volume;
	node->name = nodeName;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddMixNode(int numDropsBefore, double seconds, string nodeName)
{
	AssayNode *node = new AssayNode(MIX);
	node->numDrops = numDropsBefore;
	node->seconds = seconds;
	//node->cycles = (unsigned)ceil((double)freqInHz * seconds);
	node->name = nodeName;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddSplitNode(bool isFaultTolerant, int numDropsAfter, double seconds, string nodeName)
{
	{	// Sanity check: Must split into at least 2 droplets
		stringstream msg;
		msg << "ERROR. SPLIT node splits into " << numDropsAfter << " drops. Must split into at least 2 drop." << ends;
		claim(numDropsAfter >= 2, &msg);
	}

	AssayNode *node = NULL;
	node = new AssayNode(SPLIT);
	node->numDrops = numDropsAfter;
	node->seconds = seconds;
	//node->cycles = (unsigned)ceil((double)freqInHz * seconds);
	node->name = nodeName;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddHeatNode(double seconds, string nodeName)
{
	AssayNode *node = new AssayNode(HEAT);
	node->seconds = seconds;
	//node->cycles = (unsigned)ceil((double)freqInHz * seconds);
	node->name = nodeName;
	node->numDrops = 1;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddDetectNode(int numDropsIO, double seconds, string nodeName)
{
	AssayNode *node = new AssayNode(DETECT);
	node->seconds = seconds;
	//node->cycles = (unsigned)ceil((double)freqInHz * seconds);
	node->name = nodeName;
	node->numDrops = numDropsIO;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddOutputNode(string outputSink, string nodeName)
{
	AssayNode *node = new AssayNode(OUTPUT);
	node->portName = outputSink;
	node->name = nodeName;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddStorageNode(string nodeName)
{
	AssayNode *node = new AssayNode(STORAGE);
	node->name = nodeName;
	//node->status = COMPLETE;
	node->order = orderNum++;
	AddNodeToDAG(node);
	return node;
}
AssayNode *DAG::AddStorageHolderNode()
{
	AssayNode *node = new AssayNode(STORAGE_HOLDER);
	storageHolders.push_back(node);
	return node;

}
/*AssayNode * DAG::AddStorageNode(string nodeName)
{
	AssayNode *node = new AssayNode(STORAGE);
	node->name = nodeName;
	AddNodeToDAG(node);
	return node;
}*/
AssayNode * DAG::AddGeneralNode(string nodeName)
{
	AssayNode *node = new AssayNode(GENERAL);
	node->name = nodeName;
	AddNodeToDAG(node);
	return node;
}

void DAG::AddNodeToDAG(AssayNode *node)
{
	OperationType opType = node->type;
	node->dag = this;

	allNodes.push_back(node);
	if (opType == DISPENSE)
		heads.push_back(node);
	else if (opType == OUTPUT)
		tails.push_back(node);
	else if (opType == MIX)
		mixes.push_back(node);
	else if (opType == SPLIT)
		splits.push_back(node);
	else if (opType == HEAT)
		heats.push_back(node);
	else if (opType == DETECT)
		detects.push_back(node);
	else if (opType == STORAGE)
		storage.push_back(node);
	else
		others.push_back(node);
}

///////////////////////////////////////////////////////////////
// Removes the node from the DAG (if found) vectors of nodes.
// Does not alter any edges or connections.
///////////////////////////////////////////////////////////////
void DAG::RemoveNodeFromDAG(AssayNode *node)
{
	OperationType opType = node->type;

	bool found = false;
	vector<AssayNode *>::iterator it = allNodes.begin();
	for (; it != allNodes.end(); it++)
	{
		if (node == *it)
		{
			allNodes.erase(it);
			found = true;
			break;
		}
	}

	{	// Sanity check: Node must be in DAG
		stringstream msg;
		msg << "ERROR. Assay node " << node->GetName() << " cannot be removed because not found in dag " << GetPrintableName() << "." << endl;
		claim(found, &msg);
	}

	vector<AssayNode *> *indivList = NULL;

	if (opType == DISPENSE)
		indivList = &heads;
	else if (opType == OUTPUT)
		indivList = &tails;
	else if (opType == MIX)
		indivList = &mixes;
	else if (opType == SPLIT)
		indivList = &splits;
	else if (opType == HEAT)
		indivList = &heats;
	else if (opType == DETECT)
		indivList = &detects;
	else if (opType == STORAGE)
		indivList = &storage;
	else
		indivList = &others;

	found = false;
	it = indivList->begin();
	for (; it != indivList->end(); it++)
	{
		if (node == *it)
		{
			indivList->erase(it);
			found = true;
			break;
		}
	}

	{	// Sanity check: Node must be in DAG
		stringstream msg;
		msg << "ERROR. Assay node " << node->GetName() << " cannot be removed because not found in individual list in dag " << GetPrintableName() << "." << endl;
		claim(found, &msg);
	}
}

///////////////////////////////////////////////////////////////
// Insert a node in the middle of the original parent/child
// by changing the graph
///////////////////////////////////////////////////////////////
void DAG::InsertNode(AssayNode *p, AssayNode *c, AssayNode *insert)
{
	// Remove old connections
	vector<AssayNode*>::iterator it;
	for (it = p->children.begin(); it != p->children.end(); it++)
	{
		if ((*it) == c)
		{
			p->children.erase(it);
			break;
		}
	}
	for (it = c->parents.begin(); it != c->parents.end(); it++)
	{
		if ((*it) == p)
		{
			c->parents.erase(it);
			break;
		}
	}
	// Insert new connections
	p->children.push_back(insert);
	insert->parents.push_back(p);
	insert->children.push_back(c);
	c->parents.push_back(insert);
}

///////////////////////////////////////////////////////////////
// Make 'c' a child of 'p' (and 'p' and parent of 'c')
///////////////////////////////////////////////////////////////
void DAG::ParentChild(AssayNode *p, AssayNode *c)
{
	p->children.push_back(c);
	c->parents.push_back(p);
}

///////////////////////////////////////////////////////////////
// Get a name for printing
///////////////////////////////////////////////////////////////
string DAG::GetPrintableName()
{
	stringstream str;
	str << "DAG" << id;
	if (name != "")
		str << "(" << name << ")";
	return str.str();
}

///////////////////////////////////////////////////////////////
// Prints the scheduling time-step information for this DAG
///////////////////////////////////////////////////////////////
void DAG::PrintSchedule()
{
	cout << "Printing schedule for DAG:" << endl;
	cout << "Node Name: [start time-step, end time-step), Resource Type" << endl;
	for (int i = 0; i < allNodes.size(); i++)
		allNodes.at(i)->Print();
}

///////////////////////////////////////////////////////////////
// Prints the parent/child relationships for this DAG
///////////////////////////////////////////////////////////////
void DAG::PrintParChildRelationships()
{

	for (int i = 0; i < allNodes.size(); i++)
	{
		AssayNode *c = allNodes.at(i);
		cout << "---------------------------------------------" << endl;
		for (int j = 0; j < c->parents.size(); j++)
		{
			cout << "\t";
			c->parents.at(j)->Print();
		}
		c->Print();
		for (int j = 0; j < c->children.size(); j++)
		{
			cout << "\t";
			c->children.at(j)->Print();
		}
	}
}

///////////////////////////////////////////////////////////////
// Sets the frequency of the DAG by calculating the number of
// cycles each node will take at that frequency.
///////////////////////////////////////////////////////////////
void DAG::setFreq(unsigned long long freqHz)
{
	freqInHz = freqHz;
	for (int i = 0; i < allNodes.size(); i++)
		allNodes.at(i)->cycles = (unsigned)ceil((double)freqInHz * allNodes.at(i)->seconds);
}

///////////////////////////////////////////////////////////////
//Creates a graph of the Dag in .dot format
///////////////////////////////////////////////////////////////
void DAG::OutputGraphFile(string filename, bool color, bool fullStats)
{

	//fullStats = false; // DTG DEBUG PRINT

	ofstream out;
	//filename = "Output/" + filename + ".dot";
	filename = filename + ".dot";

	out.open(filename.c_str());

	{
		stringstream str;
		str << "Failed to properly write DAG Graph file: " << filename << endl;
		claim (out.good(), &str);
	}

	out<<"digraph G {\n";
	//out<<"  size=\"8.5,10.25\";\n";
	//out << "label=\"" << filename << "\"";

	for (int i = 0; i < allNodes.size(); ++i)
	{
		AssayNode *n = allNodes.at(i);

		string labelName;
		string colorName;
		stringstream ss;
		switch( n->GetType()){
			case GENERAL:{
				ss << "GEN_" << n->id << " ";
				//ss << "G" << " ";
				labelName = ss.str();
				colorName = "steelblue";
				break;
			}
			case STORAGE_HOLDER:{
				ss << "STORE_" << n->id << " ";
				//ss << "S" << " ";
				labelName = ss.str();
				colorName = "grey65";
				//colorName = "black";
				break;
			}
			case STORAGE:{
				ss << "IndivSTORE_" << n->id << " ";
				//ss << "S" << " ";
				labelName = ss.str();
				colorName = "grey65";
				//colorName = "black";
				break;
			}
			case DISPENSE:{
				ss << "IN_" << n->id << " " << n->GetPortName() << " ";
				//ss << "D" << " ";
				labelName = ss.str();
				colorName = "cyan";
				break;
			}
			case MIX:{
				ss << "MIX_" << n->id;
				//ss << "M" << " ";
				labelName = ss.str();
				colorName = "yellow";
				break;
			}
			case SPLIT:{
				ss << "SPLT_" << n->id << " ";
				//ss << "S" << " ";
				labelName = ss.str();
				colorName = "seagreen1";
				break;
			}
			case HEAT:{
				ss << "HEAT_" << n->id << " ";
				//ss << "H" << " ";
				labelName = ss.str();
				colorName = "lightcoral";
				break;
			}
			case DETECT:{
				ss << "DET_" << n->id << " ";
				//ss << "Dt" << " ";
				labelName = ss.str();
				colorName = "orange";
				break;
			}
			case OUTPUT:{
				ss << "OUT_" << n->id << " " << n->GetPortName() << " ";
				//ss << "O" << " ";
				labelName = ss.str();
				colorName = "orchid1";
				break;
			}
		}
		if (n->GetName() != "")
			labelName = labelName + "(" + n->GetName() + ")";
		out << n->getId() << " [label = \"" << labelName;
		//out << n->getId() << " [label = \"" << "";
		if(fullStats)
		{
			if (n->seconds > 0)
				out << " (" << n->seconds <<"s)\\n";
			else
				out << "\\n";
			if (n->GetStatus() >= SCHEDULED)
				out << "Sched: TS[" << n->startTimeStep << ", " << n->endTimeStep << ")\\n";
			if (n->GetStatus() >= BOUND)
			{
				if (n->GetType() == DISPENSE || n->GetType() == OUTPUT)
				{
					if (n->ioPort->getSide() == NORTH)
						out << "IoPort: " << n->ioPort->getId() << ", (" << n->ioPort->getPosXY() << ", NORTH)\\n";
					else if (n->ioPort->getSide() == SOUTH)
						out << "IoPort: " << n->ioPort->getId() << ", (" << n->ioPort->getPosXY() << ", SOUTH)\\n";
					else if (n->ioPort->getSide() == EAST)
						out << "IoPort: " << n->ioPort->getId() << ", (EAST, " << n->ioPort->getPosXY() << ")\\n";
					else if (n->ioPort->getSide() == WEST)
						out << "IoPort: " << n->ioPort->getId() << ", (WEST, " << n->ioPort->getPosXY() << ")\\n";
				}
				else if (n->reconfigMod != NULL)
				{
					if (n->reconfigMod->getTileNum() >= 0)
						out << "Place: TN" << n->reconfigMod->getTileNum() << ": LxTy(" << n->reconfigMod->getLX() << ", " << n->reconfigMod->getTY() << ")\\n";
					else
						out << "Place: LxTy(" << n->reconfigMod->getLX() << ", " << n->reconfigMod->getTY() << ")\\n";
				}

			}
			out << "\"";
		}
		else
			out << "\"";
		if (color)
			out << " fillcolor=" << colorName << ", style=filled];\n";
		else
			out << "];\n";

		for (int j=0; j < n->children.size(); ++j)
			out << n->getId() << " -> " << n->children.at(j)->getId() << ";\n";
	}
	out<<"}\n";
}

///////////////////////////////////////////////////////////////
// Does basic rule checking, trying to validate that the data
// input actually forms a DAG and checks for potential mistakes.
// Also, once it has validated the DAG, it does a breadth-first
// ordering of the DAG for later processing.
///////////////////////////////////////////////////////////////
void DAG::ValidateAndOrder()
{
	int numSplitChildren = 0;
	int numMixParents = 0;

	// Make sure no heads have parents and one child
	for (unsigned i = 0; i < heads.size(); i++)
	{
		AssayNode *h = heads.at(i);
		{
			stringstream msg;
			msg << "ERROR. A head (DISPENSE/TRANSFER_IN) node has a parent." << endl;
			claim(h->GetParents().size() == 0, &msg);
		}
		{
			stringstream msg;
			msg << "ERROR. A head (DISPENSE/TRANSFER_IN) node has no children." << endl;
			claim(h->GetChildren().size() > 0, &msg);
		}
	}
	// Make sure no tails have children and one parent
	for (unsigned i = 0; i < tails.size(); i++)
	{
		AssayNode *t = tails.at(i);
		{
			stringstream msg;
			msg << "ERROR. A tail (OUTPUT/TRANSFER_OUT) node has a child." << endl;
			claim(t->GetChildren().size() == 0, &msg);
		}
		{
			stringstream msg;
			msg << "ERROR. A tail (OUTPUT/TRANSFER_OUT) node has no parents." << endl;
			claim(t->GetParents().size() > 0, &msg);
		}
	}
	// Make sure mixes have 1 output
	for (unsigned i = 0; i < mixes.size(); i++)
	{
		AssayNode *m = mixes.at(i);
		numMixParents += m->parents.size();

		{
			stringstream msg;
			msg << "ERROR. A MIX node has " << m->GetParents().size() << " parents...should have " << m->numDrops << " parents." << endl;
			claim(m->GetParents().size() == m->numDrops, &msg);
		}
		{
			stringstream msg;
			msg << "ERROR. A MIX node has " << m->GetChildren().size() << " children...should have 1 child." << endl;
			claim(m->GetChildren().size() == 1, &msg);
		}
	}
	// Make sure splits have 1 input and 'numDropsAfterSplit' outputs
	for (unsigned i = 0; i < splits.size(); i++)
	{
		AssayNode *s = splits.at(i);
		numSplitChildren += s->numDrops;

		{
			stringstream msg;
			msg << "ERROR. A SPLIT node has " << s->GetChildren().size() << " children...should have " << s->numDrops << " children." << endl;
			claim(s->GetChildren().size() == s->numDrops, &msg);
		}
		{
			stringstream msg;
			msg << "ERROR. A SPLIT node has " << s->GetParents().size() << " parents...should have 1 parent." << endl;
			claim(s->GetParents().size() == 1, &msg);
		}
	}
	// Make sure all other nodes (GENERAL/FTSplit) have exactly 1 input and 1 output
	for (unsigned i = 0; i < others.size(); i++)
	{
		AssayNode *o = others.at(i);
		{
			stringstream msg;
			msg << "ERROR. A General/FTSPlit node has " << o->GetChildren().size() << " children...should have 1 child." << endl;
			claim(o->GetChildren().size() == 1, &msg);
		}
		{
			stringstream msg;
			msg << "ERROR. A General/FTSplit node has " << o->GetParents().size() << " parents...should have 1 parent." << endl;
			claim(o->GetParents().size() == 1, &msg);
		}
	}
	for (unsigned i = 0; i < heats.size(); i++)
	{
		AssayNode *h = heats.at(i);
		{
			stringstream msg;
			msg << "ERROR. A heat node has " << h->GetChildren().size() << " children...should have 1 child." << endl;
			claim(h->GetChildren().size() == 1, &msg);
		}
		{
			stringstream msg;
			msg << "ERROR. A heat node has " << h->GetParents().size() << " parents...should have 1 parent." << endl;
			claim(h->GetParents().size() == 1, &msg);
		}
	}
	for (unsigned i = 0; i < detects.size(); i++)
	{
		AssayNode *d = detects.at(i);
		{
			stringstream msg;
			msg << "ERROR. A detect node has " << d->GetChildren().size() << " children...should have " << d->numDrops << " child(ren)." << endl;
			claim(d->GetChildren().size() == d->numDrops, &msg);
		}
		{
			stringstream msg;
			msg << "ERROR. A detect node has " << d->GetParents().size() << " parents...should have " << d->numDrops << " parent(s)." << endl;
			claim(d->GetParents().size() == d->numDrops, &msg);
		}
	}


	{	// #Dispenses - #Mixes + #Splits - #Outputs should == 0
		stringstream msg;
		msg << "ERROR. #Dispenses - #Mixes + #Splits - #Outputs should = 0" << endl;
		claim(heads.size() - (numMixParents - mixes.size()) + (numSplitChildren - splits.size()) - tails.size() == 0, &msg);
	}


	// Re-init all node orders to 0
	for (unsigned i = 0; i < allNodes.size(); i++)
		allNodes.at(i)->order = 0;

	//////////////////////////////////////////////////////////////
	// Do topological sort to get ordering and determine if DAG
	// First create a mapping of the edges
	map<AssayNode*, vector<AssayNode*> *> edges;
	for (int i = 0; i < allNodes.size(); i++)
	{
		vector<AssayNode*> * childEdges = new vector<AssayNode*>();
		AssayNode *node = allNodes.at(i);
		for (int j = 0; j < node->children.size(); j++)
			childEdges->push_back(node->children.at(j));
		edges[node] = childEdges;
	}

	// Now perform algorithm as seen at
	// http://en.wikipedia.org/wiki/Topological_sorting for alg.
	vector<AssayNode*> L;
	vector<AssayNode*> S;
	for (int i = 0; i < heads.size(); i++)
		S.push_back(heads.at(i));
	while (!S.empty())
	{
		AssayNode *n = S.back();
		S.pop_back();
		L.push_back(n);

		vector<AssayNode*> *ms = edges.find(n)->second;
		while (!ms->empty())
		{
			AssayNode *m = ms->back();
			ms->pop_back();

			bool mIsReady = true;
			map<AssayNode*, vector<AssayNode*> *>::iterator it = edges.begin();
			for (it; it != edges.end(); it++)
			{
				vector<AssayNode*> *es = it->second;
				for (int i = 0; i < es->size(); i++)
				{
					if (es->at(i) == m)
					{
						mIsReady = false;
						break;
					}
				}
			}
			if (mIsReady)
				S.push_back(m);
		}
	}
	for (int i = 0; i < L.size(); i++)
		L.at(i)->order = i + 1;

	while(edges.empty())
	{
		delete edges.begin()->second;
		edges.erase(edges.begin());
	}

	//////////////////////////////////////////////////////////////
	// Now that we have an order, make sure monotonic (that is,
	// no children have a lower order than parents...should help
	// prevent cycles)
	for (unsigned i = 0; i < allNodes.size(); i++)
	{
		AssayNode *n = allNodes.at(i);
		for (unsigned j = 0; j < n->children.size(); j++)
		{
			AssayNode *c = n->children.at(j);
			{
				stringstream msg;
				msg << "ERROR. Cycle detected: Node " << n->order << " (" << n->name << ") points to higher/equal node, Node " << c->order << " (" << c->name << ")" << endl;
				claim((c->order > n->order) && n->order != 0, &msg);
			}

		}
	}
}
