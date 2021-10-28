\c DynData
CREATE TABLE mData (ts timestamp NOT NULL, 
			value real, 
			tag character(63) NOT NULL, 
			textVal character(100));
SELECT create_hypertable('mdata', 'ts');
SELECT add_retention_policy('mdata', INTERVAL '3 years');
select set_chunk_time_interval('mdata', INTERVAL '7 days');