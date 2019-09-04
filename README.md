# codebusters-player
Heuristic-based AI used while competing in the CodinGame CodeBusters challenge. Made it to the "Legendary" tier and got 21st place overall. This project was written in 1 week because that was the duration of the challenge and confined to a single file due to the submission system of Codingame.
## What's the challenge?
The goal of the game is to collect ghosts by busting and trapping them, and them bringing them to your base. The player who successfully brings the most ghosts to their base wins. Each player has a team of busters and the busters have a variety of actions: MOVE, BUST, RELEASE and STUN.  
The full rules can be found on the challenge webpage at https://www.codingame.com/multiplayer/bot-programming/codebusters (Some rules are different from the original version, as there were slight modifications after the initial challenge.)  
The leaderboard can be found at https://www.codingame.com/leaderboards/challenge/codebusters/global
## How does the code work?
Each agent acts as a state machine with decentralized decisionmaking and centralized memory & perception. Several heuristics are used to decide which state is most appropriate for the selected buster given the current situation. These heuristics were tuned over the course of the competition, albeit not very aggressively. The list of states are as follows:  
### Filler state
DEAD - When the buster is stunned, ignore all other possible states.  
### Stunning states
STUN - Stun target enemy buster near selected buster.  
CHASE ENEMY -  Follow an enemy buster to catch and stun them.  
### Ghost carrying states
SAFE - Ghost succesfully brought to base, release ghost.  
EVADE - Avoid nearby enemy busters to prevent them from stunning selected buster.  
DELIVERY - Move towards the base to carry the ghost back. 
### Cooperative movement states
HUNT - Look for an enemy buster that is believed to be carrying a ghost, and approach them to stun them.  
ASSIST - Help a buster that is carrying a ghost and has nearby enemies.  
INVESTIGATE - If an enemy buster is visibly busting an unknown ghost, move towards them to find the ghost.  
### Busting states
TOO FAR - Attempting to bust ghost, but not close enough. Move towards ghost.  
TOO CLOSE - Attempting to bust ghost, but too close. Move away from ghost.  
EXTERMINATE - Bust ghost.  
### Information gathering states
INSPECT - Check positions of previously found ghosts to see if they're still there.  
SCOUTING - Move along scouting path to check unknown positions.  
PREPARE SCOUT - Move towards set scouting path to prepare for scouting.  
## How could it have been improved?
Several of those ahead of me used a strategy called "herding" where they tried to push the ghosts towards their own base. This made it easier to get the ghosts before the other player. Unfortunately, I couldn't find a way to integrate the idea into my code.  
My "scouting" technique for finding ghosts when lacking information was very simplistic, and could be replaced with a better navigation script.  
To some degree, the movement of ghosts and enemies can be simulated and predicted, this would take some time, but it could be useful.
My combat AI could use some refinement, and the match I lose in the examples (below) is a case where it's especially troublesome.  
Of course, alternatives to heuristic-based AI are also possible, and some of the competitors built their own simulation to try and use approaches like genetic learning.  
## Example matches:
https://www.codingame.com/replay/113706506 (Win)  
https://www.codingame.com/replay/113703870 (Tie)  
https://www.codingame.com/replay/113698461 (Lose)  
