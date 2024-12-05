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
    
    bool operator < (Operation const& b) const {
        return this->timestamp < b.timestamp;
    }

};

int extract_transaction_id(const string& txIdStr);
OperationType get_operation_type(const string& command);