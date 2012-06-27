create database if not exists `capk.prod`;
use `capk.prod`;

create table if not exists `trades`
(
    `strategy_id` varchar(40) NOT NULL,
    `cl_order_id` varchar(40) NOT NULL,
    `orig_cl_order_id` varchar(40) NOT NULL,
    `exec_id` varchar(128),
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
    `transact_time_timestamp` timestamp DEFAULT CURRENT_TIMESTAMP,
    `transact_time_timespec` bigint,
    `mic` varchar(8),
    PRIMARY KEY (strategy_id, cl_order_id)
    
) ENGINE=InnoDB DEFAULT CHARSET=utf8
