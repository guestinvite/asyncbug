#include <string>
#include "FtpClient.h"


int main(int argc, char* argv[])
{
   std::string host = "speedtest.tele2.net";
   std::string port = "21";
   
   io_service csioservice;
   CsFtpClient clientFtp(host, port, csioservice);
   clientFtp.SetCredentials("anonymous", "Passwd");
   csioservice.run();
}
