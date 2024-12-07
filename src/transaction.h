#pragma once

#include <vector>
#include <queue>
#include <map>
#include <string>
#include "dataManager.h"
#include "operation.h"

using namespace std;

enum class TxnStatus {
    ACTIVE,
    COMMITTED,
    ABORTED
};

class Transaction {
public:
    int txnId; // transaction ID
    int start_ts; // start time of transaction
    int commit_ts; // commit time of transaction
    TxnStatus status; // status of transaction
    string reason_4_abort; // reason for aborting transaction
    map<string, pair<int, ValueType> > var_access_map; // to maintain the first access details of each variable, used for SSI and Available Copies checks
    map<string, bool> is_written;  // keep track of the variables written by the txn, useful at commit time
    map<string, bool> is_read; // keep track of the variables read by the txn, useful for SSI pre-commit checks
    map<Operation, vector<int> > active_sites_for_write_op;
    vector<Operation> past_operations; // past operations of this transaction 
    Operation* waiting_operation; // waiting operation
    map<string, int> current_state; // tracks current state of the transaction
    
    Transaction(int txnId, int startTime);
    // Core transaction operations
    void add_operation(Operation op);
    void set_waiting_operation(Operation* op);
    void commit(int timestamp);
    void abort(string reason);

};
