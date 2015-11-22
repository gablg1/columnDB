-- Subtraction
--
-- SELECT col2 - col3 FROM tbl1 WHERE col7 >= 127 AND col7 < 123456789;
s1=select(db1.tbl1.col7,127,123456789)
f1=fetch(db1.tbl1.col2,s1)
f2=fetch(db1.tbl1.col3,s1)
sub1=sub(f1,f2)
tuple(sub1)
