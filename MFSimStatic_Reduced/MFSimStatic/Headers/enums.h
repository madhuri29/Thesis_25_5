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
/*--------------------------------File Details----------------------------------*
 * Name: Enums (Enumerations)													*
 *																				*
 * Details: This file contains a number of enumerated types used to make the	*
 * code easier to read and understand.											*
 *-----------------------------------------------------------------------------*/
#ifndef _ENUMS_H
#define _ENUMS_H

enum Direction { EAST, WEST, NORTH, SOUTH, ON_TARGET, DIR_UNINIT };
enum AssayNodeStatus { UNBOUND_UNSCHED = 0, SCHEDULED, BOUND, ROUTED, EXECUTING, COMPLETE };
enum ResourceType { BASIC_RES = 0, D_RES = 1, H_RES = 2, DH_RES = 3, SSD_RES = 4, RB_RES = 5, RES_TYPE_MAX = RB_RES, UNKNOWN_RES };
enum OperationType { GENERAL, STORAGE_HOLDER, STORAGE, DISPENSE, MIX, SPLIT, HEAT, DETECT, OUTPUT };
enum DropletStatus { DROP_NORMAL, DROP_WAIT, DROP_INT_WAIT, DROP_PROCESSING, DROP_OUTPUT, DROP_CONSUMED, DROP_DRIFT };
enum ExecutionType { SIM_EX, PROG_EX, ALL_EX }; // Is program being run to generate simulation or actual DMFB program
enum WireSegType { LINE_WS, ARC_WS };

// Synthesis types
enum SchedulerType { LIST_S, PATH_S, GENET_S, RICKETT_S, FD_LIST_S /*,NEW_S*/};
enum PlacerType { GRISSOM_LE_B, SA_P /*,NEW_P*/}; // Placer OR Binder
enum RouterType { GRISSOM_FIX_R, GRISSOM_FIX_MAP_R, ROY_MAZE_R, BIOROUTE_R /*,NEW_R*/};

// Scheduling utility types
enum ResourceAllocationType { GRISSOM_FIX_0_RA, GRISSOM_FIX_1_RA, GRISSOM_FIX_2_RA, GRISSOM_FIX_3_RA, PC_INHERENT_RA, INHERIT_RA }; // Determines how many resources (and what type) will be available for the scheduler

// Routing utility types
enum CompactionType { NO_COMP, BEG_COMP, MID_COMP, INHERENT_COMP };
enum ProcessEngineType { FIXED_QUICK_PE, FIXED_FULL_PE }; // How to move droplets in modules

// Pin-mapping types
enum PinMapType { INDIVADDR_PM };

// Wire-routing types
enum WireRouteType { NONE_WR };

#endif
