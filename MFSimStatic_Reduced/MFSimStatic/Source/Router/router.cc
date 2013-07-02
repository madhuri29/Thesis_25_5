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
 * Source: router.cc															*
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
#include "../../Headers/Router/router.h"

///////////////////////////////////////////////////////////////////////////////////
// Constructors
///////////////////////////////////////////////////////////////////////////////////
Router::Router()
{

}
Router::Router(DmfbArch *dmfbArch)
{
	arch = dmfbArch;
}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
Router::~Router()
{

}

///////////////////////////////////////////////////////////////////////////////////
// Generic route function inherited by implemented routers.
///////////////////////////////////////////////////////////////////////////////////
void Router::route(DAG *dag, DmfbArch *arch, vector<ReconfigModule *> *rModules, map<Droplet *, vector<RoutePoint *> *> *routes, vector<vector<int> *> *pinActivations, vector<unsigned long long> *tsBeginningCycle)
{
	claim(false, "No valid router was selected for the synthesis process or no method for 'route()' was implemented for the selected router.\n");
}

///////////////////////////////////////////////////////////////////////////////
// Sets parameters for router
///////////////////////////////////////////////////////////////////////////////
void Router::setRoutingParams(CompactionType ct, ProcessEngineType pet)
{
	compactionType = ct;
	processEngineType = pet;
}

///////////////////////////////////////////////////////////////////////////////
// Performs any one-time setup operations that need to be done to operate the
// router.  These operations should not be considered in routing timings.
///////////////////////////////////////////////////////////////////////////////
void Router::preRoute(DmfbArch *arch)
{
}
///////////////////////////////////////////////////////////////////////////////
// Performs any one-time cleanup operations specific to this router
///////////////////////////////////////////////////////////////////////////////
void Router::postRoute()
{
}

///////////////////////////////////////////////////////////////////////////////
// Prints out routes in vector of routes.
///////////////////////////////////////////////////////////////////////////////
void Router::printRoutes(vector<vector<RoutePoint *> *> *routes)
{
	for (int i = 0; i < routes->size(); i++)
	{
		cout << "SubRoute " << i << ": ";
		for (int j = 0; j < routes->at(i)->size(); j++)
		{
			if (routes->at(i)->at(j))
				cout << "(" << routes->at(i)->at(j)->x << "," << routes->at(i)->at(j)->y << ",C" << routes->at(i)->at(j)->cycle << ")-->";
			else
				cout << "(S,S,Cxxx)-->";
		}
		cout << endl;
	}
	cout << "-------------------------------------------------------------------------" << endl;
}

///////////////////////////////////////////////////////////////////////////////////
// Examines all the routes to see if any droplet is occupying the cell at the given
// XY coordinates.  Returns -1 if cell is free of droplets.
///////////////////////////////////////////////////////////////////////////////////
int Router::dropletOccupyingCell(int x, int y, map<Droplet *, vector<RoutePoint *> *> *routes, vector<vector<RoutePoint *> *> *subRoutes, vector<Droplet *> *subDrops)
{
	int id = -1;
	map<Droplet *, vector<RoutePoint *> *>::iterator it = routes->begin();
	for (; it != routes->end(); it++)
	{
		if (!it->second->empty() && it->second->back()->x == x && it->second->back()->y == y && !(it->second->back()->dStatus == DROP_OUTPUT || it->second->back()->dStatus == DROP_CONSUMED))
		{
			id = it->first->id;
			break;
		}
	}

	// If droplet has already been moved this routing phase, take into account
	for (int q = 0; q < subDrops->size(); q++)
	{
		//Droplet *drop = subDrops->at(q);
		//vector<RoutePoint*> *debugRoute = subRoutes->at(q);
		//RoutePoint * debugRp = debugRoute->back();

		if (!subRoutes->at(q)->empty())
		{
			if (subDrops->at(q)->id != id && subRoutes->at(q)->back()->x == x && subRoutes->at(q)->back()->y == y)
				id = subDrops->at(q)->id;
			else if (subDrops->at(q)->id == id && !(subRoutes->at(q)->back()->x == x && subRoutes->at(q)->back()->y == y))
				id = -1;
		}
	}
	return id;
}

