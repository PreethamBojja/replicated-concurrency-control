#include "dataManager.h"
#include "valueType.h"
#include <stdexcept>
#include <iostream>
#include <string>

using namespace std;

// The DataManager class manages variable states, site status, and historical snapshots of data for a distributed database.
DataManager::DataManager(int site_id){
    id = site_id;
    isUp = true;
    vector<int> var_ids = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20};
    if (site_id % 2 == 0) {
        var_ids.push_back(site_id - 1);
        var_ids.push_back(10 + site_id - 1);
    }

    for (int var_id : var_ids) {
        string key = "x" + to_string(var_id);
        accessible[key] = true; 
        int value = var_id * 10;               
        values[key] = ValueType(value, 0 , 0); // Assuming transactions start with ID 1  
        snapshots[key].push_back(ValueType(value, 0 , 0));   
    }
    
}

// Checks if the site is up
bool DataManager::is_site_up() {
    return isUp;
}

// Reads the value of a variable as of the given timestamp (transaction start time)
ValueType DataManager::read(string variable, int at_ts) {
    ValueType result;
    vector<ValueType> snapshot = snapshots[variable];
    for (auto value : snapshot) {
        if (value.timestamp > at_ts) {
            return result;
        }
        result = value;
    }
    return result;
}

// Commits the specified value of a variable to the site at the given timestamp
void DataManager::commit(string var, ValueType value, int ts) {
    value.timestamp = ts;
    values[var] = value;
    take_snapshot(var);
}

// Captures the current state of the variable and stores it in historical snapshots
void DataManager::take_snapshot(string variable) {
    snapshots[variable].push_back(values[variable]);
}

