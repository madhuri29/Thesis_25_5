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
 * Source: test.cc																*
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
#include "../../Headers/Testing/test.h"

///////////////////////////////////////////////////////////////////////////
// This benchmark details the DAG for the PCR mixing stage detailed
// in Chakrabarty's benchmarks.
///////////////////////////////////////////////////////////////////////////
DAG *Test::Create_B1_PCRMix(double mult, int repeat)
{
	DAG *dag = new DAG();
	dag->setName("PCR");

	for (int i=0; i < repeat; i++)
	{
		AssayNode *D1 = dag->AddDispenseNode("tris-hcl", 10, "D_tris-hcl");
		AssayNode *D2 = dag->AddDispenseNode("kcl", 10, "D_kcl");
		AssayNode *D3 = dag->AddDispenseNode("bovine", 10, "D_bovine");
		AssayNode *D4 = dag->AddDispenseNode("gelatin", 10, "D_gelatin");
		AssayNode *D5 = dag->AddDispenseNode("primer", 10, "D_primer");
		AssayNode *D6 = dag->AddDispenseNode("beosynucleotide", 10, "D_beosynucleotide");
		AssayNode *D7 = dag->AddDispenseNode("amplitag", 10, "D_amplitag");
		AssayNode *D8 = dag->AddDispenseNode("lambda", 10, "D_lambda");

		//AssayNode *M1 = dag->AddMixNode(2, 3*mult, "M1");
		AssayNode *M1 = dag->AddMixNode(2, 10*mult, "M1");
		AssayNode *M2 = dag->AddMixNode(2, 5*mult, "M2");
		AssayNode *M3 = dag->AddMixNode(2, 6*mult, "M3");
		AssayNode *M4 = dag->AddMixNode(2, 5*mult, "M4");
		AssayNode *M5 = dag->AddMixNode(2, 5*mult, "M5");
		AssayNode *M6 = dag->AddMixNode(2, 10*mult, "M6");
		AssayNode *M7 = dag->AddMixNode(2, 3*mult, "M7");

		AssayNode *O1 = dag->AddOutputNode("output", "O_output");

		dag->ParentChild(D1, M1);
		dag->ParentChild(D2, M1);
		dag->ParentChild(D3, M2);
		dag->ParentChild(D4, M2);
		dag->ParentChild(D5, M4);
		dag->ParentChild(D6, M4);
		dag->ParentChild(D7, M5);
		dag->ParentChild(D8, M5);

		dag->ParentChild(M1, M3);
		dag->ParentChild(M2, M3);
		dag->ParentChild(M4, M6);
		dag->ParentChild(M5, M6);
		dag->ParentChild(M3, M7);
		dag->ParentChild(M6, M7);

		dag->ParentChild(M7, O1);
	}

    cout << "Bench1_PCRMix CREATED" << endl;
    return dag;

}

///////////////////////////////////////////////////////////////////////////
// This benchmark details the DAG for the multiplexed in-vitro diagnostics
// assay detailed in Chakrabarty's benchmarks.  Plasma/Serum are assayed
// for glucose/lactate measurements
///////////////////////////////////////////////////////////////////////////
DAG *Test::Create_B2_InVitroDiag_1(double mult, int repeat)
{
	DAG *dag = new DAG();
	dag->setName("In_Vitro_1");

	for (int i=0; i < repeat; i++)
	{
		AssayNode *D1 = dag->AddDispenseNode("S1", 10, "S1_1");
		AssayNode *D2 = dag->AddDispenseNode("S1", 10, "S1_2");
		AssayNode *D3 = dag->AddDispenseNode("S2", 10, "S2_1");
		AssayNode *D4 = dag->AddDispenseNode("S2", 10, "S2_2");
		AssayNode *D5 = dag->AddDispenseNode("R1", 10, "R1_1");
		AssayNode *D6 = dag->AddDispenseNode("R2", 10, "R2_1");
		AssayNode *D7 = dag->AddDispenseNode("R1", 10, "R1_2");
		AssayNode *D8 = dag->AddDispenseNode("R2", 10, "R2_2");

		AssayNode *M1 = dag->AddMixNode(2, 5*mult, "M1");
		AssayNode *M2 = dag->AddMixNode(2, 5*mult, "M2");
		AssayNode *M3 = dag->AddMixNode(2, 3*mult, "M3");
		AssayNode *M4 = dag->AddMixNode(2, 3*mult, "M4");

		AssayNode *d1 = dag->AddDetectNode(1, 5*mult, "dt1");
		AssayNode *d2 = dag->AddDetectNode(1, 4*mult, "dt2");
		AssayNode *d3 = dag->AddDetectNode(1, 5*mult, "dt3");
		AssayNode *d4 = dag->AddDetectNode(1, 4*mult, "dt4");

		AssayNode *O1 = dag->AddOutputNode("output", "O1");
		AssayNode *O2 = dag->AddOutputNode("output", "O2");
		AssayNode *O3 = dag->AddOutputNode("output", "O3");
		AssayNode *O4 = dag->AddOutputNode("output", "O4");

		dag->ParentChild(D1, M1);
		dag->ParentChild(D2, M2);
		dag->ParentChild(D3, M3);
		dag->ParentChild(D4, M4);
		dag->ParentChild(D5, M1);
		dag->ParentChild(D6, M2);
		dag->ParentChild(D7, M3);
		dag->ParentChild(D8, M4);


		dag->ParentChild(M1, d1);
		dag->ParentChild(M2, d2);
		dag->ParentChild(M3, d3);
		dag->ParentChild(M4, d4);

		dag->ParentChild(d1, O1);
		dag->ParentChild(d2, O2);
		dag->ParentChild(d3, O3);
		dag->ParentChild(d4, O4);
	}

	cout << "Bench2_InVitroDiag_1 CREATED" << endl;
	return dag;
}

