-- Needs test8.dsl and test9.dsl to have been executed first.
-- tbl2 has a secondary b-tree index on col1 and col2, and a clustered index on col7 with the form of a sorted column
-- testing for correctness - using a secondary tree index on the where clause
--
-- Query in SQL:
-- SELECT col5 FROM tbl2 WHERE col1 >= 220000000 and col1 < 330000000
--
-- since col1 has a secondary tree index, the index is expected to be used by the select operator 
s1=select(db1.tbl2.col1,220000000,330000000)
f1=fetch(db1.tbl2.col5,s1)
tuple(f1)
