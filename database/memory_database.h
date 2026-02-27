/**
 * Droy Database - In-Memory Database Implementation
 */

#ifndef DROY_MEMORY_DATABASE_H
#define DROY_MEMORY_DATABASE_H

#include "droy_database.h"
#include <unordered_map>
#include <mutex>

namespace droy {
namespace database {

// In-memory database implementation
class MemoryDatabase : public DatabaseConnection {
public:
    MemoryDatabase();
    ~MemoryDatabase() override;
    
    bool connect(const std::string& connectionString) override;
    void disconnect() override;
    bool isConnected() const override;
    
    bool createTable(const TableSchema& schema) override;
    bool dropTable(const std::string& tableName) override;
    bool alterTable(const std::string& tableName,
                    const std::vector<ColumnDef>& newColumns) override;
    
    QueryResult executeQuery(const Query& query) override;
    QueryResult executeSQL(const std::string& sql) override;
    
    int64_t insert(const std::string& table, const Row& row) override;
    int update(const std::string& table, const QueryFilter& filter,
               const std::map<std::string, DataValue>& values) override;
    int remove(const std::string& table, const QueryFilter& filter) override;
    
    bool beginTransaction() override;
    bool commit() override;
    bool rollback() override;
    
    std::vector<std::string> getTables() override;
    TableSchema getTableSchema(const std::string& tableName) override;
    
    std::string getLastError() const override;
    
    // Memory-specific methods
    void clear();
    size_t getTableSize(const std::string& tableName) const;
    void dumpToJSON(const std::string& filename) const;
    void loadFromJSON(const std::string& filename);
    
private:
    bool connected;
    std::string lastError;
    int64_t nextId;
    
    struct TableData {
        TableSchema schema;
        std::vector<Row> rows;
        std::map<std::string, std::vector<int64_t>> indexes;
    };
    
    std::unordered_map<std::string, TableData> tables;
    
    // Transaction support
    bool inTransaction;
    std::unordered_map<std::string, TableData> transactionBackup;
    
    std::mutex dbMutex;
    
    void buildIndexes(TableData& table);
    void updateIndexes(TableData& table, const Row& row, bool remove);
    std::vector<Row*> findMatchingRows(TableData& table, const QueryFilter& filter);
    int64_t generateId();
};

// JSON file database
class JSONDatabase : public MemoryDatabase {
public:
    JSONDatabase();
    
    bool connect(const std::string& connectionString) override;
    void disconnect() override;
    
    bool createTable(const TableSchema& schema) override;
    bool dropTable(const std::string& tableName) override;
    
    int64_t insert(const std::string& table, const Row& row) override;
    int update(const std::string& table, const QueryFilter& filter,
               const std::map<std::string, DataValue>& values) override;
    int remove(const std::string& table, const QueryFilter& filter) override;
    
    bool commit() override;
    
private:
    std::string filePath;
    bool dirty;
    
    void saveToFile();
    void loadFromFile();
};

// SQLite database wrapper (stub - would need SQLite library)
class SQLiteDatabase : public DatabaseConnection {
public:
    SQLiteDatabase();
    ~SQLiteDatabase() override;
    
    bool connect(const std::string& connectionString) override;
    void disconnect() override;
    bool isConnected() const override;
    
    bool createTable(const TableSchema& schema) override;
    bool dropTable(const std::string& tableName) override;
    bool alterTable(const std::string& tableName,
                    const std::vector<ColumnDef>& newColumns) override;
    
    QueryResult executeQuery(const Query& query) override;
    QueryResult executeSQL(const std::string& sql) override;
    
    int64_t insert(const std::string& table, const Row& row) override;
    int update(const std::string& table, const QueryFilter& filter,
               const std::map<std::string, DataValue>& values) override;
    int remove(const std::string& table, const QueryFilter& filter) override;
    
    bool beginTransaction() override;
    bool commit() override;
    bool rollback() override;
    
    std::vector<std::string> getTables() override;
    TableSchema getTableSchema(const std::string& tableName) override;
    
    std::string getLastError() const override;
    
private:
    bool connected;
    std::string dbPath;
    std::string lastError;
    void* sqliteHandle;  // Would be sqlite3* with SQLite library
};

} // namespace database
} // namespace droy

#endif // DROY_MEMORY_DATABASE_H