///////////////////////////////////////////////////////////////////////////
// This benchmark details the DAG for the multiplexed in-vitro diagnostics
// assay detailed in Chakrabarty's benchmarks.  Plasma/Serum are assayed
// for glucose/lactate/pyruvate measurements
///////////////////////////////////////////////////////////////////////////
DAG *Test::Create_B2_InVitroDiag_2(double mult, int repeat)
{
	DAG *dag = new DAG();
	dag->setName("InVitro_2");

	for (int i=0; i < repeat; i++)
	{
		AssayNode *D1 = dag->AddDispenseNode("S1", 10);
		AssayNode *D2 = dag->AddDispenseNode("S1", 10);
		AssayNode *D3 = dag->AddDispenseNode("S1", 10);
		AssayNode *D4 = dag->AddDispenseNode("S2", 10);
		AssayNode *D5 = dag->AddDispenseNode("S2", 10);
		AssayNode *D6 = dag->AddDispenseNode("S2", 10);
		AssayNode *D7 = dag->AddDispenseNode("R1", 10);
		AssayNode *D8 = dag->AddDispenseNode("R2", 10);
		AssayNode *D9 = dag->AddDispenseNode("R3", 10);
		AssayNode *D10 = dag->AddDispenseNode("R1", 10);
		AssayNode *D11 = dag->AddDispenseNode("R2", 10);
		AssayNode *D12 = dag->AddDispenseNode("R3", 10);

		AssayNode *M1 = dag->AddMixNode(2, 5*mult, "M1");
		AssayNode *M2 = dag->AddMixNode(2, 5*mult, "M2");
		AssayNode *M3 = dag->AddMixNode(2, 5*mult, "M3");
		AssayNode *M4 = dag->AddMixNode(2, 3*mult, "M4");
		AssayNode *M5 = dag->AddMixNode(2, 3*mult, "M5");
		AssayNode *M6 = dag->AddMixNode(2, 3*mult, "M6");

		AssayNode *d1 = dag->AddDetectNode(1, 5*mult, "dt1");
		AssayNode *d2 = dag->AddDetectNode(1, 4*mult, "dt2");
		AssayNode *d3 = dag->AddDetectNode(1, 6*mult, "dt3");
		AssayNode *d4 = dag->AddDetectNode(1, 5*mult, "dt4");
		AssayNode *d5 = dag->AddDetectNode(1, 4*mult, "dt5");
		AssayNode *d6 = dag->AddDetectNode(1, 6*mult, "dt6");

		AssayNode *O1 = dag->AddOutputNode("output");
		AssayNode *O2 = dag->AddOutputNode("output");
		AssayNode *O3 = dag->AddOutputNode("output");
		AssayNode *O4 = dag->AddOutputNode("output");
		AssayNode *O5 = dag->AddOutputNode("output");
		AssayNode *O6 = dag->AddOutputNode("output");

		dag->ParentChild(D1, M1);
		dag->ParentChild(D2, M2);
		dag->ParentChild(D3, M3);
		dag->ParentChild(D4, M4);
		dag->ParentChild(D5, M5);
		dag->ParentChild(D6, M6);
		dag->ParentChild(D7, M1);
		dag->ParentChild(D8, M2);
		dag->ParentChild(D9, M3);
		dag->ParentChild(D10, M4);
		dag->ParentChild(D11, M5);
		dag->ParentChild(D12, M6);

		dag->ParentChild(M1, d1);
		dag->ParentChild(M2, d2);
		dag->ParentChild(M3, d3);
		dag->ParentChild(M4, d4);
		dag->ParentChild(M5, d5);
		dag->ParentChild(M6, d6);

		dag->ParentChild(d1, O1);
		dag->ParentChild(d2, O2);
		dag->ParentChild(d3, O3);
		dag->ParentChild(d4, O4);
		dag->ParentChild(d5, O5);
		dag->ParentChild(d6, O6);
	}

	cout << "Bench2_InVitroDiag_2 CREATED" << endl;
	return dag;
}

