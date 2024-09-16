#include <simplecpp>
#include "shooter.h"
#include "bubble.h"

/* Simulation Vars */
const double STEP_TIME = 0.02;

/* Game Vars */
const int PLAY_Y_HEIGHT = 450;
const int LEFT_MARGIN = 70;
const int TOP_MARGIN = 20;
const int BOTTOM_MARGIN = (PLAY_Y_HEIGHT+TOP_MARGIN);


void move_bullets(vector<Bullet> &bullets){
    // move all bullets
    for(unsigned int i=0; i<bullets.size(); i++){
        if(!bullets[i].nextStep(STEP_TIME)){
            bullets.erase(bullets.begin()+i);
        }
    }
}

void move_bubbles(vector<Bubble> &bubbles){
    // move all bubbles
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        bubbles[i].nextStep(STEP_TIME);
    }
}

vector<Bubble> create_bubbles()
{
    // create initial bubbles in the game
    vector<Bubble> bubbles;
    bubbles.push_back(Bubble(WINDOW_X/4.0, BUBBLE_START_Y, 2*BUBBLE_DEFAULT_RADIUS, -BUBBLE_DEFAULT_VX, 0, COLOR(255,105,180)));
    bubbles.push_back(Bubble(WINDOW_X/10.0, BUBBLE_START_Y, 4*BUBBLE_DEFAULT_RADIUS, BUBBLE_DEFAULT_VX, 0, COLOR(255,105,180)));
    bubbles.push_back(Bubble(9*WINDOW_X/10.0, BUBBLE_START_Y, 4*BUBBLE_DEFAULT_RADIUS, -BUBBLE_DEFAULT_VX, 0, COLOR(255,105,180)));
    bubbles.push_back(Bubble(3*WINDOW_X/4.0, BUBBLE_START_Y, 2*BUBBLE_DEFAULT_RADIUS, BUBBLE_DEFAULT_VX, 0, COLOR(255,105,180)));
    return bubbles;
}

//check if any bullet hits any bubble
void Hit(vector<Bubble> &bubbles, vector<Bullet> &bullets,int &score){
    //iterate through all the bubbles and bullets
    for(int i=0;i<bubbles.size();i++){
        for(int j=0;j<bullets.size();j++){
            if((abs(bubbles[i].get_center_x()-bullets[j].get_center_x())<bubbles[i].get_radius()+bullets[j].get_width()/2)&&(abs(bubbles[i].get_center_y()-bullets[j].get_center_y())<bubbles[i].get_radius()+bullets[j].get_height()/2)){
                //if bubbles are of the smallest size then do nothing
                if(bubbles[i].get_radius()==BUBBLE_DEFAULT_RADIUS)
                {bullets.erase(bullets.begin()+j);   //destroys the bullet
                bubbles.erase(bubbles.begin()+i);//destroys the bubble
                score++;}
                //else split it into two bullets
                else{
                bubbles.push_back(Bubble(bubbles[i].get_center_x(),bubbles[i].get_center_y(),bubbles[i].get_radius()/2 , BUBBLE_DEFAULT_VX, 0, COLOR(255,105,180)));
                bubbles.push_back(Bubble(bubbles[i].get_center_x(),bubbles[i].get_center_y(),bubbles[i].get_radius()/2, -BUBBLE_DEFAULT_VX, 0, COLOR(255,105,180)));
                bullets.erase(bullets.begin()+j);   //destroys the bullet
                bubbles.erase(bubbles.begin()+i);//destroys the bubble
                score++;
                }

            }
        }
    }
    return;
}

