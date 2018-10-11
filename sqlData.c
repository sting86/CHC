#include <stdio.h>
#include "sqlData.h"

#define SQL_USR  "web"
#define SQL_PASS "web14913"
#define SQL_DB   "chc"
#define SQL_ADDR "127.0.0.1"

DB::DB() {
	mysql_init(&mysql);
	MYSQL* ret = mysql_real_connect(&mysql, SQL_ADDR, SQL_USR, SQL_PASS, SQL_DB, 0, NULL, 0);
	if (ret == NULL) {
		throw mysql_errno(&mysql);
	}
	
	mysql_select_db(&mysql, "controller");
}


DB::~DB() {
	setTimestamp();
	mysql_close(&mysql);
}

void DB::setTimestamp() {
	
	// update settings set AppUpdated=current_timestamp;
	
	mysql_query(&mysql, "UPDATE SETTINGS SET AppUpdated=current_timestamp");
	
	
}

void DB::logTemp(char id[15], float temp) {
	char query[200];

	snprintf(query, sizeof(query), "INSERT INTO loggedTemps(id, temp) VALUES('%s', %f)", id, temp);
	mysql_query(&mysql, query);
	
	//setTimestamp();
}
		
int addDataSet(char id[15], float temp) {
  MYSQL mysql;
  mysql_init(&mysql);

    if(mysql_real_connect(&mysql, SQL_ADDR, SQL_USR, SQL_PASS, SQL_DB, 0, NULL, 0))
        printf("Connection success!\n");
    else {
        printf("Connection Error %d, %s\n", mysql_errno(&mysql), mysql_error(&mysql));
        return -1;
    }
{
    MYSQL_RES *idZapytania;
    MYSQL_ROW  wiersz;
    char query[200];
    
    mysql_select_db(&mysql, "controller");

    snprintf(query, 200, "INSERT INTO loggedTemps(id, temp) VALUES('%s', %f)", id, temp);
    mysql_query(&mysql, query);
    
//    mysql_query(&mysql, "SELECT * FROM loggedTemps LIMIT 5" );
//    idZapytania = mysql_store_result(&mysql);
    
//    while((wiersz = mysql_fetch_row(idZapytania)) != NULL)
//    {
//        for(int i = 0;i < mysql_num_fields(idZapytania); i++)
//            printf("%s ", wiersz[i]);
//        printf("\n");
//    }
}
    mysql_close(&mysql); //
  //printf("Dupa\n");
  return 0;
}



