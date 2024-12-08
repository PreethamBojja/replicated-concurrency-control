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
    map<Operation, vector<int> > active_sites_for_write_op;  // Tracks the active sites to write for an operation
    vector<Operation> past_operations; // past operations of this transaction 
    Operation* waiting_operation; // waiting operation
    map<string, int> current_state; // tracks current state of the transaction
    
    /*
        Author : Sai Preetham Bojja
        Description : Constructor for the Transaction class
        Inputs : Transaction ID, Start time
        Outputs : Null
        Side Effects : Initializes the transaction object
    */
    Transaction(int txnId, int startTime);

    // Core transaction operations

    /*
        Author : Sai Preetham Bojja
        Description : Adds an operation to the transaction
        Inputs : Operation object
        Outputs : Null
        Side Effects : Adds the operation to the transaction
    */
    void add_operation(Operation op);

    /*
        Author : Sai Preetham Bojja
        Description : Sets the waiting operation for the transaction
        Inputs : Operation pointer
        Outputs : Null
        Side Effects : Sets the waiting operation for the transaction
    */
    void set_waiting_operation(Operation* op);

    /*
        Author : Sai Preetham Bojja
        Description : Commits the transaction
        Inputs : Commit timestamp
        Outputs : Null
        Side Effects : Commits the transaction
    */
    void commit(int timestamp);

    /*
        Author : Sai Preetham Bojja
        Description : Aborts the transaction
        Inputs : Reason for aborting
        Outputs : Null
        Side Effects : Aborts the transaction
    */
    void abort(string reason);

};
