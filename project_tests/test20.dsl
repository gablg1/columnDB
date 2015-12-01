-- Benchmark test for M1
--
-- Create table tbl6
create(tbl,"tbl6",db1,10)
create(col,"col1",db1.tbl6,unsorted)
create(col,"col2",db1.tbl6,unsorted)
create(col,"col3",db1.tbl6,unsorted)
create(col,"col4",db1.tbl6,unsorted)
create(col,"col5",db1.tbl6,unsorted)
create(col,"col6",db1.tbl6,unsorted)
create(col,"col7",db1.tbl6,unsorted)
create(col,"col8",db1.tbl6,unsorted)
create(col,"col9",db1.tbl6,unsorted)
create(col,"col10",db1.tbl6,unsorted)
-- Load data
load("data6.csv")
--
-- Execute test
--
-- Addition
--
-- SELECT MAX(col2 + col3) FROM tbl6 WHERE col7 >= 127 AND col7 < 123456789;
s1=select(db1.tbl6.col7,127,123456789)
f1=fetch(col2,s1)
f2=fetch(col3,s1)
a1=add(f1,f2)
m=max(a1)
tuple(m)
