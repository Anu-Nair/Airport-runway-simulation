#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<math.h>
#include<time.h>
#include<stdbool.h>
#define MAXQUEUE 3

typedef enum{ARRIVE,DEPART}Action;
typedef enum{n,N,w,W,s,S,e,E}direct;
typedef struct plane{
int id;
int tm;
double fuel;
char direction;
}Plane;
typedef Plane QueueEntry;
typedef struct queue{
int count;
int front;
int rear;
QueueEntry entry[MAXQUEUE];}Queue;

void Initialize(Queue *p);
int get_runway(Queue *pl,Queue *pl2,Queue *pt,Queue *pt2);
void Start (int *endtime, double *expectarrive, double *expectdepart);
void AddQueue(Plane p,Queue *q);
Plane DeleteQueue(Queue *q);
int Full(Queue *p);
int Empty(Queue *p);
int Enquire();
void NewPlane(Plane *p ,int *nplanes, int curtime, Action kind);
void Refuse(Plane p, int *nrefuse, Action kind);
void Land( Plane p, int curtime, int *nland, int *landwait);
void Fly(Plane p,int curtime, int *ntakeoff,int *takeoffwait);
void Idle(int curtime, int *idletime);
void Conclude(int nplanes, int nland, int ntakeoff, int nrefuse, int landwait, int takeoffwait, int
              idletime, int endtime, Queue *pt, Queue *pl,Queue *pl2,Queue *pt2);
