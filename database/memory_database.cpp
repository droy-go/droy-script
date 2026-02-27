/**
 * Droy Database - In-Memory Database Implementation
 */

#include "memory_database.h"
#include <algorithm>
#include <fstream>
#include <iostream>

namespace droy {
namespace database {

// ==================== MEMORY DATABASE ====================

MemoryDatabase::MemoryDatabase() 
    : connected(false), nextId(1), inTransaction(false) {}

MemoryDatabase::~MemoryDatabase() {
    disconnect();
}

bool MemoryDatabase::connect(const std::string& connectionString) {
    std::lock_guard<std::mutex> lock(dbMutex);
    connected = true;
    lastError.clear();
    return true;
}

void MemoryDatabase::disconnect() {
    std::lock_guard<std::mutex> lock(dbMutex);
    tables.clear();
    connected = false;
}

bool MemoryDatabase::isConnected() const {
    return connected;
}

bool MemoryDatabase::createTable(const TableSchema& schema) {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    if (!connected) {
        lastError = "Database not connected";
        return false;
    }
    
    if (tables.find(schema.name) != tables.end()) {
        lastError = "Table already exists: " + schema.name;
        return false;
    }
    
    TableData tableData;
    tableData.schema = schema;
    tables[schema.name] = tableData;
    
    return true;
}

bool MemoryDatabase::dropTable(const std::string& tableName) {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    if (!connected) {
        lastError = "Database not connected";
        return false;
    }
    
    auto it = tables.find(tableName);
    if (it == tables.end()) {
        lastError = "Table not found: " + tableName;
        return false;
    }
    
    tables.erase(it);
    return true;
}

bool MemoryDatabase::alterTable(const std::string& tableName,
                                 const std::vector<ColumnDef>& newColumns) {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    auto it = tables.find(tableName);
    if (it == tables.end()) {
        lastError = "Table not found: " + tableName;
        return false;
    }
    
    for (const auto& col : newColumns) {
        it->second.schema.addColumn(col);
    }
    
    return true;
}

QueryResult MemoryDatabase::executeQuery(const Query& query) {
    std::lock_guard<std::mutex> lock(dbMutex);
    QueryResult result;
    
    if (!connected) {
        result.success = false;
        result.errorMessage = "Database not connected";
        return result;
    }
    
    auto it = tables.find(query.tableName);
    if (it == tables.end()) {
        result.success = false;
        result.errorMessage = "Table not found: " + query.tableName;
        return result;
    }
    
    TableData& tableData = it->second;
    
    // Filter rows
    for (const auto& row : tableData.rows) {
        if (query.filter.evaluate(row)) {
            result.rows.push_back(row);
        }
    }
    
    // Sort
    if (!query.orderBy.empty()) {
        std::sort(result.rows.begin(), result.rows.end(),
            [&query](const Row& a, const Row& b) {
                for (const auto& order : query.orderBy) {
                    DataValue* valA = a.get(order.column);
                    DataValue* valB = b.get(order.column);
                    
                    if (!valA && !valB) continue;
                    if (!valA) return !order.ascending;
                    if (!valB) return order.ascending;
                    
                    if (valA->toString() != valB->toString()) {
                        if (order.ascending) {
                            return valA->toString() < valB->toString();
                        } else {
                            return valA->toString() > valB->toString();
                        }
                    }
                }
                return false;
            });
    }
    
    // Apply offset
    if (query.offset > 0 && query.offset < static_cast<int>(result.rows.size())) {
        result.rows.erase(result.rows.begin(), 
                          result.rows.begin() + query.offset);
    }
    
    // Apply limit
    if (query.limit >= 0 && query.limit < static_cast<int>(result.rows.size())) {
        result.rows.resize(query.limit);
    }
    
    result.totalCount = result.rows.size();
    result.success = true;
    
    return result;
}

QueryResult MemoryDatabase::executeSQL(const std::string& sql) {
    QueryResult result;
    result.success = false;
    result.errorMessage = "Raw SQL not supported in memory database";
    return result;
}

int64_t MemoryDatabase::insert(const std::string& table, const Row& row) {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    if (!connected) {
        lastError = "Database not connected";
        return -1;
    }
    
    auto it = tables.find(table);
    if (it == tables.end()) {
        lastError = "Table not found: " + table;
        return -1;
    }
    
    TableData& tableData = it->second;
    
    Row newRow = row;
    newRow.id = generateId();
    newRow.createdAt = std::chrono::system_clock::now().time_since_epoch().count();
    newRow.updatedAt = newRow.createdAt;
    
    tableData.rows.push_back(newRow);
    updateIndexes(tableData, newRow, false);
    
    return newRow.id;
}

int MemoryDatabase::update(const std::string& table, const QueryFilter& filter,
                            const std::map<std::string, DataValue>& values) {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    if (!connected) {
        lastError = "Database not connected";
        return -1;
    }
    
    auto it = tables.find(table);
    if (it == tables.end()) {
        lastError = "Table not found: " + table;
        return -1;
    }
    
    TableData& tableData = it->second;
    int count = 0;
    
    for (auto& row : tableData.rows) {
        if (filter.evaluate(row)) {
            updateIndexes(tableData, row, true);
            
            for (const auto& [key, val] : values) {
                row.set(key, val);
            }
            row.updatedAt = std::chrono::system_clock::now().time_since_epoch().count();
            
            updateIndexes(tableData, row, false);
            count++;
        }
    }
    
    return count;
}

int MemoryDatabase::remove(const std::string& table, const QueryFilter& filter) {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    if (!connected) {
        lastError = "Database not connected";
        return -1;
    }
    
    auto it = tables.find(table);
    if (it == tables.end()) {
        lastError = "Table not found: " + table;
        return -1;
    }
    
    TableData& tableData = it->second;
    int count = 0;
    
    auto newEnd = std::remove_if(tableData.rows.begin(), tableData.rows.end(),
        [&filter, &count, &tableData, this](const Row& row) {
            if (filter.evaluate(row)) {
                updateIndexes(tableData, row, true);
                count++;
                return true;
            }
            return false;
        });
    
    tableData.rows.erase(newEnd, tableData.rows.end());
    
    return count;
}

bool MemoryDatabase::beginTransaction() {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    if (inTransaction) {
        lastError = "Transaction already in progress";
        return false;
    }
    
    transactionBackup = tables;
    inTransaction = true;
    return true;
}

bool MemoryDatabase::commit() {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    if (!inTransaction) {
        lastError = "No transaction in progress";
        return false;
    }
    
    transactionBackup.clear();
    inTransaction = false;
    return true;
}

bool MemoryDatabase::rollback() {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    if (!inTransaction) {
        lastError = "No transaction in progress";
        return false;
    }
    
    tables = transactionBackup;
    transactionBackup.clear();
    inTransaction = false;
    return true;
}

std::vector<std::string> MemoryDatabase::getTables() {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    std::vector<std::string> tableNames;
    for (const auto& [name, _] : tables) {
        tableNames.push_back(name);
    }
    return tableNames;
}

TableSchema MemoryDatabase::getTableSchema(const std::string& tableName) {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    auto it = tables.find(tableName);
    if (it != tables.end()) {
        return it->second.schema;
    }
    
    return TableSchema();
}

std::string MemoryDatabase::getLastError() const {
    return lastError;
}

void MemoryDatabase::clear() {
    std::lock_guard<std::mutex> lock(dbMutex);
    tables.clear();
    nextId = 1;
}

size_t MemoryDatabase::getTableSize(const std::string& tableName) const {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    auto it = tables.find(tableName);
    if (it != tables.end()) {
        return it->second.rows.size();
    }
    return 0;
}

void MemoryDatabase::dumpToJSON(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(dbMutex);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "{";
    bool first = true;
    for (const auto& [name, tableData] : tables) {
        if (!first) file << ",";
        first = false;
        
        file << "\"" << name << "\":{";
        file << "\"schema\":";
        
        // Schema
        file << "{\"columns\":[";
        for (size_t i = 0; i < tableData.schema.columns.size(); i++) {
            if (i > 0) file << ",";
            file << "\"" << tableData.schema.columns[i].name << "\"";
        }
        file << "],";
        file << "\"primaryKeys\":[";
        for (size_t i = 0; i < tableData.schema.primaryKeys.size(); i++) {
            if (i > 0) file << ",";
            file << "\"" << tableData.schema.primaryKeys[i] << "\"";
        }
        file << "]},";
        
        // Rows
        file << "\"rows\":[";
        for (size_t i = 0; i < tableData.rows.size(); i++) {
            if (i > 0) file << ",";
            file << tableData.rows[i].toJSON();
        }
        file << "]}";
    }
    file << "}";
}

void MemoryDatabase::loadFromJSON(const std::string& filename) {
    // Simplified JSON loading - would need proper JSON parser
    // For now, just clear and let user populate
    clear();
}

void MemoryDatabase::buildIndexes(TableData& table) {
    table.indexes.clear();
    for (const auto& col : table.schema.columns) {
        for (const auto& constraint : col.constraints) {
            if (constraint == ConstraintType::INDEX) {
                table.indexes[col.name] = {};
            }
        }
    }
}

void MemoryDatabase::updateIndexes(TableData& table, const Row& row, bool remove) {
    for (auto& [colName, index] : table.indexes) {
        DataValue* val = row.get(colName);
        if (val) {
            if (remove) {
                auto it = std::find(index.begin(), index.end(), row.id);
                if (it != index.end()) {
                    index.erase(it);
                }
            } else {
                index.push_back(row.id);
            }
        }
    }
}

std::vector<Row*> MemoryDatabase::findMatchingRows(TableData& table, 
                                                    const QueryFilter& filter) {
    std::vector<Row*> matches;
    for (auto& row : table.rows) {
        if (filter.evaluate(row)) {
            matches.push_back(&row);
        }
    }
    return matches;
}

int64_t MemoryDatabase::generateId() {
    return nextId++;
}

// ==================== JSON DATABASE ====================

JSONDatabase::JSONDatabase() : dirty(false) {}

bool JSONDatabase::connect(const std::string& connectionString) {
    filePath = connectionString;
    loadFromFile();
    return MemoryDatabase::connect(connectionString);
}

void JSONDatabase::disconnect() {
    if (dirty) {
        saveToFile();
    }
    MemoryDatabase::disconnect();
}

bool JSONDatabase::createTable(const TableSchema& schema) {
    bool result = MemoryDatabase::createTable(schema);
    if (result) {
        dirty = true;
    }
    return result;
}

bool JSONDatabase::dropTable(const std::string& tableName) {
    bool result = MemoryDatabase::dropTable(tableName);
    if (result) {
        dirty = true;
    }
    return result;
}

int64_t JSONDatabase::insert(const std::string& table, const Row& row) {
    int64_t result = MemoryDatabase::insert(table, row);
    if (result > 0) {
        dirty = true;
    }
    return result;
}

int JSONDatabase::update(const std::string& table, const QueryFilter& filter,
                          const std::map<std::string, DataValue>& values) {
    int result = MemoryDatabase::update(table, filter, values);
    if (result > 0) {
        dirty = true;
    }
    return result;
}

int JSONDatabase::remove(const std::string& table, const QueryFilter& filter) {
    int result = MemoryDatabase::remove(table, filter);
    if (result > 0) {
        dirty = true;
    }
    return result;
}

bool JSONDatabase::commit() {
    bool result = MemoryDatabase::commit();
    if (result && dirty) {
        saveToFile();
    }
    return result;
}

void JSONDatabase::saveToFile() {
    dumpToJSON(filePath);
    dirty = false;
}

void JSONDatabase::loadFromFile() {
    // Would implement proper JSON parsing here
    // For now, start with empty database
}

// ==================== SQLITE DATABASE (STUB) ====================

SQLiteDatabase::SQLiteDatabase() 
    : connected(false), sqliteHandle(nullptr) {}

SQLiteDatabase::~SQLiteDatabase() {
    disconnect();
}

bool SQLiteDatabase::connect(const std::string& connectionString) {
    dbPath = connectionString;
    // Would initialize SQLite here
    connected = true;
    return true;
}

void SQLiteDatabase::disconnect() {
    // Would close SQLite connection
    connected = false;
}

bool SQLiteDatabase::isConnected() const {
    return connected;
}

bool SQLiteDatabase::createTable(const TableSchema& schema) {
    // Would execute CREATE TABLE SQL
    return true;
}

bool SQLiteDatabase::dropTable(const std::string& tableName) {
    // Would execute DROP TABLE SQL
    return true;
}

bool SQLiteDatabase::alterTable(const std::string& tableName,
                                 const std::vector<ColumnDef>& newColumns) {
    // Would execute ALTER TABLE SQL
    return true;
}

QueryResult SQLiteDatabase::executeQuery(const Query& query) {
    // Would execute SELECT SQL
    return QueryResult();
}

QueryResult SQLiteDatabase::executeSQL(const std::string& sql) {
    // Would execute raw SQL
    return QueryResult();
}

int64_t SQLiteDatabase::insert(const std::string& table, const Row& row) {
    // Would execute INSERT SQL
    return 1;
}

int SQLiteDatabase::update(const std::string& table, const QueryFilter& filter,
                            const std::map<std::string, DataValue>& values) {
    // Would execute UPDATE SQL
    return 0;
}

int SQLiteDatabase::remove(const std::string& table, const QueryFilter& filter) {
    // Would execute DELETE SQL
    return 0;
}

bool SQLiteDatabase::beginTransaction() {
    // Would execute BEGIN TRANSACTION
    return true;
}

bool SQLiteDatabase::commit() {
    // Would execute COMMIT
    return true;
}

bool SQLiteDatabase::rollback() {
    // Would execute ROLLBACK
    return true;
}

std::vector<std::string> SQLiteDatabase::getTables() {
    // Would query sqlite_master
    return {};
}

TableSchema SQLiteDatabase::getTableSchema(const std::string& tableName) {
    // Would query PRAGMA table_info
    return TableSchema();
}

std::string SQLiteDatabase::getLastError() const {
    return lastError;
}

} // namespace database
} // namespace droy
