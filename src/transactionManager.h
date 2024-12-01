#pragma once

#include <map>
#include <vector>
#include <memory>
#include "transaction.h"
#include "dataManager.h"

using namespace std;

class TransactionManager {
private:
    //Singleton
    TransactionManager();

    static int currentTimestamp;

    map<int, DataManager> sites;
    map<int, Transaction> transactions;
    map<int, Operation> siteHistory;

public:
    static TransactionManager& getInstance();

    // Transaction lifecycle methods
    void beginTransaction(int transactionId, int timestamp);
    int readOperation(int transactionId, string variable, int timestamp);
    void writeOperation(int transactionId, string variable, int value, int timestamp);
    bool endTransaction(int transactionId, int timestamp);

    // Site management
    void failSite(int siteId, int timestamp);
    void recoverSite(int siteId, int timestamp);

    bool checkForCycle(vector<Transaction*> committedTransactions, Transaction* txn);
    bool runAvailableCopiesCheck(Transaction* txn);
    void dumpSystemState();
};
