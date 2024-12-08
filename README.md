# replicated-concurrency-control

Distributed replicated concurrency control and recovery

## Authors
- Nikhil Kommineni
- Sai Preetham Bojja

## Description
This project implements a distributed replicated concurrency control and recovery system. The system is designed to handle concurrent transactions on multiple sites, and to ensure consistency and recover from failures.

## How to run
1. Compile the code using `make`
2. Run the code using `./bin/application.out <config_file> <log_file>`
3. The config file is a yaml file that contains the initial system configuration.
4. The log file is a csv file that contains the system logs.

## How to test
1. Run the test using `make test`

## How to use
1. The system supports the following inputs:
    - `begin <transaction_id>`
    - `read <transaction_id> <variable>`
    - `write <transaction_id> <variable> <value>`
    - `end <transaction_id>`
    - `dump`
    - `fail <site_id>`
    - `recover <site_id>`

Example input
```
begin(T1)
begin(T2)
W(T1,x1,101) 
W(T2,x2,202)
W(T1,x2,102) 
W(T2,x1,201)
end(T2)
end(T1)
dump()
```

## Reprozip
Use the following commands to run the project through reprozip
```bash
reprounzip directory setup rcc-submission.rpz run-dir
reprounzip directory run ./run-dir < test/test1
```
