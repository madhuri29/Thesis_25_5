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
 * Source: Dmfb Drawer.java (DMFB Drawer)										*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: October 7, 2012							*
 *																				*
 * Details: This class contains the code to draw a blank DMFB from a DMFB arch.	*
 * It also contains functions that are used to overlay additional information	*
 * on the blank DMFBs to show placement/routing informaiton.
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
package dmfbSimVisualizer.common;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.RenderingHints;
import java.awt.Stroke;
import java.awt.Toolkit;
import java.awt.geom.AffineTransform;
import java.awt.image.AffineTransformOp;
import java.awt.image.BufferedImage;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;

import dmfbSimVisualizer.common.WireSegment.SEGTYPE;
import dmfbSimVisualizer.parsers.*;
import dmfbSimVisualizer.views.Main;


public class DmfbDrawer
{
	private static String ext = "png";
	private static String outDir = "Sim";
	private static int numCellsX;
	private static int numCellsY;	
	private static int cellDim;
	private static int ioLong;
	private static int ioShort;
	private static int offset;
	private static int arrayWidth;
	private static int arrayHeight;
	private static int locWidth;
	private static int locHeight;
	private static int ioOffset;
	
	//////////////////////////////////////////////////////////////////////////////////////
	// Rotates the input image 90 degrees clock-wise
	//////////////////////////////////////////////////////////////////////////////////////
	public static BufferedImage RotateCW(BufferedImage imgIn)
	{
		int width  = imgIn.getWidth();  
		int height = imgIn.getHeight();  
		BufferedImage   ret = new BufferedImage( height, width, imgIn.getType() );  

		for( int i=0 ; i < width ; i++ )  
			for( int j=0 ; j < height ; j++ )  
				ret.setRGB( height-1-j, i, imgIn.getRGB(i,j) );  
		return ret;
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	// Resizes image to given dimensions
	//////////////////////////////////////////////////////////////////////////////////////
    public static BufferedImage Resize(BufferedImage img, int newW, int newH)
    {  
        int w = img.getWidth();  
        int h = img.getHeight();  
        BufferedImage dimg = dimg = new BufferedImage(newW, newH, img.getType());  
        Graphics2D g = dimg.createGraphics();  
        g.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);  
        g.drawImage(img, 0, 0, newW, newH, 0, 0, w, h, null);  
        g.dispose();  
        return dimg;  
    }  
	
