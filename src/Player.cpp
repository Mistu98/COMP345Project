// COMP 345 - Project
// Part 1
//
// Sophie Renaud       -  40132563
// Joey Abou-Chaker    -  40055551
// Jordan Goulet       -  40075688
// Xavier Knoll        -  40132134
// Sébastien Champoux  -  40133449
//
// Based on the 'https://www.warzone.com/' game.

#include <algorithm>
#include "Player.h"

Player::Player()
    : gameEngine(), listOfOrders(), phase(Phase::None), reinforcementPool(0), playerStrategy(new NeutralPlayerStrategy()) {}

Player::Player(GameEngine* gameEngine)
    : gameEngine(gameEngine),
      ownedTerritories(std::vector<Territory*>(0)),
      handOfCards(std::vector<Card*>(0)),
      listOfOrders(new OrdersList()),
      reinforcementPool(0),
      phase(Phase::None),
      playerStrategy(new NeutralPlayerStrategy()) {}
// Parametric constructor
Player::Player(GameEngine* gameEngine, std::vector<Territory*> terr,
               int numberOfArmies)
    : gameEngine(gameEngine),
      handOfCards(std::vector<Card*>(0)),
      reinforcementPool(numberOfArmies),
      listOfOrders(new OrdersList()),
      phase(Phase::None), 
      playerStrategy(new NeutralPlayerStrategy()) {
  for (Territory* t : terr) this->AddTerritoryToPlayer(t);
}

// Copy constructor
Player::Player(const Player& pCopy) {
  gameEngine = pCopy.gameEngine;
  for (Territory* t : pCopy.ownedTerritories) this->AddTerritoryToPlayer(t);
  handOfCards = pCopy.handOfCards;
  listOfOrders = pCopy.listOfOrders;
  reinforcementPool = pCopy.reinforcementPool;
  playerStrategy = pCopy.playerStrategy;
}

// Assignment operator
Player& Player::operator=(const Player& rightP) {
  if (&rightP != this) {
    for (Territory* t : rightP.ownedTerritories) this->AddTerritoryToPlayer(t);
    handOfCards = rightP.handOfCards;
    listOfOrders = rightP.listOfOrders;
    reinforcementPool = rightP.reinforcementPool;
    playerStrategy = rightP.playerStrategy;
  }
  return *this;
}
// Destructor
Player::~Player() {
  for (auto terr : ownedTerritories) {
    // delete terr;
    // The territories should not be deleted here, only the pointers to them.
    terr = NULL;
  }
  ownedTerritories.clear();
  for (auto cards : handOfCards) {
    delete cards;
    cards = NULL;
  }
  handOfCards.clear();
  delete listOfOrders;
  listOfOrders = nullptr;
  delete playerStrategy;
  playerStrategy = nullptr;
}

// Stream insertion operator
std::ostream& operator<<(std::ostream& out, const Player& toOutput) {
  out << "Player has " << toOutput.ownedTerritories.size() << " territories, "
      << toOutput.handOfCards.size() << " cards, " << toOutput.reinforcementPool
      << " armies in its reinforcement pool and " << *(toOutput.listOfOrders);
  return out;
}

/*
void Player::GenerateToDefend() {
  toDefend = ownedTerritories;
  std::random_shuffle(toDefend.begin(), toDefend.end());
}
*/

/*
void Player::GenerateToAttack() {
  const std::vector<Territory*>* const vectorAllTerritories =
      gameEngine->GetMap()->GetTerritories();
  std::vector<Territory*> territoriesToAttack;
  Territory* territory;

  for (int i = 0; i < vectorAllTerritories->size(); i++) {
    territory = vectorAllTerritories->at(i);
    if (territory->GetPlayer() != this) {
      if (territory->IsNeighborTo(this)) {
        territoriesToAttack.push_back(territory);
      }
    }
  }
  std::random_shuffle(territoriesToAttack.begin(), territoriesToAttack.end());
  toAttack = territoriesToAttack;
}
*/

std::vector<Territory*> Player::ToAttack() {
  return playerStrategy->toAttack();
}

// Returns a vector of pointers of territories to defend
std::vector<Territory*> Player::ToDefend() {
    return playerStrategy->toDefend();
}

