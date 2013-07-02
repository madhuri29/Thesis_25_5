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
 * Name: Wire Segment															*
 *																				*
 * Details: A basic structure to represent a wire segment for connecting the	*
 * electrodes and electrode-groups to an external pin via the PCB layers.		*
 *-----------------------------------------------------------------------------*/
#ifndef WIRE_SEGMENT_H_
#define WIRE_SEGMENT_H_

#include "../entity.h"
#include "../enums.h"

class WireSegment : Entity
{
	private:
		// General identifiers and status variables
		static int next_id;


		int pinNo;
		int layer;
		WireSegType segmentType;

		// Start/stop relative locations
		int sourceXCell;
		int sourceYCell;
		int sourceXTrack;
		int sourceYTrack;
		int destXCell;
		int destYCell;
		int destXTrack;
		int destYTrack;

		// Arc parameters
		int startAngle;
		int arcAngle;


	public:
		// Constructors
		WireSegment();
		virtual ~WireSegment();

		// Getters/Setters
		int getId() { return id; }

		friend class Util;
		friend class WireRouter;
};
#endif /* WIRE_SEGMENT_H_ */