///////////////////////////////////////////////////////////////////////////////
// Given two droplet locations, returns true if one droplet is inside the
// interference region (IR) of the other; returns false, otherwise.
///////////////////////////////////////////////////////////////////////////////
bool Router::doesInterfere(RoutePoint *r1, RoutePoint *r2)
{
	return (abs(r1->x - r2->x) <= 1 && abs(r1->y - r2->y) <= 1);
}

///////////////////////////////////////////////////////////////////////
// Compacts a list of subRoutes so that they can be routed concurrently.
// subDrops contains the droplets and subRoutes contains the routes to
// be compacted.  The indices for subDrops/subRoutes MUST match (i.e.
// subRoutes[x] is the subRoute for the droplet at subDrops[x].
// When passing in subRoutes, each subRoute contains a number of routing
// points that does not consider any other subRoute (i.e. subRoute[x][i],
// which represents droplet x's position at time i, could be the same
// location as subRoute[y][i]). Compaction will prevent droplets from
// being in the same location at the same time, adding stalls when
// necessary, so that subRoutes[x][i] is compatible with subRoutes[y][j],
// for all x, y, i and j (i.e. no unintentional mixing occurs).

// IN SHORT: after this function is called, the second index of subRoutes
// can properly be viewed as the cycle number for this sub-problem b/c
// the compaction methods use static/dynamic rules to ensure no droplet
// enters the interference region (IR) of any other droplet at any time.
///////////////////////////////////////////////////////////////////////
void Router::compactRoutes(vector<Droplet *> *subDrops, vector<vector<RoutePoint *> *> *subRoutes)
{
	Sort::sortRoutesByLength(subRoutes, subDrops); // Route longer paths first

	// Ensure that all routes' last point is labeled appropriately
	for (int i = 0; i < subRoutes->size(); i++)
		if (subRoutes->at(i)->size() > 0)
			if (subRoutes->at(i)->back()->dStatus != DROP_OUTPUT)
				subRoutes->at(i)->back()->dStatus = DROP_WAIT;


	if (compactionType == BEG_COMP)
		compactRoutesWithBegStalls(subRoutes);
	else if (compactionType == MID_COMP)
		compactRoutesWithMidStalls(subRoutes);
	else if (compactionType == INHERENT_COMP) // Ben's addition
	{ /*Do nothing...compaction already done internally by router.*/ }
	else
		decompact(subRoutes);
}

