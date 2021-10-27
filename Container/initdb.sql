\c DynData
CREATE TABLE mData (ts timestamp NOT NULL, 
			value real, 
			tag character(63) NOT NULL, 
			textVal character(100));
SELECT create_hypertable('mData', 'ts');