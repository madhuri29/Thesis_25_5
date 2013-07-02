#include "../Headers/matrix.h"
/////////////////////////////////////////////////////////////////////////
//Constructor
/////////////////////////////////////////////////////////////////////////
Matrix::Matrix()
{
	numNodes = 0;
	numEdges = 0;
}

Matrix::Matrix(int nodes , int edges)
{
	numNodes = nodes;
	numEdges = edges;

	for (int i=0; i <= numNodes; i++){
		mat.push_back(vector<Edge>());
	}
}
/////////////////////////////////////////////////////////////////////////
//Destructor
/////////////////////////////////////////////////////////////////////////
Matrix::~Matrix()
{

}
/////////////////////////////////////////////////////////////////////////
//binary search for edge , return -1 if it does not exist
/////////////////////////////////////////////////////////////////////////
int Matrix::binarySearchMat(vector<Edge> &thisvector, int searchId, int low, int high)
{
	int mid = low + (high - low)/2;
	int midId;
	int retval;
	if(high < low) {
		return -1;
	}

	midId = thisvector[mid].node_id;
	if(midId > searchId) {
		retval = Matrix::binarySearchMat(thisvector, searchId, low, mid-1);
	} else if (midId < searchId) {
		retval = Matrix::binarySearchMat(thisvector, searchId, mid+1, high);
	} else {
		retval = mid;
	}
	return retval;
}
/////////////////////////////////////////////////////////////////////////
//returns edge index in Matrix if it exists. if not , returns -1
/////////////////////////////////////////////////////////////////////////
int Matrix::edgeExists(int node1Id,int node2Id )
{
	//do binary search in vector of edges of node1Id for node2Id
	int pos = 0;
	pos = binarySearchMat(mat[node1Id],node2Id, 0 , (mat[node1Id].size()-1));
	//return -1 if edge does not exist. else return the position
	return pos;
}
/////////////////////////////////////////////////////////////////////////
//for Sort function
/////////////////////////////////////////////////////////////////////////
bool cmpFunction(Edge i, Edge j)
{
	return (i.node_id < j.node_id);
}
/////////////////////////////////////////////////////////////////////////
//add edge to matrix if it doesnt exist. else update weight.
/////////////////////////////////////////////////////////////////////////
void Matrix::addEdge (int node1Id,int node2Id )
{
	int pos = 0;
	stringstream str;
	str <<"Invalid Node Id"<<endl;

	claim(node1Id<=numNodes, &str );
	claim(node2Id<=numNodes, &str );
	pos = edgeExists(node1Id,node2Id);
	if (pos == -1){
		//create a new edge and add to vectors, sort by id
		Edge Edge1(node2Id,1);
		mat[node1Id].push_back(Edge1);
		sort(mat[node1Id].begin(), mat[node1Id].end(), cmpFunction);

		Edge Edge2(node1Id,1);
		mat[node2Id].push_back(Edge2);
		sort(mat[node2Id].begin(), mat[node2Id].end(), cmpFunction);
	} else {
		//update weight
		mat[node1Id][pos].weight++;
		pos = edgeExists(node2Id,node1Id);
		mat[node2Id][pos].weight++;
	}

}
/////////////////////////////////////////////////////////////////////////
//returns weight of edge
/////////////////////////////////////////////////////////////////////////
int Matrix::getWeight (int node1Id,int node2Id)
{
	//return weight of node1-node2 edge
	int pos=0;
	pos = edgeExists(node1Id, node2Id);
	claim(pos >= 0,"Edge does not exist");
	return mat[node1Id][pos].weight;
}
/////////////////////////////////////////////////////////////////////////
//print connectivity matrix
/////////////////////////////////////////////////////////////////////////
void Matrix::printMatrix()
{
	for(size_t i = 1 ; i < mat.size() ; ++i )
	{
		cout<<"Node"<<i<<":";
		for(size_t j = 0 ; j < mat[i].size() ; ++j)
		{
			cout<< mat[i][j].node_id<<":"<<mat[i][j].weight<<" ";
		}
		cout<<endl;
	}
}
/////////////////////////////////////////////////////////////////////////
//sum of all weights in matrix / 2 = numEdges
/////////////////////////////////////////////////////////////////////////
void Matrix::checkConnectivityMatrix()
{
	int sum = 0;
	for(size_t i = 1 ; i < mat.size() ; ++i )
	{
		for(size_t j = 0 ; j < mat[i].size() ; ++j)
		{
			sum += mat[i][j].weight;
		}
	}

	claim(sum/2 == numEdges,"Matrix check failed");
	cout<<"Matrix is correct.. can proceed"<<endl;
}
/////////////////////////////////////////////////////////////////////////
//returns a vector of Edges : the neighbors of given ID
/////////////////////////////////////////////////////////////////////////
vector<int> Matrix::getNeighbors(int nodeId)
{
	vector<int> ret_vec;
	for(int i = 0 ; i < mat[nodeId].size(); i++){
		ret_vec.push_back(mat[nodeId][i].node_id);
	}

	for(int i = 0; i < ret_vec.size(); i++)
		cout<<ret_vec[i];
	return ret_vec;
}
