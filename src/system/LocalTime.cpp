#include "IO/File.h"
#include "system/LocalTime.h"
#include "data/Convert.h"
#include "data/DateTime.h"
#include "diag/Trace.h"
#include "IO/Path.h"
#include "IO/File.h"
#include "thread/Process.h"
#include "thread/Thread.h"
#ifdef WIN32
#include <windows.h>
//#pragma comment (lib, "Advapi32.lib")
#else
#if __linux__ && !__ANDROID__
#include <linux/rtc.h>
#include <sys/ioctl.h>
#endif
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>
#endif

namespace Common
{
#ifdef WIN32
	void EnableSystemTimePriv()
	{
		HANDLE hToken;
		LUID luid;
		TOKEN_PRIVILEGES tkp;

		OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken );

		LookupPrivilegeValue( NULL, SE_SYSTEMTIME_NAME, &luid );

		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Luid = luid;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges( hToken, false, &tkp, sizeof( tkp ), NULL, NULL );

		CloseHandle( hToken ); 
	}
    void EnableTimeZonePriv()
    {
        HANDLE hToken;
        LUID luid;
        TOKEN_PRIVILEGES tkp;
        
        OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken );
        
        LookupPrivilegeValue( NULL, SE_TIME_ZONE_NAME, &luid );
        
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Luid = luid;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        
        AdjustTokenPrivileges( hToken, false, &tkp, sizeof( tkp ), NULL, NULL );
        
        CloseHandle( hToken );
    }
#endif

	bool LocalTime::setTime(const time_t& timep)
	{
#ifdef WIN32
		EnableSystemTimePriv();

		struct tm* t = localtime(&timep);

		SYSTEMTIME st;
		st.wYear = t->tm_year;
		st.wMonth = t->tm_mon;
		st.wDay = t->tm_mday;
		st.wHour = t->tm_hour;
		st.wMinute = t->tm_min;
		st.wSecond = t->tm_sec;
		st.wMilliseconds = 0;
		return SetLocalTime(&st) ? true : false;
#elif __EMSCRIPTEN__
		// Can not update time by browser.
		return false;
#else
		struct tm* t = localtime(&timep);

		struct timeval tv;
		tv.tv_sec = mktime(t);
		tv.tv_usec = 0;
		return settimeofday(&tv, NULL) == 0;
#endif
	}
	bool LocalTime::setTime(const DateTime& time, const TimeZone& tz)
	{
#ifdef DEBUG
        Debug::writeFormatLine("LocalTime::setTime, time: %s, time zone: %s",
                               time.toString().c_str(), tz.toString().c_str());
#endif
        
        LocalTime::setTimeZone(tz);
        
#ifdef WIN32
		EnableSystemTimePriv();
        
		SYSTEMTIME st;
		st.wYear = time.year();
		st.wMonth = time.month();
		st.wDay = time.day();
		st.wHour = time.hour();
		st.wMinute = time.minute();
		st.wSecond = time.second();
		st.wMilliseconds = time.millisecond();
		return SetLocalTime(&st) ? true : false;
#elif __EMSCRIPTEN__
		// Can not update time by browser.
		return false;
#else
        const TimeZone& local = tz.isEmpty() ? TimeZone::Local : tz;
        static const DateTime startUtc = DateTime(1970, 1, 1, 0, 0, 0, DateTime::Utc);
        static const DateTime startLocal = local.toLocalTime(startUtc);
        DateTime start = time.kind() == DateTime::Kind::Utc ? startUtc : startLocal;
        struct timeval tv;
        TimeSpan diff = (DateTime)time - start;
        tv.tv_sec = (time_t)diff.totalSeconds();
        tv.tv_usec = 0;
        if(settimeofday(&tv, NULL) != 0)
        {
            Debug::writeFormatLine("LocalTime::setTime failed, reason: %s", strerror(errno));
            return false;
        }
        
#if __linux__ && !__ANDROID__
        return hwclock(time.kind() == DateTime::Kind::Utc ? local.toLocalTime(time) : time);
#endif
        return true;
#endif
	}
    bool LocalTime::setTimeZone(const TimeZone& tz)
    {
        if(tz.isEmpty())
            return false;
        if(tz == TimeZone::Local)
            return false;
        
#ifdef WIN32
        EnableTimeZonePriv();
        
        TIME_ZONE_INFORMATION tzi;
        memset(&tzi, 0, sizeof(tzi));
        tzi.Bias = tz.localTimeValue();
        if(SetTimeZoneInformation(&tzi))
        {
            TimeZone::updateLocal(tz);
            return true;
        }
#else
        TimeZone::Type type = tz.type();
        if(type < TimeZone::Type::Count)
        {
            String zoneFileName = Path::combine("/usr/share/zoneinfo/", TimeZone::FileNames[type]);
            Debug::writeFormatLine("timezone file name: %s", zoneFileName.c_str());
            if(File::exists(zoneFileName))
            {
                // ln /usr/share/zoneinfo/Asia/Shanghai /etc/localtime
                Process::start(String("ln"), String::convert("-s -f %s /etc/localtime", zoneFileName.c_str()));
                Thread::msleep(1000);
                if(LocalTime::setTime(DateTime::now()))
                {
                    TimeZone::updateLocal(tz);
                    return true;
                }
            }
        }
#endif
        return false;
    }
    
#if __linux__ && !__ANDROID__
    bool LocalTime::hwclock(const DateTime& time)
    {
        system("hwclock -w");
        return true;
        
//        struct rtc_time rt;
//        rt.tm_sec = time.second();/* Seconds.[0-60] (1 leap second)*/
//        rt.tm_min = time.minute();/* Minutes.[0-59] */
//        rt.tm_hour = time.hour();/* Hours.[0-23] */
//        rt.tm_mday = time.day();/* Day.[1-31] */
//        rt.tm_mon = time.month() - 1;/* Month.[0-11] */
//        rt.tm_year = time.year() - 1900;/* Year- 1900.*/
//        rt.tm_isdst = -1;/* DST.[-1/0/1]*/
//
//        /* set your values here */
//        String rtcStr = "/dev/rtc";
//        if(File::exists(rtcStr))
//        {
//            int fd = open(rtcStr, O_RDONLY);
//            bool result = ioctl(fd, RTC_SET_TIME, &rt) == 0;
//            close(fd);
//            return result;
//        }
//        else
//        {
//            for (uint32_t i=0; i<10; i++)
//            {
//                String str = rtcStr + Int32(i).toString();
//                Debug::writeFormatLine("LocalTime::hwclock, fd: '%s'", str.c_str());
//                if(File::exists(str))
//                {
//                	Debug::writeFormatLine("LocalTime::hwclock, fd(exists): '%s'", str.c_str());
//                    int fd = open(str, O_RDONLY);
//                    bool result = ioctl(fd, RTC_SET_TIME, &rt) == 0;
//                    close(fd);
//                    Debug::writeFormatLine("LocalTime::hwclock, ioctl(result): %s", result ? "true" : "false");
//                    return result;
//                }
//            }
//
//            system("hwclock -w");
//			return true;
//        }
    }
#endif
}
