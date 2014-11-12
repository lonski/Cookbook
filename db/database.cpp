#include "database.h"

Database::Database(string fn)
{   
    database = NULL;
    open(fn);
}

Database::~Database(){ 
  close();
}

bool Database::open(string fn)
{
    char* filename = const_cast<char*>(fn.c_str());
    if(sqlite3_open(filename,&database) == SQLITE_OK)
        return true;

    return false;
}

bool Database::exists(string squery){
  vector< map<string, string> > t;
  t = query(squery);
  return !t.empty();
}

vector< map<string, string> > Database::query(string squery)
{  
    char* query = const_cast<char*>(squery.c_str());
    sqlite3_stmt *statement;
    vector< map<string, string> > results;

    if(sqlite3_prepare_v2(database, query, -1, &statement, 0) == SQLITE_OK)
    {
        int cols = sqlite3_column_count(statement);
        int result = 0;
        while(true)
        {
            result = sqlite3_step(statement);

            if(result == SQLITE_ROW)
            {                
                map<string, string> val;
                for(int col = 0; col < cols; col++)
                {
                    val[(char*)(sqlite3_column_name(statement,col))]=(char*)(sqlite3_column_text(statement, col));
                }
                results.push_back(val);
            }
            else
            {
                break;
            }
        }

        sqlite3_finalize(statement);
    }

    string error = sqlite3_errmsg(database);
    if(error != "not an error") cout << query << " " << error << endl;

    return results;
}

void Database::close()
{
    sqlite3_close(database);
}
