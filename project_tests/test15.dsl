-- Needs test14.dsl to have been executed first.
-- Correctness test: Do inserts in tbl3. 
-- 
-- Table tbl3 has three indexes, so, all should be maintained.
-- It needs to maintain a b-tree index over the leading column col4, and it has to maintain a sorted secondary index on col1 and a b-tree secondary index on col3.
--
-- Insert new data
relational_insert(db1.tbl3,-1,-11,-111,-1111,-11111,1,11)
relational_insert(db1.tbl3,-2,-22,-222,-2222,-11111,1,11)
relational_insert(db1.tbl3,-3,-33,-333,-2222,-11111,1,11)
relational_insert(db1.tbl3,-4,-44,-444,-2222,-11111,1,11)
relational_insert(db1.tbl3,-5,-55,-555,-2222,-15111,1,11)
--
-- Testing that the new updates are durable on the disk
shutdown
