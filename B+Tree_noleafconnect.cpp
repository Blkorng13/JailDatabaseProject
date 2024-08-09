#include "JailDatabase.h"


//inmate construct
inmate::inmate(int id, const string& bookingDate, const string& bookingTime, bool released, const string& releaseDate, const string& releaseTime, char gender, float los)
    : inmate_id(id), bookingdate(bookingDate), bookingtime(bookingTime), inmate_released(released), 
      releasedate(releaseDate), releasetime(releaseTime), inmate_gender(gender), LengthofStay_days(los) {}

//Node constuctor
template <typename KeyV>
BPlustreenode<KeyV>::BPlustreenode(bool leafparam) : isLeaf(leafparam) {}
//BPtree construct
template <typename KeyV>
BPlustree<KeyV>::BPlustree() : root(make_shared<BPlustreenode<KeyV>>(true)) {}

//Split child function.
//Do first for insert (see Auric notes)
template <typename KeyV>
void BPlustree<KeyV>::splitchild(shared_ptr<BPlustreenode<KeyV>>& parent, int index)
{
    //grab full child
    auto fullchild = parent -> childs[index];
    //new child to hold split keys
    auto newchild = make_shared<BPlustreenode<KeyV>>(fullchild -> isLeaf);
    int mid = ORDER/2; //midpoint for split
    parent -> keys.insert(parent -> keys.begin() + index, fullchild -> keys[mid]); //put middle key in parent
    parent -> childs.insert(parent->childs.begin() + index + 1, newchild); //put ptr to new child in parent
    //put larger half of keys in new child
    newchild -> keys.assign(fullchild ->keys.begin() + mid + 1, fullchild->keys.end());
    //put larger half of data into new child
    newchild -> inmate_data.assign(fullchild -> inmate_data.begin() + mid + 1, fullchild->inmate_data.end());

    //not not a leaf move children to new child
    //use assign(vector) function instead of iterate
    if (!fullchild -> isLeaf)
    {
        newchild -> childs.assign(fullchild -> childs.begin() + mid + 1, fullchild-> childs.end());
    }
    //use resize to update old child for keys and data
    fullchild -> keys.resize(mid);
    fullchild -> inmate_data.resize(mid);
    //non leaf resize
    if (!fullchild ->isLeaf)
    {
        fullchild -> childs.resize(mid + 1);
    }
}


//Insert Function
template <typename KeyV>
void BPlustree<KeyV>::insert(const KeyV& key, const shared_ptr<inmate>& data)
{
    if(root -> keys.size() == ORDER - 1) //is root full?
    {
        //if yes make new root
        auto new_root = make_shared<BPlustreenode<KeyV>>(false); //new root no longer leaf
        new_root -> childs.push_back(root); // put root (now child) ptr into childs
        root = new_root; //update root
        splitchild(new_root, 0); //split function - old root into 2
    }
    insert_data(root, key, data); //insert function - > into
}
//Function to insert data into node !!NO TEST YET!!
template <typename KeyV>
void BPlustree<KeyV>::insert_data(shared_ptr<BPlustreenode<KeyV>>& node, const KeyV& key, const shared_ptr<inmate>& data)
{
    int index;
    if(node -> isLeaf) //data only goes into a leaf
    {
       //look for correct spot to ensure stays sorted
       //lower_bound <algorithm> does for you! (i start, i end, value used to search)
       auto val = lower_bound(node -> keys.begin(), node -> keys.end(), key);
       //distance function gives #keys btw start and new value
       int index = distance(node -> keys.begin(), val); //this is the vector index then insert
       node ->keys.insert(val, key);
       node -> inmate_data.insert(node -> inmate_data.begin() + index, data);
    }
    else //internal nodes
    {
        //find key greater than value...upper_bound function
        auto val = upper_bound(node -> keys.begin(), node -> keys.end(), key); //key > search key
        int index = distance(node -> keys.begin(), val); //index to correct child to descend
        //get child for that index
        shared_ptr<BPlustreenode<KeyV>> child = node -> childs[index];
        if(child -> keys.size() == ORDER -1) //is child full?
        {
            splitchild(node, index);
            if(key > node -> keys[index]) { index++; } //update index for key if split
        }
    }
    if (index < node ->childs.size()) //is index working
    {
        insert_data(node -> childs[index], key, data); //recursion...should be root?
    }
}
//Traverse every leaf !!NO TEST!!
template <typename KeyV>
void BPlustree<KeyV>::traverseAll(shared_ptr<BPlustreenode<KeyV>> node, vector<shared_ptr<inmate>>& results) const {
    if (node->isLeaf) 
    {
        // Collect all data from this leaf node
        for (const auto& inmate : node->inmate_data) {
            results.push_back(inmate);
        }
    } 
    else
    {
        // do all child nodes
        for (size_t i = 0; i < node ->keys.size(); ++i)
        {
            traverseAll(node -> childs[i], results);
        }
        // do the last child node
        traverseAll(node -> childs.back(), results);
    }
}

