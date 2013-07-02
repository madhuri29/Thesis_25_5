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
 * Name: Test (Test)															*
 *																				*
 * Details: This class contains several functions of popular benchmarks that	*
 * were created by hand (manually programmed by creating nodes and edges). 		*
 * The resultant DAGs can be used for testing and the timings can be modified 	*
 * by changing the source code for a particular assay function.					*
 *-----------------------------------------------------------------------------*/
#ifndef _TEST_H
#define _TEST_H

using namespace std;
#include "../../Headers/assay_node.h"
#include "../../Headers/enums.h"
#include "../../Headers/dag.h"
#include <stdlib.h>
#include <stdio.h>
#include<list>
#include <map>

class Test
{
    public:
        // Constructors
		Test();
        virtual ~Test ();

        // Benchmarks from http://people.ee.duke.edu/~fs/Benchmark.pdf
        static DAG *Create_B1_PCRMix(double mult, int repeat);
        static DAG *Create_B2_InVitroDiag_1(double mult, int repeat);
        static DAG *Create_B2_InVitroDiag_2(double mult, int repeat);
        static DAG *Create_B2_InVitroDiag_3(double mult, int repeat);
        static DAG *Create_B2_InVitroDiag_4(double mult, int repeat);
        static DAG *Create_B2_InVitroDiag_5(double mult, int repeat);
        static DAG *Create_B3_Protein(double mult, int repeat);
};
#endif
