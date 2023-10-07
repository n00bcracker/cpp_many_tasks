#pragma once

#include "dungeon.h"

#include <stdexcept>
#include <queue>
#include <list>
#include <vector>
#include <set>
#include <cstddef>

const Room* FindFinalRoom(const Room* starting_room) {
    std::queue<const Room*> rooms;
    std::set<const Room*> visited_rooms;
    std::list<Door*> doors;
    std::vector<const std::string*> keys;

    rooms.push(starting_room);
    visited_rooms.emplace(starting_room);

    while (!rooms.empty()) {
        const Room& current_room = *rooms.front();
        rooms.pop();

        if (current_room.IsFinal()) {
            return &current_room;
        }

        for (size_t i = 0; i < current_room.NumKeys(); ++i) {
            keys.push_back(&current_room.GetKey(i));
        }

        for (size_t i = 0; i < current_room.NumDoors(); ++i) {
            doors.push_back(current_room.GetDoor(i));
        }

        for (auto iter = doors.begin(); iter != doors.end();) {
            Door& current_door = **iter;
            if (current_door.IsOpen()) {
                const Room* opened_room = current_door.GoThrough();
                if (!visited_rooms.contains(opened_room)) {
                    rooms.push(opened_room);
                    visited_rooms.emplace(opened_room);
                }
                iter = doors.erase(iter);
            } else {
                for (const auto key_pointer : keys) {
                    if (current_door.TryOpen(*key_pointer)) {
                        const Room* opened_room = current_door.GoThrough();
                        if (!visited_rooms.contains(opened_room)) {
                            rooms.push(opened_room);
                            visited_rooms.emplace(opened_room);
                        }
                        iter = doors.erase(iter);
                        break;
                    }
                }

                ++iter;
            }
        }
    }

    return nullptr;
}
