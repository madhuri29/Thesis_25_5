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
 * Source: command_line.cc														*
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
#include "../Headers/command_line.h"
#include "../Headers/synthesis.h"

//////////////////////////////////////////////////////////////////////////////
// Creates the libraries for the command line parser. User will need to
// manually add an entry here for each new synthesis step he/she creates.
//////////////////////////////////////////////////////////////////////////////
CmdLine::CmdLine()
{
	// Create library for schedulers
	sKeys.push_back("LS"); sDescrips.push_back("List Scheduling"); sEnums.push_back(LIST_S);
	sKeys.push_back("PS"); sDescrips.push_back("Grissom's Path Scheduling"); sEnums.push_back(PATH_S);
	sKeys.push_back("GA"); sDescrips.push_back("Genetic Algorithm Scheduling"); sEnums.push_back(GENET_S);
	sKeys.push_back("RGA"); sDescrips.push_back("Rickett's Genetic Algorithm Scheduling"); sEnums.push_back(RICKETT_S);
	sKeys.push_back("FDLS"); sDescrips.push_back("O' Neal's Force Directed List Scheduler"); sEnums.push_back(FD_LIST_S);
	//sKeys.push_back("X"); sDescrips.push_back("Add New Scheduling Method Here"); sEnums.push_back(NEW_S);

	// Create library for placers
	pKeys.push_back("SAP"); pDescrips.push_back("Simulated Annealing Placement"); pEnums.push_back(SA_P);
	pKeys.push_back("GLEB"); pDescrips.push_back("Grissom's Left Edge Binder"); pEnums.push_back(GRISSOM_LE_B);
	//pKeys.push_back("X"); pDescrips.push_back("Add New Placement Method Here"); pEnums.push_back(NEW_P);

	// Create library for routers
	rKeys.push_back("RMR"); rDescrips.push_back("Grissom's Simplified Roy Maze Router"); rEnums.push_back(ROY_MAZE_R);
	rKeys.push_back("BR"); rDescrips.push_back("Yuh's BioRouter (Network flow-based)"); rEnums.push_back(BIOROUTE_R);
	rKeys.push_back("FPR"); rDescrips.push_back("Grissom's Fixed-Place Router"); rEnums.push_back(GRISSOM_FIX_R);
	rKeys.push_back("FPMR"); rDescrips.push_back("Grissom's Fixed-Place Map Router"); rEnums.push_back(GRISSOM_FIX_MAP_R);
	//rKeys.push_back("X"); rDescrips.push_back("Add New Router Method Here"); rEnums.push_back(NEW_R);

	// Create library for resource-allocation types
	raKeys.push_back("FPRA0"); raDescrips.push_back("Resource Allocator for Grissom's Fixed Placement - Horiz. Route Every 0 Mods"); raEnums.push_back(GRISSOM_FIX_0_RA);
	raKeys.push_back("FPRA1"); raDescrips.push_back("Resource Allocator for Grissom's Fixed Placement - Horiz. Route Every 1 Mods"); raEnums.push_back(GRISSOM_FIX_1_RA);
	raKeys.push_back("FPRA2"); raDescrips.push_back("Resource Allocator for Grissom's Fixed Placement - Horiz. Route Every 2 Mods"); raEnums.push_back(GRISSOM_FIX_2_RA);
	raKeys.push_back("FPRA3"); raDescrips.push_back("Resource Allocator for Grissom's Fixed Placement - Horiz. Route Every 3 Mods"); raEnums.push_back(GRISSOM_FIX_3_RA);
	raKeys.push_back("PCIRA"); raDescrips.push_back("Inherent Resource Allocator for Pin-Constrained Placement"); raEnums.push_back(PC_INHERENT_RA);
	//raKeys.push_back("X"); raDescrips.push_back("Add New Resource Allocator"); raEnums.push_back(NEW_RA);

	// Create library for pin-mappers
	pmKeys.push_back("IAPM"); pmDescrips.push_back("Pin-Mapper for Individually-Addressable Designs"); pmEnums.push_back(INDIVADDR_PM);
	//pmKeys.push_back("X"); pmDescrips.push_back("Add New Pin-Mapping Method Here"); pmEnums.push_back(NEW_PM);

	// Create library for wire-routing types
	wrKeys.push_back("NOWR"); wrDescrips.push_back("No Wire Routing"); wrEnums.push_back(NONE_WR);
	//wrKeys.push_back("xWR"); wrDescrips.push_back("Add New Wire-Routing Method Here"); wrEnums.push_back(NEW_WR);

	// Create library for compaction types
	cKeys.push_back("NC"); cDescrips.push_back("No compaction"); cEnums.push_back(NO_COMP);
	cKeys.push_back("IC"); cDescrips.push_back("Inherent (used in BioRouter; does compaction internally during route computation)"); cEnums.push_back(INHERENT_COMP);
	cKeys.push_back("BC"); cDescrips.push_back("Add stalls to Beginning of routes"); cEnums.push_back(BEG_COMP);
	cKeys.push_back("MC"); cDescrips.push_back("Add stalls to Middle of routes"); cEnums.push_back(MID_COMP);
	//cKeys.push_back("X"); cDescrips.push_back("Add New Compaction Type Here"); cEnums.push_back(NEW_C);

	// Create library for processing engine types
	peKeys.push_back("FFPE"); peDescrips.push_back("Fixed Quick PE (for examining only the routes)"); peEnums.push_back(FIXED_QUICK_PE);
	peKeys.push_back("FQPE"); peDescrips.push_back("Fixed Full PE (full simulation)"); peEnums.push_back(FIXED_FULL_PE);
	//peKeys.push_back("X"); peDescrips.push_back("Add New Processing Engine Type Here"); peEnums.push_back(NEW_PR);

	// Create library for execution types
	etKeys.push_back("SE"); etDescrips.push_back("Simulation Execution (generates all simulation output files)"); etEnums.push_back(SIM_EX);
	etKeys.push_back("PE"); etDescrips.push_back("Program Execution (only generates an electrode activation program)"); etEnums.push_back(PROG_EX);
	etKeys.push_back("AE"); etDescrips.push_back("All Execution (generates all possible output)"); etEnums.push_back(ALL_EX);
	//etKeys.push_back("X"); etDescrips.push_back("Add New Execution Type Here"); etEnums.push_back(NEW_EX);
}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
CmdLine::~CmdLine(){}


