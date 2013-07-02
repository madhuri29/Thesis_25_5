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
 * Source: post_subprob_compact_router.cc										*
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
#include "../../Headers/Router/post_subprob_compact_router.h"

///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
PostSubproblemCompactionRouter::PostSubproblemCompactionRouter()
{
	thisTS = new vector<AssayNode *>();
}
PostSubproblemCompactionRouter::PostSubproblemCompactionRouter(DmfbArch *dmfbArch)
{
	arch = dmfbArch;
	thisTS = new vector<AssayNode *>();
}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
PostSubproblemCompactionRouter::~PostSubproblemCompactionRouter()
{
	///////////////////////////////////////////////////////////////////////
	// More clean-up; Free the arrays being used to keep track of cell
	// types and blockages
	///////////////////////////////////////////////////////////////////////
	if (cellType)
	{
		while (!cellType->empty())
		{
			vector<ResourceType> * col = cellType->back();
			cellType->pop_back();
			col->clear();
			delete col;
		}
		delete cellType;
	}

	if (thisTS)
	{
		thisTS->clear();
		delete thisTS;
	}
}

///////////////////////////////////////////////////////////////////////
// This function performs any one-time initializations that the router
// needs that are specific to a particular router.
///////////////////////////////////////////////////////////////////////
void PostSubproblemCompactionRouter::routerSpecificInits()
{
}

