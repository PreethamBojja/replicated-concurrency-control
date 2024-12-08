#pragma once

#include <map>
#include <vector>
#include <string>
#include "valueType.h"

using namespace std;

class DataManager {
public:
    int id; // Site ID
    bool isUp; // Tracks the site status 
    map<string, bool> accessible; // Tracks if each variable is accessible
    map<string, ValueType> values; // Current values for variables 
    map<string, vector<ValueType> > snapshots; // Historical snapshots of variables

    DataManager(int site_id); // Constructor

    /*
        Author : Sai Preetham Bojja
        Description : Checks if the site is up
        Inputs : Null
        Outputs : True if the site is up, False otherwise
        Side Effects : No side effects
    */
    bool is_site_up();
    
    /*
        Author : Sai Preetham Bojja
        Description : Reads the value of the variable before the transaction start time.
        Inputs : Variable name, Transaction start time
        Outputs : Value of the variable
        Side Effects : No side effects
    */
    ValueType read(string variable, int at_ts);

    /*
        Author : Nikhil Kommineni
        Description : commits variable value to site
        Inputs : Variable name, variable value, Transaction start time
        Side Effects : updates value of variable at site
    */
    void commit(string variable, ValueType value, int commit_ts);

    /*
        Author : Nikhil Kommineni
        Description : takes snapshot of the variable
        Inputs : Variable name
        Side Effects : adds a snapshot of the variable to site
    */
    void take_snapshot(string variable);
};