/*
Automatically creates an order and adds it to the player's list of orders.
*/
bool Player::IssueOrder() {
  // IDK what we're gonna do with this or if we can recycle some of it so I'll
  // just leave it there for now

  // phase = Phase::IssueOrders;
  // this->Notify();
  // if (reinforcementPool > 0 && ToDefend().size() > 0) {
  //  Territory* territory = ToDefend().at(rand() % ToDefend().size());
  //  territory->IncreaseToDeploy(1);
  //  AddOrderToPlayer(new Deploy(this, territory, 1));
  //  reinforcementPool--;
  //  return true;
  //}
  // if (handOfCards.size() > 0) {
  //  Order* order = handOfCards.back()->play();
  //  ConfigureOrdersVisitor configurator(this);
  //  order->setPlayer(this);
  //  order->acceptVisitor(&configurator);
  //  AddOrderToPlayer(order);
  //  handOfCards.pop_back();
  //  return true;
  //}
  // while (toAttack.size() > 0) {
  //  int troops;
  //  for (Territory* t : *ToAttack().back()->GetNeighbors()) {
  //    troops = t->GetAvailableTroops();
  //    if ((troops > 0) && (t->GetPlayer() == this)) {
  //      t->IncreaseStandByTroops(troops);
  //      AddOrderToPlayer(new Advance(this, t, ToAttack().back(), troops));
  //      return true;
  //    }
  //  }
  //  toAttack.pop_back();
  //}
  // while (toDefend.size() > 0) {
  //  int troops;
  //  for (Territory* t : *ToDefend().back()->GetNeighbors()) {
  //    troops = t->GetAvailableTroops();
  //    if ((troops > 0) && (t->GetPlayer() == this)) {
  //      t->IncreaseStandByTroops(troops);
  //      AddOrderToPlayer(new Advance(this, t, ToDefend().back(), troops));
  //      return true;
  //    }
  //  }
  //  toDefend.pop_back();
  //}
  // phase = Phase::None;
  this->phase = Phase::IssueOrders;
  this->Notify();
  bool notFinished = playerStrategy->issueOrder();
  phase = Phase::None;
  return notFinished;
}

// Adds the given territory pointer to the vector of owned territories
void Player::AddTerritoryToPlayer(Territory* territoryToAdd) {
  std::vector<Territory*>::iterator it = std::find(
      ownedTerritories.begin(), ownedTerritories.end(), territoryToAdd);
  if (it == ownedTerritories.end()) {
    ownedTerritories.push_back(territoryToAdd);
    territoryToAdd->SetPlayer(this);
    Notify();
  }
}

void Player::RemoveTerritoryFromPlayer(Territory* territoryToRemove) {
  std::vector<Territory*>::iterator it = std::find(
      ownedTerritories.begin(), ownedTerritories.end(), territoryToRemove);
  if (it != ownedTerritories.end()) {
    ownedTerritories.erase(it);
    territoryToRemove->SetPlayer(nullptr);
    Notify();
  }
}
// Adds the given card pointer to the vector of cards
void Player::AddCardToPlayer(Card* cardToAdd) {
  handOfCards.push_back(cardToAdd);
}
// Adds the given order pointer to the vector of orders
void Player::AddOrderToPlayer(Order* orderToAdd) {
  listOfOrders->queue(orderToAdd);
}

void Player::AddArmiesToReinforcementPool(int numberOfArmies) {
  // this->NotifyReinforcements(numberOfArmies);
  phase = Phase::Reinforcement;
  this->Notify();

  reinforcementPool += std::max(0, numberOfArmies);
  phase = Phase::None;
}

int Player::GetReinforcementPoolCount() { return reinforcementPool; }

// Removes a certain number of armies from the pool
// Returns the number of armies removed, which may be different
// (i.e. if less armies left than was asked for)
int Player::TakeArmiesFromReinforcementPool(int requestedNumberOfArmies) {
  int toReturn{std::min(requestedNumberOfArmies, reinforcementPool)};
  reinforcementPool =
      std::max(0, (reinforcementPool - requestedNumberOfArmies));
  return toReturn;
}

const std::vector<Territory*>* Player::GetOwnedTerritories() {
  return &ownedTerritories;
}

const std::vector<Territory*> Player::GetAdjacentTerritories() {
    std::cout << "fetching adj." << std::endl;
    const std::vector<Territory*> allTerritories =
        *gameEngine->GetMap()->GetTerritories();
    std::vector<Territory*> territories;

    for (Territory* t : allTerritories) {
        if (t->GetPlayer() != this) {
            if (t->IsNeighborTo(this)) {
                territories.push_back(t);
            }
        }
    }
    std::cout << "returning adj." << std::endl;
    return territories;
}

void Player::SetReinforcementPool(int amount) {
  reinforcementPool = std::max(0, amount);
}

/*
Executes the highest priority order as returned by popNextOrder()
*/
bool Player::ExecuteNextOrder() {
  phase = Phase::ExecuteOrders;
  this->Notify();
  Order* order = listOfOrders->popNextOrder();
  if (order == NULL) {
    return false;
  }
  order->execute();
  delete order;
  phase = Phase::None;
  if (listOfOrders->getListSize() > 0) {
    return true;
  }
  return false;
}

void Player::DrawCard() { gameEngine->PlayerDrawCard(this); }
Phase Player::GetPhase() { return phase; }
OrdersList* Player::GetOrdersList() { return listOfOrders; }
GameEngine* Player::GetGameEngine() { return this->gameEngine; }

void Player::SetPlayerStrategy(PlayerStrategy* strategy) {
  delete playerStrategy;  // Current player strategy becomes useless
  strategy->setPlayer(this);
  playerStrategy = strategy;
}

const std::vector<Card*>* const Player::GetHand() const {
    return &handOfCards;
}

// TODO - We need to implement different card playing behaviors for different strategies. 
void Player::PlayCard(int index) {
    if (index < handOfCards.size() && index >= 0) {
        Order* order = handOfCards.at(index)->play();
        ConfigureOrdersVisitor configurator(this);
        order->setPlayer(this);
        order->acceptVisitor(&configurator);
        AddOrderToPlayer(order);
        handOfCards.erase(handOfCards.begin() + index);
    }
}