///////////////////////////////////////////////////////////////////////////
// This benchmark details the DAG for the multiplexed in-vitro diagnostics
// assay detailed in Chakrabarty's benchmarks.  Plasma/Serum/Saliva are
// assayed for glucose/lactate/pyruvate measurements
///////////////////////////////////////////////////////////////////////////
DAG *Test::Create_B2_InVitroDiag_3(double mult, int repeat)
{
	DAG *dag = new DAG();
	dag->setName("InVitro_3");

	for (int i=0; i < repeat; i++)
	{
		AssayNode *D1 = dag->AddDispenseNode("S1", 10);
		AssayNode *D2 = dag->AddDispenseNode("S1", 10);
		AssayNode *D3 = dag->AddDispenseNode("S1", 10);
		AssayNode *D4 = dag->AddDispenseNode("S2", 10);
		AssayNode *D5 = dag->AddDispenseNode("S2", 10);
		AssayNode *D6 = dag->AddDispenseNode("S2", 10);
		AssayNode *D7 = dag->AddDispenseNode("S3", 10);
		AssayNode *D8 = dag->AddDispenseNode("S3", 10);
		AssayNode *D9 = dag->AddDispenseNode("S3", 10);
		AssayNode *D10 = dag->AddDispenseNode("R1", 10);
		AssayNode *D11 = dag->AddDispenseNode("R2", 10);
		AssayNode *D12 = dag->AddDispenseNode("R3", 10);
		AssayNode *D13 = dag->AddDispenseNode("R1", 10);
		AssayNode *D14 = dag->AddDispenseNode("R2", 10);
		AssayNode *D15 = dag->AddDispenseNode("R3", 10);
		AssayNode *D16 = dag->AddDispenseNode("R1", 10);
		AssayNode *D17 = dag->AddDispenseNode("R2", 10);
		AssayNode *D18 = dag->AddDispenseNode("R3", 10);

		AssayNode *M1 = dag->AddMixNode(2, 5*mult, "M1");
		AssayNode *M2 = dag->AddMixNode(2, 5*mult, "M2");
		AssayNode *M3 = dag->AddMixNode(2, 5*mult, "M3");
		AssayNode *M4 = dag->AddMixNode(2, 3*mult, "M4");
		AssayNode *M5 = dag->AddMixNode(2, 3*mult, "M5");
		AssayNode *M6 = dag->AddMixNode(2, 3*mult, "M6");
		AssayNode *M7 = dag->AddMixNode(2, 4*mult, "M7");
		AssayNode *M8 = dag->AddMixNode(2, 4*mult, "M8");
		AssayNode *M9 = dag->AddMixNode(2, 4*mult, "M9");

		AssayNode *d1 = dag->AddDetectNode(1, 5*mult, "dt1");
		AssayNode *d2 = dag->AddDetectNode(1, 4*mult, "dt2");
		AssayNode *d3 = dag->AddDetectNode(1, 6*mult, "dt3");
		AssayNode *d4 = dag->AddDetectNode(1, 5*mult, "dt4");
		AssayNode *d5 = dag->AddDetectNode(1, 4*mult, "dt5");
		AssayNode *d6 = dag->AddDetectNode(1, 6*mult, "dt6");
		AssayNode *d7 = dag->AddDetectNode(1, 5*mult, "dt7");
		AssayNode *d8 = dag->AddDetectNode(1, 4*mult, "dt8");
		AssayNode *d9 = dag->AddDetectNode(1, 6*mult, "dt9");

		AssayNode *O1 = dag->AddOutputNode("output");
		AssayNode *O2 = dag->AddOutputNode("output");
		AssayNode *O3 = dag->AddOutputNode("output");
		AssayNode *O4 = dag->AddOutputNode("output");
		AssayNode *O5 = dag->AddOutputNode("output");
		AssayNode *O6 = dag->AddOutputNode("output");
		AssayNode *O7 = dag->AddOutputNode("output");
		AssayNode *O8 = dag->AddOutputNode("output");
		AssayNode *O9 = dag->AddOutputNode("output");

		dag->ParentChild(D1, M1);
		dag->ParentChild(D2, M2);
		dag->ParentChild(D3, M3);
		dag->ParentChild(D4, M4);
		dag->ParentChild(D5, M5);
		dag->ParentChild(D6, M6);
		dag->ParentChild(D7, M7);
		dag->ParentChild(D8, M8);
		dag->ParentChild(D9, M9);
		dag->ParentChild(D10, M1);
		dag->ParentChild(D11, M2);
		dag->ParentChild(D12, M3);
		dag->ParentChild(D13, M4);
		dag->ParentChild(D14, M5);
		dag->ParentChild(D15, M6);
		dag->ParentChild(D16, M7);
		dag->ParentChild(D17, M8);
		dag->ParentChild(D18, M9);


		dag->ParentChild(M1, d1);
		dag->ParentChild(M2, d2);
		dag->ParentChild(M3, d3);
		dag->ParentChild(M4, d4);
		dag->ParentChild(M5, d5);
		dag->ParentChild(M6, d6);
		dag->ParentChild(M7, d7);
		dag->ParentChild(M8, d8);
		dag->ParentChild(M9, d9);

		dag->ParentChild(d1, O1);
		dag->ParentChild(d2, O2);
		dag->ParentChild(d3, O3);
		dag->ParentChild(d4, O4);
		dag->ParentChild(d5, O5);
		dag->ParentChild(d6, O6);
		dag->ParentChild(d7, O7);
		dag->ParentChild(d8, O8);
		dag->ParentChild(d9, O9);
	}

	cout << "Bench2_InVitroDiag_3 CREATED" << endl;
	return dag;
}

