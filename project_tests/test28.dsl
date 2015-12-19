-- Needs test25.dsl to have been executed first.
-- Testing for correctness - simple hashjoin between two tables. Columns other than those on which we join
-- are filtered upon. We aggregate the results. 
-- 
-- Query in SQL:
-- SELECT max(tbl4.col4 + tbl5.col5)
-- FROM tbl4, tbl5
-- WHERE tbl4.col1 = tbl5.col1
-- AND tbl4.col2 >= 10000 AND tbl4.col2 < 14000
-- AND tbl5.col3 >= 15000 AND tbl5.col3 < 62000;

positions1=select(db1.tbl4.col2,10000,14000)
positions2=select(db1.tbl5.col3,15000,62000)
values1=fetch(db1.tbl4.col1, positions1)
values2=fetch(db1.tbl5.col1, positions2)
r1,r2=nested_join(positions1,values1,positions2,values2)

final1=fetch(db1.tbl4.col4, r1)
final2=fetch(db1.tbl5.col5, r2)
a=add(final1, final2)
m=max(a)
tuple(m)

