/* This loads parses all of the settings in a toml file and saves them as
 * public member variables in the Config object. Any values missing in the
 * toml file will have backup default values.
 * 
 * Example usage:
 * 
 * Config config("settings.toml");
 * std::cout << config.feedUri << std::endl;
 */

#pragma once

#include <string>
#include <exception>
#include <fstream>
#include <toml/toml.h>

class Config
{
public:
    Config(std::string filePath)
    {
        std::ifstream file(filePath);
        toml::ParseResult pr = toml::parse(file);
        if (!pr.valid())
        {
            throw std::runtime_error("Failed to parse toml file");
        }
        const toml::Value& root = pr.value;
        loadSettings(root);
    }

    bool showSidebar = false;
    bool animate = true;
    std::string feedUri = "";
    int maxTitle = 64;
    int maxDescription = 128;
    int maxFramerate = 24;
    int checkDelay = 60*1000*45;

private:

    void loadSettings(const toml::Value& root)
    {
        const toml::Value *animateValue = root.find("ui.animate");
        if (animateValue && animateValue->is<bool>())
        {
            animate = animateValue->as<bool>();
        }
        const toml::Value *showSidebarValue = root.find("ui.sidebar");
        if (showSidebarValue && showSidebarValue->is<bool>())
        {
            showSidebar = showSidebarValue->as<bool>();
        }
        const toml::Value *feedUriValue = root.find("feed.uri");
        if (feedUriValue && feedUriValue->is<std::string>())
        {
            feedUri = feedUriValue->as<std::string>();
        }
        const toml::Value *maxTitleLengthValue = root.find("ui.max_title");
        if (maxTitleLengthValue && maxTitleLengthValue->is<int>())
        {
            maxTitle = maxTitleLengthValue->as<int>();
        }
        const toml::Value *maxDescriptionLengthValue = root.find("ui.max_description");
        if (maxDescriptionLengthValue && maxDescriptionLengthValue->is<int>())
        {
            maxDescription = maxDescriptionLengthValue->as<int>();
        }
        const toml::Value *maxFramerateValue = root.find("ui.max_framerate");
        if (maxFramerateValue && maxFramerateValue->is<int>())
        {
            maxFramerate = maxFramerateValue->as<int>();
        }
        const toml::Value *checkDelayValue = root.find("feed.check_delay");
        if (checkDelayValue && checkDelayValue->is<int>())
        {
            checkDelay = checkDelayValue->as<int>();
        }
    }
};