///////////////////////////////////////////////////////////////////////////
// This benchmark details the DAG for the multiplexed in-vitro diagnostics
// assay detailed in Chakrabarty's benchmarks.  Plasma/Serum/Saliva are
// assayed for glucose/lactate/pyruvate/glutamate measurements
///////////////////////////////////////////////////////////////////////////
DAG *Test::Create_B2_InVitroDiag_4(double mult, int repeat)
{
	DAG *dag = new DAG();
	dag->setName("InVitro_4");

	for (int i=0; i < repeat; i++)
	{
		AssayNode *D1 = dag->AddDispenseNode("S1", 10);
		AssayNode *D2 = dag->AddDispenseNode("S1", 10);
		AssayNode *D3 = dag->AddDispenseNode("S1", 10);
		AssayNode *D4 = dag->AddDispenseNode("S1", 10);
		AssayNode *D5 = dag->AddDispenseNode("S2", 10);
		AssayNode *D6 = dag->AddDispenseNode("S2", 10);
		AssayNode *D7 = dag->AddDispenseNode("S2", 10);
		AssayNode *D8 = dag->AddDispenseNode("S2", 10);
		AssayNode *D9 = dag->AddDispenseNode("S3", 10);
		AssayNode *D10 = dag->AddDispenseNode("S3", 10);
		AssayNode *D11 = dag->AddDispenseNode("S3", 10);
		AssayNode *D12 = dag->AddDispenseNode("S3", 10);
		AssayNode *D13 = dag->AddDispenseNode("R1", 10);
		AssayNode *D14 = dag->AddDispenseNode("R2", 10);
		AssayNode *D15 = dag->AddDispenseNode("R3", 10);
		AssayNode *D16 = dag->AddDispenseNode("R4", 10);
		AssayNode *D17 = dag->AddDispenseNode("R1", 10);
		AssayNode *D18 = dag->AddDispenseNode("R2", 10);
		AssayNode *D19 = dag->AddDispenseNode("R3", 10);
		AssayNode *D20 = dag->AddDispenseNode("R4", 10);
		AssayNode *D21 = dag->AddDispenseNode("R1", 10);
		AssayNode *D22 = dag->AddDispenseNode("R2", 10);
		AssayNode *D23 = dag->AddDispenseNode("R3", 10);
		AssayNode *D24 = dag->AddDispenseNode("R4", 10);

		AssayNode *M1 = dag->AddMixNode(2, 5*mult, "M1");
		AssayNode *M2 = dag->AddMixNode(2, 5*mult, "M2");
		AssayNode *M3 = dag->AddMixNode(2, 5*mult, "M3");
		AssayNode *M4 = dag->AddMixNode(2, 5*mult, "M4");
		AssayNode *M5 = dag->AddMixNode(2, 3*mult, "M5");
		AssayNode *M6 = dag->AddMixNode(2, 3*mult, "M6");
		AssayNode *M7 = dag->AddMixNode(2, 3*mult, "M7");
		AssayNode *M8 = dag->AddMixNode(2, 3*mult, "M8");
		AssayNode *M9 = dag->AddMixNode(2, 4*mult, "M9");
		AssayNode *M10 = dag->AddMixNode(2, 4*mult, "M10");
		AssayNode *M11 = dag->AddMixNode(2, 4*mult, "M11");
		AssayNode *M12 = dag->AddMixNode(2, 4*mult, "M12");

		AssayNode *d1 = dag->AddDetectNode(1, 5*mult, "dt1");
		AssayNode *d2 = dag->AddDetectNode(1, 4*mult, "dt2");
		AssayNode *d3 = dag->AddDetectNode(1, 6*mult, "dt3");
		AssayNode *d4 = dag->AddDetectNode(1, 5*mult, "dt4");
		AssayNode *d5 = dag->AddDetectNode(1, 5*mult, "dt5");
		AssayNode *d6 = dag->AddDetectNode(1, 4*mult, "dt6");
		AssayNode *d7 = dag->AddDetectNode(1, 6*mult, "dt7");
		AssayNode *d8 = dag->AddDetectNode(1, 5*mult, "dt8");
		AssayNode *d9 = dag->AddDetectNode(1, 5*mult, "dt9");
		AssayNode *d10 = dag->AddDetectNode(1, 4*mult, "dt10");
		AssayNode *d11 = dag->AddDetectNode(1, 6*mult, "dt11");
		AssayNode *d12 = dag->AddDetectNode(1, 5*mult, "dt12");

		AssayNode *O1 = dag->AddOutputNode("output");
		AssayNode *O2 = dag->AddOutputNode("output");
		AssayNode *O3 = dag->AddOutputNode("output");
		AssayNode *O4 = dag->AddOutputNode("output");
		AssayNode *O5 = dag->AddOutputNode("output");
		AssayNode *O6 = dag->AddOutputNode("output");
		AssayNode *O7 = dag->AddOutputNode("output");
		AssayNode *O8 = dag->AddOutputNode("output");
		AssayNode *O9 = dag->AddOutputNode("output");
		AssayNode *O10 = dag->AddOutputNode("output");
		AssayNode *O11 = dag->AddOutputNode("output");
		AssayNode *O12 = dag->AddOutputNode("output");

		dag->ParentChild(D1, M1);
		dag->ParentChild(D2, M2);
		dag->ParentChild(D3, M3);
		dag->ParentChild(D4, M4);
		dag->ParentChild(D5, M5);
		dag->ParentChild(D6, M6);
		dag->ParentChild(D7, M7);
		dag->ParentChild(D8, M8);
		dag->ParentChild(D9, M9);
		dag->ParentChild(D10, M10);
		dag->ParentChild(D11, M11);
		dag->ParentChild(D12, M12);
		dag->ParentChild(D13, M1);
		dag->ParentChild(D14, M2);
		dag->ParentChild(D15, M3);
		dag->ParentChild(D16, M4);
		dag->ParentChild(D17, M5);
		dag->ParentChild(D18, M6);
		dag->ParentChild(D19, M7);
		dag->ParentChild(D20, M8);
		dag->ParentChild(D21, M9);
		dag->ParentChild(D22, M10);
		dag->ParentChild(D23, M11);
		dag->ParentChild(D24, M12);


		dag->ParentChild(M1, d1);
		dag->ParentChild(M2, d2);
		dag->ParentChild(M3, d3);
		dag->ParentChild(M4, d4);
		dag->ParentChild(M5, d5);
		dag->ParentChild(M6, d6);
		dag->ParentChild(M7, d7);
		dag->ParentChild(M8, d8);
		dag->ParentChild(M9, d9);
		dag->ParentChild(M10, d10);
		dag->ParentChild(M11, d11);
		dag->ParentChild(M12, d12);

		dag->ParentChild(d1, O1);
		dag->ParentChild(d2, O2);
		dag->ParentChild(d3, O3);
		dag->ParentChild(d4, O4);
		dag->ParentChild(d5, O5);
		dag->ParentChild(d6, O6);
		dag->ParentChild(d7, O7);
		dag->ParentChild(d8, O8);
		dag->ParentChild(d9, O9);
		dag->ParentChild(d10, O10);
		dag->ParentChild(d11, O11);
		dag->ParentChild(d12, O12);
	}

	cout << "Bench2_InVitroDiag_4 CREATED" << endl;
	return dag;
}

