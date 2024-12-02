
#include <string>
#include <vector>
#include <stdexcept>
#include "operation.h"

using namespace std;

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
