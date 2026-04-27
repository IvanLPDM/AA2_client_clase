#include "Client.h"
#include <iostream>

Client::Client() 
{ 
    std::srand(std::time(nullptr)); 
    _p2pPort = 60000 + rand() % 1000; 
}

Client::~Client()
{
    DisconnectFromBootstrapServer();

    for (auto& peer : _peers)
    {
        if (peer.socket)
        {
            peer.socket->disconnect();
            delete peer.socket;
        }
    }
    _peers.clear();
}



#pragma region BootstrapServer



bool Client::ConnectToBootstrapServer(const std::string& ip, unsigned short port)
{
    auto resolved = sf::IpAddress::resolve(ip);

    if (_bootstrapSocket.connect(*resolved, port) != sf::Socket::Status::Done)
    {
        std::cout << "[Client] Failed to connect to bootstrap server." << std::endl;
        return false;
    }

    _bootstrapSocket.setBlocking(false);
    _selector.add(_bootstrapSocket);

    std::cout << "[Client] Connected to bootstrap server ip " + ip << std::endl;
    return true;
}

void Client::DisconnectFromBootstrapServer()
{
    _selector.remove(_bootstrapSocket);
    _bootstrapSocket.disconnect();
    std::cout << "[Client] Disconnected from bootstrap server." << std::endl;
}

bool Client::SendLogin(const std::string& username, const std::string& password)
{
    std::cout << "[Client] Login sended with user " << username << " and password " << password << std::endl;
    sf::Packet packet;
    packet << "LOGIN" << username << password;
    return _bootstrapSocket.send(packet) == sf::Socket::Status::Done;
}

bool Client::SendRegister(const std::string& username, const std::string& password)
{
    std::cout << "[Client] Register sended with user " << username << " and password " << password << std::endl;

    sf::Packet packet;
    packet << "REGISTER" << username << password;
    return _bootstrapSocket.send(packet) == sf::Socket::Status::Done;
}

// Enviar al servidor para crear room con ip

bool Client::CreateRoom(std::string roomID)
{
    std::cout << "[Client] Create Room request with id " << roomID << std::endl;

    sf::Packet packet;
    packet << "CREATE_ROOM" << roomID << _p2pPort;
    return _bootstrapSocket.send(packet) == sf::Socket::Status::Done;
}

// Enviar al servidor para unirse a room con ip

bool Client::JoinRoom(std::string roomId)
{
    std::cout << "[Client] Join Room request with id " << roomId << std::endl;
    _roomID = roomId;

    sf::Packet packet;
    packet << "JOIN_ROOM" << roomId << _p2pPort;
    return _bootstrapSocket.send(packet) == sf::Socket::Status::Done;
}

std::optional<sf::Packet> Client::WaitForServerMessage(float timeoutSeconds)
{
    if (_selector.wait(sf::seconds(timeoutSeconds)) && _selector.isReady(_bootstrapSocket))
    {
        sf::Packet packet;
        if (_bootstrapSocket.receive(packet) == sf::Socket::Status::Done)
        {
            return packet;
        }
    }
    return std::nullopt;
}

bool Client::ReceivePacketFromServer(sf::Packet& packet)
{
    return _bootstrapSocket.receive(packet) == sf::Socket::Status::Done;
}

// comprueba si hay mensajes para leer

std::optional<sf::Packet> Client::CheckServerMessage()
{
    sf::Packet packet;
    if (_bootstrapSocket.receive(packet) == sf::Socket::Status::Done)
        return packet;
    return std::nullopt;
}