///////////////////////////////////////////////////////////////////////////
// This benchmark details the DAG for the multiplexed in-vitro diagnostics
// assay detailed in Chakrabarty's benchmarks.  Plasma/Serum/Saliva/Urine
// are assayed for glucose/lactate/pyruvate/glutamate measurements
///////////////////////////////////////////////////////////////////////////
DAG *Test::Create_B2_InVitroDiag_5(double mult, int repeat)
{
	DAG *dag = new DAG();
	dag->setName("InVitro_5");

	for (int i=0; i < repeat; i++)
	{
		AssayNode *D1 = dag->AddDispenseNode("S1", 10);
		AssayNode *D2 = dag->AddDispenseNode("S1", 10);
		AssayNode *D3 = dag->AddDispenseNode("S1", 10);
		AssayNode *D4 = dag->AddDispenseNode("S1", 10);
		AssayNode *D5 = dag->AddDispenseNode("S2", 10);
		AssayNode *D6 = dag->AddDispenseNode("S2", 10);
		AssayNode *D7 = dag->AddDispenseNode("S2", 10);
		AssayNode *D8 = dag->AddDispenseNode("S2", 10);
		AssayNode *D9 = dag->AddDispenseNode("S3", 10);
		AssayNode *D10 = dag->AddDispenseNode("S3", 10);
		AssayNode *D11 = dag->AddDispenseNode("S3", 10);
		AssayNode *D12 = dag->AddDispenseNode("S3", 10);
		AssayNode *D13 = dag->AddDispenseNode("S4", 10);
		AssayNode *D14 = dag->AddDispenseNode("S4", 10);
		AssayNode *D15 = dag->AddDispenseNode("S4", 10);
		AssayNode *D16 = dag->AddDispenseNode("S4", 10);
		AssayNode *D17 = dag->AddDispenseNode("R1", 10);
		AssayNode *D18 = dag->AddDispenseNode("R2", 10);
		AssayNode *D19 = dag->AddDispenseNode("R3", 10);
		AssayNode *D20 = dag->AddDispenseNode("R4", 10);
		AssayNode *D21 = dag->AddDispenseNode("R1", 10);
		AssayNode *D22 = dag->AddDispenseNode("R2", 10);
		AssayNode *D23 = dag->AddDispenseNode("R3", 10);
		AssayNode *D24 = dag->AddDispenseNode("R4", 10);
		AssayNode *D25 = dag->AddDispenseNode("R1", 10);
		AssayNode *D26 = dag->AddDispenseNode("R2", 10);
		AssayNode *D27 = dag->AddDispenseNode("R3", 10);
		AssayNode *D28 = dag->AddDispenseNode("R4", 10);
		AssayNode *D29 = dag->AddDispenseNode("R1", 10);
		AssayNode *D30 = dag->AddDispenseNode("R2", 10);
		AssayNode *D31 = dag->AddDispenseNode("R3", 10);
		AssayNode *D32 = dag->AddDispenseNode("R4", 10);

		AssayNode *M1 = dag->AddMixNode(2, 5*mult, "M1");
		AssayNode *M2 = dag->AddMixNode(2, 5*mult, "M2");
		AssayNode *M3 = dag->AddMixNode(2, 5*mult, "M3");
		AssayNode *M4 = dag->AddMixNode(2, 5*mult, "M4");
		AssayNode *M5 = dag->AddMixNode(2, 3*mult, "M5");
		AssayNode *M6 = dag->AddMixNode(2, 3*mult, "M6");
		AssayNode *M7 = dag->AddMixNode(2, 3*mult, "M7");
		AssayNode *M8 = dag->AddMixNode(2, 3*mult, "M8");
		AssayNode *M9 = dag->AddMixNode(2, 4*mult, "M9");
		AssayNode *M10 = dag->AddMixNode(2, 4*mult, "M10");
		AssayNode *M11 = dag->AddMixNode(2, 4*mult, "M11");
		AssayNode *M12 = dag->AddMixNode(2, 4*mult, "M12");
		AssayNode *M13 = dag->AddMixNode(2, 6*mult, "M13");
		AssayNode *M14 = dag->AddMixNode(2, 6*mult, "M14");
		AssayNode *M15 = dag->AddMixNode(2, 6*mult, "M15");
		AssayNode *M16 = dag->AddMixNode(2, 6*mult, "M16");

		AssayNode *d1 = dag->AddDetectNode(1, 5*mult, "dt1");
		AssayNode *d2 = dag->AddDetectNode(1, 4*mult, "dt2");
		AssayNode *d3 = dag->AddDetectNode(1, 6*mult, "dt3");
		AssayNode *d4 = dag->AddDetectNode(1, 5*mult, "dt4");
		AssayNode *d5 = dag->AddDetectNode(1, 5*mult, "dt5");
		AssayNode *d6 = dag->AddDetectNode(1, 4*mult, "dt6");
		AssayNode *d7 = dag->AddDetectNode(1, 6*mult, "dt7");
		AssayNode *d8 = dag->AddDetectNode(1, 5*mult, "dt8");
		AssayNode *d9 = dag->AddDetectNode(1, 5*mult, "dt9");
		AssayNode *d10 = dag->AddDetectNode(1, 4*mult, "dt10");
		AssayNode *d11 = dag->AddDetectNode(1, 6*mult, "dt11");
		AssayNode *d12 = dag->AddDetectNode(1, 5*mult, "dt12");
		AssayNode *d13 = dag->AddDetectNode(1, 5*mult, "dt13");
		AssayNode *d14 = dag->AddDetectNode(1, 4*mult, "dt14");
		AssayNode *d15 = dag->AddDetectNode(1, 6*mult, "dt15");
		AssayNode *d16 = dag->AddDetectNode(1, 5*mult, "dt16");

		AssayNode *O1 = dag->AddOutputNode("output");
		AssayNode *O2 = dag->AddOutputNode("output");
		AssayNode *O3 = dag->AddOutputNode("output");
		AssayNode *O4 = dag->AddOutputNode("output");
		AssayNode *O5 = dag->AddOutputNode("output");
		AssayNode *O6 = dag->AddOutputNode("output");
		AssayNode *O7 = dag->AddOutputNode("output");
		AssayNode *O8 = dag->AddOutputNode("output");
		AssayNode *O9 = dag->AddOutputNode("output");
		AssayNode *O10 = dag->AddOutputNode("output");
		AssayNode *O11 = dag->AddOutputNode("output");
		AssayNode *O12 = dag->AddOutputNode("output");
		AssayNode *O13 = dag->AddOutputNode("output");
		AssayNode *O14 = dag->AddOutputNode("output");
		AssayNode *O15 = dag->AddOutputNode("output");
		AssayNode *O16 = dag->AddOutputNode("output");

		dag->ParentChild(D1, M1);
		dag->ParentChild(D2, M2);
		dag->ParentChild(D3, M3);
		dag->ParentChild(D4, M4);
		dag->ParentChild(D5, M5);
		dag->ParentChild(D6, M6);
		dag->ParentChild(D7, M7);
		dag->ParentChild(D8, M8);
		dag->ParentChild(D9, M9);
		dag->ParentChild(D10, M10);
		dag->ParentChild(D11, M11);
		dag->ParentChild(D12, M12);
		dag->ParentChild(D13, M13);
		dag->ParentChild(D14, M14);
		dag->ParentChild(D15, M15);
		dag->ParentChild(D16, M16);
		dag->ParentChild(D17, M1);
		dag->ParentChild(D18, M2);
		dag->ParentChild(D19, M3);
		dag->ParentChild(D20, M4);
		dag->ParentChild(D21, M5);
		dag->ParentChild(D22, M6);
		dag->ParentChild(D23, M7);
		dag->ParentChild(D24, M8);
		dag->ParentChild(D25, M9);
		dag->ParentChild(D26, M10);
		dag->ParentChild(D27, M11);
		dag->ParentChild(D28, M12);
		dag->ParentChild(D29, M13);
		dag->ParentChild(D30, M14);
		dag->ParentChild(D31, M15);
		dag->ParentChild(D32, M16);

		dag->ParentChild(M1, d1);
		dag->ParentChild(M2, d2);
		dag->ParentChild(M3, d3);
		dag->ParentChild(M4, d4);
		dag->ParentChild(M5, d5);
		dag->ParentChild(M6, d6);
		dag->ParentChild(M7, d7);
		dag->ParentChild(M8, d8);
		dag->ParentChild(M9, d9);
		dag->ParentChild(M10, d10);
		dag->ParentChild(M11, d11);
		dag->ParentChild(M12, d12);
		dag->ParentChild(M13, d13);
		dag->ParentChild(M14, d14);
		dag->ParentChild(M15, d15);
		dag->ParentChild(M16, d16);

		dag->ParentChild(d1, O1);
		dag->ParentChild(d2, O2);
		dag->ParentChild(d3, O3);
		dag->ParentChild(d4, O4);
		dag->ParentChild(d5, O5);
		dag->ParentChild(d6, O6);
		dag->ParentChild(d7, O7);
		dag->ParentChild(d8, O8);
		dag->ParentChild(d9, O9);
		dag->ParentChild(d10, O10);
		dag->ParentChild(d11, O11);
		dag->ParentChild(d12, O12);
		dag->ParentChild(d13, O13);
		dag->ParentChild(d14, O14);
		dag->ParentChild(d15, O15);
		dag->ParentChild(d16, O16);
	}

	cout << "Bench2_InVitroDiag_5 CREATED" << endl;
	return dag;
}

