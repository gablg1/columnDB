-- Needs test8.dsl and test9.dsl to have been executed first.
-- tbl2 has a secondary b-tree tree index on col1 and col2, and a clustered index on col7 with the form of a sorted column
-- testing for correctness - using a scan on the where clause (the column has no index)
--
-- Query in SQL:
-- SELECT col5 FROM tbl2 WHERE col3 >= 330000000 and col3 < 440000000
--
-- since the column has no index this is a scan
s1=select(db1.tbl2.col3,330000000,440000000)
f1=fetch(db1.tbl2.col5,s1)
tuple(f1)
