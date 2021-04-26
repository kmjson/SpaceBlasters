// Copyright 2021 Jason Kim jasonk@bu.edu
// Copyright 2021 Harin Lee harinlee@bu.edu
// Copyright 2021 Raymond Lim oscar338@bu.edu
// Copyright 2021 Ji Young Chung jery1106@bu.edu

#include<iostream>
#include<string>
#include<vector>
#include<SFML/Graphics.hpp>
#include<SFML/Network.hpp>
#include "grid.cpp"

using std::cout;
using std::cin;
using std::stoi;
using std::vector;
using std::string;

bool waiting;
bool created;
bool joining;
bool receiving;
bool end;

Grid main_grid;
Grid second_grid;

int ships_placed = 0;
vector<int> ship_sizes = {5, 4, 3, 3, 2};
vector<char> ships = {'C', 'B', 'c', 'S', 'D'};
int rotation = 0;
bool playing = false;
bool your_turn;
int your_hp = 17;
int opponent_hp = 17;

sf::RenderWindow window(sf::VideoMode(825, 550), "Space Blasters", sf::Style::Titlebar | sf::Style::Close);
string game_text;
sf::Text game_outcome;

sf::Texture bg;
sf::Texture hit;
sf::Sprite background;

// Function for placing ship interaction in the beginning of the game.
// Depending on the rotation.
void place_ship(int x, int y) {
  int size = ship_sizes.at(ships_placed);
  int col = (x - 25) / 50;
  int row = (y - 25) / 50;
  if (rotation == 0) {
    if (col + size <= 10) {
      for(int i = 0; i < size; i++) {
        if (!(main_grid.get_on_grid(x + (50 * i), y) == 'o')) {
          return;
        }
      }
      for(int i = 0; i < size; i++) {
        main_grid.update_grid(x + (50 * i), y, ships.at(ships_placed));
      }
      ships_placed += 1;
    }
  } else if (rotation == 90) {
    if (row + size <= 10) {
      for(int i = 0; i < size; i++) {
        if (!(main_grid.get_on_grid(x, y + (50 * i)) == 'o')) {
          return;
        }
      }
      for(int i = 0; i < size; i++) {
        main_grid.update_grid(x, y + (50 * i), ships.at(ships_placed));
      }
      ships_placed += 1;
    }
  } else if (rotation == 180) {
    if (col - size >= -1) {
      for(int i = 0; i < size; i++) {
        if (!(main_grid.get_on_grid(x - (50 * i), y) == 'o')) {
          return;
        }
      }
      for(int i = 0; i < size; i++) {
        main_grid.update_grid(x - (50 * i), y, ships.at(ships_placed));
      }
      ships_placed += 1;
    }
  } else if (rotation == 270) {
    if (row - size >= -1) {
      for(int i = 0; i < size; i++) {
        if (!(main_grid.get_on_grid(x, y - (50 * i)) == 'o')) {
          return;
        }
      }
      for(int i = 0; i < size; i++) {
        main_grid.update_grid(x, y - (50 * i), ships.at(ships_placed));
      }
      ships_placed += 1;
    }
  }
}

