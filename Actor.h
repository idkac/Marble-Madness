#ifndef ACTOR_H_
#define ACTOR_H_

#include "GameConstants.h"
#include "GraphObject.h"
#include "StudentWorld.h"
#include <type_traits>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

//need to make a class for collectables (aka crystal hp ammo etc)
class Actor: public GraphObject
{
    public:
        Actor(int imageID, int startX, int startY, StudentWorld* world)
            : GraphObject(imageID, startX, startY), m_world(world), m_isMarble(false),
              m_isWall(false), m_isPlayer(false), canPassThrough(false), m_isPit(false), m_isGoodie(false)
            , m_isExit(false), m_isRobot(false), m_isCrystal(false), m_isFactory(false), m_isThief(false), m_hasGoodie(false){};
        virtual void doSomething() = 0;
        StudentWorld* getWorld();
        bool canMove(int x, int y); //temp == nullptr not player not wall not factor not robot not marble
        bool isMarble() const { return m_isMarble; };
        bool isAlive() const { return m_isAlive; };
        void setAlive(bool isAlive) { m_isAlive = isAlive; };
        bool isWall() const { return m_isWall; };
        void setWall(bool isWall) { m_isWall = isWall; };
        bool isPlayer() const { return m_isPlayer; };
        void setPlayer(bool isPlayer) { m_isPlayer = isPlayer; };
        void setPassThrough (bool passThrough) { canPassThrough = passThrough; };
        bool passThrough() const { return canPassThrough; };
        bool isPit() const { return m_isPit; };
        void setPit(bool isPit) { m_isPit = isPit; };
        int returnHp() const { return m_hp; };
        void setHp(int value) { m_hp = value; };
        void setGoodie(bool isGoodie) { m_isGoodie = isGoodie; };
        bool isGoodie() const { return m_isGoodie; };
        bool isExit() const { return m_isExit; };
        void setExit(bool isExit) { m_isExit = isExit; };
        bool isRobot() const { return m_isRobot; };
        void setRobot(bool isRobot) { m_isRobot = isRobot; };
        bool isFactory() const { return m_isFactory; };
        void setFactory(bool isFactory) { m_isFactory = isFactory; };
        bool isCrystal() const { return m_isCrystal; };
        void setCrystal(bool isCrystal) { m_isCrystal = isCrystal; };
        bool isThief() const { return m_isThief; };
        void setThief(bool isThief) { m_isThief = isThief; };
        bool updateHp(int value, Actor* actorToPlaySound);
        Actor* getGoodie() { return m_Goodie;};
        void setGoodie(Actor* goodie) {m_Goodie = goodie;};
        bool hasGoodie() const { return m_hasGoodie; };
        void setHasGoodie(bool hasGoodie) {m_hasGoodie = hasGoodie;};
// updatesHp by subtracting it and returns isAlive
        virtual void triggerEffect() = 0;
    protected:
        StudentWorld* m_world;
        bool m_isMarble;
        bool m_isAlive;
        bool m_isWall;
        bool m_isPlayer;
        bool canPassThrough;
        bool m_isPit;
        bool m_isGoodie;
        bool m_isExit;
        bool m_isRobot;
        bool m_isFactory;
        bool m_isCrystal;
        bool m_isThief;
        bool m_hasGoodie;
        Actor* m_Goodie;
        int m_hp;

};

class Alive: public Actor //base class for all things alive, will never be called
{
   public:
        Alive(int imageID, int startX, int startY, int hp, StudentWorld* world)
            : Actor(imageID, startX, startY, world) { setAlive(true); setHp(hp); };
        virtual void doSomething() = 0;
        int determineDirection(int key); //determines the direction of the key that was inputted
        int isDirection(int value); //determines in the key inputed is a directional key
        void newPosition(int direction, int& x, int& y); //finds new position and updates x and y by reference
        virtual void triggerEffect() { return; };
};

class Player: public Alive
{
    public:
        Player(int startX, int startY, StudentWorld* world)
            : Alive(IID_PLAYER, startX, startY, 20, world) , m_ammoCount(20), m_crystalCount(0)
        {
            setDirection(right);
            setPlayer(true);
        };
        virtual void doSomething();
        bool moveMarble(Actor* marble, int direction, int x, int y);
        int getAmmoCount() const { return m_ammoCount; };
        void setAmmoCount(int ammo) { m_ammoCount = ammo; };
        void incCrystalCount() { m_crystalCount++; };
        void decCrystalCount() { m_crystalCount--; };
        int getCrystalCount() const { return m_crystalCount; };
    protected:
        int m_ammoCount;
        int m_crystalCount;
};

class Robot: public Alive
{
    public:
        Robot(int imageID, int startX, int startY, int direction, int hp, StudentWorld* world)
            : Alive(imageID, startX, startY, hp, world) { setRobot(true); setDirection(direction); computeTicks(); };
        virtual void doSomething() { return; };
        void computeTicks();
        bool isPlayerInSight(int xPos, int yPos, int direction);
    protected:
        int m_tick;
        int m_currentTick;
};

