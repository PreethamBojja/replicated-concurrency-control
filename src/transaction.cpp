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
   past_operations.push_back(op);
}

void Transaction::set_waiting_operation(Operation* op) {
    this->waiting_operation = op;
}

void Transaction::commit(int timestamp) {
    status = TxnStatus::COMMITTED;
    commit_ts = timestamp;
}

void Transaction::abort(string reason) {
    status = TxnStatus::ABORTED;
    reason_4_abort = reason;
}
