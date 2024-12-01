#pragma once

#include <vector>
#include <queue>
#include <map>
#include <string>
#include "dataManager.h"

using namespace std;

class Transaction {
private:
    int txnId;
    int startTime;
    int endTime;
    State state;
    string reason4abort;

    vector<Operation> pastOperations;
    Operation* currentOperation;
    queue<Operation*> queuedOperations;

    // Track transaction state and access
    map<string, int> currentState;
    map<Operation*, DataManager> accessMap;
    
public:
    // Transaction states
    enum class State {
        ACTIVE,
        COMMITTED,
        ABORTED
    };

    // Constructor
    Transaction(int txnId, int startTime);

    // Core transaction operations
    void addOperation(Operation op);
    void queueOperation(Operation op);
    bool commit(int timestamp);
    void abort(string reason);

    // Getters
    int getTransactionId();
    State getState();
    string getAbortReason();
    vector<Operation> getPastOperations();
};
