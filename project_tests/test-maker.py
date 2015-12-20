
fname = 'test24.dsl'

with open(fname) as f:
    content = f.readlines()
    i = 1
    for line in content:
    	sp = line.replace('-', '').strip().split(' ')
        if len(sp) > 1 and sp[1] == 'SELECT':
        	print 's%d=schedule_select(%s,%s,%s)' % (i, sp[6], sp[8], sp[12])
        	i += 1

    print 'execute_scheduled'

    i = 1
    for line in content:
    	sp = line.replace('-', '').strip().split(' ')
        if len(sp) > 1 and sp[1] == 'SELECT':
        	print 'r%d=fetch(%s,s%d)' % (i, sp[2], i)
        	print 'tuple(r%d)' % i
        	i += 1

