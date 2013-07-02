#include <iostream>
using namespace std;

#include "../Headers/point.hh"
#include <cmath>
double computeArea(Point &p, Point &q, Point &r);

int main()
{
	Point myPt1(2,3,8);
	Point myPt2(3,4,5);
    Point myPt3(4,5,6);
	//cout<<myPt1.distanceTo(myPt2)<<endl;
	cout<<computeArea(myPt1,myPt2,myPt3)<<endl;
	//cout<<"Pt2: ("<<myPt2.getX()<<","<<myPt2.getY()<<","<<myPt2.getZ()<<")"<<endl;
	//myPt1.setX(23);
	//myPt2.setZ(34);
	//cout<<"Pt1: ("<<myPt1.getX()<<","<<myPt1.getY()<<","<<myPt1.getZ()<<")"<<endl;
	//cout<<"Pt2: ("<<myPt2.getX()<<","<<myPt2.getY()<<","<<myPt2.getZ()<<")"<<endl;
	return 0;
}

double computeArea(Point &p, Point &q, Point &r) {
	double a = 0;
	double b = 0;
	double c = 0;
	double s = 0;
	double res = 0;
	a = p.distanceTo(q);
	b = q.distanceTo(r);
	c = r.distanceTo(p);

	cout<<"a="<<a<<endl;
	cout<<"b="<<b<<endl;
	cout<<"c="<<c<<endl;

	s = (a+b+c)/2;
	cout<<"s="<<s<<endl;
	if(s<a || s<b || s<c) {
		cout<<"Not a triangle";
	} else {
	res = sqrt(s*(s-a)*(s-b)*(s-c));
	cout<<"res="<<res<<endl;
	}
	return res;
}
