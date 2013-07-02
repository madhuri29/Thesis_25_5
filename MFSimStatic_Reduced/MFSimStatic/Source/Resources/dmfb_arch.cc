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
 * Source: dmfb_arch.cc															*
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
#include "../../Headers/Resources/dmfb_arch.h"

///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
DmfbArch::DmfbArch()
{
	externalResources = new vector<FixedModule *>();
	ioPorts = new vector<IoPort*>();
	numCellsX = -1;
	numCellsY = -1;
	wireRouter = NULL;
}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
DmfbArch::~DmfbArch()
{
	for (int i = 0; i < externalResources->size(); i++)
	{
		FixedModule *fm = externalResources->back();
		externalResources->pop_back();
		delete fm;
	}
	delete externalResources;

	for (int i = 0; i < ioPorts->size(); i++)
	{
		IoPort *p = ioPorts->back();
		ioPorts->pop_back();
		delete p;
	}
	delete ioPorts;

	if (pinMapper != NULL)
		delete pinMapper;

	if (wireRouter != NULL)
		delete wireRouter;

}



///////////////////////////////////////////////////////////////
// Determine if input exists
///////////////////////////////////////////////////////////////
IoPort * DmfbArch::getInput(string fluidName)
{
	for (int i=0; i < ioPorts->size(); i++)
	{
		IoPort *p = ioPorts->at(i);
		if (Util::StringToUpper(p->getPortName()) == Util::StringToUpper(fluidName))
			return p;
	}
	return NULL;
}
