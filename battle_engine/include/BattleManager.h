#pragma once

#include "CoreTypes.h"
#include "StatusEffect.h"
#include "Card.h"
#include <vector>
#include <iostream>

// In Unreal: UCLASS() class ABattleManager : public AActor (or UGameInstanceSubsystem)
class BattleManager {
public:
    // In Unreal: UPROPERTY()
    TSharedPtr<Player> PlayerCharacter;

    // In Unreal: UPROPERTY()
    TSharedPtr<Enemy> EnemyCharacter;

    bool bPlayerTurn = true;
    bool bBattleOver = false;

    // --- Events ---
    // These allow external systems (UI, Relics, Achievements) to hook into combat logic.
    MulticastDelegate<Combatant*> OnTurnStartDelegate;
    MulticastDelegate<Combatant*> OnTurnEndDelegate;
    MulticastDelegate<Card*, Combatant*> OnCardPlayedDelegate; // Card Played, Source

    BattleManager() {}

    void SetupBattle(TSharedPtr<Player> InPlayer, TSharedPtr<Enemy> InEnemy) {
        PlayerCharacter = InPlayer;
        EnemyCharacter = InEnemy;
        bPlayerTurn = true;
        bBattleOver = false;

        StartBattle();
    }

    void StartBattle() {
        std::cout << "Battle Start: " << PlayerCharacter->Name << " vs " << EnemyCharacter->Name << std::endl;
        StartTurn();
    }

    void StartTurn() {
        if (CheckWinCondition()) return;

        Combatant* ActiveCombatant = bPlayerTurn ? (Combatant*)PlayerCharacter.get() : (Combatant*)EnemyCharacter.get();
        OnTurnStartDelegate.Broadcast(ActiveCombatant);

        if (bPlayerTurn) {
            std::cout << "\n--- Player Turn ---" << std::endl;
            PlayerCharacter->OnTurnStart();
            PlayerCharacter->DrawCards(5);
        } else {
            std::cout << "\n--- Enemy Turn ---" << std::endl;
            EnemyCharacter->OnTurnStart();
            EnemyCharacter->ExecuteTurn(PlayerCharacter.get(), this);
            EndTurn();
        }
    }

    void EndTurn() {
        Combatant* EndingCombatant = bPlayerTurn ? (Combatant*)PlayerCharacter.get() : (Combatant*)EnemyCharacter.get();
        OnTurnEndDelegate.Broadcast(EndingCombatant);

        if (bPlayerTurn) {
            PlayerCharacter->OnTurnEnd();
            bPlayerTurn = false;
        } else {
            EnemyCharacter->OnTurnEnd();
            EnemyCharacter->DecideNextMove();
            bPlayerTurn = true;
        }

        if (!CheckWinCondition()) {
            StartTurn();
        }
    }

    void PlayCard(int32 HandIndex) {
        if (!bPlayerTurn) {
            std::cout << "Not your turn!" << std::endl;
            return;
        }

        if (HandIndex < 0 || HandIndex >= (int32)PlayerCharacter->Hand.size()) {
            std::cout << "Invalid card selection." << std::endl;
            return;
        }

        TSharedPtr<Card> CardToPlay = PlayerCharacter->Hand[HandIndex];

        if (PlayerCharacter->CurrentAP < CardToPlay->Cost) {
            std::cout << "Not enough Energy!" << std::endl;
            return;
        }

        // Pay Cost
        PlayerCharacter->CurrentAP -= CardToPlay->Cost;

        // Determine Targets
        Combatant* Target = nullptr;
        if (CardToPlay->TargetType == ECardTarget::Enemy) {
            Target = EnemyCharacter.get();
        } else if (CardToPlay->TargetType == ECardTarget::Self) {
            Target = PlayerCharacter.get();
        }

        // Execute
        FCombatContext Context = { PlayerCharacter.get(), Target, this };
        std::cout << "Played: " << CardToPlay->Name << std::endl;

        OnCardPlayedDelegate.Broadcast(CardToPlay.get(), PlayerCharacter.get());
        CardToPlay->Execute(Context);

        // Remove from hand and add to discard
        PlayerCharacter->DiscardPile.push_back(CardToPlay);
        PlayerCharacter->Hand.erase(PlayerCharacter->Hand.begin() + HandIndex);

        CheckWinCondition();
    }

    bool CheckWinCondition() {
        if (PlayerCharacter->IsDead()) {
            std::cout << "Defeat! " << PlayerCharacter->Name << " has fallen." << std::endl;
            bBattleOver = true;
            return true;
        }
        if (EnemyCharacter->IsDead()) {
            std::cout << "Victory! " << EnemyCharacter->Name << " is defeated." << std::endl;
            bBattleOver = true;
            return true;
        }
        return false;
    }
};