///////////////////////////////////////////////////////////////////////
// Initializes the array that is used by the processing engine to
// determine if a cell is equipped with a heater/detector.
///////////////////////////////////////////////////////////////////////
void PostSubproblemCompactionRouter::initCellTypeArray()
{
	// Create a 2D-array which tells if a cell is augmented with a heater or detector
	cellType = new vector<vector<ResourceType> *>();
	for (int x = 0; x < arch->getNumCellsX(); x++)
	{
		vector<ResourceType> *typeCol = new vector<ResourceType>();
		for (int y = 0; y < arch->getNumCellsY(); y++)
			typeCol->push_back(BASIC_RES);
		cellType->push_back(typeCol);
	}

	for (int i = 0; i < arch->getExternalResources()->size(); i++)
	{
		FixedModule *fm = arch->getExternalResources()->at(i);
		for (int x = fm->getLX(); x <= fm->getRX(); x++)
		{
			for (int y = fm->getTY(); y <= fm->getBY(); y++)
			{
				if (fm->getResourceType() == H_RES && cellType->at(x)->at(y) == D_RES)
					cellType->at(x)->at(y) = DH_RES;
				else if (fm->getResourceType() == H_RES)
					cellType->at(x)->at(y) = H_RES;
				else if (fm->getResourceType() == D_RES && cellType->at(x)->at(y) == H_RES)
					cellType->at(x)->at(y) = DH_RES;
				else if (fm->getResourceType() == D_RES)
					cellType->at(x)->at(y) = D_RES;
				else
					claim(false, "Unsupported cell-augmentation.");
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////
// After droplets are compacted, some droplets will get to their destinations
// first, at which point the router stops adding routing points. Thus,
// these droplets will disappear in the output b/c they finished first.
// Each droplet must be accounted for each cycle, so this function
// examines the global routes and adds routing points to any droplets that
// reached their destinations earlier than the latest droplet so the droplet
// is accounted for each cycle. Droplets that are "caught up" or "equalized"
// simply remain in their last known position. If a droplet was just
// output, then we ensure here that it has been marked properly so
// it will disappear from the DMFB array.
///////////////////////////////////////////////////////////////////////
void PostSubproblemCompactionRouter::equalizeGlobalRoutes(map<Droplet *, vector<RoutePoint *> *> *routes)
{
	//for (int i = 0; i < thisTS->size(); i++)
	//	if (thisTS->at(i)->GetType() == OUTPUT)
	//		(*routes)[thisTS->at(i)->droplets.front()]->back()->dStatus = DROP_OUTPUT;
	for (int i = 0; i < thisTS->size(); i++)
		if (thisTS->at(i)->GetType() == OUTPUT)
			if (thisTS->at(i)->droplets.size() > 0)
				(*routes)[thisTS->at(i)->droplets.front()]->back()->dStatus = DROP_OUTPUT;

	map<Droplet *, vector<RoutePoint *> *>::iterator dropIt = routes->begin();
	for (; dropIt != routes->end(); dropIt++)
	{
		if (!dropIt->second->empty())
		{
			RoutePoint *lrp = dropIt->second->back();
			if (!(lrp->dStatus == DROP_OUTPUT || lrp->dStatus == DROP_CONSUMED))
			{
				for (int c = lrp->cycle+1; c < routeCycle; c++)
				{
					RoutePoint *nrp = new RoutePoint();
					nrp->cycle = c;
					nrp->dStatus = DROP_WAIT;
					nrp->x = lrp->x;
					nrp->y = lrp->y;
					(*routes)[dropIt->first]->push_back(nrp);
					lrp = nrp;
				}
			}
		}
	}
	cycle = routeCycle;
}

///////////////////////////////////////////////////////////////////////
// This function handles the processing of a time-step.  Most routing
// methods just compute droplet routes from one I/O or module to the
// next; however, this is really incomplete b/c the droplet must be
// accounted for and, in a sense, "routed" (or "processed") inside the
// module.
//
// Thus, this function calls a method to process the time-step so that
// we can generate a COMPLETE simulation such that the results could
// be used to run an actual assay on a DMFB...ensuring that a droplet's
// exact location is accounted for during each cycle.
///////////////////////////////////////////////////////////////////////
void PostSubproblemCompactionRouter::processTimeStep(map<Droplet *, vector<RoutePoint *> *> *routes)
{
	if (processEngineType == FIXED_QUICK_PE)
		processFixPlaceTSQuick(routes);
	else if (processEngineType == FIXED_FULL_PE)
		processFixPlaceTSFull(routes);
	else { /* Do not process - will probably result in some kind of errors */ }

	startingTS++; // Time-step is complete; Advance to the next time-step
	cycle += cyclesPerTS; // Advance cycle number by whole time-step
}


///////////////////////////////////////////////////////////////////////
// Handles the processing of droplets inside a module during a time-
// step (TS). This is the quick version, which does not actually draw
// droplets moving around, but rather, computes the location the droplet
// will be in the module when the time-step is complete and places it
// (them there).
//
// If this function is used, the output of the simulation cannot be
// used to run an actual assay b/c there is no droplet location info
// during the time-steps. This method is provided for a fair comparison
// to past methods b/c past methods did not perform this processing
// in their routing calculations.
//
// For more information on how droplets are placed within the modules,
// see the description of "processFixPlaceTSFull()" b/c the final
// positions of these two processing engines are the same (this function
// just doesn't draw the droplets moving to the final position in the
// module).
///////////////////////////////////////////////////////////////////////
void PostSubproblemCompactionRouter::processFixPlaceTSQuick(map<Droplet *, vector<RoutePoint *> *> *routes)
{
	///////////////////////////////////////////////////////////////////
	// This is the QUICK version which puts the droplets directly
	// in their last cell for processing.
	///////////////////////////////////////////////////////////////////
	RoutePoint *nrp = NULL;
	RoutePoint *lrp = NULL;

	for (int i = 0; i < thisTS->size(); i++)
	{
		AssayNode *n = thisTS->at(i);
		if (!(n->GetType() == DISPENSE || n->GetType() == OUTPUT))
		{
			if (startingTS != n->startTimeStep)
			{
				for (int j = 0; j < n->GetDroplets().size(); j++)
				{
					lrp = (*routes)[n->GetDroplets().at(j)]->back();
					lrp->cycle = cycle + cyclesPerTS - 1;
				}
			}
			else
			{
				bool stayingInModule = false;

				if (n->children.front()->reconfigMod && n->reconfigMod->getLX() == n->children.front()->reconfigMod->getLX() && n->reconfigMod->getTY() == n->children.front()->reconfigMod->getTY())
					stayingInModule = true;

				// Here, figure out what kind of node it is and then move the droplets around as necessary
				if (n->GetType() == MIX)
				{
					// Merge the droplets into one droplet
					Droplet *drop = n->GetDroplets().back();
					n->droplets.pop_back();
					while(!n->GetDroplets().empty())
					{
						Droplet *d2 = n->droplets.back();
						if (d2->id < drop->id)
						{
							d2->volume += drop->volume;
							d2->uniqueFluidName += ("+" + drop->uniqueFluidName);
							(*routes)[drop]->back()->dStatus = DROP_CONSUMED;
							drop = d2;
						}
						else
						{
							drop->volume += n->GetDroplets().back()->volume;
							drop->uniqueFluidName += ( "+" + n->GetDroplets().back()->uniqueFluidName);
							(*routes)[d2]->back()->dStatus = DROP_CONSUMED;
						}
						n->droplets.pop_back();
					}
					n->droplets.push_back(drop);

					ReconfigModule *rm = n->GetReconfigMod();
					nrp = new RoutePoint(); // This route point
					nrp->dStatus = DROP_PROCESSING;
					nrp->cycle = cycle + cyclesPerTS - 1;
					if (stayingInModule)
					{
						nrp->x = rm->getLX();
						nrp->y = rm->getBY();
					}
					else
					{
						nrp->x = rm->getRX();
						nrp->y = rm->getTY();
					}
					(*routes)[drop]->push_back(nrp);
				}
				else if (n->GetType() == HEAT || n->GetType() == DETECT)
				{
					Droplet *drop = n->GetDroplets().back();
					ReconfigModule *rm = n->GetReconfigMod();
					nrp = new RoutePoint(); // This route point
					nrp->dStatus = DROP_PROCESSING;
					nrp->cycle = cycle + cyclesPerTS - 1;
					if (stayingInModule)
					{
						nrp->x = rm->getLX();
						nrp->y = rm->getBY();
					}
					else
					{
						nrp->x = rm->getRX();
						nrp->y = rm->getTY();
					}
					(*routes)[drop]->push_back(nrp);
				}
				else if (n->GetType() == STORAGE)
				{
					Droplet *drop = n->GetDroplets().back();
					ReconfigModule *rm = n->GetReconfigMod();
					lrp = (*routes)[drop]->back(); // Last route point

					nrp = new RoutePoint(); // next route point
					nrp->cycle = cycle + cyclesPerTS - 1;
					nrp->dStatus = DROP_PROCESSING;

					// If leaving module after TS, move to right and store, else keep on left
					if (!stayingInModule)
					{
						nrp->x = rm->getRX();
						nrp->y = lrp->y;
					}
					else if (n->children.front()->type != STORAGE) // DTG Debug 2/6
					{
						nrp->x = rm->getLX();
						nrp->y = rm->getBY();
					}
					else
					{
						nrp->x = lrp->x;
						nrp->y = lrp->y;
					}
					(*routes)[drop]->push_back(nrp);
				}
				else if (n->GetType() == SPLIT)
				{
					// Only split at beginning of TS
					ReconfigModule *rm = n->GetReconfigMod();
					Droplet *drop = n->GetDroplets().back();
					Droplet *drop2 = new Droplet();
					drop2->uniqueFluidName = drop->uniqueFluidName;
					drop2->volume = drop->volume / 2;
					(*routes)[drop2] = new vector<RoutePoint *>();
					n->addDroplet(drop2);

					nrp = new RoutePoint();
					nrp->x = rm->getRX();
					nrp->y = rm->getTY();
					nrp->dStatus = DROP_PROCESSING;
					nrp->cycle = cycle + cyclesPerTS - 1;
					(*routes)[drop2]->push_back(nrp);

					nrp = new RoutePoint();
					nrp->x = rm->getRX();
					nrp->y = rm->getBY();
					nrp->dStatus = DROP_PROCESSING;
					nrp->cycle = cycle + cyclesPerTS - 1;
					(*routes)[drop]->push_back(nrp);
				}
				else
					claim(false, "Unsupported opearation encountered while routing.");
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////
// Handles the processing of droplets inside a module during a time-
// step (TS). This is the long version, which draws droplets moving
// around the module during the TS. A full processing (as opposed to
// a quick processing) is required to run an actual assay as the droplet
// locations inside the module cannot be ignored in real life.
//
// This function assumes several things.  It assumes the module is at
// least 3 cells tall and 3 cells wide.
//
// For mixes, it essentially causes a droplet to travel around the
// perimeter, clock-wise, until a few cycles before the end of the
// time-step, at which point it will stop at one of the deemed exit-cells
// in the top-right or bottom-right cells of the module.
//
// Heats/detects will travel clock-wise around the perimeter until they
// find a cell equipped with a heater/detector; if one of these devices
// is not on the perimeter (but rather in the center of the module),
// it will not be found. A few cycles before the end of the time-step,
// the droplet will move to one of the deemed exit-cells in the top-right
// or bottom-right.
//
// Splits, will split the droplet into two and then move the two droplets
// into one of the deemed exit-cells in the top- or bottom- right cells
// in the module
//
// Storage go directly to the exit-cells in the top- or bottom- right
// cells of the module.
//
// ONE EXCEPTION that applies to all of the descriptions above: if a
// droplet's next destination is the same module location as the
// current module it is in, it will not position itself in one of the
// exit-cells on the right, as mentioned.  INSTEAD, it will position
// itself in one of the entry cells in the top- or bottom- LEFT cells
// of the module.
///////////////////////////////////////////////////////////////////////
void PostSubproblemCompactionRouter::processFixPlaceTSFull(map<Droplet *, vector<RoutePoint *> *> *routes)
{
	///////////////////////////////////////////////////////////////////
	// LONG version, actually moves the droplets around in the chambers.
	// Should not be enabled when doing route timings as no other works
	// include the processing time in route construction.
	///////////////////////////////////////////////////////////////////
	RoutePoint *nrp = NULL;
	RoutePoint *lrp = NULL;
	for (int i = 0; i < thisTS->size(); i++)
	{
		AssayNode *n = thisTS->at(i);

		if (!(n->GetType() == DISPENSE || n->GetType() == OUTPUT))
		{
			unsigned long long tsCycle = cycle;

			bool stayingInModule = false;

			if (n->children.front()->reconfigMod && n->reconfigMod->getLX() == n->children.front()->reconfigMod->getLX() && n->reconfigMod->getTY() == n->children.front()->reconfigMod->getTY())
				stayingInModule = true;

			// Here, figure out what kind of node it is and then move the droplets around as necessary
			if (n->GetType() == MIX)
			{
				// Merge the droplets into one droplet
				Droplet *drop = n->GetDroplets().back();
				n->droplets.pop_back();

				while(!n->GetDroplets().empty())
				{
					Droplet *d2 = n->droplets.back();
					if (d2->id < drop->id)
					{
						d2->volume += drop->volume;
						d2->uniqueFluidName += ("+" + drop->uniqueFluidName);
						(*routes)[drop]->back()->dStatus = DROP_CONSUMED;
						drop = d2;
					}
					else
					{
						drop->volume += n->GetDroplets().back()->volume;
						drop->uniqueFluidName += ( "+" + n->GetDroplets().back()->uniqueFluidName);
						(*routes)[d2]->back()->dStatus = DROP_CONSUMED;
					}
					n->droplets.pop_back();
				}
				n->droplets.push_back(drop);

				// Compute whether to leave out of top or bottom exit
				ReconfigModule *rm = n->GetReconfigMod();
				int exitY; // Tells whether droplet is exiting from the TOP-right or BOTTOM-right
				AssayNode *child = n->children.front();
				if (child->GetType() == OUTPUT)
				{
					if (child->GetIoPort()->getSide() == NORTH)
						exitY = rm->getTY();
					else if (child->GetIoPort()->getSide() == SOUTH)
						exitY = rm->getBY();
					else
					{
						if (child->GetIoPort()->getPosXY() <= rm->getTY()+1)
							exitY = rm->getTY();
						else
							exitY = rm->getBY();
					}
				}
				else
				{
					ReconfigModule *nRm = n->children.front()->GetReconfigMod(); // Next module
					if (nRm->getTY() < rm->getTY())
						exitY = rm->getTY();
					else
						exitY = rm->getBY();
				}

				lrp = (*routes)[drop]->back(); // Last route point
				while (tsCycle < cycle + cyclesPerTS)
				{
					ReconfigModule *rm = n->GetReconfigMod();
					nrp = new RoutePoint(); // This route point
					nrp->dStatus = DROP_PROCESSING;
					int numCellsInCirc = rm->getNumCellsInCirc();
					if (stayingInModule && (lrp->x == rm->getLX() && lrp->y == rm->getBY()) && (tsCycle + numCellsInCirc > cycle + cyclesPerTS) && startingTS+1 == n->endTimeStep )
					{	// Cannot make another full rotation before TS is done so just stay here in BL and take position for next TS
						nrp->x = lrp->x;
						nrp->y = lrp->y;
					}
					else if (!stayingInModule && (lrp->x == rm->getRX() && lrp->y == exitY) && (tsCycle + numCellsInCirc > cycle + cyclesPerTS) && startingTS+1 == n->endTimeStep )
					{	// Cannot make another full rotation before TS is done so just stay here in TR and take position to leave module
						nrp->x = lrp->x;
						nrp->y = lrp->y;
					}
					else
					{	// ***This won't work on linear arrays
						if (lrp->x == rm->getLX()) // If in left column
						{
							if (lrp->y > rm->getTY()) // Go north if not at north border
							{
								nrp->x = lrp->x;
								nrp->y = lrp->y-1;
							}
							else // Else turn toward east
							{
								nrp->x = lrp->x+1;
								nrp->y = lrp->y;
							}
						}
						else if (lrp->y == rm->getTY()) // If in top row
						{
							if (lrp->x < rm->getRX()) // Go east if not at east border
							{
								nrp->x = lrp->x+1;
								nrp->y = lrp->y;
							}
							else // Else turn toward south
							{
								nrp->x = lrp->x;
								nrp->y = lrp->y+1;
							}
						}
						else if (lrp->x == rm->getRX()) // If in right column
						{
							if (lrp->y < rm->getBY()) // Go south if not at south border
							{
								nrp->x = lrp->x;
								nrp->y = lrp->y+1;
							}
							else // Else turn toward west
							{
								nrp->x = lrp->x-1;
								nrp->y = lrp->y;
							}
						}
						else if (lrp->y == rm->getBY()) // If in bottom row
						{
							if (lrp->x > rm->getLX()) // Go west if not at left border
							{
								nrp->x = lrp->x-1;
								nrp->y = lrp->y;
							}
							else // Else toward north
							{
								nrp->x = lrp->x;
								nrp->y = lrp->y-1;
							}
						}
						else
							claim(false, "Unhandled processing decision for router.");
					}
					nrp->cycle = tsCycle++;
					(*routes)[drop]->push_back(nrp);
					lrp = nrp;
				}
			}
			else if (n->GetType() == HEAT || n->GetType() == DETECT)
			{
				Droplet *drop = n->GetDroplets().back();
				lrp = (*routes)[drop]->back(); // Last route point
				ResourceType rt;
				if (n->GetType() == HEAT)
					rt = H_RES;
				else
					rt = D_RES;

				while (tsCycle < cycle + cyclesPerTS)
				{
					// Compute whether to leave out of top or bottom exit
					ReconfigModule *rm = n->GetReconfigMod();
					int exitY; // Tells whether droplet is exiting from the TOP-right or BOTTOM-right
					AssayNode *child = n->children.front();
					if (child->GetType() == OUTPUT)
					{
						if (child->GetIoPort()->getSide() == NORTH)
							exitY = rm->getTY();
						else if (child->GetIoPort()->getSide() == SOUTH)
							exitY = rm->getBY();
						else
						{
							if (child->GetIoPort()->getPosXY() <= rm->getTY()+1)
								exitY = rm->getTY();
							else
								exitY = rm->getBY();
						}
					}
					else
					{
						ReconfigModule *nRm = n->children.front()->GetReconfigMod(); // Next module
						if (nRm->getTY() < rm->getTY())
							exitY = rm->getTY();
						else
							exitY = rm->getBY();
					}

					nrp = new RoutePoint(); // This route point
					nrp->dStatus = DROP_PROCESSING;
					int numCellsInCirc = rm->getNumCellsInCirc();
					if (((cellType->at(lrp->x)->at(lrp->y) == rt || cellType->at(lrp->x)->at(lrp->y) == DH_RES) && (startingTS+1 != n->endTimeStep || (startingTS+1 == n->endTimeStep && (tsCycle + numCellsInCirc < cycle + cyclesPerTS))))
							|| (stayingInModule && (lrp->x == rm->getLX() && lrp->y == rm->getBY()) && (tsCycle + numCellsInCirc > cycle + cyclesPerTS) && startingTS+1 == n->endTimeStep)
							|| (!stayingInModule && (lrp->x == rm->getRX() && lrp->y == exitY) && (tsCycle + numCellsInCirc > cycle + cyclesPerTS) && startingTS+1 == n->endTimeStep) )
					{	// Cannot make another full rotation before TS is done so just stay here
						nrp->x = lrp->x;
						nrp->y = lrp->y;
					}
					else
					{	// ***This won't work on linear arrays
						if (lrp->x == rm->getLX()) // If in left column
						{
							if (lrp->y > rm->getTY()) // Go north if not at north border
							{
								nrp->x = lrp->x;
								nrp->y = lrp->y-1;
							}
							else // Else turn toward east
							{
								nrp->x = lrp->x+1;
								nrp->y = lrp->y;
							}
						}
						else if (lrp->y == rm->getTY()) // If in top row
						{
							if (lrp->x < rm->getRX()) // Go east if not at east border
							{
								nrp->x = lrp->x+1;
								nrp->y = lrp->y;
							}
							else // Else turn toward south
							{
								nrp->x = lrp->x;
								nrp->y = lrp->y+1;
							}
						}
						else if (lrp->x == rm->getRX()) // If in right column
						{
							if (lrp->y < rm->getBY()) // Go south if not at south border
							{
								nrp->x = lrp->x;
								nrp->y = lrp->y+1;
							}
							else // Else turn toward west
							{
								nrp->x = lrp->x-1;
								nrp->y = lrp->y;
							}
						}
						else if (lrp->y == rm->getBY()) // If in bottom row
						{
							if (lrp->x > rm->getLX()) // Go west if not at left border
							{
								nrp->x = lrp->x-1;
								nrp->y = lrp->y;
							}
							else // Else toward north
							{
								nrp->x = lrp->x;
								nrp->y = lrp->y-1;
							}
						}
						else
							claim(false, "Unhandled processing decision for router.");
					}
					nrp->cycle = tsCycle++;
					(*routes)[drop]->push_back(nrp);
					lrp = nrp;
				}
			}
			else if (n->GetType() == STORAGE)
			{
				Droplet *drop = n->GetDroplets().back();
				lrp = (*routes)[drop]->back(); // Last route point

				while (tsCycle < cycle + cyclesPerTS)
				{
					nrp = new RoutePoint(); // next route point
					nrp->cycle = tsCycle++;
					nrp->dStatus = DROP_PROCESSING;

					// If leaving module after TS, move to right and store, else keep on left
					if (!stayingInModule && lrp->x != n->reconfigMod->getRX())
					{
						nrp->x = lrp->x+1;
						nrp->y = lrp->y;
					}
					else if (stayingInModule && n->children.front()->type != STORAGE && lrp->y != n->reconfigMod->getBY() && tsCycle > cycle+2)
					{
						nrp->x = lrp->x;
						nrp->y = lrp->y+1;
					}
					else
					{
						nrp->x = lrp->x;
						nrp->y = lrp->y;
					}
					// DTG Debug
					/*if (lrp->x != n->reconfigMod->getRX())
					{
						nrp->x = lrp->x+1;
						nrp->y = lrp->y;
					}
					else
					{
						nrp->x = lrp->x;
						nrp->y = lrp->y;
					}*/

					(*routes)[drop]->push_back(nrp);
					lrp = nrp;
				}
			}
			else if (n->GetType() == SPLIT)
			{
				// Only split at beginning of TS
				if (n->startTimeStep == startingTS)
				{
					Droplet *drop = n->GetDroplets().back();
					Droplet *drop2 = new Droplet();
					drop2->uniqueFluidName = drop->uniqueFluidName;
					drop2->volume = drop->volume / 2;
					(*routes)[drop2] = new vector<RoutePoint *>();
					n->addDroplet(drop2);
					lrp = (*routes)[drop]->back();

					for (int yPos = lrp->y-1; yPos >= n->reconfigMod->getTY(); yPos--)
					{
						nrp = new RoutePoint();
						nrp->x = lrp->x;
						nrp->y = lrp->y;
						nrp->dStatus = DROP_PROCESSING;
						nrp->cycle = tsCycle;
						(*routes)[drop]->push_back(nrp);
						nrp = new RoutePoint();
						nrp->x = lrp->x;
						nrp->y = yPos;
						nrp->dStatus = DROP_PROCESSING;
						nrp->cycle = tsCycle++;
						(*routes)[drop2]->push_back(nrp);
					}
				}
				unsigned long long splitCycle;
				for (int d = 0; d < n->droplets.size(); d++)
				{
					Droplet *drop = n->droplets.at(d);
					lrp = (*routes)[drop]->back();
					splitCycle = tsCycle;
					while (splitCycle < cycle + cyclesPerTS)
					{
						// Move to right
						if (lrp->x != n->reconfigMod->getRX())
						{
							nrp = new RoutePoint(); // next route point
							nrp->cycle = splitCycle++;
							nrp->dStatus = DROP_NORMAL;
							nrp->x = lrp->x+1;
							nrp->y = lrp->y;
							(*routes)[drop]->push_back(nrp);
							lrp = nrp;
						}
						else
						{
							nrp = new RoutePoint();
							nrp->x = lrp->x;
							nrp->y = lrp->y;
							nrp->dStatus = DROP_PROCESSING;
							nrp->cycle = splitCycle++;
							(*routes)[drop]->push_back(nrp);
							lrp = nrp;
						}
					}
				}
				tsCycle = splitCycle;
			}
			else
				claim(false, "Unsupported opearation encountered while routing.");
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
// After the individual routes have been computed and then compacted, this function
// takes the results of the sub-problem from subRoutes and adds them back to the
// global routes for the entire assay.
///////////////////////////////////////////////////////////////////////////////////
void PostSubproblemCompactionRouter::addSubProbToGlobalRoutes(vector<Droplet *> *subDrops, vector<vector<RoutePoint *> *> *subRoutes, map<Droplet *, vector<RoutePoint *> *> *routes)
{
	routeCycle = cycle;
	while (!subDrops->empty())
	{
		Droplet *d = subDrops->back();
		subDrops->pop_back();
		vector<RoutePoint *> *sr = subRoutes->back();
		subRoutes->pop_back();
		int delay = 0;
		while (!sr->empty())
		{
			RoutePoint *rp = sr->front();
			sr->erase(sr->begin());
			if (rp == NULL)
			{
				delay++;
				if (!(*routes)[d]->empty())
				{
					RoutePoint *lrp = (*routes)[d]->back();
					RoutePoint *nrp = new RoutePoint();
					nrp->cycle = lrp->cycle+1;
					nrp->dStatus = DROP_WAIT;
					nrp->x = lrp->x;
					nrp->y = lrp->y;
					(*routes)[d]->push_back(nrp);
				}
			}
			else
			{
				rp->cycle += delay;
				(*routes)[d]->push_back(rp);
			}
		}
		if ((*routes)[d]->back()->cycle >= routeCycle)
			routeCycle = (*routes)[d]->back()->cycle+1;
		delete sr;
	}
}

///////////////////////////////////////////////////////////////////////////////////
// Problems can arise later on in the compactor when individual routes are compacted
// together.  The main problem arises when one droplet's source is in the IR of
// another droplet's target.  Thus, the order in which droplets are compacted
// matters and can possibly solve this problem.  However, in the case of a dependency
// cycle, at least one droplet will need to be moved out of the way.  This function
// takes a more aggressive and less computationaly complex approach by removing
// all droplet-route dependencies.  If a source is in the IR of another droplet's
// target, the droplet with the conflicting source is simply moved out of the way
// before we compute the individual routes in computeIndivSupProbRoutes() so that
// it has a new source.  This funciton simply tries to move the droplet x=5 cells
// in each cardinal direction until it finds a clear path/destination for the
// conflicting source droplet; if it cannot find a clear path/dest, it reports a
// failure.
//
// WARNING: This solution is not guaranteed in that it is possible that there may
// not be a free block of cells in the immediate vicinity.  However, if we keep
// distance between modules, it should almost always work.  Even without space
// between modules, it should usually work given the module placement isn't
// extremely compact.
///////////////////////////////////////////////////////////////////////////////////
void PostSubproblemCompactionRouter::eliminateSubRouteDependencies(map<Droplet *, vector<RoutePoint *> *> *routes)
{
	// Get the nodes that need to be routed and sort
	vector<AssayNode *> routableThisTS;
	for (int i = 0; i < thisTS->size(); i++)
		if (thisTS->at(i)->GetType() != DISPENSE && thisTS->at(i)->GetStartTS() == startingTS)
			routableThisTS.push_back(thisTS->at(i));

	// Gather the source and destination cells of each routable droplet this TS
	// For now, assume a non-io source is at the location of its last route point; non-io destination is bottom-left
	map<Droplet *, RoutePoint *> *sourceCells = new map<Droplet *, RoutePoint *>();
	map<Droplet *, RoutePoint *> *targetCells = new map<Droplet *, RoutePoint *>();
	vector<Droplet *> *routingThisTS = new vector<Droplet *>();

	for (int i = 0; i < routableThisTS.size(); i++)
	{
		AssayNode *n = routableThisTS.at(i);
		for (int p = 0; p < n->GetParents().size(); p++)
		{
			routeCycle = cycle;// DTG added to compact
			AssayNode *par = n->GetParents().at(p);
			Droplet *pd = par->GetDroplets().back();
			//if (n->GetReconfigMod())
			//	n->GetReconfigMod()->incNumDrops(); // MUST clear this before leaving function b/c computeIndivSubRoutes() counts on it being untouched

			// First get sources
			RoutePoint *s = new RoutePoint();
			if (par->GetType() == DISPENSE)
			{
				if (par->GetIoPort()->getSide() == NORTH)
				{
					s->x = par->GetIoPort()->getPosXY();
					s->y = 0;
				}
				else if (par->GetIoPort()->getSide() == SOUTH)
				{
					s->x = par->GetIoPort()->getPosXY();
					s->y = arch->getNumCellsY()-1;

				}
				else if (par->GetIoPort()->getSide() == EAST)
				{
					s->x = arch->getNumCellsX()-1;
					s->y = par->GetIoPort()->getPosXY();
				}
				else if (par->GetIoPort()->getSide() == WEST)
				{
					s->x = 0;
					s->y = par->GetIoPort()->getPosXY();
				}
			}
			else
			{
				s->x = (*routes)[pd]->back()->x;
				s->y = (*routes)[pd]->back()->y; // last route point
			}
			sourceCells->insert(pair<Droplet *, RoutePoint *>(pd, s));

			// Now get targets
			RoutePoint *t = new RoutePoint();
			if (n->GetType() == OUTPUT)
			{
				if (n->GetIoPort()->getSide() == NORTH)
				{
					t->x = n->GetIoPort()->getPosXY();
					t->y = 0;
				}
				else if (n->GetIoPort()->getSide() == SOUTH)
				{
					t->x = n->GetIoPort()->getPosXY();
					t->y = arch->getNumCellsY()-1;
				}
				else if (n->GetIoPort()->getSide() == EAST)
				{
					t->x = arch->getNumCellsX()-1;
					t->y = n->GetIoPort()->getPosXY();
				}
				else if (n->GetIoPort()->getSide() == WEST)
				{
					t->x = 0;
					t->y = n->GetIoPort()->getPosXY();
				}
			}
			else // DTG, this will need to be adjusted for storage etc., when/if more than one destination in a module
			{
				t->x = n->GetReconfigMod()->getLX();
				t->y = n->GetReconfigMod()->getBY();
				//if (n->GetType() == STORAGE && n->GetReconfigMod()->getNumDrops() > 1)
				//{	// This should be made more general in case the top-left and bottom-right are < 1 cell away
				//	t->x = n->GetReconfigMod()->getLX();
				//	t->y = n->GetReconfigMod()->getTY();
				//}
				//else
				//{
				//	t->x = n->GetReconfigMod()->getLX();
				//	t->y = n->GetReconfigMod()->getBY();
				//}
			}
			targetCells->insert(pair<Droplet *, RoutePoint *>(pd, t));
		}
	}

	// Now, clear the droplet count in the modules so can be used later
	//for (int i = 0; i < routableThisTS.size(); i++)
	//	if (routableThisTS.at(i)->GetReconfigMod())
	//		routableThisTS.at(i)->GetReconfigMod()->resetNumDrops();

	// Now that we've gotten the source/target information, check to see if there is a conflict
	map<Droplet *, RoutePoint *>::iterator srcIt = sourceCells->begin();
	for (; srcIt != sourceCells->end(); srcIt++)
	{
		RoutePoint *s = sourceCells->find(srcIt->first)->second;
		Droplet *d = sourceCells->find(srcIt->first)->first;

		// If we find an interference with another droplet's target, we must move the source location
		map<Droplet *, RoutePoint *>::iterator tgtIt = targetCells->begin();
		for (; tgtIt != targetCells->end(); tgtIt++)
		{
			RoutePoint *t = targetCells->find(tgtIt->first)->second;

			// If the source/target pair is not same route and interferes with each other, try moving the source
			if (doesInterfere(s, t) && (srcIt->first != tgtIt->first))
			{
				bool reRouted = false;
				int maxRerouteDist = 5; // Try moving up to x=5 cells in each direction

				/////////////////////////
				// Try LEFT/WEST
				/////////////////////////
				if (!reRouted)
				{
					int dist = 1;
					int newXDim; // Mod
					// While less than max reroute distance, hasn't already been re-reouted, and reroute is on DMFB
					while (dist <= maxRerouteDist && !reRouted && (newXDim = s->x - dist) >= 0) // MOD
					{
						RoutePoint *sDelta = new RoutePoint();
						sDelta->x = newXDim;
						sDelta->y = s->y;

						if (!rpInterferesWithRpList(sDelta, sourceCells, d))
						{
							if (!rpInterferesWithRpList(sDelta, targetCells, d))
							{
								unsigned long long int c = (*routes)[d]->back()->cycle;
								// Add new re-route to routes
								for (int i = 1; i <= dist; i++)
								{
									RoutePoint *newRp = new RoutePoint();
									newRp->x = s->x - i; // MOD
									newRp->y = s->y;	// MOD
									newRp->dStatus = DROP_NORMAL;
									newRp->cycle = c + i;
									(*routes)[d]->push_back(newRp);
								}
								(*routes)[d]->back()->dStatus = DROP_WAIT; // Set last location as waiting
								// Update s within our sources list
								s->x = (*routes)[d]->back()->x;
								s->y = (*routes)[d]->back()->y;
								s->dStatus = (*routes)[d]->back()->dStatus;
								reRouted = true;
								routeCycle = routeCycle + dist;
								equalizeGlobalRoutes(routes); // Equalize the routes so all droplets will appear in simulation during re-route cycles
							}
						}
						dist++;
					}
				}
				/////////////////////////
				// Try RIGHT/EAST
				/////////////////////////
				if (!reRouted)
				{
					int dist = 1;
					int newXDim;
					// While less than max reroute distance, hasn't already been re-reouted, and reroute is on DMFB
					while (dist <= maxRerouteDist && !reRouted && (newXDim = s->x + dist) < arch->getNumCellsX())
					{
						RoutePoint *sDelta = new RoutePoint();
						sDelta->x = newXDim;
						sDelta->y = s->y;

						if (!rpInterferesWithRpList(sDelta, sourceCells, d))
						{
							if (!rpInterferesWithRpList(sDelta, targetCells, d))
							{
								unsigned long long int c = (*routes)[d]->back()->cycle;
								// Add new re-route to routes
								for (int i = 1; i <= dist; i++)
								{
									RoutePoint *newRp = new RoutePoint();
									newRp->x = s->x + i;
									newRp->y = s->y;
									newRp->dStatus = DROP_NORMAL;
									newRp->cycle = c + i;
									(*routes)[d]->push_back(newRp);
								}
								(*routes)[d]->back()->dStatus = DROP_WAIT; // Set last location as waiting
								// Update s within our sources list
								s->x = (*routes)[d]->back()->x;
								s->y = (*routes)[d]->back()->y;
								s->dStatus = (*routes)[d]->back()->dStatus;
								reRouted = true;
								routeCycle = routeCycle + dist;
								equalizeGlobalRoutes(routes); // Equalize the routes so all droplets will appear in simulation during re-route cycles
							}
						}
						dist++;
					}
				}
				/////////////////////////
				// Try UP/NORTH
				/////////////////////////
				if (!reRouted)
				{
					int dist = 1;
					int newYDim;
					// While less than max reroute distance, hasn't already been re-reouted, and reroute is on DMFB
					while (dist <= maxRerouteDist && !reRouted && (newYDim = s->y - dist) >= 0)
					{
						RoutePoint *sDelta = new RoutePoint();
						sDelta->x = s->x;
						sDelta->y = newYDim;

						if (!rpInterferesWithRpList(sDelta, sourceCells, d))
						{
							if (!rpInterferesWithRpList(sDelta, targetCells, d))
							{
								unsigned long long int c = (*routes)[d]->back()->cycle;
								// Add new re-route to routes
								for (int i = 1; i <= dist; i++)
								{
									RoutePoint *newRp = new RoutePoint();
									newRp->x = s->x;
									newRp->y = s->y - i;
									newRp->dStatus = DROP_NORMAL;
									newRp->cycle = c + i;
									(*routes)[d]->push_back(newRp);
								}
								(*routes)[d]->back()->dStatus = DROP_WAIT; // Set last location as waiting
								// Update s within our sources list
								s->x = (*routes)[d]->back()->x;
								s->y = (*routes)[d]->back()->y;
								s->dStatus = (*routes)[d]->back()->dStatus;
								reRouted = true;
								routeCycle = routeCycle + dist;
								equalizeGlobalRoutes(routes); // Equalize the routes so all droplets will appear in simulation during re-route cycles
							}
						}
						dist++;
					}
				}
				/////////////////////////
				// Try DOWN/SOUTH
				/////////////////////////
				if (!reRouted)
				{
					int dist = 1;
					int newYDim;
					// While less than max reroute distance, hasn't already been re-reouted, and reroute is on DMFB
					while (dist <= maxRerouteDist && !reRouted && (newYDim = s->y + dist) < arch->getNumCellsY())
					{
						RoutePoint *sDelta = new RoutePoint();
						sDelta->x = s->x;
						sDelta->y = newYDim;

						if (!rpInterferesWithRpList(sDelta, sourceCells, d))
						{
							if (!rpInterferesWithRpList(sDelta, targetCells, d))
							{
								unsigned long long int c = (*routes)[d]->back()->cycle;
								// Add new re-route to routes
								for (int i = 1; i <= dist; i++)
								{
									RoutePoint *newRp = new RoutePoint();
									newRp->x = s->x;
									newRp->y = s->y + i;
									newRp->dStatus = DROP_NORMAL;
									newRp->cycle = c + i;
									(*routes)[d]->push_back(newRp);
								}
								(*routes)[d]->back()->dStatus = DROP_WAIT; // Set last location as waiting
								// Update s within our sources list
								s->x = (*routes)[d]->back()->x;
								s->y = (*routes)[d]->back()->y;
								s->dStatus = (*routes)[d]->back()->dStatus;
								reRouted = true;
								routeCycle = routeCycle + dist;
								equalizeGlobalRoutes(routes); // Equalize the routes so all droplets will appear in simulation during re-route cycles
							}
						}
						dist++;
					}
				}

				// If was re-rotued, good..exit loop.  Else, exit and inform user.
				if (reRouted)
					break; // no need to compare to more sources...it is safe
				else
					claim(false, "Droplet dependency was not fixed;  Attempt to break dependency by moving a source to free cells was a failure.");
			}
		}
	}

	// Cleanup
	map<Droplet *, RoutePoint *>::iterator it;
	for (it = sourceCells->begin(); it != sourceCells->end(); it++)
	{
		RoutePoint *rp = it->second;
		it->second = NULL;
		delete rp;
	}
	sourceCells->clear();
	delete sourceCells;

	for (it = targetCells->begin(); it != targetCells->end(); it++)
	{
		RoutePoint *rp = it->second;
		it->second = NULL;
		delete rp;
	}
	targetCells->clear();
	delete targetCells;

	delete routingThisTS;
}

///////////////////////////////////////////////////////////////////////////////////
// Ensures that the given route point doesn't interfere with any of the route points
// in the map/list, besides the route point that corresponds to the same droplet
// (we are either comparing a source to the complete list of sources or complete
// list of targets)
///////////////////////////////////////////////////////////////////////////////////
bool PostSubproblemCompactionRouter::rpInterferesWithRpList(RoutePoint *rp, map<Droplet *, RoutePoint *> *rps, Droplet *d)
{
	// Search through entire list
	map<Droplet *, RoutePoint *>::iterator rpIt = rps->begin();
	for (; rpIt != rps->end(); rpIt++)
	{
		RoutePoint *rpCompare = rps->find(rpIt->first)->second;
		Droplet *dCompare = rpIt->first;
		if (d != dCompare && doesInterfere(rp, rpCompare))
			return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////
// Computes the individual subroutes for a sub-problem. A new sub-route is created
// for each sub-rotue and added to subRoutes; also, the corresponding droplet is
// added to subDrops (corresponding routes and droplets must share the same index
// in subRoutes and subDrops).
//
// This function is called each time-step that droplets are being routed; it computes
// routes for a single sub-problem.
//
// Upon beginning this function, subRotues and subDrops is empty.  Upon exiting
// this funciton, subDrops is filled with a droplet for each droplet being routed
// during this time-step/sub-problem AND subRoutes is filled with a non-compacted
// route for each corresponding droplet in subDrops.  The routes are computed
// in isolation to be compacted later.
//
// This is the main function to be re-written by other routing algorithms as
// the individual routes are typically computed in various ways.
///////////////////////////////////////////////////////////////////////////////////
void PostSubproblemCompactionRouter::computeIndivSupProbRoutes(vector<vector<RoutePoint *> *> *subRoutes, vector<Droplet *> *subDrops, map<Droplet *, vector<RoutePoint *> *> *routes)
{
	claim(false, "No valid router was selected for the synthesis process or no method for 'computeIndivSupProbRoutes()' was implemented for the selected router.\n");
}

///////////////////////////////////////////////////////////////////////////////////
// This function is the main public function called.  It fills the "routes" and
// "tsBeginningCycle" data structures and contains the code for the main routing flow.
///////////////////////////////////////////////////////////////////////////////////
void PostSubproblemCompactionRouter::route(DAG *dag, DmfbArch *arch, vector<ReconfigModule *> *rModules, map<Droplet *, vector<RoutePoint *> *> *routes, vector<vector<int> *> *pinActivations, vector<unsigned long long> *tsBeginningCycle)
{
	cyclesPerTS = arch->getSecPerTS() * arch->getFreqInHz();
	startingTS = 0; // The TS that is getting ready to begin (droplets are being routed to)
	routeCycle = 0;
	cycle = 0;

	// Copy all nodes to a new list to be sorted
	vector<AssayNode *> nodes;// = new vector<AssayNode *>();
	for (int i = 0; i < dag->getAllNodes().size(); i++)
		nodes.push_back(dag->getAllNodes().at(i));
	Sort::sortNodesByStartThenEndTS(&nodes);

	initCellTypeArray();
	routerSpecificInits();

	///////////////////////////////////////////////////////////////////////
	// This is the main loop. Each iteration of this loop solves one
	// time-step (routing sub-problem)
	///////////////////////////////////////////////////////////////////////
	while(!nodes.empty())
	{
		int j = 0;
		while ( j < nodes.size() && nodes.at(j)->GetStartTS() <= startingTS && nodes.at(j)->GetEndTS() >= startingTS && j < nodes.size())
			thisTS->push_back(nodes.at(j++));

		//if (startingTS >= 242)
		//{
		//	cout << "DebugPrint: Routing to TS " << startingTS << "." << endl; // DTG Debug Print
			//return;
		//}

		///////////////////////////////////////////////////////////////////////
		// First create any new droplets
		///////////////////////////////////////////////////////////////////////
		for (int i = 0; i < thisTS->size(); i++)
		{
			AssayNode *n = thisTS->at(i);
			if (n->GetType() == DISPENSE && n->GetStartTS() == startingTS)
			{	// Create new droplets to be input soon
				Droplet *d = new Droplet();
				d->uniqueFluidName = n->GetPortName();
				d->volume = n->GetVolume();
				(*routes)[d] = new vector<RoutePoint *>();
				n->addDroplet(d);
			}
		}

		///////////////////////////////////////////////////////////////////////
		// Then, get the initial individual routes to be compacted later
		///////////////////////////////////////////////////////////////////////
		routeCycle = cycle;
		vector<vector<RoutePoint *> *> *subRoutes = new vector<vector<RoutePoint *> *>(); // subProblem routes
		vector<Droplet *> *subDrops = new vector<Droplet *>(); // corresponding subProblem droplets
		//eliminateSubRouteDependencies(routes); // Optional; ensures that no source is in the IR of a target (moves the source out of way)
		computeIndivSupProbRoutes(subRoutes, subDrops, routes);
		//printSubRoutes(subRoutes, subDrops);

		///////////////////////////////////////////////////////////////////////
		// Then, compact and do maintenance on the routes
		///////////////////////////////////////////////////////////////////////
		compactRoutes(subDrops, subRoutes); // Now, do route COMPACTION
		addSubProbToGlobalRoutes(subDrops, subRoutes, routes); // Add sub-rotues to routes for entire simulation
		equalizeGlobalRoutes(routes); // Now, add cycles for the droplets that were waiting first so they look like they were really waiting there
		tsBeginningCycle->push_back(cycle); // Add cycle so we now when time-step begins
		processTimeStep(routes); // Now that routing is done, process the time-step

		///////////////////////////////////////////////////////////////////////
		// Cleanup
		///////////////////////////////////////////////////////////////////////
		for (int i = nodes.size()-1; i >= 0; i--)
		{
			if (nodes.at(i)->endTimeStep <= startingTS)
				nodes.erase(nodes.begin() + i);
		}
		while (!subRoutes->empty())
		{
			vector<RoutePoint *> * v = subRoutes->back();
			subRoutes->pop_back();
			delete v; // Individual RoutePoints are deleted later by the Util Class
		}
		delete subRoutes;
		delete subDrops;
		thisTS->clear();

		//if (this->startingTS == 25)
		//{
		//	cout << "EXITING ROUTING NOW." << endl;
		//	return;
		//}
	}
}

///////////////////////////////////////////////////////////////////////////////////
// Debug function to print the computed subroutes
///////////////////////////////////////////////////////////////////////////////////
void PostSubproblemCompactionRouter::printSubRoutes(vector<vector<RoutePoint *> *> *subRoutes, vector<Droplet *> *subDrops)
{
	cout << "Routing to TS " << startingTS << ":" << endl;
	for (int i = 0; i < subDrops->size(); i++)
	{
		Droplet *d = subDrops->at(i);
		vector<RoutePoint *> *route = subRoutes->at(i);

		cout << d->id << ": ";
		for (int j = 0; j < route->size(); j++)
			cout << "(" << route->at(j)->x << ", " << route->at(j)->y << ") ";
		cout << endl;
	}
	cout << endl;
}
