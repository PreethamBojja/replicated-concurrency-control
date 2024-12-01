#pragma once

class ValueType {
private:
    int value;
    int timestamp;
    int txnId;

public:
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

    int getValue() { 
        return value; 
    }

    int getTimestamp() { 
        return timestamp; 
    }

    int getTransactionId() { 
        return txnId; 
    }

    void setValue(int val) { 
        value = val; 
    }

    void setTimestamp(int ts) { 
        timestamp = ts; 
    }

    void setTransactionId(int txn) { 
        txnId = txn; 
    }
};