// Function for drawing updated grids.
// In the game, there are two grids. Big grid on the left representing enemy's field, small grid on the right representing user's field.
// The field contains 100 hitboxes and status of the hitbox will be stored as a char.
// Hitboxes have four status : Idle ('o'), Hit('h'), Miss('m') and Ships('C','B','c','S','D').
// Each status are presented with colors: White, Red, Yellow and gray.
void draw() {
  window.clear();
  window.draw(background);
  vector<char> main_vector = main_grid.get_grid();
  for (int i = 0; i < 100; i++) {
    sf::RectangleShape square(sf::Vector2f(50, 50));
    square.setPosition(25 + ((i % 10) * 50), 25 + ((i / 10) * 50));
    square.setOutlineThickness(2);
    square.setOutlineColor(sf::Color(171, 171, 171));
    if (main_vector.at(i) == 'o') {
      square.setFillColor(sf::Color::White);
    }
    if (main_vector.at(i) == 'h') {
      square.setTexture(&hit);
      square.setFillColor(sf::Color(255,100,100));
    }
    if (main_vector.at(i) == 'm') {
      square.setTexture(&hit);
      square.setFillColor(sf::Color::Yellow);
    }
    if (main_vector.at(i) == 'C' ||
        main_vector.at(i) == 'B' ||
        main_vector.at(i) == 'c' ||
        main_vector.at(i) == 'S' ||
        main_vector.at(i) == 'D') {
      square.setFillColor(sf::Color(230, 230, 230));
    }
    window.draw(square);
  }

  // Second grid creation and define the color of hitbox status.
  vector<char> second_vector = second_grid.get_grid();
  for (int i = 0; i < 100; i++) {
    sf::RectangleShape square(sf::Vector2f(25, 25));
    square.setPosition(550 + ((i % 10) * 25), 25 + ((i / 10) * 25));
    square.setOutlineThickness(2);
    square.setOutlineColor(sf::Color(171, 171, 171));
    if (second_vector.at(i) == 'o') {
      square.setFillColor(sf::Color::White);
    }
    if (second_vector.at(i) == 'h') {
      square.setTexture(&hit);
      square.setFillColor(sf::Color(255,100,100));
    }
    if (second_vector.at(i) == 'm') {
      square.setTexture(&hit);
      square.setFillColor(sf::Color::Yellow);
    }
    if (second_vector.at(i) == 'C' ||
        second_vector.at(i) == 'B' ||
        second_vector.at(i) == 'c' ||
        second_vector.at(i) == 'S' ||
        second_vector.at(i) == 'D') {
      square.setFillColor(sf::Color(230, 230, 230));
    }
    window.draw(square);
  }
  game_outcome.setString(game_text);
  window.draw(game_outcome);
}

// Checks for the close event.
void close () {
  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window.close();
      exit(0);
    }
  }
}

