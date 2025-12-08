#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <ctime>

#include "include/CoreTypes.h"
#include "include/StatusEffect.h"
#include "include/Card.h"
#include "include/Combatant.h"
#include "include/BattleManager.h"

// Factory to create cards (simulating loading from JSON)
class CardFactory {
public:
    static TSharedPtr<Card> CreateCard(FString CardID) {
        if (CardID == "strike") {
            return std::make_shared<AttackCard>("strike", "Strike", 1, 6);
        } else if (CardID == "defend") {
            return std::make_shared<BlockCard>("defend", "Defend", 1, 5);
        } else if (CardID == "bash") {
             return std::make_shared<AttackCard>("bash", "Bash", 2, 8);
        } else if (CardID == "intimidate") {
            return std::make_shared<EffectCard>("intimidate", "Intimidate", 0, ECardTarget::Enemy, std::make_shared<StatusWeak>(1));
        }
        return nullptr;
    }
};

int main() {
    std::srand(std::time(nullptr));

    std::cout << "Initializing Battle Engine..." << std::endl;

    // 1. Create Combatants
    auto Hero = std::make_shared<Player>("Ironclad");
    auto Boss = std::make_shared<Enemy>("Cultist", 20); // Reduced HP for quicker test

    // 2. Build Deck (Data-Driven-ish)
    std::vector<FString> StarterDeck = {"strike", "strike", "strike", "strike", "defend", "defend", "bash", "intimidate"};

    for (const auto& ID : StarterDeck) {
        auto NewCard = CardFactory::CreateCard(ID);
        if (NewCard) {
            Hero->DrawPile.push_back(NewCard);
        }
    }

    // Shuffle
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(Hero->DrawPile.begin(), Hero->DrawPile.end(), g);


    // 3. Setup Battle Manager
    BattleManager Battle;
    Battle.SetupBattle(Hero, Boss);

    // 4. Game Loop (Console Interface)
    while (!Battle.bBattleOver) {
        if (Battle.bPlayerTurn) {
            std::cout << "\nStatus: HP " << Hero->CurrentHP << "/" << Hero->MaxHP
                      << " | AP " << Hero->CurrentAP << "/" << Hero->MaxAP
                      << " | Block " << Hero->Block << std::endl;

            std::cout << "Enemy Intent: ";
            if (Boss->NextMove == Enemy::EIntent::Attack) std::cout << "Attack (" << Boss->IntentValue << ")";
            else if (Boss->NextMove == Enemy::EIntent::Defend) std::cout << "Defend";
            else std::cout << "Debuff";
            std::cout << " | HP " << Boss->CurrentHP << "/" << Boss->MaxHP << std::endl;

            std::cout << "Hand: " << std::endl;
            for (size_t i = 0; i < Hero->Hand.size(); ++i) {
                auto C = Hero->Hand[i];
                std::cout << "[" << i << "] " << C->Name << " (" << C->Cost << " AP): " << C->Description << std::endl;
            }
            std::cout << "[e] End Turn" << std::endl;

            std::cout << "> ";
            char Input;
            std::cin >> Input;

            if (std::cin.fail()) {
                break;
            }

            if (Input == 'e') {
                Battle.EndTurn();
            } else if (isdigit(Input)) {
                int Index = Input - '0';
                Battle.PlayCard(Index);
            } else {
                std::cout << "Invalid input." << std::endl;
            }
        }
    }

    std::cout << "Simulation Ended." << std::endl;
    return 0;
}