//Gestion del mensaje del servidor
void Client::HandleServerMessages(Event<> OnStartMatch)
{
    auto packetOpt = CheckServerMessage();
    if (!packetOpt.has_value()) return;

    sf::Packet packet = packetOpt.value();
    std::string cmd;
    packet >> cmd;

    if (cmd == "CREATE_OK")
    {
        std::string roomID;
        packet >> roomID;
        _roomID = roomID;
        std::cout << "[Client] Room created with ID: " << roomID << '\n';
    }
    else if (cmd == "CREATE_FAIL")
    {
        std::cout << "[Client] Room creation failed: ID already exists.\n";
    }
    else if (cmd == "ROOM_UPDATE")
    {
        int count, maxP;
        packet >> count >> maxP;
        _roomPlayerCount = count;
        _roomMaxPlayers  = maxP;
        std::cout << "[Client] Room update: " << count << "/" << maxP << '\n';
    }
    else if (cmd == "JOIN_OK")
    {
        int numPeers;
        packet >> numPeers;
        for (int i = 0; i < numPeers; ++i)
        {
            std::string ipStr;
            unsigned short port;
            packet >> ipStr >> port;

            auto resolved = sf::IpAddress::resolve(ipStr);
            if (resolved.has_value())
            {
                ConnectToPeer(resolved.value(), port);
            }
            else
            {
                std::cout << "[Client] Failed to resolve peer IP: " << ipStr << std::endl;
            }
        }
    }
    else if (cmd == "NEW_PEER")
    {
        std::string ipStr;
        unsigned short port;
        packet >> ipStr >> port;

        auto resolved = sf::IpAddress::resolve(ipStr);
        if (resolved.has_value())
        {
            ConnectToPeer(resolved.value(), port);
            std::cout << "[Client] Connected to new peer announced by server: " << ipStr << ":" << port << std::endl;

        }
        else
        {
            std::cout << "[Client] Failed to resolve new peer IP: " << ipStr << std::endl;
        }
    }
    else if (cmd == "JOIN_FAIL")
    {
        _roomID = "";
        _roomPlayerCount = 0;
        std::cout << "[Client] Failed to join room (JOIN_FAIL)." << std::endl;
    }

    else if (cmd == "RANKING_DATA")
    {
        _rankingEntries.clear();
        int numEntries;
        packet >> numEntries;
        for (int i = 0; i < numEntries; ++i)
        {
            RankingEntry entry;
            packet >> entry.rank >> entry.nickname >> entry.points >> entry.wins >> entry.losses;
            _rankingEntries.push_back(entry);
        }
        
        int hasPlayerRow;
        packet >> hasPlayerRow;
        if (hasPlayerRow)
        {
            RankingEntry playerEntry;
            packet >> playerEntry.rank >> playerEntry.nickname >> playerEntry.points >> playerEntry.wins >> playerEntry.losses;
            _rankingEntries.push_back(playerEntry);
        }
        _hasRankingData = true;
        std::cout << "[Client] Ranking data received: " << numEntries << " entries.\n";
    }
    else if (cmd == "START_P2P")
    {
        int playerIndex;
        int numPlayers;
        packet >> playerIndex >> numPlayers;

        std::cout << "[Client] Match starting! Player index: " << playerIndex << ", total players: " << numPlayers << std::endl;

        _playerIndex = playerIndex;
        _numPlayers = numPlayers;
        DisconnectFromBootstrapServer();
        OnStartMatch.Invoke();
    }
}

#pragma endregion

#pragma region P2P

void Client::ClearPeers()
{
    for (auto& peer : _peers)
    {
        if (peer.socket)
        {
            peer.socket->disconnect();
            delete peer.socket;
        }
    }
    _peers.clear();
}

std::optional<sf::Packet> Client::WaitForPeerMessage(float timeoutSeconds)
{
    if (timeoutSeconds > 0.f)
        _selector.wait(sf::seconds(timeoutSeconds));

    for (auto it = _peers.begin(); it != _peers.end(); )
    {
        auto& [socket, ip, port, peerIdx] = *it;

        
        bool shouldCheck = (timeoutSeconds > 0.f) ? _selector.isReady(*socket) : true;

        if (shouldCheck)
        {
            sf::Packet packet;
            sf::Socket::Status status = socket->receive(packet);

            if (status == sf::Socket::Status::Done)
            {
                _lastSenderIp   = ip;
                _lastSenderPort = port;
                std::cout << "[Client] Packet received from peer " << ip << ":" << port << std::endl;
                return packet;
            }
            else if (status == sf::Socket::Status::Disconnected || status == sf::Socket::Status::Error)
            {
                std::cout << "[Client] Peer disconnected: " << ip << ":" << port << std::endl;
                _selector.remove(*socket);
                delete socket;

                // Synthesize a PLAYER_DISCONNECTED packet so the game can react
                if (peerIdx != -1)
                {
                    sf::Packet synthPacket;
                    synthPacket << static_cast<int>(MessageType::PLAYER_DISCONNECTED) << peerIdx;
                    it = _peers.erase(it);
                    return synthPacket;
                }

                it = _peers.erase(it);
                continue;
            }
        }

        ++it;
    }

    return std::nullopt;
}

