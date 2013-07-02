#include "../Headers/edge.h"

Edge::Edge()
{
	node_id = 0;
	weight = 0;
}

Edge::Edge(int id, int wt)
{
	node_id = id;
	weight = wt;
}

Edge::~Edge()
{

}
