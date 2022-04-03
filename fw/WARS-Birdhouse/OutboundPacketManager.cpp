/* 
 * LoRa Birdhouse Mesh Network Project
 * Wellesley Amateur Radio Society
 * 
 * Copyright (C) 2022 Bruce MacKinnon
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "OutboundPacketManager.h"

OutboundPacketManager::OutboundPacketManager(const Clock& clock, CircularBuffer& txBuffer) 
    : _clock(clock), 
      _txBuffer(txBuffer) {
}

bool OutboundPacketManager::allocateIfPossible(const Packet& packet, unsigned int packetLen,
    uint32_t giveUpTime) {
    // Look for an unallocated packet a grab it - first come, first served.
    for (unsigned int i = 0; i < _packetCount; i++) {
        if (!_packets[i].isAllocated()) {
            _packets[i].allocate(packet, packetLen, giveUpTime);
            return true;
        }
    }
    // No unallocated packets, return the failure
    return false;
}

void OutboundPacketManager::pump() {
    for (unsigned int i = 0; i < _packetCount; i++) 
        _packets[i].transmitIfReady(_clock, _txBuffer);
}

void OutboundPacketManager::processAck(const Packet& ackPacket) {
    for (unsigned int i = 0; i < _packetCount; i++) 
        _packets[i].processAckIfRelevant(ackPacket);
}

unsigned int OutboundPacketManager::getFreeCount() const {
    unsigned int r = 0;
    for (unsigned int i = 0; i < _packetCount; i++) 
        if (!_packets[i].isAllocated()) 
            r++;
    return r;
}
