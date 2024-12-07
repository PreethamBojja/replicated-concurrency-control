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

    TransactionManager();
    static TransactionManager& get_instance();
    Transaction* get_transaction(int transactionId);
    void begin_transaction(int transactionId, int timestamp);
    int read_operation(int transactionId, string variable, int timestamp);
    void write_operation(int transactionId, string variable, int value, int timestamp);
    bool end_transaction(int transactionId, int timestamp);
    void fail_site(int siteId, int timestamp);
    void recover_site(int siteId, int timestamp);
    bool check_for_cycle(vector<Transaction*> committedTransactions, Transaction* txn);
    void dump_system_state();
};
