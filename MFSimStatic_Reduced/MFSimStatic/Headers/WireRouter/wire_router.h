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
 * Type: Wire Router															*
 * Name: General Base Wire Router												*
 *																				*
 * Not inferred or detailed in any publications									*
 * 																				*
 * Details: This is a base wire-router class which contains some basic members	*
 * and functions useful to all wire-routers.  It must be inherited from when	*
 * creating a new wire-router in order to keep the structure of the simulator	*
 * and to keep the code clean and modular.										*
 *-----------------------------------------------------------------------------*/
#ifndef WIRE_ROTUER_H_
#define WIRE_ROTUER_H_

#include "../Resources/dmfb_arch.h"
#include "../structs.h"
#include "wire_segment.h"
#include <vector>
using std::vector;

class DmfbArch;

class WireRouter
{
	public:
		// Constructors
		WireRouter();
		WireRouter(DmfbArch *dmfbArch);
		virtual ~WireRouter();

		// Getters/Setters
		vector<vector<WireSegment *> *> *getWireRoutesPerPin() { return wireRoutesPerPin; }
		int getNumHorizTracks() { return numHorizTracks; }
		int getNumVertTracks() { return numVertTracks; }
		void setNumTracks(int horiz, int vert) { numHorizTracks = horiz; numVertTracks = vert; }
		void setArch(DmfbArch *da) { arch = da; }

		// Methods
		virtual void computeWireRoutes(vector<vector<int> *> *pinActivations);

	protected:
		// Members
		DmfbArch *arch;
		vector<vector<WireSegment *> *> *wireRoutesPerPin;
		vector<vector<Coord> *> *pinCoords; // List of coordinates at each pin (array index)

		// Methods
		void computePinCoords();
		int numHorizTracks;
		int numVertTracks;

};
#endif /* WIRE_ROUTING_H_ */
