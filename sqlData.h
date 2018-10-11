#ifndef MYSQL_H
#define MYSQL_H

#include <mysql.h>
#include <exception>

int addDataSet(char id[16], float temp);

class DB {
	public:
		DB();
		~DB();
		void logTemp(char id[15], float temp);
	private:
		void setTimestamp();
		
		MYSQL mysql;
};

#endif
