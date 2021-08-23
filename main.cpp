#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <bits/stdc++.h>
using namespace std;
sf::RenderWindow window(sf::VideoMode(1094, 820), "Shopping Street");

string path = "C:/Users/danie/shopping street/pictures/";

struct Building{
    sf::Sprite sprite; // building or button sprite
    int status; // -1 is not bought, 0-2 are the levels of bought, 3 is checked button, 4 is cancel button
    int people; // number of people inside
    int location; // values are 0-7.
    int index; // values are 0-7. 0 for boutique, 1 for pet, 2 for toy, etc.
};
struct Customer{
    sf::Sprite sprite; // customer sprite
    float y; // customer's current y-value
    int internal_y; // internal customer's y-value
    float x; // customer's x-value: determines what shops a customer sees
    int animation_phase; // current phase in animation
    int animation_time; // when this reaches 0, next texture in animation
    int customer_texture; // determines what the customer looks like
    // bool visited[8];
    vector<bool> visited; // which LOCATIONS a customer has visited
    // bool visited_vendors[3];
    vector<bool> visited_vendors; // bench, music, newspaper
    int status; // 0 is walking to the right
                // 1 is walking to a building
                // 2 is walking to a vendor
                // 3 is in a building
                // 4 is on a bench
                // 5 is at music
                // 6 is at newspapers
                // 7 is leaving a building
                // 8 is leaving a vendor
    int shopping_time; // when this reaches 0, customer leaves building
    int vendor_spot; // 0-4, is the seat of the bench that customer is at
    int location; //  (x+6)/137
    int done_shopping; // 0 is still shopping, 1 is happy, 2 is left the street

    pair<float,float> next_frame; // first is x, second is y

