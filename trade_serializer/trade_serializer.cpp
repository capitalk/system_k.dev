
#include <zmq.hpp>
#include <gtest/gtest.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "utils/logging.h"
#include "utils/venue_globals.h"
#include "utils/time_utils.h"
#include "proto/capk_globals.pb.h"
#include "proto/execution_report.pb.h"
#include "proto/order_cancel.pb.h"
#include "proto/order_cancel_replace.pb.h"
#include "proto/new_order_single.pb.h"
#include "proto/position.pb.h"
#include "proto/recovered_orders.pb.h"
#include "strategy_base/order.h"
#include "utils/types.h"
#include "utils/order_constants.h"
#include "utils/msg_types.h"
#include "order-fix/msg_cache.h"

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

    
// SQL stuff
#define DBHOST "tcp://127.0.0.1:3306"
#define USER "capk"
#define PASSWORD "capk"
//#define DATABASE "`capk.prod`"
#define DATABASE "capk.prod"

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

    cout << "Database Product Name: " 
        << dbcon_meta -> getDatabaseProductName() << endl;
    cout << "Database Product Version: " 
        << dbcon_meta -> getDatabaseProductVersion() << endl;
    cout << "Database User Name: " 
        << dbcon_meta -> getUserName() << endl << endl;

    cout << "Driver name: " 
        << dbcon_meta -> getDriverName() << endl;
    cout << "Driver version: " 
        << dbcon_meta -> getDriverVersion() << endl << endl;

    cout << "Database in Read-Only Mode?: " 
        << dbcon_meta -> isReadOnly() << endl;
    cout << "Supports Transactions?: " 
        << dbcon_meta -> supportsTransactions() << endl;
    cout << "Supports DML Transactions only?: " 
        << dbcon_meta -> supportsDataManipulationTransactionsOnly() << endl;
    cout << "Supports Batch Updates?: " 
        << dbcon_meta -> supportsBatchUpdates() << endl;
    cout << "Supports Outer Joins?: " 
        << dbcon_meta -> supportsOuterJoins() << endl;
    cout << "Supports Multiple Transactions?: " 
        << dbcon_meta -> supportsMultipleTransactions() << endl;
    cout << "Supports Named Parameters?: " 
        << dbcon_meta -> supportsNamedParameters() << endl;
    cout << "Supports Statement Pooling?: " 
        << dbcon_meta -> supportsStatementPooling() << endl;
    cout << "Supports Stored Procedures?: " 
        << dbcon_meta -> supportsStoredProcedures() << endl;
    cout << "Supports Union?: " 
        << dbcon_meta -> supportsUnion() << endl << endl;

    cout << "Maximum Connections: " 
        << dbcon_meta -> getMaxConnections() << endl;
    cout << "Maximum Columns per Table: " 
        << dbcon_meta -> getMaxColumnsInTable() << endl;
    cout << "Maximum Columns per Index: " 
        << dbcon_meta -> getMaxColumnsInIndex() << endl;
    cout << "Maximum Row Size per Table: " 
        << dbcon_meta -> getMaxRowSize() << " bytes" << endl;

    cout << "\nDatabase schemas: " << endl;

    auto_ptr < ResultSet > rs ( dbcon_meta -> getSchemas());

    cout << "\nTotal number of schemas = " 
        << rs -> rowsCount() << endl;
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


