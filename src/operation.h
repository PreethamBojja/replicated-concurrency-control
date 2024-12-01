#pragma once

#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

enum OperationType {
    BEGIN, END, READ, WRITE, FAIL, RECOVER, DUMP, UNKNOWN
};

class Operation {
public:
    OperationType op_type;
    int transactionId;
    string variable;
    int value;
    int timestamp;

    Operation(OperationType type, int tx_id, const string& var = "", int val = 0, int ts = 0)
        : op_type(type), transactionId(tx_id), variable(var), value(val), timestamp(ts) {}
};

// Function to map command strings to OperationType enums
OperationType getOperationType(const string& command) {
    if (command == "begin") return BEGIN;
    if (command == "end") return END;
    if (command == "R") return READ;
    if (command == "W") return WRITE;
    if (command == "fail") return FAIL;
    if (command == "recover") return RECOVER;
    if (command == "dump") return DUMP;
    return UNKNOWN;
}

// Function to extract transaction ID from string
// TODO :  Handle corner cases
int extractTransactionId(const string& txIdStr) {
    if (txIdStr.size() > 1 && txIdStr[0] == 'T') {
        try {
            return stoi(txIdStr.substr(1)); 
        } catch (const invalid_argument& e) {
            return -1;
        }
    }
    return -1;
}
