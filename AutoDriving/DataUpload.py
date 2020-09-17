import MySQLdb

db=MySQLdb.connect("localhost","dbstone","3083","double_stone")
curs=db.cursor()

try:
    strQuery = "INSERT INTO state VALUES(NOW(),{},{},{},{},{})".format(val0, val1, val2, val3, val4)
	curs.execute(strQuery)
	db.commit()
finally:
	db.close()
