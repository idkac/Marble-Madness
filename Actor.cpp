#include "Actor.h"
#include "GameConstants.h"
#include "StudentWorld.h"
#include <cstdio>
#include <ostream>
#include <string>
#include <random>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
StudentWorld* Actor::getWorld()
{
    return m_world;
}

bool Actor::canMove(int x, int y)
{
    Actor* temp = getWorld() -> getActor(x, y);
    if (temp == nullptr || (!(temp -> isPlayer()) && !(temp -> isWall()) && !(temp -> isFactory()) && !(temp -> isRobot())
                && !(temp -> isMarble())))
        return true;
    else
        return false;
}


//updates the hp of the actor
bool Actor::updateHp(int value, Actor* actorToPlaySound)
{
    if (!(actorToPlaySound -> isMarble()) && !(actorToPlaySound -> isPlayer()))
        getWorld() -> playSound(SOUND_ROBOT_IMPACT);
    else if (!(actorToPlaySound -> isMarble()))
        getWorld() -> playSound(SOUND_PLAYER_IMPACT);

    if (m_hp - value <= 0)
    {
        m_hp = 0;
        m_isAlive = false;
        if (actorToPlaySound -> isRobot() && !(actorToPlaySound -> isThief()))
        {
            getWorld() -> playSound(SOUND_ROBOT_DIE);
            getWorld() -> increaseScore(100);
        }
        if (actorToPlaySound -> isThief())
        {
            int x = actorToPlaySound -> getX();
            int y = actorToPlaySound -> getY();
            if (actorToPlaySound -> hasGoodie())
                actorToPlaySound -> getGoodie() -> moveTo(x, y);
            getWorld() -> increaseScore(10);
        }
    }
    else
        m_hp -= value;

    return m_isAlive;
}

void Alive::newPosition(int direction,int& x, int &y)
{
    switch (direction)
    {
        case up:
        {
            y++;
            break;
        }
        case down:
        {
            y--;
            break;
        }
        case right:
        {
            x++;
            break;
        }
        case left:
        {
            x--;
            break;
        }
        default:
            break;
    }
}


int Alive::isDirection(int value)
{
    if (value == KEY_PRESS_DOWN || value == KEY_PRESS_UP
        || value == KEY_PRESS_LEFT || value == KEY_PRESS_RIGHT)
        return true;
    else
        return false;
}

int Alive::determineDirection(int key)
{
    switch (key)
    {
        case KEY_PRESS_UP:
        {
            return up;
            break;
        }
        case KEY_PRESS_DOWN:
        {
            return down;
            break;
        }
        case KEY_PRESS_RIGHT:
        {
            return right;
            break;
        }
        case KEY_PRESS_LEFT:
        {
            return left;
            break;
        }
        default:
            return right;
    }
}

bool Player::moveMarble(Actor* marble, int direction, int x, int y)
{
   int tempX = x;
   int tempY = y;
   newPosition(direction, tempX, tempY);
   if (getWorld() -> getActor(tempX, tempY) == nullptr ||
       getWorld() -> getActor(tempX, tempY) -> isPit())
   {
       if (getWorld() -> getActor(tempX, tempY) != nullptr && getWorld() -> getActor(tempX, tempY) -> isPit())
       {
           getWorld() -> getActor(tempX, tempY) -> setWall(false);
           marble -> updateHp(10, marble);
       }
       else
           marble -> moveTo(tempX, tempY);

       return true;
   }
   else
       return false;
}

void Player::doSomething()
{
    int key;
    getWorld() -> getKey(key);

    if (isDirection(key))
    {
        int direction = determineDirection(key);
        setDirection(direction);
        int x = getX();
        int y = getY();

        newPosition(direction, x, y);

        if (getWorld() -> getActor(x, y) != nullptr)
        {
            if (getWorld() -> getActor(x, y) -> isMarble())
            {
                if (moveMarble(getWorld() -> getActor(x, y), direction, x, y))
                    moveTo(x, y);
                return;
            }
            else if (getWorld() -> getActor(x, y) -> isGoodie())
            {
                getWorld() -> getActor(x, y) -> triggerEffect();
                moveTo(x, y);
            }
            else if (getWorld() -> getActor(x, y) -> isExit())
            {
                if (getWorld() -> getPlayer() -> getCrystalCount() <= 0)
                {
                    getWorld() -> playSound(SOUND_FINISHED_LEVEL);
                    getWorld() -> increaseScore(2000);
                    getWorld() -> increaseScore(getWorld() -> getBonus());
                    getWorld() -> setFinishedLevel(true);
                }
                else
                    moveTo(x, y);
            }
            else
                return;
        }
        else
            moveTo(x, y);
    }
    else if (key == KEY_PRESS_SPACE)
    {
        if (m_ammoCount > 0)
        {
        Actor* pea = new Pea(getX(), getY(), getDirection(), getWorld(), this);
        getWorld() -> addActor(pea);
        getWorld() -> playSound(SOUND_PLAYER_FIRE);
        setAmmoCount(getAmmoCount() - 1);
        return;
        }
    }
    else if (key == KEY_PRESS_ESCAPE)
    {
        getWorld() -> getPlayer() -> setAlive(false);
    }
    else if (key == KEY_PRESS_TAB)
    {
        getWorld() -> getPlayer() -> updateHp(2, getWorld() -> getPlayer());
    }

    return;
}

