#ifndef JAILDATABASE_H
#define JAILDATABASE_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream> //for get_time function -> reads dates
#include <ctime> //date and LOS
#include <memory> //smart pointers and their functions
#include <cmath> //for LOS calc
#include <algorithm> //lower_bound function
#include <iomanip> //Date and LOS


using namespace std;

//Baseline data storage for each inmate booked into the jail
struct inmate {
    int inmate_id; //assigned id number...sequential int here starting at 1...primary key
    //C++ will order dates set as YEAR-MO-DA correctly in B+tree.
    string bookingdate; //booking date (YEAR-MO-DA)
    string bookingtime; //booking time (HR:MN)
    bool inmate_released; //changed at release event
    string releasedate; //release date (YEAR-MO-DA)
    string releasetime; //release time (HR:MN)
    char inmate_gender; //gender...for possible disagg query (M/F/O)
    float LengthofStay_days; //LOS in jail in days...calculated at release event

    inmate(int id, const string& bookingDate, const string& bookingTime, bool released, 
           const string& releaseDate, const string& releaseTime, char gender, float los);
};



//Node class definition. This will be used by both inmate ID and booking date trees
//Inmate ID tree is for individual searches and booking date tree is for range query
//template suggested by Auric...you don't have to code trees for each key
//typename is like auto...depends on key type used
template <typename KeyV> //KeyV is version of key being input (Primary or secondrary)
class BPlustreenode {
public:
    //Constructor function when initialize leaf
    BPlustreenode(bool leafparam = false);
    bool isLeaf; //moved this to the top b/c order does matter for initialization
    vector<KeyV> keys; //vector that holds the keys
    //declare of vector of smart pointers of child nodes...shared_ptr declare must have type of object it points to
    vector<shared_ptr<BPlustreenode<KeyV>>> childs;
    //declare vector holding pointers to the actual data
    vector<shared_ptr<inmate>> inmate_data;  //holds data in leafs

    shared_ptr<BPlustreenode<KeyV>> neighbor; //leaf point to neighbor
    weak_ptr<BPlustreenode<KeyV>> parent; //addition of parent nodes (fingers crossed)
    //weak used for parents (Smart noes)
};

//Order definition
//constexpr is const that can be eval at compile time (more efficient than HW way)
constexpr unsigned int ORDER = 6;


//B+ tree class def
template <typename KeyV>
class BPlustree {
public:
    //Constuctor 
    BPlustree();

    //insert function declare (will do both trees at once)
    //params are const so no modified...referenced for memory saving...don't have to copy (see C++ function notes)
    void insert(const KeyV& key, const shared_ptr<inmate>& data);
    //delete funct declare - pending
//  void delete(const KeyV& key);
    //search for an inmate by ID declare
    shared_ptr<inmate> searchforinmate(const KeyV& key) const; // fixed search function
    //range query funct declare - count
    //start=first dte, end=last date
    int count_inmates(const string& start, const string& end) const;
    //range query funct declare - calcs averge LOS
    void traverseAll(shared_ptr<BPlustreenode<KeyV>> node, vector<shared_ptr<inmate>>& results) const;
    

private:
    //declare pointer to root. Shared for auto delete...will happen a lot with insert/delete
    shared_ptr<BPlustreenode<KeyV>> root;
    //declare inmate insert based on key
    //node = pointer to tree node in vector id'd by keyV
    //data = pointer to inmate data
    //will insert into a full node thats not full
    void insert_data(shared_ptr<BPlustreenode<KeyV>>& node, const KeyV& key, const shared_ptr<inmate>& data);
    //declare split node funct for internal
    //Updates parent, creates new node, divy up keys, creates link btw leaves
    //parent = ptr to parent node of child to be split
    //index = index of child to be split in parent vector of ptrs
    void splitchild(shared_ptr<BPlustreenode<KeyV>>& parent, int index);
    //split leaf
    void split_leaf(shared_ptr<BPlustreenode<KeyV>>& leaf);
    //declare traverse funct - for leaves secondary key
    void traverseleafnodes(shared_ptr<BPlustreenode<KeyV>> node, const string& start, const string& end, vector<shared_ptr<inmate>>& results) const;
    //traverse by inmate ID
    friend class Database;  // Declare Database as a friend class
};

//Define database class
class Database {
public:
    //constructor
    Database();
    //add inmate function declare
    void addinmate(const shared_ptr<inmate>& inmateptr);
    shared_ptr<inmate> searchByID(int id) const; // Calls BPlustree::search
    //function to count the # of bookings btw 2 dates
    //returns #
    int countbookings(const string& start, const string& end) const;
    //calcs LOS
    float calculateLOS(const string& bookingDate, const string& releaseDate) const;
    //function to return average LOS for persons released btw 2 dates
    //returns ALOS (float)
    float AverageLOS(const string& start, const string& end) const;
    //function for user to release an inmate. Calcs LOS
    void ChangeReleaseInfo(int inmateId, const string& releaseDate, const string& releaseTime);
    // Print all inmate data
    void printlistofinmates() const;

private:
    //Declare the 2 trees
    BPlustree<int> inmateIDTree; //primo
    BPlustree<string> bookingdateTree; //secondo
};



#endif // JAILDATABASE_H