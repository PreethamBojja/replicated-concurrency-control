#pragma once

#include <map>
#include <vector>
#include <memory>
#include "transaction.h"
#include "dataManager.h"
#include "valueType.h"

using namespace std;

class TransactionManager {
public:
    map<int, DataManager*> sites;
    map<int, Transaction*> transactions;
    map<int, vector<Operation> > site_history;
    vector<Transaction*> committed_txns;
    vector<Operation*> waiting_operations;

    /*
        Author : Sai Preetham Bojja
        Description : Constructor for the TransactionManager class
        Inputs : Null
        Outputs : Null
        Side Effects : Initializes the TransactionManager object
    */
    TransactionManager();

    /*
        Author : Sai Preetham Bojja
        Description : Returns the singleton instance of the TransactionManager
        Inputs : Null
        Outputs : Singleton instance of TransactionManager
        Side Effects : No side effects
    */
    static TransactionManager& get_instance();

    /*
        Author : Sai Preetham Bojja
        Description : Returns the transaction object with the given ID
        Inputs : Transaction ID
        Outputs : Transaction object with the given ID
        Side Effects : No side effects
    */
    Transaction* get_transaction(int transactionId);

    /*
        Author : Sai Preetham Bojja
        Description : Begins a new transaction
        Inputs : Transaction ID, Transaction start timestamp
        Outputs : Null
        Side Effects : Begins a new transaction
    */
    void begin_transaction(int transactionId, int timestamp);

    /*
        Author : Nikhil Kommineni and Sai Preetham Bojja
        Description : Performs a read operation on a variable
        Inputs : Transaction ID, Variable name, Read timestamp
        Outputs : Value of the variable
        Side Effects : No side effects
    */
    int read_operation(int transactionId, string variable, int timestamp);

    /*
        Author : Nikhil Kommineni and Sai Preetham Bojja
        Description : Performs a write operation on a variable
        Inputs : Transaction ID, Variable name, Value to be written, Write timestamp
        Outputs : Null
        Side Effects : No side effects
    */
    void write_operation(int transactionId, string variable, int value, int timestamp);

    /*
        Author : Nikhil Kommineni
        Description : Ends a transaction
        Inputs : Transaction ID, Transaction end timestamp
        Outputs : True if the transaction is committed, False if the transaction is aborted
        Side Effects : No side effects
    */
    bool end_transaction(int transactionId, int timestamp);

    /*
        Author : Sai Preetham Bojja
        Description : Fails a site
        Inputs : Site ID, Failure timestamp
        Outputs : Null
        Side Effects : No side effects
    */
    void fail_site(int siteId, int timestamp);

    /*
        Author : Sai Preetham Bojja
        Description : Recovers a failed site
        Inputs : Site ID, Recovery timestamp
        Outputs : Null
        Side Effects : No side effects
    */
    void recover_site(int siteId, int timestamp);

    /*
        Author : Nikhil Kommineni
        Description : Checks for cycles in the transaction dependency graph
        Inputs : Committed transactions, Transaction to check for cycles
        Outputs : True if there is a cycle, False otherwise
        Side Effects : No side effects
    */
    bool check_for_cycle(vector<Transaction*> committedTransactions, Transaction* txn);

    /*
        Author : Nikhil Kommineni
        Description : Dumps the current system state
        Inputs : Null
        Outputs : Null
        Side Effects : No side effects
    */
    void dump_system_state();
};
