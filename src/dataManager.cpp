#include "dataManager.h"
#include <stdexcept>
#include <iostream>

using namespace std;

DataManager::DataManager(int site_id) : site_id(site_id), isUp(true) {}
// TODO : Intialize variables x1,x2...x20

void DataManager::setDown() {
    isUp = false;
}

void DataManager::setUp() {
    isUp = true;
}

bool DataManager::isUp() const {
    return isUp;
}

void DataManager::setVariableAccessible(const string& variable, bool accessible) {
    this->accessible[variable] = accessible;
}

bool DataManager::isVariableAccessible(const string& variable) const {
    auto it = accessible.find(variable);
    return (it != accessible.end()) ? it->second : false;
}

ValueType DataManager::read(const string& variable) {
    // TODO
}

void DataManager::commit(const string& variable, const ValueType& value, int transaction_id) {
    // TODO
}

void DataManager::addSnapshot(const string& variable, const ValueType& value) {
    snapshots[variable].push_back(value);
}

vector<ValueType> DataManager::getSnapshots(const string& variable) const {
    auto it = snapshots.find(variable);
    if (it != snapshots.end()) {
        return it->second;
    }
    return {};
}

map<string, ValueType> DataManager::getCurrentValues() const {
    return values;
}