// Traverse Function based on start/end dates 
//define results before function call
template <typename KeyV>
void BPlustree<KeyV>::traverseleafnodes(shared_ptr<BPlustreenode<KeyV>> node, const string& start, const string& end, vector<shared_ptr<inmate>>& results) const {
    if (node -> isLeaf) //is leaf?
    {
        for (size_t i = 0; i < node -> keys.size(); ++i) //pass thru keys in leaf
        {
            if (node -> keys[i] >= start && node -> keys[i] <= end) //check dates
            {
                results.push_back(node -> inmate_data[i]); //if yes add to results
            }
        }
    } 
    else //not leaf
    {
        for (size_t i = 0; i < node ->keys.size(); ++i) //move thru children
        {
            if (end < node -> keys[i]) //go down first
            {
                traverseleafnodes(node -> childs[i], start, end, results);
                break;
            }
        }
        traverseleafnodes(node ->childs.back(), start, end, results); //go down last
    }
}

// Search by ID 
template <typename KeyV>
shared_ptr<inmate> BPlustree<KeyV>::searchforinmate(const KeyV& key) const
{
    shared_ptr<BPlustreenode<KeyV>> node = root; //set search node to root for start
    while (node) { //go down until hit a leaf
        if (node ->isLeaf) //found leaf
        { 
            auto val = find(node -> keys.begin(), node -> keys.end(), key); //find funct locate key set to val
            if (val != node -> keys.end()) //if key is in keys
            {
                int index = distance(node -> keys.begin(), val); //mark index
                return node ->inmate_data[index]; //inmate found return
            }
            return nullptr; //not found exit
        } 
        else //not a leaf - look for correct child
        {
            auto val = upper_bound(node->keys.begin(), node->keys.end(), key); //set val to first key > key(looking for)
            int index = distance(node->keys.begin(), val); //use val to pick correct child
            node = node -> childs[index]; //search continues down that child
        }
    }
    return nullptr; //not found
}


//Database functions
Database::Database() : inmateIDTree(), bookingdateTree() {}

// Count Bookings Function
int Database::countbookings(const string& start, const string& end) const
{
    vector<shared_ptr<inmate>> results; //def vector to store keys
    //traverse won't work WITHOUT results
    bookingdateTree.traverseleafnodes(bookingdateTree.root, start, end, results); //Traverse and collect
    return results.size();  
}

//Find the average LOS for persons btw 2 dates
float Database::AverageLOS(const string& start, const string& end) const {
    vector<shared_ptr<inmate>> results; //def vector to store
    bookingdateTree.traverseleafnodes(bookingdateTree.root, start, end, results);

    float totalLOS = 0;
    int count = 0;

    for (const auto& inmatePtr : results) //go through result vector
    {
        if (inmatePtr ->inmate_released) //only want released
        {
            totalLOS += inmatePtr -> LengthofStay_days; //add LOS to total for avg calc
            count++;
        }
    }
    //do avg calc
    if (count != 0) {return totalLOS / count;}
    else {return 0;}

}


// Calculate Length of Stay Function (WORKING)
//<ctime>
float Database::calculateLOS(const string& bookingDate, const string& releaseDate) const {
    //initialize for both tm for book&rel
    //tm has all parts: Y/M/D/H/Min/Sex
    tm booktm = {};
    tm reltm = {};
    //set istringstream on dates
    istringstream bookss(bookingDate); //<sstream> Lets  you read string
    istringstream relss(releaseDate);
    //get_time in <iomanip> extracts dates
    bookss >> get_time(&booktm, "%Y-%m-%d");
    relss >> get_time(&reltm, "%Y-%m-%d");
    //mktime is conversion function from date to number of secs from preset time
    time_t btime = mktime(&booktm);
    time_t rtime = mktime(&reltm);
    //calculate diff and convert to days from secs
    double difference = difftime(rtime, btime) / (60 * 60 * 24);
    //return -> convert double tofloat
    return static_cast<float>(difference);
}

// Release inamte Function (WorkS)
void Database::ChangeReleaseInfo(int inmateId, const string& releaseDate, const string& releaseTime)
{
    auto inmate_ptr = searchByID(inmateId);
    if (inmate_ptr) 
    {
        inmate_ptr ->inmate_released = true;
        inmate_ptr ->releasedate = releaseDate;
        inmate_ptr ->releasetime = releaseTime;
        inmate_ptr -> LengthofStay_days = calculateLOS(inmate_ptr -> bookingdate, releaseDate);
    }
}
//book new inamte (WORKS)
void Database::addinmate(const shared_ptr<inmate>& inmate_ptr) 
{
    inmateIDTree.insert(inmate_ptr -> inmate_id, inmate_ptr);
    bookingdateTree.insert(inmate_ptr -> bookingdate, inmate_ptr);
}
//database search function
shared_ptr<inmate> Database::searchByID(int id) const 
{
    return inmateIDTree.searchforinmate(id);
}

