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
 * Source: scheduler.cc															*
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
#include "../../Headers/Scheduler/scheduler.h"

///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
Scheduler::Scheduler()
{
	dispRes = new vector<IoResource*>();
	outRes = new vector<IoResource*>();
	candidateOps = new list<AssayNode*>();
	unfinishedOps = new list<AssayNode*>();
	executingOps = new list<AssayNode*>();
	internalPriorities = true;
}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
Scheduler::~Scheduler()
{
	while (!dispRes->empty())
	{
		IoResource *dr = dispRes->back();
		dispRes->pop_back();
		delete dr;
	}
	while (!outRes->empty())
	{
		IoResource *oR = outRes->back();
		outRes->pop_back();
		delete oR;
	}

	candidateOps->clear();
	unfinishedOps->clear();
	executingOps->clear();
	delete candidateOps;
	delete unfinishedOps;
	delete executingOps;
	delete dispRes;
	delete outRes;
}

///////////////////////////////////////////////////////////////////////////////////
// Generic schedule function that is inherited/overridden by actual methods.
///////////////////////////////////////////////////////////////////////////////////
unsigned long long Scheduler::schedule(DmfbArch *arch, DAG *dag)
{
	claim(false, "No valid scheduler was selected for the synthesis process or no method for 'schedule()' was implemented for the selected scheduler.\n");
}

/////////////////////////////////////////////////////////////////
// Reveals whether there are still nodes which haven't been
// scheduled
/////////////////////////////////////////////////////////////////
bool Scheduler::moreNodesToSchedule()
{
	return !candidateOps->empty();
}

///////////////////////////////////////////////////////////////
// Adds first DAG nodes to be executed to VLoC so that it can
// run the DAG.  The first nodes that can be added to the
// binding list are those with DISPENSE parents.
///////////////////////////////////////////////////////////////
void Scheduler::commissionDAG(DAG *dag)
{
	for (int i = 0; i < dag->getAllNodes().size(); i++)
	{
		AssayNode *n = dag->getAllNodes().at(i);
		if (n->GetType() != DISPENSE && n->GetType() != OUTPUT)
		{
			bool canBind = true;
			for (int j = 0; j < n->GetParents().size(); j++)
			{
				if (n->GetParents().at(j)->GetType() != DISPENSE)
					canBind = false;
			}

			if (canBind)
				candidateOps->push_back(n);
		}
	}
}

int Scheduler::getAvailResources(DmfbArch *arch)
{
	// Copy resources into local array (Temp, DTG)
	for (int i = 0; i <= RES_TYPE_MAX; i++)
		availRes[i] = arch->getPinMapper()->getAvailResCount()->at(i);

	int numModules = 0;
	for (int i = 0; i <= RES_TYPE_MAX; i++)
		numModules += availRes[i];
	return numModules;
}

///////////////////////////////////////////////////////////////////////////////////
// Resets the resource reservoirs, according to the architecture, so they can be
// used for scheduling.
///////////////////////////////////////////////////////////////////////////////////
void Scheduler::resetIoResources(DmfbArch *arch)
{
	// Now reset dispense/output resources
	while (!dispRes->empty())
	{
		IoResource *r = dispRes->back();
		dispRes->pop_back();
		delete r;
	}
	while (!outRes->empty())
	{
		IoResource *r = outRes->back();
		outRes->pop_back();
		delete r;
	}
	for (int i = 0; i < arch->getIoPorts()->size(); i++)
	{
		IoPort *iop = arch->getIoPorts()->at(i);
		if (iop->isAnInput())
		{
			IoResource *dr = new IoResource();
			dr->port = iop;
			dr->name = iop->getPortName();
			dr->lastEndTS = 0;
			dr->durationInTS = ceil((double)iop->getTimeInSec()/((double)arch->getSecPerTS()));
			dispRes->push_back(dr);
		}
		else
		{
			IoResource *outR = new IoResource();
			outR->port = iop;
			outR->name = iop->getPortName();
			outR->lastEndTS = 0;
			outR->durationInTS = ceil((double)iop->getTimeInSec()/((double)arch->getSecPerTS()));
			outRes->push_back(outR);
		}
	}
}
