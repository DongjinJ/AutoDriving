import MySQLdb

db=MySQLdb.connect("localhost","root","3083","double_stone")
curs=db.cursor()

try:
	strQuery="INSERT INTO state VALUES(NOW(),240,20,25,5,10)"
	curs.execute(strQuery,)
	db.commit()
finally:
	db.close()
