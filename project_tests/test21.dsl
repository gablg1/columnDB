-- This is a benchmark on M2 features
-- 
-- Create and load table 'tbl7'
create(tbl,"tbl7",db1,7)
create(col,"col1",db1.tbl7,unsorted)
create(col,"col2",db1.tbl7,unsorted)
create(col,"col3",db1.tbl7,unsorted)
create(col,"col4",db1.tbl7,unsorted)
create(col,"col5",db1.tbl7,unsorted)
create(col,"col6",db1.tbl7,unsorted)
create(col,"col7",db1.tbl7,sorted)


-- Load data immediately in the form of a clustered index
load("../project_tests/data7.csv")

-- Execute a large range query (similar to test 10)
--
-- Query in SQL:
-- SELECT max(col5) FROM tbl7 WHERE col7 >= 210000000 and col7 < 550000000
--
-- Since col7 has a clustered index, the index is expected to be used by the select operator 
s1=select(db1.tbl7.col7,210000000,550000000)
f1=fetch(db1.tbl7.col5,s1)
m=max(f1)
tuple(m)
