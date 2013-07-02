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
 * Source: sort.cc																*
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
#include "../Headers/sort.h"

///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
Sort::Sort() {}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
Sort::~Sort(){}

/////////////////////////////////////////////////////////////////
// Sorts nodes by starting time-step, from least to greatest
/////////////////////////////////////////////////////////////////
bool sNodesByStartTS(AssayNode *a1, AssayNode *a2) { return (a1->GetStartTS() < a2->GetStartTS()); }
bool sNodesByStartThenEndTS(AssayNode *a1, AssayNode *a2)
{
	if (a1->GetStartTS() == a2->GetStartTS())
		return (a1->GetEndTS() < a2->GetEndTS());
	else
		return (a1->GetStartTS() < a2->GetStartTS());
}
bool sPathNodesByStartTS(AssayPathNode *a1, AssayPathNode *a2) { return (a1->startTS < a2->startTS); }
/////////////////////////////////////////////////////////////////
// Sorts nodes by length, from shortest to longest
/////////////////////////////////////////////////////////////////
bool sNodesByLength(AssayNode *a1, AssayNode *a2) { return (a1->GetEndTS() - a1->GetStartTS() < a2->GetEndTS() - a2->GetStartTS()); }
/////////////////////////////////////////////////////////////////
// Sorts integers at derefrenced address in decreasing order
/////////////////////////////////////////////////////////////////
bool sDecreasingInts(int *i1, int *i2) { return ((*i1) > (*i2)); }

/////////////////////////////////////////////////////////////////
// Sorts by starting TS.  If a tie, puts the storage-holders
// first.
/////////////////////////////////////////////////////////////////
bool sNodesByStartTSThenStorageFirst(AssayNode *a1, AssayNode *a2)
{
	if (a1->GetStartTS() == a2->GetStartTS())
	{
		if (a1->GetType() == STORAGE_HOLDER && a2->GetType() != STORAGE_HOLDER)
			return true;
		else
			return false;
	}
	else
		return (a1->GetStartTS() < a2->GetStartTS());
}

/////////////////////////////////////////////////////////////////
// Sorts nodes by reconfig. module, and then by starting time-step,
// from least to greatest
/////////////////////////////////////////////////////////////////
bool sNodesByModuleThenStartTS(AssayNode *a1, AssayNode *a2)
{
	//if (a1->GetReconfigMod() != a2->GetReconfigMod())
	//	return (a1->GetReconfigMod() < a2->GetReconfigMod());
	//else
	//	return (a1->GetStartTS() < a2->GetStartTS());


	//;return true;

	if (a1->GetReconfigMod()->getId() == a2->GetReconfigMod()->getId())
		return (a1->GetStartTS() < a2->GetStartTS());
	else
		return (a1->GetReconfigMod()->getId() < a2->GetReconfigMod()->getId());
}

/////////////////////////////////////////////////////////////////
// Sorts nodes by priority, but puts outputs at the very front
// b/c they can ALWAYS go and free up system resources...not matter
// what their priority is.  HiFirst puts the higher numbers in front,
// while LoFirst puts the lower numbers first
/////////////////////////////////////////////////////////////////
bool sNodesByPriorityHiFirst(AssayNode *a1, AssayNode *a2)
{
	if (a1->GetType() == OUTPUT && a2->GetType() != OUTPUT)
		return true;
	else if (a1->GetType() != OUTPUT && a2->GetType() == OUTPUT)
		return false;
	return (a1->GetPriority() > a2->GetPriority());
}
bool sNodesByPriorityLoFirst(AssayNode *a1, AssayNode *a2)
{
	if (a1->GetType() == OUTPUT && a2->GetType() != OUTPUT)
		return true;
	else if (a1->GetType() != OUTPUT && a2->GetType() == OUTPUT)
		return false;
	return (a1->GetPriority() < a2->GetPriority());
}

///////////////////////////////////////////////////////////////
// Sort heat and detects to front of list b/c they have more
// stringent resource demands and should be processed first
///////////////////////////////////////////////////////////////
bool sNodesByLimitedResources(AssayNode *a1, AssayNode *a2)
{
	if ((a1->GetType() == HEAT || a1->GetType() == DETECT) && !(a2->GetType() == HEAT || a2->GetType() == DETECT))
		return true;
	else
		return false;

}

///////////////////////////////////////////////////////////////
// Shortest id first
///////////////////////////////////////////////////////////////
bool sNodesById(AssayNode *a1, AssayNode *a2) { return (a1->getId() < a2->getId()); }

///////////////////////////////////////////////////////////////
// Longest routes first
///////////////////////////////////////////////////////////////
bool sRoutesByLength(vector<RoutePoint *> *r1, vector<RoutePoint *> *r2)
{
	return r1->size() > r2->size();
}

