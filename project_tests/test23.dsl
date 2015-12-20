-- Needs test14.dsl to have been executed first.
-- tbl3 has a secondary b-tree tree index on col1 and col2, and a clustered index on col7 with the form of a sorted column
-- testing shared scan on 50 queries
--
-- Query in SQL:
-- Q1: SELECT tbl3.col4 FROM tbl3 WHERE tbl3.col4 >= 78091301 and tbl3.col4 < 118548668--
-- Q2: SELECT tbl3.col6 FROM tbl3 WHERE tbl3.col6 >= 153472534 and tbl3.col6 < 190489750--
-- Q3: SELECT tbl3.col4 FROM tbl3 WHERE tbl3.col3 >= -250003324 and tbl3.col3 < -153542055--
-- Q4: SELECT tbl3.col7 FROM tbl3 WHERE tbl3.col1 >= 360161038 and tbl3.col1 < 430400334--
-- Q5: SELECT tbl3.col3 FROM tbl3 WHERE tbl3.col2 >= 174964847 and tbl3.col2 < 234227183--
-- Q6: SELECT tbl3.col5 FROM tbl3 WHERE tbl3.col5 >= -367021856 and tbl3.col5 < -284434020--
-- Q7: SELECT tbl3.col7 FROM tbl3 WHERE tbl3.col7 >= 113326821 and tbl3.col7 < 141646368--
-- Q8: SELECT tbl3.col1 FROM tbl3 WHERE tbl3.col1 >= 441002272 and tbl3.col1 < 488716814--
-- Q9: SELECT tbl3.col3 FROM tbl3 WHERE tbl3.col7 >= -185636195 and tbl3.col7 < -119462531--
-- Q10: SELECT tbl3.col4 FROM tbl3 WHERE tbl3.col1 >= 84546226 and tbl3.col1 < 172851319--
-- Q11: SELECT tbl3.col2 FROM tbl3 WHERE tbl3.col2 >= -87129792 and tbl3.col2 < -59360427--
-- Q12: SELECT tbl3.col4 FROM tbl3 WHERE tbl3.col6 >= 157890939 and tbl3.col6 < 222902219--
-- Q13: SELECT tbl3.col6 FROM tbl3 WHERE tbl3.col2 >= 67235477 and tbl3.col2 < 120298588--
-- Q14: SELECT tbl3.col5 FROM tbl3 WHERE tbl3.col1 >= 275512701 and tbl3.col1 < 307758138--
-- Q15: SELECT tbl3.col2 FROM tbl3 WHERE tbl3.col6 >= 447693925 and tbl3.col6 < 505186288--
-- Q16: SELECT tbl3.col3 FROM tbl3 WHERE tbl3.col2 >= -225296608 and tbl3.col2 < -154002650--
-- Q17: SELECT tbl3.col2 FROM tbl3 WHERE tbl3.col1 >= -44265141 and tbl3.col1 < -6894806--
-- Q18: SELECT tbl3.col5 FROM tbl3 WHERE tbl3.col6 >= 277511390 and tbl3.col6 < 338521394--
-- Q19: SELECT tbl3.col3 FROM tbl3 WHERE tbl3.col4 >= -494991426 and tbl3.col4 < -416548436--
-- Q20: SELECT tbl3.col3 FROM tbl3 WHERE tbl3.col3 >= -420074968 and tbl3.col3 < -361470774--
-- Q21: SELECT tbl3.col5 FROM tbl3 WHERE tbl3.col3 >= 369545546 and tbl3.col3 < 445047037--
-- Q22: SELECT tbl3.col2 FROM tbl3 WHERE tbl3.col4 >= 410533818 and tbl3.col4 < 474542045--
-- Q23: SELECT tbl3.col5 FROM tbl3 WHERE tbl3.col1 >= -10613883 and tbl3.col1 < 48994212--
-- Q24: SELECT tbl3.col3 FROM tbl3 WHERE tbl3.col3 >= 84935049 and tbl3.col3 < 150335078--
-- Q25: SELECT tbl3.col4 FROM tbl3 WHERE tbl3.col2 >= -58653183 and tbl3.col2 < 38997682--
-- Q26: SELECT tbl3.col3 FROM tbl3 WHERE tbl3.col1 >= -497169574 and tbl3.col1 < -432013190--
-- Q27: SELECT tbl3.col1 FROM tbl3 WHERE tbl3.col1 >= -391243588 and tbl3.col1 < -332187713--
-- Q28: SELECT tbl3.col7 FROM tbl3 WHERE tbl3.col4 >= -41359716 and tbl3.col4 < 16378595--
-- Q29: SELECT tbl3.col4 FROM tbl3 WHERE tbl3.col4 >= 226215201 and tbl3.col4 < 318560817--
-- Q30: SELECT tbl3.col5 FROM tbl3 WHERE tbl3.col3 >= 149529118 and tbl3.col3 < 242744663--
-- Q31: SELECT tbl3.col2 FROM tbl3 WHERE tbl3.col2 >= 119112000 and tbl3.col2 < 195638496--
-- Q32: SELECT tbl3.col5 FROM tbl3 WHERE tbl3.col5 >= 329423194 and tbl3.col5 < 390343915--
-- Q33: SELECT tbl3.col6 FROM tbl3 WHERE tbl3.col7 >= -64643317 and tbl3.col7 < 24770102--
-- Q34: SELECT tbl3.col6 FROM tbl3 WHERE tbl3.col7 >= -453764114 and tbl3.col7 < -360281715--
-- Q35: SELECT tbl3.col6 FROM tbl3 WHERE tbl3.col7 >= 481319360 and tbl3.col7 < 527730790--
-- Q36: SELECT tbl3.col5 FROM tbl3 WHERE tbl3.col1 >= 95132764 and tbl3.col1 < 189101240--
-- Q37: SELECT tbl3.col2 FROM tbl3 WHERE tbl3.col3 >= -420254323 and tbl3.col3 < -380655100--
-- Q38: SELECT tbl3.col2 FROM tbl3 WHERE tbl3.col1 >= 499698038 and tbl3.col1 < 591263085--
-- Q39: SELECT tbl3.col4 FROM tbl3 WHERE tbl3.col6 >= 195236794 and tbl3.col6 < 259420890--
-- Q40: SELECT tbl3.col3 FROM tbl3 WHERE tbl3.col5 >= 55519873 and tbl3.col5 < 100461610--
-- Q41: SELECT tbl3.col3 FROM tbl3 WHERE tbl3.col5 >= 127128892 and tbl3.col5 < 167848424--
-- Q42: SELECT tbl3.col5 FROM tbl3 WHERE tbl3.col7 >= 371337450 and tbl3.col7 < 445257198--
-- Q43: SELECT tbl3.col1 FROM tbl3 WHERE tbl3.col3 >= -410977715 and tbl3.col3 < -381833981--
-- Q44: SELECT tbl3.col2 FROM tbl3 WHERE tbl3.col4 >= 259399505 and tbl3.col4 < 331562621--
-- Q45: SELECT tbl3.col2 FROM tbl3 WHERE tbl3.col5 >= 6506961 and tbl3.col5 < 69571682--
-- Q46: SELECT tbl3.col2 FROM tbl3 WHERE tbl3.col4 >= -158474130 and tbl3.col4 < -78983150--
-- Q47: SELECT tbl3.col4 FROM tbl3 WHERE tbl3.col4 >= -22991906 and tbl3.col4 < 38001447--
-- Q48: SELECT tbl3.col5 FROM tbl3 WHERE tbl3.col6 >= 301096974 and tbl3.col6 < 356491240--
-- Q49: SELECT tbl3.col6 FROM tbl3 WHERE tbl3.col7 >= 492349412 and tbl3.col7 < 571398582--
-- Q50: SELECT tbl3.col1 FROM tbl3 WHERE tbl3.col6 >= 336456972 and tbl3.col6 < 362680808--
s1=schedule_select(tbl3.col4,78091301,118548668)
s2=schedule_select(tbl3.col6,153472534,190489750)
s3=schedule_select(tbl3.col3,250003324,153542055)
s4=schedule_select(tbl3.col1,360161038,430400334)
s5=schedule_select(tbl3.col2,174964847,234227183)
s6=schedule_select(tbl3.col5,367021856,284434020)
s7=schedule_select(tbl3.col7,113326821,141646368)
s8=schedule_select(tbl3.col1,441002272,488716814)
s9=schedule_select(tbl3.col7,185636195,119462531)
s10=schedule_select(tbl3.col1,84546226,172851319)
s11=schedule_select(tbl3.col2,87129792,59360427)
s12=schedule_select(tbl3.col6,157890939,222902219)
s13=schedule_select(tbl3.col2,67235477,120298588)
s14=schedule_select(tbl3.col1,275512701,307758138)
s15=schedule_select(tbl3.col6,447693925,505186288)
s16=schedule_select(tbl3.col2,225296608,154002650)
s17=schedule_select(tbl3.col1,44265141,6894806)
s18=schedule_select(tbl3.col6,277511390,338521394)
s19=schedule_select(tbl3.col4,494991426,416548436)
s20=schedule_select(tbl3.col3,420074968,361470774)
s21=schedule_select(tbl3.col3,369545546,445047037)
s22=schedule_select(tbl3.col4,410533818,474542045)
s23=schedule_select(tbl3.col1,10613883,48994212)
s24=schedule_select(tbl3.col3,84935049,150335078)
s25=schedule_select(tbl3.col2,58653183,38997682)
s26=schedule_select(tbl3.col1,497169574,432013190)
s27=schedule_select(tbl3.col1,391243588,332187713)
s28=schedule_select(tbl3.col4,41359716,16378595)
s29=schedule_select(tbl3.col4,226215201,318560817)
s30=schedule_select(tbl3.col3,149529118,242744663)
s31=schedule_select(tbl3.col2,119112000,195638496)
s32=schedule_select(tbl3.col5,329423194,390343915)
s33=schedule_select(tbl3.col7,64643317,24770102)
s34=schedule_select(tbl3.col7,453764114,360281715)
s35=schedule_select(tbl3.col7,481319360,527730790)
s36=schedule_select(tbl3.col1,95132764,189101240)
s37=schedule_select(tbl3.col3,420254323,380655100)
s38=schedule_select(tbl3.col1,499698038,591263085)
s39=schedule_select(tbl3.col6,195236794,259420890)
s40=schedule_select(tbl3.col5,55519873,100461610)
s41=schedule_select(tbl3.col5,127128892,167848424)
s42=schedule_select(tbl3.col7,371337450,445257198)
s43=schedule_select(tbl3.col3,410977715,381833981)
s44=schedule_select(tbl3.col4,259399505,331562621)
s45=schedule_select(tbl3.col5,6506961,69571682)
s46=schedule_select(tbl3.col4,158474130,78983150)
s47=schedule_select(tbl3.col4,22991906,38001447)
s48=schedule_select(tbl3.col6,301096974,356491240)
s49=schedule_select(tbl3.col7,492349412,571398582)
s50=schedule_select(tbl3.col6,336456972,362680808)
execute_scheduled
r1=fetch(tbl3.col4,s1)
tuple(r1)
r2=fetch(tbl3.col6,s2)
tuple(r2)
r3=fetch(tbl3.col4,s3)
tuple(r3)
r4=fetch(tbl3.col7,s4)
tuple(r4)
r5=fetch(tbl3.col3,s5)
tuple(r5)
r6=fetch(tbl3.col5,s6)
tuple(r6)
r7=fetch(tbl3.col7,s7)
tuple(r7)
r8=fetch(tbl3.col1,s8)
tuple(r8)
r9=fetch(tbl3.col3,s9)
tuple(r9)
r10=fetch(tbl3.col4,s10)
tuple(r10)
r11=fetch(tbl3.col2,s11)
tuple(r11)
r12=fetch(tbl3.col4,s12)
tuple(r12)
r13=fetch(tbl3.col6,s13)
tuple(r13)
r14=fetch(tbl3.col5,s14)
tuple(r14)
r15=fetch(tbl3.col2,s15)
tuple(r15)
r16=fetch(tbl3.col3,s16)
tuple(r16)
r17=fetch(tbl3.col2,s17)
tuple(r17)
r18=fetch(tbl3.col5,s18)
tuple(r18)
r19=fetch(tbl3.col3,s19)
tuple(r19)
r20=fetch(tbl3.col3,s20)
tuple(r20)
r21=fetch(tbl3.col5,s21)
tuple(r21)
r22=fetch(tbl3.col2,s22)
tuple(r22)
r23=fetch(tbl3.col5,s23)
tuple(r23)
r24=fetch(tbl3.col3,s24)
tuple(r24)
r25=fetch(tbl3.col4,s25)
tuple(r25)
r26=fetch(tbl3.col3,s26)
tuple(r26)
r27=fetch(tbl3.col1,s27)
tuple(r27)
r28=fetch(tbl3.col7,s28)
tuple(r28)
r29=fetch(tbl3.col4,s29)
tuple(r29)
r30=fetch(tbl3.col5,s30)
tuple(r30)
r31=fetch(tbl3.col2,s31)
tuple(r31)
r32=fetch(tbl3.col5,s32)
tuple(r32)
r33=fetch(tbl3.col6,s33)
tuple(r33)
r34=fetch(tbl3.col6,s34)
tuple(r34)
r35=fetch(tbl3.col6,s35)
tuple(r35)
r36=fetch(tbl3.col5,s36)
tuple(r36)
r37=fetch(tbl3.col2,s37)
tuple(r37)
r38=fetch(tbl3.col2,s38)
tuple(r38)
r39=fetch(tbl3.col4,s39)
tuple(r39)
r40=fetch(tbl3.col3,s40)
tuple(r40)
r41=fetch(tbl3.col3,s41)
tuple(r41)
r42=fetch(tbl3.col5,s42)
tuple(r42)
r43=fetch(tbl3.col1,s43)
tuple(r43)
r44=fetch(tbl3.col2,s44)
tuple(r44)
r45=fetch(tbl3.col2,s45)
tuple(r45)
r46=fetch(tbl3.col2,s46)
tuple(r46)
r47=fetch(tbl3.col4,s47)
tuple(r47)
r48=fetch(tbl3.col5,s48)
tuple(r48)
r49=fetch(tbl3.col6,s49)
tuple(r49)
r50=fetch(tbl3.col1,s50)
tuple(r50)