///////////////////////////////////////////////////////////////////////////
// This benchmark details the DAG for the protein assay (based on Bradford
// reaction)detailed in Chakrabarty's benchmarks.
///////////////////////////////////////////////////////////////////////////
DAG *Test::Create_B3_Protein(double mult, int repeat)
{
	DAG *dag = new DAG();
	dag->setName("Protein");

	for (int i=0; i < repeat; i++)
	{
		vector<AssayNode*> db;
		vector<AssayNode*> dr;
		vector<AssayNode*> dlt;
		vector<AssayNode*> split;
		vector<AssayNode*> mix;
		vector<AssayNode*> det;
		vector<AssayNode*> out;

		// Create Protein Sample Dispense Node
		AssayNode * ds = dag->AddDispenseNode("DsS", 10, "ds");

		// Create Buffer Dispense Nodes
		db.push_back(NULL);
		for (int i = 1; i <= 39; i++)
		{
			char buff[12];
			sprintf(buff,"db%d", i);
			db.push_back(dag->AddDispenseNode("DsB", 10, buff));
		}

		// Create Reagent Dispense Nodes
		dr.push_back(NULL);
		for (int i = 1; i <= 8; i++)
		{
			char buff[12];
			sprintf(buff,"dr%d", i);
			dr.push_back(dag->AddDispenseNode("DsR", 10, buff));
		}

		// Create Dilute Nodes -- A dilute is a mixture followed by a split
		dlt.push_back(NULL);
		for (int i = 1; i <= 39; i++)
		{
			char buff[12];
			sprintf(buff,"dlt%d", i);
			dlt.push_back(dag->AddMixNode(2, 3*mult, buff));
		}
		split.push_back(NULL);
		for (int i = 1; i <= 7; i++)
		{
			char buff[12];
			sprintf(buff,"split%d", i);
			split.push_back(dag->AddSplitNode(false, 2, 2*mult, buff));
		}

		// Create Mix Nodes
		mix.push_back(NULL);
		for (int i = 1; i <= 8; i++)
		{
			char buff[12];
			sprintf(buff,"mix%d", i);
			mix.push_back(dag->AddMixNode(2, 3*mult, buff));
		}

		// Create Optical Detection Nodes
		det.push_back(NULL);
		for (int i = 1; i <= 8; i++)
		{
			char buff[12];
			sprintf(buff,"det%d", i);
			det.push_back(dag->AddDetectNode(1, 30*mult, buff));
		}

		// Create Output Nodes
		out.push_back(NULL);
		for (int i = 1; i <= 8; i++)
		{
			char buff[12];
			sprintf(buff,"out%d", i);
			out.push_back(dag->AddOutputNode("output", buff));
		}

		// Level 0
		dag->ParentChild(ds, dlt[1]);
		dag->ParentChild(db[1], dlt[1]);
		dag->ParentChild(dlt[1],split[1]);//Split
		//Level 1
		dag->ParentChild(db[2], dlt[2]);
		dag->ParentChild(split[1], dlt[2]);
		dag->ParentChild(split[1], dlt[3]);
		dag->ParentChild(db[3], dlt[3]);
		dag->ParentChild(dlt[2],split[2]);//Split
		dag->ParentChild(dlt[3],split[3]);//Split
		//Level 2
		dag->ParentChild(split[2],dlt[4]);
		dag->ParentChild(split[2],dlt[5]);
		dag->ParentChild(split[3],dlt[6]);
		dag->ParentChild(split[3],dlt[7]);
		//Level 3
		for (int i=4; i<=7; i++)
			dag->ParentChild(db[i], dlt[i]);
		//Level 4
		for (int i=4; i<=7; i++)
		{
			dag->ParentChild(dlt[i],split[i]);//Split
			dag->ParentChild(split[i],dlt[2*i]);
			dag->ParentChild(split[i],dlt[(2*i)+1]);
		}
		//Level 5-11
		for (int i=8; i<=39; i++)
			dag->ParentChild(db[i], dlt[i]);
		for (int i=8; i<=31; i++)
			dag->ParentChild(dlt[i], dlt[i+8]);
		//Level 12-15
		for (int i=1; i <= 8; i++)
		{
			dag->ParentChild(dlt[i+31],mix[i]);//12
			dag->ParentChild(dr[i],mix[i]);//13
			dag->ParentChild(mix[i],det[i]);//14
			dag->ParentChild(det[i],out[i]);//15
		}
	}

	cout << "Bench3_Protein CREATED" << endl;
	return dag;
}
