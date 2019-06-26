/* This file contains all of the code to draw 'cards'. The user initializes
 * the display and then draws their desired cards.
 * 
 * Example usage:
 * 
 * Display display;
 * while (!display.IsClosed())
 * {
 *  display.Clear();
 *  display.FeedBox("title", "this is the description", 15); // Draw a card
 *  // Draw more cards or shapes here.
 *  display.UpdateWindow();
 * }
 */

#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

class Display
{
private:
    bool closed = false;

    void InitWindow()
    {
        titleColor = {37, 81, 29};
        regularColor = {38, 38, 38};
        TTF_Init();
        titleFont = TTF_OpenFont("opensans.ttf", TITLE_FONT_SIZE);
        regularFont = TTF_OpenFont("opensans.ttf", REGULAR_FONT_SIZE);
        SDL_Init(SDL_INIT_EVERYTHING);
        SDL_DisplayMode displayMode;
        SDL_GetCurrentDisplayMode(0, &displayMode);
        screenWidth = displayMode.w;
        screenHeight = displayMode.h;
        SDL_Window *window = SDL_CreateWindow("rpi rss feed tv", 
            0, 0, 
            screenWidth, screenHeight, 
            SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }

    void Quit()
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        TTF_CloseFont(titleFont);
        TTF_CloseFont(regularFont);
        TTF_Quit();
    }

public:
    const int HORIZ_PADDING = 25;
    const int TEXT_PADDING = 7;
    const int ADD_BOTTOM_PADDING = 5;
    const int TITLE_FONT_SIZE = 26;
    const int REGULAR_FONT_SIZE = 16;
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *titleFont;
    TTF_Font *regularFont;
    int screenWidth, screenHeight;
    SDL_Color titleColor;
    SDL_Color regularColor;

    int GetHeight()
    {
        return screenHeight;
    }

    Display()
    {
        InitWindow();
    }

    virtual ~Display()
    {
        Quit();
    }

    void UpdateWindow()
    {
        SDL_RenderPresent(renderer);
        SDL_Event evt;
        while (SDL_PollEvent(&evt))
        {
            if (evt.type == SDL_QUIT)
            {
                closed = true;
            }
        }
    }

    bool IsClosed()
    {
        return closed;
    }

    void Rectangle(int x, int y, int width, int height)
    {
        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = width;
        rect.h = height;
        SDL_RenderFillRect(renderer, &rect);
    }

    void Circle(int x, int y, int radius)
    {
        int stop = y + radius;
        for (int i = y - radius; i <= stop; i++)
        {
            int vertOffset = i-y;
            int horzOffset = (int)sqrt(radius*radius - vertOffset*vertOffset);
            SDL_RenderDrawLine(renderer, x - horzOffset, i, horzOffset + x, i);
        }
    }

    void RoundedRectangle(int x, int y, int width, int height, int radius)
    {
        // top left circle
        Circle(x + radius, y + radius, radius);
        // top right circle
        Circle(x + width - radius, y + radius, radius);
        // bottom right circle
        Circle(x + width - radius, y + height - radius, radius);
        // bottom left circle
        Circle(x + radius, y + height - radius, radius);
        // top rectangle
        Rectangle(x + radius, y, width - radius*2, radius*2);
        // right rectangle
        Rectangle(x + width - radius*2, y + radius, radius*2, height - radius*2);
        // bottom rectangle
        Rectangle(x + radius, y + height - radius*2, width - radius*2, radius*2);
        // left rectangle
        Rectangle(x, y + radius, radius*2, height - radius*2);
        // center rectangle
        Rectangle(x + radius, y + radius, width - radius*2, height - radius*2);
    }

    void DrawText(SDL_Color color, TTF_Font *font, std::string text, int x, int y)
    {
        SDL_Surface *textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        int width, height;
        SDL_QueryTexture(textTexture, NULL, NULL, &width, &height);
        SDL_Rect textRect = {x, y, width, height};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_DestroyTexture(textTexture);
        SDL_FreeSurface(textSurface);
    }
    
    int FeedBoxHeight()
    {
        return TEXT_PADDING*3 + TITLE_FONT_SIZE + REGULAR_FONT_SIZE + ADD_BOTTOM_PADDING;
    }

    void FeedBox(std::string title, std::string body, int y)
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        RoundedRectangle(
            HORIZ_PADDING, y, 
            screenWidth - HORIZ_PADDING*2, 
            FeedBoxHeight(), 
            5);
        DrawText(titleColor, titleFont, title, HORIZ_PADDING + TEXT_PADDING, y + TEXT_PADDING);
        DrawText(regularColor, regularFont, body, HORIZ_PADDING + TEXT_PADDING, y + TITLE_FONT_SIZE + TEXT_PADDING*2);
    }

    void Clear()
    {
        SDL_SetRenderDrawColor(renderer, 189, 224, 136, 255);
        SDL_RenderClear(renderer);
    }
};