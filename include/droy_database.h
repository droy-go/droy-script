/**
 * Droy Database - Microprogramming Language
 * =========================================
 * Database extension for Droy, invoked via `set` command
 * 
 * Syntax:
 *   set db = database "name" [type: sqlite|memory|json|file]
 *   set table = db.table "tablename" [schema: {...}]
 *   set result = db.query "SELECT * FROM table"
 *   set row = table.insert {data}
 *   set count = table.update {where: ..., set: ...}
 *   set deleted = table.delete {where: ...}
 */

#ifndef DROY_DATABASE_H
#define DROY_DATABASE_H

#include "token.h"
#include "ast.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>

namespace droy {
namespace database {

// ==================== DATABASE TYPES ====================

enum class DatabaseType {
    SQLITE,      // SQLite database
    MEMORY,      // In-memory database
    JSON,        // JSON file database
    FILE,        // Flat file database
    REMOTE       // Remote/API database
};

enum class DataType {
    INTEGER,
    REAL,
    TEXT,
    BLOB,
    BOOLEAN,
    DATETIME,
    JSON,
    ARRAY,
    NULL_TYPE
};

enum class ConstraintType {
    PRIMARY_KEY,
    UNIQUE,
    NOT_NULL,
    FOREIGN_KEY,
    CHECK,
    DEFAULT,
    AUTO_INCREMENT,
    INDEX
};

// ==================== DATA VALUE ====================

struct DataValue {
    DataType type;
    std::variant<
        int64_t,           // INTEGER
        double,            // REAL
        std::string,       // TEXT, BLOB, DATETIME
        bool,              // BOOLEAN
        std::vector<DataValue>,  // ARRAY
        std::map<std::string, DataValue>,  // JSON object
        std::nullptr_t     // NULL
    > value;
    
    DataValue() : type(DataType::NULL_TYPE), value(nullptr) {}
    DataValue(int64_t v) : type(DataType::INTEGER), value(v) {}
    DataValue(double v) : type(DataType::REAL), value(v) {}
    DataValue(const std::string& v) : type(DataType::TEXT), value(v) {}
    DataValue(bool v) : type(DataType::BOOLEAN), value(v) {}
    DataValue(std::nullptr_t) : type(DataType::NULL_TYPE), value(nullptr) {}
    
    std::string toString() const;
    std::string getTypeName() const;
    bool isNull() const { return type == DataType::NULL_TYPE; }
};

// ==================== COLUMN DEFINITION ====================

struct ColumnDef {
    std::string name;
    DataType type;
    std::vector<ConstraintType> constraints;
    DataValue defaultValue;
    std::string foreignKeyRef;  // "table.column"
    std::string checkExpression;
    
    ColumnDef(const std::string& n, DataType t) 
        : name(n), type(t), defaultValue(nullptr) {}
};

// ==================== TABLE SCHEMA ====================

struct TableSchema {
    std::string name;
    std::vector<ColumnDef> columns;
    std::vector<std::string> primaryKeys;
    std::vector<std::string> indexes;
    
    void addColumn(const ColumnDef& col);
    ColumnDef* getColumn(const std::string& name);
    bool hasColumn(const std::string& name) const;
    std::string toSQL() const;
};

// ==================== ROW DATA ====================

struct Row {
    int64_t id;
    std::map<std::string, DataValue> data;
    int64_t createdAt;
    int64_t updatedAt;
    
    Row() : id(0), createdAt(0), updatedAt(0) {}
    
    DataValue* get(const std::string& column);
    void set(const std::string& column, const DataValue& value);
    bool has(const std::string& column) const;
    std::string toJSON() const;
};

// ==================== QUERY OPERATIONS ====================

enum class QueryOp {
    EQ,          // =
    NE,          // !=
    LT,          // <
    GT,          // >
    LE,          // <=
    GE,          // >=
    LIKE,        // LIKE
    IN,          // IN
    BETWEEN,     // BETWEEN
    IS_NULL,     // IS NULL
    IS_NOT_NULL  // IS NOT NULL
};

struct QueryCondition {
    std::string column;
    QueryOp op;
    DataValue value;
    DataValue value2;  // For BETWEEN
    std::vector<DataValue> values;  // For IN
    