class RageBot: public Robot
{
    public:
        RageBot(int startX, int startY, int direction, StudentWorld* world)
            : Robot(IID_RAGEBOT, startX, startY, direction, 10, world) {};
        virtual void doSomething();
        void RageBotFunction(); //handles firing and also movement
};

class ThiefBot: public Robot
{
    public:
        ThiefBot(int imageID, int startX, int startY, int hp, StudentWorld* world)
            : Robot(imageID, startX, startY, Actor::right, hp, world) { computeDistance(); setThief(true); setPassThrough(false);};
        virtual void doSomething();
        void computeDistance();
        int findDirection(bool up, bool down, bool left, bool right);
        int randomNumberGen(int start, int end);
        void ThiefBotFunction();
    protected:
        bool positionHasGoodie;
        int m_distanceBeforeTurning;
};

class MeanThiefBot: public ThiefBot
{
    public:
        MeanThiefBot(int startX, int startY, StudentWorld* world)
            : ThiefBot(IID_MEAN_THIEFBOT, startX, startY, 8, world) {};
        virtual void doSomething();
};

class ThiefFactory: public Actor
{
    public:
        ThiefFactory(int startX, int startY, bool makeMean, StudentWorld* world)
            : Actor(IID_ROBOT_FACTORY, startX, startY, world), m_makeMean(makeMean) { setFactory(true); setPassThrough(true); };
        void setMean(bool makeMean) { m_makeMean = makeMean; }
        virtual void doSomething();
        int countThiefBots();
        virtual void triggerEffect() {return;};
    protected:
        bool m_makeMean;
};

class Marble: public Alive
{
    public:
        Marble(int startX, int startY, StudentWorld* world)
            : Alive(IID_MARBLE, startX, startY, 10, world)
        {
            m_isMarble = true;
            setDirection(none);
        }
        virtual void doSomething() { return; }
};

class Pea: public Alive
{
    public:
        Pea(int startX, int startY, int direction, StudentWorld* world, Actor* whoFired)
            : Alive(IID_PEA, startX, startY, 1, world) { setVisible(false); setDirection(direction);
            setPassThrough(true); };
        virtual void doSomething();
    private:
        Actor* m_whoFired;
};

class Wall: public Actor
{
    public:
        Wall(int imageID, int startX, int startY, StudentWorld* world)
            : Actor(imageID, startX, startY, world) { setAlive(false); setWall(true); };
        virtual void doSomething() { return; };
        virtual void triggerEffect() { return; };
};

class Pit: public Wall
{
    public:
        Pit(int startX, int startY, StudentWorld* world)
            : Wall(IID_PIT, startX, startY, world) { setPassThrough(true); setPit(true); };
        virtual void doSomething() { return; }; //pit being removed with marble is implemented in player's marble
                                                //funtion, so this doesnt need to do anything ever
};

class Exit: public Actor
{
    public:
        Exit(int startX, int startY, StudentWorld* world)
            : Actor(IID_EXIT, startX, startY, world), revealed(false) { setAlive(false); setExit(true); setVisible(false); };
        virtual void doSomething();
        virtual void triggerEffect() { return; };
    protected:
        bool revealed;
};

class Goodie: public Actor
{
    public:
        Goodie(int imageID, int startX, int startY, StudentWorld* world)
            : Actor(imageID, startX, startY, world) { setAlive(false); setGoodie(true);
            setPassThrough(true); };
        virtual void doSomething() { return; };
};

class Crystal: public Goodie
{
    public:
        Crystal(int startX, int startY, StudentWorld* world)
            : Goodie(IID_CRYSTAL, startX, startY, world) { setCrystal(true); };
        virtual void doSomething() { return; };
    protected:
        virtual void triggerEffect();
};

class ExtraLifeGoodie: public Goodie
{
    public:
        ExtraLifeGoodie(int startX, int startY, StudentWorld* world)
            : Goodie(IID_EXTRA_LIFE, startX, startY, world) {};
        virtual void doSomething() { return; };
    protected:
        virtual void triggerEffect();
};

class RestoreHealthGoodie: public Goodie
{
    public:
        RestoreHealthGoodie(int startX, int startY, StudentWorld* world)
            : Goodie(IID_RESTORE_HEALTH, startX, startY, world) {};
        virtual void doSomething() { return; };
    protected:
        virtual void triggerEffect();
};

class AmmoGoodie: public Goodie
{
    public:
        AmmoGoodie(int startX, int startY, StudentWorld* world)
            : Goodie(IID_AMMO, startX, startY, world) {};
        virtual void doSomething() { return; };
    protected:
        virtual void triggerEffect();
};


#endif // ACTOR_H_
