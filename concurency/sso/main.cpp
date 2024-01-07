#include <iostream>
#include <unordered_map>
#include <map>
#include <future>
#include <vector>

using TTimestamp = std::int64_t;

using TVersionedValue = std::map<TTimestamp, int>;
using TKey = int;

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