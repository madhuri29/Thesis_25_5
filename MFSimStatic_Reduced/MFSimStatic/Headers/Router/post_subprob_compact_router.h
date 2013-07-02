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
/*------------------------------Algorithm Details-------------------------------*
 * Type: Router																	*
 * Name: Post SubProblem Compaction Base Router									*
 *																				*
 * Not inferred or detailed in any publications									*
 * 																				*
 * Details: This class is the sub-class to use for any router which computes	*
 * individual routes for a sub-problem and THEN compacts the sub-problem. This	*
 * is opposed to routers that coordinate multiple droplet movements 			*
 * concurrently. The main structure is already provided such that the			*
 * programmer should only need to re-implement computIndivSubProbRoutes().		*
 *-----------------------------------------------------------------------------*/

#ifndef POST_SUBPROB_COMPACT_ROUTER_H_
#define POST_SUBPROB_COMPACT_ROUTER_H_

#include "../Testing/elapsed_timer.h"
#include "router.h"

struct RotuingPoint;

class PostSubproblemCompactionRouter : public Router
{
	public:
		// Constructors
		PostSubproblemCompactionRouter();
		PostSubproblemCompactionRouter(DmfbArch *dmfbArch);
		virtual ~PostSubproblemCompactionRouter();

		// Methods
		void route(DAG *dag, DmfbArch *arch, vector<ReconfigModule *> *rModules, map<Droplet *, vector<RoutePoint *> *> *routes, vector<vector<int> *> *pinActivations, vector<unsigned long long> *tsBeginningCycle);

	protected:
		// Methods
		bool rpInterferesWithRpList(RoutePoint *rp, map<Droplet *, RoutePoint *> *rps, Droplet *d);
		void eliminateSubRouteDependencies(map<Droplet *, vector<RoutePoint *> *> *routes);
		virtual void computeIndivSupProbRoutes(vector<vector<RoutePoint *> *> *subRoutes, vector<Droplet *> *subDrops, map<Droplet *, vector<RoutePoint *> *> *routes);
		void addSubProbToGlobalRoutes(vector<Droplet *> *subDrops, vector<vector<RoutePoint *> *> *subRoutes, map<Droplet *, vector<RoutePoint *> *> *routes);
		void processFixPlaceTSQuick(map<Droplet *, vector<RoutePoint *> *> *routes);
		void processFixPlaceTSFull(map<Droplet *, vector<RoutePoint *> *> *routes);
		void equalizeGlobalRoutes(map<Droplet *, vector<RoutePoint *> *> *routes);
		void processTimeStep(map<Droplet *, vector<RoutePoint *> *> *routes);
		virtual void routerSpecificInits();
		void initCellTypeArray();
		void printSubRoutes(vector<vector<RoutePoint *> *> *subRoutes, vector<Droplet *> *subDrops);

		// Members
		vector<vector<ResourceType> *> *cellType;
		vector<AssayNode *> *thisTS;
};


#endif /* POST_SUBPROB_COMPACT_ROUTER_H_ */
