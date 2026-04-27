#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <string>
#include "Event.h"
#include "Enums.hpp"

struct GameResultEntry
{
    std::string nickname;
    int finishPosition;
    bool isDisconected = false;
};

struct RankingEntry
{
    int rank;
    std::string nickname;
    int points;
    int wins;
    int losses;
};

struct PeerInfo
{
    sf::TcpSocket* socket = nullptr;
    sf::IpAddress ip;
    unsigned short port = 0;
    int playerIndex = -1;

    PeerInfo() = default;
    PeerInfo(sf::TcpSocket* s, sf::IpAddress address, unsigned short p) : socket(s), ip(address), port(p), playerIndex(-1) {}
};

class Client
{
public:
    Client();
    ~Client();

    // -- BootstrapServer
    bool ConnectToBootstrapServer(const std::string& ip, unsigned short port);
    void DisconnectFromBootstrapServer();
    bool ReceivePacketFromServer(sf::Packet& packet);
    std::optional<sf::Packet> CheckServerMessage();
    void HandleServerMessages(Event<> OnStartMatch);

    // -- Matchmaking & Login
    bool SendLogin(const std::string& username, const std::string& password);
    bool SendRegister(const std::string& username, const std::string& password);
    bool CreateRoom(std::string roomID);
    bool JoinRoom(std::string roomId);
    std::optional<sf::Packet> WaitForServerMessage(float timeoutSeconds);
    void SendUsername();

    // -- P2P
    std::optional<sf::Packet> WaitForPeerMessage(float timeoutSeconds);
    void StartListeningForPeers();
    bool ReceivePacketFromPeers(sf::Packet& packet);
    void ConnectToPeer(const sf::IpAddress& ip, unsigned short port);
    void BroadcastToPeers(sf::Packet& packet);
    void UpdateP2PConnections();
    void ClearPeers();
    void SetPeerPlayerIndexByAddr(const sf::IpAddress& ip, unsigned short port, int index);

    // -- Getters & Setters
    void SetPlayerIndex(int index);
    void SetNumPlayers(int num);
    void SetMyUsername(const std::string& name);
    std::string GetMyUsername() const;
    int GetPlayerIndex() const;
    int GetNumPlayers() const;
    sf::SocketSelector GetSelector();
    std::optional<sf::IpAddress> GetLastSenderIp() const;
    unsigned short GetLastSenderPort() const;
    std::string GetRoomID() const;
    int GetRoomPlayerCount() const;
    int GetRoomMaxPlayers() const;

    // -- Ranking & game results
    void StoreGameResults(const std::vector<GameResultEntry>& results);
    void SendStoredGameResult();
    bool HasPendingGameResult() const;
    const std::vector<GameResultEntry>& GetPendingGameResults() const;
    void RequestRanking();
    const std::vector<RankingEntry>& GetRankingEntries() const;
    bool HasRankingData() const;
    void ClearRankingData();

    sf::TcpListener _p2pListener;
private:
    sf::TcpSocket _bootstrapSocket;

    int _playerIndex = -1;
    int _numPlayers = 1;

    std::string _playerUsername;
    std::string _roomID;
    int _roomPlayerCount = 0;
    int _roomMaxPlayers = 4;

    std::optional<sf::IpAddress> _lastSenderIp;
    unsigned short _lastSenderPort = 0;

    std::vector<GameResultEntry> _pendingGameResults;
    bool _hasPendingGameResult = false;

    std::vector<RankingEntry> _rankingEntries;
    bool _hasRankingData = false;

    std::vector<PeerInfo> _peers;

    unsigned short _p2pPort;
    bool _isListeningForPeers = false;

    sf::SocketSelector _selector;
};

