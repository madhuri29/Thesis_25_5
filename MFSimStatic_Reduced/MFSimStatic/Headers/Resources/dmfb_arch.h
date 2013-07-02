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
 * Name: DmfbArch (Dmfb Architecture)											*
 *																				*
 * Details: Contains the basic information needed to represent a DMFB			*
 * architecture such as dimensions, heating/detecting/io locations, etc.		*
 *-----------------------------------------------------------------------------*/
#ifndef _DMFB_ARCH_H
#define _DMFB_ARCH_H

#include "../PinMapper/pin_mapper.h"
#include "../WireRouter/wire_router.h"
#include "fixed_module.h"
#include "../util.h"
#include "io_port.h"
//#include <vector>

class PinMapper;
class WireRouter;

class DmfbArch
{
	protected:
		// Variables
		string name;
		int numCellsX;
		int numCellsY;
		double freqInHz;
		double secPerTS;
		vector<FixedModule *> *externalResources;
		vector<IoPort *> *ioPorts;
		PinMapper *pinMapper;
		WireRouter *wireRouter;



	public:
		// Constructors
		DmfbArch();
		virtual ~DmfbArch();

		// Getters/Setters
		int getNumCellsX() { return numCellsX; }
		int getNumCellsY() { return numCellsY; }
		double getFreqInHz() { return freqInHz; }
		double getSecPerTS() { return secPerTS; }
		vector<FixedModule *> *getExternalResources() { return externalResources; }
		vector<IoPort *> *getIoPorts() { return ioPorts; }
		IoPort *getInput(string fluidName);
		string getName() { return name; }
		void setPinMapper (PinMapper *pm) { pinMapper = pm; }
		void setWireRouter (WireRouter *wr) { wireRouter = wr; }
		PinMapper *getPinMapper () { return pinMapper; }
		WireRouter *getWireRouter() { return wireRouter; }

		// Methods

		// Friend Classes
		friend class Util;
};
#endif /* DMFB_ARCH_H_ */
