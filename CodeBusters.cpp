#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

/**
 * Send your busters out into the fog to trap ghosts and bring them home!
 **/
 
//Pythagorean distance utility
int py(int x1, int y1, int x2, int y2)
{
    int x=x2-x1;
    int y=y2-y1;
    return sqrt((x*x)+(y*y));
}

//Utility for counting nearby friendles vs. enemies
int surroundingChars(int chosen, int busterX[], int busterY[], int busterState[],int busterValue[],int availableStun[], int enemyX[],int enemyY[], int enemyState[],int enemyValue[], int bustersPerPlayer,int enemyIndex)
{
    int output=0;
    int chosenX=busterX[chosen];
    int chosenY=busterY[chosen];
    for(int i=0;i<bustersPerPlayer;i++)
    {
        if(py(chosenX,chosenY,busterX[i],busterY[i])<3000 && !(busterState[i]==2 && busterValue[i]>=5) && availableStun[i]<=5)
        {
            output++;
        }
    }
    for(int i=0;i<enemyIndex;i++)
    {
        if(py(chosenX,chosenY,enemyX[i],enemyY[i])<3000 && !(enemyState[i]==2 && enemyValue[i]>=5))
        {
            output--;
        }
    }
    return output;
}

int main()
{
    int score=0;
    int bustersPerPlayer; // the amount of busters you control
    cin >> bustersPerPlayer; cin.ignore();
    int ghostCount; // the amount of ghosts on the map
    cin >> ghostCount; cin.ignore();
    int myTeamId; // if this is 0, your base is on the top left of the map, if it is one, on the bottom right
    cin >> myTeamId; cin.ignore();
    
    int turns=0;
    int map[17][10];
    for(int i=0;i<17;i++)
    {
        for(int j=0;j<10;j++)
        {
            map[i][j]=200;
        }
    }
    //Seen enemies
    int enemySeen=-100;
    int seenX;
    int seenY;
    int seenId;
    
    int recentEnemyX[bustersPerPlayer];
    int recentEnemyY[bustersPerPlayer];
    int recency[bustersPerPlayer];
    //Previous ghosts
    int prevX[ghostCount];
    int prevY[ghostCount];
    int prevHealth[ghostCount];
    bool prevFree[ghostCount];
    bool ghostSeen[ghostCount];
    
    int newRelease=-1;
    int releaseTurn=-10;
    
    for(int i=0;i<ghostCount;i++)
    {
        prevFree[i]=false;
        ghostSeen[i]=false;
    }
    //Custom values for ghost 0
    prevX[0]=8000;
    prevY[0]=4500;
    prevHealth[0]=3;
    prevFree[0]=true;
    
    int scoutX[2];
    int otherScoutX[2];
    
    int availableStun[bustersPerPlayer];
    int chasingFor[bustersPerPlayer];
    bool isChasing[bustersPerPlayer];
    for(int i=0;i<bustersPerPlayer;i++)
    {
        chasingFor[i];
        isChasing[i]=false;
        availableStun[i]=0;
    }
    
    int baseCoordX;
    int baseCoordY;
    int otherBaseX;
    int otherBaseY;
    int otherCoreX;
    int otherCoreY;
    if(myTeamId==0)
    {
        baseCoordX = 0;
        baseCoordY = 0;
        otherBaseX = 14000;
        otherBaseY = 7000;
        otherCoreX = 16000;
        otherCoreY = 9000;
        
        scoutX[0] = 16000;
        scoutX[1] = 16000;
        otherScoutX[0] = 0;
        otherScoutX[1] = 0;
    }
    else
    {
        baseCoordX = 16000;
        baseCoordY = 9000;
        otherBaseX = 2000;
        otherBaseY = 2000;
        otherCoreX = 0;
        otherCoreY = 0;
        
        scoutX[0] = 0;
        scoutX[1] = 0;
        otherScoutX[0] = 16000;
        otherScoutX[1] = 16000;
    }
    
    // game loop
    while (1) {
        string busterCommand[bustersPerPlayer];
        
        for(int i=0;i<17;i++)
        {
            for(int j=0;j<10;j++)
            {
                map[i][j]++;
            }
        }
        
        int entities; // the number of busters and ghosts visible to you
        
        cin >> entities; cin.ignore();
        //Ghost variables
        int ghostId[ghostCount];
        int ghostX[ghostCount];
        int ghostY[ghostCount];
        int ghostState[ghostCount];
        int ghostAttackers[ghostCount];
        int myAttackers[ghostCount];
        bool seeGhost[ghostCount];
        int ghostIndex=0;
        //Enemy variables
        int enemyId[bustersPerPlayer];
        int enemyX[bustersPerPlayer];
        int enemyY[bustersPerPlayer];
        int enemyState[bustersPerPlayer];
        int enemyTarget[bustersPerPlayer];
        int enemyIndex=0;
        //Buster variables
        int busterX[bustersPerPlayer];
        int busterY[bustersPerPlayer];
        int busterState[bustersPerPlayer];
        int busterValue[bustersPerPlayer];
        int scoutY[2]={2100,6900};
        int neededHelp[bustersPerPlayer];
        for(int i=0;i<bustersPerPlayer;i++)
        {
            recency[i]++;
        }
        for(int i=0;i<ghostCount;i++)
        {
            seeGhost[i]=false;
            myAttackers[i]=0;
        }
        
        turns++;
        
        if(turns-enemySeen<30)
        {
            seenX+=800*(otherCoreX-seenX)/py(seenX,seenY,otherCoreX,otherCoreY);
            seenY+=800*(otherCoreY-seenY)/py(seenX,seenY,otherCoreX,otherCoreY);
        }
        cerr << turns << endl;
        for (int i = 0; i < entities; i++) {
            int entityId; // buster id or ghost id
            int x;
            int y; // position of this buster / ghost
            int entityType; // the team id if it is a buster, -1 if it is a ghost.
            int state; // For busters: 0=idle, 1=carrying a ghost.
            int value; // For busters: Ghost id being carried. For ghosts: number of busters attempting to trap this ghost.
            cin >> entityId >> x >> y >> entityType >> state >> value; cin.ignore();
            //cerr << entityId << " " << entityType << endl;
            //Code for own busters
            if(entityType==myTeamId)
            {
                if(myTeamId==1)
                {
                    entityId-=bustersPerPlayer;
                }
                busterX[entityId]=x;
                busterY[entityId]=y;
                busterState[entityId]=state;
                busterValue[entityId]=value;
                if(state==3)
                {
                    myAttackers[value]++;
                }
                
                if(state==1)
                {
                    prevFree[value]=false;
                }
                for(int j=0;j<17;j++)
                {
                    for(int k=0;k<10;k++)
                    {
                        if(py(x,y,j*1000,k*1000)<2200)
                        {
                            map[j][k]=0;
                        }
                    }
                }
            }
            //Code for ghosts
            else if(entityType==-1)
            {
                ghostId[ghostIndex]=entityId;
                ghostX[ghostIndex]=x;
                ghostY[ghostIndex]=y;
                ghostState[ghostIndex]=state;
                ghostAttackers[ghostIndex]=value;
                ghostIndex++;
                
                seeGhost[entityId]=true;
                
                prevX[entityId]=x;
                prevY[entityId]=y;
                prevHealth[entityId]=state;
                prevFree[entityId]=true;
                if(entityId!=0 && turns<120 && !ghostSeen[entityId] && (state==3 || state==15 || state==40))
                {
                    int symmetryId,symmetryX,symmetryY;
                    if(entityId%2==0)
                    {
                        symmetryId=entityId-1;
                    }
                    else
                    {
                        symmetryId=entityId+1;
                    }
                    symmetryX=16000-x;
                    symmetryY=9000-y;
                    if(!ghostSeen[symmetryId] && py(symmetryX,symmetryY,otherCoreX,otherCoreY)>10000)
                    {
                        prevX[symmetryId]=symmetryX;
                        prevY[symmetryId]=symmetryY;
                        prevHealth[symmetryId]=state;
                        prevFree[symmetryId]=true;
                    }
                }
                ghostSeen[entityId]=true;
            }
            //Code for enemies
            else
            {
                enemyId[enemyIndex]=entityId;
                enemyX[enemyIndex]=x;
                enemyY[enemyIndex]=y;
                enemyState[enemyIndex]=state;
                enemyTarget[enemyIndex]=value;
                enemyIndex++;
                
                if(state==1)
                {
                    enemySeen=turns;
                    seenX=x;
                    seenY=y;
                    seenId=entityId;
                    
                    prevFree[value]=false;
                    ghostSeen[value]=true;
                }
                if(entityId==seenId && state!=1)
                {
                    enemySeen=-100;
                }
                
                if(myTeamId==0)
                {
                    entityId-=bustersPerPlayer;
                }
            }
        }
        for(int i = 0;i < bustersPerPlayer;i++) 
        {
            neededHelp[i]=surroundingChars(i,busterX,busterY,busterState,busterValue,availableStun,enemyX,enemyY,enemyState,enemyTarget,bustersPerPlayer,enemyIndex);
            if(py(busterX[i],busterY[i],otherCoreX,otherCoreY)<8000 && busterState[i]==1)
            {
                neededHelp[i]-=1;
            }
            else if(py(busterX[i],busterY[i],baseCoordX,baseCoordY)<6000 && busterState[i]==1)
            {
                neededHelp[i]+=1;
            }
        }
        for (int i = 0; i < bustersPerPlayer; i++) 
        {
            bool noCommand = true;
            if(availableStun[i]>0)
            {
                cerr << availableStun[i] << " a " << i << endl;
                availableStun[i]--;
            }

            int x=busterX[i];
            int y=busterY[i];
            
            //Ignore anything else when stunned
            if(busterState[i]==2)
            {
                //Dead
                stringstream ss;
                ss << "MOVE " << otherBaseX << " " << otherBaseY <<" DEAD";
                busterCommand[i]=ss.str();
                noCommand=false;
            }
            
            //Stun enemies
            if(enemyIndex>0 && availableStun[i]==0 && noCommand)
            {
                int lowestId,lowestX,lowestY,lowestState,lowestDist,lowestTarget,lowestIndex;
                int lowestValue=100000;
                for(int j=0;j<enemyIndex;j++)
                {
                    int modif=0;
                    int distance = py(x,y,enemyX[j],enemyY[j]);
                    if(enemyState[j]==3)
                    {
                        bool seeGhost = false;
                        for(int k=0;k<ghostIndex;k++)
                        {
                            if(enemyTarget[j]==ghostId[k])
                            {
                                if(((myAttackers[k]==0 && (ghostAttackers[k]*4>ghostState[k])) || ghostState[k]<(6*myAttackers[k])) && !(busterState[i]==3 && enemyTarget[j]!=busterValue[i]))
                                {
                                    enemyState[j]=1;
                                }
                                seeGhost = true;
                                break;
                            }
                        }
                        if(!seeGhost)
                        {
                            enemyState[j]=0;
                        }
                    }
                    if(enemyState[j]==1)
                    {
                        modif=-3000;
                    }
                    if((distance+modif)<lowestValue && (enemyState[j]!=2 && enemyState[j]!=3) && py(enemyX[j],enemyY[j],otherCoreX,otherCoreY)>1600)
                    {
                        lowestValue=distance+modif;
                        lowestDist=distance;
                        lowestIndex=j;
                        lowestX=enemyX[j];
                        lowestY=enemyY[j];
                        lowestId=enemyId[j];
                        lowestState=enemyState[j];
                        lowestTarget=enemyTarget[j];
                    }
                }
                cerr << i  << " a " << lowestValue << " b " << lowestState << " c " << lowestId << endl;
                if(lowestValue!=100000)
                {
                    //Removed as this reduced score in the final version
                    /*bool onlyStunner = true;
                    for(int k=0;k<bustersPerPlayer;k++)
                    {
                        if(k!=i && py(x,y,busterX[k],busterY[k])<3000)
                        {
                            //onlyStunner=true;
                            if(availableStun[k]<=5)
                            {
                                onlyStunner=false;
                            }
                        }
                    }*/
                    if(lowestDist<1760 && lowestState!=3 /*&& !(busterState[i]==1 && neededHelp[i]>-1 && !onlyStunner)*/)
                    {
                        stringstream ss;
                        enemyState[lowestIndex]=2;
                        ss << "STUN " << lowestId << " STUN";
                        busterCommand[i]=ss.str();
                        availableStun[i]=20;
                        if(lowestState==1)
                        {
                            //THIS CODE DOES NOT CHASE RELEASED GHOSTS
                            prevX[lowestTarget]=lowestX;
                            prevY[lowestTarget]=lowestY;
                            prevHealth[lowestTarget]=0;
                            prevFree[lowestTarget]=true;
                            newRelease = lowestTarget;
                            releaseTurn = turns;
                        }
                        if(lowestId==seenId)
                        {
                            enemySeen=-100;
                        }
                        noCommand=false;
                    }
                    else if(lowestState==1 && busterState[i]!=1 && lowestDist<5000 && chasingFor[i]<3)
                    {
                        //Chase enemy
                        stringstream ss;
                        ss << "MOVE " << lowestX << " " << lowestY << " CHASE ENEMY";
                        busterCommand[i]=ss.str();
                        noCommand=false;
                        isChasing[i]=true;
                        chasingFor[i]++;
                    }
                }
            }
            
            //Carry ghosts back or release them
            if(busterState[i]==1 && noCommand)
            {
                int distance = py(busterX[i],busterY[i],baseCoordX,baseCoordY);
                if(distance<1600)
                {
                    //Safe
                    score++;
                    stringstream ss;
                    ss << "RELEASE" << " SAFE";
                    busterCommand[i]=ss.str();
                    noCommand=false;
                }
                if(distance>2400 && surroundingChars(i,busterX,busterY,busterState,busterValue,availableStun,enemyX,enemyY,enemyState,enemyTarget,bustersPerPlayer,enemyIndex)<1 && noCommand)
                {
                    double movementX=0,movementY=0;
                    for(int j=0;j<bustersPerPlayer;j++)
                    {
                        int difference = py(x,y,busterX[j],busterY[j]);
                        if(j!=i && difference<3000 && difference!=0 && !(busterState[j]==2 && busterValue[j]>=5) && availableStun[i]<=5)
                        {
                            movementX+=double(busterX[j]-x)/pow(double(difference),1.1);
                            movementY+=double(busterY[j]-y)/pow(double(difference),1.1);
                        }
                    }
                    for(int j=0;j<enemyIndex;j++)
                    {
                        int difference = py(x,y,enemyX[j],enemyY[j]);
                        if(difference<3000 && difference!=0 && !(enemyState[j]==2 && enemyTarget[j]<=5))
                        {
                            movementX-=4*double(enemyX[j]-x)/pow(double(difference),1.1);
                            movementY-=4*double(enemyY[j]-y)/pow(double(difference),1.1);
                        }
                    }
                    if(turns<180)
                    {
                        movementX+=8*double(baseCoordX-x)/pow(double(distance),1.1);
                        movementY+=8*double(baseCoordY-y)/pow(double(distance),1.1);
                    }
                    else
                    {
                        movementX+=2*double(baseCoordX-x)/pow(double(distance),1.1);
                        movementY+=2*double(baseCoordY-y)/pow(double(distance),1.1);
                    }
                    cerr << "BC X " << movementX << " Y " << movementY << endl;
                    int outX = int(movementX*10000);
                    int outY = int(movementY*10000);
                    //Evade
                    stringstream ss;
                    ss << "MOVE " << x+outX << " " << y+outY << " EVADE";
                    busterCommand[i]=ss.str();
                    noCommand=false;
                }
                if(noCommand)
                {
                    int moveX = x + ((distance-1598)*(baseCoordX-x)/1598);
                    int moveY = y + ((distance-1598)*(baseCoordY-y)/1598);
                    //Delivery
                    stringstream ss;
                    ss << "MOVE " << moveX << " " << moveY << " DELIVERY";
                    busterCommand[i]=ss.str();
                    noCommand=false;
                }
            }
            
            //Steal ghosts from enemies
            if(turns-enemySeen<30 && noCommand)
            {
                int enemyDist = py(seenX,seenY,otherCoreX,otherCoreY);
                int predictX=seenX;
                int predictY=seenY;
                bool doHunt = true;
                if(py(x,y,seenX,seenY)<1000/* && neededHelp[i]<0*/)
                {
                    doHunt=false;
                    enemySeen=-100;
                }
                for(int j=1;enemyDist>1600;j++)
                {
                    cerr << predictX << " pred " << predictY << " pred2 " << enemyDist << endl;
                    predictX+=800*(otherCoreX-predictX)/enemyDist;
                    predictY+=800*(otherCoreY-predictY)/enemyDist;
                    enemyDist=py(predictX,predictY,otherCoreX,otherCoreY);
                    int predictVersus = py(predictX,predictY,x,y);
                    cerr << predictVersus << " t " << 1760+(800*j) << " t " << availableStun[i] << " t " << j << endl;
                    if(predictVersus<(1760+(800*j)) && availableStun[i]<j && doHunt)
                    {
                        int moveX=x+(1000*(predictX-x)/predictVersus);
                        int moveY=y+(1000*(predictY-y)/predictVersus);
                        cerr << moveX << " mov " << moveY << endl;
                        stringstream ss;
                        ss << "MOVE " << moveX << " " << moveY << " HUNT";
                        busterCommand[i]=ss.str();
                        noCommand=false;
                        break;
                    }
                }
            }
            
            //Help ghost carriers in trouble
            if(noCommand)
            {
                int lowestX,lowestY,lowestValue=100000,lowestId;
                for(int j=0;j<bustersPerPlayer;j++)
                {
                    if(neededHelp[j]<=0 && busterState[j]==1)
                    {
                        lowestValue=py(busterX[i],busterY[i],busterX[j],busterY[j]);
                        lowestX=busterX[j];
                        lowestY=busterY[j];
                        lowestId=j;
                    }
                }
                if(lowestValue!=100000)
                {
                    neededHelp[lowestId]++;
                    stringstream ss;
                    ss << "MOVE " << lowestX << " " << lowestY << " ASSIST";
                    busterCommand[i]=ss.str();
                    noCommand=false;
                }
            }
            
            //Investigate if enemy busting unknown ghost
            if(noCommand)
            {
                int lowestId,lowestX,lowestY;
                int lowestValue=100000;
                for(int j=0;j<enemyIndex;j++)
                {
                    int distance = py(x,y,enemyX[j],enemyY[j]);
                    if(distance<lowestValue && enemyState[j]==3)
                    {
                        bool canSee=false;
                        for(int k=0;k<ghostIndex;k++)
                        {
                            if(ghostId[k]==enemyTarget[j])
                            {
                                canSee=true;
                            }
                        }
                        if(!canSee)
                        {
                            lowestValue=distance;
                            lowestX=enemyX[j];
                            lowestY=enemyY[j];
                            lowestId=enemyId[j];
                        }
                    }
                }
                if(lowestValue!=100000)
                {
                    //Investigate
                    stringstream ss;
                    ss << "MOVE " << lowestX << " " << lowestY << " INVESTIGATE";
                    busterCommand[i]=ss.str();
                    noCommand=false;
                }
            }
            
            //Bust or hunt ghosts if any visible
            if(ghostIndex>0 && noCommand)
            {
                int lowestId,lowestX,lowestY,lowestDist,lowestState;
                int lowestValue=100000;
                for(int j=0;j<ghostIndex;j++)
                {
                    int basePriority = py(ghostX[j],ghostY[j],baseCoordX,baseCoordY)/4;
                    int distance = py(x,y,ghostX[j],ghostY[j]);
                    if(((distance+ghostState[j]*1000-myAttackers[ghostId[j]]*10000+basePriority)<lowestValue || (lowestDist<900 && distance<1760 && distance>900)) && (ghostState[j]<40 || turns>80) && (ghostState[j]<15 || turns>10))
                    {
                        int inVicinity=0;
                        bool canAdd=true;
                        bool canReach=true;
                        bool needed=true;
                        if(ghostAttackers[j]==0 && ghostState[j]<=5 && busterState[i]==0 && distance>900)
                        {
                            int leastId,leastValue=100000;
                            for(int k=0;k<bustersPerPlayer;k++)
                            {
                                int tempDist = py(busterX[k],busterY[k],ghostX[j],ghostY[j]);
                                if(tempDist<leastValue && busterState[k]==0 && tempDist>900)
                                {
                                    leastId=k;
                                    leastValue=tempDist;
                                }
                            }
                            if(leastId!=i)
                            {
                                needed=false;
                            }
                        }
                        if(ghostAttackers[j]>0 && ghostAttackers[j]!=myAttackers[j]*2)
                        {
                            if((distance-1760)>(ghostState[j]*800/ghostAttackers[j]))
                            {
                                canReach=false;
                            }
                        }
                        if(ghostAttackers[j]>myAttackers[ghostId[j]])
                        {
                            canAdd=false;
                            for(int k=0;k<bustersPerPlayer;k++)
                            {
                                if(busterState[k]!=3 && py(busterX[k],busterY[k],ghostX[j],ghostY[j])<3000 && !(busterState[k]==2 && busterValue[k]>=5))
                                {
                                    inVicinity++;
                                }
                            }
                        }
                        //cerr << needed << " " << canReach << " " << inVicinity << " " << myAttackers[ghostId[j]] << " " << ghostAttackers[j] << " " << turns-releaseTurn << " " << newRelease << endl;
                        if(score*2+1==ghostCount || (needed && canReach && (canAdd || ((inVicinity+myAttackers[ghostId[j]])>=(ghostAttackers[j]-myAttackers[ghostId[j]]) || (turns-releaseTurn<=3 && ghostId[j]==newRelease) || ghostState[j]==0)) && (py(baseCoordX,baseCoordY,ghostX[j],ghostY[j])>1600 || turns>160)))
                        {
                            lowestValue=distance+ghostState[j]*1000-myAttackers[ghostId[j]]*10000+basePriority;
                            lowestDist=distance;
                            lowestX=ghostX[j];
                            lowestY=ghostY[j];
                            lowestId=ghostId[j];
                            lowestState=ghostState[j];
                        }
                    }
                }
                if(lowestValue!=100000)
                {
                    if(lowestDist>1760)
                    {
                        //Too far
                        stringstream ss;
                        ss<<"MOVE "<<lowestX<<" "<<lowestY<<" TOO FAR";
                        busterCommand[i]=ss.str();
                        noCommand=false;
                    }
                    else if(lowestDist<900)
                    {
                        //Too close
                        stringstream ss;
                        int moveX,moveY;
                        if(lowestDist!=0)
                        {
                            cerr << x << " a " << y << " l " << lowestDist << " lowest" << lowestX << " a " << lowestY << endl;
                            moveX = x - ((901-lowestDist)*(lowestX-x)/lowestDist);
                            moveY = y - ((901-lowestDist)*(lowestY-y)/lowestDist);
                        }
                        else
                        {
                            moveX = otherBaseX;
                            moveY = otherBaseY;
                        }
                        ss<<"MOVE "<< moveX <<" "<< moveY <<" TOO CLOSE";
                        busterCommand[i]=ss.str();
                        noCommand=false;
                    }
                    else
                    {
                        //Exterminate
                        stringstream ss;
                        ss<<"BUST "<<lowestId<<" EXTERMINATE";
                        busterCommand[i]=ss.str();
                        noCommand=false;
                    }
                }
            }
            
            //Inspect previously known ghost positions if any remain
            if(noCommand)
            {
                int lowestX,lowestY,lowestDist;
                int lowestValue=100000;
                for(int j=0;j<ghostCount;j++)
                {
                    int basePriority = py(prevX[j],prevY[j],baseCoordX,baseCoordY)/4;
                    int distance = py(x,y,prevX[j],prevY[j]);
                    if(distance<800)
                    {
                        prevFree[j]=false;
                    }
                    if(/*!seeGhost[j] && */(distance+prevHealth[j]*100+basePriority)<lowestValue && (prevHealth[j]<40 || turns>80) && (prevHealth[j]<15 || turns>10) && prevFree[j] && (py(baseCoordX,baseCoordY,prevX[j],prevY[j])>1600 || turns>160))
                    {
                        lowestValue=distance+prevHealth[j]*500+basePriority;
                        lowestDist=distance;
                        lowestX=prevX[j];
                        lowestY=prevY[j];
                    }
                }
                if(lowestValue!=100000/* && lowestDist<5000*/)
                {
                    //Inspect
                    stringstream ss;
                    ss << "MOVE " << lowestX << " " << lowestY << " INSPECT";
                    busterCommand[i]=ss.str();
                    noCommand=false;
                }
                
            }
            
            //Map new positions, disabled as it ended up reducing score
            /*if(noCommand)
            {
                for(int j=0;j<17;j++)
                {
                    for(int k=0;k<10;k++)
                    {
                        //cerr << py(otherCoreX,otherCoreY,j*1000,k*1000) << " a " << map[j][k] << " b " << py(x,y,j*1000,k*1000) << endl;
                        if(py(otherCoreX,otherCoreY,j*1000,k*1000)>12000 && map[j][k]>200 && py(x,y,j*1000,k*1000)<3000 && turns<140)
                        {
                            //Check unseen positions
                            stringstream ss;
                            ss << "MOVE " << j*1000 << " " << k*1000 << " LEARN";
                            busterCommand[i]=ss.str();
                            noCommand=false;
                        }
                    }
                }
            }*/
            
            //Scout if no command
            if(noCommand)
            {
                if(busterY[i]==scoutY[i%2])
                {
                    if(busterX[i]==scoutX[i%2])
                    {
                        int temp = scoutX[i%2];
                        scoutX[i%2] = otherScoutX[i%2];
                        otherScoutX[i%2] = temp;
                    }
                    //Scout
                    stringstream ss;
                    ss<<"MOVE "<<scoutX[i%2]<<" "<<scoutY[i%2]<<" SCOUTING";
                    busterCommand[i]=ss.str();
                }
                else
                {
                    //Prepare scout
                    stringstream ss;
                    ss<<"MOVE "<<busterX[i]<<" "<<scoutY[i%2]<<" PREPARE SCOUT";
                    busterCommand[i]=ss.str();
                }
            }
            
            //Check if chasing anything and reset
            if(!isChasing[i])
            {
                chasingFor[i]=0;
            }
            
            cout << busterCommand[i] << endl;
        }
    }
}