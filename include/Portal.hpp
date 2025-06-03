#pragma once

#include "Types.hpp"
#include "ConfigReader.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

class Portal {
    public:
        enum class PortalDirection {IN, OUT};

        Portal ();

        ~Portal ();

        void initPortal(int pid, PortalDirection pdirection, PortalConfig portalConf);

        int getPortalID();
        PortalDirection getPortalDirection();
        bool isValidPortal( void );
        Types::Position getEntryPosition( void );
        Types::Position getExitPosition( void );
        int getCoolDown( void );

        void startCoolDown( void );
        bool iterateCoolDown( void );

    private:
        int id;
        int cooldown;
        int cooldownCounter;
        std::vector<Types::Color> allowedColors;
        bool preserveDirection;
        PortalDirection direction;
        Types::Position exitPosition;
};
