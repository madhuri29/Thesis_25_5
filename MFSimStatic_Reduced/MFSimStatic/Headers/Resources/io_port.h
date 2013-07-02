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
 * Name: IoPort (I/O Port)														*
 *																				*
 * Details: Represents an input or output port and holds all the necessary info	*
 * to represent this type of structure.											*
 *-----------------------------------------------------------------------------*/
#ifndef _IO_PORT_H
#define _IO_PORT_H

using namespace std;
#include "../entity.h"
#include "../enums.h"
#include <string>

class IoPort : public Entity
{
	protected:
		// Variables
		static int next_id;
		bool isInput; // True if input, false if output
		Direction ioSide; // Side of the DMFB which the I/O port resides
		int posXY; // Position along the side of the array
		double seconds; // Time it takes to dispense or output a single droplet in seconds
		string portName; // The name of the fluid (for input) or sink name (for output)
		int pinNo;

	public:
		// Constructors
		IoPort(bool input, Direction side, int pos, double timeSec, string pName);
		virtual ~IoPort();

		// Getters/Setters
		bool isAnInput() { return isInput; }
		string getPortName() { return portName; }
		double getTimeInSec() { return seconds; }
		Direction getSide() { return ioSide; }
		int getPosXY() { return posXY; }
		void setPinNo(int pn) { pinNo = pn; }
		int getPinNo() { return pinNo; }

		// Friend Classes
		friend class Util;
};
#endif /* IO_PORT_H_ */
