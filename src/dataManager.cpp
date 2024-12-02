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

void DataManager::site_down() {
    isUp = false;
}

void DataManager::site_up() {
    isUp = true;
}

bool DataManager::is_site_up() {
    return isUp;
}

void DataManager::set_access_flag(string variable, bool access_flag) {
    accessible[variable] = access_flag;
}

bool DataManager::is_accessible(string variable) {
    auto it = accessible.find(variable);
    return (it != accessible.end()) ? it->second : false;
}

bool DataManager::read(string variable, ValueType &dest) {
    // TODO
    if (!isUp) {
        return false;
    }

    ValueType val = values[variable];
    if (is_accessible(variable)) {
        dest.timestamp = val.getTimestamp();
        dest.txnId = val.getTransactionId();
        dest.value = val.getValue();
        return true;
    } else {
        return false;
    }
}

void DataManager::commit(string var_id, ValueType value, int ts) {
    // TODO
    value.timestamp = ts;
    values[var_id] = value;
}

void DataManager::take_snapshot(string variable) {
    snapshots[variable].push_back(values[variable]);
}

vector<ValueType> DataManager::getSnapshots(string variable) {
    auto it = snapshots.find(variable);
    if (it != snapshots.end()) {
        return it->second;
    }
    return {};
}

// map<string, ValueType> DataManager::getCurrentValues() const {
//     return values;
// }
