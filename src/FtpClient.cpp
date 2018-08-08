#include "FtpClient.h"
#include "Utils.h"
#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/array.hpp>

CsFtpClient::CsFtpClient
(
std::string const & host,
std::string const & port,
boost::asio::io_service& io_service
)
: m_query(host, port)
, m_resolver(io_service)
, m_socket(io_service)
{
   logMsg("\n=-=-=-=-=-=-=-=-=-= \n", true);
   m_buffer = new char[256];
   Start();
}

void CsFtpClient::SetCredentials
(
std::string user, std::string pass
)
{
   m_username = user;
   m_passwd = pass;
}

void CsFtpClient::Start()
{
   m_resolver.async_resolve
      (
      m_query,
      boost::bind
      (
      &CsFtpClient::ResolvedH,
      this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::iterator
      )
      );
}

void CsFtpClient::ResolvedH
(
const boost::system::error_code &ec,
tcp::resolver::iterator it
)
{
   if (!ec)
   {
      logMsg("0  Resolved.");
      m_socket.async_connect
         (
         *it,
         boost::bind
         (
         &CsFtpClient::ConnectedH,
         this,
         boost::asio::placeholders::error,
         ++it
         )
         );
   }
   else
   {
      logMsg("7  Error from resolve!");
   }
}

void CsFtpClient::ConnectedH
(
const boost::system::error_code& err,
tcp::resolver::iterator endpoint_iterator
)
{
   if (!err)
   {
      boost::asio::async_write
         (
         m_socket,
         m_request,
         boost::bind
         (
         &CsFtpClient::CheckAliveH,
         this,
         boost::asio::placeholders::error
         )
         );
      logMsg("0  Connected.");
   }
   else
   {
      logMsg("7  Error: " + err.message());
   }
}

void CsFtpClient::CheckAliveH
(
const boost::system::error_code& err
)
{
   if (!err)
   {
      logMsg("0  Waiting now for a response...");
      // Read the response status line.
      boost::asio::async_read_until
         (
         m_socket,
         m_response,
         "\r\n",
         boost::bind
         (
         &CsFtpClient::Response1H,
         this,
         boost::asio::placeholders::error,
         boost::asio::placeholders::bytes_transferred
         )
         );
   }
   else
   {
      logMsg("Error response_handler: " + err.message());
   }
}

/// <summary>
/// Handle response from the server.
/// </summary>
/// <param name="err"></param>
void CsFtpClient::Response1H
(
const boost::system::error_code& err,
std::size_t bytes_transferred
)
{
   if (!err)
   {
      m_response.sgetn(m_buffer, std::streamsize(bytes_transferred));
      m_response.sungetc();
      //std::istream response_stream(&m_response);
      std::string data(m_buffer, bytes_transferred);
      //m_response.consume(bytes_transferred);
      //m_response.commit(bytes_transferred);
      std::ostream responseClearer(&m_response);
      char zeroes[100] = { 0 };
      std::string blanc("              ");
      //responseClearer.write(zeroes, 100);
      direct_insert(m_response, "          ");
      m_strResponse = data;
      //std::getline(response_stream, m_strResponse);
      logMsg("?  Received " + std::to_string(bytes_transferred) + " bytes: \"" + trimmed(m_strResponse) + "\" (trimmed from \\r and \\n)");

      // =================================================
      // HERE COMES THE GREAT WEIRDNESS:
      // 'Round' is just a wrapper for SendToFTPServer;
      // but uncommenting alternatively 
      // the first or the second line below, gives
      // two different behaviours!
       SendToFTPServer("USER " + m_username + "\r\n");
        //Round("USER " + m_username + "\r\n");
      // ==================================================
   }
   else
   {
      logMsg("Error read_handler: " + err.message());
   }
}


void CsFtpClient::SendToFTPServer(std::string req)
{
   logMsg("0  Sending this request  : " + trimmed(req));

   logMsg("\r\n"); logMsg("\r\n");
   std::ostream request_stream(&m_request);
   request_stream << req;
   boost::asio::async_write
      (
      m_socket,
      m_request,
      boost::bind
      (
      &CsFtpClient::EatResponseH,
      this,
      boost::asio::placeholders::error
      )
      );
}


/// <summary>
/// Check if server is awake.
/// </summary>
/// <param name="err"></param>
void CsFtpClient::EatResponseH
(
const boost::system::error_code& err
)
{
   if (!err)
   {
      logMsg("0  Eating now a response...");
      // Read the response status line.
      boost::asio::async_read_until
         (
         m_socket,
         m_response,
         "\r\n",
         boost::bind
         (
         &CsFtpClient::Response2H,
         this,
         boost::asio::placeholders::error,
         boost::asio::placeholders::bytes_transferred
         )
         );
   }
   else
   {
      logMsg("Error : " + err.message());
   }
}

/// <summary>
/// Handle response from the server.
/// </summary>
/// <param name="err"></param>
void CsFtpClient::Response2H
(
const boost::system::error_code& err,
std::size_t bytes_transferred
)
{
   if (!err)
   {
      std::istream response_stream(&m_response);
      m_strResponse = "";
      std::getline(response_stream, m_strResponse);
      logMsg("?  receive2d " + std::to_string(bytes_transferred) + " bytes: \"" + trimmed(m_strResponse) + "\" (trimmed from \\r and \\n)");
      m_received = true;
   }
   else
   {
      logMsg("Error : " + err.message());
   }
}

/// <summary>
/// A back-and-forth with the server.
/// Blocks until received a response.
/// </summary>
/// <param name="request"></param>
/// <param name="expectedCode"></param>
/// <returns>true if received success code.</returns>
bool CsFtpClient::Round(std::string const & request, std::string expectedCode)
{
   m_received = false;
   try{
      SendToFTPServer(request);
   }
   catch (...)
   {
      std::cerr << "exc";
   }
   //logfile << &m_response; // WEIRD
   logfile.flush();          // WEIRD
   while (!m_received)
   {
      logMsg("1  Waiting....");
      boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
   }
   return true;
}

