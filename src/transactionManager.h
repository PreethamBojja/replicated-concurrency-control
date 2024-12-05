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
    //Singleton
    TransactionManager();

    map<int, DataManager*> sites;

    map<int, Transaction*> transactions;

    map<int, vector<Operation> > site_history;

    vector<Transaction*> committed_txns;

    vector<Operation*> waiting_operations;

    static TransactionManager& get_instance();

    Transaction* get_transaction(int transactionId);

    // Transaction lifecycle methods
    void begin_transaction(int transactionId, int timestamp);

    int read_operation(int transactionId, string variable, int timestamp);

    void write_operation(int transactionId, string variable, int value, int timestamp);

    bool end_transaction(int transactionId, int timestamp);

    // Site management
    void fail_site(int siteId, int timestamp);

    void recover_site(int siteId, int timestamp);

    bool check_for_cycle(vector<Transaction*> committedTransactions, Transaction* txn);

    bool run_available_copies_check(Transaction* txn);
    
    void dump_system_state();
};
