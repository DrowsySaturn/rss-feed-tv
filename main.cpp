/* This is the main file in the project. This project loads a configuration and
 * displays a fullscreen RSS feed based on the URI specified in the config.
 */

#include "http.h"
#include "display.h"
#include "rss.h"
#include "http.h"
#include "config.h"

#include <fstream>
#include <sstream>
#include <ctime>
#include <string>
#include <memory>
#include <SDL2/SDL.h>

using namespace std::literals::string_literals;

std::unique_ptr<Config> config;

struct RssCard
{
    int currentPos = 0;
    int expectedPos = 0;
    RssItem rss;

    RssCard(RssItem rss, int expectedPos) 
    : rss(rss), expectedPos(expectedPos) {}
};

class FeedStreamer
{
private:

    const int CARD_SPACING = 15;

    long lastCheck = 0; 

    std::string FileGetContents(std::string filePath)
    {
        std::ifstream file(filePath);
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    bool showFeedInfo;
    std::vector<RssCard> items;

    bool HasRssItem(RssItem item)
    {
        for (int i = 0; i < items.size(); i++)
        {
            if (item == items[i].rss)
            {
                return true;
            }
        }
        return false;
    }

    void InsertRssItem(RssItem item, int feedBoxHeight, int spacing)
    {
        for (int i = 0; i < items.size(); i++)
        {
            items[i].expectedPos += feedBoxHeight + spacing;
        }
        RssCard newCard = {item, 15};
        items.push_back(newCard);
    }

    void InsertRssFeed(const std::vector<RssItem> &items, int feedBoxHeight, int spacing)
    {
        for (const RssItem &item : items)
        {
            if (!HasRssItem(item))
            {
                InsertRssItem(item, feedBoxHeight, spacing);
            }
        }
    }

    void MoveCards(int speed)
    {
        for (RssCard &card : items)
        {
            if (card.currentPos != card.expectedPos)
            {
                if (config->animate)
                {
                    if (card.expectedPos - card.currentPos < speed)
                    {
                        card.currentPos = card.expectedPos;
                    }
                    else 
                    {
                        card.currentPos += speed;
                    }
                }
                else 
                {
                    card.currentPos = card.expectedPos;
                }
            }
        }
    }

    void DrawCards(Display &display)
    {
        for (RssCard &card : items)
        {
            if (card.currentPos < display.GetHeight())
            {
                display.FeedBox(card.rss.title, card.rss.description, card.currentPos);
            }
        }
    }

    void UpdateFeed(std::string feedUri, int feedBoxHeight)
    {
        if (time(NULL) - lastCheck > config->checkDelay)
        {
            try 
            {
                std::string rssXml = HttpGet(feedUri);
                RssReader reader(rssXml, config->maxTitle, config->maxDescription);
                InsertRssFeed(reader.Posts(), feedBoxHeight, CARD_SPACING);
            }
            catch (Poco::Exception &ex)
            {
                // Failed to get feed, ignore for now.
            }
            lastCheck = time(NULL);
        }
    }

public:
    FeedStreamer()
    {
        showFeedInfo = config->showSidebar;
    }
    
    void MainLoop()
    {
        Display display;
        long lastFrame = 0;
        while (!display.IsClosed())
        {
            display.Clear();
            UpdateFeed(config->feedUri, display.FeedBoxHeight());
            MoveCards(time(NULL) - lastFrame);
            lastFrame = time(NULL);
            DrawCards(display);
            SDL_Delay(1000/config->maxFramerate);
            display.UpdateWindow();
        }
    }

};

int main(int argc, char **argv) 
{
    try 
    {
        config = std::make_unique<Config>("settings.toml");
    }
    catch (std::exception &ex)
    {
        std::cout << "Failed to load config: " << ex.what() << std::endl;
    }
    FeedStreamer().MainLoop();
    return 0;
}