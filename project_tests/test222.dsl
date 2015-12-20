-- Needs test14.dsl to have been executed first.
-- tbl3 has a secondary b-tree tree index on col1 and col2, and a clustered index on col7 with the form of a sorted column
-- testing shared scan on 10 queries
--
s1=schedule_select(db1.tbl3.col4,78091301,118548668)
s2=schedule_select(db1.tbl3.col6,153472534,190489750)
s3=schedule_select(db1.tbl3.col3,-250003324,-153542055)
s4=schedule_select(db1.tbl3.col1,360161038,430400334)
s5=schedule_select(db1.tbl3.col2,174964847,234227183)
s6=schedule_select(db1.tbl3.col5,-367021856,-284434020)
s7=schedule_select(db1.tbl3.col7,113326821,141646368)
s8=schedule_select(db1.tbl3.col1,441002272,488716814)
s9=schedule_select(db1.tbl3.col7,-185636195,-119462531)
s10=schedule_select(db1.tbl3.col1,84546226,172851319)

execute_scheduled
