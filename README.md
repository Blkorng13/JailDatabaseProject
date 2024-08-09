Project: B+ Tree Implementation for a Jail Population Database
Introduction:
This project was chosen as the start of a long-term project to design and build a jail population database. I have worked with numerous local and state criminal justice agencies (local jails and prison systems) over the past 20 years. Many of these agencies, because of limited budgets, do not have up to date databases and/or staff to maintain a database, let alone analyze the data they contain to produce useful insights for agency stakeholders.

My long-term goal is to design and build a database that can be used by jails and prisons that is flexible enough to fit the nuances of many local justice systems and can provide a wide range of information and services. This project will be the foundation for this effort by implementing a B+ tree, associated functions for building and maintaining the tree, and one specialized function related to analysis of jail populations (this has not been coded as of submission).

I built it all in C++ simply because its the language I am most familiar with right now.

 Why a B+ Tree:
 B+ trees are often the "chosen" data structure for databases because they store data efficiently and perform operations such as insert, find, and delete with a time complexity of O(log n). This makes managing and storing large volumes of data more manageable and efficient. In particular, queries over a range of values are efficient, something a well-tooled jail database would do over many categories of variables. 

 B+ trees store data values in the leaf nodes, while internal nodes are only for indexing. Again, this is benefical for range query speed. The leaf nodes of a B+ tree are linked with neighboring leaf nodes similar to a linked list. This allows easy access across the actual data. In addition to leaf node linking, other B+ Tree invariants are:
1. B+ tree nodes have an order of n (maximum number of child nodes) and maximum key capacity of n -1. 
2. All leaf nodes must be at the same depth in the true. This is to ensure the tree remains balanced.
3. Each internal node only holds keys for indexing to the leaf nodes. Internal nodes must stay at least half full.
4. Keys in nodes are kept sorted. This is another plus for speedy searches.

Current Set-up:

The database inmate data is held in a structure:
Inamte ID (inmate_ID). This is the primary key for the B+ Tree
Booking date (bookingdate) - date the inamte was booked (YEAR-MO-DA). This is a secondary key and has its own tree for queries.
Booking time (bookingtime) - time of booking (HR:MN)
Released flag (inmate_released) - indicates the inmate was released from jail
Release date (releasedate) - date of release (YEAR-MO-DA)
Release time (releasetime) - time of release (HR:MN)
Gender (inmate_gender) - included for possible disaggregation queries (M/F/O)
Lenght of stay in jail in days (LengthofStay_days) calculated at release event

Classes (all defined in the header file):
1. B+ Tree node:
isLeaf - identifies leafs //moved this to the top b/c order does matter for initialization
keys - vector that holds the keys
childs - vecto to hold children pointers
inmate_data - vector of pointers to data
neigbor - leaf neighbor pointer
parent - pointer to parent node 

2. The B+ Tree:
This class contains functions to insert nodes, split nodes, insert data, and traverse the trees. There is no delete node function (I didn't even get close to making one).

3. Database:
This class defines the two trees and functions for adding an inmate, releasing an inmate, calculating length of stay. Ideally the leafs of both trees point to the same data. I have not tested if this is what is really going on.

Walk through:
Currently the most recent version is not building. The problem (I think) centers around my use of smart pointers. I chose to use them for the ease of memory deallocation and because I wanted to learn about them. When I first started coding I forgot to include the leaf neighbor pointers. I was pretty deep in when I realized this screw-up Once I realized this mistake I added these pointers and had to create a leaf split function which seems to be where my logic errors are occurring. The learning curve around using the smart pointer functions, calling the parent pointers in functions, etc. was steep and probably was an added complication that I was not ready for. I have yet to figure it all out.

You can look at the code for the current version contained in B+Tree_current.cpp.

An earlier version is in B+Tree_noleafconnect. This version you can execute to see some of the Database functions run which I already set up in the main function. It only partially accepts entering inmates so the trees are not correctly building. But I thought I would include it for the LOS calculation function when converts stings to dates and then makes the calculation. I am proud of that portion.






