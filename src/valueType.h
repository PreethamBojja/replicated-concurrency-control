#pragma once

// Variable's value
class ValueType {
public:
    int value;
    int timestamp;
    int txnId;

    ValueType() {
        value = 0;
        timestamp = 0;
        txnId = 0;
    }

    ValueType(int val, int ts, int txn) {
        value = val;
        timestamp = ts;
        txnId = txn;
    }
};
