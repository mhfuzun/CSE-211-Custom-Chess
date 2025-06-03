#include "CoolDown.hpp"

#include "Types.hpp"
#include "Portal.hpp"
#include "ConfigReader.hpp"
#include "assert_utils.hpp"

CoolDown::CoolDown() {

}

CoolDown::~CoolDown() {

}

void CoolDown::iterateCooldown() {
    if (!coolQueue.empty()) {
        if (coolQueue.front()->iterateCoolDown()) {
            coolQueue.pop();
        }
    }
}

void CoolDown::pushCooldown(Portal *portal) {
    ASSERT_MSG((portal != nullptr), "invalid portal data!");

    coolQueue.push(portal);
}