// Main function. Initiation of the game is described in this function.
int main(int argc, char **argv) {

  if (!bg.loadFromFile("bg.png"))
    return -1;
  background.setTexture(bg);

  if (!hit.loadFromFile("hitbox.png"))
    return -1;


  // Setting variables, loading textures and files.
  waiting = true;
  created = false;
  joining = false;
  receiving = false;
  end = false;

  sf::UdpSocket socket;
  char buffer[1024];
  std::size_t received;
  sf::IpAddress ip;
  unsigned short port;
  socket.setBlocking(false);

  main_grid.create(25, 25, 500);
  second_grid.create(550, 25, 250);

  sf::RectangleShape Carrier(sf::Vector2f(250, 50));
  sf::RectangleShape Battleship(sf::Vector2f(200, 50));
  sf::RectangleShape Cruiser(sf::Vector2f(150, 50));
  sf::RectangleShape Submarine(sf::Vector2f(150, 50));
  sf::RectangleShape Destroyer(sf::Vector2f(100, 50));

  sf::Font font;
  sf::Text create_room;
  sf::Text join_room;
  font.loadFromFile("/usr/share/fonts/truetype/ubuntu/Ubuntu-BI.ttf");
  create_room.setFont(font);
  create_room.setString("Create Room");
  create_room.setFillColor(sf::Color::White);
  join_room.setFont(font);
  join_room.setString("Join Room");
  join_room.setFillColor(sf::Color::White);
  game_outcome.setFont(font);
  game_outcome.setCharacterSize(26);
  game_outcome.setFillColor(sf::Color::White);
  game_outcome.setPosition(550, 300);

  // Game initiation
  while (window.isOpen()) {

    window.clear();

    // The user's mouse will be tracked constatnly with Postion X and Y.
    sf::Vector2i position = sf::Mouse::getPosition(window);
    int x = position.x;
    int y = position.y;

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      if (event.type == sf::Event::MouseButtonPressed) {
        switch (event.key.code) {

        // Definition of Overall Left click events.
        case sf::Mouse::Left:
          // Left click events during the create/join phase.
          if (waiting) {
            if (x < 412) {
              waiting = false;
              created = true;
              // cout << "socket!\n";
            } else {
              waiting = false;
              joining = true;
            }
          } else if (ships_placed < 5 && main_grid.on_grid(x, y)) {
            // Left click event during ship placing phase.
            place_ship(x, y);
          } else {
            // Left click event that activates only in the user's turn.
            // If !your_turn, there is no click event so user have to wait until user's turn.
            if (your_turn && main_grid.is_empty(x, y)) {
              int col = (x - 25) / 50;
              int row = (y - 25) / 50;
              int index = (row * 10) + col;
              string input = std::to_string(index);
              socket.send(input.c_str(), 3, ip, port);
              receiving = true;
              while(receiving) {
                close();
                socket.receive(&buffer, sizeof(buffer), received, ip, port);
                if (received != 0) {
                  receiving = false;
                }
              }
              if (buffer[0] == 'o') {
                main_grid.update_grid(index, 'm');
                game_text = "You Missed!";
                your_turn = false;
              } else {
                main_grid.update_grid(index, 'h');
                game_text = "You Hit a Ship!";
                opponent_hp--;
                if (opponent_hp <= 0) {
                  your_turn = false;
                  end = true;
                }
              }
              draw();
              window.display();
            }
          }
          break;

        // Overall right click events. Basically it is only for rotation when placing ship.
        case sf::Mouse::Right:
          // cout << "Right " << x << " " << y << "\n";
          rotation = (rotation + 90) % 360;
          break;
        }
      }
    }

    // The end game phase. Defeat/Victory decleration will be made.
    if (end) {
      if(your_hp == 0){
        game_text = "All of your\nships have been\ndestroyed!\nYou lose!";
      }
      if(opponent_hp == 0){
        game_text = "You have destroyed\nall of your\nopponent's ships!\nYou win!";
      }
      draw();
      window.display();
    }

    // The window starts with "waiting" value as TRUE, so This function is the first function that appears to the user.
    // Create U.I of create/join room and initiate the listener to establish the networking property.
    else if (waiting) {
      create_room.setPosition(110, 250);
      join_room.setPosition(550, 250);
      sf::RectangleShape line(sf::Vector2f(4, 550));
      line.setFillColor(sf::Color::White);
      line.setPosition(410, 0);
      window.draw(create_room);
      window.draw(join_room);
      window.draw(line);
      window.display();
    }

    else if (created) {
      create_room.setPosition(205, 250);
      create_room.setString("Waiting for someone to join!");
      window.draw(create_room);
      window.display();
      string text = "c";
      socket.bind(60001);
      while(created) {
        close();
        socket.receive(&buffer, sizeof(buffer), received, ip, port);
        if (buffer[0] == 'c') {
          socket.send(text.c_str(), text.length() + 1, ip, port);
          created = false;
          your_turn = true;
        }
      }
    }

    else if (joining) {
      create_room.setPosition(250, 250);
      create_room.setString("Enter an IP Address!");
      window.draw(create_room);
      window.display();
      string ip_input;
      cout << "Enter here: ";
      cin >> ip_input;
      ip = ip_input;
      string text = "c";
      socket.bind(60002);
      socket.send(text.c_str(), text.length() + 1, ip, 60001);
      while(joining) {
        close();
        socket.receive(&buffer, sizeof(buffer), received, ip, port);
        if (buffer[0] == 'c') {
          joining = false;
          your_turn = false;
        }
      }
    }

    else {
      // During the ship placement phase, draw U.I of the grids. Keep update until all placed.
      if (ships_placed < 5) {
        vector<char> main_vector = main_grid.get_grid();
        for (int i = 0; i < 100; i++) {
          sf::RectangleShape square(sf::Vector2f(50, 50));
          square.setPosition(25 + ((i % 10) * 50), 25 + ((i / 10) * 50));
          square.setOutlineThickness(2);
          square.setOutlineColor(sf::Color(171, 171, 171));
          if (main_vector.at(i) == 'o') {
            square.setFillColor(sf::Color::White);
          }
          if (main_vector.at(i) == 'h') {
            square.setTexture(&hit);
            square.setFillColor(sf::Color(255,100,100));
          }
          if (main_vector.at(i) == 'm') {
            square.setFillColor(sf::Color::Yellow);
          }
          if (main_vector.at(i) == 'C' ||
              main_vector.at(i) == 'B' ||
              main_vector.at(i) == 'c' ||
              main_vector.at(i) == 'S' ||
              main_vector.at(i) == 'D') {
            square.setFillColor(sf::Color(230, 230, 230));
          }
          window.draw(square);
        }

        vector<char> second_vector = second_grid.get_grid();
        for (int i = 0; i < 100; i++) {
          sf::RectangleShape square(sf::Vector2f(25, 25));
          square.setPosition(550 + ((i % 10) * 25), 25 + ((i / 10) * 25));
          square.setOutlineThickness(2);
          square.setOutlineColor(sf::Color(171, 171, 171));
          if (second_vector.at(i) == 'o') {
            square.setFillColor(sf::Color::White);
          }
          if (second_vector.at(i) == 'h') {
            square.setTexture(&hit);
            square.setFillColor(sf::Color(255,100,100));
          }
          if (second_vector.at(i) == 'm') {
            square.setFillColor(sf::Color::Yellow);
          }
          if (second_vector.at(i) == 'C' ||
              second_vector.at(i) == 'B' ||
              second_vector.at(i) == 'c' ||
              second_vector.at(i) == 'S' ||
              second_vector.at(i) == 'D') {
            square.setFillColor(sf::Color(230, 230, 230));
          }
          window.draw(square);
        }

        // During the ship placement phase, Show the possible ship position centered on the position of mouse pointer.
        if (main_grid.on_grid(x, y)) {
          int col = (x - 25) / 50;
          int row = (y - 25) / 50;
          if (ships_placed == 0) {
            Carrier.setPosition((col * 50) + 50, (row * 50) + 50);
            Carrier.setOrigin(25, 25);
            Carrier.setRotation(rotation);
            Carrier.setFillColor(sf::Color(225, 225, 225));
            window.draw(Carrier);
          } else if (ships_placed == 1) {
            Battleship.setPosition((col * 50) + 50, (row * 50) + 50);
            Battleship.setOrigin(25, 25);
            Battleship.setRotation(rotation);
            Battleship.setFillColor(sf::Color(225, 225, 225));
            window.draw(Battleship);
          } else if (ships_placed == 2) {
            Cruiser.setPosition((col * 50) + 50, (row * 50) + 50);
            Cruiser.setOrigin(25, 25);
            Cruiser.setRotation(rotation);
            Cruiser.setFillColor(sf::Color(225, 225, 225));
            window.draw(Cruiser);
          } else if (ships_placed == 3) {
            Submarine.setPosition((col * 50) + 50, (row * 50) + 50);
            Submarine.setOrigin(25, 25);
            Submarine.setRotation(rotation);
            Submarine.setFillColor(sf::Color(225, 225, 225));
            window.draw(Submarine);
          } else if (ships_placed == 4) {
            Destroyer.setPosition((col * 50) + 50, (row * 50) + 50);
            Destroyer.setOrigin(25, 25);
            Destroyer.setRotation(rotation);
            Destroyer.setFillColor(sf::Color(225, 225, 225));
            window.draw(Destroyer);
          }
        }
        game_outcome.setString("Get Ready For\nLaunch!");
        window.draw(game_outcome);
        window.display();
      }

      // When 5 ships are placed and user's field is finalized, send "r" to let enemy know the user is ready to play.
      // When "r" is recieved so enemy is also ready, pass the first grid vector into second grid and initiate "playing" status.
      else if (ships_placed == 5) {
        socket.send("r", 2, ip, port);
        socket.receive(&buffer, sizeof(buffer), received, ip, port);
        if (buffer[0] == 'r') {
          second_grid.replace_grid(main_grid.get_grid());
          if (main_grid.clear()) {
            ships_placed++;
            playing = true;
            draw();
            window.display();
          }
        }
      }

      // Logic when it is not your turn.
      // Recieve hit point from enemy, resend the result to enemy, and update the grid.
      // If ship is hit, check if the life remains and end the game if life is 0.
      // Otherwise, "your turn" status turns on.
      else if (!your_turn && playing) {
        receiving = true;
        while (receiving) {
          close();
          socket.receive(&buffer, sizeof(buffer), received, ip, port);
          if (received != 0) {
            receiving = false;
          }
        }
        int index = stoi(buffer);
        // cout << index << "\n";
        if (second_grid.get_on_grid(index) == 'o') {
          string hit = "o";
          socket.send(hit.c_str(), 2, ip, port);
          second_grid.update_grid(index, 'm');
          game_text = "Your Opponent\nMissed!";
          your_turn = true;
        } else {
          string hit = "h";
          socket.send(hit.c_str(), 2, ip, port);
          second_grid.update_grid(index, 'h');
          game_text = "Your Opponent Hit!";
          your_hp--;
          if (your_hp <= 0) {
            end = true;
          }
        }
        draw();
        window.display();
      }

      // Logic when it is your turn.
      // Create hover box on the grid for targeting. 
      else if (your_turn && playing) {
        if (main_grid.is_empty(x,y)) {
          int col = (x-25)/50;
          int row = (y-25)/50;
          draw();
          sf::RectangleShape hover(sf::Vector2f(50, 50));
          hover.setPosition((col*50)+25, (row*50)+25);
          hover.setFillColor(sf::Color(100, 100, 100));
          window.draw(hover);
          window.display();
        }
        else {
          draw();
          window.display();
        }
      }
    }
  }

  // End of main.
  return 0;
}
