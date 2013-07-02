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
/*--------------------------------File Details----------------------------------*
 * Name: Structures																*
 *																				*
 * Details: This file contains some basic structures used by various classes.	*
 * They are placed here to keep the other files from getting clustered.			*
 *-----------------------------------------------------------------------------*/
#ifndef _STRUCTS_H
#define _STRUCTS_H

#include <ext/slist>
#include <sstream>
#include "enums.h"
#include <vector>
#include <string.h>
#include <list>
#include <map>
#include "Resources/reconfig_module.h"
#include "Resources/fixed_module.h"
using namespace std;

class IoPort;
class AssayNode;

/////////////////////////////////////////////////////////////////
// Coord: Simple (x, y) coordinate.
/////////////////////////////////////////////////////////////////
struct Coord {
    int x;
    int y;
};

/////////////////////////////////////////////////////////////////
// AssayPathNode Structure: Used to represent multiple assay
// nodes that are part of the same path (connected by continuous
// edges) and have the same resource type.
/////////////////////////////////////////////////////////////////
struct AssayPathNode {
	vector<AssayNode *> pathNodes;
	ResourceType resType;
	unsigned long long startTS;
	unsigned long long endTS;
	FixedModule *res;
	vector<AssayPathNode *> parents;
	vector<AssayPathNode *> children;
};

/////////////////////////////////////////////////////////////////
// BoardCell: Holds the basic information for a cell to be used
// with various routing algorithms.
/////////////////////////////////////////////////////////////////
struct BoardCell {
    int x;
    int y;
    int S;
    int C;
    //everything below is useful for debugging purposes.
    int route_distance;
    BoardCell* trace;
    vector<BoardCell *> path;
};

/////////////////////////////////////////////////////////////////
// SoukupCell: Holds the basic information for a cell to be used
// with Soukup's algorithm (maze routing)
/////////////////////////////////////////////////////////////////
struct SoukupCell {
	int x;
	int y;
	int S;
	int C;
	SoukupCell* trace;
};
struct LeeCell {
	int x;
	int y;
	int S;
	int C;
	int val;
	LeeCell* trace;
};
/////////////////////////////////////////////////////////////////
// RoutePoint: Holds the basic information for a point in a
// droplet's route
/////////////////////////////////////////////////////////////////
struct RoutePoint {
	unsigned long long cycle;
	int x;
	int y;
	DropletStatus dStatus;
};

/////////////////////////////////////////////////////////////////
// IoResource Structure: Holds basic info for a Dispense
// resource so the scheduler can tell if a proper Dispense resource
// is ready.
/////////////////////////////////////////////////////////////////
struct IoResource {
	string name;
	IoPort *port;
	unsigned long long lastEndTS; // Used for LS and schedulers that don't look back
	unsigned long long durationInTS;
	list<AssayNode *> schedule;
	map<unsigned long long, bool> isActiveAtTS; // Used for PathSched and schedulers that DO look back
};

/////////////////////////////////////////////////////////////////
// ResHist Structure: Holds info which details available
// resources for a particular time-step
/////////////////////////////////////////////////////////////////
struct ResHist {
	unsigned long long timeStep;
	int availRes[RES_TYPE_MAX+1];
	int dropsInLoc;
	int dropsInStorage;
};

/////////////////////////////////////////////////////////////////
// DropletDependenceVertex Structure: Used to keep track of
// dependencies...helps detect/free deadlock
/////////////////////////////////////////////////////////////////
struct ModuleDependenceVertex
{
	AssayNode *operation;
	vector<ModuleDependenceVertex *> preds;
	vector<ModuleDependenceVertex *> succs;
	int utilNum; // Number used for priorities, ordering and numbering
	bool alreadyChecked;
	bool inCurrCycle;

	~ModuleDependenceVertex()
	{
		operation = NULL;
		//preds = succs = NULL;
	}
};

#endif
