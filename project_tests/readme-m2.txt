README MILESTONE 2 TESTS
-=-=-=-=-=-=-=-=-=-=-=-=

In this milestone, we test the functionality to create, maintain and presist 
index structures in your data system. In addition, we expect single 
table queries to exploit the presence of indexes to improve their 
performance. 

Each of the test files (test8.dsl to test19.dsl) are self-explanitory and 
properly commented to explain what is expected from the system. We will 
explain how to use a combination of test files to test for specific 
scenarios.

NOTE: 'tbl2' and 'tbl3' have the same data organized in 7 columns. However they 
have different index structures constructed on different columns. We will use 
this organization to compare the following setups:

* Setup 1: Scan vs. secondary b-tree index
* Setup 2: Scan vs. clustered b-tree index
* Setup 3: Secondary b-tree index vs. secondary index in the form of a sorted column

Setup 1: Scan vs. secondary b-tree Index
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
test12 and test17 do a SELECT on 'col3' from 'tbl2' and 'tbl3' respectively. 'tbl2' 
has no index on 'col3' whereas 'tbl3' has a secondary b-tree index on 'col3'. We 
expect test17 to use the secondary index and perform better than test12.


Setup 2: Scan vs. clustered b-tree Index
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
test13 and test18 do a SELECT on 'col4' from 'tbl2' and 'tbl3' respectively. 
'tbl2' has no index on 'col4' whereas 'tbl3' has a clustered b-tree index on 'col4'.
We expect test18 to use the clustered index and perform better than test13.


Setup 3: Secondary b-tree index vs. 
Secondary index in the form of a sorted column
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
test11 and test16 do a select on 'col1' from 'tbl2' and 'tbl3' respectively. 
'tbl2' has secondary btree index on 'col1' whereas 'tbl3' has a secondary index 
in the form of a sorted column on 'col1'. This test will compare between the 
performance of the two secondary index structures.


Comments
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
We expect all tests to be executed as follows:
"make testX"
which would run the testX.dsl as input in the client.
It should be expected that the server is already running.















