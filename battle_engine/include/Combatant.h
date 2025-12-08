#pragma once

#include "CoreTypes.h"
#include "StatusEffect.h"
#include "Card.h"
#include <vector>
#include <algorithm>
#include <random>

// In Unreal: UCLASS() class ACombatant : public AActor (or UActorComponent)
class Combatant {
public:
    FString Name;
    int32 MaxHP;
    int32 CurrentHP;
    int32 Block = 0;
    int32 MaxAP = 3;
    int32 CurrentAP = 0;

    // Card Piles
    // In Unreal: UPROPERTY() TArray<UCard*> DrawPile;
    TArray<TSharedPtr<Card>> DrawPile;
    TArray<TSharedPtr<Card>> Hand;
    TArray<TSharedPtr<Card>> DiscardPile;

    // Status Effects
    TArray<TSharedPtr<StatusEffect>> StatusEffects;

    Combatant(FString InName, int32 InMaxHP, int32 InMaxAP = 3)
        : Name(InName), MaxHP(InMaxHP), CurrentHP(InMaxHP), MaxAP(InMaxAP) {}

    virtual ~Combatant() = default;

    bool IsDead() const { return CurrentHP <= 0; }

    // --- Combat Actions ---

    // In Unreal: UFUNCTION(BlueprintCallable)
    virtual void TakeDamage(int32 Amount, Combatant* Source) {
        // Apply vulnerability
        float32 ModifiedAmount = (float32)Amount;
        for (auto& Status : StatusEffects) {
            ModifiedAmount = Status->GetDamageModifier(ModifiedAmount, this);
        }

        // Apply Block
        int32 DamageToHP = (int32)ModifiedAmount;
        if (Block > 0) {
            if (Block >= DamageToHP) {
                Block -= DamageToHP;
                DamageToHP = 0;
            } else {
                DamageToHP -= Block;
                Block = 0;
            }
        }

        CurrentHP -= DamageToHP;
        std::cout << Name << " took " << DamageToHP << " damage. HP: " << CurrentHP << "/" << MaxHP << std::endl;
    }

    virtual void Heal(int32 Amount) {
        CurrentHP = std::min(CurrentHP + Amount, MaxHP);
    }

    virtual void GainBlock(int32 Amount) {
        Block += Amount;
        std::cout << Name << " gained " << Amount << " block. Total: " << Block << std::endl;
    }

    virtual void ApplyStatus(TSharedPtr<StatusEffect> NewStatus) {
        // Check if status already exists (simple logic: stack or refresh)
        bool Found = false;
        for (auto& Status : StatusEffects) {
            if (Status->Name == NewStatus->Name) {
                Status->Duration += NewStatus->Duration; // Extend duration
                Status->Value += NewStatus->Value;       // Stack value
                Found = true;
                break;
            }
        }
        if (!Found) {
            StatusEffects.push_back(NewStatus);
            NewStatus->OnApply(this);
        }
        std::cout << Name << " applied " << NewStatus->Name << std::endl;
    }

    // --- Turn Lifecycle ---

    virtual void OnTurnStart() {
        Block = 0; // Block resets each turn usually
        CurrentAP = MaxAP;

        // Process Status Effects
        for (auto it = StatusEffects.begin(); it != StatusEffects.end(); ) {
            (*it)->OnTurnStart(this);
            if ((*it)->Duration == 0) {
                it = StatusEffects.erase(it); // Remove expired (if logic handles it here)
            } else {
                ++it;
            }
        }
    }

    virtual void OnTurnEnd() {
        // Discard Hand
        for (auto& Card : Hand) {
            DiscardPile.push_back(Card);
        }
        Hand.clear();

        // Process Status Effects
        for (auto it = StatusEffects.begin(); it != StatusEffects.end(); ) {
            (*it)->OnTurnEnd(this);
            if ((*it)->Duration == 0 && (*it)->Value == 0) { // Cleanup condition
                 it = StatusEffects.erase(it);
            } else {
                ++it;
            }
        }
    }

    // --- Card Management ---

    void DrawCards(int32 Amount) {
        for (int i = 0; i < Amount; i++) {
            if (DrawPile.empty()) {
                if (DiscardPile.empty()) break; // No cards left
                ReshuffleDiscard();
            }
            if (!DrawPile.empty()) {
                Hand.push_back(DrawPile.back());
                DrawPile.pop_back();
            }
        }
    }

    void ReshuffleDiscard() {
        std::cout << "Reshuffling discard into draw pile..." << std::endl;
        // Move discard to draw
        DrawPile.insert(DrawPile.end(), DiscardPile.begin(), DiscardPile.end());
        DiscardPile.clear();
        // Shuffle
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(DrawPile.begin(), DrawPile.end(), g);
    }

    // --- Stats Helpers ---
    float32 GetOutgoingDamageModifier(float32 BaseDamage) {
        float32 Damage = BaseDamage;
        for (auto& Status : StatusEffects) {
            Damage = Status->GetOutputDamageModifier(Damage, this);
        }
        return Damage;
    }
};

class Player : public Combatant {
public:
    Player(FString Name = "Hero") : Combatant(Name, 80, 3) {}
};

class Enemy : public Combatant {
public:
    // Simple intent system
    enum class EIntent {
        Attack,
        Defend,
        Debuff
    };

    EIntent NextMove;
    int32 IntentValue; // e.g. Damage amount

    Enemy(FString Name, int32 HP) : Combatant(Name, HP, 3) {
        DecideNextMove();
    }

    void DecideNextMove() {
        // Random simple logic for demo
        int r = rand() % 3;
        if (r == 0) {
            NextMove = EIntent::Attack;
            IntentValue = 10;
        } else if (r == 1) {
            NextMove = EIntent::Defend;
            IntentValue = 5;
        } else {
            NextMove = EIntent::Debuff;
            IntentValue = 2; // Duration
        }
    }

    void ExecuteTurn(Combatant* Target, BattleManager* Manager);
};
