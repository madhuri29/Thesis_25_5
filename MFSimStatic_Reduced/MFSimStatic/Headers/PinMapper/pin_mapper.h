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
 * Type: Pin Mapper																*
 * Name: General Base Pin Mapper												*
 *																				*
 * Not inferred or detailed in any publications									*
 * 																				*
 * Details: This is a base pin-mapper class which contains some basic members	*
 * and functions useful to all pin-mappers.  It must be inherited from when		*
 * creating a new pin-mapper in order to keep the structure of the simulator	*
 * and to keep the code clean and modular.										*
 *-----------------------------------------------------------------------------*/
#ifndef PIN_MAPPER_H_
#define PIN_MAPPER_H_

#include "../Resources/fixed_module.h"
#include "../Resources/dmfb_arch.h"
#include <vector>
using std::vector;

class DmfbArch;

class PinMapper
{
	public:
		// Constructors
		PinMapper();
		PinMapper(DmfbArch *dmfbArch);
		virtual ~PinMapper();

		// Getters/Setters
		vector<vector<int> *> *getPinMapping() { return pinMapping; }
		vector<int> *getInputPins() { return inputPins; }
		vector<int> *getOutputPins() { return outputPins; }
		vector<int> *getAvailResCount() { return availResCount; }
		vector<vector<FixedModule *> *> *getAvailRes() { return availRes; }
		void setArch(DmfbArch *da) { arch = da; }
		vector<int> *getSpecialPurposePins() { return specialPurposePins; }
		ResourceAllocationType setResAllocType(ResourceAllocationType rat) { resAllocType = rat; }

		// Methods
		virtual void setCustomMapping();
		virtual void setMapPreSched();
		virtual void setMapPostRoute(vector<vector<int> *> *pinActivations);
		void initPinMapping();
		void printPinMapping();
		virtual void flattenSpecialPurposePins();
		virtual void unflattenSpecialPurposePins();


	protected:
		// Methods

		// Members
		DmfbArch *arch;
		vector<vector<int> *> *pinMapping;
		vector<int> *inputPins;
		vector<int> *outputPins;
		vector<int> *specialPurposePins;
		vector<int> *availResCount; // Tells the available module resource availability (not dispense/output info)
		vector<vector<FixedModule *> *> *availRes; // Tells the module resource availability (not dispense/output info)
		ResourceAllocationType resAllocType; // Resource allocation type

};
#endif /* PIN_MAPPER_H_ */
