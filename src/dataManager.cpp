#include "dataManager.h"
#include "valueType.h"
#include <stdexcept>
#include <iostream>
#include <string>

using namespace std;

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

bool DataManager::is_site_up() {
    return isUp;
}

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

void DataManager::commit(string var, ValueType value, int ts) {
    value.timestamp = ts;
    values[var] = value;
    take_snapshot(var);
}

void DataManager::take_snapshot(string variable) {
    snapshots[variable].push_back(values[variable]);
}