int 
positionsByStrategyId(sql::Connection* sql_cxn, 
        const capk::strategy_id_t& strategy_id, 
        capkproto::position* position)
{
/* select order_status, CONVERT(order_status, UNSIGNED INTEGER) as num, CHAR(order_status) as ch from order_state;
 */
    assert(sql_cxn);
    assert(position);
#ifdef DEBUG
    pan::log_DEBUG("Fetching positionByStrategyId with parameters:", 
            "\nstrategy_id: ", 
            pan::blob(strategy_id.get_uuid(), strategy_id.size()), 
            "\nposition: ", position->DebugString());
#endif

    PreparedStatement* sql_prepared_statement;
    ResultSet* sql_result_set;
    const char* const kPositionsByStrategyId = "select \
                                         strategy_id \
                                         cl_order_id \
                                         orig_cl_order_id \
                                         exec_id \
                                         exec_trans_type \
                                         order_status \
                                         exec_type \
                                         symbol \
                                         security_type \
                                         side \
                                         order_qty \
                                         ord_type \
                                         price \
                                         last_shares \
                                         last_price \
                                         leaves_qty \
                                         cum_qty \
                                         avg_price \
                                         time_in_force \
                                         transact_time \
                                         exec_inst \
                                         handl_inst \
                                         order_reject_reason \
                                         min_qty \
                                         current)time_timespec \
                                         venue_id \
                                         account \
                                         from trades \
                                         where strategy_id = ? ";

    int result_count = 0;

    try {
        sql_prepared_statement = sql_cxn->prepareStatement (kPositionsByStrategyId);

        //char strategy_id_buffer[UUID_STRLEN];
        capk::uuidbuf_t strategy_id_buffer;
        sql_prepared_statement->setString (1, strategy_id.c_str(strategy_id_buffer));

        sql_result_set = sql_prepared_statement->executeQuery();

        result_count = sql_result_set->rowsCount();
        capkproto::execution_report trade;
        while (sql_result_set->next()) {
            // KTK TODO - finish position recovery
            //trade 
        }
#ifdef DEBUG
        pan::log_DEBUG(kPositionsByStrategyId, "\nQuery returned ", 
                pan::integer(result_count), " row(s).");
#endif 
    }
    catch (SQLException &e) {
        pan::log_CRITICAL("ERROR: SQLException in ",  __FILE__ , \
        " (",  __func__,  ") on line " , pan::integer(__LINE__));

        pan::log_CRITICAL("ERROR: ", e.what(), \
        " (MySQL error code: ", pan::integer(e.getErrorCode()),  \
        ", SQLState: " , e.getSQLState() , ")");
    } 
    return (result_count == 1); 
}

int 
openOrdersByStrategyId(sql::Connection* sql_cxn, 
        const capk::strategy_id_t& strategy_id, 
        capkproto::recovered_orders* recovered_orders)
{
    //select * from order_state where CHAR(order_status) = '0';
    return (-1);
}




void
handleRecovery(zmq::context_t* context, sql::Connection* sql_cxn)
{
    assert(sql_cxn);
    assert(context);
    // set up zmq listener
    int rc;
    zmq::socket_t recovery_listener_socket(*context, ZMQ_REP);
    recovery_listener_socket.bind(capk::kRECOVERY_LISTENER_ADDR);
    recovery_listener_socket.connect(capk::kRECOVERY_LISTENER_ADDR);

    int64_t more = 0;
    size_t more_size = sizeof(more);
    capk::strategy_id_t strategy_id(false);

    zmq::message_t msg_strategy_id;
    zmq::message_t msg_type;
    while (1) {
        do {
            rc = recovery_listener_socket.recv(&msg_strategy_id);
#ifdef DEBUG
            pan::log_DEBUG("handleRecovery received SID: ", 
                    pan::blob(msg_strategy_id.data(), msg_strategy_id.size()));
#endif
            strategy_id.set(static_cast<const char*>(msg_strategy_id.data()), 
                    msg_strategy_id.size());

            rc = recovery_listener_socket.recv(&msg_type);
            assert(rc);

            recovery_listener_socket.getsockopt(ZMQ_RCVMORE, 
                    &more, 
                    &more_size);

            assert(more == 0); // should only receive sid and msg_type 
        } while (more);

        capk::msg_t request_type = *(static_cast<capk::msg_t*>(msg_type.data()));
        zmq::message_t response(sizeof(capk::POSITION_REQ));

        // What kind of request is this? 
        if (request_type == capk::POSITION_REQ) {
            capkproto::position position_by_strategy_id;
            int recovered_execution_count = positionsByStrategyId(sql_cxn, strategy_id, &position_by_strategy_id);
            memcpy(response.data(), &capk::POSITION_REQ, sizeof(capk::POSITION_REQ));
            recovery_listener_socket.send(response, 0);
        }
        else if (request_type == capk::OPEN_ORDER_REQ) {

            capkproto::recovered_orders recovered_orders;
            int recovered_order_count = openOrdersByStrategyId(sql_cxn, strategy_id, &recovered_orders);
            memcpy(response.data(), &capk::OPEN_ORDER_REQ, sizeof(capk::OPEN_ORDER_REQ));
            recovery_listener_socket.send(response);
        }
        else {
            pan::log_CRITICAL("handleRecovery received unknown message type (", pan::integer(request_type), ") ignored.");
        }
    }
}


