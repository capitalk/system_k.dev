
#include <zmq.hpp>
#include <gtest/gtest.h>
#include <boost/shared_ptr.hpp>
#include "../order-fix/logging.h"
#include "proto/capk_globals.pb.h"
#include "proto/execution_report.pb.h"
#include "utils/time_utils.h"
#include "../order-fix/msg_cache.h"

/* Standard C++ headers */
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <stdexcept>

/* MySQL Connector/C++ specific headers */
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/metadata.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/exception.h>
#include <cppconn/warning.h>

// ZMQ stuff
//#define TRADE_LISTENER_BIND_ADDRESS "tcp://127.0.0.1:9898"
#define TRADE_LISTENER_BIND_ADDRESS "ipc:///tmp/trade_serializer"
    
// SQL stuff
#define DBHOST "tcp://127.0.0.1:3306"
#define USER "capk"
#define PASSWORD "capk"
#define DATABASE "`capk.prod`"

#define NUMOFFSET 100
#define COLNAME 200

using namespace std;
using namespace sql;

static void retrieve_data_and_print (ResultSet *rs, int type, int colidx, string colname) {

    /* retrieve the row count in the result set */
    cout << "\nRetrieved " << rs -> rowsCount() << " row(s)." << endl;

    cout << "\nCityName" << endl;
    cout << "--------" << endl;

    /* fetch the data : retrieve all the rows in the result set */
    while (rs->next()) {
        if (type == NUMOFFSET) {
                       cout << rs -> getString(colidx) << endl;
        } else if (type == COLNAME) {
                       cout << rs -> getString(colname) << endl;
        } // if-else
    } // while

    cout << endl;

} // retrieve_data_and_print()

static void retrieve_dbmetadata_and_print (Connection *dbcon) {

    if (dbcon -> isClosed()) {
        throw runtime_error("DatabaseMetaData FAILURE - database connection closed");
    }

    cout << "\nDatabase Metadata" << endl;
    cout << "-----------------" << endl;

    cout << boolalpha;

    /* The following commented statement won't work with Connector/C++ 1.0.5 and later */
    //auto_ptr < DatabaseMetaData > dbcon_meta (dbcon -> getMetaData());

    DatabaseMetaData *dbcon_meta = dbcon -> getMetaData();

    cout << "Database Product Name: " << dbcon_meta -> getDatabaseProductName() << endl;
    cout << "Database Product Version: " << dbcon_meta -> getDatabaseProductVersion() << endl;
    cout << "Database User Name: " << dbcon_meta -> getUserName() << endl << endl;

    cout << "Driver name: " << dbcon_meta -> getDriverName() << endl;
    cout << "Driver version: " << dbcon_meta -> getDriverVersion() << endl << endl;

    cout << "Database in Read-Only Mode?: " << dbcon_meta -> isReadOnly() << endl;
    cout << "Supports Transactions?: " << dbcon_meta -> supportsTransactions() << endl;
    cout << "Supports DML Transactions only?: " << dbcon_meta -> supportsDataManipulationTransactionsOnly() << endl;
    cout << "Supports Batch Updates?: " << dbcon_meta -> supportsBatchUpdates() << endl;
    cout << "Supports Outer Joins?: " << dbcon_meta -> supportsOuterJoins() << endl;
    cout << "Supports Multiple Transactions?: " << dbcon_meta -> supportsMultipleTransactions() << endl;
    cout << "Supports Named Parameters?: " << dbcon_meta -> supportsNamedParameters() << endl;
    cout << "Supports Statement Pooling?: " << dbcon_meta -> supportsStatementPooling() << endl;
    cout << "Supports Stored Procedures?: " << dbcon_meta -> supportsStoredProcedures() << endl;
    cout << "Supports Union?: " << dbcon_meta -> supportsUnion() << endl << endl;

    cout << "Maximum Connections: " << dbcon_meta -> getMaxConnections() << endl;
    cout << "Maximum Columns per Table: " << dbcon_meta -> getMaxColumnsInTable() << endl;
    cout << "Maximum Columns per Index: " << dbcon_meta -> getMaxColumnsInIndex() << endl;
    cout << "Maximum Row Size per Table: " << dbcon_meta -> getMaxRowSize() << " bytes" << endl;

    cout << "\nDatabase schemas: " << endl;

    auto_ptr < ResultSet > rs ( dbcon_meta -> getSchemas());

    cout << "\nTotal number of schemas = " << rs -> rowsCount() << endl;
    cout << endl;

    int row = 1;

    while (rs -> next()) {
        cout << "\t" << row << ". " << rs -> getString("TABLE_SCHEM") << endl;
        ++row;
    } // while

    cout << endl << endl;

} // retrieve_dbmetadata_and_print()

