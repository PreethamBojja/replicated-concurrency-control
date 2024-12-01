#include "transactionManager.h"
#include "operation.h"
#include <iostream>
#include <algorithm>

using namespace std;

TransactionManager& TransactionManager::getInstance() {
    static TransactionManager instance;
    return instance;
}

TransactionManager::TransactionManager() {
    for (int i = 1; i <= 10; ++i) {
        sites[i] = DataManager(i);
    }
}

Transaction* TransactionManager::getTransaction(int transactionId) {
    auto it = transactions.find(transactionId);
    return (it != transactions.end()) ? it->second : nullptr;
}

void TransactionManager::beginTransaction(int transactionId, int timestamp) {
    transactions[transactionId] = Transaction(transactionId, timestamp);
}

int TransactionManager::readOperation(int transactionId, string variable, int timestamp) {
    // TODO: Implement read operation
    return -1;
}

void TransactionManager::writeOperation(int transactionId, string variable, int value, int timestamp) {

    return
}

bool TransactionManager::endTransaction(int transactionId, int timestamp) {
    
    auto* txn = getTransaction(transactionId);  
    if (!txn) {
        cerr << "Transaction " << transactionId << " not found." << endl;
        return false;
    }

    bool commitResult = txn->commit(timestamp);
    if (commitResult) {
        cout << "Transaction " << transactionId << " committed" << endl;
    } else {
        cout << "Transaction " << transactionId << " aborted: " << txn->getAbortReason() << endl;
    }

    return commitResult;
}

void TransactionManager::failSite(int siteId, int timestamp){
    // TODO: Implement fail 
}

void TransactionManager::recoverSite(int siteId, int timestamp){
    // TODO: Implement recover
}

void TransactionManager::dumpSystemState(){
    // TODO: Implement dump
}

bool TransactionManager::checkForCycle(vector<Transaction*> committed_transactions, Transaction* txn) {
    // TODO: Implement cycle detection in serialization graph
    return true;
}

bool TransactionManager::runAvailableCopiesCheck(Transaction* txn) {
    // TODO: Implement available copies validation
    return true;
}