bool
orderExists(Connection* sql_cxn, 
                const capk::strategy_id_t& strategy_id, 
                const capk::order_id_t& order_id)
{
#ifdef DEBUG
    pan::log_DEBUG("Checking orderExists with parameters:", 
            "\nstrategy_id: ", 
            pan::blob(strategy_id.get_uuid(), strategy_id.size()), 
            "\norder id: ", pan::blob(order_id.get_uuid(), order_id.size()));
#endif

    PreparedStatement* sql_prepared_statement;
    ResultSet* sql_result_set;
    const char* const kSelectCountTrade = "select count(*) \
                                         from order_status \
                                         where strategy_id = ? \
                                         and cl_order_id = ?";

    int result_count = 0;

    try {
        sql_prepared_statement = sql_cxn->prepareStatement (kSelectCountTrade);

        //char strategy_id_buffer[UUID_STRLEN];
        capk::uuidbuf_t strategy_id_buffer;
        sql_prepared_statement->setString (1, strategy_id.c_str(strategy_id_buffer));

        //char order_id_buffer[UUID_STRLEN];
        capk::uuidbuf_t order_id_buffer;
        sql_prepared_statement->setString (2, order_id.c_str(order_id_buffer));

        sql_result_set = sql_prepared_statement->executeQuery();

        result_count = sql_result_set->rowsCount();
        /*        
        while (sql_result_set->next()) {
                       execution_count += sql_result_set->getInt(1);
        }
        */
#ifdef DEBUG
        pan::log_DEBUG(kSelectCountTrade, "\nQuery returned ", 
                pan::integer(result_count), " row(s).");
#endif 
    }
    catch (SQLException &e) {
        pan::log_CRITICAL("ERROR: SQLException in ",  __FILE__ , \
        " (",  __func__,  ") on line " , pan::integer(__LINE__));

        pan::log_CRITICAL("ERROR: ", e.what(), \
        " (MySQL error code: ", pan::integer(e.getErrorCode()),  \
        ", SQLState: " , e.getSQLState() , ")");
    } 
    return (result_count == 1); 
}

void 
insertTrade(Connection* sql_cxn, 
        const capk::strategy_id_t& strategy_id, 
        const capkproto::execution_report& er) 
{
    int insert_count = 0;
    PreparedStatement *sql_prepared_statement = NULL;
#ifdef DEBUG
    pan::log_DEBUG("Running insertTrade(execution_report) with:", 
            "\nstrategy_id: ", 
            pan::blob(strategy_id.get_uuid(), strategy_id.size()), 
            "\nexecution_report: ", er.DebugString());
#endif

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
                                 current_time_timespec, \
                                 venue_id,\
                                 account) \
                                 VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

    try {
        sql_prepared_statement = sql_cxn->prepareStatement (kInsertTrade);
        capk::uuidbuf_t strategy_id_buffer;
        //char strategy_id_buffer[UUID_STRLEN];
        sql_prepared_statement->setString (1, strategy_id.c_str(strategy_id_buffer));

        capk::uuidbuf_t order_id_buffer;
        //char order_id_buffer[UUID_STRLEN];
        capk::order_id_t order_id(false);
        order_id.set(er.cl_order_id().c_str(), er.cl_order_id().size());
        sql_prepared_statement->setString (2, order_id.c_str(order_id_buffer));

        capk::uuidbuf_t orig_cl_order_id_buffer;
        //char orig_cl_order_id_buffer[UUID_STRLEN];
        capk::order_id_t orig_cl_order_id(false);
        orig_cl_order_id.set(er.orig_cl_order_id().c_str(), er.orig_cl_order_id().size());
        sql_prepared_statement->setString (3, orig_cl_order_id.c_str(orig_cl_order_id_buffer));

        //orderExists(sql_cxn, strategy_id, order_id, orig_cl_order_id);

        orig_cl_order_id.set(er.orig_cl_order_id().c_str(), er.orig_cl_order_id().size());
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
        sql_prepared_statement->setInt (26, er.venue_id());
        sql_prepared_statement->setString (27, er.account().c_str());
        insert_count = sql_prepared_statement->executeUpdate();
#ifdef DEBUG
        pan::log_DEBUG(kInsertTrade, " inserted: ", pan::integer(insert_count));
#endif
       
        sql_cxn->commit();
    } 
    catch (SQLException &e) {
        pan::log_CRITICAL("ERROR: SQLException in ",  __FILE__ , \
        " (",  __func__,  ") on line " , pan::integer(__LINE__));

        pan::log_CRITICAL("ERROR: ", e.what(), \
        " (MySQL error code: ", pan::integer(e.getErrorCode()),  \
        ", SQLState: " , e.getSQLState() , ")");
    } 
    if (sql_prepared_statement) {
        delete sql_prepared_statement;
    }
}