static void retrieve_rsmetadata_and_print (ResultSet *rs) {

    if (rs -> rowsCount() == 0) {
        throw runtime_error("ResultSetMetaData FAILURE - no records in the result set");
    }

    cout << "ResultSet Metadata" << endl;
    cout << "------------------" << endl;

    /* The following commented statement won't work with Connector/C++ 1.0.5 and later */
    //auto_ptr < ResultSetMetaData > res_meta ( rs -> getMetaData() );

    ResultSetMetaData *res_meta = rs -> getMetaData();

    int numcols = res_meta -> getColumnCount();
    cout << "\nNumber of columns in the result set = " << numcols << endl << endl;

    cout.width(20);
    cout << "Column Name/Label";
    cout.width(20);
    cout << "Column Type";
    cout.width(20);
    cout << "Column Size" << endl;

    for (int i = 0; i < numcols; ++i) {
        cout.width(20);
        cout << res_meta -> getColumnLabel (i+1);
        cout.width(20); 
        cout << res_meta -> getColumnTypeName (i+1);
        cout.width(20); 
        cout << res_meta -> getColumnDisplaySize (i+1) << endl << endl;
    }

    cout << "\nColumn \"" << res_meta -> getColumnLabel(1);
    cout << "\" belongs to the Table: \"" << res_meta -> getTableName(1);
    cout << "\" which belongs to the Schema: \"" << res_meta -> getSchemaName(1) << "\"" << endl << endl;

} // retrieve_rsmetadata_and_print()

bool
executionExists(Connection* sql_cxn, 
                const strategy_id_t& strategy_id, 
                const order_id_t& order_id, 
                const order_id_t& orig_order_id)
{
        PreparedStatement* sql_prepared_statement;
        ResultSet* sql_result_set;
        const char* kSelectCountTrade = "select count(*) \
                                         from trades \
                                         where strategy_id = ? \
                                         and cl_order_id = ?";

        sql_prepared_statement = sql_cxn->prepareStatement (kSelectCountTrade);

        char strategy_id_buffer[UUID_STRLEN];
        sql_prepared_statement->setString (1, strategy_id.c_str(strategy_id_buffer));

        char order_id_buffer[UUID_STRLEN];
        sql_prepared_statement->setString (2, order_id.c_str(order_id_buffer));

        sql_result_set = sql_prepared_statement->executeQuery();
/*
        char orig_oid_buffer[UUID_STRLEN];
        order_id_t orig_oid(false);
        orig_oid.set(er.orig_cl_order_id().c_str(), er.orig_cl_order_id().size());
        sql_prepared_statement->setString (3, orig_oid.c_str(orig_oid_buffer));
*/

        int row_count = sql_result_set->rowsCount();
        pan::log_DEBUG(kSelectCountTrade, "\nQuery returned ", 
                pan::integer(row_count), " row(s).");
                
        int execution_count = 0;
        while (sql_result_set->next()) {
                       execution_count = sql_result_set->getInt(1);
        }
        pan::log_DEBUG("Count of executions: ", pan::integer(execution_count));
        return (execution_count == 1); 
}

