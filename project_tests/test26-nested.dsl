-- Needs test25.dsl to have been executed first.
-- Testing for correctness - simple hashjoin between two tables. The columns based on which we join are also
-- filtered upon.  
--
-- Query in SQL:
-- SELECT tbl4.col1, tbl5.col1
-- FROM tbl4, tbl5
-- WHERE tbl4.col1 = tbl5.col1
-- AND tbl4.col1 >= 20000 AND tbl4.col1 < 40000
-- AND tbl5.col1 >= 30000 AND tbl5.col1 < 70000;

positions1=select(db1.tbl4.col1,20000,40000)
positions2=select(db1.tbl5.col1,30000,70000)
values1=fetch(db1.tbl4.col1, positions1)
values2=fetch(db1.tbl5.col1, positions2)
r1,r2=nested_join(positions1,values1,positions2,values2)

final1=fetch(db1.tbl4.col1, r1)
final2=fetch(db1.tbl5.col1, r2)
tuple_vectors(final1, final2)

