/**
 * Droy Database - Implementation
 */

#include "droy_database.h"
#include <sstream>
#include <iomanip>
#include <chrono>

namespace droy {
namespace database {

// ==================== DATA VALUE ====================

std::string DataValue::toString() const {
    switch (type) {
        case DataType::INTEGER:
            return std::to_string(std::get<int64_t>(value));
        case DataType::REAL:
            return std::to_string(std::get<double>(value));
        case DataType::TEXT:
            return std::get<std::string>(value);
        case DataType::BOOLEAN:
            return std::get<bool>(value) ? "true" : "false";
        case DataType::NULL_TYPE:
            return "null";
        default:
            return "[complex]";
    }
}

std::string DataValue::getTypeName() const {
    switch (type) {
        case DataType::INTEGER: return "INTEGER";
        case DataType::REAL: return "REAL";
        case DataType::TEXT: return "TEXT";
        case DataType::BLOB: return "BLOB";
        case DataType::BOOLEAN: return "BOOLEAN";
        case DataType::DATETIME: return "DATETIME";
        case DataType::JSON: return "JSON";
        case DataType::ARRAY: return "ARRAY";
        case DataType::NULL_TYPE: return "NULL";
        default: return "UNKNOWN";
    }
}

// ==================== TABLE SCHEMA ====================

void TableSchema::addColumn(const ColumnDef& col) {
    columns.push_back(col);
}

ColumnDef* TableSchema::getColumn(const std::string& name) {
    for (auto& col : columns) {
        if (col.name == name) {
            return &col;
        }
    }
    return nullptr;
}

bool TableSchema::hasColumn(const std::string& name) const {
    for (const auto& col : columns) {
        if (col.name == name) {
            return true;
        }
    }
    return false;
}

std::string TableSchema::toSQL() const {
    std::ostringstream oss;
    oss << "CREATE TABLE " << name << " (\n";
    
    for (size_t i = 0; i < columns.size(); i++) {
        const auto& col = columns[i];
        oss << "  " << col.name << " " << col.getTypeName();
        
        for (const auto& constraint : col.constraints) {
            switch (constraint) {
                case ConstraintType::PRIMARY_KEY:
                    oss << " PRIMARY KEY";
                    break;
                case ConstraintType::NOT_NULL:
                    oss << " NOT NULL";
                    break;
                case ConstraintType::UNIQUE:
                    oss << " UNIQUE";
                    break;
                case ConstraintType::AUTO_INCREMENT:
                    oss << " AUTOINCREMENT";
                    break;
                default:
                    break;
            }
        }
        
        if (!col.defaultValue.isNull()) {
            oss << " DEFAULT " << col.defaultValue.toString();
        }
        
        if (i < columns.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << ")";
    return oss.str();
}

std::string ColumnDef::getTypeName() const {
    switch (type) {
        case DataType::INTEGER: return "INTEGER";
        case DataType::REAL: return "REAL";
        case DataType::TEXT: return "TEXT";
        case DataType::BLOB: return "BLOB";
        case DataType::BOOLEAN: return "BOOLEAN";
        case DataType::DATETIME: return "DATETIME";
        case DataType::JSON: return "JSON";
        default: return "TEXT";
    }
}

// ==================== ROW ====================

DataValue* Row::get(const std::string& column) {
    auto it = data.find(column);
    if (it != data.end()) {
        return &it->second;
    }
    return nullptr;
}

void Row::set(const std::string& column, const DataValue& value) {
    data[column] = value;
}

bool Row::has(const std::string& column) const {
    return data.find(column) != data.end();
}

std::string Row::toJSON() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"id\":" << id << ",";
    oss << "\"data\":{";
    
    bool first = true;
    for (const auto& [key, val] : data) {
        if (!first) oss << ",";
        first = false;
        
        oss << "\"" << key << "\":";
        if (val.type == DataType::TEXT) {
            oss << "\"" << val.toString() << "\"";
        } else {
            oss << val.toString();
        }
    }
    
    oss << "},";
    oss << "\"createdAt\":" << createdAt << ",";
    oss << "\"updatedAt\":" << updatedAt;
    oss << "}";
    
    return oss.str();
}

// ==================== QUERY FILTER ====================

void QueryFilter::addCondition(const QueryCondition& cond, LogicOp logic) {
    conditions.push_back(cond);
    if (!conditions.empty()) {
        logicOps.push_back(logic);
    }
}

bool QueryFilter::evaluate(const Row& row) const {
    if (conditions.empty()) {
        return true;
    }
    
    bool result = true;
    for (size_t i = 0; i < conditions.size(); i++) {
        const auto& cond = conditions[i];
        DataValue* val = row.get(cond.column);
        
        if (!val) {
            return false;
        }
        
        bool condResult = false;
        switch (cond.op) {
            case QueryOp::EQ:
                condResult = (val->toString() == cond.value.toString());
                break;
            case QueryOp::NE:
                condResult = (val->toString() != cond.value.toString());
                break;
            case QueryOp::LT:
                condResult = (val->toString() < cond.value.toString());
                break;
            case QueryOp::GT:
                condResult = (val->toString() > cond.value.toString());
                break;
            case QueryOp::LE:
                condResult = (val->toString() <= cond.value.toString());
                break;
            case QueryOp::GE:
                condResult = (val->toString() >= cond.value.toString());
                break;
            case QueryOp::IS_NULL:
                condResult = val->isNull();
                break;
            case QueryOp::IS_NOT_NULL:
                condResult = !val->isNull();
                break;
            default:
                condResult = true;
                break;
        }
        
        if (i == 0) {
            result = condResult;
        } else if (i < logicOps.size()) {
            if (logicOps[i - 1] == LogicOp::AND) {
                result = result && condResult;
            } else if (logicOps[i - 1] == LogicOp::OR) {
                result = result || condResult;
            }
        }
    }
    
    return result;
}

std::string QueryFilter::toSQL() const {
    if (conditions.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << "WHERE ";
    
    for (size_t i = 0; i < conditions.size(); i++) {
        const auto& cond = conditions[i];
        
        if (i > 0 && i - 1 < logicOps.size()) {
            oss << (logicOps[i - 1] == LogicOp::AND ? " AND " : " OR ");
        }
        
        oss << cond.column << " ";
        
        switch (cond.op) {
            case QueryOp::EQ: oss << "="; break;
            case QueryOp::NE: oss << "!="; break;
            case QueryOp::LT: oss << "<"; break;
            case QueryOp::GT: oss << ">"; break;
            case QueryOp::LE: oss << "<="; break;
            case QueryOp::GE: oss << ">="; break;
            case QueryOp::LIKE: oss << "LIKE"; break;
            case QueryOp::IN: oss << "IN"; break;
            case QueryOp::IS_NULL: oss << "IS NULL"; break;
            case QueryOp::IS_NOT_NULL: oss << "IS NOT NULL"; break;
            default: oss << "="; break;
        }
        
        if (cond.op != QueryOp::IS_NULL && cond.op != QueryOp::IS_NOT_NULL) {
            if (cond.value.type == DataType::TEXT) {
                oss << " '" << cond.value.toString() << "'";
            } else {
                oss << " " << cond.value.toString();
            }
        }
    }
    
    return oss.str();
}

// ==================== QUERY ====================

Query& Query::select(const std::vector<std::string>& cols) {
    columns = cols;
    return *this;
}

Query& Query::where(const QueryCondition& cond) {
    filter.addCondition(cond);
    return *this;
}

Query& Query::order(const std::string& col, bool asc) {
    orderBy.emplace_back(col, asc);
    return *this;
}

Query& Query::take(int n) {
    limit = n;
    return *this;
}

Query& Query::skip(int n) {
    offset = n;
    return *this;
}

std::string Query::toSQL() const {
    std::ostringstream oss;
    oss << "SELECT ";
    
    if (columns.empty()) {
        oss << "*";
    } else {
        for (size_t i = 0; i < columns.size(); i++) {
            if (i > 0) oss << ", ";
            oss << columns[i];
        }
    }
    
    oss << " FROM " << tableName;
    
    std::string whereClause = filter.toSQL();
    if (!whereClause.empty()) {
        oss << " " << whereClause;
    }
    
    if (!orderBy.empty()) {
        oss << " ORDER BY ";
        for (size_t i = 0; i < orderBy.size(); i++) {
            if (i > 0) oss << ", ";
            oss << orderBy[i].column;
            if (!orderBy[i].ascending) {
                oss << " DESC";
            }
        }
    }
    
    if (limit >= 0) {
        oss << " LIMIT " << limit;
    }
    
    if (offset > 0) {
        oss << " OFFSET " << offset;
    }
    
    return oss.str();
}

// ==================== QUERY RESULT ====================

Row* QueryResult::first() {
    if (!rows.empty()) {
        return &rows[0];
    }
    return nullptr;
}

Row* QueryResult::last() {
    if (!rows.empty()) {
        return &rows[rows.size() - 1];
    }
    return nullptr;
}

Row* QueryResult::at(size_t index) {
    if (index < rows.size()) {
        return &rows[index];
    }
    return nullptr;
}

std::string QueryResult::toJSON() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"success\":" << (success ? "true" : "false") << ",";
    oss << "\"count\":" << rows.size() << ",";
    oss << "\"totalCount\":" << totalCount << ",";
    oss << "\"executionTime\":" << executionTime << ",";
    if (!errorMessage.empty()) {
        oss << "\"error\":\"" << errorMessage << "\",";
    }
    oss << "\"rows\":[";
    
    for (size_t i = 0; i < rows.size(); i++) {
        if (i > 0) oss << ",";
        oss << rows[i].toJSON();
    }
    
    oss << "]}";
    return oss.str();
}

// ==================== DATABASE RUNTIME ====================

DatabaseRuntime& DatabaseRuntime::getInstance() {
    static DatabaseRuntime instance;
    return instance;
}

void DatabaseRuntime::registerDatabase(const std::string& name, DatabasePtr db) {
    databases[name] = db;
}

DatabasePtr DatabaseRuntime::getDatabase(const std::string& name) {
    auto it = databases.find(name);
    if (it != databases.end()) {
        return it->second;
    }
    return nullptr;
}

void DatabaseRuntime::unregisterDatabase(const std::string& name) {
    databases.erase(name);
    tableSchemas.erase(name);
}

void DatabaseRuntime::registerTable(const std::string& dbName, 
                                     const std::string& tableName,
                                     const TableSchema& schema) {
    tableSchemas[dbName][tableName] = schema;
}

TableSchema* DatabaseRuntime::getTableSchema(const std::string& dbName,
                                              const std::string& tableName) {
    auto dbIt = tableSchemas.find(dbName);
    if (dbIt != tableSchemas.end()) {
        auto tableIt = dbIt->second.find(tableName);
        if (tableIt != dbIt->second.end()) {
            return &tableIt->second;
        }
    }
    return nullptr;
}

QueryResult DatabaseRuntime::executeQuery(const std::string& dbName, 
                                           const Query& query) {
    QueryResult result;
    
    DatabasePtr db = getDatabase(dbName);
    if (!db) {
        result.success = false;
        result.errorMessage = "Database not found: " + dbName;
        return result;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    result = db->executeQuery(query);
    auto end = std::chrono::high_resolution_clock::now();
    
    result.executionTime = std::chrono::duration<double>(end - start).count();
    return result;
}

int64_t DatabaseRuntime::executeInsert(const std::string& dbName,
                                        const std::string& table,
                                        const Row& row) {
    DatabasePtr db = getDatabase(dbName);
    if (!db) {
        return -1;
    }
    return db->insert(table, row);
}

int DatabaseRuntime::executeUpdate(const std::string& dbName,
                                    const std::string& table,
                                    const QueryFilter& filter,
                                    const std::map<std::string, DataValue>& values) {
    DatabasePtr db = getDatabase(dbName);
    if (!db) {
        return -1;
    }
    return db->update(table, filter, values);
}

int DatabaseRuntime::executeDelete(const std::string& dbName,
                                    const std::string& table,
                                    const QueryFilter& filter) {
    DatabasePtr db = getDatabase(dbName);
    if (!db) {
        return -1;
    }
    return db->remove(table, filter);
}

// ==================== AST VISITORS ====================

void DatabaseDeclaration::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string DatabaseDeclaration::toString() const {
    std::ostringstream oss;
    oss << "set " << variableName << " = database \"" << databaseName << "\"";
    switch (type) {
        case DatabaseType::SQLITE: oss << " type: sqlite"; break;
        case DatabaseType::MEMORY: oss << " type: memory"; break;
        case DatabaseType::JSON: oss << " type: json"; break;
        case DatabaseType::FILE: oss << " type: file"; break;
        case DatabaseType::REMOTE: oss << " type: remote"; break;
    }
    return oss.str();
}

void TableDeclaration::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string TableDeclaration::toString() const {
    std::ostringstream oss;
    oss << "set " << variableName << " = " << databaseVar << ".table \"" 
        << tableName << "\"";
    return oss.str();
}

void QueryStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string QueryStatement::toString() const {
    std::ostringstream oss;
    oss << "set " << variableName << " = ";
    if (isRawSQL) {
        oss << databaseVar << ".query \"" << rawSQL << "\"";
    } else {
        oss << databaseVar << ".\"" << tableName << "\".query(...)";
    }
    return oss.str();
}

void InsertStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string InsertStatement::toString() const {
    std::ostringstream oss;
    oss << "set " << variableName << " = " << tableVar << ".insert { ... }";
    return oss.str();
}

void UpdateStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string UpdateStatement::toString() const {
    std::ostringstream oss;
    oss << "set " << variableName << " = " << tableVar << ".update { ... }";
    return oss.str();
}

void DeleteStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string DeleteStatement::toString() const {
    std::ostringstream oss;
    oss << "set " << variableName << " = " << tableVar << ".delete { ... }";
    return oss.str();
}

void TransactionStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string TransactionStatement::toString() const {
    std::ostringstream oss;
    oss << "transaction " << databaseVar << " { ... }";
    return oss.str();
}

void MigrationStatement::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string MigrationStatement::toString() const {
    std::ostringstream oss;
    oss << databaseVar << ".migrate v" << version << " { ... }";
    return oss.str();
}

void TableReference::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string TableReference::toString() const {
    return databaseVar + ".table(\"" + tableName + "\")";
}

void QueryBuilder::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string QueryBuilder::toString() const {
    std::ostringstream oss;
    oss << tableVar << ".query()";
    if (!conditions.empty()) {
        oss << ".where(...)";
    }
    if (!orders.empty()) {
        oss << ".order(...)";
    }
    if (limit >= 0) {
        oss << ".take(" << limit << ")";
    }
    return oss.str();
}

void ResultMethod::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

std::string ResultMethod::toString() const {
    std::ostringstream oss;
    oss << resultVar << "." << methodName << "()";
    return oss.str();
}

} // namespace database
} // namespace droy