/* Set the status of order to the specified order_status
 */
int 
setOrderStatus(Connection* sql_cxn,
        const capk::strategy_id& strategy_id, 
        const capk::order_id_t& cl_order_id,
        const capk::OrdStatus_t& ord_status)
{
    int update_count = 0;

    // get the order ids in string format
    //char strategy_id_buffer[UUID_STRLEN];
    //char cl_order_id_buffer[UUID_STRLEN];
    capk::uuidbuf_t strategy_id_buffer;
    capk::uuidbuf_t cl_order_id_buffer;

#ifdef DEBUG
    pan::log_DEBUG("setOrderStatus() called with parameters:",
        "\nstrategy_id: ", pan::blob(strategy_id.get_uuid(), strategy_id.size()),
        "\ncl_order_id: ", pan::blob(cl_order_id.get_uuid(), cl_order_id.size()), 
        "\nord_status: ", pan::integer(ord_status));
#endif

    PreparedStatement *sql_prepared_statement;
    const char* const kUpdateOrderStatus = "UPDATE order_state set order_status = ? \
                                            where strategy_id = ? \
                                            and cl_order_id = ?";
    try {
        sql_prepared_statement = sql_cxn->prepareStatement (kUpdateOrderStatus);
        sql_prepared_statement->setInt (1, ord_status);
        sql_prepared_statement->setString (2, strategy_id.c_str(strategy_id_buffer));
        sql_prepared_statement->setString (3, cl_order_id.c_str(cl_order_id_buffer));
        update_count = sql_prepared_statement->executeUpdate();
        sql_cxn->commit();
    }
    catch (SQLException &e) {
        pan::log_CRITICAL("ERROR: SQLException in ",  __FILE__ , \
        " (",  __func__,  ") on line " , pan::integer(__LINE__));

        pan::log_CRITICAL("ERROR: ", e.what(), \
        " (MySQL error code: ", pan::integer(e.getErrorCode()),  \
        ", SQLState: " , e.getSQLState() , ")");
    } 
    catch (std::runtime_error &e) {
        cout << "ERROR: runtime_error in " << __FILE__;
        cout << " (" << __func__ << ") on line " << __LINE__ << endl;
        cout << "ERROR: " << e.what() << endl;
    }
#ifdef DEBUG
    pan::log_DEBUG(kUpdateOrderStatus, " updated: ", pan::integer(update_count));
#endif

    return update_count;
}

