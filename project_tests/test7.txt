-- Final Queries
--
-- SELECT max(col1) FROM tbl1 WHERE col2 < 20 AND col3 >= 30;
s1=select(db1.tbl1.col2,null,20)
f1=fetch(db1.tbl1.col3,s1)
s2=select(s1,f1,30,null)
f2=fetch(db1.tbl1.col1,s2)
r=max(f2)
tuple(r)
