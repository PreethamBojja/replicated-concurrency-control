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
    bool is_site_up();
    ValueType read(string variable, int at_ts);
    void commit(string variable, ValueType value, int commit_ts);
    void take_snapshot(string variable);
};