void
insertOrder(Connection* sql_cxn, 
        const capk::strategy_id_t& strategy_id, 
        const capkproto::execution_report& er) 
{
    // We'll usually call this on replace since the old order is 
    // cancelled and the new one is then entered into the order_state 
    // table
#ifdef DEBUG
    pan::log_DEBUG("insertOrder() called with parameters:",
        "\nstrategy_id: ", pan::blob(strategy_id.get_uuid(), strategy_id.size()),
        "\nexecution_report: ",er.DebugString());
#endif

    int update_count = 0;
    PreparedStatement *sql_prepared_statement = NULL;

    const char* kInsertOrder = "INSERT INTO order_state \
                                (strategy_id, \
                                 cl_order_id, \
                                 orig_cl_order_id, \
                                 order_status, \
                                 symbol, \
                                 side, \
                                 order_qty, \
                                 ord_type, \
                                 price, \
                                 time_in_force, \
                                 venue_id, \
                                 account, \
                                 current_time_timespec) \
                                 VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)";
   try {
        // Setup SQL statements
        sql_prepared_statement = sql_cxn->prepareStatement (kInsertOrder);
        //char strategy_id_buffer[UUID_STRLEN];
        capk::uuidbuf_t strategy_id_buffer;
        sql_prepared_statement->setString (1, strategy_id.c_str(strategy_id_buffer));

        // get the order ids in string format
        //char cl_order_id_buffer[UUID_STRLEN];
        capk::uuidbuf_t cl_order_id_buffer;
        capk::order_id_t cl_order_id(false);
        cl_order_id.set(er.cl_order_id().c_str(), er.cl_order_id().size());
        sql_prepared_statement->setString (2, cl_order_id.c_str(cl_order_id_buffer));

        //char orig_cl_order_id_buffer[UUID_STRLEN];
        capk::uuidbuf_t orig_cl_order_id_buffer;
        capk::order_id_t orig_cl_order_id(false);
        orig_cl_order_id.set(er.orig_cl_order_id().c_str(), er.orig_cl_order_id().size());
        sql_prepared_statement->setString (3, orig_cl_order_id.c_str(orig_cl_order_id_buffer));

        //orderExists(sql_cxn, strategy_id, order_id, orig_cl_order_id);

        sql_prepared_statement->setInt (4, capk::ORD_STATUS_NEW);
        sql_prepared_statement->setString (5, er.symbol().c_str());
        sql_prepared_statement->setInt (6, static_cast<int>(er.side()));
        sql_prepared_statement->setDouble (7, er.order_qty());
        sql_prepared_statement->setInt (8, er.ord_type());
        sql_prepared_statement->setDouble (9, er.price());
        sql_prepared_statement->setInt (10, er.time_in_force());
        sql_prepared_statement->setInt (11, er.venue_id());
        sql_prepared_statement->setString (12, er.account().c_str());
        // current timestamp omitted since it is auto field
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        int64_t bigint_time;
        capk::timespec2int64_t(&ts, &bigint_time);
        sql_prepared_statement->setInt64 (13, bigint_time);
        update_count = sql_prepared_statement->executeUpdate();
       
        sql_cxn->commit();
    } 
    catch (SQLException &e) {
        pan::log_CRITICAL("ERROR: SQLException in ",  __FILE__ , \
        " (",  __func__,  ") on line " , pan::integer(__LINE__));

        pan::log_CRITICAL("ERROR: ", e.what(), \
        " (MySQL error code: ", pan::integer(e.getErrorCode()),  \
        ", SQLState: " , e.getSQLState() , ")");
    } 
#ifdef DEBUG
    pan::log_DEBUG(kInsertOrder, " inserted: ", pan::integer(update_count));
#endif
    if (sql_prepared_statement) {
        delete sql_prepared_statement;
    }
}


int 
insertOrder(Connection* sql_cxn, 
        const capk::strategy_id_t& strategy_id, 
        const capkproto::new_order_single& nos) 
{
#ifdef DEBUG
    pan::log_DEBUG("insertOrder() called with parameters:",
        "\nstrategy_id: ", pan::blob(strategy_id.get_uuid(), strategy_id.size()),
        "\nnew_order_single: ",nos.DebugString());
#endif

    int update_count = 0;
    PreparedStatement *sql_prepared_statement = NULL;

    const char* kInsertOrder = "INSERT INTO order_state \
                                (strategy_id, \
                                 cl_order_id, \
                                 order_status, \
                                 symbol, \
                                 side, \
                                 order_qty, \
                                 ord_type, \
                                 price, \
                                 time_in_force, \
                                 venue_id, \
                                 account, \
                                 current_time_timespec) \
                                 VALUES (?,?,?,?,?,?,?,?,?,?,?,?)";

    try {
        // Setup SQL statements
        sql_prepared_statement = sql_cxn->prepareStatement (kInsertOrder);
        //char strategy_id_buffer[UUID_STRLEN];
        capk::uuidbuf_t strategy_id_buffer;
        sql_prepared_statement->setString (1, strategy_id.c_str(strategy_id_buffer));

        // Prepare data to insert
        //char order_id_buffer[UUID_STRLEN];
        capk::uuidbuf_t order_id_buffer;
        capk::order_id_t order_id(false);
        order_id.set(nos.order_id().c_str(), nos.order_id().size());
        sql_prepared_statement->setString (2, order_id.c_str(order_id_buffer));

        //orderExists(sql_cxn, strategy_id, order_id, orig_cl_order_id);

        sql_prepared_statement->setInt (3, capk::ORD_STATUS_NEW);
        sql_prepared_statement->setString (4, nos.symbol().c_str());
        sql_prepared_statement->setInt (5, static_cast<int>(nos.side()));
        sql_prepared_statement->setDouble (6, nos.order_qty());
        sql_prepared_statement->setInt (7, nos.ord_type());
        sql_prepared_statement->setDouble (8, nos.price());
        sql_prepared_statement->setInt (9, nos.time_in_force());
        sql_prepared_statement->setInt (10, nos.venue_id());
        sql_prepared_statement->setString (11, nos.account().c_str());
        // current timestamp omitted since it is auto field
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        int64_t bigint_time;
        capk::timespec2int64_t(&ts, &bigint_time);
        sql_prepared_statement->setInt64 (12, bigint_time);
        update_count = sql_prepared_statement->executeUpdate();
       
        sql_cxn->commit();
    }
    catch (SQLException &e) {
        pan::log_CRITICAL("ERROR: SQLException in ",  __FILE__ , \
        " (",  __func__,  ") on line " , pan::integer(__LINE__));

        pan::log_CRITICAL("ERROR: ", e.what(), \
        " (MySQL error code: ", pan::integer(e.getErrorCode()),  \
        ", SQLState: " , e.getSQLState() , ")");
    } 
#ifdef DEBUG
    pan::log_DEBUG(kInsertOrder, " inserted: ", pan::integer(update_count));
#endif
    if (sql_prepared_statement) {
        delete sql_prepared_statement;
    }
    return update_count;
}

