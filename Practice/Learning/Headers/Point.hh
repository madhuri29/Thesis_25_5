// A 3D point class!
 class Point {

 private:
     // Coordinates of the point.
     double x_coord;
     double y_coord;
     double z_coord;

 public:
     // Constructors
     Point();				// Default constructor
     Point(double x, double y, double z);		// Two-argument constructor

     // Destructor
     ~Point();

     // Accessor methods
     double getX();
     double getY();
     double getZ();

     // Mutator methods
     void setX(double val);
     void setY(double val);
     void setZ(double val);

     //Methods
     double distanceTo(Point &pt);
 };
