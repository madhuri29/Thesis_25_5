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
/*--------------------------------Class Details---------------------------------*
 * Name: Synthesis																*
 *																				*
 * Details: Contains the primary public functions to be called by end-user.		*
 * EntireFlow(), Schedule(), Place() and Route() are all high level calls.		*
 * Calls all the functions that handle the file I/O for synthesis.				*
 *-----------------------------------------------------------------------------*/
#ifndef _SYNTHESIS_H
#define _SYNTHESIS_H

//#include "Scheduler/force_directed_list_scheduler.h"
//#include "Scheduler/rickett_scheduler.h"
//#include "Scheduler/genet_scheduler.h"
#include "Scheduler/list_scheduler.h"
#include "Scheduler/path_scheduler.h"
#include "Scheduler/scheduler.h"

#include "Placer/grissom_left_edge_binder.h"
//#include "Placer/sa_placer.h"
#include "Placer/placer.h"

//#include "Router/grissom_fixed_place_map_router.h"
//#include "Router/grissom_fixed_place_router.h"
#include "Router/roy_maze_router.h"
//#include "Router/bioroute_router.h"
#include "Router/router.h"

#include "PinMapper/indiv_addr_pin_mapper.h"
#include "Testing/elapsed_timer.h"
#include "Resources/dmfb_arch.h"
#include "enums.h"
#include "dag.h"

class Scheduler;
class Placer;
class Router;

class Synthesis
{
	private:
		// Methods
		void deleteAndRenewVariables();
		void synthesizeDesign();
		static Scheduler * getNewSchedulingMethod(SchedulerType st);
		static Placer * getNewPlacementMethod(PlacerType pt);
		static Router * getNewRoutingMethod(RouterType rt, DmfbArch *arch);
		static void setPinMappingMethod(PinMapType pmt, ResourceAllocationType rat, DmfbArch *arch);
		static void setWireRoutingMethod(WireRouteType wrt, DmfbArch *arch);
		static void printRoutingStats(vector<unsigned long long> *tsBeginningCycle, DmfbArch *arch);

	protected:
		// Variables
		Scheduler *scheduler;
		Placer *placer;
		Router *router;
		WireRouter *wireRouter;
		map<Droplet *, vector<RoutePoint *> *> *routes;
		vector<vector<int> *> *pinActivations;
		vector<ReconfigModule *> *rModules;
		vector<unsigned long long> *tsBeginningCycle;
		DmfbArch *arch;
		DAG *dag;
		ExecutionType executionType;
	public:
		// Constructors
		Synthesis(SchedulerType st, PlacerType pt, RouterType rt, ResourceAllocationType rat, PinMapType pmt, WireRouteType wrt, CompactionType ct, ProcessEngineType pet, ExecutionType et, DAG *assay, DmfbArch *dmfbArch);
		virtual ~Synthesis();

		// Methods
		static void EntireFlow(string assayFile, string archFile, SchedulerType st, PlacerType pt, RouterType rt, ResourceAllocationType rat, PinMapType pmt, WireRouteType wrt, CompactionType ct, ProcessEngineType pet, ExecutionType et, int maxStorageDropsPerMod, int cellsBetweenModIR, int numHorizTracks, int numVertTracks);
		static void Schedule(string inputDagFile, string inputArchFile, string outputFile, SchedulerType st, ResourceAllocationType rat, PinMapType pmt, int maxStorageDropsPerMod);
		static void Place(string inputFile, string outputFile, PlacerType pt, PinMapType pmt, int maxStorageDropsPerMod, int cellsBetweenModIR);
		static void Route(string inputFile, RouterType rt, PinMapType pmt, CompactionType ct, ProcessEngineType pet, int cellsBetweenModIR);
		static void WireRoute(string inputFile, WireRouteType wrt, int numHorizTracks, int numVertTracks);

		// Getters/Setters
		Scheduler *getScheduler() { return scheduler; }
		Placer *getPlacer() { return placer; }
		Router *getRouter() { return router; }
		WireRouter *getWireRouter() { return wireRouter; }
		DmfbArch *getArch() { return arch; }
};
#endif /* _SYNTHESIS_H */
