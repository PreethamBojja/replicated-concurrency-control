#include "transaction.h"
#include "operation.h"
#include <iostream>
#include <algorithm>

Transaction::Transaction(int txnId, int startTime){
    this->txnId = txnId;
    this->status = TxnStatus::ACTIVE;
    this->start_ts = startTime;
    this->commit_ts = INT_MAX;

    this->is_read.clear();
    this->is_written.clear();
}

void Transaction::addOperation(Operation op) {
   // TODO
   pastOperations.push_back(op);
}

void Transaction::queueOperation(Operation op) {
    // TODO
    queuedOperations.push(op);
}

void Transaction::commit(int timestamp) {
    // TODO: Implement commit

    // do we need the accessMap during txn commit ??
    // are we writing replicated variables to all avaliable copies or just the copy we read it from ?
    // should the transaction manager handle routing write operations during commit time, since we are only simulation the logic ?
    status = TxnStatus::COMMITTED;
    commit_ts = timestamp;
}

void Transaction::abort(string reason) {
    // TODO
    status = TxnStatus::ABORTED;
    reason4abort = reason;
}

// // move to transaction manager ??
// bool validateReadWriteConflicts(vector<Transaction*> committedTransactions) {
//     // TODO: Implement detailed read-write conflict detection
//     // Check for consecutive RW edges in serialization graph
//     return true;
// }

// // move this to transaction manager ??
// bool Transaction::validateAvailableCopies() {
//     // TODO
//     return true;
// }

// // move to transaction manager
// bool Transaction::hasConflictingAccess(Transaction other) const {
//     // TODO
// }

int Transaction::getTransactionId(){ 
    return txnId; 
}
TxnStatus Transaction::getStatus(){ 
    return status; 
}
string Transaction::getAbortReason(){ 
    return reason4abort;
}
vector<Operation> Transaction::getPastOperations(){
    return pastOperations;
}
