#ifndef FTP_CLIENT_HEADER
#define FTP_CLIENT_HEADER

#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/scoped_array.hpp>

using namespace boost::asio;
using namespace boost::asio::ip;

class CsFtpClient
   : public std::enable_shared_from_this<CsFtpClient>
{
public:
   enum Command
   {
      USER,
      PASS,
      PASV,
      SIZE,
      TYPE,
      RETR,
      QUIT
   };
   CsFtpClient
      (
      std::string const & host,
      std::string const & port,
      boost::asio::io_service& io_service
      );

   void ResolvedH(const boost::system::error_code &ec,
      tcp::resolver::iterator it);

   void ConnectedH(const boost::system::error_code& err,
      tcp::resolver::iterator endpoint_iterator);

   void CheckAliveH(const boost::system::error_code& err);

   void Response1H(const boost::system::error_code& err, std::size_t bytes_transferred);

   void EatResponseH(const boost::system::error_code& err);

   void Response2H(const boost::system::error_code& err, std::size_t bytes_transferred);

   void SendToFTPServer(std::string);

   bool RecvFromFTPServer();// const boost::system::error_code& err);

   void readHandler(const boost::system::error_code &ec, std::size_t bytes_transferred);

   void Start();

   void SetCredentials(std::string, std::string);

   void Dialog();

   bool Action(Command);

   bool Round(std::string const &, std::string ec = "");

private:
   tcp::resolver::query m_query;
   tcp::resolver m_resolver;
   tcp::socket m_socket;
   streambuf m_request;
   streambuf m_response;
   std::string m_strResponse;
   std::string m_username;
   std::string m_passwd;
   std::string m_filename;
   bool m_received;
   char* m_buffer;
};

#endif