void Client::StartListeningForPeers()
{
    if (_isListeningForPeers) return;

    if (_p2pListener.listen(_p2pPort) != sf::Socket::Status::Done)
    {
        std::cerr << "[Client] Failed to listen on P2P port " << _p2pPort << std::endl;
    }
    else
    {
        _p2pListener.setBlocking(false);
        _isListeningForPeers = true;
        std::cout << "[Client] Listening for P2P on port " << _p2pPort << std::endl;
    }
}

void Client::ConnectToPeer(const sf::IpAddress& ip, unsigned short port)
{
    if (ip == sf::IpAddress::getLocalAddress() && port == _p2pPort)
    {
        std::cout << "[Client] Skipping self-connection to " << ip << ":" << port << std::endl;
        return;
    }

    for (const auto& peer : _peers)
    {
        if (peer.ip == ip && peer.port == port)
        {
            std::cout << "[Client] Already connected to peer: " << ip << ":" << port << std::endl;
            return;
        }
    }

    std::cout << "[Client] Trying to connect to peer: " << ip << ":" << port << std::endl;

    sf::TcpSocket* peerSocket = new sf::TcpSocket();

    if (peerSocket->connect(ip, port) == sf::Socket::Status::Done)
    {
        peerSocket->setBlocking(false);
        _peers.emplace_back(peerSocket, ip, port);
        _selector.add(*peerSocket);
        std::cout << "[Client] Connected to peer: " << ip << ":" << port << std::endl;
    }
    else
    {
        std::cout << "[Client] Failed to connect to peer: " << ip << ":" << port << std::endl;
        delete peerSocket;
    }
}

void Client::BroadcastToPeers(sf::Packet& packet)
{
    std::cout << "[Client] Trying to broadcast to " << _peers.size() << " peer(s)." << std::endl;

    for (auto& [socket, ip, port, peerIdx] : _peers)
    {
        if (socket->send(packet) == sf::Socket::Status::Done)
        {
            std::cout << "[Client] Packet sent to peer " << ip << ":" << port << std::endl;
        }
        else
        {
            std::cout << "[Client] Failed to send packet to " << ip << ":" << port << std::endl;
        }
    }
}


void Client::UpdateP2PConnections()
{
    if (!_isListeningForPeers) return;

    sf::TcpSocket* newPeer = new sf::TcpSocket();
    if (_p2pListener.accept(*newPeer) == sf::Socket::Status::Done)
    {
        newPeer->setBlocking(false);
        auto optionalIp = newPeer->getRemoteAddress();
        unsigned short port = newPeer->getRemotePort();
        if (optionalIp.has_value())
        {
            sf::IpAddress ip = optionalIp.value();

            for (const auto& peer : _peers)
            {
                if (peer.ip == ip && peer.port == port)
                {
                    std::cout << "[Client] Peer already connected: " << ip << ":" << port << std::endl;
                    delete newPeer;
                    return;
                }
            }

            _peers.emplace_back(newPeer, ip, port);
            _selector.add(*newPeer);
            std::cout << "[Client] New peer connected: " << ip << ":" << port << std::endl;
        }
        else
        {
            std::cout << "[Client] New peer connected but no valid IP." << std::endl;
            delete newPeer;
        }
    }
    else
    {
        delete newPeer;
    }
}

