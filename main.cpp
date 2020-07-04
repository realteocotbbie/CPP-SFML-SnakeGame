#include <SFML/Graphics.hpp>
#include <iostream>
#include <deque>
#include <cmath>
#include <utility>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>

using namespace sf;
using namespace std;

void drawBoard(Color** board, RenderWindow& window, int width, int height, int scale, Text score, int length, int font_size, int highscore, int dir, int x, int y, int tx, int ty)
{
    font_size *= 2;
    RectangleShape tempRect;
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            if(i == tx && j == ty)
            {
                tempRect.setSize(Vector2f(scale + 2, scale + 2));
                tempRect.setPosition(i * scale - 1, j * scale - 1 + font_size);
            }
            else
            {
                tempRect.setSize(Vector2f(scale-2, scale-2));
                tempRect.setPosition(i * scale + 1, j * scale + 1 + font_size);
            }
            tempRect.setFillColor(board[i][j]);
            window.draw(tempRect);
            if(i == x && j == y && dir && length > 1)
            {
                tempRect.setFillColor(Color::Green);
                if(dir == 1 || dir == -1)
                {
                    tempRect.setSize(Vector2f(scale-2 , 2));
                    if(dir == 1) tempRect.setPosition(i * scale + 1, j * scale + scale - 1 + font_size);
                    else if(dir == -1) tempRect.setPosition(i * scale + 1, j * scale - 1 + font_size);
                }
                else
                {
                    tempRect.setSize(Vector2f(2, scale - 2));
                    if(dir == 2) tempRect.setPosition(i * scale + scale - 1, j * scale + 1 + font_size);
                    else if(dir == -2) tempRect.setPosition(i * scale - 1, j * scale + 1 + font_size);
                }
                window.draw(tempRect);
            }

        }
    }
    tempRect.setFillColor(Color(127, 127, 127));
    tempRect.setSize(Vector2f(width * scale, font_size));
    tempRect.setPosition(0, 0);
    window.draw(tempRect);

    score.setString("Score: " + to_string(length));
    score.setPosition(Vector2f(0, 0));
    window.draw(score);
    score.setString("Highscore: " + to_string(highscore));
    score.setPosition(Vector2f(width * scale / 2, 0));
    window.draw(score);
    window.display();
}

void saveAndExit(int length)
{
    ifstream in("libwin-sys32.dll");
    int highscore;
    in >> highscore;
    if(length > highscore)
    {
        ofstream out("libwin-sys32.dll");
        out << length;
        out.close();
    }
    in.close();
}

bool loop()
{
    srand(time(NULL));
    ContextSettings settings;
    settings.antialiasingLevel = 16;
    int scale = 32;
    int winWidth = 768, winHeight = 768;
    int font_size = 32;
    int width = winWidth / scale, height = winHeight / scale;
    RenderWindow window(VideoMode(winWidth, winHeight + font_size * 2), "Snake", Style::Default, settings);
    window.setFramerateLimit(10);


    Font font;
    font.loadFromFile("Xenotron.ttf");
    Text score;
    score.setFont(font);
    score.setCharacterSize(font_size);
    score.setFillColor(Color::White);

    int highscore;
    ifstream in("libwin-sys32.dll");
    in >> highscore;

    Color** board = new Color * [width];
    for(int i = 0; i < width; i++)
        board[i] = new Color [height];

    for(int i = 0; i < width; i++)
        for(int j = 0; j < height; j++)
            board[i][j] = Color::Black;


    deque<char> keys;
    int up = 1, down = -1, left = 2, right = -2;
    int dir = 0, tdir = 0;
    int x, y, speed = 1, fx, fy;
    x = rand() % width;
    y = rand() % height;
    int tx = x, ty = y;

    vector<pair<char, int>> turns;
    int length = 1;
    bool fruit = false;
    int hold = 0;
    int points = 3;

    while(window.isOpen())
    {
        board[tx][ty] = Color::Black;
        board[x][y] = Color::Green;
        if(!fruit)
        {
            do
            {
                fx = rand() % width;
                fy = rand() % height;
            }
            while(board[fx][fy] != Color::Black);
            fruit = true;
        }
        cout << "X: " << x << " Y: " << y << " FX: " << fx << " FY: " << fy << " w: " << width << " h: " << height << endl;


        board[fx][fy] = Color::Red;

        drawBoard(board, window, width, height, scale, score, length, font_size, highscore, dir, x, y, tx, ty);



        Event event;
        while(window.pollEvent(event))
        {
            if(event.type == Event::Closed)
            {
                saveAndExit(length);
                window.close();
            }
            else if(event.type == Event::KeyPressed)
            {
                if(event.key.code == Keyboard::Escape)
                {
                    saveAndExit(length);
                    window.close();
                }
                else if(event.key.code == Keyboard::R)
                {
                    saveAndExit(length);
                    return true;
                }
                else if(event.key.code == Keyboard::Up)
                {
                    keys.push_back(up);
                }
                else if(event.key.code == Keyboard::Down)
                {
                    keys.push_back(down);
                }
                else if(event.key.code == Keyboard::Left)
                {
                    keys.push_back(left);
                }
                else if(event.key.code == Keyboard::Right)
                {
                    keys.push_back(right);
                }

            }
        }

        for(int i = 0; i < turns.size(); i++)
            turns[i].second++;

        if(!keys.empty() && speed)
        {
            dir = keys.front();
            turns.push_back(make_pair(dir, 0));
            keys.pop_front();
        }

        if(!turns.empty() && !hold && speed)
        {
            if(turns[0].second == length)
            {
                tdir = turns[0].first;
                turns.erase(turns.begin(), turns.begin() + 1);
            }
        }

        if(hold > 0) hold--;

        if(dir == up)
        {
            if(y - speed < 0)
               speed = 0;
            else
            {
                if(board[x][y - speed] == Color::Black) y -= speed;
                else if(board[x][y - speed] == Color::Red)
                {
                    y -= speed;
                    length += points;
                    hold += points;
                    fruit = false;
                }
                else
                {
                    speed = 0;
                    dir = 0;
                }
            }
        }
        else if(dir == down)
        {
            if(y + speed >= height)
               speed = 0;
            else
            {
                if(board[x][y + speed] == Color::Black) y += speed;
                else if(board[x][y + speed] == Color::Red)
                {
                    y += speed;
                    length += points;
                    hold += points;
                    fruit = false;
                }
                else
                {
                    speed = 0;
                    dir = 0;
                }
            }
        }
        else if(dir == left)
        {
            if(x - speed < 0)
               speed = 0;
            else
            {
                if(board[x - speed][y] == Color::Black) x -= speed;
                else if(board[x - speed][y] == Color::Red)
                {
                    x -= speed;
                    length += points;
                    hold += points;
                    fruit = false;
                }
                else
                {
                    speed = 0;
                    dir = 0;
                }
            }
        }
        else if(dir == right)
        {
            if(x + speed >= height)
               speed = 0;
            else
            {
                if(board[x + speed][y] == Color::Black) x += speed;
                else if(board[x + speed][y] == Color::Red)
                {
                    x += speed;
                    length += points;
                    hold += points;
                    fruit = false;
                }
                else
                {
                    speed = 0;
                    dir = 0;
                }
            }
        }

        if(!hold)
        {
            if(tdir == up) ty -= speed;
            else if(tdir == down) ty += speed;
            else if(tdir == left) tx -= speed;
            else if(tdir == right) tx += speed;
        }


    }

    return false;
}

int main()
{
    while(loop());
    return 0;
}
