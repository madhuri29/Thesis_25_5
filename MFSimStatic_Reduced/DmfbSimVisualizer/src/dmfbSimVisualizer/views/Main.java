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
 * Source: Main.java (Main)														*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: October 7, 2012							*
 *																				*
 * Details: GUI frame/window that shows the main controls of the program.		*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/

package dmfbSimVisualizer.views;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.EventQueue;
import java.awt.Font;
import java.awt.Frame;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import javax.swing.JMenuBar;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.ImageIcon;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.GroupLayout;
import javax.swing.GroupLayout.Alignment;
import javax.swing.LayoutStyle.ComponentPlacement;
import javax.swing.border.TitledBorder;
import javax.swing.plaf.ProgressBarUI;
import javax.swing.plaf.basic.BasicProgressBarUI;
import javax.swing.UIManager;
import java.awt.Toolkit;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Arrays;
import java.util.Comparator;

import javax.swing.JLabel;
import javax.swing.JComboBox;
import javax.swing.DefaultComboBoxModel;

import dmfbSimVisualizer.common.*;
import dmfbSimVisualizer.parsers.*;

import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;
import javax.swing.JButton;

import com.sun.j3d.utils.applet.MainFrame;
import javax.swing.JTextField;
import javax.swing.JProgressBar;
import javax.swing.JCheckBox;

public class Main extends JFrame {

	private JPanel contentPane;
	private JPanel pnlInitViewPreview;
	private JComboBox cbVisType;
	private JButton btnVisualize;
	private JSpinner spnWidth;
	private JSpinner spnHeight;
	private JTextField txtInput;
	private JProgressBar pbVisualize;
	private volatile boolean isOutputting;
	private Main mainPtr;
	private JLabel lblOutStatus;
	private JComboBox cbMovQuality;
	private JSpinner spnFreq;
	private JCheckBox cbxModuleIr;
	private JCheckBox cbxDropletIr;
	private JCheckBox cbxModuleNames;
	private JCheckBox cbxPinNumbers;
	private JCheckBox cbxPinActivations;
	private JCheckBox cbxDroplets;
	private JCheckBox cbxResourceLocations;
	private JCheckBox cbxWireSegments;

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (Throwable e) {
			e.printStackTrace();
		}
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					Main frame = new Main();
					frame.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the frame.
	 */
	public Main() {
		initComponents();
		createEvents();
	}
		
	//////////////////////////////////////////////////////////////////////////////////////
	// initComponents()
	// Created using Eclipse WindowBuilder Pro Plugin. Creates the windows and components
	// and lays them out accordingly. 	 
	//////////////////////////////////////////////////////////////////////////////////////
	private void initComponents()
	{
		mainPtr = this;
		isOutputting = false;
		
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); 
		setBounds(100, 100, 941, 597);
		
		JMenuBar menuBar = new JMenuBar();
		setJMenuBar(menuBar);
		
		JMenu mnFile = new JMenu("File");
		menuBar.add(mnFile);
		
