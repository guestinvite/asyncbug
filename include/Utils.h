#ifndef FTP_CLIENT_UTILS_HEADER
#define FTP_CLIENT_UTILS_HEADER

#include <fstream>
#include <iostream>

#define DEBUG_ONLY_WIPE_AFTER
#ifdef DEBUG_ONLY_WIPE_AFTER
#include <Windows.h>
#endif

std::ofstream logfile("log.txt", std::ios::app);
//std::ostream & logfile = std::cout;

std::string trimmed(std::string const s)
{
   std::string res(s);
   int i{};
   std::string trim = "\r\n";

   while ((i = res.find_first_of(trim)) != -1)
      res.erase(i, 1);
   return res;
}

std::string daytime_string()
{
   time_t now = time(0);
   struct tm * dt = gmtime(&now);
   std::string res = "[" + std::to_string(dt->tm_mon + 1) + "/" + std::to_string(dt->tm_mday) + " ";
   res += std::to_string(dt->tm_hour + 2) + ":" + std::to_string(dt->tm_min);
   res += ":" + std::to_string(dt->tm_sec) + "] ";
   return res;
}

#ifdef DEBUG_ONLY_WIPE_AFTER
std::string precise_time_string()
{
   SYSTEMTIME t;
   GetSystemTime(&t);
   std::string res = "[" + std::to_string(t.wMonth) + "/" + std::to_string(t.wDay) + " ";
   res += std::to_string(t.wHour + 2) + ":" + std::to_string(t.wMinute);
   res += ":" + std::to_string(t.wSecond) + ".";
   res += std::to_string(t.wMilliseconds) + "] ";
   return res;
}
#endif

void logMsg(std::string const & msg, bool raw = false)
{
   if (raw)
   {
      logfile << msg;
   }
   else
   {
#ifndef DEBUG_ONLY_WIPE_AFTER
      logfile << daytime_string() << msg << std::endl;
#else
      logfile << precise_time_string() << msg << std::endl;
#endif
   }
   logfile.flush();
}

void direct_insert(boost::asio::streambuf& sb, std::string const& data)
{
   auto size = data.size();
   auto buffer = sb.prepare(size);
   std::copy(std::begin(data), std::end(data), boost::asio::buffer_cast<char*>(buffer));
   sb.commit(size);
}

#endif
