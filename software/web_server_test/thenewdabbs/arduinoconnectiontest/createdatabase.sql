CREATE DATABASE thenewd1_arduinoconnectiontest;
USE thenewd1_arduinoconnectiontest;

CREATE TABLE tbl_Data (
	DataId int NOT NULL AUTO_INCREMENT,
	DataValue varchar(255) NOT NULL,
	CreatedTime timestamp DEFAULT CURRENT_TIMESTAMP,
	PRIMARY KEY (DataId)
) ENGINE=INNODB;