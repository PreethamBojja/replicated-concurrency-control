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

/**
Read Operation :

1. Unreplicated variable :  Check if the site is up if it is up read the value from the time the transaction begun, if it’s down then queue the read operation
 
2. Replicated variable :  Check if the site is valid, if it’s valid and the site is up then read the value from the time the transaction begun. If it’s valid and
                          the site is not up then go to the next valid site that is up. If all valid sites are down then add the operation to queue.
                          If all the sites are invalid then abort the transaction.

   How to check if the site is valid or not : Check if there is any fail of that site between the last commit time before transaction start time of that variable and the transaction start time.
*/

int TransactionManager::readOperation(int transactionId, string variable, int timestamp) {
    
    Transaction* txn = getTransaction(transactionId);

    if (txn->currentState.find(variable) != txn->currentState.end()) {
        return txn->currentState[variable];
    }

    int var_id = stoi(variable.substr(1));
    bool valid_site_exists = false;
    ValueType var_instance;

    if (var_id % 2 == 1) {
        int site_id = 1 + var_id % 10;
        DataManager* site = sites[site_id];
        if (site->is_site_up()) {
            var_instance = site->read(variable, txn->start_ts);
            int val = var_instance.getValue();
            txn->addOperation(Operation(OperationType::READ, transactionId, variable, val, timestamp));
            txn->var_access_map[variable] = make_pair(site_id, var_instance);
            txn->currentState[variable] = val;
            return val;
        }
    } else {
        vector<int> valid_sites = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        for (auto site_id : valid_sites) {
            DataManager* site = sites[site_id];
            vector<Operation> site_history = siteHistory[site_id];

            bool is_site_valid_for_read = true;
            var_instance = site->read(variable, txn->start_ts);
            int var_last_commit_before_ts_start = var_instance.getTimestamp();
            for (auto it = site_history.rbegin(); it != site_history.rend(); it++) {
                if (it->timestamp >= var_last_commit_before_ts_start && it->timestamp <= txn->start_ts && it->op_type == OperationType::FAIL) {
                    is_site_valid_for_read = false;
                    break;
                }
            }

            if (is_site_valid_for_read) {
                valid_site_exists = true;
            }

            if (is_site_valid_for_read && site->is_site_up()) {
                int val = var_instance.getValue();
                txn->addOperation(Operation(OperationType::READ, transactionId, variable, val, timestamp));
                txn->var_access_map[variable] = make_pair(site_id, var_instance);
                txn->currentState[variable] = val;
                return val;
            }
        }
    }

    if (!valid_site_exists) {
        txn->abort("no valid site for read " + variable);
    } else {
        txn->queueOperation(Operation(OperationType::READ, transactionId, variable, 0, timestamp));
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
    auto op = Operation(OperationType::WRITE, txn_id, variable, value, timestamp);
    txn->addOperation(op);
    vector<int> active_sites;
    int var_id = stoi(variable.substr(1));
    if (var_id % 2 == 1) {
        DataManager *site = sites[var_id % 10 + 1];
        if (site->is_site_up()) {
            active_sites.push_back(1 + var_id % 10);
        }
    } else {
        for (int i = 1; i < 11; i++) {
            if (sites[i]->is_site_up()) {
                active_sites.push_back(i);
            }
        }
    }
    txn->active_sites_for_write_op[op] = active_sites;
    return;
}

bool TransactionManager::endTransaction(int transactionId, int timestamp) {
    Transaction* txn = getTransaction(transactionId);  
    if (!txn) {
        cerr << "Transaction " << transactionId << " not found." << endl;
        return false;
    }

    // run pre-commit checks on the txn
    bool is_commitable = true;
    for (auto it : txn->active_sites_for_write_op) {
        int op_ts = it.first.timestamp;
        string variable = it.first.variable;
        for (auto site_id : it.second) {

            // available copies check on writes
            for (auto site_op : siteHistory[site_id]) {
                if (site_op.op_type == OperationType::FAIL && site_op.timestamp > op_ts) {
                    is_commitable = false;
                    break;
                }
            }

            // first-committer advantage check
            DataManager *site = sites[site_id];
            if (site->values[variable].timestamp > txn->start_ts) {
                is_commitable = false;
            }

            if (!is_commitable) {
                break;
            }
        }
        if (!is_commitable) {
            break;
        }
    }
    // pre-commit checks go here:
    //      1. available copies
    //      2. first committer adv check
    //      3. SSI checks
    if (is_commitable) {
        for (auto it : txn->active_sites_for_write_op) {
            string variable = it.first.variable;
            ValueType value = ValueType(txn->currentState[variable], timestamp, transactionId);
            for (auto site_id : it.second) {
                sites[site_id]->commit(variable, value, timestamp);
            }
        }
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
