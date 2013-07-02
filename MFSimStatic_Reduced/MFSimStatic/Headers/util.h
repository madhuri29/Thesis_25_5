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
 * Name: Util (Utility)															*
 *																				*
 * Details: This class handles ALL I/O for the synthesis interface files.		*
 * Simply pass it the necessary classes that were modified by the synthesis		*
 * methods and it will write the file to output to be saved or used later by	*
 * the simulator or visualizer.  Also reads interface files into the necessary	*
 * data structures to be used directly by the synthesis methods. Creates output	*
 * files that can be read by the Java Visualizer.								*
 *																				*
 * Changing the output of any file will cause issues if not done properly. For	*
 * instance, if you change the information in the routing output file, you will	*
 * also need to change the way the Java Visualizer reads that file or the 		*
 * visualizer may break.														*
 *-----------------------------------------------------------------------------*/
#ifndef _UTIL_H
#define _UTIL_H

using namespace std;
#include "Resources/dmfb_arch.h"
#include <fstream>
#include <string>
#include "sort.h"
#include "dag.h"

class DmfbArch;

class Util
{
	public:
		// Constructors
		Util();
		virtual ~Util();

		// String Methods
		static void TokenizeString(string str, string token, vector<string> *dest);
		static string StringToUpper(string strToConvert);
		static string TrimString(string str);
		static int countSubstring(const string& str, const string& sub);
		static string getOperationString(OperationType ot);
		OperationType getOpTypeFromString(string ot);

		// File Input Methods
		static void ParseLine(string line, string *tag, vector<string> *parameters);
		static DAG *ReadDagFromFile(string fileName);
		static DmfbArch *ReadDmfbArchFromFile(string fileName);
		static void ReadScheduledDagAndArchFromFile(DAG *dag, DmfbArch *arch, string fileName);
		static void ReadPlacedDagAndArchFromFile(DAG *dag, DmfbArch *arch, vector<ReconfigModule *> *rModules, string fileName);
		static void ReadRoutedSimPinsAndArchFromFile(DmfbArch *arch, vector<vector<int> *> *pinActivations, string fileName);

		// File Output Methods
		static void WriteDagToFile(DAG *dag, string fileName);
		static void WriteScheduledDagAndArchToFile(DAG *dag, DmfbArch *arch, string fileName);
		static void WritePlacedDagAndArchToFile(DAG *dag, DmfbArch *arch, vector<ReconfigModule *> *rModules, string fileName);
		static void WriteRoutedDagAndArchToFile(DAG *dag, DmfbArch *arch, vector<ReconfigModule *> *rModules, map<Droplet *, vector<RoutePoint *> *> *routes, vector<vector<int> *> *pinActivations, vector<unsigned long long> *tsBeginningCycle, string fileName);
		static void WriteCompactedRoutesToFile(DAG *dag, DmfbArch *arch, vector<ReconfigModule *> *rModules, map<Droplet *, vector<RoutePoint *> *> *routes, vector<unsigned long long> *tsBeginningCycle, string fileName);
		static void WriteDmfbProgramToFile(DAG *dag, DmfbArch *arch, vector<ReconfigModule *> *rModules, map<Droplet *, vector<RoutePoint *> *> *routes, vector<unsigned long long> *tsBeginningCycle, string fileName);
		static void WriteHardwareFileWithWireRoutes(DmfbArch *arch, string fileName);
};
#endif