		JMenuItem mntmExit = new JMenuItem("Exit");
		mntmExit.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				dispose();
			}
		});
		mntmExit.setIcon(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/exit_16.png")));
		mnFile.add(mntmExit);
		
		JMenu mnHelp = new JMenu("Help");
		menuBar.add(mnHelp);
		
		JMenuItem mntmAboutUs = new JMenuItem("About Us...");
		mntmAboutUs.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				AboutUs about = new AboutUs();
				about.setModalityType(java.awt.Dialog.ModalityType.APPLICATION_MODAL);
				about.setVisible(true);
			}
		});
		
		JMenuItem mntmControls3D = new JMenuItem("3D Controls");
		mntmControls3D.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				Controls3D controls = new Controls3D();
				controls.setModalityType(java.awt.Dialog.ModalityType.MODELESS);
				controls.setVisible(true);
			}
		});
		mnHelp.add(mntmControls3D);
		mntmAboutUs.setIcon(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/about_16.png")));
		mnHelp.add(mntmAboutUs);
		
		
		contentPane = new JPanel();
		contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
		setContentPane(contentPane);
		
		JPanel pnlInit = new JPanel();
		GroupLayout gl_contentPane = new GroupLayout(contentPane);
		gl_contentPane.setHorizontalGroup(
			gl_contentPane.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_contentPane.createSequentialGroup()
					.addContainerGap()
					.addComponent(pnlInit, GroupLayout.DEFAULT_SIZE, 1049, Short.MAX_VALUE)
					.addContainerGap())
		);
		gl_contentPane.setVerticalGroup(
			gl_contentPane.createParallelGroup(Alignment.LEADING)
				.addComponent(pnlInit, GroupLayout.DEFAULT_SIZE, 476, Short.MAX_VALUE)
		);
		
		JPanel pnlInitConfig = new JPanel();
		pnlInitConfig.setBorder(new TitledBorder(null, "Settings", TitledBorder.LEADING, TitledBorder.TOP, null, null));
		
		JPanel pnlInitView = new JPanel();
		pnlInitView.setBorder(new TitledBorder(UIManager.getBorder("TitledBorder.border"), "Visualization Example Preview", TitledBorder.LEADING, TitledBorder.TOP, null, null));
		GroupLayout gl_pnlInit = new GroupLayout(pnlInit);
		gl_pnlInit.setHorizontalGroup(
			gl_pnlInit.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_pnlInit.createSequentialGroup()
					.addContainerGap()
					.addComponent(pnlInitConfig, GroupLayout.DEFAULT_SIZE, 587, Short.MAX_VALUE)
					.addGap(18)
					.addComponent(pnlInitView, GroupLayout.PREFERRED_SIZE, 426, GroupLayout.PREFERRED_SIZE)
					.addGap(10))
		);
		gl_pnlInit.setVerticalGroup(
			gl_pnlInit.createParallelGroup(Alignment.TRAILING)
				.addGroup(gl_pnlInit.createSequentialGroup()
					.addContainerGap()
					.addGroup(gl_pnlInit.createParallelGroup(Alignment.TRAILING)
						.addComponent(pnlInitView, Alignment.LEADING, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
						.addComponent(pnlInitConfig, Alignment.LEADING, GroupLayout.DEFAULT_SIZE, 509, Short.MAX_VALUE))
					.addContainerGap())
		);
		
		JLabel lblVisualizationType = new JLabel("Visualization Type:");
		
		cbVisType = new JComboBox();
		cbVisType.setModel(new DefaultComboBoxModel(new String[] {"Hardware Description", "Unscheduled DAG", "Scheduled DAG", "Placed DAG", "Cyclic Simulation", "Cyclic Routes", "Compact Simulation", "Compact Routes", "2D Placement", "3D Placement"}));
		
		JLabel lblMaxDimensions = new JLabel("Max Dimensions:");
		
		spnWidth = new JSpinner();
		spnWidth.setModel(new SpinnerNumberModel(400, 1, 10000, 1));
		
		JLabel lblPxWide = new JLabel("px width");
		
		spnHeight = new JSpinner();
		spnHeight.setModel(new SpinnerNumberModel(400, 1, 10000, 1));
		
		JLabel lblPxHeight = new JLabel("px height");
		
		btnVisualize = new JButton("Visualize");
		btnVisualize.setIcon(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/play_16.png")));
		
		JLabel lblInputFile = new JLabel("Input File:");
		
		txtInput = new JTextField();
		txtInput.setColumns(10);
		txtInput.setText("Output/4_Hardware_Description.txt");
		
		UIManager.put("ProgressBar.selectionBackground",Color.BLACK);
		UIManager.put("ProgressBar.selectionForeground",Color.WHITE);
		pbVisualize = new JProgressBar();
		pbVisualize.setFont(new Font("Tahoma", Font.BOLD, 11));
		pbVisualize.setStringPainted(true);
		pbVisualize.setForeground(Color.BLUE);
		
		JLabel lblOutputProgress = new JLabel("Output Progress:");
		
		lblOutStatus = new JLabel("Status");
		
		JLabel lblMovie = new JLabel("Movie:");
		
		cbMovQuality = new JComboBox();
		cbMovQuality.setModel(new DefaultComboBoxModel(new String[] {"None", "Low Quality", "High Quality"}));
		cbMovQuality.setSelectedIndex(1);
		
		spnFreq = new JSpinner();
		spnFreq.setModel(new SpinnerNumberModel(100, 1, 1000, 1));
		
		JLabel lblHzfps = new JLabel("Hz (fps)");
		
		JPanel pnlDraw = new JPanel();
		pnlDraw.setBorder(new TitledBorder(null, "Draw Options", TitledBorder.LEADING, TitledBorder.TOP, null, null));
		GroupLayout gl_pnlInitConfig = new GroupLayout(pnlInitConfig);
		gl_pnlInitConfig.setHorizontalGroup(
			gl_pnlInitConfig.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_pnlInitConfig.createSequentialGroup()
					.addContainerGap()
					.addGroup(gl_pnlInitConfig.createParallelGroup(Alignment.TRAILING)
						.addComponent(lblInputFile)
						.addComponent(lblVisualizationType)
						.addComponent(lblMaxDimensions)
						.addComponent(lblMovie))
					.addPreferredGap(ComponentPlacement.RELATED)
					.addGroup(gl_pnlInitConfig.createParallelGroup(Alignment.LEADING)
						.addComponent(txtInput, GroupLayout.DEFAULT_SIZE, 306, Short.MAX_VALUE)
						.addComponent(cbVisType, 0, 306, Short.MAX_VALUE)
						.addGroup(gl_pnlInitConfig.createSequentialGroup()
							.addGroup(gl_pnlInitConfig.createParallelGroup(Alignment.LEADING, false)
								.addComponent(cbMovQuality, 0, GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
								.addGroup(gl_pnlInitConfig.createSequentialGroup()
									.addComponent(spnWidth, GroupLayout.PREFERRED_SIZE, 60, GroupLayout.PREFERRED_SIZE)
									.addPreferredGap(ComponentPlacement.RELATED)
									.addComponent(lblPxWide)))
							.addPreferredGap(ComponentPlacement.UNRELATED)
							.addGroup(gl_pnlInitConfig.createParallelGroup(Alignment.LEADING, false)
								.addComponent(spnFreq, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
								.addComponent(spnHeight, GroupLayout.PREFERRED_SIZE, 60, Short.MAX_VALUE))
							.addPreferredGap(ComponentPlacement.RELATED)
							.addGroup(gl_pnlInitConfig.createParallelGroup(Alignment.LEADING)
								.addComponent(lblHzfps)
								.addComponent(lblPxHeight))))
					.addContainerGap())
				.addGroup(Alignment.TRAILING, gl_pnlInitConfig.createSequentialGroup()
					.addGap(20)
					.addGroup(gl_pnlInitConfig.createParallelGroup(Alignment.LEADING)
						.addGroup(gl_pnlInitConfig.createSequentialGroup()
							.addComponent(btnVisualize)
							.addGap(73))
						.addGroup(gl_pnlInitConfig.createSequentialGroup()
							.addComponent(lblOutputProgress)
							.addPreferredGap(ComponentPlacement.RELATED)
							.addComponent(pbVisualize, GroupLayout.DEFAULT_SIZE, 312, Short.MAX_VALUE))
						.addComponent(lblOutStatus, Alignment.TRAILING)))
				.addGroup(gl_pnlInitConfig.createSequentialGroup()
					.addContainerGap()
					.addComponent(pnlDraw, GroupLayout.PREFERRED_SIZE, 402, GroupLayout.PREFERRED_SIZE)
					.addContainerGap(GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
		);
		gl_pnlInitConfig.setVerticalGroup(
			gl_pnlInitConfig.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_pnlInitConfig.createSequentialGroup()
					.addContainerGap()
					.addGroup(gl_pnlInitConfig.createParallelGroup(Alignment.BASELINE)
						.addComponent(lblVisualizationType)
						.addComponent(cbVisType, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
					.addGap(18)
					.addGroup(gl_pnlInitConfig.createParallelGroup(Alignment.BASELINE)
						.addComponent(lblInputFile)
						.addComponent(txtInput, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
					.addGap(18)
					.addGroup(gl_pnlInitConfig.createParallelGroup(Alignment.BASELINE)
						.addComponent(lblMaxDimensions)
						.addComponent(spnWidth, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
						.addComponent(lblPxWide)
						.addComponent(spnHeight, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
						.addComponent(lblPxHeight))
					.addGap(18)
					.addGroup(gl_pnlInitConfig.createParallelGroup(Alignment.BASELINE)
						.addComponent(lblMovie)
						.addComponent(cbMovQuality, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
						.addComponent(spnFreq, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE)
						.addComponent(lblHzfps))
					.addGap(14)
					.addComponent(pnlDraw, GroupLayout.PREFERRED_SIZE, 92, GroupLayout.PREFERRED_SIZE)
					.addPreferredGap(ComponentPlacement.UNRELATED)
					.addComponent(btnVisualize)
					.addGap(18)
					.addGroup(gl_pnlInitConfig.createParallelGroup(Alignment.BASELINE)
						.addComponent(lblOutputProgress)
						.addComponent(pbVisualize, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
					.addPreferredGap(ComponentPlacement.RELATED)
					.addComponent(lblOutStatus)
					.addContainerGap(144, Short.MAX_VALUE))
		);
		
		cbxModuleIr = new JCheckBox("Module IR");
		cbxModuleIr.setEnabled(false);
		cbxModuleIr.setSelected(true);
		cbxModuleIr.setToolTipText("Draws a shaded region covering the electrodes surrounding a module.");
		
		cbxDropletIr = new JCheckBox("Droplet IR");
		cbxDropletIr.setEnabled(false);
		cbxDropletIr.setSelected(true);
		cbxDropletIr.setToolTipText("Draws a shaded region covering the electrodes surrounding a droplet.");
		
		cbxModuleNames = new JCheckBox("Module Names");
		cbxModuleNames.setEnabled(false);
		cbxModuleNames.setSelected(true);
		cbxModuleNames.setToolTipText("Draws the name on each module which corresponds to a node in one of the DAGs.");
		
		cbxPinNumbers = new JCheckBox("Pin Numbers");
		cbxPinNumbers.setEnabled(false);
		cbxPinNumbers.setToolTipText("Draws the pin-numbers on each electrode.");
		
		cbxPinActivations = new JCheckBox("Pin Activations");
		cbxPinActivations.setEnabled(false);
		cbxPinActivations.setToolTipText("Fills in the electrodes that are activated each cycle.");
		
		cbxDroplets = new JCheckBox("Droplets");
		cbxDroplets.setEnabled(false);
		cbxDroplets.setSelected(true);
		
		cbxResourceLocations = new JCheckBox("Resource Locations");
		cbxResourceLocations.setSelected(true);
		cbxResourceLocations.setEnabled(false);
		
		cbxWireSegments = new JCheckBox("Wire Segments");
		GroupLayout gl_pnlDraw = new GroupLayout(pnlDraw);
		gl_pnlDraw.setHorizontalGroup(
			gl_pnlDraw.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_pnlDraw.createSequentialGroup()
					.addContainerGap()
					.addGroup(gl_pnlDraw.createParallelGroup(Alignment.LEADING)
						.addComponent(cbxModuleNames)
						.addComponent(cbxDropletIr)
						.addComponent(cbxModuleIr))
					.addPreferredGap(ComponentPlacement.UNRELATED)
					.addGroup(gl_pnlDraw.createParallelGroup(Alignment.LEADING)
						.addComponent(cbxPinActivations)
						.addGroup(gl_pnlDraw.createSequentialGroup()
							.addGroup(gl_pnlDraw.createParallelGroup(Alignment.LEADING)
								.addComponent(cbxDroplets)
								.addComponent(cbxPinNumbers))
							.addGap(16)
							.addGroup(gl_pnlDraw.createParallelGroup(Alignment.LEADING)
								.addComponent(cbxWireSegments)
								.addComponent(cbxResourceLocations))))
					.addContainerGap(67, Short.MAX_VALUE))
		);
		gl_pnlDraw.setVerticalGroup(
			gl_pnlDraw.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_pnlDraw.createSequentialGroup()
					.addGroup(gl_pnlDraw.createParallelGroup(Alignment.BASELINE)
						.addComponent(cbxModuleIr)
						.addComponent(cbxDroplets)
						.addComponent(cbxResourceLocations))
					.addPreferredGap(ComponentPlacement.RELATED)
					.addGroup(gl_pnlDraw.createParallelGroup(Alignment.BASELINE)
						.addComponent(cbxDropletIr)
						.addComponent(cbxPinNumbers)
						.addComponent(cbxWireSegments))
					.addPreferredGap(ComponentPlacement.RELATED, GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
					.addGroup(gl_pnlDraw.createParallelGroup(Alignment.BASELINE)
						.addComponent(cbxModuleNames)
						.addComponent(cbxPinActivations)))
		);
		pnlDraw.setLayout(gl_pnlDraw);
		pnlInitConfig.setLayout(gl_pnlInitConfig);
		
		//pnlInitViewPreview = new JPanel();
		pnlInitViewPreview = new ImagePanel(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/hardware.png")));
		GroupLayout gl_pnlInitView = new GroupLayout(pnlInitView);
		gl_pnlInitView.setHorizontalGroup(
			gl_pnlInitView.createParallelGroup(Alignment.LEADING)
				.addGroup(gl_pnlInitView.createSequentialGroup()
					.addContainerGap()
					.addComponent(pnlInitViewPreview, GroupLayout.DEFAULT_SIZE, GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
					.addGap(142))
		);
		gl_pnlInitView.setVerticalGroup(
			gl_pnlInitView.createParallelGroup(Alignment.TRAILING)
				.addGroup(Alignment.LEADING, gl_pnlInitView.createSequentialGroup()
					.addComponent(pnlInitViewPreview, GroupLayout.DEFAULT_SIZE, GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
					.addContainerGap())
		);
		pnlInitView.setLayout(gl_pnlInitView);
		pnlInit.setLayout(gl_pnlInit);
		contentPane.setLayout(gl_contentPane);
		
		setMovieSettingsEnabled(false);
		setOutDimsEnabled(true);
		setModDrawOptionsEnabled(false);
		setCyclicDrawOptionsEnabled(false);
		cbxPinNumbers.setEnabled(true);
		cbxResourceLocations.setEnabled(true);
	}

	
	//////////////////////////////////////////////////////////////////////////////////////
	// createEvents()
	// All event handlers are found here.
	//////////////////////////////////////////////////////////////////////////////////////
	private void createEvents()
	{
		setTitle("DMFB Static Simulator Visualizer");
		setIconImage(Toolkit.getDefaultToolkit().getImage(Main.class.getResource("/dmfbSimVisualizer/resources/logo_32.png")));
		
		// Changes the preview image when a change is made
		cbVisType.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				if (cbVisType.getSelectedItem().toString() == "Hardware Description")
				{
					((ImagePanel)pnlInitViewPreview).updateImage(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/hardware.png")));
					txtInput.setText("Output/4_Hardware_Description.txt");
					setMovieSettingsEnabled(false);
					setOutDimsEnabled(true);
					setModDrawOptionsEnabled(false);
					setCyclicDrawOptionsEnabled(false);
					cbxPinNumbers.setEnabled(true);
					cbxResourceLocations.setEnabled(true);
					cbxWireSegments.setEnabled(true);
				}
				else if (cbVisType.getSelectedItem().toString() == "Unscheduled DAG")
				{
					((ImagePanel)pnlInitViewPreview).updateImage(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/dagInit.png")));
					txtInput.setText(getMostRecentDotFile("0_"));
					setMovieSettingsEnabled(false);
					setOutDimsEnabled(false);
					setModDrawOptionsEnabled(false);
					setCyclicDrawOptionsEnabled(false);
					cbxResourceLocations.setEnabled(false);
					cbxWireSegments.setEnabled(false);
				}
				else if (cbVisType.getSelectedItem().toString() == "Scheduled DAG")
				{
					((ImagePanel)pnlInitViewPreview).updateImage(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/dagSched.png")));
					txtInput.setText(getMostRecentDotFile("1_"));
					setMovieSettingsEnabled(false);
					setOutDimsEnabled(false);
					setModDrawOptionsEnabled(false);
					setCyclicDrawOptionsEnabled(false);
					cbxResourceLocations.setEnabled(false);
					cbxWireSegments.setEnabled(false);
				}
				else if (cbVisType.getSelectedItem().toString() == "Placed DAG")
				{
					((ImagePanel)pnlInitViewPreview).updateImage(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/dagPlaced.png")));
					txtInput.setText(getMostRecentDotFile("2_"));
					setMovieSettingsEnabled(false);
					setOutDimsEnabled(false);
					setModDrawOptionsEnabled(false);
					setCyclicDrawOptionsEnabled(false);
					cbxResourceLocations.setEnabled(false);
					cbxWireSegments.setEnabled(false);
				}
				else if (cbVisType.getSelectedItem().toString() == "Cyclic Simulation")
				{
					((ImagePanel)pnlInitViewPreview).updateImage(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/cyclicSim.png")));
					txtInput.setText("Output/3_ROUTE_to_SIM.txt");
					setMovieSettingsEnabled(true);
					setOutDimsEnabled(true);
					setModDrawOptionsEnabled(true);
					setCyclicDrawOptionsEnabled(true);
					cbxResourceLocations.setEnabled(true);
					cbxWireSegments.setEnabled(false);
				}
				else if (cbVisType.getSelectedItem().toString() == "Cyclic Routes")
				{
					((ImagePanel)pnlInitViewPreview).updateImage(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/cyclicRoutes.png")));
					txtInput.setText("Output/3_ROUTE_to_SIM.txt");
					setMovieSettingsEnabled(true);
					setOutDimsEnabled(true);
					setModDrawOptionsEnabled(true);
					setCyclicDrawOptionsEnabled(true);
					cbxResourceLocations.setEnabled(true);
					cbxWireSegments.setEnabled(false);
				}
				else if (cbVisType.getSelectedItem().toString() == "Compact Simulation")
				{
					((ImagePanel)pnlInitViewPreview).updateImage(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/compactSim.png")));
					txtInput.setText("Output/3_COMPACT_ROUTE_to_SIM.txt");
					setMovieSettingsEnabled(false);
					setOutDimsEnabled(true);
					setModDrawOptionsEnabled(true);
					setCyclicDrawOptionsEnabled(false);
					cbxResourceLocations.setEnabled(true);
					cbxWireSegments.setEnabled(false);
				}
				else if (cbVisType.getSelectedItem().toString() == "Compact Routes")
				{
					((ImagePanel)pnlInitViewPreview).updateImage(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/compactRoutes.png")));
					txtInput.setText("Output/3_COMPACT_ROUTE_to_SIM.txt");
					setMovieSettingsEnabled(false);
					setOutDimsEnabled(true);
					setModDrawOptionsEnabled(true);
					setCyclicDrawOptionsEnabled(false);
					cbxResourceLocations.setEnabled(true);
					cbxWireSegments.setEnabled(false);
				}
				else if (cbVisType.getSelectedItem().toString() == "2D Placement")
				{
					((ImagePanel)pnlInitViewPreview).updateImage(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/2dPlace.png")));
					txtInput.setText("Output/2_PLACE_to_ROUTE.txt");
					setMovieSettingsEnabled(false);
					setOutDimsEnabled(true);
					setModDrawOptionsEnabled(true);
					setCyclicDrawOptionsEnabled(false);
					cbxResourceLocations.setEnabled(true);
					cbxWireSegments.setEnabled(false);
				}
				else if (cbVisType.getSelectedItem().toString() == "3D Placement")
				{
					((ImagePanel)pnlInitViewPreview).updateImage(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/3dPlace.png")));
					txtInput.setText("Output/2_PLACE_to_ROUTE.txt");
					setMovieSettingsEnabled(false);
					setOutDimsEnabled(false);
					setModDrawOptionsEnabled(false);
					setCyclicDrawOptionsEnabled(false);
					cbxResourceLocations.setEnabled(false);
					cbxWireSegments.setEnabled(false);
				}
				else
					MFError.DisplayError("Unknown/unhandled visualization type selected.");
				repaint();
			}
		});
		
		// Calls the appropriate visualizer to do it's job
		btnVisualize.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				
				if (!isOutputting)
				{
					isOutputting = true;
					DrawOptions drawOpts = new DrawOptions();
					drawOpts.maxWidth = (Integer) spnWidth.getValue();
					drawOpts.maxHeight = (Integer) spnHeight.getValue();
					drawOpts.frequency = (Integer) spnFreq.getValue();
					drawOpts.drawModIr = cbxModuleIr.isSelected();					
					drawOpts.drawDropIr = cbxDropletIr.isSelected();
					drawOpts.drawModuleNames = cbxModuleNames.isSelected();
					drawOpts.drawDroplets = cbxDroplets.isSelected();
					drawOpts.drawPinNumbers = cbxPinNumbers.isSelected();
					drawOpts.drawPinActivations = cbxPinActivations.isSelected();	
					drawOpts.drawResourceLocations = cbxResourceLocations.isSelected();
					drawOpts.drawWireSegments = cbxWireSegments.isSelected();
					drawOpts.movQuality = cbMovQuality.getSelectedItem().toString();
					drawOpts.simType = cbVisType.getSelectedItem().toString();
					drawOpts.inFile = txtInput.getText();
					
					Thread thread1 = new Thread(new OutputThread("outThread", mainPtr, drawOpts), "thread");
					thread1.start();
					btnVisualize.setText("Stop");
					btnVisualize.setIcon(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/stop_16.png")));
				}
				else
				{
					isOutputting = false;
					btnVisualize.setText("Visualize");
					btnVisualize.setIcon(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/play_16.png")));
				}
			}
		});
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// Utility Functions
	//////////////////////////////////////////////////////////////////////////////////////
	public void updateProgress(int percent, String status)
	{
		pbVisualize.setValue(percent);
		lblOutStatus.setText(status);
		
		if (!isOutputting)
		{
			pbVisualize.setForeground(Color.RED);
			lblOutStatus.setText("Halted at " + lblOutStatus.getText());
		}
		else if (percent < 100)
			pbVisualize.setForeground(Color.BLUE);
		else
		{
			pbVisualize.setForeground(new Color(34, 177, 76));
		}
		
		repaint();
	}
	public void setMovieSettingsEnabled(boolean enable)
	{
		spnFreq.setEnabled(enable);
		cbMovQuality.setEnabled(enable);
	}
	public void setOutDimsEnabled(boolean enable)
	{
		spnWidth.setEnabled(enable);
		spnHeight.setEnabled(enable);
	}
	public void setModDrawOptionsEnabled(boolean enable)
	{
		cbxModuleIr.setEnabled(enable);					
		cbxModuleNames.setEnabled(enable);
	}
	public void setCyclicDrawOptionsEnabled(boolean enable)
	{
		cbxDropletIr.setEnabled(enable);
		cbxDroplets.setEnabled(enable);
		cbxPinNumbers.setEnabled(enable);
		cbxPinActivations.setEnabled(enable);
	}
	public String getMostRecentDotFile(String prefix)
	{
		String inDir = "Output";
	    File dir = new File(inDir);
	    File[] files = dir.listFiles();
	    Arrays.sort(files, new Comparator<File>(){
	        public int compare(File f1, File f2)
	        {
	            return Long.valueOf(f2.lastModified()).compareTo(f1.lastModified());
	        } });
	    
	    for (int i = 0; i < files.length; i++)
	    	if (files[i].getName().startsWith(prefix) && files[i].getName().endsWith(".dot"))
	    		return files[i].getPath();
	    return "No file in Output directory automatically recognized.";
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	// getters/setters
	//////////////////////////////////////////////////////////////////////////////////////
	public boolean isOutputting() {
		return isOutputting;
	}
	public void setOutputting(boolean isOutputting) {
		this.isOutputting = isOutputting;
		mainPtr.btnVisualize.setText("Visualize");
		btnVisualize.setIcon(new ImageIcon(Main.class.getResource("/dmfbSimVisualizer/resources/play_16.png")));
	}
}
