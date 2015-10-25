-- Needs test8.dsl and test9.dsl to have been executed first.
-- tbl2 has a secondary b-tree tree index on col1 and col2, and a clustered index on col7 with the form of a sorted column
-- testing for correctness - using a column that has no index (will also be used for testing time)
--
-- Query in SQL:
-- SELECT col5 FROM tbl2 WHERE col4 >= 440000000 and col4 < 550000000
--
-- since the column has no index this is a scan
s1=select(db1.tbl2.col4,440000000,550000000)
f1=fetch(db1.tbl2.col5,s1)
tuple(f1)
