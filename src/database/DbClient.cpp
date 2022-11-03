#include "database/DbClient.h"
#include "diag/Trace.h"
#include "diag/Stopwatch.h"
#include "thread/Locker.h"
#include "data/PList.h"
#include "IO/Path.h"
#include "IO/Directory.h"
#include "IO/FileStream.h"
#include "system/Application.h"

namespace Database
{
    DbClient::DbClient()
    {
        
    }
    DbClient::~DbClient()
    {
    }

    bool DbClient::retriveCount(const String& sql, int& count)
    {
        DataTable table("countTable");
        table.addColumn(new DataColumn("count", ValueTypes::Integer32));
        if(executeSqlQuery(sql, table))
        {
            if(table.rowCount() == 1)
            {
                count = table.rows()->at(0)->cells()->at(0)->value().nValue;
                return true;
            }
        }
        return false;
    }

    bool DbClient::executeSql(const String& sql, DataTable& table, bool transaction)
    {
        return executeSql(sql, transaction);
    }
    
    void DbClient::printErrorInfo(const String& methodName, const String& sql, const String& error)
    {
        const int maxCount = 512;
        const char* sqlStr;
        String temp;
        if (sql.length() > maxCount)
        {
            temp = sql.substr(0, maxCount);
            sqlStr = temp.c_str();
        }
        else
        {
            sqlStr = sql.c_str();
        }
        
        String errorStr = error.isNullOrEmpty() ? getErrorMsg() : error;
        
        if (sql.length() > 0)
        {
            Trace::writeFormatLine("Method'%s' error, msg: %s, sql: %s", methodName.c_str(), errorStr.c_str(), sqlStr);
        }
        else
        {
            Trace::writeFormatLine("Method'%s' error, msg: %s", methodName.c_str(), errorStr.c_str());
        }
    }
    
    Value DbClient::convertValue(const ValueTypes type, const char* str)
    {
        Value value = DbValue::nullValue();
        if (str == NULL)
            return value;
        
        switch (type)
        {
            case Null:
                break;
            case Integer32:
                Int32::parse(str, value.nValue);
                break;
            case Integer64:
                Int64::parse(str, value.lValue);
                break;
            case Float32:
            case Float64:
                Double::parse(str, value.dValue);
                break;
            case Text:
                if (strlen(str) > 0)
                {
                    DataCell::setStringValue(value, str);
                }
                break;
            case Date:
            case Timestamp:
            {
                DateTime time;
                if (DateTime::parse(str, time))
                {
                    value.tValue = time.ticks();
                }
                break;
            }
            case Blob:
                if (strlen(str) > 0)
                {
                    DataCell::setStringValue(value, str);
                }
                break;
            default:
                assert(false);
                break;
        }
        return value;
    }

#ifdef DEBUG
    void DbClient::createSqlFile(const String& fileName, const String& sql) const
    {
        String sqlPath = Path::combine(Application::instance()->rootPath(), "sql");
        if(!Directory::exists(sqlPath))
            Directory::createDirectory(sqlPath);
        String fullFileName = Path::combine(sqlPath, fileName);
        FileStream fs(fullFileName, FileMode::FileCreate, FileAccess::FileReadWrite);
        fs.writeText(sql);
        fs.close();
    }
#endif
}