	//////////////////////////////////////////////////////////////////////////////////////
    // Draws an image containing an ioPort
	//////////////////////////////////////////////////////////////////////////////////////
	public static BufferedImage DrawIoPort(int cellDim, Color wellColor, Color inputCellColor, boolean isInput)
	{
		BufferedImage input = new BufferedImage(cellDim*3, (int)((double)cellDim*1.5), BufferedImage.TYPE_INT_RGB);
		Graphics2D g2dIn = (Graphics2D) input.getGraphics();
		g2dIn.setColor(Color.WHITE);
		
		// Draw Pads
		g2dIn.setStroke(new BasicStroke(1));
		g2dIn.fillRect(0,  0, input.getWidth(), input.getHeight());		
		g2dIn.setColor(wellColor);
		g2dIn.fillRect(0, 0, 2*cellDim, (int)((double)cellDim*1.5)-1);
		g2dIn.setColor(inputCellColor);
		g2dIn.fillRect(2*cellDim, (int)((double)input.getHeight() / 2.0 - (double)cellDim / 2.0), cellDim, cellDim);
		g2dIn.setColor(Color.BLACK);
		g2dIn.drawRect(0, 0, 2*cellDim, (int)((double)cellDim*1.5)-1);
		g2dIn.drawRect(2*cellDim, (int)((double)input.getHeight() / 2.0 - (double)cellDim / 2.0), cellDim, cellDim);
		
		// Draw Arrow
		if (isInput)
		{
			g2dIn.setStroke(new BasicStroke(cellDim/5));		
			g2dIn.drawLine(3*cellDim-(cellDim/10), (int)((double)input.getHeight() / 2.0), 3*cellDim-(cellDim/10) - (int)((double)cellDim / 4.0), (int)((double)input.getHeight() / 2.0) - (int)((double)cellDim / 4.0)); // \
			g2dIn.drawLine(2*cellDim+cellDim/10, (int)((double)input.getHeight() / 2.0), 3*cellDim-cellDim/10, (int)((double)input.getHeight() / 2.0)); // ----
			g2dIn.drawLine(3*cellDim-(cellDim/10), (int)((double)input.getHeight() / 2.0), 3*cellDim-(cellDim/10) - (int)((double)cellDim / 4.0), (int)((double)input.getHeight() / 2.0) + (int)((double)cellDim / 4.0)); // /
		}
		else
		{
			g2dIn.setStroke(new BasicStroke(cellDim/5));
			g2dIn.drawLine(2*cellDim+(cellDim/10), (int)((double)input.getHeight() / 2.0), 2*cellDim+(cellDim/10) + (int)((double)cellDim / 4.0), (int)((double)input.getHeight() / 2.0) - (int)((double)cellDim / 4.0)); // /
			g2dIn.drawLine(2*cellDim+cellDim/10, (int)((double)input.getHeight() / 2.0), 3*cellDim-cellDim/10, (int)((double)input.getHeight() / 2.0)); // ----
			g2dIn.drawLine(2*cellDim+(cellDim/10), (int)((double)input.getHeight() / 2.0), 2*cellDim+(cellDim/10) + (int)((double)cellDim / 4.0), (int)((double)input.getHeight() / 2.0) + (int)((double)cellDim / 4.0)); // \
		}
		return input;
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	// Sets the dimensions of the unit cell, which in turn sets the dimensions of the 
	// ioPorts, the array, and the LoC (entire image). Ensures that the largest possible
	// image will be created, while staying within the max width/height passed in
	//////////////////////////////////////////////////////////////////////////////////////
	public static void SetDmfbUnitDims(DmfbArch da, int maxWidth, int maxHeight)
	{
		// Set dimension values
		numCellsX = da.numXcells;
		numCellsY = da.numYcells;			
		cellDim = Math.min(maxWidth / (numCellsX + 6), maxHeight / (numCellsY + 6)); //60;
		ioLong = cellDim * 3;
		ioShort = (int)((double)cellDim * 1.5);
		offset = ioLong;
		arrayWidth =  cellDim * numCellsX;
		arrayHeight = cellDim * numCellsY;
		locWidth = arrayWidth + ioLong*2;
		locHeight = arrayHeight + ioLong*2;
		ioOffset = (ioShort - cellDim)/2;
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	// Draws a blank DMFB (contains electrodes, ioPorts, any heaters/detecters/etc...but
	// no droplets, reconfigurable modules or anything else specific to a certain time
	//////////////////////////////////////////////////////////////////////////////////////
	public static BufferedImage DrawBlankDmfb(DmfbArch da, DrawOptions drawOptions, Map<Integer, ArrayList<String>> coordsAtPin, Map<Integer, ArrayList<WireSegment>> wireSegsToPin) throws IOException
	{
		// Open Auxiliary Image Files	
		Image heatImg = (new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/heat_64.png"))).getImage();
		BufferedImage heat = new BufferedImage(heatImg.getWidth(null), heatImg.getHeight(null), BufferedImage.TYPE_INT_ARGB);
		((Graphics2D)heat.getGraphics()).drawImage(heatImg, 0, 0, null);
		heat = DmfbDrawer.Resize(heat, cellDim, cellDim);
		Image detImg = (new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/detect_64.png"))).getImage();
		BufferedImage detect = new BufferedImage(detImg.getWidth(null), detImg.getHeight(null), BufferedImage.TYPE_INT_ARGB);
		((Graphics2D)detect.getGraphics()).drawImage(detImg, 0, 0, null);
		detect = DmfbDrawer.Resize(detect, cellDim, cellDim);
		
		// Create I/O port images
		BufferedImage inputLeft = DrawIoPort(cellDim, new Color(155,187,89), new Color(215,228,189), true);
		BufferedImage inputTop = RotateCW(inputLeft);
		BufferedImage inputRight = RotateCW(inputTop);
		BufferedImage inputBottom = RotateCW(inputRight);		
		BufferedImage outputLeft = DrawIoPort(cellDim, new Color(230,224,236), new Color(179,162,199), false);
		BufferedImage outputTop = RotateCW(outputLeft);
		BufferedImage outputRight = RotateCW(outputTop);
		BufferedImage outputBottom = RotateCW(outputRight);
		
		// Create tiled LoC
		BufferedImage imgDmfb = new BufferedImage(locWidth, locHeight, BufferedImage.TYPE_INT_RGB);	
		Graphics2D g2d = (Graphics2D) imgDmfb.getGraphics();
		g2d.setColor(Color.WHITE);
		g2d.fillRect(0,  0, locWidth, locHeight);

		// Draw the pin-numbers on the bottom-right side	
		Color[] colors = {Color.RED, Color.GREEN, Color.ORANGE, Color.MAGENTA, Color.PINK, Color.CYAN, Color.YELLOW,  Color.LIGHT_GRAY};
		int maxLayers = 8;
		if (drawOptions.drawWireSegments && wireSegsToPin != null)
		{
			//fontSize = cellDim/2;
			//font = new Font("Arial", Font.BOLD, fontSize);
			//g2d.setFont(font);
			g2d.setColor(Color.RED);
			
    		
			for (Integer pinNo : wireSegsToPin.keySet())
			{
				ArrayList<WireSegment> wire = wireSegsToPin.get(pinNo);
	    		for (int i = 0; i < wire.size(); i++)
	    		{
	    			WireSegment ws = wire.get(i);
	    			
	    			if (ws.layer >= maxLayers)
	    				MFError.DisplayError("Currently not enough colors chosen for more than " + maxLayers + " layers.");
	    			else
	    				g2d.setColor(colors[ws.layer]);
	    			
	    			int trackXOff = cellDim / (da.numHTracks + 1);
	    			int trackYOff = cellDim / (da.numVTracks + 1); 
	    			if (ws.segmentType == SEGTYPE.LINE)
	    			{
	    				int x1 = ioLong + ws.sourceXCell*cellDim + (ws.sourceXTrack+1)*trackXOff;
	    				int y1 = ioLong + ws.sourceYCell*cellDim + (ws.sourceYTrack+1)*trackYOff;
	    				int x2 = ioLong + ws.destXCell*cellDim + (ws.destXTrack+1)*trackXOff;
	    				int y2 = ioLong + ws.destYCell*cellDim + (ws.destYTrack+1)*trackYOff;
	    				g2d.drawLine(x1, y1, x2, y2);
	    				g2d.fillOval(x1-trackXOff/2, y1-trackYOff/2, trackXOff, trackYOff);
	    				g2d.fillOval(x2-trackXOff/2, y2-trackYOff/2, trackXOff, trackYOff);
	    			}
	    			else
	    				MFError.DisplayError("Unknown wire-segment type.");
	    		}
			}
		}
		
		// Draw electrodes
		g2d.setColor(Color.BLACK);
		g2d.setStroke(new BasicStroke(1));
		for (int x = 0; x < numCellsX; x++)
			g2d.drawLine(offset + x*cellDim, offset, offset + x*cellDim, offset + arrayHeight);
		for (int y = 0; y < numCellsY; y++)
			g2d.drawLine(offset, offset + y*cellDim, offset + arrayWidth, offset + y*cellDim);
		//g2d.setStroke(new BasicStroke(1));

		
		// Draw resource locations (for fixed placements)
		if (drawOptions.drawResourceLocations)
		{
			for (int i = 0; i < da.ResourceLocations.size(); i++)
			{
				FixedArea fa = da.ResourceLocations.get(i);
	
				// Draw border around fixed area
				g2d.setColor(Color.BLACK);
				Stroke oldStroke = g2d.getStroke();
				g2d.setStroke(new BasicStroke(2));
				g2d.drawRect(fa.tl_x * cellDim + offset, fa.tl_y * cellDim + offset, (fa.br_x-fa.tl_x+1)*cellDim, (fa.br_y-fa.tl_y+1)*cellDim);
				g2d.setStroke(oldStroke);
			}
		}

		// Draw external resource graphics...says if can heat/detect
		for (int i = 0; i < da.ExternalResources.size(); i++)
		{
			FixedArea fa = da.ExternalResources.get(i);

			if (fa.opType == OperationType.DETECT)
			{	    		
				for (int x = fa.tl_x; x <= fa.br_x; x++)
				{
					for (int y = fa.tl_y; y <= fa.br_y; y++)
					{
						g2d.drawImage(detect, null, (x * cellDim) + offset, (y * cellDim) + offset);
						//g2d.setColor(Color.BLACK);
						//g2d.drawString(fa.name, x * cellDim + cellDim/5, y * cellDim + cellDim/2);	    				
					}
				}

			}
			else if (fa.opType == OperationType.HEAT)
			{
				for (int x = fa.tl_x; x <= fa.br_x; x++)
				{
					for (int y = fa.tl_y; y <= fa.br_y; y++)
					{
						g2d.drawImage(heat, null, (x * cellDim) + offset, (y * cellDim) + offset);
						//g2d.setColor(Color.BLACK);
						//g2d.drawString(fa.name, x * cellDim + cellDim/3, y * cellDim + 2*cellDim/3);
					}
				}
			}
		}

		// Draw chamber I/Os
		for (int i = 0; i < da.IoPorts.size(); i++)
		{
			IoPort ioPort = da.IoPorts.get(i);
			int fontSize = cellDim/2;
			Font font = new Font("Arial", Font.BOLD, fontSize);
			g2d.setFont(font);
			g2d.setColor(Color.BLACK);

			if (ioPort.opType.equals(OperationType.INPUT))
			{
				if (ioPort.side.contains("TOP") || ioPort.side.contains("NORTH"))
				{
					g2d.drawImage(inputTop, null, (ioPort.pos_xy * cellDim) + offset - ioOffset, 0);
					g2d.drawString(ioPort.portName.substring(0, Math.min(ioPort.portName.length(), 3)), (ioPort.pos_xy * cellDim) + offset - ioOffset + cellDim/5, cellDim + fontSize/3);			
				}
				if (ioPort.side.contains("BOTTOM") || ioPort.side.contains("SOUTH"))
				{
					g2d.drawImage(inputBottom, null, (ioPort.pos_xy * cellDim) + offset - ioOffset, offset + arrayHeight);
					g2d.drawString(ioPort.portName.substring(0, Math.min(ioPort.portName.length(), 3)), (ioPort.pos_xy * cellDim) + offset - ioOffset + cellDim/5, locHeight - cellDim + fontSize/3);
				}
				if (ioPort.side.contains("LEFT") || ioPort.side.contains("WEST"))
				{
					g2d.drawImage(inputLeft, null, 0, (ioPort.pos_xy * cellDim) + offset - ioOffset);
					g2d.drawString(ioPort.portName.substring(0, Math.min(ioPort.portName.length(), 3)), cellDim/5, (ioPort.pos_xy * cellDim) + ioLong + ioShort/2);
				}
				if (ioPort.side.contains("RIGHT") || ioPort.side.contains("EAST"))
				{
					g2d.drawImage(inputRight, null, offset + arrayWidth, (ioPort.pos_xy * cellDim) + offset - ioOffset);
					g2d.drawString(ioPort.portName.substring(0, Math.min(ioPort.portName.length(), 3)), locWidth - 2*cellDim + cellDim/5, (ioPort.pos_xy * cellDim) + ioLong + ioShort/2);
				} 	
			}
			else
			{
				if (ioPort.side.contains("TOP") || ioPort.side.contains("NORTH"))
				{
					g2d.drawImage(outputTop, null, (ioPort.pos_xy * cellDim) + offset - ioOffset, 0);
					g2d.drawString(ioPort.portName.substring(0, Math.min(ioPort.portName.length(), 3)), (ioPort.pos_xy * cellDim) + offset - ioOffset + cellDim/5, cellDim + fontSize/3);
				}
				if (ioPort.side.contains("BOTTOM") || ioPort.side.contains("SOUTH"))
				{
					g2d.drawImage(outputBottom, null, (ioPort.pos_xy * cellDim) + offset - ioOffset, offset + arrayHeight);
					g2d.drawString(ioPort.portName.substring(0, Math.min(ioPort.portName.length(), 3)), (ioPort.pos_xy * cellDim) + offset - ioOffset + cellDim/5, locHeight - cellDim + fontSize/3);
				}
				if (ioPort.side.contains("LEFT") || ioPort.side.contains("WEST"))
				{
					g2d.drawImage(outputLeft, null, 0, (ioPort.pos_xy * cellDim) + offset - ioOffset);
					g2d.drawString(ioPort.portName.substring(0, Math.min(ioPort.portName.length(), 3)), cellDim/5, (ioPort.pos_xy * cellDim) + ioLong + ioShort/2);
				}
				if (ioPort.side.contains("RIGHT") || ioPort.side.contains("EAST"))
				{
					g2d.drawImage(outputRight, null, offset + arrayWidth, (ioPort.pos_xy * cellDim) + offset - ioOffset);
					g2d.drawString(ioPort.portName.substring(0, Math.min(ioPort.portName.length(), 3)), locWidth - 2*cellDim + cellDim/5, (ioPort.pos_xy * cellDim) + ioLong + ioShort/2);
				}
			}
		}

		// Draw Array Border
		g2d.setColor(Color.BLACK);
		Stroke oldStroke = g2d.getStroke();
		g2d.setStroke(new BasicStroke(2));
		g2d.drawRect(offset, offset, arrayWidth, arrayHeight);
		g2d.setStroke(oldStroke);		

		// Draw the coordinates on the left/top side
		int fontSize = cellDim/2;
		Font font = new Font("Arial", Font.BOLD, fontSize);
		g2d.setFont(font);
		g2d.setColor(Color.BLACK);
		for (int i = 0; i < numCellsX; i++)
			g2d.drawString(String.valueOf(i), ioLong + i*cellDim + cellDim/10, ioLong + fontSize);
		for (int i = 1; i < numCellsY; i++)
			g2d.drawString(String.valueOf(i), ioLong + cellDim/10, ioLong + i*cellDim + fontSize);	

		
		// Draw the pin-numbers on the bottom-right side								
		if (drawOptions.drawPinNumbers && coordsAtPin != null)
		{
			fontSize = cellDim/2;
			font = new Font("Arial", Font.BOLD, fontSize);
			g2d.setFont(font);
			g2d.setColor(Color.GRAY);
    		
			for (Integer pin : coordsAtPin.keySet())
			{
				ArrayList<String> coords = coordsAtPin.get(pin);
	    		if (coords != null && pin > 0)
	    		{
		    		for (int i = 0; i < coords.size(); i++)
		    		{
						String coord = coords.get(i).substring(coords.get(i).indexOf("("), coords.get(i).indexOf(")")+1);
						int x = Integer.parseInt(coord.substring(coord.indexOf("("), coord.indexOf(",")).replaceAll("[^\\d]", ""));
						int y = Integer.parseInt(coord.substring(coord.indexOf(","), coord.indexOf(")")).replaceAll("[^\\d]", ""));
						g2d.drawString(String.valueOf(pin), ioLong + (x+1)*cellDim - fontSize*2, ioLong + (y+1)*cellDim - fontSize/2);
		    		}
	    		}
			}
		}

		//File f = new File("DMFB" + "." + ext);
		//ImageIO.write(imgDmfb, ext, f);
		return imgDmfb;
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	// Given the Graphics2D object of a DMFB image, draws the reconfigurable modules
	// on top at the given time-step
	//////////////////////////////////////////////////////////////////////////////////////
	public static void DrawReconfigAreasByTS(ArrayList<ReconfigArea> reconfigAreas, DrawOptions drawOptions, Graphics2D g2d2, int beginningTS, boolean isRoutingStage)
	{
    	for (int j = 0; j < reconfigAreas.size(); j++)
    	{
    		ReconfigArea ra = reconfigAreas.get(j);
    		if (ra.start_TS > beginningTS)
    			break;
    		if (beginningTS >= ra.start_TS && beginningTS < ra.stop_TS || 
    				(beginningTS >= ra.start_TS && beginningTS == ra.stop_TS && isRoutingStage))
    		{
    			// Default colors
    			Color raColor = Color.LIGHT_GRAY;
    			Color irColor = new Color(0.75f, 0.312f, 0.300f, 0.5f); // Red (default IR color)
		    	Font font = new Font("Arial", Font.BOLD, cellDim/2);
		    	g2d2.setFont(font);	
		    	Color fontColor = Color.BLACK;
		    	
    			if (beginningTS == ra.stop_TS && isRoutingStage) 
    			{	// Recently ended module, dim so we can see where droplets are coming from in routing stage
    				raColor = new Color(0.8f, 0.8f, 0.8f, 0.2f); // Light gray
    				irColor = new Color(0.2f, 0.2f, 0.2f, 0.2f); // Light gray
    				fontColor = Color.GRAY;
    			}
    			else if (ra.opType.equals(OperationType.MIX))
    				raColor = new Color(0.554f, 0.703f, 0.886f, 0.5f); // Blue
    			else if (ra.opType.equals(OperationType.SPLIT))
    				raColor = new Color(0.714f, 0.867f, 0.906f, 0.5f); // Light Blue
    			else if (ra.opType.equals(OperationType.HEAT))
    				raColor = new Color(0.964f, 0.585f, 0.273f, 0.5f); // Orange
    			else if (ra.opType.equals(OperationType.DETECT))
    				raColor = new Color(0.765f, 0.738f, 0.589f, 0.5f); // Tan
    			else if (ra.opType.equals(OperationType.STORAGE))
    				raColor = new Color(1.0f, 1.0f, 0.717f, 0.5f); // Light Yellow
    			
    			// DRAW Interference Regions and Reconfigurable Areas	
    			for (int x = ra.tl_x-1; x <= ra.br_x+1; x++)
    			{
    				for (int y = ra.tl_y-1; y <= ra.br_y+1; y++)
    				{
    					if (x >= ra.tl_x && x <= ra.br_x && y >= ra.tl_y && y <= ra.br_y)
    					{
    						g2d2.setColor(raColor);
    						g2d2.fillRect(ioLong + x * cellDim, ioLong + y * cellDim, cellDim, cellDim);
    						
    						if (drawOptions.drawModuleNames)
    	    	       		{
    							g2d2.setColor(fontColor);
    							g2d2.drawString(ra.name, x * cellDim + cellDim/3 + offset, y * cellDim + 2*cellDim/3 + offset);
    	    	       		}
    					}
    					else if (drawOptions.drawModIr)
    					{
    						g2d2.setColor(irColor);
    						g2d2.fillRect(ioLong + x * cellDim, ioLong + y * cellDim, cellDim, cellDim);
    	    	       		g2d2.setColor(fontColor);
    	    	    		g2d2.drawString("IR", x * cellDim + cellDim/3 + offset, y * cellDim + 2*cellDim/3 + offset);
    					}	    					   		    				
    				}
    			}	    			
    		}
    	}
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	// Given the Graphics2D object of a DMFB image, draws the reconfigurable modules
	// on top at the given cycle
	//////////////////////////////////////////////////////////////////////////////////////
	public static void DrawReconfigAreasByCycle(ArrayList<ReconfigArea> reconfigAreas, DrawOptions drawOptions, Graphics2D g2d2, int cn)
	{
    	// Draw reconfigurable modules and interference regions
    	for (int j = 0; j < reconfigAreas.size(); j++)
    	{
    		ReconfigArea ra = reconfigAreas.get(j);
    		if (ra.start_cycle > cn)
    			break;
    		if (cn >= ra.start_cycle && cn < ra.stop_cycle)
    		{
    			Color raColor = Color.LIGHT_GRAY;
    			if (ra.opType.equals(OperationType.MIX))
    				raColor = new Color(0.554f, 0.703f, 0.886f, 0.5f); // Blue
    			else if (ra.opType.equals(OperationType.SPLIT))
    				raColor = new Color(0.714f, 0.867f, 0.906f, 0.5f); // Light Blue
    			else if (ra.opType.equals(OperationType.HEAT))
    				raColor = new Color(0.964f, 0.585f, 0.273f, 0.5f); // Orange
    			else if (ra.opType.equals(OperationType.DETECT))
    				raColor = new Color(0.765f, 0.738f, 0.589f, 0.5f); // Tan
    			else if (ra.opType.equals(OperationType.STORAGE))
    				raColor = new Color(1.0f, 1.0f, 0.717f, 0.5f); // Light Yellow
    			
    			// DRAW Interference Regions and Reconfigurable Areas
    			Color irColor = new Color(0.75f, 0.312f, 0.300f, 0.5f); // Red
		    	Font font = new Font("Arial", Font.BOLD, cellDim/2);
		    	g2d2.setFont(font);	
		    	
    			for (int x = ra.tl_x-1; x <= ra.br_x+1; x++)
    			{
    				for (int y = ra.tl_y-1; y <= ra.br_y+1; y++)
    				{
    					if (x >= ra.tl_x && x <= ra.br_x && y >= ra.tl_y && y <= ra.br_y)
    					{
    						g2d2.setColor(raColor);
    						g2d2.fillRect(ioLong + x * cellDim, ioLong + y * cellDim, cellDim, cellDim);
    						
    						if (drawOptions.drawModuleNames)
    						{
	    	    	       		g2d2.setColor(Color.BLACK);
	    	    	    		g2d2.drawString(ra.name, x * cellDim + cellDim/3 + offset, y * cellDim + 2*cellDim/3 + offset);
    						}
    					}
    					else if (drawOptions.drawModIr)
    					{
    						g2d2.setColor(irColor);
    						g2d2.fillRect(ioLong + x * cellDim, ioLong + y * cellDim, cellDim, cellDim);
    	    	       		g2d2.setColor(Color.BLACK);
    	    	    		g2d2.drawString("IR", x * cellDim + cellDim/3 + offset, y * cellDim + 2*cellDim/3 + offset);
    					}	    					   		    				
    				}
    			}	    			
    		}
    	}
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	// Draws all cycles, including droplet locations and reconfigurable modules at each
	// cycle; Can output all cycles (drawAllCycles = true), or just routing cycles
	// (drawAllCycles = false). Also dictates the quality of the movie (made up of all
	// the output cycles).
	//////////////////////////////////////////////////////////////////////////////////////
	public static void DrawCycles(RouteParser p, boolean drawAllCycles, DrawOptions drawOptions, Main main) throws IOException
	{		
		// Obtain all relevant data from the parser
		ArrayList<ArrayList<Droplet>> dropletHistory = p.getDropletHistory();
		ArrayList<ArrayList<Integer>> pinHistory = p.getPinHistory();
		Map<Integer, ArrayList<String>> coordsAtPin = p.getCoordsAtPin();
		ArrayList<ReconfigArea> reconfigAreas = p.getReconfigAreas();
		ArrayList<Integer> RouteCycleRange = p.getRouteCycleRange();
		ArrayList<Integer> TsCycleRange = p.getTsCycleRange();
		ArrayList<Integer> cycleNums = p.getCycleNums();		
		DmfbArch da = new DmfbArch();	
		da.ExternalResources = p.getExternalResources();
		da.ResourceLocations = p.getResourceLocations();
		da.numXcells = p.getNumXcells();
		da.numYcells = p.getNumYcells();
		da.IoPorts = p.getIoPorts();
	
		
		// Set the unit dimensions
		SetDmfbUnitDims(da, drawOptions.maxWidth, drawOptions.maxHeight);
		
		// Sort the reconfigurable areas by starting/stoping time (increasing)
		Collections.sort(reconfigAreas, new Comparator<ReconfigArea>() {
		    public int compare (ReconfigArea ra1, ReconfigArea ra2) {
				if (ra1.start_TS == ra2.start_TS)
					return (int)(ra1.stop_TS - ra2.stop_TS);
				else
					return (int)(ra1.start_TS - ra2.start_TS);
			}
	    });
		
		
		// First, output basic statistics for routing
		int cyclesRouting = 0;
		int subProblems = 0;
		for (int i = 0; i < RouteCycleRange.size(); i=i+2)
		{
			cyclesRouting += (RouteCycleRange.get(i+1) - RouteCycleRange.get(i));
			if (!RouteCycleRange.get(i).equals(RouteCycleRange.get(i+1)))
				subProblems++;
		}
		File outputFile = new File(outDir + "/routedStats.txt");
	    FileWriter out = new FileWriter(outputFile);
	    out.write("Cycles used for ");
	    out.write(Integer.toString(subProblems));
	    out.write(" routing sub-problems: ");
	    out.write(Integer.toString(cyclesRouting));
	    out.close();
			
		
        int numDroplets = 0;   
        
		// Open all necessary files
		FileWriter fstream = new FileWriter("order.txt");
        BufferedWriter order = new BufferedWriter(fstream);		

		// Draw a new image for each cycle
		int timeStep = -1;
		boolean routing = true;
		
		int lowerBound;
		int upperBound;
		
		while (true)
		{

    		lowerBound = RouteCycleRange.get(0);
    		RouteCycleRange.remove(0);
    		upperBound = RouteCycleRange.get(0);
    		RouteCycleRange.remove(0);
    		timeStep++;    		
    		routing = true;
    		if (lowerBound <= cycleNums.get(0) && upperBound > cycleNums.get(0))
    			break;    		
    		
    		lowerBound = TsCycleRange.get(0);
    		TsCycleRange.remove(0);
    		upperBound = TsCycleRange.get(0);
    		TsCycleRange.remove(0);
    		routing = false;
    		if (lowerBound <= cycleNums.get(0) && upperBound > cycleNums.get(0))
    			break;			
		}

				
	    for (int i = 0; i < cycleNums.size(); i++)
	    {  	
			if (!main.isOutputting())
				return;
			
	    	int cn = cycleNums.get(i);
	    	
	    	// Determine if this is a routing cycle
	    	if (cn == upperBound)
	    	{
				while (true)
				{
					if (!routing)
					{
			    		lowerBound = RouteCycleRange.get(0);
			    		RouteCycleRange.remove(0);
			    		upperBound = RouteCycleRange.get(0);
			    		RouteCycleRange.remove(0);
			    		timeStep++;    		
			    		routing = true;
			    		if (lowerBound <= cn && upperBound > cn)
			    			break;
			    		else if (TsCycleRange.isEmpty())
			    			break;
					}
					else
					{
			    		lowerBound = TsCycleRange.get(0);
			    		TsCycleRange.remove(0);
			    		upperBound = TsCycleRange.get(0);
			    		TsCycleRange.remove(0);
			    		routing = false;
			    		if (lowerBound <= cn && upperBound > cn)
			    			break;
			    		else if (RouteCycleRange.isEmpty())
			    			break;
					}
				}
	    	}
	    	

	    	if (drawAllCycles || routing)
	    	{
			    // Create new base LoC image to draw droplets on
	    		BufferedImage cycleLoc = DrawBlankDmfb(da, drawOptions, coordsAtPin, null);
		    	Graphics2D g2d2 = (Graphics2D) cycleLoc.getGraphics();
		    	
		    	// Draw the time-step
		    	Font font = new Font("Arial", Font.BOLD, cellDim);
		    	g2d2.setFont(font);
		    	if (routing)
		    	{
					String beg;
					if (timeStep == 0)
						beg = "00";
					else
						beg = String.valueOf(timeStep-1);
					
			    	g2d2.setColor(new Color(0.0f, 0.0f, 0.0f, 0.5f));
			    	g2d2.drawString("Route", cellDim/5, cellDim);
			    	g2d2.drawString("to TS " + String.valueOf(timeStep), cellDim/5, cellDim*2);
		    	}
		    	else
		    	{
	
			    	g2d2.setColor(new Color(0.0f, 0.0f, 0.0f, 0.5f));
			    	g2d2.drawString("TS " + String.valueOf(timeStep), cellDim/5, cellDim);
	
		    	}
	
		    	// Draw reconfigurable modules and interference regions
		    	DrawReconfigAreasByTS(reconfigAreas, drawOptions, g2d2, timeStep, routing);
		    	for (int j = reconfigAreas.size()-1; j >= 0; j--)
		    		if (reconfigAreas.get(j).stop_TS < timeStep && !routing)
		    			reconfigAreas.remove(j);	
		    	//DrawReconfigAreasByCycle(reconfigAreas, drawOptions, g2d2, cn);		    	
		    	//for (int j = reconfigAreas.size()-1; j >= 0; j--)
		    	//	if (reconfigAreas.get(j).stop_cycle < cycleNums.get(i))
		    	//		reconfigAreas.remove(j);
		    	
		    	
		    	// Draw Active Pins
		    	if (drawOptions.drawPinActivations)
		    	{
			    	ArrayList<Integer> pins = pinHistory.get(i);
			    	for (int j = 0; j < pins.size(); j++)
			    	{
			    		ArrayList<String> coords = coordsAtPin.get(pins.get(j));
			    		if (coords != null)
			    		{
				    		for (int k = 0; k < coords.size(); k++)
				    		{
								String coord = coords.get(k).substring(coords.get(k).indexOf("("), coords.get(k).indexOf(")")+1);
								int x = Integer.parseInt(coord.substring(coord.indexOf("("), coord.indexOf(",")).replaceAll("[^\\d]", ""));
								int y = Integer.parseInt(coord.substring(coord.indexOf(","), coord.indexOf(")")).replaceAll("[^\\d]", ""));
					    		g2d2.setColor(new Color(0.0f, 0.5f, 0.5f, 0.5f)); // Blue
								g2d2.fillRect(ioLong + x * cellDim, ioLong + y * cellDim, cellDim, cellDim);
								
								// Draw the pin-numbers on the bottom-right side								
								if (drawOptions.drawPinNumbers && coordsAtPin != null)
								{
									int fontSize = cellDim/2;
									font = new Font("Arial", Font.BOLD, fontSize);
									g2d2.setFont(font);
									g2d2.setColor(Color.GRAY);
									g2d2.drawString(String.valueOf(pins.get(j)), ioLong + (x+1)*cellDim - fontSize, ioLong + (y+1)*cellDim - fontSize/2);
								}
				    		}
			    		}
			    	}
		    	}
		    	
		    	// Draw droplets
		    	if (drawOptions.drawDroplets)
		    	{
			    	ArrayList<Droplet> droplets = dropletHistory.get(i);
			    	font = new Font("Arial", Font.BOLD, cellDim/2);
			    	g2d2.setFont(font);	
			    	for (int j = 0; j < droplets.size(); j++)
			    	{
			    		Droplet vD = droplets.get(j);
			    		int x = vD.xCell;
			    		int y = vD.yCell;
			    		
			    		// Draw Interference Region for Droplet
			    		if (drawOptions.drawDropIr)
			    		{
				    		Color color = new Color(0.75f, 0.312f, 0.300f, 0.5f); // Red 
				    		g2d2.setColor(color);
				    		for (int irX = x-1; irX <= x+1; irX++)
				    		{
				    			for (int irY = y-1; irY <= y+1; irY++)
				    			{
				    				if (irX >= 0 && irY >= 0 && irX < numCellsX && irY < numCellsY && !(vD.Status.contains("PROCESSING") || vD.Status.contains("PROCESS WAIT")))
				    					g2d2.fillRect(ioLong + irX * cellDim, ioLong + irY * cellDim, cellDim, cellDim);
				    			}
				    		}
			    		}
			    				    			    		
			    		if (vD.Global_ID > numDroplets)
			    			numDroplets = vD.Global_ID;
			    		
			    		if (vD.Color == vD.Color.RED)
			    			g2d2.setColor(Color.RED);
			    		else if (vD.Color == vD.Color.GREEN)
			    			g2d2.setColor(Color.GREEN);
			    		else if (vD.Color == vD.Color.YELLOW)
			    			g2d2.setColor(Color.YELLOW);
			    		else if (vD.Color == vD.Color.WHITE)
			    			g2d2.setColor(Color.WHITE);
			    		else if (vD.Color == vD.Color.ORANGE)
			    			g2d2.setColor(Color.ORANGE);
			    		else if (vD.Color == vD.Color.CYAN)
			    			g2d2.setColor(Color.CYAN);
			    		else if (vD.Color == vD.Color.WHITE)
			    			g2d2.setColor(Color.WHITE);
			    		else if (vD.Color == vD.Color.MAGENTA)
			    			g2d2.setColor(Color.MAGENTA);
			    		else 
			    			g2d2.setColor(Color.BLACK);
			    			    		
			    		// Draw Droplet
			    		g2d2.fillOval(x * cellDim + offset, y * cellDim + offset, cellDim, cellDim);
			       		g2d2.setColor(Color.BLACK);
			    		g2d2.drawString(Integer.toString(vD.Global_ID), x * cellDim + cellDim/3 + offset, y * cellDim + cellDim/2 + offset);	
			    	}
		    	}
		    	
		    	// Output image file and add new line to order file (so the movie encoder can order the image properly
		    	File f = new File(outDir + "/" + cycleNums.get(i) + "." + ext);
				ImageIO.write(cycleLoc, ext, f);
				order.write(outDir + "/" + cycleNums.get(i) + "." + ext);
				order.newLine();
	    	}
	    	// Update progress bar
	    	main.updateProgress((int)((double)i/(double)cycleNums.size()*100), "Cycle " + i + "/" + cycleNums.size());
	    }
	    main.updateProgress(100, "Cycle " + cycleNums.size() + "/" + cycleNums.size());
	    order.close();
	    
	    // Create movie - Something needs to be fixed here b/c the "proc" never seems to end. Calling proc.waitFor() or proc.wait()
	    // never returns. Thus, the movie file will not be finalized and available to the user until the program is shut...also, 
	    // clicking on random controls in the GUI seems to somehow make the thread end. This could be a problem with mencoder.
	    if (drawOptions.movQuality == "Low Quality")
	    {		    	    
		    String fileName = outDir + "/MFSim";
		    fileName += numCellsX + "x" + numCellsY + "_" + numDroplets + "Drops";
	    	fileName += "_LQ.avi";
	    	Runtime r = Runtime.getRuntime();	
	    	Process proc = r.exec("../Shared/mencoder \"mf://@order.txt\" -mf type=png:fps=" + drawOptions.frequency + " -ovc lavc -o " + fileName);	    	
	    }
	    else if (drawOptions.movQuality == "High Quality")
	    {
		    String fileName = outDir + "/MFSim";
		    fileName += numCellsX + "x" + numCellsY + "_" + numDroplets + "Drops";
	    	fileName += "_HQ.avi";
		    Runtime r = Runtime.getRuntime();	
		    Process proc = r.exec("../Shared/mencoder -mc 0 -noskip -skiplimit 0 -ovc lavc -lavcopts vcodec=mpeg4:vhq:trell:mbd=2:vmax_b_frames=1:v4mv:vb_strategy=0:vlelim=0:vcelim=0:cmp=6:subcmp=6:precmp=6:predia=3:dia=3:vme=4:vqscale=1 \"mf://@order.txt\" -mf type=png:fps=" + drawOptions.frequency + " -o " + fileName);
	    }
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	// Draws all compact routes.
	//////////////////////////////////////////////////////////////////////////////////////
	public static void DrawAllRoutes(CompactRouteParser cp, DrawOptions drawOptions, Main main) throws IOException
	{
		ArrayList<Map<Integer, ArrayList<Integer>>> routesAtTS = cp.getRoutesAtTS();
		Map<Integer, ArrayList<String>> nodesAtMod = cp.getNodesAtMod();
		Map<Integer, ArrayList<String>> nodesAtIo = cp.getNodesAtIo();		
		ArrayList<ReconfigArea> reconfigAreas = cp.getReconfigAreas();
		
		DmfbArch da = new DmfbArch();	
		da.ExternalResources = cp.getExternalResources();
		da.ResourceLocations = cp.getResourceLocations();
		da.numXcells = cp.getNumXcells();
		da.numYcells = cp.getNumYcells();
		da.IoPorts = cp.getIoPorts();

		// Set the unit dimensions
		SetDmfbUnitDims(da, drawOptions.maxWidth, drawOptions.maxHeight);
		
		// Sort the reconfigurable areas by starting/stoping time (increasing)
		Collections.sort(reconfigAreas, new Comparator<ReconfigArea>() {
		    public int compare (ReconfigArea ra1, ReconfigArea ra2) {
				if (ra1.start_TS == ra2.start_TS)
					return (int)(ra1.stop_TS - ra2.stop_TS);
				else
					return (int)(ra1.start_TS - ra2.start_TS);
			}
	    });
		
        int numDroplets = 0;        
	
		// Draw a new image for each routing phase
		for (int i = 0; i < routesAtTS.size(); i++)
		{
			if (!main.isOutputting())
				return;
			
			Map<Integer, ArrayList<Integer>> routes = routesAtTS.get(i);
			
			if (routes.size() > 0)
			{
				int beginningTS = i;
				String beg;
				if (beginningTS == 0)
					beg = "00";
				else
					beg = String.valueOf(beginningTS-1);
				
			    // Create new base LoC image to draw routes on
		    	BufferedImage tsLoc = DrawBlankDmfb(da, drawOptions, null, null);
		    	Graphics2D g2d2 = (Graphics2D) tsLoc.getGraphics();		    	
    	
		    	// Draw the time-step
		    	Font font = new Font("Arial", Font.BOLD, cellDim); // DTG resize auto
		    	g2d2.setFont(font);					
		    	g2d2.setColor(new Color(0.0f, 0.0f, 0.0f, 0.5f));
		    	g2d2.drawString("Route", cellDim/5, cellDim);
		    	g2d2.drawString("to TS " + String.valueOf(beginningTS), cellDim/5, cellDim*2);   	
		    	
		    	// Draw reconfigurable modules and interference regions
		    	DrawReconfigAreasByTS(reconfigAreas, drawOptions, g2d2, beginningTS, true);
		    	for (int j = reconfigAreas.size()-1; j >= 0; j--)
		    		if (reconfigAreas.get(j).stop_TS < beginningTS)
		    			reconfigAreas.remove(j);		    	
		    	
		    	// Create a new LoC image which will show all the routes for the routing phase
		    	BufferedImage routesLoc = new BufferedImage(locWidth, locHeight, BufferedImage.TYPE_INT_RGB);
		    	Graphics2D g2dRoutes = (Graphics2D) routesLoc.getGraphics();
		    	g2dRoutes.drawImage(tsLoc, null, 0, 0);	    	
			
				Iterator<Integer> it = routes.keySet().iterator();
				int numDrops = 0;
			    	
		    	///////////////////////////////////////////////////////////////////////////////////
		    	// Each iteration through this loop draws a new route
		    	///////////////////////////////////////////////////////////////////////////////////
				while (it.hasNext())
				{
			    	// Create a new LoC image for this specific route
			    	BufferedImage routeLoc = new BufferedImage(locWidth, locHeight, BufferedImage.TYPE_INT_RGB);
			    	Graphics2D g2dRoute = (Graphics2D) routeLoc.getGraphics();
			    	g2dRoute.drawImage(tsLoc, null, 0, 0);	
			    	
					int dropId = (Integer) it.next();
					ArrayList<Integer> route = routes.get(dropId);
					
					// Select a color for the route
					//Color[] colors = {Color.CYAN, Color.GREEN, Color.ORANGE, Color.PINK, Color.YELLOW, Color.RED, Color.LIGHT_GRAY, Color.MAGENTA};
					ArrayList<Color> colors = new ArrayList<Color>();
					colors.add(new Color(0.0f, 1.0f, 1.0f, .5f));// Cyan
					colors.add(new Color(1.0f, 0.6f, 0.2f, .5f));// Orange
					colors.add(new Color(1.0f, 1.0f, 0.0f, .5f));// Yellow
					colors.add(new Color(1.0f, 0.2f, 0.6f, .5f));// Pink
					
					int routePart = 0;
					while (route.size() > 0)
					{				
						int x = route.get(0);
						route.remove(0);
			    		int y = route.get(0);
			    		route.remove(0);
			    		
						g2dRoutes.setColor(colors.get(numDrops % colors.size()));
						g2dRoute.setColor(colors.get(numDrops % colors.size()));
			    		
			    		g2dRoutes.fillRect(x * cellDim + ioLong, y * cellDim+1 + ioLong, cellDim, cellDim);
			    		g2dRoute.fillRect(x * cellDim + ioLong, y * cellDim+1 + ioLong, cellDim, cellDim);
			    			    		
			    		// Draw Droplet
			    		//g2d2.fillOval(x * cellWidth + offset, y * cellHeight + offset, cellWidth, cellHeight);
			       		g2dRoutes.setColor(Color.BLACK);
			       		g2dRoute.setColor(Color.BLACK);
			       		
			    		font = new Font("Arial", Font.BOLD, cellDim/2);
			    		g2dRoutes.setFont(font);
			    		g2dRoute.setFont(font);
			    		g2dRoutes.drawString(Integer.toString(dropId) + "_" + Integer.toString(routePart), x * cellDim + (Integer)cellDim/10  + ioLong, y * cellDim + (Integer)cellDim/2 + ioLong);
			    		g2dRoute.drawString(Integer.toString(dropId) + "_" + Integer.toString(routePart), x * cellDim + (Integer)cellDim/10 + ioLong, y * cellDim + (Integer)cellDim/2 + ioLong);
			    		routePart++;
					}	
					numDrops++;
					
					// Output image file
			    	File f = new File(outDir + "/" + beg + "_" + beginningTS + "_Drop" + dropId + "." + ext);
					ImageIO.write(routeLoc, ext, f);
				}
				
		    	File f = new File(outDir + "/" + beg + "_" + beginningTS + "_DropAll." + ext);
				ImageIO.write(routesLoc, ext, f);
			}
			// Update progress bar
			main.updateProgress((int)((double)i/(double)routesAtTS.size()*100), "Route " + i + "/" + routesAtTS.size());
		}
		main.updateProgress(100, "Route " + routesAtTS.size() + "/" + routesAtTS.size());
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Draws the time-steps given either a placed parser or compact parser (both contain the necessary
	// information for drawing the time-steps)
	/////////////////////////////////////////////////////////////////////////////////////////////////
	public static void DrawAllTimeSteps(PlacedParser pp, CompactRouteParser cp, DrawOptions drawOptions, Main main) throws IOException
	{
		Map<Integer, ArrayList<String>> nodesAtMod;
		ArrayList<AssayNode> ioNodes;
		ArrayList<ReconfigArea> reconfigAreas;		
		DmfbArch da = new DmfbArch();	
		
		// Extract data from one of the parsers
		if (pp != null)
		{
			nodesAtMod = pp.getNodesAtMod();
			ioNodes = pp.getIoNodes();
			reconfigAreas = pp.getReconfigAreas();
				
			da.ExternalResources = pp.getExternalResources();
			da.ResourceLocations = pp.getResourceLocations();
			da.numXcells = pp.getNumXcells();
			da.numYcells = pp.getNumYcells();
			da.IoPorts = pp.getIoPorts();
		}
		else if (cp != null)
		{
			nodesAtMod = cp.getNodesAtMod();
			ioNodes = cp.getIoNodes();
			reconfigAreas = cp.getReconfigAreas();
				
			da.ExternalResources = cp.getExternalResources();
			da.ResourceLocations = cp.getResourceLocations();
			da.numXcells = cp.getNumXcells();
			da.numYcells = cp.getNumYcells();
			da.IoPorts = cp.getIoPorts();
		}
		else
		{
			MFError.DisplayError("You must pass a valid parser to the DrawAllTimeSteps() function.");
			return;
		}
		
		// Set the unit dimensions
		SetDmfbUnitDims(da, drawOptions.maxWidth, drawOptions.maxHeight);
		
		// Sort the reconfigurable areas by starting/stoping time (increasing)
		Collections.sort(reconfigAreas, new Comparator<ReconfigArea>() {
		    public int compare (ReconfigArea ra1, ReconfigArea ra2) {
				if (ra1.start_TS == ra2.start_TS)
					return (int)(ra1.stop_TS - ra2.stop_TS);
				else
					return (int)(ra1.start_TS - ra2.start_TS);
			}
	    });
		
		// First, output basic statistics for area usage
		int cellsUsed = 0;
		for (int i = 0; i < reconfigAreas.size(); i++)
		{
			ReconfigArea ra = reconfigAreas.get(i);
			cellsUsed += (ra.br_x - ra.tl_x + 3) * (ra.br_y - ra.tl_y + 3) * (ra.stop_TS-ra.start_TS);
		}
	    int mTS = (int)(reconfigAreas.get(reconfigAreas.size()-1).stop_TS);
	    int totalCells = mTS * numCellsX*numCellsY;
		File outputFile = new File(outDir + "/placedStats.txt");
	    FileWriter out = new FileWriter(outputFile);
	    out.write("Cells used (including IR) / total cells (cumulative over ");
	    out.write(Integer.toString(mTS));
	    out.write(" timesteps: ");
	    out.write(Integer.toString(cellsUsed));
	    out.write("/");
	    out.write(Integer.toString(totalCells));
	    out.write(" = ");
	    out.write(Double.toString((double)cellsUsed/(double)totalCells));
	    out.close();

		// Get the number of Time-steps
		int maxTS = -1;
		for (int i = 0; i < reconfigAreas.size(); i++)
			if (reconfigAreas.get(i).stop_TS > maxTS)
				maxTS = (int)reconfigAreas.get(i).stop_TS;		
		for (int i = 0; i < ioNodes.size(); i++)
			if (ioNodes.get(i).stopTS > maxTS)
				maxTS = (int)ioNodes.get(i).stopTS;		
		
		// Draw a new image for each routing phase
		for (int i = 0; i < maxTS; i++)
		{
			if (!main.isOutputting())
				return;
			
			int beginningTS = i;
			
		    // Create new base LoC image to draw modules on
	    	BufferedImage tsLoc = DrawBlankDmfb(da, drawOptions, null, null);
	    	Graphics2D g2d2 = (Graphics2D) tsLoc.getGraphics();
	    	
	    	// Draw time-step label
	    	Font font = new Font("Arial", Font.BOLD, cellDim);
	    	g2d2.setFont(font);
	    	g2d2.setColor(new Color(0.0f, 0.0f, 0.0f, 0.5f));
	    	g2d2.drawString("TS " + String.valueOf(beginningTS), cellDim/5, cellDim);	

	    	
	    	// Draw reconfigurable modules and interference regions and reconfigurable nodes
	    	DrawReconfigAreasByTS(reconfigAreas, drawOptions, g2d2, beginningTS, false);
	    	font = new Font("Arial", Font.BOLD, cellDim/2);
	    	g2d2.setFont(font);
	    	
	    	for (int j = 0; j < reconfigAreas.size(); j++)
	    	{
	    		ReconfigArea ra = reconfigAreas.get(j);
	    		if (ra.start_TS > beginningTS)
	    			break;
	    		if (beginningTS >= ra.start_TS && beginningTS < ra.stop_TS)
	    		{
	    			// Now draw the overlay node which shows which nodes are bound to the modules
	    			int nodeWidth = cellDim*(ra.br_x-ra.tl_x+1+2);
	    			int nodeHeight = cellDim*2;
	    			int nodeX = ra.tl_x-1;
	    			int nodeY = ra.tl_y-1;
	    			g2d2.setColor(new Color(0.0f, 0.0f, 0.0f, 0.7f));
	    			g2d2.fillOval(nodeX*cellDim + offset, nodeY*cellDim + offset, nodeWidth, nodeHeight);
	    			g2d2.setColor(Color.BLACK);
	    			g2d2.drawOval(nodeX*cellDim + offset, nodeY*cellDim + offset, nodeWidth, nodeHeight);
	    			
	    			ArrayList<String> nodes = nodesAtMod.get(ra.id);
	    			for (int k = 0; k < nodes.size(); k++)
	    			{
	    				g2d2.setColor(Color.RED);
	    				g2d2.drawString(nodes.get(k), nodeX*cellDim + offset + (int)nodeHeight/8, nodeY*cellDim + offset + (int)nodeHeight/3 + 20*k);
	    			}
	    		}	    		
	    	}
	    	for (int j = reconfigAreas.size()-1; j >= 0; j--)
	    		if (reconfigAreas.get(j).stop_TS < beginningTS)
	    			reconfigAreas.remove(j);	
	    	
	    	// Now draw any IO nodes
	    	for (int j = 0; j < ioNodes.size(); j++)
	    	{
	    		AssayNode n = ioNodes.get(j);
	    		if (beginningTS >= n.startTS && beginningTS < n.stopTS)
	    		{
	    			for (int k = 0; k < da.IoPorts.size(); k++)
	    			{
	    				IoPort p = da.IoPorts.get(k);
	    				if (p.id == n.boundResourceId)
	    				{
	    					int nodeX = 0;
	    					int nodeY = 0;
	    					int nodeWidth = ioShort;
	    					int nodeHeight = ioShort;
	    					
	    					if (p.side.equals("TOP") || p.side.equals("NORTH"))
	    					{
	    						nodeX = ioLong + p.pos_xy*cellDim - (Integer)((ioShort-cellDim)/2);
	    						nodeY = 0;
	    					}
	    					else if (p.side.equals("BOTTOM") || p.side.equals("SOUTH"))
	    					{
	    						nodeX = ioLong + p.pos_xy*cellDim - (Integer)((ioShort-cellDim)/2);
	    						nodeY = locHeight-ioShort;
	    					}
	    					else if (p.side.equals("LEFT") || p.side.equals("WEST"))
	    					{
	    						nodeX = 0;
	    						nodeY = ioLong + p.pos_xy*cellDim - (Integer)((ioShort-cellDim)/2);
	    					}
	    					else if (p.side.equals("RIGHT") || p.side.equals("EAST"))
	    					{
	    						nodeX = locWidth-ioShort;
	    						nodeY = ioLong + p.pos_xy*cellDim - (Integer)((ioShort-cellDim)/2);
	    					}
	    					else
	    						MFError.DisplayError("Invalid I/O port side.");
	    					
	    					String label = "";
	    					if (n.type == OperationType.INPUT)
	    						label = "IN_" + n.id;
	    					else
	    						label = "OUT_" + n.id;
	    					
	    					if (!n.name.isEmpty())
	    						label += ("(" + n.name + ")");
	    					
	    	    			g2d2.setColor(new Color(0.0f, 0.0f, 0.0f, 0.7f));
	    	    			g2d2.fillOval(nodeX, nodeY, nodeWidth, nodeHeight);
	    	    			g2d2.setColor(Color.BLACK);
	    	    			g2d2.drawOval(nodeX, nodeY, nodeWidth, nodeHeight);
		    				g2d2.setColor(Color.RED);
		    				g2d2.drawString(label, nodeX + (int)nodeHeight/8, nodeY + (int)nodeHeight/3);	    	    			
	    					break;
	    				}
	    			}
	    		}
	    	}
	    	for (int j = ioNodes.size()-1; j >= 0; j--)
	    		if (ioNodes.get(j).stopTS < beginningTS)
	    			ioNodes.remove(j);
	    	
	    	// Output image
	    	File f = new File(outDir + "/" + beginningTS + "." + ext);
			ImageIO.write(tsLoc, ext, f);
			
			// Update progress bar
			main.updateProgress((int)((double)i/(double)maxTS*100), "TimeStep " + i + "/" + maxTS);
		}
		main.updateProgress(100, "TimeStep " + maxTS + "/" + maxTS);
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Draws the hardware given a hardware description file.
	/////////////////////////////////////////////////////////////////////////////////////////////////
	public static void DrawHardware(HardwareParser hp, DrawOptions drawOptions, Main main) throws IOException
	{
		//Map<Integer, ArrayList<String>> nodesAtMod;
		//ArrayList<AssayNode> ioNodes;
		
		
		// Extract data from the parser
		Map<Integer, ArrayList<String>> coordsAtPin = hp.getCoordsAtPin();
		Map<Integer, ArrayList<WireSegment>> wireSegsToPin = hp.getWireSegsToPin();
		DmfbArch da = new DmfbArch();			
		da.ExternalResources = hp.getExternalResources();
		da.ResourceLocations = hp.getResourceLocations();
		da.numXcells = hp.getNumXcells();
		da.numYcells = hp.getNumYcells();
		da.numHTracks = hp.getNumHTracks();
		da.numVTracks = hp.getNumVTracks();
		
		da.IoPorts = hp.getIoPorts();

		// Set the unit dimensions
		SetDmfbUnitDims(da, drawOptions.maxWidth, drawOptions.maxHeight);
		
		BufferedImage biHw = DrawBlankDmfb(da, drawOptions, coordsAtPin, wireSegsToPin);
    	File f = new File(outDir + "/" + "HW" + "." + ext);
		ImageIO.write(biHw, ext, f);
		
		// Draw an image for each pin
		if (coordsAtPin != null)
		{
			// For each pin
			int pinCount = 0;
			for (Integer pin : coordsAtPin.keySet())
			{
				if (!main.isOutputting())
					return;
				
				BufferedImage biPin = DrawBlankDmfb(da, drawOptions, null, null);
				Graphics2D g2d = (Graphics2D)biPin.getGraphics();
				
				ArrayList<String> coords = coordsAtPin.get(pin);
				ArrayList<WireSegment> wire = wireSegsToPin.get(pin);
				
				// First, draw the pin-numbers on the appropriate electrodes
	    		if (coords != null && pin > 0)
	    		{
		    		for (int i = 0; i < coords.size(); i++)
		    		{
		    			int fontSize = cellDim/2;
		    			Font font = new Font("Arial", Font.BOLD, fontSize);
		    			g2d.setFont(font);
		    			g2d.setColor(Color.GRAY);
						String coord = coords.get(i).substring(coords.get(i).indexOf("("), coords.get(i).indexOf(")")+1);
						int x = Integer.parseInt(coord.substring(coord.indexOf("("), coord.indexOf(",")).replaceAll("[^\\d]", ""));
						int y = Integer.parseInt(coord.substring(coord.indexOf(","), coord.indexOf(")")).replaceAll("[^\\d]", ""));
						g2d.drawString(String.valueOf(pin), ioLong + (x+1)*cellDim - fontSize*2, ioLong + (y+1)*cellDim - fontSize/2);
		    		}
	    		}
	    		
	    		// Now draw wire segments
	    		Color[] colors = {Color.RED, Color.GREEN, Color.ORANGE, Color.MAGENTA, Color.PINK, Color.CYAN, Color.YELLOW,  Color.LIGHT_GRAY};
	    		int maxLayers = 8;
	    		if (wire != null)
	    		{
    	    		for (int i = 0; i < wire.size(); i++)
    	    		{
    	    			WireSegment ws = wire.get(i);
    	    			
    	    			if (ws.layer >= maxLayers)
    	    				MFError.DisplayError("Currently not enough colors chosen for more than " + maxLayers + " layers.");
    	    			else
    	    				g2d.setColor(colors[ws.layer]);
    	    			
    	    			int trackXOff = cellDim / (da.numHTracks + 1);
    	    			int trackYOff = cellDim / (da.numVTracks + 1); 
    	    			if (ws.segmentType == SEGTYPE.LINE)
    	    			{
    	    				int x1 = ioLong + ws.sourceXCell*cellDim + (ws.sourceXTrack+1)*trackXOff;
    	    				int y1 = ioLong + ws.sourceYCell*cellDim + (ws.sourceYTrack+1)*trackYOff;
    	    				int x2 = ioLong + ws.destXCell*cellDim + (ws.destXTrack+1)*trackXOff;
    	    				int y2 = ioLong + ws.destYCell*cellDim + (ws.destYTrack+1)*trackYOff;
    	    				g2d.drawLine(x1, y1, x2, y2);
    	    				g2d.fillOval(x1-trackXOff/2, y1-trackYOff/2, trackXOff, trackYOff);
    	    				g2d.fillOval(x2-trackXOff/2, y2-trackYOff/2, trackXOff, trackYOff);
    	    			}
    	    			else
    	    				MFError.DisplayError("Unknown wire-segment type.");
    	    		}
	    		}
	    		
	        	f = new File(outDir + "/Pin" + pin + "." + ext);
	    		ImageIO.write(biPin, ext, f);
	    		main.updateProgress((int)((double)pinCount++/(double)coordsAtPin.size()*100), "PinOut " + pin + "/" + coordsAtPin.size());
			}
			
			
		}		
		// Update progress bar	
		main.updateProgress(100, "PinOuts Complete");
	} 
}