void Pea::doSomething()
{
    if (!isAlive())
       return;
    else
    {
        int x = getX();
        int y = getY();
        newPosition(getDirection(), x, y);

        if (getWorld() -> getActor(x, y) == nullptr || getWorld() -> getActor(x, y) -> passThrough())
        {
            if (getWorld() -> hasThiefOnTop(x, y))
            {
                getWorld() -> getThief(x, y) -> updateHp(2, getWorld() -> getThief(x, y));
                setVisible(false);
                setHp(0);
                setAlive(false);
                return;
            }
            setVisible(true);
            moveTo(x, y);
        }
        else
        {
            if (getWorld() -> getActor(x, y) -> isAlive())
            {
                getWorld() -> getActor(x, y) -> updateHp(2, getWorld() -> getActor(x, y));
            }
            setVisible(false);
            setHp(0);
            setAlive(false);
        }
    }
}

void Crystal::triggerEffect()
{
    getWorld() -> increaseScore(50);
    setGoodie(false);
    getWorld() -> playSound(SOUND_GOT_GOODIE);
    getWorld() -> getPlayer() -> decCrystalCount();
}

void Exit::doSomething()
{
    if (getWorld() -> getPlayer() -> getCrystalCount() > 0)
        return;
    else
    {
        if (!revealed)
        {
            setVisible(true);
            getWorld() -> playSound(SOUND_REVEAL_EXIT);
            revealed = true;
        }
    }
}

void ExtraLifeGoodie::triggerEffect()
{
    getWorld() -> incLives();
    getWorld() -> increaseScore(1000);
    getWorld() -> playSound(SOUND_GOT_GOODIE);
    setGoodie(false);
}

void RestoreHealthGoodie::triggerEffect()
{
    getWorld() -> getPlayer() -> setHp(20);
    getWorld() -> increaseScore(500);
    getWorld() -> playSound(SOUND_GOT_GOODIE);
    setGoodie(false);
}

void AmmoGoodie::triggerEffect()
{
    getWorld() -> getPlayer() -> setAmmoCount(getWorld() -> getPlayer() -> getAmmoCount() + 20);
    getWorld() -> increaseScore(100);
    getWorld() -> playSound(SOUND_GOT_GOODIE);
    setGoodie(false);
}

void Robot::computeTicks()
{
    m_tick = (28 - getWorld() -> getLevel()) / 4;

    if (m_tick < 3)
        m_tick = 3;
}

void RageBot::doSomething()
{
    if (m_currentTick < m_tick)
    {
        m_currentTick++;
        return;
    }
    else
    {
        RageBotFunction();
        m_currentTick = 0;
        return;
    }
}

void RageBot::RageBotFunction()
{
    int x = getX();
    int y = getY();
    newPosition(getDirection(), x, y);
    if (isPlayerInSight(getX(), getY(), getDirection()))
    {
        Actor* pea = new Pea(getX(), getY(), getDirection(), getWorld(), this);
        getWorld() -> addActor(pea);
        getWorld() -> playSound(SOUND_ENEMY_FIRE);
    }
    else
    {
        if (canMove(x, y))
            moveTo(x, y);
        else
        {
            int direction = getDirection();
            switch (direction)
            {
                case up:
                    setDirection(down);
                    break;
                case down:
                    setDirection(up);
                    break;
                case left:
                    setDirection(right);
                    break;
                case right:
                    setDirection(left);
                        break;
                default:
                    break;
            }
        }
    }
}

bool Robot::isPlayerInSight(int xPos, int yPos, int direction)
{
    switch (direction)
    {
        case up:
        {
            if (getWorld() -> getPlayer() -> getX() != xPos)
                return false;
            else if (getWorld() -> getPlayer() -> getY() > yPos)
            {
                for (int i = yPos + 1; i < getWorld() -> getPlayer() -> getY(); i++)
                {//case where the actor is something that blocks sight
                 //need to add || case for when the actor is a factory
                    if (!canMove(xPos, i) && !(getWorld() -> getActor(xPos, i) -> isPit()))
                        return false;
                }
                return true;
            }
            break;
        }
        case down:
        {
            if (getWorld() -> getPlayer() -> getX() != xPos)
                return false;
            else if (getWorld() -> getPlayer() -> getY() < yPos)
            {
                for (int i = yPos - 1; i > getWorld() -> getPlayer() -> getY(); i--)
                {
                    if (!canMove(xPos, i) && !(getWorld() -> getActor(xPos, i) -> isPit()))
                        return false;
                }
                return true;
            }
            break;
        }
        case left:
        {
            if (getWorld() -> getPlayer() -> getY() != yPos)
                return false;
            else if (getWorld() -> getPlayer() -> getX() < xPos)
            {
                for (int i = xPos - 1; i > getWorld() -> getPlayer() -> getX(); i--)
                {//case where the actor is something that blocks sight
                 //need to add || case for when the actor is a factory
                    if (!canMove(i, yPos) && !(getWorld() -> getActor(i, yPos) -> isPit()))
                        return false;
                }
                return true;
            }
            break;
        }
        case right:
        {
            if (getWorld() -> getPlayer() -> getY() != yPos)
                return false;
            else if (getWorld() -> getPlayer() -> getX() > xPos)
            {
                for (int i = xPos + 1; i < getWorld() -> getPlayer() -> getX(); i++)
                {//case where the actor is something that blocks sight
                 //need to add || case for when the actor is a factory
                    if (!canMove(i, yPos) && !(getWorld() -> getActor(i, yPos) -> isPit()))
                        return false;
                }
                return true;
            }
            break;
        }

        default:
            return false;
    }
    return false;
}