void 
updateExecutionReport(Connection* sql_cxn, 
        const strategy_id_t& strategy_id, 
        const capkproto::execution_report& er) 
{
    int update_count = 0;
    PreparedStatement *sql_prepared_statement;

    const char* kInsertTrade = "INSERT INTO trades \
                                (strategy_id, \
                                 cl_order_id, \
                                 orig_cl_order_id, \
                                 exec_id, \
                                 exec_trans_type, \
                                 order_status, \
                                 exec_type, \
                                 symbol, \
                                 security_type, \
                                 side, \
                                 order_qty, \
                                 ord_type, \
                                 price, \
                                 last_shares, \
                                 last_price, \
                                 leaves_qty, \
                                 cum_qty, \
                                 avg_price, \
                                 time_in_force, \
                                 transact_time, \
                                 exec_inst, \
                                 handl_inst, \
                                 order_reject_reason, \
                                 min_qty, \
                                 transact_time_timespec, \
                                 mic) \
                                 VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

    sql_prepared_statement = sql_cxn->prepareStatement (kInsertTrade);
    char strategy_id_buffer[UUID_STRLEN];
    sql_prepared_statement->setString (1, strategy_id.c_str(strategy_id_buffer));

    char order_id_buffer[UUID_STRLEN];
    order_id_t order_id(false);
    order_id.set(er.cl_order_id().c_str(), er.cl_order_id().size());
    sql_prepared_statement->setString (2, order_id.c_str(order_id_buffer));

    char orig_order_id_buffer[UUID_STRLEN];
    order_id_t orig_order_id(false);
    orig_order_id.set(er.orig_cl_order_id().c_str(), er.orig_cl_order_id().size());
    sql_prepared_statement->setString (3, orig_order_id.c_str(orig_order_id_buffer));

    executionExists(sql_cxn, strategy_id, order_id, orig_order_id);

    orig_order_id.set(er.orig_cl_order_id().c_str(), er.orig_cl_order_id().size());
    sql_prepared_statement->setString (4, er.exec_id().c_str());
    sql_prepared_statement->setInt (5, static_cast<const char>(er.exec_trans_type()));
    sql_prepared_statement->setInt (6, static_cast<const char>(er.order_status()));
    sql_prepared_statement->setInt (7, static_cast<const char>(er.exec_type()));
    sql_prepared_statement->setString (8, er.symbol().c_str());
    sql_prepared_statement->setString (9, er.security_type().c_str());
    sql_prepared_statement->setInt (10, static_cast<int>(er.side()));
    sql_prepared_statement->setDouble (11, er.order_qty());
    sql_prepared_statement->setInt (12, er.ord_type());
    sql_prepared_statement->setDouble (13, er.price());
    sql_prepared_statement->setDouble (14, er.last_shares());
    sql_prepared_statement->setDouble (15, er.last_price());
    sql_prepared_statement->setDouble (16, er.leaves_qty());
    sql_prepared_statement->setDouble (17, er.cum_qty());
    sql_prepared_statement->setDouble (18, er.avg_price());
    sql_prepared_statement->setInt (19, er.time_in_force());
    sql_prepared_statement->setString (20, er.transact_time().c_str());
    sql_prepared_statement->setString (21, er.exec_inst().c_str());
    sql_prepared_statement->setInt (22, er.handl_inst());
    sql_prepared_statement->setInt (23, er.order_reject_reason());
    sql_prepared_statement->setDouble (24, er.min_qty());
    // current timestamp omitted since it is auto field
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    int64_t bigint_time;
    capk::timespec2int64_t(&ts, &bigint_time);
    sql_prepared_statement->setInt64 (25, bigint_time);
    sql_prepared_statement->setString (26, er.mic());
    update_count = sql_prepared_statement->executeUpdate();
   
    sql_cxn->commit();
    //delete sql_prepared_statement;
}


