#include "GameManager.hpp"
#include "ConfigReader.hpp"
#include "ChessBoard.hpp"

#include <iostream>
#include <string>

GameManager::GameManager(const GameConfig &_config)
: config(_config) // Üye başlatma listesi ile referans başlatılıyor
{}

GameManager::~GameManager () {
    
}