void 
updateOrder(Connection* sql_cxn, 
        const capk::strategy_id_t& strategy_id, 
        const capkproto::execution_report& er) 
{

#ifdef DEBUG
    pan::log_DEBUG("updateOrder() called with parameters:", 
            "\nstrategy_id: ", pan::blob(strategy_id.get_uuid(), strategy_id.size()), 
            "\nexecution_report: ", er.DebugString().c_str());
#endif

    // prepare the order ids in string format
    capk::order_id_t cl_order_id(false);
    cl_order_id.set(er.cl_order_id().c_str(), er.cl_order_id().size());

    capk::order_id_t orig_cl_order_id(false);
    if (er.has_orig_cl_order_id()) {
        orig_cl_order_id.set(er.orig_cl_order_id().c_str(), er.orig_cl_order_id().size());
    }

    // insepect the execution report to determine what 
    // the current status of the order is
    capk::OrdStatus_t ordStatus = static_cast<capk::OrdStatus_t>(er.order_status());

    // CHeck order status and update order state appropriately
    if (ordStatus == capk::ORD_STATUS_NEW ||
        ordStatus == capk::ORD_STATUS_PENDING_CANCEL ||
        ordStatus == capk::ORD_STATUS_PENDING_REPLACE || 
        ordStatus == capk::ORD_STATUS_PENDING_NEW) {
        // Can't assert this since by definition new orders don't exist!
        //assert(orderExists(sql_cxn, strategy_id, cl_order_id) == true);
        return;
    }
    else if (ordStatus == capk::ORD_STATUS_REPLACE) {    
        // insert replaced order first which has a new order_id 
        // in cl_ord_id and mark the order status of the order 
        // in orig_cl_ord_id as CANCELLED
        insertOrder(sql_cxn, strategy_id, er);
        int update_count = setOrderStatus(sql_cxn, strategy_id, orig_cl_order_id, capk::ORD_STATUS_CANCELLED);
        //assert(update_count == 1);
        if (update_count !=1) {
            pan::log_CRITICAL("Update of REPLACED order failed - key violation?\nsetOrderStatus() update_count [",
                    pan::integer(update_count), 
                    "]\nstrategy_id: ", 
                    pan::blob(strategy_id.get_uuid(), strategy_id.size()), 
                    "\norig_cl_order_id: ", 
                    pan::blob(orig_cl_order_id.get_uuid(), orig_cl_order_id.size()), 
                    "capk::ORD_STATUS_CANCELLED on replacement");
        }
#ifdef DEBUG
        else {
            pan::log_NOTICE("setOrderStatus on capk::ORD_STATUS_REPLACE succeeded");
        }
#endif
    }
    else if (ordStatus == capk::ORD_STATUS_CANCELLED || 
            ordStatus == capk::ORD_STATUS_REJECTED) {
        // just update the status to a terminal state
        int update_count = setOrderStatus(sql_cxn, strategy_id, orig_cl_order_id, ordStatus);
        //assert(update_count == 1);
        if (update_count != 1) {
            pan::log_CRITICAL("Update of CANCELLED or REJECTED order failed - key violation?\nsetOrderStatus() update_count [", 
                    pan::integer(update_count), 
                    "]\nstrategy_id: ", 
                    pan::blob(strategy_id.get_uuid(), strategy_id.size()), 
                    "\norig_cl_order_id: ", 
                    pan::blob(orig_cl_order_id.get_uuid(), orig_cl_order_id.size()), 
                    "\ncapk::ORD_STATUS_REJECTED or capk::ORD_STATUS_CANCELLED");
        }
#ifdef DEBUG
        else {
            pan::log_NOTICE("setOrderStatus on capk::ORD_STATUS_CANCELLED || capk::ORD_STATUS_REJECTED succeeded");
        }
#endif
    }
    else if (ordStatus == capk::ORD_STATUS_FILL || 
            ordStatus == capk::ORD_STATUS_PARTIAL_FILL) {

        // if the order has been fully filled set it as such but don't 
        // update if it is a partial since this means part of the order
        // is still in the market
        // N.B. ThiS IMPLIES THAT AT THE END OF A STRING OF PARTIALS
        // WE GET A FILL MESSAGE!
        if (ordStatus == capk::ORD_STATUS_FILL) {
           int update_count = setOrderStatus(sql_cxn, strategy_id, cl_order_id, ordStatus); 
           if (update_count != 1) {
                pan::log_CRITICAL("Update of FILL or PARTIAL_FILL order failed - key violation?\nsetOrderStatus() update_count [", 
                        pan::integer(update_count), 
                        "]\nstrategy_id: ", 
                        pan::blob(strategy_id.get_uuid(), strategy_id.size()), 
                        "\norig_cl_order_id: ", 
                        pan::blob(orig_cl_order_id.get_uuid(), orig_cl_order_id.size()), 
                        "capk::ORD_STATUS_FILL or capk::ORD_STATUS_PARTIAL_FILL");
           }
#ifdef DEBUG
           else {
                pan::log_NOTICE("setOrderStatus on capk::ORD_STATUS_FILL || capk::ORD_STATUS_PARTIAL_FILL succeeded");
           }
#endif 
        }
        // insert the trade regardless of full or partial fill
        insertTrade(sql_cxn, 
                strategy_id, 
                er);
    }
    else {
        pan::log_CRITICAL("Order status unknown for serializing order - order follows:\n", er.DebugString().c_str());
    }

}

