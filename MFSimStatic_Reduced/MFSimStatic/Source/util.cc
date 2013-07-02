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
 * Source: util.cc																*
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
#include "../Headers/util.h"

///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
Util::Util() {}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
Util::~Util(){}

///////////////////////////////////////////////////////////////
// Converts to string with all upper-case characters
///////////////////////////////////////////////////////////////
string Util::StringToUpper(string strToConvert)
{
   for(unsigned int i=0;i<strToConvert.length();i++)
   {
	  strToConvert[i] = toupper(strToConvert[i]);
   }
   return strToConvert;//return the converted string
}

///////////////////////////////////////////////////////////////
// Counts the number of times a sub-string (sub) occurs in
// another string (str)
///////////////////////////////////////////////////////////////
int Util::countSubstring(const string& str, const string& sub)
{
	if (sub.length() == 0)
		return 0;
	int count = 0;
	for (size_t offset = str.find(sub); offset != std::string::npos; offset = str.find(sub, offset + sub.length()))
		count++;
	return count;
}

////////////////////////////////////////////////////////////////////////////
// Writes the DAG to a text file so it can be read and processed by any
// scheduler later.
////////////////////////////////////////////////////////////////////////////
void Util::WriteDagToFile(DAG *dag, string fileName)
{
	ofstream os;
	os.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly write DAG to file: " << fileName << endl;
		claim (os.good(), &str);
	}

	os << "// DAG Specification for " << dag->getName() << endl;
	if (!dag->getName().empty())
		os << "DAGNAME (" << dag->getName() << ")" << endl;
	else
		os << "DAGNAME (DAG)" << endl;

	for (int i = 0; i < dag->getAllNodes().size(); i++)
	{
		AssayNode *node = dag->getAllNodes().at(i);
		os << "NODE (";

		if (node->GetType() == DISPENSE) // Id, Type, FluidName, Volume, Name
			os << node->id << ", DISPENSE, "  << node->portName <<  ", " << node->volume << ", " << node->name << ")\n";
		else if (node->GetType() == MIX) // Id, Type, NumDropsBefore, Time (s), Name
			os << node->id << ", MIX, " << node->numDrops <<  ", " << node->seconds << ", " << node->name << ")\n";
		else if (node->GetType() == SPLIT) // Id, Type, NumDropsAfter, Time (s), Name
			os << node->id << ", SPLIT, " << node->numDrops <<  ", " << node->seconds << ", " << node->name << ")\n";
		else if (node->GetType() == HEAT)// Id, Type, Time (s), Name
			os << node->id << ", HEAT, " << node->seconds << ", " << node->name << ")\n";
		else if (node->GetType() == DETECT) // Id, Type, NumDropsIo, Time (s), Name
			os << node->id << ", DETECT, " << node->numDrops <<  ", " << node->seconds << ", " << node->name << ")\n";
		else if (node->GetType() == OUTPUT) // Id, Type, SinkName, Name
			os << node->id << ", OUTPUT, " << node->portName << ", " << node->name << ")\n";
		else if (node->GetType() == STORAGE) // Id, Type, Name
			os << node->id << ", STORAGE, " << node->name << ")\n";
		else // Id, Type, Name
			os << node->id << ", GENERAL, " << node->name << ")\n";

		for (int c = 0; c < node->children.size(); c++)
			os << "EDGE (" << node->id << ", " << node->children.at(c)->id << ")\n";
	}
	os.close();
}

////////////////////////////////////////////////////////////////////////////
// Writes the DAG to a text file so it can be read and processed by any
// scheduler later.
////////////////////////////////////////////////////////////////////////////
void Util::WriteScheduledDagAndArchToFile(DAG *dag, DmfbArch *arch, string fileName)
{
	ofstream os;
	os.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly write Scheduled DAG/Arch to file: " << fileName << endl;
		claim (os.good(), &str);
	}

	DAG *d = dag;
	DmfbArch *a = arch;

	// First output the architecture
	os << "// ARCHITECTURE Specification" << endl;
	os << "ARCHNAME (" << a->getName() << ")" << endl;
	os << "DIM (" << a->getNumCellsX() << ", " << a->getNumCellsY() << ")" << endl;
	for (int i = 0; i < a->externalResources->size(); i++)
	{
		FixedModule *fm = a->externalResources->at(i);
		os << "EXTERNAL (" << (int)fm->resourceType << ", " << fm->leftX << ", " << fm->topY << ", " << fm->rightX << ", " << fm->bottomY << ")" << endl;
		/*if (fm->resourceType == D_RES)
			os << "EXTERNAL (DETECT, " << fm->leftX << ", " << fm->topY << ", " << fm->rightX << ", " << fm->bottomY << ")" << endl;
		else if (fm->resourceType == H_RES)
			os << "EXTERNAL (HEAT, " << fm->leftX << ", " << fm->topY << ", " << fm->rightX << ", " << fm->bottomY << ")" << endl;
		else
			claim(false, "Unknown EXTERNAL module type being ouput to file.");*/
	}
	for (int i = 0; i < a->ioPorts->size(); i++)
	{
		IoPort *iop = a->ioPorts->at(i);

		if (iop->isInput)
			os << "INPUT (";
		else
			os << "OUTPUT (";

		if (iop->ioSide == NORTH)
			os << "NORTH, " << iop->posXY << ", " << iop->seconds << ", " << iop->pinNo << ", " << iop->portName << ")" << endl;
		else if (iop->ioSide == SOUTH)
			os << "SOUTH, " << iop->posXY << ", " << iop->seconds << ", " << iop->pinNo << ", " << iop->portName << ")" << endl;
		else if (iop->ioSide == EAST)
			os << "EAST, " << iop->posXY << ", " << iop->seconds << ", " << iop->pinNo << ", " << iop->portName << ")" << endl;
		else if (iop->ioSide == WEST)
			os << "WEST, " << iop->posXY << ", " << iop->seconds << ", " << iop->pinNo << ", " << iop->portName << ")" << endl;
		else
			claim(false, "Unknown IO side direction specified while outputting to file.");
	}
	os << "FREQ (" << a->freqInHz << ")" << endl;
	os << "TIMESTEP (" << a->secPerTS << ")" << endl << endl;

	os << "PINMAP (";
	for (int y = 0; y < arch->getNumCellsY(); y++)
	{
		for (int x = 0; x < arch->getNumCellsX(); x++)
		{
			os << arch->getPinMapper()->getPinMapping()->at(x)->at(y);
			if (!(x == arch->getNumCellsX()-1 && y == arch->getNumCellsY()-1))
				os << ", ";
		}
	}
	os << ")" << endl;

	arch->getPinMapper()->flattenSpecialPurposePins();
	if (arch->getPinMapper()->getSpecialPurposePins()->size() > 0)
	{
		os << "SPECIALPINS (" << arch->getPinMapper()->getSpecialPurposePins()->at(0);
		for (int i = 1; i < arch->getPinMapper()->getSpecialPurposePins()->size(); i++)
			os << ", " << arch->getPinMapper()->getSpecialPurposePins()->at(i);
		os << ")" << endl;
	}

	os << "RESOURCECOUNT (" << arch->getPinMapper()->getAvailResCount()->at(0);
	for (int i = 1; i <= RES_TYPE_MAX; i++)
		os << ", " << arch->getPinMapper()->getAvailResCount()->at(i);
	os << ")" << endl;

	os << endl << "// Locations of the resources listed above (if any)" << endl;
	vector<vector<FixedModule *> *> *ar = arch->getPinMapper()->getAvailRes();
	for (int i = 0; i < ar->size(); i++)
		for (int j = 0; j < ar->at(i)->size(); j++)
			os << "RESOURCELOCATION (" << (int)ar->at(i)->at(j)->resourceType << ", " << ar->at(i)->at(j)->leftX << ", " << ar->at(i)->at(j)->topY << ", " << ar->at(i)->at(j)->rightX << ", " << ar->at(i)->at(j)->bottomY << ", " << ar->at(i)->at(j)->tiledNum << ")" << endl;


	// Now output scheduled DAG with resource type
	os << endl << "// DAG Specification for " << dag->getName() << endl;
	if (!dag->getName().empty())
		os << "DAGNAME (" << dag->getName() << ")" << endl;
	else
		os << "DAGNAME (DAG)" << endl;

	for (int i = 0; i < dag->getAllNodes().size(); i++)
	{
		AssayNode *node = dag->getAllNodes().at(i);

		/*string resType;
		if (node->boundedResType == BASIC_RES)
			resType = "B";
		else if (node->boundedResType == D_RES)
			resType = "D";
		else if (node->boundedResType == H_RES)
			resType = "H";
		else if (node->boundedResType == DH_RES)
			resType = "DH";
		else if (node->boundedResType == SSD_RES)
			resType = "SS";*/
		//else
		//	claim(false, "Unhandled resource type when reading in SCHED-->PLACE interface file.");

		os << "NODE (";

		if (node->GetType() == DISPENSE) // Id, Type, FluidName, Volume, Name, StartTS, EndTS
			os << node->id << ", DISPENSE, " << node->portName <<  ", " << node->volume << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ")\n";
		else if (node->GetType() == MIX) // Id, Type, NumDropsBefore, Time (s), Name, StartTS, EndTS, ResourceType
			os << node->id << ", MIX, " << node->numDrops <<  ", " << node->seconds << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << (int)node->boundedResType << ")\n";
		else if (node->GetType() == SPLIT) // Id, Type, NumDropsAfter, Time (s), Name, StartTS, EndTS, ResourceType
			os << node->id << ", SPLIT, " << node->numDrops <<  ", " << node->seconds << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << (int)node->boundedResType << ")\n";
		else if (node->GetType() == HEAT)// Id, Type, Time (s), Name, StartTS, EndTS, ResourceType
			os << node->id << ", HEAT, " << node->seconds << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << (int)node->boundedResType << ")\n";
		else if (node->GetType() == DETECT) // Id, Type, NumDropsIo, Time (s), Name, StartTS, EndTS, ResourceType
			os << node->id << ", DETECT, " << node->numDrops <<  ", " << node->seconds << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << (int)node->boundedResType << ")\n";
		else if (node->GetType() == OUTPUT) // Id, Type, SinkName, Name, StartTS, EndTS
			os << node->id << ", OUTPUT, " << node->portName << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ")\n";
		else if (node->GetType() == STORAGE) // Id, Type, Name, StartTS, EndTS, ResourceType
			os << node->id << ", STORAGE, " << node->startTimeStep << ", " << node->endTimeStep << ", " << (int)node->boundedResType << ")\n";
		else // Id, Type, Name, StartTS, EndTS, ResourceType
			os << node->id << ", GENERAL, " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << (int)node->boundedResType << ")\n";

		for (int c = 0; c < node->children.size(); c++)
			os << "EDGE (" << node->id << ", " << node->children.at(c)->id << ")\n";
	}

	// Now output scheduled STORAGE_HOLDER nodes with resource type
	for (int i = 0; i < dag->getAllStorageHolders().size(); i++)
	{
		AssayNode *node = dag->getAllStorageHolders().at(i);

		/*string resType;
		if (node->boundedResType == BASIC_RES)
			resType = "B";
		else if (node->boundedResType == D_RES)
			resType = "D";
		else if (node->boundedResType == H_RES)
			resType = "H";
		else if (node->boundedResType == DH_RES)
			resType = "DH";
		else if (node->boundedResType == SSD_RES)
			resType = "SS";*/

		os << "NODE (";

		if (node->GetType() == STORAGE_HOLDER) // Type, Id, StartTS, EndTS, ResourceType
			os << node->id << ", STORAGE_HOLDER, " << node->startTimeStep << ", " << node->endTimeStep << ", " << (int)node->boundedResType << ")\n";
		else
			claim(false, "Invalid type for STORAGE_HOLDER node.");

		//for (int c = 0; c < node->children.size(); c++)
		//	os << "EDGE (" << node->id << ", " << node->children.at(c)->id << ")\n";
	}
	os.close();
}