//check if the bullet hits the user
void bubble_hits_shooter(vector<Bubble> &bubbles, Shooter shooter,int &health,bool &hit){
    //decrease health only if it hasnt decreased in the previous iteration
    for(int i=0;i<bubbles.size();i++){
        int distance_between_head_and_bubble=sqrt((shooter.get_head_center_x()-bubbles[i].get_center_x())*(shooter.get_head_center_x()-bubbles[i].get_center_x()) + (shooter.get_head_center_y()-bubbles[i].get_center_y())*(shooter.get_head_center_y()-bubbles[i].get_center_y()));
        if((distance_between_head_and_bubble<shooter.get_head_radius()+bubbles[i].get_radius())||((abs(bubbles[i].get_center_x()-shooter.get_body_center_x())<bubbles[i].get_radius()+shooter.get_body_width()/2)&&((abs(bubbles[i].get_center_y()-shooter.get_body_center_y())<bubbles[i].get_radius()+shooter.get_body_height()/2)))){
           if(!hit){
           health--;
           hit=true;
           }
           return;}
    }
    //if bubble leaves the shooter hit is set back to false
    hit=false;
    return;
}

int main()
{
    initCanvas("Bubble Trouble", WINDOW_X, WINDOW_Y);

    Line b1(0, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
    b1.setColor(COLOR(0, 0, 255));

    string msg_cmd("Cmd: _");
    Text charPressed(LEFT_MARGIN, BOTTOM_MARGIN, msg_cmd);

    string msg_result;
    Text game_result{WINDOW_X/2,WINDOW_Y/2, msg_result};

    //displaying the timer
    float time=50.00;
    string msg_timer("TIME: __");
    msg_timer[msg_timer.length()-1] =time;
    Text timer{30,25,msg_timer};
    timer.setMessage(msg_timer);

    //displaying the score
    int score=0;
    string msg_score("Score: __");
    msg_score[msg_score.length()-1] =48+score%10;
    msg_score[msg_score.length()-2] =48+score/10;
    Text game_score(430,470,msg_score);
    game_score.setMessage(msg_score);

    //displaying the health
    int health=3;
    string msg_health("Health: _");
    msg_health[msg_health.length()-1] =health;
    Text Health(430,25,msg_health);
    Health.setMessage(msg_health);

    // Intialize the shooter
    Shooter shooter(SHOOTER_START_X, SHOOTER_START_Y, SHOOTER_VX);

    // Initialize the bubbles
    vector<Bubble> bubbles = create_bubbles();

    // Initialize the bullets (empty)
    vector<Bullet> bullets;

    XEvent event;

    //initializing hit to false
    bool hit=false;
    //initializing game_progress to true
    bool game_progress=true;

    // Main game loop
    while (true)
    {   //if game ends then the code will not get executed
        if(game_progress){
        bool pendingEvent = checkEvent(event);
        if (pendingEvent)
        {
            // Get the key pressed
            char c = charFromEvent(event);
            msg_cmd[msg_cmd.length() - 1] = c;
            charPressed.setMessage(msg_cmd);

            // Update the shooter
            if(c == 'a')
                shooter.move(STEP_TIME, true);
            else if(c == 'd')
                shooter.move(STEP_TIME, false);
            else if(c == 'w')
                bullets.push_back(shooter.shoot());
            else if(c == 'q')
                return 0;
        }

        // Update the bubbles
        move_bubbles(bubbles);

        // Update the bullets
        move_bullets(bullets);

        //decrementing the timer in every iteration and updating the text
        time =time-0.04;
        msg_timer[msg_timer.length()-1] =48+(int)time%10;
        msg_timer[msg_timer.length()-2] =48+(int)time/10;
        timer.setMessage(msg_timer);

        //checking if the bullets hit the bubbles and updating the text
        Hit(bubbles ,bullets,score);
        msg_score[msg_score.length()-1] =48+score%10;
        msg_score[msg_score.length()-2] =48+score/10;
        game_score.setMessage(msg_score);

        //checking if the bubbles hit the shooter and updating the text
        bubble_hits_shooter(bubbles,shooter,health, hit);
        msg_health[msg_health.length()-1] =48+health;
        Health.setMessage(msg_health);

        if(health==0){game_result.setMessage("You Lose");game_progress=false;}//if health reaches 0, the game terminates and the user looses
        if(bubbles.size()==0){game_result.setMessage("Congractulations");game_progress=false;}//if bubble countreaches 0, the game terminates and the user wins
        if(time<=0){game_result.setMessage("Time's up");game_progress=false;}//if timer reaches 0, the game terminates and the user looses
        wait(STEP_TIME);
        }


    }

}
