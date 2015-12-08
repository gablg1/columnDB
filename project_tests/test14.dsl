-- Test for creating table with indexes
--
-- Table tbl3 has a clustered index with col4 being the leading column. 
-- The clustered index has the form of a b-tree. 
-- The table also has two secondary indexes on col1 and col3; the secondary indexy on col1 has the form of a sorted column and the secondary index on col3 has the form of a b-tree.
--
-- Loads data from: data2.csv
--
-- Create Table
create(tbl,"tbl3",db1,7)
create(col,"col1",db1.tbl3,unsorted)
create(col,"col2",db1.tbl3,unsorted)
create(col,"col3",db1.tbl3,unsorted)
-- sorted column (it can be only one)
create(col,"col4",db1.tbl3,primary)
--
create(col,"col5",db1.tbl3,unsorted)
create(col,"col6",db1.tbl3,unsorted)
create(col,"col7",db1.tbl3,unsorted)
--
-- Load data in the clustered index and populate all secondary indexes as well. 
load("../project_tests/data2.csv")
--
-- Create two secondary indexes: a sorted secondary index on col1 and b-tree secondary index on col3
create(idx,db1.tbl3.col1,sorted)
create(idx,db1.tbl3.col3,btree)
-- Make the clustered index of b-tree form (instead of just sorted column) by declaring a b-tree on top of the leading column of the clustered index.
create(idx,db1.tbl3.col4,btree)
--
--
shutdown
