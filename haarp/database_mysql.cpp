#include <mysql/mysql.h>
#include <iostream>
#include <string>
#include <ctime>
#include <math.h>
#include <cstdlib>
#include <cstring>
#include "database_mysql.h"
#include "utils.h"

int Database::ping() {
	return mysql_ping(&conn);
}

int Database::open(string host,string username,string password,string database) {
	operation = "";
	mysql_init(&conn);
	if (mysql_real_connect(&conn, host.c_str(), username.c_str(), password.c_str(), database.c_str(), 0, NULL, 0)) {
		//mysql_autocommit(&conn,1);
		connected = true;
		return 0;
	} else return -1;
}

bool Database::isconnect() {
	return connected;
}

long int Database::getID() {
	return mysql_thread_id(&conn);
}

int Database::set(string sql) {  //o ok, -1 no ok
	string reg = regex_match_nocase("^(UPDATE|DELETE|INSERT) ", trimstr(sql));
	if (reg != "")
		operation = UpperCase(trimstr(reg));
	//cout << "set: "<<sql<<endl;
	if(!mysql_query(&conn, sql.c_str())) 
		return 0; 
	else 
		return -1;
}

long long int Database::get_affect_rows() {
	return (long long int) mysql_affected_rows(&conn);
} 

int Database::get(string sql) {
	string reg = regex_match_nocase("^(SELECT) ", trimstr(sql));
	if (reg != "")
		operation = UpperCase(trimstr(reg));
    //cout << "get: "<<sql<<endl;
    if ( !mysql_query(&conn, sql.c_str()) ) {
        qry = mysql_store_result(&conn);
        return 0;
    } else return -1;
}

int Database::get_num_rows() {
	//if ( qry )
		return mysql_num_rows(qry);
	//else 
	//	return 0;
}
string Database::get(string field, int line) {
    //cout << "get2: "<<field<<endl;
    int pos=-1;
    int cols = mysql_num_fields(qry);
    int lines = get_num_rows();

    fields = mysql_fetch_fields(qry);
	//cout << "lines "<< lines<<" erro "<<getError()<<endl;
    if (line <= lines) {

        for(int i=0;i<= (cols-1);++i){
            if (fields[i].name == field) {
               pos = i;
               break;
            }
        }
        mysql_data_seek(qry,line-1);
        return mysql_fetch_row(qry)[pos];

    } else {
	    return "";
    }

}

void Database::clear(){
    mysql_free_result(qry);
}

string Database::getError(){
    return mysql_error(&conn);
}

void Database::close() {
	if(!connected) return;
	connected = false;
	mysql_close(&conn);
}

const string Database::sqlconv(string sql) {
	SearchReplace(sql,"'","\\'");
	SearchReplace(sql,";","\\;");
	SearchReplace(sql,"\"","\\\"");
	return sql;
}

string Database::getRealEscapeString(string source) {
	char *end = new char[source.size()*2 + 1];
	mysql_real_escape_string(&conn, end, source.c_str(), source.size());
	string out(end);
	delete [] end;
	return out;
}

Database::Database() {
	connected = false;
	operation = "";
}

Database::~Database() {
}

