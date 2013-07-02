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
 * Source: placer.cc															*
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
#include "../../Headers/Placer/placer.h"

///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
Placer::Placer()
{
	dispRes = new vector<IoResource*>();
	outRes = new vector<IoResource*>();
}

///////////////////////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////////////////////
Placer::~Placer()
{
	for (int i = 0; i <= RES_TYPE_MAX; i++)
		availRes[i].clear(); // Don't delete them here, will be deleted in pin-mapper
	while (!dispRes->empty())
	{
		IoResource *dr = dispRes->back();
		dispRes->pop_back();
		delete dr;
	}
	while (!outRes->empty())
	{
		IoResource *oR = outRes->back();
		outRes->pop_back();
		delete oR;
	}
	delete dispRes;
	delete outRes;
}

///////////////////////////////////////////////////////////////////////////////////
// Generic function called to do placement.
///////////////////////////////////////////////////////////////////////////////////
void Placer::place(DmfbArch *arch, DAG *dag, vector<ReconfigModule *> *rModules)
{
	claim(false, "No valid placer was selected for the synthesis process or no method for 'place()' was implemented for the selected placer.\n");
}

///////////////////////////////////////////////////////////////////////////////////
// Resets the resource reservoirs, according to the architecture, so they can be
// used for placement.
///////////////////////////////////////////////////////////////////////////////////
void Placer::resetIoResources(DmfbArch *arch)
{
	// Now reset dispense/output resources
	while (!dispRes->empty())
	{
		IoResource *r = dispRes->back();
		dispRes->pop_back();
		delete r;
	}
	while (!outRes->empty())
	{
		IoResource *r = outRes->back();
		outRes->pop_back();
		delete r;
	}
	for (int i = 0; i < arch->getIoPorts()->size(); i++)
	{
		IoPort *iop = arch->getIoPorts()->at(i);
		if (iop->isAnInput())
		{
			IoResource *dr = new IoResource();
			dr->port = iop;
			dr->name = iop->getPortName();
			dr->lastEndTS = 0;
			dr->durationInTS = ceil((double)iop->getTimeInSec()/((double)arch->getSecPerTS()));
			dispRes->push_back(dr);
		}
		else
		{
			IoResource *outR = new IoResource();
			outR->port = iop;
			outR->name = iop->getPortName();
			outR->lastEndTS = 0;
			outR->durationInTS = ceil((double)iop->getTimeInSec()/((double)arch->getSecPerTS()));
			outRes->push_back(outR);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
// Copies resources for local use that were determined in the FPPC pin-mapper.
///////////////////////////////////////////////////////////////////////////////////
void Placer::getAvailResources(DmfbArch *arch)
{
	// Copy the module resources into the local array (Temp, DTG)
	for (int i = 0; i <= RES_TYPE_MAX; i++)
		for (int j = 0; j < arch->getPinMapper()->getAvailRes()->at(i)->size(); j++)
			availRes[i].push_back(arch->getPinMapper()->getAvailRes()->at(i)->at(j));
}
