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
/*------------------------Class/Implementation Details--------------------------*
 * Source: RouteParser.java (Router Parser)										*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: October 7, 2012							*
 *																				*
 * Details: Parses an input file based on the normal routed format.				*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/

package dmfbSimVisualizer.parsers;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import dmfbSimVisualizer.common.*;

public class RouteParser {
	private ArrayList<ArrayList<Droplet>> DropletHistory;
	private ArrayList<ArrayList<Integer>> PinHistory;
	private ArrayList<Integer> CycleNums;
	private ArrayList<Integer> RouteCycleRange;
	private ArrayList<Integer> TsCycleRange;
	private ArrayList<FixedArea> ExternalResources;
	private ArrayList<FixedArea> ResourceLocations;
	private ArrayList<ReconfigArea> ReconfigAreas;
	private ArrayList<IoPort> IoPorts;
	private Map<Integer, ArrayList<String>> CoordsAtPin;
	private int numXcells;
	private int numYcells;
	
	//////////////////////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////////////////////
	public RouteParser(String fileName)
	{
		DropletHistory = new ArrayList<ArrayList<Droplet>>();
		PinHistory = new ArrayList<ArrayList<Integer>>();
		CycleNums = new ArrayList<Integer>();
		RouteCycleRange = new ArrayList<Integer>();
		TsCycleRange = new ArrayList<Integer>();
		ExternalResources = new ArrayList<FixedArea>();
		ResourceLocations = new ArrayList<FixedArea>();
		ReconfigAreas = new ArrayList<ReconfigArea>();
		IoPorts = new ArrayList<IoPort>();
		CoordsAtPin = new HashMap<Integer, ArrayList<String>>();
		numXcells = 0;
		numYcells = 0;
		ReadFile(fileName);
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	//Open and read from MF output file
	//////////////////////////////////////////////////////////////////////////////////////
	private void ReadFile(String fileName) {
		BufferedReader bf = null;
		try {
			bf = new BufferedReader(new FileReader(fileName));
			String line = null;
			int cycleNum = 0;
			
			while ((line = bf.readLine()) != null)
			{
				line = line.toUpperCase();
				if (line.contains("=======================INITIALIZATION======================="))
				{
					// Read each line
					while (((line = bf.readLine()) != null))
					{
						line = line.toUpperCase();
						if (line.contains("=======================INIT DONE======================="))
							break;
						else if (line.startsWith(("DIM (")))
						{
							String params = line.substring(line.indexOf("(")+1, line.indexOf(")"));
							String[] tokens = params.split(",");
							for (int i = 0; i < tokens.length; i++)
								tokens[i] = tokens[i].trim();
							if (tokens.length != 2)
							{
								MFError.DisplayError(line + "\n\n" + "Dim must have 2 parameters: ([X_Cells], [Y_Cells])");
								return;
							}
							numXcells = Integer.parseInt(tokens[0]);
							numYcells = Integer.parseInt(tokens[1]);
						}
						else if (line.startsWith("PINMAP ("))
						{
							String params = line.substring(line.indexOf("(")+1, line.indexOf(")"));
							String[] tokens = params.split(",");
							for (int i = 0; i < tokens.length; i++)
								tokens[i] = tokens[i].trim();
							if (tokens.length != numXcells*numYcells)
							{
								MFError.DisplayError(line + "\n\n" + "Dim must have " + numXcells*numYcells + " parameters: ([X_Cells], [Y_Cells])");
								return;
							}
							
							int i = 0;
							
							for (int y = 0; y < numYcells; y++)
							{
								for (int x = 0; x < numXcells; x++)
								{
									String coord = "(" + x + ", " + y + ")";
									
									int pinNo = Integer.parseInt(tokens[i++]);
									
									ArrayList<String> al = CoordsAtPin.get(pinNo);
									if (al == null)
									{
										al = new ArrayList<String>();
										CoordsAtPin.put(pinNo, al);
									}									
									CoordsAtPin.get(pinNo).add(coord);
								}
							}
						}
						else if (line.startsWith("EXTERNALHEATER (") || line.startsWith("EXTERNALDETECTOR ("))
						{
							String params = line.substring(line.indexOf("(")+1, line.indexOf(")"));
							String[] tokens = params.split(",");
							for (int i = 0; i < tokens.length; i++)
								tokens[i] = tokens[i].trim();
							if (tokens.length != 5)
							{
								MFError.DisplayError(line + "\n\n" + "ExternalHeater/ExternalDetector must have 5 parameters: ([Heater/Detector_Id], [TL_X], [TL_Y], [BR_X], [BR_Y]) ");
								return;
							}
							FixedArea la = new FixedArea();
							la.id = Integer.parseInt(tokens[0]);
							la.tl_x = Integer.parseInt(tokens[1]);
							la.tl_y = Integer.parseInt(tokens[2]);
							la.br_x = Integer.parseInt(tokens[3]);
							la.br_y = Integer.parseInt(tokens[4]);
							
							if (line.startsWith("EXTERNALHEATER"))
							{
								la.name = "FH" + String.valueOf(la.id);
								la.opType = OperationType.HEAT;
							}
							else
							{
								la.name = "FD" + String.valueOf(la.id);
								la.opType = OperationType.DETECT;
							}

							ExternalResources.add(la);
						}
						else if (line.startsWith("RESOURCELOCATION ("))
						{
							String params = line.substring(line.indexOf("(")+1, line.indexOf(")"));
							String[] tokens = params.split(",");
							for (int i = 0; i < tokens.length; i++)
								tokens[i] = tokens[i].trim();
							if (tokens.length != 6)
							{
								MFError.DisplayError(line + "\n\n" + "Resource Location must have 6 parameters: ([HEAT/DETECT], [TL_X], [TL_Y], [BR_X], [BR_Y], tileNum) ");
								return;
							}
							FixedArea fa = new FixedArea();
							fa.tl_x = Integer.parseInt(tokens[1]);
							fa.tl_y = Integer.parseInt(tokens[2]);
							fa.br_x = Integer.parseInt(tokens[3]);
							fa.br_y = Integer.parseInt(tokens[4]);
							// We don't care about the resource type (tokens[0]); just using to draw box

							ResourceLocations.add(fa);
						}
						else if (line.startsWith("RECONFIGMIXER (") || line.startsWith("RECONFIGSPLITTER (") || line.startsWith("RECONFIGHEATER (") || line.startsWith("RECONFIGDETECTOR (") || line.startsWith("RECONFIGSTORAGE ("))
						{
							String params = line.substring(line.indexOf("(")+1, line.indexOf(")"));
							String[] tokens = params.split(",");
							for (int i = 0; i < tokens.length; i++)
								tokens[i] = tokens[i].trim();
							if (!(tokens.length == 7 || tokens.length == 8))
							{
								MFError.DisplayError(line + "\n\n" + "ReconfigMixer/Splitter/Heater/Detector must have 7 or 8 parameters: ([Mixer/Splitter/Heater/Detector_Id], [TL_X], [TL_Y], [BR_X], [BR_Y], [Start_Cycle_Inclusive], [Stop_Cycle_Exclusive], [Reconfig_Name](optional))");
								return;
							}
							ReconfigArea la = new ReconfigArea();
							la.id = Integer.parseInt(tokens[0]);
							la.tl_x = Integer.parseInt(tokens[1]);
							la.tl_y = Integer.parseInt(tokens[2]);
							la.br_x = Integer.parseInt(tokens[3]);
							la.br_y = Integer.parseInt(tokens[4]);
							la.start_TS = Long.parseLong(tokens[5]);
							la.stop_TS = Long.parseLong(tokens[6]);
														
							if (line.startsWith("RECONFIGMIXER ("))
							{
								la.opType = OperationType.MIX;
								la.name = "M" + String.valueOf(la.id);
							}
							else if (line.startsWith("RECONFIGSPLITTER ("))
							{
								la.opType = OperationType.SPLIT;
								la.name = "S" + String.valueOf(la.id);
							}
							else if (line.startsWith("RECONFIGHEATER ("))
							{
								la.opType = OperationType.HEAT;
								la.name = "H" + String.valueOf(la.id);
							}
							else if (line.startsWith("RECONFIGDETECTOR ("))
							{
								la.opType = OperationType.DETECT;
								la.name = "D" + String.valueOf(la.id);
							}
							else if (line.startsWith("RECONFIGSTORAGE ("))
							{
								la.opType = OperationType.STORAGE;
								la.name = "St" + String.valueOf(la.id);
							}
							if (tokens.length == 8)
								la.name = tokens[7];

							ReconfigAreas.add(la);
						}
						else if (line.startsWith("INPUT (") || line.startsWith("OUTPUT ("))
						{
							String params = line.substring(line.indexOf("(")+1, line.indexOf(")"));
							String[] tokens = params.split(",");
							for (int i = 0; i < tokens.length; i++)
								tokens[i] = tokens[i].trim();
							if (tokens.length != 4)
							{
								MFError.DisplayError(line + "\n\n" + "Input/Output must have 4 parameters: ([Input/Output_Id], [Side=Top/Bottom/Left/Right], [PosXorY], [FluidName])");
								return;
							}
							IoPort ioPort = new IoPort();
							ioPort.id = Integer.parseInt(tokens[0]);
							ioPort.side = tokens[1].toUpperCase();
							ioPort.pos_xy = Integer.parseInt(tokens[2]);
							ioPort.portName = tokens[3].toUpperCase();
							
							if (line.startsWith("INPUT ("))
							{
								ioPort.opType = OperationType.INPUT;
								ioPort.name = "I" + String.valueOf(ioPort.id);
							}
							else
							{
								ioPort.opType = OperationType.OUTPUT;
								ioPort.name = "O" + String.valueOf(ioPort.id);
							}
							IoPorts.add(ioPort);
							
						}
						else if (line.startsWith("ROUTETO ("))
						{
							String params = line.substring(line.indexOf("(")+1, line.indexOf(")"));
							String[] tokens = params.split(",");
							for (int i = 0; i < tokens.length; i++)
								tokens[i] = tokens[i].trim();
							if (tokens.length != 3)
							{
								MFError.DisplayError(line + "\n\n" + "RouteTo must have 3 parameters: ([tsAboutToBegin], [startCycle], [endCycle])");
								return;
							}
							RouteCycleRange.add(Integer.parseInt(tokens[1]));
							RouteCycleRange.add(Integer.parseInt(tokens[2]));							
						}
						else if (line.startsWith("TSRANGE ("))
						{
							String params = line.substring(line.indexOf("(")+1, line.indexOf(")"));
							String[] tokens = params.split(",");
							for (int i = 0; i < tokens.length; i++)
								tokens[i] = tokens[i].trim();
							if (tokens.length != 3)
							{
								MFError.DisplayError(line + "\n\n" + "TS_Range must have 3 parameters: ([timeStep], [startCycle], [endCycle])");
								return;
							}
							TsCycleRange.add(Integer.parseInt(tokens[1]));
							TsCycleRange.add(Integer.parseInt(tokens[2]));							
						}
						else if (!(line.isEmpty() || line.startsWith("//")))
						{
							MFError.DisplayError(line + "\n\n" + "Unspecified line type for Initialization.");
							return;
						}
					}					
				}
				if (line.contains("COMMIT CYCLE "))
				{
					ArrayList<Droplet> Droplets = new ArrayList<Droplet>();
					ArrayList<Integer> Pins = new ArrayList<Integer>();
					cycleNum = Integer.parseInt(line.replaceAll( "[^\\d]", "" ));
					while (((line = bf.readLine()) != null))
					{
						line = line.toUpperCase();
						if (line.contains("NUMBER OF PINS TO ACTIVATE"))
						{
							DropletHistory.add(Droplets);
							CycleNums.add(cycleNum);
							break;
						}
						
						if (line.contains("DROPLET") && line.contains("CELL"))
						{
							int dropletNum;
							int x;
							int y;
							
							dropletNum = Integer.parseInt(line.substring(0, line.indexOf("CELL")).replaceAll("[^\\d]", ""));
							String coords = line.substring(line.indexOf("("), line.indexOf(")")+1);
							x = Integer.parseInt(coords.substring(coords.indexOf("("), coords.indexOf(",")).replaceAll("[^\\d]", ""));
							y = Integer.parseInt(coords.substring(coords.indexOf(","), coords.indexOf(")")).replaceAll("[^\\d]", ""));
							
							//String cellNumString = line.substring(line.indexOf("CELL"));
							//if (cellNumString.contains("("))
							//	cellNumString = cellNumString.substring(0, cellNumString.indexOf("("));
							//cellNum = Integer.parseInt(cellNumString.replaceAll("[^\\d]", ""));
							
							if (line.contains("INTERFERENCE WAIT"))
							{
								Droplet vD = new Droplet(x, y, dropletNum, "INTERFERENCE WAIT");
								Droplets.add(vD);
							}
							else if (line.contains("PROCESSING"))
							{
								Droplet vD = new Droplet(x, y, dropletNum, "PROCESSING");
								Droplets.add(vD);
							}

							else if (line.contains("RETIRING"))
							{
								Droplet vD = new Droplet(x, y, dropletNum, "RETIRING");
								Droplets.add(vD);
							}
							else if (line.contains("OUTPUTTING"))
							{
								Droplet vD = new Droplet(x, y, dropletNum, "OUTPUTTING");
								Droplets.add(vD);
							}
							else if (line.contains("PROCESS WAIT"))
							{
								Droplet vD = new Droplet(x, y, dropletNum, "PROCESS WAIT");
								Droplets.add(vD);
							}
							else
							{
								Droplet vD = new Droplet(x, y, dropletNum, "");
								Droplets.add(vD);
							}
						
						}
						else if (line.startsWith("ACTIVEPINS ("))
						{
							String params = line.substring(line.indexOf("(")+1, line.indexOf(")"));
							String[] tokens = params.split(",");
							for (int i = 0; i < tokens.length; i++)
								Pins.add(Integer.parseInt(tokens[i].trim()));
							PinHistory.add(Pins);							
						}
						else
						{
							MFError.DisplayError("INVALID DROPLET IN CYCLE " + cycleNum);
							return;
						}
					}					
				}				
			}			
		} catch (FileNotFoundException ex) {
			MFError.DisplayError("FileNotFoundException: " + ex.getMessage());
		} catch (IOException ex) {
			MFError.DisplayError("IOException: " + ex.getMessage());
		} finally {
			// Close the BufferedReader
			try {
				if (bf != null)
					bf.close();
			} catch (IOException ex) {
				ex.printStackTrace();
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Getters/Setters
	//////////////////////////////////////////////////////////////////////////////////////
	public ArrayList<ArrayList<Droplet>> getDropletHistory() {
		return DropletHistory;
	}
	public ArrayList<ArrayList<Integer>> getPinHistory() {
		return PinHistory;
	}
	public ArrayList<Integer> getCycleNums() {
		return CycleNums;
	}	
	public int getNumXcells() {
		return numXcells;
	}
	public int getNumYcells() {
		return numYcells;
	}
	public ArrayList<FixedArea> getExternalResources() {
		return ExternalResources;
	}
	public ArrayList<FixedArea> getResourceLocations() {
		return ResourceLocations;
	}
	public ArrayList<ReconfigArea> getReconfigAreas() {
		return ReconfigAreas;
	}
	public ArrayList<IoPort> getIoPorts() {
		return IoPorts;
	}
	public ArrayList<Integer> getRouteCycleRange() {
		return RouteCycleRange;
	}
	public ArrayList<Integer> getTsCycleRange() {
		return TsCycleRange;
	}
	public Map<Integer, ArrayList<String>> getCoordsAtPin() {
		return CoordsAtPin;
	}
}
