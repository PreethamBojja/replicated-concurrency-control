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
    OperationType op_type; // Type of operation
    int transactionId; // ID of the transaction
    string variable; // Target variable (for READ/WRITE operations)
    int value; // Value to be written (for WRITE operations)
    int timestamp; // Timestamp of the operation

    /*
        Author : Nikhil Kommineni
        Description : Constructor for the Operation class
        Inputs : Operation type, Transaction ID, Variable name, Value, Timestamp
        Outputs : Null
        Side Effects : Initializes the operation object
    */
    Operation(OperationType type, int tx_id, const string& var = "", int val = 0, int ts = 0)
        : op_type(type), transactionId(tx_id), variable(var), value(val), timestamp(ts) {}
    
    /*
        Author : Nikhil Kommineni
        Description : Operator overload for less than comparison
        Inputs : Another operation object
        Outputs : True if the current operation is less than the input operation, False otherwise
        Side Effects : No side effects
    */
    bool operator < (Operation const& b) const {
        return this->timestamp < b.timestamp;
    }

};

int extract_transaction_id(const string& txIdStr);
OperationType get_operation_type(const string& command);