void 
insertExecutionReport(Connection* sql_cxn, 
        const strategy_id_t& strategy_id, 
        const capkproto::execution_report& er) 
{
    int insert_count = 0;
    PreparedStatement *sql_prepared_statement;

    const char* kInsertTrade = "INSERT INTO trades \
                                (strategy_id, \
                                 cl_order_id, \
                                 orig_cl_order_id, \
                                 exec_id, \
                                 exec_trans_type, \
                                 order_status, \
                                 exec_type, \
                                 symbol, \
                                 security_type, \
                                 side, \
                                 order_qty, \
                                 ord_type, \
                                 price, \
                                 last_shares, \
                                 last_price, \
                                 leaves_qty, \
                                 cum_qty, \
                                 avg_price, \
                                 time_in_force, \
                                 transact_time, \
                                 exec_inst, \
                                 handl_inst, \
                                 order_reject_reason, \
                                 min_qty, \
                                 transact_time_timespec, \
                                 mic) \
                                 VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

    sql_prepared_statement = sql_cxn->prepareStatement (kInsertTrade);
    char strategy_id_buffer[UUID_STRLEN];
    sql_prepared_statement->setString (1, strategy_id.c_str(strategy_id_buffer));

    char order_id_buffer[UUID_STRLEN];
    order_id_t order_id(false);
    order_id.set(er.cl_order_id().c_str(), er.cl_order_id().size());
    sql_prepared_statement->setString (2, order_id.c_str(order_id_buffer));

    char orig_order_id_buffer[UUID_STRLEN];
    order_id_t orig_order_id(false);
    orig_order_id.set(er.orig_cl_order_id().c_str(), er.orig_cl_order_id().size());
    sql_prepared_statement->setString (3, orig_order_id.c_str(orig_order_id_buffer));

    executionExists(sql_cxn, strategy_id, order_id, orig_order_id);

    orig_order_id.set(er.orig_cl_order_id().c_str(), er.orig_cl_order_id().size());
    sql_prepared_statement->setString (4, er.exec_id().c_str());
    sql_prepared_statement->setInt (5, static_cast<const char>(er.exec_trans_type()));
    sql_prepared_statement->setInt (6, static_cast<const char>(er.order_status()));
    sql_prepared_statement->setInt (7, static_cast<const char>(er.exec_type()));
    sql_prepared_statement->setString (8, er.symbol().c_str());
    sql_prepared_statement->setString (9, er.security_type().c_str());
    sql_prepared_statement->setInt (10, static_cast<int>(er.side()));
    sql_prepared_statement->setDouble (11, er.order_qty());
    sql_prepared_statement->setInt (12, er.ord_type());
    sql_prepared_statement->setDouble (13, er.price());
    sql_prepared_statement->setDouble (14, er.last_shares());
    sql_prepared_statement->setDouble (15, er.last_price());
    sql_prepared_statement->setDouble (16, er.leaves_qty());
    sql_prepared_statement->setDouble (17, er.cum_qty());
    sql_prepared_statement->setDouble (18, er.avg_price());
    sql_prepared_statement->setInt (19, er.time_in_force());
    sql_prepared_statement->setString (20, er.transact_time().c_str());
    sql_prepared_statement->setString (21, er.exec_inst().c_str());
    sql_prepared_statement->setInt (22, er.handl_inst());
    sql_prepared_statement->setInt (23, er.order_reject_reason());
    sql_prepared_statement->setDouble (24, er.min_qty());
    // current timestamp omitted since it is auto field
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    int64_t bigint_time;
    capk::timespec2int64_t(&ts, &bigint_time);
    sql_prepared_statement->setInt64 (25, bigint_time);
    sql_prepared_statement->setString (26, er.mic());
    insert_count = sql_prepared_statement->executeUpdate();
    pan::log_DEBUG("executeUpdate() returned: ", pan::integer(insert_count));
   
    sql_cxn->commit();
    //delete sql_prepared_statement;
}

void
listen(zmq::context_t& context, sql::Connection* sql_cxn)
{
    assert(sql_cxn);
    // set up zmq listener
    int rc;
    zmq::socket_t subscriber(context, ZMQ_DEALER);
    subscriber.bind(TRADE_LISTENER_BIND_ADDRESS);
    subscriber.connect(TRADE_LISTENER_BIND_ADDRESS);
    //const char* filter = "";
    //subscriber.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));
    //pan::log_DEBUG("Listenting on: ", TRADE_LISTENER_BIND_ADDRESS, " with filter \"", filter, "\"");
    //Statement *sql_statement;
    //ResultSet *sql_result_set;
    //PreparedStatement *sql_prepared_statement;
    bool parse_ok;
    int64_t more = 0;
    size_t more_size = sizeof(more);
    strategy_id_t strategy_id(false);

    capkproto::execution_report er;
    while (1) {
        do {
            zmq::message_t msg_strategy_id;
            rc = subscriber.recv(&msg_strategy_id);
            pan::log_DEBUG("SID: [", pan::integer(msg_strategy_id.size()), "]",  pan::blob(msg_strategy_id.data(), msg_strategy_id.size()));
            strategy_id.set(static_cast<const char*>(msg_strategy_id.data()), msg_strategy_id.size());

            zmq::message_t msg_body;
            rc = subscriber.recv(&msg_body);
            assert(rc);
            subscriber.getsockopt(ZMQ_RCVMORE, &more, &more_size);

            parse_ok = er.ParseFromArray(msg_body.data(), msg_body.size());
            pan::log_DEBUG("Listener received:\n", er.DebugString());

            insertExecutionReport(sql_cxn, strategy_id, er);

        } while (more);
    }
}

Connection* 
createMysqlConnection(const char* host, 
            const char* user,
            const char* password, 
            const char* database)
{
    Connection* con;
    Driver* driver;
    try {
        driver = get_driver_instance();
        assert(driver);
        con = driver->connect(host, user, password); 
        return con;
    }
    catch (SQLException &e) {
        cout << "ERROR: SQLException in " << __FILE__;
        cout << " (" << __func__<< ") on line " << __LINE__ << endl;
        cout << "ERROR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << ")" << endl;
    }
    catch (std::exception e) {
        std::cerr << "Exception while connecting to database" << e.what() << std::endl;
    }
    return NULL;
}

