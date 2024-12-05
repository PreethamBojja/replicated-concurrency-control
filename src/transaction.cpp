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
    this->waiting_operation = nullptr;
}

void Transaction::add_operation(Operation op) {
   // TODO
   past_operations.push_back(op);
}

void Transaction::set_waiting_operation(Operation* op) {
    // TODO
    this->waiting_operation = op;
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
    reason_4_abort = reason;
}
