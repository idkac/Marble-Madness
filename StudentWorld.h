#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_
#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <vector>
#include "Actor.h"
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Player;

class StudentWorld : public GameWorld
{
        public:
                StudentWorld(std::string assetPath);
                virtual int init();
                virtual int move();
                virtual void cleanUp();
                ~StudentWorld();
                Actor* getActor(int x, int y);
                bool checkLocation(double input, double self);
                void addActor(Actor* actor);
                void removeDeadActors();
                Player* getPlayer() const { return m_player; };
                int getBonus() const { return bonus; };
                void setFinishedLevel(bool finishedLevel) { m_finishedLevel = finishedLevel; };
                bool isFinishedLevel() const { return m_finishedLevel; };
                int findThief(int xPos, int yPos);
                bool hasThiefOnTop(int xPos, int yPos);
                Actor* getThief(int x, int y);
        private:
                std::vector<Actor*> actorList;
                Player* m_player;
                int bonus, score, level;
                std::string curLevel;
                bool m_finishedLevel;
};

#endif // STUDENTWORLD_H_
