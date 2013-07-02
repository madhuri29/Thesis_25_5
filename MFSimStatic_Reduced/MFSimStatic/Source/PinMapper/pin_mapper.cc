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
 * Source: pin_mapper.cc														*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: December 11, 2012							*
 *																				*
 * Details: N/A																	*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
#include "../../Headers/PinMapper/pin_mapper.h"

///////////////////////////////////////////////////////////////////////////////////
// Constructors
///////////////////////////////////////////////////////////////////////////////////
PinMapper::PinMapper()
{
	pinMapping = new vector<vector<int> *>();
	inputPins = new vector<int>();
	outputPins = new vector<int>();
	specialPurposePins = new vector<int>();
	availResCount = new vector<int>();
	for (int i = 0; i <= RES_TYPE_MAX; i++)
		availResCount->push_back(0);
	availRes = new vector<vector<FixedModule *> *>();
	for (int i = 0; i <= RES_TYPE_MAX; i++)
		availRes->push_back(new vector<FixedModule *>());
}
PinMapper::PinMapper(DmfbArch *dmfbArch)
{
	arch = dmfbArch;
	pinMapping = new vector<vector<int> *>();
	inputPins = new vector<int>();
	outputPins = new vector<int>();
	specialPurposePins = new vector<int>();
	availResCount = new vector<int>();
	for (int i = 0; i <= RES_TYPE_MAX; i++)
		availResCount->push_back(0);
	availRes = new vector<vector<FixedModule *> *>();
	for (int i = 0; i <= RES_TYPE_MAX; i++)
		availRes->push_back(new vector<FixedModule *>());
}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
PinMapper::~PinMapper()
{
	while (pinMapping->size() > 0)
	{
		vector<int> *col = pinMapping->back();
		pinMapping->pop_back();
		delete col;
	}
	delete pinMapping;

	while (availRes->size() > 0)
	{
		vector<FixedModule *> *resources = availRes->back();
		availRes->pop_back();
		while(resources->size() > 0)
		{
			FixedModule *fm = resources->back();
			resources->pop_back();
			if (fm)
				delete fm;
		}
		delete resources;
	}
	delete availRes;

	delete availResCount;
	delete inputPins;
	delete outputPins;
	delete specialPurposePins;
}

///////////////////////////////////////////////////////////////
// Initializes the pin-mapping to a blank array of the size
// of the dimensions already specified. Array is filled with
// -1's, which denotes that the electrodes are not mapped yet.
///////////////////////////////////////////////////////////////
void PinMapper::initPinMapping()
{
	claim(arch->getNumCellsX() > 0 && arch->getNumCellsY() > 0, "DMFB dimensions must be set before initializing pin map.");

	// First, delete the current mapping...
	for (int i = 0; i < pinMapping->size(); i++)
	{
		vector<int> *col = pinMapping->back();
		pinMapping->pop_back();
		delete col;
	}

	// Then clear the i/o pins
	inputPins->clear();
	outputPins->clear();

	// ... then create a 2D-array which holds the pin numbers for each electrode
	for (int x = 0; x < arch->getNumCellsX(); x++)
	{
		vector<int> *pinCol = new vector<int>();
		for (int y = 0; y < arch->getNumCellsY(); y++)
			pinCol->push_back(-1);
		pinMapping->push_back(pinCol);
	}
}

///////////////////////////////////////////////////////////////
// Prints the pin-mapping to the console.
///////////////////////////////////////////////////////////////
void PinMapper::printPinMapping()
{
	cout << "Printing Pin-mapping:" << endl << endl;
	for (int y = 0; y < arch->getNumCellsY(); y++)
	{
		for (int x = 0; x < arch->getNumCellsX(); x++)
		{
			if (pinMapping->at(x)->at(y) >= 0)
				cout << pinMapping->at(x)->at(y);
			else
				cout << "-";

			if (x == arch->getNumCellsX()-1)
				cout << endl;
			else
				cout << "\t";
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////
// This function should be overridden to create a custom pin-mapping.
///////////////////////////////////////////////////////////////////////////////////
void PinMapper::setCustomMapping()
{
	claim(false, "No Pin-mapper has been selected.");
}

///////////////////////////////////////////////////////////////////////////////////
// This function should be overridden; it is called just before scheduling.
///////////////////////////////////////////////////////////////////////////////////
void PinMapper::setMapPreSched()
{
	initPinMapping();
	setCustomMapping();
}

///////////////////////////////////////////////////////////////////////////////////
// This function should be overridden; it is called just after routing.
///////////////////////////////////////////////////////////////////////////////////
void PinMapper::setMapPostRoute(vector<vector<int> *> *pinActivations)
{
	initPinMapping();
	setCustomMapping();
}

///////////////////////////////////////////////////////////////////////////////////
// This function should be overridden; it is called to flatten all of the special-
// purpose pin vectors for a particular pin-mapper into a single vector that
// can be written to file. It is arranged such that a single number dictating
// directly the length of the next sub-list exists before each sub-list.
//
// NOTE: Order MUST match with unflattenSpecialPurposePins().
///////////////////////////////////////////////////////////////////////////////////
void PinMapper::flattenSpecialPurposePins()
{
	claim(false, "No Pin-mapper has been selected.");
}

///////////////////////////////////////////////////////////////////////////////////
// This function should be overridden; it is called to un-flatten the single
// special-purpose pin vector into all of the special-purpose pin vectors
// (for reading from file). It is arranged such that a single number dictating
// directly the length of the next sub-list exists before each sub-list.
//
// NOTE: Order MUST match with flattenSpecialPurposePins().
///////////////////////////////////////////////////////////////////////////////////
void PinMapper::unflattenSpecialPurposePins()
{
	claim(false, "No Pin-mapper has been selected.");
}
