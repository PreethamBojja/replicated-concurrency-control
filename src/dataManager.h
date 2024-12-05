#pragma once

#include <map>
#include <vector>
#include <string>
#include "valueType.h"

using namespace std;

class DataManager {
public:
    int id;
    bool isUp;

    // Tracks if each variable is accessible
    map<string, bool> accessible;

    // Current values for variables
    map<string, ValueType> values;

    // Historical snapshots of variables
    map<string, vector<ValueType> > snapshots;

    DataManager(int site_id);

    bool is_site_up();

    ValueType read(string variable, int at_ts);
    
    void commit(string variable, ValueType value, int commit_ts);

    void take_snapshot(string variable);
};
