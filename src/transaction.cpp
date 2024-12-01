#include "transaction.h"
#include <iostream>
#include <algorithm>

Transaction::Transaction(int txnId, int startTime){
    this->txnId = txnId;
    this->state = State::ACTIVE;
    this->currentOperation = nullptr;
    this->startTime = startTime;
    this->endTime = INT_MAX;
}

void Transaction::addOperation(Operation op) {
   // TODO
}

void Transaction::queueOperation(Operation op) {
    // TODO
}

bool Transaction::commit(int timestamp) {
    // TODO: Implement commit
    return true;
}

void Transaction::abort(string reason) {
    // TODO
    state = State::ABORTED;
    reason4abort = reason;
}

bool Transaction::validateReadWriteConflicts(vector<Transaction*> committedTransactions) {
    // TODO: Implement detailed read-write conflict detection
    // Check for consecutive RW edges in serialization graph
    return true;
}

bool Transaction::validateAvailableCopies() {
    // TODO
    return true;
}

void Transaction::recordAccess(Operation op, DataManager dataManager) {
    // TOD0
}

bool Transaction::hasConflictingAccess(Transaction other) const {
    // TODO
}

int Transaction::getTransactionId(){ 
    return txnId; 
}
Transaction::State Transaction::getState(){ 
    return state; 
}
string Transaction::getAbortReason(){ 
    return reason4abort;
}
vector<Operation> Transaction::getPastOperations(){
    return pastOperations;
}
