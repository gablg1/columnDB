-- Test for creating table with indexes
--
-- Table tbl2 has a clustered index with col7 being the leading column. 
-- The clustered index has the form of a sorted column. 
-- The table also has two secondary indexes on col1 and col2; both secondary indexes are b-trees. 
--
-- Loads data from: data1.csv
--
-- Create Table
create(tbl,"tbl2",db1,7)
create(col,"col1",db1.tbl2,unsorted)
create(col,"col2",db1.tbl2,unsorted)
create(col,"col3",db1.tbl2,unsorted)
create(col,"col4",db1.tbl2,unsorted)
create(col,"col5",db1.tbl2,unsorted)
create(col,"col6",db1.tbl2,unsorted)
-- sorted column (it can be only one. the table should be maintained ordered on column 7 throughout execution)
--create(col,"col7",db1.tbl2,primary)
create(col,"col7",db1.tbl2,unsorted)
--
--
-- Load data immediately in the form of a clustered index
load("../project_tests/data1.csv")
--
-- Create two secondary indexes on col1 and col2 - They are expected to get bulk loaded with all the data we have already.
--create(idx,db1.tbl2.col1,btree)
--create(idx,db1.tbl2.col2,btree)
--
-- Testing that the data and their indexes are durable on disk.
shutdown
