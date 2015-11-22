-- Average
--
-- SELECT avg(col5) FROM tbl1 WHERE col7 >= 127 AND col7 < 1234567890;
s1=select(db1.tbl1.col7,127,1234567890)
f1=fetch(db1.tbl1.col5,s1)
a1=avg(f1)
tuple(a1)
--
-- SELECT avg(col7) FROM tbl1;
a2=avg(db1.tbl1.col7)
tuple(a2)
