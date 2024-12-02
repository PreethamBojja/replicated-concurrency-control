#include "transactionManager.h"
#include "dataManager.h"
#include "operation.h"
#include "transaction.h"
#include "valueType.h"
#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

TransactionManager& TransactionManager::getInstance() {
    static TransactionManager instance;
    return instance;
}

TransactionManager::TransactionManager() {
    for (int i = 1; i <= 10; ++i) {
        sites[i] = new DataManager(i);
    }
}

Transaction* TransactionManager::getTransaction(int transactionId) {
    auto it = transactions.find(transactionId);
    return (it != transactions.end()) ? it->second : nullptr;
}

void TransactionManager::beginTransaction(int transactionId, int timestamp) {
    transactions[transactionId] = new Transaction(transactionId, timestamp);
}

int TransactionManager::readOperation(int transactionId, string variable, int timestamp) {
    // TODO: Implement read operation
    Transaction* txn = getTransaction(transactionId);

    if (txn->currentState.find(variable) != txn->currentState.end()) {
        return txn->currentState[variable];
    }

    int var_id = stoi(variable.substr(1));
    vector<int> valid_sites = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    if (var_id % 2 == 1) {
        int site_id = 1 + var_id % 10;
        valid_sites.clear();
        valid_sites.push_back(site_id);
    }
    bool all_valid_sites_down = true;
    for (auto site_id : valid_sites) {
        DataManager* site = sites[site_id];
        vector<Operation> site_history = siteHistory[site_id];
        ValueType var_instance;
        bool flag = site->read(variable, var_instance);

        if (flag) {
            all_valid_sites_down = false;

            int var_last_commit = var_instance.getTimestamp();
            int is_site_valid_for_read = true;
            for (auto it = site_history.rbegin(); it != site_history.rend(); it++) {
                if (it->timestamp >= var_last_commit && it->op_type == OperationType::FAIL) {
                    is_site_valid_for_read = false;
                    break;
                }
            }

            if (is_site_valid_for_read) {
                int val = var_instance.getValue();
                txn->addOperation(Operation(OperationType::READ, transactionId, variable, val, timestamp));

                // record the first access of "varible" by the transaction "transactionId"
                // used to build the dependency graph for SSI
                // maybe useful for Available Copies checks
                // can maintian and update a running SSI graph 
                txn->var_access_map[variable] = make_pair(site_id, var_instance);

                // update current state
                txn->currentState[variable] = val;

                return val;
            }
        }
    }

    // decide when to queue to op vs when to abort the transaction
    // if no valid site is up, queue the operation
    // from the project doc:
    /**
        if the transaction manager TM requests a read on a replicated data item
        x for read-write transaction T and cannot get it due to failure, the TM
        should try another site (all in the same step). If no relevant site is available,
        then T must wait. Note that T must have access to the version of x that
        was the last to commit before T began. (As mentioned above, if every site
        failed after a commit to x but before T began, then T should abort and will
        never reach an end(T).)
     */
    if (all_valid_sites_down) {
        txn->queueOperation(Operation(OperationType::READ, transactionId, variable, 0, timestamp));
    } else {
        txn->abort("no valid site");
    }

    return -1;
}

void TransactionManager::writeOperation(int txn_id, string variable, int value, int timestamp) {
    // just update the value in the txns local state
    // write to all valid sites at commit time!
    Transaction* txn = getTransaction(txn_id);
    txn->currentState[variable] = value;
    txn->is_written[variable] = true;

    // record write operations of the txn
    // use brute force to get all available copies for
    /**
        Though I won’t require that you implement a distributed version of SSI, please use
        the abort rule for writes on the Available Copies algorithm anyway (if T
        writes to a site s and then s fails before T commits, then T should abort
        at end(T); you need not do that for reads), because a truly distributed im-
        plementation would have local information that would disappear on failure.
     */
    txn->addOperation(Operation(OperationType::WRITE, txn_id, variable, value, timestamp));
    
    return;
}

bool TransactionManager::endTransaction(int transactionId, int timestamp) {
    Transaction* txn = getTransaction(transactionId);  
    if (!txn) {
        cerr << "Transaction " << transactionId << " not found." << endl;
        return false;
    }

    // run pre-commit checks on the txn
    bool is_commitable = false;
    // pre-commit checks go here:
    //      1. available copies
    //      2. SSI checks
    if (is_commitable) {
        txn->commit(timestamp);
        cout << "Transaction " << transactionId << " committed" << endl;
    } else {
        txn->abort("pre-commit checks failed");
        cout << "Transaction " << transactionId << " aborted: " << txn->getAbortReason() << endl;
    }

    return is_commitable;
}

void TransactionManager::failSite(int site_id, int timestamp){
    // TODO: Implement fail 
    DataManager *site = sites[site_id];
    siteHistory[site_id].push_back(Operation(OperationType::FAIL, -1, "", 0, timestamp));
    site->isUp = false;
}

void TransactionManager::recoverSite(int site_id, int timestamp){
    // TODO: Implement recover
    DataManager* site = sites[site_id];
    site->isUp = true;
    siteHistory[site_id].push_back(Operation(OperationType::RECOVER, -1, "", -1, timestamp));
    for (int var_id = 2; var_id <= 20; var_id += 2) {
        string var = "x" + to_string(var_id);
        site->accessible[var] = false;
    }
    if (site_id % 2 == 0) {
        vector<int> var_ids = {site_id - 1, 10 + site_id - 1};
        for (auto var_id : var_ids) {
            string var = "x" + to_string(var_id);
            site->accessible[var] = true;
        }
    }
}

void TransactionManager::dumpSystemState(){
    // TODO: Implement dump
    for (int site_id = 1; site_id < 11; site_id++) {
        cout << "site " << site_id << " - ";
        vector<int> var_ids = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20};
        if (site_id % 2 == 0) {
            var_ids.push_back(site_id - 1);
            var_ids.push_back(10 + site_id - 1);
        }
        sort(var_ids.begin(), var_ids.end());


        DataManager *site = sites[site_id];
        for (auto var_id : var_ids) {
            string var = "x" + to_string(var_id);
            cout << var << ": " << site->values[var].getValue() << ", ";
        }
        cout << endl;
    }
}

bool TransactionManager::checkForCycle(vector<Transaction*> committed_transactions, Transaction* txn) {
    // TODO: Implement cycle detection in serialization graph
    return true;
}

bool TransactionManager::runAvailableCopiesCheck(Transaction* txn) {
    // TODO: Implement available copies validation
    return true;
}
