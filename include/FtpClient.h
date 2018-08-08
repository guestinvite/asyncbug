#ifndef FTP_CLIENT_HEADER
#define FTP_CLIENT_HEADER

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/scoped_array.hpp>
#include <memory>


using namespace boost::asio;
using namespace boost::asio::ip;

class CsFtpClient
   : public std::enable_shared_from_this<CsFtpClient>
{
public:
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

   void Start();

   void SetCredentials(std::string, std::string);


   void initiateReadResponse();
   void handleResponse(std::string const& buffer);
   void sendRequest(std::shared_ptr<std::string> preq);

//   bool Round(std::string const &, std::string ec = "");

private:
   tcp::resolver::query m_query;
   tcp::resolver m_resolver;
   tcp::socket m_socket;
   streambuf m_response;
   std::string m_username;
   std::string m_passwd;
   std::string m_filename;
};

#endif