    QueryCondition(const std::string& col, QueryOp o, const DataValue& val)
        : column(col), op(o), value(val) {}
};

enum class LogicOp {
    AND,
    OR,
    NOT
};

struct QueryFilter {
    std::vector<QueryCondition> conditions;
    std::vector<LogicOp> logicOps;
    
    void addCondition(const QueryCondition& cond, LogicOp logic = LogicOp::AND);
    bool evaluate(const Row& row) const;
    std::string toSQL() const;
};

struct QueryOrder {
    std::string column;
    bool ascending;
    
    QueryOrder(const std::string& col, bool asc = true) 
        : column(col), ascending(asc) {}
};

struct Query {
    std::string tableName;
    std::vector<std::string> columns;  // Empty = all columns (*)
    QueryFilter filter;
    std::vector<QueryOrder> orderBy;
    int limit;
    int offset;
    
    Query(const std::string& table) 
        : tableName(table), limit(-1), offset(0) {}
    
    Query& select(const std::vector<std::string>& cols);
    Query& where(const QueryCondition& cond);
    Query& order(const std::string& col, bool asc = true);
    Query& take(int n);
    Query& skip(int n);
    
    std::string toSQL() const;
};

// ==================== QUERY RESULT ====================

struct QueryResult {
    std::vector<Row> rows;
    int totalCount;
    bool success;
    std::string errorMessage;
    double executionTime;
    
    QueryResult() : totalCount(0), success(true), executionTime(0) {}
    
    Row* first();
    Row* last();
    Row* at(size_t index);
    size_t count() const { return rows.size(); }
    bool empty() const { return rows.empty(); }
    std::string toJSON() const;
};

// ==================== DATABASE CONNECTION ====================

class DatabaseConnection {
public:
    virtual ~DatabaseConnection() = default;
    
    virtual bool connect(const std::string& connectionString) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    
    virtual bool createTable(const TableSchema& schema) = 0;
    virtual bool dropTable(const std::string& tableName) = 0;
    virtual bool alterTable(const std::string& tableName, 
                            const std::vector<ColumnDef>& newColumns) = 0;
    
    virtual QueryResult executeQuery(const Query& query) = 0;
    virtual QueryResult executeSQL(const std::string& sql) = 0;
    
    virtual int64_t insert(const std::string& table, const Row& row) = 0;
    virtual int update(const std::string& table, const QueryFilter& filter, 
                       const std::map<std::string, DataValue>& values) = 0;
    virtual int remove(const std::string& table, const QueryFilter& filter) = 0;
    
    virtual bool beginTransaction() = 0;
    virtual bool commit() = 0;
    virtual bool rollback() = 0;
    
    virtual std::vector<std::string> getTables() = 0;
    virtual TableSchema getTableSchema(const std::string& tableName) = 0;
    
    virtual std::string getLastError() const = 0;
};

using DatabasePtr = std::shared_ptr<DatabaseConnection>;

// ==================== DATABASE FACTORY ====================

class DatabaseFactory {
public:
    static DatabasePtr create(DatabaseType type);
    static DatabasePtr createSQLite(const std::string& path);
    static DatabasePtr createMemory();
    static DatabasePtr createJSON(const std::string& path);
    static DatabasePtr createFile(const std::string& path);
};

// ==================== AST NODES FOR DATABASE ====================

// set db = database "name" [type: ...]
class DatabaseDeclaration : public Statement {
public:
    std::string variableName;
    std::string databaseName;
    DatabaseType type;
    std::string connectionString;
    std::map<std::string, std::string> options;
    
    DatabaseDeclaration(const std::string& var, const std::string& name,
                        DatabaseType t = DatabaseType::MEMORY)
        : variableName(var), databaseName(name), type(t) {}
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "DatabaseDeclaration"; }
};

// set table = db.table "name" [schema: {...}]
class TableDeclaration : public Statement {
public:
    std::string variableName;
    std::string databaseVar;
    std::string tableName;
    TableSchema schema;
    bool ifNotExists;
    
