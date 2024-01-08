#include <iostream>
#include <unordered_map>
#include <map>
#include <future>
#include <vector>

using TTimestamp = std::int64_t;

using TVersionedValue = std::map<TTimestamp, int>;
using TKey = int;

// Multiple writer single reader
// see lock free hash map that should be implemented in dynamic store
// in dynstore we have revision, to map timestamp to a certain revision
// revision is monotonic inside tablet, but timestamps are not
// meaning timestamp2 can be commited, and then timestamp1 can be seen the first time in them tablet

// What if: we created t1 then t2. t2 changed something, then commited.
// Then we first time read from DS using t1, and we saw changes from t2! wtf?
// does not it break snapshot isolation?

// Transactions in dynamic store:
//  == Locks ==
// We have locks on table rows:
//  int ReadLockCount;
//  TTimestamp LastReadLockTimestamp;
//  TTransaction WriteTransaction
//  TTimestamp PrepareTimestamp;

// Typical case:
// Writes are done without timestamp (???)

// Versioned write with ts is used only during replication.

// How we write rows to dynamic store:
// 0. start transaction with native client
// 1. make some inserts, make some deletes
// 2. class TTransaction buffers all the changes before commit
// 3. user calls commit on transaction
// 4. transaction sends buffered changes to dynamic store
// 5. transaction initiates 2 phase commit on 

// 6) Peek random cell as coordinator
// 7) send to all 2pc prepare
// 8) commit to all 2pc commit

// Transactions are appearing in DS on demand during write command
// Dynamic table transactions have leases
// client has to ping transactions after write and until 2pc commit finishes.
// TransactionManager on tab node side watches transaction leases and cleans up staled transactions
// clean up includes releasing all locks.

// During prepare phase transaction become persistent, it is registered in hydra
// and its lease is not renewed anymore.
// Coordinator of transaction should either commit transaction or rollback in the end.

// Writer receives conformation of commit after all participants returned ok on prepare,
// and coordinator successfully commited in the log, that transaction is going to be commited on all participants (no rollback)

// That means, that participants still holds all the locks on rows for transaction,
// and the same client can receive lock conflicts on successful writes or even stale read.
// To conquer this, there is a mechanism for blocking reads and writes from DS after rows rites are prepared for commit.

// There is also a weird mechanism of writing rows concurrently with initiating 2pc and checking signatures in tab nodes.

// === How locks work ======
// raft automata -- persistent stream of deterministic mutations 
// in reality there are two parts: transient and persistent
// persistent state is changed inside mutations, deterministic and with no side effects.
// transient part -- can be read and written outside of mutation.
// write in persistent state outside mutation is strictly prohibited.

// There are 3 phase of write
// pre-lock
// lock
// commit
// Before write we have to transiently lock rows, it happens in prelock phase and only in the leader
// who is leader here? probably leading peer of tablet cell.
// If there are no keys yet (they will be created) we create keys, with no values, and lock them.

// We have two maps of transactions: persistent ones and transinient ones
// Pre-lock is needed, because we do not wait the outcome of the mutation phase,
// we have to make mutation succeed as long as hydra is working.
// This is not required, but we can not provide meaningful feedback to the user from mutations.

// Data is published to the dynamic store after commit, before that they are buffered in write log transaction(?)
// But the needed keys are locked, so nobody can overwrite keys during commit process

// How we guarantee snapshot isolation?
// How we serve reads for tx2, when transaction holds locks,
// and we do not know which TS it will have, but it is not commited yet, and tx2 > tx2,
// lock is essentially prepared timestamp. If somebody is reading with ts less than ts, we should 


struct TTransaction {
    TTimestamp StartTimestamp = 0;
    std::promise<void> CommitedPromise;
};

using PTransaction = std::shared_ptr<TTransaction>;

struct TTablet
{
    // Can throw.
    void Prepare(PTransaction transaction) {
        CheckPrepared(transaction);
        Prepared.push_back(std::move(transaction));
    }

    // Can not throw.
    std::future<void> Commit(PTransaction transaction) {
        return transaction->CommitedPromise.get_future();
    }

private:
    void CheckPrepared(PTransaction transaction) {

    }

private:
    std::unordered_map<TKey, TVersionedValue> MemTable;
    std::vector<PTransaction> Prepared;
    TTimestamp LastCommited;
};

int main() {
    std::cout << "Hello World!" <<  std::endl;
    return 0;
}