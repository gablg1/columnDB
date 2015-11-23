-- Needs test8.dsl to have been executed first.
-- Correctness test: Do inserts in tbl2. 
-- 
-- Table tbl2 has two secondary indexes and one clustered index, so, all should be maintained when we insert new data.
-- This means that the table should be always sorted on col7 and the secondary indexes on col1 and col2 should be updated
--
-- Insert new data
relational_insert(db1.tbl2,-1,-11,-111,-1111,-11111,1,11)
relational_insert(db1.tbl2,-2,-22,-222,-2222,-11111,1,11)
relational_insert(db1.tbl2,-3,-33,-333,-2222,-11111,1,11)
relational_insert(db1.tbl2,-4,-44,-444,-2222,-11111,1,11)
relational_insert(db1.tbl2,-5,-55,-555,-2222,-15111,1,11)
--
-- Testing that the new updates are durable on the disk
shutdown