    TableDeclaration(const std::string& var, const std::string& dbVar,
                     const std::string& name)
        : variableName(var), databaseVar(dbVar), tableName(name), 
          ifNotExists(false) {}
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "TableDeclaration"; }
};

// set result = db.query "SQL" | db.table("name").where(...)
class QueryStatement : public Statement {
public:
    std::string variableName;
    std::string databaseVar;
    std::string tableName;
    std::string rawSQL;
    Query query;
    bool isRawSQL;
    
    QueryStatement(const std::string& var, const std::string& dbVar)
        : variableName(var), databaseVar(dbVar), isRawSQL(false) {}
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "QueryStatement"; }
};

// set row = table.insert {data}
class InsertStatement : public Statement {
public:
    std::string variableName;
    std::string tableVar;
    std::map<std::string, ExprPtr> data;
    
    InsertStatement(const std::string& var, const std::string& table)
        : variableName(var), tableVar(table) {}
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "InsertStatement"; }
};

// set count = table.update {where: ..., set: ...}
class UpdateStatement : public Statement {
public:
    std::string variableName;
    std::string tableVar;
    QueryFilter whereClause;
    std::map<std::string, ExprPtr> setValues;
    
    UpdateStatement(const std::string& var, const std::string& table)
        : variableName(var), tableVar(table) {}
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "UpdateStatement"; }
};

// set deleted = table.delete {where: ...}
class DeleteStatement : public Statement {
public:
    std::string variableName;
    std::string tableVar;
    QueryFilter whereClause;
    
    DeleteStatement(const std::string& var, const std::string& table)
        : variableName(var), tableVar(table) {}
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "DeleteStatement"; }
};

// transaction { ... }
class TransactionStatement : public Statement {
public:
    std::string databaseVar;
    std::vector<StmtPtr> statements;
    
    TransactionStatement(const std::string& dbVar, std::vector<StmtPtr> stmts)
        : databaseVar(dbVar), statements(std::move(stmts)) {}
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "TransactionStatement"; }
};

// db.migrate { ... }
class MigrationStatement : public Statement {
public:
    std::string databaseVar;
    int version;
    std::vector<StmtPtr> upStatements;
    std::vector<StmtPtr> downStatements;
    
    MigrationStatement(const std::string& dbVar, int ver)
        : databaseVar(dbVar), version(ver) {}
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "MigrationStatement"; }
};

// ==================== DATABASE EXPRESSIONS ====================

// db.table("name")
class TableReference : public Expression {
public:
    std::string databaseVar;
    std::string tableName;
    
    TableReference(const std::string& db, const std::string& table)
        : databaseVar(db), tableName(table) {}
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "TableReference"; }
};

// query.where(...).order(...).take(...)
class QueryBuilder : public Expression {
public:
    std::string tableVar;
    std::vector<QueryCondition> conditions;
    std::vector<QueryOrder> orders;
    int limit;
    int offset;
    
    QueryBuilder(const std::string& table) 
        : tableVar(table), limit(-1), offset(0) {}
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "QueryBuilder"; }
};

// result.first(), result.count(), etc.
class ResultMethod : public Expression {
public:
    std::string resultVar;
    std::string methodName;
    std::vector<ExprPtr> arguments;
    
    ResultMethod(const std::string& result, const std::string& method)
        : resultVar(result), methodName(method) {}
    
    void accept(ASTVisitor* visitor) override;
    std::string toString() const override;
    std::string getNodeType() const override { return "ResultMethod"; }
};

// ==================== DATABASE RUNTIME ====================

class DatabaseRuntime {
public:
    static DatabaseRuntime& getInstance();
    
    void registerDatabase(const std::string& name, DatabasePtr db);
    DatabasePtr getDatabase(const std::string& name);
    void unregisterDatabase(const std::string& name);
    
