/* This file converts an RSS XML string into objects containing the title and
 * description of each post in the RSS XML string.
 * 
 * All HTML is removed from the description as well as most non-alphanumeric characters.
 */
#pragma once

#include <rapidxml.hpp>
#include <vector>
#include <string>
#include <cstring>

inline std::string StripHtml(std::string input)
{
    bool ignoring = false;
    std::string result = "";
    int len = input.size();
    for (int i = 0; i < len; i++)
    {
        if (input[i] == '<')
        {
            ignoring = true;
        }
        else if (!ignoring)
        {
            char next = input[i];
            if (isalpha(next) || isdigit(next) 
                || next == ' ' || next == '.' 
                || next == '!' || next == ':' 
                || next == '/')
                result += input[i];
            else 
                result += ' ';
        }
        else if (input[i] == '>')
        {
            ignoring = false;
        }
    }
    return result;
}

class RssItem
{
public:
    std::string title;
    std::string description;

    RssItem(std::string title, std::string description) 
    : title(title), description(description) { }

    bool equals(const RssItem &other)
    {
        return (*this) == other;
    }

    bool operator==(const RssItem &other)
    {
        return title == other.title && description == other.description;
    }

    bool operator!=(const RssItem &other)
    {
        return !((*this) == other);
    }
};


class RssReader
{
public:
    RssReader(std::string xml, int maxTitleLen, int maxDescLen)
    {
        this->maxTitleLen = maxTitleLen;
        this->maxDescLen = maxDescLen;
        std::vector<char> xmlVec(xml.begin(), xml.end());
        xmlVec.push_back('\0');
        rapidxml::xml_document<> doc;
        doc.parse<0>(&xmlVec[0]);
        rapidxml::xml_node<> *channelNode = doc.first_node()->first_node("channel");
        rapidxml::xml_node<> *titleNode = channelNode->first_node("title");
        title = titleNode->value();
        description = titleNode->next_sibling("description")->value();
        rapidxml::xml_node<> *itemNode = channelNode->first_node("item");
        while (itemNode != nullptr)
        {
            IncludePost(itemNode->first_node("title")->value(), itemNode->first_node("description")->value());
            itemNode = itemNode->next_sibling("item");
        }
    }

    std::vector<RssItem> Posts()
    {
        std::vector<RssItem> result;
        result.reserve(PostCount());
        for (int i = 0; i < PostCount(); i++)
        {
            result.push_back({PostTitle(i), PostDescription(i)});
        }
        return result;
    }

    int PostCount()
    {
        return numPosts;
    }

    std::string PostTitle(int index)
    {
        return postTitles.at(index);
    }

    std::string PostDescription(int index)
    {
        return postDescriptions.at(index);
    }

    std::string ChannelTitle()
    {
        return title;
    }

    std::string ChannelDescription()
    {
        return description;
    }

private:
    void IncludePost(std::string title, std::string description)
    {
        description = StripHtml(description);
        if (title.size() > maxTitleLen)
        {
            title = title.substr(0, maxTitleLen);
        }
        if (description.size() > maxDescLen)
        {
            description = description.substr(0, maxDescLen);
        }
        postTitles.push_back(title);
        postDescriptions.push_back(description);
        numPosts++;
    }

    std::string title;
    std::string description;
    std::vector<std::string> postTitles;
    std::vector<std::string> postDescriptions;
    int numPosts = 0;
    int maxTitleLen;
    int maxDescLen;
};