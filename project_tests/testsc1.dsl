-- Needs test14.dsl to have been executed first.
-- tbl3 has a secondary b-tree tree index on col1 and col2, and a clustered index on col7 with the form of a sorted column
-- testing shared scan on 10 queries
--
s1=schedule_select(db1.tbl5.col4,78091301,118548668)
s2=schedule_select(db1.tbl5.col4,153472534,190489750)
s3=schedule_select(db1.tbl5.col4,-250003324,-153542055)
s4=schedule_select(db1.tbl5.col4,360161038,430400334)
s5=schedule_select(db1.tbl5.col4,174964847,234227183)
s6=schedule_select(db1.tbl5.col4,-367021856,-284434020)
s7=schedule_select(db1.tbl5.col4,113326821,141646368)
s8=schedule_select(db1.tbl5.col4,441002272,488716814)
s9=schedule_select(db1.tbl5.col4,-185636195,-119462531)
s10=schedule_select(db1.tbl5.col4,84546226,172851319)

execute_scheduled

s1=schedule_select(db1.tbl5.col4,78091301,118548668)
s2=schedule_select(db1.tbl5.col4,153472534,190489750)
s3=schedule_select(db1.tbl5.col4,-250003324,-153542055)
s4=schedule_select(db1.tbl5.col4,360161038,430400334)
s5=schedule_select(db1.tbl5.col4,174964847,234227183)
s6=schedule_select(db1.tbl5.col4,-367021856,-284434020)
s7=schedule_select(db1.tbl5.col4,113326821,141646368)
s8=schedule_select(db1.tbl5.col4,441002272,488716814)
s9=schedule_select(db1.tbl5.col4,-185636195,-119462531)
s10=schedule_select(db1.tbl5.col4,84546226,172851319)

execute_scheduled_sequentially
size(db1.tbl5)
