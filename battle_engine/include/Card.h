#pragma once

#include "CoreTypes.h"
#include "StatusEffect.h"

class Card {
public:
    FString Name;
    FString Description;
    int32 Cost;
    ECardType Type;
    ECardTarget TargetType;
    FString ID; // For loading from JSON

    Card(FString InID, FString InName, int32 InCost, ECardType InType, ECardTarget InTarget, FString InDesc)
        : Name(InName), Description(InDesc), Cost(InCost), Type(InType), TargetType(InTarget), ID(InID) {}

    virtual ~Card() = default;

    // The main function to execute the card's logic
    virtual void Execute(const FCombatContext& Context) = 0;

    // To display dynamic text (e.g. "Deal 6 (8) damage")
    virtual FString GetDynamicDescription() const { return Description; }

    // Prototype pattern for Deck creation
    virtual TSharedPtr<Card> Clone() const = 0;
};

// --- Base implementations for easier subclassing ---

class AttackCard : public Card {
public:
    int32 DamageAmount;

    AttackCard(FString InID, FString InName, int32 InCost, int32 InDamage)
        : Card(InID, InName, InCost, ECardType::Attack, ECardTarget::Enemy, "Deal " + std::to_string(InDamage) + " damage."),
          DamageAmount(InDamage) {}

    void Execute(const FCombatContext& Context) override;
    TSharedPtr<Card> Clone() const override { return std::make_shared<AttackCard>(ID, Name, Cost, DamageAmount); }
};

class BlockCard : public Card {
public:
    int32 BlockAmount;

    BlockCard(FString InID, FString InName, int32 InCost, int32 InBlock)
        : Card(InID, InName, InCost, ECardType::Skill, ECardTarget::Self, "Gain " + std::to_string(InBlock) + " Block."),
          BlockAmount(InBlock) {}

    void Execute(const FCombatContext& Context) override;
    TSharedPtr<Card> Clone() const override { return std::make_shared<BlockCard>(ID, Name, Cost, BlockAmount); }
};

class EffectCard : public Card {
public:
    TSharedPtr<StatusEffect> EffectToApply;

    EffectCard(FString InID, FString InName, int32 InCost, ECardTarget InTarget, TSharedPtr<StatusEffect> InEffect)
        : Card(InID, InName, InCost, ECardType::Skill, InTarget, "Apply " + InEffect->Name),
          EffectToApply(InEffect) {}

    void Execute(const FCombatContext& Context) override;

    // Deep copy needed for the effect
    TSharedPtr<Card> Clone() const override {
        return std::make_shared<EffectCard>(ID, Name, Cost, TargetType, EffectToApply->Clone());
    }
};
