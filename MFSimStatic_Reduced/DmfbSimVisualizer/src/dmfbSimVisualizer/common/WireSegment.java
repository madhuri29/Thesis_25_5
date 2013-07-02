package dmfbSimVisualizer.common;

public class WireSegment {
	public enum SEGTYPE { LINE, ARC } 
	public int pinNo;
	public int layer;
	public SEGTYPE segmentType;	
	
	// Start/stop relative locations
	public int sourceXCell;
	public int sourceYCell;
	public int sourceXTrack;
	public int sourceYTrack;
	public int destXCell;
	public int destYCell;
	public int destXTrack;
	public int destYTrack;
	
	// Arc parameters
	public int startAngle;
	public int arcAngle;
}