///////////////////////////////////////////////////////////////
// Latest ending time-steps first, then sort the storage nodes
// to the end.
// *****Changed to:
// If is a storage node and not changing modules from parent
// node's module, then sorted to front
///////////////////////////////////////////////////////////////
bool sNodesByLatestTSThenStorage(AssayNode *a1, AssayNode *a2)
{
	if (a1->GetType() == STORAGE && a2->GetType() != STORAGE)
		return true;
	else if (a1->GetType() != STORAGE && a2->GetType() == STORAGE)
		return false;
	else if (a1->GetType() == STORAGE && a2->GetType() == STORAGE)
	{
		ReconfigModule *rm1 = a1->GetReconfigMod();
		ReconfigModule *rm2 = a2->GetReconfigMod();

		if (rm1->getTY() == rm2->getTY() && rm1->getLX() == rm2->getLX())
			return true;
		else
			return false;
	}
	else // Non-storage nodes
		return false;
	/*if (a1->GetEndTS() != a2->GetEndTS())
		return a1->GetEndTS() > a2->GetEndTS();
	else if (a1->GetType() == STORAGE && a2->GetType() != STORAGE)
		return false;
	else
		return true;*/
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// Wrapper functions for sorts
/////////////////////////////////////////////////////////////////
void Sort::sortNodesByStartTS(list<AssayNode*>* l) { l->sort(sNodesByStartTS); }
void Sort::sortNodesByStartTSThenStorageFirst(list<AssayNode*>* l) { l->sort(sNodesByStartTSThenStorageFirst); }
void Sort::sortNodesByPriorityHiFirst(list<AssayNode*>* l) { l->sort(sNodesByPriorityHiFirst); }
void Sort::sortNodesByPriorityLoFirst(list<AssayNode*>* l) { l->sort(sNodesByPriorityLoFirst); }
void Sort::sortNodesByPriorityLoFirst(vector<AssayNode*>* v) { sort(v->begin(), v->end(), sNodesByPriorityLoFirst); }
void Sort::sortNodesByLimitedResources(list<AssayNode*>* l) { l->sort(sNodesByLimitedResources); }
void Sort::sortNodesById(vector<AssayNode*>* v) { sort(v->begin(), v->end(), sNodesById); }
void Sort::sortNodesByModuleThenStartTS(vector<AssayNode*>* v) { sort(v->begin(), v->end(), sNodesByModuleThenStartTS); }
void Sort::sortNodesByStartThenEndTS(vector<AssayNode *>* v) { sort(v->begin(), v->end(), sNodesByStartThenEndTS); }
void Sort::sortNodesByLatestTSThenStorage(vector<AssayNode *> *v) { sort(v->begin(), v->end(), sNodesByLatestTSThenStorage); }
void Sort::sortPathNodesByStartTS(list<AssayPathNode*>* l) { l->sort(sPathNodesByStartTS); }
void Sort::sortNodesByLength(vector<AssayNode*>* v) { sort(v->begin(), v->end(), sNodesByLength); }
void Sort::sortRoutesByLength(vector<vector<RoutePoint *> *> * v, vector<Droplet *> *vd)
{
	map<vector<RoutePoint *> *, Droplet *> link;
	for (int i = 0; i < v->size(); i++)
		link[v->at(i)] = vd->at(i);
	sort(v->begin(), v->end(), sRoutesByLength);
	vd->clear();
	for (int i = 0; i < v->size(); i++)
		vd->push_back(link[v->at(i)]);
}
void Sort::sortPopBySchedTimes(vector< map<AssayNode*, unsigned> *> *pop, vector<unsigned> * times)
{
	map<unsigned *, map<AssayNode*, unsigned> *> link;
	for (int i = 0; i < pop->size(); i++)
		link[&(times->at(i))] = pop->at(i);

	for (int i = 0; i < times->size(); i++)
		cout << times->at(i) << "(" << pop->at(i) << ")-";
	cout << endl;

	sort(times->begin(), times->end());



	pop->clear();
	for (int i = 0; i < times->size(); i++)
		pop->push_back(link[&(times->at(i))]);

	for (int i = 0; i < times->size(); i++)
		cout << times->at(i) << "(" << pop->at(i) << ")-";
	cout << endl;
	exit(1);
}

///////////////////////////////////////////////////////////////////////////////////////
// Sorts routingThisTS in decreasing order, based on the manhattan distance between
// the corresponding source and target cells.
///////////////////////////////////////////////////////////////////////////////////////
void Sort::sortDropletsInDecManhattanDist(vector<Droplet *> *routingThisTS, map<Droplet *, SoukupCell *> *sourceCells, map<Droplet *, SoukupCell *> *targetCells)
{
	vector<int *> distances;
	map<int *, Droplet *> link;
	for (int i = 0; i < routingThisTS->size(); i++)
	{
		SoukupCell *s = sourceCells->at(routingThisTS->at(i));
		SoukupCell *t = targetCells->at(routingThisTS->at(i));
		int *manhattanDist = new int();
		*manhattanDist = abs(s->x - t->x) + abs(s->y - t->y);
		distances.push_back(manhattanDist);
		link[manhattanDist] = routingThisTS->at(i);
	}

	//for (int i = 0; i < routingThisTS->size(); i++)
	//	cout << "D" << routingThisTS->at(i)->getId() << ": " << *distances.at(i) << endl;

	sort(distances.begin(), distances.end(), sDecreasingInts);

	routingThisTS->clear();
	for (int i = 0; i < distances.size(); i++)
		routingThisTS->push_back(link[distances.at(i)]);

	//for (int i = 0; i < routingThisTS->size(); i++)
	//	cout << "D" << routingThisTS->at(i)->getId() << ": " << *distances.at(i) << endl;


	while (!distances.empty())
	{
		int *i = distances.back();
		distances.pop_back();
		delete i;
	}
}
