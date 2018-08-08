#include "FtpClient.h"
#include "Utils.h"
#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/array.hpp>
#include <boost/make_shared.hpp>

CsFtpClient::CsFtpClient
        (
                std::string const &host,
                std::string const &port,
                boost::asio::io_service &io_service
        )
        : m_query(host, port), m_resolver(io_service), m_socket(io_service) {
    logMsg("\n=-=-=-=-=-=-=-=-=-= \n", true);
    Start();
}

void CsFtpClient::SetCredentials
        (
                std::string user, std::string pass
        ) {
    m_username = user;
    m_passwd = pass;
}

void CsFtpClient::Start() {
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
        ) {
    if (!ec) {
        logMsg("0  Resolved.");
        boost::asio::async_connect(m_socket, it, boost::bind(&CsFtpClient::ConnectedH,
                                                             this,
                                                             boost::asio::placeholders::error,
                                                             boost::asio::placeholders::iterator));
    } else {
        logMsg("7  Error from resolve!");
    }
}

void CsFtpClient::ConnectedH
        (
                const boost::system::error_code &err,
                tcp::resolver::iterator
        ) {
    if (!err) {
        logMsg("0  Connected.");
        initiateReadResponse();
    } else {
        logMsg("7  Error: " + err.message());
    }
}

namespace {
    void removeTrailingLF(std::string &buffer) {
        if (buffer.size() > 0 && buffer.back() == '\r')
            buffer.erase(std::prev(buffer.end()));
    }
}

void CsFtpClient::initiateReadResponse() {
    boost::asio::async_read_until(m_socket, m_response, "\r\n",
                                  [this](boost::system::error_code const &ec, std::size_t bytes) {
                                      if (ec) {
                                          logMsg("read error: " + ec.message());
                                      } else {
                                          std::string buffer;
                                          std::istream is(&m_response);
                                          std::getline(is, buffer);
                                          removeTrailingLF(buffer);
                                          this->handleResponse(buffer);
                                      }

                                  });
}

void CsFtpClient::handleResponse(std::string const &buffer) {
    logMsg("handling response: " + buffer);

    auto first_code = buffer.begin();
    auto last_code = std::find(first_code, buffer.end(), ' ');
    if (last_code == buffer.end()) {
        logMsg("no code in response");
        return;
    }

    auto first_message = std::next(last_code);
    auto last_message = buffer.end();

    auto code = std::string(first_code, last_code);
    if (code == "220") {
        sendRequest(std::make_shared<std::string>("USER " + this->m_username + '\n'));
    } else if (code == "331") {
        sendRequest(std::make_shared<std::string>("PASS " + this->m_passwd + '\n'));
    } else if (code == "230") {
        logMsg("logged in - what next?");
    } else {
        logMsg("unrecognised response code");
    }
}

void CsFtpClient::sendRequest(std::shared_ptr<std::string> preq) {
    boost::asio::async_write(m_socket, boost::asio::buffer(*preq),
                             [this, preq](boost::system::error_code const &ec, std::size_t bytes) {
                                 if (ec != boost::system::error_code()) {
                                     logMsg("send error:" + ec.message());
                                 } else {
                                     initiateReadResponse();
                                 }

                             });
}


