-- Test Select + Fetch
--
-- SELECT col1 FROM tbl1 WHERE col1 < -2100000000;
s1=select(db1.tbl1.col1,null,-2100000000)
f1=fetch(db1.tbl1.col1,s1)
tuple(f1)
--
-- SELECT col2 FROM tbl1 WHERE col1 >= 2100000000;
s2=select(db1.tbl1.col1,2100000000,null)
f2=fetch(db1.tbl1.col2,s2)
tuple(f2)
--
-- SELECT col9 FROM tbl1 WHERE col10 >= -1000 AND col10 < 1000;
s3=select(db1.tbl1.col10,-1000,1000)
f3=fetch(db1.tbl1.col9,s3)
tuple(f3)
--
-- SELECT col9 FROM tbl1 WHERE col10 >= -12000 AND col10 < 12000;
s4=select(db1.tbl1.col10,-12000,12000)
f4=fetch(db1.tbl1.col9,s4)
tuple(f4)
