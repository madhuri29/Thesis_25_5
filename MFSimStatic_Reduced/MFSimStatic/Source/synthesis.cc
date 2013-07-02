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
 * Source: synthesis.cc															*
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
#include "../Headers/synthesis.h"

///////////////////////////////////////////////////////////////
// Synthesis constructor. Initializes synthesis methods
///////////////////////////////////////////////////////////////
Synthesis::Synthesis(SchedulerType st, PlacerType pt, RouterType rt, ResourceAllocationType rat, PinMapType pmt, WireRouteType wrt, CompactionType ct, ProcessEngineType pet, ExecutionType et, DAG *assay, DmfbArch *dmfbArch)
{
	arch = dmfbArch;
	dag = assay;
	executionType = et;
	rModules = new vector<ReconfigModule *>();
	routes = new map<Droplet *, vector<RoutePoint *> *>();
	tsBeginningCycle = new vector<unsigned long long>();
	pinActivations = new vector<vector<int> *>();

	scheduler = getNewSchedulingMethod(st);
	placer = getNewPlacementMethod(pt);
	router = getNewRoutingMethod(rt, arch);
	setWireRoutingMethod(wrt, arch);
	setPinMappingMethod(pmt, rat, arch);
	router->setRoutingParams(ct, pet);
}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
Synthesis::~Synthesis()
{
	delete scheduler;
	delete placer;
	delete router;
	delete tsBeginningCycle;

	for (int i = 0; i < rModules->size(); i++)
	{
		ReconfigModule *rm = rModules->back();
		rModules->pop_back();
		delete rm;
	}
	delete rModules;

	while (!routes->empty())
	{
		Droplet *d = routes->begin()->first;
		vector<RoutePoint *> *route = routes->begin()->second;
		routes->erase(d);
		delete d;
		while (!route->empty())
		{
			RoutePoint *rp = route->back();
			route->pop_back();
			delete rp;
		}
		delete route;
	}
	delete routes;

	while (!pinActivations->empty())
	{
		vector<int> *pins = pinActivations->back();
		pinActivations->pop_back();
		pins->clear();
		delete pins;
	}
	delete pinActivations;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Simply deletes and re-news the variables in synthesis.  This function is called in-between
// scheduling, placement and routing so that the interfaces (file input and output) can be
// properly used.  This is used to ensure that the scheduling, placement and routing stages
// are properly separated from each other.
///////////////////////////////////////////////////////////////////////////////////////////////
void Synthesis::deleteAndRenewVariables()
{
	delete arch;
	delete dag;
	delete tsBeginningCycle;
	for (int i = 0; i < rModules->size(); i++)
	{
		ReconfigModule *rm = rModules->back();
		rModules->pop_back();
		delete rm;
	}
	delete rModules;
	while (!routes->empty())
	{
		Droplet *d = routes->begin()->first;
		vector<RoutePoint *> *route = routes->begin()->second;
		routes->erase(d);
		delete d;
		while (!route->empty())
		{
			RoutePoint *rp = route->back();
			route->pop_back();
			delete rp;
		}
		delete route;
	}
	delete routes;
	while (!pinActivations->empty())
	{
		vector<int> *pins = pinActivations->back();
		pinActivations->pop_back();
		pins->clear();
		delete pins;
	}


	arch = new DmfbArch();
	dag = new DAG();
	rModules = new vector<ReconfigModule *>();
	routes = new map<Droplet *, vector<RoutePoint *> *>();
	pinActivations = new vector<vector<int> *>();
	tsBeginningCycle = new vector<unsigned long long>();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// This function calls the scheduilng, placement and routing functions to compute a design
// for the given assay and DMFB architecture.  Also performs timing calculations.
///////////////////////////////////////////////////////////////////////////////////////////////
void Synthesis::synthesizeDesign()
{
	stringstream fName;
	string dir = "Output/";
	// Set the frequency for the DAG based on the architecture frequency
	for (int i = 0; i < dag->getAllNodes().size(); i++)
		dag->getAllNodes().at(i)->SetNumCycles( (unsigned)ceil((double)arch->getFreqInHz() * dag->getAllNodes().at(i)->GetNumSeconds()) );

	/////////////////////////////////////////////////////////
	// Pre-synthesis
	/////////////////////////////////////////////////////////
	if (executionType == SIM_EX || executionType == ALL_EX)
	{
		Util::WriteDagToFile(dag, "Output/0_DAG_to_SCHED.txt");
		fName.str("0_" + dag->getName());
		dag->OutputGraphFile(dir + fName.str(), true, true);
		cout << dag->getAllNodes().size() << " total nodes; " << dag->getNumNonIoNodes() << " non-I/O nodes." << endl;
	}

	/////////////////////////////////////////////////////////
	// Set Pin-mapping (if implemented)
	/////////////////////////////////////////////////////////
	arch->getPinMapper()->setMapPreSched();

	/////////////////////////////////////////////////////////
	// Scheduling ///////////////////////////////////////////
	/////////////////////////////////////////////////////////
	ElapsedTimer sTime("Scheduling Time");
	sTime.startTimer();
	scheduler->schedule(arch, dag);
	sTime.endTimer();
	sTime.printElapsedTime();

	/////////////////////////////////////////////////////////
	// Scheduling --> Placement Interface
	// NOTE: This is unnecessary, but is used to keep the
	// stages of synthesis properly separated.
	/////////////////////////////////////////////////////////
	if (executionType == SIM_EX || executionType == ALL_EX)
	{
		Util::WriteScheduledDagAndArchToFile(dag, arch, "Output/1_SCHED_to_PLACE.txt");
		//deleteAndRenewVariables();
		//Util::ReadScheduledDagAndArchFromFile(dag, arch, "Output/1_SCHED_to_PLACE.txt");
		fName.str("1_" + dag->getName() + "_Sched");
		dag->OutputGraphFile(dir + fName.str(), true, true);
	}

	/////////////////////////////////////////////////////////
	// Placement ////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	ElapsedTimer pTime("Placement Time");
	pTime.startTimer();
	placer->place(arch, dag, rModules);
	pTime.endTimer();
	pTime.printElapsedTime();

	/////////////////////////////////////////////////////////
	// Placement --> Routing Interface
	// NOTE: This is unnecessary, but is used to keep the
	// stages of synthesis properly separated.
	/////////////////////////////////////////////////////////
	if (executionType == SIM_EX || executionType == ALL_EX)
	{
		Util::WritePlacedDagAndArchToFile(dag, arch, rModules, "Output/2_PLACE_to_ROUTE.txt");
		//deleteAndRenewVariables();
		//Util::ReadPlacedDagAndArchFromFile(dag, arch, rModules, "Output/2_PLACE_to_ROUTE.txt");
		fName.str("2_" + dag->getName() + "_Placed");
		dag->OutputGraphFile(dir + fName.str(), true, true);
	}

	/////////////////////////////////////////////////////////
	// Routing //////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	ElapsedTimer rTime("Routing Time");
	rTime.startTimer();
	router->route(dag, arch, rModules, routes, pinActivations, tsBeginningCycle);
	rTime.endTimer();
	rTime.printElapsedTime();


	/////////////////////////////////////////////////////////
	// Compute droplet locations from pin-activations
	// (if required)
	/////////////////////////////////////////////////////////
	if (router->getDropLocationsFromSim())
	{
		ElapsedTimer simTime("Simulation Time");
		simTime.startTimer();
		router->simulateDropletMotion(routes, pinActivations);
		simTime.endTimer();
		simTime.printElapsedTime();
	}
	else
	{
		ElapsedTimer simTime("Pin-Compute From Droplet-Motion Time");
		simTime.startTimer();
		router->setPinActivationsFromDropletMotion(routes, pinActivations);
		simTime.endTimer();
		simTime.printElapsedTime();
	}
	printRoutingStats(tsBeginningCycle, arch);

	/////////////////////////////////////////////////////////
	// Set Pin-mapping (if implemented)
	/////////////////////////////////////////////////////////
	arch->getPinMapper()->setMapPostRoute(pinActivations);

	/////////////////////////////////////////////////////////
	// Compute wire-routes from pin-mapping and pin-activations
	// (if required)
	/////////////////////////////////////////////////////////
	ElapsedTimer wrTime("Wire-Routing Time");
	wrTime.startTimer();
	arch->getWireRouter()->computeWireRoutes(pinActivations);
	wrTime.endTimer();
	wrTime.printElapsedTime();

	/////////////////////////////////////////////////////////
	// Routing --> Output Interface
	// NOTE: Must run the WriteCompacted() function last b/c
	// it deletes the routes as it goes.
	/////////////////////////////////////////////////////////
	if (executionType == PROG_EX || executionType == ALL_EX)
		Util::WriteDmfbProgramToFile(dag, arch, rModules, routes, tsBeginningCycle, "Output/3_ELEC_ACTIVATIONS.mfprog");
	if (executionType == SIM_EX || executionType == ALL_EX)
	{
		Util::WriteRoutedDagAndArchToFile(dag, arch, rModules, routes, pinActivations, tsBeginningCycle, "Output/3_ROUTE_to_SIM.txt");
		Util::WriteCompactedRoutesToFile(dag, arch, rModules, routes, tsBeginningCycle, "Output/3_COMPACT_ROUTE_to_SIM.txt");
	}
	Util::WriteHardwareFileWithWireRoutes(arch, dir + "4_HARDWARE_DESCRIPTION.txt");
}

///////////////////////////////////////////////////////////////////////////////////////
// Runs through the entire synthesis process of scheduling, placement and routing.
// Outputs intermediate files all along the way, as well as the final droplet output.
///////////////////////////////////////////////////////////////////////////////////////
void Synthesis::EntireFlow(string assayFile, string archFile, SchedulerType st, PlacerType pt, RouterType rt, ResourceAllocationType rat, PinMapType pmt, WireRouteType wrt, CompactionType ct, ProcessEngineType pet, ExecutionType et, int maxStorageDropsPerMod, int cellsBetweenModIR, int numHorizTracks, int numVertTracks)
{
	// Read Sequencing Graph DAG from file
	DAG *dag = Util::ReadDagFromFile(assayFile);

	// Read Architectural Description File
	DmfbArch * arch = Util::ReadDmfbArchFromFile(archFile);

	// Select Synthesis Methods and synthesize/create design
	Synthesis *syn = new Synthesis(st, pt, rt, rat, pmt, wrt, ct, pet, et, dag, arch);
	syn->getScheduler()->setMaxStoragePerModule(maxStorageDropsPerMod);
	syn->getPlacer()->setMaxStoragePerModule(maxStorageDropsPerMod);
	syn->getPlacer()->setHCellsBetweenModIR(cellsBetweenModIR);
	syn->getPlacer()->setVCellsBetweenModIR(cellsBetweenModIR);
	syn->getRouter()->setHCellsBetweenModIR(cellsBetweenModIR);
	syn->getRouter()->setVCellsBetweenModIR(cellsBetweenModIR);
	syn->getArch()->getWireRouter()->setNumTracks(numHorizTracks, numVertTracks);
	syn->synthesizeDesign();

	delete syn;
	delete dag;
	delete arch;
}

///////////////////////////////////////////////////////////////////////////////////////
// Takes in a assay (in the form of a DAG) and an architecture file, schedules it via
// the specified scheduler, and outputs a scheduled DAG.
///////////////////////////////////////////////////////////////////////////////////////
void Synthesis::Schedule(string inputDagFile, string inputArchFile, string outputFile, SchedulerType st, ResourceAllocationType rat, PinMapType pmt, int maxStorageDropsPerMod)
{
	/////////////////////////////////////////////////////////
	// Pre-synthesis
	/////////////////////////////////////////////////////////
	DAG *dag = Util::ReadDagFromFile(inputDagFile);
	DmfbArch *arch = Util::ReadDmfbArchFromFile(inputArchFile);
	setPinMappingMethod(pmt, rat, arch);

	// Set the frequency for the DAG based on the architecture frequency
	for (int i = 0; i < dag->getAllNodes().size(); i++)
		dag->getAllNodes().at(i)->SetNumCycles( (unsigned)ceil((double)arch->getFreqInHz() * dag->getAllNodes().at(i)->GetNumSeconds()) );

	string dir = outputFile.substr(0, outputFile.find_last_of("/")+1);
	Util::WriteDagToFile(dag, dir + "0_DAG_to_SCHED.txt");
	stringstream fName;
	fName.str("0_" + dag->getName());
	dag->OutputGraphFile(dir + fName.str(), true, true);
	cout << dag->getAllNodes().size() << " total nodes; " << dag->getNumNonIoNodes() << " non-I/O nodes." << endl;

	/////////////////////////////////////////////////////////
	// Set Pin-mapping (if implemented)
	/////////////////////////////////////////////////////////
	arch->getPinMapper()->setMapPreSched();

	/////////////////////////////////////////////////////////
	// Scheduling ///////////////////////////////////////////
	/////////////////////////////////////////////////////////
	Scheduler *scheduler = getNewSchedulingMethod(st);
	scheduler->setMaxStoragePerModule(maxStorageDropsPerMod);
	ElapsedTimer sTime("Scheduling Time");
	sTime.startTimer();
	scheduler->schedule(arch, dag);
	sTime.endTimer();
	sTime.printElapsedTime();

	/////////////////////////////////////////////////////////
	// Scheduling --> Placement Interface
	/////////////////////////////////////////////////////////
	Util::WriteScheduledDagAndArchToFile(dag, arch, outputFile);

	/////////////////////////////////////////////////////////
	// Cleanup
	/////////////////////////////////////////////////////////
	delete scheduler;
	delete dag;
	delete arch;
}

///////////////////////////////////////////////////////////////////////////////////////
// Takes in a scheduled DAG file, places it via the specified placer, and outputs a
// placed DAG file.
///////////////////////////////////////////////////////////////////////////////////////
void Synthesis::Place(string inputFile, string outputFile, PlacerType pt, PinMapType pmt, int maxStorageDropsPerMod, int cellsBetweenModIR)
{
	/////////////////////////////////////////////////////////
	// Scheduling --> Placement Interface
	/////////////////////////////////////////////////////////
	DAG *dag = new DAG();
	DmfbArch *arch = new DmfbArch();
	setPinMappingMethod(pmt, INHERIT_RA, arch);
	vector<ReconfigModule *> *rModules = new vector<ReconfigModule *>();
	Util::ReadScheduledDagAndArchFromFile(dag, arch, inputFile);
	string dir = outputFile.substr(0, outputFile.find_last_of("/")+1);
	stringstream fName;
	fName.str("1_" + dag->getName() + "_Sched");
	dag->OutputGraphFile(dir + fName.str(), true, true);

	/////////////////////////////////////////////////////////
	// Placement ////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	Placer *placer = getNewPlacementMethod(pt);
	placer->setMaxStoragePerModule(maxStorageDropsPerMod);
	placer->setHCellsBetweenModIR(cellsBetweenModIR);
	placer->setVCellsBetweenModIR(cellsBetweenModIR);
	ElapsedTimer pTime("Placement Time");
	pTime.startTimer();
	placer->place(arch, dag, rModules);
	pTime.endTimer();
	pTime.printElapsedTime();

	/////////////////////////////////////////////////////////
	// Placement --> Routing Interface
	/////////////////////////////////////////////////////////
	Util::WritePlacedDagAndArchToFile(dag, arch, rModules, outputFile);

	/////////////////////////////////////////////////////////
	// Cleanup
	/////////////////////////////////////////////////////////
	delete placer;
	delete dag;
	delete arch;
	for (int i = 0; i < rModules->size(); i++)
	{
		ReconfigModule *rm = rModules->back();
		rModules->pop_back();
		delete rm;
	}
	delete rModules;
}

///////////////////////////////////////////////////////////////////////////////////////
// Takes in a placed DAG file, routes via the specified router, and then outputs a
// simulation
///////////////////////////////////////////////////////////////////////////////////////
void Synthesis::Route(string inputFile, RouterType rt, PinMapType pmt, CompactionType ct, ProcessEngineType pet, int cellsBetweenModIR)
{
	/////////////////////////////////////////////////////////
	// Placement --> Routing Interface
	/////////////////////////////////////////////////////////
	DAG *dag = new DAG();
	DmfbArch *arch = new DmfbArch();
	setPinMappingMethod(pmt, INHERIT_RA, arch);
	vector<ReconfigModule *> *rModules = new vector<ReconfigModule *>();
	map<Droplet *, vector<RoutePoint *> *> *routes = new map<Droplet *, vector<RoutePoint *> *>();
	vector<vector<int> *> *pinActivations = new vector<vector<int> *>();
	vector<unsigned long long> *tsBeginningCycle = new vector<unsigned long long>();
	Util::ReadPlacedDagAndArchFromFile(dag, arch, rModules, inputFile);
	string dir = inputFile.substr(0, inputFile.find_last_of("/")+1);
	stringstream fName;
	fName.str("2_" + dag->getName() + "_Placed");
	dag->OutputGraphFile(dir + fName.str(), true, true);

	/////////////////////////////////////////////////////////
	// Routing //////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	Router *router	= getNewRoutingMethod(rt, arch);
	router->setRoutingParams(ct, pet);
	router->setHCellsBetweenModIR(cellsBetweenModIR);
	router->setVCellsBetweenModIR(cellsBetweenModIR);
	router->preRoute(arch);
	ElapsedTimer rTime("Routing Time");
	rTime.startTimer();
	router->route(dag, arch, rModules, routes, pinActivations, tsBeginningCycle);
	rTime.endTimer();
	rTime.printElapsedTime();

	/////////////////////////////////////////////////////////
	// Compute droplet locations from pin-activations
	// (if required)
	/////////////////////////////////////////////////////////
	if (router->getDropLocationsFromSim())
	{
		ElapsedTimer simTime("Simulation Time");
		simTime.startTimer();
		router->simulateDropletMotion(routes, pinActivations);
		simTime.endTimer();
		simTime.printElapsedTime();
	}
	else
	{
		ElapsedTimer simTime("Pin-Compute From Droplet-Motion Time");
		simTime.startTimer();
		router->setPinActivationsFromDropletMotion(routes, pinActivations);
		simTime.endTimer();
		simTime.printElapsedTime();
	}
	printRoutingStats(tsBeginningCycle, arch);

	/////////////////////////////////////////////////////////
	// Set Pin-mapping (if implemented)
	/////////////////////////////////////////////////////////
	arch->getPinMapper()->setMapPostRoute(pinActivations);

	/////////////////////////////////////////////////////////
	// Routing --> Output Interface
	// NOTE: Must run the WriteCompacted() function last b/c
	// it deletes the routes as it goes.
	/////////////////////////////////////////////////////////
	Util::WriteRoutedDagAndArchToFile(dag, arch, rModules, routes, pinActivations, tsBeginningCycle, dir + "3_ROUTE_to_SIM.txt");
	Util::WriteCompactedRoutesToFile(dag, arch, rModules, routes, tsBeginningCycle, "Output/3_COMPACT_ROUTE_to_SIM.txt");

	/////////////////////////////////////////////////////////
	// Cleanup
	/////////////////////////////////////////////////////////
	delete router;
	delete arch;
	delete dag;
	delete tsBeginningCycle;
	for (int i = 0; i < rModules->size(); i++)
	{
		ReconfigModule *rm = rModules->back();
		rModules->pop_back();
		delete rm;
	}
	delete rModules;
	while (!routes->empty())
	{
		Droplet *d = routes->begin()->first;
		vector<RoutePoint *> *route = routes->begin()->second;
		routes->erase(d);
		delete d;
		while (!route->empty())
		{
			RoutePoint *rp = route->back();
			route->pop_back();
			delete rp;
		}
		delete route;
	}
	delete routes;
	while (!pinActivations->empty())
	{
		vector<int> *pins = pinActivations->back();
		pinActivations->pop_back();
		pins->clear();
		delete pins;
	}
	delete pinActivations;
}

void Synthesis::WireRoute(string inputFile, WireRouteType wrt, int numHorizTracks, int numVertTracks)
{
	/////////////////////////////////////////////////////////
	// Synthesis --> Wire-Routing Interface
	/////////////////////////////////////////////////////////
	DmfbArch *arch = new DmfbArch();
	// At this point, it doesn't really matter what pin-mapping type we have b/c we will
	// just read in the pin-assignment from prior sched/place/routing. Just create a
	// new pin-mapper so we can read in the pin-mapping.
	setPinMappingMethod(INDIVADDR_PM, INHERIT_RA, arch);
	vector<vector<int> *> *pinActivations = new vector<vector<int> *>();
	Util::ReadRoutedSimPinsAndArchFromFile(arch, pinActivations, inputFile);
	string dir = inputFile.substr(0, inputFile.find_last_of("/")+1);


	/////////////////////////////////////////////////////////
	// Wire-Routing /////////////////////////////////////////
	/////////////////////////////////////////////////////////
	setWireRoutingMethod(wrt, arch);
	arch->getWireRouter()->setNumTracks(numHorizTracks, numVertTracks);
	ElapsedTimer wrTime("Wire-Routing Time");
	wrTime.startTimer();
	arch->getWireRouter()->computeWireRoutes(pinActivations);
	wrTime.endTimer();
	wrTime.printElapsedTime();

	//printWireRoutingStats(arch->getWireRouter()->getSOMETHING());

	/////////////////////////////////////////////////////////
	// Hardware Description file (with wire-router)
	/////////////////////////////////////////////////////////
	Util::WriteHardwareFileWithWireRoutes(arch, dir + "4_HARDWARE_DESCRIPTION.txt");

	/////////////////////////////////////////////////////////
	// Cleanup
	/////////////////////////////////////////////////////////
	delete arch;
	while (!pinActivations->empty())
	{
		vector<int> *pins = pinActivations->back();
		pinActivations->pop_back();
		pins->clear();
		delete pins;
	}
	delete pinActivations;
}

///////////////////////////////////////////////////////////////////////////////////////
// Initializes a new scheduler
///////////////////////////////////////////////////////////////////////////////////////
Scheduler * Synthesis::getNewSchedulingMethod(SchedulerType st)
{
	if (st == LIST_S)
		return new ListScheduler();
	else if (st == PATH_S)
		return new PathScheduler();
	//else if (st == GENET_S)
		//return new GenetScheduler();
	//else if (st == RICKETT_S)
	//	return new RickettScheduler();
	//else if (st == FD_LIST_S)
	//	return new FDLScheduler();
	else
		return new Scheduler();
}
///////////////////////////////////////////////////////////////////////////////////////
// Initializes a new placer
///////////////////////////////////////////////////////////////////////////////////////
Placer * Synthesis::getNewPlacementMethod(PlacerType pt)
{
	if (pt == GRISSOM_LE_B)
		return new GrissomLEBinder();
	//else if (pt == SA_P)
	//	return new SAPlacer();
	else
		return new Placer();
}
///////////////////////////////////////////////////////////////////////////////////////
// Initializes a new router
///////////////////////////////////////////////////////////////////////////////////////
Router * Synthesis::getNewRoutingMethod(RouterType rt, DmfbArch *arch)
{
	//if (rt == GRISSOM_FIX_R)
	//	return new GrissomFixedPlaceRouter(arch);
	//else if (rt == GRISSOM_FIX_MAP_R)
	//	return new GrissomFixedPlaceMapRouter(arch);
	if (rt == ROY_MAZE_R)
		return new RoyMazeRouter(arch);
	//else if (rt == BIOROUTE_R)
	//	return new BioRouter(arch);
	else
		return new Router(arch);
}

///////////////////////////////////////////////////////////////////////////////////////
// Initializes a new router
///////////////////////////////////////////////////////////////////////////////////////
void Synthesis::setWireRoutingMethod(WireRouteType wrt, DmfbArch *arch)
{
	WireRouter *wr;
	if (wrt == NONE_WR)
		wr = new WireRouter(arch);
	else
		wr = new WireRouter(arch);

	wr->setArch(arch); // Shouldn't be necessary, but 'arch' is not "sticking" in the constructor
	arch->setWireRouter(wr);
}
///////////////////////////////////////////////////////////////////////////////////////
// Initializes a new pin-mapper
///////////////////////////////////////////////////////////////////////////////////////
void Synthesis::setPinMappingMethod(PinMapType pmt, ResourceAllocationType rat, DmfbArch *arch)
{
	PinMapper *pm;
	if (pmt == INDIVADDR_PM)
		pm = new IndivAddrPinMapper(arch);
	else
		pm = new PinMapper(arch);

	pm->setArch(arch); // Shouldn't be necessary, but 'arch' is not "sticking" in the constructor
	pm->setResAllocType(rat);
	arch->setPinMapper(pm);
}

///////////////////////////////////////////////////////////////////////////////////////
// Prints number of cycles spent routing
///////////////////////////////////////////////////////////////////////////////////////
void Synthesis::printRoutingStats(vector<unsigned long long> *tsBeginningCycle, DmfbArch *arch)
{
	unsigned long long routingCycles = 0;
	for (int i = 0; i < tsBeginningCycle->size(); i++)
	{
		unsigned long long lastTSEnd = 0;
		if (i > 0)
			lastTSEnd = tsBeginningCycle->at(i-1) + (int)(arch->getFreqInHz()*arch->getSecPerTS());
		routingCycles += (tsBeginningCycle->at(i)-lastTSEnd);
	}
	cout << "Number of cycles spent routing: " << routingCycles << " (" << (double)((double)routingCycles/arch->getFreqInHz()) << "s @" << arch->getFreqInHz() << "Hz)" << endl;
}
