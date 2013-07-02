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
 * Source: main.cc																*
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
#ifndef _MAIN
#define _MAIN

using namespace std;
#include "../Headers/Testing/test.h"
#include "../Headers/command_line.h"
#include "../Headers/synthesis.h"
#include "../Headers/enums.h"

#include <math.h>

///////////////////////////////////////////////////////////////////////////////////
// Main function called when starting program.
///////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	CmdLine cmdLine;

	if (cmdLine.IsUsingCommandLine(argc))
		cmdLine.ExecuteCommand(argc, argv);
	else // Hard-coded Sandbox
	{
		//cmdLine.ForceCorrectUsage(); // Comment out to be able to run following code
		cout << "Executing Hard-coded Sandbox MF Simulation" << endl << "-----------------------------------------------"<< endl;


		//DAG *d = BiocodeTest::Create_B3_Protein_Levels(12, 1, 1);
		//Util::WriteDagToFile(d, "B4_ProteinSplit_12_Eq.txt");
		//exit(1);
		////////////////////////////////////////////////////////////////////////////
		// Synthesis Variables
		////////////////////////////////////////////////////////////////////////////
		int maxDropsPerStorageMod = 2;
		int minCellsBetweenIrMods = 1;
		int numHorizTracks = 3;
		int numVertTracks = 3;

		////////////////////////////////////////////////////////////////////////////
		// JETC STATIC ONLINE TESTS
		////////////////////////////////////////////////////////////////////////////

		//DAG * dag = BiocodeTest::Create_2LevelProtein_With_SplitVolumeDetects();
		//DAG * dag = BiocodeTest::Create_2LevelProtein_Lev1FailureSubGraph();
		//DAG * dag = BiocodeTest::Create_2LevelProtein_Lev2SingleFailureSubGraph();
		//DAG * dag = BiocodeTest::Create_2LevelProtein_Lev2DoubleFailureSubGraph();
		//Util::WriteDagToFile(dag, "Assays/B3_ProteinSplit2FT.txt");
		//Synthesis::Schedule("Assays/B3_ProteinSplit2FT.txt", "DmfbArchs/Arch_15_19_B3.txt", "Output/1_SCHED_to_PLACE.txt", LIST_S, GRISSOM_FIX_RA, INDIVADDR_PM, maxDropsPerStorageMod);
		//Synthesis::Place("Output/1_SCHED_to_PLACE.txt", "Output/2_PLACE_to_ROUTE.txt", GRISSOM_FIX_P, INDIVADDR_PM, maxDropsPerStorageMod, minCellsBetweenIrMods);
		//Synthesis::Route("Output/2_PLACE_to_ROUTE.txt", ROY_MAZE_R, INDIVADDR_PM, BEG_COMP, FIXED_FULL_PE, minCellsBetweenIrMods);

		////////////////////////////////////////////////////////////////////////////
		// Entire Synthesis Flow Tests
		////////////////////////////////////////////////////////////////////////////
		//Synthesis::EntireFlow("Assays/B1_PCR.txt", "DmfbArchs/Arch_15_19_B1.txt", LIST_S, GRISSOM_LE_B, ROY_MAZE_R, GRISSOM_FIX_1_RA, INDIVADDR_PM, NONE_WR, BEG_COMP, FIXED_FULL_PE, ALL_EX, maxDropsPerStorageMod, minCellsBetweenIrMods, numHorizTracks, numVertTracks);
		//Synthesis::EntireFlow("Assays/B1_PCR.txt", "DmfbArchs/Arch_15_13_B1.txt", PATH_S, GRISSOM_LE_B, ROY_MAZE_R, GRISSOM_FIX_1_RA, INDIVADDR_PM, NONE_WR, BEG_COMP, FIXED_FULL_PE, ALL_EX, maxDropsPerStorageMod, minCellsBetweenIrMods, numHorizTracks, numVertTracks);
		//Synthesis::EntireFlow("Assays/B1_PCR.txt", "DmfbArchs/Arch_PC1_B1.txt", LIST_S, GRISSOM_LE_B, ROY_MAZE_R, GRISSOM_FIX_1_RA, INDIVADDR_PM, NONE_WR, BEG_COMP, FIXED_FULL_PE, ALL_EX, maxDropsPerStorageMod, minCellsBetweenIrMods, numHorizTracks, numVertTracks);
		//Synthesis::EntireFlow("Assays/B2_InVitro_Ex1_2s_2r.txt", "DmfbArchs/Arch_PC1_B2.txt", LIST_S, GRISSOM_LE_B, ROY_MAZE_R, GRISSOM_FIX_1_RA, INDIVADDR_PM, NONE_WR, BEG_COMP, FIXED_FULL_PE, ALL_EX, maxDropsPerStorageMod, minCellsBetweenIrMods, numHorizTracks, numVertTracks);
		//Synthesis::EntireFlow("Assays/B2_InVitro_Ex1_2s_2r.txt", "DmfbArchs/Arch_PC1_B2.txt", PATH_S, GRISSOM_LE_B, ROY_MAZE_R, GRISSOM_FIX_1_RA, INDIVADDR_PM, NONE_WR, BEG_COMP, FIXED_FULL_PE, ALL_EX, maxDropsPerStorageMod, minCellsBetweenIrMods, numHorizTracks, numVertTracks);
		//Synthesis::EntireFlow("Assays/B2_InVitro_Ex2_2s_3r.txt", "DmfbArchs/Arch_PC1_B2.txt", LIST_S, GRISSOM_LE_B, ROY_MAZE_R, GRISSOM_FIX_1_RA, INDIVADDR_PM, NONE_WR, BEG_COMP, FIXED_FULL_PE, ALL_EX, maxDropsPerStorageMod, minCellsBetweenIrMods, numHorizTracks, numVertTracks);
		//Synthesis::EntireFlow("Assays/B2_InVitro_Ex3_3s_3r.txt", "DmfbArchs/Arch_PC1_B2.txt", LIST_S, GRISSOM_LE_B, ROY_MAZE_R, GRISSOM_FIX_1_RA, INDIVADDR_PM, NONE_WR, BEG_COMP, FIXED_FULL_PE, ALL_EX, maxDropsPerStorageMod, minCellsBetweenIrMods, numHorizTracks, numVertTracks);
		//Synthesis::EntireFlow("Assays/B2_InVitro_Ex4_3s_4r.txt", "DmfbArchs/Arch_PC1_B2.txt", LIST_S, GRISSOM_LE_B, ROY_MAZE_R, GRISSOM_FIX_1_RA, INDIVADDR_PM, NONE_WR, BEG_COMP, FIXED_FULL_PE, ALL_EX, maxDropsPerStorageMod, minCellsBetweenIrMods, numHorizTracks, numVertTracks);
		//Synthesis::EntireFlow("Assays/B2_InVitro_Ex5_4s_4r.txt", "DmfbArchs/Arch_PC1_B2.txt", LIST_S, GRISSOM_LE_B, ROY_MAZE_R, GRISSOM_FIX_1_RA, INDIVADDR_PM, NONE_WR, BEG_COMP, FIXED_FULL_PE, ALL_EX, maxDropsPerStorageMod, minCellsBetweenIrMods, numHorizTracks, numVertTracks);
		//Synthesis::EntireFlow("Assays/B3_Protein.txt", "DmfbArchs/Arch_PC1_B3.txt", LIST_S, GRISSOM_LE_B, ROY_MAZE_R, GRISSOM_FIX_1_RA, INDIVADDR_PM, NONE_WR, BEG_COMP, FIXED_FULL_PE, ALL_EX, maxDropsPerStorageMod, minCellsBetweenIrMods, numHorizTracks, numVertTracks);
		//Synthesis::EntireFlow("Assays/B4_ProteinSplit_1_Eq.txt", "DmfbArchs/Arch_PC1_B3.txt", LIST_S, GRISSOM_LE_B, ROY_MAZE_R, GRISSOM_FIX_1_RA, INDIVADDR_PM, NONE_WR, BEG_COMP, FIXED_FULL_PE, ALL_EX, maxDropsPerStorageMod, minCellsBetweenIrMods, numHorizTracks, numVertTracks);


		////////////////////////////////////////////////////////////////////////////
		// Individual Synthesis Steps Tests - Field-Programmable Pin-Constrained
		////////////////////////////////////////////////////////////////////////////
		//Synthesis::Schedule("Assays/B1_PCR.txt", "DmfbArchs/Arch_PC1_B1.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_S, PC_INHERENT_RA, FPPC_PM, maxDropsPerStorageMod);
		//Synthesis::Schedule("Assays/B2_InVitro_Ex1_2s_2r.txt", "DmfbArchs/Arch_PC1_B2.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_S, PC_INHERENT_RA, FPPC_PM, maxDropsPerStorageMod);
		//Synthesis::Schedule("Assays/B2_InVitro_Ex2_2s_3r.txt", "DmfbArchs/Arch_PC1_B2.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_S, PC_INHERENT_RA, FPPC_PM, maxDropsPerStorageMod);
		//Synthesis::Schedule("Assays/B2_InVitro_Ex3_3s_3r.txt", "DmfbArchs/Arch_PC1_B2.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_S, PC_INHERENT_RA, FPPC_PM, maxDropsPerStorageMod);
		//Synthesis::Schedule("Assays/B2_InVitro_Ex4_3s_4r.txt", "DmfbArchs/Arch_PC1_B2.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_S, PC_INHERENT_RA, FPPC_PM, maxDropsPerStorageMod);
		//Synthesis::Schedule("Assays/B2_InVitro_Ex5_4s_4r.txt", "DmfbArchs/Arch_PC1_B2.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_S, PC_INHERENT_RA, FPPC_PM, maxDropsPerStorageMod);
		//Synthesis::Schedule("Assays/B3_Protein.txt", "DmfbArchs/Arch_PC1_B3.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_S, PC_INHERENT_RA, FPPC_PM, maxDropsPerStorageMod);
		//Synthesis::Schedule("Assays/B4_ProteinSplit_4_Eq.txt", "DmfbArchs/Arch_PC1_B3.txt", "Output/1_SCHED_to_PLACE.txt", FPPC_S, PC_INHERENT_RA, FPPC_PM, maxDropsPerStorageMod);
		//Synthesis::Place("Output/1_SCHED_to_PLACE.txt", "Output/2_PLACE_to_ROUTE.txt", FPPC_LE_B, FPPC_PM, maxDropsPerStorageMod, minCellsBetweenIrMods);
		//Synthesis::Route("Output/2_PLACE_to_ROUTE.txt", FPPC_R, FPPC_PM, INHERENT_COMP, FIXED_FULL_PE, minCellsBetweenIrMods);

		////////////////////////////////////////////////////////////////////////////
		// Individual Synthesis Steps Tests - Independently Addressed Electrodes
		////////////////////////////////////////////////////////////////////////////
		Synthesis::Schedule("Assays/B4_ProteinSplit_3_Eq.txt", "DmfbArchs/Arch_15_19_B3.txt", "Output/1_SCHED_to_PLACE.txt", LIST_S, GRISSOM_FIX_1_RA, INDIVADDR_PM, maxDropsPerStorageMod);
		//Synthesis::Schedule("Assays/B1_PCR.txt", "DmfbArchs/Arch_15_19_B1.txt", "Output/1_SCHED_to_PLACE.txt", LIST_S, GRISSOM_FIX_1_RA, INDIVADDR_PM, maxDropsPerStorageMod);
		//Synthesis::Schedule("Assays/B2_InVitro_Ex1_2s_2r.txt", "DmfbArchs/Arch_15_19_B2.txt", "Output/1_SCHED_to_PLACE.txt", LIST_S, GRISSOM_FIX_1_RA, INDIVADDR_PM, maxDropsPerStorageMod);
		//Synthesis::Schedule("Assays/B2_InVitro_Ex2_2s_3r.txt", "DmfbArchs/Arch_15_19_B2.txt", "Output/1_SCHED_to_PLACE.txt", GENET_S, GRISSOM_FIX_1_RA, INDIVADDR_PM, maxDropsPerStorageMod);
		//Synthesis::Schedule("Assays/B2_InVitro_Ex3_3s_3r.txt", "DmfbArchs/Arch_15_19_B2.txt", "Output/1_SCHED_to_PLACE.txt", LIST_S, GRISSOM_FIX_1_RA, INDIVADDR_PM, maxDropsPerStorageMod);
		//Synthesis::Schedule("Assays/B2_InVitro_Ex4_3s_4r.txt", "DmfbArchs/Arch_15_19_B2.txt", "Output/1_SCHED_to_PLACE.txt", LIST_S, GRISSOM_FIX_1_RA, INDIVADDR_PM, maxDropsPerStorageMod);
		//Synthesis::Schedule("Assays/B2_InVitro_Ex5_4s_4r.txt", "DmfbArchs/Arch_15_19_B2.txt", "Output/1_SCHED_to_PLACE.txt", LIST_S, GRISSOM_FIX_1_RA, INDIVADDR_PM, maxDropsPerStorageMod);
		//Synthesis::Schedule("Assays/B1_PCR.txt", "DmfbArchs/Arch_15_19_B1.txt", "Output/1_SCHED_to_PLACE.txt", PATH_S, GRISSOM_FIX_1_RA, INDIVADDR_PM, maxDropsPerStorageMod);
		//Synthesis::Schedule("Assays/B4_ProteinSplit_3_Eq.txt", "DmfbArchs/Arch_15_19_B3.txt", "Output/1_SCHED_to_PLACE.txt", GENET_S, GRISSOM_FIX_1_RA, INDIVADDR_PM, maxDropsPerStorageMod);
		Synthesis::Place("Output/1_SCHED_to_PLACE.txt", "Output/2_PLACE_to_ROUTE.txt", GRISSOM_LE_B, INDIVADDR_PM, maxDropsPerStorageMod, minCellsBetweenIrMods);
		Synthesis::Route("Output/2_PLACE_to_ROUTE.txt", ROY_MAZE_R, INDIVADDR_PM, BEG_COMP, FIXED_FULL_PE, minCellsBetweenIrMods);
		//Synthesis::WireRoute("Output/3_ROUTE_to_SIM.txt", NONE_WR, numHorizTracks, numVertTracks);
		cout << "-------------------------" << endl << "Exiting MF Simulator" << endl;
	}

	return 0;
}

#endif
