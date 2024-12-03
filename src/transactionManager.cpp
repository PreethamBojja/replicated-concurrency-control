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
            txn->is_read[variable] = true;
            txn->var_access_map[variable] = make_pair(site_id, var_instance);
            txn->currentState[variable] = val;
            cout << variable << " of T" << transactionId << " reads " << val << endl;
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
                txn->is_read[variable] = true;
                txn->addOperation(Operation(OperationType::READ, transactionId, variable, val, timestamp));
                txn->var_access_map[variable] = make_pair(site_id, var_instance);
                txn->currentState[variable] = val;
                cout << variable << " of T" << transactionId << " reads " << val << endl;
                return val;
            }
        }
    }
    // cout << "read operation failed: " << transactionId << " " << variable << " at time " << timestamp << endl;
    if (!valid_site_exists) {
        txn->abort("no valid site for read " + variable);
    } else {
        txn->setWaitingOperation( new Operation(OperationType::READ, transactionId, variable, 0, timestamp));
        cout << variable << " of T" << transactionId << " waits" << endl;
    }

    return -1;
}

void TransactionManager::writeOperation(int txn_id, string variable, int value, int timestamp) {
    // just update the value in the txns local state
    // write to all valid sites at commit time!
    Transaction* txn = getTransaction(txn_id);
    if(stoi(variable.substr(1))%2 == 1) {
        int site_id = 1 + (stoi(variable.substr(1))%10);
        DataManager* site = sites[site_id];
        if(!site->is_site_up()){
            txn->setWaitingOperation( new Operation(OperationType::WRITE, txn_id, variable, value, timestamp));
            cout << variable << " of T" << txn_id << " waits" << endl;
            return;
        }
    }
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

    if (is_commitable) {
        is_commitable = !checkForCycle(committed_txns, txn);
    }
    // pre-commit checks go here:
    //      1. available copies
    //      2. first committer adv check
    //      3. SSI checks
    if (is_commitable) {
        for (auto it : txn->active_sites_for_write_op) {
            string variable = it.first.variable;
            ValueType value = ValueType(txn->currentState[variable], timestamp, transactionId);
            cout << variable << "," << value.value << " from T" << transactionId << " is written to sites ";
            for (auto site_id : it.second) {
                sites[site_id]->commit(variable, value, timestamp);
                cout << site_id << " ";
            }
            cout << endl;
        }
        txn->commit(timestamp);
        committed_txns.push_back(txn);

        cout << "----- T" << transactionId << " committed" << endl;
    } else {
        txn->abort("pre-commit checks failed");
        cout << "----- T" << transactionId << " aborted: " << txn->getAbortReason() << endl;
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

    // Check and execute operations that are waiting for the recovery of this site
    for(auto it = transactions.begin(); it != transactions.end(); it++) {
        Transaction* txn = it->second;
        Operation* queuedOperation = txn->waiting_operation;
        if(txn->status == TxnStatus::ACTIVE && queuedOperation){
            string waiting_variable = queuedOperation->variable;
            //Check if the waiting transaction is waiting for this site
            if(queuedOperation->op_type == OperationType::WRITE){
                // Only unreplicated variabes have waits for writes
                int waiting_for_site = 1 + (stoi(waiting_variable.substr(1))%10);
                
                if(waiting_for_site == site_id) {
                    waiting_operations.push_back(queuedOperation);
                    txn->setWaitingOperation(nullptr);
                }   
            } else if(queuedOperation->op_type == OperationType::READ) {
                if(stoi(waiting_variable.substr(1))%2 == 1) {
                    waiting_operations.push_back(queuedOperation);
                    txn->setWaitingOperation(nullptr);
                } else {
                    // Check if this is valid site for read
                    DataManager* site = sites[site_id];
                    vector<Operation> site_history = siteHistory[site_id];

                    ValueType var_instance = site->read(waiting_variable, txn->start_ts);
                    int var_last_commit_before_ts_start = var_instance.getTimestamp();
                    for (auto it = site_history.rbegin(); it != site_history.rend(); it++) {
                        if (it->timestamp >= var_last_commit_before_ts_start && it->timestamp <= txn->start_ts && it->op_type == OperationType::FAIL) {
                            continue;
                        }
                    }
                    waiting_operations.push_back(queuedOperation);
                    txn->setWaitingOperation(nullptr);
                }
            }
        }
    }
    // Sort the waiting_operations based on their timestamp
    sort(waiting_operations.begin(), waiting_operations.end(), [](Operation* a, Operation* b) {
        return a->timestamp < b->timestamp;
    });

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

void dfs(map<int, vector<pair<int, string> > > &adj_list, map<int, bool> visited, vector<vector<string> > &cycles, int curr, vector<string> path) {
    if (visited[curr]) {
        cycles.push_back(path);
        return;
    }
    visited[curr] = true;

    for (auto nxt_node : adj_list[curr]) {
        path.push_back(nxt_node.second);
        dfs(adj_list, visited, cycles, nxt_node.first, path);
        path.pop_back();
    }
}

bool TransactionManager::checkForCycle(vector<Transaction*> c_txns, Transaction* txn) {
    // TODO: Implement cycle detection in serialization graph

    vector<Transaction*> all_txns = c_txns;
    all_txns.push_back(txn);

    map<int, vector<pair<int, string> > > adj_list;
    for (auto txn : all_txns) {
        adj_list[txn->txnId] = {};
        /** code to print out the adj list
            cout << "txn: " << txn->txnId << endl;
            cout << "start_ts: " << txn->start_ts << ", commit_ts: " << txn->commit_ts << endl;
            cout << "write_set: ";
            for (auto it : txn->is_written) {
                if (it.second) cout << it.first << ", ";
            } cout << endl;
            cout << "read_set: ";
            for (auto it : txn->is_read) {
                if (it.second) cout << it.first << ", ";
            } cout << endl;
            cout << " -------------- " << endl;
        */
    }
    for (int idx_i = 0; idx_i < all_txns.size(); idx_i++) {
        for (int idx_j = idx_i + 1; idx_j < all_txns.size(); idx_j++) {
            Transaction *txn_i = all_txns[idx_i];
            Transaction *txn_j = all_txns[idx_j];

            if (txn_i->start_ts > txn_j->start_ts) {
                swap(txn_i, txn_j);
            }

            /**
                Upon end(T'),
                add T --ww--> T' to the serialization graph if T commits before T'
                begins, and they both write to x.

                Upon end(T'), add  
                T --wr-->T' to the serialization graph if T writes to x,
                commits before T' begins, and T' reads from x.
             */
            if (txn_i->commit_ts < txn_j->start_ts) {
                for (int i = 1; i < 21; i++) {
                    string var = "x" + to_string(i);
                    if (txn_i->is_written[var] && txn_j->is_written[var]) {
                        adj_list[txn_i->txnId].push_back(make_pair(txn_j->txnId, "ww"));
                    }

                    if (txn_i->is_written[var] && txn_j->is_read[var]) {
                        adj_list[txn_i->txnId].push_back(make_pair(txn_j->txnId, "wr"));
                    }
                }
            }

            /**
                Upon end(T') 
                add
                T --rw --> T' to the serialization graph if T reads from x, T' writes to
                x, and T begins before end(T').
             */

            for (int i = 1; i < 21; i++) {
                string var = "x" + to_string(i);
                if (txn_i->is_read[var] && txn_j->is_written[var]) {
                    adj_list[txn_i->txnId].push_back(make_pair(txn_j->txnId, "rw"));
                }

                if (txn_j->start_ts < txn_i->commit_ts) {
                    if (txn_j->is_read[var] && txn_i->is_written[var]) {
                        adj_list[txn_j->txnId].push_back(make_pair(txn_i->txnId, "rw"));
                    }
                }
            }
        }
    }

    // for (auto it : adj_list) {
    //     cout << it.first << ": ";
    //     for (auto node : it.second) {
    //         cout << node.first << ", ";
    //     } cout << endl;
    // }
    // cout << endl;

    // check for a cycle with adjacent "rw" edges
    map<int, bool> visited;
    vector<vector<string> > cycles;
    vector<string> path;

    // collect all cycles
    for (auto txn : all_txns) {
        dfs(adj_list, visited, cycles, txn->txnId, path);
    }

    for (auto cycle : cycles) {
        for (int idx = 0; idx < cycle.size() - 1; idx++) {
            if (cycle[idx] == "rw" && cycle[idx + 1] == "rw") {
                return true;
            }
        }
        if (cycle[0] == "rw" && cycle[cycle.size() - 1] == "rw") {
            return true;
        }
    }
 
    return false;
}

bool TransactionManager::runAvailableCopiesCheck(Transaction* txn) {
    // TODO: Implement available copies validation
    return true;
}