    // Day n is 18 + 2*n customers
    // Bus stop is 10 customers

};
// returns the income of a building.
// a is the building that someone leaves.
int getIncome(Building a){
    int Prices[8][3]={{50,75,100},{75,112,149},{40,60,80},{70,105,140},{25,37,49},{80,120,160},{100,150,200},{150,225,300}};
    if(a.status >= 3) a.status = a.status/3-1;
    if (a.status < 0 || a.status > 2){
        cout << ("---ERROR---ERROR---ERROR--- When calling getIncome, a.status was not between 0 and 2. ---ERROR---ERROR---ERROR---");
        return 0;
    }
    return Prices[a.index][a.status];
}
// a is the button
// returns price of button a
int getPrice(Building a){
    int Prices[8]={2500,3000,7000,15000,20000,25000,35000,55000};
    if(a.status >= 3) a.status = a.status/3-1;
    if (a.status < 0 || a.status > 2){
        cout << ("---ERROR---ERROR---ERROR--- When calling getPrice, a.status was not between 0 and 2. ---ERROR---ERROR---ERROR---");
        return 0;
    }
    return Prices[a.index] * (a.status + 1); // a.status should be 0-2; status of the button.
}
// a is a button or building
// returns capacity of it
int getCapacity(Building a){
    int Capacity[8]={4,5,6,8,10,9,8,20};
    if(a.status >= 3) a.status = a.status/3-1;
    if (a.status < 0 || a.status > 2){
        cout << ("---ERROR---ERROR---ERROR--- When calling getCapacity, a.status was not between 0 and 2. ---ERROR---ERROR---ERROR---");
        return 0;
    }
    return Capacity[a.index] * (a.status + 1);
}
bool isMouse(sf::Sprite button, sf::Sprite cursor, bool &moused){
    if(moused) return false;
    sf::FloatRect zoo(0,0,0,0);
    zoo = button.getGlobalBounds(); // (zoo.left, zoo.top) is the location of button. zoo.width X zoo.height are dimensions. (NOTE: float values)
    sf::Vector2f cursor_pos = cursor.getPosition();
    moused = zoo.contains(cursor_pos);
    return moused;
    // return cursor_pos.x>=zoo.left&&cursor_pos.y>=zoo.top&&cursor_pos.x<=(zoo.left+zoo.width)&&cursor_pos.y<=(zoo.top+zoo.height);
    // cout << zoo.left << " " << zoo.top << " " << zoo.width << " " << zoo.height << endl;
}
// Debug Function to draw a Sprite.
void Draw(sf::Sprite &thing){
    float x, y;
    sf::Vector2i localPosition = sf::Mouse::getPosition(window);
    x = localPosition.x;
    y = localPosition.y;
    thing.setPosition(x,y);
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) thing.scale(1.001f, 1.001f);
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) thing.scale(0.999f, 0.999f);
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::E)) cout << "x: " << x << " y: " << y << "scale: " << thing.getScale().x << endl;
    window.draw(thing);
}
// Debug Function to draw Text.
// LeftClick is true if left is clicked this frame and not previous frame.
void DrawText(sf::Text &thing, bool LeftClick, bool RightClick, bool UpArrow, bool DownArrow){
    // if(thing.status == -1) return;
    float x, y;
    sf::Vector2i localPosition = sf::Mouse::getPosition(window);
    x = localPosition.x;
    y = localPosition.y;
    if(UpArrow) thing.setCharacterSize(thing.getCharacterSize() + 1);
    else if(DownArrow) thing.setCharacterSize(thing.getCharacterSize() - 1);
    // if(LeftClick) thing.setOutlineThickness(thing.getOutlineThickness() - 1);
    // else if(RightClick) thing.setOutlineThickness(thing.getOutlineThickness() + 1);
    thing.setPosition(x,y);
    cout << "x: " << x << " y: " << y << "Character size: " << thing.getCharacterSize() << " Outline Thickness: " << thing.getOutlineThickness() << endl;
    window.draw(thing);
}
void DrawRect(sf::RectangleShape &rect, bool LeftClick, bool RightClick){
    float x, y;
    sf::Vector2i localPosition = sf::Mouse::getPosition(window);
    x = localPosition.x;
    y = localPosition.y;
    rect.setPosition(x,y);
    sf::Vector2f Size;
    Size = rect.getSize();
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) Size.y += 0.2;
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) Size.y -= 0.2;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) Size.x += 0.2;
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) Size.x -= 0.2;
    if(LeftClick) rect.setOutlineThickness(rect.getOutlineThickness() + 0.1);
    else if(RightClick) rect.setOutlineThickness(rect.getOutlineThickness() - 0.1);
    rect.setSize(Size);
    cout << "Location: (x= " << x << ", y= " << y << ") Size: x= " << Size.x << " y= " << Size.y << " Outline: " << rect.getOutlineThickness() << endl;
    window.draw(rect);
}
void DrawCircle(sf::CircleShape &circ, bool LeftClick, bool RightClick){
    float x, y;
    sf::Vector2i localPosition = sf::Mouse::getPosition(window);
    x = localPosition.x;
    y = localPosition.y;
    circ.setPosition(x,y);
    sf::Vector2f Scale;
    Scale = circ.getScale();
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) Scale.y += 0.01;
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) Scale.y -= 0.01;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) Scale.x += 0.01;
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) Scale.x -= 0.01;
    if(LeftClick) circ.setOutlineThickness(circ.getOutlineThickness() + 0.01);
    else if(RightClick) circ.setOutlineThickness(circ.getOutlineThickness() - 0.01);
    circ.setScale(Scale);
    cout << "Location: (x= " << x << ", y= " << y << ") Scale: x=" << Scale.x << " y= " << Scale.y << " Outline: " << circ.getOutlineThickness() << endl;
    window.draw(circ);
}
int main(){
    int fps_initial = 60, fps = 60;
    float speed = 10.0f;
    srand(time(NULL));

    sf::Sprite cursor;
    cursor.setScale(0.3f,0.3f);
    cursor.setOrigin(18.0f,0.0f);

    sf::Texture cursor_texture1, cursor_texture2;
    cursor_texture1.loadFromFile(path + "cursor1.png");
    cursor_texture2.loadFromFile(path + "cursor2.png");
    sf::IntRect cursor_rect1(0,0,88,108);
    sf::IntRect cursor_rect2(0,0,98,104);
    cursor.setTexture(cursor_texture1);
    cursor.setTextureRect(cursor_rect1);

    string building_names[] = {"boutique","pet","toy","jewelry","restaurant","electro","furniture","market"};
    string building_names_full[] = {"Boutique", "Pet Shop", "Toy Store", "Jewelry", "Food", "Electronics", "Furniture Store", "Super Market"};
    sf::Texture buildings_texture[8][3], graybuildings_texture[8][3], buttons_texture[8][5], graybuttons_texture[8][3];
    for(int i=0;i<8;i++){
        for(int j=0;j<3;j++){
            buildings_texture[i][j].loadFromFile(path+building_names[i]+"/"+building_names[i]+" "+to_string(j+1)+".png");
            graybuildings_texture[i][j].loadFromFile(path+building_names[i]+"/"+building_names[i]+" "+to_string(j+1)+" gray.png");
            buttons_texture[i][j].loadFromFile(path+building_names[i]+"/"+building_names[i]+" "+to_string(j+1)+" button.PNG");
            graybuttons_texture[i][j].loadFromFile(path+building_names[i]+"/"+building_names[i]+" "+to_string(j+1)+" button gray.png");
        }
        buttons_texture[i][4].loadFromFile(path+building_names[i]+"/"+building_names[i]+" check.PNG");
        buttons_texture[i][3].loadFromFile(path+building_names[i]+"/"+building_names[i]+" cancel.PNG");
    }
    sf::FloatRect texture_rect_bounds[8][3]; // used to draw buildings
    sf::IntRect texture_rect[8][3];
    Building buildings[8][2];
    vector<int> building_locations(8,-1);
    float building_time[] = {5.00, 6.25, 8.3333, 25.00, 50.00, 12.50, 16.6666, 33.3333}; // time in seconds for the shopping time at each building in slow mode
    float vendor_time[] = {5.00, 10.00, 17.50};
    // time = {100/20, 100/16, 100/12, 100/4, 100/2, 100/8, 100/6, 100/3}
    float building_scales[8]={0.1027f, 0.0905f, 0.098f, 0.0897f, 0.1027f, 0.083f, 0.0895f, 0.1035f};
    for(int i=0;i<8;i++){
        buildings[i][0].index = i;
        buildings[i][1].index = i;
        buildings[i][0].status = -1; // building is not bought; should be -1
        buildings[i][1].status = 0; // button is to buy level 1 building
        buildings[i][0].people = 0;
        buildings[i][1].people = -1; // button has no people
        buildings[i][0].location = -1;
        buildings[i][1].location = i; // button is at location i
        /*if(getPrice(buildings[i][1]) <= 5050){
            buildings[i][1].sprite.setTexture(buttons_texture[i][0]);
        }
        else{
            buildings[i][1].sprite.setTexture(graybuttons_texture[i][0]);
        }*/
        buildings[i][0].sprite.setTexture(buildings_texture[i][0]);
        buildings[i][1].sprite.setTexture(buttons_texture[i][0]);
        if(getPrice(buildings[i][1]) > 5050) buildings[i][1].sprite.setTexture(graybuttons_texture[i][0]);
        buildings[i][0].sprite.setScale(building_scales[i],building_scales[i]);
        buildings[i][1].sprite.setScale(0.104f,0.104f);
        int building_x = buildings[i][0].location*137;
        sf::FloatRect *zoo = nullptr;
        zoo = new sf::FloatRect(0,0,0,0);
        *zoo = buildings[i][0].sprite.getGlobalBounds();
        buildings[i][0].sprite.setPosition(building_x - (zoo->width/2) + 64, 418 - zoo->height);
        delete zoo;
        // moves the building from top left corner to mid-left edge
        buildings[i][1].sprite.setPosition(110*buildings[i][1].location,691);
        for(int j=2; j>=0; j--){
            sf::Sprite *temp_building = nullptr;
            temp_building = new sf::Sprite;
            temp_building->setTexture(buildings_texture[i][j]);
            texture_rect_bounds[i][j] = temp_building->getLocalBounds();
            delete temp_building;
            texture_rect[i][j].left = 0;
            texture_rect[i][j].top = 0;
            texture_rect[i][j].width = (int)texture_rect_bounds[i][j].width;
            texture_rect[i][j].height = (int)texture_rect_bounds[i][j].height;
            //cout << "Dimensions of " << building_names_full[i] << " " << j << ": " << texture_rect[i][j].width << " x " << texture_rect[i][j].height << endl;
        }
    }

    sf::Sprite gray_buildings[8];
    for(int i=0; i<8; i++){
        gray_buildings[i].setTexture(graybuildings_texture[0][0]);
        gray_buildings[i].setScale(building_scales[0],building_scales[0]);
    }
    int gray_buildings_status = -1; // index of building

    sf::Sprite build_men[8], orange_gradient[8];
    sf::Texture build_men_texture, orange_gradient_texture;
    sf::RectangleShape orange_gradient_rect[8];
    sf::Text orange_gradient_text[8];
    sf::Font font, roboto_light, roboto_regular, roboto_medium, roboto_bold, roboto_black;
    build_men_texture.loadFromFile(path+"build/build men.PNG");
    orange_gradient_texture.loadFromFile(path+"build/orange gradient.PNG");
    for(int i=0; i<8; i++){
        build_men[i].setTexture(build_men_texture);
        build_men[i].setPosition(137*i + 8,347);
        build_men[i].setScale(0.244f,0.244f);
        orange_gradient[i].setTexture(orange_gradient_texture);
        orange_gradient[i].setScale(0.244f,0.244f);
        orange_gradient[i].setPosition(137*i + 4,300);
        orange_gradient_rect[i].setPosition(137*i+4,300); // location 3 = 278,300
        orange_gradient_rect[i].setFillColor(sf::Color(0,0,0,0));
        orange_gradient_rect[i].setOutlineColor(sf::Color::Black);
        orange_gradient_rect[i].setOutlineThickness(2);
        orange_gradient_rect[i].setSize(sf::Vector2f(122, 22));
        orange_gradient_text[i].setCharacterSize(20);
        orange_gradient_text[i].setFont(roboto_regular);
        orange_gradient_text[i].setFillColor(sf::Color::Black);
        orange_gradient_text[i].setString("0%");
        orange_gradient_text[i].setPosition(137*i+53,299); // x = 48 for 10%
    }
    sf::RectangleShape capacity_rect[8];
    sf::Sprite red_gradient[8];
    sf::Text capacity_text[8];
    sf::Texture red_gradient_texture;
    red_gradient_texture.loadFromFile(path+"red gradient.PNG");
    for(int i=0; i<8; i++){
        red_gradient[i].setTexture(red_gradient_texture);
        red_gradient[i].setScale(0.244f,0.244f);
        red_gradient[i].setPosition(137*i+4,249);
        red_gradient[i].setTextureRect(sf::IntRect(0,0,0,100));
        capacity_rect[i].setPosition(137*i+4,249);
        capacity_rect[i].setFillColor(sf::Color(0,0,0,0));
        capacity_rect[i].setOutlineColor(sf::Color::Black);
        capacity_rect[i].setOutlineThickness(2);
        capacity_rect[i].setSize(sf::Vector2f(122,22));
        capacity_text[i].setCharacterSize(20);
        capacity_text[i].setFont(roboto_regular);
        capacity_text[i].setFillColor(sf::Color::Black);
        capacity_text[i].setString("0/5");
        capacity_text[i].setPosition(137*i+53,248);
    }

    sf::CircleShape build_shadow[8];
    for(int i=0; i<8; i++){
        build_shadow[i].setRadius(68);
        build_shadow[i].setOutlineThickness(0);
        build_shadow[i].setScale(1,0.42);
        build_shadow[i].setFillColor(sf::Color::Black);
        build_shadow[i].setOrigin(68,68);
        build_shadow[i].setPosition(137*i+64,395);
    }
    sf::Texture backdrop_texture[3];
    backdrop_texture[0].loadFromFile(path+"menu.png");
    backdrop_texture[1].loadFromFile(path+"new york.png");
    backdrop_texture[2].loadFromFile(path+"new york 2.png");
    sf::Sprite backdrop;
    backdrop.setPosition(0,0);

    int font_size = 32;
    font.loadFromFile(path+"fonts/Pacifico.ttf");
    roboto_medium.loadFromFile(path+"fonts/Roboto-Medium.ttf");
    roboto_light.loadFromFile(path+"fonts/Roboto-Light.ttf");
    roboto_regular.loadFromFile(path+"fonts/Roboto-Regular.ttf");
    roboto_bold.loadFromFile(path+"fonts/Roboto-Bold.ttf");
    roboto_black.loadFromFile(path+"fonts/Roboto-Black.ttf");
    sf::Text balance_text("Cash: $0", font);
    sf::Text score_text("Score: 0", font);
    sf::Text day_text("Day 0", font);
    sf::Text insufficient_funds_text("This building is already maxed.", font);
    sf::Text building_name_text("Building name", roboto_medium);
    sf::Text info_text("Build Cost: $0\nCapacity: 0\nGoods Cost: $0", roboto_medium);
    int building_name_x[] = {72,181,285,408,532,608,697,815};
    balance_text.setCharacterSize(font_size);
    score_text.setCharacterSize(font_size);
    day_text.setCharacterSize(font_size);
    balance_text.setFillColor(sf::Color::White);
    score_text.setFillColor(sf::Color::White);
    day_text.setFillColor(sf::Color::White);
    balance_text.setOutlineColor(sf::Color::Black);
    score_text.setOutlineColor(sf::Color::Black);
    day_text.setOutlineColor(sf::Color::Black);
    balance_text.setOutlineThickness(1);
    score_text.setOutlineThickness(1);
    day_text.setOutlineThickness(1);
    balance_text.setPosition(256,10);
    score_text.setPosition(10,10);
    day_text.setPosition(947,10);

    insufficient_funds_text.setCharacterSize(26);
    insufficient_funds_text.setFillColor(sf::Color::White);
    insufficient_funds_text.setOutlineColor(sf::Color::Black);
    insufficient_funds_text.setOutlineThickness(1);
    insufficient_funds_text.setPosition(380,610);
    insufficient_funds_text.setStyle(sf::Text::Bold);

    building_name_text.setCharacterSize(26);
    building_name_text.setFillColor(sf::Color::White);
    building_name_text.setOutlineColor(sf::Color::Black);
    building_name_text.setOutlineThickness(1);
    building_name_text.setPosition(0,0);
    building_name_text.setStyle(sf::Text::Bold);
    building_name_text.setLetterSpacing(0.2);

    info_text.setCharacterSize(18);
    info_text.setFillColor(sf::Color::White);
    info_text.setOutlineColor(sf::Color::Black);
    info_text.setOutlineThickness(1);
    info_text.setPosition(0,0);
    info_text.setLetterSpacing(0.6);

    sf::RectangleShape rect, insufficient_funds_rect, info_rect[13];
    rect.setSize(sf::Vector2f(100, 50));
    rect.setOutlineColor(sf::Color::Black);
    rect.setOutlineThickness(1);
    rect.setFillColor(sf::Color::Blue);

    insufficient_funds_rect.setFillColor(sf::Color(0,102,255));
    insufficient_funds_rect.setOutlineColor(sf::Color::Black);
    insufficient_funds_rect.setOutlineThickness(3);
    insufficient_funds_rect.setSize(sf::Vector2f(637,66));
    insufficient_funds_rect.setPosition(222,594);

    for(int i=0; i<12; i++){
        info_rect[i].setFillColor(sf::Color(0,153,255));
        info_rect[i].setOutlineColor(sf::Color::Black);
        info_rect[i].setOutlineThickness(2);
        info_rect[i].setSize(sf::Vector2f(240,110));
        if(i<8) info_rect[i].setPosition(110 * i,579);
        else info_rect[i].setPosition(853,579);
    }
    // info_rect.setPosition(2,579);

    int insufficient_funds_time = 0, INSUFFICIENT_FUNDS_TIME_MAX = (int)(1.75*(float)fps);
    int BUILD_TIME_MAX = 5*fps * 10; // 5 seconds. To speed up, just subtract this number.
    vector<int> info_time(13,0);
    vector<bool> gray_draw(8, true);
    vector<int> build_time(8,0);
    vector<int> build_status(8,-1);
    vector<int> build_animation(8,0);
    vector<int> upgrade_time(8,0);

    sf::Texture start_button_texture;
    start_button_texture.loadFromFile(path+"buttons/start game.png");
    sf::Sprite start_button;
    start_button.setTexture(start_button_texture);
    start_button.setPosition(800,700);
    start_button.setScale(0.7f,0.7f);
    bool moused;
    bool LeftClick = false, RightClick = false, isLeft = false, isRight = false, isStart, isButton, isGray = false;
    bool isDown = false, isUp = false, DownArrow = false, UpArrow = false;
    int phase = 0, cursor_state = 0;
    int balance = 200000, score = 0, day = 0;

    sf::Texture bottom_right_buttons_textures[3][2][3]; // (x,y) of the button; z = whether mouse is over button or not
    string bottom_right_button_names[] = {"benches","music","news","bus","next day"};
    string bottom_right_button_names_full[] = {"Bench","Music Box","Newspaper Box","Bus Stop"};
    sf::Sprite bottom_right_buttons[3][2];
    for(int k=0; k<5; k++){
        int i = k/2;
        int j = k%2;
        bottom_right_buttons_textures[i][j][0].loadFromFile(path+"buttons/"+bottom_right_button_names[k]+" button 1.png");
        bottom_right_buttons_textures[i][j][1].loadFromFile(path+"buttons/"+bottom_right_button_names[k]+" button 2.png");
        if(k<4) bottom_right_buttons_textures[i][j][2].loadFromFile(path+"buttons/"+bottom_right_button_names[k]+" button gray.png");
        bottom_right_buttons[i][j].setTexture(bottom_right_buttons_textures[i][j][0]);
        bottom_right_buttons[i][j].setScale(0.25f,0.25f);  // Benches Button: 879 692 0.25; Bus Stop: 986, 730
        bottom_right_buttons[i][j].setPosition(879+107*j, 692+38*i);
    }
    vector<int> bottom_right_buttons_status(5,0);
    int available_vendor_locations = 7;

    int gray_vendor_status = -1; // 0 is bench, 1 is music, 2 is news
    vector<int> vendor_location(8,-1); // 0 is bench, 1 is music, 2 is news
    vector<vector<int>> vendor_customers(8,vector<int>(5,-1)); // vendor_customers[0][1] is the 2nd customer at the vendor at location 0
    sf::IntRect vendor_rect[3];
    sf::FloatRect vendor_rect_bounds[3];
    sf::Sprite vendor[8];
    sf::Texture vendor_texture[3], gray_vendor_texture[3];
    string vendor_names[] = {"bench", "music", "news"};
    int vendor_cost[]={150,1000,800,5000,0};
    int vendor_adjust[]={40,20,-10,25};
    string vendor_desc[]={"A bench slows down\ncustomers.","Music box slows down\ncustomers.","Place where customers\ncan buy newspapers.","Bus stop delivers new\ncustomers every day."};
    int vendor_coords[][3]={{177,514,20},{211,487,25},{190,488,25}}; // vendor_coords[i][0] is x, vendor_coords[i][1] is y, vendor_coords[i][2]/100 is scale
    for(int i=0; i<3; i++){
        gray_vendor_texture[i].loadFromFile(path+"gray "+vendor_names[i]+".png");
        vendor_texture[i].loadFromFile(path+vendor_names[i]+".png");
        sf::Sprite temp_vendor;
        temp_vendor.setTexture(vendor_texture[i]);
        vendor_rect_bounds[i] = temp_vendor.getLocalBounds();
        vendor_rect[i].left = 0;
        vendor_rect[i].top = 0;
        vendor_rect[i].width = (int)vendor_rect_bounds[i].width;
        vendor_rect[i].height = (int)vendor_rect_bounds[i].height;
    }
    vendor[0].setPosition(157+136*0,514); // bench vendor[0].x=157 vendor[1].x=293
    vendor[0].setScale(0.2,0.2);
    vendor[1].setPosition(221+136*0,487);
    vendor[1].setScale(0.25,0.25);
    vendor[2].setPosition(200+136*0,488);
    vendor[2].setScale(0.25,0.25);
    for(int i=0; i<8; i++){
        vendor[i].setTexture(vendor_texture[0]);
        vendor[i].setPosition(157+136*i,514);
        vendor[i].setScale(0.25,0.25);
    }
    vector<string> customer_names = {"red shirt","blue dress","green dress","green shirt","orange shirt","pink shirt"}; // NOTE: TEMPORARY
    vector<Customer> customers;

    // sf::Sprite sprite, int y, int internal_y, int x, int animation_phase, int animation_time, int customer_texture, bool visited[8], bool visited_vendors[3], int status, int shopping_time, int building_at
    sf::Texture customer_walk_texture[8][7], customer_music_texture[8][3], customer_news_texture[8][5], customer_bench_texture[8];
    sf::IntRect customer_walk_rect[8][7], customer_music_rect[8][3], customer_news_rect[8][5], customer_bench_rect[8];
    int customer_walk_origin[8][7] = {{20,33,27,20,22,36,26},{17,20,38,16,19,37,27},{16,18,34,26,17,19,36},{16,21,32,26,16,18,35},{16,21,34,27,15,19,35},{15,21,33,27,15,18,35},{25,16,20,35,16,18,34},{23,37,27,22,22,37,23}}; // determines the x of the customer's origin
    int customer_music_origin[8][3] = {{24,24,25},{21,23,22},{23,22,22},{25,25,25},{27,27,27},{26,26,26},{22,21,20},{29,27,23}};
    int customer_news_origin[8][5] = {56,57,52,53,55,55,52,59};
    int customer_bench_origin[8] = {67,30,31,46,51,50,31,37};
    int customers_left = 0;
    sf::Vector2u zoo1;
    for(int i=0; i<(int)customer_names.size(); i++){
        for(int j=0; j<7; j++){
            customer_walk_texture[i][j].loadFromFile(path+"customers/"+customer_names[i]+"/walk "+to_string(j+1)+".PNG");
            zoo1 = customer_walk_texture[i][j].getSize();
            customer_walk_rect[i][j].left = 0;
            customer_walk_rect[i][j].top = 0;
            customer_walk_rect[i][j].width = zoo1.x;
            customer_walk_rect[i][j].height = zoo1.y;
            //cout << "Dimensions of walk " + to_string(j+1) + ": " << zoo1.x << " x " << zoo1.y << endl;
            if(j<3){
                customer_music_texture[i][j].loadFromFile(path+"customers/"+customer_names[i]+"/music "+to_string(j+1)+".PNG");
                zoo1 = customer_music_texture[i][j].getSize();
                customer_music_rect[i][j].left = 0;
                customer_music_rect[i][j].top = 0;
                customer_music_rect[i][j].width = zoo1.x;
                customer_music_rect[i][j].height = zoo1.y;
            }
            if(j<5){
                customer_news_texture[i][j].loadFromFile(path+"customers/"+customer_names[i]+"/news "+to_string(j+1)+".PNG");
                zoo1 = customer_news_texture[i][j].getSize();
                customer_news_rect[i][j].left = 0;
                customer_news_rect[i][j].top = 0;
                customer_news_rect[i][j].width = zoo1.x;
                customer_news_rect[i][j].height = zoo1.y;
            }
        }
        customer_bench_texture[i].loadFromFile(path+"customers/"+customer_names[i]+"/bench.PNG");
        zoo1 = customer_bench_texture[i].getSize();
        customer_bench_rect[i].left = 0;
        customer_bench_rect[i].top = 0;
        customer_bench_rect[i].width = zoo1.x;
        customer_bench_rect[i].height = zoo1.y;
    }
    int ANIMATION_TIME_MAX = fps/10, ANIMATION_MUSIC_MAX = fps/2, ANIMATION_NEWS_MAX = fps; // SUBJECT TO CHANGE
    Customer customer_init;
    customer_init.x=0;
    customer_init.internal_y=450; // min:444 max:477
    customer_init.y=customer_init.internal_y;
    customer_init.animation_time=ANIMATION_TIME_MAX;
    customer_init.animation_phase=0;
    customer_init.customer_texture=0;
    for(int i=0; i<8; i++) customer_init.visited.push_back(false);
    for(int i=0; i<3; i++) customer_init.visited_vendors.push_back(false);
    customer_init.status=0;
    customer_init.shopping_time=-1;
    customer_init.sprite.setTexture(customer_walk_texture[0][0]);
    customer_init.sprite.setTextureRect(customer_walk_rect[0][0]);
    customer_init.sprite.setScale(0.35,0.35);
    customer_init.sprite.setPosition(0,0);
    customer_init.next_frame.first = speed*7.5f/170.25f; // 17.25 seconds
    customer_init.next_frame.second = 0;

    // customers.push_back(customer_init);

    sf::FloatRect zoo(0,0,0,0);
    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        moused = false;
        window.clear();
        backdrop.setTexture(backdrop_texture[phase]);
        window.draw(backdrop);
        cursor.setTexture(cursor_texture1);
        cursor.setTextureRect(cursor_rect1);
        sf::Vector2i localPosition = sf::Mouse::getPosition(window);
        cursor.setPosition(localPosition.x,localPosition.y);
        isLeft = sf::Mouse::isButtonPressed(sf::Mouse::Left);
        isRight = sf::Mouse::isButtonPressed(sf::Mouse::Right);
        isDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
        isUp = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);

        if(phase==0){
            isStart = isMouse(start_button, cursor, moused);
            if(isStart && !isLeft){
                // start_button texture 2
                start_button.setColor(sf::Color::Green);
                cursor.setTexture(cursor_texture2);
                cursor.setTextureRect(cursor_rect2);
            }
            else{
                start_button.setColor(sf::Color::White);
            }
            if(isLeft && !LeftClick){ // user clicked this frame
                if(isStart){
                    cursor_state = 1;
                }
                else if(false){ // room for more buttons

                }
                else cursor_state = 0;
            }
            if(isLeft){
                if(isStart && cursor_state==1){
                    start_button.setColor(sf::Color::Red);
                    cursor.setTexture(cursor_texture2);
                    cursor.setTextureRect(cursor_rect2);
                    // start_button texture 3
                }
                else if(false && cursor_state==2){ // room for more buttons

                }
            }
            if(!isLeft && LeftClick){
                if(cursor_state==1 && isStart){
                    // start the game
                    phase = 1;
                }
                else if(cursor_state==2 && false){ // room for more buttons

                }
                cursor_state = 0;
            }
            window.draw(start_button);
        }
        else if(phase==1){
            for(int i=0;i<8;i++){
                if(buildings[i][0].status>=0){ // building should be drawn
                    window.draw(buildings[i][0].sprite);
                    window.draw(capacity_rect[buildings[i][0].location]);
                    window.draw(red_gradient[buildings[i][0].location]);
                    window.draw(capacity_text[buildings[i][0].location]);
                    /*
                    if(isMouse(buildings[i][0].sprite, cursor, moused)){
                        if(isUp){
                            buildings[i][0].sprite.scale(1.001f,1.001f);
                            sf::Vector2f loc = buildings[i][0].sprite.getPosition();
                            buildings[i][0].sprite.setPosition(loc.x,loc.y-0.14f);
                            cout << "scale: " << buildings[i][0].sprite.getScale().x << endl;
                        }
                        if(isDown){
                            buildings[i][0].sprite.scale(0.999f,0.999f);
                            sf::Vector2f loc = buildings[i][0].sprite.getPosition();
                            buildings[i][0].sprite.setPosition(loc.x,loc.y+0.14f);
                            cout << "scale: " << buildings[i][0].sprite.getScale().x << endl;
                        }
                    }*/
                }
                isButton = isMouse(buildings[i][1].sprite, cursor, moused);
                if(isButton){
                    cursor.setTexture(cursor_texture2);
                    cursor.setTextureRect(cursor_rect2);
                }
                if(isLeft && !LeftClick){
                    if(isButton){
                        //cout << "Button " << building_names_full[i] << " pressed, status is " << buildings[i][1].status << endl;
                        if(buildings[i][1].status >=0 && buildings[i][1].status <=2){
                            if(balance >= getPrice(buildings[i][1])){

                                if(buildings[i][1].status == 0){
                                    // buy building by showing gray silhouettes

                                    if(gray_buildings_status != -1){ // another button is currently at cancel
                                        int prev = gray_buildings_status;
                                        buildings[prev][1].status = 0;
                                        buildings[prev][1].sprite.setTexture(buttons_texture[prev][0]);
                                    }
                                    /*if(upgrade_status != -1){ // another button is currently being upgraded
                                        upgrade_time[upgrade_status] = 0;
                                        buildings[upgrade_status]
                                    }*/
                                    gray_buildings_status = i;
                                    //cout << "gray_buildings_status: " << gray_buildings_status << endl;
                                    buildings[i][1].status = 3; // cancel
                                    buildings[i][1].sprite.setTexture(buttons_texture[i][3]);
                                    for(int i=0; i<8; i++) gray_draw[i] = true;
                                    for(int i=0; i<8; i++){
                                        if(buildings[i][0].location >= 0){
                                            gray_draw[buildings[i][0].location] = false;
                                        }
                                        gray_buildings[i].setTexture(graybuildings_texture[gray_buildings_status][0]);
                                        gray_buildings[i].setTextureRect(texture_rect[gray_buildings_status][0]);
                                        gray_buildings[i].setScale(building_scales[gray_buildings_status],building_scales[gray_buildings_status]);
                                        //cout << "width= " << texture_rect[gray_buildings_status].width << " height= " << texture_rect[gray_buildings_status].height << endl;
                                    }
                                    for(int i=0; i<8; i++){
                                        if(gray_draw[i]){
                                            int gray_building_x = i*137;
                                            gray_buildings[i].setPosition(gray_building_x - (texture_rect_bounds[gray_buildings_status][0].width*building_scales[gray_buildings_status]/2) + 64, 418 - texture_rect_bounds[gray_buildings_status][0].height*building_scales[gray_buildings_status]);
                                        }
                                    }
                                }
                                else if(buildings[i][0].people > 0){
                                    insufficient_funds_time = INSUFFICIENT_FUNDS_TIME_MAX;
                                    insufficient_funds_text.setString("You can't upgrade while customers are shopping.");
                                    insufficient_funds_text.setPosition(256,610);
                                }
                                else{ // upgrade building
                                    upgrade_time[i] = BUILD_TIME_MAX * 5/8;
                                    buildings[i][1].status = 3*(buildings[i][1].status + 1);
                                    buildings[i][1].sprite.setTexture(buttons_texture[i][3]);
                                    cout << building_names_full[i] << " upgraded at location " << buildings[i][0].location << endl;
                                    sf::FloatRect *zoo = nullptr;
                                    zoo = new sf::FloatRect(0,0,0,0);
                                    *zoo = buildings[i][0].sprite.getGlobalBounds();
                                    int orange_height = 30; // Height that orange_gradient_rect is above capacity_rect when you upgrade a building
                                    orange_gradient_rect[buildings[i][0].location].setPosition(137*buildings[i][0].location+4,buildings[i][0].status==0?249-orange_height:196-orange_height); ///////////////////////// NOTE: CHANGE THIS
                                    orange_gradient[buildings[i][0].location].setPosition(137*buildings[i][0].location + 4,buildings[i][0].status==0?249-orange_height:196-orange_height);
                                    orange_gradient_text[buildings[i][0].location].setPosition(137*buildings[i][0].location+53,buildings[i][0].status==0?248-orange_height:195-orange_height);
                                    delete zoo;
                                }

                            }
                            else{
                                insufficient_funds_time = INSUFFICIENT_FUNDS_TIME_MAX;
                                insufficient_funds_text.setString("You don't have enough money.");
                                insufficient_funds_text.setPosition(380,610);
                            }
                        }
                        else if(buildings[i][1].status == 4){
                            insufficient_funds_time = INSUFFICIENT_FUNDS_TIME_MAX;
                            insufficient_funds_text.setString("This building is already maxed.");
                            insufficient_funds_text.setPosition(370,610);
                            // display message "building is maxed out"
                        }
                        else if(buildings[i][1].status == 3){

                            if(build_time[buildings[i][0].location] > BUILD_TIME_MAX*3/8){
                                // cancel action of buying a building, but show gray silhouettes
                                if(gray_buildings_status != -1){ // another button is currently at cancel
                                    int prev = gray_buildings_status;
                                    buildings[prev][1].status = 0;
                                    buildings[prev][1].sprite.setTexture(buttons_texture[prev][0]);
                                }
                                gray_buildings_status = i;
                                build_time[buildings[i][0].location] = 0;
                                build_status[buildings[i][0].location] = -1;
                                buildings[i][0].location = -1;


                                for(int i=0; i<8; i++) gray_draw[i] = true;
                                for(int i=0; i<8; i++){
                                    if(buildings[i][0].location >= 0){
                                        gray_draw[buildings[i][0].location] = false;
                                    }
                                    gray_buildings[i].setTexture(graybuildings_texture[gray_buildings_status][0]);
                                    gray_buildings[i].setTextureRect(texture_rect[gray_buildings_status][0]);
                                    gray_buildings[i].setScale(building_scales[gray_buildings_status],building_scales[gray_buildings_status]);
                                    //cout << "width= " << texture_rect[gray_buildings_status].width << " height= " << texture_rect[gray_buildings_status].height << endl;
                                }
                                for(int i=0; i<8; i++){
                                    if(gray_draw[i]){
                                        int gray_building_x = i*137;
                                        gray_buildings[i].setPosition(gray_building_x - (texture_rect_bounds[gray_buildings_status][0].width*building_scales[gray_buildings_status]/2) + 64, 418 - texture_rect_bounds[gray_buildings_status][0].height*building_scales[gray_buildings_status]);
                                        //gray_buildings[i].setPosition(gray_building_x - (zoo.width*building_scale/2) + 64, 418 - zoo.height*building_scale);

                                    }
                                }


                            }
                            else if(build_time[buildings[i][0].location]<=0){
                                gray_buildings_status = -1;
                                buildings[i][1].status = 0;
                                buildings[i][1].sprite.setTexture(buttons_texture[i][0]);
                            }
                        }
                        else if(buildings[i][1].status == 6 || buildings[i][1].status == 9){
                            buildings[i][1].status = (buildings[i][1].status/3)-1;
                            buildings[i][1].sprite.setTexture(buttons_texture[i][buildings[i][1].status]);
                            upgrade_time[i] = 0;
                        }
                        else{
                            cout << "--- ERROR --- This button's status is fucking unreadable. It's status is " << buildings[i][1].status << endl;
                        }
                    }
                }

                if(isButton){
                    if(info_time[i] < 20){
                        info_time[i]++;
                        if(info_time[i]>=20){
                            building_name_text.setString(building_names_full[i]);
                            if(buildings[i][1].status == 4){ // building is maxed
                                info_text.setString("Maximum upgrade level\nhas been reached.");
                            }
                            else{ // show button's info
                                info_text.setString("Build Cost: $"+ to_string(getPrice(buildings[i][1])) +"\nCapacity: "+ to_string(getCapacity(buildings[i][1]))+"\nGoods Cost: $"+to_string(getIncome(buildings[i][1])));
                            }
                            building_name_text.setPosition(building_name_x[i], 584);
                            info_text.setPosition(15 + 110*i, 620);
                        }
                    }
                    // display cost and stuff
                }
                else if(info_time[i] > 0){
                    info_time[i]-=2;
                    if(info_time[i] < 0) info_time[i] = 0;
                }
                if(info_time[i] > 0){
                    // setSize(sf::Vector2f(240,110));
                    // setOutlineThickness(2);

                    info_rect[i].setSize(sf::Vector2f(info_time[i]*24/2, info_time[i]*11/2));
                    // info_rect[i].setOutlineThickness(info_time[i]/25);
                    if(info_time[i] < 50){
                        info_rect[i].setPosition(110 * i + (240-info_time[i]*24/2)/2, 579 + (110-info_time[i]*11/2)/2);
                    }
                }
                window.draw(buildings[i][1].sprite);
                // draw buttons

            }
            for(int k=0; k<5; k++){ // bottom right buttons
                int i = k/2;
                int j = k%2;
                isButton = isMouse(bottom_right_buttons[i][j], cursor, moused);
                if(isButton){
                    cursor.setTexture(cursor_texture2);
                    cursor.setTextureRect(cursor_rect2);
                    if(k<4 && info_time[8+k] < 20){
                        info_time[8+k]++;
                        if(info_time[8+k]>=20){
                            building_name_text.setString(bottom_right_button_names_full[k]);
                            info_text.setString("Build Cost: $" + to_string(vendor_cost[k])+"\n"+vendor_desc[k]);
                            building_name_text.setPosition(900+vendor_adjust[k],584);
                            info_text.setPosition(875,620);
                        }
                    }
                    if(bottom_right_buttons_status[k]==0){ // button does not look like it is hovered over
                        bottom_right_buttons_status[k]=1;
                        bottom_right_buttons[i][j].setTexture(bottom_right_buttons_textures[i][j][1]);
                    }
                    if(isLeft && !LeftClick){
                        if(k<4){
                            if(balance>=vendor_cost[k]){
                                if(k==3){ // buy bus stop

                                }
                                else if(gray_vendor_status!=k){
                                    gray_vendor_status=k;
                                    for(int l=1; l<8; l++){
                                        if(vendor_location[l]==-1){ // {157,514,20}
                                            vendor[l].setTexture(gray_vendor_texture[k]);
                                            vendor[l].setTextureRect(vendor_rect[k]);
                                            vendor[l].setPosition(vendor_coords[k][0]+136*l-136,vendor_coords[k][1]);
                                            vendor[l].setScale(((float)vendor_coords[k][2])/100.0,((float)vendor_coords[k][2])/100.0);
                                            //cout << "vendor[" << l << "] at (" << vendor_coords[k][0]+136*l << " , " << vendor_coords[k][1] << ") scale: " << ((float)vendor_coords[k][2])/100.0 << endl;
                                        }
                                    }
                                }
                                else{
                                    gray_vendor_status=-1;
                                }
                            }
                            else{
                                insufficient_funds_time = INSUFFICIENT_FUNDS_TIME_MAX;
                                insufficient_funds_text.setString("You don't have enough money.");
                                insufficient_funds_text.setPosition(380,610);
                            }

                        }
                        else if(customers_left<=0){ // next day button pressed
                            day++;
                            customers.clear();
                            customers_left = 2*day+18;
                            for(int i=0; i<customers_left; i++){
                                customer_init.internal_y = rand() % 34 + 435;// min:415 max:490 middle: 452
                                customer_init.y = customer_init.internal_y;
                                customer_init.x = -67.5*i; // note: unverified
                                customer_init.customer_texture = rand() % (int)customer_names.size();
                                //for(int i=0; i<8; i++) customer_init.visited[i]=false;
                                //for(int i=0; i<3; i++) customer_init.visited_vendors[i]=false;
                                customer_init.sprite.setTexture(customer_walk_texture[customer_init.customer_texture][0]);
                                customer_init.sprite.setTextureRect(customer_walk_rect[customer_init.customer_texture][0]);
                                customer_init.sprite.setPosition(customer_init.x,customer_init.y);
                                customers.push_back(customer_init);
                            }

                        }
                        // if vendor_location[0-6] is equal to -1, then gray vendor should be drawn

                    }
                }
                else{
                    if(bottom_right_buttons_status[k]==1){ // buttons looks like it is hovered over
                        bottom_right_buttons_status[k]=0;
                        bottom_right_buttons[i][j].setTexture(bottom_right_buttons_textures[i][j][0]);
                    }
                    if(info_time[8+k] > 0){
                        info_time[8+k]-=2;
                        if(info_time[8+k]<0) info_time[8+k]=0;
                    }
                }


                if(info_time[8+k]>0){
                    info_rect[8+k].setSize(sf::Vector2f(info_time[8+k]*24/2, info_time[8+k]*11/2));
                    if(info_time[8+k] < 20){
                        info_rect[8+k].setPosition(853 + (240-info_time[8+k]*24/2)/2, 579 + (110-info_time[8+k]*11/2)/2);
                    }
                }
                window.draw(bottom_right_buttons[i][j]);
            }
            for(int i=0; i<(int)customers.size(); i++){ // used to decide next_frame and update customer status if necessary
                // 0 is walking to the right
                // 1 is walking to a building
                // 2 is walking to a vendor
                // 3 is in a building
                // 4 is on a bench
                // 5 is at music
                // 6 is at newspapers
                // 7 is leaving a building
                // 8 is leaving a vendor
                customers[i].location = ((int)customers[i].x+6)/137;
                //if(customers[i].x <= 0) customers[i].location = -1;
                if(customers[i].location < 0){
                    customers[i].x+=speed*7.5f/170.25f;
                    if(customers[i].location==-1){
                        customers[i].sprite.setPosition(customers[i].x,customers[i].internal_y);
                        window.draw(customers[i].sprite);
                    }
                    continue;
                }
                if(customers[i].location > 7){
                    if(customers[i].done_shopping!=2){
                        customers[i].done_shopping=2;
                        customers_left--;
                    }
                    if(customers[i].location==7){
                        customers[i].x+=speed*7.5f/170.25f;
                        customers[i].sprite.setPosition(customers[i].x,customers[i].y);
                        window.draw(customers[i].sprite);
                    }
                    continue;
                }
                if(customers[i].status == 0){ // walking to the right
                    if(building_locations[customers[i].location] != -1 && upgrade_time[building_locations[customers[i].location]]==0 && buildings[building_locations[customers[i].location]][0].people < getCapacity(buildings[building_locations[customers[i].location]][0]) && !customers[i].visited[customers[i].location]){ // first check if current building is available
                        float x_dist = (float)customers[i].location*137.0+62.0-customers[i].x;
                        float y_dist = 415.0-customers[i].y;
                        customers[i].next_frame.first = x_dist*speed/((float)fps_initial*2.25*10.0);
                        customers[i].next_frame.second = y_dist*speed/((float)fps_initial*2.25*10.0);
                        customers[i].status = 1;
                        customers[i].visited[customers[i].location] = true;
                        // 1.5 seconds
                    }
                    else if(vendor_location[customers[i].location] > -1 && !customers[i].visited_vendors[vendor_location[customers[i].location]]){ // second check if current vendor is available
                        customers[i].vendor_spot = -1;
                        for(int j=0; j<5; j++){
                            if(vendor_location[customers[i].location] == 0 && j>=3) break;
                            if(vendor_customers[customers[i].location][j]==-1){
                                customers[i].vendor_spot = j;
                                break;
                            }
                        }
                        if(customers[i].vendor_spot > -1){ // walk to vendor accepted
                            vendor_customers[customers[i].location][customers[i].vendor_spot] = i;
                            customers[i].visited_vendors[vendor_location[customers[i].location]] = true;
                            float x_dist;
                            float y_dist;
                            if(vendor_location[customers[i].location]==0){
                                x_dist = (float)customers[i].location*137.0+77.0+(float)(28.0*(customers[i].vendor_spot-1))-customers[i].x;
                                y_dist = 507.0-customers[i].y;
                            }
                            else{
                                x_dist = (float)customers[i].location*137.0+77.0+(float)(20.0*(customers[i].vendor_spot-2))-customers[i].x;
                                y_dist = 480.0-customers[i].y;
                            }
                            customers[i].next_frame.first = x_dist*speed/((float)fps_initial*2.25*10.0);
                            customers[i].next_frame.second = y_dist*speed/((float)fps_initial*2.25*10.0);
                            customers[i].status = 2;
                        }
                    }
                }
                else if(customers[i].status == 1){
                    if(customers[i].y <= 415){
                        customers[i].x = customers[i].location*137+62;
                        customers[i].y = 415;
                        if(building_locations[customers[i].location] != -1 && upgrade_time[building_locations[customers[i].location]]==0 && buildings[building_locations[customers[i].location]][0].people < getCapacity(buildings[building_locations[customers[i].location]][0])){
                            // able to enter building
                            customers[i].status = 3;
                            customers[i].next_frame.first = 0;
                            customers[i].next_frame.second = 0;
                            customers[i].shopping_time = 5*fps_initial*(int)speed;
                            buildings[building_locations[customers[i].location]][0].people++;
                            capacity_text[customers[i].location].setString(to_string(buildings[building_locations[customers[i].location]][0].people) + "/" + to_string(getCapacity(buildings[building_locations[customers[i].location]][0])));
                            red_gradient[customers[i].location].setTextureRect(sf::IntRect(0,0,500*buildings[building_locations[customers[i].location]][0].people/getCapacity(buildings[building_locations[customers[i].location]][0]),92));
                        }
                        else{ // tried to enter building and couldn't
                            customers[i].status = 7;
                            float x_dist = (float)customers[i].location*137.0+130.0-customers[i].x;
                            float y_dist = (float)customers[i].internal_y-customers[i].y;
                            customers[i].next_frame.first = x_dist*speed/((float)fps_initial*2.25*10.0);
                            customers[i].next_frame.second = y_dist*speed/((float)fps_initial*2.25*10.0);
                            window.draw(customers[i].sprite);
                        }
                        // enter building blah blah blah
                    }
                }
                else if(customers[i].status == 2){
                    if(vendor_location[customers[i].location]==0 && customers[i].y >= 510){
                        customers[i].sprite.setScale(0.26,0.26);
                        customers[i].y = 507;
                        customers[i].x = customers[i].location*137+77+(28*(customers[i].vendor_spot-1));
                        customers[i].status = 4;
                        customers[i].next_frame.first = 0;
                        customers[i].next_frame.second = 0;
                        customers[i].shopping_time = 5*fps_initial*(int)speed;
                    }
                    else if(vendor_location[customers[i].location]>=1 && vendor_location[customers[i].location]<=2 && customers[i].y >= 480){
                        customers[i].y = 480;
                        customers[i].x = customers[i].location*137+62+(20*(customers[i].vendor_spot-2));
                        customers[i].status = vendor_location[customers[i].location] + 4;
                        customers[i].next_frame.first = 0;
                        customers[i].next_frame.second = 0;
                        customers[i].animation_time = 0;
                        customers[i].animation_phase = 5;
                        customers[i].shopping_time = 5*fps_initial*(int)speed;
                    }

                }
                else if(customers[i].status == 3){
                    customers[i].shopping_time-=(int)speed;
                    if(customers[i].shopping_time <= 0){
                        customers[i].shopping_time=0;
                        customers[i].status = 7;
                        buildings[building_locations[customers[i].location]][0].people--;
                        capacity_text[customers[i].location].setString(to_string(buildings[building_locations[customers[i].location]][0].people) + "/" + to_string(getCapacity(buildings[building_locations[customers[i].location]][0])));
                        red_gradient[customers[i].location].setTextureRect(sf::IntRect(0,0,500*buildings[building_locations[customers[i].location]][0].people/getCapacity(buildings[building_locations[customers[i].location]][0]),92));
                        float x_dist = (float)customers[i].location*137.0+130.0-customers[i].x;
                        float y_dist = (float)customers[i].internal_y-customers[i].y;
                        customers[i].next_frame.first = x_dist*speed/((float)fps_initial*2.25*10.0);
                        customers[i].next_frame.second = y_dist*speed/((float)fps_initial*2.25*10.0);
                        customers[i].animation_time = 0;
                        customers[i].animation_phase = 6;
                    }
                }
                else if(customers[i].status == 4 || customers[i].status == 5 || customers[i].status == 6){ // at a vendor
                    customers[i].shopping_time-=(int)speed;
                    if(customers[i].shopping_time <= 0){ // done vending
                        customers[i].shopping_time=0;
                        vendor_customers[customers[i].location][customers[i].vendor_spot] = -1;
                        customers[i].sprite.setScale(0.35,0.35);
                        customers[i].vendor_spot = -1;
                        customers[i].status = 8;
                        float x_dist = (float)customers[i].location*137.0+130.0-customers[i].x;
                        float y_dist = (float)customers[i].internal_y-510;
                        customers[i].next_frame.first = x_dist*speed/((float)fps_initial*2.25*10.0);
                        customers[i].next_frame.second = y_dist*speed/((float)fps_initial*2.25*10.0);
                        customers[i].animation_phase = 6;
                        customers[i].animation_time = 0;
                    }
                }
                else if(customers[i].status == 7){ // leaving a building
                    if(customers[i].y >= customers[i].internal_y){ // done leaving building
                        customers[i].x = customers[i].location*137+130;
                        customers[i].y = customers[i].internal_y;
                        // first check if next building available
                        if(customers[i].location < 7 && building_locations[customers[i].location+1] != -1 && upgrade_time[building_locations[customers[i].location+1]]==0 && buildings[building_locations[customers[i].location+1]][0].people < getCapacity(buildings[building_locations[customers[i].location+1]][0]) && !customers[i].visited[customers[i].location+1]){
                            float x_dist = (float)(customers[i].location+1)*137.0+62.0-customers[i].x;
                            float y_dist = 415.0-customers[i].y;
                            customers[i].next_frame.first = x_dist*speed/((float)fps_initial*2.25*10.0);
                            customers[i].next_frame.second = y_dist*speed/((float)fps_initial*2.25*10.0);
                            customers[i].status = 1;
                            customers[i].visited[customers[i].location+1] = true;
                        }
                        // second check if current vendor is available
                        else if(vendor_location[customers[i].location] > -1 && !customers[i].visited_vendors[vendor_location[customers[i].location]]){
                            customers[i].vendor_spot = -1;
                            for(int j=0; j<5; j++){
                                if(vendor_location[customers[i].location] == 0 && j>=3) break;
                                if(vendor_customers[customers[i].location][j]==-1){
                                    customers[i].vendor_spot = j;
                                    break;
                                }
                            }
                            if(customers[i].vendor_spot > -1){ // walk to vendor accepted
                                vendor_customers[customers[i].location][customers[i].vendor_spot] = i;
                                customers[i].visited_vendors[vendor_location[customers[i].location]] = true;
                                float x_dist = (float)customers[i].location*137.0+62.0+(float)(20*(vendor_location[customers[i].location]==0?customers[i].vendor_spot-1:customers[i].vendor_spot-2))-customers[i].x;
                                float y_dist;
                                if(vendor_location[customers[i].location] == 0) y_dist = 510.0-customers[i].y;
                                else y_dist = 480.0-customers[i].y;
                                customers[i].next_frame.first = x_dist*speed/((float)fps_initial*2.25*10.0);
                                customers[i].next_frame.second = y_dist*speed/((float)fps_initial*2.25*10.0);
                                customers[i].status = 2;
                            }
                            else{ // walk to vendor rejected
                                customers[i].status = 0;
                                customers[i].next_frame.first = speed*7.5f/170.25f;
                                customers[i].next_frame.second = 0;
                            }
                        }
                        // walk right
                        else{
                            customers[i].status = 0;
                            customers[i].next_frame.first = speed*7.5f/170.25f;
                            customers[i].next_frame.second = 0;
                        }
                        if(customers[i].status == 7) cout << "!!!WARNING IT IS IMPOSSIBLE FOR customers[i].status == 7 HERE!!!" << endl;
                    }
                }
                else if(customers[i].status == 8){ // leaving a vendor
                    if(customers[i].y <= customers[i].internal_y){ // done leaving vendor
                        customers[i].x = customers[i].location*137+130;
                        customers[i].y = customers[i].internal_y;
                        // first check if current building is available
                        if(building_locations[customers[i].location] != -1 && upgrade_time[building_locations[customers[i].location]]==0 && buildings[building_locations[customers[i].location]][0].people < getCapacity(buildings[building_locations[customers[i].location]][0]) && !customers[i].visited[customers[i].location]){
                            float x_dist = (float)customers[i].location*137.0+62.0-customers[i].x;
                            float y_dist = 415.0-customers[i].y;
                            customers[i].next_frame.first = x_dist*speed/((float)fps_initial*2.25*10.0);
                            customers[i].next_frame.second = y_dist*speed/((float)fps_initial*2.25*10.0);
                            customers[i].status = 1;
                            customers[i].visited[customers[i].location] = true;
                            // 1.5 seconds
                        }

                        // second check if next building is available
                        // third check if next vendor is available
                        else{
                            customers[i].status = 0;
                            customers[i].next_frame.first = speed*7.5f/170.25f;
                            customers[i].next_frame.second = 0;
                        }
                        if(customers[i].status == 8) cout << "!!!WARNING IT IS IMPOSSIBLE FOR customers[i].status == 8 HERE!!!" << endl;
                    }
                }

                // 0 is walking to the right
                // 1 is walking to a building
                // 2 is walking to a vendor
                // 3 is in a building
                // 4 is on a bench
                // 5 is at music
                // 6 is at newspapers
                // 7 is leaving a building
                // 8 is leaving a vendor
                customers[i].x+=customers[i].next_frame.first;
                customers[i].y+=customers[i].next_frame.second;
                customers[i].sprite.setPosition((float)customers[i].x,(float)customers[i].y);

                // walking animation
                if(customers[i].status == 0 || customers[i].status == 1 || customers[i].status == 2 || customers[i].status == 7 || customers[i].status == 8){
                    customers[i].animation_time-=fps_initial;
                    if(customers[i].animation_time<=0){
                        customers[i].animation_time = fps*ANIMATION_TIME_MAX;
                        customers[i].animation_phase++;
                        if(customers[i].animation_phase >= 7) customers[i].animation_phase=0;
                        //cout << customers[i].animation_time << " " << customers[i].animation_phase << endl;
                        customers[i].sprite.setTexture(customer_walk_texture[customers[i].customer_texture][customers[i].animation_phase]);
                        customers[i].sprite.setTextureRect(customer_walk_rect[customers[i].customer_texture][customers[i].animation_phase]);
                        customers[i].sprite.setOrigin(customer_walk_origin[customers[i].customer_texture][customers[i].animation_phase], customer_walk_rect[customers[i].customer_texture][customers[i].animation_phase].height/2);
                    }

                }
                // sitting animation
                else if(customers[i].status == 4){
                    customers[i].sprite.setTexture(customer_bench_texture[customers[i].customer_texture]);
                    customers[i].sprite.setTextureRect(customer_bench_rect[customers[i].customer_texture]);
                    customers[i].sprite.setOrigin(customer_bench_origin[customers[i].customer_texture], customer_bench_rect[customers[i].customer_texture].height/2);
                }

                // music animation
                else if(customers[i].status == 5){
                    customers[i].animation_time-=fps_initial;
                    if(customers[i].animation_time<=0){
                        customers[i].animation_time = fps*ANIMATION_MUSIC_MAX;
                        customers[i].animation_phase++;
                        if(customers[i].animation_phase >= 3) customers[i].animation_phase=0;
                        customers[i].sprite.setTexture(customer_music_texture[customers[i].customer_texture][customers[i].animation_phase]);
                        customers[i].sprite.setTextureRect(customer_music_rect[customers[i].customer_texture][customers[i].animation_phase]);
                        customers[i].sprite.setOrigin(customer_music_origin[customers[i].customer_texture][customers[i].animation_phase], customer_music_rect[customers[i].customer_texture][customers[i].animation_phase].height/2);
                    }
                }

                // news animation
                else if(customers[i].status == 6){
                    customers[i].animation_time-=fps_initial;
                    if(customers[i].animation_time<=0){
                        customers[i].animation_time = fps*ANIMATION_NEWS_MAX;
                        customers[i].animation_phase++;
                        if(customers[i].animation_phase >= 5) customers[i].animation_phase=0;
                        customers[i].sprite.setTexture(customer_news_texture[customers[i].customer_texture][customers[i].animation_phase]);
                        customers[i].sprite.setTextureRect(customer_news_rect[customers[i].customer_texture][customers[i].animation_phase]);
                        customers[i].sprite.setOrigin(customer_news_origin[customers[i].customer_texture][customers[i].animation_phase], customer_news_rect[customers[i].customer_texture][customers[i].animation_phase].height/2);
                    }
                }
                if(customers[i].status != 3) window.draw(customers[i].sprite);
                /*if(isRight && !RightClick && i==0){
                    cout << "customers[" << i << "]\nvisited: ";
                    for(bool a : customers[i].visited) cout << a << " ";
                    cout << "\nvisited_vendors: ";
                    for(bool a : customers[i].visited_vendors) cout << a << " ";
                    cout << "\nstatus: " << customers[i].status << "\nshopping time: "<<customers[i].shopping_time<<"\nvendor_spot: "<<customers[i].vendor_spot<<"\nlocation: "<<customers[i].location<<endl;
                    cout << "Location: ( " << customers[i].x << " , " << customers[i].y << " )\n";
                }*/
                //cout << customers[i].visited_vendors[2] << " ";
            }
            //cout << endl;


            if(gray_buildings_status >= 0){
                for(int i=0; i<8; i++){
                    if(gray_draw[i]){
                        isGray = isMouse(gray_buildings[i], cursor, moused);
                        if(isGray && !isLeft){
                            cursor.setTexture(cursor_texture2);
                            cursor.setTextureRect(cursor_rect2);
                            window.draw(gray_buildings[i]);
                        }
                        else if(isGray && isLeft && !LeftClick){
                            cout << building_names_full[gray_buildings_status] << " bought at location " << i << endl;
                            buildings[gray_buildings_status][0].location = i;
                            build_status[i] = gray_buildings_status;
                            build_time[i] = BUILD_TIME_MAX;
                            build_animation[i] = rand() % 2;
                            build_animation[i] = 0; // Note: When more animations are added remove this
                            gray_buildings_status = -1;
                            orange_gradient[i].setPosition(137*i + 4,300);
                            orange_gradient_rect[i].setPosition(137*i+4,300);
                            orange_gradient_text[i].setPosition(137*i+53,299);
                            for(int i=0; i<8; i++) gray_draw[i] = false;

                        }
                        else{
                            window.draw(gray_buildings[i]);
                        }
                    }
                }
            }

            // NOTE: build_time[0] is for location 0 while upgrade_time[0] is for boutique! NOT the same!
            for(int i=0; i<8; i++){
                if(build_time[i] > 0){ // i represents the location
                    // BUILD_TIME_MAX = 600
                    // 5/8 of the build time should be the build men
                    if(build_time[i] > BUILD_TIME_MAX*3/8){
                        window.draw(build_men[i]);
                        int build_percent = 100*(BUILD_TIME_MAX-build_time[i])/(BUILD_TIME_MAX*5/8);
                        orange_gradient[i].setTextureRect(sf::IntRect(0,0,5*build_percent,92));
                        orange_gradient_text[i].setString(to_string(build_percent) + "%");
                        window.draw(orange_gradient[i]);
                        window.draw(orange_gradient_rect[i]);
                        window.draw(orange_gradient_text[i]);
                        //if(build_time[i]%10==0) cout << "build_time[" << i << "] = " << build_time[i] << endl;
                    }
                    else{
                        buildings[build_status[i]][1].sprite.setTexture(buttons_texture[build_status[i]][1]);
                        if(build_animation[i] == 0){ // standard build animation
                            if(build_time[i] > BUILD_TIME_MAX*5/16){ // shadow growing
                                float growth_percentage = (((float)BUILD_TIME_MAX*3/8)-(float)build_time[i])/((float)BUILD_TIME_MAX/16);
                                //if((int)(growth_percentage*10000)%100==0)
                                //    cout << "Shadow Growth: " << growth_percentage << endl;
                                build_shadow[i].setScale(growth_percentage, growth_percentage*.42);
                            }
                            else if(build_time[i] > BUILD_TIME_MAX*7/32){
                                // shadow is at full growth
                            }
                            else if(build_time[i] > BUILD_TIME_MAX*5/32){ // shadow shrinking
                                float growth_percentage = ((float)build_time[i]-((float)BUILD_TIME_MAX*5/32))/((float)BUILD_TIME_MAX/16);
                                //if((int)(growth_percentage*10000)%100==0)
                                //    cout << "Shadow Growth: " << growth_percentage << endl;
                                build_shadow[i].setScale(growth_percentage,growth_percentage*.42);
                            }
                            if(build_time[i] > BUILD_TIME_MAX*5/32){
                                window.draw(build_shadow[i]);
                            }

                        }
                        else if(build_animation[i]==1){

                        }
                        else{

                        }
                    }

                    build_time[i]-=(int)speed;

                    if(build_time[i] <= 0){
                        build_time[i]=0;
                        buildings[build_status[i]][0].status = 0;
                        buildings[build_status[i]][1].status = 1;
                        buildings[build_status[i]][1].sprite.setTexture(buttons_texture[build_status[i]][1]);
                        int building_x = i*137;
                        sf::FloatRect *zoo = nullptr;
                        zoo = new sf::FloatRect(0,0,0,0);
                        *zoo = buildings[build_status[i]][0].sprite.getGlobalBounds();
                        buildings[build_status[i]][0].sprite.setPosition(building_x - (zoo->width/2) + 64, 418 - zoo->height);
                        delete zoo;
                        building_locations[i]=build_status[i];
                        capacity_text[i].setString("0/" + to_string(getCapacity(buildings[building_locations[i]][0])));
                        capacity_text[i].setPosition(137*i+53,248);
                        red_gradient[i].setPosition(137*i+4,249);
                        capacity_rect[i].setPosition(137*i+4,249);
                        build_status[i] = -1;
                    }
                }
                if(upgrade_time[i] > 0){ // i represents the building
                    int upgrade_percent = 100*((BUILD_TIME_MAX * 5/8)-upgrade_time[i]) / (BUILD_TIME_MAX * 5/8);
                    //if(upgrade_time[i]%10==0) cout << "Upgrade_time[" << i << "] = " << upgrade_time[i] << endl;
                    orange_gradient[buildings[i][0].location].setTextureRect(sf::IntRect(0,0,5*upgrade_percent,92));
                    orange_gradient_text[buildings[i][0].location].setString(to_string(upgrade_percent) + "%");
                    window.draw(orange_gradient[buildings[i][0].location]);
                    window.draw(orange_gradient_rect[buildings[i][0].location]);
                    window.draw(orange_gradient_text[buildings[i][0].location]);
                    upgrade_time[i]-=(int)speed;
                    if(upgrade_time[i] <= 0){
                        upgrade_time[i]=0;
                        buildings[i][1].status = buildings[i][1].status/3;
                        if(buildings[i][1].status==3) buildings[i][1].status++;
                        buildings[i][1].sprite.setTexture(buttons_texture[i][buildings[i][1].status]);
                        buildings[i][0].status++;
                        buildings[i][0].sprite.setTexture(buildings_texture[i][buildings[i][0].status]);
                        buildings[i][0].sprite.setTextureRect(texture_rect[i][buildings[i][0].status]);
                        capacity_text[buildings[i][0].location].setString("0/"+to_string(getCapacity(buildings[i][0])));
                        capacity_text[buildings[i][0].location].setPosition(137*buildings[i][0].location+53,(buildings[i][0].status==2?158:195));
                        red_gradient[buildings[i][0].location].setPosition(137*buildings[i][0].location+4,(buildings[i][0].status==2?159:196));
                        capacity_rect[buildings[i][0].location].setPosition(137*buildings[i][0].location+4,(buildings[i][0].status==2?159:196));
                        cout << building_names_full[i] << " has been upgraded to level " << buildings[i][0].status+1 << endl;
                        int building_x = buildings[i][0].location*137;
                        sf::FloatRect *zoo = nullptr;
                        zoo = new sf::FloatRect(0,0,0,0);
                        *zoo = buildings[i][0].sprite.getGlobalBounds();
                        buildings[i][0].sprite.setPosition(building_x - (zoo->width/2) + 64, 418 - zoo->height);
                        delete zoo;
                    }
                }
            }
            if(gray_vendor_status>=0){
                for(int i=1; i<8; i++){
                    if(vendor_location[i]==-1){
                        isGray = isMouse(vendor[i], cursor, moused);
                        if(isGray && !isLeft){
                            cursor.setTexture(cursor_texture2);
                            cursor.setTextureRect(cursor_rect2);
                            window.draw(vendor[i]);
                        }
                        else if(isGray && isLeft && !LeftClick){
                            cout << vendor_names[gray_vendor_status] << " bought at location " << i << endl;
                            available_vendor_locations--;
                            if(available_vendor_locations==0){
                                bottom_right_buttons_status[0]=2;
                                bottom_right_buttons_status[1]=2;
                                bottom_right_buttons_status[2]=2;
                                bottom_right_buttons[0][0].setTexture(bottom_right_buttons_textures[0][0][2]);
                                bottom_right_buttons[0][1].setTexture(bottom_right_buttons_textures[0][1][2]);
                                bottom_right_buttons[1][0].setTexture(bottom_right_buttons_textures[1][0][2]);
                            }
                            vendor_location[i]=gray_vendor_status;
                            vendor[i].setTexture(vendor_texture[gray_vendor_status]);
                            gray_vendor_status=-1;
                        }
                        else{
                            window.draw(vendor[i]);
                        }
                    }
                }
            }
            for(int i=1; i<8; i++){
                if(vendor_location[i]!=-1) window.draw(vendor[i]);
            }
            if(insufficient_funds_time > 0){
                if(insufficient_funds_time < INSUFFICIENT_FUNDS_TIME_MAX/5){ // fade out animation
                    insufficient_funds_rect.setFillColor(sf::Color(0,102,255,insufficient_funds_time*256/(INSUFFICIENT_FUNDS_TIME_MAX/5)));
                    insufficient_funds_rect.setOutlineColor(sf::Color(0,0,0,insufficient_funds_time*256/(INSUFFICIENT_FUNDS_TIME_MAX/5)));
                    insufficient_funds_text.setFillColor(sf::Color(255,255,255,insufficient_funds_time*256/(INSUFFICIENT_FUNDS_TIME_MAX/5)));
                    insufficient_funds_text.setOutlineColor(sf::Color(0,0,0,insufficient_funds_time*256/(INSUFFICIENT_FUNDS_TIME_MAX/5)));
                }
                else if(insufficient_funds_time > INSUFFICIENT_FUNDS_TIME_MAX*4/5){
                    insufficient_funds_rect.setFillColor(sf::Color(0,102,255,(INSUFFICIENT_FUNDS_TIME_MAX-insufficient_funds_time)*256/(INSUFFICIENT_FUNDS_TIME_MAX/5)));
                    insufficient_funds_rect.setOutlineColor(sf::Color(0,0,0,(INSUFFICIENT_FUNDS_TIME_MAX-insufficient_funds_time)*256/(INSUFFICIENT_FUNDS_TIME_MAX/5)));
                    insufficient_funds_text.setFillColor(sf::Color(255,255,255,(INSUFFICIENT_FUNDS_TIME_MAX-insufficient_funds_time)*256/(INSUFFICIENT_FUNDS_TIME_MAX/5)));
                    insufficient_funds_text.setOutlineColor(sf::Color(0,0,0,(INSUFFICIENT_FUNDS_TIME_MAX-insufficient_funds_time)*256/(INSUFFICIENT_FUNDS_TIME_MAX/5)));
                }
                else if(insufficient_funds_time > INSUFFICIENT_FUNDS_TIME_MAX*7/10){
                    insufficient_funds_rect.setFillColor(sf::Color(0,102,255));
                    insufficient_funds_rect.setOutlineColor(sf::Color::Black);
                    insufficient_funds_text.setFillColor(sf::Color::White);
                    insufficient_funds_text.setOutlineColor(sf::Color::Black);
                }

                window.draw(insufficient_funds_rect);
                window.draw(insufficient_funds_text);
                insufficient_funds_time--;
                // cout << "insufficient_funds_time = " << insufficient_funds_time << endl;
            }
            for(int i=0; i<12; i++){
                if(info_time[i] > 0){
                    window.draw(info_rect[i]);
                    if(info_time[i] >= 20){
                        window.draw(info_text);
                        window.draw(building_name_text);
                    }
                }
            }

            //window.draw(insufficient_funds_rect);
            // DrawText(building_name_text, isLeft && !LeftClick, isRight && !RightClick, isUp && !UpArrow, isDown && !DownArrow);
            // buttons location: (110*location,691) scale: 0.104
            balance_text.setString("Cash: " + to_string(balance));
            score_text.setString("Score: " + to_string(score));
            day_text.setString("Day: " + to_string(day));
            window.draw(balance_text);
            window.draw(score_text);
            window.draw(day_text);
            //available_vendor_locations++;
            //if((available_vendor_locations-7)%100==0) cout << available_vendor_locations << endl;
            //DrawCircle(build_shadow[0], false, false);
                //window.draw(bottom_right_buttons[0][0]);
            //DrawRect(info_rect[8], false, false);

        }
        // for(Building a : buildings) window.draw(a.sprite);

        //DrawRect(orange_gradient_rect[0], isLeft && !LeftClick, isRight && !RightClick);
        //Draw(customers[0].sprite);
        // DrawText(orange_gradient_text[0], false, false, isUp && !UpArrow, isDown && !DownArrow);
        // cout << moused << endl;

        //if(phase==0)
        if(isUp && !UpArrow){
            fps+=5;
            cout << "fps=" << fps << endl;
        }
        if(isDown && !DownArrow){
            fps-=5;
            cout << "fps=" << fps << endl;
        }
        INSUFFICIENT_FUNDS_TIME_MAX = (int)(1.75*(float)fps);
            window.draw(cursor);
        window.setMouseCursorVisible(false);
        window.setFramerateLimit(fps);
        window.display();
        LeftClick = isLeft;
        RightClick = isRight;
        UpArrow = isUp;
        DownArrow = isDown;
    }

    return 0;
}