void Randomize(void);
int RandomNumber(double expectedvalue);
char* get_dir(int d);
int weather();
int main()
{
    Queue landing, takeoff;
    Queue *pl = &landing;
    Queue *pt = &takeoff;
    Queue landing2, takeoff2;
    Queue *pl2 = &landing2;
    Queue *pt2 = &takeoff2;
    Queue *choice;
    Plane plane,plane1;

    int curtime;
    int endtime;
    double expectarrive;
    double expectdepart;
    int i;
    int idletime;
    int landwait;
    int nland;
    int nplanes;
    int nrefuse;
    int ntakeoff;
    int pri;
    int takeoffwait;

    Initialize(pl);
    Initialize(pt);
    Initialize(pl2);
    Initialize(pt2);


    nplanes = 0;
    nland = 0;
    ntakeoff = 0;
    nrefuse = 0;
    landwait = 0;
    takeoffwait = 0;
    idletime = 0;
    Start(&endtime, &expectarrive, &expectdepart);
    for(curtime = 1 ; curtime <= endtime ; curtime++)
    {
        printf("\n\n");
        printf("                                                             CURRENT TIME : %d\n",curtime);
        if(weather()<20)
        {
            printf("                                           THUNDERSTORM WARNING----------RUNWAYS ON STAY!(no land/take-off)\n");
            Idle(curtime, &idletime);
        }
        else{
        pri = RandomNumber(expectarrive);
        for(i =1 ; i<=pri ; i++)
        {
            NewPlane(&plane, &nplanes, curtime, ARRIVE );
            if(plane.direction==(0)||plane.direction==(1)||plane.direction==(2)||plane.direction==(3))
               choice=pl;
            else
                choice=pl2;
            if (Full(choice))
                Refuse(plane, &nrefuse, ARRIVE);
            else
            {
                if(plane.fuel<10)
                {
                    printf("                                           Emergency landing!-----FUEL LOW!!\n");
                    Land(plane, curtime, &nland, &landwait);
                }
                else
                AddQueue(plane, choice);
            }
        }
        pri = RandomNumber(expectdepart);
        for(i = 1 ; i<=pri; i++)
        {
            NewPlane( &plane, &nplanes, curtime, DEPART);
            if(plane.direction==(0)||plane.direction==(1)||plane.direction==(2)||plane.direction==(3))
               choice=pt;
            else
                choice=pt2;
            if( Full(choice))
                Refuse(plane, &nrefuse, DEPART);
            else
            {
                AddQueue(plane, choice);
            }
        }
        if((! Empty(pl))&&(!Empty(pl2)))
        {
            printf("                      Runway 1:(Land    )");
            plane1=DeleteQueue( pl);
            Land(plane1, curtime, &nland, &landwait);
            printf("                      Runway 2:(Land    )");
            plane1=DeleteQueue( pl2);
            Land(plane1, curtime, &nland, &landwait);
        }
        else if((!Empty(pl))&&!(Empty(pt2)))
        {
            printf("                      Runway 1:(Land    )");
            plane1=DeleteQueue( pl);
            Land(plane1, curtime, &nland, &landwait);
            printf("                      Runway 2:(Take-off)");
            plane1=DeleteQueue( pt2);
            Fly(plane1, curtime, &ntakeoff, &takeoffwait);
        }
        else if((!Empty(pt))&&(!Empty(pl2)))
        {
            printf("                      Runway 2:(Land    )");
            plane1=DeleteQueue( pl2);
            Land(plane1, curtime, &nland, &landwait);
            printf("                      Runway 1:(Take-off)");
            plane1=DeleteQueue( pt);
            Fly(plane1, curtime, &ntakeoff, &takeoffwait);
        }
        else if((! Empty (pt))&&(!Empty(pt2)))
        {
            printf("                      Runway 1:(Take-off)");
            plane1=DeleteQueue( pt);
            Fly(plane1, curtime, &ntakeoff, &takeoffwait);
            printf("                      Runway 2:(Take-off)");
            plane1=DeleteQueue( pt2);
            Fly(plane1, curtime, &ntakeoff, &takeoffwait);
        }
        else if((!Empty(pl))||(!Empty(pl2))||(!Empty(pt))||(!Empty(pt2)))
        {
            int not_empty=get_runway(pl,pl2,pt,pt2);
            switch(not_empty)
            {
            case 1:
                printf("                      Runway 1:(Land    )");
                plane1=DeleteQueue( pl);
                Land(plane1, curtime, &nland, &landwait);
                break;
            case 2:
                printf("                      Runway 2:(Land    )");
                plane1=DeleteQueue( pl2);
                Land(plane1, curtime, &nland, &landwait);
                break;
            case 3:
                printf("                      Runway 1:(Take-off)");
                plane1=DeleteQueue( pt);
                Fly(plane1, curtime, &ntakeoff, &takeoffwait);
                break;
            case 4:
                printf("                      Runway 2:(Take-off)");
                plane1=DeleteQueue( pt2);
                Fly(plane1, curtime, &ntakeoff, &takeoffwait);
                break;
            }
        }
        else
            Idle(curtime, &idletime);

    }
    }
    Conclude(nplanes, nland, ntakeoff, nrefuse, landwait, takeoffwait, idletime, endtime, pt,
             pl,pl2,pt2);
    return 0;
}
int get_runway(Queue *pl,Queue *pl2,Queue *pt,Queue *pt2)
{
    if(!Empty(pl))
        return 1;
    else if(!Empty(pl2))
        return 2;
    else if(!Empty(pt))
        return 3;
    else if(!Empty(pt2))
        return 4;
}
int weather()
{
    return rand()%100;
}
void Start (int *endtime, double *expectarrive, double *expectdepart)
{
    bool ok=false;
    printf("          ========================================================================================================================================\n");
    printf("                              ______\n");
    printf("                               _\\ _~-\\___\n");
    printf("                        =  = ==(____AA____D\n");
    printf("                                    \\_____\\___________________,-~~~~~~~`-.._\n");
    printf("                                      /     o O o o o o O O o o o o o o O o  |\\_\n");
    printf("                                      `~-.__        ___..----..                  )\n");
    printf("                                          `---~~\\___________/------------`````\n");
    printf("                                           =  ===(_________D\n");
    printf("          ========================================================================================================================================\n");
    printf("                                   S I M U L A T I O N   -     A I R P O R T    R  U N W A Y    C O N T R O L    \n");
    printf("          ----------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("                                           This program simulates an airport with TWO runways. \n");
    printf("                                           Depending on direction of plane the appropriate runway is chosen.\n");
    printf("                                           One plane can land or depart in each unit of time in each runway\n");
    printf("                                           Up to %d planes can ", MAXQUEUE);
    printf(" be waiting to land or takeoff at any time.\n");
    printf("                                           Bad weather--Runway stalled. NO plane can take-off/land.\n");
    printf("                                           Low fuel--Emergency landing permitted.\n");
    printf("                                           How many units of time will the simulation run ? ");
    scanf( "                                           %d", endtime);
    Randomize();
    do
    {
        printf("                                           Expected number of arrivals per unit of time(Probability)? (Enter a real number<=1)");
        scanf(" %lf", expectarrive);
        printf("                                           Expected number of departures per unit time(Probability)?");
        scanf(" %lf", expectdepart);
        if(*expectarrive <0.0 || *expectdepart <0.0)
        {
            printf("                                           These numbers must not be negative. \n");
            ok = false;
        }
        else if(*expectarrive + *expectdepart > 1.0)
        {
            printf("                                           The airport will become saturated. Read new numbers? (Enter 1 or 0)\n");
            int n = !Enquire();
            if(n==1)
            {
                ok=true;
                printf("                                           S I M U L A T I O N    S T O P P E D   B Y     U S E R\n");
            }
            else
            {
                ok=false;
            }
        }
        else
            ok = true;
    }
    while(ok == false);

}
void Initialize(Queue *p)
{
    p->front=-1;
    p->rear=-1;
    p->count=0;
}
int Full(Queue *p)
{
    if( (p->front == p->rear + 1) || (p->front == 0 && p->rear == MAXQUEUE-1)) return 1;
    return 0;
}

int Empty(Queue *p)
{
    if(p->front == -1) return 1;
    return 0;
}

void AddQueue(Plane p,Queue *q)
{
    q->count++;
        if(q->front == -1) q->front = 0;
        q->rear = (q->rear + 1) % MAXQUEUE;
        q->entry[q->rear] = p;
}


Plane DeleteQueue(Queue *q)
{
    q->count--;
        Plane p = q->entry[q->front];
        if (q->front == q->rear){
            q->front = -1;
            q->rear = -1;
        }
        else
            q->front = (q->front + 1) % MAXQUEUE;
        return p;
}
void Randomize(){
srand((unsigned int) (time(NULL) % 10000));
}


int Enquire()
{
    int user_ans;
    scanf("\n                                           %d",&user_ans);
    return user_ans;
}
void NewPlane(Plane *p ,int *nplanes, int curtime, Action kind)
{
    direct d=rand()%8;
    (*nplanes)++;
    p ->
    id = *nplanes;
    p ->
    tm = curtime;
    p->fuel=rand()%30;
    p->direction=d;
    switch(kind)
    {
    case ARRIVE :
        printf("                                           Plane %3d             %s          ready to land. \n", *nplanes,get_dir(d));
        break;
    case DEPART :
        printf("                                           Plane %3d             %s          ready to take off.\n" , *nplanes,get_dir(d));
        break;
    }
}
char* get_dir(int d)
{
    switch(d)
    {
    case 0:
    case 1:
        return "NORTH";
        break;
    case 2:
    case 3:
        return "WEST ";
        break;
    case 4:
    case 5:
        return "SOUTH";
        break;
    case 6:
    case 7:
        return "EAST ";
        break;

    }
}
void Refuse(Plane p, int *nrefuse, Action kind)
{
    switch(kind)
    {
    case ARRIVE :
        printf("                                           Plane %3d                            directed to another airport.\n", p.id);
        break;
    case DEPART :
        printf("                                           Plane %3d                            told to try later.\n", p.id);
        break;
    }
    (*nrefuse) ++;
}

void Land( Plane p, int curtime, int *nland, int *landwait)
{
    int wait;

    wait = curtime - p.tm;
    printf("                                         %3d : Plane %3d landed---------in queue %d units .\n", curtime, p.id, wait);
    (*nland)++;
    *landwait += wait;
}

void Fly(Plane p,int curtime, int *ntakeoff,int *takeoffwait)
{
    int wait;
    wait = curtime - p.tm;
    printf("                                         %3d : Plane %3d took off-------in queue %d units. \n",curtime, p.id, wait);
    (*ntakeoff)++;
    *takeoffwait += wait;
}

void Idle(int curtime, int *idletime)
{
    printf("                                         %3d : Runway is idle.\n", curtime);
    (*idletime)++;
}

void Conclude(int nplanes, int nland, int ntakeoff, int nrefuse, int landwait, int takeoffwait, int
              idletime, int endtime, Queue *pt, Queue *pl,Queue *pl2,Queue *pt2)
{
    printf("\n          ========================================================================================================================================\n");
    printf("                                           Simulation has concluded after %d units. \n", endtime);
    printf("          ----------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("                                           Total number of planes processed:   %3d \n", nplanes);
    printf("                                           Number of planes landed:            %3d\n", nland);
    printf("                                           Number of planes takenoff:          %3d\n", ntakeoff);

    printf("                                           Number of planes refused use:       %3d\n", nrefuse);
    printf("                                           Number left ready to land:          %3d\n", pl->count+pl2->count);
    printf("                                           Number left ready to take off:      %3d\n", pt->count+pt2->count);
    if(endtime > 0)
        printf("                                           Percentage of time runway idle :        %6.2f\n", ((double) idletime/endtime) *100.0);
    if(nland > 0)
        printf("                                           Average wait time to land :             %6.2f\n",(double)landwait/nland);
    if(ntakeoff > 0)
        printf("                                           Average wait time to take off:          %6.2f\n", (double) takeoffwait/ntakeoff);

}


int RandomNumber(double expectedvalue)
{
    int n =0;
    double em;
    double x;
    em = exp(-expectedvalue);
    x = rand() / ((double)INT_MAX);
    if(expectedvalue*rand()<20)
    {
          return 0;

    }

    for(;(x<em);x=x*rand()/expectedvalue)
    {
        n=n+1;
    }

    return n;
}