int main(int argc, const char *argv[]) {

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    //Driver *driver;
    Connection *con;
    //Statement *stmt;
    //ResultSet *res;
    //PreparedStatement *prep_stmt;
    //Savepoint *savept;

    //int updatecount = 0;

    /* initiate url, user, password and database variables */
    string url(argc >= 2 ? argv[1] : DBHOST);
    const string user(argc >= 3 ? argv[2] : USER);
    const string password(argc >= 4 ? argv[3] : PASSWORD);
    const string database(argc >= 5 ? argv[4] : DATABASE);
    con = createMysqlConnection(url.c_str(),
                                user.c_str(),
                                password.c_str(),
                                database.c_str());
    assert(con);

    // setup logging
    std::string logFileName = createTimestampedLogFilename("trade_serializer");
    logging_init(logFileName.c_str());
    // initialize zmq context
    zmq::context_t context(1);

    try {

        /* alternate syntax using auto_ptr to create the db connection */
        //auto_ptr  con (driver -> connect(url, user, password));

        /* turn off the autocommit */
        con -> setAutoCommit(0);

        cout << "\nDatabase connection\'s autocommit mode = " << con -> getAutoCommit() << endl;
        pan::log_NOTICE("\nDatabase connection\'s autocommit mode = ",
               pan::integer(con->getAutoCommit()));


        /* select appropriate database schema */
        con -> setSchema(database);

#if 0
        /* retrieve and display the database metadata */
        retrieve_dbmetadata_and_print (con);

        /* create a statement object */
        stmt = con -> createStatement();

        cout << "Executing the Query: \"SELECT * FROM City\" .." << endl;

        /* run a query which returns exactly one result set */
        res = stmt -> executeQuery ("SELECT * FROM City");

        cout << "Retrieving the result set .." << endl;

        /* retrieve the data from the result set and display on stdout */
        retrieve_data_and_print (res, NUMOFFSET, 1, string("CityName"));

        /* retrieve and display the result set metadata */
        retrieve_rsmetadata_and_print (res);
        cout << "Demonstrating Prepared Statements .. " << endl << endl;

        /* insert couple of rows of data into City table using Prepared Statements */
        prep_stmt = con -> prepareStatement ("INSERT INTO City (CityName) VALUES (?)");

        cout << "\tInserting \"London, UK\" into the table, City .." << endl;

        prep_stmt -> setString (1, "London, UK");
        updatecount = prep_stmt -> executeUpdate();

        cout << "\tCreating a save point \"SAVEPT1\" .." << endl;
        savept = con -> setSavepoint ("SAVEPT1");

        cout << "\tInserting \"Paris, France\" into the table, City .." << endl;

        prep_stmt -> setString (1, "Paris, France");
        updatecount = prep_stmt -> executeUpdate();

        cout << "\tRolling back until the last save point \"SAVEPT1\" .." << endl;
        con -> rollback (savept);
        con -> releaseSavepoint (savept);

        cout << "\tCommitting outstanding updates to the database .." << endl;
        con -> commit();

        cout << "\nQuerying the City table again .." << endl;

        /* re-use result set object */
        res = NULL;
        res = stmt -> executeQuery ("SELECT * FROM City");

        /* retrieve the data from the result set and display on stdout */
        retrieve_data_and_print (res, COLNAME, 1, string ("CityName"));

        cout << "Cleaning up the resources .." << endl;

#endif 
        listen(context, con); 
        /* Clean up */
        pan::log_DEBUG("Cleaning up the resources ..");
        //delete res;
        //delete stmt;
        //delete prep_stmt;
        con -> close();
        delete con;

    } catch (SQLException &e) {
        cout << "ERROR: SQLException in " << __FILE__;
        cout << " (" << __func__<< ") on line " << __LINE__ << endl;
        cout << "ERROR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << ")" << endl;

        if (e.getErrorCode() == 1047) {
            /*
            Error: 1047 SQLSTATE: 08S01 (ER_UNKNOWN_COM_ERROR)
            Message: Unknown command
            */
            cout << "\nYour server does not seem to support Prepared Statements at all. ";
            cout << "Perhaps MYSQL < 4.1?" << endl;
        }

        return EXIT_FAILURE;
    } catch (std::runtime_error &e) {

        cout << "ERROR: runtime_error in " << __FILE__;
        cout << " (" << __func__ << ") on line " << __LINE__ << endl;
        cout << "ERROR: " << e.what() << endl;

        return EXIT_FAILURE;
    }
    google::protobuf::ShutdownProtobufLibrary();
    return EXIT_SUCCESS;
} // main()
