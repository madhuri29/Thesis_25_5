#include "../Headers/qval.h"

QVal::QVal()
{
	p = 0;
	q = 0;
	gain = 0;
}

QVal::QVal(int a,int b, int g)
{
	p = a;
	q = b;
	gain = g;
}

QVal::~QVal()
{

}
