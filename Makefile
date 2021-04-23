# Copyright 2021 Jason Kim jasonk@bu.edu

host: host.cpp
	g++ host.cpp -o host -pthread -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network