//////////////////////////////////////////////////////////////////////////////
// Determines if using the command line to control program.
//////////////////////////////////////////////////////////////////////////////
bool CmdLine::IsUsingCommandLine(int argc)
{
	if (argc > 1)
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////////
// Prints the usage string which describes how to use the parameters of the
// simulator via the command line
//////////////////////////////////////////////////////////////////////////////
string CmdLine::GetUsageString()
{
	stringstream ss("");
	ss << "Welcome to the UCR DMFB Synthesis Simulator, v2.0" << endl;
	ss << "Usage for Entire Flow:\n\t$MFSimStatic ef [schedulerType] [placerType] [routerType] [resourceAllocationType] [pinMapType] [wireRouteType] [compactionType] [processingEngineType] [executionType] inputAssayFile inputArchFile maxDropsPerStorageMod numCellsBetweenMods numHorizTracks numVertTracks" << endl;
	ss << "Usage for Scheduling:\n\t$MFSimStatic s [schedulerType] [resourceAllocationType] [pinMapType] inputAssayFile inputArchFile outputInterfaceFile maxDropsPerStorageMod" << endl;
	ss << "Usage for Placement:\n\t$MFSimStatic p [placerType] [pinMapType] inputInterfaceFile outputInterfaceFile maxDropsPerStorageMod numCellsBetweenMods" << endl;
	ss << "Usage for Routing:\n\t$MFSimStatic r [routerType] [pinMapType] [compactionType] [processingEngineType] inputInterfaceFile numCellsBetweenMods" << endl;
	ss << "Usage for WireRouting:\n\t$MFSimStatic wr [wireRouteType] inputInterfaceFile numHorizTracks numVertTracks" << endl;

	ss << endl << "where options include (omit quotations):" << endl;

	// Scheduler Types
	ss << "\t[schedulerType]" << endl;
	for (int i = 0; i < sKeys.size(); i++)
		ss << "\t\t\"" << sKeys.at(i) << "\" -- " << sDescrips.at(i) << endl;

	// Placer Types
	ss << "\t[placementType]" << endl;
	for (int i = 0; i < pKeys.size(); i++)
		ss << "\t\t\"" << pKeys.at(i) << "\" -- " << pDescrips.at(i) << endl;

	// Router Types
	ss << "\t[routerType]" << endl;
	for (int i = 0; i < rKeys.size(); i++)
		ss << "\t\t\"" << rKeys.at(i) << "\" -- " << rDescrips.at(i) << endl;

	// Resource-Allocation Types
	ss << "\t[resourceAllocationType]" << endl;
	for (int i = 0; i < raKeys.size(); i++)
		ss << "\t\t\"" << raKeys.at(i) << "\" -- " << raDescrips.at(i) << endl;

	// Pin-Map Types
	ss << "\t[pinMapType]" << endl;
	for (int i = 0; i < pmKeys.size(); i++)
		ss << "\t\t\"" << pmKeys.at(i) << "\" -- " << pmDescrips.at(i) << endl;

	// Wire-Route Types
	ss << "\t[wireRouteType]" << endl;
	for (int i = 0; i < wrKeys.size(); i++)
		ss << "\t\t\"" << wrKeys.at(i) << "\" -- " << wrDescrips.at(i) << endl;

	// Compaction Types
	ss << "\t[compactionType]" << endl;
	for (int i = 0; i < cKeys.size(); i++)
		ss << "\t\t\"" << cKeys.at(i) << "\" -- " << cDescrips.at(i) << endl;

	// Processing Engine Types
	ss << "\t[processingEngineType]" << endl;
	for (int i = 0; i < peKeys.size(); i++)
		ss << "\t\t\"" << peKeys.at(i) << "\" -- " << peDescrips.at(i) << endl;

	// Execution Types
	ss << "\t[executionType]" << endl;
	for (int i = 0; i < etKeys.size(); i++)
		ss << "\t\t\"" << etKeys.at(i) << "\" -- " << etDescrips.at(i) << endl;

	// File Names
	ss << "\tinputAssayFile" << endl;
	ss << "\t\te.g. \"Assays/B1_PCR.txt\"" << endl;
	ss << "\tinputArchFile" << endl;
	ss << "\t\te.g. \"DmfbArchs/Arch_16_17_B1.txt\"" << endl;
	ss << "\tinputInterfaceFile/inputInterfaceFile" << endl;
	ss << "\t\te.g. \"Output/1_SCHED_to_PLACE.txt\"" << endl;
	ss << "\t\te.g. \"Output/2_PLACE_to_ROUTE.txt\"" << endl;
	ss << "\t\te.g. \"Output/3_ROUTE_to_SIM.txt\"" << endl;

	ss << "\tmaxDropsPerStorageMod (May not be applicable for all simulations)" << endl;
	ss << "\t\te.g. 1, 2, etc. " << endl;
	ss << "\tnumCellsBetweenMods (May not be applicable for all simulations)" << endl;
	ss << "\t\te.g. 0, 1, 2, etc." << endl;

	ss << "\tnumHorizTracks/numVertTracks (May not be applicable for all simulations)" << endl;
	ss << "\t\te.g. 1, 2, 3, etc." << endl;

	ss << "----------------------------------------" << endl;
	ss << "Examples:" << endl;
	ss << "$MFSimStatic ef LS GFP RMR FPRA IAPM BC FFPE SE Assays/B1_PCR.txt DmfbArchs/Arch_16_17_B1.txt 2 1" << endl;
	ss << "$MFSimStatic s LS FPRA IAPM Assays/B1_PCR.txt DmfbArchs/Arch_16_17_B1.txt Output/1_SCHED_to_PLACE.txt 1" << endl;
	ss << "$MFSimStatic p GFP IAPM Output/1_SCHED_to_PLACE.txt Output/2_PLACE_to_ROUTE.txt 2 1" << endl;
	ss << "$MFSimStatic r RMR IAPM BC FFPE Output/2_PLACE_to_ROUTE.txt 1" << endl;
	ss << "$MFSimStatic wr NOWR Output/3_ROUTE_to_SIM.txt 3 3 " << endl;

	return ss.str();
}


//////////////////////////////////////////////////////////////////////////////
// Prints the usage string and causes the program to halt.
//////////////////////////////////////////////////////////////////////////////
void CmdLine::ForceCorrectUsage()
{
	cout << GetUsageString() << endl << "EXITING.";
	exit(0);
}

//////////////////////////////////////////////////////////////////////////////
// Takes in the parameters from the command line and executes the requested
// simulations.
//////////////////////////////////////////////////////////////////////////////
void CmdLine::ExecuteCommand(int argc, char **argv)
{
	if (!IsUsingCommandLine(argc))
		ForceCorrectUsage();

	string cmd = Util::StringToUpper(argv[1]);
	if (cmd == "EF")
	{
		if (argc != 17)
		{
			cout << "Improper number of parameters for entire flow (EF)." << endl << endl;
			ForceCorrectUsage();
		}

		int sIndex = GetLibIndexFromSchedKey(Util::StringToUpper(argv[2]));
		int pIndex = GetLibIndexFromPlaceKey(Util::StringToUpper(argv[3]));
		int rIndex = GetLibIndexFromRouteKey(Util::StringToUpper(argv[4]));
		int raIndex = GetLibIndexFromResourceAllocationKey(Util::StringToUpper(argv[5]));
		int pmIndex = GetLibIndexFromPinMapKey(Util::StringToUpper(argv[6]));
		int wrIndex = GetLibIndexFromWrKey(Util::StringToUpper(argv[7]));
		int cIndex = GetLibIndexFromCompKey(Util::StringToUpper(argv[8]));
		int peIndex = GetLibIndexFromPeKey(Util::StringToUpper(argv[9]));
		int etIndex = GetLibIndexFromEtKey(Util::StringToUpper(argv[10]));
		string assayFile = argv[11];
		string archFile = argv[12];
		int maxDropsPerStorageMod = atoi(argv[13]);
		int numCellsBetweenMods = atoi(argv[14]);
		int numHorizTracks = atoi(argv[15]);
		int numVertTracks = atoi(argv[16]);

		cout << "Executing entire synthesis flow with: ";
		cout << sDescrips.at(sIndex) << ", ";
		cout << pDescrips.at(pIndex) << ", ";
		cout << rDescrips.at(rIndex) << ", ";
		cout << raDescrips.at(raIndex) << ", ";
		cout << pmDescrips.at(pmIndex) << ", ";
		cout << wrDescrips.at(wrIndex) << ", ";
		cout << cDescrips.at(cIndex) << ", ";
		cout << peDescrips.at(peIndex) << ", ";
		cout << etDescrips.at(etIndex) << ", ";
		cout << assayFile << " Assay File, ";
		cout << archFile << " Arch File, ";
		cout << maxDropsPerStorageMod << " maxDropsPerStorageMod, ";
		cout << numCellsBetweenMods << " numCellsBetweenMods, ";
		cout << numHorizTracks << " numHorizTracks, ";
		cout << numVertTracks << " numVertTracks." << endl;

		Synthesis::EntireFlow(assayFile, archFile, sEnums.at(sIndex), pEnums.at(pIndex), rEnums.at(rIndex), raEnums.at(raIndex), pmEnums.at(pmIndex), wrEnums.at(wrIndex), cEnums.at(cIndex), peEnums.at(peIndex), etEnums.at(etIndex), maxDropsPerStorageMod, numCellsBetweenMods, numHorizTracks, numVertTracks);

	}
	else if (cmd == "S")
	{
		if (argc != 9)
		{
			cout << "Improper number of parameters for scheduling (S)." << endl << endl;
			ForceCorrectUsage();
		}

		int sIndex = GetLibIndexFromSchedKey(Util::StringToUpper(argv[2]));
		int raIndex = GetLibIndexFromResourceAllocationKey(Util::StringToUpper(argv[3]));
		int pmIndex = GetLibIndexFromPinMapKey(Util::StringToUpper(argv[4]));
		string assayFile = argv[5];
		string archFile = argv[6];
		string outFile = argv[7];
		int maxDropsPerStorageMod = atoi(argv[8]);

		cout << "Executing scheduler with: ";
		cout << sDescrips.at(sIndex) << ", ";
		cout << raDescrips.at(raIndex) << ", ";
		cout << pmDescrips.at(pmIndex) << ", ";
		cout << assayFile << " Assay File, ";
		cout << archFile << " Arch File, ";
		cout << outFile << " Out File." << endl;

		Synthesis::Schedule(assayFile, archFile, outFile, sEnums.at(sIndex), raEnums.at(raIndex), pmEnums.at(pmIndex), maxDropsPerStorageMod);
	}
	else if (cmd == "P")
	{
		if (argc != 8)
		{
			cout << "Improper number of parameters for placement (P)." << endl << endl;
			ForceCorrectUsage();
		}

		int pIndex = GetLibIndexFromPlaceKey(Util::StringToUpper(argv[2]));
		int pmIndex = GetLibIndexFromPinMapKey(Util::StringToUpper(argv[3]));
		string inFile = argv[4];
		string outFile = argv[5];
		int maxDropsPerStorageMod = atoi(argv[6]);
		int numCellsBetweenMods = atoi(argv[7]);

		cout << "Executing placer with: ";
		cout << pDescrips.at(pIndex) << ", ";
		cout << inFile << " In File, ";
		cout << outFile << " Out File." << endl;


		Synthesis::Place(inFile, outFile, pEnums.at(pIndex), pmEnums.at(pmIndex), maxDropsPerStorageMod, numCellsBetweenMods);
	}
	else if (cmd == "R")
	{
		if (argc != 8)
		{
			cout << "Improper number of parameters for routing (R)." << endl << endl;
			ForceCorrectUsage();
		}

		int rIndex = GetLibIndexFromRouteKey(Util::StringToUpper(argv[2]));
		int pmIndex = GetLibIndexFromPinMapKey(Util::StringToUpper(argv[3]));
		int cIndex = GetLibIndexFromCompKey(Util::StringToUpper(argv[4]));
		int peIndex = GetLibIndexFromPeKey(Util::StringToUpper(argv[5]));
		string inFile = argv[6];
		int numCellsBetweenMods = atoi(argv[7]);

		cout << "Executing router with: ";
		cout << rDescrips.at(rIndex) << ", ";
		cout << cDescrips.at(cIndex) << ", ";
		cout << peDescrips.at(peIndex) << ", ";
		cout << inFile << " In File." << endl;

		Synthesis::Route(inFile, rEnums.at(rIndex), pmEnums.at(pmIndex), cEnums.at(cIndex), peEnums.at(peIndex), numCellsBetweenMods);
	}
	else if (cmd == "WR")
	{
		if (argc != 6)
		{
			cout << "Improper number of parameters for wire-routing (WR)." << endl << endl;
			ForceCorrectUsage();
		}

		//wr [wireRouteType] inputInterfaceFile numHorizTracks numVertTracks
		int wrIndex = GetLibIndexFromWrKey(Util::StringToUpper(argv[2]));
		string inFile = argv[3];
		int numHorizTracks = atoi(argv[4]);
		int numVertTracks = atoi(argv[5]);

		cout << "Executing wire-router with: ";
		cout << wrDescrips.at(wrIndex) << ", ";
		cout << inFile << " Input File, ";
		cout << numHorizTracks << " numHorizTracks, ";
		cout << numVertTracks << " numVertTracks." << endl;

		Synthesis::WireRoute(inFile, wrEnums.at(wrIndex), numHorizTracks, numVertTracks);
	}
	else
		ForceCorrectUsage();

	cout << "-------------------------" << endl << "Simulation Complete. Exiting Static MF Simulator." << endl;
}

//////////////////////////////////////////////////////////////////////////////
// Given a key, searches the scheduler library to find the requested
// scheduler. Returns the index into the library.
//////////////////////////////////////////////////////////////////////////////
int CmdLine::GetLibIndexFromSchedKey(string sKey)
{
	for (int i = 0; i < sKeys.size(); i++)
		if (sKey == sKeys.at(i))
			return i;

	cout << "Invalid parameter: " << sKey << " is not a valid scheduler key/type." << endl;
	ForceCorrectUsage();
}

//////////////////////////////////////////////////////////////////////////////
// Given a key, searches the placer library to find the requested placer.
// Returns the index into the library.
//////////////////////////////////////////////////////////////////////////////
int CmdLine::GetLibIndexFromPlaceKey(string pKey)
{
	for (int i = 0; i < pKeys.size(); i++)
		if (pKey == pKeys.at(i))
			return i;

	cout << "Invalid parameter: " << pKey << " is not a valid placer key/type." << endl;
	ForceCorrectUsage();
}

//////////////////////////////////////////////////////////////////////////////
// Given a key, searches the router library to find the requested router.
// Returns the index into the library.
//////////////////////////////////////////////////////////////////////////////
int CmdLine::GetLibIndexFromRouteKey(string rKey)
{
	for (int i = 0; i < rKeys.size(); i++)
		if (rKey == rKeys.at(i))
			return i;

	cout << "Invalid parameter: " << rKey << " is not a valid router key/type." << endl;
	ForceCorrectUsage();
}

//////////////////////////////////////////////////////////////////////////////
// Given a key, searches the resource allocation library to find the requested
// resource allocation type. Returns the index into the library.
//////////////////////////////////////////////////////////////////////////////
int CmdLine::GetLibIndexFromResourceAllocationKey(string raKey)
{
	for (int i = 0; i < raKeys.size(); i++)
		if (raKey == raKeys.at(i))
			return i;

	cout << "Invalid parameter: " << raKey << " is not a valid resource-allocation key/type." << endl;
	ForceCorrectUsage();
}

//////////////////////////////////////////////////////////////////////////////
// Given a key, searches the pin-mapper library to find the requested pin-mapper.
// Returns the index into the library.
//////////////////////////////////////////////////////////////////////////////
int CmdLine::GetLibIndexFromPinMapKey(string pmKey)
{
	for (int i = 0; i < pmKeys.size(); i++)
		if (pmKey == pmKeys.at(i))
			return i;

	cout << "Invalid parameter: " << pmKey << " is not a valid pin-mapper key/type." << endl;
	ForceCorrectUsage();
}

//////////////////////////////////////////////////////////////////////////////
// Given a key, searches the execution type library to find the requested
// wire-routing type. Returns the index into the library.
//////////////////////////////////////////////////////////////////////////////
int CmdLine::GetLibIndexFromWrKey(string wrKey)
{
	for (int i = 0; i < wrKeys.size(); i++)
		if (wrKey == wrKeys.at(i))
			return i;

	cout << "Invalid parameter: " << wrKey << " is not a valid wire-routing key/type." << endl;
	ForceCorrectUsage();
}

//////////////////////////////////////////////////////////////////////////////
// Given a key, searches the compaction library to find the requested router.
// Returns the index into the library.
//////////////////////////////////////////////////////////////////////////////
int CmdLine::GetLibIndexFromCompKey(string cKey)
{
	for (int i = 0; i < cKeys.size(); i++)
		if (cKey == cKeys.at(i))
			return i;

	cout << "Invalid parameter: " << cKey << " is not a valid compaction key/type." << endl;
	ForceCorrectUsage();
}

//////////////////////////////////////////////////////////////////////////////
// Given a key, searches the processing engine library to find the requested
// processing engine. Returns the index into the library.
//////////////////////////////////////////////////////////////////////////////
int CmdLine::GetLibIndexFromPeKey(string peKey)
{
	for (int i = 0; i < peKeys.size(); i++)
		if (peKey == peKeys.at(i))
			return i;

	cout << "Invalid parameter: " << peKey << " is not a valid processing engine key/type." << endl;
	ForceCorrectUsage();
}

//////////////////////////////////////////////////////////////////////////////
// Given a key, searches the execution type library to find the requested
// execution type. Returns the index into the library.
//////////////////////////////////////////////////////////////////////////////
int CmdLine::GetLibIndexFromEtKey(string etKey)
{
	for (int i = 0; i < etKeys.size(); i++)
		if (etKey == etKeys.at(i))
			return i;

	cout << "Invalid parameter: " << etKey << " is not a valid execution key/type." << endl;
	ForceCorrectUsage();
}
