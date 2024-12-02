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
    int startTime;
    int commit_ts;
    TxnStatus status;
    string reason4abort;

    // to maintain the first access details of each variable
    // used for SSI and Available Copies checks
    map<string, pair<int, ValueType> > var_access_map;
    // keep track of the variables written by the txn, useful at commit time
    map<string, bool> is_written;

    vector<Operation> pastOperations;

    // we dont need a currentOperation ?
    // last performed operation is the last item in the pastOperations vector
    // Operation currentOperation;
    
    queue<Operation> queuedOperations;

    // Track transaction state and access
    map<string, int> currentState;

    // put accessMap in Transaction Manager??
    // map<Operation*, DataManager> accessMap;
    
// public:
    // Constructor
    Transaction(int txnId, int startTime);

    // Core transaction operations
    void addOperation(Operation op);
    void queueOperation(Operation op);
    void commit(int timestamp);
    void abort(string reason);

    // Getters
    int getTransactionId();
    TxnStatus getStatus();
    string getAbortReason();
    vector<Operation> getPastOperations();
};
