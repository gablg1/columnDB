-- Needs test14.dsl to have been executed first.
-- tbl3 has a secondary b-tree tree index on col1 and col2, and a clustered index on col7 with the form of a sorted column
-- testing shared scan on 10 queries
--
-- Query in SQL:
-- Q1: SELECT tbl3.col4 FROM tbl3 WHERE tbl3.col4 >= 78091301 and tbl3.col4 < 118548668--
s1=select(db1.tbl3.col4,78091301,118548668)
f1=fetch(db1.tbl3.col4,s1)
tuple(f1)

-- Q2: SELECT tbl3.col6 FROM tbl3 WHERE tbl3.col6 >= 153472534 and tbl3.col6 < 190489750--
s2=select(db1.tbl3.col6,153472534,190489750)
f2=fetch(db1.tbl3.col6,s2)
tuple(f2)

-- Q3: SELECT tbl3.col4 FROM tbl3 WHERE tbl3.col3 >= -250003324 and tbl3.col3 < -153542055--
s3=select(db1.tbl3.col3,-25003324,-153542055)
f3=fetch(db1.tbl3.col4,s3)
tuple(f3)

-- Q4: SELECT tbl3.col7 FROM tbl3 WHERE tbl3.col1 >= 360161038 and tbl3.col1 < 430400334--
s4=select(db1.tbl3.col1,360161038,430400334)
f4=fetch(db1.tbl3.col7,s4)
tuple(f4)

-- Q5: SELECT tbl3.col3 FROM tbl3 WHERE tbl3.col2 >= 174964847 and tbl3.col2 < 234227183--
s5=select(db1.tbl3.col2,174964847,234227183)
f5=fetch(db1.tbl3.col3,s5)
tuple(f5)

-- Q6: SELECT tbl3.col5 FROM tbl3 WHERE tbl3.col5 >= -367021856 and tbl3.col5 < -284434020--
s6=select(db1.tbl3.col5,-367021856,-284434020)
f6=fetch(db1.tbl3.col6,s6)
tuple(f6)

-- Q7: SELECT tbl3.col7 FROM tbl3 WHERE tbl3.col7 >= 113326821 and tbl3.col7 < 141646368--
s7=select(db1.tbl3.col7,113326821,141646368)
f7=fetch(db1.tbl3.col7,s7)
tuple(f7)

-- Q8: SELECT tbl3.col1 FROM tbl3 WHERE tbl3.col1 >= 441002272 and tbl3.col1 < 488716814--
s8=select(db1.tbl3.col1,441002272,488716814)
f8=fetch(db1.tbl3.col1,s8)
tuple(f8)

-- Q9: SELECT tbl3.col3 FROM tbl3 WHERE tbl3.col7 >= -185636195 and tbl3.col7 < -119462531--
s9=select(db1.tbl3.col7,-185636195,-119462531)
f9=fetch(db1.tbl3.col3,s9)
tuple(f9)

-- Q10: SELECT tbl3.col4 FROM tbl3 WHERE tbl3.col1 >= 84546226 and tbl3.col1 < 172851319--
s10=select(db1.tbl3.col1,84546226,172851319)
f10=fetch(db1.tbl3.col4,s10)
tuple(f10)
