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
 * Source: wire_segment.cc														*
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
#include "../../Headers/WireRouter/wire_segment.h"

int WireSegment::next_id = 1;

///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
WireSegment::WireSegment()
{
	id = next_id++;

	pinNo -1;
	layer -1;
	segmentType = LINE_WS;

	// Start/stop relative locations
	sourceXCell = -1;
	sourceYCell = -1;
	sourceXTrack = -1;
	sourceYTrack = -1;
	destXCell = -1;
	destYCell = -1;
	destXTrack = -1;
	destYTrack = -1;

	// Arc parameters
	startAngle = -1;
	arcAngle = -1;
}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
WireSegment::~WireSegment()
{

}


