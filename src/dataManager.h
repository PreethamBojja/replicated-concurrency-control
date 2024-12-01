#pragma once

#include <map>
#include <vector>
#include <string>
#include "valueType.h"

using namespace std;

class DataManager {
private:
    static int Id;
    bool isUp;

    // Tracks if each variable is accessible
    map<string, bool> accessible;

    // Current values for variables
    map<string, ValueType> values;

    // Historical snapshots of variables
    map<string, vector<ValueType>> snapshots;

public:
    void setUp();
    bool isUp();

    void setVariableAccessible(string variable, bool accessible);
    bool isVariableAccessible(string variable);

    ValueType read(string variable);
    void commit(string variable, ValueType value, int transactionId);
};
