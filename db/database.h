#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include "sqlite3.h"

using namespace std;

typedef vector< map<string,string> > SQLTable;
typedef vector< map<string,string> >::iterator SQLRowsIterator;
typedef map<string,string>::iterator SQLColsIterator;

class Database
{
public:
    Database(string fn);
    ~Database();

    bool open(string fn);
    vector< map<string, string> > query(string squery);
    bool exists(string query);
    void close();

private:   
    sqlite3 *database; 
};

#endif

