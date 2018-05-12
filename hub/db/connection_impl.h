// Copyright 2018 IOTA Foundation

#ifndef HUB_DB_CONNECTION_IMPL_H_
#define HUB_DB_CONNECTION_IMPL_H_

#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <sqlpp11/transaction.h>

#include "hub/db/connection.h"
#include "hub/db/db.h"
#include "hub/db/helper.h"
#include "hub/db/types.h"

namespace hub {
namespace db {

template <typename Conn>
class TransactionImpl : public Transaction {
 public:
  explicit TransactionImpl(sqlpp::transaction_t<Conn>&& transaction)
      : _transaction(std::move(transaction)) {}

  void commit() override { _transaction.commit(); }
  void rollback() override { _transaction.rollback(); }

 private:
  sqlpp::transaction_t<Conn> _transaction;
};

template <typename Conn, typename Conf>
class ConnectionImpl : public virtual Connection {
 public:
  explicit ConnectionImpl(const Config& config) {
    _conn = std::move(fromConfig(config));
  }

  std::unique_ptr<Conn> fromConfig(const Config& config);

  std::unique_ptr<Transaction> transaction() override {
    return std::make_unique<TransactionImpl<Conn>>(std::move(sqlTransaction()));
  }

  void createUser(const std::string& identifier) override {
    db::helper<Conn>::createUser(*_conn, identifier);
  }

  std::optional<int64_t> userIdFromIdentifier(
      const std::string& identifier) override {
    return db::helper<Conn>::userIdFromIdentifier(*_conn, identifier);
  }

  std::vector<AddressWithID> unsweptUserAddresses() override {
    return db::helper<Conn>::unsweptUserAddresses(*_conn);
  }

  std::vector<std::string> tailsForAddress(uint64_t userId) override {
    return db::helper<Conn>::tailsForAddress(*_conn, userId);
  }

  std::optional<uint64_t> availableBalanceForUser(uint64_t userId) override {
    return db::helper<Conn>::availableBalanceForUser(*_conn, userId);
  }

  void createUserAddress(const std::string& address, uint64_t userId,
                         const std::string& uuid) override {
    db::helper<Conn>::createUserAddress(*_conn, address, userId, uuid);
  }

  void createUserAddressBalanceEntry(uint64_t addressId, int64_t amount,
                                     const UserAddressBalanceReason reason,
                                     std::optional<std::string> tailHash,
                                     std::optional<uint64_t> sweepId) override {
    db::helper<Conn>::createUserAddressBalanceEntry(*_conn, addressId, amount,
                                                    reason, std::move(tailHash),
                                                    std::move(sweepId));
  }

  void createUserAccountBalanceEntry(
      uint64_t userId, int64_t amount, const UserAccountBalanceReason reason,
      const std::optional<uint64_t> fkey = {}) override {
    db::helper<Conn>::createUserAccountBalanceEntry(*_conn, userId, amount,
                                                    reason, std::move(fkey));
  }

  uint64_t createWithdrawal(const std::string& uuid, uint64_t userId,
                            uint64_t amount,
                            const std::string& payoutAddress) override {
    return db::helper<Conn>::createWithdrawal(*_conn, uuid, userId, amount,
                                              payoutAddress);
  }

  size_t cancelWithdrawal(const std::string& uuid) override {
    return db::helper<Conn>::cancelWithdrawal(*_conn, uuid);
  }

  std::optional<AddressWithUUID> selectFirstUserAddress() override {
    return db::helper<Conn>::selectFirstUserAddress(*_conn);
  }

  void markUUIDAsSigned(const std::string& uuid) override {
    db::helper<Conn>::markUUIDAsSigned(*_conn, uuid);
  }

  std::vector<UserBalanceEvent> getUserAccountBalances(
      uint64_t userId) override {
    return db::helper<Conn>::getUserAccountBalances(*_conn, userId);
  }

  std::vector<Sweep> getUnconfirmedSweeps(
      const std::chrono::system_clock::time_point& olderThan) override {
    return db::helper<Conn>::getUnconfirmedSweeps(*_conn, olderThan);
  }

  void createTail(uint64_t sweepId, const std::string& hash) override {
    db::helper<Conn>::createTail(*_conn, sweepId, hash);
  }

  std::vector<std::string> getTailsForSweep(uint64_t sweepId) override {
    return db::helper<Conn>::getTailsForSweep(*_conn, sweepId);
  }

  void markTailAsConfirmed(const std::string& hash) override {
    db::helper<Conn>::markTailAsConfirmed(*_conn, hash);
  }

  std::vector<UserBalanceEvent> getAccountBalances(
      std::chrono::system_clock::time_point newerThan) override {
    return db::helper<Conn>::getAccountBalances(*_conn, newerThan);
  }

  void insertUserTransfers(
      const std::vector<UserTransfer>& transfers) override {
    db::helper<Conn>::insertUserTransfers(*_conn, transfers);
  }

  std::map<std::string, int64_t> userIdsFromIdentifiers(
      const std::set<std::string>& identifiers) override {
    return db::helper<Conn>::userIdsFromIdentifiers(*_conn, identifiers);
  }

  std::map<uint64_t, int64_t> getTotalAmountForUsers(
      const std::set<uint64_t>& ids) override {
    return db::helper<Conn>::getTotalAmountForUsers(*_conn, ids);
  }

  std::map<uint64_t, int64_t> getTotalAmountForAddresses(
      const std::set<uint64_t>& ids) override {
    return db::helper<Conn>::getTotalAmountForAddresses(*_conn, ids);
  }

  void execute(const std::string& what) override { _conn->execute(what); }

 protected:
  sqlpp::transaction_t<Conn> sqlTransaction() {
    return sqlpp::transaction_t<Conn>(*_conn, true);
  }

 private:
  std::unique_ptr<Conn> _conn;
};

}  // namespace db
}  // namespace hub

#endif  // HUB_DB_CONNECTION_IMPL_H_