int ThiefBot::randomNumberGen(int start, int end)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(start,end);
    int result = distribution(gen);
    return result;
}

void ThiefBot::computeDistance()
{
    m_distanceBeforeTurning = randomNumberGen(1, 6);
}

void ThiefBot::doSomething()
{
    if (m_currentTick < m_tick)
    {
        m_currentTick++;
        return;
    }
    else
    {
        ThiefBotFunction();
        m_currentTick = 0;
    }
}

void ThiefBot::ThiefBotFunction()
{
     int x = getX();
     int y = getY();
     newPosition(getDirection(), x, y);
     if (m_distanceBeforeTurning == 0 || !canMove(x, y))
     {
         computeDistance();
         bool bup = true;
         bool bdown = true;
         bool bleft = true;
         bool bright = true;
         int direction = -1;

         while (direction == -1)
         {
             direction = findDirection(bup, bdown, bleft, bright);
             int xTemp = getX();
             int yTemp = getY();
             newPosition(direction, xTemp, yTemp);
             if (!canMove(xTemp, yTemp))
             {
                 switch (direction)
                 {
                     case up:
                     {
                         bup = false;
                         break;
                     }
                     case down:
                     {
                         bdown = false;
                         break;
                     }
                     case left:
                     {
                         bleft = false;
                         break;
                     }
                     case right:
                     {
                         bright = false;
                         break;
                     }
                     default:
                         break;
                 }
                 direction = -1;
             }
             if (!bup && !bdown && !bleft && !bright)
                 break;
         }
         if (direction != -1)
         {
             setDirection(direction);
         }
     }
     else
     {
         if (getWorld() -> getActor(x, y) != nullptr)
         {
             if (getWorld() -> getActor(x, y) -> isGoodie() && !(getWorld() -> getActor(x, y) -> isCrystal()))
             {
                 if (!m_hasGoodie)
                 {
                     int random = randomNumberGen(1, 10);
                     if (random == 1)
                     {
                         m_hasGoodie = true;
                         setGoodie(getWorld() -> getActor(x, y));
                         getWorld() -> getActor(x, y) -> moveTo(100, 100);
                         getWorld() -> playSound(SOUND_ROBOT_MUNCH);
                     }
                 }
             }
         }
         moveTo(x, y);
         m_distanceBeforeTurning--;
     }
}

int ThiefBot::findDirection(bool up, bool down, bool left, bool right)
{
    int direction = randomNumberGen(1, 4);
    switch (direction) {
        case 1:
        {
            if (!up)
                return -1;
            else
                return Actor::up;
            break;
        }
        case 2:
        {
            if (!down)
                return -1;
            else
                return Actor::down;
            break;
        }
        case 3:
        {
            if (!left)
                return -1;
            else
                return Actor::left;
            break;
        }
        case 4:
        {
            if (!right)
                return -1;
            else
                return Actor::right;
            break;
        }
    }
    return -1;
}

void MeanThiefBot::doSomething()
{
    if (m_currentTick < m_tick)
    {
        m_currentTick++;
        return;
    }
    else
    {
        int x = getX();
        int y = getY();
        newPosition(getDirection(), x, y);
        if (isPlayerInSight(getX(), getY(), getDirection()))
        {
            Actor* pea = new Pea(getX(), getY(), getDirection(), getWorld(), this);
            getWorld() -> addActor(pea);
            getWorld() -> playSound(SOUND_ENEMY_FIRE);
        }
        else
        {
            ThiefBotFunction();
        }
        m_currentTick = 0;
    }
}

void ThiefFactory::doSomething()
{
    int count = countThiefBots();
    if (count < 3 && !(getWorld() -> hasThiefOnTop(getX(), getY())))
    {
        if (m_makeMean)
        {
            Actor* meanThief = new MeanThiefBot(getX(), getY(), getWorld());
            getWorld() -> addActor(meanThief);
        }
        else
        {
            Actor* thief = new ThiefBot(IID_THIEFBOT, getX(), getY(), 5, getWorld());
            getWorld() -> addActor(thief);
        }
    }
}

int ThiefFactory::countThiefBots()
{
    int result = 0;
    int xPos = getX();
    int yPos = getY();

    result = getWorld() -> findThief(xPos, yPos);
    return result;
}
