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
 * Source: wire_router.cc														*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: April 1, 2013								*
 *																				*
 * Details: N/A																	*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
#include "../../Headers/WireRouter/wire_router.h"

///////////////////////////////////////////////////////////////////////////////////
// Constructors
///////////////////////////////////////////////////////////////////////////////////
WireRouter::WireRouter()
{
	wireRoutesPerPin = new vector<vector<WireSegment *> *>();
	//pinCoords = new vector<vector<Coord> *>();
	numHorizTracks = 0;
	numVertTracks = 0;
}
WireRouter::WireRouter(DmfbArch *dmfbArch)
{
	arch = dmfbArch;
	wireRoutesPerPin = new vector<vector<WireSegment *> *>();
	pinCoords = new vector<vector<Coord> *>();
	numHorizTracks = 0;
	numVertTracks = 0;
}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
WireRouter::~WireRouter()
{
	while (wireRoutesPerPin->size() > 0)
	{
		vector<WireSegment *> *pinWires = wireRoutesPerPin->back();
		wireRoutesPerPin->pop_back();
		while (pinWires->size() > 0)
		{
			WireSegment *ws = pinWires->back();
			pinWires->pop_back();
			delete ws;
		}
		delete pinWires;
	}
	delete wireRoutesPerPin;

	while (pinCoords->size() > 0)
	{
		vector<Coord> *coords = pinCoords->back();
		pinCoords->pop_back();
		coords->clear();
		delete coords;
	}
	delete pinCoords;
}

///////////////////////////////////////////////////////////////////////////////////
// Given the pin-mapping (contained in the DmfbArch), computes (re-organizes) the
// electrode coordinates to which each pin must connect.
///////////////////////////////////////////////////////////////////////////////////
void WireRouter::computePinCoords()
{
	vector<vector<int> *> *pinMap = arch->getPinMapper()->getPinMapping();

	// Populate pinCoords so each index is the pin number and the item at the index
	// is a vector or coordinates corresponding to that pin number.
	for (int x = 0; x < arch->getNumCellsX(); x++)
	{
		for (int y = 0; y < arch->getNumCellsY(); y++)
		{
			Coord c;
			c.x = x;
			c.y = y;
			int pinNo = pinMap->at(x)->at(y);

			// Create the necessary arrays, if haven't yet
			for (int i = pinCoords->size(); i <= pinNo; i++)
				pinCoords->push_back(new vector<Coord>());

			pinCoords->at(pinNo)->push_back(c);
		}
	}

}

///////////////////////////////////////////////////////////////////////////////////
// Given the pin-mapping (contained in the DmfbArch), computes the wire routing
// to connect all the pins together and to the output of the device.  Also, has
// access to the pin activations list in case this algorithm needs to modify the
// pin-mapping itself.
//
// NOTE: This function will need to be over-ridden with a new wire-routing method.
// For now, it simply creates three dummy wires and spits out a message saying
// no wire-routing algorithm is implemented...really, it should spit out an error
// (uncomment the claim).
///////////////////////////////////////////////////////////////////////////////////
void WireRouter::computeWireRoutes(vector<vector<int> *> *pinActivations)
{
	//claim(false, "No valid wire-router was selected for the synthesis process or no method for 'computeWireRoutes()' was implemented for the selected wire-router.\n");

	computePinCoords();

	// Just create some dummy wire segments for testing purposes
	vector< vector<WireSegment *> *> *wires = arch->getWireRouter()->getWireRoutesPerPin();
	for (int i = 0; i < 3; i++)
	{
		vector<WireSegment *> *wire = new vector<WireSegment *>();
		WireSegment *ws = new WireSegment();
		wire->push_back(ws);
		wires->push_back(wire);

		ws->segmentType = LINE_WS;
		ws->pinNo = i;
		ws->layer = i;
		ws->sourceXCell = i;
		ws->sourceYCell = i;
		ws->sourceXTrack = i;
		ws->sourceYTrack = i;

		ws->destXCell = i;
		ws->destYCell = i+i;
		ws->destXTrack = i;
		ws->destYTrack = i;
	}

	cout << "No wire-router implemented; no wire routes generated." << endl;
}
