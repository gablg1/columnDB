-- Needs test14.dsl and test15.dsl to have been executed first.
-- tbl3 has a secondary b-tree index on col3, a secondary index on col1, and a btree clustered index on col4
-- testing for correctness - using a clustered index with the form of a btree on the where clause
--
-- Query in SQL:
-- SELECT col5 FROM tbl3 WHERE col4 >= 440000000 and col4 < 550000000
--
-- since the column has an btree clustered index, the index is expected to be used
s1=select(db1.tbl3.col4,440000000,550000000)
f1=fetch(db1.tbl3.col5,s1)
tuple(f1)