//print list of bookings (WORKS)
void Database::printlistofinmates() const
{
    vector<shared_ptr<inmate>> results;
    inmateIDTree.traverseAll(inmateIDTree.root, results);

    for (const auto& inmate_ptr : results) {
        cout << "Inmate ID: " << inmate_ptr ->inmate_id << endl;
        if (inmate_ptr -> inmate_released)
        {
            cout << "Released: " << "Yes" << endl;
            cout << "Length of Stay (days): " << inmate_ptr -> LengthofStay_days << endl;
        }
        else
        {
            cout << "Released: " << "No" << endl;
        }
        cout << "Gender: " << inmate_ptr -> inmate_gender << endl;
        cout << "------------------------" << endl;
    }
}

int main() {

        // Make trees
    BPlustree<int> inmateIDTree;
    BPlustree<string> bookingdateTree;
    Database jail;

    // Test cases 
    vector<shared_ptr<inmate>> testCases = {

        make_shared<inmate>(1, "2024-01-01", "12:00", true, "2024-01-10", "09:00", 'M', 9),
        make_shared<inmate>(2, "2024-02-01", "14:30", true, "2024-02-15", "10:00", 'F', 14),
        make_shared<inmate>(3, "2024-03-01", "09:15", false, "", "", 'M', 0), 
        make_shared<inmate>(4, "2024-04-01", "11:45", true, "2024-04-10", "08:00", 'O', 9),
        make_shared<inmate>(5, "2024-05-01", "15:00", true, "2024-05-20", "12:00", 'F', 19),
  
        make_shared<inmate>(6, "2024-06-01", "07:30", true, "2024-06-12", "06:00", 'M', 11),
        make_shared<inmate>(7, "2024-07-01", "08:00", true, "2024-07-15", "07:30", 'F', 14),
        make_shared<inmate>(8, "2024-08-01", "09:00", false, "", "", 'M', 0),
        make_shared<inmate>(9, "2024-09-01", "10:30", true, "2024-09-10", "09:00", 'O', 9),
        make_shared<inmate>(10, "2024-10-01", "11:00", true, "2024-10-20", "10:00", 'F', 19),

        make_shared<inmate>(11, "2024-11-01", "12:00", true, "2024-11-12", "11:00", 'M', 11),
        make_shared<inmate>(12, "2024-12-01", "13:00", true, "2024-12-15", "12:30", 'F', 14),
        make_shared<inmate>(13, "2024-01-15", "14:00", false, "", "", 'M', 0),
        make_shared<inmate>(14, "2024-02-15", "15:00", true, "2024-02-28", "13:00", 'O', 13),
        make_shared<inmate>(15, "2024-03-15", "16:00", true, "2024-03-30", "14:00", 'F', 15),

        make_shared<inmate>(16, "2024-04-15", "17:00", true, "2024-04-28", "15:00", 'M', 13),
        make_shared<inmate>(17, "2024-05-15", "18:00", true, "2024-05-31", "16:00", 'F', 16),
        make_shared<inmate>(18, "2024-06-15", "19:00", false, "", "", 'M', 0),
        make_shared<inmate>(19, "2024-07-15", "20:00", true, "2024-07-31", "17:00", 'O', 16),
        make_shared<inmate>(20, "2024-08-15", "21:00", true, "2024-08-31", "18:00", 'F', 16),

        make_shared<inmate>(21, "2024-09-15", "22:00", true, "2024-09-30", "19:00", 'M', 15),
        make_shared<inmate>(22, "2024-10-15", "23:00", true, "2024-10-31", "20:00", 'F', 16),
        make_shared<inmate>(23, "2024-11-15", "00:00", false, "", "", 'M', 0),
        make_shared<inmate>(24, "2024-12-15", "01:00", true, "2024-12-31", "21:00", 'O', 16),
        make_shared<inmate>(25, "2024-01-10", "02:00", true, "2024-01-20", "22:00", 'F', 10)
    };

    // Add test cases to the database
    for (const auto& inmate_ptr : testCases) {
        jail.addinmate(inmate_ptr);
    }

    cout << "Running?" << endl;

    jail.printlistofinmates();
    //test release inmate
    int id = 3;
    string reldate = "2024-04-10";
    string reltime = "18:30";
    jail.ChangeReleaseInfo(id, reldate, reltime);

    jail.printlistofinmates();


    return 0;
}