////////////////////////////////////////////////////////////////////////////
// Reads and re-creates the DAG structure saved in the specified file so it
// can be processed by a scheduler.
////////////////////////////////////////////////////////////////////////////
DAG *Util::ReadDagFromFile(string fileName)
{
	DAG *dag = new DAG();
	map<int, AssayNode *> nodes;
	vector<int> edges;

	ifstream is;
	is.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly read DAG file: " << fileName << endl;
		claim (is.good(), &str);
	}

	string line;
	while (!is.eof())
	{
		getline(is, line);
		string tag;
		vector<string> p;
		if (!(line.empty() || line.substr(0,2) == "//"))
		{
			ParseLine(line, &tag, &p);

			tag = StringToUpper(tag);
			// Recreate Nodes and save edges for later
			if (tag == "DAGNAME")
			{
				claim(p.size() == 1, "Invalid number of parameters for DAGNAME.");
				dag->setName(p.at(0));
			}
			else if (tag == "NODE")
			{
				AssayNode *n;
				if (StringToUpper(p.at(1)) == "DISPENSE")
				{
					claim(p.size() == 5, "Invalid number of parameters for DISPENSE.");
					n = dag->AddDispenseNode(p.at(2), atof(p.at(3).c_str()), p.at(4));
				}
				else if (StringToUpper(p.at(1)) == "MIX")
				{
					claim(p.size() == 5, "Invalid number of parameters for MIX.");
					n = dag->AddMixNode(atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
				}
				else if (StringToUpper(p.at(1)) == "SPLIT")
				{
					claim(p.size() == 5, "Invalid number of parameters for SPLIT.");
					n = dag->AddSplitNode(false, atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
				}
				else if (StringToUpper(p.at(1)) == "HEAT")
				{
					claim(p.size() == 4, "Invalid number of parameters for HEAT.");
					n = dag->AddHeatNode(atof(p.at(2).c_str()), p.at(3));
				}
				else if (StringToUpper(p.at(1)) == "DETECT")
				{
					claim(p.size() == 5, "Invalid number of parameters for DETECT.");
					n = dag->AddDetectNode(atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
				}
				else if (StringToUpper(p.at(1)) == "OUTPUT")
				{
					claim(p.size() == 4, "Invalid number of parameters for OUTPUT.");
					n = dag->AddOutputNode(p.at(2), p.at(3));
				}
				else if (StringToUpper(p.at(1)) == "STORAGE")
				{
					claim(p.size() == 3, "Invalid number of parameters for STORAGE.");
					n = dag->AddStorageNode(p.at(2));
				}
				else // GENERAL
				{
					claim(p.size() == 3, "Invalid number of parameters for GENERAL.");
					n = dag->AddGeneralNode(p.at(2));
				}
				n->id = atoi(p.at(0).c_str());
				nodes[n->id] = n;
			}
			else if (tag == "EDGE")
			{
				claim(p.size() == 2, "Invalid number of parameters for an EDGE.");
				edges.push_back(atoi(p.at(0).c_str()));
				edges.push_back(atoi(p.at(1).c_str()));
			}
			else
			{
				stringstream ss;
				ss << "Invalid tag type: " << tag << endl;
				claim(false, &ss);
			}
		}
	}

	// Recreate Edges
	while (!edges.empty())
	{
		dag->ParentChild(nodes[edges[0]], nodes[edges[1]]);
		edges.erase(edges.begin());
		edges.erase(edges.begin());
	}

	is.close();
	return dag;
}


////////////////////////////////////////////////////////////////////////////
// Reads and re-creates the DmfbArch structure saved in the specified file
// so it can be processed used during synthesis to know the target DMFB.
////////////////////////////////////////////////////////////////////////////
DmfbArch *Util::ReadDmfbArchFromFile(string fileName)
{
	DmfbArch *arch = new DmfbArch;

	ifstream is;
	is.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly read DMFB Architecture file: " << fileName << endl;
		claim (is.good(), &str);
	}

	string line;
	while (!is.eof())
	{
		getline(is, line);
		string tag;
		vector<string> p;
		if (!(line.empty() || line.substr(0,2) == "//"))
		{
			ParseLine(line, &tag, &p);
			tag = StringToUpper(tag);
			// Recreates Architecture Details
			if (tag == "ARCHNAME")
			{
				claim(p.size() == 1, "Invalid number of parameters for ARCHNAME.");
				arch->name = p.at(0);
			}
			else if (tag == "DIM")
			{
				claim(p.size() == 2, "Invalid number of parameters for dimension (DIM) tag.");
				arch->numCellsX = atoi(p.at(0).c_str());
				arch->numCellsY = atoi(p.at(1).c_str());
			}
			else if (tag == "FREQ")
			{
				claim(p.size() == 1, "Invalid number of parameters for FREQ tag.");
				arch->freqInHz = atof(p.at(0).c_str());
			}
			else if (tag == "TIMESTEP")
			{
				claim(p.size() == 1, "Invalid number of parameters for timestep (s) tag.");
				arch->secPerTS = atof(p.at(0).c_str());
			}
			else if (tag == "EXTERNAL")
			{
				claim(p.size() == 5, "Invalid number of parameters for EXTERNAL module/resource.");
				if (StringToUpper(p.at(0)) == "HEAT")
					arch->externalResources->push_back(new FixedModule(H_RES, atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				else if (StringToUpper(p.at(0)) == "DETECT")
					arch->externalResources->push_back(new FixedModule(D_RES, atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				else // GENERAL
					claim(false, "Invalid type of EXTERNAL module.");
			}
			else if (tag == "INPUT")
			{
				claim(p.size() == 4, "Invalid number of parameters for an INPUT.");
				if (StringToUpper(p.at(0)) == "NORTH")
					arch->ioPorts->push_back(new IoPort(true, NORTH, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(3)));
				else if (StringToUpper(p.at(0)) == "SOUTH")
					arch->ioPorts->push_back(new IoPort(true, SOUTH, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(3)));
				else if (StringToUpper(p.at(0)) == "EAST")
					arch->ioPorts->push_back(new IoPort(true, EAST, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(3)));
				else if (StringToUpper(p.at(0)) == "WEST")
					arch->ioPorts->push_back(new IoPort(true, WEST, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(3)));
				else
					claim(false, "Invalid side for an INPUT.");
			}
			else if (tag == "OUTPUT")
			{
				claim(p.size() == 4, "Invalid number of parameters for an OUTPUT.");
				if (StringToUpper(p.at(0)) == "NORTH")
					arch->ioPorts->push_back(new IoPort(false, NORTH, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(3)));
				else if (StringToUpper(p.at(0)) == "SOUTH")
					arch->ioPorts->push_back(new IoPort(false, SOUTH, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(3)));
				else if (StringToUpper(p.at(0)) == "EAST")
					arch->ioPorts->push_back(new IoPort(false, EAST, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(3)));
				else if (StringToUpper(p.at(0)) == "WEST")
					arch->ioPorts->push_back(new IoPort(false, WEST, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(3)));
				else
					claim(false, "Invalid side for an OUTPUT.");
			}
			else
			{
				stringstream ss;
				ss << "Invalid tag type: " << tag << endl;
				claim(false, &ss);
			}
		}
	}

	is.close();
	return arch;
}

////////////////////////////////////////////////////////////////////////////
// Reads and re-creates the DAG structure and ARCH structure saved in the
// specified file so it can be processed by a placer.
////////////////////////////////////////////////////////////////////////////
void Util::ReadScheduledDagAndArchFromFile(DAG *dag, DmfbArch *arch, string fileName)
{
	map<int, AssayNode*> nodes;
	vector<int> edges;

	ifstream is;
	is.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly read Scheduled DAG/Arch file: " << fileName << endl;
		claim (is.good(), &str);
	}

	string line;
	while (!is.eof())
	{
		getline(is, line);
		string tag;
		vector<string> p;
		if (!(line.empty() || line.substr(0,2) == "//"))
		{
			ParseLine(line, &tag, &p);
			tag = StringToUpper(tag);

			// Recreate Nodes and save edges for later
			if (tag == "ARCHNAME")
			{
				claim(p.size() == 1, "Invalid number of parameters for ARCHNAME.");
				arch->name = p.at(0);
			}
			else if (tag == "DIM")
			{
				claim(p.size() == 2, "Invalid number of parameters for dimension (DIM) tag.");
				arch->numCellsX = atoi(p.at(0).c_str());
				arch->numCellsY = atoi(p.at(1).c_str());
				arch->getPinMapper()->initPinMapping();
			}
			else if (tag == "FREQ")
			{
				claim(p.size() == 1, "Invalid number of parameters for FREQ tag.");
				arch->freqInHz = atof(p.at(0).c_str());
			}
			else if (tag == "TIMESTEP")
			{
				claim(p.size() == 1, "Invalid number of parameters for timestep (s) tag.");
				arch->secPerTS = atof(p.at(0).c_str());
			}
			else if (tag == "EXTERNAL")
			{
				claim(p.size() == 5, "Invalid number of parameters for EXTERNAL module/resource.");
				/*if ((ResourceType)atoi(p.at(0).c_str()) == H_RES)
					arch->fixedModules->push_back(new FixedModule(H_RES, atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				else if ((ResourceType)atoi(p.at(0).c_str()) == D_RES)
					arch->fixedModules->push_back(new FixedModule(D_RES, atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				else // GENERAL
					claim(false, "Invalid type of fixed module.");*/
				arch->externalResources->push_back(new FixedModule((ResourceType)atoi(p.at(0).c_str()), atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));

			}
			else if (tag == "INPUT")
			{
				claim(p.size() == 5, "Invalid number of parameters for an INPUT.");
				if (StringToUpper(p.at(0)) == "NORTH")
					arch->ioPorts->push_back(new IoPort(true, NORTH, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(4)));
				else if (StringToUpper(p.at(0)) == "SOUTH")
					arch->ioPorts->push_back(new IoPort(true, SOUTH, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(4)));
				else if (StringToUpper(p.at(0)) == "EAST")
					arch->ioPorts->push_back(new IoPort(true, EAST, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(4)));
				else if (StringToUpper(p.at(0)) == "WEST")
					arch->ioPorts->push_back(new IoPort(true, WEST, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(4)));
				else
					claim(false, "Invalid side for an INPUT.");
				arch->ioPorts->back()->setPinNo(atoi(p.at(3).c_str()));
			}
			else if (tag == "OUTPUT")
			{
				claim(p.size() == 5, "Invalid number of parameters for an OUTPUT.");
				if (StringToUpper(p.at(0)) == "NORTH")
					arch->ioPorts->push_back(new IoPort(false, NORTH, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(4)));
				else if (StringToUpper(p.at(0)) == "SOUTH")
					arch->ioPorts->push_back(new IoPort(false, SOUTH, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(4)));
				else if (StringToUpper(p.at(0)) == "EAST")
					arch->ioPorts->push_back(new IoPort(false, EAST, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(4)));
				else if (StringToUpper(p.at(0)) == "WEST")
					arch->ioPorts->push_back(new IoPort(false, WEST, atoi(p.at(1).c_str()), atof(p.at(2).c_str()), p.at(4)));
				else
					claim(false, "Invalid side for an OUTPUT.");
				arch->ioPorts->back()->setPinNo(atoi(p.at(3).c_str()));
			}
			else if (tag == "DAGNAME")
			{
				claim(p.size() == 1, "Invalid number of parameters for DAGNAME.");
				dag->setName(p.at(0));
			}
			else if (tag == "NODE")
			{
				/*ResourceType rt;
				if (p.back() == "B")
					rt = BASIC_RES;
				else if (p.back() == "D")
					rt = D_RES;
				else if (p.back() == "H")
					rt = H_RES;
				else if (p.back() == "DH")
					rt = DH_RES;
				else if (p.back() == "SS")
					rt = SSD_RES;
				else
					rt = UNKNOWN_RES;
					*/

				AssayNode *n;
				if (StringToUpper(p.at(1)) == "DISPENSE")
				{
					claim(p.size() == 7, "Invalid number of parameters for DISPENSE.");
					n = dag->AddDispenseNode(p.at(2), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
				}
				else if (StringToUpper(p.at(1)) == "MIX")
				{
					claim(p.size() == 8, "Invalid number of parameters for MIX.");
					n = dag->AddMixNode(atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
					n->boundedResType = (ResourceType)atoi(p.back().c_str());//rt;
				}
				else if (StringToUpper(p.at(1)) == "SPLIT")
				{
					claim(p.size() == 8, "Invalid number of parameters for SPLIT.");
					n = dag->AddSplitNode(false, atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
					n->boundedResType = (ResourceType)atoi(p.back().c_str());
				}
				else if (StringToUpper(p.at(1)) == "HEAT")
				{
					claim(p.size() == 7, "Invalid number of parameters for HEAT.");
					n = dag->AddHeatNode(atof(p.at(2).c_str()), p.at(3));
					n->startTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->boundedResType = (ResourceType)atoi(p.back().c_str());
				}
				else if (StringToUpper(p.at(1)) == "DETECT")
				{
					claim(p.size() == 8, "Invalid number of parameters for DETECT.");
					n = dag->AddDetectNode(atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
					n->boundedResType = (ResourceType)atoi(p.back().c_str());
				}
				else if (StringToUpper(p.at(1)) == "OUTPUT")
				{
					claim(p.size() == 6, "Invalid number of parameters for OUTPUT.");
					n = dag->AddOutputNode(p.at(2), p.at(3));
					n->startTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
				}
				else if (StringToUpper(p.at(1)) == "STORAGE")
				{
					claim(p.size() == 5, "Invalid number of parameters for STORAGE.");
					n = dag->AddStorageNode();
					n->startTimeStep = strtoul(p.at(2).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(3).c_str(), NULL, 0);
					n->boundedResType = (ResourceType)atoi(p.back().c_str());
				}
				else if (StringToUpper(p.at(1)) == "STORAGE_HOLDER")
				{
					claim(p.size() == 5, "Invalid number of parameters for STORAGE_HOLDER.");
					n = dag->AddStorageHolderNode();
					n->startTimeStep = strtoul(p.at(2).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(3).c_str(), NULL, 0);
					n->boundedResType = (ResourceType)atoi(p.back().c_str());
				}
				else // GENERAL
				{
					claim(p.size() == 6, "Invalid number of parameters for GENERAL.");
					n = dag->AddGeneralNode(p.at(2));
					n->startTimeStep = strtoul(p.at(3).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
					n->boundedResType = (ResourceType)atoi(p.back().c_str());
				}
				n->id = atoi(p.at(0).c_str());
				n->status = SCHEDULED;
				nodes[n->id] = n;
			}
			else if (tag == "EDGE")
			{
				claim(p.size() == 2, "Invalid number of parameters for an EDGE.");
				edges.push_back(atoi(p.at(0).c_str()));
				edges.push_back(atoi(p.at(1).c_str()));
			}
			else if (tag == "PINMAP")
			{
				claim(p.size() == arch->numCellsX*arch->numCellsY, "PinMap should be the same size of the array.");
				int i = 0;
				for (int y = 0; y < arch->numCellsY; y++)
					for (int x = 0; x < arch->numCellsX; x++)
						arch->pinMapper->getPinMapping()->at(x)->at(y) = atoi(p.at(i++).c_str());
			}
			else if (tag == "SPECIALPINS")
			{
				for (int i = 0; i < p.size(); i++)
					arch->pinMapper->getSpecialPurposePins()->push_back(atoi(p.at(i).c_str()));
				arch->getPinMapper()->unflattenSpecialPurposePins();
			}
			else if (tag == "RESOURCECOUNT")
			{
				for (int i = 0; i < p.size(); i++)
					arch->pinMapper->getAvailResCount()->at(i) = atoi(p.at(i).c_str());
			}
			else if (tag == "RESOURCELOCATION")
			{
				arch->pinMapper->getAvailRes()->at((ResourceType)atoi(p.at(0).c_str()))->push_back(new FixedModule((ResourceType)atoi(p.at(0).c_str()), atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				arch->pinMapper->getAvailRes()->at((ResourceType)atoi(p.at(0).c_str()))->back()->setTileNum(atoi(p.at(5).c_str()));
			}
			else
			{
				stringstream ss;
				ss << "Invalid tag type: " << tag << endl;
				claim(false, &ss);
			}
		}
	}

	// Recreate Edges
	while (!edges.empty())
	{
		dag->ParentChild(nodes[edges[0]], nodes[edges[1]]);
		//dag->ParentChild(dag->getAllNodes().at(edges.at(0)-1), dag->getAllNodes().at(edges.at(1)-1));
		edges.erase(edges.begin());
		edges.erase(edges.begin());
	}

	is.close();
}

////////////////////////////////////////////////////////////////////////////
// Writes the DAG to a text file so it can be read and processed by any
// router later.
////////////////////////////////////////////////////////////////////////////
void Util::WritePlacedDagAndArchToFile(DAG *dag, DmfbArch *arch, vector<ReconfigModule *> *rModules, string fileName)
{
	ofstream os;
	os.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly write Placed DAG/Arch to file: " << fileName << endl;
		claim (os.good(), &str);
	}

	DAG *d = dag;
	DmfbArch *a = arch;

	// First output the architecture
	os << "// ARCHITECTURE Specification" << endl;
	os << "ARCHNAME (" << a->getName() << ")" << endl;
	os << "DIM (" << a->getNumCellsX() << ", " << a->getNumCellsY() << ")" << endl;
	for (int i = 0; i < a->externalResources->size(); i++)
	{
		FixedModule *fm = a->externalResources->at(i);
		os << "EXTERNAL (" << (int)fm->resourceType << ", " << fm->leftX << ", " << fm->topY << ", " << fm->rightX << ", " << fm->bottomY << ")" << endl;
		/*if (fm->resourceType == DETECT)
			os << "EXTERNAL (DETECT, " << fm->leftX << ", " << fm->topY << ", " << fm->rightX << ", " << fm->bottomY << ")" << endl;
		else if (fm->resourceType == HEAT)
			os << "EXTERNAL (HEAT, " << fm->leftX << ", " << fm->topY << ", " << fm->rightX << ", " << fm->bottomY << ")" << endl;
		else
			claim(false, "Unknown EXTERNAL module type being ouput to file.");*/
	}
	for (int i = 0; i < a->ioPorts->size(); i++)
	{
		IoPort *iop = a->ioPorts->at(i);

		if (iop->isInput)
			os << "INPUT (";
		else
			os << "OUTPUT (";

		if (iop->ioSide == NORTH)
			os << iop->id << ", NORTH, " << iop->posXY << ", " << iop->seconds << ", " << iop->pinNo << ", " << iop->portName << ")" << endl;
		else if (iop->ioSide == SOUTH)
			os << iop->id << ", SOUTH, " << iop->posXY << ", " << iop->seconds << ", " << iop->pinNo << ", " << iop->portName << ")" << endl;
		else if (iop->ioSide == EAST)
			os << iop->id << ", EAST, " << iop->posXY << ", " << iop->seconds << ", " << iop->pinNo << ", " << iop->portName << ")" << endl;
		else if (iop->ioSide == WEST)
			os << iop->id << ", WEST, " << iop->posXY << ", " << iop->seconds << ", " << iop->pinNo << ", " << iop->portName << ")" << endl;
		else
			claim(false, "Unknown IO side direction specified while outputting to file.");
	}
	os << "FREQ (" << a->freqInHz << ")" << endl;
	os << "TIMESTEP (" << a->secPerTS << ")" << endl << endl;

	os << "PINMAP (";
	for (int y = 0; y < arch->getNumCellsY(); y++)
	{
		for (int x = 0; x < arch->getNumCellsX(); x++)
		{
			os << arch->getPinMapper()->getPinMapping()->at(x)->at(y);
			if (!(x == arch->getNumCellsX()-1 && y == arch->getNumCellsY()-1))
				os << ", ";
		}
	}
	os << ")" << endl;

	arch->getPinMapper()->flattenSpecialPurposePins();
	if (arch->getPinMapper()->getSpecialPurposePins()->size() > 0)
	{
		os << "SPECIALPINS (" << arch->getPinMapper()->getSpecialPurposePins()->at(0);
		for (int i = 1; i < arch->getPinMapper()->getSpecialPurposePins()->size(); i++)
			os << ", " << arch->getPinMapper()->getSpecialPurposePins()->at(i);
		os << ")" << endl;
	}

	os << "RESOURCECOUNT (" << arch->getPinMapper()->getAvailResCount()->at(0);
	for (int i = 1; i <= RES_TYPE_MAX; i++)
		os << ", " << arch->getPinMapper()->getAvailResCount()->at(i);
	os << ")" << endl;

	os << endl << "// Locations of the resources listed above (if any)" << endl;
	vector<vector<FixedModule *> *> *ar = arch->getPinMapper()->getAvailRes();
	for (int i = 0; i < ar->size(); i++)
		for (int j = 0; j < ar->at(i)->size(); j++)
			os << "RESOURCELOCATION (" << (int)ar->at(i)->at(j)->resourceType << ", " << ar->at(i)->at(j)->leftX << ", " << ar->at(i)->at(j)->topY << ", " << ar->at(i)->at(j)->rightX << ", " << ar->at(i)->at(j)->bottomY << ", " << ar->at(i)->at(j)->tiledNum << ")" << endl;


	// Now output the reconfigurable modules
	os << endl << "// Reconfigurable Modules" << endl;
	for (int i = 0; i < rModules->size(); i++)
	{
		ReconfigModule *rm = rModules->at(i);

		string opType = "";
		if (rm->boundNode == NULL)
			opType = getOperationString(GENERAL);
		else
			opType = getOperationString(rm->boundNode->type);

		os << "RECONFIG (" << rm->id << ", " << opType << ", " << (int)rm->resourceType << ", " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << rm->startTimeStep << ", " << rm->endTimeStep << ", " << rm->tiledNum << ")" << endl;
		/*if (rm->resourceType == MIX)
			os << "RECONFIG (" << rm->id << ", " << "MIX, " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << rm->startTimeStep << ", " << rm->endTimeStep << ")" << endl;
		else if (rm->resourceType == SPLIT)
			os << "RECONFIG (" << rm->id << ", " << "SPLIT, " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << rm->startTimeStep << ", " << rm->endTimeStep << ")" << endl;
		else if (rm->resourceType == HEAT)
			os << "RECONFIG (" << rm->id << ", " << "HEAT, " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << rm->startTimeStep << ", " << rm->endTimeStep << ")" << endl;
		else if (rm->resourceType == DETECT)
			os << "RECONFIG (" << rm->id << ", " << "DETECT, " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << rm->startTimeStep << ", " << rm->endTimeStep << ")" << endl;
		else if (rm->resourceType == STORAGE)
			os << "RECONFIG (" << rm->id << ", " << "STORAGE, " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << rm->startTimeStep << ", " << rm->endTimeStep << ")" << endl;
		else if (rm->resourceType == STORAGE_HOLDER)
			os << "RECONFIG (" << rm->id << ", " << "STORAGE_HOLDER, " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << rm->startTimeStep << ", " << rm->endTimeStep << ")" << endl;
		else if (rm->resourceType == GENERAL)
			os << "RECONFIG (" << rm->id << ", " << "GENERAL, " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << rm->startTimeStep << ", " << rm->endTimeStep << ")" << endl;
		else
			claim(false, "Unknown reconfigurable module type.");*/
	}
	os << endl;

	// Now output scheduled DAG with resource bindings to specific IoPorts or Reconfigurable Modules
	os << "// DAG Specification for " << dag->getName() << endl;
	if (!dag->getName().empty())
		os << "DAGNAME (" << dag->getName() << ")" << endl;
	else
		os << "DAGNAME (DAG)" << endl;

	for (int i = 0; i < dag->getAllNodes().size(); i++)
	{
		AssayNode *node = dag->getAllNodes().at(i);

		string resType;
		if (node->boundedResType == BASIC_RES)
			resType = "B";
		else if (node->boundedResType == D_RES)
			resType = "D";
		else if (node->boundedResType == H_RES)
			resType = "H";
		else if (node->boundedResType == DH_RES)
			resType = "DH";

		os << "NODE (";

		if (node->GetType() == DISPENSE) // Id, Type, FluidName, Volume, Name, StartTS, EndTS, IoPortId
			os << node->id << ", DISPENSE, " << node->portName <<  ", " << node->volume << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << node->ioPort->id << ")\n";
		else if (node->GetType() == MIX) // Id, Type, NumDropsBefore, Time (s), Name, StartTS, EndTS, ReconfigModId
			os << node->id << ", MIX, " << node->numDrops <<  ", " << node->seconds << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << node->reconfigMod->id << ")\n";
		else if (node->GetType() == SPLIT) // Id, Type, NumDropsAfter, Time (s), Name, StartTS, EndTS, ReconfigModId
			os << node->id << ", SPLIT, " << node->numDrops <<  ", " << node->seconds << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << node->reconfigMod->id << ")\n";
		else if (node->GetType() == HEAT)// Id, Type, Time (s), Name, StartTS, EndTS, ReconfigModId
			os << node->id << ", HEAT, " << node->seconds << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << node->reconfigMod->id << ")\n";
		else if (node->GetType() == DETECT) // Id, Type, NumDropsIo, Time (s), Name, StartTS, EndTS, ReconfigModId
			os << node->id << ", DETECT, " << node->numDrops <<  ", " << node->seconds << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << node->reconfigMod->id << ")\n";
		else if (node->GetType() == OUTPUT) // Id, Type, SinkName, Name, StartTS, EndTS, IoPortId
			os << node->id << ", OUTPUT, " << node->portName << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << node->ioPort->id << ")\n";
		else if (node->GetType() == STORAGE) // Id, Type, StartTS, EndTS, ReconfigModId
			os << node->id << ", STORAGE, " << node->startTimeStep << ", " << node->endTimeStep << ", " << node->reconfigMod->id << ")\n";
		else // Id, Type, Name, StartTS, EndTS, ReconfigModId
			os << node->id << ", GENERAL, " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << node->reconfigMod->id << ")\n";

		for (int c = 0; c < node->children.size(); c++)
			os << "EDGE (" << node->id << ", " << node->children.at(c)->id << ")\n";
	}
	os.close();
}

////////////////////////////////////////////////////////////////////////////
// Reads and re-creates the DAG structure and ARCH structure saved in the
// specified file so it can be processed by a placer.
////////////////////////////////////////////////////////////////////////////
void Util::ReadPlacedDagAndArchFromFile(DAG *dag, DmfbArch *arch, vector<ReconfigModule *> *rModules, string fileName)
{
	map<int, AssayNode*> nodes;
	map<int, IoPort*> ports;
	map<int, ReconfigModule*> rMods;
	vector<int> edges;

	ifstream is;
	is.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly read Placed DAG/Arch file: " << fileName << endl;
		claim (is.good(), &str);
	}

	string line;
	while (!is.eof())
	{
		getline(is, line);
		string tag;
		vector<string> p;
		if (!(line.empty() || line.substr(0,2) == "//"))
		{
			ParseLine(line, &tag, &p);
			tag = StringToUpper(tag);

			// Recreate Nodes and save edges for later
			if (tag == "ARCHNAME")
			{
				claim(p.size() == 1, "Invalid number of parameters for ARCHNAME.");
				arch->name = p.at(0);
			}
			else if (tag == "DIM")
			{
				claim(p.size() == 2, "Invalid number of parameters for dimension (DIM) tag.");
				arch->numCellsX = atoi(p.at(0).c_str());
				arch->numCellsY = atoi(p.at(1).c_str());
				arch->getPinMapper()->initPinMapping();
			}
			else if (tag == "FREQ")
			{
				claim(p.size() == 1, "Invalid number of parameters for FREQ tag.");
				arch->freqInHz = atof(p.at(0).c_str());
			}
			else if (tag == "TIMESTEP")
			{
				claim(p.size() == 1, "Invalid number of parameters for timestep (s) tag.");
				arch->secPerTS = atof(p.at(0).c_str());
			}
			else if (tag == "EXTERNAL")
			{
				claim(p.size() == 5, "Invalid number of parameters for EXTERNAL module/resource.");
				/*if (StringToUpper(p.at(0)) == "HEAT")
					arch->fixedModules->push_back(new FixedModule(H_RES, atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				else if (StringToUpper(p.at(0)) == "DETECT")
					arch->fixedModules->push_back(new FixedModule(D_RES, atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				else // GENERAL
					claim(false, "Invalid type of EXTERNAL module.");*/
				arch->externalResources->push_back(new FixedModule((ResourceType)atoi(p.at(0).c_str()), atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
			}
			else if (tag == "RECONFIG")
			{
				claim(p.size() == 10, "Invalid number of parameters for reconfigurable module/resource.");
				ReconfigModule *rm = new ReconfigModule((ResourceType)atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str()), atoi(p.at(5).c_str()), atoi(p.at(6).c_str()));
				rm->id = atoi(p.at(0).c_str());
				rm->startTimeStep = strtoul(p.at(7).c_str(), NULL, 0);
				rm->endTimeStep = strtoul(p.at(8).c_str(), NULL, 0);
				rm->tiledNum = atoi(p.at(9).c_str());
				//rm->boundNode->type = getOpTypeFromString(p.at(1).c_str());

				rMods[rm->id] = rm;
			}
			else if (tag == "INPUT")
			{
				claim(p.size() == 6, "Invalid number of parameters for an INPUT.");
				Direction dir = DIR_UNINIT;
				if (StringToUpper(p.at(1)) == "NORTH")
					dir = NORTH;
				else if (StringToUpper(p.at(1)) == "SOUTH")
					dir = SOUTH;
				else if (StringToUpper(p.at(1)) == "EAST")
					dir = EAST;
				else if (StringToUpper(p.at(1)) == "WEST")
					dir = WEST;
				else
					claim(false, "Invalid side for an INPUT.");

				IoPort *iop = new IoPort(true, dir, atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(5));
				iop->setPinNo(atoi(p.at(4).c_str()));
				iop->id = atoi(p.at(0).c_str());
				ports[iop->id] = iop;
			}
			else if (tag == "OUTPUT")
			{
				claim(p.size() == 6, "Invalid number of parameters for an OUTPUT.");
				Direction dir = DIR_UNINIT;
				if (StringToUpper(p.at(1)) == "NORTH")
					dir = NORTH;
				else if (StringToUpper(p.at(1)) == "SOUTH")
					dir = SOUTH;
				else if (StringToUpper(p.at(1)) == "EAST")
					dir = EAST;
				else if (StringToUpper(p.at(1)) == "WEST")
					dir = WEST;
				else
					claim(false, "Invalid side for an OUTPUT.");

				IoPort *iop = new IoPort(false, dir, atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(5));
				iop->setPinNo(atoi(p.at(4).c_str()));
				iop->id = atoi(p.at(0).c_str());
				ports[iop->id] = iop;
			}
			else if (tag == "DAGNAME")
			{
				claim(p.size() == 1, "Invalid number of parameters for DAGNAME.");
				dag->setName(p.at(0));
			}
			else if (tag == "NODE")
			{
				AssayNode *n;
				if (StringToUpper(p.at(1)) == "DISPENSE")
				{
					claim(p.size() == 8, "Invalid number of parameters for DISPENSE.");
					n = dag->AddDispenseNode(p.at(2), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
					n->ioPort = ports[atoi(p.at(7).c_str())];
				}
				else if (StringToUpper(p.at(1)) == "MIX")
				{
					claim(p.size() == 8, "Invalid number of parameters for MIX.");
					n = dag->AddMixNode(atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
					n->reconfigMod = rMods[atoi(p.at(7).c_str())];
					n->reconfigMod->boundNode = n;
					n->boundedResType = n->reconfigMod->resourceType;
				}
				else if (StringToUpper(p.at(1)) == "SPLIT")
				{
					claim(p.size() == 8, "Invalid number of parameters for SPLIT.");
					n = dag->AddSplitNode(false, atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
					n->reconfigMod = rMods[atoi(p.at(7).c_str())];
					n->reconfigMod->boundNode = n;
					n->boundedResType = n->reconfigMod->resourceType;
				}
				else if (StringToUpper(p.at(1)) == "HEAT")
				{
					claim(p.size() == 7, "Invalid number of parameters for HEAT.");
					n = dag->AddHeatNode(atof(p.at(2).c_str()), p.at(3));
					n->startTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->reconfigMod = rMods[atoi(p.at(6).c_str())];
					n->reconfigMod->boundNode = n;
					n->boundedResType = n->reconfigMod->resourceType;
				}
				else if (StringToUpper(p.at(1)) == "DETECT")
				{
					claim(p.size() == 8, "Invalid number of parameters for DETECT.");
					n = dag->AddDetectNode(atoi(p.at(2).c_str()), atof(p.at(3).c_str()), p.at(4));
					n->startTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(6).c_str(), NULL, 0);
					n->reconfigMod = rMods[atoi(p.at(7).c_str())];
					n->reconfigMod->boundNode = n;
					n->boundedResType = n->reconfigMod->resourceType;
				}
				else if (StringToUpper(p.at(1)) == "OUTPUT")
				{
					claim(p.size() == 7, "Invalid number of parameters for OUTPUT.");
					n = dag->AddOutputNode(p.at(2), p.at(3));
					n->startTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(5).c_str(), NULL, 0);
					n->ioPort = ports[atoi(p.at(6).c_str())];
				}
				else if (StringToUpper(p.at(1)) == "STORAGE")
				{
					claim(p.size() == 5, "Invalid number of parameters for STORAGE.");
					n = dag->AddStorageNode();
					n->startTimeStep = strtoul(p.at(2).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(3).c_str(), NULL, 0);
					n->reconfigMod = rMods[atoi(p.at(4).c_str())];
					n->reconfigMod->boundNode = n;
					n->boundedResType = n->reconfigMod->resourceType;
				}
				else // GENERAL
				{
					claim(p.size() == 6, "Invalid number of parameters for GENERAL.");
					n = dag->AddGeneralNode(p.at(2));
					n->startTimeStep = strtoul(p.at(3).c_str(), NULL, 0);
					n->endTimeStep = strtoul(p.at(4).c_str(), NULL, 0);
					n->reconfigMod = rMods[atoi(p.at(5).c_str())];
					n->reconfigMod->boundNode = n;
					n->boundedResType = n->reconfigMod->resourceType;
				}
				n->id = atoi(p.at(0).c_str());
				n->status = BOUND;
				nodes[n->id] = n;
			}
			else if (tag == "EDGE")
			{
				claim(p.size() == 2, "Invalid number of parameters for an EDGE.");
				edges.push_back(atoi(p.at(0).c_str()));
				edges.push_back(atoi(p.at(1).c_str()));
			}
			else if (tag == "PINMAP")
			{
				arch->getPinMapper()->initPinMapping();
				claim(p.size() == arch->numCellsX*arch->numCellsY, "PinMap should be the same size of the array.");
				int i = 0;
				for (int y = 0; y < arch->numCellsY; y++)
					for (int x = 0; x < arch->numCellsX; x++)
						arch->pinMapper->getPinMapping()->at(x)->at(y) = atoi(p.at(i++).c_str());
			}
			else if (tag == "SPECIALPINS")
			{
				for (int i = 0; i < p.size(); i++)
					arch->pinMapper->getSpecialPurposePins()->push_back(atoi(p.at(i).c_str()));
				arch->getPinMapper()->unflattenSpecialPurposePins();
			}
			else if (tag == "RESOURCECOUNT")
			{
				for (int i = 0; i < p.size(); i++)
					arch->pinMapper->getAvailResCount()->at(i) = atoi(p.at(i).c_str());
			}
			else if (tag == "RESOURCELOCATION")
			{
				arch->pinMapper->getAvailRes()->at((ResourceType)atoi(p.at(0).c_str()))->push_back(new FixedModule((ResourceType)atoi(p.at(0).c_str()), atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				arch->pinMapper->getAvailRes()->at((ResourceType)atoi(p.at(0).c_str()))->back()->setTileNum(atoi(p.at(5).c_str()));
			}
			else
			{
				stringstream ss;
				ss << "Invalid tag type: " << tag << endl;
				claim(false, &ss);
			}
		}
	}

	// Recreate Edges
	while (!edges.empty())
	{
		dag->ParentChild(nodes[edges[0]], nodes[edges[1]]);
		//dag->ParentChild(dag->getAllNodes().at(edges.at(0)-1), dag->getAllNodes().at(edges.at(1)-1));
		edges.erase(edges.begin());
		edges.erase(edges.begin());
	}

	// Now add IOPorts and Reconfigurable Modules to the synthesis members
	map<int, IoPort*>::iterator pIt = ports.begin();
	for (; pIt != ports.end(); pIt++)
		arch->ioPorts->push_back(pIt->second);

	map<int, ReconfigModule*>::iterator rIt = rMods.begin();
	for (; rIt != rMods.end(); rIt++)
		rModules->push_back(rIt->second);

	is.close();
}

///////////////////////////////////////////////////////////////
// Reads the routed file to gather info about the pin-mapping
// (which is potentially in multiple interface files) and the
// pin-activations, which is only in the routed file.
///////////////////////////////////////////////////////////////
void Util::ReadRoutedSimPinsAndArchFromFile(DmfbArch *arch, vector<vector<int> *> *pinActivations, string fileName)
{
	map<int, IoPort*> ports;


	ifstream is;
	is.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly read Routed DAG/Arch file: " << fileName << endl;
		claim (is.good(), &str);
	}

	string line;
	while (!is.eof())
	{
		getline(is, line);
		string tag;
		vector<string> p;
		if (!(line.empty() || line.substr(0,2) == "//"))
		{
			ParseLine(line, &tag, &p);
			tag = StringToUpper(tag);

			// Recreate Nodes and save edges for later
			if (tag == "ARCHNAME")
			{
				claim(p.size() == 1, "Invalid number of parameters for ARCHNAME.");
				arch->name = p.at(0);
			}
			else if (tag == "DIM")
			{
				claim(p.size() == 2, "Invalid number of parameters for dimension (DIM) tag.");
				arch->numCellsX = atoi(p.at(0).c_str());
				arch->numCellsY = atoi(p.at(1).c_str());
				arch->getPinMapper()->initPinMapping();
			}
			else if (tag == "FREQ")
			{
				claim(p.size() == 1, "Invalid number of parameters for FREQ tag.");
				arch->freqInHz = atof(p.at(0).c_str());
			}
			else if (tag == "TIMESTEP")
			{
				claim(p.size() == 1, "Invalid number of parameters for timestep (s) tag.");
				arch->secPerTS = atof(p.at(0).c_str());
			}
			else if (tag == "EXTERNAL")
			{
				claim(p.size() == 5, "Invalid number of parameters for EXTERNAL module/resource.");
				arch->externalResources->push_back(new FixedModule((ResourceType)atoi(p.at(0).c_str()), atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
			}
			else if (tag == "INPUT")
			{
				claim(p.size() == 4, "Invalid number of parameters for an INPUT.");
				Direction dir = DIR_UNINIT;
				if (StringToUpper(p.at(1)) == "TOP")
					dir = NORTH;
				else if (StringToUpper(p.at(1)) == "BOTTOM")
					dir = SOUTH;
				else if (StringToUpper(p.at(1)) == "RIGHT")
					dir = EAST;
				else if (StringToUpper(p.at(1)) == "LEFT")
					dir = WEST;
				else
					claim(false, "Invalid side for an INPUT.");

				IoPort *iop = new IoPort(true, dir, atoi(p.at(2).c_str()), 0, p.at(3));
				//iop->setPinNo(atoi(p.at(0).c_str()));
				iop->id = atoi(p.at(0).c_str());
				ports[iop->id] = iop;
			}
			else if (tag == "OUTPUT")
			{
				claim(p.size() == 4, "Invalid number of parameters for an OUTPUT.");
				Direction dir = DIR_UNINIT;
				if (StringToUpper(p.at(1)) == "TOP")
					dir = NORTH;
				else if (StringToUpper(p.at(1)) == "BOTTOM")
					dir = SOUTH;
				else if (StringToUpper(p.at(1)) == "RIGHT")
					dir = EAST;
				else if (StringToUpper(p.at(1)) == "LEFT")
					dir = WEST;
				else
					claim(false, "Invalid side for an OUTPUT.");

				IoPort *iop = new IoPort(true, dir, atoi(p.at(2).c_str()), 0, p.at(3));
				//iop->setPinNo(atoi(p.at(4).c_str()));
				iop->id = atoi(p.at(0).c_str());
				ports[iop->id] = iop;
			}
			else if (tag == "PINMAP")
			{
				arch->getPinMapper()->initPinMapping();
				claim(p.size() == arch->numCellsX*arch->numCellsY, "PinMap should be the same size of the array.");
				int i = 0;
				for (int y = 0; y < arch->numCellsY; y++)
					for (int x = 0; x < arch->numCellsX; x++)
						arch->pinMapper->getPinMapping()->at(x)->at(y) = atoi(p.at(i++).c_str());
			}
			else if (tag == "SPECIALPINS")
			{
				for (int i = 0; i < p.size(); i++)
					arch->pinMapper->getSpecialPurposePins()->push_back(atoi(p.at(i).c_str()));
				arch->getPinMapper()->unflattenSpecialPurposePins();
			}
			else if (tag == "RESOURCECOUNT")
			{
				for (int i = 0; i < p.size(); i++)
					arch->pinMapper->getAvailResCount()->at(i) = atoi(p.at(i).c_str());
			}
			else if (tag == "RESOURCELOCATION")
			{
				arch->pinMapper->getAvailRes()->at((ResourceType)atoi(p.at(0).c_str()))->push_back(new FixedModule((ResourceType)atoi(p.at(0).c_str()), atoi(p.at(1).c_str()), atoi(p.at(2).c_str()), atoi(p.at(3).c_str()), atoi(p.at(4).c_str())));
				arch->pinMapper->getAvailRes()->at((ResourceType)atoi(p.at(0).c_str()))->back()->setTileNum(atoi(p.at(5).c_str()));
			}
			else if (tag == "ACTIVEPINS")
			{
				vector<int> *activePins = new vector<int>();
				for (int i = 0; i < p.size(); i++)
					activePins->push_back(atoi(p.at(i).c_str()));
				pinActivations->push_back(activePins);
			}
			else
			{
				//stringstream ss;
				//ss << "Invalid tag type: " << tag << endl;
				//claim(false, &ss);
			}
		}
	}


	// Now add IOPorts to the architecture
	map<int, IoPort*>::iterator pIt = ports.begin();
	for (; pIt != ports.end(); pIt++)
		arch->ioPorts->push_back(pIt->second);

	is.close();
}

///////////////////////////////////////////////////////////////
// Writes the final simulation results to an output file that
// can be read by the Java visualizer tools
///////////////////////////////////////////////////////////////
void Util::WriteRoutedDagAndArchToFile(DAG *dag, DmfbArch *arch, vector<ReconfigModule *> *rModules, map<Droplet *, vector<RoutePoint *> *> *routes, vector<vector<int> *> *pinActivations, vector<unsigned long long> *tsBeginningCycle, string fileName)
{
	ofstream os;
	os.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly write Cyclic Routed DAG to file: " << fileName << endl;
		claim (os.good(), &str);
	}

	DAG *d = dag;
	DmfbArch *a = arch;

	// First output the architecture
	os << "=======================Initialization=======================" << endl;
	os << "DIM (" << a->getNumCellsX() << ", " << a->getNumCellsY() << ")" << endl;
	os << endl << "// IO Ports from " << a->getName() << endl;
	for (int i = 0; i < a->ioPorts->size(); i++)
	{
		IoPort *iop = a->ioPorts->at(i);

		if (iop->isInput)
			os << "INPUT (";
		else
			os << "OUTPUT (";

		if (iop->ioSide == NORTH)
			os << iop->id  << ", top, " << iop->posXY << ", " << iop->portName << ")" << endl;
		else if (iop->ioSide == SOUTH)
			os << iop->id  << ", bottom, " << iop->posXY << ", " << iop->portName << ")" << endl;
		else if (iop->ioSide == EAST)
			os << iop->id  << ", right, " << iop->posXY << ", " << iop->portName << ")" << endl;
		else if (iop->ioSide == WEST)
			os << iop->id  << ", left, " << iop->posXY << ", " << iop->portName << ")" << endl;
		else
			claim(false, "Unknown IO side direction specified while outputting to file.");
	}


	os << "PINMAP (";
	for (int y = 0; y < arch->getNumCellsY(); y++)
	{
		for (int x = 0; x < arch->getNumCellsX(); x++)
		{
			os << arch->getPinMapper()->getPinMapping()->at(x)->at(y);
			if (!(x == arch->getNumCellsX()-1 && y == arch->getNumCellsY()-1))
				os << ", ";
		}
	}
	os << ")" << endl;

	os << endl << "// Non-reconfigurable Resources from " << a->getName() << endl;
	for (int i = 0; i < a->externalResources->size(); i++)
	{
		FixedModule *fm = a->externalResources->at(i);
		if (fm->resourceType == D_RES)
			os << "EXTERNALDETECTOR (" << fm->id << ", " << fm->leftX << ", " << fm->topY << ", " << fm->rightX << ", " << fm->bottomY << ")" << endl;
		else if (fm->resourceType == H_RES)
			os << "EXTERNALHEATER (" << fm->id << ", " << fm->leftX << ", " << fm->topY << ", " << fm->rightX << ", " << fm->bottomY << ")" << endl;
		else
			claim(false, "Unknown EXTERNAL module type being ouput to file.");
	}

	//DTG Debug
	//for (int i = 0; i < 200; i++)
	//	tsBeginningCycle->push_back(12345);

	os << endl << "// Reconfigurable Resources" << endl;
	for (int i = 0; i < rModules->size(); i++)
	{
		ReconfigModule *rm = rModules->at(i);
		unsigned long long begTS = rm->startTimeStep;//tsBeginningCycle->at(rm->startTimeStep);
		unsigned long long endTS = rm->endTimeStep;//tsBeginningCycle->at(rm->endTimeStep);
		if (rm->boundNode->type == MIX)
			os << "RECONFIGMIXER (" << rm->id << ", " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << begTS << ", " << endTS << ")" << endl;
		else if (rm->boundNode->type == SPLIT)
			os << "RECONFIGSPLITTER (" << rm->id << ", " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << begTS << ", " << endTS << ")" << endl;
		else if (rm->boundNode->type == HEAT)
			os << "RECONFIGHEATER (" << rm->id << ", " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << begTS << ", " << endTS << ")" << endl;
		else if (rm->boundNode->type == DETECT)
			os << "RECONFIGDETECTOR (" << rm->id << ", " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << begTS << ", " << endTS << ")" << endl;
		else if (rm->boundNode->type == STORAGE || rm->boundNode->type == STORAGE_HOLDER)
			os << "RECONFIGSTORAGE (" << rm->id << ", " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << begTS << ", " << endTS << ")" << endl;
		else if (rm->boundNode->type == GENERAL)
			os << "RECONFIGGENERAL (" << rm->id << ", " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << begTS << ", " << endTS << ")" << endl;
		else
			claim(false, "Unsupported reconfigurable resource type.");
	}

	os << endl << "// Locations of the resources listed above (if any)" << endl;
	vector<vector<FixedModule *> *> *ar = arch->getPinMapper()->getAvailRes();
	for (int i = 0; i < ar->size(); i++)
		for (int j = 0; j < ar->at(i)->size(); j++)
			os << "RESOURCELOCATION (" << (int)ar->at(i)->at(j)->resourceType << ", " << ar->at(i)->at(j)->leftX << ", " << ar->at(i)->at(j)->topY << ", " << ar->at(i)->at(j)->rightX << ", " << ar->at(i)->at(j)->bottomY << ", " << ar->at(i)->at(j)->tiledNum << ")" << endl;

	os << endl << "// Time-step to cycle conversions" << endl;
	for (int i = 0; i < tsBeginningCycle->size(); i++)
	{
		unsigned long long lastTSEnd = 0;
		if (i > 0)
			lastTSEnd = tsBeginningCycle->at(i-1) + (int)(a->freqInHz*a->secPerTS);

		os << "ROUTETO (" << i << ", " << lastTSEnd << ", " << tsBeginningCycle->at(i) << ")" << endl;
		os << "TSRANGE (" << i << ", " << tsBeginningCycle->at(i) << ", " << tsBeginningCycle->at(i) + (int)(a->freqInHz*a->secPerTS) << ")" << endl;
	}

	os << "=======================Init Done=======================" << endl;

	bool isDone = false;
	//map<Droplet *, vector<RoutePoint *> *> *routes
	map<Droplet *, vector<RoutePoint *> *>::iterator it = routes->begin();
	unsigned long long cycle = 10000000000000;
	unsigned long long lastCycle = 0;
	while (it != routes->end())
	{
		if (!it->second->empty() && it->second->front()->cycle < cycle)
			cycle = it->second->front()->cycle;
		if (!it->second->empty() && it->second->back()->cycle > lastCycle)
			lastCycle = it->second->back()->cycle;
		it++;
	}

	// Create string stream for each cycle
	vector<stringstream *> cycleStrings;
	for (int i = cycle; i <= lastCycle+1; i++)
	{
		stringstream *ss = new stringstream();
		(*ss) << "=======================Commit Cycle " << i << "=======================" << endl;
		cycleStrings.push_back(ss);
	}

	// Add routing points to appropriate cycle
	for (it = routes->begin(); it != routes->end(); it++)
	{
		vector<RoutePoint *> *route = it->second;
		for (int i = 0; i < route->size(); i++)
		{
			RoutePoint *rp = route->at(i);

			int id = it->first->getId();
			(*cycleStrings.at(rp->cycle - cycle)) << "Droplet " << it->first->getId() << ", Cell: (" << rp->x << ", " << rp->y << ")";

			if (rp->dStatus == DROP_PROCESSING)
				(*cycleStrings.at(rp->cycle - cycle)) << "--PROCESSING";
			else if (rp->dStatus == DROP_CONSUMED)
				(*cycleStrings.at(rp->cycle - cycle)) << "--RETIRING";
			else if (rp->dStatus == DROP_OUTPUT)
				(*cycleStrings.at(rp->cycle - cycle)) << "--OUTPUTTING";
			else if (rp->dStatus == DROP_WAIT)
				(*cycleStrings.at(rp->cycle - cycle)) << "--PROCESS WAIT";
			else if (rp->dStatus == DROP_INT_WAIT)
				(*cycleStrings.at(rp->cycle - cycle)) << "--INTERFERENCE WAIT";
			(*cycleStrings.at(rp->cycle - cycle)) << endl;
		}
	}

	for (int i = 0; i < cycleStrings.size(); i++)
	{
		if (pinActivations->at(i+cycle)->size() > 0)
		{
			(*cycleStrings.at(i)) << "ActivePins (";
			for (int j = 0; j < pinActivations->at(i+cycle)->size(); j++)
			{
				(*cycleStrings.at(i)) << pinActivations->at(i+cycle)->at(j);
				if (j < pinActivations->at(i+cycle)->size()-1)
					(*cycleStrings.at(i)) << ", ";
			}
			(*cycleStrings.at(i)) << ")" << endl;
		}
	}

	for (int i = cycle; i <= lastCycle+1; i++)
		os << cycleStrings.at(i - cycle)->str() << "Number of pins to activate = " << pinActivations->at(i)->size() << endl;
		//os << cycleStrings.at(i - cycle)->str() << "Number of pins to activate = 1" << endl;

	// Cleanup stringstreams
	while (!cycleStrings.empty())
	{
		stringstream *ss = cycleStrings.back();
		cycleStrings.pop_back();
		delete ss;
	}

	os.close();
}

void Util::WriteHardwareFileWithWireRoutes(DmfbArch *arch, string fileName)
{
	ofstream os;
	os.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly write Hardware Description (with Wire-routes) to file: " << fileName << endl;
		claim (os.good(), &str);
	}

	DmfbArch *a = arch;

	// Output the architecture
	os << "DIM (" << a->getNumCellsX() << ", " << a->getNumCellsY() << ")" << endl;
	os << endl << "// IO Ports from " << a->getName() << endl;
	for (int i = 0; i < a->ioPorts->size(); i++)
	{
		IoPort *iop = a->ioPorts->at(i);

		if (iop->isInput)
			os << "INPUT (";
		else
			os << "OUTPUT (";

		if (iop->ioSide == NORTH)
			os << iop->id  << ", top, " << iop->posXY << ", " << iop->portName << ")" << endl;
		else if (iop->ioSide == SOUTH)
			os << iop->id  << ", bottom, " << iop->posXY << ", " << iop->portName << ")" << endl;
		else if (iop->ioSide == EAST)
			os << iop->id  << ", right, " << iop->posXY << ", " << iop->portName << ")" << endl;
		else if (iop->ioSide == WEST)
			os << iop->id  << ", left, " << iop->posXY << ", " << iop->portName << ")" << endl;
		else
			claim(false, "Unknown IO side direction specified while outputting to file.");
	}


	os << "PINMAP (";
	for (int y = 0; y < arch->getNumCellsY(); y++)
	{
		for (int x = 0; x < arch->getNumCellsX(); x++)
		{
			os << arch->getPinMapper()->getPinMapping()->at(x)->at(y);
			if (!(x == arch->getNumCellsX()-1 && y == arch->getNumCellsY()-1))
				os << ", ";
		}
	}
	os << ")" << endl;

	os << endl << "// Non-reconfigurable Resources from " << a->getName() << endl;
	for (int i = 0; i < a->externalResources->size(); i++)
	{
		FixedModule *fm = a->externalResources->at(i);
		if (fm->resourceType == D_RES)
			os << "EXTERNALDETECTOR (" << fm->id << ", " << fm->leftX << ", " << fm->topY << ", " << fm->rightX << ", " << fm->bottomY << ")" << endl;
		else if (fm->resourceType == H_RES)
			os << "EXTERNALHEATER (" << fm->id << ", " << fm->leftX << ", " << fm->topY << ", " << fm->rightX << ", " << fm->bottomY << ")" << endl;
		else
			claim(false, "Unknown EXTERNAL module type being ouput to file.");
	}


	os << endl << "// Locations of the module locations (if fixed/applicable)" << endl;
	vector<vector<FixedModule *> *> *ar = arch->getPinMapper()->getAvailRes();
	for (int i = 0; i < ar->size(); i++)
		for (int j = 0; j < ar->at(i)->size(); j++)
			os << "RESOURCELOCATION (" << (int)ar->at(i)->at(j)->resourceType << ", " << ar->at(i)->at(j)->leftX << ", " << ar->at(i)->at(j)->topY << ", " << ar->at(i)->at(j)->rightX << ", " << ar->at(i)->at(j)->bottomY << ", " << ar->at(i)->at(j)->tiledNum << ")" << endl;


	//NUMVTRACKS (3)

	os << endl << "// Wire-segment descriptions" << endl;
	os << "NUMHTRACKS (" << a->getWireRouter()->getNumHorizTracks() << ")" << endl;
	os << "NUMVTRACKS (" << a->getWireRouter()->getNumVertTracks() << ")" << endl;
	vector< vector<WireSegment *> *> *wires = a->getWireRouter()->getWireRoutesPerPin();
	for (int i = 0; i < wires->size(); i++)
	{
		vector<WireSegment *> *wire = wires->at(i);
		for (int j = 0; j < wire->size(); j++)
		{
			WireSegment *ws = wire->at(j);

			if (ws->segmentType == LINE_WS)
				os << "RELLINE (";
			else
				os << "RELARC (";

			os << ws->pinNo << ", " << ws->layer << ", " << ws->sourceXCell << ", " << ws->sourceYCell << ", " << ws->sourceXTrack << ", " << ws->sourceYTrack;
			os << ", " << ws->destXCell << ", " << ws->destYCell << ", " << ws->destXTrack << ", " << ws->destYTrack;

			if (ws->segmentType == LINE_WS)
				os << ")" << endl;
			else
				os << ", " << ws->startAngle << ", " << ws->arcAngle << ")" << endl;
		}
	}

	os.close();
}

///////////////////////////////////////////////////////////////
// Writes compacted simulation results to file so that they
// can be displayed by the Java visualization tools.
///////////////////////////////////////////////////////////////
void Util::WriteCompactedRoutesToFile(DAG *dag, DmfbArch *arch, vector<ReconfigModule *> *rModules, map<Droplet *, vector<RoutePoint *> *> *routes, vector<unsigned long long> *tsBeginningCycle, string fileName)
{
	ofstream os;
	os.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly write Compacted Routed DAG to file: " << fileName << endl;
		claim (os.good(), &str);
	}

	DAG *d = dag;
	DmfbArch *a = arch;

	// First output the architecture
	os << "=======================Initialization=======================" << endl;
	os << "DIM (" << a->getNumCellsX() << ", " << a->getNumCellsY() << ")" << endl;
	os << endl << "// IO Ports from " << a->getName() << endl;
	for (int i = 0; i < a->ioPorts->size(); i++)
	{
		IoPort *iop = a->ioPorts->at(i);

		if (iop->isInput)
			os << "INPUT (";
		else
			os << "OUTPUT (";

		if (iop->ioSide == NORTH)
			os << iop->id  << ", top, " << iop->posXY << ", " << iop->portName << ")" << endl;
		else if (iop->ioSide == SOUTH)
			os << iop->id  << ", bottom, " << iop->posXY << ", " << iop->portName << ")" << endl;
		else if (iop->ioSide == EAST)
			os << iop->id  << ", right, " << iop->posXY << ", " << iop->portName << ")" << endl;
		else if (iop->ioSide == WEST)
			os << iop->id  << ", left, " << iop->posXY << ", " << iop->portName << ")" << endl;
		else
			claim(false, "Unknown IO side direction specified while outputting to file.");
	}

	os << endl << "// Non-reconfigurable Resources from " << a->getName() << endl;
	for (int i = 0; i < a->externalResources->size(); i++)
	{
		FixedModule *fm = a->externalResources->at(i);
		if (fm->resourceType == D_RES)
			os << "EXTERNALDETECTOR (" << fm->id << ", " << fm->leftX << ", " << fm->topY << ", " << fm->rightX << ", " << fm->bottomY << ")" << endl;
		else if (fm->resourceType == H_RES)
			os << "EXTERNALHEATER (" << fm->id << ", " << fm->leftX << ", " << fm->topY << ", " << fm->rightX << ", " << fm->bottomY << ")" << endl;
		else
			claim(false, "Unknown EXTERNAL module type being ouput to file.");
	}

	os << endl << "// Reconfigurable Resources" << endl;
	for (int i = 0; i < rModules->size(); i++)
	{
		ReconfigModule *rm = rModules->at(i);
		unsigned long long begTS = rm->startTimeStep;
		unsigned long long endTS = rm->endTimeStep;
		if (rm->boundNode->type == MIX)
			os << "RECONFIGMIXER (" << rm->id << ", " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << begTS << ", " << endTS << ")" << endl;
		else if (rm->boundNode->type == SPLIT)
			os << "RECONFIGSPLITTER (" << rm->id << ", " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << begTS << ", " << endTS << ")" << endl;
		else if (rm->boundNode->type == HEAT)
			os << "RECONFIGHEATER (" << rm->id << ", " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << begTS << ", " << endTS << ")" << endl;
		else if (rm->boundNode->type == DETECT)
			os << "RECONFIGDETECTOR (" << rm->id << ", " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << begTS << ", " << endTS << ")" << endl;
		else if (rm->boundNode->type == STORAGE || rm->boundNode->type == STORAGE_HOLDER)
			os << "RECONFIGSTORAGE (" << rm->id << ", " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << begTS << ", " << endTS << ")" << endl;
		else if (rm->boundNode->type == GENERAL)
			os << "RECONFIGGENERAL (" << rm->id << ", " << rm->leftX << ", " << rm->topY << ", " << rm->rightX << ", " << rm->bottomY << ", " << begTS << ", " << endTS << ")" << endl;
		else
			claim(false, "Unsupported reconfigurable resource type.");
	}

	os << endl << "// Locations of the resources listed above (if any)" << endl;
	vector<vector<FixedModule *> *> *ar = arch->getPinMapper()->getAvailRes();
	for (int i = 0; i < ar->size(); i++)
		for (int j = 0; j < ar->at(i)->size(); j++)
			os << "RESOURCELOCATION (" << (int)ar->at(i)->at(j)->resourceType << ", " << ar->at(i)->at(j)->leftX << ", " << ar->at(i)->at(j)->topY << ", " << ar->at(i)->at(j)->rightX << ", " << ar->at(i)->at(j)->bottomY << ", " << ar->at(i)->at(j)->tiledNum << ")" << endl;

	// Now output scheduled DAG with resource bindings to specific IoPorts or Reconfigurable Modules
	os << endl << "// DAG Specification for " << dag->getName() << endl;
	if (!dag->getName().empty())
		os << "DAGNAME (" << dag->getName() << ")" << endl;
	else
		os << "DAGNAME (DAG)" << endl;

	for (int i = 0; i < dag->getAllNodes().size(); i++)
	{
		AssayNode *node = dag->getAllNodes().at(i);

		string resType;
		if (node->boundedResType == BASIC_RES)
			resType = "B";
		else if (node->boundedResType == D_RES)
			resType = "D";
		else if (node->boundedResType == H_RES)
			resType = "H";
		else if (node->boundedResType == DH_RES)
			resType = "DH";

		os << "NODE (";

		if (node->GetType() == DISPENSE) // Id, Type, FluidName, Volume, Name, StartTS, EndTS, IoPortId
			os << node->id << ", DISPENSE, " << node->portName <<  ", " << node->volume << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << node->ioPort->id << ")\n";
		else if (node->GetType() == MIX) // Id, Type, NumDropsBefore, Time (s), Name, StartTS, EndTS, ReconfigModId
			os << node->id << ", MIX, " << node->numDrops <<  ", " << node->seconds << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << node->reconfigMod->id << ")\n";
		else if (node->GetType() == SPLIT) // Id, Type, NumDropsAfter, Time (s), Name, StartTS, EndTS, ReconfigModId
			os << node->id << ", SPLIT, " << node->numDrops <<  ", " << node->seconds << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << node->reconfigMod->id << ")\n";
		else if (node->GetType() == HEAT)// Id, Type, Time (s), Name, StartTS, EndTS, ReconfigModId
			os << node->id << ", HEAT, " << node->seconds << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << node->reconfigMod->id << ")\n";
		else if (node->GetType() == DETECT) // Id, Type, NumDropsIo, Time (s), Name, StartTS, EndTS, ReconfigModId
			os << node->id << ", DETECT, " << node->numDrops <<  ", " << node->seconds << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << node->reconfigMod->id << ")\n";
		else if (node->GetType() == OUTPUT) // Id, Type, SinkName, Name, StartTS, EndTS, IoPortId
			os << node->id << ", OUTPUT, " << node->portName << ", " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << node->ioPort->id << ")\n";
		else if (node->GetType() == STORAGE) // Id, Type, StartTS, EndTS, ReconfigModId
			os << node->id << ", STORAGE, " << node->startTimeStep << ", " << node->endTimeStep << ", " << node->reconfigMod->id << ")\n";
		else // Id, Type, Name, StartTS, EndTS, ReconfigModId
			os << node->id << ", GENERAL, " << node->name << ", " << node->startTimeStep << ", " << node->endTimeStep << ", " << node->reconfigMod->id << ")\n";

		for (int c = 0; c < node->children.size(); c++)
			os << "EDGE (" << node->id << ", " << node->children.at(c)->id << ")\n";
	}


	os << "=======================Init Done=======================" << endl;


	// Create string stream for each routing phase
	vector<stringstream *> rpStrings;
	for (int i = 0; i <= tsBeginningCycle->size(); i++)
	{
		stringstream *ss = new stringstream();
		(*ss) << "=======================Routing to TimeStep " << i << "=======================" << endl;
		rpStrings.push_back(ss);
	}

	// Add routes to appropriate routing phases
	map<Droplet *, vector<RoutePoint *> *>::iterator it = routes->begin();
	for (it = routes->begin(); it != routes->end(); it++)
	{
		vector<RoutePoint *> *route = it->second;
		int rIndex = 0; // Route index
		int beginTS = 0; // Routing to (routing phase) index

		bool hasBegun = false;
		while (rIndex < route->size() && beginTS < tsBeginningCycle->size())
		{
			// Compute routing borders
			unsigned long long routeBeginCycle = 0;
			if (beginTS > 0)
				routeBeginCycle = tsBeginningCycle->at(beginTS-1) + (unsigned long long)(a->secPerTS * a->freqInHz);
			unsigned long long routeEndCycle = tsBeginningCycle->at(beginTS);


			RoutePoint *rp = route->at(rIndex);
			if (rp->cycle >= routeBeginCycle && rp->cycle <= routeEndCycle && (rp->dStatus == DROP_NORMAL || rp->dStatus == DROP_OUTPUT))
			{
				if (hasBegun == false)
					(*rpStrings.at(beginTS)) << "Droplet " << it->first->getId() << endl;
				hasBegun = true;

				(*rpStrings.at(beginTS)) << "(" << rp->x << ", " << rp->y << ")" << endl;
			}
			else
			{
				if (hasBegun == true)
					(*rpStrings.at(beginTS)) << "End Route" << endl;
				hasBegun = false;
			}
			if (rp->cycle <= routeEndCycle+1)
				rIndex++;
			else
				beginTS++;
		}
	}

	// Write each routing phase
	for (int i = 0; i < rpStrings.size(); i++)
		os << rpStrings.at(i)->str() << "Number of droplets routed = " << Util::countSubstring(Util::StringToUpper(rpStrings.at(i)->str()), "DROPLET") << endl;

	// Cleanup stringstreams
	while (!rpStrings.empty())
	{
		stringstream *ss = rpStrings.back();
		rpStrings.pop_back();
		delete ss;
	}

	os.close();
}

///////////////////////////////////////////////////////////////
// Writes file that has the bare-minimum information needed
// to actually execute the assay on a real-life DMFB (i.e.
// contains just the electrode activations)
///////////////////////////////////////////////////////////////
void Util::WriteDmfbProgramToFile(DAG *dag, DmfbArch *arch, vector<ReconfigModule *> *rModules, map<Droplet *, vector<RoutePoint *> *> *routes, vector<unsigned long long> *tsBeginningCycle, string fileName)
{
	ofstream os;
	os.open(fileName.c_str());

	{
		stringstream str;
		str << "Failed to properly write DMFB Program to file: " << fileName << endl;
		claim (os.good(), &str);
	}

	map<Droplet *, vector<RoutePoint *> *>::iterator it = routes->begin();
	unsigned long long cycle = 10000000000000;
	unsigned long long lastCycle = 0;
	while (it != routes->end())
	{
		if (!it->second->empty() && it->second->front()->cycle < cycle)
			cycle = it->second->front()->cycle;
		if (!it->second->empty() && it->second->back()->cycle > lastCycle)
			lastCycle = it->second->back()->cycle;
		it++;
	}

	// Create string stream for each cycle
	vector<stringstream *> cycleStrings;
	for (int i = cycle; i <= lastCycle+1; i++)
	{
		stringstream *ss = new stringstream();
		(*ss) << i << ":";
		cycleStrings.push_back(ss);
	}

	// Add routing points to appropriate cycle
	for (it = routes->begin(); it != routes->end(); it++)
	{
		vector<RoutePoint *> *route = it->second;
		for (int i = 0; i < route->size(); i++)
		{
			RoutePoint *rp = route->at(i);
			(*cycleStrings.at(rp->cycle - cycle)) << " (" << rp->x << "," << rp->y << ")";
		}
	}

	// Write each cycle
	for (int i = cycle; i <= lastCycle+1; i++)
		os << cycleStrings.at(i - cycle)->str() << endl;

	// Cleanup stringstreams
	while (!cycleStrings.empty())
	{
		stringstream *ss = cycleStrings.back();
		cycleStrings.pop_back();
		delete ss;
	}

	os.close();
}

////////////////////////////////////////////////////////////////////////////
// Given a line, parses and finds the tag and parameters and passes
// back by reference.
////////////////////////////////////////////////////////////////////////////
void Util::ParseLine(string line, string *tag, vector<string> *parameters)
{
	string params;
	size_t begin = line.find("(");
	size_t end = line.find(")");
	*tag = TrimString(line.substr(0, begin));
	params = line.substr(begin+1,end-begin-1);
	TokenizeString(params, ",", parameters);
}

////////////////////////////////////////////////////////////////////////////
// Remove all white space characters from beginning and end
////////////////////////////////////////////////////////////////////////////
string Util::TrimString(string str)
{
	while (str.find_first_of(" \n\r\t") == 0)
		str = str.substr(1);
	while (str.find_last_of(" \n\r\t") == str.length()-1)
		str = str.substr(0, str.length()-1);
	return str;
}

////////////////////////////////////////////////////////////////////////////
// Divide str into small substrings, based on token, and place in dest.
////////////////////////////////////////////////////////////////////////////
void Util::TokenizeString(string str, string token, vector<string> *dest)
{
	size_t begin = 0;
	size_t end = str.find(token);
	string next;
	string remaining = str;

	while (end != string::npos)
	{

		next = remaining.substr(begin, end-begin);
		remaining = remaining.substr(end+1);
		dest->push_back(TrimString(next));
		next.clear();
		end = remaining.find(token);
	}
	dest->push_back(TrimString(remaining));
}

////////////////////////////////////////////////////////////////////////////
// Returns the string name of the operation type
////////////////////////////////////////////////////////////////////////////
string Util::getOperationString(OperationType ot)
{
	if (ot == MIX)
		return "MIX";
	else if (ot == SPLIT)
		return "SPLIT";
	else if (ot == HEAT)
		return "HEAT";
	else if (ot == DETECT)
		return "DETECT";
	else if (ot == STORAGE)
		return "STORAGE";
	else if (ot == STORAGE_HOLDER)
		return "STORAGE_HOLDER";
	else if (ot == GENERAL)
		return "GENERAL";
	else
		claim(false, "Unknown operation type.");
}

////////////////////////////////////////////////////////////////////////////
// Returns the operation-type given a string.
////////////////////////////////////////////////////////////////////////////
OperationType Util::getOpTypeFromString(string ot)
{
	ot = StringToUpper(TrimString(ot));
	if (ot == "MIX")
		return MIX;
	else if (ot == "SPLIT")
		return SPLIT;
	else if (ot == "HEAT")
		return HEAT;
	else if (ot == "DETECT")
		return DETECT;
	else if (ot == "STORAGE")
		return STORAGE;
	else if (ot == "STORAGE_HOLDER")
		return STORAGE_HOLDER;
	else if (ot == "GENERAL")
		return GENERAL;
	else
		claim(false, "Invalid operation type.");
}
