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
    int txnId;
    int start_ts;
    int commit_ts;
    TxnStatus status;
    string reason_4_abort;

    // to maintain the first access details of each variable
    // used for SSI and Available Copies checks
    map<string, pair<int, ValueType> > var_access_map;
    // keep track of the variables written by the txn, useful at commit time
    map<string, bool> is_written;
    // keep track of the variables read by the txn, useful for SSI pre-commit checks
    map<string, bool> is_read;

    map<Operation, vector<int> > active_sites_for_write_op;

    vector<Operation> past_operations;
    
    Operation* waiting_operation;

    // Track transaction state and access
    map<string, int> current_state;

    // put accessMap in Transaction Manager??
    // map<Operation*, DataManager> accessMap;
    
    Transaction(int txnId, int startTime);

    // Core transaction operations
    void add_operation(Operation op);
    void set_waiting_operation(Operation* op);
    void commit(int timestamp);
    void abort(string reason);

};
