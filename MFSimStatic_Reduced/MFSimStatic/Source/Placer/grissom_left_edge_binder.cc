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
 * Source: grissom_left_edge_binder.cc												*
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
#include "../../Headers/Placer/grissom_left_edge_binder.h"

///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
GrissomLEBinder::GrissomLEBinder()
{
}

///////////////////////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////////////////////
GrissomLEBinder::~GrissomLEBinder()
{
}

///////////////////////////////////////////////////////////////////////////////////
// Places the scheduled DAG (contained by synthesis) according to the demo
// placement algorithm.

// This algorithm works more like a binder as it assumes that the module locations
// are essentially fixed each time-cycle. Thus, the placer is really just binding
// scheduled operations to available work areas.
///////////////////////////////////////////////////////////////////////////////////
void GrissomLEBinder::place(DmfbArch *arch, DAG *schedDag, vector<ReconfigModule *> *rModules)
{
	getAvailResources(arch);
	resetIoResources(arch);

	int maxStoragePerModule = getMaxStoragePerModule();

	int INPUT_RES = RES_TYPE_MAX + 1;
	int OUTPUT_RES = RES_TYPE_MAX + 2;

	vector< list<AssayNode*> * > opsByResType; // Operations distinguished by operation types
	list<AssayNode*> storeList;
	for (int i = 0; i <= RES_TYPE_MAX + 2; i++)// +2 for inputs and outputs
		opsByResType.push_back(new list<AssayNode*>());
	for (int i = 0; i < schedDag->getAllNodes().size(); i++)
	{
		AssayNode *n = schedDag->getAllNodes().at(i);
		if (n->GetType() == DISPENSE)
			opsByResType.at(INPUT_RES)->push_back(n);
		else if (n->GetType() == OUTPUT)
			opsByResType.at(OUTPUT_RES)->push_back(n);
		else if (n->GetType() != STORAGE)
			opsByResType.at(n->boundedResType)->push_back(n);
		else if (n->GetType() == STORAGE)
			storeList.push_back(n);
	}
	for (int i = 0; i < schedDag->getAllStorageHolders().size(); i++)
	{
		AssayNode *n = schedDag->getAllStorageHolders().at(i);
		opsByResType.at(n->boundedResType)->push_back(n);
	}
	for (int i = 0; i < opsByResType.size(); i++)
		Sort::sortNodesByStartTSThenStorageFirst(opsByResType.at(i));
		//Sort::sortNodesByStartTS(opsByResType.at(i));


	/*for (list<AssayNode *>::iterator it = opsByResType.at(D_RES)->begin(); it != opsByResType.at(D_RES)->end(); it++)
	{
		AssayNode *n = (*it);
		if ((*it)->type == STORAGE_HOLDER)
			cout << (*it)->startTimeStep << " S" << endl;
		else
			cout << (*it)->startTimeStep << " O" << endl;
	}*/


	//for (int i = 0; i < schedDag->storage.size(); i++)
	//	storeList.push_back(schedDag->storage.at(i));

	// Do Left-Edge binding for modules
	map<FixedModule *, vector<AssayNode *> *> modSchedule;
	for (int i = 0; i <= RES_TYPE_MAX; i++)
	{
		ResourceType rt = (ResourceType)i;
		for (int j = 0; j < availRes[i].size(); j++)
		{
			vector<AssayNode *> * sched = new vector<AssayNode *>();
			FixedModule *fm = availRes[i].at(j);
			modSchedule[fm] = sched;

			unsigned long long lastEnd = 0;
			list<AssayNode *> scheduled;
			list<AssayNode *>::iterator it = opsByResType.at(rt)->begin();
			for (; it != opsByResType.at(rt)->end(); it++)
			{
				AssayNode *n = *it;
				if (n->GetStartTS() >= lastEnd)
				{
					sched->push_back(n);
					scheduled.push_back(n);
					lastEnd = n->GetEndTS();
					n->status = BOUND;
					n->reconfigMod = new ReconfigModule(fm->getResourceType(), fm->getLX(), fm->getTY(), fm->getRX(), fm->getBY());
					n->reconfigMod->startTimeStep = n->startTimeStep;
					n->reconfigMod->endTimeStep = n->endTimeStep;
					n->reconfigMod->boundNode = n;
					n->reconfigMod->tiledNum = fm->getTileNum();
					rModules->push_back(n->reconfigMod);
				}
			}
			while (!scheduled.empty())
			{
				opsByResType.at(rt)->remove(scheduled.front());
				scheduled.pop_front();
			}
		}
	}

	// Sort the lists and copy
	Sort::sortNodesByModuleThenStartTS(&(schedDag->storageHolders)); // This may be unnecessary
	Sort::sortNodesByStartTS(&storeList);
	list<AssayNode*> holdList;
	for (int i = 0; i < schedDag->storageHolders.size(); i++)
		holdList.push_back(schedDag->storageHolders.at(i));
	//Sort::sortNodesByStartTS(&holdList);

	//for (list<AssayNode *>::iterator it = holdList.begin(); it != holdList.end(); it++)
	//	cout << "M (" << (*it)->reconfigMod->getLX() << ", " << (*it)->reconfigMod->getTY() << ")\tStart: " << (*it)->startTimeStep << "-" << (*it)->endTimeStep << endl;


	// Do Left-Edge binding of individual storage nodes into storage-holder scheduled nodes
	// Assumes scheduler will always produce a list of storage holders of 1 TS in length
	while (!storeList.empty())
	{
		//cout << "Num Storage: " << storeList.size() << endl; // DTG Debug

		AssayNode *sNode = storeList.front();
		storeList.pop_front();

		unsigned long long runningEnd = 0;
		bool scheduled = false;
		bool split = false;
		list<AssayNode *> holdersFull;
		list<AssayNode *>::iterator it = holdList.begin();
		for(; it != holdList.end(); it++)
		{

			//cout << "Num Storage_Holders: " << holdList.size() << endl; // DTG Debug

			AssayNode *sHolder = *it;
			if (sNode->GetStatus() != BOUND)
			{
				if (sHolder->GetStartTS() == sNode->GetStartTS()
						//&& sHolder->GetEndTS() <= sNode->GetEndTS()
						&& sHolder->storageOps.size() < maxStoragePerModule)
				{
					sNode->status = BOUND;
					sNode->reconfigMod = sHolder->reconfigMod;
					sNode->boundedResType = sHolder->boundedResType;
					sHolder->storageOps.push_back(sNode);
					sHolder->numDrops++;
					runningEnd = sHolder->endTimeStep;
					scheduled = true;
					if (sHolder->storageOps.size() >= maxStoragePerModule)
						holdersFull.push_back(sHolder);
				}
			}
			else if (scheduled) /*sNode->status == BOUND*/
			{
				if (sHolder->GetStartTS() == runningEnd
						&& sHolder->storageOps.size() < maxStoragePerModule
						&& sHolder->reconfigMod == sNode->reconfigMod)
				{
					sHolder->storageOps.push_back(sNode);
					sHolder->numDrops++;
					runningEnd = sHolder->endTimeStep;
					if (sHolder->storageOps.size() >= maxStoragePerModule)
						holdersFull.push_back(sHolder);
				}
				else
					split = true;
			}

			// If we're at the end of the list and node hasn't been fully sched, split
			it++;
			if (sNode->endTimeStep != runningEnd && it == holdList.end())
				split = true;
			it--;

			if (split)
			{
				AssayNode *storeSecPart = schedDag->AddStorageNode();
				storeSecPart->startTimeStep = runningEnd;
				storeSecPart->endTimeStep = sNode->endTimeStep;
				sNode->endTimeStep = runningEnd;
				storeSecPart->status = SCHEDULED;
				schedDag->InsertNode(sNode, sNode->children.front(), storeSecPart);
				storeList.remove(sNode);
				storeList.push_front(storeSecPart);
				break;
			}
			if (runningEnd == sNode->endTimeStep)
			{
				storeList.remove(sNode);
				break;
			}
		}
		while (!holdersFull.empty())
		{
			holdList.remove(holdersFull.front());
			holdersFull.pop_front();
		}
	}

	// Now do Left-Edge binding for inputs
	for (int i = 0; i < dispRes->size(); i++)
	{
		unsigned long long lastEnd = 0;
		list<AssayNode *> scheduled;
		IoResource *dr = dispRes->at(i);
		list<AssayNode *>::iterator it = opsByResType.at(INPUT_RES)->begin();
		for (; it != opsByResType.at(INPUT_RES)->end(); it++)
		{
			AssayNode *n = *it;
			if (n->GetPortName() == dr->name && n->GetStartTS() >= lastEnd)
			{
				dr->schedule.push_back(n);
				scheduled.push_back(n);
				lastEnd = n->GetEndTS();
				n->status = BOUND;
				n->ioPort = dr->port;
			}
		}
		while (!scheduled.empty())
		{
			opsByResType.at(INPUT_RES)->remove(scheduled.front());
			scheduled.pop_front();
		}
	}

	// Now do binding for outputs...timings don't really matter for outputs
	for (int i = 0; i < outRes->size(); i++)
	{
		list<AssayNode *> scheduled;
		IoResource *outR = outRes->at(i);
		list<AssayNode *>::iterator it = opsByResType.at(OUTPUT_RES)->begin();
		for (; it != opsByResType.at(OUTPUT_RES)->end(); it++)
		{
			AssayNode *n = *it;
			if (n->GetPortName() == outR->name)
			{
				outR->schedule.push_back(n);
				scheduled.push_back(n);
				n->status = BOUND;
				n->ioPort = outR->port;
			}
		}
		while (!scheduled.empty())
		{
			opsByResType.at(OUTPUT_RES)->remove(scheduled.front());
			scheduled.pop_front();
		}
	}

	{	// Sanity check: All nodes should be bound by now
		stringstream msg;
		msg << "ERROR. All nodes were not bound during Left-Edge Bind (Grissom Fixed Placer)...there is probably a problem with the schedule." << ends;
		bool allBound = true;
		for (int i = 0; i < schedDag->allNodes.size(); i++)
		{
			if (schedDag->allNodes.at(i)->GetStatus() != BOUND)
			{
				cout << "Unbound Node: ";
				schedDag->allNodes.at(i)->Print(); // Debugging
				cout << endl;
				allBound = false;
			}
		}
		claim(allBound, &msg);
	}

	// Cleanup
	while (!opsByResType.empty())
	{
		list<AssayNode*> *l = opsByResType.back();
		opsByResType.pop_back();
		delete l;
	}
	map<FixedModule *, vector<AssayNode *> *>::iterator it = modSchedule.begin();
	for (; it != modSchedule.end(); it++)
		delete it->second;
	modSchedule.clear();

}
