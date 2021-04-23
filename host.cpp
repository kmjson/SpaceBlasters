// Copyright 2021 J.Carruthers jbc@bu.edu

// socket programming using UDP and SFML
// g++ networking_send.cpp -l sfml-network -o sendmsg

#include<iostream>
#include<string>
#include<vector>
#include<thread>
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

sf::RenderWindow window(sf::VideoMode(825, 550), "Battleship", sf::Style::Titlebar | sf::Style::Close);

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

void draw() {
  window.clear();
  vector<char> main_vector = main_grid.get_grid();
  for (int i = 0; i < 100; i++) {
    sf::RectangleShape square(sf::Vector2f(50, 50));
    square.setPosition(25 + ((i % 10) * 50), 25 + ((i / 10) * 50));
    square.setOutlineThickness(2);
    square.setOutlineColor(sf::Color(171, 171, 171));
    // square.setTexture(&water);
    if (main_vector.at(i) == 'o') {
      square.setFillColor(sf::Color::White);
    }
    if (main_vector.at(i) == 'h') {
      square.setFillColor(sf::Color::Red);
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
    // square.setTexture(&water);
    if (second_vector.at(i) == 'o') {
      square.setFillColor(sf::Color::White);
    }
    if (second_vector.at(i) == 'h') {
      square.setFillColor(sf::Color::Red);
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
}

int main(int argc, char **argv) {

  waiting = true;
  created = false;
  joining = false;
  end = false;

  sf::TcpSocket socket;
  char buffer[1024];
  std::size_t received;

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

  sf::Texture water;
  if (!water.loadFromFile("water.png"))
    return -1;

  // sf::RenderWindow window(sf::VideoMode(825, 550), "Battleship", sf::Style::Titlebar | sf::Style::Close);
  while (window.isOpen()) {
    window.clear();

    sf::Vector2i position = sf::Mouse::getPosition(window);
    int x = position.x;
    int y = position.y;

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        socket.disconnect();
        window.close();
      }
      if (event.type == sf::Event::MouseButtonPressed) {
        // int x = event.mouseButton.x;
        // int y = event.mouseButton.y;
        switch (event.key.code) {
        case sf::Mouse::Left:
          cout << "Left " << x << " " << y << "\n";
          if (waiting) {
            if (x < 412) {
              waiting = false;
              created = true;
              cout << "socket!\n";
            } else {
              waiting = false;
              joining = true;
            }
          } else if (ships_placed < 5 && main_grid.on_grid(x, y)) {
            place_ship(x, y);
          } else {
            if (your_turn && main_grid.is_empty(x, y)) {
              int col = (x - 25) / 50;
              int row = (y - 25) / 50;
              int index = (row * 10) + col;
              string input = std::to_string(index);
              socket.send(input.c_str(), 3);
              socket.receive(&buffer, sizeof(buffer), received);
              if (buffer[0] == 'o') {
                main_grid.update_grid(index, 'm');
                your_turn = false;
              } else {
                main_grid.update_grid(index, 'h');
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
        case sf::Mouse::Right:
          cout << "Right " << x << " " << y << "\n";
          rotation = (rotation + 90) % 360;
          break;
        }
      }
    }

    if (end) {}

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
      sf::TcpListener listener;
      listener.listen(60002);
      listener.accept(socket);
      string text = "c";
      socket.send(text.c_str(), text.length() + 1);
      created = false;
      your_turn = true;
    }

    else if (joining) {
      // sf::IpAddress ip = sf::IpAddress::getLocalAddress();
      create_room.setPosition(250, 250);
      create_room.setString("Enter an IP Address!");
      window.draw(create_room);
      window.display();
      string ip;
      cout << "Enter here: ";
      cin >> ip;
      socket.connect(ip, 60002);
      socket.receive(&buffer, sizeof(buffer), received);
      if (buffer[0] == 'c') {
        joining = false;
        your_turn = false;
      }
    }

    else {

      if (ships_placed < 5) {
        vector<char> main_vector = main_grid.get_grid();
        for (int i = 0; i < 100; i++) {
          sf::RectangleShape square(sf::Vector2f(50, 50));
          square.setPosition(25 + ((i % 10) * 50), 25 + ((i / 10) * 50));
          square.setOutlineThickness(2);
          square.setOutlineColor(sf::Color(171, 171, 171));
          // square.setTexture(&water);
          if (main_vector.at(i) == 'o') {
            square.setFillColor(sf::Color::White);
          }
          if (main_vector.at(i) == 'h') {
            square.setFillColor(sf::Color::Red);
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
          // square.setTexture(&water);
          if (second_vector.at(i) == 'o') {
            square.setFillColor(sf::Color::White);
          }
          if (second_vector.at(i) == 'h') {
            square.setFillColor(sf::Color::Red);
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
        window.display();
      }

      else if (ships_placed == 5) {
        socket.send("r", 2);
        socket.receive(&buffer, sizeof(buffer), received);
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

      else if (!your_turn && playing) {
        socket.receive(&buffer, sizeof(buffer), received);
        int index = stoi(buffer);
        cout << index << "\n";
        if (second_grid.get_on_grid(index) == 'o') {
          string hit = "o";
          socket.send(hit.c_str(), 2);
          second_grid.update_grid(index, 'm');
          your_turn = true;
        } else {
          string hit = "h";
          socket.send(hit.c_str(), 2);
          second_grid.update_grid(index, 'h');
          your_hp--;
          if (your_hp <= 0) {
            end = true;
          }
        }
        draw();
        window.display();
      }

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
  return 0;
}
