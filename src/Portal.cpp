#include "Portal.hpp"

#include "Types.hpp"
#include "ConfigReader.hpp"

Portal::Portal () {

}

Portal::~Portal () {

}


void Portal::initPortal(int pid, PortalDirection pdirection, PortalConfig portalConf) {
    id = pid;
    direction = pdirection;
    cooldown = portalConf.properties.cooldown;
    cooldownCounter = 0;
    preserveDirection = portalConf.properties.preserve_direction;
    entryPosition.x = portalConf.positions.entry.x;
    entryPosition.y = portalConf.positions.entry.y;
    exitPosition.x = portalConf.positions.exit.x;
    exitPosition.y = portalConf.positions.exit.y;

    for (const auto &color : portalConf.properties.allowed_colors) {
        if (color == "white") {
            allowedColors.push_back(Types::Color::WHITE);
        } else if (color == "black") {
            allowedColors.push_back(Types::Color::BLACK);
        } else {
            allowedColors.push_back(Types::Color::EMPTY);
        }
    }
}

int Portal::getPortalID() {
    return id;
}

Portal::PortalDirection Portal::getPortalDirection() {
    return direction;
}

bool Portal::isValidPortal( void ) {
    return (cooldownCounter == 0);
}

Types::Position Portal::getEntryPosition( void ) {
    if (isValidPortal()) {
        return entryPosition;
    } else {
        return  {-1, -1};
    }
}

Types::Position Portal::getExitPosition( void ) {
    if (isValidPortal()) {
        return exitPosition;
    } else {
        return  {-1, -1};
    }
}

void Portal::startCoolDown( void ) {
    cooldownCounter = cooldown;
}

bool Portal::iterateCoolDown( void ) {
    cooldownCounter--;
    if (cooldownCounter <= 0) {
        cooldownCounter = 0;

        return true;
    } else {
        return false;
    }
}

int Portal::getCoolDown( void ) {
    return cooldownCounter;
}

bool Portal::validatePortalUse(Types::Piece piece) {
    if (!isValidPortal()) return false;

    for (const auto& allowedColor : allowedColors) {
        if (piece.color == allowedColor) {
            return true;
        }
    }
    
    return false;
}