///////////////////////////////////////////////////////////////////////////////
// Flattens the droplet routes so that they are routed one-by-one, instead
// of concurrently.
///////////////////////////////////////////////////////////////////////////////
void Router::decompact(vector<vector<RoutePoint *> *> *subRoutes)
{
	// Now add stalls to any droplets that had to wait to be routed/decompacted
	unsigned long long numStalls = 0;
	for (int i = 0; i < subRoutes->size(); i++)
	{
		vector<RoutePoint *> *subRoute = subRoutes->at(i);
		if (subRoute->size() > 0)
		{
				for (int j = 0; j < numStalls; j++)
					subRoute->insert(subRoute->begin(), NULL);
				numStalls = subRoute->size();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Given a vector full of sequential routes, compacts them such that each
// equal index of any subRoute can be executed at the same time.  Adds any
// necessary stalls at the beginning of the route.
//
// Compacts the routes by combining the routes from longest to shortest.
// If route being placed interferes with a route that is already placed, the
// route being placed will be delayed by 1 cycle until it can be placed.
///////////////////////////////////////////////////////////////////////////////
void Router::compactRoutesWithBegStalls(vector<vector<RoutePoint *> *> *subRoutes)
{
	//printRoutes(subRoutes);

	int longestRoute = 0;
	if (subRoutes->size() > 0)
		longestRoute = subRoutes->at(0)->size();

	int numStallsToPrepend = 1;
	for (int i = 0; i < subRoutes->size(); i++)
	{
		vector<RoutePoint *> *subRoute = subRoutes->at(i);
		RoutePoint *destPt = NULL;
		if (subRoute->size() > 0)
			destPt = subRoute->back();

		// Check entire route
		bool isInterference = false;
		int j = 0; // The index used to traverse a specific route/cycle

		//printRoutes(subRoutes);

		int numStallsInserted = 0;
		while (j != max((int)subRoute->size(), longestRoute) && subRoute->size() > 0)
		{
			int debugSubRouteNum = -1;
			RoutePoint *debugSubRouteRp = NULL;
			//printRoutes(subRoutes);

			RoutePoint *rp = NULL;
			if (j <= subRoute->size()-1)
				rp = subRoute->at(j);
			else
				rp = subRoute->back();

			if (rp)
			{
				// Check against the previous routes that have been compacted
				for (int k = 0; k < i; k++)
				{
					vector<RoutePoint *> *pastRoute = subRoutes->at(k);

					RoutePoint *rpLc = NULL; // This route's last cycle
					if (j > 0 && j <= subRoute->size()-1)
						rpLc = subRoute->at(j-1);
					else
						rpLc = subRoute->back();

					RoutePoint *rpNc = NULL; // This route's next cycle
					if (j+1 <= subRoute->size()-1)
						rpNc = subRoute->at(j+1);
					else
						rpNc = subRoute->back();

					RoutePoint *prp = NULL; // Past route's current cycle
					if (j <= pastRoute->size()-1)
						prp = pastRoute->at(j);
					else if (pastRoute->back()->dStatus != DROP_OUTPUT)
						prp = pastRoute->back();

					RoutePoint *prpLc = NULL; // Past route's last cycle
					if (j > 0 && j <= pastRoute->size()-1)
						prpLc = pastRoute->at(j-1);
					else if (pastRoute->back()->dStatus != DROP_OUTPUT)
						prpLc = pastRoute->back();

					RoutePoint *prpNc = NULL; // Past route's next cycle
					if (j+1 <= pastRoute->size()-1)
						prpNc = pastRoute->at(j+1);
					else if (pastRoute->back()->dStatus != DROP_OUTPUT)
						prpNc = pastRoute->back();

					// Check dynamic droplet rules so this and last droplet locations don't interfere
					if (prp && doesInterfere(rp, prp) && !(doesInterfere(rp, destPt) && prp->dStatus == DROP_WAIT))
							isInterference = true;
					if (prpLc && doesInterfere(rp, prpLc) && !(doesInterfere(rp, destPt) && prpLc->dStatus == DROP_WAIT)) // DTG, is this if-statement necessary??
						isInterference = true;

					if (prp && rpNc && doesInterfere(rpNc, prp) && !(doesInterfere(rpNc, destPt) && prp->dStatus == DROP_WAIT))
						isInterference = true;
					if (prpNc && doesInterfere(rp, prpNc) && !(doesInterfere(rp, destPt) && prpNc->dStatus == DROP_WAIT))
						isInterference = true;
					if (prp && rpLc && doesInterfere(rpLc, prp) && !(doesInterfere(rpLc, destPt) && prp->dStatus == DROP_WAIT))
						isInterference = true;
					if (prpLc && rpLc && doesInterfere(rpLc, prpLc) && !(doesInterfere(rpLc, destPt) && prpLc->dStatus == DROP_WAIT))
						isInterference = true;

					// Get debug info if found interference for possible display to user
					if (isInterference)
					{
						debugSubRouteNum = k;
						debugSubRouteRp = prp;
						break;
					}
				}
			}
			if (isInterference)
			{	// Add a few stalls at the beginning and try again
				for (int m = 0; m < numStallsToPrepend; m++)
					subRoute->insert(subRoute->begin(), NULL);
				isInterference = false;
				numStallsInserted += numStallsToPrepend;
				j = 0;
			}
			else
				j++;

			// Sanity check; ensure we're not in an infinite loop
			int stallInsertionLimit = 1000;
			if (numStallsInserted >= stallInsertionLimit)
			{
				stringstream msg;
				msg << "The stall insertion limit of " << stallInsertionLimit << " has been reached during route compaction." << endl;
				msg << "This likely indicates a problem with the individual route generation, such as blockages not being computed properly." << endl;
				msg << "The problem manifests with SubRoute #" << i << " of " << subRoutes->size();
				msg << ", specifically when the droplet at location (" << rp->x << ", " << rp->y << ")";
				msg << " interferes with the droplet in SubRoute #" << debugSubRouteNum << " at location (";
				msg << debugSubRouteRp->x << ", " << debugSubRouteRp->y << ")." << endl;
				msg << "A sub-route dump has been generated just above this.  It is recommended that you step through the compaction code to see where the uncompactable droplet interference is taking place to help debug your router." << endl;

				printRoutes(subRoutes);
				claim(false, &msg);

			}
		}
		if (j > longestRoute)
			longestRoute = j;
	}
	//printRoutes(subRoutes);

}

///////////////////////////////////////////////////////////////////////////////
// Given a vector full of sequential routes, compacts them such that each
// equal index of any subRoute can be executed at the same time.  Adds any
// necessary stalls in the middle of the route.
//
// Compacts the routes by combining the routes from longest to shortest.
// If route being placed interferes with a route that is already placed, the
// route being placed will be delayed by 1 cycle until it can be placed.
///////////////////////////////////////////////////////////////////////////////
void Router::compactRoutesWithMidStalls(vector<vector<RoutePoint *> *> *subRoutes)
{
	//printRoutes(subRoutes);

	bool deadlock = false;
	int numStallsToPrepend = 1;
	int insertionOffset = 2; // # of cycles to offset the insertion of stalls before actual interference

	int longestRoute = 0;
	if (subRoutes->size() > 0)
		longestRoute = subRoutes->at(0)->size();

	for (int i = 0; i < subRoutes->size(); i++)
	{
		if (deadlock)
			break;

		vector<RoutePoint *> *subRoute = subRoutes->at(i);
		RoutePoint *destPt = NULL;
		if (subRoute->size() > 0)
			destPt = subRoute->back();
		int numStallsAdded = 0;

		// Check entire route
		bool isInterference = false;
		int j = 0; // The index used to traverse a specific route/cycle

		while (j != max((int)subRoute->size(), longestRoute) && subRoute->size() > 0)
		{
			RoutePoint *rp = NULL;
			if (j <= subRoute->size()-1)
				rp = subRoute->at(j);
			else
				rp = subRoute->back();

			int insertionIndex = -1;

			if (rp)
			{
				// Check against the previous routes that have been compacted
				for (int k = 0; k < i; k++)
				{
					vector<RoutePoint *> *pastRoute = subRoutes->at(k);

					RoutePoint *rpLc = NULL; // This route's last cycle
					if (j > 0 && j <= subRoute->size()-1)
						rpLc = subRoute->at(j-1);
					else
						rpLc = subRoute->back();

					RoutePoint *rpNc = NULL; // This route's next cycle
					if (j+1 <= subRoute->size()-1)
						rpNc = subRoute->at(j+1);
					else
						rpNc = subRoute->back();

					RoutePoint *prp = NULL; // Past route's current cycle
					if (j <= pastRoute->size()-1)
						prp = pastRoute->at(j);
					else
						prp = pastRoute->back();

					RoutePoint *prpLc = NULL; // Past route's last cycle
					if (j > 0 && j <= pastRoute->size()-1)
						prpLc = pastRoute->at(j-1);
					else
						prpLc = pastRoute->back();

					RoutePoint *prpNc = NULL; // Past route's next cycle
					if (j+1 <= pastRoute->size()-1)
						prpNc = pastRoute->at(j+1);
					else
						prpNc = pastRoute->back();

					// Check dynamic droplet rules so this and last droplet locations don't interfere
					if (prp && doesInterfere(rp, prp) && !(doesInterfere(rp, destPt) && prp->dStatus == DROP_WAIT))
					{
							isInterference = true;
							break;
					}
					if (prpLc && doesInterfere(rp, prpLc) && !(doesInterfere(rp, destPt) && prpLc->dStatus == DROP_WAIT)) // DTG, is this if-statement necessary??
					{
						isInterference = true;
						break;
					}
					if (prp && rpNc && doesInterfere(rpNc, prp) && !(doesInterfere(rpNc, destPt) && prp->dStatus == DROP_WAIT))
					{
						isInterference = true;
						break;
					}
					if (prpNc && doesInterfere(rp, prpNc) && !(doesInterfere(rp, destPt) && prpNc->dStatus == DROP_WAIT))
					{
						isInterference = true;
						break;
					}
					if (prp && rpLc && doesInterfere(rpLc, prp) && !(doesInterfere(rpLc, destPt) && prp->dStatus == DROP_WAIT))
					{
						isInterference = true;
						break;
					}
					if (prpLc && rpLc && doesInterfere(rpLc, prpLc) && !(doesInterfere(rpLc, destPt) && prpLc->dStatus == DROP_WAIT))
					{
						isInterference = true;
						break;
					}
				}
			}
			if (isInterference)
			{	// Add a few stalls at the beginning and try again

				numStallsAdded++;
				if (numStallsAdded >= 100)
				{
					deadlock = true;
					break;
				}

				insertionIndex = j; // Tells where the interference is

				if ((insertionIndex - insertionOffset <= 0) || (insertionIndex - insertionOffset >= subRoute->size()-1))
					for (int m = 0; m < numStallsToPrepend; m++)
						subRoute->insert(subRoute->begin(), NULL);
				else
				{
					for (int m = 0; m < numStallsToPrepend; m++)
					{
						//cout << subRoute->size() << endl;
						RoutePoint *rpLast = subRoute->at(insertionIndex-insertionOffset);
						if (rpLast)
						{
							RoutePoint *rpStall = new RoutePoint();
							rpStall->x = rpLast->x;
							rpStall->y = rpLast->y;
							rpStall->dStatus = DROP_INT_WAIT;
							subRoute->insert(subRoute->begin()+insertionIndex-1, rpStall);
						}
						else
							subRoute->insert(subRoute->begin()+insertionIndex-1, NULL);
					}
				}
				isInterference = false;
				j = 0;
			}
			else
				j++;
		}
		if (j > longestRoute)
			longestRoute = j;
	}

	// If found a deadlock (inserting stalls in middle did not work), remove all inserts and try
	// adding stalls to beginning.
	if (deadlock)
	{
		//cout << "TRY WITH STALLS AT BEGINNING." << endl;

		for (int i = 0; i < subRoutes->size(); i++)
		{
			vector<RoutePoint *> *subRoute = subRoutes->at(i);
			for (int j = subRoute->size()-1; j >=0; j--)
			{
				if (subRoute->at(j) == NULL || subRoute->at(j)->dStatus == DROP_INT_WAIT)
				{
					RoutePoint *rp = subRoutes->at(i)->at(j);
					subRoutes->at(i)->erase(subRoutes->at(i)->begin()+j);
					if (rp)
						delete rp;
				}
			}
		}
		compactRoutesWithBegStalls(subRoutes);
	}
	//printRoutes(subRoutes);
}

///////////////////////////////////////////////////////////////////////////////
// Gets the pin-activations from the droplet locations. Assumes that the
// electrode underneath any droplet is activated.
///////////////////////////////////////////////////////////////////////////////
void Router::setPinActivationsFromDropletMotion(map<Droplet *, vector<RoutePoint *> *> *routes, vector<vector<int> *> *pinActivations)
{
	PinMapper *pm = arch->getPinMapper();
	vector<vector<int> *> *pinMapping = pm->getPinMapping();

	// Find the max cycle
	unsigned long long maxCycle = 0;
	map<Droplet *, vector<RoutePoint *> *>::iterator it = routes->begin();
	for (; it != routes->end(); it++)
		if (it->second->back()->cycle > maxCycle)
			maxCycle = it->second->back()->cycle;

	/////////////////////// DEBUG PRINT
	/*int d = 0;
	for (it = routes->begin(); it != routes->end(); it++)
	{
		vector<RoutePoint *> *route = it->second;
		cout << "Route #" << d << " size(" << route->size() << "): " << endl;
		for (int i = 0; i < route->size(); i++)
		{
			if (i == 279)
				cout <<"BREAK";
			RoutePoint *dbg = route->at(i);
			cout << "i=" << i << ", c=" << dbg->cycle << " | ";
		}
		d++;
		cout << endl;
	}*/
	///////////////////// END DEBUG PRINT

	// Create a pin-activation vector for each cycle
	for (unsigned long long i = 0; i <= maxCycle+1; i++)
		pinActivations->push_back(new vector<int>);

	// Now, add pin under each droplet to pin-activation list
	for (it = routes->begin(); it != routes->end(); it++)
	{
		vector<RoutePoint *> *route = it->second;
		for (unsigned long long i = 0; i < route->size(); i++)
		{
			//RoutePoint *dbg = route->at(i);
			//cout << "DEBUG : i=" << i << ", c=" << dbg->cycle << ", x=" << dbg->x << ", y=" << dbg->y << endl;
			pinActivations->at(route->at(i)->cycle)->push_back(pinMapping->at(route->at(i)->x)->at(route->at(i)->y));
		}
	}

	// Debug print
	/*for (unsigned long long i = 0; i < pinActivations->size(); i++)
	{
		cout << i << ": ";
		for (int j = 0; j < pinActivations->at(i)->size(); j++)
			cout << pinActivations->at(i)->at(j) << ", ";
		cout << endl;
	}*/
}


///////////////////////////////////////////////////////////////////////////////
// Now that the pin activations have been computed, simulate droplet motion
// from these pin activations. This is not close to a perfect simulation...it
// can't handle complex cases, but should work well for the simple cases of moving
// droplets, simple splits and merges
///////////////////////////////////////////////////////////////////////////////
bool revSort (int i,int j) { return (j<i); }
void Router::simulateDropletMotion(map<Droplet *, vector<RoutePoint *> *> *routes, vector<vector<int> *> *pinActivations)
{
	PinMapper *pm = arch->getPinMapper();
	vector<vector<int> *> *pinMapping = pm->getPinMapping();

	// Get the min I/O pin # so we know when to input/output a droplet
	/*int minIoPin = arch->getIoPorts()->at(0)->getPinNo();
	for (int i = 0; i < arch->getIoPorts()->size(); i++)
		if (arch->getIoPorts()->at(i)->getPinNo() < minIoPin)
			minIoPin = arch->getIoPorts()->at(i)->getPinNo();*/
	int minIoPin = min(pm->getInputPins()->front(), pm->getOutputPins()->front());


	for (int c = 0; c < cycle; c++)
	{
		vector<int> *pins = pinActivations->at(c);
		sort(pins->begin(), pins->end(), revSort); // Get I/O out there first

		// Debug print
		//cout << "C" << c << ": ";
		//for (int p = 0; p < pins->size(); p++)
		//	cout << pins->at(p) << "--";
		//cout << endl;

		// Check the I/O pins (should be at front of pins list b/c of last sort)
		for (int p = 0; p < pins->size(); p++)
		{
			// If I/O pin, handle accordingly
			if (pins->at(p) >= minIoPin)
			{
				for (int i = 0; i < arch->getIoPorts()->size(); i++)
				{
					if (arch->getIoPorts()->at(i)->getPinNo() == pins->at(p))
					{
						// Get port and location
						IoPort *port = arch->getIoPorts()->at(i);
						int px;
						int py;
						if (port->getSide() == NORTH)
						{
							px = port->getPosXY();
							py = 0;
						}
						if (port->getSide() == SOUTH)
						{
							px = port->getPosXY();
							py = arch->getNumCellsY()-1;
						}
						if (port->getSide() == EAST)
						{
							px = arch->getNumCellsX()-1;
							py = port->getPosXY();
						}
						if (port->getSide() == WEST)
						{
							px = 0;
							py = port->getPosXY();
						}

						// If input Port, create new droplet
						if (port->isAnInput())
						{
							Droplet *d = new Droplet();
							d->uniqueFluidName = port->getPortName();
							//d->volume = n->GetVolume();
							(*routes)[d] = new vector<RoutePoint *>();

							// Create new route point and add it to droplet list
							RoutePoint *rp = new RoutePoint();
							rp->cycle = c;
							rp->dStatus = DROP_NORMAL;
							rp->x = px;
							rp->y = py;
							(*routes)[d]->push_back(rp);
						}
						else // If output, seal off the droplet as being output
						{
							map<Droplet *, vector<RoutePoint *> *>::iterator dropIt = routes->begin();
							for (; dropIt != routes->end(); dropIt++)
							{
								vector<RoutePoint *> *route = dropIt->second;
								RoutePoint *lrp = route->back(); // Last route point

								if (lrp->x == px && lrp->y == py && lrp->cycle == c-1)
									lrp->dStatus = DROP_OUTPUT;
							}
						}
						break;
					}
				}
			}
			else
				break; // No more I/O pins
		}


		// Check each droplet to see if a pin is adjacent to a droplet
		map<Droplet *, vector<RoutePoint *> *>::iterator dropIt = routes->begin();
		for (; dropIt != routes->end(); dropIt++)
		{
			vector<RoutePoint *> *route = dropIt->second;
			RoutePoint *lrp = route->back(); // Last route point

			if (lrp->dStatus != DROP_OUTPUT && lrp->dStatus != DROP_CONSUMED && lrp->dStatus != DROP_DRIFT)
			{
				// Check each cell around against the activated pins to see if this droplet will move
				vector<RoutePoint *> next; // Next droplet locations
				for (int y = lrp->y-1; y <= lrp->y+1; y++)
				{
					for (int x = lrp->x-1; x <= lrp->x+1; x++)
					{
						if (x >= 0 && x < arch->getNumCellsX() && y >= 0 && y < arch->getNumCellsY()) // On the board
						{
							if (!(lrp->x == x && lrp->y == y)) // Don't look at the middle for now
							{
								for (int p = 0; p < pins->size(); p++) // Compare to the pins being activated this cycle
								{
									if (pinMapping->at(x)->at(y) == pins->at(p)) // If a match, add to next droplet locations
									{
										RoutePoint *rp = new RoutePoint();
										rp->cycle = c;
										rp->dStatus = DROP_NORMAL;
										rp->x = x;
										rp->y = y;
										next.push_back(rp);
									}
								}
							}
						}
					}
				}

				// If just one adjacent electrode in the next list, move to this cell
				// (ignore stretching over multiple elecs for now)
				if (next.size() == 1)
					route->push_back(next.back());
				else if (next.size() > 1) // If more, create new droplets
				{
					route->push_back(next.front());
					for (int n = 1; n < next.size(); n++)
					{
						Droplet *d = new Droplet();
						d->uniqueFluidName = dropIt->first->uniqueFluidName;
						(*routes)[d] = new vector<RoutePoint *>();
						(*routes)[d]->push_back(next.at(n));
					}
				}
				else // No adjacent electrodes activated
				{
					bool stationary = false;
					for (int p = 0; p < pins->size(); p++)
					{
						if (pinMapping->at(lrp->x)->at(lrp->y) == pins->at(p)) // If a match, add to next droplet locations
						{
							stationary = true;
							break;
						}
					}

					// If found stationary, add routepoint for the droplet to stay still
					if (stationary)
					{
						// Make sure a route point doesn't exist already (could have just been dispensed)
						if (lrp->cycle != c)
						{
							RoutePoint *rp = new RoutePoint();
							rp->cycle = c;
							rp->dStatus = DROP_WAIT;
							rp->x = lrp->x;
							rp->y = lrp->y;
							route->push_back(rp);
						}
					}
					else
					{
						cout << "Drop drift error at (" << dropIt->second->back()->x << ", " << dropIt->second->back()->y << ")" << endl;
						claim(false, "Drift detected. Droplet on board with no underlying or adjacent electrodes activated.");
						//return;
					}
				}
			}
		}

		// Now, check to see if any droplets have just reached the same cell;
		// If so, merge them into one droplet
		for (dropIt = routes->begin(); dropIt != routes->end(); dropIt++)
		{
			vector<RoutePoint *> *route = dropIt->second;
			RoutePoint *lrp0 = route->back(); // Last route point
			if (lrp0->dStatus == DROP_NORMAL || lrp0->dStatus == DROP_WAIT)
			{
				map<Droplet *, vector<RoutePoint *> *>::iterator dropIt2 = routes->begin();
				for (; dropIt2 != routes->end(); dropIt2++)
				{
					vector<RoutePoint *> *route2 = dropIt2->second;
					RoutePoint *lrp2 = route2->back(); // Last route point

					if(lrp0 != lrp2 && lrp0->cycle == lrp2->cycle &&
							lrp0->x == lrp2->x && lrp0->y == lrp2->y &&
							(lrp2->dStatus == DROP_NORMAL || lrp2->dStatus == DROP_WAIT))
					{
						route2->pop_back();
						delete lrp2;
						if (route2->size() > 0)
							route2->back()->dStatus = DROP_CONSUMED;
					}

				}
			}
		}

	}
}
