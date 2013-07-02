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
 * Source: Droplet.java (Droplet)												*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: October 7, 2012							*
 *																				*
 * Details: Provides a basic structure for a droplet.							*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/

package dmfbSimVisualizer.common;



public class Droplet {
	public enum COLOR { GREEN, YELLOW, ORANGE, RED, CYAN, LIGHT_GRAY, WHITE, MAGENTA } 
	public int Cell;
	public int Global_ID;
	public String Status;
	public COLOR Color;
	public int xCell;
	public int yCell;
	
	//////////////////////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////////////////////
	public Droplet(int cell, int gid, String status)
	{
		Cell = cell;
		Global_ID = gid;
		Status = status;
		Color = GetColor(Status);		
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////////////////////
	public Droplet(int x, int y, int gid, String status)
	{
		xCell = x;
		yCell = y;
		Global_ID = gid;
		Status = status;
		Color = GetColor(Status);
	}	
	
	//////////////////////////////////////////////////////////////////////////////////////
	// Gets the appropriate color given the status of the droplet
	//////////////////////////////////////////////////////////////////////////////////////
	private COLOR GetColor(String status)
	{
		if (status.contains("INTERFERENCE WAIT"))
			return COLOR.YELLOW;
		else if (status.contains("PROCESSING"))
			return COLOR.WHITE;
		else if (status.contains("RETIRING"))
			return COLOR.CYAN;
		else if (status.contains("OUTPUTTING"))
			return COLOR.ORANGE;
		else if (status.contains("PROCESS WAIT"))
			return COLOR.RED;
		else
			return COLOR.GREEN;
	}
}

