#pragma once

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/URI.h>
#include <iostream>
#include <sstream>

std::string HttpGet(std::string url)
{
    using Poco::Net::HTTPRequest;
    using Poco::Net::HTTPMessage;
    Poco::URI uri(url);
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
    std::string path = uri.getPathAndQuery();
    path = path.empty() ? "/" : path;
    Poco::Net::HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
    session.sendRequest(req);
    Poco::Net::HTTPResponse res;
    std::istream &is = session.receiveResponse(res);
    std::stringstream ss;
    ss << is.rdbuf();
    return ss.str();
}

