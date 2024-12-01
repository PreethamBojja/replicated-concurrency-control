#include "dataManager.h"
#include "valueType.h"
#include <stdexcept>
#include <iostream>

using namespace std;

DataManager::DataManager(int siteId){
    Id = siteId;
    isUp = true;
    for (int i = 1; i <= 20; ++i) {
        string key = "x" + to_string(i);
        accessible[key] = true; 
        int value = i * 10;               
        values[key] = new valueType(value, 0 , 0); // Assuming transactions start with ID 1  
        snapshots[key] = {new valueType(value, 0 , 0)};   
    }
}

void DataManager::setDown() {
    isUp = false;
}

void DataManager::setUp() {
    isUp = true;
}

bool DataManager::isUp() const {
    return isUp;
}

void DataManager::setVariableAccessible(string variable, bool accessible) {
    accessible[variable] = accessible;
}

bool DataManager::isVariableAccessible(string variable) const {
    auto it = accessible.find(variable);
    return (it != accessible.end()) ? it->second : false;
}

ValueType DataManager::read(string variable) {
    // TODO
}

void DataManager::commit(string variable, ValueType value, int transaction_id) {
    // TODO
}

void DataManager::addSnapshot(string variable, ValueType value) {
    snapshots[variable].push_back(value);
}

vector<ValueType> DataManager::getSnapshots(string variable) const {
    auto it = snapshots.find(variable);
    if (it != snapshots.end()) {
        return it->second;
    }
    return {};
}

map<string, ValueType> DataManager::getCurrentValues() const {
    return values;
}
