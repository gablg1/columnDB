-- DDL file for Milestone 4 (Note, tbl4 and tbl5 are structured identically)
--
-- Table tbl4 and tbl5 have a clustered index with col7 being the leading column.
-- The clustered index has the form of a sorted column.
-- The table also has two secondary indexes on col1 and col2; both secondary indexes are b-trees.
--
-- Create Table4
create(tbl,"tbl4",db1,7)
create(col,"col1",db1.tbl4,unsorted)
create(col,"col2",db1.tbl4,unsorted)
create(col,"col3",db1.tbl4,unsorted)
create(col,"col4",db1.tbl4,unsorted)
create(col,"col5",db1.tbl4,unsorted)
create(col,"col6",db1.tbl4,unsorted)
-- sorted column (it can be only one. the table should be maintained ordered on column 7 throughout execution)
create(col,"col7",db1.tbl4,sorted)
--
-- Create Table5
create(tbl,"tbl5",db1,7)
create(col,"col1",db1.tbl5,unsorted)
create(col,"col2",db1.tbl5,unsorted)
create(col,"col3",db1.tbl5,unsorted)
create(col,"col4",db1.tbl5,unsorted)
create(col,"col5",db1.tbl5,unsorted)
create(col,"col6",db1.tbl5,unsorted)
-- sorted column (it can be only one. the table should be maintained ordered on column 7 throughout execution)
create(col,"col7",db1.tbl5,sorted)
--
-- Load data immediately in the form of a clustered index
load("../project_tests/data4.csv")
load("../project_tests/data5.csv")
--
-- Create two secondary indexes on col1 and col2 - They are expected to get bulk loaded with all the data we have already.
create(idx,db1.tbl4.col1,btree)
create(idx,db1.tbl4.col2,btree)
create(idx,db1.tbl5.col1,btree)
create(idx,db1.tbl5.col2,btree)
--
-- Testing that the data and indexes are durable on disk.
positions1=select(db1.tbl4.col1,20000,40000)
shutdown