    void registerTable(const std::string& dbName, const std::string& tableName,
                       const TableSchema& schema);
    TableSchema* getTableSchema(const std::string& dbName, 
                                const std::string& tableName);
    
    QueryResult executeQuery(const std::string& dbName, const Query& query);
    int64_t executeInsert(const std::string& dbName, const std::string& table,
                          const Row& row);
    int executeUpdate(const std::string& dbName, const std::string& table,
                      const QueryFilter& filter,
                      const std::map<std::string, DataValue>& values);
    int executeDelete(const std::string& dbName, const std::string& table,
                      const QueryFilter& filter);
    
private:
    DatabaseRuntime() = default;
    
    std::map<std::string, DatabasePtr> databases;
    std::map<std::string, std::map<std::string, TableSchema>> tableSchemas;
};

// ==================== LLVM DATABASE GENERATOR ====================

class DatabaseLLVMGenerator {
public:
    DatabaseLLVMGenerator(LLVMGenerator* baseGenerator);
    
    void generateDatabaseDeclaration(DatabaseDeclaration* node);
    void generateTableDeclaration(TableDeclaration* node);
    void generateQueryStatement(QueryStatement* node);
    void generateInsertStatement(InsertStatement* node);
    void generateUpdateStatement(UpdateStatement* node);
    void generateDeleteStatement(DeleteStatement* node);
    void generateTransactionStatement(TransactionStatement* node);
    void generateMigrationStatement(MigrationStatement* node);
    
    void generateTableReference(TableReference* node);
    void generateQueryBuilder(QueryBuilder* node);
    void generateResultMethod(ResultMethod* node);
    
private:
    LLVMGenerator* generator;
    
    llvm::Value* createDatabaseConnection(const std::string& name, 
                                           DatabaseType type);
    llvm::Value* createTableReference(const std::string& dbVar,
                                       const std::string& tableName);
    llvm::Value* executeQuery(llvm::Value* db, const Query& query);
    llvm::Value* executeInsert(llvm::Value* table, 
                                const std::map<std::string, llvm::Value*>& data);
    llvm::Value* executeUpdate(llvm::Value* table, llvm::Value* filter,
                                const std::map<std::string, llvm::Value*>& values);
    llvm::Value* executeDelete(llvm::Value* table, llvm::Value* filter);
};

// ==================== PARSER EXTENSIONS ====================

class DatabaseParser {
public:
    // Parse: set db = database "name" [type: ...]
    static StmtPtr parseDatabaseDeclaration(Parser* parser);
    
    // Parse: set table = db.table "name" [schema: {...}]
    static StmtPtr parseTableDeclaration(Parser* parser, 
                                          const std::string& dbVar);
    
    // Parse: set result = db.query ...
    static StmtPtr parseQueryStatement(Parser* parser, 
                                        const std::string& dbVar);
    
    // Parse: set row = table.insert {...}
    static StmtPtr parseInsertStatement(Parser* parser,
                                         const std::string& tableVar);
    
    // Parse: set count = table.update {...}
    static StmtPtr parseUpdateStatement(Parser* parser,
                                         const std::string& tableVar);
    
    // Parse: set deleted = table.delete {...}
    static StmtPtr parseDeleteStatement(Parser* parser,
                                         const std::string& tableVar);
    
    // Parse: transaction { ... }
    static StmtPtr parseTransactionStatement(Parser* parser);
    
    // Parse: db.migrate { ... }
    static StmtPtr parseMigrationStatement(Parser* parser,
                                            const std::string& dbVar);
    
    // Parse query conditions
    static QueryCondition parseQueryCondition(Parser* parser);
    static QueryFilter parseQueryFilter(Parser* parser);
    static QueryOrder parseQueryOrder(Parser* parser);
    
    // Parse data type
    static DataType parseDataType(const std::string& typeName);
    static ColumnDef parseColumnDefinition(Parser* parser);
    static TableSchema parseTableSchema(Parser* parser);
};

} // namespace database
} // namespace droy

#endif // DROY_DATABASE_H