void
handleOrders(zmq::context_t* context, sql::Connection* sql_cxn)
{
    assert(sql_cxn);
    assert(context);

    // set up zmq listener
    int rc;
    zmq::socket_t trade_receiver_socket(*context, ZMQ_DEALER);
    trade_receiver_socket.bind(capk::kTRADE_SERIALIZATION_ADDR);
    //trade_receiver_socket.connect(capk::kTRADE_SERIALIZATION_ADDR);
    pan::log_DEBUG("Trade serialiation listenting on: ", capk::kTRADE_SERIALIZATION_ADDR); 

    bool parse_ok;
    int64_t more = 0;
    size_t more_size = sizeof(more);
    capk::strategy_id_t strategy_id(false);

    capkproto::new_order_single nos;
    //capkproto::order_cancel oc;
    //capkproto::order_cancel_replace ocr;
    capkproto::execution_report er;

    while (1) {
        do {
            // PART 1 - Strategy ID
            zmq::message_t msg_strategy_id(false);
            rc = trade_receiver_socket.recv(&msg_strategy_id);
#ifdef DEBUG
            pan::log_DEBUG("handleOrder received SID: [", 
                    pan::integer(msg_strategy_id.size()), 
                    "]",  
                    pan::blob(msg_strategy_id.data(), msg_strategy_id.size()));
#endif
            strategy_id.set(static_cast<const char*>(msg_strategy_id.data()), msg_strategy_id.size());

            // PART 2 - Message type (EXEC_RPT, ORDER_NEW, ...) 
            zmq::message_t msg_type;
            rc = trade_receiver_socket.recv(&msg_type);
            assert(rc);

            // PART 2 - Message body 
            zmq::message_t msg_body;
            rc = trade_receiver_socket.recv(&msg_body);
            assert(rc);
            trade_receiver_socket.getsockopt(ZMQ_RCVMORE, &more, &more_size);
            
            capk::msg_t protobuf_type = *(static_cast<capk::msg_t*>(msg_type.data()));

            if (protobuf_type == capk::ORDER_NEW) {
                parse_ok = nos.ParseFromArray(msg_body.data(), msg_body.size());
                assert(parse_ok);
                insertOrder(sql_cxn, strategy_id, nos);
            }
            else if (protobuf_type == capk::EXEC_RPT) {
                parse_ok = er.ParseFromArray(msg_body.data(), msg_body.size());
                assert(parse_ok);
                updateOrder(sql_cxn, strategy_id, er);
            }
            else {
                pan::log_CRITICAL("handleOrders unknown message type for protobuf (capk::msg_t) (", pan::integer(protobuf_type), ") ignored.");
            }
/*
            else if (protobuf_type == capk::ORDER_CANCEL) {
                parse_ok = oc.ParseFromArray(msg_body.data(), msg_body.size());
                assert(parse_ok);
                updateOrder(sql_cxn, strategy_id, oc);
            }
            else if (protobuf_type == capk::ORDER_REPLACE) {
                parse_ok = ocr.ParseFromArray(msg_body.data(), msg_body.size());
                assert(parse_ok);
                updateOrder(sql_cxn, strategy_id, ocr);
            }
*/
            //pan::log_DEBUG("Listener received:\n", er.DebugString());


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

    Connection *con1;
    Connection *con2;
    bool shouldAutoReconnect = true;

    /* initiate url, user, password and database variables */
    string url(argc >= 2 ? argv[1] : DBHOST);
    const string user(argc >= 3 ? argv[2] : USER);
    const string password(argc >= 4 ? argv[3] : PASSWORD);
    const string database(argc >= 5 ? argv[4] : DATABASE);

    con1 = createMysqlConnection(url.c_str(),
                                user.c_str(),
                                password.c_str(),
                                database.c_str());
    con1->setClientOption("OPT_RECONNECT", &shouldAutoReconnect);
    assert(con1);
    con2 = createMysqlConnection(url.c_str(),
                                user.c_str(),
                                password.c_str(),
                                database.c_str());
    con2->setClientOption("OPT_RECONNECT", &shouldAutoReconnect);
    assert(con2);
    pan::log_NOTICE("Connecting to database...");
    pan::log_NOTICE("dbhost: ", url.c_str());
    pan::log_NOTICE("user: ", user.c_str());
    pan::log_NOTICE("database: ", database.c_str());


    // setup logging
    std::string logFileName = createTimestampedLogFilename("trade_serializer");
    logging_init(logFileName.c_str());
    pan::log_NOTICE("Logging to: ", logFileName.c_str()); 
    // initialize zmq context
    zmq::context_t* context= new zmq::context_t(1);
    assert(context);

    try {

        /* alternate syntax using auto_ptr to create the db connection */
        //auto_ptr  con (driver -> connect(url, user, password));

        /* turn off the autocommit */
        con1 -> setAutoCommit(0);

        pan::log_NOTICE("Database connection autocommit mode = ",
               pan::integer(con1->getAutoCommit()));


        /* select appropriate database schema */
        con1 -> setSchema(database);

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
        //handleOrders(context, con1); 
        boost::thread* order_listener_thread = 
            new boost::thread(boost::bind(&handleOrders, context, con1));

        boost::thread* recovery_listener_thread = 
            new boost::thread(boost::bind(&handleRecovery, context, con2));

        order_listener_thread->join();
        recovery_listener_thread->join();
/* 
        OrderListener order_listener(&context, capk::TRADE_SERIALIZER_ADDR);
        boost::thread* order_listener_thread = new boost::thread(boost::bind(&order_listener::run, order_listener));

        RecoveryListener recovery_listener(&context, capk::RECOVERY_LISTENER_ADDR);
        boost::thread* recovery_listener_thread = new boost::thread(boost::bind(&recovery_listener::run, recovery_listener));
        
        order_listener_thread.join();
        recovery_listener_thread.join();
*/
        /* Clean up */
        pan::log_DEBUG("Cleaning up the resources ..");
        //delete res;
        //delete stmt;
        //delete prep_stmt;
        con1 -> close();
        delete con1;

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
