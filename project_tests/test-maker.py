
fname = 'test23.dsl'
col = 'col4'

with open(fname) as f:
    content = f.readlines()
    i = 1
    for line in content:
    	sp = line.strip('- \n').split(' ')
        if len(sp) > 1 and sp[1] == 'SELECT':
        	print 's%d=schedule_select(db1.tbl3.%s,%s,%s)' % (i, col, sp[8], sp[12])
        	i += 1

    print 'execute_scheduled'
###
  #  i = 1
  #j  for line in content:
   # 	sp = line.strip('- \n').split(' ')
   #     if len(sp) > 1 and sp[1] == 'SELECT':
   ##jj     	print 'r%d=fetch(%s,s%d)' % (i, sp[2], i)
     #   	print 'tuple(r%d)' % i
     #   	i += 1

