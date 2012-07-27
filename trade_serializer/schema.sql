create database if not exists `capk.prod`;
use `capk.prod`;
drop table if exists `order_state`;
/*
create table if not exists `status_types`
(
    `order_status_type` tinyint,
    `order_status_desc` varchar(32)
    PRIMARY KEY(order_status_type)
) ENGINE=InnoDB DEFAULT CHARSET=utf8
insert into status_types (order_status_type, order_status_desc) values (0, 'WORKING');
insert into status_types (order_status_type, order_status_desc) values (1, 'CANCELLED');
insert into status_types (order_status_type, order_status_desc) values (2, 'FILLED');
insert into status_types (order_status_type, order_status_desc) values (3, 'REJECTED');
*/
/*
create table if not exists `orders`
(
    `strategy_id` varchar(40) NOT NULL,
    `cl_order_id` varchar(40) NOT NULL,
    `orig_cl_order_id` varchar(40) NOT NULL,
    `exec_id` varchar(128) NOT NULL,
    `exec_trans_type` varchar(1),
    `order_status` varchar(1),
    `exec_type` varchar(1), 
    `symbol` varchar(8) NOT NULL, 
    `security_type` varchar(6),
    `side` tinyint NOT NULL,
    `order_qty` double NOT NULL, 
    `ord_type` tinyint, 
    `price` double NOT NULL, 
    `last_shares` double, 
    `last_price` double, 
    `leaves_qty` double, 
    `cum_qty` double, 
    `avg_price` double, 
    `time_in_force` tinyint,
    `transact_time` varchar(64), 
    `exec_inst` varchar(16), 
    `handl_inst` tinyint, 
    `order_reject_reason` tinyint, 
    `min_qty` double,
    `current_time_timestamp` timestamp DEFAULT CURRENT_TIMESTAMP,
    `current_time_timespec` bigint,
    `venue_id` int,
    `account` varchar(16)
    
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

create index index_prefix_strategy_id on orders (strategy_id(13));
create index index_prefix_cl_order_id on orders (cl_order_id(13));
create index index_prefix_orig_cl_order_id on orders (orig_cl_order_id(13));
create index index_exec_id on orders (exec_id);
*/
create table if not exists `order_state`
(
    `strategy_id` varchar(40) NOT NULL,
    `cl_order_id` varchar(40) NOT NULL,
    `orig_cl_order_id` varchar(40),
    `order_status` int,
    `symbol` varchar(8) NOT NULL, 
    `side` tinyint NOT NULL,
    `order_qty` double NOT NULL, 
    `ord_type` tinyint, 
    `price` double NOT NULL, 
    `time_in_force` tinyint,
    `transact_time` varchar(64), 
    `current_time_timestamp` timestamp DEFAULT CURRENT_TIMESTAMP,
    `current_time_timespec` bigint,
    `venue_id` int NOT NULL,
    `account` varchar(16),
    PRIMARY KEY(strategy_id, cl_order_id)
    
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

create index index_prefix_strategy_id on order_state (strategy_id(13));
create index index_prefix_cl_order_id on order_state (cl_order_id(13));
create index index_prefix_orig_cl_order_id on order_state (orig_cl_order_id(13));
create index index_order_status on order_state (order_status);



create table if not exists `trades`
(
    `strategy_id` varchar(40) NOT NULL,
    `cl_order_id` varchar(40) NOT NULL,
    `orig_cl_order_id` varchar(40) NOT NULL,
    `exec_id` varchar(128) NOT NULL,
    `exec_trans_type` int,
    `order_status` int,
    `exec_type` int, 
    `symbol` varchar(8) NOT NULL, 
    `security_type` varchar(6),
    `side` tinyint NOT NULL,
    `order_qty` double NOT NULL, 
    `ord_type` tinyint, 
    `price` double NOT NULL, 
    `last_shares` double, 
    `last_price` double, 
    `leaves_qty` double, 
    `cum_qty` double, 
    `avg_price` double, 
    `time_in_force` tinyint,
    `transact_time` varchar(64), 
    `exec_inst` varchar(16), 
    `handl_inst` tinyint, 
    `order_reject_reason` tinyint, 
    `min_qty` double,
    `current_time_timestamp` timestamp DEFAULT CURRENT_TIMESTAMP,
    `current_time_timespec` bigint,
    `venue_id` int,
    `account` varchar(16)
    
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

create index index_prefix_strategy_id on trades (strategy_id(13));
create index index_prefix_cl_order_id on trades (cl_order_id(13));
create index index_prefix_orig_cl_order_id on trades (orig_cl_order_id(13));
create index index_exec_id on trades (exec_id);
