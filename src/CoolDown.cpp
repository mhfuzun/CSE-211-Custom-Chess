#include "CoolDown.hpp"

#include "Types.hpp"
#include "Portal.hpp"
#include "ConfigReader.hpp"
#include "assert_utils.hpp"

#include <iostream>

CoolDown::CoolDown(ChessBoard &_board) : board(_board) {

}

CoolDown::~CoolDown() {

}

void CoolDown::iterateCooldown() {
    // std::cout << "[CoolDown]: iterate" << std::endl;
    if (!coolQueue.empty()) {
        int portalID = coolQueue.front();
        Portal* portal = board.getPortalWithID(portalID);
        if (portal != nullptr) {
            std::cout << "[CoolDown]: check.." << portalID << std::endl;
            if (portal->iterateCoolDown()) {
                std::cout << "[CoolDown]: pop: " << portalID << std::endl;
                coolQueue.pop();
            }
        }
    }
}

void CoolDown::pushCooldown(int portalID) {
    std::cout << "[CoolDown]: push: " << portalID << std::endl;
    Portal* portal = board.getPortalWithID(portalID);
    if (portal != nullptr) {
        portal->startCoolDown();
        coolQueue.push(portalID);
    }
    
}

void CoolDown::dumpCoolDown( void ) {
    std::cout << "[CoolDown]:" << std::endl;
    int size = coolQueue.size();
    while (size--) {
        int portalID = coolQueue.front();
        Portal* portal = board.getPortalWithID(portalID);
        if (portal != nullptr) {
            std::cout << "  portalID: " << portalID << ", cooldown: " << portal->getCoolDown() << std::endl; 
        } else {
            std::cout << "  portal could not founded!, portalID: " << portalID << std::endl;
        }
        coolQueue.pop();
        coolQueue.push(portalID);
    }
}
