-- Needs test8.dsl and test9.dsl to have been executed first.
-- tbl2 has a secondary b-tree tree index on col1 and col2, and a clustered index on col7 with the form of a sorted column
-- testing for correctness - using a clustered index in the form of a sorted column on the where clause
--
-- Query in SQL:
-- SELECT col5 FROM tbl2 WHERE col7 >= 210000000 and col7 < 550000000
--
-- since col7 has a clustered index, the index is expected to be used by the select operator 
s1=select(db1.tbl2.col7,210000000,550000000)
f1=fetch(db1.tbl2.col5,s1)
tuple(f1)
