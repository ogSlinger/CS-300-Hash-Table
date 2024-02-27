//============================================================================
// Name        : HashTable.cpp
// Author      : Derek Spaulding
// Version     : 1.0
// Copyright   : Copyright © 2024 SNHU COCE
// Description : Hash Table CSV Parser
//============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <string> // atoi
#include <time.h>
#include <cmath>

#include "CSVparser.hpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 179;

// forward declarations
double strToDouble(string str, char ch);

// define a structure to hold bid information
struct Bid {
    string bidId; // unique identifier
    string title;
    string fund;
    double amount;
    Bid() {
        amount = 0.0;
    }
};

//I added prime functions while resizing vectors
bool isPrime(int num) {
    if (num <= 1) return false;
    for (int i = 2; i <= std::sqrt(num); ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

int getNextPrime(int n) {
    n *= 2;
    while (!isPrime(n)) {
        ++n;
    }
    return n;
}

//============================================================================
// Hash Table class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:

    // Define structures to hold bids
    struct Node {
        Bid bid;
        unsigned int key;
        bool emptyAtStart;
        bool emptyAfterRemoval;

        // default constructor
        Node() {
            key = UINT_MAX;
            emptyAtStart = true;
            emptyAfterRemoval = false;
        }

        // initialize with a bid
        Node(Bid aBid) : Node() {
            bid = aBid;
            emptyAtStart = false;
            emptyAfterRemoval = false;
        }

        // initialize with a bid and a key
        Node(Bid aBid, unsigned int aKey) : Node(aBid) {
            key = aKey;
        }
    };

    vector<Node*> nodes;
    unsigned int tableSize = DEFAULT_SIZE;
    unsigned int hash(int key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Bid bid);
    void PrintAll();
    void Remove(string bidId);
    void resizeHashTable();
    Bid Search(string bidId);
};

/**
 * Default constructor
 */
HashTable::HashTable() {
    // FIXME (1): Initialize the structures used to hold bids
    
    // Initalize node structure by resizing tableSize
    this->nodes.resize(this->tableSize);
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(unsigned int size) {
    // invoke local tableSize to size with this->
    // resize nodes size

    this->tableSize = size;
    this->nodes.resize(size);
}


/**
 * Destructor
 */
HashTable::~HashTable() {
    // FIXME (2): Implement logic to free storage when class is destroyed
    
    // erase nodes beginning
    for (int i = 0; i < nodes.size(); i++) {
        if (this->nodes.at(i) != nullptr) {
            delete this->nodes.at(i);
        }
    }
}

/**
 * Calculate the hash value of a given key.
 * Note that key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index.
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(int key) {
    // FIXME (3): Implement logic to calculate a hash value
    // return key tableSize
    if (key < 0) {
        key = key * -1;
    }

    key = (key % this->nodes.size()) - 1;
    return key;
}

/**
 * Insert a bid
 *
 * @param bid The bid to insert
 */
void HashTable::Insert(Bid bid) {

    unsigned int key = this->hash(std::stoi(bid.bidId));
    Node* nodeAtKey = this->nodes.at(key);

    if (nodeAtKey == nullptr) {
        Node* newNode = new Node(bid, key);
        this->nodes.at(key) = newNode;
    }

    // else if node is not used
    else if (nodeAtKey->emptyAtStart) {
        nodeAtKey->key = key;
        nodeAtKey->emptyAtStart = false;
        nodeAtKey->emptyAfterRemoval = false;
        nodeAtKey->bid = bid;       
    }

    // else find the next open node
    else {
        unsigned int startIndex = key;
        size_t size = this->nodes.size();
        while (this->nodes.at(key) != nullptr) {
            if (this->nodes.at(key)->emptyAtStart) {
                break;
            }
            key++;
            if (key >= size - 1) {
                key = 0;
            }
            if (key == startIndex) {
                this->resizeHashTable();
                size = this->nodes.size();
            }
        }
        // add new newNode to end
        
        if (this->nodes.at(key) == nullptr) {
            Node* newNode = new Node(bid, startIndex);
            this->nodes.at(key) = newNode;
        }
        else if (this->nodes.at(key)->emptyAtStart) {
            nodeAtKey->key = key;
            nodeAtKey->emptyAtStart = false;
            nodeAtKey->emptyAfterRemoval = false;
            nodeAtKey->bid = bid;
        }
    }          
}

/**
 * Print all bids
 */
void HashTable::PrintAll() {
    // FIXME (6): Implement logic to print all bids
    // for node begin to end iterate
    Node* currNode = nullptr;
    for (int i = 0; i < this->nodes.size(); i++) {
        currNode = this->nodes.at(i);
        if (this->nodes.at(i) != nullptr) {
            if (currNode->key != UINT_MAX) {
                std::cout << "Key: " << currNode->key << " || "
                    << "bidID: " << currNode->bid.bidId << " || "
                    << "title: " << currNode->bid.title << " || "
                    << "amount: " << currNode->bid.amount << " || "
                    << "fund: " << currNode->bid.fund << std::endl;
            }
        }
    }
}

/**
 * Remove a bid
 *
 * @param bidId The bid id to search for
 */
void HashTable::Remove(string bidId) {
    unsigned int hashKey = this->hash(std::stoi(bidId));
    unsigned int startKey = hashKey;

    while (true) {
        if (this->nodes.at(hashKey) != nullptr) {
            if (this->nodes.at(hashKey)->bid.bidId == bidId) {
                break;
            }

            if (hashKey == this->nodes.size() - 1) {
                hashKey = 0;
            }
            else {
                hashKey++;
            }

            if (hashKey == startKey) {
                std::cout << "Removal Fialed! Bid ID not found.";
                return;
            }
        }
        else {
            std::cout << "Removal Fialed! Bid ID not found.";
            return;
        }
    }

    std::cout << "DELETING " << this->nodes.at(hashKey)->bid.bidId << std::endl;
    delete this->nodes.at(hashKey);
    Node* newNode = new Node();
    newNode->emptyAfterRemoval = true;
    this->nodes.at(hashKey) = newNode;
}

/**
* Auto Resize Hash Table
*/
void HashTable::resizeHashTable(){
    size_t size = 0;
    
    //Check how many actual non-dead nodes there are and removes them
    //Also counts how many elements are real
    for (int i = 0; i < this->nodes.size(); i++) {
        if (this->nodes.at(i) != nullptr) {
            if (this->nodes.at(i)->emptyAfterRemoval) {
                delete this->nodes.at(i);
                nodes.at(i) = nullptr;
            }
            else if (!(this->nodes.at(i)->emptyAtStart)) {
                size++;
            }
        }
    }

    size = getNextPrime(size); //getNextPrime returns prime after n^2
    std::vector<Node*> newVector(size); 
    size_t hashKey;

    for (int i = 0; i < this->nodes.size(); i++) {
        if (this->nodes.at(i) != nullptr) {
            hashKey = hash(std::stoi(this->nodes.at(i)->bid.bidId));
            newVector.at(hashKey) = this->nodes.at(i);
        }
    }

    this->nodes = newVector;
}

/**
 * Search for the specified bidId
 *
 * @param bidId The bid id to search for
 */
Bid HashTable::Search(string bidId) {
    Bid bid;

    unsigned int hashKey = this->hash(std::stoi(bidId));
    unsigned int startKey = hashKey;

    while (this->nodes.at(hashKey)->bid.bidId != bidId) {
        if (this->nodes.at(hashKey) != nullptr) {
            if (hashKey == this->nodes.size() - 1) {
                hashKey = 0;
            }
            else {
                hashKey++;
            }

            if ((hashKey == startKey) || this->nodes.at(hashKey)->emptyAtStart) {
                std::cout << "Search Fialed! Returning Empty Bid." << std::endl;
                return bid;
            }
        }
    }

    bid = this->nodes.at(hashKey)->bid;
    return bid;
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the bid information to the console (std::out)
 *
 * @param bid struct containing the bid info
 */
void displayBid(Bid bid) {
    std::cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
            << bid.fund << std::endl;
    return;
}

/**
 * Load a CSV file containing bids into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the bids read
 */
void loadBids(string csvPath, HashTable* hashTable) {
    std::cout << "Loading CSV file " << csvPath << std::endl;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    // read and display header row - optional
    vector<string> header = file.getHeader();
    for (auto const& c : header) {
        std::cout << c << " | ";
    }
    std::cout << "" << std::endl;

    try {
        // loop to read rows of a CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of bids
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

            // push this bid to the end
            hashTable->Insert(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {

    // process command line arguments
    string csvPath, bidKey;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        bidKey = "98109";
        break;
    case 3:
        csvPath = argv[1];
        bidKey = argv[2];
        break;
    default:
        csvPath = "eBid_Monthly_Sales_Dec_2016.csv";
        bidKey = "98223";
    }

    // Define a timer variable
    clock_t ticks;

    // Define a hash table to hold all the bids
    HashTable* bidTable;

    Bid bid;
    bidTable = new HashTable();
    
    int choice = 0;
    while (choice != 9) {
        std::cout << "Menu:" << std::endl;
        std::cout << "  1. Load Bids" << std::endl;
        std::cout << "  2. Display All Bids" << std::endl;
        std::cout << "  3. Find Bid" << std::endl;
        std::cout << "  4. Remove Bid" << std::endl;
        std::cout << "  9. Exit" << std::endl;
        std::cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
            
            // Initialize a timer variable before loading bids
            ticks = clock();

            // Complete the method call to load the bids
            try {
                loadBids(csvPath, bidTable);
            }
            catch (const std::exception& e) {
                cout << "Failed to load bids: " << e.what() << std::endl;
            }
            
            cout << "Bids loaded";

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << std::endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << std::endl;
            break;

        case 2:
            bidTable->PrintAll();
            break;

        case 3:
            ticks = clock();

            bid = bidTable->Search(bidKey);

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
                std::cout << "Bid Id " << bidKey << " not found." << std::endl;
            }

            std::cout << "time: " << ticks << " clock ticks" << std::endl;
            std::cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << std::endl;
            break;

        case 4:
            bidTable->Remove(bidKey);
            break;
        }
    }

    std::cout << "Good bye." << std::endl;

    return 0;
}
