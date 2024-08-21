# lewsql buffer manager

The responsibility of the buffer manager is to abstract over OS file(s) and provide a way for the 
rest of the database management system to stream large amounts of data to and from database tables without worrying about the loading and 
unloading of data when RAM is limmited.

