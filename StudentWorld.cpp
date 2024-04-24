#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include "GameWorld.h"
#include "Level.h"
#include <cctype>
#include <ostream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp


StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

int StudentWorld::init()
{
    bonus = 1000;
    setFinishedLevel(false);

    ostringstream oss;
    int currLevel = getLevel();
    oss.fill('0');//fill the text with 0's in case it's like a single digit or smth.
    oss << setw(2) << currLevel;
    curLevel = "level" + oss.str() + ".txt";
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(curLevel);

    if (result == Level::load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    else if (getLevel() == 100)
        return GWSTATUS_PLAYER_WON;

    for (int row = 0; row < 15; row++)
    {
        for (int column = 0; column < 15; column++)
        {
            Level::MazeEntry item = lev.getContentsOf(column, row);

            switch (item)
            {
                case Level::player :
                {
                    m_player = new Player(column, row, this);
                    actorList.push_back(m_player);
                    break;
                }
                case Level::wall :
                {
                    Actor* wall = new Wall(IID_WALL, column, row, this);
                    actorList.push_back(wall);
                    break;
                }
                case Level::marble :
                {
                    Actor* marble = new Marble(column, row, this);
                    actorList.push_back(marble);
                    break;
                }
                case Level::pit :
                {
                    Actor* pit = new Pit(column, row, this);
                    actorList.push_back(pit);
                    break;
                }
                case Level::crystal :
                {
                    Actor* crystal = new Crystal(column, row, this);
                    actorList.push_back(crystal);
                    break;
                }
                case Level::exit :
                {
                    Actor* exit = new Exit(column, row, this);
                    actorList.push_back(exit);
                    break;
                }
                case Level::extra_life :
                {
                    Actor* extra_life = new ExtraLifeGoodie(column, row, this);
                    actorList.push_back(extra_life);
                    break;
                }
                case Level::restore_health :
                {
                    Actor* heal = new RestoreHealthGoodie(column, row, this);
                    actorList.push_back(heal);
                    break;
                }
                case Level::ammo :
                {
                    Actor* ammo = new AmmoGoodie(column, row, this);
                    actorList.push_back(ammo);
                    break;
                }
                case Level::vert_ragebot :
                {
                    Actor* vert = new RageBot(column, row, Actor::down, this);
                    actorList.push_back(vert);
                    break;
                }
                case Level::horiz_ragebot :
                {
                    Actor* horiz = new RageBot(column, row, Actor::right, this);
                    actorList.push_back(horiz);
                    break;
                }
                case Level::thiefbot_factory :
                {
                    Actor* thiefFactory = new ThiefFactory(column, row, false, this);
                    actorList.push_back(thiefFactory);
                    break;
                }
                case Level::mean_thiefbot_factory :
                {
                    Actor* meanFactory = new ThiefFactory(column, row, true, this);
                    actorList.push_back(meanFactory);
                    break;
                }
                default:
                    break;
            }
        }
    }

    for (int row = 0; row < VIEW_WIDTH; row++)
    {
        for (int column = 0; column < VIEW_HEIGHT; column++)
        {
            Level::MazeEntry item = lev.getContentsOf(row, column);
            if (item == Level::crystal)
            {
                m_player -> incCrystalCount();
            }
        }
    }

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you type q

    string gameText = "";
    string scoreText = std::to_string(getScore());
    string livesText = std::to_string(getLives());
    string healthText = std::to_string(m_player -> returnHp() * 5);
    string ammoText = std::to_string(m_player -> getAmmoCount());
    string bonusText = std::to_string(bonus);

    int currentLevel = std::stoi(curLevel.substr(5,2));

    string levelText;

    if (currentLevel < 10)
        levelText = "0" + std::to_string(currentLevel);
    else
        levelText = std::to_string(currentLevel);

    gameText += "Score: " + scoreText + " Level: " + levelText + " Lives: " + livesText + " Health: " + healthText
        + "% Ammo: " + ammoText + " Bonus: " + bonusText;
    setGameStatText(gameText);

    for(Actor* a : actorList)
    {
         if (!(a -> isAlive()) && !(a -> isWall()) && !(a -> isGoodie()) && !(a -> isExit()) && !(a -> isFactory()))
         {}
         else
             a -> doSomething();
    }

    if (!(m_player -> isAlive()))
    {
        decLives();
        playSound(SOUND_PLAYER_DIE);
        return GWSTATUS_PLAYER_DIED;
    }

    removeDeadActors();

    if (bonus > 0)
        bonus--;

    if (isFinishedLevel())
    {
        return GWSTATUS_FINISHED_LEVEL;
    }
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for (Actor* a: actorList)
       delete a;
    actorList.clear();
    m_player = nullptr;
}


StudentWorld::~StudentWorld()
{
    cleanUp();
}

Actor* StudentWorld::getActor(int x, int y)
{
   for (int i = 0; i < actorList.size(); i++)
   {
       if (checkLocation(actorList[i] -> getX(), x) && checkLocation(actorList[i] -> getY(), y))
           return actorList[i];
   }
   return nullptr;
}

bool StudentWorld::checkLocation(double input, double self)
{
    if (input >= self)
        return ((input - self) < 0.05);
    else
        return ((self - input) < 0.05);
}


void StudentWorld::addActor(Actor *actor)
{
    actorList.push_back(actor);
}

//currently doesnt work the pea is turned off but is not deleted for some reason which i have no idea why but need to be figured out
void StudentWorld::removeDeadActors()
{
    for(int i = 0; i < actorList.size(); i++)
    {
        if (!(actorList[i] -> isAlive()) && !(actorList[i] -> isWall()) && !(actorList[i] -> isGoodie())
            && !(actorList[i] -> isExit()) && !(actorList[i] -> isFactory()))
        {
            delete actorList[i];
            actorList.erase(actorList.begin() + i);
        }
    }
}

int StudentWorld::findThief(int xPos, int yPos)
{
    int result = 0;
    int minX = xPos - 3;
    if (minX < 0)
        minX = 0;
    int minY = yPos - 3;
    if (minY < 0)
        yPos = 0;
    int maxX = xPos + 3;
    if (maxX > 14)
       maxX = 14;
    int maxY = yPos + 3;
    if (maxY > 14)
        maxY = 14;

    for (int i = minX; i <= maxX; i++)
    {
        for (int j = minY; j <= maxY; j++)
        {
            for (Actor* temp : actorList)
            {
                if (temp -> getX() == i && temp -> getY() == j)
                    if (temp -> isThief())
                        result++;
            }
        }
    }
    return result;
}

bool StudentWorld::hasThiefOnTop(int xPos, int yPos)
{
    for (Actor* temp : actorList)
    {
        if (temp -> isThief() && temp -> getX() == xPos && temp -> getY() == yPos)
            return true;
    }
    return false;
}

Actor* StudentWorld::getThief(int x, int y)
{
    for (Actor* temp : actorList)
        if (temp -> isThief() && temp -> getX() == x && temp -> getY() == y)
            return temp;
    return nullptr;
}
