# Running libwallet_api tests

## Environment for the tests
* Running haven node, linked to private/public testnet. 
  By default, tests expect daemon running at ```localhost:27750```,
  can be overridden with environment variable ```TESTNET_DAEMON_ADDRESS=<your_daemon_address>```
  [Manual](https://github.com/moneroexamples/private-testnet) explaining how to run private testnet.

* Directory with pre-generated wallets
  (wallets called alice and bob). 
  By default, tests expect these wallets to be in ```/var/monero/testnet_pvt```. 
  Directory can be overriden with environment variable ```WALLETS_ROOT_DIR=<your_directory_with_wallets>```.
  Directory and files should be writable for the user running tests.


## Generating test wallets
* ```create_wallets.sh``` - this script will create wallets (alice, bob) in current directory. 
  Directory can be changed in ```./conf.sh```  Also Directory pointing to haven-wallet-cli can be changed here. 
  when running first time, please uncomment line ```#create_wallet wallet_m``` to create miner wallet as well. 
  This wallet should be used for mining and all test wallets supposed to be seed from this miner wallet

* ```mining_start.sh``` and ```mining_stop.sh``` - helper scripts to start and stop mining on miner wallet

* ```send_funds.sh``` - script for seeding test wallets. Please run this script when you have enough money on miner wallet