void Client::SendUsername()
{
    sf::Packet packet;
    packet << static_cast<int>(MessageType::PLAYER_PROFILE)
        << _playerIndex
        << _playerUsername;

    BroadcastToPeers(packet);

    std::cout << "[Client] Sent username '" << _playerUsername << "' as playerIndex " << _playerIndex << " to all peers";

}

bool Client::ReceivePacketFromPeers(sf::Packet& packet)
{
    _selector.wait(sf::Time::Zero);

    for (auto it = _peers.begin(); it != _peers.end(); )
    {
        auto& [socket, ip, port, peerIdx] = *it;

        if (_selector.isReady(*socket))
        {
            sf::Packet temp;
            sf::Socket::Status status = socket->receive(temp);

            if (status == sf::Socket::Status::Done)
            {
                packet = temp;
                std::cout << "[Client] Packet received from peer " << ip << ":" << port << std::endl;
                return true;
            }
            else if (status == sf::Socket::Status::Disconnected || status == sf::Socket::Status::Error)
            {
                std::cout << "[Client] Peer disconnected: " << ip << ":" << port << std::endl;
                _selector.remove(*socket);
                delete socket;
                it = _peers.erase(it);
                continue;
            }
        }

        ++it;
    }

    return false;
}

#pragma endregion

#pragma region Getters & Setters

    void Client::SetPlayerIndex(int index) { _playerIndex = index; }

    void Client::SetNumPlayers(int num) { _numPlayers = num; }

    void Client::SetMyUsername(const std::string& name) { _playerUsername = name; }
    std::string Client::GetMyUsername() const { return _playerUsername; }

    int Client::GetPlayerIndex() const { return _playerIndex; }
    int Client::GetNumPlayers()  const { return _numPlayers; }

    sf::SocketSelector Client::GetSelector() { return _selector; }

    std::optional<sf::IpAddress> Client::GetLastSenderIp() const { return _lastSenderIp; }
    unsigned short Client::GetLastSenderPort()  const { return _lastSenderPort; }
    std::string Client::GetRoomID() const { return _roomID; }
    int Client::GetRoomPlayerCount() const { return _roomPlayerCount; }
    int Client::GetRoomMaxPlayers()  const { return _roomMaxPlayers; }

    void Client::SetPeerPlayerIndexByAddr(const sf::IpAddress& ip, unsigned short port, int index)
    {
        for (auto& peer : _peers)
        {
            if (peer.ip == ip && peer.port == port)
            {
                peer.playerIndex = index;
                return;
            }
        }
    }

    void Client::StoreGameResults(const std::vector<GameResultEntry>& results)
    {
        _pendingGameResults = results;
        _hasPendingGameResult  = true;
        std::cout << "[Client] Game results stored for " << results.size() << " players.\n";
    }

    void Client::SendStoredGameResult()
    {
        if (!_hasPendingGameResult) return;

        sf::Packet packet;
        packet << "GAME_RESULT" << static_cast<int>(_pendingGameResults.size());
        for (const auto& entry : _pendingGameResults)
            packet << entry.nickname << entry.finishPosition;

        if (_bootstrapSocket.send(packet) == sf::Socket::Status::Done)
        {
            std::cout << "[Client] GAME_RESULT sent.\n";
            _hasPendingGameResult = false;
            _pendingGameResults.clear();
        }
    }

    bool Client::HasPendingGameResult() const { return _hasPendingGameResult; }

    const std::vector<GameResultEntry>& Client::GetPendingGameResults() const { return _pendingGameResults; }

    void Client::RequestRanking()
    {
        sf::Packet packet;
        packet << "RANKING_REQUEST" << _playerUsername;
        _bootstrapSocket.send(packet);
        std::cout << "[Client] RANKING_REQUEST sent.\n";
    }

    const std::vector<RankingEntry>& Client::GetRankingEntries() const { return _rankingEntries; }
    bool Client::HasRankingData() const { return _hasRankingData; }

    void Client::ClearRankingData() 
    { 
        _hasRankingData = false; _rankingEntries.clear(); 
    }

#pragma endregion